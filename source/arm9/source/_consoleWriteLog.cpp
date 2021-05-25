
#include <NDS.h>

#include "_console.h"
#include "_consoleWriteLog.h"

void PrfStart(void)
{
  TIMER0_CR=0;
  TIMER0_DATA=0;
  TIMER0_CR=TIMER_ENABLE | TIMER_DIV_1024;
//  TIMER0_CR=TIMER_ENABLE | TIMER_DIV_64;
}

void PrfEnd(int data)
{
  double us=TIMER0_DATA;
  us=us*1024/(33.34*1000*1000);
//  us=us*64/(33.34*1000*1000);
  
  if(data!=-1){
    _consolePrintf("prf data=%d %6dus\n",data,(s32)(us*1000*1000));
    }else{
    _consolePrintf("prf %6dus\n",(s32)(us*1000*1000));
  }
}
