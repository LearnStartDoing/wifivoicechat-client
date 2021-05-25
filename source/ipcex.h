/*---------------------------------------------------------------------------------
	$Id: ipc.h,v 1.13 2006/01/17 09:47:00 wntrmute Exp $

	Inter Processor Communication

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

	$Log: ipc.h,v $
	Revision 1.13  2006/01/17 09:47:00  wntrmute
	*** empty log message ***
	
	Revision 1.12  2005/09/20 04:59:04  wntrmute
	replaced defines with enums
	replaced macros with static inlines
	
	Revision 1.11  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.10  2005/08/03 05:26:42  wntrmute
	use BIT macro
	corrected header include

	Revision 1.9  2005/07/27 02:20:05  wntrmute
	resynchronise with ndslib
	Updated GL with float wrappers for NeHe


---------------------------------------------------------------------------------*/

#ifndef NDS_IPCEX_INCLUDE
#define NDS_IPCEX_INCLUDE


#include <nds/jtypes.h>

#define PlayEnv_Speaker (0)
#define PlayEnv_Headphone1 (1)
#define PlayEnv_Headphone2 (2)

#define strpcmControl_NOP (0)
#define strpcmControl_Play (1)
#define strpcmControl_Stop (2)
#define strpcmControl_MicStart (3)
#define strpcmControl_MicStop (4)

#define IR_NULL (0)
#define IR_strpcm (1)
#define IR_mic (2)

#define IPCEXTouchPadMax (16)
#define IPCEXTouchPadMaxMask (IPCEXTouchPadMax-1)

typedef struct {
  u32 ReadPos,WritePos;
  u8 Press[IPCEXTouchPadMax];
  u16 X[IPCEXTouchPadMax],Y[IPCEXTouchPadMax];
} TIPCEXTouchPad;

//---------------------------------------------------------------------------------
typedef struct sTransferRegionEX {
//---------------------------------------------------------------------------------
  u32 UserLanguage;       // from BIOS
  u32 PlayEnv;
  
  TIPCEXTouchPad IPCEXTouchPad;
  
  u32 IR;
  
  s16 *pmicBuf;
  
  vu32 strpcmControl;
  vu32 strpcmFreq,strpcmSamples;
  s16 *pstrpcmBuf;
} TransferRegionEX, * pTransferRegionEX;

#define IPCEX ((TransferRegionEX volatile *)(0x027FF000+sizeof(TransferRegion)))

#endif


