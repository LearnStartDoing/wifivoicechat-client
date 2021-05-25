
// callback to allow wifi library to notify arm9
void arm7_synctoarm9() { // send fifo message
   REG_IPC_FIFO_TX = 0x87654321;
}
// interrupt handler to allow incoming notifications from arm9
void arm7_fifo() { // check incoming fifo messages
   u32 msg = REG_IPC_FIFO_RX;
   if(msg==0x87654321) Wifi_Sync();
}

void main_wifi_Init(void)
{
  irqSet(IRQ_WIFI, Wifi_Interrupt); // set up wifi interrupt
  irqEnable(IRQ_WIFI);

  { // sync with arm9 and init wifi
  	u32 fifo_temp;   

	  while(1) { // wait for magic number
    	while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
      fifo_temp=REG_IPC_FIFO_RX;
      if(fifo_temp==0x12345678) break;
   	}
   	while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
   	fifo_temp=REG_IPC_FIFO_RX; // give next value to wifi_init
   	Wifi_Init(fifo_temp);
   	
   	irqSet(IRQ_FIFO_NOT_EMPTY,arm7_fifo); // set up fifo irq
   	irqEnable(IRQ_FIFO_NOT_EMPTY);
   	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

   	Wifi_SetSyncHandler(arm7_synctoarm9); // allow wifi lib to notify arm9
  } // arm7 wifi init complete
}

