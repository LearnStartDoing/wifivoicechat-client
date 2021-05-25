
#include <NDS.h>
#include "_const.h"

void StrCopy(const char *src,char *dst)
{
  if(dst==0) return;
  if(src==0){
    dst[0]=0;
    return;
  }
  
  while(*src!=0){
    *dst=*src;
    src++;
    dst++;
  }
  
  *dst=0;
}

bool isStrEqual(const char *s1,const char *s2)
{
  if((s1==0)&&(s2==0)) return(true);
  if((s1==0)||(s2==0)) return(false);
  
  while(*s1==*s2){
    if((*s1==0)||(*s2==0)){
      if((*s1==0)&&(*s2==0)){
        return(true);
        }else{
        return(false);
      }
    }
    s1++;
    s2++;
  }
  return(false);
}

void StrAppend(char *s,const char *add)
{
  if((s==0)||(add==0)) return;
  
  while(*s!=0){
    s++;
  }
  
  while(*add!=0){
    *s=*add;
    s++;
    add++;
  }
  
  *s=0;
}

