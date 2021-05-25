
#ifndef cglscreen_h
#define cglscreen_h

#include <stdlib.h>
#include <NDS.h>

#include "cglcanvas.h"

enum EScrMainID {ScrMainID_View,ScrMainID_Back};

class CglScreenMain
{
  u16 *VRAMBufArray[2];
  u32 BackVRAMPage;
  bool WideFlag;
  CglScreenMain(const CglScreenMain&);
  CglScreenMain& operator=(const CglScreenMain&);
public:
  CglCanvas *pCanvas;
  CglScreenMain(void);
  ~CglScreenMain(void);
  void Flip(const bool ShowFlag);
  u16* GetVRAMBuf(EScrMainID ScrMainID) const;
  void SetTargetPage(EScrMainID ScrMainID);
  void SetBlendLevel(const int BlendLevel);
  void SetBlendLevelManual(const int BlendLevelBack,const int BlendLevelView);
  void CopyFullViewToBack(void);
  void CopyFullBackToView(void);
  void SetWideFlag(bool w);
  bool GetWideFlag(void);
  void SetViewSize(int w,int h);
  void SetViewport(int x,int y,float fx,float fy);
};

class CglScreenMainOverlay
{
  CglScreenMainOverlay(const CglScreenMainOverlay&);
  CglScreenMainOverlay& operator=(const CglScreenMainOverlay&);
public:
  CglCanvas *pCanvas;
  CglScreenMainOverlay(void);
  ~CglScreenMainOverlay(void);
  u16* GetVRAMBuf(void) const;
  void SetVisible(bool Visible);
  void SetAlpha(u16 alpha);
};

class CglScreenSub
{
  u32 FlipMode;
  CglScreenSub(const CglScreenSub&);
  CglScreenSub& operator=(const CglScreenSub&);
public:
  CglCanvas *pCanvas;
  CglScreenSub(void);
  ~CglScreenSub(void);
  u16* GetVRAMBuf(void) const;
  void SetFlipMode(u32 _FlipMode);
};

#endif

