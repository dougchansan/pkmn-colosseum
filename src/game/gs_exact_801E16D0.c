/** Exact VTR/movie state helpers, 0x801E16D0 - 0x801E189C. */
#include "dolphin/types.h"
#include "game/gs_mem.h"

/* Retail symbols for the canonical GSmem API declared by gs_mem.h. */
extern u16 GSmemGetHandleByPtr(void* ptr); /* GSmemFindHandle */
extern void* fn_800E24B0(u16 handle); /* GSmemLock */
extern void GSmemFree(u16 handle); /* GSmemFree */

s32 GSmovieGetFrame(void)
{
    extern s32 THPPlayerGetFrame(void);

    return THPPlayerGetFrame();
}

void fn_801E16F0(void)
{
    extern u8 lbl_8047B440;
    extern u8 lbl_8047B441;
    extern void* lbl_8047B450;
    extern void THPPlayerDrawDone(void);
    extern s32 THPPlayerGetState(void);
    extern void THPPlayerStop(void);
    extern BOOL THPPlayerClose(void);
    extern void GSscratchSetValid(void);
    u16 handle;
    u8 active;

    if (lbl_8047B440 == 0 || lbl_8047B441 == 0) {
        active = 0;
    } else {
        active = 1;
    }
    if (active != 0) {
        THPPlayerDrawDone();
        switch (THPPlayerGetState()) {
        case 3:
        case 5:
            if (lbl_8047B441 != 0) {
                THPPlayerStop();
                THPPlayerClose();
                handle = GSmemGetHandleByPtr(lbl_8047B450);
                if (handle != 0) {
                    fn_800E24B0(handle);
                    GSmemFree(handle);
                }
                lbl_8047B441 = 0;
                GSscratchSetValid();
            }
            break;
        }
    }
}

void fn_801E17A8(void)
{
    extern u8 lbl_8047B440;
    extern u8 lbl_8047B441;
    extern s32 lbl_8047B454;
    extern s32 lbl_8047B458;
    extern s32 lbl_8047B45C;
    extern s32 lbl_80469030[4];
    extern s32 THPPlayerDrawCurrentFrame(s32, s32, s32, s32, s32);
    u8 active;

    if (lbl_8047B440 == 0 || lbl_8047B441 == 0) {
        active = 0;
    } else {
        active = 1;
    }
    if (active != 0) {
        THPPlayerDrawCurrentFrame(lbl_8047B45C, lbl_8047B458, lbl_8047B454,
                    lbl_80469030[0], lbl_80469030[1]);
    }
}

void GSmovieClose(void)
{
    extern u8 lbl_8047B441;
    extern void* lbl_8047B450;
    extern void THPPlayerStop(void);
    extern BOOL THPPlayerClose(void);
    extern void GSscratchSetValid(void);
    u16 handle;

    if (lbl_8047B441 != 0) {
        THPPlayerStop();
        THPPlayerClose();
        handle = GSmemGetHandleByPtr(lbl_8047B450);
        if (handle != 0) {
            fn_800E24B0(handle);
            GSmemFree(handle);
        }
        lbl_8047B441 = 0;
        GSscratchSetValid();
    }
}

u8 GSmovieIsActive(void)
{
    extern u8 lbl_8047B440;
    extern u8 lbl_8047B441;

    if (lbl_8047B440 == 0 || lbl_8047B441 == 0) {
        return 0;
    }
    return 1;
}
