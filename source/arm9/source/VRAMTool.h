
#ifndef	VRAMTool_H
#define	VRAMTool_H

#include <NDS.h>

#include "_const.h"

#include "unicode.h"

extern void VRAM_SetPixel(s32 x,s32 y,u16 data);
extern void VRAM_ClearPixel(s32 x,s32 y);
extern u16 VRAM_GetPixel(s32 x,s32 y);
extern void VRAM_SetCharLine(s32 y,s32 ysize,u16 col);
extern void VRAM_SetCharLineRect(u16 *pBuf,s32 BufWidth,s32 x,s32 y,s32 w,s32 h,u16 col);

extern void VRAM_WriteString(s32 x,s32 y,const char *str,u16 col);
extern void VRAM_WriteStringRect(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col);
extern u32 VRAM_GetStringWidth(const char *str);

extern void VRAM_WriteStringRectL(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col);
extern void VRAM_WriteStringRectUTF8(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const char *str,u16 col);
extern u32 VRAM_GetStringWidthUTF8(const char *str);

extern void VRAM_WriteStringRectW(u16 *pBuf,s32 w,s32 wLimit,s32 x,s32 y,const UnicodeChar *str,u16 col);
extern u32 VRAM_GetStringWidthW(const UnicodeChar *str);

extern void VRAM_Clear(void);

#endif
