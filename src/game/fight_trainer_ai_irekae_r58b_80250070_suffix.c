#include "dolphin/types.h"

extern u8 fn_80236BFC();
extern u8 fn_8023753C();

s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    if (fn_80236BFC(ctx, param1, 0x9) == 1) {
        return 0x9;
    }
    if (fn_80236BFC(ctx, param1, 0xA) == 1) {
        return 0xA;
    }
    if (fn_80236BFC(ctx, param1, 0x1E) == 1 && fn_8023753C(ctx, param1) == 0) {
        return 0xB;
    }
    if (fn_80236BFC(ctx, param1, 0xE) == 1) {
        return 0xC;
    }
    if (fn_80236BFC(ctx, param1, 0x17) == 1) {
        return 0xD;
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        return 0xE;
    }
    if (fn_80236BFC(ctx, param1, 0x19) == 1) {
        return 0xF;
    }
    if (fn_80236BFC(ctx, param1, 0x1B) == 1) {
        return 0x10;
    }
    if (fn_80236BFC(ctx, param1, 0x1C) == 1) {
        return 0x11;
    }
    if (fn_80236BFC(ctx, param1, 0x1D) == 1) {
        return 0x12;
    }
    if (fn_80236BFC(ctx, param1, 0x26) == 1) {
        return 0x13;
    }
    if (fn_80236BFC(ctx, param1, 0x27) == 1) {
        return 0x14;
    }
    if (fn_80236BFC(ctx, param1, 0x28) == 1) {
        return 0x15;
    }
    if (fn_80236BFC(ctx, param1, 0x29) == 1) {
        return 0x16;
    }
    if (fn_80236BFC(ctx, param1, 0x2A) == 1) {
        return 0x17;
    }
    return fn_80236BFC(ctx, param1, 0x30) == 1 ? 0x18 : 0;
}
