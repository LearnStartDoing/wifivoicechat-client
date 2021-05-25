
#ifndef cglunicode_h
#define cglunicode_h

#include <stdlib.h>
#include <NDS.h>

#include "cglcanvas.h"

#include "tglunicode.h"

class CglUnicode
{
  int CodePage;
  bool AnkTable[0x100];
  int Count;
  TglUnicode *Local2UnicodeTable;
  CglUnicode(const CglUnicode&);
  CglUnicode& operator=(const CglUnicode&);
public:
  CglUnicode(const u8 *_buf,const int _size);
  ~CglUnicode(void);
  bool isAnkChar(unsigned char test) const;
  TglUnicode GetUnicode(u16 LocalChar) const;
};

#endif

