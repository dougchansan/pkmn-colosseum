/**
 * @file pda_exact_8003C7C0.c
 * @brief PDA party-select scene thread, 0x8003C7C0 - 0x8003CE1C.
 *
 * Standalone copy of fn_8003C7C0 from pda_range_80037158.c. Unit flag
 * -opt nopeephole replaces the shared file's local "peephole off".
 */
#include "dolphin/types.h"

typedef struct PdaModelWindow {
    u8 pad00[0x8];
    s32 field_08;
    s32 field_0C;
    s32 field_10;
    u8 pad14[0x4];
    f32 field_18;
    u8 pad1C[0xC];
    f32 field_28;
    u8 pad2C[0x18];
    f32 alphaScale;
    u8 pad48[0x4C];
    s8 variant;
    u8 pad95[0xB];
} PdaModelWindow; /* lbl_803A6748, size 0xA0 */

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern PdaModelWindow lbl_803A6748;
extern u8 lbl_802EF0A8[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern s32 lbl_8047A4DC;
extern f32 lbl_8047BAC4;
extern f32 lbl_8047BAC8;
extern f32 lbl_8047BAD4;
extern u8 lbl_803A6610[];

extern u16 GSmemGetHandleByPtr(void* ptr);
extern void GSmemFree(u16 handle);
extern void GSmemUnlock(u16 handle);
extern u32 soundStartRaw(u32, u32, u32);
extern s32 darkPokemonGetFlag1C(u16 id);
extern u32 darkPokemonSaveGet8(u16 id);
extern u8 darkPokemonCheckFlag16(u16 id);
extern u8 darkPokemonCheckFlag12(u16 id);
extern u8 darkPokemonCheckFlag14(u16 id);
extern void darkPokemonSetFlag0C(u16 id, s32 flag);
extern u32 gamedataGetStatus();
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);
extern void pdaSnagListBuild(void);
extern void menuModelInit(void* work, s32 w, s32 h);
extern void menuPokemonModelRequest(void* work, u32 pokemon);
extern s32 menuOpen(s32 id, s32 flag);
extern void menuCloseCustom(s32 slot, s32 arg1, s32 arg2);
extern u32 pokemonCreateRndFit(void* work, s32 a, s32 b, s32 c, s32 d);
extern void winMsgClose(s32 id);
extern void menuModelTerm(void* work);

void pdaSnagListRun(void)
{
    extern void _threadSwitch(void);
    u32 work;
    u8* model;
    s32 state;
    s32 running;
    s32 idx;
    f32 step;
    f32 cur;
    u32 rnd;
    u16 species;
    u16 workHandle;
    u16 listHandle;
    s32 mode;
    u16 entry;
    u16 rndId;
    u8 shadow;
    u8 snagged;
    s16* dim;

    state = 0;
    running = 1;
    pdaSnagListBuild();
    *(f32*)((u8*)&lbl_803A6748 + 0x14) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x18) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x1c) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x20) = lbl_8047BAC4;
    *(s32*)((u8*)&lbl_803A6748 + 0x0) = 0;
    *(s32*)((u8*)&lbl_803A6748 + 0x4) = 0;
    *(s32*)((u8*)&lbl_803A6748 + 0x8) = 0;
    *(f32*)((u8*)&lbl_803A6748 + 0x3c) = lbl_8047BAC4;
    *(s8*)((u8*)&lbl_803A6748 + 0x40) = 0;
    *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
    pokemonCreate((u32)lbl_803A6610, 1, 10, gamedataGetStatus(0, 1));
    *(u32*)((u8*)&lbl_803A6748 + 0x98) =
        pokemonCreateRndFit(lbl_803A6610, -1, -1, 0, 0);
    *(f32*)((u8*)&lbl_803A6748 + 0x28) = lbl_8047BAC4;
    if (lbl_8047A4DC >= 10) {
        *(s32*)((u8*)&lbl_803A6748 + 0xc) = 10;
    } else {
        *(s32*)((u8*)&lbl_803A6748 + 0xc) = lbl_8047A4DC;
    }
    model = (u8*)&lbl_803A6748 + 0x4c;
    *(s32*)((u8*)&lbl_803A6748 + 0x10) = lbl_8047A4DC;
    *(f32*)((u8*)&lbl_803A6748 + 0x34) = *(f32*)((u8*)&lbl_803A6748 + 0x2c) =
        *(s16*)(lbl_802EF0A8 + 0x509e);
    *(f32*)((u8*)&lbl_803A6748 + 0x38) = *(f32*)((u8*)&lbl_803A6748 + 0x30) =
        *(s16*)(lbl_802EF0A8 + 0x50a0);
    dim = (s16*)(lbl_802EF0A8 + 0x17ca6);
    menuModelInit(model, dim[0], dim[1]);

    while (1) {
        switch (state) {
        case 0:
            menuOpen(0x2f, 0);
            menuOpen(0x2e, 0);
            state = 1;
            break;
        case 1:
            menuOpen(0x30, 0);
            menuOpen(0x2d, 0);
            menuOpen(0x2b, 0);
            menuOpen(0x2c, 0);
            if (menuOpen(0x32, 1) >= 0) {
                if (*(s32*)((u8*)&lbl_803A6748 + 0x10) != 0) {
                    state = 2;
                    soundStartRaw(0x24, 0, 0);
                    work = lbl_8047A4D0;
                    idx = *(s32*)&lbl_803A6748;
                    species = lbl_8047A4D4[idx].field_00;
                    entry = lbl_8047A4D4[(u16)idx].battleId;
                    rndId = lbl_8047A4D4[idx].battleId;
                    darkPokemonGetFlag1C(entry);
                    darkPokemonCheckFlag16(entry);
                    shadow = darkPokemonCheckFlag12(entry);
                    snagged = darkPokemonCheckFlag14(entry);
                    if (shadow != 0) {
                        if (snagged != 0) {
                            mode = 1;
                        } else {
                            mode = 0;
                        }
                    } else {
                        mode = 2;
                    }
                    pokemonCreate(work, species, 10, gamedataGetStatus(0, 1));
                    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
                    darkPokemonSaveGet8(rndId);
                    pokemonBiosSetRnd(work, rnd);
                    switch (mode) {
                    case 0:
                    case 2:
                        pokemonBiosSetDarkpokemonDataId(work, 1);
                        pokemonBiosSetDp(work, 10);
                        break;
                    case 1:
                        pokemonBiosSetDarkpokemonDataId(work, 0);
                        pokemonBiosSetDp(work, 0);
                        break;
                    default:
                        break;
                    }
                    menuPokemonModelRequest(model, lbl_8047A4D0);
                    *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
                } else {
                    state = 1;
                }
            } else {
                state = 5;
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
            }
            break;
        case 2:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                state = 3;
                menuCloseCustom(0x2f, 0, 1);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
            } else {
                _threadSwitch();
            }
            break;
        case 3:
            menuOpen(0x33, 0);
            if (menuOpen(0x34, 1) < 0) {
                state = 4;
                soundStartRaw(0x25, 0, 0);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
            }
            break;
        case 4:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                menuCloseCustom(0x33, 0, 1);
                darkPokemonSetFlag0C(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
                state = 0;
            } else {
                _threadSwitch();
            }
            break;
        case 5:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                state = 0x64;
            } else {
                _threadSwitch();
            }
            break;
        case 0x64:
            menuCloseCustom(0x30, 0, 1);
            menuCloseCustom(0x2f, 0, 1);
            menuCloseCustom(0x2d, 0, 1);
            menuCloseCustom(0x2b, 0, 1);
            menuCloseCustom(0x2c, 0, 1);
            menuCloseCustom(0x2e, 0, 1);
            menuCloseCustom(0x32, 0, 1);
            winMsgClose(1);
            running = 0;
            break;
        default:
            break;
        }
        if (running == 0) {
            break;
        }
    }
    menuModelTerm(model);
    if (lbl_8047A4D0 != 0) {
        workHandle = GSmemGetHandleByPtr((void*)lbl_8047A4D0);
        if (workHandle != 0) {
            GSmemUnlock(workHandle);
            GSmemFree(workHandle);
        }
        lbl_8047A4D0 = 0;
    }
    if (lbl_8047A4D4 != NULL) {
        listHandle = GSmemGetHandleByPtr(lbl_8047A4D4);
        if (listHandle != 0) {
            GSmemUnlock(listHandle);
            GSmemFree(listHandle);
        }
        lbl_8047A4D4 = NULL;
    }
}
