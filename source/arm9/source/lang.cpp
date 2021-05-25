
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <NDS.h>

#include "_console.h"
#include "_const.h"
#include "memtool.h"

#include "lang.h"

static ELang EL;

void SetLang(const ELang _EL)
{
  EL=_EL;
}

extern ELang GetLang(void)
{
  return(EL);
}

