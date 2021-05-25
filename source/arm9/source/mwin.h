
#ifndef mwin_h
#define mwin_h

#include <NDS.h>

#include "unicode.h"

#include "rect.h"

#define WM_About (0)
#define WM_Progress (1)
#define WM_Picture (2)
#define WM_ItemPalette (3)
#define WM_ColorPicker (4)
#define WM_UserList (5)
#define WM_UserInfo (6)
#define WM_MicVolume (7)
#define WM_RoomNum (8)
#define WM_PlayEnv (9)
#define WinBodyCount (10)

typedef struct {
  bool Enabled;
  bool isV;
  TRect ClientRect;
  s32 GripMax,GripSize,GripPos;
} TSBar;

enum ESelDisp {SelDisp_Top,SelDisp_Bottom};

typedef struct {
  u32 WinMode;
  ESelDisp SelDisp;
  bool DynamicAllocMem;
  bool Visible;
  bool TopFlag;
  u32 Priority;
  char TitleStr[128];
  TRect Rect;
  CglCanvas *pCanvas;
  TRect ClientRect;
  u16 *WinBuf;
  
  u16 ClientBGColor;
  TSBar SBarV,SBarH;
  bool CloseButton;
  
  void (*CallBack_Draw)(u32 WinIndex);
  void (*CallBack_CloseButton)(u32 WinIndex);
  void (*CallBack_MouseDown)(u32 WinIndex,s32 x,s32 y);
  void (*CallBack_MouseMove)(u32 WinIndex,s32 x,s32 y,bool CanIgnore);
  void (*CallBack_MouseUp)(u32 WinIndex,s32 x,s32 y);
} TWinBody;

#define TitleBarHeight (12+(1*2))

typedef struct {
  u16 *pTitleABM,*pTitleDBM;
  u32 TitleBM_W,TitleBM_H;
  u16 *pClientBM;
  u32 ClientBM_W,ClientBM_H;
  u32 CloseBtnBM_W,CloseBtnBM_H;
  u16 *pCloseBtnBM;
  u16 *pPrgBarABM,*pPrgBarDBM;
  u32 PrgBarBM_W,PrgBarBM_H;
  u16 *pSB_BGBM,*pSB_BodyBM,*pSB_PartBM;
} TSkinBM;

extern void MWin_Init(void);
extern void MWin_Free(void);
extern void MWin_AllRefresh(void);
extern void MWin_AllTrans(void);

extern void MWin_AllocMem(u32 WinIndex);
extern void MWin_FreeMem(u32 WinIndex);

extern u32 MWin_SetActive(u32 WinBodyIndex);
extern u32 MWin_SetActiveTopMost(void);
extern void MWin_RefreshScreenMask(void);
extern void MWin_DrawTitleBar(u32 WinIndex);
extern void MWin_DrawClientFrame(u32 WinIndex);
extern void MWin_DrawClientBG(u32 WinIndex);
extern void MWin_DrawClient(u32 WinIndex);
extern void MWin_DrawSBarVH(u32 WinIndex);
extern void MWin_TransWindow(u32 WinIndex);
extern void MWin_ClearDesktop(void);
extern void MWin_RefreshWindow(u32 WinIndex);

extern void MWin_SetVisible(u32 WinIndex,bool Visible);
extern bool MWin_GetVisible(u32 WinIndex);

extern void MWin_SetWindowTitle(u32 WinIndex,char *TitleStr);

extern TWinBody* MWin_GetWinBody(u32 WinIndex);

extern u32 MWin_GetWindowIndexFromPos(s32 x,s32 y);
extern u32 MWin_GetTopWinIndex(void);

extern s32 MWin_GetClientWidth(u32 WinIndex);
extern s32 MWin_GetClientHeight(u32 WinIndex);

extern s32 MWin_ProgressMax;
extern s32 MWin_ProgressPosition;

extern void MWin_ProgressShow(char *TitleStr_ENG,char *TitleStr_JPN,s32 _Max);
extern void MWin_ProgressSetPos(s32 _Position);
extern void MWin_ProgressHide(void);

extern void MWin_SetSBarV(u32 WinIndex,s32 GripMax,s32 GripSize,s32 GripPos);
extern void MWin_SetSBarH(u32 WinIndex,s32 GripMax,s32 GripSize,s32 GripPos);
extern void MWin_SetSBarVPos(u32 WinIndex,s32 GripPos);
extern void MWin_SetSBarHPos(u32 WinIndex,s32 GripPos);
extern s32 MWin_GetSBarVPos(u32 WinIndex);
extern s32 MWin_GetSBarHPos(u32 WinIndex);

extern void MWin_MouseDown(bool DispSwap,s32 x,s32 y);
extern void MWin_MouseMove(bool DispSwap,s32 x,s32 y,bool CanIgnore);
extern void MWin_MouseUp(bool DispSwap);

extern void MWinCallBack_RegistCallBack(TWinBody *pwb);

#endif

