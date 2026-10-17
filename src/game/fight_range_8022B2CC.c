/**
 * @file fight_range_8022B2CC.c
 * @brief Fight target selection, 0x8022B2CC - 0x8022BB84 (fightSeqSelectTarget,
 *        _fightOutPokemonSoubiItemEffectSub), with .data 0x8039A220 - 0x8039A388.
 *
 * The .data part is fightSeqSetTsuikaKouka's switch table (written out as data, see
 * below) followed by this unit's two switch tables. _fightOutPokemonSoubiItemEffectSub's table
 * starts at 0x8039A314 and fightSeqSelectTarget's at 0x8039A2F4, both 4 mod 8, and
 * MWCC aligns each .data section to 8, so the carve has to start its .data
 * at 0x8039A220.
 *
 * fightSeqSelectTarget's Follow Me redirect is the same inline as the one in
 * fight_range_candidate_80219838.c; its locals, numbered in reverse
 * declaration order, give retail's side/floor registers.
 */
#include "dolphin/types.h"

extern u8 lbl_80478D78[8];
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);
extern void fightSeqSetTsuikaKouka();

/* RULE-EXCEPTION(user-approved): another function's switch table written as a
 * data initializer so the carve can own 8-aligned .data -- see
 * docs/RULE_EXCEPTIONS.md */
#if !defined(FIGHT_RANGE_802249B8_COMBINED)
void* jumptable_8039A220[53] = {
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xBC8),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xC50),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xE1C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xD04),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xD88),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xE70),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xEB4),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0xF70),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1020),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1094),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x11F0),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1234),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x126C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1450),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1494),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x14CC),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x14F0),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1514),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1584),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x15A8),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1108),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x117C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1610),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1668),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x175C),
    (void*)((u8*)fightSeqSetTsuikaKouka + 0x1738),
};
#endif

extern u32 wazaGetStatus();
extern u32 fightTargetGetPtrAsNowFightType();
extern u32 fightFloorGetValidFightOutPokemonCount();
extern u32 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
extern u8 fightOutPokemonIsZokuseiDataId();
extern u32 fightFloorGetFightOutPokemonPtrRandom();
extern u32 fightTargetGetPtr();
extern u32 fightOutPokemonGetTokuseiDataId();
extern u32 fightFloorGetStatus();
extern u8 fightSideIsJoutaiDataId();
extern u32 fightSideGetJoutaiUserFightTargetId();
extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
extern u8 fightOutPokemonCheckFightOut();
extern u32 fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst();
extern void pokemonSetStatus();

/* The live Follow Me user (side joutai 0x4d) on the side that
 * fightTargetGetPtrAsNowFightType(3, attacker) returns, or 0. */
static inline u32 fightGetFollowMeTarget(u32 attacker)
{
    u32 target;
    u32 userId;
    u32 side;
    u32 redirected;
    u16 floor;

    side = fightTargetGetPtrAsNowFightType(3, attacker);
    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    redirected = 0;
    if (fightSideIsJoutaiDataId(side, 0x4d) == 1) {
        userId = fightSideGetJoutaiUserFightTargetId(side, 0x4d);
        if ((userId & 0xffff) != 0) {
            target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(userId, floor);
            if (target != 0 && fightOutPokemonCheckFightOut(target) == 1) {
                redirected = target;
            }
        }
    }
    return redirected;
}

u32 fightSeqSelectTarget(u32 attacker, u32 move, u32 r5, u32 selector, u32 r7, u32 r8, s8 in_r9)
{
    u32 result;
    u32 count;
    u32 abilityCount;
    u32 redirected;
    u32 targetAbility;
    u16 moveType;
    u8 category;

    result = 0;
    if (in_r9 < 0) {
        category = wazaGetStatus(0, move, 5, 0);
    } else {
        category = in_r9;
    }
    if ((u16)move == 0xae && fightOutPokemonIsZokuseiDataId(attacker, 7) == 0) {
        category = 5;
    }
    moveType = wazaGetStatus(0, move, 3, 0);
    count = fightFloorGetValidFightOutPokemonCount(0, 0, attacker, 1);
    abilityCount = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(0, 0x1f, 2, attacker);

    switch (category) {
    case 0:
        if ((u16)count >= 2) {
            if ((u8)r7 == 1) {
                if (selector != 0) {
                    result = ((u32 (*)(u32, u32, u32))selector)(attacker, move, r5);
                } else {
                    result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 2, attacker);
                }
            } else {
                result = fightTargetGetPtrAsNowFightType(0x12, 0);
            }
            if ((u8)r8 == 1) {
                targetAbility = fightOutPokemonGetTokuseiDataId(result);
                redirected = fightGetFollowMeTarget(attacker);
                if (redirected != 0) {
                    result = redirected;
                } else if ((u16)targetAbility != 0x1f && moveType == 0xd && (u16)abilityCount != 0) {
                    result = fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(0, 0x1f, 1, 2, attacker);
                    pokemonSetStatus(result, 0, 0x114, 0, 1);
                }
            }
        } else if ((u8)r7 == 1) {
            result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 3, attacker);
        }
        break;
    case 1:
    case 4:
    case 6:
    case 7:
        if ((u8)r7 == 1) {
            result = fightTargetGetPtr(0xf, attacker, r5);
            if (fightOutPokemonCheckFightOut(result) == 0) {
                result = fightTargetGetPtrAsNowFightType(0xe, result);
            }
        }
        break;
    case 3:
        if ((u8)r7 == 1) {
            result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 2, attacker);
        }
        break;
    case 2:
    case 5:
        if ((u8)r7 == 1) {
            result = attacker;
        }
        break;
    }
    return result;
}

u32 _fightOutPokemonSoubiItemEffectSub(void* rawCtx)
{
    extern u8 lbl_80379A3C[];
    extern u8 lbl_80379A5A[];
    extern u8 lbl_80379A78[];
    extern u8 lbl_80379A96[];
    extern u8 lbl_80379AB4[];
    extern u8 lbl_80379AD2[];
    extern u8 lbl_80379AEE[];
    extern u8 lbl_80379B0C[];
    extern u16 lbl_80279FD0[8];
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u16 fightOutPokemonGetSoubiItemSoubiDataId();
    extern void figthOutPokemonGetSoubiItemBuff();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightFloorSetStatus();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80119F50();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u8 fightOutPokemonIsJoutaiNormal();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern s32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    u8 result = 0;
    u32 pokemon;
    u8* next;
    u16 itemType;
    u8* msg;
    u16 i;
    u32 ctx;

    ctx = (u32)rawCtx;
    pokemon = fightOutPokemonGetPokemonPtr(ctx);
    next = (u8*)(u32)fightOutPokemonGetSoubiItemDataId(ctx);
    itemType = fightOutPokemonGetSoubiItemSoubiDataId(ctx);
    figthOutPokemonGetSoubiItemBuff(ctx);
    msg = 0;

    if (fightOutPokemonCheckFightOut(ctx) == 0) {
        return 1;
    }

    fightFloorSetStatus(0, 0, 0x56, 0, (u16)(u32)next);

    switch ((u16)itemType) {
    case 2: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 5) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 5);
            next = lbl_80379A3C;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 4: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 3) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            next = lbl_80379A5A;
        }
        msg = next;
        if (next != 0) {
            result = 1;
            break;
        }

        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 4) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            next = lbl_80379A5A;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 5: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 6) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 6);
            next = lbl_80379A78;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 6: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 7) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 7);
            next = lbl_80379A96;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 3: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 8) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 8);
            fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
            next = lbl_80379AB4;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 8: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 9);
            next = lbl_80379AD2;
        }
        msg = next;
        if (next != 0) {
            result = 2;
        }
        break;
    }
    case 0x1C: {
        next = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0xA) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0xA);
            next = lbl_80379AEE;
        }
        msg = next;
        if (next != 0) {
            fn_80119F50(0xA);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            lbl_80478D78[5] = 0;
            result = 2;
        }
        break;
    }
    case 9:
        if (fightOutPokemonIsJoutaiNormal(ctx) == 0 ||
            fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) == 1) {
            lbl_80478D78[5] = 0;
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 3) == 1 ||
                fightOutPokemonIsPokemonJoutaiDataId(ctx, 4) == 1) {
                fn_80119F50(3);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 8) == 1) {
                fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                fn_80119F50(8);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 5) == 1) {
                fn_80119F50(5);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 6) == 1) {
                fn_80119F50(6);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 7) == 1) {
                fn_80119F50(7);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) == 1) {
                fn_80119F50(9);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            pokemonInitJoutai(pokemon);
            fightOutPokemonWriteJoutaiDataId(ctx, 9);
            fightOutPokemonResetSeqStatus(ctx, 0);
            result = 1;
            msg = lbl_80379AEE;
        }
        break;
    case 0x17:
        for (i = 0; i < 7; i++) {
            if (pokemonGetStatus(ctx, 0, lbl_80279FD0[i], 0) < 6) {
                pokemonSetStatus(ctx, 0, lbl_80279FD0[i], 0, 6);
                result = 5;
            }
        }
        if (result != 0) {
            msg = lbl_80379B0C;
        }
        break;
    }

    if (result != 0) {
        fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
        fightFloorSetStatus(0, 0, 0x49, 0, ctx);
        if (result == 1 && fightOutPokemonIsUseHensinBuff(ctx) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7C, 0, 0);
        }
        if (msg != 0) {
            wsRunScript(lbl_8047B62C, msg, 0);
        }
    }
    return 1;
}
