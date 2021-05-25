
#ifndef glib_h
#define glib_h

#include <NDS.h>

#define GLIBID "glib ver 0.1 by Moonlight."

#ifndef ScreenWidth
#define ScreenWidth (256)
#endif

#ifndef ScreenHeight
#define ScreenHeight (192)
#endif

#ifndef BIT15
#define BIT15 (1<<15)
#endif

#include "glglobal.h"

#include "cglscreen.h"
#include "cglcanvas.h"
#include "cglfont.h"
#include "cglunicode.h"
#include "cglb15.h"
#include "cgltgf.h"

static inline u16 ColorMargeAlpha(const u16 col1,const u16 col2,const int Alpha)
{
  if(Alpha==32) return(col1 | BIT15);
  if(Alpha==0) return(col2 | BIT15);
  
  u32 r1,g1,b1;
  u32 Alpha1=32-Alpha;
  
  r1=((col1>>0)&0x1f)*Alpha1/32;
  g1=((col1>>5)&0x1f)*Alpha1/32;
  b1=((col1>>10)&0x1f)*Alpha1/32;
  
  u32 r2,g2,b2;
  u32 Alpha2=Alpha;
  
  r2=((col2>>0)&0x1f)*Alpha2/32;
  g2=((col2>>5)&0x1f)*Alpha2/32;
  b2=((col2>>10)&0x1f)*Alpha2/32;
  
  return(RGB15(r1+r2,g1+g2,b1+b2)|BIT15);
}

static inline u16 ColorMargeHalf(const u16 col1,const u16 col2)
{
  u32 c1=col1;
  u32 c2=col2;
  
  c1=(c1 & RGB15(30,30,30)) >> 1;
  c2=(c2 & RGB15(30,30,30)) >> 1;
  
  return((c1+c2)|BIT15);
}

static inline u16 ColorMargeAlphaAdd(const u16 col1,const u16 col2,const int Alpha)
{
  if(Alpha==0) return(col1 | BIT15);
  
  u32 r1,g1,b1;
  u32 Alpha1=32-Alpha;
  
  r1=((col1>>0)&0x1f)*Alpha1/32;
  g1=((col1>>5)&0x1f)*Alpha1/32;
  b1=((col1>>10)&0x1f)*Alpha1/32;
  
  u32 r2,g2,b2;
  
  r2=((col2>>0)&0x1f);
  g2=((col2>>5)&0x1f);
  b2=((col2>>10)&0x1f);
  
  return(RGB15(r1+r2,g1+g2,b1+b2)|BIT15);
}

#endif

