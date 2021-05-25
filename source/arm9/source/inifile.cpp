
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"

#include "inifile.h"
#include "strtool.h"
#include "memtool.h"

#include "mwin_color.h"

TGlobalINI GlobalINI;

void InitINI(void)
{
  {
    TiniSystem *System=&GlobalINI.System;
    
  }
  
}

static char section[128];
static u32 readline;

static void readsection(char *str)
{
  str++;
  
  u32 ofs;
  
  ofs=0;
  while(*str!=']'){
    if((128<=ofs)||(*str==0)){
      _consolePrintf("line%d error.\nThe section name doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    section[ofs]=*str;
    str++;
    ofs++;
  }
  section[ofs]=0;
}

static void setColorCoord(u16 *storeitem,const int ivalue,const char *value)
{
  if(ivalue==-1) return;
  
  u32 v=0;
  
  while(*value!=0){
    char c=*value;
    
    if(('0'<=c)&&(c<='9')){
      v<<=4;
      v|=0x00+(c-'0');
    }
    if(('a'<=c)&&(c<='f')){
      v<<=4;
      v|=0x0a+(c-'a');
    }
    if(('A'<=c)&&(c<='F')){
      v<<=4;
      v|=0x0a+(c-'A');
    }
    
    value++;
  }
  
  u32 r,g,b;
  
  r=(v >> 16) & 0xff;
  g=(v >> 8) & 0xff;
  b=(v >> 0) & 0xff;
  
  *storeitem=RGB15(r/8,g/8,b/8) | BIT(15);
}

static void readkey(char *str)
{
  if(section[0]==0){
    _consolePrintf("line%d error.\nThere is a key ahead of the section name.\n",readline);
//    ShowLogHalt();
    return;
  }
  
  char key[128],value[128];
  
  u32 ofs;
  
  ofs=0;
  while(*str!='='){
    if((128<=ofs)||(*str==0)){
      _consolePrintf("line%d error.\nThe key name doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    key[ofs]=*str;
    str++;
    ofs++;
  }
  key[ofs]=0;
  
  str++;
  
  ofs=0;
  while(*str!=0){
    if(128<=ofs){
      _consolePrintf("line%d error.\nThe value doesn't end correctly.\n",readline);
      ShowLogHalt();
    }
    value[ofs]=*str;
    str++;
    ofs++;
  }
  value[ofs]=0;
  
  s32 ivalue=atoi(value);
  bool bvalue;
  
  if(ivalue==0){
    bvalue=false;
    }else{
    bvalue=true;
  }
  
  if(strcmp(section,"System")==0){
    TiniSystem *System=&GlobalINI.System;
    
/*
    if(strcmp(key,"StartPath")==0){
      strcpy(System->StartPath,value);
      return;
    }
*/
    
  }
  
  if(strcmp(section,"SkinInfo")==0){
    if(strcmp(key,"Title")==0) return;
    if(strcmp(key,"Author")==0) return;
    if(strcmp(key,"LastUpdate")==0) return;
  }
  
  if(strcmp(section,"ColorCoordination")==0){
    
    if(strcmp(key,"DesktopBG")==0){
      setColorCoord(&MWC_DesktopBG,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleA_Bright")==0){
      setColorCoord(&MWC_TitleA_Bright,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleA_Dark")==0){
      setColorCoord(&MWC_TitleA_Dark,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleA_BG")==0){
      setColorCoord(&MWC_TitleA_BG,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleA_Text")==0){
      setColorCoord(&MWC_TitleA_Text,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleA_TextShadow")==0){
      setColorCoord(&MWC_TitleA_TextShadow,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleD_Bright")==0){
      setColorCoord(&MWC_TitleD_Bright,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleD_Dark")==0){
      setColorCoord(&MWC_TitleD_Dark,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleD_BG")==0){
      setColorCoord(&MWC_TitleD_BG,ivalue,value);
      return;
    }
    if(strcmp(key,"TitleD_Text")==0){
      setColorCoord(&MWC_TitleD_Text,ivalue,value);
      return;
    }
    if(strcmp(key,"ScrollBar")==0){
      setColorCoord(&MWC_ScrollBar,ivalue,value);
      return;
    }
    if(strcmp(key,"FrameBright")==0){
      setColorCoord(&MWC_FrameBright,ivalue,value);
      return;
    }
    if(strcmp(key,"FrameDark")==0){
      setColorCoord(&MWC_FrameDark,ivalue,value);
      return;
    }
    if(strcmp(key,"ClientBG")==0){
      setColorCoord(&MWC_ClientBG,ivalue,value);
      return;
    }
    if(strcmp(key,"ClientText")==0){
      setColorCoord(&MWC_ClientText,ivalue,value);
      return;
    }
    if(strcmp(key,"FileSelectBG")==0){
      setColorCoord(&MWC_FileSelectBG,ivalue,value);
      return;
    }
    if(strcmp(key,"FileSelectText")==0){
      setColorCoord(&MWC_FileSelectText,ivalue,value);
      return;
    }
    if(strcmp(key,"FileSelectCursor")==0){
      setColorCoord(&MWC_FileSelectCursor,ivalue,value);
      return;
    }
    if(strcmp(key,"FileSelectPlay")==0){
      setColorCoord(&MWC_FileSelectPlay,ivalue,value);
      return;
    }
    if(strcmp(key,"TextViewBG")==0){
      setColorCoord(&MWC_TextViewBG,ivalue,value);
      return;
    }
    if(strcmp(key,"TextViewText")==0){
      setColorCoord(&MWC_TextViewText,ivalue,value);
      return;
    }
    if(strcmp(key,"ProgressEnd")==0){
      setColorCoord(&MWC_ProgressEnd,ivalue,value);
      return;
    }
    if(strcmp(key,"ProgressLast")==0){
      setColorCoord(&MWC_ProgressLast,ivalue,value);
      return;
    }
    
  }
  
  
  _consolePrintf("line%d error.\ncurrent section [%s] unknown key=%s value=%s\n",readline,section,key,value);
//  ShowLogHalt();
}

static void internal_LoadGlobalINI(char *pini,u32 inisize)
{
  section[0]=0;
  readline=0;
  
  u32 iniofs=0;
  
  while(iniofs<inisize){
    
    readline++;
    
    u32 linelen=0;
    
    // Calc Line Length
    {
      char *s=&pini[iniofs];
      
      while(0x20<=*s){
        linelen++;
        s++;
        if(inisize<=(iniofs+linelen)) break;
      }
      *s=0;
    }
    
    if(linelen!=0){
      char c=pini[iniofs];
      if((c==';')||(c=='/')||(c=='!')){
        // comment line
        }else{
        if(c=='['){
          readsection(&pini[iniofs]);
          }else{
          readkey(&pini[iniofs]);
        }
      }
    }
    
    iniofs+=linelen;
    
    // skip NULL,CR,LF
    {
      char *s=&pini[iniofs];
      
      while(*s<0x20){
        iniofs++;
        s++;
        if(inisize<=iniofs) break;
      }
    }
    
  }
}

#include "shell.h"

void LoadINI(char *inifn)
{
  char *pini;
  u32 inisize;
  
  Shell_ReadFile(inifn,(void**)&pini,(int*)&inisize);
  if((pini==NULL)||(inisize==0)) return;
  
  _consolePrintf("load '/shell/%s\n",inifn);
  internal_LoadGlobalINI(pini,inisize);
  
  free(pini); pini=NULL;
}

void LoadSkinINI(char *inifn)
{
  char *pini;
  u32 inisize;
  
  Shell_ReadSkinFile(inifn,(void**)&pini,(int*)&inisize);
  if((pini==NULL)||(inisize==0)) return;
  
  _consolePrintf("load /shell/%s\n",inifn);
  internal_LoadGlobalINI(pini,inisize);
  
  free(pini); pini=NULL;
}

