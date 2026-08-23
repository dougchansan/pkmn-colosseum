#include "dolphin/os/OSInterrupt.h"

extern volatile u16 lbl_8047B084;
extern volatile BOOL lbl_8047B080;

void hwDisableIrq(void)
{
    u16 v = lbl_8047B084++;

    if (v == 0) {
        lbl_8047B080 = OSDisableInterrupts();
    }
}
