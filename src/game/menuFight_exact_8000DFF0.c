#include "dolphin/types.h"

extern u8* windowGetKeyInfo(void);
extern u32 windowGetParam(u8* window, s32 index);
extern u8 fn_801F18DC(s32 controller);
extern u8 fightFloorIsUseFightTimerCommand(s32 controller);
extern u8 fightTimerCommandIsOver(void);
extern u16 std103_battleGetResult(void);

static inline u8 menuFightIsTimeUp(void)
{
    if (fn_801F18DC(0) != 0) {
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
            return 1;
        }
        if (std103_battleGetResult() == 1) {
            return 1;
        }
    }
    return 0;
}

static inline s32 menuFightTargetSecretSide(s32 target)
{
    switch (target) {
    case 0x45:
        return 1;
    case 0x46:
        return 0;
    case 0x47:
        return 2;
    case 0x48:
        return 2;
    default:
        return -1;
    }
}

void menuFightButtonTargetSecret(u8* ctx)
{
    u8* key;
    u8* entry;
    u16 bits;
    s32 selected;
    s32 i;

    key = windowGetKeyInfo();
    selected = -1;
    entry = (u8*)windowGetParam(ctx, 0);
    bits = *(u16*)(key + 4);
    if (bits & 1) {
        selected = 0;
    } else if (bits & 8) {
        selected = 1;
    } else if (bits & 4) {
        selected = 2;
    } else if (bits & 0x200) {
        ctx[0x98] = 1;
        ctx[0x99] = 1;
        *(s32*)(ctx + 0x80) = -1;
    }

    if (selected >= 0) {
        for (i = 0; i < 4; i++) {
            if (selected == menuFightTargetSecretSide(*(s32*)(entry + 4))) {
                ctx[0x98] = 1;
                *(s32*)(ctx + 0x80) = selected;
                return;
            }
            entry += 8;
        }
    }

    if (menuFightIsTimeUp() != 0) {
        ctx[0x98] = 1;
        ctx[0x99] = 1;
    }
}
