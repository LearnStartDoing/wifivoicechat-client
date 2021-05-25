
#ifndef __Cons_h
#define __Cons_h

#include "unicode.h"

#include "_g721.h"

#define PenPacketHeader (0x5a)
#define PenPacketHeader32 (0x5a5a5a5a)

enum EConStatus {ConStatus_Null=0,ConStatus_Login=1,ConStatus_Execute=2};

enum EConUserLng {ConUserLng_JPN=0,ConUserLng_ENG=1,ConUserLng_FRE=2,ConUserLng_DEU=3,ConUserLng_ITA=4,ConUserLng_ESP=5};
#define TConUserLng_Max (6)

extern char* Get_ConUserLngStr(EConUserLng ConUserLng);

#define UserNameSize (10+1)
#define UserMsgSize (26+1)

extern g721 g721_state_enc,g721_state_dec;
extern u32 g721_dec_LastFrameIndex;

#define g721_recvbufsize (g721_BlockBytes*8)
extern u32 g721_recv_readpos,g721_recv_writepos;
extern u8 g721_recvbuf[g721_recvbufsize];

#define g721_send_count (g721_DivCount)
extern u32 g721_send_pos;
extern u8 g721_sendbuf[g721_BlockBytes];

#define RecvBufMax (128*1024)
extern u8 *pRecvBuf;
extern u32 RecvBufSize;
extern u32 RecvBufPos;

extern u32 NetworkLatency;

extern void Cons_Buf_Init(void);

extern void Cons_Buf_AddBuf(void *pbuf,u32 bufsize);
extern void Cons_Buf_DelBuf(u32 delsize);
extern u32 Cons_Buf_GetDataPos(void);
extern void Cons_Buf_SetPos(u32 pos);
extern u32 Cons_Buf_GetPos(void);
extern u32 Cons_Buf_GetRestSize(void);
extern void Cons_Buf_GetBuf(void *pdstbuf,u32 dstsize);
extern u32 Cons_Buf_GetBuf32(void);

#endif

