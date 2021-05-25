
// ndslib -> libnds

#define _reg_libnds_h // ignore

#ifndef _reg_libnds_h
#define _reg_libnds_h

#define REG_SPICNT (SERIAL_CR)
#define REG_SPIDATA (SERIAL_DATA)

#define SPI_ENABLE (SERIAL_ENABLE)

#define SPI_BAUD_1MHz (SPI_BAUDRATE_1Mhz)

#define REG_IME (IME)
#define REG_IE (IE)
#define REG_IF (IF)

#define REG_IPC_SYNC (IPC_SYNC)

#define REG_KEYXY (XKEYS)
#define	REG_KEYINPUT (KEYS)

#endif
