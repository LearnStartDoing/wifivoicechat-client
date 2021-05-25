
#ifndef arm9tcm_h
#define arm9tcm_h

#define CODE_IN_ITCM __attribute__ ((section (".itcm"),long_call))
//#define CODE_IN_ITCM
#define DATA_IN_DTCM __attribute__ ((aligned(4),section (".dtcm")))
//#define DATA_IN_DTCM

#define ATTR_LC __attribute__ ((long_call))

#endif

