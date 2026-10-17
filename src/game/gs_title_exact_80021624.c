#include "dolphin/types.h"

extern void titleMenuMain(void);

/* Retained in the original gs_title translation unit despite no direct callsite. */
#pragma force_active on

void fn_80021624(void)
{
    titleMenuMain();
}
