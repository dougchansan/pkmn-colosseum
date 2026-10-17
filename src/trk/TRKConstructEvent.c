#include "dolphin/types.h"

void TRKConstructEvent(void* event, s32 type) {
    ((s32*)event)[0] = type;
    ((s32*)event)[1] = 0;
    ((s32*)event)[2] = -1;
}
