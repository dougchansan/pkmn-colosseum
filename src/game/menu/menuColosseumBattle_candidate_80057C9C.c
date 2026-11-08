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
extern void GSresReleaseById(s32);
extern s32 GSresGroupGetState(s32);
extern void fsysStartLoad(s32);
extern void GSresReleaseOwner(s32);
extern s32 menuPokemonStatusOpenB(void*, void*, s32*);

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

void pcBoxRunStatus(void* pokemon, void* callback, s32* navigation)
{
    u8 ownsRequest = 0;

    if (getConnectJobState() != 2) {
        lbl_803A9A08.request = 1;
        ownsRequest = 1;
        while (getConnectJobState() != 1) {
            _threadSwitch();
        }
    }

    GSresReleaseById(0x70B);
    GSresReleaseOwner(0x70B);
    fsysStartLoad(0x66F);
    while (GSresGroupGetState(0x66F) == 1) {
        _threadSwitch();
    }

    menuPokemonStatusOpenB(pokemon, callback, navigation);
    GSresReleaseById(0x66F);
    GSresReleaseOwner(0x66F);
    fsysStartLoad(0x70B);
    while (GSresGroupGetState(0x70B) == 1) {
        _threadSwitch();
    }

    if (ownsRequest) {
        lbl_803A9A08.request = 0;
    }
}
