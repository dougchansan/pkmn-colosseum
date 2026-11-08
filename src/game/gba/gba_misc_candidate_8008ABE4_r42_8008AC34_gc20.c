#include "dolphin/types.h"

/* Per-port GBA link state (1 = poll keys, 2 = busy, 3 = send 0x11). */
extern s32 lbl_803FB318[4];
/* Per-port result of the last poll. */
extern s32 lbl_803FB308[4];
/* Per-port GBA key word returned by the last successful poll. */
extern u16 lbl_8047A67C[4];
/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

extern s32 gbaCmdKeepAlive(s32 chan);
extern s32 gbaCmdReadKeys(s32 chan, u16* keys);

/*
 * Polls the GBA on a controller port that has no pad attached (index is
 * 1-based). Transient errors (1 or 2) are retried up to ten times before the
 * port's link state is dropped.
 */
void gbaPollPort(s32 index)
{
    s32 result;
    u16 keys;

    switch (lbl_803FB318[index - 1]) {
    case 1:
        result = gbaCmdReadKeys(index - 1, &keys);
        if (result == 0) {
            lbl_8047A684[index - 1] = 0;
            lbl_8047A67C[index - 1] = keys;
        } else if (result <= 2 && ++lbl_8047A684[index - 1] <= 10) {
            result = 0;
        } else {
            lbl_8047A67C[index - 1] = 0;
            /* Same stores as gbaCommandSetKeyState(index, 0), expanded inline. */
            lbl_803FB318[index - 1] = 0;
            lbl_803FB308[index - 1] = 0;
            lbl_8047A684[index - 1] = 0;
            lbl_8047A67C[index - 1] = 0;
        }
        break;
    case 3:
        result = gbaCmdKeepAlive(index - 1);
        if (result == 0) {
            lbl_8047A684[index - 1] = 0;
        } else if (result <= 2 && ++lbl_8047A684[index - 1] <= 10) {
            result = 0;
        } else {
            lbl_803FB318[index - 1] = 0;
        }
        lbl_8047A67C[index - 1] = 0;
        break;
    case 2:
        lbl_8047A67C[index - 1] = 0;
        result = 0;
        break;
    default:
        lbl_8047A67C[index - 1] = 0;
        result = 1;
        break;
    }
    lbl_803FB308[index - 1] = result;
}
