
#ifndef cimfs_h
#define cimfs_h

#include "unicode.h"

#define IMFS_FilenameLengthMax (64)

#define IMFSTitle "ImplantFilesystem ver0.3"
#define EXFSTitle "ExtendFilesystem ver0.3"

#define FT_Path (0)
#define FT_FileFlat (1)
#define FT_FileLZ77 (2)
#define FT_FileLZSS (3)
#define FT_FileZLIB (4)

typedef struct {
  u32 Offset;
} TPathInfo;

typedef struct {
  u32 Hash;
  u32 Offset;
} TFileInfo;

class CIMFS
{
  u32 PathCount;
  TPathInfo *PathInfo;
  u8 *PathData;
  
  u32 FileCount;
  TFileInfo *FileInfo;
  u8 *FileData;
  
  void Init(u8 *data);
  
  CIMFS(const CIMFS&);
  CIMFS& operator=(const CIMFS&);
protected:
public:
  CIMFS(void);
  ~CIMFS(void);
  
  bool InitIMFS(void);
  bool InitEXFS(void);
  
  u32 GetPathIndex(const char *str);
  u32 GetFileTopIndex(u32 PathIndex);
  u32 GetFileCount(u32 PathIndex);
  
  char* GetPathName(u32 PathIndex);
  
  u32 GetIndexFromFilename(u32 PathIndex,const char *fn);
  u32 GetIndexFromLongFilename(u32 PathIndex,const char *lfn);
  u32 GetFileTypeFromIndex(u32 PathIndex,u32 idx);
  u32 GetFileTypeFromFilename(u32 PathIndex,const char *fn);
  char* GetFilenameFromIndex(u32 PathIndex,u32 idx);
  char* GetFilenameExtFromIndex(u32 PathIndex,u32 idx);
  UnicodeChar* GetLongFilenameFromIndex(u32 PathIndex,u32 idx);
  u32 GetFilenameSizeFromIndex(u32 PathIndex,u32 idx);
  u32 GetLongFilenameSizeFromIndex(u32 PathIndex,u32 idx);
  u32 GetFileDataSizeFromIndex(u32 PathIndex,u32 idx);
  u32 GetFileDataSizeFromFilename(u32 PathIndex,const char *fn);
  void GetFileDataFromIndex(u32 PathIndex,u32 idx,u8 *dstbuf);
  void GetFileDataFromFilename(u32 PathIndex,const char *fn,u8 *dstbuf);
  u8* GetFileDirectDataFromIndex(u32 PathIndex,u32 idx);
  u8* GetFileDirectDataFromFilename(u32 PathIndex,const char *fn);
  u32 GetTargetPathIndexFromIndex(u32 PathIndex,u32 idx);
  u32 GetTargetPathIndexFromFilename(u32 PathIndex,const char *fn);
};

extern CIMFS *pIMFS;
extern CIMFS *pEXFS;

#endif


