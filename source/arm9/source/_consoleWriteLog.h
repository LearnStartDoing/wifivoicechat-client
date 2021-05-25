
#ifndef _consoleWriteLog_h
#define _consoleWriteLog_h

//#define WriteLogFILELINE

#ifdef WriteLogFILELINE
#define cwl(); _cwl(__FILE__,__LINE__);
#else
#define cwl(); 
#endif

#ifdef __cplusplus
extern "C" {
#endif

void PrfStart(void);
void PrfEnd(int data);

#ifdef __cplusplus
}
#endif

static inline void _cwl(char *file,int line)
{
  char *seek=file;
  
  while(*seek!=0){
    if(*seek=='/') file=seek;
    seek++;
  }
  
  _consolePrintf("%s%d",file,line);
}

#endif
