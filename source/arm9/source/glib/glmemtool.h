
#ifndef glmemtool_h
#define glmemtool_h

void glMemCopy16CPU(void *src,void *dst,u32 len);
void glMemCopy32CPU(void *src,void *dst,u32 len);
void glMemSet16CPU(vu16 v,void *dst,u32 len);
void glMemSet32CPU(u32 v,void *dst,u32 len);

void glMemCopy32swi256bit(void *src,void *dst,u32 len);

void *glsafemalloc(int size);
void glsafefree(void *ptr);
bool gltestmalloc(int size);

#endif

