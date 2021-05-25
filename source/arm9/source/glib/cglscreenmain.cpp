
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglscreen.h"

#define VRAMBuf ((u16*)(0x06000000))

#define BG2_CR_BASE (BG_BMP16_256x256 | BG_BMP_BASE(0))
#define BG3_CR_BASE (BG_BMP16_256x256 | BG_BMP_BASE(6))

#define BG2_CR_BASE512 (BG_BMP16_512x512 | BG_BMP_BASE(0))

CglScreenMain::CglScreenMain(void)
{
  BG2_CR = BG2_CR_BASE | BG_PRIORITY_2;
  BG3_CR = BG3_CR_BASE | BG_PRIORITY_1;
  
  {
    BG2_XDX = 1 << 8;
    BG2_XDY = 0 << 8;
    BG2_YDX = 0 << 8;
    BG2_YDY = 1 << 8;
    BG2_CX = 0;
    BG2_CY = 0;
  }
  {
    BG3_XDX = 1 << 8;
    BG3_XDY = 0 << 8;
    BG3_YDX = 0 << 8;
    BG3_YDY = 1 << 8;
    BG3_CX = 0;
    BG3_CY = 0;
  }
  
  BackVRAMPage=1;
  
  VRAMBufArray[0]=&VRAMBuf[(0*ScreenHeight)*ScreenWidth];
  VRAMBufArray[1]=&VRAMBuf[(1*ScreenHeight)*ScreenWidth];
  
  pCanvas=new CglCanvas(VRAMBufArray[0],ScreenWidth,ScreenHeight,pf15bit);
  pCanvas->SetColor(RGB15(31,31,31)|BIT15);
  pCanvas->FillBox(0,0,ScreenWidth,ScreenHeight);
  
  pCanvas->SetVRAMBuf(VRAMBufArray[1],ScreenWidth,ScreenHeight,pf15bit);
  pCanvas->FillBox(0,0,ScreenWidth,ScreenHeight);
  
  WideFlag=false;
  
  Flip(true);
}

CglScreenMain::~CglScreenMain(void)
{
  delete pCanvas; pCanvas=NULL;
}

void CglScreenMain::Flip(const bool ShowFlag)
{
  if(WideFlag==true) return;
  
  BackVRAMPage=1-BackVRAMPage;
  pCanvas->SetVRAMBuf(VRAMBufArray[BackVRAMPage],ScreenWidth,ScreenHeight,pf15bit);
  
  if(BackVRAMPage==0){
    BG2_CR = BG2_CR_BASE | BG_PRIORITY_2;
    BG3_CR = BG3_CR_BASE | BG_PRIORITY_1;
    }else{
    BG2_CR = BG2_CR_BASE | BG_PRIORITY_1;
    BG3_CR = BG3_CR_BASE | BG_PRIORITY_2;
  }
  
/*
  if(ShowFlag==true){
    SetBlendLevel(16);
    }else{
    SetBlendLevel(0);
  }
*/

  // ignore blend level
  // blent target to MainOverlay (OBJ)
  if(BackVRAMPage==0){
    BLEND_CR=BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG3;
    }else{
    BLEND_CR=BLEND_ALPHA | BLEND_SRC_SPRITE | BLEND_DST_BG2;
  }

}

u16* CglScreenMain::GetVRAMBuf(EScrMainID ScrMainID) const
{
  if(WideFlag==true) return(VRAMBufArray[0]);
  
  switch(ScrMainID){
    case ScrMainID_View: return(VRAMBufArray[1-BackVRAMPage]); break;
    case ScrMainID_Back: return(VRAMBufArray[BackVRAMPage]); break;
  }
  return(NULL);
}

void CglScreenMain::SetTargetPage(EScrMainID ScrMainID)
{
  if(WideFlag==true){
    pCanvas->SetVRAMBuf(VRAMBufArray[0],512,256,pf15bit);
    return;
  }
  
  switch(ScrMainID){
    case ScrMainID_View: pCanvas->SetVRAMBuf(VRAMBufArray[1-BackVRAMPage],ScreenWidth,ScreenHeight,pf15bit); break;
    case ScrMainID_Back: pCanvas->SetVRAMBuf(VRAMBufArray[BackVRAMPage],ScreenWidth,ScreenHeight,pf15bit); break;
    default: pCanvas->SetVRAMBuf(NULL,ScreenWidth,ScreenHeight,pf15bit); break;
  }
}

void CglScreenMain::SetBlendLevel(const int BlendLevel)
{
  if(WideFlag==true){
    BLEND_CR=0;
    return;
  }
  
  SetBlendLevelManual(16-BlendLevel,BlendLevel);
}

void CglScreenMain::SetBlendLevelManual(const int BlendLevelBack,const int BlendLevelView)
{
  if(WideFlag==true){
    BLEND_CR=0;
    return;
  }
  
  if(BackVRAMPage==0){
    BLEND_CR=BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG2;
    }else{
    BLEND_CR=BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
  }
  
  int blb=BlendLevelBack;
  int blv=BlendLevelView;
  
  if(blb<0) blb=0;
  if(16<blb) blb=16; // Max16
  if(blv<0) blv=0;
  if(16<blv) blv=16; // Max16
  
  BLEND_AB=(blv << 0) | (blb << 8);
}

void CglScreenMain::CopyFullViewToBack(void)
{
  if(WideFlag==true) return;
  
  u16 *src=GetVRAMBuf(ScrMainID_View);
  u16 *dst=GetVRAMBuf(ScrMainID_Back);
  glMemCopy16CPU(src,dst,ScreenHeight*ScreenWidth*2);
}

void CglScreenMain::CopyFullBackToView(void)
{
  if(WideFlag==true) return;
  
  u16 *src=GetVRAMBuf(ScrMainID_Back);
  u16 *dst=GetVRAMBuf(ScrMainID_View);
  glMemCopy16CPU(src,dst,ScreenHeight*ScreenWidth*2);
}

void CglScreenMain::SetWideFlag(bool w)
{
  WideFlag=w;
  
  if(WideFlag==false){
    vramSetMainBanks(VRAM_A_MAIN_BG_0x6000000, VRAM_B_MAIN_SPRITE, VRAM_C_MAIN_BG_0x6020000, VRAM_D_SUB_SPRITE);
    BG2_CR = BG2_CR_BASE | BG_PRIORITY_2;
    BG3_CR = BG3_CR_BASE | BG_PRIORITY_1;
    BackVRAMPage=1;
    }else{
    vramSetMainBanks(VRAM_A_MAIN_BG_0x6000000, VRAM_B_MAIN_SPRITE, VRAM_C_MAIN_BG_0x6020000, VRAM_D_SUB_SPRITE);//, VRAM_D_MAIN_BG_0x6040000);
    BG2_CR = BG2_CR_BASE512 | BG_PRIORITY_1;
    BG3_CR = BG_PRIORITY_3;
    BackVRAMPage=0;
  }
  
  SetTargetPage(ScrMainID_Back);
  SetBlendLevel(16);
}

bool CglScreenMain::GetWideFlag(void)
{
  return(WideFlag);
}

void CglScreenMain::SetViewSize(int w,int h)
{
  float ratio=(float)w/pCanvas->GetWidth();
  
  u16 XDX=(u16)(ratio*0x100);
  u16 YDY=(u16)(ratio*0x100);
  
  BG2_XDX = XDX;
  BG2_XDY = 0;
  BG2_YDX = 0;
  BG2_YDY = YDY;
  
  BG3_XDX = XDX;
  BG3_XDY = 0;
  BG3_YDX = 0;
  BG3_YDY = YDY;
  
  BG2_CX=0;
  BG2_CY=0;
  BG3_CX=0;
  BG3_CY=0;
}

void CglScreenMain::SetViewport(int x,int y,float fx,float fy)
{
  static int lx,ly;
  static float lfx,lfy;
  
  if((lfx!=fx)||(lfy!=fy)){
    lfx=fx;
    lfy=fy;
    u16 XDX=(u16)((1/fx)*0x100);
    u16 YDY=(u16)((1/fy)*0x100);
    
    BG2_XDX = XDX;
    BG2_XDY = 0;
    BG2_YDX = 0;
    BG2_YDY = YDY;
    
    BG3_XDX = XDX;
    BG3_XDY = 0;
    BG3_YDX = 0;
    BG3_YDY = YDY;
  }
  
  if((lx!=x)||(ly!=y)){
    lx=x;
    ly=y;
    x=(int)(-(x/fx)*0x100);
    y=(int)(-(y/fy)*0x100);
    
    BG2_CX=x;
    BG2_CY=y;
    BG3_CX=x;
    BG3_CY=y;
  }
}

