/**
 * @file fight_trainer_ai_waza_hit_candidate_80256ED0.c
 * @brief Candidate fightTrainerAiWazaHit.cpp range, 0x80256ED0 - 0x802570D0.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

void fightTrainerAiWazaHit120(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightOutPokemonGetSex();
    extern void fn_80236BFC();
    extern void fn_80237288();
    extern void fn_80237F74();
    extern void _fightTrainerAiWazaHitCheck();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r26 = r4;
    r27 = r3;
    r29 = r5;
    r28 = r6;
    r3 = r26;
    fightOutPokemonGetSex();
    r30 = r3;
    r3 = r28;
    fightOutPokemonGetSex();
    r31 = r3;
    r3 = r27;
    r4 = r26;
    r5 = r29;
    r6 = r28;
    r7 = 0x0;
    _fightTrainerAiWazaHitCheck();
    r0 = r3;
    r3 = r27;
    r29 = r0;
    r4 = r28;
    r5 = 0xc;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r29 = 0x0;
    }
    r3 = r27;
    r4 = r28;
    r5 = 0x11;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r27;
        r4 = r28;
        r5 = 0x14;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r28;
            r5 = 0x7;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = r27;
                r4 = r28;
                r5 = 0xf;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                    r3 = r27;
                    r4 = r28;
                    r5 = 0x48;
                    fn_80237F74();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                        r3 = r27;
                        r4 = r28;
                        r5 = 0x29;
                        fn_80237F74();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r27;
                            r4 = r28;
                            r5 = 0x28;
                            fn_80237F74();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x1) {
                                r3 = r27;
                                r4 = r28;
                                r5 = 0xc;
                                fn_80237F74();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r0 = 0x0;
                                    goto L_80257034;
    }
    }
    }
    }
    }
    }
    }
    }
    r0 = 0x1;
L_80257034:
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r30 & 0xFF;
    r0 = r31 & 0xFF;
    if (r3 != (u32)r0) {
        r3 = r27;
        r4 = r28;
        fn_80237288();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r27;
            r4 = r28;
            r5 = 0xa;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r0 = r30 & 0xFF;
                if (r0 != (u32)0x2) {
                    r0 = r31 & 0xFF;
                    if (r0 == (u32)0x2) {
        }
        }
        }
        }
        r29 = 0x0;
                    }
    if ((s32)r29 == (s32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = 0x1;

    return;
}
