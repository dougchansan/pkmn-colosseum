/**
 * PC/People menu driver and save path, 0x8003842C - 0x80038E74.
 *
 * RULE-EXCEPTION(user-approved): GC/1.3.2 C++ compilation, pooled table
 * ownership, helper-local allocation, unit-level peephole disabling and
 * scoped compiler controls reproduce retail code. The load-progress and
 * save routines retain the byte-verified shared conversion-bias imports.
 * See docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern "C" {

extern void* lbl_8047A480;
extern s8 lbl_8047A490;
extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern f32 lbl_8047A488;
extern f32 lbl_8047A484;

u8 lbl_802E51C8[96] = {
    0x00, 0x00, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x28, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x50, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x78, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x90, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
u8 lbl_802E5228[96] = {
    0x00, 0x00, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x68, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x88, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x9C, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0xB0, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
u8 lbl_802E5288[32] = {
    0x42, 0x28, 0x00, 0x00, 0x43, 0x33, 0x00, 0x00, 0x42, 0x50, 0x00, 0x00,
    0x43, 0x6A, 0x00, 0x00, 0x42, 0x78, 0x00, 0x00, 0x43, 0x90, 0x80, 0x00,
    0x42, 0x90, 0x00, 0x00, 0x43, 0xAC, 0x00, 0x00,
};
u8 lbl_802E52A8[16] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0xC9, 0x0F, 0xDB, 0x40, 0x49, 0x0F, 0xDB,
    0x40, 0x96, 0xCB, 0xE4,
};
u8 lbl_803A6498[0x48];
u8 lbl_803A64E0[12];
u8 lbl_803A64EC[0x60];
u8 lbl_803A654C[0x64];
/* The same four-row reset is expanded at each menu-choice transition. */
static inline void pdaResetPeopleTables(void)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    s32 i;
    u8* src;
    u8* dst;
    lbl_8047A480 = (void*)0x1b5a;
    src = lbl_802E51C8;
    dst = lbl_803A654C;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = lbl_802E5228;
    dst = lbl_803A64EC;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)lbl_802E52A8;
    lbl_8047A484 = *(f32*)lbl_802E52A8;
    lbl_8047A47D = 0;
}
/* Separate second-loop locals retain the initial expansion's allocation. */
static inline void pdaResetPeopleTablesFirst(void)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    u8* rowDest;
    s32 rowIndex;
    u8* rowSource;
    s32 i;
    u8* src;
    lbl_8047A480 = (void*)0x1b5a;
    src = lbl_802E51C8;
    dst = lbl_803A654C;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    rowSource = lbl_802E5228;
    rowDest = lbl_803A64EC;
    for (rowIndex = 0; rowIndex < 4; rowSource += 0x18, rowDest += 0x18, rowIndex++) {
        memcpy(rowDest, rowSource, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)lbl_802E52A8;
    lbl_8047A484 = *(f32*)lbl_802E52A8;
    lbl_8047A47D = 0;
}

#pragma push
/* RULE-EXCEPTION(user-approved): retain retail copies and local allocation;
 * both settings are recorded in docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
#pragma opt_lifetimes off
extern void menuCloseCustom(s32, s32, s32);
void std024_dialogDisplayPCBoxMenu(void)
{
    extern u8 lbl_802EF0A8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void soundDuck(s32 a, s32 b, s32 c, s32 d);
    extern void soundUnduck(s32 a, s32 b, s32 c);
    extern void floorBattleEndTerminateScripts(void);
    extern void pcBoxStorageMain(void);
    extern void itemStorageRun(void);
    extern s32 pcMenuSave(void);
    extern void menuModelInit(u8* model, s16 a, s16 b);
    extern void menuModelSetMotion(u8* model, s32 motion);
    extern void menuPokemonModelRequestById(u8* model, s32 id);
    extern void menuModelTerm(u8* model);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern void peopleInfoGetMotion(void* info, s32 kind, s32* out0, s32* out1);
    s32 state;
    s32 running;
    s32 choice;
    s32 motion;
    s32 scratch;

    state = 0;
    running = 1;
    soundDuck(0x55, 0x1f4, 1, 1);
    do {
        switch (state) {
        case 0:
            pdaResetPeopleTablesFirst();
            menuOpen(0x18, 0);
            menuOpen(0x1e, 0);
            menuOpen(0x1f, 0);
            menuOpen(0x1a, 0);
            menuOpen(0x1b, 0);
            menuOpen(0x19, 0);
            state = 1;
            break;
        case 1:
            lbl_8047A490 = -1;
            menuOpen(0x20, 0);
            menuOpen(0x21, 0);
            menuOpen(0x22, 0);
            menuOpen(0x23, 0);
            choice = menuOpen(0x1d, 1);
            if (choice >= 0) {
                lbl_8047A490 = (s8)choice;
                switch (choice) {
                case 0:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0xa;
                    break;
                case 1:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0x14;
                    break;
                case 2:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0x1e;
                    break;
                case 3:
                    state = 0x3e8;
                    break;
                }
            } else {
                state = 0x3e8;
            }
            break;
        case 0xa:
            pcBoxStorageMain();
            state = 1;
            break;
        case 0x14:
            lbl_8047A480 = (void*)0x1b61;
            itemStorageRun();
            lbl_8047A480 = (void*)0x1b5a;
            state = 1;
            break;
        case 0x1e:
            menuModelInit(lbl_803A6498, (s16)(*(s16*)(lbl_802EF0A8 + 0x5fd6) + 0xa),
            *(s16*)(lbl_802EF0A8 + 0x5fd8));
            menuPokemonModelRequestById(lbl_803A6498, 0xf70400);
            peopleInfoGetMotion(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &scratch);
            menuModelSetMotion(lbl_803A6498, motion);
            choice = pcMenuSave();
            menuModelTerm(lbl_803A6498);
            state = choice;
            break;
        case 0x28:
            state = 0x3e8;
            break;
        case 0x3e8:
            menuCloseCustom(0x1d, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    soundUnduck(0x3e8, 1, 1);
    floorBattleEndTerminateScripts();
}
#pragma pop

#pragma push
/**
 * @file pda_exact_800388C4.c
 * @brief Byte-exact PDA teardown helper, 0x800388C4 - 0x80038990.
 */

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
void pcCloseAllMenus(void)
{
    extern void menuCloseCustom(s32 slot, s32 arg1, s32 arg2);

    menuCloseCustom(0x19, 0, 1);
    menuCloseCustom(0x1a, 0, 1);
    menuCloseCustom(0x1b, 0, 1);
    menuCloseCustom(0x18, 0, 1);
    menuCloseCustom(0x1e, 0, 1);
    menuCloseCustom(0x1f, 0, 1);
    menuCloseCustom(0x20, 0, 1);
    menuCloseCustom(0x21, 0, 1);
    menuCloseCustom(0x22, 0, 1);
    menuCloseCustom(0x23, 0, 1);
    menuCloseCustom(0x1d, 0, 1);
}
#pragma scheduling reset
#pragma pop

#pragma push
/* RULE-EXCEPTION(user-approved): temporary shared conversion-bias imports;
 * see docs/RULE_EXCEPTIONS.md. */
/**
 * @file pda_candidate_80038990.c
 * @brief Byte-exact PDA load-progress thread, 0x80038990 - 0x80038A00.
 */

extern f32 lbl_8047A494;

void pcFrameStepTask(void)
{
    extern s32 GSgfxGetVsyncRate(void);
    extern u32 GSgetFrameStep(void);
    extern void _threadSwitch(void);

    while (1) {
        lbl_8047A494 = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
        _threadSwitch();
    }
}
#pragma pop

#pragma push
/** Byte-exact PDA angle reset, 0x80038A00 - 0x80038A0C. */

extern f32 lbl_8047A484;
extern f32 lbl_8047BA58;

void pcResetTimer8047A484(void)
{
    lbl_8047A484 = lbl_8047BA58;
}
#pragma pop

extern u32 lbl_8047A498;
extern u32 lbl_8047A49C;
extern u32 lbl_8047A4A0;
#pragma push
/* RULE-EXCEPTION(user-approved): retain the retail copy and loop allocation;
 * the comma assignment and helper-local shaping are documented in
 * docs/RULE_EXCEPTIONS.md. The conversion bias uses constant_import. */
#pragma peephole off
static inline void pdaResetPeopleTablesSave(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    s32 i;
    u8* src;
    lbl_8047A480 = (void*)0x1b5a;
    src = lbl_802E51C8;
    dst = lbl_803A654C;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    s32 rowIndex;
    u8* rowSource;
    u8* rowDest;
    rowSource = lbl_802E5228;
    rowDest = lbl_803A64EC;
    for (rowIndex = 0; rowIndex < 4; rowSource += 0x18, rowDest += 0x18, rowIndex++) {
        memcpy(rowDest, rowSource, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)lbl_802E52A8;
    lbl_8047A484 = *(f32*)lbl_802E52A8;
    lbl_8047A47D = 0;
}

static inline void pdaFillSavePrompt(void)
{
    extern u32 gamedataGetPlaytime(void);
    extern u32 darkPokemonCountFlag14(void);
    extern u32 darkPokemonCountFlag12(void);
    extern void msgctrlSetValue(s32 id, u32 value);
    u32 playTime;
    playTime = gamedataGetPlaytime();
    if (lbl_8047A498 != 0) {
        playTime = lbl_8047A498;
    }
    lbl_8047A49C = darkPokemonCountFlag14();
    lbl_8047A4A0 = darkPokemonCountFlag12();
    msgctrlSetValue(0x4c, playTime);
    msgctrlSetValue(0x2f, darkPokemonCountFlag12());
    msgctrlSetValue(0x30, darkPokemonCountFlag14());
}

typedef struct PdaSaveImage {
    u8 bytes[0x1dfd0];
} PdaSaveImage;
/* RULE-EXCEPTION(user-approved): restore save-handler lifetime optimization;
 * see docs/RULE_EXCEPTIONS.md for the measured unit settings. */
#pragma opt_lifetimes on
s32 pcMenuSave(void)
{
    extern s32 menuOpen(s32 menu, s32 mode);
    extern s8 menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern u32 gamedataGetPlaytime(void);
    extern void gamedataSetPlaytime(f32 seconds);
    extern void* memcardAllocBuffer(void);
    extern void memcardFreeBuffer(void* buffer);
    extern s32 memcardWaitTask2(s32 a, s32 b, s32 c);
    extern void* savedataGetStatus(void* buffer, s32 kind);
    extern u32 gamedatasaveGetStatus(void* buffer, s32 kind);
    extern u32 heroGetStatus(void* data, s32 a, s32 b);
    extern void heroSetStatus(s32 a, s32 b, u8 value);
    extern u8 heroMoveIsMember(s32 index);
    extern void* gamedatasaveBiosGetPtr(void* data);
    extern s32 gamedatasaveBiosGetFloorid(void* data);
    extern s32 gamedatasaveBiosGetPrevfloorid(void* data);
    extern u8 gamedatasaveBiosGetFloorposindex(void* data);
    extern void gamedatasaveBiosSetFloorid(void* data, s32 value);
    extern void gamedatasaveBiosSetPrevfloorid(void* data, s32 value);
    extern void gamedatasaveBiosSetFloorposindex(void* data, u8 value);
    extern void floorGetSaveInfo(s32* floorId, s32* prevFloorId, u8* posIndex);
    void* backup;
    u8* slot;
    u8 saved;
    s32 result;
    s32 running;
    s32 state;
    void* data;
    s8 answer;
    s32 prevFloorId;
    s32 floorId;
    u8 posIndex;
    state = 0;
    running = 1;
    slot = lbl_803A64E0;
    do {
        switch (state) {
        case 0:
            lbl_8047A480 = (void*)0x3c01;
            menuOpen(0x28, 0);
            state = 1;
            break;
        case 1:
            pdaFillSavePrompt();
            answer = menuSubOpenYesNo(0, 0x3c, 0x118, 0);
            if (answer == 0) {
                state = 2;
            } else if (answer == 1) {
                state = 0x64;
                result = 1;
            } else {
                state = 0x64;
                result = 1;
            }
            break;
        case 2:
            backup = (saved = 1, memcardAllocBuffer());
            *(PdaSaveImage*)backup = *(PdaSaveImage*)savedataGetStatus(0, 0);
            if (memcardWaitTask2(3, 2, 0) == 3 &&
                gamedatasaveGetStatus(0, 4) != 0) {
                u32 previousHeroId = heroGetStatus(savedataGetStatus(backup, 2), 2, 0);
                if (previousHeroId == heroGetStatus(0, 2, 0) ||
                    gamedatasaveGetStatus(backup, 4) == 0) {
                    lbl_8047A498 = gamedataGetPlaytime();
                    pdaFillSavePrompt();
                    lbl_8047A480 = (void*)0x1b60;
                    answer = menuSubOpenYesNo(0, 0x3c, 0x118, 1);
                    if (answer != 0) {
                        saved = 0;
                    } else {
                        saved = 1;
                    }
                    lbl_8047A498 = gamedataGetPlaytime() - lbl_8047A498;
                }
            }
            *(PdaSaveImage*)savedataGetStatus(0, 0) = *(PdaSaveImage*)backup;
            gamedataSetPlaytime((f32)(lbl_8047A498 + gamedataGetPlaytime()));
            lbl_8047A498 = 0;
            memcardFreeBuffer(backup);
            if (saved) {
                state = 4;
                result = 1;
            } else {
                state = 0;
            }
            break;
        case 4:
            heroSetStatus(0, 0x18, (u8)(heroMoveIsMember(1) ? 1 : 0));
            savedataGetStatus(0, 0);
            data = gamedatasaveBiosGetPtr(savedataGetStatus(0, 1));
            *(s32*)lbl_803A64E0 = gamedatasaveBiosGetFloorid(data);
            *(s32*)&slot[4] = gamedatasaveBiosGetPrevfloorid(data);
            slot[8] = gamedatasaveBiosGetFloorposindex(data);
            floorGetSaveInfo(&floorId, &prevFloorId, &posIndex);
            gamedatasaveBiosSetFloorid(data, floorId);
            gamedatasaveBiosSetPrevfloorid(data, prevFloorId);
            gamedatasaveBiosSetFloorposindex(data, posIndex);
            memcardWaitTask2(4, 2, 0);
            state = 0x64;
            break;
        case 0x64:
            menuCloseCustom(0x28, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    pdaResetPeopleTablesSave(lbl_803A6498, lbl_802E51C8);
    return result;
}

#pragma pop

}
