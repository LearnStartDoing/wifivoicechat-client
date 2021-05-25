
#include <stdio.h>
#include <stdlib.h>

#include <NDS.h>

#include "_console.h"

#include "glib/glib.h"

#include "mwin.h"
#include "strtool.h"
#include "memtool.h"
#include "FontPro.h"
#include "unicode.h"

#include "_const.h"
#include "inifile.h"
#include "lang.h"

#include "VRAMTool.h"

#include "mwin_color.h"

#define PrgBarHeight (16)

static TWinBody WinBody[WinBodyCount];

static u32 PriorityLevel;

static u8 *ScreenMask;

TSkinBM SkinBM;

// --------------------------------------------------------

// --------------------------------------------------------

#include "shell.h"

#include "mwin_bmp.h"

void MWin_Init(void)
{
  PriorityLevel=0;
  
  ScreenMask=(u8*)safemalloc(ScreenWidth*ScreenHeight);
  MemSet8DMA3(0,ScreenMask,ScreenWidth*ScreenHeight);
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    
    pwb->WinMode=cnt;
    pwb->SelDisp=SelDisp_Top;
    
    pwb->DynamicAllocMem=false;
    pwb->WinBuf=NULL;
    pwb->pCanvas=NULL;
    pwb->ClientBGColor=MWC_ClientBG;
    
    pwb->Visible=true;
    pwb->TopFlag=false;
    pwb->Priority=PriorityLevel;
    PriorityLevel++;
    
    pwb->SBarV.Enabled=false;
    pwb->SBarV.isV=true;
    
    pwb->SBarH.Enabled=false;
    pwb->SBarH.isV=false;
    
    pwb->CloseButton=true;
    
    char *pttl_eng=NULL,*pttl_jpn=NULL;
    
    switch(pwb->WinMode){
      case WM_About: {
        pttl_eng="About...";
        pttl_jpn="About...";
        pwb->Rect.x=8;
        pwb->Rect.y=8;
        pwb->ClientRect.w=192;
        pwb->ClientRect.h=FontProHeight*5;
        pwb->SBarV.Enabled=true;
      } break;
      case WM_Progress: {
        pwb->Visible=false;
        pttl_eng="Progress...";
        pttl_jpn="処理中…";
        pwb->Rect.x=ScreenWidth-132;
        pwb->Rect.y=ScreenHeight-40;
        pwb->ClientRect.w=128;
        pwb->ClientRect.h=PrgBarHeight;
        pwb->ClientBGColor=0;
        pwb->CloseButton=false;
      } break;
      case WM_Picture: {
        pwb->Visible=true;
        pttl_eng="Chat (ShoulderBtn = Swap screen)";
        pttl_jpn="チャット (肩ボタン = 上下入れ替え)";
        pwb->SelDisp=SelDisp_Bottom;
        pwb->Rect.x=0;
        pwb->Rect.y=0;
        pwb->ClientRect.w=ScreenWidth-8-1-1;
        pwb->ClientRect.h=ScreenHeight-1-TitleBarHeight-8-1;
        pwb->ClientBGColor=0;
        pwb->CloseButton=false;
        pwb->SBarV.Enabled=true;
        pwb->SBarH.Enabled=true;
      } break;
      case WM_ItemPalette: {
        pwb->Visible=true;
        pttl_eng="Item palette";
        pttl_jpn="アイテムパレット";
        pwb->Rect.x=4;
        pwb->Rect.y=144;
        pwb->ClientRect.w=16*7;
        pwb->ClientRect.h=16;
        pwb->ClientBGColor=MWC_FileSelectBG;
        pwb->CloseButton=false;
        pwb->SBarV.Enabled=false;
        pwb->SBarH.Enabled=false;
      } break;
      case WM_ColorPicker: {
        pwb->Visible=true;
        pttl_eng="Color picker";
        pttl_jpn="カラーピッカー";
        pwb->Rect.x=122;
        pwb->Rect.y=32;
        pwb->ClientRect.w=128;
        pwb->ClientRect.h=128;
        pwb->ClientBGColor=0;
        pwb->CloseButton=true;
        pwb->SBarV.Enabled=false;
        pwb->SBarH.Enabled=false;
      } break;
      case WM_UserList: {
        pwb->Visible=true;
        pttl_eng="User list";
        pttl_jpn="ユーザーリスト";
        pwb->Rect.x=16;
        pwb->Rect.y=64;
        pwb->ClientRect.w=96;
        pwb->ClientRect.h=FontProHeight*5;
        pwb->CloseButton=true;
        pwb->SBarV.Enabled=true;
        pwb->SBarH.Enabled=false;
      } break;
      case WM_UserInfo: {
        pwb->Visible=false;
        pttl_eng="User info";
        pttl_jpn="ユーザー情報";
        pwb->Rect.x=128;
        pwb->Rect.y=104;
        pwb->ClientRect.w=120;
        pwb->ClientRect.h=FontProHeight*5;
        pwb->CloseButton=true;
        pwb->SBarV.Enabled=false;
        pwb->SBarH.Enabled=false;
      } break;
      case WM_MicVolume: {
        pwb->Visible=false;
        pttl_eng="M";
        pttl_jpn="M";
        pwb->Rect.x=256-20;
        pwb->Rect.y=16;
        pwb->ClientRect.w=16;
        pwb->ClientRect.h=128;
        pwb->ClientBGColor=0;
        pwb->CloseButton=false;
        pwb->SBarV.Enabled=false;
        pwb->SBarH.Enabled=false;
      } break;
      case WM_RoomNum: {
        pwb->Visible=false;
        pttl_eng="Select private room number. (000 = global)";
        pttl_jpn="プライベートルーム選択 (000 = 共通ロビー)";
        pwb->Rect.x=0;
        pwb->Rect.y=0;
        pwb->ClientRect.w=ScreenWidth-8-1-1;
        pwb->ClientRect.h=ScreenHeight-1-TitleBarHeight-1;
        pwb->ClientBGColor=MWC_FileSelectBG;
        pwb->CloseButton=true;
        pwb->SBarV.Enabled=true;
      } break;
      case WM_PlayEnv: {
        pwb->Visible=true;
        pttl_eng="Environment";
        pttl_jpn="音響設定";
        pwb->Rect.x=8;
        pwb->Rect.y=90;
        pwb->ClientRect.w=90;
        pwb->ClientRect.h=FontProHeight*3;
        pwb->ClientBGColor=MWC_FileSelectBG;
        pwb->CloseButton=true;
        pwb->SBarV.Enabled=false;
        pwb->SBarH.Enabled=false;
      } break;
      default: {
        _consolePrintf("FatalError. mwin illigal initialize.\n");
        ShowLogHalt();
        while(1);
      }
    }
    
    switch(GetLang()){
      case EL_ENG: snprintf(pwb->TitleStr,128,"%s",pttl_eng); break;
      case EL_JPN: snprintf(pwb->TitleStr,128,"%s",pttl_jpn); break;
      default: snprintf(pwb->TitleStr,128,"%s",pttl_eng); break;
    }
    
    pwb->ClientRect.x=1;
    pwb->ClientRect.y=TitleBarHeight+1;
    pwb->Rect.w=pwb->ClientRect.x+pwb->ClientRect.w+1;
    if(pwb->ClientRect.h==0){
      pwb->Rect.h=pwb->ClientRect.y-1;
      }else{
      pwb->Rect.h=pwb->ClientRect.y+pwb->ClientRect.h+1;
    }
    
    {
      TSBar *psb=&pwb->SBarV;
      if(psb->Enabled==true){
        TRect *pcr=&psb->ClientRect;
        
        pcr->x=pwb->ClientRect.x+pwb->ClientRect.w;
        pcr->y=pwb->ClientRect.y;
        pcr->w=8;
        pcr->h=pwb->ClientRect.h;
        
        pwb->Rect.w+=8;
      }
      
    }
    
    {
      TSBar *psb=&pwb->SBarH;
      if(psb->Enabled==true){
        TRect *pcr=&psb->ClientRect;
        
        pcr->x=pwb->ClientRect.x;
        pcr->y=pwb->ClientRect.y+pwb->ClientRect.h;
        pcr->w=pwb->ClientRect.w;
        pcr->h=8;
        
        pwb->Rect.h+=8;
      }
      
    }
    
//    pwb->DynamicAllocMem=false; // disabled
    
    if(pwb->DynamicAllocMem==false){
      if(pwb->WinBuf==NULL) pwb->WinBuf=(u16*)safemalloc(pwb->Rect.w*pwb->Rect.h*2);
      pwb->pCanvas=new CglCanvas(pwb->WinBuf,pwb->Rect.w,pwb->Rect.h,pf15bit);
      }else{
      pwb->WinBuf=NULL;
    }
    
    if(pwb->WinMode==WM_Progress){
      void MWinCallProgress_Draw(u32 WinIndex);
      pwb->CallBack_Draw=MWinCallProgress_Draw;
      pwb->CallBack_CloseButton=NULL;
      pwb->CallBack_MouseDown=NULL;
      pwb->CallBack_MouseMove=NULL;
      pwb->CallBack_MouseUp=NULL;
      }else{
      MWinCallBack_RegistCallBack(pwb);
    }
  }
  
  LoadSkinBM();
}

void MWin_Free(void)
{
}

void MWin_AllRefresh(void)
{
  MWin_ClearDesktop();
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    MWin_RefreshWindow(cnt);
  }
}

void MWin_AllTrans(void)
{
  MWin_ClearDesktop();
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    MWin_TransWindow(cnt);
  }
}

void MWin_AllocMem(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->DynamicAllocMem==false){
    if(pwb->WinBuf==NULL){
      _consolePrintf("MWinError:DynamicAllocMem==false and WinBuf==NULL\n");
    }
    return;
  }
  
  if(pwb->WinBuf==NULL){
    pwb->WinBuf=(u16*)safemalloc(pwb->Rect.w*pwb->Rect.h*2);
    MemSet16DMA3(0,pwb->WinBuf,pwb->Rect.w*pwb->Rect.h*2);
  }
}

extern void MWin_FreeMem(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->DynamicAllocMem==false){
    if(pwb->WinBuf==NULL){
      _consolePrintf("MWinError:DynamicAllocMem==false and WinBuf==NULL\n");
    }
    return;
  }
  
  if(pwb->WinBuf!=NULL){
    safefree(pwb->WinBuf); pwb->WinBuf=NULL;
  }
}

u32 MWin_SetActive(u32 WinBodyIndex)
{
  u32 DeactiveWinIndex=(u32)-1;
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    
    if(cnt==WinBodyIndex){
      pwb->TopFlag=true;
      pwb->Priority=PriorityLevel;
      PriorityLevel++;
      }else{
      if(pwb->TopFlag==true){
        DeactiveWinIndex=cnt;
        pwb->TopFlag=false;
      }
    }
  }
  
  return(DeactiveWinIndex);
}

u32 MWin_SetActiveTopMost(void)
{
  u32 _Priority=0;
  u32 idx=(u32)-1;
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    
    if(pwb->Visible==true){
      if(_Priority<=pwb->Priority){
        _Priority=pwb->Priority;
        idx=cnt;
      }
    }
  }
  
  if(idx==(u32)-1) return(idx);
  
  MWin_SetActive(idx);
  return(idx);
}

void MWin_InsideScreen(TRect *Rect)
{
  if(Rect->x<0){
    Rect->w=Rect->w+Rect->x;
    Rect->x=0;
  }
  if(Rect->y<0){
    Rect->h=Rect->h+Rect->y;
    Rect->y=0;
  }
  if((Rect->x+Rect->w)>ScreenWidth) Rect->w=ScreenWidth-Rect->x;
  if((Rect->y+Rect->h)>ScreenHeight) Rect->h=ScreenHeight-Rect->y;
}

void MWin_RefreshScreenMask(void)
{
  u32 SortIndex[WinBodyCount];
  u32 SortPriority[WinBodyCount];
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    
    SortIndex[cnt]=cnt;
    SortPriority[cnt]=pwb->Priority;
  }
  
  for(u32 c0=0;c0<WinBodyCount-1;c0++){
    for(u32 c1=c0+1;c1<WinBodyCount;c1++){
      if(SortPriority[c0]>SortPriority[c1]){
        u32 tmp;
        
        tmp=SortIndex[c0];
        SortIndex[c0]=SortIndex[c1];
        SortIndex[c1]=tmp;
        
        tmp=SortPriority[c0];
        SortPriority[c0]=SortPriority[c1];
        SortPriority[c1]=tmp;
      }
    }
  }
  
  u8 *pMask=&ScreenMask[0];
  
  for(s32 y=0;y<ScreenHeight;y++){
    MemSet8DMA3(0xff,pMask,ScreenWidth);
    pMask+=ScreenWidth;
  }
  
  for(u32 sortcnt=0;sortcnt<WinBodyCount;sortcnt++){
    u32 cnt=SortIndex[sortcnt];
    TWinBody *pwb=&WinBody[cnt];
    
    if((pwb->Visible==true)&&(pwb->SelDisp==SelDisp_Top)){
      TRect Rect=pwb->Rect;
      
      MWin_InsideScreen(&Rect);
      
      u8 *pMask=&ScreenMask[Rect.x+(Rect.y*ScreenWidth)];
      
      for(s32 y=0;y<Rect.h;y++){
        MemSet8DMA3(cnt,pMask,Rect.w);
        pMask+=ScreenWidth;
      }
    }
  }
}

void MWin_SetVisible(u32 WinIndex,bool Visible)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  pwb->Visible=Visible;
}

bool MWin_GetVisible(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  return(pwb->Visible);
}

void MWin_DrawTitleBar(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  
  u16 *pBuf;
  u16 col;
  
  s32 BufWidth=pwb->Rect.w;
  
  TRect Rect;
  
  Rect.x=0;
  Rect.y=0;
  Rect.w=pwb->Rect.w;
  Rect.h=TitleBarHeight;
  
  bool ActiveFlag;
  
  if(pwb->TopFlag==true){
    ActiveFlag=true;
    }else{
    ActiveFlag=false;
  }
  
  if(pwb->SelDisp==SelDisp_Bottom) ActiveFlag=true;
  
  u16 *pTitleBM;
  const TSkinBM *psbm=&SkinBM;
  
  if(ActiveFlag==true){
    pTitleBM=psbm->pTitleABM;
    }else{
    pTitleBM=psbm->pTitleDBM;
  }
  
  if(pTitleBM!=NULL){ // BitmapTitleBar
    pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
    for(s32 y=0;y<TitleBarHeight;y++){
      MemCopy16DMA3(&pTitleBM[0],&pBuf[0],BufWidth*2);
      pTitleBM+=psbm->TitleBM_W;
      pBuf+=BufWidth;
    }
    
    }else{ // default
    if(ActiveFlag==true){
      col=MWC_TitleA_Bright;
      }else{
      col=MWC_TitleD_Bright;
    }
    
    // TopLine
    pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
    MemSet16DMA3(col,pBuf,Rect.w*2);
    
    // LeftLine
    pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
    for(s32 y=0;y<Rect.h;y++){
      pBuf[0]=col;
      pBuf+=BufWidth;
    }
    
    if(ActiveFlag==true){
      col=MWC_TitleA_Dark;
      }else{
      col=MWC_TitleD_Dark;
    }
    
    // BottomLine
    pBuf=&pwb->WinBuf[(Rect.y+Rect.h-1)*BufWidth];
    MemSet16DMA3(col,&pBuf[1],(Rect.w-1)*2);
    
    // RightLine
    pBuf=&pwb->WinBuf[Rect.x+Rect.w-1+(Rect.y*BufWidth)];
    pBuf+=BufWidth;
    for(s32 y=1;y<Rect.h;y++){
      pBuf[0]=col;
      pBuf+=BufWidth;
    }
    
    if(ActiveFlag==true){
      col=MWC_TitleA_BG;
      }else{
      col=MWC_TitleD_BG;
    }
    
    // FillBG
    pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
    pBuf+=BufWidth;
    for(s32 y=1;y<TitleBarHeight-1;y++){
      MemSet16DMA3(col,&pBuf[1],(BufWidth-1-1)*2);
      pBuf+=BufWidth;
    }
  }
  
  if(pwb->CloseButton==true){
    u16 *pBM=psbm->pCloseBtnBM;
    u32 BM_W=psbm->CloseBtnBM_W;
    u32 BM_H=psbm->CloseBtnBM_H;
    
    if(pBM!=NULL){ // Bitmap CloseButton
      pBuf=&pwb->WinBuf[(Rect.x+Rect.w-BM_W)+(Rect.y*BufWidth)];
      for(u32 y=0;y<BM_H;y++){
        for(u32 x=0;x<BM_W;x++){
          if(pBM[x]!=0) pBuf[x]=pBM[x];
        }
        pBM+=BM_H;
        pBuf+=BufWidth;
      }
    }
  }
  
  pBuf=&pwb->WinBuf[0];
  
  char msg[128];
  
  if(pwb->WinMode!=WM_Picture){
    snprintf(msg,128,"%s",pwb->TitleStr);
    }else{
    extern u32 RoomNum_CurrentNum;
    snprintf(msg,128,"%s Room:%03d",pwb->TitleStr,RoomNum_CurrentNum);
  }
  
  if(ActiveFlag==true){
    VRAM_WriteStringRectUTF8(pBuf,BufWidth,BufWidth,4+1,1+1,msg,MWC_TitleA_TextShadow);
    VRAM_WriteStringRectUTF8(pBuf,BufWidth,BufWidth,4+0,1+0,msg,MWC_TitleA_Text);
    }else{
    VRAM_WriteStringRectUTF8(pBuf,BufWidth,BufWidth,4+0,1+0,msg,MWC_TitleD_Text);
  }
}

void MWin_DrawClientFrame(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  
  if(pwb->ClientRect.h==0) return;
  
  u32 BufWidth=pwb->Rect.w;
  
  TRect Rect;
  
  Rect.x=0;
  Rect.y=TitleBarHeight;
  Rect.w=pwb->Rect.w;
  Rect.h=pwb->Rect.h-Rect.y;
  
  u16 *pBuf;
  u16 col;
  
  col=MWC_FrameBright;
  
  pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
  MemSet16DMA3(col,pBuf,Rect.w*2);
  
  pBuf=&pwb->WinBuf[Rect.x+(Rect.y*BufWidth)];
  for(s32 y=0;y<Rect.h;y++){
    pBuf[0]=col;
    pBuf+=BufWidth;
  }
  
  col=MWC_FrameDark;
  
  pBuf=&pwb->WinBuf[(Rect.y+Rect.h-1)*BufWidth];
  MemSet16DMA3(col,&pBuf[1],(Rect.w-1)*2);
  
  pBuf=&pwb->WinBuf[Rect.x+Rect.w-1+(Rect.y*BufWidth)];
  pBuf+=BufWidth;
  for(s32 y=1;y<Rect.h;y++){
    pBuf[0]=col;
    pBuf+=BufWidth;
  }
  
}

void MWin_DrawClientBG(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  
  u16 col=pwb->ClientBGColor;
  
  if(pwb->Rect.h<=TitleBarHeight) return;
  if(col==0) return;
  
  u32 BufWidth=pwb->Rect.w;
  
  u16 *pBuf;
  
  u16 *pClientBM;
  u32 ClientBM_W;
  const TSkinBM *psbm=&SkinBM;
  
  switch(WinIndex){
    default: {
      pClientBM=psbm->pClientBM;
      ClientBM_W=psbm->ClientBM_W;
    } break;
  }
  
  if(pClientBM!=NULL){ // BitmapBG
    pBuf=&pwb->WinBuf[pwb->ClientRect.x+(pwb->ClientRect.y)*BufWidth];
    for(s32 y=0;y<pwb->ClientRect.h;y++){
      MemCopy16DMA3(pClientBM,pBuf,pwb->ClientRect.w*2);
      pClientBM+=ClientBM_W;
      pBuf+=BufWidth;
    }
    
    }else{ // default
    pBuf=&pwb->WinBuf[pwb->ClientRect.x+(pwb->ClientRect.y)*BufWidth];
    for(s32 y=0;y<pwb->ClientRect.h;y++){
      MemSet16DMA3(col,pBuf,pwb->ClientRect.w*2);
      pBuf+=BufWidth;
    }
  }

}

void MWin_TransWindow(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  if(pwb->Visible==false) return;
  if(pwb->SelDisp==SelDisp_Bottom){
    u16 *pVRAMBuf=pScreenMainOverlay->GetVRAMBuf();
    u16 *pSrcBuf=&pwb->WinBuf[0];
    
    MemCopy32swi256bit(pSrcBuf,pVRAMBuf,ScreenHeight*ScreenWidth*2);
    return;
  }
  
  u32 BufWidth=pwb->Rect.w;
  
  TRect Rect=pwb->Rect;
  
  s32 ofsx,ofsy;
  
  ofsx=Rect.x;
  ofsy=Rect.y;
  
  MWin_InsideScreen(&Rect);
  
  ofsx=Rect.x-ofsx;
  ofsy=Rect.y-ofsy;
  
  u16 *pSrcBuf=&pwb->WinBuf[ofsx+(ofsy*BufWidth)];
  u16 *pVRAMBuf=pScreenSub->GetVRAMBuf();
  u16 *pDstBuf=&pVRAMBuf[Rect.x+(Rect.y*ScreenWidth)];
  
  if(pwb->TopFlag==true){
    for(s32 y=0;y<Rect.h;y++){
      MemCopy16DMA3(pSrcBuf,pDstBuf,Rect.w*2);
      pSrcBuf+=BufWidth;
      pDstBuf+=ScreenWidth;
    }
    return;
  }
  
  u8 *pMask=&ScreenMask[Rect.x+(Rect.y*ScreenWidth)];
  
  for(s32 y=0;y<Rect.h;y++){
    for(s32 x=0;x<Rect.w;x++){
      if(pMask[x]==WinIndex){
        pDstBuf[x]=pSrcBuf[x];
      }
    }
    pSrcBuf+=BufWidth;
    pDstBuf+=ScreenWidth;
    pMask+=ScreenWidth;
  }
}

void MWin_ClearDesktop(void)
{
  u16 *pDstBuf;
  u8 *pMask;
  
  u16 *pVRAMBuf=pScreenSub->GetVRAMBuf();
  
  pDstBuf=&pVRAMBuf[0];
  pMask=&ScreenMask[0];
  // ほんとはScreenXじゃなくて、psbm->DesktopBM_Xを使うべき…
  
  for(s32 y=0;y<ScreenHeight;y++){
    for(s32 x=0;x<ScreenWidth;x++){
      if(pMask[x]==0xff) pDstBuf[x]=0;
    }
    pDstBuf+=ScreenWidth;
    pMask+=ScreenWidth;
  }
}

void MWin_DrawClient(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  if(pwb->Visible==false) return;
  
  MWin_DrawClientBG(WinIndex);
  
  if(pwb->CallBack_Draw!=NULL) pwb->CallBack_Draw(WinIndex);
}

static bool InitSBImageFlag=false;
static u16 SBImageBG8[8*8]={
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
	0xE0,0x79,0x88,0x88,0x89,0x89,0x88,0xE0,
};
static u16 SBImageBody8[8*8]={
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
};
static u16 SBImagePart8[8*8]={
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x88,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0xB8,0xB8,0xB8,0xB8,0x79,0xE0,
	0xE0,0x79,0x89,0x79,0x79,0x79,0x79,0xE0,
	0xE0,0x79,0x88,0x89,0x79,0x79,0x79,0xE0,
};

void MWin_DrawSBar(u32 WinIndex,TSBar *psb)
{
  if(InitSBImageFlag==false){
    InitSBImageFlag=true;
    
    u16 col;
    u32 r,g,b;
    
    col=MWC_ScrollBar;
    
    r=(col >> 0) & 0x1f;
    g=(col >> 5) & 0x1f;
    b=(col >> 10) & 0x1f;
    
    const TSkinBM *psbm=&SkinBM;
    
    if(psbm->pSB_BGBM!=NULL){
      MemCopy16DMA3(psbm->pSB_BGBM,SBImageBG8,8*8*2);
      }else{
      for(u32 cnt=0;cnt<8*8;cnt++){
        u32 col=(((u32)SBImageBG8[cnt])*0x123/0x100)/8; // 0xe0 -> 0xff -> 0x1f
        SBImageBG8[cnt]=RGB15(col*r/0x20,col*g/0x20,col*b/0x20) | BIT(15);
      }
    }
    if(psbm->pSB_BodyBM!=NULL){
      MemCopy16DMA3(psbm->pSB_BodyBM,SBImageBody8,8*8*2);
      }else{
      for(u32 cnt=0;cnt<8*8;cnt++){
        u32 col=(((u32)SBImageBody8[cnt])*0x123/0x100)/8; // 0xe0 -> 0xff -> 0x1f
        SBImageBody8[cnt]=RGB15(col*r/0x20,col*g/0x20,col*b/0x20) | BIT(15);
      }
    }
    if(psbm->pSB_PartBM!=NULL){
      MemCopy16DMA3(psbm->pSB_PartBM,SBImagePart8,8*8*2);
      }else{
      for(u32 cnt=0;cnt<8*8;cnt++){
        u32 col=(((u32)SBImagePart8[cnt])*0x123/0x100)/8; // 0xe0 -> 0xff -> 0x1f
        SBImagePart8[cnt]=RGB15(col*r/0x20,col*g/0x20,col*b/0x20) | BIT(15);
      }
    }
  }
  
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  
  TRect *pcr=&psb->ClientRect;
  
  if(psb->Enabled==false) return;
  
  u32 BufWidth=pwb->Rect.w;
  
  {
    u16 *pBuf=&pwb->WinBuf[pcr->x+(pcr->y*BufWidth)];
    u16 *pCol=&SBImageBG8[0];
    
    if(psb->isV==true){
      for(s32 y=0;y<pcr->h;y++){
        u16 *_pCol=&pCol[(y&7)*8];
        for(s32 x=0;x<8;x++){
          pBuf[x]=_pCol[x];
        }
        pBuf+=BufWidth;
      }
      }else{
      for(s32 y=0;y<8;y++){
        u16 *_pCol=&pCol[y];
        for(s32 x=0;x<pcr->w;x++){
          pBuf[x]=_pCol[(x&7)*8];
        }
        pBuf+=BufWidth;
      }
    }
  }
  
  if((psb->GripMax==0)||(psb->GripSize==0)) return;
  
  float fGripSize=(float)psb->GripSize/psb->GripMax;
  float fGripPos=(float)psb->GripPos/psb->GripMax;
  
  s32 iGripMax,iGripSize,iGripPos;
  
  if(psb->isV==true){
    iGripMax=pcr->h;
    }else{
    iGripMax=pcr->w;
  }
  iGripSize=(s32)(fGripSize*iGripMax);
  iGripPos=(s32)(fGripPos*iGripMax);
  
  if(iGripSize>iGripMax){
    iGripPos=0;
    iGripSize=iGripMax;
  }
  
  if(iGripSize<8) iGripSize=8;
  if(iGripMax<(iGripPos+iGripSize)) iGripPos=iGripMax-iGripSize;
  
  u16 *pBuf=&pwb->WinBuf[pcr->x+(pcr->y*BufWidth)];
  
//  _consolePrintf("%f %f %d %d %d\n",fGripSize,fGripPos,iGripSize,iGripPos,iGripMax);
  
  if(psb->isV==true){
    u16 *_pBuf;
    u16 *pCol;
    
    for(int y=0;y<4;y++){
      pCol=&SBImagePart8[y*8];
      _pBuf=&pBuf[(iGripPos+y)*BufWidth];
      for(s32 x=0;x<8;x++){
        _pBuf[x]=pCol[x];
      }
    }
    for(int y=0;y<4;y++){
      pCol=&SBImagePart8[(4+y)*8];
      _pBuf=&pBuf[(iGripPos+iGripSize-4+y)*BufWidth];
      for(s32 x=0;x<8;x++){
        _pBuf[x]=pCol[x];
      }
    }
    pCol=&SBImageBody8[0];
    _pBuf=&pBuf[(iGripPos+4)*BufWidth];
    for(s32 y=4;y<iGripSize-4;y++){
      u16 *_pCol=&pCol[(y&7)*8];
      for(s32 x=0;x<8;x++){
        _pBuf[x]=_pCol[x];
      }
      _pBuf+=BufWidth;
    }
    }else{
    u16 *_pBuf;
    u16 *pCol;
    
    for(int y=0;y<4;y++){
      pCol=&SBImagePart8[y*8];
      _pBuf=&pBuf[(iGripPos+y)];
      for(s32 x=0;x<8;x++){
        *_pBuf=pCol[x];
        _pBuf+=BufWidth;
      }
    }
    for(int y=0;y<4;y++){
      pCol=&SBImagePart8[(4+y)*8];
      _pBuf=&pBuf[(iGripPos+iGripSize-4+y)];
      for(s32 x=0;x<8;x++){
        *_pBuf=pCol[x];
        _pBuf+=BufWidth;
      }
    }
    pCol=&SBImageBody8[0];
    _pBuf=&pBuf[(iGripPos+0)];
    for(s32 y=0;y<8;y++){
      u16 *_pCol=&pCol[y];
      for(s32 x=2;x<(iGripSize-2);x++){
        _pBuf[x]=_pCol[(x&7)*8];
      }
      _pBuf+=BufWidth;
    }
  }
}

void MWin_DrawSBarVH(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->WinBuf==NULL) return;
  
  MWin_DrawSBar(WinIndex,&pwb->SBarV);
  MWin_DrawSBar(WinIndex,&pwb->SBarH);
  
  if((pwb->SBarV.Enabled==true)&&(pwb->SBarH.Enabled==true)){
    int ox,oy;
    
    ox=pwb->ClientRect.x+pwb->ClientRect.w;
    oy=pwb->ClientRect.y+pwb->ClientRect.h;
    
    for(int y=0;y<8;y++){
      u16 *pbuf=&pwb->WinBuf[ox+((oy+y)*pwb->Rect.w)];
      for(int x=0;x<8;x++){
        pbuf[x]=MWC_ClientBG;
      }
    }
  }
}

void MWin_RefreshWindow(u32 WinIndex)
{
  MWin_DrawTitleBar(WinIndex);
  MWin_DrawClientFrame(WinIndex);
  MWin_DrawSBarVH(WinIndex);
  MWin_DrawClient(WinIndex);
  MWin_TransWindow(WinIndex);
}

void MWin_SetWindowTitle(u32 WinIndex,char *TitleStr)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
//  sprintf(pwb->TitleStr,"%s",TitleStr);
  
  for(u32 cnt=0;cnt<128;cnt++){
    pwb->TitleStr[cnt]=TitleStr[cnt];
  }
  
  MWin_DrawTitleBar(WinIndex);
}

TWinBody* MWin_GetWinBody(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  return(pwb);
}

void MWin_SetSBarV(u32 WinIndex,s32 GripMax,s32 GripSize,s32 GripPos)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarV;
  
  psb->GripMax=GripMax;
  psb->GripSize=GripSize;
  
  MWin_SetSBarVPos(WinIndex,GripPos);
}

void MWin_SetSBarH(u32 WinIndex,s32 GripMax,s32 GripSize,s32 GripPos)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarH;
  
  psb->GripMax=GripMax;
  psb->GripSize=GripSize;
  
  MWin_SetSBarHPos(WinIndex,GripPos);
}

void MWin_SetSBarVPos(u32 WinIndex,s32 GripPos)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarV;
  
  if(psb->GripMax<(GripPos+psb->GripSize)) GripPos=psb->GripMax-psb->GripSize;
  if(GripPos<0) GripPos=0;
  
  psb->GripPos=GripPos;
}

void MWin_SetSBarHPos(u32 WinIndex,s32 GripPos)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarH;
  
  if(psb->GripMax<(GripPos+psb->GripSize)) GripPos=psb->GripMax-psb->GripSize;
  if(GripPos<0) GripPos=0;
  
  psb->GripPos=GripPos;
}

s32 MWin_GetSBarVPos(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->SBarV.Enabled==false) return(-1);
  
  return(pwb->SBarV.GripPos);
}

s32 MWin_GetSBarHPos(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  if(pwb->SBarH.Enabled==false) return(-1);
  
  return(pwb->SBarH.GripPos);
}

void MWin_SetSBarVPosFromPixel(u32 WinIndex,s32 px)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarV;
  
  float pos;
  
  pos=px*psb->GripMax;
  pos/=psb->ClientRect.h;
  pos-=psb->GripSize/2;
  psb->GripPos=(u32)pos;
  
  if(psb->GripPos>(psb->GripMax-psb->GripSize)) psb->GripPos=psb->GripMax-psb->GripSize;
  if(psb->GripPos<0) psb->GripPos=0;
}

void MWin_SetSBarHPosFromPixel(u32 WinIndex,s32 px)
{
  TWinBody *pwb=&WinBody[WinIndex];
  TSBar *psb=&pwb->SBarH;
  
  float pos;
  
  pos=px*psb->GripMax;
  pos/=psb->ClientRect.w;
  pos-=psb->GripSize/2;
  psb->GripPos=(u32)pos;
  
  if(psb->GripPos>(psb->GripMax-psb->GripSize)) psb->GripPos=psb->GripMax-psb->GripSize;
  if(psb->GripPos<0) psb->GripPos=0;
}

u32 MWin_GetWindowIndexFromPos(s32 x,s32 y)
{
  u32 SortIndex[WinBodyCount];
  u32 SortPriority[WinBodyCount];
  
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    
    SortIndex[cnt]=cnt;
    SortPriority[cnt]=pwb->Priority;
  }
  
  for(u32 c0=0;c0<WinBodyCount-1;c0++){
    for(u32 c1=c0+1;c1<WinBodyCount;c1++){
      if(SortPriority[c0]>SortPriority[c1]){
        u32 tmp;
        
        tmp=SortIndex[c0];
        SortIndex[c0]=SortIndex[c1];
        SortIndex[c1]=tmp;
        
        tmp=SortPriority[c0];
        SortPriority[c0]=SortPriority[c1];
        SortPriority[c1]=tmp;
      }
    }
  }
  
  u32 WinIndex=0xff;
  
  for(u32 sortcnt=0;sortcnt<WinBodyCount;sortcnt++){
    u32 cnt=SortIndex[sortcnt];
    TWinBody *pwb=&WinBody[cnt];
    
    if((pwb->Visible==true)&&(pwb->SelDisp==SelDisp_Top)){
      if((pwb->Rect.x<=x)&&(x<(pwb->Rect.x+pwb->Rect.w))){
        if((pwb->Rect.y<=y)&&(y<(pwb->Rect.y+pwb->Rect.h))){
          WinIndex=cnt;
        }
      }
    }
  }
  
  return(WinIndex);
}

u32 MWin_GetTopWinIndex(void)
{
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    TWinBody *pwb=&WinBody[cnt];
    if(pwb->Visible==true){
      if(pwb->TopFlag==true){
        return(cnt);
      }
    }
  }
  
  return(0xff);
}

s32 MWin_GetClientWidth(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  return(pwb->ClientRect.w);
}

s32 MWin_GetClientHeight(u32 WinIndex)
{
  TWinBody *pwb=&WinBody[WinIndex];
  
  return(pwb->ClientRect.h);
}

u32 ActiveWinIndex=0xff;
s32 mcx,mcy,mlx,mly;
bool HoldTitleBar=false;
bool HoldCloseButton=false;
bool HoldSBarV=false;
bool HoldSBarH=false;
bool HoldClient=false;

bool CheckInsideRect(TRect Rect,s32 x,s32 y)
{
  if((Rect.x<=x)&&(x<(Rect.x+Rect.w))){
    if((Rect.y<=y)&&(y<(Rect.y+Rect.h))){
      return(true);
    }
  }
  return(false);
}

void MWin_MouseDown(bool DispSwap,s32 x,s32 y)
{
  ActiveWinIndex=MWin_GetWindowIndexFromPos(x,y);
  if(DispSwap==false) ActiveWinIndex=WM_Picture;
  
  if(ActiveWinIndex!=WM_Picture){
    u32 DeactiveWinIndex=MWin_GetTopWinIndex();
    
    if(ActiveWinIndex!=DeactiveWinIndex){
      if(DeactiveWinIndex!=0xff){
        TWinBody *pwb=&WinBody[DeactiveWinIndex];
        
        pwb->TopFlag=false;
        MWin_DrawTitleBar(DeactiveWinIndex);
        MWin_TransWindow(DeactiveWinIndex);
      }
      
      if(ActiveWinIndex!=0xff){
        MWin_SetActive(ActiveWinIndex);
        MWin_RefreshScreenMask();
        
        MWin_DrawTitleBar(ActiveWinIndex);
        MWin_TransWindow(ActiveWinIndex);
      }
    }
  }
  
  if(ActiveWinIndex==0xff) return;
  
  TWinBody *pwb=&WinBody[ActiveWinIndex];
  
  mcx=x-pwb->Rect.x;
  mcy=y-pwb->Rect.y;
  
  HoldTitleBar=false;
  HoldCloseButton=false;
  HoldSBarV=false;
  HoldSBarH=false;
  HoldClient=false;
  
  if(CheckInsideRect(pwb->ClientRect,mcx,mcy)==true){
    HoldClient=true;
    }else{
    if(mcy<TitleBarHeight){
      if(mcx<(pwb->Rect.w-TitleBarHeight)){
        if(false){ // if(pwb->WinMode==WM_PicView){ // Picture window locked position.
          }else{
          HoldTitleBar=true;
        }
        }else{
        HoldCloseButton=true;
      }
    }
    if(pwb->SBarV.Enabled==true){
      if(CheckInsideRect(pwb->SBarV.ClientRect,mcx,mcy)==true) HoldSBarV=true;
    }
    if(pwb->SBarH.Enabled==true){
      if(CheckInsideRect(pwb->SBarH.ClientRect,mcx,mcy)==true) HoldSBarH=true;
    }
  }
  
  if(ActiveWinIndex==WM_Picture) HoldTitleBar=false; // lock window position.
  
  if(HoldTitleBar==true){};
  if(HoldCloseButton==true){};
  
  if(HoldSBarV==true){
    TSBar *psb=&pwb->SBarV;
    
    s32 px=mcy-psb->ClientRect.y;
    MWin_SetSBarVPosFromPixel(ActiveWinIndex,px);
    MWin_DrawSBar(ActiveWinIndex,&pwb->SBarV);
    MWin_DrawClient(ActiveWinIndex);
    MWin_TransWindow(ActiveWinIndex);
  }
  
  if(HoldSBarH==true){
    TSBar *psb=&pwb->SBarH;
    
    s32 px=mcx-psb->ClientRect.x;
    MWin_SetSBarHPosFromPixel(ActiveWinIndex,px);
    MWin_DrawSBar(ActiveWinIndex,&pwb->SBarH);
    MWin_DrawClient(ActiveWinIndex);
    MWin_TransWindow(ActiveWinIndex);
  }
  
  if(HoldClient==true){
    s32 x=mcx-pwb->ClientRect.x;
    s32 y=mcy-pwb->ClientRect.y;
    
    if(pwb->CallBack_MouseDown!=NULL) pwb->CallBack_MouseDown(ActiveWinIndex,x,y);
  }
  
  mlx=mcx;
  mly=mcy;
  
}

void MWin_MouseMove(bool DispSwap,s32 x,s32 y,bool CanIgnore)
{
  if(ActiveWinIndex==0xff) return;
  
  if(CanIgnore==true){
    if((HoldTitleBar==true)||(HoldSBarV==true)||(HoldSBarH==true)) return;
  }
  
  TWinBody *pwb=&WinBody[ActiveWinIndex];
  
  mcx=x-pwb->Rect.x;
  mcy=y-pwb->Rect.y;
  
  if((mcx==mlx)&&(mcy==mly)) return;
  
  if(HoldTitleBar==true){
    pwb->Rect.x+=mcx-mlx;
    pwb->Rect.y+=mcy-mly;
    MWin_RefreshScreenMask();
    MWin_TransWindow(ActiveWinIndex);
    MWin_ClearDesktop();
    for(u32 cnt=0;cnt<WinBodyCount;cnt++){
      if(cnt!=ActiveWinIndex) MWin_TransWindow(cnt);
    }
    mcx=x-pwb->Rect.x;
    mcy=y-pwb->Rect.y;
  }
  
  if(HoldSBarV==true){
    TSBar *psb=&pwb->SBarV;
    
    s32 px=mcy-psb->ClientRect.y;
    MWin_SetSBarVPosFromPixel(ActiveWinIndex,px);
    MWin_DrawSBar(ActiveWinIndex,&pwb->SBarV);
    MWin_DrawClient(ActiveWinIndex);
    MWin_TransWindow(ActiveWinIndex);
  }
  
  if(HoldSBarH==true){
    TSBar *psb=&pwb->SBarH;
    
    s32 px=mcx-psb->ClientRect.x;
    MWin_SetSBarHPosFromPixel(ActiveWinIndex,px);
    MWin_DrawSBar(ActiveWinIndex,&pwb->SBarH);
    MWin_DrawClient(ActiveWinIndex);
    MWin_TransWindow(ActiveWinIndex);
  }
  
  if(HoldClient==true){
    s32 x=mcx-pwb->ClientRect.x;
    s32 y=mcy-pwb->ClientRect.y;
    
    if(pwb->CallBack_MouseMove!=NULL) pwb->CallBack_MouseMove(ActiveWinIndex,x,y,CanIgnore);
  }
  
  mlx=mcx;
  mly=mcy;
  
}

void MWin_MouseUp(bool DispSwap)
{
  if(ActiveWinIndex==0xff) return;
  
  TWinBody *pwb=&WinBody[ActiveWinIndex];
  
  if(HoldCloseButton==true){
    if(pwb->CloseButton==true){
      if(pwb->CallBack_CloseButton!=NULL) pwb->CallBack_CloseButton(ActiveWinIndex);
    }
  }
  
  if(HoldTitleBar==true){};
  if(HoldSBarV==true){};
  if(HoldSBarH==true){};
  
  if(HoldClient==true){
    s32 x=mcx-pwb->ClientRect.x;
    s32 y=mcy-pwb->ClientRect.y;
    
    if(pwb->CallBack_MouseUp!=NULL) pwb->CallBack_MouseUp(ActiveWinIndex,x,y);
  }
  
  ActiveWinIndex=0xff;
  
  HoldTitleBar=false;
  HoldSBarV=false;
  HoldSBarH=false;
  HoldClient=false;
  
  mcx=0;
  mcy=0;
  mlx=mcx;
  mly=mcy;
}

#include "mwin_progress.h"

// ----------------------------------------------------

