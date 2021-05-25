
#include <stdio.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"

#include "VRAMTool.h"
#include "FontPro.h"
#include "memtool.h"

#define VRAMBuf ((u16*)(0x06000000))

void VRAM_SetPixel(s32 x,s32 y,u16 data)
{
  u32 dstadr=x+(y*ScreenWidth);
  
  VRAMBuf[dstadr]=data | BIT15;
}

void VRAM_ClearPixel(s32 x,s32 y)
{
  u32 dstadr=x+(y*ScreenWidth);
  
  VRAMBuf[dstadr]=0;
}

u16 VRAM_GetPixel(s32 x,s32 y)
{
  u32 dstadr=x+(y*ScreenWidth);
  
  return(VRAMBuf[dstadr] & (~BIT15));
}

void VRAM_SetCharLine(s32 y,s32 ysize,u16 col)
{
  u32 dstadr=y*ScreenWidth;
  u16 *dstbuf=&VRAMBuf[dstadr];
  
  col|=BIT15;
  
  MemSet16DMA3(col,dstbuf,256*ysize*2);
  return;
}

void VRAM_SetCharLineRect(u16 *pBuf,s32 BufWidth,s32 x,s32 y,s32 w,s32 h,u16 col)
{
  u16 *dstbuf=&pBuf[x+(y*BufWidth)];
  
  col|=BIT15;
  
  for(s32 y=0;y<h;y++){
    MemSet16DMA3(col,dstbuf,w*2);
    dstbuf+=BufWidth;
  }
  
}

void VRAM_WriteString(s32 x,s32 y,const char *str,u16 col)
{
  if(str==NULL) return;
  
  col|=BIT15;
  
  while(*str!=0){
    UnicodeChar ccode;
    
    ccode=(UnicodeChar)*str;
    str++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_WriteChar(x,y,ccode,col);
    x+=FontProWidth;
  }
}

void VRAM_WriteStringRect(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col)
{
  if(str==NULL) return;
  
  col|=BIT15;
  
  while(*str!=0){
    UnicodeChar ccode;
    
    ccode=(UnicodeChar)*str;
    str++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_WriteCharRect(pBuf,w,wLimit,x,y,ccode,col);
    x+=FontProWidth;
    
    if(wLimit<=x) break;
  }
}

u32 VRAM_GetStringWidth(const char *str)
{
  if(str==NULL) return(0);
  
  u32 x=0;
  
  while(*str!=0){
    UnicodeChar ccode;
    
    ccode=(UnicodeChar)*str;
    str++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_GetCharWidth(ccode);
    x+=FontProWidth;
  }
  
  return(x);
}

void VRAM_WriteStringRectL(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col)
{
  if(str==NULL) return;
  if(*str==0) return;
  
  UnicodeChar *unistr=(UnicodeChar*)safemalloc((strlen(str)+1)*2);
  
  StrConvert_Local2Unicode(str,unistr);
  
  VRAM_WriteStringRectW(pBuf,w,wLimit,x,y,unistr,col);
  
  safefree(unistr); unistr=NULL;
}

void VRAM_WriteStringRectUTF8(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col)
{
  if(str==NULL) return;
  if(*str==0) return;
  
  UnicodeChar *unistr=(UnicodeChar*)safemalloc((strlen(str)+1)*2);
  
  StrConvert_UTF82Unicode(str,unistr);
  
  VRAM_WriteStringRectW(pBuf,w,wLimit,x,y,unistr,col);
  
  safefree(unistr); unistr=NULL;
}

u32 VRAM_GetStringWidthUTF8(const char *str)
{
  if(str==NULL) return(0);
  
  UnicodeChar *unistrmst=(UnicodeChar*)safemalloc((strlen(str)+1)*2);
  UnicodeChar *unistr=unistrmst;
  
  StrConvert_UTF82Unicode(str,unistr);
  
  u32 x=0;
  
  while(*unistr!=0){
    UnicodeChar uc;
    
    uc=*unistr;
    unistr++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_GetCharWidth(uc);
    x+=FontProWidth;
  }
  
  safefree(unistrmst); unistrmst=NULL;
  
  return(x);
}

void VRAM_WriteStringRectW(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const UnicodeChar *str,u16 col)
{
  if(str==NULL) return;
  if(*str==0) return;
  
  col|=BIT15;
  
  while(*str!=0){
    UnicodeChar uc;
    
    uc=*str;
    str++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_WriteCharRect(pBuf,w,wLimit,x,y,uc,col);
    x+=FontProWidth;
    if(0x100<uc) x++;
    
    if(wLimit<=x) break;
  }
}

u32 VRAM_GetStringWidthW(const UnicodeChar *str)
{
  if(str==NULL) return(0);
  
  u32 x=0;
  
  while(*str!=0){
    UnicodeChar uc;
    
    uc=*str;
    str++;
    
    u32 FontProWidth;
    FontProWidth=FontPro_GetCharWidth(uc);
    x+=FontProWidth;
  }
  
  return(x);
}

void VRAM_Clear(void)
{
  MemSet16DMA3(0x294c | BIT15,&VRAMBuf[0],256*256*2);
}

