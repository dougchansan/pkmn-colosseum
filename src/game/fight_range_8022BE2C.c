/**
 * @file fight_range_8022BE2C.c
 * @brief Held-item effects, 0x8022BE2C - 0x8022D20C (fightOutPokemonDoSoubiItemCheck,
 *        fn_8022D084), with their switch tables (.data 0x8039A388 -
 *        0x8039A478).
 *
 * Standalone copy of the shared candidate body at GC/1.3 -O4,s with no
 * pragmas. What D17's 99.81% wall needed:
 * - the max HP (g, u16, assigned straight from the call so it keeps its own
 *   register web), the case-7 loop index (i) and the PP / stat loop value
 *   (pp) are separate locals, pp declared second and g where t4 was;
 * - the random pick is `ap = avail; ap += index; sel = *ap;` (base before
 *   the modulo operands);
 * - the used-move bit test goes through an isBitSet inline, which keeps the
 *   call result as the left operand of the and.
 *
 * RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own
 * initializer constants, read through type puns -- see
 * docs/RULE_EXCEPTIONS.md. The kinds table {1, 2, 3, 4, 5} is .sdata2
 * 0x8047E604 (4 mod 8; MWCC aligns each .sdata2 section to 8) and the
 * avail initializer {-1 x5} is .rodata 0x80279FF8, followed by
 * _fightClearRageSub's table at 0x8027A00C (4 mod 8), so neither can be owned here.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[8];
extern void* lbl_8047B62C;
extern void wsRunScript();
extern u16 lbl_80279FD0[8];
extern u8 lbl_80379B06[];

typedef struct {
    s32 v[5];
} S32x5;

/* Stand-ins for the kinds and avail initializers (see above). */
extern u32 lbl_8047E604;
extern u8 lbl_8047E608;
extern const S32x5 lbl_80279FF8;

static inline u8 isBitSet(u32 bits, u8 bit)
{
    return (bits & (1 << bit)) != 0;
}

u32 fightOutPokemonDoSoubiItemCheck(u32 ctx, u8 mode) {
    extern u16 lbl_80279FD0[8];
    extern u8  lbl_80379F58[];
    extern u8  lbl_80379A36[];
    extern u8  lbl_80379A54[];
    extern u8  lbl_80379A72[];
    extern u8  lbl_80379A90[];
    extern u8  lbl_80379AAE[];
    extern u8  lbl_80379ACC[];
    extern u8  lbl_80379AE8[];
    extern u8  lbl_80379B06[];
    extern u8  lbl_80379B22[];
    extern u8  lbl_80379B45[];
    extern u8  lbl_80379B5B[];
    extern u8  lbl_80379B82[];
    extern u8  lbl_80379BB4[];
    extern u8  lbl_80379BD1[];
    extern u32 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern s32 figthOutPokemonGetSoubiItemBuff();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightFloorSetStatus();
    extern u32 fightOutPokemonIsNokoriHpFollowing(u32, u16);
    extern u8  fightOutPokemonIsHpMantan();
    extern void wazaSetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonWazaCheckValid();
    extern u8  pokemonWazaGetMaxPP();
    extern u32 pokemonSetStatus();
    extern u8  fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonJoutai08();
    extern u8  fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId(u32, u32, u8, u32);
    extern s32 wazaGetStatus();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void tasteDataGetPtr();
    extern void tasteDataGetNigateMsgDataId();
    extern u16 fightOutPokemonMaxHpWaruValue(u32, u16);
    extern u32 fightOutPokemonGetTasteLike();
    extern u8  fn_802025B8();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern u16 GSrand(void);
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8  fightOutPokemonIsJoutaiNormal();
    extern void fn_80119F50();
    extern u32 pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern void wsRunScript();
    u32 t2;
    u32 pp;
    s32 c;
    u32 result = 0;
    u8* msg = 0;
    u8 mx;
    s32 sel;
    u8 selkind;
    s32 op;
    s32 sum;
    u16 f;
    u16 g;
    s32 i;
    u16 t3;
    u32 e;
    s32 amt;
    u16 pick;
    u8 npp;
    u8 kinds[5];
    u32 d;
    u8 selstatus;
    u32 t1;
    s32 jj;
    s32* ap;
    s32 avail[5];

    *(u32*)kinds = lbl_8047E604;
    kinds[4] = lbl_8047E608;
    *(S32x5*)avail = lbl_80279FF8;

    if ((u8)fightOutPokemonCheckFightOut(ctx) == 0) {
        return 0;
    }
    t1 = fightOutPokemonGetSoubiItemDataId(ctx);
    t2 = fightOutPokemonGetSoubiItemSoubiDataId(ctx);
    c = figthOutPokemonGetSoubiItemBuff(ctx);
    d = pokemonGetStatus(ctx, 0, 0xD9, 0);
    e = fightOutPokemonGetPokemonPtr(ctx);
    f = pokemonGetStatus(e, 0, 0x83, 0);
    g = pokemonGetStatus(e, 0, 0x87, 0);
    {
        extern void fightFloorSetStatus(u32, u32, u32, u32, u16);
        fightFloorSetStatus(0, 0, 0x56, 0, (u16)t1);
    }

    switch ((u16)t2) {
    case 1:
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            amt = c;
            if (f + c > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            result = 4;
            msg = lbl_80379B22;
        }
        break;
    case 7:
        if (mode == 0 || mode == 2) {
            t1 = fightPokemonGetPokemonPtr(pokemonGetStatus(ctx, 0, 0xD5, 0));
            for (i = 0; (u16)i < 4; i++) {
                if ((u8)pokemonWazaCheckValid(t1, i)) {
                    t3 = pokemonGetStatus(t1, 0, 0x7F, i);
                    pp = pokemonGetStatus(t1, 0, 0x80, i) & 0xFF;
                    if (pp == 0) {
                        break;
                    }
                }
            }
            if ((u16)i == 4) {
                break;
            }
            mx = pokemonWazaGetMaxPP(t1, i);
            sum = pp + c;
            npp = sum;
            if (sum > mx) {
                npp = mx;
            }
            pokemonSetStatus(t1, 0, 0x80, i, npp);
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x10) == 0 && fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x31) == 1 &&
                !isBitSet(fightOutPokemonGetPokemonJoutai08(ctx, 0x31), i) &&
                fightOutPokemonIsUseHensinBuff(ctx) == 1) {
                fightOutPokemonSetHensinPokemonStatusId(ctx, 0x80, i & 0xFF, 1);
            }
            wazaGetStatus(0, t3, 1, 0);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            result = 3;
            msg = lbl_80379B45;
        }
        break;
    case 0x17:
        for (t1 = 0; (u16)t1 < 7; t1++) {
            if ((s32)pokemonGetStatus(ctx, 0, lbl_80279FD0[(u16)t1], 0) < 6) {
                pokemonSetStatus(ctx, 0, lbl_80279FD0[(u16)t1], 0, 6);
                result = 5;
            }
        }
        if ((u8)result == 0) {
            break;
        }
        msg = lbl_80379B06;
        break;
    case 0x2B:
        if ((fightOutPokemonIsHpMantan(ctx) == 0 && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            amt = fightOutPokemonMaxHpWaruValue(ctx, 0x10);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            result = 4;
            msg = lbl_80379B5B;
        }
        break;
    case 0xA:
        t1 = 0;
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            tasteDataGetPtr(0);
            tasteDataGetNigateMsgDataId();
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            amt = fightOutPokemonMaxHpWaruValue(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            if ((s8)fightOutPokemonGetTasteLike(ctx, 0) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xB:
        t1 = 0;
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            tasteDataGetPtr(1);
            tasteDataGetNigateMsgDataId();
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            amt = fightOutPokemonMaxHpWaruValue(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            if ((s8)fightOutPokemonGetTasteLike(ctx, 1) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xC:
        t1 = 0;
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            tasteDataGetPtr(2);
            tasteDataGetNigateMsgDataId();
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            amt = fightOutPokemonMaxHpWaruValue(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            if ((s8)fightOutPokemonGetTasteLike(ctx, 2) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xD:
        t1 = 0;
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            tasteDataGetPtr(3);
            tasteDataGetNigateMsgDataId();
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            amt = fightOutPokemonMaxHpWaruValue(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            if ((s8)fightOutPokemonGetTasteLike(ctx, 3) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xE:
        t1 = 0;
        if (((u8)fightOutPokemonIsNokoriHpFollowing(ctx, 2) && mode == 0) ||
            (fightOutPokemonIsHpMantan(ctx) == 0 && mode == 2)) {
            tasteDataGetPtr(4);
            tasteDataGetNigateMsgDataId();
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            amt = fightOutPokemonMaxHpWaruValue(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            wazaSetStatus(d, 0, 0x2D, 0, -amt);
            if ((s8)fightOutPokemonGetTasteLike(ctx, 4) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xF:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 pokemonGetStatus(u32, u32, u32, u32);
                t1 = pokemonGetStatus(ctx, 0, 0xE6, 0);
            }
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x11;
                lbl_80379F58[0x160A4] = 0xF;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x10:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 pokemonGetStatus(u32, u32, u32, u32);
                t1 = pokemonGetStatus(ctx, 0, 0xE7, 0);
            }
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x12;
                lbl_80379F58[0x160A4] = 0x10;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x11:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 pokemonGetStatus(u32, u32, u32, u32);
                t1 = pokemonGetStatus(ctx, 0, 0xEA, 0);
            }
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x13;
                lbl_80379F58[0x160A4] = 0x11;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x12:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 pokemonGetStatus(u32, u32, u32, u32);
                t1 = pokemonGetStatus(ctx, 0, 0xE8, 0);
            }
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x14;
                lbl_80379F58[0x160A4] = 0x12;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x13:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 pokemonGetStatus(u32, u32, u32, u32);
                t1 = pokemonGetStatus(ctx, 0, 0xE9, 0);
            }
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x15;
                lbl_80379F58[0x160A4] = 0x13;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x14:
        if (mode == 0 || mode == 2) {
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) == 0) {
                break;
            }
            if (fn_802025B8(ctx, 0xF) != 2) {
                break;
            }
            fightOutPokemonWritePokemonJoutaiDataId(ctx, 0xF, 0);
            result = 2;
            msg = lbl_80379BD1;
        }
        break;
    case 0x15:
        if (mode == 0 || mode == 2) {
            if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) == 0) {
                break;
            }
            t3 = 0;
            for (jj = 0; (u16)jj < 5; jj++) {
                avail[(u16)jj] = -1;
            }
            for (pp = 0; (u16)pp < 5; pp++) {
                t1 = kinds[(u16)pp];
                switch (t1) {
                case 1: op = 0xE6; break;
                case 2: op = 0xE7; break;
                case 3: op = 0xEA; break;
                case 4: op = 0xE8; break;
                case 5: op = 0xE9; break;
                case 6: op = 0xEB; break;
                case 7: op = 0xEC; break;
                default: op = 0; break;
                }
                if ((u8)pokemonGetStatus(ctx, 0, op, 0) < 12) {
                    avail[t3] = t1;
                    t3++;
                }
            }
            if (t3 < 1) {
                break;
            }
            pick = GSrand() % t3;
            ap = avail;
            ap += pick;
            sel = *ap;
            if (sel == -1) {
                break;
            }
            selkind = sel;
            selstatus = selkind + 0x26;
            t2 = 0;
            if (mode == 0 || mode == 2) {
                switch (selkind) {
                case 1: op = 0xE6; break;
                case 2: op = 0xE7; break;
                case 3: op = 0xEA; break;
                case 4: op = 0xE8; break;
                case 5: op = 0xE9; break;
                case 6: op = 0xEB; break;
                case 7: op = 0xEC; break;
                default: op = 0; break;
                }
                {
                    extern u8 pokemonGetStatus(u32, u32, u32, u32);
                    t1 = pokemonGetStatus(ctx, 0, op, 0);
                }
                if ((u8)fightOutPokemonIsNokoriHpFollowing(ctx, c) && t1 < 12) {
                    lbl_80379F58[0x1601E] = selkind + 0x20;
                    lbl_80379F58[0x160A4] = selstatus;
                    lbl_80379F58[0x160A5] = 0;
                    t2 = (u32)lbl_80379BB4;
                }
            }
            msg = (u8*)t2;
            if (t2 == 0) {
                break;
            }
            result = 5;
        }
        break;
    case 2:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 5) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 5);
            t1 = (u32)lbl_80379A36;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 4:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 3) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            t1 = (u32)lbl_80379A54;
        }
        msg = (u8*)t1;
        if (t1 != 0) {
            result = 1;
            break;
        }
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 4) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            t1 = (u32)lbl_80379A54;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 5:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 6) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 6);
            t1 = (u32)lbl_80379A72;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 6:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 7) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 7);
            t1 = (u32)lbl_80379A90;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 3:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 8) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 8);
            fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
            t1 = (u32)lbl_80379AAE;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 8:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 9);
            t1 = (u32)lbl_80379ACC;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 2;
        break;
    case 9:
        if ((u8)fightOutPokemonIsJoutaiNormal(ctx)) {
            if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) != 1) {
                break;
            }
        }
        t2 = 0;
        lbl_80478D78[5] = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 3) == 1 || fightOutPokemonIsPokemonJoutaiDataId(ctx, 4) == 1) {
            fn_80119F50(3);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2 = 1;
        }
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 8) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
            fn_80119F50(8);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2++;
        }
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 5) == 1) {
            fn_80119F50(5);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2++;
        }
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 6) == 1) {
            fn_80119F50(6);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2++;
        }
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 7) == 1) {
            fn_80119F50(7);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2++;
        }
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 9) == 1) {
            fn_80119F50(9);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            t2++;
        }
        if ((u16)t2 >= 2) {
            lbl_80478D78[5] = 1;
        }
        pokemonInitJoutai(e);
        fightOutPokemonWriteJoutaiDataId(ctx, 9);
        fightOutPokemonResetSeqStatus(ctx, 0);
        result = 1;
        msg = lbl_80379AE8;
        break;
    case 0x1C:
        t1 = 0;
        if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0xA) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0xA);
            t1 = (u32)lbl_80379AE8;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        fn_80119F50(0xA);
        msgctrlSetValue(0xD, GSmsgGetGSchar());
        lbl_80478D78[5] = 0;
        result = 2;
        break;
    }

    if ((u8)result != 0) {
        fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
        fightFloorSetStatus(0, 0, 0x36, 0, ctx);
        fightFloorSetStatus(0, 0, 0x49, 0, ctx);
        switch ((u8)result) {
        case 2:
            if ((u8)result == 2) {
                break;
            }
            break;
        case 1:
            if (fightOutPokemonIsUseHensinBuff(ctx) == 1) {
                fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7C, 0, 0);
            }
            break;
        }
        if (msg != 0) {
            wsRunScript(lbl_8047B62C, msg, 0);
        }
    }
    return result;
}

u8 fn_8022D084(void* ctx) {
    extern u16 fightOutPokemonGetSoubiItemDataId();
    extern u16 fightOutPokemonGetSoubiItemSoubiDataId();
    extern void figthOutPokemonGetSoubiItemBuff();
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightOutPokemonCheckFightOut();
    extern u8 fightFloorSetStatus();
    extern void fightTrainerSetStatus();
    extern s32   pokemonGetStatus();
    extern void pokemonSetStatus();
    u16 val30;
    u16 val1;
    u8 result;
    void* tmp;
    u8 i;

    result = 0;
    val1 = fightOutPokemonGetSoubiItemDataId();
    val30 = fightOutPokemonGetSoubiItemSoubiDataId(ctx);
    figthOutPokemonGetSoubiItemBuff(ctx);
    tmp = (void*)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctx);

    if ((u8)fightOutPokemonCheckFightOut(ctx) == 0) {
        return 0;
    }
    fightFloorSetStatus(0, 0, 0x56, 0, val1);

    switch (val30) {
    case 0x20:
        fightTrainerSetStatus(tmp, 0, 0x48, 0, 2);
        break;
    case 0x17:
        for (i = 0; i < 7; i++) {
            if (pokemonGetStatus(ctx, 0, lbl_80279FD0[i], 0) < 6) {
                pokemonSetStatus(ctx, 0, lbl_80279FD0[i], 0, 6);
                result = 5;
            }
        }
        if (result != 0) {
            fightFloorSetStatus(0, 0, 0x4b, 0, (u32)ctx);
            fightFloorSetStatus(0, 0, 0x36, 0, (u32)ctx);
            fightFloorSetStatus(0, 0, 0x49, 0, (u32)ctx);
            wsRunScript(lbl_8047B62C, (void*)&lbl_80379B06, 0);
        }
        break;
    }
    return result;
}

