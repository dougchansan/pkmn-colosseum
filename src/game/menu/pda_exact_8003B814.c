#include "dolphin/types.h"

extern s32 pdaSnagListDetailKey(void* window, s32 mode);

void fn_8003B814(void* window)
{
    pdaSnagListDetailKey(window, 1);
}

void fn_8003B838(void* window)
{
    pdaSnagListDetailKey(window, 0);
}
