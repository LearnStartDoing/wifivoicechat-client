/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.4 2005/09/17 23:15:13 wntrmute Exp $

	Basic Hello World

	$Log: template.c,v $
	Revision 1.4  2005/09/17 23:15:13  wntrmute
	corrected iprintAt in templates
	
	Revision 1.3  2005/09/05 00:32:20  wntrmute
	removed references to IPC struct
	replaced with API functions
	
	Revision 1.2  2005/08/31 01:24:21  wntrmute
	updated for new stdio support

	Revision 1.1  2005/08/03 06:29:56  wntrmute
	added templates


---------------------------------------------------------------------------------*/
#include <nds.h>
#include "../../ipcex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "_consolewritelog.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"
#include "arm9tcm.h"

#include "glib/glib.h"

#include "_Cons.h"
#include "_PenPacket.h"

#include "mwin.h"
#include "mwin_color.h"
#include "FontPro.h"
#include "VRAMTool.h"
#include "zlibhelp.h"

#include "cimfs.h"
#include "inifile.h"
#include "shell.h"
#include "unicode.h"
#include "lang.h"

#include "sound.h"

#include "_g721.h"

#include "networksetting.h"

TNetworkSetting NetworkSetting={"NetworkSetting","mdxonlinemirror.dyndns.org",9011,2};

//---------------------------------------------------------------------------------

#include "main_wifi.h"

void ShowLogHalt(void)
{
  videoSetModeSub(MODE_2_2D | DISPLAY_BG2_ACTIVE);
  _consolePrintf("\n");
  while(1){
    swiWaitForVBlank();
  }
}

static DATA_IN_DTCM bool VBlankPassed;

static void InterruptHandler_VBlank(void)
{
  VBlankPassed=true;
  
#ifdef PRIVATE
  u16 KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
  
  if((KEYS_CUR & KEY_L)!=0){
    REG_IME = IME_DISABLE;  // Disable interrupts
    WAIT_CR |= (0x8080);  // ARM7 has access to GBA cart
    *((vu32*)0x027FFFFC) = 0;  // Start cluster of NDS to load
    *((vu32*)0x027FFE04) = (u32)0xE59FF018;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE04;  // Set ARM9 Loop address
    swiSoftReset();  // Reset
  }
  
  if((KEYS_CUR & KEY_R)!=0){
    REG_IME = IME_DISABLE;  // Disable interrupts
    *((vu32*)0x027FFE24) = (u32)0x027FFE05;  // power off
    while(1);
  }
#endif
}

#include "setarm9_reg_waitcr.h"

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

  REG_IME=0;
  
  POWER_CR = POWER_ALL_2D;
  POWER_CR &= ~POWER_SWAP_LCDS;
  
  irqInit();
  irqSet(IRQ_VBLANK, InterruptHandler_VBlank);
  irqEnable(IRQ_VBLANK);
  
  glSetFuncDebugPrint(_consolePrint);
  glDefaultMemorySetting();
  
  {
    SUB_BG2_CR = BG_256_COLOR | BG_RS_64x64 | BG_MAP_BASE(8) | BG_TILE_BASE(0) | BG_PRIORITY_0; // Tile16kb Map2kb(64x32)
    
    BG_PALETTE_SUB[(0*16)+0] = RGB15(0,0,0); // unuse (transparent)
    BG_PALETTE_SUB[(0*16)+1] = RGB15(0,0,8) | BIT(15); // BG color
    BG_PALETTE_SUB[(0*16)+2] = RGB15(0,0,0) | BIT(15); // Shadow color
    BG_PALETTE_SUB[(0*16)+3] = RGB15(31,31,31) | BIT(15); // Text color
    
    u16 XDX=(u16)((8.0/6)*0x100);
    u16 YDY=(u16)((8.0/6)*0x100);
    
    SUB_BG2_XDX = XDX;
    SUB_BG2_XDY = 0;
    SUB_BG2_YDX = 0;
    SUB_BG2_YDY = YDY;
    
    SUB_BG2_CX=-1;
    SUB_BG2_CY=-1;
    
    //consoleInit() is a lot more flexible but this gets you up and running quick
    _consoleInitDefault((u16*)(SCREEN_BASE_BLOCK_SUB(8)), (u16*)(CHAR_BASE_BLOCK_SUB(0)));
    _consoleClear();
  }
  
  glDefaultClassCreate();
  pScreenMain->SetWideFlag(true);
  
  _consolePrintf("boot %s %s\n%s\n%s\n%s\n%s\n\n",ROMTITLE,ROMVERSION,ROMDATE,ROMAUTHOR1,ROMAUTHOR2,ROMENV);
  
  SetARM9_REG_WaitCR();
  
  {
    void main_OpenSocket(void);
    void main_CloseSocket(void);
    void mainloop_init(void);
    void mainloop(void);
    Sound_Init();
    WifiConnected=false;
    main_wifi_Init();
#ifdef NotConnect
#else
    main_wifi_AutoConnect();
    main_OpenSocket();
    Wifi_GetData(WIFIGETDATA_MACADDRESS,6,(unsigned char *)&UserMacAddr);
#endif
    SetARM9_REG_WaitCR();
    mainloop_init();
    mainloop();
    main_CloseSocket();
  }
  
  main_wifi_Free();
  
  ShowLogHalt();
}

static DATA_IN_DTCM struct hostent * myhost;
static DATA_IN_DTCM struct sockaddr_in mysain;
static DATA_IN_DTCM int mysock=0;

void main_OpenSocket(void)
{
  _consolePrintf("Connect to %s:%d\n",NetworkSetting.ServerName,NetworkSetting.ServerPort);
  _consolePrintf("\n");
  
  myhost = gethostbyname( NetworkSetting.ServerName );
  
#ifdef PRIVATE
  myhost->h_addr_list[0][0]=192;
  myhost->h_addr_list[0][1]=168;
  myhost->h_addr_list[0][2]=0;
  myhost->h_addr_list[0][3]=21;
#endif
  
  mysock=socket(AF_INET,SOCK_DGRAM,0);
  struct sockaddr_in sain;
  sain.sin_family=AF_INET;
  sain.sin_port=htons(NetworkSetting.ServerPort);
  sain.sin_addr.s_addr=INADDR_ANY;
  bind(mysock,(struct sockaddr *) &sain,sizeof(sain));
  
  int i=1;
  ioctl(mysock,FIONBIO,&i); // set to non-blocking socket
  
  mysain.sin_family=AF_INET;
  mysain.sin_port=htons(NetworkSetting.ServerPort);
  mysain.sin_addr.s_addr= *( (unsigned long *)(myhost->h_addr_list[0]) );
}

void main_CloseSocket(void)
{
  shutdown(mysock,0); // good practice to shutdown the socket.
  
  close(mysock); // remove the socket.
  
  mysock=0;
}

extern void SendPacketToServer_AddPacket(const EPenPacketType PenPacketType,const void *packet,const u32 packetsize);

#define UDPPacketMaxSize (1316)

static DATA_IN_DTCM u8 SendPacketToServer_buf[UDPPacketMaxSize];
static DATA_IN_DTCM u32 SendPacketToServer_bufsize=0;

static void SendPacketToServer_ExecSend(void)
{
  if(SendPacketToServer_bufsize==0) return;
//  _consolePrintf("Send:%dbyte\n",bufsize);
  sendto(mysock,&SendPacketToServer_buf[0],SendPacketToServer_bufsize,0,(struct sockaddr *)&mysain,sizeof(mysain));
  SendPacketToServer_bufsize=0;
}

void SendPacketToServer_AddPacket(const EPenPacketType PenPacketType,const void *packet,const u32 packetsize)
{
  if(UDPPacketMaxSize<(SendPacketToServer_bufsize+(4*3)+packetsize)) SendPacketToServer_ExecSend();
  
  {
    u32 head[3]={PenPacketHeader32,(u32)PenPacketType,packetsize};
    MemCopy8CPU(&head[0],&SendPacketToServer_buf[SendPacketToServer_bufsize],4*3);
    SendPacketToServer_bufsize+=4*3;
  }
  
  MemCopy8CPU(packet,&SendPacketToServer_buf[SendPacketToServer_bufsize],packetsize);
  SendPacketToServer_bufsize+=packetsize;
}

static DATA_IN_DTCM bool mf;
static DATA_IN_DTCM s32 mx,my;

static void Proc_TouchPad(void)
{
  u16 KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
  
  static DATA_IN_DTCM bool DispSwap=false;
  
  bool CurSwap;
  if((KEYS_CUR & (KEY_L | KEY_R))!=0){
    CurSwap=true;
    }else{
    CurSwap=false;
  }
  
  if((KEYS_CUR & (KEY_UP | KEY_RIGHT | KEY_DOWN | KEY_LEFT | KEY_A | KEY_B | KEY_X | KEY_Y))!=0){
    const u32 ActiveWinIndex=WM_Picture;
    
    s32 vx=0,vy=0;
    
    if((KEYS_CUR & (KEY_UP | KEY_X))!=0) vy=-1;
    if((KEYS_CUR & (KEY_DOWN | KEY_B))!=0) vy=1;
    if((KEYS_CUR & (KEY_LEFT | KEY_Y))!=0) vx=-1;
    if((KEYS_CUR & (KEY_RIGHT | KEY_A))!=0) vx=1;
    
    vx*=4;
    vy*=4;
    
    if(vx!=0){
      s32 sx=MWin_GetSBarHPos(ActiveWinIndex);
      MWin_SetSBarHPos(ActiveWinIndex,sx+vx);
    }
    if(vy!=0){
      s32 sy=MWin_GetSBarVPos(ActiveWinIndex);
      MWin_SetSBarVPos(ActiveWinIndex,sy+vy);
    }
    
    MWin_DrawSBarVH(ActiveWinIndex);
    MWin_DrawClient(ActiveWinIndex);
    MWin_TransWindow(ActiveWinIndex);
  }
  
  TIPCEXTouchPad TouchPad;
  MemCopy32CPU((void*)&IPCEX->IPCEXTouchPad,&TouchPad,sizeof(TIPCEXTouchPad));
  
  if(TouchPad.ReadPos==TouchPad.WritePos){
    if(mf==false){
      if(CurSwap!=DispSwap){
        DispSwap=CurSwap;
        if(DispSwap==false){
          POWER_CR &= ~POWER_SWAP_LCDS;
          }else{
          POWER_CR |= POWER_SWAP_LCDS;
        }
      }
    }
    return;
  }
  
  while(TouchPad.ReadPos!=TouchPad.WritePos){
    u32 rpos=TouchPad.ReadPos;
    bool tpress=(bool)TouchPad.Press[rpos];
    s32 tx=(s32)TouchPad.X[rpos];
    s32 ty=(s32)TouchPad.Y[rpos];
    
    if(tpress==true){
      if(mf==false){
        mf=true;
        MWin_MouseDown(DispSwap,tx,ty);
        mx=tx;
        my=ty;
        }else{
        s32 dx=abs(mx-tx);
        s32 dy=abs(my-ty);
        if((2<=dx)||(2<=dy)){
          if((dx<64)&&(dy<64)){
            bool CanIgnore=true;
            if(TouchPad.WritePos==((rpos+1)&IPCEXTouchPadMaxMask)) CanIgnore=false;
            MWin_MouseMove(DispSwap,tx,ty,CanIgnore);
            mx=tx;
            my=ty;
          }
        }
      }
      }else{
      if(mf==true){
        mf=false;
        MWin_MouseUp(DispSwap);
      }
      if(CurSwap!=DispSwap){
        DispSwap=CurSwap;
        if(DispSwap==false){
          POWER_CR &= ~POWER_SWAP_LCDS;
          }else{
          POWER_CR |= POWER_SWAP_LCDS;
        }
      }
    }
    
    TouchPad.ReadPos=(rpos+1)&IPCEXTouchPadMaxMask;
  }
  
  IPCEX->IPCEXTouchPad.ReadPos=TouchPad.ReadPos;
}

static void Proc_UDPRecv(void)
{
  struct sockaddr_in sain;
  int sainsize=sizeof(sain);
  
  REG_IME=0;
  int recvsize=recvfrom(mysock,&pRecvBuf[RecvBufSize],RecvBufMax-RecvBufSize,0,(struct sockaddr *)&sain,&sainsize);
//  if(RecvBufSize!=0) _consolePrintf("RecvAdd:%d->%dbyte\n",RecvBufSize,recvsize);
  if(0<recvsize) RecvBufSize+=recvsize;
  REG_IME=1;
}

#define DSScrYCount (DSScrHeight/DSScrBlockSize)
static bool DSScrY[DSScrYCount];

TPenPacket_UserList PenPacket_UserList;
bool PenPacket_UserList_ReqRefresh;

static void Proc_Con_Proc(void)
{
  u32 datapos;
  EPenPacketType PacketType;
  u32 PacketSize;
  static DATA_IN_DTCM TPenPacket_SetStatus PenPacket_SetStatus;
  static DATA_IN_DTCM TPenPacket_GetDSScr PenPacket_GetDSScr;
  static TPenPacket_SetDSScr PenPacket_SetDSScr;
  static DATA_IN_DTCM TPenPacket_SetLatency PenPacket_SetLatency;
  static DATA_IN_DTCM TPenPacket_Line PenPacket_Line;
  static DATA_IN_DTCM TPenPacket_TickLine PenPacket_TickLine;
  static DATA_IN_DTCM TPenPacket_DrawBox PenPacket_DrawBox;
  static DATA_IN_DTCM TPenPacket_FillBox PenPacket_FillBox;
  static DATA_IN_DTCM TPenPacket_Erase PenPacket_Erase;
  static DATA_IN_DTCM TPenPacket_Sendg721 PenPacket_Sendg721;
  static DATA_IN_DTCM TPenPacket_ChangeRoom PenPacket_ChangeRoom;
  
  while(1){
    datapos=Cons_Buf_GetDataPos();
    if(datapos==(u32)-1) return;
    
    Cons_Buf_SetPos(datapos);
    if(Cons_Buf_GetRestSize()<8) return;
    
    PacketType=(EPenPacketType)Cons_Buf_GetBuf32();
    PacketSize=Cons_Buf_GetBuf32();
    
    if(Cons_Buf_GetRestSize()<PacketSize) return;
    
    switch(PacketType){
      case PenPacketType_SetStatus: {
        if(PacketSize<=TPenPacket_SetStatus_Size){
          Cons_Buf_GetBuf(&PenPacket_SetStatus,PacketSize);
//          PenPacket_SetStatus_Proc(&PenPacket_SetStatus,Con);
        }
      } break;
      case PenPacketType_GetDSScr: {
        if(PacketSize<=TPenPacket_GetDSScr_Size){
          Cons_Buf_GetBuf(&PenPacket_GetDSScr,PacketSize);
//          PenPacket_GetDSScr_Proc(&PenPacket_GetDSScr,Con,Main.UDPS);
        }
      } break;
      case PenPacketType_SetDSScr: {
        if(PacketSize<=TPenPacket_SetDSScr_Size){
          TZLIBData zs;
          zs.SrcSize=PacketSize;
          zs.pSrcBuf=(u8*)safemalloc(zs.SrcSize);
          zs.DstSize=TPenPacket_SetDSScr_Size;
          zs.pDstBuf=(u8*)&PenPacket_SetDSScr;
          Cons_Buf_GetBuf(zs.pSrcBuf,zs.SrcSize);
          zlibdecompress(&zs);
          safefree(zs.pSrcBuf); zs.pSrcBuf=NULL;
          if(DSScrY[PenPacket_SetDSScr.y/DSScrBlockSize]==false){
            DSScrY[PenPacket_SetDSScr.y/DSScrBlockSize]=true;
            PenPacket_SetDSScr_Proc(&PenPacket_SetDSScr);
          }
        }
      } break;
      case PenPacketType_UserList: {
        if(PacketSize<=TPenPacket_UserList_Size){
          TZLIBData zs;
          zs.SrcSize=PacketSize;
          zs.pSrcBuf=(u8*)safemalloc(zs.SrcSize);
          zs.DstSize=TPenPacket_UserList_Size;
          zs.pDstBuf=(u8*)&PenPacket_UserList;
          Cons_Buf_GetBuf(zs.pSrcBuf,zs.SrcSize);
          zlibdecompress(&zs);
          safefree(zs.pSrcBuf); zs.pSrcBuf=NULL;
          PenPacket_UserList_Proc(&PenPacket_UserList);
          PenPacket_UserList_ReqRefresh=true;
        }
      } break;
      case PenPacketType_SetLatency: {
        if(PacketSize<=TPenPacket_SetLatency_Size){
          Cons_Buf_GetBuf(&PenPacket_SetLatency,PacketSize);
          PenPacket_SetLatency_Proc(&PenPacket_SetLatency);
        }
      } break;
      case PenPacketType_Line: {
        if(PacketSize<=TPenPacket_Line_Size){
          Cons_Buf_GetBuf(&PenPacket_Line,PacketSize);
          PenPacket_Line_Proc(&PenPacket_Line);
        }
      } break;
      case PenPacketType_TickLine: {
        if(PacketSize<=TPenPacket_TickLine_Size){
          Cons_Buf_GetBuf(&PenPacket_TickLine,PacketSize);
          PenPacket_TickLine_Proc(&PenPacket_TickLine);
        }
      } break;
      case PenPacketType_DrawBox: {
        if(PacketSize<=TPenPacket_DrawBox_Size){
          Cons_Buf_GetBuf(&PenPacket_DrawBox,PacketSize);
          PenPacket_DrawBox_Proc(&PenPacket_DrawBox);
        }
      } break;
      case PenPacketType_FillBox: {
        if(PacketSize<=TPenPacket_FillBox_Size){
          Cons_Buf_GetBuf(&PenPacket_FillBox,PacketSize);
          PenPacket_FillBox_Proc(&PenPacket_FillBox);
        }
      } break;
      case PenPacketType_Erase: {
        if(PacketSize<=TPenPacket_Erase_Size){
          Cons_Buf_GetBuf(&PenPacket_Erase,PacketSize);
          PenPacket_Erase_Proc(&PenPacket_Erase);
        }
      } break;
      case PenPacketType_Sendg721: {
        if(PacketSize<=TPenPacket_Sendg721_Size){
          Cons_Buf_GetBuf(&PenPacket_Sendg721,PacketSize);
          PenPacket_Sendg721_Proc(&PenPacket_Sendg721);
        }
      } break;
      case PenPacketType_ChangeRoom: {
        if(PacketSize<=TPenPacket_ChangeRoom_Size){
          Cons_Buf_GetBuf(&PenPacket_ChangeRoom,PacketSize);
          PenPacket_ChangeRoom_Proc(&PenPacket_ChangeRoom);
          
          extern u32 RoomNum_CurrentNum;
          extern u32 RoomNum_RequestNum;
          extern u32 RoomNum_SelectNum;
          
          if(RoomNum_CurrentNum==RoomNum_RequestNum){
            RoomNum_RequestNum=(u32)-1;
            RoomNum_SelectNum=(u32)-1;
            MWin_ProgressHide();
            MWin_DrawClient(WM_RoomNum);
            if(MWin_GetVisible(WM_RoomNum)==true){
              void MWinCallRoomNum_CloseButton(u32 WinIndex);
              MWinCallRoomNum_CloseButton(WM_RoomNum);
            }
            MWin_RefreshWindow(WM_Picture);
          }
        }
      } break;
    }
    
    Cons_Buf_DelBuf(Cons_Buf_GetPos());
    
    {
      extern bool curmicbuf_ready;
      if(curmicbuf_ready==true) break;
    }
    
  }
}

static void mainloop_SetLatency(void)
{
  int timeout=-1;
  
  MWin_ProgressShow("Connecting to server...","サーバに接続しています…",1);
  
  u32 lasthb=IPC->heartbeat;
  
  while(1){
    if(timeout<=0){
      if(timeout!=-1) _consolePrintf("Send SetLatency\n");
      Sound_Start("process.wav");
      PenPacket_SetLatency_Send(NetworkSetting.Latency);
      SendPacketToServer_ExecSend();
      timeout=60;
      }else{
      if(lasthb!=IPC->heartbeat){
        lasthb=IPC->heartbeat;
        timeout--;
      }
    }
    Proc_UDPRecv();
    Proc_Con_Proc();
    if(NetworkSetting.Latency==NetworkLatency) break;
  }
  
  MWin_ProgressHide();
}

static void mainloop_GetDSScr(void)
{
  for(u32 idx=0;idx<DSScrYCount;idx++){
    DSScrY[idx]=false;
  }
  
  int timeout=-1;
  
  MWin_ProgressShow("Get previous screen","画像を準備しています…",DSScrYCount);
  
  u32 lastprgpos=0;
  u32 lasthb=IPC->heartbeat;
  
  while(1){
    if(timeout<=0){
      if(timeout==0) _consolePrintf("Retry GetDSScr\n");
      Sound_Start("process.wav");
      for(u32 idx=0;idx<DSScrYCount;idx++){
        if(DSScrY[idx]==false) PenPacket_GetDSScr_Send(idx*DSScrBlockSize);
      }
      SendPacketToServer_ExecSend();
      timeout=60;
      }else{
      if(lasthb!=IPC->heartbeat){
        lasthb=IPC->heartbeat;
        timeout--;
      }
    }
    Proc_UDPRecv();
    Proc_Con_Proc();
    {
      u32 rest=0;
      for(u32 idx=0;idx<DSScrYCount;idx++){
        if(DSScrY[idx]==false) rest++;
      }
      if(rest==0) break;
      if(lastprgpos!=rest){
        lastprgpos=rest;
        MWin_ProgressSetPos(DSScrYCount-rest);
      }
    }
  }
  
  MWin_ProgressHide();
}

void mainloop_init(void)
{
  IPCEX->PlayEnv=PlayEnv_Speaker;
#ifdef TESTCODE
  IPCEX->PlayEnv=PlayEnv_Headphone2;
#endif
  
  Cons_Buf_Init();
  
  PenPacket_Init();
  
#ifdef USEIMFS
  pIMFS=new CIMFS();
  if(pIMFS->InitIMFS()==false){
    _consolePrintf("IMFS open error.\n");
    while(1);
  }
#else
  _consolePrintf("IMFS disabled!!\n");
  while(1);
#endif
  
  {
    u8 *pfon;
    int dummysize;
    
    _consolePrintf("load [system.fon]\n");
    Shell_ReadFile("system.fon",(void**)&pfon,&dummysize);
    
    if(pfon==NULL){
      _consolePrintf("notfound '/shell/system.fon'\n\n");
      _consolePrintf("'ChangeCodePage/cp(your codepage).bat' is executed. When GBFS is used, 'WriteFiles_gbfs.bat' is executed.\n\n");
      _consolePrintf("Please refer to 'ChangeCodePage/codepage.txt' for CodePage.\n\n");
      ShowLogHalt();
    }
    
    FontPro_Init(pfon);
  }
  
  {
    static volatile signed char sc=-1;
    static volatile unsigned char uc=-1;
    static volatile char c=-1;
    
    if((s32)c==(s32)sc) _consolePrintf("Default char is signed char.\n");
    if((s32)c==(s32)uc) _consolePrintf("Default char is unsigned char.\n");
  }
  
  {
    u32 UserLanguage=(u32)-1;
    u32 Timeout=0x10000;
    
    while(UserLanguage==(u32)-1){
      UserLanguage=IPCEX->UserLanguage;
      Timeout--;
      if(Timeout==0){
        _consolePrintf("NDS farmware language read error. ARM7CPU stopped...?\n");
        while(1);
      }
    }
    _consolePrintf("NDS farmware language ID : %d\n",UserLanguage);
    
    switch(UserLanguage){
      case 1: SetLang(EL_ENG); break; // eng
      case 2: SetLang(EL_ENG); break; // fre
      case 4: SetLang(EL_ENG); break; // ita
      case 3: SetLang(EL_ENG); break; // deu
      case 5: SetLang(EL_ENG); break; // esp
      case 0: SetLang(EL_JPN); break; // jpn
      default: SetLang(EL_ENG); break;
    }
  }
  
  {
    u16 KEYS_CUR;
    
    KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
    while(KEYS_CUR!=0){
      KEYS_CUR=(~REG_KEYINPUT)&0x3ff;
    }
  }
  
  InitINI();
  LoadINI("global.ini");
  LoadSkinINI("skin.ini");
  
  _consolePrintf("Initialize MWin components\n");
  
  MWin_Init();
  
  MWin_SetActive(WM_ColorPicker);
//  MWin_SetActive(WM_MicVolume);
  MWin_SetActive(WM_About);
  MWin_SetActive(WM_UserList);
  MWin_SetActive(WM_PlayEnv);
  MWin_SetActive(WM_ItemPalette);
//  MWin_SetActive(WM_RoomNum);
//  for(u32 WM=0;WM<WinBodyCount;WM++) MWin_SetVisible(WM,false);
  MWin_RefreshScreenMask();
  
  MWin_SetSBarV(WM_About,5,5,0);
  
  MWin_SetSBarV(WM_UserList,TPenPacket_UserList_UserMax,5,0);
  
  MWin_SetSBarV(WM_RoomNum,100,12,0);
  
  u32 ofsx=(DSScrWidth-ScreenWidth)/2;
  u32 ofsy=(DSScrHeight-ScreenHeight)/1;
  
  switch(UserLng){
    case ConUserLng_JPN: ofsx=ofsx*0; ofsy=ofsy*0; break;
    case ConUserLng_ENG: ofsx=ofsx*1; ofsy=ofsy*0; break;
    case ConUserLng_FRE: ofsx=ofsx*2; ofsy=ofsy*0; break;
    case ConUserLng_DEU: ofsx=ofsx*0; ofsy=ofsy*1; break;
    case ConUserLng_ITA: ofsx=ofsx*1; ofsy=ofsy*1; break;
    case ConUserLng_ESP: ofsx=ofsx*2; ofsy=ofsy*1; break;
    default: ofsx=ofsx*0; ofsy=ofsy*0; break;
  }
  
  MWin_SetSBarH(WM_Picture,DSScrWidth,ScreenWidth,ofsx);
  MWin_SetSBarV(WM_Picture,DSScrHeight,ScreenHeight,ofsy);
  
  {
    char str[128];
    sprintf(str,"About... %s",ROMTITLE);
    MWin_SetWindowTitle(WM_About,str);
  }
  
  MWin_AllRefresh();
  
}

static void RedrawWave(s16 *pbuf)
{return;
  u16 *pscrbuf=pScreenMain->GetVRAMBuf(ScrMainID_View);
//  MemSet16DMA3(RGB15(0,0,0)|BIT15,pscrbuf,ScreenHeight*ScreenWidth*2);
  static s16 last[ScreenWidth];
  for(u32 idx=0;idx<ScreenWidth-1;idx++){
    s16 data=(pbuf[idx]/32)+96;
    if(data<0) data=0;
    if(192<=data) data=192-1;
    pscrbuf[last[idx]*ScreenWidth]=RGB15(0,0,0)|BIT15;
    last[idx]=data;
    pscrbuf[data*ScreenWidth]=RGB15(31,31,31)|BIT15;
    pscrbuf++;
  }
}

extern u32 MicVolume;
u32 MicVolume;

static void mainloop_RefreshMicVolume(void)
{return;
  static u32 LastMicVolume=(u32)-1;
  if(LastMicVolume!=MicVolume){
    PrfStart();
    LastMicVolume=MicVolume;
    MWin_DrawClient(WM_MicVolume);
    MWin_TransWindow(WM_MicVolume);
    PrfEnd(0);
  }
}

static s16 curmicbuf[g721_DivSamples];
DATA_IN_DTCM bool curmicbuf_ready;
static u32 Mic_IgnoreCount;

static void InterruptHandler_IPC_SYNC(void)
{
  switch(IPCEX->IR){
    case IR_NULL: {
    } break;
    case IR_strpcm: {
      {
        static void (*lp_DC_FlushAll)(void)=DC_FlushAll;
        lp_DC_FlushAll();
      }
      
      for(vu32 a=0;a<0x1000;a++);
      
      s16 *pdstbuf=IPCEX->pstrpcmBuf;
      static DATA_IN_DTCM u32 delay=0;
      
      if(delay!=0){
        if(g721_recv_writepos!=g721_recv_readpos) delay--;
        MemSet16CPU(0,pdstbuf,g721_DivSamples*2);
        }else{
        if(g721_recv_writepos==g721_recv_readpos){ // empty
          MemSet16CPU(0,pdstbuf,g721_DivSamples*2);
          delay=NetworkSetting.Latency*g721_DivCount;
          }else{
          g721_DivDecode(g721_state_dec,&g721_recvbuf[g721_recv_readpos],pdstbuf);
          g721_recv_readpos+=g721_DivBytes;
          if(g721_recv_readpos==g721_recvbufsize) g721_recv_readpos=0;
        }
      }
      DC_FlushRangeOverrun(pdstbuf, g721_DivSamples*2);
    } break;
    case IR_mic: {
      {
        static void (*lp_DC_FlushAll)(void)=DC_FlushAll;
        lp_DC_FlushAll();
      }
      
      if(Mic_IgnoreCount!=0){
        Mic_IgnoreCount--;
        }else{
        if(curmicbuf_ready==false){
          s16 *psrcbuf=IPCEX->pmicBuf;
          DC_FlushRangeOverrun(psrcbuf, g721_DivSamples*2);
          MemCopy16DMA3(psrcbuf,&curmicbuf[0],g721_DivSamples*2);
          curmicbuf_ready=true;
        }
      }
    } break;
  }
  
  IPCEX->IR=IR_NULL;
}

static u16 PenColor16_Line=0,PenColor16_TickLine=1,PenColor16_DrawBox=2,PenColor16_FillBox;

#include "ThemeColorTable.h"

void PenColor16Init(void)
{
  u16 FavoriteColor16=ThemeColorTable16[PersonalData->theme] | BIT15;
  {
    u16 r,g,b;
    r=(FavoriteColor16 >> 10) & 0x1f;
    g=(FavoriteColor16 >> 5) & 0x1f;
    b=(FavoriteColor16 >> 0) & 0x1f;
    FavoriteColor16=RGB15(r,g,b) | BIT15;
  }
  
  u16 c=FavoriteColor16;
  PenColor16_Line=c;
  PenColor16_TickLine=c;
  PenColor16_DrawBox=c;
  PenColor16_FillBox=c;
}

#define MicVolume_Min (0x40000)
#define MicVolume_Max (0x200000)

void mainloop(void)
{
  _consolePrint("mainloop\n");
  
  mf=false;
  mx=0;
  my=0;
  
  VBlankPassed=false;
  
  PenPacket_UserList.UserCount=0;
  PenPacket_UserList_ReqRefresh=true;
  PenColor16Init();
  
#ifdef NotConnect
#else
  if(WifiConnected==true){
    _consolePrintf("Status change to execute mode.\n");
    mainloop_SetLatency();
    mainloop_GetDSScr();
    for(u32 idx=0;idx<8;idx++){
      PenPacket_SetStatus_Send(ConStatus_Execute);
      SendPacketToServer_ExecSend();
      swiWaitForVBlank();
    }
  }
#endif
  
  irqSet(IRQ_IPC_SYNC,(VoidFunctionPointer)InterruptHandler_IPC_SYNC);
  REG_IPC_SYNC=IPC_SYNC_IRQ_ENABLE;
  
  { // start mic
    MicVolume=0;
    curmicbuf_ready=false;
    Mic_IgnoreCount=g721_DivCount*2;
    IPCEX->pmicBuf=(s16*)safemalloc(g721_DivSamples*2);
    IPCEX->strpcmControl=strpcmControl_MicStart;
    while(IPCEX->strpcmControl!=strpcmControl_NOP){
    }
  }
  
  { // start stream pcm
    IPCEX->strpcmControl=strpcmControl_NOP;
    IPCEX->strpcmFreq=g721_PCMFreq;
    IPCEX->strpcmSamples=g721_DivSamples;
    IPCEX->pstrpcmBuf=(s16*)safemalloc(g721_DivSamples*2);
    MemSet16DMA3(0,IPCEX->pstrpcmBuf,g721_DivSamples*2);
    IPCEX->strpcmControl=strpcmControl_Play;
    while(IPCEX->strpcmControl!=strpcmControl_NOP){
    }
  }
  
  PrintFreeMem();
  
  pScreenMain->Flip(true);
  
  u32 lasthb=IPC->heartbeat>>4;
  
  while(1){
    if(VBlankPassed==true){
      VBlankPassed=false;
      Proc_TouchPad();
      u32 hb=IPC->heartbeat>>4;
      if(lasthb!=hb){
        lasthb=hb;
        SendPacketToServer_ExecSend();
      }
    }
    Proc_UDPRecv();
    Proc_Con_Proc();
    
    if(PenPacket_UserList_ReqRefresh==true){
      PenPacket_UserList_ReqRefresh=false;
      MWin_DrawClient(WM_UserList);
      MWin_TransWindow(WM_UserList);
    }
    
    if(curmicbuf_ready==true){
      static s16 cur[g721_DivSamples];
      
      MemCopy16DMA3(&curmicbuf[0],&cur[0],g721_DivSamples*2);
      curmicbuf_ready=false;
      
      if(g721_send_pos==0){
        u32 total=0;
        u32 last=0;
        for(u32 idx=0;idx<g721_DivSamples;idx++){
          u32 smp=(u32)abs((s32)cur[idx]);
          last=((last*7)+smp)/8;
          total+=last;
        }
        
#ifdef TESTCODE
        total=0;
#endif
        
        total<<=2; // 14bit -> 16bit
        MicVolume=total;
        mainloop_RefreshMicVolume();
        
        if(total<MicVolume_Min) total=0;
        if(MicVolume_Max<total){
          total=0;
          _consolePrint("!!MIC Overflow!! ");
        }
        
        if(total!=0){
          g721_DivEncode(g721_state_enc,cur,&g721_sendbuf[g721_DivBytes*g721_send_pos]);
          g721_send_pos++;
        }
        }else{
        g721_DivEncode(g721_state_enc,cur,&g721_sendbuf[g721_DivBytes*g721_send_pos]);
        g721_send_pos++;
      }
      
      if(g721_send_pos==g721_send_count){
        PenPacket_Sendg721_Send(g721_sendbuf);
        g721_send_pos=0;
      }
      
#ifdef TESTCODE
      {
        static u32 b=0;
        if(b!=0){
          b--;
          }else{
          b=100*0;
          TPenPacket_DrawBox packet;
          packet.Color=RGB15(31,31,31) | BIT15;
          packet.lastx=0;
          packet.lasty=0;
          packet.curx=1;
          packet.cury=1;
          SendPacketToServer_AddPacket(PenPacketType_DrawBox,&packet,TPenPacket_DrawBox_Size);
        }
      }
#endif
    }
    
    {
      extern u32 RoomNum_RequestNum;
      extern u32 RoomNum_SelectNum;
      if(RoomNum_RequestNum!=(u32)-1){
        const u32 prgmax=20;
        static u32 prgpos;
        static u32 timeout;
        if(MWin_GetVisible(WM_Progress)==false){
          char *pformat;
          switch(GetLang()){
            case EL_ENG: pformat="Change room to %03d"; break;
            case EL_JPN: pformat="部屋番号(%03d)を変更中…"; break;
            default: pformat="Change room to %03d"; break;
          }
          char str[128];
          snprintf(str,128,pformat,RoomNum_RequestNum);
          MWin_ProgressShow(str,str,prgmax);
          prgpos=0;
          timeout=0;
        }
        if(timeout!=0){
          timeout--;
          swiWaitForVBlank();
          }else{
          PenPacket_ChangeRoom_Send(RoomNum_RequestNum);
          SendPacketToServer_ExecSend();
          timeout=15;
          prgpos++;
          if(prgpos==prgmax){
            Sound_Start("error.wav");
            RoomNum_RequestNum=(u32)-1;
            RoomNum_SelectNum=(u32)-1;
            MWin_DrawClient(WM_RoomNum);
            MWin_TransWindow(WM_RoomNum);
            MWin_ProgressHide();
            }else{
            MWin_ProgressSetPos(prgpos);
          }
        }
      }
    }
    
  }
}

#include "main_drawcb.h"
#include "main_drawcb_closebutton.h"
#include "main_drawcb_regist.h"

