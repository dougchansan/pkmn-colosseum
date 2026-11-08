#include "dolphin/types.h"

extern s32 pdaSnagListDetailKey(void* window, s32 mode);

void pdaSnagListCursor(void* window)
{
    pdaSnagListDetailKey(window, 1);
}

void pdaSnagListCursor2(void* window)
{
    pdaSnagListDetailKey(window, 0);
}
