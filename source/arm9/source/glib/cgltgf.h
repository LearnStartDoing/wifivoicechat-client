
#ifndef CglTGF_h
#define CglTGF_h

#include <stdlib.h>
#include <NDS.h>

#include "cglcanvas.h"

class CglTGF
{
  int Width,Height;
  u16 *data;
  CglTGF(const CglTGF&);
  CglTGF& operator=(const CglTGF&);
  void BitBltBeta(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const;
  void BitBltTrans(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const;
public:
  CglTGF(const u8 *_buf,const int _size);
  ~CglTGF(void);
  int GetWidth(void) const;
  int GetHeight(void) const;
  void BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop) const;
};

#endif

