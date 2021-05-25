
#include <stdio.h>
#include <stdlib.h>

#include <NDS.h>


#include "_console.h"

#include "memtool.h"

#include "FontPro.h"
#include "unicode.h"

#include "_const.h"

#define VRAMBuf ((u16*)(0x06000000))

static u8 *pfonbuf=NULL;

static u32 FontCount;
static u32 FontHeight;
static u8 *FontData;

typedef struct {
  u8 *data;
} TFontCache;

#define FontCacheCount (0x80)
static TFontCache *FontCache=NULL;

static void CreateFontCache(UnicodeChar ccode)
{
  if(FontCount<=ccode) return;
  
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return;
  
  FontCache[ccode].data=(u8*)safemalloc(FontProWidth*FontProHeight);
  
  u8 *dstbuf=&FontCache[ccode].data[0];
  
  u32 BitCount;
  u32 BitImage;
  
  BitCount=8;
  BitImage=*pDataPtr;
  pDataPtr++;
  
//  _consolePrintf("%x %dx%d %x\n",ccode,FontProWidth,FontProHeight,*pOffsetPtr);
  
  for(u32 y=0;y<FontProHeight;y++){
    for(u32 x=0;x<FontProWidth;x++){
      *dstbuf=BitImage & 0x01;
      dstbuf++;
      BitCount--;
      BitImage>>=1;
      if(BitCount==0){
        BitCount=8;
        BitImage=*pDataPtr;
        pDataPtr++;
      }
    }
  }
}

void FontPro_Init(u8 *pfon)
{
  FontPro_Free();
  
  pfonbuf=pfon;
  
  FontHeight=*(u16*)&pfon[0];
  FontCount=*(u16*)&pfon[2];
  pfon+=4;
  
  FontData=(u8*)pfon;
  
  if(FontCount==0){
    _consolePrintf("not include fontdata.\n");
    ShowLogHalt();
  }
  if(FontProHeight!=FontHeight){
    _consolePrintf("not support (FontProHeight!=FontHeight)\n");
    ShowLogHalt();
  }
  
  _consolePrintf("Create FontCache.\n");
  FontCache=(TFontCache*)safemalloc(FontCacheCount*sizeof(TFontCache));
  for(UnicodeChar ccode=0;ccode<FontCacheCount;ccode++){
    FontCache[ccode].data=NULL;
    CreateFontCache(ccode);
  }
  
  _consolePrintf("Proportional Font Initialized.\n");
  
}

void FontPro_Free(void)
{
  if(pfonbuf!=NULL){
    safefree(pfonbuf); pfonbuf=NULL;
  }
  
  if(FontCache!=NULL){
    for(UnicodeChar ccode=0;ccode<FontCacheCount;ccode++){
      safefree(FontCache[ccode].data); FontCache[ccode].data=NULL;
    }
    safefree(FontCache); FontCache=NULL;
  }
}

static u32 FontProCache_WriteChar(s32 x,s32 y,UnicodeChar ccode,u16 col)
{
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  u8 *pCacheDataPtr=(u8*)&FontCache[ccode].data;
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(4);
  
  u32 dstadr=(x+0)+(y*ScreenWidth);
  u16 *dstbuf=&VRAMBuf[dstadr];
  
  col|=BIT(15);
  
  for(u32 y=0;y<FontProHeight;y++){
    for(u32 x=0;x<FontProWidth;x++){
      if(*pCacheDataPtr!=0) dstbuf[x]=col;
      pCacheDataPtr++;
    }
    dstbuf+=ScreenWidth;
  }
  
  return(FontProWidth);
}

u32 FontPro_WriteChar(s32 x,s32 y,UnicodeChar ccode,u16 col)
{
  if(FontCount<=ccode) return(4);
  
  if(ccode<FontCacheCount){
    if(FontCache[ccode].data!=NULL){
      return(FontProCache_WriteChar(x,y,ccode,col));
    }
  }
  
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(4);
  
  u32 dstadr=(x+0)+(y*ScreenWidth);
  u16 *dstbuf=&VRAMBuf[dstadr];
  
  u32 BitCount;
  u32 BitImage;
  
  BitCount=8;
  BitImage=*pDataPtr;
  pDataPtr++;
  
  col|=BIT(15);
  
  for(u32 y=0;y<FontProHeight;y++){
    for(u32 x=0;x<FontProWidth;x++){
      if((BitImage & 0x01)!=0) dstbuf[x]=col;
      BitCount--;
      BitImage>>=1;
      if(BitCount==0){
        BitCount=8;
        BitImage=*pDataPtr;
        pDataPtr++;
      }
    }
    dstbuf+=ScreenWidth;
  }
  
  return(FontProWidth);
}

static u32 FontProCache_WriteCharRect(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,UnicodeChar ccode,u16 col)
{
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  u8 *pCacheDataPtr=(u8*)&FontCache[ccode].data[0];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(4);
  
  if((x+(s32)FontProWidth)>wLimit) return(FontProWidth);
  
  u32 dstadr=(x+0)+(y*w);
  u16 *dstbuf=&pBuf[dstadr];
  
  col|=BIT(15);
  
  for(u32 y=0;y<FontProHeight;y++){
    for(u32 x=0;x<FontProWidth;x++){
      if(*pCacheDataPtr!=0) dstbuf[x]=col;
      pCacheDataPtr++;
    }
    dstbuf+=w;
  }
  
  return(FontProWidth);
}

u32 FontPro_WriteCharRect(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,UnicodeChar ccode,u16 col)
{
  if(FontCount<=ccode) return(4);
  
  if(ccode<FontCacheCount){
    if(FontCache[ccode].data!=NULL){
      return(FontProCache_WriteCharRect(pBuf,w,wLimit,x,y,ccode,col));
    }
  }
  
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(4);
  
  if((x+(s32)FontProWidth)>wLimit) return(FontProWidth);
  
  u32 dstadr=(x+0)+(y*w);
  u16 *dstbuf=&pBuf[dstadr];
  
  u32 BitCount;
  u32 BitImage;
  
  BitCount=8;
  BitImage=*pDataPtr;
  pDataPtr++;
  
  col|=BIT(15);
  
  for(u32 y=0;y<FontProHeight;y++){
    for(u32 x=0;x<FontProWidth;x++){
      if((BitImage & 0x01)!=0) dstbuf[x]=col;
      BitCount--;
      BitImage>>=1;
      if(BitCount==0){
        BitCount=8;
        BitImage=*pDataPtr;
        pDataPtr++;
      }
    }
    dstbuf+=w;
  }
  
  return(FontProWidth);
}

u32 FontPro_GetCharWidth(u32 ccode)
{
  if(FontCount<=ccode) return(4);
  
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(4);
  
  return(FontProWidth);
}

bool FontPro_Exists(u32 ccode)
{
  if(FontCount<=ccode) return(false);
  
  u32 *pOffsetPtr=(u32*)&FontData[ccode*4];
  u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];
  
  u32 FontProWidth=*pDataPtr;
  pDataPtr++;
  
  if(FontProWidth==0) return(false);
  
  return(true);
}

