
#include <nds.h>
#include "def_libnds.h"

#include "a7sleep.h"

static u8 PM_GetRegister(int reg)
{

//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
	SerialWaitBusy();
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg | 0x80;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz ;
	REG_SPIDATA = 0;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 

	return REG_SPIDATA & 0xff;
}
 
static void PM_SetRegister(int reg, int control)
{

	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz | SPI_CONTINUOUS;
	REG_SPIDATA = reg;
 
	SerialWaitBusy();
//	while(REG_SPICNT & SPI_BUSY)
//		SWI_WaitByLoop(1);
 
	REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER |SPI_BAUD_1MHz;
	REG_SPIDATA = control;


}

static void PM_SetControl(int control)
{
	PM_SetRegister(0, PM_GetRegister(0) | control);
}

void a7lcd_select(int control)
{
	control |= PM_GetRegister(0) & ~(PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP);
	PM_SetRegister(0, control&255);
}

void a7poff(void)
{
  PM_SetControl(1<<6);//6 DS power (0: on, 1: shut down!) 
}

void a7SetSoundAmplifier(bool e)
{
	u8 control;
	control = PM_GetRegister(0) & ~PM_SOUND_PWR;
	if(e==true) control|=PM_SOUND_PWR;
	PM_SetRegister(0, control&255);
}

