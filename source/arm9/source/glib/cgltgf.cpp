
#include <stdlib.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cgltgf.h"

#include "cglstream.h"

CglTGF::CglTGF(const u8 *_buf,const int _size)
{
  CglStream stream(_buf,_size);
  
  Width=stream.Readu16();
  Height=stream.Readu16();
  
  int size;
  
  size=stream.GetSize()-stream.GetOffset();
  data=(u16*)glsafemalloc(size);
  
  stream.ReadBuffer(data,size);
}

CglTGF::~CglTGF(void)
{
  glsafefree(data); data=NULL;
}

int CglTGF::GetWidth(void) const
{
  return(Width);
}

int CglTGF::GetHeight(void) const
{
  return(Height);
}

void CglTGF::BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop) const
{
  u16 *pdata=data;
  
  for(int y=0;y<Height;y++){
    int x=0;
    while(x<Width){
      int alpha,len;
      {
        u16 curdata=*pdata++;
        alpha=curdata & 0xff;
        len=curdata >> 8;
      }
      if(alpha==31){
        x+=len;
        }else{
        if(alpha==0){
          for(int cnt=0;cnt<len;cnt++){
            pDestCanvas->SetPixel(nDestLeft+x,nDestTop+y,*pdata++);
            x++;
          }
          }else{
          alpha=31-alpha;
          for(int cnt=0;cnt<len;cnt++){
            pDestCanvas->SetPixelAlphaAdd(nDestLeft+x,nDestTop+y,*pdata++,alpha);
            x++;
          }
        }
        
      }
    }
  }
}
