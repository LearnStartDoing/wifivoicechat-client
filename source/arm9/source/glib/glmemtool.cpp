
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"

#include "glCP15.h"

#define CACHE_LINE_SIZE (32)
#define prcdiv (0x40)

static void glDC_FlushRangeOverrun(void *v,u32 size)
{
  static void (*lp_DC_FlushRange)(void *base, u32 size)=DC_FlushRange;
  static void (*lp_DC_InvalidateRange)(void *base, u32 size)=DC_InvalidateRange;
  
  size&=~(CACHE_LINE_SIZE-1);
  size+=CACHE_LINE_SIZE;
  
  if(size==0) return;
  
  lp_DC_FlushRange(v,size);
  lp_DC_InvalidateRange(v,size);
}

void glMemCopy16CPU(void *src,void *dst,u32 len)
{
  if(len<2) return;
  
  u8 *_src=(u8*)src;
  u8 *_dst=(u8*)dst;
  
  while(prcdiv<=len){
    memcpy(_dst,_src,prcdiv);
    glCP15_DrainWriteBuffer();
    len-=prcdiv;
    _src+=prcdiv;
    _dst+=prcdiv;
  }
  
  if(2<=len){
    memcpy(_dst,_src,len);
    glCP15_DrainWriteBuffer();
  }
}

void glMemCopy32CPU(void *src,void *dst,u32 len)
{
  glMemCopy16CPU(src,dst,len);
}

void glMemSet16CPU(vu16 v,void *dst,u32 len)
{
  if(len<2) return;
  
  u16 *_dst=(u16*)dst;
  
  while(prcdiv<=len){
    for(u32 cnt=0;cnt<(prcdiv/2);cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
    len-=prcdiv;
    _dst+=prcdiv/2;
  }
  
  if(2<=len){
    for(u32 cnt=0;cnt<(len/2);cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
  }
}

void glMemSet32CPU(u32 v,void *dst,u32 len)
{
  if(len<4) return;
  
  u32 *_dst=(u32*)dst;
  
  while(prcdiv<=len){
    for(u32 cnt=0;cnt<(prcdiv/4);cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
    len-=prcdiv;
    _dst+=prcdiv/4;
  }
  
  if(4<=len){
    for(u32 cnt=0;cnt<(len/4);cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
  }
}

void glMemSet8CPU(u8 v,void *dst,u32 len)
{
  if(len<1) return;
  
  u8 *_dst=(u8*)dst;
  
  while(prcdiv<=len){
    for(u32 cnt=0;cnt<prcdiv;cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
    len-=prcdiv;
    _dst+=prcdiv;
  }
  
  if(1<=len){
    for(u32 cnt=0;cnt<len;cnt++){
      _dst[cnt]=v;
    }
    glCP15_DrainWriteBuffer();
  }
}

void glMemCopy32swi256bit(void *src,void *dst,u32 len)
{
  swiFastCopy(src,dst,COPY_MODE_COPY | (len/4));
}

extern void ShowLogHalt(void);

void glsafemalloc_halt(void)
{
  glDebugPrintf("Please start with the START button if you use M3.\n");
  ShowLogHalt();
}

void *glsafemalloc(int size)
{
//  return(malloc(size));
  
  if(size<=0) return(NULL);
  
  void *ptr;
  u32 adr;
  
  ptr=malloc(size+(64*1024)); // 64kbは必ず残す
  
  if(ptr==NULL){
    glDebugPrintf("malloc(%dbyte) fail NULL\n",size);
    return(NULL);
  }
  
  adr=(u32)ptr;
  
  if((adr&3)!=0){ // 4byteアライメントされていなかったら
    glDebugPrintf("malloc(%d) fail not 4bytealian\n",size);
    glsafemalloc_halt();
    return(NULL);
  }
  
  if(adr<0x02000000){ // 先頭アドレスがメインメモリより前だったら
    glDebugPrintf("malloc(%d) fail adr<MemArea\n",size);
    glsafemalloc_halt();
    return(NULL);
  }
  
  if((0x02000000+(4*1024*1024))<=(adr+size)){ // 終端アドレスがメインメモリより後だったら
    glDebugPrintf("malloc(%d) fail MemArea<adr+size\n",size);
    glsafemalloc_halt();
    return(NULL);
  }
  
  // 64kb余剰確保したから取り直す
  
  void *checkedptr=ptr;
  
  free(ptr);
  ptr=malloc(size);
//  ptr=realloc(checkedptr,size);
  
  adr=(u32)ptr;
  
/*
  if(checkedptr!=ptr){ // チェック済みの取得ポインタと違ったら
    glDebugPrintf("malloc(%d) fail reallocerror.%x!=%x\n",size,(u32)checkedptr,(u32)ptr);
    glsafemalloc_halt();
    return(NULL);
  }
*/
  
  if((adr&3)!=0){ // 4byteアライメントされていなかったら
    glDebugPrintf("malloc(%d) fail reallocerror.%x!=%x\n",size,(u32)checkedptr,(u32)ptr);
    glsafemalloc_halt();
    return(NULL);
  }
  
  if(adr<0x02000000){ // 先頭アドレスがメインメモリより前だったら
    glDebugPrintf("malloc(%d) fail reallocerror.%x!=%x\n",size,(u32)checkedptr,(u32)ptr);
    glsafemalloc_halt();
    return(NULL);
  }
  
  if((0x02000000+(4*1024*1024))<=(adr+size)){ // 終端アドレスがメインメモリより後だったら
    glDebugPrintf("malloc(%d) fail reallocerror.%x!=%x\n",size,(u32)checkedptr,(u32)ptr);
    glsafemalloc_halt();
    return(NULL);
  }
  
/*
  {
    u8 *pbuf=(u8*)ptr;
    for(s32 cnt=0;cnt<size;cnt++){
      pbuf[cnt]=0;
    }
  }
*/
  
  return(ptr);

}

void glsafefree(void *ptr)
{
  if(ptr==NULL){
    glDebugPrintf("safefree Request NullPointer.\n");
    return;
  }
  
  free(ptr);
}

