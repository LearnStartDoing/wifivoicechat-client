
#ifndef __g721_h
#define __g721_h

#include "g721lib/g721lib.h"

typedef struct g721_state *g721;

#define g721_PCMFreq (16384)
#define g721_BlockBytes (1300*1)
#define g721_BlockSamples (g721_BlockBytes*2)

#define g721_DivCount (4)
#define g721_DivBytes ((1300*1)/g721_DivCount)
#define g721_DivSamples ((g721_BlockBytes*2)/g721_DivCount)

void g721_Renew(g721 g721state);
void g721_DivEncode(g721 g721state,s16 *psamples,u8 *pblock);
bool g721_DivDecode(g721 g721state,u8 *pblock,s16 *psamples);

#endif

