
#include <stdlib.h>
#include <string.h>
#include <NDS.h>

#include "glib.h"
#include "glmemtool.h"
#include "cglstream.h"

CglStream::CglStream(const u8 *_buf,const int _size)
{
  buf=_buf;
  size=_size;
  ofs=0;
}

CglStream::~CglStream(void)
{
}

int CglStream::GetOffset(void) const
{
  return(ofs);
}

void CglStream::SetOffset(int _ofs)
{
  ofs=_ofs;
  if(size<ofs) ofs=size;
}

int CglStream::GetSize(void) const
{
  return(size);
}

bool CglStream::eof(void) const
{
  if(ofs==size){
    return(true);
    }else{
    return(false);
  }
}

u8 CglStream::Readu8(void)
{
  if(eof()==true) return(0);
  
  return(buf[ofs++]);
}

u16 CglStream::Readu16(void)
{
  u16 data;
  
  data=(u16)Readu8();
  data=data | ((u16)Readu8() << 8);
  
  return(data);
}

u32 CglStream::Readu32(void)
{
  u32 data;
  
  data=(u32)Readu8();
  data=data | ((u32)Readu8() << 8);
  data=data | ((u32)Readu8() << 16);
  data=data | ((u32)Readu8() << 24);
  
  return(data);
}

void CglStream::ReadBuffer(void *_dstbuf,const int _readsize)
{
  if(eof()==true) return;
  
  int readsize;
  
  if((ofs+_readsize)<=size){
    readsize=_readsize;
    }else{
    readsize=size-ofs;
  }
  
  memmove((u8*)_dstbuf,&buf[ofs],readsize);
  
  ofs+=readsize;
}

