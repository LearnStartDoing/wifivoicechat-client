
#ifndef shell_h
#define shell_h

extern void Shell_ReadFile(const char *fn,void **pbuf,int *psize);
extern void Shell_ReadSkinFile(const char *fn,void **pbuf,int *psize);
extern bool Shell_ReadSEFile(const char *fn,void **pbuf,int *psize);

#endif

