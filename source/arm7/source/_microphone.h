/*---------------------------------------------------------------------------------
	$Id: audio.h,v 1.13 2005/08/23 17:06:10 wntrmute Exp $

	ARM7 audio control

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

	$Log: audio.h,v $
	Revision 1.13  2005/08/23 17:06:10  wntrmute
	converted all endings to unix
	
	Revision 1.12  2005/08/01 23:18:12  wntrmute
	adjusted headers for logging
	

---------------------------------------------------------------------------------*/

#ifndef _MICROPHONE_ARM7_INCLUDE
#define _MICROPHONE_ARM7_INCLUDE

#include <nds.h>

//---------------------------------------------------------------------------------
// Sound (ARM7 only)
//---------------------------------------------------------------------------------
#ifndef ARM7
#error Audio is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/arm7/serial.h>

//---------------------------------------------------------------------------------
// registers
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
// not sure on the following
//---------------------------------------------------------------------------------


/*---------------------------------------------------------------------------------
	microphone code based on neimod's microphone example.
	See: http://neimod.com/dstek/ 
	Chris Double (chris.double@double.co.nz)
	http://www.double.co.nz/nintendo_ds
---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------
	Read a byte from the microphone
---------------------------------------------------------------------------------*/
//s32 _MIC_ReadData();

/*---------------------------------------------------------------------------------
	Fill the buffer with data from the microphone. The buffer will be
	signed sound data at 16kHz. Once the length of the buffer is
	reached, no more data will be stored. Uses ARM7 timer 0.  
---------------------------------------------------------------------------------*/
void _StartRecording(s16* buffer, int length, int freq);

/*---------------------------------------------------------------------------------
	Stop recording data, and return the length of data recorded.
---------------------------------------------------------------------------------*/
void _StopRecording();

/* This must be called during IRQ_TIMER0 */
//void _ProcessMicrophoneTimerIRQ();

void _PM_SetAmp(u8 control);

//---------------------------------------------------------------------------------
// Turn the microphone on 
//---------------------------------------------------------------------------------
static inline void _MIC_On() {
//---------------------------------------------------------------------------------
  PM_SetAmp(PM_AMP_ON);
}


//---------------------------------------------------------------------------------
// Turn the microphone off 
//---------------------------------------------------------------------------------
static inline void _MIC_Off() {
//---------------------------------------------------------------------------------
  PM_SetAmp(PM_AMP_OFF);
}

#ifdef __cplusplus
}
#endif

#endif

