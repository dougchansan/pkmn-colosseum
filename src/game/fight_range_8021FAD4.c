/**
 * @file fight_range_8021FAD4.c
 * @brief wsCmd49 (move-script command), 0x8021FAD4 - 0x80220778, with its
 *        switch jump table (.data 0x8039A0A8 - 0x8039A0F0).
 *
 * Moved out of the shared fight_range_80211A00.c so the unit compiles only
 * this function and owns its jump table. In the shared file it sat inside an
 * optimize_for_size region; the unit builds at -O4,s instead.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);
extern u8 lbl_80478D78[8];

void wsCmd49(void)
{
    extern u32 fightFloorGetStatus();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightFloorReplaceFightOutPokemonAttackToDefense();
    extern void fightFloorSetStatus();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    u16* status;
    extern void fightMenuCloseMsg();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightOutPokemonCheckNoAttackFlag();
    extern u8 fightOutPokemonCheckValid();
    extern u8 fightOutPokemonIsAlly();
    extern u16 fightOutPokemonIsJoutaiKie();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u16 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetMotoWazaDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightOutPokemonInitJoutaiKie();
    extern void fightOutPokemonSetVisibility();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u8 fightWazaIsHit();
    extern u8 fightWazaIsJoutaiDataId();
    extern void fightWazaInitLoop();
    extern s32 pokemonGetStatus();
    /* RULE-EXCEPTION(user-approved): pokemonSetStatus prototype with a u16 last parameter (include/game/pokemon_evolution.h says u32) so the (u16) conversions follow the target move — see docs/RULE_EXCEPTIONS.md */
    extern void pokemonSetStatus(u32, int, int, int, u16);
    extern s8 pokemonSearchWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u8 fn_802025B8();
    extern s32 fightOutPokemonGetPokemonJoutai0C();
    extern void fightOutPokemonSetPokemonJoutai0C();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern void fn_8022D2CC();
    extern u8 _wsTokuseiAfterDamage();
    extern void _fightTokuseiCureJoutaiSub();
    extern void _wsCheckSynchronizeAttacker();
    extern u32 _wsSoubiItemAfterDamage();
    extern void _fightOutPokemonDoStockItemSub();
    extern void _fightOutPokemonSoubiItemEffectSub();
    extern void _WsWkcActSubJoutaiMigawari__FPvUsPv();
    extern void fn_801DA3CC();
    extern u8 lbl_8047B627;
    extern u8 lbl_80378D02[];
    extern u8 lbl_803792C9[];
    extern u8 lbl_80379BF4[];
    extern u32 lbl_80379BFF[];
    u16 floorStatus[1];
    u32 stopState;
    u8 modeValue;
    u32 mode;
    u32 attacker;
    u32 defender;
    u16 heldItem;
    u16 movePower;
    u32 usedMove;
    u16 moveEffect;
    u32 originalMove;
    u32 moveStatus;
    u8 moveType;
    u8 stop;
    u8 moveHit;
    s16 defenderValue11C;
    s16 defenderValue11E;
    u16 moveJoutai;
    u32 validOriginalMove;
    s8 count;
    u32 state;
    u32 other;
    u32 flags1;
    u32 flags2;

    floorStatus[0] = fightFloorGetStatus(0, 0, 0x14, 0);
    stop = 0;
    /* RULE-EXCEPTION(user-approved): codegen-only cast - the (u16) widening of the u8 script byte keeps stopState a plain u32 coloured as in retail (replaces a volatile local) - see docs/RULE_EXCEPTIONS.md */
    stopState = (u16)lbl_8047B610[2];
    mode = lbl_8047B610[1];
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    if (attacker != 0) {
        heldItem = fightOutPokemonGetSoubiItemSoubiDataId();
        moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
        moveHit = fightWazaIsHit(moveStatus);
        usedMove = fightOutPokemonGetUseWazaDataId(attacker);
        originalMove = fightOutPokemonGetMotoWazaDataId(attacker);
        validOriginalMove = originalMove & ~-((u16)originalMove == 0xFFFF);
        movePower = wazaGetStatus(moveStatus, 0, 0x30, 0, (u16)originalMove);
        moveType = wazaGetStatus(0, usedMove, 5, 0);
        moveEffect = wazaGetStatus(0, usedMove, 9, 0);
        moveJoutai = wazaGetStatus(0, usedMove, 7, 0);
    } else {
        heldItem = 0;
        moveStatus = 0;
        moveHit = 0;
        usedMove = 0;
        originalMove = 0;
        validOriginalMove = 0;
        movePower = 9;
        moveType = 1;
        moveEffect = 0;
        moveJoutai = 0;
    }

    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (defender != 0) {
        defenderValue11C = pokemonGetStatus(defender, 0, 0x11C, 0);
        defenderValue11E = pokemonGetStatus(defender, 0, 0x11E, 0);
    } else {
        defenderValue11C = 0;
        defenderValue11E = 0;
    }
    modeValue = (u8)mode;

    do {
        switch (lbl_8047B627) {
        case 0:
            if (fightOutPokemonIsPokemonJoutaiDataId(defender, 0x13) == 1 &&
                fightOutPokemonCheckFightOut(defender) == 1 &&
                attacker != defender &&
                fightOutPokemonIsAlly(attacker, defender) == 0 &&
                moveHit == 1 &&
                (defenderValue11C != 0 || defenderValue11E != 0) &&
                moveJoutai != 0) {
                count = pokemonGetStatus(defender, 0, 0xE6, 0);
                if (count < 12) {
                    pokemonSetStatus(defender, 0, 0xE6, 0, (s8)(count + 1));
                    wsRunScript(lbl_8047B62C, lbl_80378D02, 0);
                    stop = 1;
                }
            }
            lbl_8047B627++;
            break;

        case 1:
            if (fightOutPokemonIsPokemonJoutaiDataId(defender, 7) == 1 &&
                fightOutPokemonCheckFightOut(defender) == 1 &&
                attacker != defender &&
                pokemonGetStatus(defender, 0, 0x11E, 0) != 0 &&
                fightWazaIsHit(moveStatus) == 1 &&
                (u16)wazaGetStatus(0, usedMove, 3, 0) == 10) {
                fightOutPokemonWriteJoutaiDataId(defender, 7);
                if (fightOutPokemonIsUseHensinBuff(defender) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(defender, 0x7C, 0, 0);
                }
                wsRunScript(lbl_8047B62C, lbl_803792C9, 0);
                stop = 1;
            }
            lbl_8047B627++;
            break;

        case 2:
            fn_8022D2CC(attacker, defender);
            lbl_8047B627++;
            break;

        case 3:
            if (_wsTokuseiAfterDamage(attacker, defender) != 0) {
                stop = 1;
            }
            lbl_8047B627++;
            break;

        case 4:
            fightFloorLoopValidFightOutPokemon(0, _fightTokuseiCureJoutaiSub, 0, 0);
            lbl_8047B627++;
            break;

        case 5:
            _wsCheckSynchronizeAttacker(attacker, defender);
            lbl_8047B627++;
            break;

        case 6:
            if ((lbl_8047B618 & 0x02000000) != 0 && heldItem == 0x1D &&
                (u16)originalMove != 0xA5 && (u16)originalMove != 0x164 &&
                fn_802025B8(attacker, 0x36) == 2) {
                if ((u16)originalMove == 0xE2 && fightWazaIsJoutaiDataId(moveStatus, 0x45) == 1) {
                    lbl_8047B627++;
                    break;
                }
                fightOutPokemonWritePokemonJoutaiDataId(attacker, 0x36, 0);
                fightOutPokemonSetPokemonJoutai0C(attacker, 0x36, originalMove);
            }
            if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x36) == 1) {
                other = fightOutPokemonGetPokemonJoutai0C(attacker, 0x36);
                if (pokemonSearchWazaDataId(fightOutPokemonGetPokemonPtr(attacker), other) < 0) {
                    fightOutPokemonWriteJoutaiDataId(attacker, 0x36);
                }
            }
            lbl_8047B627++;
            break;

        case 7:
            fightFloorLoopValidFightOutPokemon(0, _fightOutPokemonDoStockItemSub, 0, 0);
            lbl_8047B627++;
            break;

        case 8:
            fightFloorLoopValidFightOutPokemon(0, _fightOutPokemonSoubiItemEffectSub, 0, 0);
            lbl_8047B627++;
            break;

        case 9:
            _wsSoubiItemAfterDamage(attacker, defender);
            lbl_8047B627++;
            break;

        case 10:
            if (fightOutPokemonCheckFightOut(attacker) == 1) {
                state = fightOutPokemonIsJoutaiKie(attacker);
                if (state != 0) {
                    if (state != 0x1F) {
                        fightOutPokemonSetVisibility(attacker, 0);
                    } else {
                        other = pokemonGetStatus(attacker, 0, 0xEE, 0);
                        fn_801DA3CC(other, 3);
                    }
                }
            }
            lbl_8047B627++;
            break;

        case 11:
            if (fightOutPokemonCheckFightOut(attacker) == 1 &&
                (moveHit == 0 || fightOutPokemonIsJoutaiKie(attacker) == 0 ||
                 fightOutPokemonCheckNoAttackFlag(attacker) == 1)) {
                fightOutPokemonInitJoutaiKie(attacker);
                pokemonSetStatus(attacker, 0, 0x115, 0, 1);
                fightOutPokemonSetVisibility(attacker, 1);
                other = pokemonGetStatus(attacker, 0, 0xEE, 0);
                if (other != 0) {
                    fn_801DA36C((void*)other, 3);
                }
            }
            lbl_8047B627++;
            break;

        case 12:
            if (fightOutPokemonCheckFightOut(defender) == 1 &&
                pokemonGetStatus(defender, 0, 0x115, 0) == 0 &&
                fightOutPokemonIsJoutaiKie(defender) == 0) {
                fightOutPokemonSetVisibility(defender, 1);
                other = pokemonGetStatus(defender, 0, 0xEE, 0);
                if (other != 0) {
                    fn_801DA36C((void*)other, 3);
                }
            }
            lbl_8047B627++;
            break;

        case 13:
            fightFloorLoopValidFightOutPokemon(0, _WsWkcActSubJoutaiMigawari__FPvUsPv, 0, 0);
            lbl_8047B627++;
            break;

        case 14:
            if ((lbl_8047B618 & 0x1000) != 0) {
                fightFloorReplaceFightOutPokemonAttackToDefense(0);
                lbl_8047B618 &= 0xFFFFEFFF;
                attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
                if (attacker != 0) {
                    heldItem = fightOutPokemonGetSoubiItemSoubiDataId();
                    moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
                    moveHit = fightWazaIsHit(moveStatus);
                    usedMove = fightOutPokemonGetUseWazaDataId(attacker);
                    originalMove = fightOutPokemonGetMotoWazaDataId(attacker);
                    validOriginalMove = originalMove & ~-((u16)originalMove == 0xFFFF);
                    movePower = wazaGetStatus(moveStatus, 0, 0x30, 0, (u16)originalMove);
                    moveType = wazaGetStatus(0, usedMove, 5, 0);
                    moveEffect = wazaGetStatus(0, usedMove, 9, 0);
                    moveJoutai = wazaGetStatus(0, usedMove, 7, 0);
                } else {
                    heldItem = 0;
                    moveStatus = 0;
                    moveHit = 0;
                    usedMove = 0;
                    originalMove = 0;
                    validOriginalMove = 0;
                    movePower = 9;
                    moveType = 1;
                    moveEffect = 0;
                    moveJoutai = 0;
                }
                defender = fightTargetGetPtrAsNowFightType(0x12, 0);
                if (defender != 0) {
                    defenderValue11C = pokemonGetStatus(defender, 0, 0x11C, 0);
                    defenderValue11E = pokemonGetStatus(defender, 0, 0x11E, 0);
                } else {
                    defenderValue11C = 0;
                    defenderValue11E = 0;
                }
            }

            if ((u16)wazaGetStatus(0, validOriginalMove, 9, 0) != 0x7F ||
                ((u16)wazaGetStatus(0, validOriginalMove, 9, 0) == 0x7F && moveHit == 0)) {
                pokemonSetStatus(attacker, 0, 0xEF, 0, (u16)originalMove);
            }
            if (fightOutPokemonCheckFightOut(attacker) == 1 &&
                (u16)wazaGetStatus(0, validOriginalMove, 9, 0) != 0x7F) {
                if ((lbl_8047B618 & 0x02000000) != 0) {
                    pokemonSetStatus(attacker, 0, 0xF0, 0, (u16)originalMove);
                    pokemonSetStatus(attacker, 0, 0xF1, 0, (u16)usedMove);
                } else {
                    pokemonSetStatus(attacker, 0, 0xF0, 0, 0xFFFF);
                    pokemonSetStatus(attacker, 0, 0xF1, 0, 0xFFFF);
                }
                if (fightOutPokemonCheckFightOut(defender) == 1) {
                    /* RULE-EXCEPTION(user-approved): carrier pointer - status only re-reads floorStatus[0]; going through it gives the read the register order retail has - see docs/RULE_EXCEPTIONS.md */
                    status = floorStatus;
                    state = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, status[0]);
                    pokemonSetStatus(defender, 0, 0xF2, 0, state);
                }
                if ((lbl_8047B618 & 0x02000000) != 0 && moveHit == 1) {
                    if ((u16)originalMove == 0xFFFF) {
                        pokemonSetStatus(defender, 0, 0xF3, 0, (u16)originalMove);
                    } else {
                        pokemonSetStatus(defender, 0, 0xF3, 0, (u16)usedMove);
                        /* RULE-EXCEPTION(user-approved): no-op (u16) cast on a u16 local for argument order — see docs/RULE_EXCEPTIONS.md */
                        pokemonSetStatus(defender, 0, 0xF4, 0, (u16)movePower);
                    }
                } else {
                    pokemonSetStatus(defender, 0, 0xF3, 0, 0xFFFF);
                }
            }
            lbl_8047B627++;
            break;

        case 15:
            if (fightOutPokemonCheckValid(attacker) == 1 &&
                (u8)wazaGetStatus(0, validOriginalMove, 0x11, 0) == 1 &&
                (lbl_8047B618 & 0x02000000) != 0 && attacker != defender &&
                fightOutPokemonCheckValid(defender) == 1 && moveHit == 1) {
                fightOutPokemonSetOumuWazaDataId(defender, attacker, originalMove);
            }
            lbl_8047B627++;
            break;

        case 16:
            if ((lbl_8047B618 & 0x80000) == 0 &&
                (u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 &&
                pokemonGetStatus(attacker, 0, 0x109, 0) == 0 &&
                moveType == 4 && (lbl_8047B618 & 0x200) == 0) {
                other = fightTargetGetPtrAsNowFightType(0xE, defender);
                if (fightOutPokemonCheckFightOut(other) == 1) {
                    fightFloorSetStatus(0, 0, 0x43, 0, other);
                    fightWazaInitLoop(moveStatus);
                    {
                    flags1 = lbl_8047B618 & 0xFFFFFFBF;
                    lbl_80478D78[3] = 0;
                    *(volatile u32*)&lbl_8047B618 = flags1;
                    flags2 = lbl_8047B618 & 0xFFFFBFFF;
                    *(volatile u32*)&lbl_8047B618 = flags2;
                    lbl_80478D78[6] = 0;
                    lbl_8047B618 = flags2 | 0x200;
                    lbl_8047B627 = 0;
                    wsRunScript(lbl_8047B62C, lbl_80379BF4, 0);
                    lbl_8047B610 = (u8*)lbl_80379BFF[moveEffect];
                    return;
                    }
                }
                lbl_8047B618 |= 0x200;
            }
            lbl_8047B627++;
            break;

        case 17:
            if (modeValue == 0) {
                fightMenuAllFightTrainerCloseStatusMenu(0);
                fightMenuAllFightOutPokemonCloseStatusMenu(0);
                fightMenuCloseMsg();
            }
            break;
        }

        if (modeValue == 1 && stop == 0) {
            lbl_8047B627 = 0x11;
        }
        if (modeValue == 2 && (u8)stopState == lbl_8047B627) {
            lbl_8047B627 = 0x11;
        }
    } while (lbl_8047B627 != 0x11 && stop == 0);

    if (lbl_8047B627 == 0x11 && stop == 0) {
        lbl_8047B610 += 3;
    }
}
