
void MWinCallBack_RegistCallBack(TWinBody *pwb)
{
  switch(pwb->WinMode){
    case WM_About: {
      pwb->CallBack_Draw=MWinCallAbout_Draw;
      pwb->CallBack_CloseButton=MWinCallAbout_CloseButton;
      pwb->CallBack_MouseDown=MWinCallAbout_MouseDown;
      pwb->CallBack_MouseMove=MWinCallAbout_MouseMove;
      pwb->CallBack_MouseUp=MWinCallAbout_MouseUp;
    } break;
//    case WM_Progress: break;
    case WM_Picture: {
      pwb->CallBack_Draw=MWinCallPicture_Draw;
      pwb->CallBack_CloseButton=MWinCallPicture_CloseButton;
      pwb->CallBack_MouseDown=MWinCallPicture_MouseDown;
      pwb->CallBack_MouseMove=MWinCallPicture_MouseMove;
      pwb->CallBack_MouseUp=MWinCallPicture_MouseUp;
    } break;
    case WM_ItemPalette: {
      pwb->CallBack_Draw=MWinCallItemPalette_Draw;
      pwb->CallBack_CloseButton=MWinCallItemPalette_CloseButton;
      pwb->CallBack_MouseDown=MWinCallItemPalette_MouseDown;
      pwb->CallBack_MouseMove=MWinCallItemPalette_MouseMove;
      pwb->CallBack_MouseUp=MWinCallItemPalette_MouseUp;
    } break;
    case WM_ColorPicker: {
      pwb->CallBack_Draw=MWinCallColorPicker_Draw;
      pwb->CallBack_CloseButton=MWinCallColorPicker_CloseButton;
      pwb->CallBack_MouseDown=MWinCallColorPicker_MouseDown;
      pwb->CallBack_MouseMove=MWinCallColorPicker_MouseMove;
      pwb->CallBack_MouseUp=MWinCallColorPicker_MouseUp;
    } break;
    case WM_UserList: {
      pwb->CallBack_Draw=MWinCallUserList_Draw;
      pwb->CallBack_CloseButton=MWinCallUserList_CloseButton;
      pwb->CallBack_MouseDown=MWinCallUserList_MouseDown;
      pwb->CallBack_MouseMove=MWinCallUserList_MouseMove;
      pwb->CallBack_MouseUp=MWinCallUserList_MouseUp;
    } break;
    case WM_UserInfo: {
      pwb->CallBack_Draw=MWinCallUserInfo_Draw;
      pwb->CallBack_CloseButton=MWinCallUserInfo_CloseButton;
      pwb->CallBack_MouseDown=MWinCallUserInfo_MouseDown;
      pwb->CallBack_MouseMove=MWinCallUserInfo_MouseMove;
      pwb->CallBack_MouseUp=MWinCallUserInfo_MouseUp;
    } break;
    case WM_MicVolume: {
      pwb->CallBack_Draw=MWinCallMicVolume_Draw;
      pwb->CallBack_CloseButton=MWinCallMicVolume_CloseButton;
      pwb->CallBack_MouseDown=MWinCallMicVolume_MouseDown;
      pwb->CallBack_MouseMove=MWinCallMicVolume_MouseMove;
      pwb->CallBack_MouseUp=MWinCallMicVolume_MouseUp;
    } break;
    case WM_RoomNum: {
      pwb->CallBack_Draw=MWinCallRoomNum_Draw;
      pwb->CallBack_CloseButton=MWinCallRoomNum_CloseButton;
      pwb->CallBack_MouseDown=MWinCallRoomNum_MouseDown;
      pwb->CallBack_MouseMove=MWinCallRoomNum_MouseMove;
      pwb->CallBack_MouseUp=MWinCallRoomNum_MouseUp;
    } break;
    case WM_PlayEnv: {
      pwb->CallBack_Draw=MWinCallPlayEnv_Draw;
      pwb->CallBack_CloseButton=MWinCallPlayEnv_CloseButton;
      pwb->CallBack_MouseDown=MWinCallPlayEnv_MouseDown;
      pwb->CallBack_MouseMove=MWinCallPlayEnv_MouseMove;
      pwb->CallBack_MouseUp=MWinCallPlayEnv_MouseUp;
    } break;
    default: {
      _consolePrintf("MWinCallBack_RegistCallBack unknown WinMode=%d\n",pwb->WinMode);
      ShowLogHalt();
    } break;
  }
}

