#include "dolphin/types.h"

extern s32 fn_8003B85C(void* window, s32 mode);

void fn_8003B814(void* window)
{
    fn_8003B85C(window, 1);
}

void fn_8003B838(void* window)
{
    fn_8003B85C(window, 0);
}
