/** Exact standalone owner for 0x801643D8 - 0x80164488 (hw_dolphin.c ends at
 * 0x801643D8). */
#include "dolphin/types.h"

extern void ReverbHICallback(u32 left, u32 right, u32 surround, void* work);
extern u32 lbl_8047B054;

u32 salMalloc(u32 size) {
    return ((u32 (*)(u32))lbl_8047B054)(size);
}

void salFree(u32 allocation) {
    u32* callbacks = &lbl_8047B054;

    ((void (*)(u32))callbacks[1])(allocation);
}

void sndAuxCallbackReverbHI(u8 type, u32* data, u8* work) {
    switch (type) {
    case 0:
        if (work[0x1C4] != 0) {
            break;
        }
        ReverbHICallback(data[0], data[1], data[2], work);
        break;
    case 1:
        break;
    }
}
