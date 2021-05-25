
static s32 Global_LastY;

enum EPenItem {PenItem_Line=0,PenItem_TickLine=1,PenItem_DrawBox=2,PenItem_FillBox=3,PenItem_Erase=4,PenItem_ColorPicker=5,PenItem_RoomNum=6,PenItem_Max=7};

static EPenItem PenItemIndex_Current=PenItem_TickLine,PenItemIndex_Cursor=PenItem_Max;

#define PlayEnv_Max (3)
static u32 PlayEnv_Cursor=PlayEnv_Max;

static void Global_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  y/=FontProHeight/2;
  
  Global_LastY=y;
}

static void Global_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  if(CanIgnore==true) return;
  
  y/=FontProHeight/2;
  
  MWin_SetSBarVPos(WinIndex,MWin_GetSBarVPos(WinIndex)-(y-Global_LastY));
  MWin_DrawSBarVH(WinIndex);
  MWin_DrawClient(WinIndex);
  MWin_TransWindow(WinIndex);
  
  Global_LastY=y;
}

static void Global_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  y/=FontProHeight/2;
  
  Global_LastY=0;
}

static void MWinCallAbout_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  u32 TopIndex=MWin_GetSBarVPos(WM_About);
  
  s32 BufWidth=pwb->Rect.w;
  u16 col=MWC_ClientText;
  
  s32 wLimit=pwb->ClientRect.x+pwb->ClientRect.w;
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  for(s32 y=0;y<pwb->ClientRect.h/FontProHeight;y++){
    s32 idx=TopIndex+y;
    char *str;
    
    switch(idx){
      case 0: str=ROMVERSION; break;
      case 1: str=ROMDATE; break;
      case 2: str=ROMAUTHOR1; break;
      case 3: str=ROMAUTHOR2; break;
      case 4: str=ROMENV; break;
      
      default: str=""; break;
    }
    
    VRAM_WriteStringRect(pBuf,BufWidth,wLimit,ofsx+4,ofsy+(y*FontProHeight),str,col);
  }
}

static void MWinCallPicture_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  if(pwb->WinBuf==NULL) return;
  
  u32 ofsx,ofsy;
  ofsx=MWin_GetSBarHPos(WM_Picture);
  ofsy=MWin_GetSBarVPos(WM_Picture);
  
  ofsx=-ofsx;
  ofsy=-ofsy;
  
  REG_IME=0;
  swiWaitForVBlank();
  pScreenMain->SetViewport(ofsx,ofsy,1,1);
  REG_IME=1;
}

static void MWinCallItemPalette_Draw(u32 WinIndex)
{
  static CglB15 *pglB15=NULL;
  
  if(pglB15==NULL){
    u8 *b15data;
    u32 b15size;
    Shell_ReadFile("ItemPalette.b15",(void**)&b15data,(int*)&b15size);
    if(b15data==NULL){
      _consolePrintf("not found.\n");
      ShowLogHalt();
    }
    pglB15=new CglB15(b15data,b15size);
    free(b15data); b15data=NULL;
  }
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  
  s32 BufWidth=pwb->Rect.w;
  
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  {
    u16 c=0 | BIT15;
    switch(PenItemIndex_Current){
      case PenItem_Line: c=PenColor16_Line; break;
      case PenItem_TickLine: c=PenColor16_TickLine; break;
      case PenItem_DrawBox: c=PenColor16_DrawBox; break;
      case PenItem_FillBox: c=PenColor16_FillBox; break;
      case PenItem_Erase: break;
      case PenItem_ColorPicker: break;
      case PenItem_RoomNum: break;
      case PenItem_Max: break;
    }
    VRAM_SetCharLineRect(pBuf,BufWidth,ofsx+(16*PenItem_ColorPicker),ofsy+0,16,16,c);
  }
  
  u16 cols[PenItem_Max];
  
  for(u32 idx=0;idx<PenItem_Max;idx++){
    u16 col=0;
    if(idx==(u32)PenItemIndex_Current) col=MWC_FileSelectPlay;
    if(idx==(u32)PenItemIndex_Cursor) col=MWC_FileSelectCursor;
    if(col!=0){
      VRAM_SetCharLineRect(pBuf,BufWidth,ofsx+(16*idx),ofsy+0,16,16,col);
      col=MWC_FileSelectBG;
      }else{
      col=MWC_FileSelectText;
    }
    cols[idx]=col;
  }
  
  for(u32 y=0;y<(u32)pwb->ClientRect.y;y++){
    u16 *psrcbuf=pglB15->pCanvas->GetScanLine(y);
    u16 *pdstbuf=pwb->pCanvas->GetScanLine(ofsy+y);
    pdstbuf+=ofsx;
    for(u32 x=0;x<(u32)pwb->ClientRect.w;x++){
      if(psrcbuf[x]==0x8000) pdstbuf[x]=cols[x/16];
    }
  }
}

static u32 ColorPicker_Bright=31;

static inline u16 ColorBright(const u16 col,const int Bright)
{
  u32 r,g,b;
  
  r=((col>>0)&0x1f)*Bright/32;
  g=((col>>5)&0x1f)*Bright/32;
  b=((col>>10)&0x1f)*Bright/32;
  
  return(RGB15(r,g,b)|BIT15);
}

static void MWinCallColorPicker_Draw(u32 WinIndex)
{
  static CglB15 *pglB15=NULL;
  
  if(pglB15==NULL){
    u8 *b15data;
    u32 b15size;
    Shell_ReadFile("ColPickB128.b15",(void**)&b15data,(int*)&b15size);
    if(b15data==NULL){
      _consolePrintf("not found.\n");
      ShowLogHalt();
    }
    pglB15=new CglB15(b15data,b15size);
    free(b15data); b15data=NULL;
  }
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  if(pwb->WinBuf==NULL) return;
  
  u32 w=pglB15->GetWidth();
  u32 h=pglB15->GetHeight()-16;
  u32 br=ColorPicker_Bright;
  
  pglB15->BitBlt(pwb->pCanvas,ofsx,ofsy+(h-16),pglB15->GetWidth(),pglB15->GetHeight(),0,h-16);
  
  for(u32 y=0;y<h;y++){
    u16 *psrcbuf=pglB15->pCanvas->GetScanLine(y);
    u16 *pdstbuf=pwb->pCanvas->GetScanLine(ofsy+y);
    pdstbuf+=ofsx;
    for(u32 x=0;x<w;x++){
      pdstbuf[x]=ColorBright(psrcbuf[x],br);
    }
  }
}

static void MWinCallUserList_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  u32 TopIndex=MWin_GetSBarVPos(WM_UserList);
  
  s32 BufWidth=pwb->Rect.w;
  u16 col=MWC_ClientText;
  
  s32 wLimit=pwb->ClientRect.x+pwb->ClientRect.w;
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  TPenPacket_UserList *packet=&PenPacket_UserList;
  
  for(s32 y=0;y<pwb->ClientRect.h/FontProHeight;y++){
    s32 idx=TopIndex+y;
    s32 tx=ofsx+4;
    s32 ty=ofsy+(y*FontProHeight);
    if(packet->UserCount<=idx){
      VRAM_WriteStringRect(pBuf,BufWidth,wLimit,tx,ty,"empty...",col);
      }else{
      TPenPacket_UserList_User *puser=&packet->Users[idx];
      char *pstr=Get_ConUserLngStr(EConUserLng(puser->iUserLng));
      VRAM_WriteStringRect(pBuf,BufWidth,wLimit,tx,ty,pstr,col);
      tx+=VRAM_GetStringWidth(pstr)+2;
      VRAM_WriteStringRectW(pBuf,BufWidth,wLimit,tx,ty,puser->UserName,col);
    }
  }
}

static u32 ShowUserIndex=(u32)-1;

static void MWinCallUserInfo_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  s32 BufWidth=pwb->Rect.w;
  u16 col=MWC_ClientText;
  
  s32 wLimit=pwb->ClientRect.x+pwb->ClientRect.w;
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  TPenPacket_UserList *packet=&PenPacket_UserList;
  
  if(packet->UserCount<=ShowUserIndex) return;
  
  TPenPacket_UserList_User *puser=&packet->Users[ShowUserIndex];
  
  s32 tx=ofsx+4;
  s32 ty=ofsy;
  
  {
    char tmp[256];
    snprintf(tmp,256,"Language:%s",Get_ConUserLngStr(EConUserLng(puser->iUserLng)));
    VRAM_WriteStringRect(pBuf,BufWidth,wLimit,tx,ty,tmp,col);
    ty+=FontProHeight;
  }
  
  VRAM_WriteStringRectW(pBuf,BufWidth,wLimit,tx,ty,puser->UserName,col);
  ty+=FontProHeight;
  VRAM_WriteStringRectW(pBuf,BufWidth,wLimit,tx,ty,puser->UserMsg,col);
  ty+=FontProHeight;
  
  {
    char tmp[256];
    u32 sec=puser->UserOnTime;
    snprintf(tmp,256,"Time %02d:%02d:%02d",sec/60,(sec/60)%60,sec%60);
    VRAM_WriteStringRect(pBuf,BufWidth,wLimit,tx,ty,tmp,col);
    ty+=FontProHeight;
  }
  
  {
    char tmp[256];
    u8 *pmac=(u8*)&puser->UserMacAddr;
    snprintf(tmp,256,"MAC%02X:%02X:%02X:%02X:%02X:%02X",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
    VRAM_WriteStringRect(pBuf,BufWidth,wLimit,tx,ty,tmp,col);
    ty+=FontProHeight;
  }
}

static void MWinCallMicVolume_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  
  extern u32 MicVolume;
  u32 total=MicVolume;
  
  {
    u16 col=RGB15(0,31,0)|BIT15;
    if(total<MicVolume_Min) col=RGB15(0,0,31)|BIT15;
    if(MicVolume_Max<total) col=RGB15(31,0,0)|BIT15;
    pwb->pCanvas->SetColor(col);
  }
  
  TRect Rect=pwb->ClientRect;
  
  u32 h=Rect.h;
  u32 lev=(total/0x100)*h/(MicVolume_Max/0x100);
  
  if(h<=lev) lev=h-1;
  
  if(2<=lev){
    pwb->pCanvas->FillBox(Rect.x+0,Rect.y+(h-lev),Rect.w,lev-1);
    pwb->pCanvas->SetColor(RGB15(0,0,0)|BIT15);
    pwb->pCanvas->FillBox(Rect.x+0,Rect.y+0,Rect.w,h-lev);
  }
}

#define RoomNum_itemx (0)
#define RoomNum_itemy (2)
#define RoomNum_itemw (24)
#define RoomNum_itemh (FontProHeight+2)
#define RoomNum_itemxcnt (10)
#define RoomNum_itemycnt (12)
#define RoomNum_RoomCount (10000)

u32 RoomNum_CurrentNum=0;
u32 RoomNum_RequestNum=(u32)-1;
u32 RoomNum_SelectNum=(u32)-1;

static void MWinCallRoomNum_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  u32 TopIndex=MWin_GetSBarVPos(WM_RoomNum);
  
  s32 BufWidth=pwb->Rect.w;
  
  s32 wLimit=pwb->ClientRect.x+pwb->ClientRect.w;
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  for(s32 y=0;y<RoomNum_itemycnt;y++){
    for(s32 x=0;x<RoomNum_itemxcnt;x++){
      u32 idx=((TopIndex+y)*RoomNum_itemxcnt)+x;
      u32 tx=ofsx+RoomNum_itemx+(x*RoomNum_itemw);
      u32 ty=ofsy+RoomNum_itemy+(y*RoomNum_itemh);
      
      u16 col=0;
      if(idx==RoomNum_CurrentNum) col=MWC_FileSelectPlay;
      if(idx==RoomNum_SelectNum) col=MWC_FileSelectCursor;
      if(col!=0){
        VRAM_SetCharLineRect(pBuf,BufWidth,tx,ty,RoomNum_itemw,RoomNum_itemh,col);
        col=MWC_FileSelectBG;
        }else{
        col=MWC_FileSelectText;
      }
      
      if(idx<RoomNum_RoomCount){
        char str[16];
        char *pstr=str;
        switch(idx){
          case  60: pstr="JPN"; break;
          case  71: pstr="ENG"; break;
          case  82: pstr="FRE"; break;
          case  93: pstr="DEU"; break;
          case 104: pstr="ITA"; break;
          case 115: pstr="ESP"; break;
          default: snprintf(pstr,4,"%03d",idx); break;
        }
        u32 strw=VRAM_GetStringWidth(pstr);
        VRAM_WriteStringRectUTF8(pBuf,BufWidth,wLimit,tx+((RoomNum_itemw-strw)/2),ty,pstr,col);
      }
    }
  }
}

static void MWinCallPlayEnv_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  s32 BufWidth=pwb->Rect.w;
  
  s32 wLimit=pwb->ClientRect.x+pwb->ClientRect.w;
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=&pwb->WinBuf[0];
  
  if(pwb->WinBuf==NULL) return;
  
  for(u32 y=0;y<PlayEnv_Max;y++){
    u32 tx=ofsx;
    u32 ty=ofsy+(y*FontProHeight);
    u32 tw=pwb->ClientRect.w;
    u32 th=FontProHeight;
    
    u16 col=0;
    if(y==IPCEX->PlayEnv) col=MWC_FileSelectPlay;
    if(y==PlayEnv_Cursor) col=MWC_FileSelectCursor;
    if(col!=0){
      VRAM_SetCharLineRect(pBuf,BufWidth,tx,ty,tw,th,col);
      col=MWC_FileSelectBG;
      }else{
      col=MWC_FileSelectText;
    }
    
    char *pstreng=NULL,*pstrjpn=NULL;
    switch(y){
      case PlayEnv_Speaker: {
        pstreng="Speaker";
        pstrjpn="内蔵スピーカー";
      } break;
      case PlayEnv_Headphone1: {
        pstreng="Headphone 1";
        pstrjpn="ヘッドフォン 1";
      } break;
      case PlayEnv_Headphone2: {
        pstreng="Headphone 2";
        pstrjpn="ヘッドフォン 2";
      } break;
      default: {
        pstreng="NULL";
        pstrjpn="NULL";
      } break;
    }
    char *pstr;
    switch(GetLang()){
      case EL_ENG: pstr=pstreng; break;
      case EL_JPN: pstr=pstrjpn; break;
      default: pstr=pstreng; break;
    }
    u32 strw=VRAM_GetStringWidthUTF8(pstr);
    VRAM_WriteStringRectUTF8(pBuf,BufWidth,wLimit,tx+((tw-strw)/2),ty,pstr,col);
  }
}

// MouseCallback

static void MWinCallAbout_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseDown(WinIndex,x,y);
}

static void MWinCallAbout_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  Global_MouseMove(WinIndex,x,y,CanIgnore);
}

static void MWinCallAbout_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseUp(WinIndex,x,y);
}

static s32 Picture_LastX,Picture_LastY;

static void MWinCallPicture_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  Picture_LastX=x;
  Picture_LastY=y;
  
  if(PenItemIndex_Current==PenItem_Erase){
    void MWinCallPicture_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore);
    MWinCallPicture_MouseMove(WinIndex,x,y,false);
  }
}

static void MWinCallPicture_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  u32 ofsx,ofsy;
  ofsx=MWin_GetSBarHPos(WM_Picture);
  ofsy=MWin_GetSBarVPos(WM_Picture);
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  
  ofsx+=pwb->ClientRect.x+pwb->Rect.x;
  ofsy+=pwb->ClientRect.y+pwb->Rect.y;
  
  switch(PenItemIndex_Current){
    case PenItem_Line: {
      TPenPacket_Line packet;
      packet.Color=PenColor16_Line;
      packet.lastx=ofsx+Picture_LastX;
      packet.lasty=ofsy+Picture_LastY;
      packet.curx=ofsx+x;
      packet.cury=ofsy+y;
      SendPacketToServer_AddPacket(PenPacketType_Line,&packet,TPenPacket_Line_Size);
      PenPacket_Line_Proc(&packet);
      Picture_LastX=x;
      Picture_LastY=y;
    } break;
    case PenItem_TickLine: {
      TPenPacket_TickLine packet;
      packet.Color=PenColor16_TickLine;
      packet.lastx=ofsx+Picture_LastX;
      packet.lasty=ofsy+Picture_LastY;
      packet.curx=ofsx+x;
      packet.cury=ofsy+y;
      SendPacketToServer_AddPacket(PenPacketType_TickLine,&packet,TPenPacket_TickLine_Size);
      PenPacket_TickLine_Proc(&packet);
      Picture_LastX=x;
      Picture_LastY=y;
    } break;
    case PenItem_DrawBox: {
    } break;
    case PenItem_FillBox: {
    } break;
    case PenItem_Erase: {
      TPenPacket_Erase packet;
      packet.curx=ofsx+x;
      packet.cury=ofsy+y;
      SendPacketToServer_AddPacket(PenPacketType_Erase,&packet,TPenPacket_Erase_Size);
      PenPacket_Erase_Proc(&packet);
    } break;
    case PenItem_ColorPicker: break;
    case PenItem_RoomNum: break;
    case PenItem_Max: break;
  }
}

static void MWinCallPicture_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  u32 ofsx,ofsy;
  ofsx=MWin_GetSBarHPos(WM_Picture);
  ofsy=MWin_GetSBarVPos(WM_Picture);
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  
  ofsx+=pwb->ClientRect.x+pwb->Rect.x;
  ofsy+=pwb->ClientRect.y+pwb->Rect.y;
  
  switch(PenItemIndex_Current){
    case PenItem_Line: {
    } break;
    case PenItem_TickLine: {
    } break;
    case PenItem_DrawBox: {
      TPenPacket_DrawBox packet;
      packet.Color=PenColor16_DrawBox;
      packet.lastx=ofsx+Picture_LastX;
      packet.lasty=ofsy+Picture_LastY;
      packet.curx=ofsx+x;
      packet.cury=ofsy+y;
      SendPacketToServer_AddPacket(PenPacketType_DrawBox,&packet,TPenPacket_DrawBox_Size);
      PenPacket_DrawBox_Proc(&packet);
    } break;
    case PenItem_FillBox: {
      TPenPacket_FillBox packet;
      packet.Color=PenColor16_FillBox;
      packet.lastx=ofsx+Picture_LastX;
      packet.lasty=ofsy+Picture_LastY;
      packet.curx=ofsx+x;
      packet.cury=ofsy+y;
      SendPacketToServer_AddPacket(PenPacketType_FillBox,&packet,TPenPacket_FillBox_Size);
      PenPacket_FillBox_Proc(&packet);
    } break;
    case PenItem_Erase: {
    } break;
    case PenItem_ColorPicker: break;
    case PenItem_RoomNum: break;
    case PenItem_Max: break;
  }
  
  Picture_LastX=0;
  Picture_LastY=0;
}

static void MWinCallItemPalette_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  void MWinCallItemPalette_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore);
  MWinCallItemPalette_MouseMove(WinIndex,x,y,false);
}

static void MWinCallItemPalette_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  if((0<=y)&&(y<16)){
    s32 idx=x/16;
    if((0<=idx)&&(idx<PenItem_Max)){
      PenItemIndex_Cursor=(EPenItem)idx;
      }else{
      PenItemIndex_Cursor=PenItem_Max;
    }
    }else{
    PenItemIndex_Cursor=PenItem_Max;
  }
  
  if(MWin_GetVisible(WM_ItemPalette)==true){
    MWin_DrawClient(WM_ItemPalette);
    MWin_TransWindow(WM_ItemPalette);
  }
}

static void MWinCallItemPalette_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  u32 idx=(u32)PenItemIndex_Cursor;
  
  if((0<=idx)&&(idx<PenItem_Max)){
    Sound_Start("execute.wav");
    if((EPenItem)idx==PenItem_ColorPicker){
      MWin_SetVisible(WM_ColorPicker,true);
      u32 WinIndex=MWin_SetActive(WM_ColorPicker);
      MWin_RefreshScreenMask();
      if(WinIndex!=(u32)-1) MWin_RefreshWindow(WinIndex);
      MWin_RefreshWindow(WM_ColorPicker);
      idx=PenItem_Max;
    }
    if((EPenItem)idx==PenItem_RoomNum){
      MWin_SetVisible(WM_RoomNum,true);
      u32 WinIndex=MWin_SetActive(WM_RoomNum);
      MWin_RefreshScreenMask();
      if(WinIndex!=(u32)-1) MWin_RefreshWindow(WinIndex);
      MWin_RefreshWindow(WM_RoomNum);
      idx=PenItem_Max;
    }
  }
  
  if(idx!=PenItem_Max) PenItemIndex_Current=(EPenItem)idx;
  PenItemIndex_Cursor=PenItem_Max;
  
  if(MWin_GetVisible(WM_ItemPalette)==true){
    MWin_DrawClient(WM_ItemPalette);
    MWin_TransWindow(WM_ItemPalette);
  }
}

static void MWinCallColorPicker_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  void MWinCallColorPicker_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore);
  MWinCallColorPicker_MouseMove(WinIndex,x,y,false);
}

static void MWinCallColorPicker_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  if(CanIgnore==true) return;
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  if(pwb==NULL) return;
  
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  if(x<0) x=0;
  if(pwb->ClientRect.w<=x) x=pwb->ClientRect.w-1;
  if(y<0) y=0;
  if(pwb->ClientRect.h<=y) y=pwb->ClientRect.h-1;
  
  u16 col=pwb->pCanvas->GetPixel(ofsx+x,ofsy+y);
  
  if(y<(128-16)){
    switch(PenItemIndex_Current){
      case PenItem_Line: PenColor16_Line=col; break;
      case PenItem_TickLine: PenColor16_TickLine=col; break;
      case PenItem_DrawBox: PenColor16_DrawBox=col; break;
      case PenItem_FillBox: PenColor16_FillBox=col; break;
      case PenItem_Erase: break;
      case PenItem_ColorPicker: break;
      case PenItem_RoomNum: break;
      case PenItem_Max: break;
    }
    MWin_DrawClient(WM_ItemPalette);
    MWin_TransWindow(WM_ItemPalette);
    }else{
    ColorPicker_Bright=col&0x1f;
    MWin_DrawClient(WM_ColorPicker);
    MWin_TransWindow(WM_ColorPicker);
  }
}

static void MWinCallColorPicker_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  MWinCallColorPicker_MouseDown(WinIndex,x,y);
  Sound_Start("execute.wav");
}

static void MWinCallUserList_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseDown(WinIndex,x,y);
}

static void MWinCallUserList_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  Global_MouseMove(WinIndex,x,y,CanIgnore);
}

static void MWinCallUserList_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseUp(WinIndex,x,y);
  
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  if(pwb==NULL) return;
  
  if((y<0)||(pwb->ClientRect.h<=y)) return;
  
  u32 idx=MWin_GetSBarVPos(WinIndex);
  idx+=y/FontProHeight;
  
  TPenPacket_UserList *packet=&PenPacket_UserList;
  if(packet->UserCount<=idx) return;
  ShowUserIndex=idx;
  
  if(MWin_GetVisible(WM_UserInfo)==false){
    MWin_SetVisible(WM_UserInfo,true);
    u32 NewWinIndex=MWin_SetActive(WM_UserInfo);
    MWin_RefreshScreenMask();
    if(NewWinIndex!=(u32)-1) MWin_RefreshWindow(NewWinIndex);
  }
  MWin_RefreshWindow(WM_UserInfo);
  
  Sound_Start("execute.wav");
}

static void MWinCallUserInfo_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseDown(WinIndex,x,y);
}

static void MWinCallUserInfo_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  Global_MouseMove(WinIndex,x,y,CanIgnore);
}

static void MWinCallUserInfo_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  Global_MouseUp(WinIndex,x,y);
}

static void MWinCallMicVolume_MouseDown(u32 WinIndex,s32 x,s32 y)
{
}

static void MWinCallMicVolume_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
}

static void MWinCallMicVolume_MouseUp(u32 WinIndex,s32 x,s32 y)
{
}

static void MWinCallRoomNum_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  if(MWin_GetVisible(WM_Progress)==true) return;
  
  u32 TopIndex=MWin_GetSBarVPos(WinIndex);
  
  RoomNum_SelectNum=(u32)-1;
  
  x-=RoomNum_itemx;
  y-=RoomNum_itemy;
  if((x<0)||(y<0)) return;
  
  u32 xidx=x/RoomNum_itemw;
  u32 yidx=y/RoomNum_itemh;
  if((RoomNum_itemxcnt<=xidx)||(RoomNum_itemycnt<=yidx)) return;
  
  u32 idx=((TopIndex+yidx)*RoomNum_itemxcnt)+xidx;
  
  RoomNum_SelectNum=idx;
  
  MWin_DrawClient(WinIndex);
  MWin_TransWindow(WinIndex);
}

static void MWinCallRoomNum_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  if(CanIgnore==true) return;
  if(MWin_GetVisible(WM_Progress)==true) return;
  
  MWinCallRoomNum_MouseDown(WinIndex,x,y);
}

static void MWinCallRoomNum_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  if(MWin_GetVisible(WM_Progress)==true) return;
  
  u32 idx=(u32)RoomNum_SelectNum;
  
  if((0<=idx)&&(idx<RoomNum_RoomCount)){
    RoomNum_RequestNum=idx;
//    RoomNum_SelectNum=(u32)-1;
  }
  
  MWin_DrawClient(WinIndex);
  MWin_TransWindow(WinIndex);
}

static void MWinCallPlayEnv_MouseDown(u32 WinIndex,s32 x,s32 y)
{
  if((0<=y)&&(y<(FontProHeight*PlayEnv_Max))){
    s32 idx=y/FontProHeight;
    if((0<=idx)&&(idx<PlayEnv_Max)){
      PlayEnv_Cursor=idx;
      }else{
      PlayEnv_Cursor=PlayEnv_Max;
    }
    }else{
    PlayEnv_Cursor=PlayEnv_Max;
  }
  
  if(MWin_GetVisible(WinIndex)==true){
    MWin_DrawClient(WinIndex);
    MWin_TransWindow(WinIndex);
  }
}

static void MWinCallPlayEnv_MouseMove(u32 WinIndex,s32 x,s32 y,bool CanIgnore)
{
  if(CanIgnore==true) return;
  MWinCallPlayEnv_MouseDown(WinIndex,x,y);
}

static void MWinCallPlayEnv_MouseUp(u32 WinIndex,s32 x,s32 y)
{
  u32 idx=PlayEnv_Cursor;
  
  if((0<=idx)&&(idx<3)){
    Sound_Start("execute.wav");
    IPCEX->PlayEnv=idx;
  }
  
  PlayEnv_Cursor=PlayEnv_Max;
  
  if(MWin_GetVisible(WM_PlayEnv)==true){
    MWin_DrawClient(WM_PlayEnv);
    MWin_TransWindow(WM_PlayEnv);
  }
}
