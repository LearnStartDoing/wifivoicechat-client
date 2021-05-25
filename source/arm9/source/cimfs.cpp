
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"

#include "cimfs.h"

CIMFS *pIMFS,*pEXFS;

#ifndef USEIMFS

CIMFS::CIMFS(void)
{
}

CIMFS::~CIMFS(void)
{
}

bool CIMFS::InitIMFS(void)
{
}

bool CIMFS::InitEXFS(void)
{
}

void CIMFS::Init(u8 *data)
{
}

u32 CIMFS::GetPathIndex(const char *str)
{
}

u32 CIMFS::GetFileTopIndex(u32 PathIndex)
{
}

u32 CIMFS::GetFileCount(u32 PathIndex)
{
}

char* CIMFS::GetPathName(u32 PathIndex)
{
}

static u32 GetStrCRC32Fast(const char *str)
{
}

u32 CIMFS::GetIndexFromFilename(u32 PathIndex,const char *fn)
{
}

u32 CIMFS::GetIndexFromLongFilename(u32 PathIndex,const char *lfn)
{
}

u32 CIMFS::GetFileTypeFromIndex(u32 PathIndex,u32 idx)
{
}

u32 CIMFS::GetFileTypeFromFilename(u32 PathIndex,const char *fn)
{
}

u32 CIMFS::GetFilenameSizeFromIndex(u32 PathIndex,u32 idx)
{
}

u32 CIMFS::GetLongFilenameSizeFromIndex(u32 PathIndex,u32 idx)
{
}

char* CIMFS::GetFilenameFromIndex(u32 PathIndex,u32 idx)
{
}

UnicodeChar* CIMFS::GetLongFilenameFromIndex(u32 PathIndex,u32 idx)
{
}

char* CIMFS::GetFilenameExtFromIndex(u32 PathIndex,u32 idx)
{
}

u32 CIMFS::GetFileDataSizeFromIndex(u32 PathIndex,u32 idx)
{
}

u32 CIMFS::GetFileDataSizeFromFilename(u32 PathIndex,const char *fn)
{
}

void CIMFS::GetFileDataFromIndex(u32 PathIndex,u32 idx,u8 *dstbuf)
{
}

void CIMFS::GetFileDataFromFilename(u32 PathIndex,const char *fn,u8 *dstbuf)
{
}

u8* CIMFS::GetFileDirectDataFromIndex(u32 PathIndex,u32 idx)
{
}

u8* CIMFS::GetFileDirectDataFromFilename(u32 PathIndex,const char *fn)
{
}

u32 CIMFS::GetTargetPathIndexFromIndex(u32 PathIndex,u32 idx)
{
}

u32 CIMFS::GetTargetPathIndexFromFilename(u32 PathIndex,const char *fn)
{
}

#else

#include "IMFSROM_bin.h"

#define IMFS_MagicID (0x53464d49)
#define IMFS_VersionID (0x00020002)

#define EXFS_MagicID (0x53465845)

#include "strtool.h"
#include "memtool.h"

CIMFS::CIMFS(void)
{
  Init(NULL);
}

CIMFS::~CIMFS(void)
{
  Init(NULL);
}

bool CIMFS::InitIMFS(void)
{
  Init(NULL);
  
  Init((u8*)&IMFSROM_bin[0]);
  return(true);
}

bool CIMFS::InitEXFS(void)
{
  {
#define GAME_PAK		0x08000000			// Game pack start address

// コピペ from mpcfio.cpp
	//	for NDS access to GBA cart
			vu16* GBA_ROM_TOP=	(vu16*)(GAME_PAK);
			vu16 dummy;
		//	WAIT_CR=0x0000;
		//	WAIT_CR=0x4000;
			*(vu16*)0x4000204=0x4000;
			dummy=GBA_ROM_TOP[0];
	//
#undef GAME_PAK
  }
  
  u32 MagicID=EXFS_MagicID;
  
  u32 *pAreaSearch;
  
  {
    extern u32 __appended_data;
    u32 adr;
    adr=(u32)&__appended_data-0x02000000;
    pAreaSearch=(u32*)((0x08000000+adr) & 0xffffff00);
  }
  
  while(1){
    if(pAreaSearch[1]==IMFS_VersionID){
      if(pAreaSearch[0]==MagicID){
        Init((u8*)pAreaSearch);
        return(true);
      }
    }
    
    pAreaSearch+=0x100/4;
    if(pAreaSearch==(u32*)(0x08000000+(1*1024*1024))) break; // limit 1MByte
  }
  
  return(false);
}

void CIMFS::Init(u8 *data)
{
  PathCount=0;
  PathInfo=NULL;
  PathData=NULL;
  
  FileCount=0;
  FileInfo=NULL;
  FileData=NULL;
  
  if(data==NULL) return;
  
  if( (((u32*)data)[0]!=IMFS_MagicID) && (((u32*)data)[0]!=EXFS_MagicID)){
    _consolePrintf("Error. IMFS_MagicID\n");
    ShowLogHalt();
  }
  
  if(((u32*)data)[1]!=IMFS_VersionID){
    _consolePrintf("Error. IMFS_VersionID\n");
    ShowLogHalt();
  }
  
  u32 PathBasePos,FileBasePos;
  
  PathCount=((u32*)data)[3];
  PathBasePos=((u32*)data)[4];
  
  FileCount=((u32*)data)[5];
  FileBasePos=((u32*)data)[6];
  
  PathInfo=(TPathInfo*)&data[PathBasePos];
  PathData=(u8*)&PathInfo[PathCount];
  
  FileInfo=(TFileInfo*)&data[FileBasePos];
  FileData=(u8*)&FileInfo[FileCount];
  
//  _consolePrintf("*PathInfo=0x%08X PathCount=%d\n", (u32)PathInfo,PathCount);
//  _consolePrintf("*FileInfo=0x%08X FileCount=%d\n", (u32)FileInfo,FileCount);
//  ShowLogHalt();
}

u32 CIMFS::GetPathIndex(const char *str)
{
  if(str==NULL) return((u32)-1);
  
  if(str[0]==0) return(0);
  
  for(u32 PathIndex=0;PathIndex<PathCount;PathIndex++){
    u8 *_PathData=&PathData[PathInfo[PathIndex].Offset];
    char *_PathName=(char*)&_PathData[8];
    
    if(isStrEqual(str,_PathName)==true){
      return(PathIndex);
    }
  }
  
  return((u32)-1);
}

u32 CIMFS::GetFileTopIndex(u32 PathIndex)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  
  u8 *_PathData=&PathData[PathInfo[PathIndex].Offset];
  return(*(u32*)&_PathData[0]);
}

u32 CIMFS::GetFileCount(u32 PathIndex)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  
  u8 *_PathData=&PathData[PathInfo[PathIndex].Offset];
  return(*(u32*)&_PathData[4]);
}

char* CIMFS::GetPathName(u32 PathIndex)
{
  if(PathIndex==(u32)-1) return(NULL);
  
  u8 *_PathData=&PathData[PathInfo[PathIndex].Offset];
  return((char*)&_PathData[8]);
}

static u32 GetStrCRC32Fast(const char *str)
{
  if(str==NULL) return((u32)-1);
  
  u32 CRC=0;
  while(*str!=0){
    CRC=CRC ^ (u32)*str;
    CRC=((CRC >> (32-8)) & 0x000000ff)+((CRC & 0x00ffffff) << 8);
    str++;
  }
  
  return(CRC);
}

u32 CIMFS::GetIndexFromFilename(u32 PathIndex,const char *fn)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(fn==NULL) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  u32 FileCount=GetFileCount(PathIndex);
  
  u32 idx;
  u32 SrcHash;
  
  SrcHash=GetStrCRC32Fast(fn);
  
  for(idx=FileTopIndex;idx<FileTopIndex+FileCount;idx++){
    if(SrcHash==FileInfo[idx].Hash){
      u8 *_FileData=&FileData[FileInfo[idx].Offset];
      char *FindStr=(char*)&_FileData[12];
      if(isStrEqual(fn,FindStr)==true){
        return(idx-FileTopIndex);
      }
    }
  }
  
  return((u32)-1);
}

u32 CIMFS::GetIndexFromLongFilename(u32 PathIndex,const char *lfn)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(lfn==NULL) return((u32)-1);
  
  UnicodeChar unilfn[256];
  
  StrConvert_UTF82Unicode(lfn,unilfn);
  
  u32 FileCount=GetFileCount(PathIndex);
  
  for(u32 idx=0;idx<FileCount;idx++){
    UnicodeChar *FindStr=GetLongFilenameFromIndex(PathIndex,idx);
    if(Unicode_isEqual(unilfn,FindStr)==true){
      return(idx);
    }
  }
  
  return((u32)-1);
}

u32 CIMFS::GetFileTypeFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(idx==(u32)-1) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return(*(u32*)&_FileData[0]);
}

u32 CIMFS::GetFileTypeFromFilename(u32 PathIndex,const char *fn)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(fn==NULL) return((u32)-1);
  
  u32 idx=GetIndexFromFilename(PathIndex,fn);
  return(GetFileTypeFromIndex(PathIndex,idx));
}

u32 CIMFS::GetFilenameSizeFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(idx==(u32)-1) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return(_FileData[8]);
}

u32 CIMFS::GetLongFilenameSizeFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(idx==(u32)-1) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return(_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)]);
}

char* CIMFS::GetFilenameFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return(NULL);
  if(idx==(u32)-1) return(NULL);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return((char*)&_FileData[12]);
}

UnicodeChar* CIMFS::GetLongFilenameFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return(NULL);
  if(idx==(u32)-1) return(NULL);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return((UnicodeChar*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4]);
}

char* CIMFS::GetFilenameExtFromIndex(u32 PathIndex,u32 idx)
{
  char *fn=GetFilenameFromIndex(PathIndex,idx);
  
  while(1){
    if(*fn=='.') return(fn);
    if(*fn==0) return(fn);
    fn++;
  }
}

u32 CIMFS::GetFileDataSizeFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(idx==(u32)-1) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  return(*(u32*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)]);
}

u32 CIMFS::GetFileDataSizeFromFilename(u32 PathIndex,const char *fn)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(fn==NULL) return((u32)-1);
  
  u32 idx=GetIndexFromFilename(PathIndex,fn);
  return(GetFileDataSizeFromIndex(PathIndex,idx));
}

#include "zlib/zlib.h"

// 返値:解凍済みデータサイズ(-1:error)
static inline int zlib_decompress(const u8 *compbuf,const u32 compsize,u8 *decompbuf,const u32 decompsize)
{
    z_stream z;                     /* ライブラリとやりとりするための構造体 */
    
    int status;

    /* すべてのメモリ管理をライブラリに任せる */
    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    /* 初期化 */
    if (inflateInit(&z) != Z_OK) {
        _consolePrintf("inflateInit: %s\n", (z.msg) ? z.msg : "???");
        return(-1);
    }

    z.next_in = (u8*)compbuf;
    z.avail_in = compsize;
    z.next_out = decompbuf;        /* 出力ポインタ */
    z.avail_out = decompsize;    /* 出力バッファのサイズ */

    while(1){
      status = inflate(&z, Z_NO_FLUSH); /* 展開 */
      if (status == Z_STREAM_END) break; /* 完了 */
      if (status != Z_OK) {   /* エラー */
        _consolePrintf("inflate: %s\n", (z.msg) ? z.msg : "???");
        return(-1);
      }
      if (z.avail_out == 0) { /* 出力バッファが尽きれば */
        _consolePrintf("deflate buffer overflow.");
        return(-1);
      }
    }
    
    u32 tmp;
    
    tmp=decompsize-z.avail_out;
    
    /* 後始末 */
    if (inflateEnd(&z) != Z_OK) {
        _consolePrintf("inflateEnd: %s\n", (z.msg) ? z.msg : "???");
        return(-1);
    }
    
    return(tmp);
}

void CIMFS::GetFileDataFromIndex(u32 PathIndex,u32 idx,u8 *dstbuf)
{
  if(dstbuf==NULL){
    _consolePrintf("GetFileDataFromIndex\nRequest malloc(size)\n");
    ShowLogHalt();
  }
  
  if(PathIndex==(u32)-1) return;
  if(idx==(u32)-1) return;
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  u32 FileType=*(u32*)&_FileData[0];
  u32 dstsize=*(u32*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)+0];
  u32 srcsize=*(u32*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)+4];
  u8 *srcbuf=(u8*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)+8];
  
  switch(FileType){
    case FT_Path:
      _consolePrintf("GetFileDataFromIndex\nRequest Path.\n");
      ShowLogHalt();
      break;
    case FT_FileFlat:
      for(u32 cnt=0;cnt<dstsize;cnt++){
        dstbuf[cnt]=srcbuf[cnt];
      }
      break;
    case FT_FileLZ77:
      swiDecompressLZSSWram((void*)srcbuf, (void*)dstbuf);
      break;
    case FT_FileLZSS:
      swiDecompressLZSSWram((void*)srcbuf, (void*)dstbuf);
      break;
    case FT_FileZLIB:
      zlib_decompress(srcbuf,srcsize,dstbuf,dstsize);
      break;
  }
  
}

void CIMFS::GetFileDataFromFilename(u32 PathIndex,const char *fn,u8 *dstbuf)
{
  if(PathIndex==(u32)-1) return;
  if(fn==NULL) return;
  
  u32 idx=GetIndexFromFilename(PathIndex,fn);
  GetFileDataFromIndex(PathIndex,idx,dstbuf);
}

u8* CIMFS::GetFileDirectDataFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return(NULL);
  if(idx==(u32)-1) return(NULL);
  if(GetFileTypeFromIndex(PathIndex,idx)!=FT_FileFlat){
    _consolePrintf("GetFileDirectData() for FT_FileFlat only.\n");
    ShowLogHalt();
    while(1);
    return(NULL);
  }
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  u8 *srcbuf=(u8*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)+8];
  
  return(srcbuf);
}

u8* CIMFS::GetFileDirectDataFromFilename(u32 PathIndex,const char *fn)
{
  if(PathIndex==(u32)-1) return(NULL);
  if(fn==NULL) return(NULL);
  
  u32 idx=GetIndexFromFilename(PathIndex,fn);
  return(GetFileDirectDataFromIndex(PathIndex,idx));
}

u32 CIMFS::GetTargetPathIndexFromIndex(u32 PathIndex,u32 idx)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(idx==(u32)-1) return((u32)-1);
  
  u32 FileTopIndex=GetFileTopIndex(PathIndex);
  
  u8 *_FileData=&FileData[FileInfo[FileTopIndex+idx].Offset];
  u32 FileType=*(u32*)&_FileData[0];
  
  if(FileType!=FT_Path){
    _consolePrintf("GetTargetPathIndexFromIndex\nRequest Path.\n");
    ShowLogHalt();
  }
  
  return(*(u32*)&_FileData[12+GetFilenameSizeFromIndex(PathIndex,idx)+4+GetLongFilenameSizeFromIndex(PathIndex,idx)+0]);
}

u32 CIMFS::GetTargetPathIndexFromFilename(u32 PathIndex,const char *fn)
{
  if(PathIndex==(u32)-1) return((u32)-1);
  if(fn==NULL) return((u32)-1);
  
  u32 idx=GetIndexFromFilename(PathIndex,fn);
  return(GetTargetPathIndexFromIndex(PathIndex,idx));
}

#endif

