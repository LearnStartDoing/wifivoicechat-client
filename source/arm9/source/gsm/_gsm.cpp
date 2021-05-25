
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"

#include "_gsm.h"

gsm GSM_Renew(gsm gsmstate)
{
  if(gsmstate!=NULL) gsm_destroy(gsmstate);
  gsmstate=gsm_create();
  return(gsmstate);
}

void GSM_BlockEncode(gsm gsmstate,s16 *psamples,u8 *pblock)
{
  for(u32 cnt=0;cnt<GSM_BlockCount;cnt++){
    gsm_encode (gsmstate, psamples, pblock);
    pblock+=((GSM_BlockBytes/GSM_BlockCount) + 1) / 2;
    psamples+=(GSM_BlockSamples/GSM_BlockCount) / 2;
    gsm_encode (gsmstate, psamples, pblock);
    pblock+=((GSM_BlockBytes/GSM_BlockCount) + 0) / 2;
    psamples+=(GSM_BlockSamples/GSM_BlockCount) / 2;
  }
}

bool GSM_BlockDecode(gsm gsmstate,u8 *pblock,s16 *psamples)
{
  for(u32 cnt=0;cnt<GSM_BlockCount;cnt++){
    if(gsm_decode(gsmstate, pblock, psamples)!=0) return(false);
    pblock+=((GSM_BlockBytes/GSM_BlockCount) + 1) / 2;
    psamples+=(GSM_BlockSamples/GSM_BlockCount) / 2;
    if(gsm_decode(gsmstate, pblock, psamples)!=0) return(false);
    pblock+=((GSM_BlockBytes/GSM_BlockCount) + 0) / 2;
    psamples+=(GSM_BlockSamples/GSM_BlockCount) / 2;
  }
  
  return(true);
}

