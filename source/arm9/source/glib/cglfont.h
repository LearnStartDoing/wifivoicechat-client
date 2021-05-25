
#ifndef cglfont_h
#define cglfont_h

#include <stdlib.h>
#include <NDS.h>

#include "cglcanvas.h"

#include "tglunicode.h"

enum EFontBPP {FontBPP0bit,FontBPP1bit,FontBPP2bit};

class CglFont
{
  int Height;
  EFontBPP BPP;
  int Count;
  u8 **DataTable;
  u8 *Data;
  u16 BGColor,TextColor;
  int WidthPadding,HeightPadding;
  bool Transparent;
  bool AlphaBlend;
  CglFont(const CglFont&);
  CglFont& operator=(const CglFont&);
  u8* GetBulkData(const TglUnicode uidx) const;
  void DrawFont1bpp(CglCanvas *pCanvas,const int x,const int y,const u8 *BulkData) const;
  void DrawFont2bpp(CglCanvas *pCanvas,const int x,const int y,const u8 *BulkData) const;
public:
  CglFont(const u8 *_buf,const int _size);
  ~CglFont(void);
  void DrawFont(CglCanvas *pCanvas,const int x,const int y,const TglUnicode uidx) const;
  int GetFontWidth(const TglUnicode uidx) const;
  int GetFontHeight(void) const;
  void SetBGColor(const u16 Color);
  void SetTextColor(const u16 Color);
  void SetPadding(const int Width,const int Height);
  void SetTransparent(const bool _Transparent);
  void SetAlphaBlend(const bool _AlphaBlend);
};

#endif

