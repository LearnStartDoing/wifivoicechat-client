
#ifndef lang_h
#define lang_h

#include "unicode.h"

enum ELang {EL_ENG,EL_JPN};

extern void SetLang(const ELang _EL);
extern ELang GetLang(void);

#endif

