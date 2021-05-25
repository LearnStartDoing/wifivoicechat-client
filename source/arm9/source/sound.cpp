
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"
#include "memtool.h"
#include "shell.h"

#include "sound.h"

static TransferSound staticTransferSound;

typedef struct {
  u32 Freq;
  u32 BufOffset;
  u32 BufCount;
  s16 *lbuf,*rbuf;
} TSound;

static TSound Sound;

typedef struct {
  u16 wFormatTag;
  u16 nChannels;
  u32 nSamplesPerSec;
  u32 nAvgBytesPerSec;
  u16 nBlockAlign;
  u16 wBitsPerSample;
  u16 cbSize;
} WAVEFORMATEX;

static u8 *pfilebuf;
static u32 filesize;
static u32 filepos;

static void file_setbuf(u8 *pbuf,u32 bufsize)
{
  pfilebuf=pbuf;
  filesize=bufsize;
}

static void file_free(void)
{
  pfilebuf=NULL;
  filesize=0;
}

static u32 file_getpos(void)
{
  return(filepos);
}

static void file_setpos(u32 pos)
{
  filepos=pos;
}

static u32 file_readbuf(void *pdst,u32 size)
{
  u8 *pdst8=(u8*)pdst;
  
  for(u32 idx=0;idx<size;idx++){
    pdst8[idx]=pfilebuf[filepos];
    filepos++;
  }
  
  return(size);
}

static void LoadWaveFile(TSound *pSnd,u8 *pbuf,u32 bufsize)
{
  file_setbuf(pbuf,bufsize);
  
  {
    WAVEFORMATEX wfex;
    
    file_setpos(0x14);
    file_readbuf(&wfex,sizeof(WAVEFORMATEX));
    
    if(wfex.wFormatTag!=0x0001){
      _consolePrintf("Illigal CompressFormat Error. wFormatTag=0x%x\n",wfex.wFormatTag);
      file_free();
      return;
    }
    
    if(wfex.nChannels!=2){
      _consolePrintf("Channels Error. nChannels=%d\n",wfex.nChannels);
      file_free();
      return;
    }
    
    if(wfex.wBitsPerSample!=16){
      _consolePrintf("Bits/Sample Error. wBitsPerSample=%d\n",wfex.wBitsPerSample);
      file_free();
      return;
    }
    
    pSnd->Freq=wfex.nSamplesPerSec;
  }
  
  // find "data"
  {
    file_setpos(0);
    
    u32 ofs=0;
    
    char *buf=(char*)safemalloc(256);
    
    u32 size=file_readbuf(buf,256);
    if(size<4){
      _consolePrintf("can not find data chunk.\n");
      ofs=0;
      }else{
      while(1){
        if(buf[ofs]=='d'){
          if((buf[ofs+1]=='a')&&(buf[ofs+2]=='t')&&(buf[ofs+3]=='a')) break;
        }
        ofs++;
        if(ofs==(size-4)){
          _consolePrintf("can not find data chunk.\n");
          file_free();
          return;
        }
      }
    }
    
    safefree(buf); buf=NULL;
    
    if(ofs==0){
      file_free();
      return;
    }
    
    file_setpos(ofs+4);
  }
  
  u32 BufCount;
  file_readbuf(&BufCount,4);
  
  pSnd->BufOffset=file_getpos();
  pSnd->BufCount=BufCount/2/2; // 16bit 2ch only.
  pSnd->lbuf=NULL;
  pSnd->rbuf=NULL;
  
  file_setpos(pSnd->BufOffset);
  
  s16 *preadbuf=(s16*)safemalloc(pSnd->BufCount*2*2);
  if(preadbuf==NULL){
    _consolePrintf("can not allocate read buffer. (%dbyte)\n",pSnd->BufCount*2*2);
    file_free();
    return;
  }
  file_readbuf(&preadbuf[0],pSnd->BufCount*2*2);
  
  pSnd->lbuf=(s16*)safemalloc(pSnd->BufCount*2);
  pSnd->rbuf=(s16*)safemalloc(pSnd->BufCount*2);
  if((pSnd->lbuf==NULL)||(pSnd->rbuf==NULL)){
    _consolePrintf("can not allocate sound buffer. (%dbyte)\n",pSnd->BufCount*2*2);
    if(pSnd->lbuf!=NULL){
      safefree(pSnd->lbuf); pSnd->lbuf=NULL;
    }
    if(pSnd->rbuf!=NULL){
      safefree(pSnd->rbuf); pSnd->rbuf=NULL;
    }
    safefree(preadbuf); preadbuf=NULL;
    file_free();
    return;
  }
  
  for(u32 idx=0;idx<pSnd->BufCount;idx++){
    pSnd->lbuf[idx]=preadbuf[idx*2+0];
    pSnd->rbuf[idx]=preadbuf[idx*2+1];
  }
  
  safefree(preadbuf); preadbuf=NULL;
  
  file_free();
  
  return;
}

static void playSoundBlock(TransferSound *snd)
{
  DC_FlushRange( snd, sizeof(TransferSound) );
  IPC->soundData = snd;
}

void Sound_Init(void)
{
  Sound.Freq=0;
  Sound.BufOffset=0;
  Sound.BufCount=0;
  Sound.lbuf=NULL;
  Sound.rbuf=NULL;
}

void Sound_Start(const char *wavfn)
{
  volatile TransferSound *ptsnd=&staticTransferSound;
  volatile TransferSoundData *ptsnddata=ptsnd->data;
  
  if(Sound.lbuf!=NULL){
    safefree(Sound.lbuf); Sound.lbuf=NULL;
  }
  if(Sound.rbuf!=NULL){
    safefree(Sound.rbuf); Sound.rbuf=NULL;
  }
  
  u8 *pbuf=NULL;
  u32 bufsize=0;
  
  if(Shell_ReadSEFile(wavfn,(void**)&pbuf,(int*)&bufsize)==false) return;
  LoadWaveFile(&Sound,pbuf,bufsize);
  safefree(pbuf); pbuf=NULL;
  
  ptsnddata[0].rate=Sound.Freq;
  ptsnddata[0].data=Sound.lbuf;
  ptsnddata[0].len=Sound.BufCount*2;
  ptsnddata[0].vol=128/4;
  ptsnddata[0].pan=0;
  ptsnddata[0].format=0;
  
  ptsnddata[1].rate=Sound.Freq;
  ptsnddata[1].data=Sound.rbuf;
  ptsnddata[1].len=Sound.BufCount*2;
  ptsnddata[1].vol=128/4;
  ptsnddata[1].pan=127;
  ptsnddata[1].format=0;
  
  ptsnd->count=2;
  
  playSoundBlock(&staticTransferSound);
}

