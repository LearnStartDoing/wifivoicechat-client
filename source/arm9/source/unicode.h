
#ifndef unicode_h
#define unicode_h

#include "glib/cglunicode.h"

typedef u16 UnicodeChar;

extern bool Unicode_isEqual(const UnicodeChar *s1,const UnicodeChar *s2);
extern void Unicode_Add(UnicodeChar *s1,const UnicodeChar *s2);
extern u32 Unicode_GetLength(const UnicodeChar *s);

extern void StrConvert_Ank2Unicode(const char *srcstr,UnicodeChar *dststr);
extern void StrConvert_UTF82Unicode(const char *srcstr,UnicodeChar *dststr);

extern void StrConvert_SetUnicode(CglUnicode *_pglUnicode);
extern void StrConvert_Local2Unicode(const char *srcstr,UnicodeChar *dststr);

#endif
