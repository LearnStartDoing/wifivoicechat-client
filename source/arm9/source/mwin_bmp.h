
#define BI_RGB (0)
#define BI_RLE8 (1)
#define BI_RLE4 (2)
#define BI_Bitfields (3)

typedef struct {
  u8 bfType[2];
  u32 bfSize;
  u16 bfReserved1;
  u16 bfReserved2;
  u32 bfOffset;
  u32 biSize;
  u32 biWidth;
  u32 biHeight;
  u16 biPlanes;
  u16 biBitCount;
  u32 biCopmression;
  u32 biSizeImage;
  u32 biXPixPerMeter;
  u32 biYPixPerMeter;
  u32 biClrUsed;
  u32 biCirImportant;
  u8 *pPalette;
  u8 *pBitmap;
  
  u32 DataWidth;
} TBMPHeader;

static u16 GetVariable16bit(void *pb)
{
  u16 res;
  u8 *pb8=(u8*)pb;
  
  res=(u32)pb8[0] << 0;
  res+=(u32)pb8[1] << 8;
  
  return(res);
}

static u32 GetVariable32bit(void *pb)
{
  u32 res;
  u8 *pb8=(u8*)pb;
  
  res=(u32)pb8[0] << 0;
  res+=(u32)pb8[1] << 8;
  res+=(u32)pb8[2] << 16;
  res+=(u32)pb8[3] << 24;
  
  return(res);
}

static char *BMP_LoadErrorStr;

static bool GetBMPHeader(u8 *pb,TBMPHeader *pBMPHeader)
{
  if(pb==NULL){
    BMP_LoadErrorStr="SourceData Null.";
    return(false);
  }
  if(pBMPHeader==NULL){
    BMP_LoadErrorStr="pBMPHeader Null.";
    return(false);
  }
  
  pBMPHeader->bfType[0]=pb[0];
  pBMPHeader->bfType[1]=pb[1];
  pBMPHeader->bfSize=GetVariable32bit(&pb[2]);
  pBMPHeader->bfReserved1=GetVariable16bit(&pb[6]);
  pBMPHeader->bfReserved2=GetVariable16bit(&pb[8]);
  pBMPHeader->bfOffset=GetVariable32bit(&pb[10]);
  pBMPHeader->biSize=GetVariable32bit(&pb[14+0]);
  pBMPHeader->biWidth=GetVariable32bit(&pb[14+4]);
  pBMPHeader->biHeight=GetVariable32bit(&pb[14+8]);
  pBMPHeader->biPlanes=GetVariable16bit(&pb[14+12]);
  pBMPHeader->biBitCount=GetVariable16bit(&pb[14+14]);
  pBMPHeader->biCopmression=GetVariable32bit(&pb[14+16]);
  pBMPHeader->biSizeImage=GetVariable32bit(&pb[14+20]);
  pBMPHeader->biXPixPerMeter=GetVariable32bit(&pb[14+24]);
  pBMPHeader->biYPixPerMeter=GetVariable32bit(&pb[14+28]);
  pBMPHeader->biClrUsed=GetVariable32bit(&pb[14+32]);
  pBMPHeader->biCirImportant=GetVariable32bit(&pb[14+36]);
  
  pBMPHeader->pPalette=&pb[14+40];
  pBMPHeader->pBitmap=&pb[pBMPHeader->bfOffset];
  
  pBMPHeader->DataWidth=0;
  
  if((pBMPHeader->bfType[0]!='B')||(pBMPHeader->bfType[1]!='M')){
    BMP_LoadErrorStr="Error MagicID!=BM";
    return(false);
  }
  
  if(pBMPHeader->biCopmression!=BI_RGB){
    BMP_LoadErrorStr="Error notsupport Compression";
    return(false);
  }
  
  if(pBMPHeader->biHeight>=0x80000000){
    BMP_LoadErrorStr="Error notsupport OS/2 format";
    return(false);
  }
  
  if(pBMPHeader->biPlanes!=1){
    BMP_LoadErrorStr="Error notsupport Planes!=1";
    return(false);
  }
  
  switch(pBMPHeader->biBitCount){
    case 1:
      BMP_LoadErrorStr="Error notsupport 1bitcolor.";
      return(false);
    case 4:
      BMP_LoadErrorStr="Error notsupport 4bitcolor.";
      return(false);
    case 8:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*1;
      break;
    case 16:
      BMP_LoadErrorStr="Error notsupport 16bitcolor.";
      return(false);
    case 24:
      pBMPHeader->DataWidth=pBMPHeader->biWidth*3;
      break;
    case 32:
      BMP_LoadErrorStr="Error notsupport 32bitcolor.";
      return(false);
    default:
      BMP_LoadErrorStr="Error Unknown xxBitColor.";
      return(false);
  }
  
  if((pBMPHeader->DataWidth&3)!=0){
    pBMPHeader->DataWidth+=4-(pBMPHeader->DataWidth&3);
  }
  
  BMP_LoadErrorStr="";
  return(true);
}

static char bmerrstr1[256],bmerrstr2[256];

static bool intLoadBM(const char *bmpfn,u16 *pbm,const u32 bmw,const u32 bmh)
{
  bmerrstr1[0]=0;
  bmerrstr2[0]=0;
  
  if(pbm==NULL){
    snprintf(bmerrstr1,256,"BitmapMemory is NULL.");
    snprintf(bmerrstr2,256,"The memory is insufficient?");
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    return(false);
  }
  
  u8 *bmdata;
  u32 bmsize;
  
  Shell_ReadSkinFile(bmpfn,(void**)&bmdata,(int*)&bmsize);
  if(bmdata==NULL){
    return(false);
    }else{
//    _consolePrintf("loadskin /shell/%s\n",bmpfn);
  }
  
  TBMPHeader BMPHeader;
  
  if(GetBMPHeader(bmdata,&BMPHeader)==false){
    snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
    snprintf(bmerrstr2,256,"%s",BMP_LoadErrorStr);
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  if((BMPHeader.biWidth==1)&&(BMPHeader.biHeight==1)){
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  if((BMPHeader.biWidth<bmw)||(BMPHeader.biHeight<bmh)){
    snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
    snprintf(bmerrstr2,256,"%d x %dpixel 8 or 24bitcolor NoCompression.",bmw,bmh);
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  u32 gr=0,gg=0,gb=0;
  
#define Gravity(c,cg) { \
  c+=cg; \
  cg=c&7; \
  c=c>>3; \
  if((c&(~0x1f))!=0) c=(c<0) ? 0x00 : 0x1f; \
}

  for(u32 y=0;y<bmh;y++){
    u8 *pSrcBM=&BMPHeader.pBitmap[(BMPHeader.biHeight-1-y)*BMPHeader.DataWidth];
    u16 *pDstBM=&pbm[y*bmw];
    
    switch(BMPHeader.biBitCount){
      case 8: {
        u8 *PaletteTable=BMPHeader.pPalette;
        for(u32 x=0;x<bmw;x++){
          u8 *pal;
          u32 r,g,b;
          
          pal=&PaletteTable[*pSrcBM*4];
          pSrcBM+=1;
          
          b=pal[0];
          g=pal[1];
          r=pal[2];
          
          Gravity(b,gb);
          Gravity(g,gg);
          Gravity(r,gr);
          
          pDstBM[x]=RGB15(r,g,b) | BIT(15);
        }
        break;
      }
      case 24: {
        for(u32 x=0;x<bmw;x++){
          u32 r,g,b;
          
          b=pSrcBM[0];
          g=pSrcBM[1];
          r=pSrcBM[2];
          pSrcBM+=3;
          
          Gravity(b,gb);
          Gravity(g,gg);
          Gravity(r,gr);
          
          pDstBM[x]=RGB15(r,g,b) | BIT(15);
        }
        break;
      }
    }
    
  }
  
#undef Gravity

  free(bmdata); bmdata=NULL;
  
  return(true);
}

static bool intLoadBM_TransGreen(const char *bmpfn,u16 *pbm,const u32 bmw,const u32 bmh)
{
  bmerrstr1[0]=0;
  bmerrstr2[0]=0;
  
  if(pbm==NULL){
    snprintf(bmerrstr1,256,"BitmapMemory is NULL.");
    snprintf(bmerrstr2,256,"The memory is insufficient?");
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    return(false);
  }
  
  u8 *bmdata;
  u32 bmsize;
  
  Shell_ReadSkinFile(bmpfn,(void**)&bmdata,(int*)&bmsize);
  if(bmdata==NULL){
    return(false);
    }else{
//    _consolePrintf("loadskin /shell/%s\n",bmpfn);
  }
  
  TBMPHeader BMPHeader;
  
  if(GetBMPHeader(bmdata,&BMPHeader)==false){
    snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
    snprintf(bmerrstr2,256,"%s",BMP_LoadErrorStr);
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  if((BMPHeader.biWidth==1)&&(BMPHeader.biHeight==1)){
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  if((BMPHeader.biWidth<bmw)||(BMPHeader.biHeight<bmh)){
    snprintf(bmerrstr1,256,"Request /shell/%s WindowsBitmapFormat",bmpfn);
    snprintf(bmerrstr2,256,"%d x %dpixel 8 or 24bitcolor NoCompression.",bmw,bmh);
    _consolePrintf("%s\n",bmerrstr1);
    _consolePrintf("%s\n",bmerrstr2);
    free(bmdata); bmdata=NULL;
    return(false);
  }
  
  u32 gr=0,gg=0,gb=0;
  
#define Gravity(c,cg) { \
  c+=cg; \
  cg=c&7; \
  c=c>>3; \
  if((c&(~0x1f))!=0) c=(c<0) ? 0x00 : 0x1f; \
}

  for(u32 y=0;y<bmh;y++){
    u8 *pSrcBM=&BMPHeader.pBitmap[(BMPHeader.biHeight-1-y)*BMPHeader.DataWidth];
    u16 *pDstBM=&pbm[y*bmw];
    
    switch(BMPHeader.biBitCount){
      case 8: {
        u8 *PaletteTable=BMPHeader.pPalette;
        for(u32 x=0;x<bmw;x++){
          u8 *pal;
          u32 r,g,b;
          
          pal=&PaletteTable[*pSrcBM*4];
          pSrcBM+=1;
          
          b=pal[0];
          g=pal[1];
          r=pal[2];
          
          if((b==0x00)&&(g==0xff)&&(r==0x00)){
            pDstBM[x]=0;
            }else{
            Gravity(b,gb);
            Gravity(g,gg);
            Gravity(r,gr);
            
            pDstBM[x]=RGB15(r,g,b) | BIT(15);
          }
        }
        break;
      }
      case 24: {
        for(u32 x=0;x<bmw;x++){
          u32 r,g,b;
          
          b=pSrcBM[0];
          g=pSrcBM[1];
          r=pSrcBM[2];
          pSrcBM+=3;
          
          if((b==0x00)&&(g==0xff)&&(r==0x00)){
            pDstBM[x]=0;
            }else{
            Gravity(b,gb);
            Gravity(g,gg);
            Gravity(r,gr);
            
            pDstBM[x]=RGB15(r,g,b) | BIT(15);
          }
        }
        break;
      }
    }
    
  }
  
#undef Gravity

  free(bmdata); bmdata=NULL;
  
  return(true);
}

void LoadSkinBM(void)
{
  MemSet8DMA3(0,&SkinBM,sizeof(TSkinBM));
  
  TSkinBM *psbm=&SkinBM;
  
  psbm->TitleBM_W=ScreenWidth;
  psbm->TitleBM_H=TitleBarHeight;
  
  if(psbm->pTitleABM!=NULL){
    free(psbm->pTitleABM); psbm->pTitleABM=NULL;
  }
  psbm->pTitleABM=(u16*)safemalloc(psbm->TitleBM_W*psbm->TitleBM_H*2);
  
  if(intLoadBM("titlea.bmp",psbm->pTitleABM,psbm->TitleBM_W,psbm->TitleBM_H)==false){
    if(psbm->pTitleABM!=NULL){
      free(psbm->pTitleABM); psbm->pTitleABM=NULL;
    }
  }
  
  if(psbm->pTitleDBM!=NULL){
    free(psbm->pTitleDBM); psbm->pTitleDBM=NULL;
  }
  psbm->pTitleDBM=(u16*)safemalloc(psbm->TitleBM_W*psbm->TitleBM_H*2);
  
  if(intLoadBM("titled.bmp",psbm->pTitleDBM,psbm->TitleBM_W,psbm->TitleBM_H)==false){
    if(psbm->pTitleDBM!=NULL){
      free(psbm->pTitleDBM); psbm->pTitleDBM=NULL;
    }
  }
  
  if(psbm->pCloseBtnBM!=NULL){
    free(psbm->pCloseBtnBM); psbm->pCloseBtnBM=NULL;
  }
  psbm->CloseBtnBM_W=TitleBarHeight;
  psbm->CloseBtnBM_H=TitleBarHeight;
  psbm->pCloseBtnBM=(u16*)safemalloc(psbm->CloseBtnBM_W*psbm->CloseBtnBM_H*2);
  
  if(intLoadBM_TransGreen("closebtn.bmp",psbm->pCloseBtnBM,psbm->CloseBtnBM_W,psbm->CloseBtnBM_H)==false){
    if(psbm->pCloseBtnBM!=NULL){
      free(psbm->pCloseBtnBM); psbm->pCloseBtnBM=NULL;
    }
  }
  
  if(psbm->pClientBM!=NULL){
    free(psbm->pClientBM); psbm->pClientBM=NULL;
  }
  psbm->ClientBM_W=ScreenWidth;
  psbm->ClientBM_H=ScreenHeight;
  psbm->pClientBM=(u16*)safemalloc(psbm->ClientBM_W*psbm->ClientBM_H*2);
  
  if(intLoadBM("client.bmp",psbm->pClientBM,psbm->ClientBM_W,psbm->ClientBM_H)==false){
    if(psbm->pClientBM!=NULL){
      free(psbm->pClientBM); psbm->pClientBM=NULL;
    }
  }
  
  psbm->PrgBarBM_W=ScreenWidth;
  psbm->PrgBarBM_H=PrgBarHeight;
  
  if(psbm->pPrgBarABM!=NULL){
    free(psbm->pPrgBarABM); psbm->pPrgBarABM=NULL;
  }
  psbm->pPrgBarABM=(u16*)safemalloc(psbm->PrgBarBM_W*psbm->PrgBarBM_H*2);
  
  if(intLoadBM("prgbara.bmp",psbm->pPrgBarABM,psbm->PrgBarBM_W,psbm->PrgBarBM_H)==false){
    if(psbm->pPrgBarABM!=NULL){
      free(psbm->pPrgBarABM); psbm->pPrgBarABM=NULL;
    }
  }
  
  if(psbm->pPrgBarDBM!=NULL){
    free(psbm->pPrgBarDBM); psbm->pPrgBarDBM=NULL;
  }
  psbm->pPrgBarDBM=(u16*)safemalloc(psbm->PrgBarBM_W*psbm->PrgBarBM_H*2);
  
  if(intLoadBM("prgbard.bmp",psbm->pPrgBarDBM,psbm->PrgBarBM_W,psbm->PrgBarBM_H)==false){
    if(psbm->pPrgBarDBM!=NULL){
      free(psbm->pPrgBarDBM); psbm->pPrgBarDBM=NULL;
    }
  }
  
  if(psbm->pSB_BGBM!=NULL){
    free(psbm->pSB_BGBM); psbm->pSB_BGBM=NULL;
  }
  psbm->pSB_BGBM=(u16*)safemalloc(8*8*2);
  
  if(intLoadBM("sb_bg.bmp",psbm->pSB_BGBM,8,8)==false){
    if(psbm->pSB_BGBM!=NULL){
      free(psbm->pSB_BGBM); psbm->pSB_BGBM=NULL;
    }
  }
  
  if(psbm->pSB_BodyBM!=NULL){
    free(psbm->pSB_BodyBM); psbm->pSB_BodyBM=NULL;
  }
  psbm->pSB_BodyBM=(u16*)safemalloc(8*8*2);
  
  if(intLoadBM("sb_body.bmp",psbm->pSB_BodyBM,8,8)==false){
    if(psbm->pSB_BodyBM!=NULL){
      free(psbm->pSB_BodyBM); psbm->pSB_BodyBM=NULL;
    }
  }
  
  if(psbm->pSB_PartBM!=NULL){
    free(psbm->pSB_PartBM); psbm->pSB_PartBM=NULL;
  }
  psbm->pSB_PartBM=(u16*)safemalloc(8*8*2);
  
  if(intLoadBM("sb_part.bmp",psbm->pSB_PartBM,8,8)==false){
    if(psbm->pSB_PartBM!=NULL){
      free(psbm->pSB_PartBM); psbm->pSB_PartBM=NULL;
    }
  }
}

