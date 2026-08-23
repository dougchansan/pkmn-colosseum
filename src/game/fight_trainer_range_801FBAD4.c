#include "dolphin/types.h"

extern void _threadSwitch(void);
extern void fn_801DA8C4(void* ptr, u16 field, u32 size);
extern u8 fn_801DA94C(void* ptr, u16 field, u32 size);
extern void fn_801DA9E8(void* ptr, u16 field, u32 size);
extern void fn_801DDD28(void* ptr, u16 field, u32 size, u32 flags);
extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
extern u32 itemGetStatus(u32 a, u32 b, u32 c, u32 d);
extern void* fightOutPokemonGetPokemonPtr(void* ctx);
extern u32 pokemonGetStatus(void* pokemon, u32 a, u32 b, u32 c);

void fightTrainerHokakuThrowEffect(void* context, u32 param, u8 mode)
{
    void* data;
    u32 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    animId = itemGetStatus(0, param, 0x12, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 2) {
        fn_801DA8C4(data, animId, 4);
    } else if (mode == 3) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 4) {
        fn_801DA9E8(data, 0x57, 4);
    } else if (mode == 5) {
        do {
            if ((u8)fn_801DA94C(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 6) {
        fn_801DA8C4(data, 0x57, 4);
    }
}

void fightTrainerBallThrowEffect(void* context, void* trainerCtx, u8 mode)
{
    void* data;
    u8 typeId;
    u32 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    typeId = (u8)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainerCtx),
                                       0, 0x73, 0);
    animId = itemGetStatus(0, typeId, 0x11, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, animId, 4);
    }
}
