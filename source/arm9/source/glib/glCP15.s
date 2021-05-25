
.arm
.align 4


.global glCP15_GetID
glCP15_GetID:
		mrc	p15, 0, r0, c0, c0, 0
		bx	lr


.global glCP15_GetCacheType
glCP15_GetCacheType:
		mrc	p15, 0, r0, c0, c0, 1
		bx	lr


.global glCP15_GetTCMSize
glCP15_GetTCMSize:
		mrc	p15, 0, r0, c0, c0, 2
		bx	lr


.global glCP15_GetControl
.global glCP15_SetControl

glCP15_GetControl:
		mrc	p15, 0, r0, c1, c0, 0
		bx	lr

glCP15_SetControl:
		mcr	p15, 0, r0, c1, c0, 0
		bx	lr


.global glCP15_GetDataCachable
.global glCP15_GetInstructionCachable
.global glCP15_SetDataCachable
.global glCP15_SetInstructionCachable

glCP15_GetDataCachable:
		mrc	p15, 0, r0, c2, c0, 0
		bx	lr

glCP15_GetInstructionCachable:
		mrc	p15, 0, r0, c2, c0, 1
		bx	lr

glCP15_SetDataCachable:
		mcr	p15, 0, r0, c2, c0, 0
		bx	lr

glCP15_SetInstructionCachable:
		mcr	p15, 0, r0, c2, c0, 1
		bx	lr


.global glCP15_GetDataBufferable
.global glCP15_SetDataBufferable

glCP15_GetDataBufferable:
		mrc	p15, 0, r0, c3, c0, 0
		bx	lr

glCP15_SetDataBufferable:
		mcr	p15, 0, r0, c3, c0, 0
		bx	lr


.global glCP15_GetDataPermissions
.global glCP15_GetInstructionPermissions
.global glCP15_SetDataPermissions
.global glCP15_SetInstructionPermissions

glCP15_GetDataPermissions:
		mrc	p15, 0, r0, c5, c0, 2
		bx	lr

glCP15_GetInstructionPermissions:
		mrc	p15, 0, r0, c5, c0, 3
		bx	lr

glCP15_SetDataPermissions:
		mcr	p15, 0, r0, c5, c0, 2
		bx	lr

glCP15_SetInstructionPermissions:
		mcr	p15, 0, r0, c5, c0, 3
		bx	lr


.global	glCP15_GetRegion0
.global	glCP15_GetRegion1
.global	glCP15_GetRegion2
.global	glCP15_GetRegion3
.global	glCP15_GetRegion4
.global	glCP15_GetRegion5
.global	glCP15_GetRegion6
.global	glCP15_GetRegion7
.global	glCP15_SetRegion0
.global	glCP15_SetRegion1
.global	glCP15_SetRegion2
.global	glCP15_SetRegion3
.global	glCP15_SetRegion4
.global	glCP15_SetRegion5
.global	glCP15_SetRegion6
.global	glCP15_SetRegion7

glCP15_GetRegion0:
		mrc	p15, 0, r0, c6, c0, 0
		bx	lr
glCP15_GetRegion1:
		mrc	p15, 0, r0, c6, c1, 0
		bx	lr
glCP15_GetRegion2:
		mrc	p15, 0, r0, c6, c2, 0
		bx	lr
glCP15_GetRegion3:
		mrc	p15, 0, r0, c6, c3, 0
		bx	lr
glCP15_GetRegion4:
		mrc	p15, 0, r0, c6, c4, 0
		bx	lr
glCP15_GetRegion5:
		mrc	p15, 0, r0, c6, c5, 0
		bx	lr
glCP15_GetRegion6:
		mrc	p15, 0, r0, c6, c6, 0
		bx	lr
glCP15_GetRegion7:
		mrc	p15, 0, r0, c6, c7, 0
		bx	lr


glCP15_SetRegion0:
		mcr	p15, 0, r0, c6, c0, 0
		bx	lr
glCP15_SetRegion1:
		mcr	p15, 0, r0, c6, c1, 0
		bx	lr
glCP15_SetRegion2:
		mcr	p15, 0, r0, c6, c2, 0
		bx	lr
glCP15_SetRegion3:
		mcr	p15, 0, r0, c6, c3, 0
		bx	lr
glCP15_SetRegion4:
		mcr	p15, 0, r0, c6, c4, 0
		bx	lr
glCP15_SetRegion5:
		mcr	p15, 0, r0, c6, c5, 0
		bx	lr
glCP15_SetRegion6:
		mcr	p15, 0, r0, c6, c6, 0
		bx	lr
glCP15_SetRegion7:
		mcr	p15, 0, r0, c6, c7, 0
		bx	lr


.global glCP15_FlushICache
.global glCP15_FlushICacheEntry
.global glCP15_PrefetchICacheLine
.global glCP15_FlushDCache
.global glCP15_FlushDCacheEntry
.global glCP15_CleanDCacheEntry
.global glCP15_CleanAndFlushDCache
.global glCP15_CleanAndFlushDCacheEntry
.global glCP15_CleanDCacheEntryByIndex
.global glCP15_CleanAndFlushDCacheEntryByIndex

glCP15_FlushICache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c5, 0
		bx	lr

glCP15_FlushICacheEntry:
		mcr	p15, 0, r0, c7, c5, 1
		bx	lr

glCP15_PrefetchICacheLine:
		mcr	p15, 0, r0, c7, c13, 1
		bx	lr

glCP15_FlushDCache:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c6, 0
		bx	lr

glCP15_FlushDCacheEntry:
		mcr	p15, 0, r0, c7, c6, 1
		bx	lr

glCP15_CleanDCacheEntry:
		mcr	p15, 0, r0, c7, c10, 1
		bx	lr

glCP15_CleanAndFlushDCache:
		mov	r1, #0
	outer_loop:
		mov	r0, #0
	inner_loop:
		orr	r2, r1, r0
		mcr	p15, 0, r2, c7, c14, 2
		add	r0, r0, #0x20
		cmp	r0, #0x400
		bne	inner_loop
		add	r1, r1, #0x40000000
		cmp	r1, #0x0
		bne	outer_loop
		bx	lr

glCP15_CleanAndFlushDCacheEntry:
		mcr	p15, 0, r0, c7, c14, 1
		bx	lr

glCP15_CleanDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c10, 2
		bx	lr

glCP15_CleanAndFlushDCacheEntryByIndex:
		mcr	p15, 0, r0, c7, c14, 2
		bx	lr


.global glCP15_DrainWriteBuffer
glCP15_DrainWriteBuffer:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c10, 4
		bx	lr


.global glCP15_WaitForInterrupt
glCP15_WaitForInterrupt:
		mov	r0, #0
		mcr	p15, 0, r0, c7, c0, 4
		bx	lr


.global glCP15_GetDCacheLockdown
.global glCP15_GetICacheLockdown
.global glCP15_SetDCacheLockdown
.global glCP15_SetICacheLockdown

glCP15_GetDCacheLockdown:
		mrc	p15, 0, r0, c9, c0, 0
		bx	lr

glCP15_GetICacheLockdown:
		mrc	p15, 0, r0, c9, c0, 1
		bx	lr

glCP15_SetDCacheLockdown:
		mcr	p15, 0, r0, c9, c0, 0
		bx	lr

glCP15_SetICacheLockdown:
		mcr	p15, 0, r0, c9, c0, 1
		bx	lr


.global glCP15_GetDTCM
.global glCP15_GetITCM
.global glCP15_SetDTCM
.global glCP15_SetITCM

glCP15_GetDTCM:
		mrc	p15, 0, r0, c9, c1, 0
		bx	lr

glCP15_GetITCM:
		mrc	p15, 0, r0, c9, c1, 1
		bx	lr

glCP15_SetDTCM:
		mcr	p15, 0, r0, c9, c1, 0
		bx	lr

glCP15_SetITCM:
		mcr	p15, 0, r0, c9, c1, 1
		bx	lr

.global glCP15_ITCMEnableDefault
glCP15_ITCMEnableDefault:
		mcr	p15, 0, r0, c7, c5	

		mov r0, #0							@make sure protection regions are all disabled
		mcr p15, 0, r0, c6, c0, 0
		mcr p15, 0, r0, c6, c1, 0
		mcr p15, 0, r0, c6, c2, 0
		mcr p15, 0, r0, c6, c3, 0
		mcr p15, 0, r0, c6, c4, 0
		mcr p15, 0, r0, c6, c5, 0
		mcr p15, 0, r0, c6, c6, 0
		mcr p15, 0, r0, c6, c7, 0
		
		ldr r0, =0x3F					@set read write access on regon 0 and 1
		mcr p15, 0, r0, c6, c0, 0

		ldr r0, =0x3					@set all mem to read write			
		mcr p15, 0, r0, c5, c0, 3
	
		mov r0, #1      					@enable catching for region 1
		mcr p15, 0, r0, c2, c0, 1
	
		ldr r0, =0x5707D			@enable ITCM and Protection unit
		mrc p15, 0, r0, c1, c0, 0
		orr r0, r1, r0
		mcr p15, 0, r0, c1, c0, 0

.end
