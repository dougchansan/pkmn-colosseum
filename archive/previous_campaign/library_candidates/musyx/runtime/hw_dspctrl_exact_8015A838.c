#include "musyx/runtime/hw_dspctrl.h"

void salInitHRTFBuffer(void)
{
    memset(lbl_8047B018, 0, 0x100);
    DCFlushRangeNoSync(lbl_8047B018, 0x100);
}

u32 salExitDspCtrl(void)
{
    u8 i;

    salFree(lbl_8047B018);
    for (i = 0; i < lbl_8047B05D; i++) {
        salFree(lbl_8047B024[i].pb);
        salFree(lbl_8047B024[i].patchData);
    }
    for (i = 0; i < lbl_8047B05C; i++) {
        salFree(lbl_80447E60[i].spb);
        salFree(lbl_80447E60[i].main[0]);
    }
    salFree(lbl_8047B020);
    salFree(lbl_8047B024);
    salFree(lbl_8047B01C);
    salFree(lbl_8047B010);
    return 1;
}
