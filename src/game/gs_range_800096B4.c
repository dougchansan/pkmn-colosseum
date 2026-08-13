/**
 * @file gs_range_800096B4.c
 * @brief gs-engine, 0x800096B4 - 0x8000BE74.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * fn_800096B4 (0x800096B4, size 0x23E0) remains asm-only; the ten small
 * debug-menu callback functions below are decompiled to real C.
 */
#include "dolphin/types.h"

extern void menuDbgItemCreate(void);
extern s32 menuOpen(u32 a, u32 b);
extern void heroAddPokecoupon(u32 a, s32 b);
extern void heroAddPokedoru(u32 a, s32 b);
extern u8 fn_801EF63C(void);
extern void pokemonInit(u8* a);
extern void heroCatchPokemon(u32 a, u8* b, u32 c, u32 d, u32 e);
extern u32 heroGetStatus(u8* a, u32 b, u32 c);
extern u8 lbl_80478840;
extern u8 lbl_803A1A48[];
extern u32 lbl_80478B80;
extern u32 lbl_80478BD8;
extern u32* lbl_80478E60;
extern u32* lbl_80478F08;
extern u32* lbl_80478F68;
extern u32* lbl_80478F90;
extern s32 lbl_8047A290;
extern f32 lbl_8047B6D0;
extern s32 lbl_8047E700;
extern void* memcpy(void* dst, const void* src, u32 size);

#define FN800096B4_CLAMP_MIN(v, lo) \
    do { if ((v) < (lo)) { (v) = (lo); } } while (0)
#define FN800096B4_CLAMP_MAX(v, hi) \
    do { if ((v) > (hi)) { (v) = (hi); } } while (0)
#define FN800096B4_CLAMP(v, lo, hi) \
    do { FN800096B4_CLAMP_MIN(v, lo); FN800096B4_CLAMP_MAX(v, hi); } while (0)

#pragma peephole off
s32 fn_800096B4(u32 arg0, s32 arg1, u8* arg2, u8* arg3, u8* arg4, u8* arg5) {
    extern u16 GSmsgGetGSchar();
    extern u16 fightTrainerGetStatus();
    extern void fightTrainerSetStatus();
    extern u8 fn_8001E224();
    extern u8 fn_80119DD0();
    extern void fn_8012173C();
    extern void fn_801217B4();
    extern s8 fn_8012182C();
    extern s8 fn_8012189C();
    extern void fn_8012190C();
    extern s16 fn_80121984();
    extern void fn_801219F4();
    extern u8 fn_80121A6C();
    extern void fn_80121B4C();
    extern u8 fn_80142984();
    extern s32 gamedataGetStatus();
    extern void menuCloseCustom();
    extern s32 menuOpenCustom();
    extern void menuSubCloseNumberInput();
    extern s8 pokemonCheckRare();
    extern u8 pokemonCheckValid();
    extern void pokemonCreate();
    extern u16 pokemonCreateRndFit();
    extern void pokemonDoItemSoubi();
    extern u16 pokemonGetJoutaiDataId();
    extern u16 pokemonGetNowLevelToExp();
    extern u16 pokemonGetSoubiItemDataId();
    extern u16 pokemonGetStatus();
    extern void pokemonGrowBasisStatus();
    extern void pokemonInitDarkPokemon();
    extern void pokemonInitJoutai();
    extern u8 pokemonIsDarkPokemon();
    extern void pokemonSetCatchStatus();
    extern void pokemonSetDarkPokemonStatus();
    extern void pokemonSetStatus();
    extern void pokemonSetTokuseiFlag();
    extern u8 pokemonWazaCheckValid();
    extern void pokemonWazaCreate();
    extern u8 pokemonWazaGetMaxPP();
    extern void pokemonWazaInit();

    const u16 stat_group_1[6] = { 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92 };
    const u16 stat_group_2[6] = { 0x93, 0x94, 0x95, 0x96, 0x97, 0x98 };
    struct SavedPokemon {
        u8 bytes[0x138];
    } saved;
    s32 catchSeed;
    s32 selection;
    s32 delta;
    u16 trainerId;
    s32 changed;
    s32 i;

    if (arg2 != NULL) {
        *arg2 = 0;
    }
    if (arg3 != NULL) {
        *arg3 = 0;
    }
    if (arg4 != NULL) {
        *arg4 = 0;
    }
    if (arg5 != NULL) {
        *arg5 = 0;
    }

    catchSeed = lbl_8047E700;
    saved = *(struct SavedPokemon*)arg0;

    if ((u8)pokemonCheckValid(arg0) == 0) {
        pokemonCreate(arg0, 0x115, 1, gamedataGetStatus(0, 1));
        pokemonSetCatchStatus(arg0, 0, 1, 0, 2, 0, &catchSeed);
    }
    trainerId = pokemonGetStatus(arg0, 0, 0xC9, 0);

    for (;;) {
        lbl_8047A290 = 0;
        selection = menuOpenCustom(0xD, 0, 0, 0, 1, 2, arg0, &lbl_8047A290);
        delta = lbl_8047A290;

        if (selection == -1) {
            menuCloseCustom(0xD, 0, 1);
            *(struct SavedPokemon*)arg0 = saved;
            return -1;
        }

        if (selection == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }
            menuCloseCustom(0x44, 0, 1);
            menuCloseCustom(0xD, 0, 1);

            if ((u8)pokemonCheckValid(arg0) == 0) {
                pokemonInit((u8*)arg0);
            } else {
                pokemonGetSoubiItemDataId(arg0);
                if (fn_80142984() == 0) {
                    pokemonDoItemSoubi(arg0, 0, 0);
                }
                if ((trainerId != 0) && (arg1 == 1)) {
                    u16 species = pokemonGetStatus(arg0, 0, 0x6E, 0);

                    fightTrainerSetStatus(0, trainerId, 0x15, 0, species);
                    fightTrainerSetStatus(0, trainerId, 0xE, 0, pokemonGetStatus(0, species, 1, 0));
                    fightTrainerSetStatus(0, trainerId, 0x11, 0, pokemonGetStatus(arg0, 0, 0x7A, 0));
                    for (i = 0; i < 6; i++) {
                        fightTrainerSetStatus(0, trainerId, 0xF, i, pokemonGetStatus(arg0, 0, stat_group_1[i], 0));
                        fightTrainerSetStatus(0, trainerId, 0x10, i, pokemonGetStatus(arg0, 0, stat_group_2[i], 0));
                    }
                    fightTrainerSetStatus(0, trainerId, 0x14, 0, pokemonGetStatus(arg0, 0, 0xB7, 0));
                    fightTrainerSetStatus(0, trainerId, 0x16, 0, pokemonGetSoubiItemDataId(arg0));
                    fightTrainerSetStatus(0, trainerId, 0x19, 0, pokemonGetStatus(arg0, 0, 0x99, 0));
                    fightTrainerSetStatus(0, trainerId, 0x1A, 0, pokemonGetStatus(arg0, 0, 0xBA, 0));
                    fightTrainerSetStatus(0, trainerId, 0x1B, 0, pokemonGetStatus(arg0, 0, 0xBF, 0));
                    for (i = 0; i < 4; i++) {
                        fightTrainerSetStatus(0, trainerId, 0x17, i, pokemonGetStatus(arg0, 0, 0x7F, i));
                        fightTrainerSetStatus(0, trainerId, 0x18, i, pokemonGetStatus(arg0, 0, 0x81, i));
                    }
                    fightTrainerSetStatus(0, trainerId, 0x13, 0, pokemonGetStatus(arg0, 0, 0xC3, 0));
                }
            }
            return 1;
        }

        if (delta == 0) {
            continue;
        }

        changed = 0;
        switch (selection) {
        case 0x0E: {
            s32 species = pokemonGetStatus(arg0, 0, 0x6E, 0) + delta;
            FN800096B4_CLAMP(species, 0, (s32)(*lbl_80478F90) - 1);
            pokemonSetStatus(arg0, 0, 0x6E, 0, species);
            pokemonSetStatus(arg0, 0, 0x77, 0, GSmsgGetGSchar(pokemonGetStatus(0, species, 1, 0)));
            pokemonSetTokuseiFlag(arg0, 0);
            changed = 1;
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x10: {
            s32 level = pokemonGetStatus(arg0, 0, 0x7A, 0) + delta;
            FN800096B4_CLAMP(level, 1, 0x64);
            pokemonSetStatus(arg0, 0, 0x7A, 0, level);
            changed = 1;
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x12: {
            s32 exp;
            if ((u32)(delta + 0x10000) == 1U) {
                exp = 1;
            } else if ((u32)delta == (u32)-1) {
                exp = 0x1E8480;
            } else if (fn_8001E224(&exp, 1, 0x32, 0x32, 0) == 0) {
                menuSubCloseNumberInput();
                break;
            } else {
                menuSubCloseNumberInput();
                FN800096B4_CLAMP(exp, 1, 0x1E8480);
            }
            pokemonSetStatus(arg0, 0, 0x79, 0, exp);
            changed = 1;
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x14: {
            s32 hp = pokemonGetStatus(arg0, 0, 0x83, 0) + delta;
            FN800096B4_CLAMP_MIN(hp, 0);
            if (hp > pokemonGetStatus(arg0, 0, 0x87, 0)) {
                hp = pokemonGetStatus(arg0, 0, 0x87, 0);
            }
            pokemonSetStatus(arg0, 0, 0x83, 0, hp);
            changed = 1;
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x1E: {
            s32 maxHp = pokemonGetStatus(arg0, 0, 0x87, 0) + delta;
            FN800096B4_CLAMP(maxHp, 0, 0x3E7);
            pokemonSetStatus(arg0, 0, 0x87, 0, maxHp);
            pokemonSetStatus(arg0, 0, 0x83, 0, maxHp);
            changed = 1;
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23: {
            static const u16 ids[] = { 0x88, 0x89, 0x8C, 0x8A, 0x8B };
            s32 v = pokemonGetStatus(arg0, 0, ids[selection - 0x1F], 0) + delta;
            FN800096B4_CLAMP(v, 0, 0x3E7);
            pokemonSetStatus(arg0, 0, ids[selection - 0x1F], 0, v);
            break;
        }
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29: {
            static const u16 ids[] = { 0x93, 0x94, 0x95, 0x98, 0x96, 0x97 };
            s32 v = pokemonGetStatus(arg0, 0, ids[selection - 0x24], 0) + delta;
            FN800096B4_CLAMP(v, 0, 0x1F);
            pokemonSetStatus(arg0, 0, ids[selection - 0x24], 0, v);
            changed = 1;
            if ((selection == 0x24) && (arg3 != NULL)) {
                *arg3 = 1;
            }
            break;
        }
        case 0x2A:
        case 0x2E:
        case 0x2F: {
            s32 sum = 0;
            s32 idx = selection - 0x2A;
            s32 v;
            for (i = 0; i < 6; i++) {
                sum += pokemonGetStatus(arg0, 0, stat_group_1[i], 0);
            }
            if ((delta <= 0) || ((u32)sum < 0x1FEU)) {
                v = pokemonGetStatus(arg0, 0, stat_group_1[idx], 0) + delta;
                FN800096B4_CLAMP(v, 0, 0xFF);
                pokemonSetStatus(arg0, 0, stat_group_1[idx], 0, v);
                changed = 1;
                if ((selection == 0x2A) && (arg3 != NULL)) {
                    *arg3 = 1;
                }
            }
            break;
        }
        case 0x2D: {
            s32 sum;
            s32 v;

            if (delta > 0) {
                sum = pokemonGetStatus(arg0, 0, 0x8D, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8E, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8F, 0);
                sum += pokemonGetStatus(arg0, 0, 0x90, 0);
                sum += pokemonGetStatus(arg0, 0, 0x91, 0);
                sum += pokemonGetStatus(arg0, 0, 0x92, 0);
                if ((u32)sum >= 0x1FE) {
                    break;
                }
            }
            v = pokemonGetStatus(arg0, 0, 0x90, 0) + delta;
            FN800096B4_CLAMP(v, 0, 0xFF);
            pokemonSetStatus(arg0, 0, 0x90, 0, v);
            changed = 1;
            break;
        }
        case 0x2C: {
            s32 sum;
            s32 v;

            if (delta > 0) {
                sum = pokemonGetStatus(arg0, 0, 0x8D, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8E, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8F, 0);
                sum += pokemonGetStatus(arg0, 0, 0x90, 0);
                sum += pokemonGetStatus(arg0, 0, 0x91, 0);
                sum += pokemonGetStatus(arg0, 0, 0x92, 0);
                if ((u32)sum >= 0x1FE) {
                    break;
                }
            }
            v = pokemonGetStatus(arg0, 0, 0x8F, 0) + delta;
            FN800096B4_CLAMP(v, 0, 0xFF);
            pokemonSetStatus(arg0, 0, 0x8F, 0, v);
            changed = 1;
            break;
        }
        case 0x2B: {
            s32 sum;
            s32 v;

            if (delta > 0) {
                sum = pokemonGetStatus(arg0, 0, 0x8D, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8E, 0);
                sum += pokemonGetStatus(arg0, 0, 0x8F, 0);
                sum += pokemonGetStatus(arg0, 0, 0x90, 0);
                sum += pokemonGetStatus(arg0, 0, 0x91, 0);
                sum += pokemonGetStatus(arg0, 0, 0x92, 0);
                if ((u32)sum >= 0x1FE) {
                    break;
                }
            }
            v = pokemonGetStatus(arg0, 0, 0x8E, 0) + delta;
            FN800096B4_CLAMP(v, 0, 0xFF);
            pokemonSetStatus(arg0, 0, 0x8E, 0, v);
            changed = 1;
            break;
        }
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34: {
            s32 slot = selection - 0x31;
            s32 move = pokemonGetStatus(arg0, 0, 0x7F, slot);
            if ((u32)(move - 0x164) > 1U) {
                move += delta;
                if (move < 0) {
                    pokemonWazaInit(arg0, slot);
                } else {
                    if (move >= 0x163) {
                        move = 0x162;
                    }
                    pokemonWazaCreate(arg0, slot, move);
                }
            }
            break;
        }
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38: {
            s32 slot = selection - 0x35;
            if (pokemonWazaCheckValid(arg0, slot) != 0) {
                s32 pp = pokemonGetStatus(arg0, 0, 0x80, slot) + delta;
                FN800096B4_CLAMP_MIN(pp, 0);
                if (pp > (s32)pokemonWazaGetMaxPP(arg0, slot)) {
                    pp = pokemonWazaGetMaxPP(arg0, slot);
                }
                pokemonSetStatus(arg0, 0, 0x80, slot, (u8)pp);
            }
            break;
        }
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C: {
            s32 slot = selection - 0x39;
            if (pokemonWazaCheckValid(arg0, slot) != 0) {
                s32 up = pokemonGetStatus(arg0, 0, 0x81, slot) + delta;
                FN800096B4_CLAMP(up, 0, 3);
                pokemonSetStatus(arg0, 0, 0x81, slot, (u8)up);
                pokemonSetStatus(arg0, 0, 0x80, slot, pokemonWazaGetMaxPP(arg0, slot));
            }
            break;
        }
        case 0x3E: {
            u8 flag = (pokemonGetStatus(arg0, 0, 0xB7, 0) == 1) ? 0 : 1;
            if (pokemonGetStatus(0, pokemonGetStatus(arg0, 0, 0x6E, 0), 0x17, 1) == 0) {
                flag = 0;
            }
            pokemonSetTokuseiFlag(arg0, flag);
            if (arg4 != NULL) {
                *arg4 = 1;
            }
            break;
        }
        case 0x40: {
            s32 item = pokemonGetSoubiItemDataId(arg0) + delta;
            FN800096B4_CLAMP(item, 0, (s32)lbl_80478BD8 - 1);
            pokemonDoItemSoubi(arg0, item, 1);
            if (arg5 != NULL) {
                *arg5 = 1;
            }
            break;
        }
        case 0x42: {
            s32 v;
            if ((u32)(delta + 0x10000) == 1U) {
                v = 0;
            } else if ((u32)delta == (u32)-1) {
                v = -1;
            } else if (fn_8001E224(&v, 1, 0x32, 0x32, 0) == 0) {
                menuSubCloseNumberInput();
                break;
            } else {
                menuSubCloseNumberInput();
            }
            pokemonSetStatus(arg0, 0, 0x6F, 0, v);
            changed = 1;
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x43: {
            s32 a = pokemonGetStatus(arg0, 0, 0xBA, 0) + delta;
            FN800096B4_CLAMP(a, 0, (s32)lbl_80478B80 - 1);
            pokemonSetStatus(
                arg0, 0, 0x6F, 0,
                pokemonCreateRndFit(
                    arg0,
                    a,
                    pokemonGetStatus(arg0, 0, 0xBF, 0),
                    pokemonCheckRare(arg0),
                    pokemonGetStatus(arg0, 0, 0x75, 0)));
            changed = 1;
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x44: {
            s32 b = pokemonGetStatus(arg0, 0, 0xBF, 0) + delta;
            FN800096B4_CLAMP(b, 0, (s32)(*lbl_80478E60) - 1);
            pokemonSetStatus(
                arg0, 0, 0x6F, 0,
                pokemonCreateRndFit(
                    arg0,
                    pokemonGetStatus(arg0, 0, 0xBA, 0),
                    b,
                    pokemonCheckRare(arg0),
                    pokemonGetStatus(arg0, 0, 0x75, 0)));
            changed = 1;
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x45:
            pokemonSetStatus(
                arg0, 0, 0x6F, 0,
                pokemonCreateRndFit(
                    arg0,
                    pokemonGetStatus(arg0, 0, 0xBA, 0),
                    pokemonGetStatus(arg0, 0, 0xBF, 0),
                    (delta > 0) ? 1 : 0,
                    pokemonGetStatus(arg0, 0, 0x75, 0)));
            changed = 1;
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        case 0x47: {
            s32 v;
            if ((u32)(delta + 0x10000) == 1U) {
                v = 0;
            } else if ((u32)delta == (u32)-1) {
                v = -1;
            } else if (fn_8001E224(&v, 1, 0x32, 0x32, 0) == 0) {
                menuSubCloseNumberInput();
                break;
            } else {
                menuSubCloseNumberInput();
            }
            pokemonSetStatus(arg0, 0, 0x75, 0, v);
            if (arg2 != NULL) {
                *arg2 = 1;
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x49: {
            s32 v = pokemonGetStatus(arg0, 0, 0x99, 0) + delta;
            FN800096B4_CLAMP(v, 0, 0xFF);
            pokemonSetStatus(arg0, 0, 0x99, 0, v);
            break;
        }
        case 0x4B: {
            s32 v = pokemonGetStatus(arg0, 0, 0xB5, 0) + delta;
            FN800096B4_CLAMP(v, 0, 0xFF);
            pokemonSetStatus(arg0, 0, 0xB5, 0, v);
            break;
        }
        case 0x4D: {
            s32 joutai = pokemonGetJoutaiDataId(arg0);
            joutai = (joutai == 0) ? (delta + 2) : (joutai + delta);
            FN800096B4_CLAMP(joutai, 0, 8);
            if (joutai < 3) {
                joutai = 0;
            }
            pokemonInitJoutai(arg0);
            if ((joutai != 0) && (fn_80121A6C(arg0, joutai) == 2)) {
                fn_801219F4(arg0, joutai, 0);
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0x4E: {
            s32 joutai = pokemonGetJoutaiDataId(arg0);
            if (joutai != 0) {
                if (joutai != 4) {
                    s32 max = fn_8012189C(arg0, joutai);
                    if (max >= 0) {
                        s32 v = max + delta;
                        FN800096B4_CLAMP(v, 0, 0x10);
                        fn_8012173C(arg0, joutai, v);
                        if (fn_8012182C(arg0, joutai) > v) {
                            fn_801217B4(arg0, joutai, v);
                        }
                        if (arg3 != NULL) {
                            *arg3 = 1;
                        }
                    }
                } else {
                    s32 v = fn_80121984(arg0, joutai) + delta;
                    FN800096B4_CLAMP(v, 0, fn_80119DD0(joutai));
                    fn_8012190C(arg0, joutai, v);
                    if (arg3 != NULL) {
                        *arg3 = 1;
                    }
                }
            }
            break;
        }
        case 0x4F: {
            s32 joutai = pokemonGetJoutaiDataId(arg0);
            if ((joutai != 0) && (joutai != 4) && (fn_8012189C(arg0, joutai) >= 0)) {
                s32 v = fn_8012182C(arg0, joutai) + delta;
                FN800096B4_CLAMP(v, 0, fn_8012189C(arg0, joutai));
                fn_801217B4(arg0, joutai, v);
                if (arg3 != NULL) {
                    *arg3 = 1;
                }
            }
            break;
        }
        case 0xA7B: {
            s32 v = pokemonGetStatus(arg0, 0, 0xC3, 0) + delta;
            FN800096B4_CLAMP(v, 0, (s32)(*lbl_80478F68) - 1);
            if (v == 0) {
                pokemonInitDarkPokemon(arg0);
            } else {
                pokemonSetStatus(arg0, 0, 0xC3, 0, v);
                pokemonSetDarkPokemonStatus(arg0, v);
            }
            if (arg3 != NULL) {
                *arg3 = 1;
            }
            break;
        }
        case 0xA7D:
            if (pokemonGetStatus(arg0, 0, 0xC3, 0) != 0) {
                s32 v;
                if ((u32)(delta + 0x10000) == 1U) {
                    v = -1;
                } else if ((u32)delta == (u32)-1) {
                    v = 0x639C;
                } else if (fn_8001E224(&v, 0, 0x32, 0x32, 0) == 0) {
                    menuSubCloseNumberInput();
                    break;
                } else {
                    menuSubCloseNumberInput();
                    if (v < -1) {
                        v = -1;
                    }
                    if ((f32)v > lbl_8047B6D0) {
                        v = 0x639C;
                    }
                }
                pokemonSetStatus(arg0, 0, 0xC5, 0, v);
                if (arg3 != NULL) {
                    *arg3 = 1;
                }
            }
            break;
        case 0xA7F:
            if (pokemonGetStatus(arg0, 0, 0xC3, 0) != 0) {
                s32 v;
                if ((u32)(delta + 0x10000) == 1U) {
                    v = 0;
                } else if ((u32)delta == (u32)-1) {
                    v = 0x1E8480;
                } else if (fn_8001E224(&v, 1, 0x32, 0x32, 0) == 0) {
                    menuSubCloseNumberInput();
                    break;
                } else {
                    menuSubCloseNumberInput();
                    FN800096B4_CLAMP(v, 0, 0x1E8480);
                }
                pokemonSetStatus(arg0, 0, 0xC6, 0, v);
            }
            break;
        case 0xA81:
            if (pokemonGetStatus(arg0, 0, 0xC3, 0) != 0) {
                s32 v = pokemonGetStatus(arg0, 0, 0xC7, 0) + delta;
                FN800096B4_CLAMP(v, 0, 0xFF);
                pokemonSetStatus(arg0, 0, 0xC7, 0, v);
            }
            break;
        case 0xA83:
            if (pokemonIsDarkPokemon(arg0) != 0) {
                if (delta > 0) {
                    fn_801219F4(arg0, 0x3E, 0);
                } else {
                    fn_80121B4C(arg0, 0x3E);
                }
                if (arg3 != NULL) {
                    *arg3 = 1;
                }
            }
            break;
        case 0x5F8:
            if ((trainerId != 0) && (arg1 == 1)) {
                s32 v = fightTrainerGetStatus(0, trainerId, 0x1C, 0) + delta;
                FN800096B4_CLAMP(v, 0, 3);
                fightTrainerSetStatus(0, trainerId, 0x1C, 0, v);
            }
            break;
        case 0x5F9:
            if ((trainerId != 0) && (arg1 == 1)) {
                s32 v = fightTrainerGetStatus(0, trainerId, 0x1D, 0) + delta;
                FN800096B4_CLAMP(v, 0, (s32)(*lbl_80478F08) - 1);
                fightTrainerSetStatus(0, trainerId, 0x1D, 0, v);
            }
            break;
        default:
            break;
        }

        if (changed != 0) {
            pokemonSetTokuseiFlag(arg0, pokemonGetStatus(arg0, 0, 0xB7, 0));
            pokemonGrowBasisStatus(
                arg0,
                (selection == 0x10) ? pokemonGetNowLevelToExp(arg0)
                                    : pokemonGetStatus(arg0, 0, 0x79, 0));
        }
    }
}

/* fn_8000BA94 - 0x8000BA94 | size: 0x24 */
#pragma scheduling off
#pragma peephole on
s32 fn_8000BA94(void) {
    menuDbgItemCreate();
    return 0;
}
#pragma scheduling on

/* fn_8000BAB8 - 0x8000BAB8 | size: 0x48 */
#pragma peephole off
s32 fn_8000BAB8(void) {
    s32 val = menuOpen(2, 1);
    if (val == -1) { return 0; }
    heroAddPokecoupon(0, val);
    return 0;
}
#pragma peephole on

/* fn_8000BB00 - 0x8000BB00 | size: 0x48 */
#pragma peephole off
s32 fn_8000BB00(void) {
    s32 val = menuOpen(2, 1);
    if (val == -1) { return 0; }
    heroAddPokedoru(0, val);
    return 0;
}
#pragma peephole on

/* dbgMenuHeroPokemonAdd - 0x8000BB48 | size: 0xA4 */
s32 dbgMenuHeroPokemonAdd(void) {
    if ((u8)fn_801EF63C() == 1) { return -1; }
    if (lbl_80478840 != 0) {
        pokemonInit(lbl_803A1A48);
        lbl_80478840 = 0;
    }
    if (fn_800096B4((u32)lbl_803A1A48, 0, 0, 0, 0, 0) < 0) { return -1; }
    heroCatchPokemon(0, lbl_803A1A48, 0, 4, 1);
    return -1;
}

/* fn_8000BBEC - 0x8000BBEC | size: 0x6C */
s32 fn_8000BBEC(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 5);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}

/* fn_8000BC58 - 0x8000BC58 | size: 0x6C */
s32 fn_8000BC58(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 4);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}

/* fn_8000BCC4 - 0x8000BCC4 | size: 0x6C */
s32 fn_8000BCC4(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 3);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}

/* fn_8000BD30 - 0x8000BD30 | size: 0x6C */
s32 fn_8000BD30(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 2);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}

/* fn_8000BD9C - 0x8000BD9C | size: 0x6C */
s32 fn_8000BD9C(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 1);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}

/* fn_8000BE08 - 0x8000BE08 | size: 0x6C */
s32 fn_8000BE08(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = heroGetStatus(NULL, 3, 0);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
