
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglcanvas.h"

#include "cp0_glf_bin.h"
#include "cp0_glu_bin.h"

static CglFont *pCglFontDefault=NULL;
static CglUnicode *pCglUnicodeDefault=NULL;

CglCanvas::CglCanvas(u16 *_VRAMBuf,const int _Width,const int _Height,const EPixelFormat _PixelFormat)
{
  VRAMBuf=NULL;
  VRAMBufInsideAllocatedFlag=false;
  Width=0;
  Height=0;
  ScanLine=NULL;
  Color=0;
  LastX=0;
  LastY=0;
  
  if(pCglFontDefault==NULL){
    pCglFontDefault=new CglFont((u8*)cp0_glf_bin,cp0_glf_bin_size);
  }
  pCglFont=pCglFontDefault;
  
  if(pCglUnicodeDefault==NULL){
    pCglUnicodeDefault=new CglUnicode((u8*)cp0_glu_bin,cp0_glu_bin_size);
  }
  pCglUnicode=pCglUnicodeDefault;
  
  SetVRAMBuf(_VRAMBuf,_Width,_Height,_PixelFormat);
}

CglCanvas::~CglCanvas(void)
{
  if(VRAMBufInsideAllocatedFlag==true){
    VRAMBufInsideAllocatedFlag=false;
    glsafefree(VRAMBuf); VRAMBuf=NULL;
  }
  if(ScanLine!=NULL){
    glsafefree(ScanLine); ScanLine=NULL;
  }
}

u16* CglCanvas::GetVRAMBuf(void) const
{
  return(VRAMBuf);
}

void CglCanvas::SetVRAMBuf(u16 *_VRAMBuf,const int _Width,const int _Height,const EPixelFormat _PixelFormat)
{
  if(VRAMBufInsideAllocatedFlag==true){
    VRAMBufInsideAllocatedFlag=false;
    glsafefree(VRAMBuf); VRAMBuf=NULL;
  }
  
  Width=_Width;
  
  if(Height!=_Height){
    Height=_Height;
    if(ScanLine!=NULL){
      glsafefree(ScanLine); ScanLine=NULL;
    }
    ScanLine=(u16**)glsafemalloc(Height*sizeof(u16*));
  }
  
  PixelFormat=_PixelFormat;
  
  if(_VRAMBuf==NULL){
    VRAMBufInsideAllocatedFlag=true;
    VRAMBuf=(u16*)glsafemalloc(Width*Height*sizeof(u16));
    }else{
    VRAMBufInsideAllocatedFlag=false;
    VRAMBuf=_VRAMBuf;
  }
  
  for(int y=0;y<Height;y++){
    ScanLine[y]=&VRAMBuf[y*Width];
  }
}

int CglCanvas::GetWidth(void) const
{
  return(Width);
}

int CglCanvas::GetHeight(void) const
{
  return(Height);
}

u16* CglCanvas::GetScanLine(const int y) const
{
  if((y<0)||(Height<=y)) return(NULL);
  
  return(ScanLine[y]);
}

bool CglCanvas::isInsidePosition(const int x,const int y) const
{
  if((x<0)||(Width<=x)) return(false);
  if((y<0)||(Height<=y)) return(false);
  
  return(true);
}

void CglCanvas::SetPixel(const int x,const int y,const u16 rgb)
{
//  if(isInsidePosition(x,y)==false) return;
  
  ScanLine[y][x]=rgb;
}

void CglCanvas::SetPixelHalf(const int x,const int y,const u16 rgb)
{
//  if(isInsidePosition(x,y)==false) return;
  
  ScanLine[y][x]=ColorMargeHalf(ScanLine[y][x],rgb);
}

void CglCanvas::SetPixelAlpha(const int x,const int y,const u16 rgb,const int Alpha)
{
//  if(isInsidePosition(x,y)==false) return;
  
  ScanLine[y][x]=ColorMargeAlpha(ScanLine[y][x],rgb,Alpha);
}

void CglCanvas::SetPixelAlphaAdd(const int x,const int y,const u16 rgb,const int Alpha)
{
//  if(isInsidePosition(x,y)==false) return;
  
  ScanLine[y][x]=ColorMargeAlphaAdd(ScanLine[y][x],rgb,Alpha);
}

u16 CglCanvas::GetPixel(const int x,const int y) const
{
//  if(isInsidePosition(x,y)==false) return(0);
  
  return(ScanLine[y][x]);
}

void CglCanvas::SetColor(const u16 _Color)
{
  Color=_Color;
}

void CglCanvas::DrawLine(const int x1,const int y1,const int x2,const int y2)
{
  if((x1==x2)&&(y1==y2)) return;
  
  if(x1==x2){
    int ys,ye;
    if(y1<y2){
      ys=y1;
      ye=y2-1;
      }else{
      ys=y2+1;
      ye=y1;
    }
    for(int py=ys;py<=ye;py++){
      SetPixel(x1+0,py+0,Color);
    }
    return;
  }
  
  if(y1==y2){
    int xs,xe;
    if(x1<x2){
      xs=x1;
      xe=x2-1;
      }else{
      xs=x2+1;
      xe=x1;
    }
    for(int px=xs;px<=xe;px++){
      SetPixel(px+0,y1+0,Color);
    }
    return;
  }
  
  if(abs(x2-x1)>abs(y2-y1)){
    int px=0;
    int pyf8=0;
    int xe=x2-x1;
    int yef8=(y2-y1)<<8;
    int xv;
    int yvf8;
    
    if(0<xe){
      xv=1;
      }else{
      xv=-1;
    }
    yvf8=yef8/abs(xe);
    
    while(px!=xe){
      u32 dx=x1+px;
      u32 dy=y1+(int)(pyf8>>8);
      SetPixel(dx+0,dy+0,Color);
      px+=xv;
      pyf8+=yvf8;
    }
    return;
    
    }else{
    int pxf8=0;
    int py=0;
    int xef8=(x2-x1)<<8;
    int ye=y2-y1;
    int xvf8;
    int yv;
    
    xvf8=xef8/abs(ye);
    if(0<ye){
      yv=1;
      }else{
      yv=-1;
    }
    
    while(py!=ye){
      u32 dx=x1+(int)(pxf8>>8);
      u32 dy=y1+py;
      SetPixel(dx+0,dy+0,Color);
      pxf8+=xvf8;
      py+=yv;
    }
    return;
  }
}

void CglCanvas::DrawTickLine(const int x1,const int y1,const int x2,const int y2)
{
  if((x1==x2)&&(y1==y2)) return;
  
  if(x1==x2){
    int ys,ye;
    if(y1<y2){
      ys=y1;
      ye=y2-1;
      }else{
      ys=y2+1;
      ye=y1;
    }
    for(int py=ys;py<=ye;py++){
      SetPixelHalf(x1-1,py+0,Color);
      SetPixel(x1+0,py+0,Color);
      SetPixelHalf(x1+1,py+0,Color);
    }
    return;
  }
  
  if(y1==y2){
    int xs,xe;
    if(x1<x2){
      xs=x1;
      xe=x2-1;
      }else{
      xs=x2+1;
      xe=x1;
    }
    for(int px=xs;px<=xe;px++){
      SetPixelHalf(px+0,y1-1,Color);
      SetPixel(px+0,y1+0,Color);
      SetPixelHalf(px+0,y1+1,Color);
    }
    return;
  }
  
  if(abs(x2-x1)>abs(y2-y1)){
    int px=0;
    int pyf8=0;
    int xe=x2-x1;
    int yef8=(y2-y1)<<8;
    int xv;
    int yvf8;
    
    if(0<xe){
      xv=1;
      }else{
      xv=-1;
    }
    yvf8=yef8/abs(xe);
    
    while(px!=xe){
      u32 dx=x1+px;
      u32 dy=y1+(int)(pyf8>>8);
      SetPixelHalf(dx+0,dy-1,Color);
      SetPixel(dx+0,dy+0,Color);
      SetPixelHalf(dx+0,dy+1,Color);
      px+=xv;
      pyf8+=yvf8;
    }
    return;
    
    }else{
    int pxf8=0;
    int py=0;
    int xef8=(x2-x1)<<8;
    int ye=y2-y1;
    int xvf8;
    int yv;
    
    xvf8=xef8/abs(ye);
    if(0<ye){
      yv=1;
      }else{
      yv=-1;
    }
    
    while(py!=ye){
      u32 dx=x1+(int)(pxf8>>8);
      u32 dy=y1+py;
      SetPixelHalf(dx-1,dy+0,Color);
      SetPixel(dx+0,dy+0,Color);
      SetPixelHalf(dx+1,dy+0,Color);
      pxf8+=xvf8;
      py+=yv;
    }
    return;
  }
}

void CglCanvas::MoveTo(const int x,const int y)
{
  LastX=x;
  LastY=y;
}

void CglCanvas::LineTo(const int x,const int y)
{
  DrawLine(LastX,LastY,x,y);
  
  LastX=x;
  LastY=y;
}


void CglCanvas::FillBox(const int x,const int y,const int w,const int h)
{
  for(int py=y;py<(y+h);py++){
/*
    for(int px=x;px<(x+w);px++){
      SetPixel(px,py,Color);
    }
*/
    u16 *pbuf=GetScanLine(py);
    if(pbuf!=NULL){
      glMemSet16CPU(Color,&pbuf[x],w*2);
    }
  }
}

void CglCanvas::DrawBox(const int x,const int y,const int w,const int h)
{
  if((w==0)||(h==0)) return;
  
  if((w==1)&&(h==1)){
    SetPixel(x,y,Color);
    return;
  }
  
  if(w==1){
    DrawLine(x,y,x,y+h);
    return;
  }
  
  if(h==1){
    DrawLine(x,y,x+w,y);
    return;
  }
  
  const int x1=x,y1=y,x2=x+w-1,y2=y+h-1;
  
  DrawLine(x1,y1,x2,y1);
  DrawLine(x2,y1,x2,y2);
  DrawLine(x2,y2,x1,y2);
  DrawLine(x1,y2,x1,y1);
}

void CglCanvas::SetFontBGColor(const u16 Color)
{
  CglFont *pFont=(CglFont*)pCglFont;
  
  pFont->SetBGColor(Color);
}

void CglCanvas::SetFontTextColor(const u16 Color)
{
  CglFont *pFont=(CglFont*)pCglFont;
  
  pFont->SetTextColor(Color);
}

void CglCanvas::TextOutA(const int x,const int y,const char *str) const
{
  int dx=x;
  int dy=y;
  
  CglFont *pFont=(CglFont*)pCglFont;
  
  while(*str!=0){
    TglUnicode uidx=(TglUnicode)*str++;
    pFont->DrawFont((CglCanvas*)this,dx,dy,uidx);
    dx+=pFont->GetFontWidth(uidx);
  }
}

void CglCanvas::TextOutL(const int x,const int y,const char *str) const
{
  int dx=x;
  int dy=y;
  
  CglFont *pFont=(CglFont*)pCglFont;
  CglUnicode *pUnicode=(CglUnicode*)pCglUnicode;
  
  while(*str!=0){
    TglUnicode uidx;
    {
      u16 lidx=(u16)*str++;
      if(pUnicode->isAnkChar((char)lidx)==false){
        lidx=(lidx << 8) | ((u16)*str++);
      }
      
      uidx=pUnicode->GetUnicode(lidx);
    }
    
    pFont->DrawFont((CglCanvas*)this,dx,dy,uidx);
    dx+=pFont->GetFontWidth(uidx);
  }
}

void CglCanvas::TextOutW(const int x,const int y,const TglUnicode *str) const
{
  int dx=x;
  int dy=y;
  
  CglFont *pFont=(CglFont*)pCglFont;
  
  while(*str!=0){
    pFont->DrawFont((CglCanvas*)this,dx,dy,*str);
    dx+=pFont->GetFontWidth(*str);
    str++;
  }
}

int CglCanvas::GetTextWidthA(const char *str) const
{
  int w=0;
  
  CglFont *pFont=(CglFont*)pCglFont;
  
  while(*str!=0){
    TglUnicode uidx=(TglUnicode)*str++;
    w+=pFont->GetFontWidth(uidx);
  }
  
  return(w);
}

int CglCanvas::GetTextWidthL(const char *str) const
{
  int w=0;
  
  CglFont *pFont=(CglFont*)pCglFont;
  CglUnicode *pUnicode=(CglUnicode*)pCglUnicode;
  
  while(*str!=0){
    TglUnicode uidx;
    {
      u16 lidx=(u16)*str++;
      if(pUnicode->isAnkChar((char)lidx)==false){
        lidx=(lidx << 8) | ((u16)*str++);
      }
      
      uidx=pUnicode->GetUnicode(lidx);
    }
    
    w+=pFont->GetFontWidth(uidx);
  }
  
  return(w);
}

int CglCanvas::GetTextWidthW(const TglUnicode *str) const
{
  int w=0;
  
  CglFont *pFont=(CglFont*)pCglFont;
  
  while(*str!=0){
    w+=pFont->GetFontWidth(*str);
    str++;
  }
  
  return(w);
}

void CglCanvas::SetCglFont(void *_pCglFont)
{
  if(_pCglFont==NULL){
    pCglFont=pCglFontDefault;
    }else{
    pCglFont=_pCglFont;
  }
}

void CglCanvas::SetCglUnicode(void *_pCglUnicode)
{
  if(_pCglUnicode==NULL){
    pCglUnicode=pCglUnicodeDefault;
    }else{
    pCglUnicode=_pCglUnicode;
  }
}

void CglCanvas::BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop,const bool TransFlag) const
{
  if(TransFlag==false){
    BitBltBeta(pDestCanvas,nDestLeft,nDestTop,nWidth,nHeight,nSrcLeft,nSrcTop);
    }else{
    BitBltTrans(pDestCanvas,nDestLeft,nDestTop,nWidth,nHeight,nSrcLeft,nSrcTop);
  }
}

void CglCanvas::BitBltBeta(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const
{
  int w=nWidth;
  int h=nHeight;
  
  if(Width<=nSrcLeft) return;
  if(Height<=nSrcTop) return;
  
  if(Width<(nSrcLeft+w)) w=Width-nSrcLeft;
  if(Height<(nSrcTop+h)) h=Height-nSrcTop;
  
  u16 *pdata=&VRAMBuf[nSrcLeft+(nSrcTop*Width)];
  
  for(int y=0;y<h;y++){
    for(int x=0;x<w;x++){
      pDestCanvas->SetPixel(nDestLeft+x,nDestTop+y,pdata[x]);
    }
    pdata+=Width;
  }
}

void CglCanvas::BitBltTrans(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const
{
  int w=nWidth;
  int h=nHeight;
  
  if(Width<=nSrcLeft) return;
  if(Height<=nSrcTop) return;
  
  if(Width<(nSrcLeft+w)) w=Width-nSrcLeft;
  if(Height<(nSrcTop+h)) h=Height-nSrcTop;
  
  u16 *pdata=&VRAMBuf[nSrcLeft+(nSrcTop*Width)];
  
  for(int y=0;y<h;y++){
    for(int x=0;x<w;x++){
      u16 data=pdata[x];
      if(data!=0){
        pDestCanvas->SetPixel(nDestLeft+x,nDestTop+y,data);
      }
    }
    pdata+=Width;
  }
}

