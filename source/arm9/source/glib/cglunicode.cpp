
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglunicode.h"

#include "cglstream.h"

CglUnicode::CglUnicode(const u8 *_buf,const int _size)
{
  CglStream stream(_buf,_size);
  
  CodePage=stream.Readu16();
  
  for(int idx=0;idx<0x100;idx++){
    if(stream.Readu8()==0){
      AnkTable[idx]=false;
      }else{
      AnkTable[idx]=true;
    }
  }
  
  Count=stream.Readu16();
  
  Local2UnicodeTable=(TglUnicode*)glsafemalloc(Count*sizeof(TglUnicode));
  
  stream.ReadBuffer(Local2UnicodeTable,Count*sizeof(TglUnicode));
}

CglUnicode::~CglUnicode(void)
{
  glsafefree(Local2UnicodeTable); Local2UnicodeTable=NULL;
}

bool CglUnicode::isAnkChar(unsigned char test) const
{
  return(AnkTable[test]);
}

TglUnicode CglUnicode::GetUnicode(u16 LocalChar) const
{
  if(Count<=LocalChar) return((TglUnicode)'?');
  
  return(Local2UnicodeTable[LocalChar]);
}

