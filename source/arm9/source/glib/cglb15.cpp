
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglb15.h"

#include "cglstream.h"

CglB15::CglB15(const u8 *_buf,const int _size)
{
  CglStream stream(_buf,_size);
  
  Width=stream.Readu16();
  Height=stream.Readu16();
  
  if(stream.Readu16()==0){
    TransFlag=false;
    }else{
    TransFlag=true;
  }
  
  data=(u16*)glsafemalloc(Height*Width*sizeof(u16));
  
  stream.ReadBuffer(data,Height*Width*sizeof(u16));
  
  pCanvas=new CglCanvas(&data[0],Width,Height,pf15bit);
}

CglB15::~CglB15(void)
{
  delete pCanvas; pCanvas=NULL;
  glsafefree(data); data=NULL;
}

int CglB15::GetWidth(void) const
{
  return(Width);
}

int CglB15::GetHeight(void) const
{
  return(Height);
}

void CglB15::BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const
{
  pCanvas->BitBlt(pDestCanvas,nDestLeft,nDestTop,nWidth,nHeight,nSrcLeft,nSrcTop,TransFlag);
}

