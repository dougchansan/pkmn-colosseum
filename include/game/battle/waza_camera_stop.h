#ifndef GAME_BATTLE_WAZA_CAMERA_STOP_H
#define GAME_BATTLE_WAZA_CAMERA_STOP_H

#include "dolphin/types.h"

extern void* lbl_8047B3EC; /* active waza camera sequence */
extern void* lbl_8047B3F0; /* its camera animation */

extern void GSscene_SetMode(s32 arg);
extern void cameraStopAnime(void* arg);
extern void fn_801765F4(s32 arg);
extern s32 GSgetVersionMinor(void);

/*
 * wazaCameraStop -- end the active waza camera: stop its camera animation
 * (or put the scene back to mode 8 if it has none), clear the sequence and
 * restore scene mode 2 when GSgetVersionMinor reports 2.
 *
 * Expanded inline at four sites in retail (battleCameraWazaRestart, battleCameraWazaUpdate,
 * battleCameraWazaStop, battleCameraWazaStopIfOwner): find_inline_expansions.py block 0x801D2FAC
 * 0x801D301C scores 1.000 at battleCameraWazaStopIfOwner and 0.857 at the other two, with
 * the same five calls in the same order.
 */
static inline void wazaCameraStop(void)
{
    void* obj;

    obj = lbl_8047B3F0;
    if (obj == NULL) {
        GSscene_SetMode(8);
    } else {
        if (*(u32*)((u8*)obj + 0x18) != 0 && *(u32*)((u8*)obj + 0x20) != 0) {
            cameraStopAnime(obj);
        }
        lbl_8047B3F0 = NULL;
    }
    fn_801765F4(0);
    lbl_8047B3EC = NULL;
    if (GSgetVersionMinor() == 2) {
        GSscene_SetMode(2);
    }
}

#endif /* GAME_BATTLE_WAZA_CAMERA_STOP_H */
