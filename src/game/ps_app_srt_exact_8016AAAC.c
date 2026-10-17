#include "game/ps_types.h"

extern PSAppSRT* lbl_8047B124;
extern u16 lbl_8047B110;
extern u16 lbl_8047B116;
extern u16 lbl_8047B120;
extern void HSD_Free(void* allocation);
extern void* HSD_MemAlloc(s32 size);
extern void* memset(void* destination, s32 value, u32 size);

void psRemoveAppSRT(void)
{
    PSAppSRT* appSRT = lbl_8047B124;

    while (appSRT != NULL) {
        PSAppSRT* next = appSRT->next;
        HSD_Free(appSRT);
        appSRT = next;
    }

    lbl_8047B124 = NULL;
}

s32 psInitAppSRT(s32 count, s32 size)
{
    s32 i = 0;

    lbl_8047B116 = 0;
    lbl_8047B110 = 0;
    lbl_8047B120 = size;
    lbl_8047B124 = NULL;

    while (i < count) {
        PSAppSRT* appSRT = HSD_MemAlloc(size);

        if (appSRT == NULL) {
            return i;
        }

        memset(appSRT, 0, size);
        appSRT->next = lbl_8047B124;
        lbl_8047B124 = appSRT;
        i++;
    }

    return i;
}
