/**
 * @file gs_range_80033278.c
 * @brief menuCardE / colosseum sysvars code, 0x80033278 - 0x80035C48 (16 fns).
 *
 * One unit for the whole span so that the Card-e work buffers at
 * 0x803A3278-0x803A3E58 are .bss objects defined in this translation unit,
 * as they were in the original: MWCC then addresses them off one pooled base
 * register in _sysvarsProcessData and fn_80034FB4 (lbl_803A3278 + 0x10 /
 * 0xB0 / 0xBC), and keeps the 0xBC object offset out of field displacements.
 *
 * cardeArenaRoomInit, fn_80035D70 and cardeArenaRoomExit live in game/gs_exact_80035C48.c.
 *
 * openingDemoFinish (formerly the last function here) is the opening-demo
 * finish callback of the movie TU and now lives in game/movie.c.
 */
#include "dolphin/types.h"

/* One opponent record of the colosseum menu block (0x28 bytes). */
typedef struct SysvarsOpponentSource {
    u8 name[0x0C];
    u8 category;
    s8 moveIndex[4];
    u8 pad_11;
    u16 field_12;
    u16 field_14;
    u16 field_16;
    u16 field_18;
    u32 field_1C;
    u16 field_20;
    u8 pad_22[2];
    u8 field_24;
} SysvarsOpponentSource;

typedef struct SysvarsMoveSnapshot {
    u8 data[0x2A];
} SysvarsMoveSnapshot;

typedef struct SysvarsOpponentSnapshot {
    u8 trainerType;
    u8 region;
    u8 variant;
    u8 enabled;
    u8 selection;
    u8 pad_05;
    u8 name[0x0C];
    u8 category;
    u8 pad_13;
    u16 field_14;
    u16 field_16;
    u16 field_18;
    u16 field_1A;
    u8 field_1C;
    u8 pad_1D;
    u16 field_1E;
    u8 field_20;
    u8 pad_21;
    SysvarsMoveSnapshot moves[4];
} SysvarsOpponentSnapshot;

/*
 * Savedata block 0xD, as handed out by savedataGetStatus(0, 0xD). Only the
 * recent-opponent history is modelled; the offset is part of the member so
 * the loads keep the retail 0x41E4-based displacements.
 */
typedef struct SysvarsSaveWork {
    u8 pad_0000[0x41E4];
    SysvarsOpponentSnapshot history[10];
} SysvarsSaveWork;

/* The Card-e data block (lbl_803A3334, 0xB24 bytes). */
typedef struct CardEData {
    s32 card_type;                          /* 0x000 */
    u8 pad_004[4];
    u8 cardId;                              /* 0x008 */
    u8 pad_009;
    u8 title[0x1A];                         /* 0x00A */
    s8 row;                                 /* 0x024 */
    u8 pad_025;
    s8 column;                              /* 0x026 */
    u8 pad_027;
    u8 names[3][0x10];                      /* 0x028 */
    s8 layerCount;                          /* 0x058 */
    u8 pad_059;
    s8 columnCount;                         /* 0x05A */
    s8 menu1[3];                            /* 0x05B */
    s8 menu2[3];                            /* 0x05E */
    s8 bonus[3];                            /* 0x061 */
    u16 items[3];                           /* 0x064 */
    u8 pad_06A[0x3AC - 0x6A];
    SysvarsOpponentSource opponents[0x2E];  /* 0x3AC */
    u8 pad_ADC[0xAFC - 0xADC];
    u32 floorId;                            /* 0xAFC */
    u16 floorName[0x12];                    /* 0xB00 */
} CardEData;

typedef struct CardEGridEntry {
    u16 id;
    u8 pad02[0x18];
    u8 key;
    s8 layers;
    s8 rows;
    s8 columns;
    u8 rowType[6];
} CardEGridEntry;

/* One entry of the recent-floor history (lbl_803A3278). */
typedef struct SysvarsFloorHistory {
    u8 floor;
    s8 region;
    s8 variant;
} SysvarsFloorHistory;

/* Card-e work buffers, .bss 0x803A3278 - 0x803A3E58. */
/* The five recent floors, padded to the 0x10-byte retail object. */
typedef struct SysvarsFloorHistoryList {
    SysvarsFloorHistory entries[5];
    u8 pad_0F;
} SysvarsFloorHistoryList;

SysvarsFloorHistoryList lbl_803A3278;
u8 lbl_803A3288[0xA0];
u16 lbl_803A3328[6];
CardEData lbl_803A3334;

/* Byte view of the card block, for the record copies that walk it raw. */
#define CARD_E_BYTES ((u8*)&lbl_803A3334)

extern SysvarsSaveWork* lbl_8047A430;
extern CardEGridEntry* lbl_8047A434;
extern u8 lbl_8047A439;
extern u32 lbl_8047A444;
extern s8 lbl_8047A448;
extern u8 lbl_803F7A30[];
extern const f32 lbl_8047B9F8;
extern const f32 lbl_8047BA18;
extern const f32 lbl_8047BA28;
extern u8 lbl_8047BA1C;
extern const char lbl_80266FAC[];
extern const char lbl_80266FB8[];
extern const char lbl_8047BA20[6];

extern void GBAInit(void);
extern void gbaBootPrepare(s32);
extern s32 windowGetActiveID(void);
extern u8* windowGetKeyInfo(void);
extern void gbaBootCancel(void);
extern void gbaBootWaitThread(void);
extern s32 gbaBootRun(s32, s32*);
extern void cardeDecode(void*, const void*, u32);
extern void cardeMatrixSetFlagC9(u8);
extern void cardeMatrixSetFlagC8(u8);
extern s32 menuOpen(s32, s32);
extern void menuClose(s32);
extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, ...);
extern u8* GScharCpy(void*, const void*);
extern void* fightFloorDataBiosGetPtr(u16);
extern void* fightFloorDataBiosGetName(void*);
extern void* GSmsgGetGSchar(void*);
extern void cardeMatrixSetTitle(void*);
extern void msgctrlSetValue(u32, u32);
extern void winMsgOpen(s32, u32, s32, s32);
extern void winMsgClose(s32);
extern void __assert(const char*, s32, const char*);
extern void cardeMatrixUpdateSortKeys(void);
extern CardEGridEntry* cardeGridGetRecord(void*, CardEData*, u8);
extern void cardeMatrixLoadEntries(void);
extern u8* cardeSaveGetCard(CardEGridEntry*, s8, s8, s8);
extern u8* cardeSaveGetLevel(CardEGridEntry*, s8);
extern void menuCardEMatrixSetTarget(u8, s8, s8, s8, s32);
extern void cardeMatrixClearAnimTimer(void);
extern u8* cardeSaveSetCard(CardEGridEntry*, CardEData*, s8);
extern u8* cardeSaveSetSummary(CardEGridEntry*, CardEData*, s8);
extern void cardeSaveClearSummary(CardEGridEntry*, CardEData*, s8);
extern void soundStartSE(u32);
extern void _threadSwitch(void);
extern s32 GSgfxGetVsyncRate(void);
extern u32 GSgetFrameStep(void);
extern void heroPartyHealAll(s32);
extern void* savedataGetStatus(s32, s32);
extern void* heroBiosGetPokemonPtr(void*, u16);
extern void pokemonBiosSetItemDataId(void*, u16);
extern u32 cardeBuildTrainerEncounter(void*, s32, s32);
extern void menuCardEMatrixSelectCard(u8, s8);
extern u8 cardeSaveRemoveCard(CardEGridEntry*, CardEData*, s8);
extern void cardeMatrixCommitEntry(void);
extern void cardeSaveClearIfEmpty(CardEGridEntry*);
extern void soundStartRaw(s32, s32, s32);
extern void heroAddPokecoupon(s32, s32);
extern s32 heroItemAddItemDataId(void*, u16, s32, s32);
extern u16 pcboxDelItem(void*, u16, s32);
extern void std130_soundUnk_801653cc(s32, s32, s32);
extern void cardeGridSetShadowPokemon(u8*, CardEData*, s8);
extern s32 gbaTransferUI(void);
extern void cardeSelectOpponentLevel(u8* valid, s8* selectionOut, u8* valueOut);
extern void cardeArenaRecordResult(u8 enabled, s32 selection,
                        SysvarsOpponentSource* source);

static inline void sysvarsWaitForTransfer(void)
{
    f32 progress;

    progress = lbl_8047B9F8;
    while (progress < lbl_8047BA18) {
        _threadSwitch();
        progress += (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    }
}

/* The same wait for two seconds (gbaTransferUI's GBA link retries). */
static inline void sysvarsWaitTwoSeconds(void)
{
    f32 progress;

    progress = lbl_8047B9F8;
    while (progress < lbl_8047BA28) {
        _threadSwitch();
        progress += (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    }
}

/*
 * MWCC lays out the pooled .bss in first-reference order. In the original
 * translation unit, earlier functions (0x800301B0..) used the Card-e head
 * first, then the name, items and card data. This never-called stand-in
 * references them in that order, so the pool offsets (0x10, 0xB0, 0xBC)
 * match retail. Nothing calls it, so the linker strips it.
 */
/* RULE-EXCEPTION(user-approved): stripped stand-in that only reproduces the TU's .bss pool order - see docs/RULE_EXCEPTIONS.md */
void sysvarsCardEPoolOrder(void)
{
    lbl_803A3278.entries[0].floor = 0;
    lbl_803A3288[0] = 0;
    lbl_803A3328[0] = 0;
    lbl_803A3334.card_type = 0;
}

static inline void sysvarsRestoreItems(void)
{
    s32 i;
    void* save;

    heroPartyHealAll(0);
    save = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        /* RULE-EXCEPTION(user-approved): codegen-only cast; (u8) before the u16 index parameter gives retail's clrlwi 24 and argument order (i < 6, so the value is unchanged) - see docs/RULE_EXCEPTIONS.md */
        pokemonBiosSetItemDataId(heroBiosGetPokemonPtr(save, (u8)i), lbl_803A3328[i]);
    }
}

static inline s32 sysvarsFindEmptyLayer(CardEData* card)
{
    s32 i;
    s8 row;
    s8 column;
    s32 found;

    row = card->row;
    found = -1;
    column = card->column;
    for (i = 0; i < (s8)card->layerCount; i++) {
        if (cardeSaveGetCard(lbl_8047A434, i, row, column)[0xC] == 0) {
            found = i;
            break;
        }
    }
    return found;
}

static inline u8 sysvarsLayerOpen(CardEData* card, s8 layer)
{
    s32 row;

    row = card->row;
    if (row == card->row && lbl_803A3334.menu2[layer] < 0) {
        return 0;
    }
    if (cardeSaveGetLevel(lbl_8047A434, layer)[0x1C + row * 0xE] != 0) {
        return 0;
    }
    return 1;
}

static inline u8 sysvarsStageTwoReady(CardEData* card, s8 layer)
{
    s32 column;
    s8 row;
    u8 valid;

    valid = 1;
    row = card->row;
    if (card->menu2[layer] < 0) {
        valid = 0;
    } else {
        for (column = 0; column < (s8)card->columnCount; column++) {
            if (cardeSaveGetCard(lbl_8047A434, layer, row, column)[0xC] == 0) {
                valid = 0;
                break;
            }
        }
    }
    return valid;
}

static inline u8 sysvarsAllFilled(CardEData* card, s8 layer)
{
    s8 row;
    s32 column;

    if (card->bonus[layer] < 0) {
        return 0;
    }
    for (row = 0; row < lbl_8047A434->rows; row++) {
        for (column = 0; column < (s8)card->columnCount; column++) {
            if (cardeSaveGetCard(lbl_8047A434, layer, row, column)[0xC] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

static inline s32 sysvarsCheckLayers(CardEData* card)
{
    s32 secondary;
    s32 empty;
    s8 open;

    secondary = 4;
    empty = sysvarsFindEmptyLayer(card);
    switch (empty) {
    case -1:
        secondary = 2;
        break;
    case 1:
    case 2:
        for (open = 0; open < empty; open++) {
            if (sysvarsLayerOpen(card, open)) {
                secondary = 3;
                break;
            }
        }
        break;
    }
    return secondary;
}

static inline s32 sysvarsCheckCard(CardEData* card)
{
    s32 secondary;
    s32 action;

    action = 4;
    if (cardeSaveGetCard(lbl_8047A434, card->layerCount - 1, card->row, card->column)[0xC] != 0) {
        action = 2;
    } else {
        secondary = sysvarsCheckLayers(card);
        if (secondary != 4) {
            action = secondary;
        }
    }
    return action;
}

static inline s32 sysvarsAskRetry(void)
{
    s32 result;

    sysvarsRestoreItems();
    winMsgOpen(8, 0x3B7C, 0, 0);
    result = menuOpen(0xA9, 1);
    menuClose(0xA9);
    winMsgClose(1);
    return result;
}

s32 _sysvarsProcessData__FP16sysvarsFuncEntryPc(void* entry, char* text)
{
    u8* name;
    CardEData* card;
    s32 result;
    s32 action;
    s8 open;
    u32 menuResult;
    u32 message;
    u16 item;
    u8 valid;
    void* floor;

    cardeMatrixSetFlagC9(lbl_8047A439 == 0);
    cardeMatrixSetFlagC8(1);
    result = menuOpen(0xA6, 1);
    cardeMatrixSetFlagC8(0);
    if (result != 0) {
        return 1;
    }
    if (lbl_8047A439 != 0) {
        return 2;
    }

    /* RULE-EXCEPTION(user-approved): carrier; the card pointer set before name puts retail's card address (base+0xBC) ahead of the name address in the loop preheader - see docs/RULE_EXCEPTIONS.md */
    card = &lbl_803A3334;
    name = lbl_803A3288;

restart:
    switch (gbaTransferUI()) {
    case 0:
        return 0;
    case 1:
        return 2;
    case 2:
        break;
    }

    if (lbl_803A3334.card_type == 1) {
        u16* floorName;
        lbl_8047A444 = lbl_803A3334.floorId;
        floorName = lbl_803A3334.floorName;
        if (floorName[0] != 0) {
            GScharCpy(lbl_803A3288, floorName);
        } else {
            floor = fightFloorDataBiosGetPtr(lbl_8047A444);
            if (floor != NULL) {
                GScharCpy(lbl_803A3288, GSmsgGetGSchar(fightFloorDataBiosGetName(floor)));
            } else {
                GScharCpy(lbl_803A3288, &lbl_8047BA1C);
            }
        }
        cardeMatrixSetTitle(lbl_803A3288);
        msgctrlSetValue(0x4D, (u32)lbl_803A3288);
        winMsgOpen(8, 0x3B55, 1, 0);
        winMsgClose(1);
        return 2;
    }

    if (lbl_803A3334.card_type != 0) {
        __assert(lbl_80266FAC, 0x6C8, lbl_80266FB8);
    }

    cardeMatrixUpdateSortKeys();
    lbl_8047A434 = cardeGridGetRecord(0, &lbl_803A3334, 1);
    cardeMatrixLoadEntries();
    if (lbl_8047A434 == NULL) {
        __assert(lbl_80266FAC, 0x6DB, lbl_8047BA20);
        winMsgOpen(8, 0x3B91, 1, 0);
        return 0;
    }

    action = sysvarsCheckCard(card);

    switch (action) {
    case 2:
    default:
        winMsgOpen(8, 0x3B58, 1, 0);
        winMsgClose(1);
        return 2;

    case 3:
        for (open = 0; open < lbl_803A3334.layerCount; open++) {
            if (sysvarsLayerOpen(&lbl_803A3334, open)) {
                break;
            }
        }
        switch (lbl_8047A434->rowType[(s8)lbl_803A3334.row]) {
        case 0:
            message = 0x3C7C;
            break;
        case 1:
            message = 0x3C7D;
            break;
        case 2:
            message = 0x3C7F;
            break;
        case 3:
            message = 0x3C81;
            break;
        case 4:
            message = 0x3C83;
            break;
        case 5:
            message = 0x3C85;
            break;
        case 6:
            message = 0x3C88;
            break;
        case 7:
            message = 0x3C8B;
            break;
        default:
            message = 0x3C7C;
            break;
        }
        msgctrlSetValue(0x31, message);
        msgctrlSetValue(0x4D, (u32)lbl_803A3334.names[open]);
        msgctrlSetValue(0x57, (u32)lbl_803A3334.names[open + 1]);
        menuCardEMatrixSetTarget(lbl_803A3334.cardId, open, lbl_803A3334.row, lbl_803A3334.column, 6);
        winMsgOpen(8, 0x3B5B, 1, 0);
        winMsgClose(1);
        cardeMatrixClearAnimTimer();
        return 2;

    case 4:
        break;
    }

    lbl_8047A448 = sysvarsFindEmptyLayer(&lbl_803A3334);
    winMsgClose(1);
    cardeSaveSetCard(lbl_8047A434, &lbl_803A3334, lbl_8047A448);
    menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, lbl_803A3334.column, 1);
    soundStartSE(0x4C5);
    sysvarsWaitForTransfer();
    winMsgOpen(8, 0x3B6D, 1, 0);
    winMsgClose(1);
    sysvarsRestoreItems();

first_menu:
    menuResult = cardeBuildTrainerEncounter(&lbl_803A3334.opponents[lbl_803A3334.menu1[lbl_8047A448]], 8, 0);
    menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, name);
    menuCardEMatrixSelectCard(lbl_803A3334.cardId, lbl_8047A448);

    if (menuResult != 2) {
        sysvarsRestoreItems();
        winMsgOpen(8, 0x3B70, 0, 0);
        result = menuOpen(0xA9, 1);
        menuClose(0xA9);
        winMsgClose(1);
        if (result == 0) {
            goto first_menu;
        }
        menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, lbl_803A3334.column, 7);
        soundStartSE(0x4C6);
        sysvarsWaitForTransfer();
        cardeMatrixUpdateSortKeys();
        if (cardeSaveRemoveCard(lbl_8047A434, &lbl_803A3334, lbl_8047A448) != 0) {
            cardeMatrixClearAnimTimer();
            cardeMatrixCommitEntry();
            cardeMatrixUpdateSortKeys();
            cardeSaveClearIfEmpty(lbl_8047A434);
        }
        cardeMatrixClearAnimTimer();
        cardeMatrixLoadEntries();
        return 2;
    }

    menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, lbl_803A3334.column, 2);
    sysvarsWaitForTransfer();
    menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, lbl_803A3334.column, 4);
    soundStartSE(0x3C6);
    sysvarsWaitForTransfer();
    soundStartRaw(0x3CC, 0, 0);
    heroAddPokecoupon(0, 0x32);
    winMsgOpen(8, 0x3B72, 1, 0);

    if (sysvarsStageTwoReady(&lbl_803A3334, lbl_8047A448)) {
        cardeSaveSetSummary(lbl_8047A434, &lbl_803A3334, lbl_8047A448);
        menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, -1, 1);
        soundStartSE(0x4C5);
        sysvarsWaitForTransfer();
        winMsgOpen(8, 0x3B74, 1, 0);
        winMsgClose(1);

        menuResult = cardeBuildTrainerEncounter(&lbl_803A3334.opponents[lbl_803A3334.menu2[lbl_8047A448]], 9, 0);
        menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, name);
        menuCardEMatrixSelectCard(lbl_803A3334.cardId, lbl_8047A448);

        if (menuResult != 2) {
            menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, -1, 7);
            soundStartSE(0x4C6);
            sysvarsWaitForTransfer();
            cardeSaveClearSummary(lbl_8047A434, &lbl_803A3334, lbl_8047A448);
            menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, lbl_803A3334.column, 8);
            soundStartSE(0x4C6);
            sysvarsWaitForTransfer();
            cardeMatrixUpdateSortKeys();
            if (cardeSaveRemoveCard(lbl_8047A434, &lbl_803A3334, lbl_8047A448) != 0) {
                cardeMatrixClearAnimTimer();
                cardeMatrixCommitEntry();
                cardeMatrixUpdateSortKeys();
                cardeSaveClearIfEmpty(lbl_8047A434);
            }
            cardeMatrixClearAnimTimer();
            cardeMatrixLoadEntries();
            winMsgOpen(8, 0x3B78, 1, 0);
            cardeMatrixClearAnimTimer();
            if (sysvarsAskRetry() == 0) {
                goto restart;
            }
            return 2;
        }

        menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, -1, 2);
        sysvarsWaitForTransfer();
        menuCardEMatrixSetTarget(lbl_803A3334.cardId, lbl_8047A448, lbl_803A3334.row, -1, 4);
        soundStartSE(0x3C6);
        sysvarsWaitForTransfer();
        soundStartRaw(0x3CC, 0, 0);
        heroAddPokecoupon(0, 0x50);
        winMsgOpen(8, 0x3B76, 1, 0);

        item = lbl_803A3334.items[lbl_8047A448];
        valid = 1;
        if (item == 0) {
            valid = 0;
        } else if (heroItemAddItemDataId(0, item, 1, -1) != 0) {
            if (pcboxDelItem(0, item, 1) != 0) {
                valid = 0;
            }
        }
        if (valid) {
            msgctrlSetValue(0x2D, item);
            winMsgOpen(8, 0x4273, 1, 0);
        }
        cardeMatrixClearAnimTimer();
    }

    if (!sysvarsAllFilled(&lbl_803A3334, lbl_8047A448)) {
        cardeMatrixClearAnimTimer();
        if (sysvarsAskRetry() == 0) {
            goto restart;
        }
        return 2;
    }

    std130_soundUnk_801653cc(0x4CF, 0, 0xFF);
    cardeGridSetShadowPokemon(0, &lbl_803A3334, lbl_8047A448);
    msgctrlSetValue(0x4D, (u32)lbl_803A3334.title);
    msgctrlSetValue(0x57, (u32)lbl_803A3334.names[lbl_8047A448]);
    winMsgOpen(8, 0x3B7A, 1, 0);
    winMsgClose(1);
    return 0;
}

/* Run the GBA transfer UI until it finishes or the user cancels it. */
s32 gbaTransferUI(void)
{
    s32 state;
    s32 nextState;
    s32 transferActive;
    u8 waitForWindow;
    s32 watchedWindow;

    watchedWindow = -1;
    GBAInit();
    gbaBootPrepare(0);
    transferActive = 0;
    state = 1;
    waitForWindow = 0;

    for (;;) {
        if (windowGetActiveID() == watchedWindow &&
            (*(u16*)(windowGetKeyInfo() + 4) & 0x20) != 0) {
            if (state == 18) {
                gbaBootCancel();
            } else {
                if (transferActive != 0) {
                    gbaBootWaitThread();
                }
                soundStartSE(0x3C7);
                winMsgClose(1);
                winMsgOpen(8, 0x44ED, 1, 0);
                winMsgClose(1);
                return 1;
            }
        }

        nextState = gbaBootRun(state, &transferActive);
        _threadSwitch();
        if (waitForWindow != 0 && nextState == 0) {
            continue;
        }
        waitForWindow = 0;
        if (nextState != 0) {
            state = nextState;
        }

        switch (state) {
        case 2:
            winMsgOpen(8, 0x3B88, 0, 0);
            sysvarsWaitTwoSeconds();
            /* fallthrough */
        case 19:
            soundStartSE(0x3C7);
            if (transferActive != 0) {
                gbaBootWaitThread();
            }
            winMsgOpen(8, 0x3B91, 1, 0);
            return 0;

        case 17:
            winMsgOpen(8, 0x3B88, 0, 0);
            sysvarsWaitTwoSeconds();
            state = 18;
            gbaBootRun(state, &transferActive);
            break;

        case 21:
            if (transferActive != 0) {
                gbaBootWaitThread();
            }
            soundStartSE(0x3C7);
            winMsgClose(1);
            winMsgOpen(8, 0x44ED, 1, 0);
            winMsgClose(1);
            return 1;

        case 18:
            winMsgOpen(8, 0x3B8E, 0, 0);
            watchedWindow = windowGetActiveID();
            waitForWindow = 1;
            break;

        case 20:
            gbaBootWaitThread();
            cardeDecode(&lbl_803A3334, lbl_803F7A30 + 0x2388,
                        0x1040);
            return 2;
        }
    }
}

/* cardeMenuDrawSprite - 0x800345A4 | size: 0x164 */
void cardeMenuDrawSprite(void* unused, u8* sprite) {
    extern u32 lbl_802E61D8[];
    extern void winSpriteSetDisp(void*, u32);
    extern u32 heroGetStatus(s32, s32, s32);
    u32 value;
    u32 display;
    u32 limit;
    s16 id;

    value = heroGetStatus(0, 0xE, 0);
    id = *(s16*)(sprite + 6);
    switch (id) {
    case 0x7BC:
    case 0x7F1:
        limit = lbl_802E61D8[3];
        display = value - limit;
        limit = value | ~limit;
        display = limit - (display >> 1);
        winSpriteSetDisp(sprite, display >> 31);
        break;
    case 0x7F0:
    case 0x805:
        display = 0;
        if (value >= lbl_802E61D8[2] && value < lbl_802E61D8[3]) {
            display = 1;
        }
        winSpriteSetDisp(sprite, display & 0xFF);
        break;
    case 0x7EF:
    case 0x804:
        display = 0;
        if (value >= lbl_802E61D8[1] && value < lbl_802E61D8[2]) {
            display = 1;
        }
        winSpriteSetDisp(sprite, display & 0xFF);
        break;
    case 0x7EE:
    case 0x803:
        display = 0;
        if (value >= lbl_802E61D8[0] && value < lbl_802E61D8[1]) {
            display = 1;
        }
        winSpriteSetDisp(sprite, display & 0xFF);
        break;
    }
}

/* cardeMenuDrawSprite2 - 0x80034708 | size: 0xB0 */
void cardeMenuDrawSprite2(void* unused, u8* sprite) {
    extern u32 lbl_8047A458;
    extern u32 lbl_80266FA0[];
    extern void winSpriteSetDisp(void*, u32);
    extern u8* windowSearchID(s32);
    u32* table = NULL;
    u8* window;

    switch (lbl_8047A458) {
    case 1:
        table = lbl_80266FA0;
        break;
    }

    if (table != NULL) {
        winSpriteSetDisp(sprite, 1);
        switch (*(s16*)(sprite + 6)) {
        case 0x7CC:
            window = windowSearchID(0xA4);
            if (window != NULL) {
                *(u32*)(sprite + 0x4C) = table[(s8)window[0x95]];
            }
            break;
        }
    } else {
        *(u32*)(sprite + 0x4C) = 0;
        winSpriteSetDisp(sprite, 0);
    }
}

/* cardeSetRecordFlag - 0x800347B8 | size: 0xC */
void cardeSetRecordFlag(void) {
    extern u8 lbl_8047A440;

    lbl_8047A440 = 1;
}

/* cardeBuzzer_800347C4 - 0x800347C4 | size: 0x24 */
#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma kept for byte match - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
void cardeBuzzer_800347C4(void) {
    extern void soundStartSE(u32);

    soundStartSE(0x26);
}
#pragma pop

/* cardeBuzzer_800347E8 - 0x800347E8 | size: 0x24 */
#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma kept for byte match - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
void cardeBuzzer_800347E8(void) {
    extern void soundStartSE(u32);

    soundStartSE(0x26);
}
#pragma pop

/* cardeBuzzer_8003480C - 0x8003480C | size: 0x24 */
#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma kept for byte match - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
void cardeBuzzer_8003480C(void) {
    extern void soundStartSE(u32);

    soundStartSE(0x26);
}
#pragma pop

/*
 * Record the current opponent in the recent-opponent history. Nothing happens
 * if this trainer/region/variant triple is already listed; otherwise the ten
 * slots shift down by one and the new record is written into the last slot.
 *
 * The new record is filled through a byte cursor: after the header it walks
 * the four 0x2A-byte move slots, which is how the retail code addresses them
 * (base advancing by 0x2A, move payload at displacement 0x22).
 */
void cardeArenaRecordResult(u8 enabled, s32 selection,
                 SysvarsOpponentSource* source)
{
    extern void GScharCpy(void*, const void*);
    s32 region;
    s32 variant;
    u8* destination;
    s32 moveIndex;
    s32 i;
    u8 isNew;

    isNew = 1;
    region = lbl_803A3334.row;
    variant = lbl_803A3334.column;
    for (i = 0; i < 10; i++) {
        if (lbl_8047A430->history[i].trainerType == lbl_803A3334.cardId &&
            lbl_8047A430->history[i].region == region &&
            lbl_8047A430->history[i].variant == variant) {
            isNew = 0;
            break;
        }
    }

    if (isNew) {
        for (i = 0; i < 9; i++) {
            lbl_8047A430->history[i] = lbl_8047A430->history[i + 1];
        }

        destination = (u8*)&lbl_8047A430->history[9];
        destination[0] = lbl_803A3334.cardId;
        destination[1] = lbl_803A3334.row;
        destination[2] = lbl_803A3334.column;
        destination[3] = enabled;
        destination[4] = selection;
        GScharCpy(destination + 6, source->name);
        destination[0x12] = source->category;
        *(u16*)(destination + 0x14) = source->field_12;
        *(u16*)(destination + 0x16) = source->field_14;
        *(u16*)(destination + 0x18) = source->field_16;
        *(u16*)(destination + 0x1A) = source->field_18;
        destination[0x1C] = source->field_1C;
        *(u16*)(destination + 0x1E) = source->field_20;
        destination[0x20] = source->field_24;

        for (i = 0; i < 4; i++) {
            moveIndex = source->moveIndex[i];
            if (moveIndex < 0) {
                *(u16*)(destination + 0x22) = 0;
            } else {
                *(SysvarsMoveSnapshot*)(destination + 0x22) =
                    *(SysvarsMoveSnapshot*)(CARD_E_BYTES + 0x514 +
                                            moveIndex * 0x2A);
            }
            destination += 0x2A;
        }
    }
}

/* The highest required level among a battle group's four members.
 * RULE-EXCEPTION(user-approved): single-use inline helper; only inlining
 * keeps retail's `li r5,0; mr r6,r5` start of the loop - see
 * docs/RULE_EXCEPTIONS.md */
static inline u8 sysvarsGroupMaxLevel(u8* group)
{
    u16 slot;
    u8 maxRequired;
    s32 index;
    u8 required;

    maxRequired = 0;
    for (slot = 0; slot < 4; slot++) {
        index = (s8)group[0x3B9 + slot];
        if (index >= 0) {
            required = CARD_E_BYTES[index * 0x2A + 0x517];
            if (maxRequired < required) {
                maxRequired = required;
            }
        }
    }
    return maxRequired;
}

void cardeSelectOpponentLevel(u8* valid, s8* selectionOut, u8* valueOut)
{
    extern void* heroGetStatus(s32, s32, u16);
    extern u8 pokemonCheckValid(void*);
    extern u32 pokemonGetStatus(void*, s32, s32, s32);
    extern void* cardeSaveGetLevel(void*, s8);
    extern u32 _fadeEffectGetRandom__FUl(u32);
    u8* pokemon;
    u8 maxLevel;
    u8 maxRequired;
    u16 slot;
    s32 candidate;
    s8 best;
    u16 party;
    u8 region;
    u8* layer;
    s32 ties;
    u8 level;
    s32 score[6];
    u8* group;
    s32 index;
    s32 difference;
    s32 sign;
    u8 required;

    best = 0;
    ties = 1;
    region = CARD_E_BYTES[0x24];
    maxLevel = 1;
    for (party = 0; party < 6; party++) {
        pokemon = heroGetStatus(0, 3, party);
        if (pokemonCheckValid(pokemon) == 1) {
            level = pokemonGetStatus(pokemon, 0, 0x7A, 0);
            if (maxLevel < level) {
                maxLevel = level;
            }
        }
    }

    for (candidate = 0; candidate < (s8)CARD_E_BYTES[0x58]; candidate++) {
        score[candidate] = 0x7FFFFFFF;
        layer = cardeSaveGetLevel(lbl_8047A434, candidate);
        if (layer[0x1C + (s8)region * 0xE] != 0) {
            group = CARD_E_BYTES + lbl_803A3334.menu2[candidate] * 0x28;
            maxRequired = sysvarsGroupMaxLevel(group);
            difference = maxLevel - maxRequired;
            sign = difference >> 31;
            score[candidate] = (sign ^ difference) - sign;
            if (score[candidate] < score[best]) {
                best = candidate;
                ties = 1;
            } else if (score[candidate] == score[best]) {
                ties++;
                if (_fadeEffectGetRandom__FUl(ties) == 0) {
                    best = candidate;
                }
            }
        }
    }

    *valid = score[best] < 0x7FFFFFFF;
    if (*valid != 0) {
        *selectionOut = best;
        *valueOut = lbl_803A3334.menu2[*selectionOut];
    } else {
        *selectionOut = (s8)(CARD_E_BYTES[0x58] - 1);
        *valueOut = lbl_803A3334.menu1[*selectionOut];
    }
}

/* cardeSubMenuDrawText - 0x80034DC0 | size: 0x78 */
void cardeSubMenuDrawText(u8* arg0, u8* arg1) {
    extern u32 lbl_8047A44C;
    extern void GSmsgPrintColor(s32, s32, u32, u16);
    extern void msgctrlSetValue(s32, s32);
    extern void GSmsgPrintRect(s32, s32, s32, s32, u32, u16);
    u8 byte;
    s32 mask;
    s32 combined;

    byte = arg0[0x8B];
    mask = -0x100;
    combined = byte | mask;
    GSmsgPrintColor(0, 0, combined, 0x3CC8);
    msgctrlSetValue(0x34, lbl_8047A44C);
    GSmsgPrintRect(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), combined, 0x3CC9);
}

/* cardeSubMenuDrawText2 - 0x80034E38 | size: 0xB8 */
void cardeSubMenuDrawText2(u8* arg0, u8* arg1) {
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern void GSmsgPrintColor(s32, s32, u32, u16);
    extern void msgctrlSetValue(s32, u32);
    extern void GSmsgPrintRect(s32, s32, s32, s32, u32, u16);
    u32 count;
    u32 value;
    s32 mask;

    count = lbl_8047A450;
    if (count > lbl_8047A44C) {
        value = arg0[0x8B] | 0xFFA08000;
    } else {
        mask = -0x100;
        value = arg0[0x8B];
        value = value | (mask & 0xFFFFFFFFFFFFFFFFu);
    }
    GSmsgPrintColor(0, 0, value, 0x3CC7);
    if (lbl_8047A450 <= 0x98967F) {
        msgctrlSetValue(0x34, lbl_8047A450);
    } else {
        msgctrlSetValue(0x34, 0x98967F);
    }
    GSmsgPrintRect(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), value, 0x3CC9);
}

/* cardeMenuDrawText - 0x80034EF0 | size: 0x94 */
#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma kept for byte match - see docs/RULE_EXCEPTIONS.md */
#pragma optimization_level 1
void cardeMenuDrawText(u8* arg0, u8* arg1) {
    extern void GSmsgPrintColor(s32, s32, u32, u16);
    extern s32 heroGetStatus(s32, s32, s32);
    extern void msgctrlSetValue(s32, s32);
    extern void GSmsgPrintRect(s32, s32, s16, s16, u32, u16);
    s32 mask;
    u32 byte;
    s32 value;
    s32 status;
    s16 position;

    byte = arg0[0x8B];
    mask = -0x100;
    value = byte | mask;
    GSmsgPrintColor(0, 0, value, 0x3CC6);
    status = heroGetStatus(0, 0xD, 0);
    msgctrlSetValue(0x34, status);
    /* RULE-EXCEPTION(user-approved): dead store of the x position; at this
     * function's optimization level it only orders the argument setup (li r8
     * before the extsh) - see docs/RULE_EXCEPTIONS.md */
    position = *(s16*)(arg1 + 0x54) + 2;
    GSmsgPrintRect(0, 0, *(s16*)(arg1 + 0x54) + 2, *(s16*)(arg1 + 0x56), value, 0x3CD3);
}
#pragma pop

/* cardeSubMenuDrawText3 - 0x80034F84 | size: 0x2C */
#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma kept for byte match - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size on
void cardeSubMenuDrawText3(void) {
    extern void msgctrlSetValue();
    msgctrlSetValue(0x37, lbl_803A3288);
}
#pragma optimize_for_size reset
#pragma pop

/* cardeNop_80034FB0 - 0x80034FB0 | size: 0x4 */
void cardeNop_80034FB0(void) {
}

extern s32 lbl_8047A43C;
extern u16 pokemonBiosGetItemDataId(void*);

static inline void sysvarsStoreItems(void)
{
    s32 i;
    void* save;

    save = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        /* RULE-EXCEPTION(user-approved): codegen-only cast; (u8) before the u16 index parameter gives retail's clrlwi 24 (i < 6, so the value is unchanged) - see docs/RULE_EXCEPTIONS.md */
        lbl_803A3328[i] = pokemonBiosGetItemDataId(heroBiosGetPokemonPtr(save, (u8)i));
    }
}

static inline void sysvarsLoadItems(void)
{
    s32 i;
    void* save;

    save = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        /* RULE-EXCEPTION(user-approved): codegen-only cast; (u8) before the u16 index parameter gives retail's clrlwi 24 (i < 6, so the value is unchanged) - see docs/RULE_EXCEPTIONS.md */
        pokemonBiosSetItemDataId(heroBiosGetPokemonPtr(save, (u8)i), lbl_803A3328[i]);
    }
}

/* RULE-EXCEPTION(user-approved): single-use inline helper; it numbers the search locals as retail does - see docs/RULE_EXCEPTIONS.md */
static inline u8 sysvarsFloorSeen(CardEData* menu)
{
    s32 index;
    s8 variant;
    u8 found;
    s8 region;
    u8 floor;
    SysvarsFloorHistory* entry;

    found = 0;
    if (lbl_8047A43C > 0) {
        floor = menu->cardId;
        region = menu->row;
        variant = menu->column;
        for (index = 0, entry = lbl_803A3278.entries; index < lbl_8047A43C; index++, entry++) {
            if (floor == entry->floor &&
                region == entry->region &&
                variant == entry->variant) {
                found = 1;
                break;
            }
        }
    }
    return found;
}

/* fn_80034FB4 - 0x80034FB4 | size: 0xC94 */
void cardeBattleRun(void)
{
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A449;
    extern u8 lbl_8047A44A;
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern u8 lbl_8047A454;
    extern s32 lbl_8047A458;
    extern const f32 lbl_8047B9FC;
    extern const f32 lbl_8047BA00;
    extern u8 lbl_8047BA1C;
    extern void cardeAwardCoupons(void);
    extern void cardeBattleSelectCard(void);
    extern u8 floorIsLinking(void);
    extern void floorBattleEndTerminateScripts(void);
    extern u8 menuIsCheck(s32);
    extern void fadeSet(u16, f32);
    extern s8 fadeCheck(u8);
    extern void floorLink(u32, s32);
    u8 duplicate;
    void* entry;
    u32 amount;
    s32 state;
    u8 value;
    CardEData* menu;
    void* save;
    s32 index;
    u16* floorName;
    f32 progress;
    s32 result;
    u8 selection;
    u8 valid;
    u32 name;

    lbl_8047A438 = 1;
    menu = &lbl_803A3334;
    /* RULE-EXCEPTION(user-approved): codegen-shaping type; the name buffer's address is kept as a u32 (as msgctrlSetValue takes it), which the native port accepts for 32-bit guest addresses - see docs/RULE_EXCEPTIONS.md */
    name = (u32)lbl_803A3288;
    floorName = menu->floorName;

    while ((state = lbl_8047A458) > 0) {
        switch (state) {
        case 1:
            if (lbl_8047A454 == 1) {
                fadeSet(2, lbl_8047BA00);
            }
            result = menuOpen(0xA4, 1);
            lbl_8047A454 = 1;
            fadeCheck(1);
            switch (result) {
            case 0:
                lbl_8047A458 = 2;
                break;
            case 1:
                lbl_8047A458 = 3;
                break;
            case -1:
                lbl_8047A458 = 0;
                break;
            case 2:
                lbl_8047A458 = 0;
                break;
            default:
                lbl_8047A458 = 0;
                break;
            }
            if (lbl_8047A458 == 0) {
                soundStartSE(0x3C7);
                winMsgOpen(8, 0x3B54, 1, 0);
                winMsgClose(1);
            } else {
                fadeSet(3, lbl_8047BA00);
                fadeCheck(1);
                menuClose(0xA4);
            }
            break;

        case 2:
            fadeSet(2, lbl_8047BA00);
            menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, name);
            fadeCheck(1);
            sysvarsStoreItems();
            while (lbl_8047A458 == 2) {
                /* RULE-EXCEPTION(user-approved): no-argument call to the two-parameter _sysvarsProcessData, which ignores both parameters; retail does a bare bl - see docs/RULE_EXCEPTIONS.md */
                extern s32 _sysvarsProcessData__FP16sysvarsFuncEntryPc(void);

                lbl_8047A458 = _sysvarsProcessData__FP16sysvarsFuncEntryPc();
            }
            heroPartyHealAll(0);
            sysvarsLoadItems();
            if (lbl_8047A439 != 0) {
                lbl_8047A458 = 0;
            }
            if (lbl_8047A458 != 0) {
                soundStartSE(0x3C7);
                fadeSet(3, lbl_8047BA00);
                fadeCheck(1);
                menuClose(0xA6);
            }
            break;

        case 3:
            sysvarsStoreItems();
            lbl_8047A450 = 0;
            lbl_8047A44A = 0;
            lbl_8047A43C = 0;
            for (index = 0; index < 5; index++) {
                lbl_803A3278.entries[index].floor = 0;
                lbl_803A3278.entries[index].region = 0;
                lbl_803A3278.entries[index].variant = 0;
            }
            if (menuIsCheck(0xA5) != 0) {
                menuClose(0xA5);
            }
            fadeSet(2, lbl_8047BA00);
            menuOpen(0xA5, 1);
            fadeCheck(1);
            lbl_8047A458 = 5;
            break;

        case 4:
            if (lbl_8047A449 != 0) {
                heroPartyHealAll(0);
            }
            if (lbl_8047A450 > lbl_8047A44C) {
                std130_soundUnk_801653cc(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B8D, 1, 0);
                winMsgClose(1);
                progress = lbl_8047B9F8;
                while (progress < lbl_8047B9FC) {
                    _threadSwitch();
                    progress += (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
                }
            }
            winMsgOpen(8, 0x3B54, 1, 1);
            winMsgClose(1);
            lbl_8047A43C = 0;
            for (index = 0; index < 5; index++) {
                lbl_803A3278.entries[index].floor = 0;
                lbl_803A3278.entries[index].region = 0;
                lbl_803A3278.entries[index].variant = 0;
            }
            sysvarsLoadItems();
            lbl_8047A458 = 0;
            break;

        case 5:
            result = gbaTransferUI();
            switch (result) {
            case 0:
                lbl_8047A458 = 4;
                continue;
            case 1:
                lbl_8047A458 = 6;
                continue;
            case 2:
            default:
                break;
            }
            switch (lbl_803A3334.card_type) {
            case 1:
                lbl_8047A458 = 7;
                continue;
            case 0:
                break;
            default:
                continue;
            }
            save = savedataGetStatus(0, 0xD);
            lbl_8047A434 = cardeGridGetRecord(save, &lbl_803A3334, 0);
            if (lbl_8047A434 != 0) {
                result = 4;
                entry = cardeSaveGetCard(lbl_8047A434, (s8)(menu->layerCount - 1),
                                    menu->row, menu->column);
                if (((u8*)entry)[0xC] != 0) {
                    duplicate = sysvarsFloorSeen(menu);
                    if (duplicate == 0) {
                        lbl_803A3278.entries[lbl_8047A43C].floor = menu->cardId;
                        lbl_803A3278.entries[lbl_8047A43C].region = menu->row;
                        lbl_803A3278.entries[lbl_8047A43C].variant = menu->column;
                        lbl_8047A43C++;
                        if (lbl_8047A43C >= 5) {
                            lbl_8047A43C = 4;
                        }
                    }
                    if (duplicate != 0) {
                        result = 3;
                    }
                } else {
                    result = 2;
                }
            } else {
                result = 2;
            }
            switch (result) {
            case 2:
                lbl_8047A458 = 8;
                break;
            case 3:
                lbl_8047A458 = 9;
                break;
            case 4:
                lbl_8047A458 = 10;
                break;
            case 5:
            default:
                lbl_8047A458 = 4;
                break;
            }
            break;

        case 6:
            winMsgOpen(8, 0x3B8A, 0, 0);
            result = menuOpen(0xA9, 1);
            menuClose(0xA9);
            winMsgClose(1);
            switch (result) {
            case 1:
                lbl_8047A458 = 5;
                continue;
            case -1:
                lbl_8047A458 = 5;
                continue;
            case 0:
            default:
                break;
            }
            if (lbl_8047A450 > lbl_8047A44C) {
                std130_soundUnk_801653cc(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B8D, 1, 0);
                winMsgClose(1);
                progress = lbl_8047B9F8;
                while (progress < lbl_8047B9FC) {
                    _threadSwitch();
                    progress += (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
                }
            }
            if (lbl_8047A449 != 0) {
                heroPartyHealAll(0);
            }
            sysvarsLoadItems();
            result = 1;
            lbl_8047A458 = result;
            if (result != 0) {
                fadeSet(3, lbl_8047BA00);
                fadeCheck(1);
                menuClose(0xA5);
            }
            break;

        case 7:
            lbl_8047A444 = menu->floorId;
            if (*floorName != 0) {
                GScharCpy(lbl_803A3288, floorName);
            } else {
                entry = fightFloorDataBiosGetPtr((u16)lbl_8047A444);
                if (entry != 0) {
                    GScharCpy(lbl_803A3288, GSmsgGetGSchar(fightFloorDataBiosGetName(entry)));
                } else {
                    GScharCpy(lbl_803A3288, &lbl_8047BA1C);
                }
            }
            msgctrlSetValue(0x4D, name);
            winMsgOpen(8, 0x3B57, 1, 0);
            winMsgClose(1);
            lbl_8047A458 = 5;
            break;

        case 8:
            winMsgOpen(8, 0x3B5A, 1, 0);
            winMsgClose(1);
            lbl_8047A458 = 5;
            break;

        case 9:
            winMsgOpen(8, 0x3B5D, 1, 0);
            winMsgClose(1);
            lbl_8047A458 = 5;
            break;

        case 10:
            /* RULE-EXCEPTION(user-approved): codegen-only cast; the s8 selection is read back as a u8 local - see docs/RULE_EXCEPTIONS.md */
            cardeSelectOpponentLevel(&valid, (s8*)&selection, &value);
            cardeArenaRecordResult(valid, selection,
                        &lbl_803A3334.opponents[(s8)value]);
            amount = lbl_8047A450 + 1;
            if (amount > 0x98967F) {
                amount = 0x98967F;
            }
            msgctrlSetValue(0x2F, amount);
            winMsgOpen(8, 0x3B5E, 1, 0);
            winMsgClose(1);
            if (lbl_8047A450 == 0) {
                heroPartyHealAll(0);
            }
            lbl_8047A458 = 11;
            break;

        case 11:
            cardeBattleSelectCard();
            break;

        case 12:
            lbl_8047A44A = 0;
            if (lbl_8047A450 > lbl_8047A44C) {
                std130_soundUnk_801653cc(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B75, 1, 0);
                winMsgClose(1);
            }
            lbl_8047A43C = 0;
            for (index = 0; index < 5; index++) {
                lbl_803A3278.entries[index].floor = 0;
                lbl_803A3278.entries[index].region = 0;
                lbl_803A3278.entries[index].variant = 0;
            }
            heroPartyHealAll(0);
            winMsgOpen(8, 0x3BD7, 1, 0);
            result = menuOpen(0xA9, 1);
            menuClose(0xA9);
            winMsgClose(1);
            switch (result) {
            case 0:
                lbl_8047A458 = 3;
                continue;
            case 1:
                lbl_8047A458 = 1;
                break;
            case -1:
                lbl_8047A458 = 1;
                break;
            default:
                break;
            }
            progress = lbl_8047B9F8;
            while (progress < lbl_8047B9FC) {
                _threadSwitch();
                progress += (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
            }
            sysvarsLoadItems();
            if (lbl_8047A458 != 0) {
                fadeSet(3, lbl_8047BA00);
                fadeCheck(1);
                menuClose(0xA5);
            }
            break;

        case 13:
            cardeAwardCoupons();
            break;

        case 14:
            if (lbl_8047A450 > 0x1E) {
                winMsgOpen(8, 0x3B5C, 1, 0);
                winMsgClose(1);
            } else {
                winMsgOpen(8, 0x3B5F, 1, 0);
                winMsgClose(1);
            }
            lbl_8047A44A = 1;
            lbl_8047A458 = 11;
            break;
        }
    }

    if (floorIsLinking() != 0) {
        floorBattleEndTerminateScripts();
    } else {
        floorLink(0x80, 0);
    }
    lbl_8047A438 = 0;
}
