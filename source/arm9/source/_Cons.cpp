
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "arm9tcm.h"

#include "_Cons.h"

g721 g721_state_enc,g721_state_dec;
u32 g721_dec_LastFrameIndex;

u32 g721_recv_readpos,g721_recv_writepos;
u8 g721_recvbuf[g721_recvbufsize];

u32 g721_send_pos;
u8 g721_sendbuf[g721_BlockBytes];

u32 NetworkLatency;

char* Get_ConUserLngStr(EConUserLng ConUserLng)
{
  char *pstr;
  switch(ConUserLng){
    case ConUserLng_JPN: pstr="JPN"; break;
    case ConUserLng_ENG: pstr="ENG"; break;
    case ConUserLng_FRE: pstr="FRE"; break;
    case ConUserLng_DEU: pstr="DEU"; break;
    case ConUserLng_ITA: pstr="ITA"; break;
    case ConUserLng_ESP: pstr="ESP"; break;
    default: pstr="???"; break;
  }
  return(pstr);
}

u8 *pRecvBuf;
u32 RecvBufSize;
u32 RecvBufPos;

void Cons_Buf_Init(void)
{
  if(pRecvBuf!=NULL){
    safefree(pRecvBuf); pRecvBuf=NULL;
  }
  
  pRecvBuf=(u8*)safemalloc(RecvBufMax);
  RecvBufSize=0;
  RecvBufPos=0;
  
  static DATA_IN_DTCM struct g721_state stenc,stdec;
  g721_state_enc=&stenc;
  g721_Renew(g721_state_enc);
  g721_state_dec=&stdec;
  g721_Renew(g721_state_dec);
  g721_dec_LastFrameIndex=0;
  
  g721_recv_readpos=0;
  g721_recv_writepos=0;
  MemSet16CPU(0,g721_recvbuf,g721_recvbufsize);
  
  g721_send_pos=0;
  MemSet16CPU(0,g721_sendbuf,g721_BlockBytes);
  
  NetworkLatency=0;
}

void Cons_Buf_AddBuf(void *pbuf,u32 bufsize)
{
  MemCopy8CPU(pbuf,&pRecvBuf[RecvBufSize],bufsize);
  RecvBufSize+=bufsize;
}

void Cons_Buf_DelBuf(u32 delsize)
{
  if((RecvBufSize-delsize)==0){
    RecvBufSize=0;
    RecvBufPos=0;
  }
}

u32 Cons_Buf_GetDataPos(void)
{
  u32 headnum=0;
  
  for(u32 idx=RecvBufPos;idx<RecvBufSize;idx++){
    if(pRecvBuf[idx]!=PenPacketHeader){
      headnum=0;
      }else{
      headnum++;
      if(headnum==4) return(idx+1);
    }
  }
  
  return((u32)-1);
}

void Cons_Buf_SetPos(u32 pos)
{
  RecvBufPos=pos;
}

u32 Cons_Buf_GetPos(void)
{
  return(RecvBufPos);
}

u32 Cons_Buf_GetRestSize(void)
{
  return(RecvBufSize-RecvBufPos);
}

void Cons_Buf_GetBuf(void *pdstbuf,u32 dstsize)
{
  MemCopy8CPU(&pRecvBuf[RecvBufPos],pdstbuf,dstsize);
  RecvBufPos+=dstsize;
}

u32 Cons_Buf_GetBuf32(void)
{
  u32 i;
  Cons_Buf_GetBuf(&i,4);
  return(i);
}

