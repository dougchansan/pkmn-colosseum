/**
 * @file fight_trainer_ai_waza_value_exact_8024C5BC.c
 * @brief Exact pure-C trainer AI value island, 0x8024C5BC - 0x8024D818.
 */
#include "game/colosseum.h"

u32 fightTrainerAiWazaValueTobihaneru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u16 fn_802367CC(void*, u32, u16*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(void*, u32, u32);
    u32 outPokemon[8];
    u16 moves[10];
    u32 handle;
    u16 outCount;
    u16 moveCount;
    u16 i;
    u16 j;
    u8 found;

    handle = 0;
    outCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 0, 1);
    if (fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x7e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7e);
    }
    if (fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x7f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7f);
    }
    if (fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x80);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x80);
    }
    if (fn_80236BFC(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x81);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x81);
    }
    if (fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x82);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x82);
    }
    if (fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x83);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x83);
    }
    if (fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x84);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x84);
    }
    if (fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x85);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x85);
    }
    if (fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x86);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x86);
    }
    if (fn_80236D60(ctx, param1, param3) > 0) {
        handle = fn_80239984(handle, ctx, 0x87);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x87);
    }
    handle = fn_80239984(handle, ctx, 0x88);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x88);
    if (fn_80236BFC(ctx, param1, 3) == 1) {
        handle = fn_80239984(handle, ctx, 0x89);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x89);
    }
    if (fn_80236BFC(ctx, param1, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x8a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8a);
    }
    if (fn_80236BFC(ctx, param1, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x8b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8b);
    }
    if (fn_80236BFC(ctx, param1, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x8c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8c);
    }
    if (fn_80236BFC(ctx, param1, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x8d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8d);
    }
    if (fn_80236BFC(ctx, param1, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x8e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8e);
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x8f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x8f);
    }
    if (fn_80236BFC(ctx, param1, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x90);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x90);
    }
    if (fn_80236BFC(ctx, param1, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x91);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x91);
    }

    for (i = 0; i < outCount; i++) {
        moveCount = fn_802367CC(ctx, outPokemon[i], moves, 0, 0);
        if (moveCount != 0) {
            found = 0;
            for (j = 0; j < moveCount; j++) {
                if (fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(ctx, 0x1f, moves[j]) == 1) {
                    handle = fn_80239984(handle, ctx, 0x92);
                    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x92);
                    found = 1;
                    break;
                }
            }
            if (found == 1) {
                break;
            }
        }
    }

    if (fn_80236BFC(ctx, param1, 0x1d) == 1) {
        handle = fn_80239984(handle, ctx, 0x93);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x93);
    }
    return handle;
}

u32 fightTrainerAiWazaValueSorawotobu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u16 fn_802367CC(void*, u32, u16*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(void*, u32, u32);
    u32 outPokemon[8];
    u16 moves[10];
    u32 handle;
    u32 stateType;
    u16 outCount;
    u16 moveCount;
    u16 i;
    u16 j;
    u8 found;

    handle = 0;
    stateType = 0;
    outCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 0, 1);
    if (fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x69);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x69);
    }
    if (fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x6a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6a);
    }
    if (fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x6b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6b);
    }
    if (fn_80236BFC(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x6c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6c);
    }
    if (fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x6d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6d);
    }
    if (fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x6e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6e);
    }
    if (fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x6f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x6f);
    }
    if (fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x70);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x70);
    }
    if (fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x71);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x71);
    }
    if (fn_80236D60(ctx, param1, param3) > 0) {
        handle = fn_80239984(handle, ctx, 0x72);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x72);
    }
    if (fn_80236BFC(ctx, param1, 3) == 1) {
        handle = fn_80239984(handle, ctx, 0x73);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x73);
    }
    if (fn_80236BFC(ctx, param1, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x74);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x74);
    }
    if (fn_80236BFC(ctx, param1, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x75);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x75);
    }
    if (fn_80236BFC(ctx, param1, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x76);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x76);
    }
    if (fn_80236BFC(ctx, param1, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x77);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x77);
    }
    if (fn_80236BFC(ctx, param1, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x78);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x78);
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x79);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x79);
    }
    if (fn_80236BFC(ctx, param1, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x7a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7a);
    }
    if (fn_80236BFC(ctx, param1, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x7b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7b);
    }

    if ((param2 & 0xffff) == 0x13 || (param2 & 0xffff) == 0x154) {
        stateType = 0x1f;
    } else if ((param2 & 0xffff) == 0x5b) {
        stateType = 0x20;
    } else if ((param2 & 0xffff) == 0x123) {
        stateType = 0x21;
    }

    for (i = 0; i < outCount; i++) {
        moveCount = fn_802367CC(ctx, outPokemon[i], moves, 0, 0);
        if (moveCount != 0) {
            found = 0;
            for (j = 0; j < moveCount; j++) {
                if (fightTrainerAiCheckJoutaiKieWazaHitWazaDataId(ctx, stateType, moves[j]) == 1) {
                    handle = fn_80239984(handle, ctx, 0x7c);
                    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7c);
                    found = 1;
                    break;
                }
            }
            if (found == 1) {
                break;
            }
        }
    }

    if (fn_80236BFC(ctx, param1, 0x1d) == 1) {
        handle = fn_80239984(handle, ctx, 0x7d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x7d);
    }
    return handle;
}
