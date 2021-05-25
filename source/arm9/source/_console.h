
#ifndef _console_h
#define _console_h

#ifdef __cplusplus

extern void _consoleInit(u16* font, u16* charBase, u16 numCharacters, u8 charStart, u16* map, u8 pal, u8 bitDepth);
extern void _consoleInitDefault(u16* map, u16* charBase);

extern void _consolePrint(const char* s);
extern void _consolePrintf(const char* format, ...);

extern void _consolePrintSet(int x, int y);
extern int _consoleGetPrintSetY(void);

extern void _consolePrintChar(char c);

extern void _consoleClear(void);

#endif

#ifdef __cplusplus
extern "C" {
#endif

void _consolePrintOne(char *str,u32 v);

#ifdef __cplusplus
}
#endif

#endif
