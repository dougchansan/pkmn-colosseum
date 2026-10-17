/** Exact tool-entry battle-state accessors, 0x8025D9A8 - 0x8025DBB0. */
#include "dolphin/types.h"

typedef struct ToolentryBattleState {
    u32 unk_00;
    u32 battleType;
    u32 unk_08;
    u32 unk_0C;
    u32 unk_10;
    u32 count_14;
    u32 count_18;
} ToolentryBattleState;

extern ToolentryBattleState* fn_8006B5A8(void);
extern u8* savedataBattleGetEntryPtr(s32 index);
extern u32 fn_8006A7D8(u32 entry);
extern u32 fn_8006A7E8(u32 entry);

u32 battleResultGet00(void)
{
    return fn_8006B5A8()->unk_00;
}

u32 battleResultGet10(void)
{
    return fn_8006B5A8()->unk_10;
}

u32 toolentryTaisenGetHomePlace(s32 index)
{
    return (u16)fn_8006A7E8((u32)savedataBattleGetEntryPtr(index));
}

u32 toolentryTaisenGetBattlePlayerID(s32 index)
{
    return fn_8006A7D8((u32)savedataBattleGetEntryPtr(index));
}

u32 toolentryTaisenGetEntryPlayerNum(void)
{
    s32 mode;
    u32 res;
    ToolentryBattleState* state;

    state = fn_8006B5A8();
    res = 2;
    mode = state->battleType;
    switch (mode) {
    case 0:
    case 1:
        res = 2;
        break;
    case 2:
        res = 4;
        break;
    }
    return res;
}

u32 toolentryTaisenGetBattleType(void)
{
    return fn_8006B5A8()->battleType;
}

u32 battleResultGet0C(void)
{
    return fn_8006B5A8()->unk_0C;
}

u32 battleResultGet08(void)
{
    return fn_8006B5A8()->unk_08;
}

u32 battleResultDec18(void)
{
    ToolentryBattleState* state;

    state = fn_8006B5A8();
    if (state->count_18 != 0) {
        state->count_18--;
    }
    return state->count_18;
}

u32 battleResultInc18(void)
{
    ToolentryBattleState* state;

    state = fn_8006B5A8();
    state->count_18++;
    return state->count_18;
}

u32 battleResultGet18(void)
{
    return fn_8006B5A8()->count_18;
}

u32 battleResultInc14(void)
{
    ToolentryBattleState* state;

    state = fn_8006B5A8();
    state->count_14++;
    return state->count_14;
}
