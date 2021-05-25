
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_console.h"
#include "maindef.h"
#include "memtool.h"
#include "_const.h"

#include "_g721.h"

void g721_Renew(g721 g721state)
{
  _consolePrintf("r");
  g72x_init_state(g721state);
}

void g721_DivEncode(g721 g721state,s16 *psamples,u8 *pblock)
{
/*
  for(u32 idx=0;idx<g721_BlockBytes;idx++){
    u32 code;
    code=g721_encoder(psamples[idx],g721state);
    pblock[idx]=code;
  }
*/
  g721_blkencoder(psamples,pblock,g721_DivSamples,g721state);
}

bool g721_DivDecode(g721 g721state,u8 *pblock,s16 *psamples)
{
/*
  for(u32 idx=0;idx<g721_BlockBytes;idx++){
    s32 smp;
    smp=g721_decoder(pblock[idx],g721state);
    psamples[idx]=smp;
  }
*/
  g721_blkdecoder(pblock,psamples,g721_DivSamples,g721state);
  
  return(true);
}

