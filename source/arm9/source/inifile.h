
#ifndef inifile_h
#define inifile_h

#include "rect.h"

enum EClosedSholderButton {ECSB_Disabled=0,ECSB_Flexible=1,ECSB_AlwaysDisabled=2,ECSB_Enabled=3};
enum EScreenFlip {ESF_Normal=0,ESF_Flip=1,ESF_VFlip=2,WFS_HFlip=3};
enum EiniSystemWhenPanelClose {EISWPC_BacklightOff=0,EISWPC_DSPowerOff=1,EISWPC_PlayShutdownSound=2};

typedef struct {
} TiniSystem;

typedef struct {
  TiniSystem System;
} TGlobalINI;

extern TGlobalINI GlobalINI;

extern void InitINI(void);
extern void LoadINI(char *inifn);
extern void LoadSkinINI(char *inifn);

#endif


