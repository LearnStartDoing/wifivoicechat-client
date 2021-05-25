
#ifndef __gsm_h
#define __gsm_h

#include "gsm/gsm.h"

#define GSM_PCMFreq (16384)
#define GSM_BlockCount (16)
#define GSM_BlockSamples (320*GSM_BlockCount)
#define GSM_BlockBytes (65*GSM_BlockCount)

gsm GSM_Renew(gsm gsmstate);
void GSM_BlockEncode(gsm gsmstate,s16 *psamples,u8 *pblock);
bool GSM_BlockDecode(gsm gsmstate,u8 *pblock,s16 *psamples);

#endif

