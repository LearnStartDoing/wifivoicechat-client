
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglfont.h"

#include "cglstream.h"

CglFont::CglFont(const u8 *_buf,const int _size)
{
  CglStream stream(_buf,_size);
  
  Height=stream.Readu16();
  
  int bpp=stream.Readu16();
  
  switch(bpp){
    case 0: BPP=FontBPP0bit; break;
    case 1: BPP=FontBPP1bit; break;
    case 2: BPP=FontBPP2bit; break;
    default: BPP=FontBPP0bit; break;
  }
  
  Count=stream.Readu16();
  
  DataTable=(u8**)glsafemalloc(Count*sizeof(u8*));
  
  stream.ReadBuffer(DataTable,Count*sizeof(u8*));
  
  int DataSize;
  
  DataSize=stream.GetSize()-stream.GetOffset();
  Data=(u8*)glsafemalloc(DataSize);
  
  stream.ReadBuffer(Data,DataSize);
  
  for(int idx=0;idx<Count;idx++){
    int ofs=(int)DataTable[idx];
    DataTable[idx]=&Data[ofs];
  }
  
  BGColor=RGB15(31,31,31)|BIT15;
  TextColor=RGB15(0,0,0)|BIT15;
  
  WidthPadding=0;
  HeightPadding=0;
  
  Transparent=true;
  AlphaBlend=true;
}

CglFont::~CglFont(void)
{
  glsafefree(DataTable); DataTable=NULL;
  glsafefree(Data); Data=NULL;
}

u8* CglFont::GetBulkData(const TglUnicode uidx) const
{
  return(DataTable[uidx]);
}

void CglFont::DrawFont1bpp(CglCanvas *pCanvas,const int x,const int y,const u8 *BulkData) const
{
  int Width=*BulkData++;
  if(Width==0) return;
  
  u32 BitCount;
  u32 BitImage;
  
  BitCount=8;
  BitImage=*BulkData;
  BulkData++;
  
  for(int py=0;py<Height;py++){
    for(int px=0;px<Width;px++){
      if((BitImage & 0x01)!=0){
        pCanvas->SetPixel(x+px,y+py,TextColor);
        }else{
        if(Transparent==false){
          pCanvas->SetPixel(x+px,y+py,BGColor);
        }
      }
      BitCount--;
      BitImage>>=1;
      if(BitCount==0){
        BitCount=8;
        BitImage=*BulkData;
        BulkData++;
      }
    }
  }
  
}

void CglFont::DrawFont2bpp(CglCanvas *pCanvas,const int x,const int y,const u8 *BulkData) const
{
  int Width=*BulkData++;
  if(Width==0) return;
  
  u32 BitCount;
  u32 BitImage;
  
  BitCount=8;
  BitImage=*BulkData;
  BulkData++;
  
  u16 NoTransColor[4];
  
  if(Transparent==false){
    NoTransColor[0]=BGColor;
    NoTransColor[1]=ColorMargeAlpha(BGColor,TextColor,12);
    NoTransColor[2]=ColorMargeAlpha(BGColor,TextColor,24);
    NoTransColor[3]=TextColor;
  }
  
  for(int py=0;py<Height;py++){
    for(int px=0;px<Width;px++){
      u32 bright=BitImage & 0x03;
      if(Transparent==false){
        pCanvas->SetPixel(x+px,y+py,NoTransColor[bright]);
        }else{
        if(AlphaBlend==false){
          if(bright!=0) pCanvas->SetPixel(x+px,y+py,NoTransColor[bright]);
          }else{
          switch(bright){
            case 1: pCanvas->SetPixelAlpha(x+px,y+py,TextColor,12); break;
            case 2: pCanvas->SetPixelAlpha(x+px,y+py,TextColor,24); break;
            case 3: pCanvas->SetPixel(x+px,y+py,TextColor); break;
          }
        }
      }
      BitCount-=2;
      BitImage>>=2;
      if(BitCount==0){
        BitCount=8;
        BitImage=*BulkData;
        BulkData++;
      }
    }
  }
  
}

void CglFont::DrawFont(CglCanvas *pCanvas,const int x,const int y,const TglUnicode uidx) const
{
  switch(BPP){
    case FontBPP0bit: break;
    case FontBPP1bit: DrawFont1bpp(pCanvas,x,y,GetBulkData(uidx)); break;
    case FontBPP2bit: DrawFont2bpp(pCanvas,x,y,GetBulkData(uidx)); break;
    default: break;
  }
}

int CglFont::GetFontWidth(const TglUnicode uidx) const
{
  u8 *BulkData=GetBulkData(uidx);
  
  int Width=*BulkData;
  
  if(0x100<=uidx) Width++;
  
  return(Width+WidthPadding);
}

int CglFont::GetFontHeight(void) const
{
  return(Height+HeightPadding);
}

void CglFont::SetBGColor(const u16 Color)
{
  BGColor=Color;
}

void CglFont::SetTextColor(const u16 Color)
{
  TextColor=Color;
}

void CglFont::SetPadding(const int Width,const int Height)
{
  WidthPadding=Width;
  HeightPadding=Height;
}

void CglFont::SetTransparent(const bool _Transparent)
{
  Transparent=_Transparent;
}

void CglFont::SetAlphaBlend(const bool _AlphaBlend)
{
  AlphaBlend=_AlphaBlend;
}

