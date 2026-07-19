/**
 * @file fight_range_exact_80220778.c
 * @brief Exact pure-C fight-sequence helper run, 0x80220778 - 0x80220868.
 */
#include "dolphin/types.h"

#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
s32 _WsWkcActSubJoutaiMigawari__FPvUsPv(void* ctx, u16 param2, void* param3) {
    extern u8 fn_802026E4();
    extern s32 fn_801FEF74();
    extern void fn_80202810();
    if (fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) <= 0) {
        fn_80202810(ctx, 0x14);
    }
    return 1;
}
#undef fn_80202810
#undef fn_801FEF74

#define fn_8012640C pokemonGetStatus
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801254B4 pokemonSetStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
s32 fn_802207D4(u32 ctx) {
    extern u32 fn_8012640C();
    extern u16 fn_8020147C();
    extern u32 fn_801254B4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u16 field = (u16)fn_8012640C(ctx, 0, 0xfb, 0);
    if (field != 0) {
        fn_8020147C(ctx, field, 1, 0);
        fn_801254B4(ctx, 0, 0xfb, 0, 0);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x82, 0, 0);
        }
    }
    return 1;
}
#undef fn_8012640C
#undef fn_8020147C
#undef fn_801254B4
#undef fn_801FECD4
#undef fn_801FE7EC
