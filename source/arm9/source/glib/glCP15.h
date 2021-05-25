/*---------------------------------------------------------------------------------
	$Id: CP15.h,v 1.3 2005/08/23 17:06:10 wntrmute Exp $

	ARM9 CP15 control

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

	$Log: CP15.h,v $
	Revision 1.3  2005/08/23 17:06:10  wntrmute
	converted all endings to unix
	
	Revision 1.2  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging
	

---------------------------------------------------------------------------------*/

#ifndef glCP15_INCLUDE
#define glCP15_INCLUDE

//////////////////////////////////////////////////////////////////////

#ifndef ARM9
#error CP15 is only for the ARM9
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#include "nds.h"

//////////////////////////////////////////////////////////////////////

#define CPUID_IMPLEMENTOR(id) ((id)>>24)
#define CPUID_ARCH(id)        (((id)>>16) & 0xF)
#define CPUID_PART(id)        (((id)>>4) & 0xFFF)
#define CPUID_VERSION(id)     ((id) & 0xF)

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetID(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetCacheType(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetTCMSize(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetControl(void);
extern void glCP15_SetControl(uint32 data);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetDataCachable(void);
extern uint32 glCP15_GetInstructionCachable(void);
extern void glCP15_SetDataCachable(void);
extern void glCP15_SetInstructionCachable(void);
extern uint32 glCP15_GetDataBufferable(void);
extern void glCP15_SetDataBufferable(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetDataPermissions(void);
extern uint32 glCP15_GetInstructionPermissions(void);
extern void glCP15_SetDataPermissions(void);
extern void glCP15_SetInstructionPermissions(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetRegion0(void);
extern uint32 glCP15_GetRegion1(void);
extern uint32 glCP15_GetRegion2(void);
extern uint32 glCP15_GetRegion3(void);
extern uint32 glCP15_GetRegion4(void);
extern uint32 glCP15_GetRegion5(void);
extern uint32 glCP15_GetRegion6(void);
extern uint32 glCP15_GetRegion7(void);
extern void glCP15_SetRegion0(uint32 data);
extern void glCP15_SetRegion1(uint32 data);
extern void glCP15_SetRegion2(uint32 data);
extern void glCP15_SetRegion3(uint32 data);
extern void glCP15_SetRegion4(uint32 data);
extern void glCP15_SetRegion5(uint32 data);
extern void glCP15_SetRegion6(uint32 data);
extern void glCP15_SetRegion7(uint32 data);

//////////////////////////////////////////////////////////////////////

extern void glCP15_FlushICache(void);
extern void glCP15_FlushICacheEntry(uint32 address);
extern void glCP15_PrefetchICacheLine(uint32 address);
extern void glCP15_FlushDCache(void);
extern void glCP15_FlushDCacheEntry(uint32 address);
extern void glCP15_CleanDCacheEntry(uint32 address);
extern void glCP15_CleanAndFlushDCacheEntry(uint32 address);
extern void glCP15_CleanDCacheEntryByIndex(uint32 index);
extern void glCP15_CleanAndFlushDCacheEntryByIndex(uint32 index);
extern void glCP15_DrainWriteBuffer(void);

//////////////////////////////////////////////////////////////////////

extern void glCP15_WaitForInterrupt(void);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetDCacheLockdown(void);
extern uint32 glCP15_GetICacheLockdown(void);
extern void glCP15_SetDCacheLockdown(uint32 data);
extern void glCP15_SetICacheLockdown(uint32 data);

//////////////////////////////////////////////////////////////////////

extern uint32 glCP15_GetDTCM(void);
extern uint32 glCP15_GetITCM(void);
extern void glCP15_SetDTCM(uint32 data);
extern void glCP15_SetITCM(uint32 data);

//////////////////////////////////////////////////////////////////////
extern void glCP15_ITCMEnableDefault(void);

//////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////
