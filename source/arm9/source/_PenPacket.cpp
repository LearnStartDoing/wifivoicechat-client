
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dswifi9.h>

#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "_console.h"
#include "_consoleWriteLog.h"

#include "_PenPacket.h"

#include "glib/glib.h"

#include "_g721.h"

EConUserLng UserLng;
UnicodeChar UserName[UserNameSize];
UnicodeChar UserMsg[UserMsgSize];
u64 UserMacAddr;

void PenPacket_Init(void)
{
  u16 *pbuf=pScreenMain->pCanvas->GetVRAMBuf();
  
  for(u32 y=0;y<DSScrHeight;y++){
    MemSet16DMA3(RGB15(0,0,0)|BIT15,pbuf,DSScrWidth*2);
    pbuf+=DSScrWidth;
  }
  
  UserLng=(EConUserLng)PersonalData->language;
  
  {
    MemSet16CPU(0,&UserName[0],UserNameSize*2);
    u32 len=PersonalData->nameLen;
    if(UserNameSize<len) len=UserNameSize;
    for(u32 idx=0;idx<len;idx++){
      UserName[idx]=PersonalData->name[idx];
    }
  }
  
  {
    MemSet16CPU(0,&UserMsg[0],UserMsgSize*2);
    u32 len=PersonalData->messageLen;
    if(UserMsgSize<len) len=UserMsgSize;
    for(u32 idx=0;idx<len;idx++){
      UserMsg[idx]=PersonalData->message[idx];
    }
  }
  
//  UserMacAddr=0;
}

extern void SendPacketToServer_AddPacket(const EPenPacketType PenPacketType,const void *packet,const u32 packetsize);

void PenPacket_GetDSScr_Send(u16 y)
{
  TPenPacket_GetDSScr packet;
  
  packet.y=y;
  SendPacketToServer_AddPacket(PenPacketType_GetDSScr,&packet,TPenPacket_GetDSScr_Size);
}

void PenPacket_SetStatus_Send(EConStatus ConStatus)
{
  TPenPacket_SetStatus packet;
  
  packet.ProtcolVersion=ThisProtcolVersion;
  packet.iStatus=(u16)ConStatus;
  packet.iUserLng=(u16)UserLng;
  MemCopy16CPU(UserName,packet.UserName,UserNameSize*2);
  MemCopy16CPU(UserMsg,packet.UserMsg,UserMsgSize*2);
  MemCopy16CPU(&UserMacAddr,&packet.UserMacAddr,8);
  
#ifdef TESTCODE
  packet.UserName[0]='D';
  packet.UserName[1]='S';
  packet.UserName[2]='L';
  packet.UserName[3]='B';
  packet.UserName[4]='l';
  packet.UserName[5]='u';
  packet.UserName[6]='e';
  packet.UserName[7]=0;
  packet.UserMsg[0]=0;
  packet.UserMacAddr=rand()&0xffffffff;
  packet.UserMacAddr=(packet.UserMacAddr<<16) | 0x1600;
#endif
  
  SendPacketToServer_AddPacket(PenPacketType_SetStatus,&packet,TPenPacket_SetStatus_Size);
}

void PenPacket_SetDSScr_Proc(TPenPacket_SetDSScr *packet)
{
  u16 *psrcbuf=packet->data;
  u16 *pdstbuf=pScreenMain->pCanvas->GetVRAMBuf();
  
  pdstbuf+=DSScrWidth*packet->y;
  
  MemCopy16DMA3(psrcbuf,pdstbuf,DSScrWidth*DSScrBlockSize*2);
}

void PenPacket_UserList_Proc(TPenPacket_UserList *packet)
{
}

void PenPacket_SetLatency_Proc(TPenPacket_SetLatency *packet)
{
  NetworkLatency=packet->Latency;
}

void PenPacket_SetLatency_Send(u32 Latency)
{
  static TPenPacket_SetLatency packet;
  packet.Latency=Latency;
  SendPacketToServer_AddPacket(PenPacketType_SetLatency,&packet,TPenPacket_SetLatency_Size);
}

void PenPacket_Line_Proc(TPenPacket_Line *packet)
{
  u16 col=packet->Color;
  u16 x1=packet->lastx;
  u16 y1=packet->lasty;
  u16 x2=packet->curx;
  u16 y2=packet->cury;
  
  CglCanvas *pCanvas=pScreenMain->pCanvas;
  
  pCanvas->SetColor(col | BIT15);
  pCanvas->DrawLine(x1,y1,x2,y2);
}

void PenPacket_TickLine_Proc(TPenPacket_TickLine *packet)
{
  u16 col=packet->Color;
  u16 x1=packet->lastx;
  u16 y1=packet->lasty;
  u16 x2=packet->curx;
  u16 y2=packet->cury;
  
  CglCanvas *pCanvas=pScreenMain->pCanvas;
  
  pCanvas->SetColor(col | BIT15);
  pCanvas->DrawTickLine(x1,y1,x2,y2);
}

void PenPacket_DrawBox_Proc(TPenPacket_DrawBox *packet)
{
  u16 col=packet->Color;
  u16 x1=packet->lastx;
  u16 y1=packet->lasty;
  u16 x2=packet->curx;
  u16 y2=packet->cury;
  
  if(x2<x1){
    u16 tmp=x1;
    x1=x2;
    x2=tmp;
  }
  x2=x2-x1;
  
  if(y2<y1){
    u16 tmp=y1;
    y1=y2;
    y2=tmp;
  }
  y2=y2-y1;
  
  CglCanvas *pCanvas=pScreenMain->pCanvas;
  
  pCanvas->SetColor(col | BIT15);
  pCanvas->DrawBox(x1,y1,x2,y2);
}

void PenPacket_FillBox_Proc(TPenPacket_FillBox *packet)
{
  u16 col=packet->Color;
  u16 x1=packet->lastx;
  u16 y1=packet->lasty;
  u16 x2=packet->curx;
  u16 y2=packet->cury;
  
  if(x2<x1){
    u16 tmp=x1;
    x1=x2;
    x2=tmp;
  }
  x2=x2-x1;
  
  if(y2<y1){
    u16 tmp=y1;
    y1=y2;
    y2=tmp;
  }
  y2=y2-y1;
  
  CglCanvas *pCanvas=pScreenMain->pCanvas;
  
  pCanvas->SetColor(col | BIT15);
  pCanvas->FillBox(x1,y1,x2,y2);
}

void PenPacket_Erase_Proc(TPenPacket_Erase *packet)
{
  u16 x=packet->curx;
  u16 y=packet->cury;
  
  CglCanvas *pCanvas=pScreenMain->pCanvas;
  
  pCanvas->SetColor(RGB15(0,0,0) | BIT15);
  pCanvas->FillBox(x-4,y-4,8,8);
}

void PenPacket_Sendg721_Proc(TPenPacket_Sendg721 *packet)
{
  if(packet->FrameIdx==0){
    g721_Renew(g721_state_enc);
    }else{
    if(g721_dec_LastFrameIndex==packet->FrameIdx) return;
  }
  
  if((g721_dec_LastFrameIndex+1)<packet->FrameIdx) g721_Renew(g721_state_dec);
  g721_dec_LastFrameIndex=packet->FrameIdx;
  
  MemCopy8CPU(packet->BlockData,&g721_recvbuf[g721_recv_writepos],g721_BlockBytes);
  g721_recv_writepos+=g721_BlockBytes;
  if(g721_recv_writepos==g721_recvbufsize) g721_recv_writepos=0;
  
  if(g721_recv_writepos==g721_recv_readpos) _consolePrintf("fullbuf.");
}

void PenPacket_Sendg721_Send(u8 *pByteData)
{
  TPenPacket_Sendg721 packet;
  
  static u32 g721_enc_LastFrameIndex=0;
  packet.FrameIdx=g721_enc_LastFrameIndex;
  g721_enc_LastFrameIndex++;
  
  if(packet.FrameIdx==0) g721_Renew(g721_state_enc);
//  g721_BlockEncode(g721_state_enc,pSamples,packet.BlockData);
  MemCopy8CPU(pByteData,packet.BlockData,g721_BlockBytes);
  
  SendPacketToServer_AddPacket(PenPacketType_Sendg721,&packet,TPenPacket_Sendg721_Size);
}

void PenPacket_ChangeRoom_Proc(TPenPacket_ChangeRoom *packet)
{
  extern u32 RoomNum_CurrentNum;
  
  RoomNum_CurrentNum=packet->RoomNum;
}

void PenPacket_ChangeRoom_Send(u16 RoomNum)
{
  TPenPacket_ChangeRoom packet;
  
  packet.RoomNum=RoomNum;
  
  SendPacketToServer_AddPacket(PenPacketType_ChangeRoom,&packet,TPenPacket_ChangeRoom_Size);
}

