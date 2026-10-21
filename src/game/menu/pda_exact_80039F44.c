/** Byte-exact PDA button callback, 0x80039F44 - 0x80039F70. */
#include "dolphin/types.h"

extern s32 lbl_8047A4B8;
extern void menuButtonNormal(void* button);

void itemStorageListButton(void* button)
{
    if (lbl_8047A4B8 < 0) {
        menuButtonNormal(button);
    }
}
