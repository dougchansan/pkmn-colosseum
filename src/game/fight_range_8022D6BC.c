/**
 * @file fight_range_8022D6BC.c
 * @brief Ability after-hit carve, 0x8022D6BC - 0x8022DCB8 (_wsTokuseiAfterDamage), with
 *        its switch table (.data 0x8039A478 - 0x8039A538), linked.
 *
 * Standalone copy of the shared candidate body, built with GC/1.3.2 at -O4,s
 * (configure.py) with no pragmas.
 */
#include "dolphin/types.h"

extern u8 lbl_80478D78[1];
extern u16 GSrand(void);
extern void wsRunScript(void*, void*, u8);

u32 _wsTokuseiAfterDamage(u32 attacker, u32 defender)
{
    extern u32 pokemonGetStatus();
    extern u16 wazaGetStatus();
    extern void wazaSetStatus();
    extern u16 fightFloorGetStatus();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetWazaZokuseiDataId();
    extern u8 fightWazaIsHit();
    extern u8 fightOutPokemonGetSex();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightOutPokemonSetZokuseiDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void fn_8010C4D4();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void wsRunScript();
    extern u8 fn_802025B8();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern u8 lbl_803798F3[];
    extern u8 lbl_80379907[];
    extern u8 lbl_8037992F[];
    extern u8 lbl_80379932[];
    extern void* lbl_8047B62C;
    extern u32 lbl_8047B618;
    extern const u16 lbl_8047E600;
    extern const u8 lbl_8047E602;

    s16 status11C;
    s16 status11E;
    u32 defenderAbility;
    u32 moveId;
    u16 moveJoutai;
    u32 moveType;
    u8 moveStatus13;
    u32 moveStatus;
    u8 moveHit;
    u8 status107;
    u32 result;
    u8 attackerSex;
    u8 defenderSex;
    u32 target;
    u32 attackerAbility;
    u8 values[3];

    status11C = pokemonGetStatus(defender, 0, 0x11C, 0);
    status11E = pokemonGetStatus(defender, 0, 0x11E, 0);
    attackerAbility = fightOutPokemonGetTokuseiDataId(attacker);
    defenderAbility = fightOutPokemonGetTokuseiDataId(defender);
    moveId = fightOutPokemonGetUseWazaDataId(attacker);
    moveJoutai = (u16)wazaGetStatus(0, moveId, 7, 0);
    moveType = fightOutPokemonGetWazaZokuseiDataId(attacker);
    moveStatus13 = wazaGetStatus(0, moveId, 0xD, 0);
    moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
    moveHit = fightWazaIsHit(moveStatus);
    status107 = pokemonGetStatus(attacker, 0, 0x107, 0);
    /* RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own
     * {1, 2, 5} table - see docs/RULE_EXCEPTIONS.md. It stays in
     * sdata2_8047E538.c: MWCC aligns each .sdata2 section to 8, so a carve
     * cannot own 0x8047E600 - 0x8047E604 while fightOutPokemonDoSoubiItemCheck's table follows
     * at 0x8047E604. */
    *(u16*)values = lbl_8047E600;
    values[2] = lbl_8047E602;
    result = 0;
    attackerSex = fightOutPokemonGetSex(attacker);
    defenderSex = fightOutPokemonGetSex(defender);
    target = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(
        defender, fightFloorGetStatus(0, 0, 0x14, 0));

    switch ((u16)defenderAbility) {
    case 0x10:
        if (moveHit == 1 && (u16)moveId != 0xA5 && moveJoutai != 0 &&
            (u16)moveId != 0x164 && (status11C != 0 || status11E != 0) &&
            fightOutPokemonIsZokuseiDataId(defender, moveType) == 0 &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            u32 i;
            for (i = 0; (u8)i < 2; i++) {
                fightOutPokemonSetZokuseiDataId(defender, i, moveType);
            }
            fn_8010C4D4(moveType);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            wsRunScript(lbl_8047B62C, lbl_803798F3, 0);
            result = 1;
        }
        break;

    case 0x18:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            wazaSetStatus(moveStatus, 0, 0x2D, 0,
                          fightOutPokemonMaxHpWaruValue(attacker, 0x10));
            wsRunScript(lbl_8047B62C, lbl_80379907, 0);
            result = 1;
        }
        break;

    case 0x1B:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (GSrand() % 10 == 0) {
                u32 pick = (u16)GSrand();
                u32 pickDivisor = 3;
                lbl_80478D78[3] = values[(u8)(pick % pickDivisor)];
                lbl_8047B618 |= 0x2000;
                lbl_80478D78[3] += 0x40;
                wsRunScript(lbl_8047B62C, lbl_8037992F, 0);
                result = 1;
            }
        }
        break;

    case 0x26:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (GSrand() % 3 == 0) {
                u32 value = 0x42;
                lbl_80478D78[3] = value;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                value = 0;
                wsRunScript(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 9:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (GSrand() % 3 == 0) {
                u32 value = 0x45;
                lbl_80478D78[3] = value;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                value = 0;
                wsRunScript(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 0x31:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && moveStatus13 != 0 &&
            (status11C != 0 || status11E != 0)) {
            if (GSrand() % 3 == 0) {
                u32 value = 0x43;
                lbl_80478D78[3] = value;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                value = 0;
                wsRunScript(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 0x38:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && moveStatus13 != 0 &&
            (status11C != 0 || status11E != 0) &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            if (GSrand() % 3 == 0 &&
                (u16)attackerAbility != 0xC &&
                attackerSex != defenderSex && fn_802025B8(attacker, 0xA) == 2 &&
                attackerSex != 2 && defenderSex != 2) {
                fightOutPokemonWritePokemonJoutaiDataId(attacker, 0xA, target);
                wsRunScript(lbl_8047B62C, lbl_80379932, 0);
                result = 1;
            }
        }
        break;
    }

    return result;
}
