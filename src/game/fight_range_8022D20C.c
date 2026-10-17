#include "dolphin/types.h"

extern u8 lbl_80478D78[8];
extern u32 lbl_8047B618;
extern u8 lbl_8047B628;
extern void* lbl_8047B62C;
extern u8 lbl_80379945[];
extern u8 lbl_8037994A[];

extern void wsRunScript(void* context, void* script, u8 preserveState);

void _wsCheckSynchronizeAttacker(void* context) {
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonCheckFightOut();
    extern void fightFloorSetStatus();
    u16 value = fightOutPokemonGetTokuseiDataId(context);
    u8 state;

    if ((u8)fightOutPokemonCheckFightOut(context) != 0 && value == 0x1C &&
        (lbl_8047B618 & 0x4000) != 0) {
        state = lbl_8047B628;
        lbl_8047B618 = lbl_8047B618 & ~0x4000;
        state = state & 0x3F;
        lbl_8047B628 = state;
        if (state == 6) {
            lbl_8047B628 = 2;
        }
        lbl_80478D78[3] = lbl_8047B628;
        fightFloorSetStatus(0, 0, 0x4B, 0, (u32)context);
        lbl_8047B618 = lbl_8047B618 | 0x2000;
        wsRunScript(lbl_8047B62C, (void*)&lbl_80379945, 0);
    }
}

void fn_8022D2CC(void* attacker, void* context) {
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonCheckFightOut();
    extern void fightFloorSetStatus();
    u16 value = fightOutPokemonGetTokuseiDataId(context);
    u8 state;

    if ((u8)fightOutPokemonCheckFightOut(context) != 0 && value == 0x1C &&
        (lbl_8047B618 & 0x4000) != 0) {
        state = lbl_8047B628;
        lbl_8047B618 = lbl_8047B618 & ~0x4000;
        state = state & 0x3F;
        lbl_8047B628 = state;
        if (state == 6) {
            lbl_8047B628 = 2;
        }
        lbl_80478D78[3] = lbl_8047B628 + 0x40;
        fightFloorSetStatus(0, 0, 0x4B, 0, (u32)context);
        lbl_8047B618 = lbl_8047B618 | 0x2000;
        wsRunScript(lbl_8047B62C, (void*)&lbl_80379945, 0);
    }
}

u32 _fightTokuseiCureJoutaiSub(u32 pokemon) {
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fn_80119F50();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightFloorSetStatus();
    u8 result;
    u32 ability;
    u32 pokemonPtr;

    ability = pokemon;
    while (1) {
        if (fightOutPokemonCheckFightOut(ability) == 0) {
            return 1;
        }

        pokemon = fightOutPokemonGetTokuseiDataId(ability);
        pokemonPtr = fightOutPokemonGetPokemonPtr(ability);
        result = 0;

        switch ((u16)pokemon) {
        case 0x11:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 3) == 1 || fightOutPokemonIsPokemonJoutaiDataId(ability, 4) == 1) {
                fn_80119F50(3);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 1;
            }
            break;
        case 0x14:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 9) == 1) {
                fn_80119F50(9);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 2;
            }
            break;
        case 7:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 5) == 1) {
                fn_80119F50(5);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 1;
            }
            break;
        case 0xF:
        case 0x48:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 8) == 1) {
                fightOutPokemonWriteJoutaiDataId(ability, 0x17);
                fn_80119F50(8);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 1;
            }
            break;
        case 0x29:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 6) == 1) {
                fn_80119F50(6);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 1;
            }
            break;
        case 0x28:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 7) == 1) {
                fn_80119F50(7);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 1;
            }
            break;
        case 0xC:
            if (fightOutPokemonIsPokemonJoutaiDataId(ability, 10) == 1) {
                fn_80119F50(10);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
                result = 3;
            }
            break;
        }

        if (result == 0) {
            goto return_one;
        }

        switch (result) {
        case 1:
            pokemonInitJoutai(pokemonPtr);
            fightOutPokemonResetSeqStatus(ability, 0);
            goto apply_hensin;
        case 2:
            fightOutPokemonWriteJoutaiDataId(ability, 9);
            goto apply_hensin;
        case 3:
            fightOutPokemonWriteJoutaiDataId(ability, 10);
        default:
        apply_hensin:
            if (fightOutPokemonIsUseHensinBuff(ability) == 1) {
                fightOutPokemonSetHensinPokemonStatusId(ability, 0x7C, 0, 0);
            }
            fightFloorSetStatus(0, 0, 0x4B, 0, ability);
            break;
        }

        wsRunScript(lbl_8047B62C, lbl_8037994A, 0);
    }

return_one:
    return 1;
}
