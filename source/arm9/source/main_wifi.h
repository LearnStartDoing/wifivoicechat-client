
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// for some odd reason I can't seem to find vcount in ndslib -sigh- (I mean, in a place it doesn't conflict with other things)
#define		VCOUNT		(*((u16 volatile *) 0x04000006))

#define WifiTimerInterval_ms (5)

static bool WifiConnected;

//---------------------------------------------------------------------------------
// Dswifi helper functions

// wifi timer function, to update internals of sgIP
void WifiTimer(void) {
  Wifi_Timer(WifiTimerInterval_ms);
}

// notification function to send fifo message to arm7
void arm9_synctoarm7() { // send fifo message
   REG_IPC_FIFO_TX=0x87654321;
}

// interrupt handler to receive fifo messages from arm7
void arm9_fifo() { // check incoming fifo messages
   u32 value = REG_IPC_FIFO_RX;
   if(value == 0x87654321) Wifi_Sync();
}

void main_wifi_Init(void)
{
  // send fifo message to initialize the arm7 wifi
  
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & clear FIFO
  
  u32 Wifi_pass= Wifi_Init(WIFIINIT_OPTION_USELED);
  REG_IPC_FIFO_TX=0x12345678;
  REG_IPC_FIFO_TX=Wifi_pass;
  
  *((volatile u16 *)0x0400010E) = 0; // disable timer3
  
  irqSet(IRQ_TIMER3, WifiTimer); // setup timer IRQ
  irqEnable(IRQ_TIMER3);
  irqSet(IRQ_FIFO_NOT_EMPTY, arm9_fifo); // setup fifo IRQ
  irqEnable(IRQ_FIFO_NOT_EMPTY);
  
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ; // enable FIFO IRQ
  
  Wifi_SetSyncHandler(arm9_synctoarm7); // tell wifi lib to use our handler to notify arm7
  
  // set timer3
  *((volatile u16 *)0x0400010C) = -(131062*WifiTimerInterval_ms/1000); // 131062 * 256 cycles = ~1000ms;
  *((volatile u16 *)0x0400010E) = 0x00C2; // enable, irq, 1/256 clock
  
  while(Wifi_CheckInit()==0) { // wait for arm7 to be initted successfully
    while(VCOUNT>192); // wait for vblank
    while(VCOUNT<192);
  }
  
  // wifi init complete - wifi lib can now be used!
}

void main_wifi_AutoConnect(void)
{
  // simple WFC connect:
  
  WIFI_ASSOCSTATUS st;
  
  _consolePrintf("Connecting via WFC data\n");
  Wifi_DisconnectAP();
  Wifi_AutoConnect(); // request connect
  st=ASSOCSTATUS_DISCONNECTED;
  
  while(1) {
    WIFI_ASSOCSTATUS cst=(WIFI_ASSOCSTATUS)Wifi_AssocStatus(); // check status
    if(st!=cst){
      st=cst;
      switch(st){
        case ASSOCSTATUS_DISCONNECTED: _consolePrintf("ASSOCSTATUS_DISCONNECTED\n"); break;
        case ASSOCSTATUS_SEARCHING: _consolePrintf("ASSOCSTATUS_SEARCHING\n"); break;
        case ASSOCSTATUS_AUTHENTICATING: _consolePrintf("ASSOCSTATUS_AUTHENTICATING\n"); break;
        case ASSOCSTATUS_ASSOCIATING: _consolePrintf("ASSOCSTATUS_ASSOCIATING\n"); break;
        case ASSOCSTATUS_ACQUIRINGDHCP: _consolePrintf("ASSOCSTATUS_ACQUIRINGDHCP\n"); break;
        case ASSOCSTATUS_ASSOCIATED: _consolePrintf("ASSOCSTATUS_ASSOCIATED\n"); break;
        case ASSOCSTATUS_CANNOTCONNECT: {
          _consolePrintf("ASSOCSTATUS_CANNOTCONNECT\n");
          _consolePrintf("\n");
          _consolePrintf("Fatal error!! can not connect.\n");
          ShowLogHalt();
        } break;
      }
    }
    if(st==ASSOCSTATUS_ASSOCIATED){
      _consolePrintf("Connected successfully!\n");
      break;
    }
  }
  
  // if connected, you can now use the berkley sockets interface to connect to the internet!
  WifiConnected=true;
}

void main_wifi_Free(void)
{
  _consolePrintf("Wifi_DisconnectAP();\n");
  Wifi_DisconnectAP();
  _consolePrintf("Wifi_DisableWifi();\n");
  Wifi_DisableWifi();
}

