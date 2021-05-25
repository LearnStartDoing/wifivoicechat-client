
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_const.h"
#include "_console.h"
#include "shell.h"

#include "memtool.h"

#include "cimfs.h"

void Shell_ReadFile(const char *fn,void **pbuf,int *psize)
{
  _consolePrintf("read from /shell/%s\n",fn);
  
  CIMFS *pFS=pIMFS;
  
  s32 PathIndex=pFS->GetPathIndex("/shell");
  s32 FileIndex=pFS->GetIndexFromFilename(PathIndex,fn);
  if(FileIndex==-1){
    FileIndex=pFS->GetIndexFromLongFilename(PathIndex,fn);
  }
  
  if((PathIndex!=-1)&&(FileIndex!=-1)){
    *psize=pFS->GetFileDataSizeFromIndex(PathIndex,FileIndex);
    *pbuf=(void*)safemalloc(*psize);
    
    if(pFS->GetFileTypeFromIndex(PathIndex,FileIndex)==FT_FileFlat){
      u8 *data=pFS->GetFileDirectDataFromIndex(PathIndex,FileIndex);
      if(data!=NULL){
        MemCopy8CPU(data,*pbuf,*psize);
        return;
      }
      }else{
      pFS->GetFileDataFromIndex(PathIndex,FileIndex,(u8*)*pbuf);
      return;
    }
  }
  
  *psize=0;
  *pbuf=NULL;
}

void Shell_ReadSkinFile(const char *fn,void **pbuf,int *psize)
{
  _consolePrintf("read from /shell/skin/%s\n",fn);
  
  CIMFS *pFS=pIMFS;
  
  s32 PathIndex=pFS->GetPathIndex("/shell/skin");
  s32 FileIndex=pFS->GetIndexFromFilename(PathIndex,fn);
  if(FileIndex==-1){
    FileIndex=pFS->GetIndexFromLongFilename(PathIndex,fn);
  }
  
  if((PathIndex!=-1)&&(FileIndex!=-1)){
    *psize=pFS->GetFileDataSizeFromIndex(PathIndex,FileIndex);
    *pbuf=(void*)safemalloc(*psize);
    
    if(pFS->GetFileTypeFromIndex(PathIndex,FileIndex)==FT_FileFlat){
      u8 *data=pFS->GetFileDirectDataFromIndex(PathIndex,FileIndex);
      if(data!=NULL){
        MemCopy8CPU(data,*pbuf,*psize);
        return;
      }
      }else{
      pFS->GetFileDataFromIndex(PathIndex,FileIndex,(u8*)*pbuf);
      return;
    }
  }
  
  *psize=0;
  *pbuf=NULL;
}

bool Shell_ReadSEFile(const char *fn,void **pbuf,int *psize)
{
//  _consolePrintf("read from /shell/se/%s\n",fn);
  
  CIMFS *pFS=pIMFS;
  
  s32 PathIndex=pFS->GetPathIndex("/shell/se");
  s32 FileIndex=pFS->GetIndexFromFilename(PathIndex,fn);
  if(FileIndex==-1){
    FileIndex=pFS->GetIndexFromLongFilename(PathIndex,fn);
  }
  
  if((PathIndex!=-1)&&(FileIndex!=-1)){
    *psize=pFS->GetFileDataSizeFromIndex(PathIndex,FileIndex);
    *pbuf=(void*)safemalloc(*psize);
    
    if(pFS->GetFileTypeFromIndex(PathIndex,FileIndex)==FT_FileFlat){
      u8 *data=pFS->GetFileDirectDataFromIndex(PathIndex,FileIndex);
      if(data!=NULL){
        MemCopy8CPU(data,*pbuf,*psize);
        return(true);
      }
      }else{
      pFS->GetFileDataFromIndex(PathIndex,FileIndex,(u8*)*pbuf);
      return(true);
    }
  }
  
  *psize=0;
  *pbuf=NULL;
  return(false);
}

