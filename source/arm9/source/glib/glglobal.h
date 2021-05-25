
#ifndef glglobal_h
#define glglobal_h

#include <stdio.h>
#include <NDS.h>

#include "cglscreen.h"

extern CglScreenMain *pScreenMain;
extern CglScreenMainOverlay *pScreenMainOverlay;
extern CglScreenSub *pScreenSub;

// request
// 192kb < VRAM_MAIN_BG
// 96kb < VRAM_MAIN_SPRITE
// 96kb < VRAM_SUB_SPRITE

extern void glDefaultMemorySetting(void);
extern void glDefaultClassCreate(void);
extern void glDefaultClassFree(void);

extern void glSetFuncDebugPrint(void (*_DebugPrint)(const char* s));

void glDebugPrintf(const char* format, ...);

#endif

