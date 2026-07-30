/**
 * @file menuColosseumBattle_candidate_80057C9C.c
 * @brief menuColosseumBattle.cpp candidate range, 0x80057C9C - 0x80057DE8.
 */
#include "dolphin/types.h"

typedef struct ColosseumConnectJob {
    s32 running;
    s32 ready;
    s32 request;
    s32 canceled;
} ColosseumConnectJob;

extern ColosseumConnectJob lbl_803A9A08;
extern void _threadSwitch(void);
extern void fn_8017B1CC(s32);
extern s32 fn_8017B2CC(s32);
extern void fn_8017B3E4(s32);
extern void fn_800F915C(s32);
extern s32 fn_80097D94(void*, void*, s32*);

static inline s32 getConnectJobState(void)
{
    if (lbl_803A9A08.running != 0) {
        if (lbl_803A9A08.ready != 0) {
            return 1;
        }
        return 0;
    }
    return 2;
}

#pragma peephole off
s32 fn_80057C9C(void* pokemon, void* callback, s32* navigation)
{
    u8 ownsRequest = 0;
    s32 status;

    if (getConnectJobState() != 2) {
        lbl_803A9A08.request = 1;
        ownsRequest = 1;
        while (getConnectJobState() != 1) {
            _threadSwitch();
        }
    }

    fn_8017B1CC(0x70B);
    fn_800F915C(0x70B);
    fn_8017B3E4(0x66F);
    while (fn_8017B2CC(0x66F) == 1) {
        _threadSwitch();
    }

    fn_80097D94(pokemon, callback, navigation);
    fn_8017B1CC(0x66F);
    fn_800F915C(0x66F);
    fn_8017B3E4(0x70B);
    while (status = fn_8017B2CC(0x70B), status == 1) {
        _threadSwitch();
    }

    if (ownsRequest) {
        lbl_803A9A08.request = 0;
    }
    return status;
}
#pragma peephole reset
