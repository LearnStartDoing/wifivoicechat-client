
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglscreen.h"

#define VRAMBuf ((u16*)&SPRITE_GFX_SUB[0])

//a global copy of sprite attribute memory
static SpriteEntry sprites_sub[128];

//rotation attributes overlap so assign then to the same location
static pSpriteRotation spriteRotations_SUB = (pSpriteRotation)sprites_sub;

//turn off all the sprites
static inline void initSprites(void)
{
  glMemSet16CPU(0, sprites_sub, 128 * sizeof(SpriteEntry));
  
  u32 i;
  for(i = 0; i < 128; i++)
  {
     sprites_sub[i].attribute[0] = ATTR0_DISABLED | 0;
     sprites_sub[i].attribute[1] = 0 | 0;
     sprites_sub[i].attribute[2] = 0;
     sprites_sub[i].attribute[3] = 0;
  }
  
  for(i=0;i<32;i++){
    spriteRotations_SUB[i].hdx=1*0x100;
    spriteRotations_SUB[i].hdy=0*0x100;
    spriteRotations_SUB[i].vdx=0*0x100;
    spriteRotations_SUB[i].vdy=1*0x100;
  }

}

//copy our sprite to object attribute memory
static inline void updateOAM(u32 Count)
{
  DC_FlushAll();
  glMemCopy16CPU(sprites_sub, OAM_SUB, Count * sizeof(SpriteEntry));
}

static inline void SetPosSprite(u32 objno,s32 x,s32 y,bool EnabledBlend,u32 FlipMode)
{
  switch(FlipMode){
    case 0: {
    } break;
    case 1: {
      x=256-64-x;
      y=192-64-y;
    } break;
    case 2: {
      y=192-64-y;
    } break;
    case 3: {
      x=256-64-x;
    } break;
  }
  
  u32 sx=x;
  u32 sy=y;
  
  if(x<0) sx=x+512;
  if(y<0) sy=y+256;
  
  sx&=512-1;
  sy&=256-1;
  
  sx=x & (512-1);
  sy=y & (256-1);
  
  u16 attr0=0;
  u16 attr1=0;
  
  attr0|=ATTR0_NORMAL | ATTR0_BMP | ATTR0_SQUARE | sy;
  
  if(EnabledBlend==false){
    attr0|=ATTR0_TYPE_NORMAL;
    }else{
    attr0|=ATTR0_TYPE_BLENDED;
  }
  
  attr1|=ATTR1_ROTDATA(0) | ATTR1_SIZE_64 | sx;
  
  switch(FlipMode){
    case 0: {
    } break;
    case 1: {
      attr1|=ATTR1_FLIP_X | ATTR1_FLIP_Y;
    } break;
    case 2: {
      attr1|=ATTR1_FLIP_Y;
    } break;
    case 3: {
      attr1|=ATTR1_FLIP_X;
    } break;
  }
  
  sprites_sub[objno].attribute[0] = attr0;
  sprites_sub[objno].attribute[1] = attr1;
}

static inline void HiddenSprite(u32 objno)
{
  sprites_sub[objno].attribute[0] = ATTR0_DISABLED;
}

static inline void SetSprite(u32 objno,u32 Priority,u32 num,u16 alpha)
{
  sprites_sub[objno].attribute[2] = ATTR2_PRIORITY(Priority) | ATTR2_ALPHA(alpha) | num;
}

void SubDisplay_Init(bool EnabledBlend,u16 alpha,u32 FlipMode)
{
  if(EnabledBlend==false) alpha=15;
  
  initSprites();
  updateOAM(128);
  
  s32 spx[12],spy[12];
  
  spx[ 0]=0; spy[ 0]=0;
  spx[ 1]=1; spy[ 1]=0;
  spx[ 2]=2; spy[ 2]=0;
  spx[ 3]=3; spy[ 3]=0;
  spx[ 4]=0; spy[ 4]=1;
  spx[ 5]=1; spy[ 5]=1;
  spx[ 6]=2; spy[ 6]=1;
  spx[ 7]=3; spy[ 7]=1;
  spx[ 8]=0; spy[ 8]=2;
  spx[ 9]=1; spy[ 9]=2;
  spx[10]=2; spy[10]=2;
  spx[11]=3; spy[11]=2;
  
  for(u32 idx=0;idx<12;idx++){
    SetSprite(idx,0,(8*spx[idx])+((8*8*4)*spy[idx]),alpha);
    
    s32 px=spx[idx];
    s32 py=spy[idx];
    
    px*=64;
    py*=64;
    
    SetPosSprite(idx,(s32)px,(s32)py,EnabledBlend,FlipMode);
  }
  
  updateOAM(12);
}

CglScreenSub::CglScreenSub(void)
{
  pCanvas=new CglCanvas(VRAMBuf,ScreenWidth,ScreenHeight,pf15bit);
  pCanvas->SetColor(RGB15(0,0,0));
  pCanvas->FillBox(0,0,ScreenWidth,ScreenHeight);
  
  FlipMode=0; // Normal
  
  SubDisplay_Init(false,15,FlipMode);
}

CglScreenSub::~CglScreenSub(void)
{
  delete pCanvas; pCanvas=NULL;
}

u16* CglScreenSub::GetVRAMBuf(void) const
{
  return(VRAMBuf);
}

void CglScreenSub::SetFlipMode(u32 _FlipMode)
{
  FlipMode=_FlipMode;
  
  SubDisplay_Init(false,15,FlipMode);
}
