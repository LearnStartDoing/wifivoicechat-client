
#include <stdio.h>

#include <NDS.h>

#include "_const.h"

#include "memtool.h"
#include "_console.h"

#include "unicode.h"

bool Unicode_isEqual(const UnicodeChar *s1,const UnicodeChar *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(*s1==*s2){
    if((*s1==0)||(*s2==0)){
      if((*s1==0)&&(*s2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    s1++;
    s2++;
  }
  return(false);
}

void Unicode_Add(UnicodeChar *s1,const UnicodeChar *s2)
{
  while(*s1!=0){
    s1++;
  }
  while(*s2!=0){
    *s1=*s2;
    s1++; s2++;
  }
  
  *s1=(UnicodeChar)0;
}

u32 Unicode_GetLength(const UnicodeChar *s)
{
  u32 len=0;
  
  while(*s!=0){
    len++;
    s++;
  }
  return(len);
}

void StrConvert_Ank2Unicode(const char *srcstr,UnicodeChar *dststr)
{
  while(*srcstr!=0){
    *dststr=(UnicodeChar)*srcstr;
    dststr++;
    srcstr++;
  }
  
  *dststr=(UnicodeChar)0;
}

void StrConvert_UTF82Unicode(const char *srcstr,UnicodeChar *dststr)
{
  while(*srcstr!=0){
    u16 b0=(byte)srcstr[0],b1=(byte)srcstr[1],b2=(byte)srcstr[2];
    u16 uc;
    
    if(b0<0x80){
      uc=b0;
      srcstr++;
      }else{
      if((b0&0xe0)==0xc0){ // 0b 110. ....
        uc=((b0&~0xe0)<<6)+((b1&~0xc0)<<0);
        srcstr+=2;
        }else{
        if((b0&0xf0)==0xe0){ // 0b 1110 ....
          uc=((b0&~0xf0)<<12)+((b1&~0xc0)<<6)+((b2&~0xc0)<<0);
          srcstr+=3;
          }else{
          uc=(u16)'?';
          srcstr+=4;
        }
      }
    }
    
    *dststr=uc;
    dststr++;
  }
  
  *dststr=(UnicodeChar)0;
}

static CglUnicode *pglUnicode;

void StrConvert_SetUnicode(CglUnicode *_pglUnicode)
{
  pglUnicode=_pglUnicode;
}

void StrConvert_Local2Unicode(const char *srcstr,UnicodeChar *dststr)
{
  while(*srcstr!=0){
    TglUnicode uidx;
    {
      u16 lidx=(u16)*srcstr++;
      if(pglUnicode->isAnkChar((char)lidx)==false){
        lidx=(lidx << 8) | ((u16)*srcstr++);
      }
      
      uidx=pglUnicode->GetUnicode(lidx);
    }
    
    *dststr++=uidx;
  }
  
  *dststr=(UnicodeChar)0;
}

