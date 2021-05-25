
#ifndef __PenPacket_h
#define __PenPacket_h

#include "unicode.h"

#include "_Cons.h"
#include "_g721.h"

#define DSScrWidth (512)
#define DSScrHeight (256)

#define DSScrBlockSize (4)

#define ThisProtcolVersion (11)

extern EConUserLng UserLng;
extern UnicodeChar UserName[UserNameSize];
extern UnicodeChar UserMsg[UserMsgSize];
extern u64 UserMacAddr;

extern u16 DSScrBuf[DSScrHeight][DSScrWidth];

enum EPenPacketType {PenPacketType_SetStatus=0,PenPacketType_GetDSScr=1,PenPacketType_SetDSScr=2,PenPacketType_UserList=3,PenPacketType_SetLatency=4,
                     PenPacketType_Line=10,PenPacketType_TickLine=11,PenPacketType_DrawBox=12,PenPacketType_FillBox=13,PenPacketType_Erase=14,
                     PenPacketType_Sendg721=20,
                     PenPacketType_ChangeRoom=30};

void PenPacket_Init(void);

typedef struct {
  u16 ProtcolVersion;
  u16 iStatus;
  u16 iUserLng;
  UnicodeChar UserName[UserNameSize];
  UnicodeChar UserMsg[UserMsgSize];
  u16 dummy0,dummy1,dummy2;
  u64 UserMacAddr;
} TPenPacket_SetStatus;
#define TPenPacket_SetStatus_Size sizeof(TPenPacket_SetStatus)
extern void PenPacket_SetStatus_Send(EConStatus ConStatus);

typedef struct {
  u16 y;
} TPenPacket_GetDSScr;
#define TPenPacket_GetDSScr_Size sizeof(TPenPacket_GetDSScr)
extern void PenPacket_GetDSScr_Send(u16 y);

typedef struct {
  u16 y;
  u16 data[DSScrWidth*DSScrBlockSize];
} TPenPacket_SetDSScr;
#define TPenPacket_SetDSScr_Size sizeof(TPenPacket_SetDSScr)
extern void PenPacket_SetDSScr_Proc(TPenPacket_SetDSScr *packet);

#define TPenPacket_UserList_UserMax (64)
typedef struct {
  u16 iUserLng;
  u16 dummy;
  UnicodeChar UserName[UserNameSize];
  UnicodeChar UserMsg[UserMsgSize];
  u16 UserOnTime;
  u16 dummy0,dummy1,dummy2;
  u64 UserMacAddr;
} TPenPacket_UserList_User;

typedef struct {
  u16 UserCount;
  u16 dummy0,dummy1,dummy2;
  TPenPacket_UserList_User Users[TPenPacket_UserList_UserMax];
} TPenPacket_UserList;
#define TPenPacket_UserList_Size sizeof(TPenPacket_UserList)
extern void PenPacket_UserList_Proc(TPenPacket_UserList *packet);

typedef struct {
  u32 Latency;
} TPenPacket_SetLatency;
#define TPenPacket_SetLatency_Size sizeof(TPenPacket_SetLatency)
extern void PenPacket_SetLatency_Proc(TPenPacket_SetLatency *packet);
extern void PenPacket_SetLatency_Send(u32 Latency);

typedef struct {
  u16 Color;
  u16 lastx,lasty,curx,cury;
} TPenPacket_Line;
#define TPenPacket_Line_Size sizeof(TPenPacket_Line)
extern void PenPacket_Line_Proc(TPenPacket_Line *packet);

typedef struct {
  u16 Color;
  u16 lastx,lasty,curx,cury;
} TPenPacket_TickLine;
#define TPenPacket_TickLine_Size sizeof(TPenPacket_TickLine)
extern void PenPacket_TickLine_Proc(TPenPacket_TickLine *packet);

typedef struct {
  u16 Color;
  u16 lastx,lasty,curx,cury;
} TPenPacket_DrawBox;
#define TPenPacket_DrawBox_Size sizeof(TPenPacket_DrawBox)
extern void PenPacket_DrawBox_Proc(TPenPacket_DrawBox *packet);

typedef struct {
  u16 Color;
  u16 lastx,lasty,curx,cury;
} TPenPacket_FillBox;
#define TPenPacket_FillBox_Size sizeof(TPenPacket_FillBox)
extern void PenPacket_FillBox_Proc(TPenPacket_FillBox *packet);

typedef struct {
  u16 curx,cury;
} TPenPacket_Erase;
#define TPenPacket_Erase_Size sizeof(TPenPacket_Erase)
extern void PenPacket_Erase_Proc(TPenPacket_Erase *packet);

typedef struct {
  u32 FrameIdx;
  u8 BlockData[g721_BlockBytes];
} TPenPacket_Sendg721;
#define TPenPacket_Sendg721_Size sizeof(TPenPacket_Sendg721)
extern void PenPacket_Sendg721_Proc(TPenPacket_Sendg721 *packet);
extern void PenPacket_Sendg721_Send(u8 *pByteData);

typedef struct {
  u32 dummy;
} TPenPacket_Renewg721;
#define TPenPacket_Renewg721_Size sizeof(TPenPacket_Renewg721)
extern void PenPacket_Renewg721_Proc(TPenPacket_Renewg721 *packet);
extern void PenPacket_Renewg721_Send(void);

typedef struct {
  u16 RoomNum;
} TPenPacket_ChangeRoom;
#define TPenPacket_ChangeRoom_Size sizeof(TPenPacket_ChangeRoom)
extern void PenPacket_ChangeRoom_Proc(TPenPacket_ChangeRoom *packet);
extern void PenPacket_ChangeRoom_Send(u16 RoomNum);

#endif
