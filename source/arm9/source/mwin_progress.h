
s32 MWin_ProgressActiveWinIndex=0;
s32 MWin_ProgressMax=0;
s32 MWin_ProgressPosition=0;

void MWin_ProgressShow(char *TitleStr_ENG,char *TitleStr_JPN,s32 _Max)
{
  MWin_ProgressMax=_Max;
  MWin_ProgressPosition=0;
  
  u32 WinIndex=WM_Progress;
  
  TWinBody *pwb=&WinBody[WinIndex];
  
  switch(GetLang()){
    case EL_ENG: snprintf(pwb->TitleStr,128,"%s",TitleStr_ENG); break;
    case EL_JPN: snprintf(pwb->TitleStr,128,"%s",TitleStr_JPN); break;
    default: snprintf(pwb->TitleStr,128,"%s",TitleStr_ENG); break;
  }
  
  MWin_SetVisible(WinIndex,true);
  MWin_ProgressActiveWinIndex=MWin_SetActive(WinIndex);
  MWin_RefreshScreenMask();
  MWin_RefreshWindow(WinIndex);
}

void MWin_ProgressSetPos(s32 _Position)
{
  MWin_ProgressPosition=_Position;
  
  u32 WinIndex=WM_Progress;
  
  MWin_DrawClient(WinIndex);
  MWin_TransWindow(WinIndex);
}

void MWin_ProgressHide(void)
{
  MWin_ProgressMax=0;
  MWin_ProgressPosition=0;
  
  u32 WinIndex=WM_Progress;
  
  MWin_SetVisible(WinIndex,false);
  MWin_SetActive(MWin_ProgressActiveWinIndex);
  MWin_RefreshScreenMask();
  
  MWin_ClearDesktop();
  for(u32 cnt=0;cnt<WinBodyCount;cnt++){
    MWin_TransWindow(cnt);
  }
}

void MWinCallProgress_Draw(u32 WinIndex)
{
  TWinBody* pwb=MWin_GetWinBody(WinIndex);
  TSkinBM *pSkinBM=&SkinBM;
  
  s32 w=pwb->ClientRect.w;
  s32 h=pwb->ClientRect.h;
  
  s32 BufWidth=pwb->Rect.w;
  
  u32 ofsx=pwb->ClientRect.x;
  u32 ofsy=pwb->ClientRect.y;
  
  u16 *pBuf=pwb->WinBuf;
  if(pBuf==NULL) return;
  pBuf+=ofsx+(ofsy*BufWidth);
  
  float fv;
  s32 v;
  
  fv=(float)MWin_ProgressPosition/MWin_ProgressMax;
  v=(s32)(fv*w);
  if(v<0) v=0;
  if(w<=v) v=w-1;
  
  const TSkinBM *psbm=pSkinBM;
  
  u16 *pPrgBarABM=psbm->pPrgBarABM;
  u16 *pPrgBarDBM=psbm->pPrgBarDBM;
  u32 PrgBarBM_W=psbm->PrgBarBM_W;
  
  if((pPrgBarABM!=NULL)&&(pPrgBarDBM!=NULL)){
    for(s32 y=0;y<h;y++){
      MemCopy16DMA3(&pPrgBarABM[0],&pBuf[0],v*2);
      MemCopy16DMA3(&pPrgBarDBM[v],&pBuf[v],(w-v)*2);
      pBuf+=BufWidth;
      pPrgBarABM+=PrgBarBM_W;
      pPrgBarDBM+=PrgBarBM_W;
    }
    
    }else{
    u16 colLeft=MWC_ProgressEnd;
    u16 colRight=MWC_ProgressLast;
    
    for(s32 y=0;y<h;y++){
      MemSet16DMA3(colLeft,&pBuf[0],v*2);
      MemSet16DMA3(colRight,&pBuf[v],(w-v)*2);
      pBuf+=BufWidth;
    }
  }
}

