#include "dolphin/types.h"

void menuDBDrawItem(u32 eventContext, u32* model)
{
    model[0x4C / sizeof(u32)] = 0x43E3;
}
