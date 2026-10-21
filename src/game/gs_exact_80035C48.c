/**
 * @file gs_exact_80035C48.c
 * @brief gs-engine code, 0x80035C48 - 0x80035DD4 (3 fns): the tail of the
 *        0x80033278 range that precedes the movie TU. Split out so the exact
 *        functions link while the rest of the range is still being matched;
 *        the unit builds with -opt nopeephole instead of local pragmas.
 */
#include "dolphin/types.h"

/* cardeArenaRoomInit - 0x80035C48 | size: 0x128 */
void cardeArenaRoomInit(void) {
    extern u8 lbl_803A3288[];
    extern u32 lbl_8047A430;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u8 lbl_8047A440;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u32 lbl_8047A44C;
    extern u8 lbl_8047A454;
    extern u32 lbl_8047A458;
    extern void* cardeArenaWalk(s32, s32);
    extern s32 cardeArenaCountRecords(s32);
    extern u32 GSmsgGetGSchar(u32);
    extern u32 std093_getCurrentGroupID(void);
    extern u8* savedataGetStatus(s32, s32);
    extern u32 std228_soundGetCurrentBackgroundMusic(void);
    extern void std104_soundSetBackgroundMusic(u32, s32, s32);
    extern void cameraPlayAnime(u32, u32, s32, s32);
    extern void GScharLenCpy(void*, u32, s32);
    u8* work;
    u32 soundId;
    u8 resetFlag;

    if (lbl_8047A438 == 0) {
        if (lbl_8047A439 != 0) {
            lbl_8047A458 = 2;
        } else {
            lbl_8047A458 = 1;
        }
        cameraPlayAnime(std093_getCurrentGroupID(), 0x11171800, 0, 0);
        if (lbl_8047A439 != 0) {
            soundId = 0x446;
        } else {
            soundId = 0x4CD;
        }
        if (soundId != std228_soundGetCurrentBackgroundMusic()) {
            std104_soundSetBackgroundMusic(soundId, 0, 0x7F);
        }
        lbl_8047A454 = 0;
        lbl_8047A449 = 0;
        lbl_8047A444 = 0x21;
        GScharLenCpy(lbl_803A3288, GSmsgGetGSchar(0x3CD1), 0x50);
        *(u16*)(lbl_803A3288 + 0x9E) = 0;
        work = savedataGetStatus(0, 0xD);
        lbl_8047A430 = (u32)work;
        resetFlag = lbl_8047A440;
        lbl_8047A44C = *(u32*)(work + 0x49C8);
        if (resetFlag != 0) {
            if (cardeArenaCountRecords(0) > 0) {
                *(u16*)cardeArenaWalk(0, 0) = 0;
            }
            lbl_8047A440 = 0;
        }
    }
}

/* fn_80035D70 - 0x80035D70 | size: 0x30 */
void std202_warpToMenuRoom393(void) {
    extern void std098_mapWarpToMenuRoom(u32);
    extern void _threadSwitch(void);
    extern u8 lbl_8047A439;
    u32 flag;

    flag = 1;
    lbl_8047A439 = flag;
    std098_mapWarpToMenuRoom(0x393);
    _threadSwitch();
}

/* cardeArenaRoomExit - 0x80035DA0 | size: 0x34 */
void cardeArenaRoomExit(void) {
    extern void floorLink(u32, s32);
    extern void _threadSwitch(void);
    extern u8 lbl_8047A439;

    lbl_8047A439 = 0;
    floorLink(0x393, 0);
    _threadSwitch();
}
