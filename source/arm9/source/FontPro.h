
#ifndef FontPro_h
#define FontPro_h

#include "unicode.h"

#define FontProHeight (12)

extern void FontPro_Init(u8 *pfon);
extern void FontPro_Free(void);

extern u32 FontPro_WriteChar(s32 x,s32 y,UnicodeChar ccode,u16 col);
extern u32 FontPro_WriteCharRect(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,UnicodeChar ccode,u16 col);

extern u32 FontPro_GetCharWidth(u32 ccode);

extern bool FontPro_Exists(u32 ccode);

#endif

