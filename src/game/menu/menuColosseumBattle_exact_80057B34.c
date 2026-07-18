/**
 * @file menuColosseumBattle_exact_80057B34.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x80057B34 - 0x80057C9C.
 */
#include "dolphin/types.h"
#include "game/menu/menu_name_entry.h"

typedef struct ColosseumBattleConnectState {
    s32 active;
    s32 connected;
    s32 busyRequest;
    s32 reserved;
} ColosseumBattleConnectState;

extern void _threadSwitch(void);
extern void fn_800F915C(s32);
extern void fn_8017B3E4(s32);
extern s32 fn_8017B2CC(s32);
extern void fn_8017B1CC(s32);

extern u8 lbl_803A9A08[];

/* Address: 0x80057B34 | Size: 0x168 */
#pragma push
#pragma peephole off
u32 fn_80057B34(u32 arg0, u32 arg1) {
    u32 result;
    u8 requestedConnection;
    ColosseumBattleConnectState* state;

    requestedConnection = 0;
    state = (ColosseumBattleConnectState*)lbl_803A9A08;

    if ((state->active != 0 ? (state->connected != 0 ? 1 : 0) : 2) != 2) {
        state = (ColosseumBattleConnectState*)lbl_803A9A08;
        requestedConnection = 1;
        state->busyRequest = 1;
        while ((state->active != 0 ? (state->connected != 0 ? 1 : 0) : 2) != 1) {
            _threadSwitch();
        }
    }

    fn_8017B1CC(0x70B);
    fn_800F915C(0x70B);
    fn_8017B3E4(0x670);
    while (fn_8017B2CC(0x670) == 1) {
        _threadSwitch();
    }

    result = menuNameEntryCall(arg0, arg1);
    fn_8017B1CC(0x670);
    fn_800F915C(0x670);
    fn_8017B3E4(0x70B);
    while (fn_8017B2CC(0x70B) == 1) {
        _threadSwitch();
    }

    if (requestedConnection != 0) {
        ((ColosseumBattleConnectState*)lbl_803A9A08)->busyRequest = 0;
    }
    return result;
}
#pragma pop
