/**
 * @file fight_trainer_ai_waza_hit_candidate_802546E8.c
 * @brief Candidate fightTrainerAiWazaHit.cpp range, 0x802546E8 - 0x80254810.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

void fightTrainerAiWazaHit192(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r6 = 0x0;
    r7 = 0x0;
    r5 = (u32)sp + 0x1c;
    r29 = r3;
    r27 = r4;
    fn_802367CC();
    r31 = r3;
    r4 = r29;
    r5 = (u32)sp + 0x30;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r28 = r3;
    r3 = r29;
    r4 = r27;
    r5 = 0x27;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r27 = (u32)sp + 0x30;
    r28 = r28 & 0xFFFF;
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)r28) break;
        r4 = *(u32*)(r27 + r0);
        if (r4 != (u32)0x0) {
            r3 = r29;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x0;
            fn_802367CC();
            r5 = (u32)sp + 0x1c;
            r4 = r31 & 0xFFFF;
            r8 = (u32)sp + 0x8;
            r0 = r3 & 0xFFFF;
            r10 = 0x0;
            while (1) {
                r3 = r10 & 0xFFFF;
                if (r3 >= (u32)r0) break;
                r9 = 0x0;
                while (1) {
                    r3 = r9 & 0xFFFF;
                    if (r3 >= (u32)r4) break;
                    r6 = *(u16*)(r8 + r7);
                    r3 = *(u16*)(r5 + r3);
                    if (r6 == (u32)r3) {
                        r3 = 0x1;
                        return;
                    }
                    r9 = r9 + 0x1;

                }
                r10 = r10 + 0x1;

            }
        }
        r30 = r30 + 0x1;

    }
    r3 = 0x0;

    return;
}
