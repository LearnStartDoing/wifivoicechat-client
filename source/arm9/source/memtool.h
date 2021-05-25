
#ifndef memtool_h
#define memtool_h

#ifdef __cplusplus
extern "C" {
#endif

void DC_FlushRangeOverrun(const void *v,u32 size);

void MemCopy8CPU(const void *src,void *dst,u32 len);
void MemCopy16CPU(const void *src,void *dst,u32 len);
void MemCopy32CPU(const void *src,void *dst,u32 len);
void MemSet16CPU(vu16 v,void *dst,u32 len);
void MemSet32CPU(u32 v,void *dst,u32 len);
void MemCopy16DMA3(const void *src,void *dst,u32 len);
void MemCopy32DMA3(const void *src,void *dst,u32 len);
void MemSet16DMA3(vu16 v,void *dst,u32 len);
void MemSet32DMA3(u32 v,void *dst,u32 len);
void MemSet8DMA3(u8 v,void *dst,u32 len);

void MemCopy16DMA2(void *src,void *dst,u32 len);
void MemSet16DMA2(u16 v,void *dst,u32 len);

void MemCopy32swi256bit(void *src,void *dst,u32 len);

void *safemalloc(int size);
void safefree(void *ptr);
bool testmalloc(int size);
void PrintFreeMem(void);

#ifdef __cplusplus
}
#endif

#endif

