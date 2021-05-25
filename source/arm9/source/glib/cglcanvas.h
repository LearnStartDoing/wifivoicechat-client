
#ifndef cglcanvas_h
#define cglcanvas_h

#include <stdlib.h>
#include <NDS.h>

#include "tglunicode.h"

enum EPixelFormat {pf15bit};

class CglCanvas
{
  u16 *VRAMBuf;
  bool VRAMBufInsideAllocatedFlag;
  int Width;
  int Height;
  EPixelFormat PixelFormat;
  u16 **ScanLine;
  u16 Color;
  int LastX,LastY;
  void *pCglFont;
  void *pCglUnicode;
  CglCanvas(const CglCanvas&);
  CglCanvas& operator=(const CglCanvas&);
  bool isInsidePosition(const int x,const int y) const;
  void BitBltBeta(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const;
  void BitBltTrans(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop) const;
public:
  CglCanvas(u16 *_VRAMBuf,const int _Width,const int _Height,const EPixelFormat _PixelFormat);
  ~CglCanvas(void);
  u16* GetVRAMBuf(void) const;
  void SetVRAMBuf(u16 *_VRAMBuf,const int _Width,const int _Height,const EPixelFormat _PixelFormat);
  int GetWidth(void) const;
  int GetHeight(void) const;
  u16* GetScanLine(const int y) const;
  void SetPixel(const int x,const int y,const u16 rgb);
  void SetPixelHalf(const int x,const int y,const u16 rgb);
  void SetPixelAlpha(const int x,const int y,const u16 rgb,const int Alpha);
  void SetPixelAlphaAdd(const int x,const int y,const u16 rgb,const int Alpha);
  u16 GetPixel(const int x,const int y) const;
  void SetColor(const u16 _Color);
  void DrawLine(const int x1,const int y1,const int x2,const int y2);
  void DrawTickLine(const int x1,const int y1,const int x2,const int y2);
  void MoveTo(const int x,const int y);
  void LineTo(const int x,const int y);
  void FillBox(const int x,const int y,const int w,const int h);
  void DrawBox(const int x,const int y,const int w,const int h);
  void SetFontBGColor(const u16 Color);
  void SetFontTextColor(const u16 Color);
  void TextOutA(const int x,const int y,const char *str) const;
  void TextOutL(const int x,const int y,const char *str) const;
  void TextOutW(const int x,const int y,const TglUnicode *str) const;
  int GetTextWidthA(const char *str) const;
  int GetTextWidthL(const char *str) const;
  int GetTextWidthW(const TglUnicode *str) const;
  void SetCglFont(void *_pCglFont);
  void SetCglUnicode(void *_pCglUnicode);
  void BitBlt(CglCanvas *pDestCanvas,const int nDestLeft,const int nDestTop,const int nWidth,const int nHeight,const int nSrcLeft,const int nSrcTop,const bool TransFlag) const;
};

#endif

