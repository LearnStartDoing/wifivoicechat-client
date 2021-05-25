/*---------------------------------------------------------------------------------
	$Id: template.c,v 1.2 2005/09/07 20:06:06 wntrmute Exp $

	Simple ARM7 stub (sends RTC, TSC, and X/Y data to the ARM 9)

	$Log: template.c,v $
	Revision 1.2  2005/09/07 20:06:06  wntrmute
	updated for latest libnds changes
	
	Revision 1.8  2005/08/03 05:13:16  wntrmute
	corrected sound code


---------------------------------------------------------------------------------*/
#include <nds.h>

#include <nds/bios.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/clock.h>

#include <dswifi7.h>

#include "_microphone.h"

#include "../../ipcex.h"

#include "_g721.h"

#define NULL (0)

#undef SOUND_FREQ
//#define SOUND_FREQ(n)	(0x10000 - (16756000 / (n)))
#define SOUND_FREQ(n)	(0x10000 - (16777216 / (n)))

static void MemCopy16DMA3(void *src,void *dst,u32 len)
{
  while((DMA3_CR&DMA_BUSY)!=0);
  DMA3_SRC = (u32)src;
  DMA3_DEST = (u32)dst;
  DMA3_CR=(DMA_16_BIT | DMA_ENABLE | DMA_START_NOW | DMA_SRC_INC | DMA_DST_INC)+(len/2);
  while((DMA3_CR&DMA_BUSY)!=0);
}

static void MemSet16DMA3(u16 v,void *dst,u32 len)
{
  while((DMA3_CR&DMA_BUSY)!=0);
  DMA3_SRC = (u32)&v;
  DMA3_DEST = (u32)dst;
  DMA3_CR=(DMA_16_BIT | DMA_ENABLE | DMA_START_NOW | DMA_SRC_FIX | DMA_DST_INC)+(len/2);
  while((DMA3_CR&DMA_BUSY)!=0);
}

//---------------------------------------------------------------------------------
static void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format) {
//---------------------------------------------------------------------------------
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}


//---------------------------------------------------------------------------------
static s32 getFreeSoundChannel() {
//---------------------------------------------------------------------------------
	int i;
	for (i=2; i<16; i++) { // 2 to 15
		if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
	}
	return -1;
}

static void VblankHandler_MainThread(void) {
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0, batt=0, aux=0;
	int t1=0, t2=0;
	uint32 temp=0;
	uint8 ct[sizeof(IPC->curtime)];

	// Read the touch screen
	but = REG_KEYXY;

	if (!(but & (1<<6))) {

		touchPosition tempPos = touchReadXY();

//		x = tempPos.x;
//		y = tempPos.y;
		xpx = tempPos.px;
		ypx = tempPos.py;
	}
	
    {
      bool press;
      bool ignore=false;
      static u32 ignorecount;
      if((but & 0x40)==0){
        if(ignorecount!=0){
          ignorecount--;
          ignore=true;
          }else{
          press=true;
          if((xpx<2)||((256-2)<xpx)) ignore=true;
          if((ypx<2)||((192-2)<ypx)) ignore=true;
        }
        }else{
        if(ignorecount!=0) ignore=true;
        ignorecount=1;
        press=false;
      }
      
      if(ignore==false){
        TIPCEXTouchPad *pTouchPad=&IPCEX->IPCEXTouchPad;
        u32 wpos=pTouchPad->WritePos;
        pTouchPad->Press[wpos]=(u8)press;
        pTouchPad->X[wpos]=(u16)xpx;
        pTouchPad->Y[wpos]=(u16)ypx;
        pTouchPad->WritePos=(wpos+1)&IPCEXTouchPadMaxMask;
      }
    }

//	z1 = touchRead(TSC_MEASURE_Z1);
//	z2 = touchRead(TSC_MEASURE_Z2);

	
//	batt = touchRead(TSC_MEASURE_BATTERY);
//	aux  = touchRead(TSC_MEASURE_AUX);

	// Read the time
//	rtcGetTime((uint8 *)ct);
//	BCDToInteger((uint8 *)&(ct[1]), 7);

	// Read the temperature
//	temp = touchReadTemperature(&t1, &t2);

	// Update the IPC struct
//	IPC->buttons		= but;
//	IPC->touchX			= x;
//	IPC->touchY			= y;
//	IPC->touchXpx		= xpx;
//	IPC->touchYpx		= ypx;
//	IPC->touchZ1		= z1;
//	IPC->touchZ2		= z2;
//	IPC->battery		= batt;
//	IPC->aux			= aux;

/*
	for(i=0; i<sizeof(ct); i++) {
		IPC->curtime[i] = ct[i];
	}
*/

//	IPC->temperature = temp;
//	IPC->tdiode1 = t1;
//	IPC->tdiode2 = t2;
}

static bool VBlankApply;

//---------------------------------------------------------------------------------
static void VblankHandler(void) {
//---------------------------------------------------------------------------------
  VBlankApply=true;
  
  IPC->heartbeat++;
  VblankHandler_MainThread();
//  IPCEX->IR=IR_vsync;
//  REG_IPC_SYNC|=IPC_SYNC_IRQ_REQUEST;
}

#include "a7sleep.h"

#include "main_wifi.h"

#define strpcmMultiple (2)
static u32 strpcmWritePos;
static bool strpcm_reqcopy;
static s16 *strpcm_reqcopy_pdstbuf;
static s16 pstrpcmBuf[g721_BlockSamples*strpcmMultiple*2];

static void InterruptHandler_Timer3_strpcm(void)
{
  strpcm_reqcopy_pdstbuf=&pstrpcmBuf[g721_DivSamples*strpcmMultiple*strpcmWritePos];
  strpcmWritePos++;
  if(strpcmWritePos==(g721_DivCount*2)) strpcmWritePos=0; // *2 for Double buffering.
  strpcm_reqcopy=true;
}

static void strpcmPlay(void)
{
  strpcmWritePos=false;
  strpcm_reqcopy=false;
  strpcm_reqcopy_pdstbuf=NULL;
  
  if(IPCEX->strpcmSamples!=g721_DivSamples) while(1); // halt.
  
  if(IPCEX->strpcmFreq!=g721_PCMFreq) while(1); // halt.
  
  u32 bytelen=g721_BlockSamples*strpcmMultiple*2; // *2 for Double buffering.
  
  MemSet16DMA3(0,pstrpcmBuf,bytelen*2);
  
  SCHANNEL_TIMER(0)  = SOUND_FREQ(IPCEX->strpcmFreq*strpcmMultiple);
  SCHANNEL_SOURCE(0) = (u32)pstrpcmBuf;
  SCHANNEL_LENGTH(0) = (bytelen*2) >> 2 ; // for 16bit (4byte/1block)
  SCHANNEL_CR(0)     = SCHANNEL_ENABLE | SOUND_REPEAT | SOUND_VOL(127) | SOUND_PAN(64) | SOUND_16BIT;
  
  TIMER2_DATA = (0x10000 - ((16777216*2) / IPCEX->strpcmFreq));
  TIMER2_CR = TIMER_DIV_1;
  
  TIMER3_DATA = 0x10000 - g721_DivSamples;
  TIMER3_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
  
  irqSet(IRQ_TIMER3, InterruptHandler_Timer3_strpcm);
  irqEnable(IRQ_TIMER3);
  
  TIMER2_CR |= TIMER_ENABLE;
}

static void strpcmStop(void)
{
  irqDisable(IRQ_TIMER3);
  
  TIMER2_CR = 0;
  TIMER3_CR = 0;
  SCHANNEL_CR(0)=0;
}

static void main_strpcm_proc(void)
{
  if(strpcm_reqcopy==false) return;
  strpcm_reqcopy=false;
  
  while(IPCEX->IR!=IR_NULL){
    vu32 w;
    for(w=0;w<0x100;w++);
  }
  IPCEX->IR=IR_strpcm;
  REG_IPC_SYNC|=IPC_SYNC_IRQ_REQUEST;
  while(IPCEX->IR!=IR_NULL){
    vu32 w;
    for(w=0;w<0x100;w++);
  }
  
  s16 *psrc=IPCEX->pstrpcmBuf;
  s16 *pdst=strpcm_reqcopy_pdstbuf;
  
  u32 shift=2; // 14bit -> 16bit
  switch(IPCEX->PlayEnv){
    case PlayEnv_Speaker: break;
    case PlayEnv_Headphone1: shift+=1; break; // volume x2
    case PlayEnv_Headphone2: shift+=2; break; // volume x4
  }
  
  static s32 stlast=0;
  s32 last=stlast;
  
  u32 idx;
  for(idx=0;idx<g721_DivSamples;idx++){
    s32 smp=((s32)psrc[idx])<<shift;
    if(smp<-0x8000) smp=-0x8000;
    if(0x7fff<smp) smp=0x7fff;
    pdst[idx*2+0]=(s16)last;
    pdst[idx*2+1]=(s16)((last+smp)/2);
    last=smp;
  }
  
  stlast=last;
  
//  if(strpcm_reqcopy==true) while(1);
}

static bool micFlipFlag;
static bool mic_reqcopy;
static s16 pmicBuf[g721_DivSamples*2];

void _ProcessMicrophoneFlipTimerIRQ()
{
  mic_reqcopy=true;
}

static void micStart(void)
{
  micFlipFlag=false;
  mic_reqcopy=false;
  
  MemSet16DMA3(0,IPCEX->pmicBuf,g721_DivSamples*2);
  _StartRecording(pmicBuf,g721_DivSamples*2,g721_PCMFreq);
}

static void micStop(void)
{
  _StopRecording();
}

static void main_mic_proc(void)
{
  if(mic_reqcopy==false) return;
  mic_reqcopy=false;
  
  s16 *psrc;
  s16 *pdst=IPCEX->pmicBuf;
  
  if(micFlipFlag==false){
    micFlipFlag=true;
    psrc=&pmicBuf[g721_DivSamples*0];
    }else{
    micFlipFlag=false;
    psrc=&pmicBuf[g721_DivSamples*1];
  }
  
  const u32 size=g721_DivSamples;
  u32 idx;
  for(idx=0;idx<size;idx++){
    s32 data=((s32)psrc[idx])<<2; // 12bit -> 14bit
    if(data<-0x1f00) data=-0x1f00;
    if(0x1eff<data) data=0x1eff;
    pdst[idx]=(s16)data;
  }
  
  while(IPCEX->IR!=IR_NULL){
    vu32 w;
    for(w=0;w<0x100;w++);
  }
  IPCEX->IR=IR_mic;
  REG_IPC_SYNC|=IPC_SYNC_IRQ_REQUEST;
  
//  if(mic_reqcopy==true) while(1);
}

// read firmware.
// reference hbfirmware.zip/firmware/arm7/source/settings.c

//typedef void(*call0)(void);
typedef void(*call3)(u32,void*,u32);

//size must be a multiple of 4
static inline void read_nvram(u32 src, void *dst, u32 size) {
	((call3)0x2437)(src,dst,size);
}

//read firmware settings
static inline void load_PersonalData() {
	u32 src, count0, count1;

	read_nvram(0x20, &src, 4);		//find settings area
	src=(src&0xffff)*8;

	read_nvram(src+0x70, &count0, 4);	//pick recent copy
	read_nvram(src+0x170, &count1, 4);
	if((u16)count0<(u16)count1){
		src+=0x100;
	}
	
	read_nvram(src, PersonalData, 0x80);
	if(swiCRC16(0xffff,PersonalData,0x70) != ((u16*)PersonalData)[0x72/2]){ 	//invalid / corrupt?
		read_nvram(src^0x100, PersonalData, 0x80);	//try the older copy
	}
}

static inline void main_InitSoundDevice(void)
{
  powerON(POWER_SOUND);
  SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
  
  swiChangeSoundBias(1,0x400);
  a7SetSoundAmplifier(true);
  
  IPC->soundData = 0;
  IPCEX->strpcmControl=strpcmControl_NOP;
}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
  IPCEX->IR=IR_NULL;
  IPCEX->pmicBuf=NULL;
  IPCEX->UserLanguage=-1;
  
  IPCEX->IPCEXTouchPad.ReadPos=0;
  IPCEX->IPCEXTouchPad.WritePos=0;
  
  REG_IME=0;
  REG_IE=0;
  REG_IF=REG_IF;
  
  // Clear DMA
  u32 i;
  for(i=0;i<0x30/4;i++){
    *((vu32*)(0x40000B0+i))=0;
  }
  
  rtcReset();
  a7lcd_select(PM_BACKLIGHT_TOP | PM_BACKLIGHT_BOTTOM);
  
  touchReadXY();
  
  REG_SPICNT = SPI_ENABLE|SPI_CONTINUOUS|SPI_DEVICE_NVRAM;
  load_PersonalData();
  REG_SPICNT = 0;
  IPCEX->UserLanguage=PersonalData->language;
  
  //enable sound
  main_InitSoundDevice();
  
  irqInit();
  VBlankApply=false;
  irqSet(IRQ_VBLANK, VblankHandler);
  irqEnable(IRQ_VBLANK);
  
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & prepare fifo asap
  
  main_wifi_Init();
  
  while (1){
    while(1){
      swiIntrWait(1,IRQ_VBLANK|IRQ_TIMER1|IRQ_TIMER3); // vblank, mic, strpcm
      main_mic_proc();
      main_strpcm_proc();
      if(VBlankApply==true) break;
    }
    VBlankApply=false;
    Wifi_Update(); // update wireless in vblank
    
    u32 strpcmControl=IPCEX->strpcmControl;
    if(strpcmControl!=strpcmControl_NOP){
      switch(strpcmControl){
        case strpcmControl_NOP: break;
        case strpcmControl_Play: strpcmPlay(); break;
        case strpcmControl_Stop: strpcmStop(); break;
        case strpcmControl_MicStart: micStart(); break;
        case strpcmControl_MicStop: micStop(); break;
      }
      IPCEX->strpcmControl=strpcmControl_NOP;
    }
    
    {
      // base source writed by SaTa. (include 2005/09/27 Moonlight.)
      // 垂直同期割り込みとかで定期的にチェック
      // **********************************************************************
      // Put this code into your ARM7 interrupt handler
      u32 bootfunc=*((vu32*)0x027FFE24);
      if (bootfunc == (u32)0x027FFE05){
        REG_IME=IME_DISABLE;
        a7poff();
        while(1);
      }
      if (bootfunc == (u32)0x027FFE04){
        a7lcd_select(PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP);
        REG_IME = IME_DISABLE;	// Disable interrupts
        REG_IF = REG_IF;	// Acknowledge interrupt
        *((vu32*)0x027FFE34) = (u32)0x08000000;	// Bootloader start address
        swiSoftReset();	// Jump to boot loader
      }
      // ***********************************************************************
    }
    
    //sound code  :)
    TransferSound *snd = IPC->soundData;
    if ((0 != snd)&&(snd->count!=0)) {
      IPC->soundData = 0;
      u32 i;
      for (i=0; i<snd->count; i++) {
        s32 chan = getFreeSoundChannel();
        if(chan>=0) startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
      }
    }
    
/*
    if(IPCEX->IR==IR_NULL){
      Wifi_Update(); // update wireless in vblank
      REG_IME=0;
      IPC->heartbeat++;
      VblankHandler_MainThread();
      IPCEX->IR=IR_vsync;
      REG_IPC_SYNC|=IPC_SYNC_IRQ_REQUEST;
      REG_IME=1;
    }
*/
  }
}


