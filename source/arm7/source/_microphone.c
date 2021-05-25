/*---------------------------------------------------------------------------------
	$Id: microphone.c,v 1.4 2005/09/07 18:06:27 wntrmute Exp $

	Microphone control for the ARM7

  Copyright (C) 2005
			Michael Noland (joat)
			Jason Rogers (dovoto)
			Dave Murphy (WinterMute)
			Chris Double (doublec)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

	$Log: microphone.c,v $
	Revision 1.4  2005/09/07 18:06:27  wntrmute
	use new register names
	
	Revision 1.3  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.2  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib

	Revision 1.1  2005/07/12 17:32:20  wntrmute
	added microphone functions

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <nds/jtypes.h>

#include "_microphone.h"
#include "../../ipcex.h"

#include "_g721.h"

#define NULL (0)

//---------------------------------------------------------------------------------
// Turn on the Microphone Amp. Code based on neimod's example.
//---------------------------------------------------------------------------------
void _PM_SetAmp(u8 control) {
//---------------------------------------------------------------------------------
	SerialWaitBusy();
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = PM_AMP_OFFSET;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
	REG_SPIDATA = control;
}

//---------------------------------------------------------------------------------
// Read a byte from the microphone. Code based on neimod's example.
//---------------------------------------------------------------------------------
static s32 _MIC_ReadData12() {
//---------------------------------------------------------------------------------
	s32 result, result2;
	s32 snddata;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_MICROPHONE | SPI_BAUD_2MHz | SPI_CONTINUOUS;
	REG_SPIDATA = 0xE4;// 0xE4=12bit 0xEC=8bit  // Touchscreen command format for AUX

	SerialWaitBusy();

	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result = REG_SPIDATA;
  	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_TOUCH | SPI_BAUD_2MHz;
	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result2 = REG_SPIDATA;
	
	result = result & 0x7f;
	result2 = (result2>>3)&0x1f;
	
	result = (result<<5) | result2; // u12bit
	snddata=((s32)result)-0x7ff; // s12bit;
	return(snddata);
}

s32 _MIC_ReadData8to12() {
//---------------------------------------------------------------------------------
	s32 result, result2;
	s32 snddata;

	SerialWaitBusy();

	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_MICROPHONE | SPI_BAUD_2MHz | SPI_CONTINUOUS;
	REG_SPIDATA = 0xEC;  // Touchscreen command format for AUX

	SerialWaitBusy();

	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result = REG_SPIDATA;
  	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_TOUCH | SPI_BAUD_2MHz;
	REG_SPIDATA = 0x00;

	SerialWaitBusy();

	result2 = REG_SPIDATA;
	
	result = result & 0x7f;
	result2 = (result2>>7)&0x01;
	
	result = (result<<1) | result2; // u8bit
	snddata=(((s32)result)-128)<<4; // s12bit;
	return (snddata);
}

static s16* microphone_buffer = NULL;
static int microphone_buffer_length;
static int microphone_buffer_count;

bool microphone_reqcopy;

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

extern void _ProcessMicrophoneFlipTimerIRQ(void);

//---------------------------------------------------------------------------------
static void _ProcessMicrophoneTimerIRQ() {
//---------------------------------------------------------------------------------
  u32 cnt=microphone_buffer_count;
  microphone_buffer[cnt] = _MIC_ReadData12();
  cnt++;
  if(cnt==(g721_DivSamples*1)){
    _ProcessMicrophoneFlipTimerIRQ();
    }else{
    if(cnt==(g721_DivSamples*2)){
      cnt=0;
      _ProcessMicrophoneFlipTimerIRQ();
    }
  }
  microphone_buffer_count=cnt;
}

//---------------------------------------------------------------------------------
void _StartRecording(s16* buffer, int length, int freq) {
//---------------------------------------------------------------------------------
  if(length!=(g721_DivSamples*2)) while(1);
  
  microphone_buffer = buffer;
  microphone_buffer_length = g721_DivSamples*2;
  microphone_buffer_count = 0;
  
  _MIC_On();
  
  // Setup a 16kHz timer
  TIMER0_DATA = (0x10000 - ((16777216*2) / freq));
  TIMER0_CR = TIMER_DIV_1 | TIMER_IRQ_REQ;
  
  irqSet(IRQ_TIMER0,_ProcessMicrophoneTimerIRQ);
  irqEnable(IRQ_TIMER0);
  
  TIMER0_CR |= TIMER_ENABLE;
}

//---------------------------------------------------------------------------------
void _StopRecording() {
//---------------------------------------------------------------------------------
  TIMER0_CR = 0;
  TIMER1_CR = 0;
  irqDisable(IRQ_TIMER0);
  
  _MIC_Off();
  microphone_buffer = NULL;
}

