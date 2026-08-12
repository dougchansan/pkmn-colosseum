/**
 * @file gs_range_80033278.c
 * @brief gs-engine code, 0x80033278 - 0x80035E04 (20 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

extern void GBAInit(void);
extern void fn_8007B090(s32);
extern s32 windowGetActiveID(void);
extern u8* windowGetKeyInfo(void);
extern void fn_8007AAFC(void);
extern void fn_8007AAA8(void);
extern s32 fn_8007AB10(s32, u32*);
extern void fn_80166A28(u32);
extern void winMsgClose(s32);
extern void winMsgOpen(s32, u32, s32, s32);
extern void _threadSwitch(void);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern void fn_80080310(void*, const void*, u32);
extern u8 lbl_803A3334[];
extern u8 lbl_803F7A30[];
extern f32 lbl_8047B9F8;
extern f32 lbl_8047BA28;

s32 _sysvarsProcessData__FP16sysvarsFuncEntryPc(void* entry, char* data)
{
    extern u8 lbl_803A3278[];
    extern u32 lbl_8047A430;
    extern u32 lbl_8047A434;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u32 lbl_8047A43C;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u8 lbl_8047A44A;
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern u8 lbl_8047A454;
    extern u32 lbl_8047A458;
    extern f32 lbl_8047B9F8;
    extern f32 lbl_8047B9FC;
    extern f32 lbl_8047BA00;
    extern f64 lbl_8047BA08;
    extern f64 lbl_8047BA10;
    extern u8 lbl_8047BA1C[];
    extern void fn_8003258C(void);
    extern void fn_800327FC(void);
    extern s32 fn_80033278(void);
    extern s32 fn_80032ED8(s32, s32, s32);
    extern s32 fn_80034280(void);
    extern void fn_80034830(u8, u8, const void*);
    extern void fn_80034B5C(u8*, u8*, u8*);
    extern void* fn_80082EA4(void*, s32, s32, s32);
    extern void* fn_800836AC(s32, void*, s32);
    extern s32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern void fn_800F9E70(void);
    extern u32 fn_800FA280(u32);
    extern u8 fn_800FF52C(void);
    extern void fn_800FF660(void);
    extern void fn_80102510(void);
    extern void fn_80102620(void);
    extern s32 fn_8010264C(void);
    extern s32 fn_801026A4(void);
    extern s32 fn_801046B8(void);
    extern void fn_801069FC(void);
    extern void fn_80106D3C(void);
    extern void fn_80113828(s32, s32);
    extern void fn_8011DCB4(void*, u16);
    extern s32 fn_8011F1A0(void*);
    extern void* fn_80129280(s32, s32);
    extern void fn_8012AC08(void*, s32);
    extern void fn_80132A38(s32, void*);
    extern void fn_80165668(s32, s32, s32);
    extern void fn_80166A28(s32);
    extern void fn_80166AB8(s32, s32, s32);
    extern void fn_801C40F0(s32);
    extern void fn_801C41C8(s32);
    extern void fn_801D0AFC(s32);
    extern void* savedataGetStatus(s32, s32);
    extern void* heroBiosGetPokemonPtr(void*, u8);
    extern void heroAddPokecoupon(s32, s32);
    extern void pokemonBiosSetItemDataId(void*, u16);
    extern void msgctrlSetValue(s32, void*);
    extern void GScharCpy(void*, const void*);
    extern u32 fightFloorDataBiosGetPtr(u16);
    extern u32 fightFloorDataBiosGetName(u32);
    extern void* GSmsgGetGSchar(u32);
    extern void fn_8007C764(u8);
    extern void fn_8007C7A8(s32);
    extern s32 menuOpen(s32, s32);
    extern void fn_8007C7EC(void);
    extern void fn_8007CAB0(void);
    extern void fn_8007CB54(void*);
    extern void fn_8007C450(u8, u8, u8, u8, s32);
    extern void fn_8007C300(u8, u8);
    extern void fn_8007C414(void);
    extern void fn_8007C634(void);
    extern void fn_80082CF0(void*, void*, u8);
    extern u8 fn_80082738(void*, void*, u8);
    extern void fn_80082650(void*);
    extern void fn_80082BA4(void*, void*, u8);
    extern void fn_80082960(void*, void*, u8);
    extern void fn_800832C8(s32, void*, u8);
    extern u16 pcboxDelItem(s32, u16, s32);
    extern s32 heroItemAddItemDataId(s32, u16, s32, s32);
    extern void menuClose(s32);
    extern s32 menuOpenCustom(s32, ...);
    u8* base;
    u8* menuState;
    u8* itemState;
    void* save;
    void* party;
    void* item;
    s32 stateResult;
    s32 menuResult;
    s32 effectType;
    s32 itemResult;
    s32 i;
    s32 current;
    s32 randomPick;
    u8 selection;
    u8 region;
    f32 progress;

    (void)entry;
    (void)data;

    base = lbl_803A3278;
    lbl_8047A438 = 1;
    menuState = base + 0xBC;
    itemState = base + 0x10;

    fn_8007C764((u8)(lbl_8047A439 == 0));
    fn_8007C7A8(1);
    menuResult = menuOpen(0xA6, 1);
    fn_8007C7A8(0);
    if (menuResult != 0) {
        return 1;
    }
    if (lbl_8047A439 != 0) {
        return 2;
    }

    while (1) {
        stateResult = fn_80034280();
        if (stateResult == 0) {
            lbl_8047A438 = 0;
            return 0;
        }
        if (stateResult == 1) {
            lbl_8047A438 = 0;
            return 2;
        }

        if (*(s32*)(menuState + 0x0) == 1) {
            lbl_8047A444 = *(u32*)(menuState + 0xAFC);
            if (*(u16*)(menuState + 0xB00) != 0) {
                GScharCpy(base + 0x10, menuState + 0xB00);
            } else {
                current = (u16)lbl_8047A444;
                item = (void*)fightFloorDataBiosGetPtr((u16)current);
                if (item != NULL) {
                    GScharCpy(base + 0x10, GSmsgGetGSchar(fightFloorDataBiosGetName((u32)item)));
                } else {
                    GScharCpy(base + 0x10, lbl_8047BA1C);
                }
            }
            fn_8007CB54(base + 0x10);
            msgctrlSetValue(0x4D, base + 0x10);
            winMsgOpen(8, 0x3B55, 1, 0);
            winMsgClose(1);
            lbl_8047A438 = 0;
            return 2;
        }

        fn_8007CAB0();
        lbl_8047A434 = (u32)fn_800836AC(0, menuState, 1);
        fn_8007C7EC();
        if (lbl_8047A434 == 0) {
            winMsgOpen(8, 0x3B91, 1, 0);
            lbl_8047A438 = 0;
            return 0;
        }

        current = (s8)menuState[0x58] - 1;
        item = fn_80082EA4((void*)lbl_8047A434, current, menuState[0x24], menuState[0x26]);
        effectType = 4;
        if (*(u8*)((u8*)item + 0x0C) != 0) {
            effectType = 2;
        }

        if (effectType == 3) {
            selection = 0;
        } else if (effectType == 4) {
            selection = 0;
            fn_80034B5C(&selection, base + 0x48, base + 0x49);
        } else {
            selection = 0;
        }

        if (effectType < 3) {
            winMsgOpen(8, 0x3B58, 1, 0);
            winMsgClose(1);
            lbl_8047A438 = 0;
            return 2;
        }

        region = menuState[0x24];
        current = (s8)menuState[0x5E + (s8)base[0x48]];
        if (current >= 0) {
            msgctrlSetValue(0x31, (void*)0x3C7C);
            msgctrlSetValue(0x4D, menuState + 0x28 + (s8)base[0x48] * 0x10);
            msgctrlSetValue(0x57, menuState + 0x38 + (s8)base[0x48] * 0x10);
            fn_8007C450(menuState[0x08], base[0x48], region, menuState[0x26], 6);
            winMsgOpen(8, 0x3B5B, 1, 0);
            winMsgClose(1);
            fn_8007C414();
            lbl_8047A438 = 0;
            return 2;
        }

        fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], menuState[0x26], 1);
        fn_80166A28(0x4C5);
        progress = lbl_8047B9F8;
        while (progress < lbl_8047B9FC) {
            _threadSwitch();
            progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
        winMsgOpen(8, 0x3B6D, 1, 0);
        fn_801D0AFC(0);
        save = savedataGetStatus(0, 2);
        for (i = 0; i < 6; i++) {
            party = heroBiosGetPokemonPtr(save, (u8)i);
            pokemonBiosSetItemDataId(party, *(u16*)(base + 0xB0 + i * 2));
        }

        while (1) {
            current = (s8)base[0x48];
            itemResult = fn_80032ED8((s32)(menuState + 0x3AC + (s8)menuState[0x5B + current] * 0x28),
                                     8, (s32)itemState);
            menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, itemState);
            fn_8007C300(menuState[0x08], base[0x48]);
            if (itemResult == 2) {
                break;
            }
            fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], -1, 7);
            fn_80166A28(0x4C6);
            progress = lbl_8047B9F8;
            while (progress < lbl_8047B9FC) {
                _threadSwitch();
                progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            fn_8007CAB0();
            if (fn_80082738((void*)lbl_8047A434, menuState, base[0x48]) != 0) {
                fn_8007C414();
                fn_8007C634();
                fn_8007CAB0();
                fn_80082650((void*)lbl_8047A434);
            }
            fn_8007C414();
            fn_8007C7EC();
            lbl_8047A438 = 0;
            return 2;
        }

        fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], menuState[0x26], 2);
        progress = lbl_8047B9F8;
        while (progress < lbl_8047B9FC) {
            _threadSwitch();
            progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
        fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], menuState[0x26], 4);
        fn_80166A28(0x3C6);
        progress = lbl_8047B9F8;
        while (progress < lbl_8047B9FC) {
            _threadSwitch();
            progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
        fn_80166AB8(0x3CC, 0, 0);
        heroAddPokecoupon(0, 0x32);
        winMsgOpen(8, 0x3B72, 1, 0);

        current = (s8)base[0x48];
        selection = 1;
        if ((s8)menuState[0x5E + current] >= 0) {
            for (i = 0; i < (s8)menuState[0x5A]; i++) {
                item = fn_80082EA4((void*)lbl_8047A434, current, menuState[0x24], (u8)i);
                if (*(u8*)((u8*)item + 0x0C) == 0) {
                    selection = 0;
                    break;
                }
            }
        } else {
            selection = 0;
        }

        if (selection == 0) {
            fn_80082BA4((void*)lbl_8047A434, menuState, base[0x48]);
            fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], -1, 1);
            fn_80166A28(0x4C5);
            progress = lbl_8047B9F8;
            while (progress < lbl_8047B9FC) {
                _threadSwitch();
                progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            winMsgOpen(8, 0x3B74, 1, 0);
            current = (s8)base[0x48];
            itemResult = fn_80032ED8((s32)(menuState + 0x3AC + (s8)menuState[0x5E + current] * 0x28),
                                     9, (s32)itemState);
            menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, itemState);
            fn_8007C300(menuState[0x08], base[0x48]);
            if (itemResult != 2) {
                fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], -1, 7);
                fn_80166A28(0x4C6);
                progress = lbl_8047B9F8;
                while (progress < lbl_8047B9FC) {
                    _threadSwitch();
                    progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
                fn_8007CAB0();
                fn_80082960((void*)lbl_8047A434, menuState, base[0x48]);
                fn_8007C414();
                fn_8007C634();
                fn_8007CAB0();
                fn_80082650((void*)lbl_8047A434);
                fn_8007C414();
                fn_8007C7EC();
                lbl_8047A438 = 0;
                return 2;
            }
            fn_8007C450(menuState[0x08], base[0x48], menuState[0x24], menuState[0x26], 4);
            progress = lbl_8047B9F8;
            while (progress < lbl_8047B9FC) {
                _threadSwitch();
                progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
        }

        item = fn_80082EA4((void*)lbl_8047A434, (s8)base[0x48], menuState[0x24], menuState[0x26]);
        fn_80034830(*(u8*)((u8*)item + 0x0C), base[0x48], item);
        lbl_8047A43C++;
        if (*(u8*)((u8*)item + 0x0D) != 0) {
            randomPick = fn_800FA280(2);
            effectType = (*(u8*)((u8*)item + 0x0E) >> (randomPick * 4)) & 0xF;
            if (effectType == 0) {
                effectType = 1;
            }
            switch (effectType) {
            case 1:
                winMsgOpen(8, 0x3B76, 1, 0);
                break;
            case 2:
                winMsgOpen(8, 0x3B77, 1, 0);
                break;
            case 3:
                winMsgOpen(8, 0x3B78, 1, 0);
                break;
            case 4:
                winMsgOpen(8, 0x3B79, 1, 0);
                break;
            default:
                winMsgOpen(8, 0x3B76, 1, 0);
                break;
            }
            winMsgClose(1);
        }

        if (fn_800FF52C() != 0) {
            fn_800FF660();
        } else {
            fn_80113828(0x80, 0);
        }
        lbl_8047A438 = 0;
        return 0;
    }
}

static void sysvarsWaitForTransfer(void)
{
    f32 progress;

    progress = lbl_8047B9F8;
    while (progress < lbl_8047BA28) {
        _threadSwitch();
        progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}

/* Run the GBA transfer UI until it finishes or the user cancels it. */
s32 fn_80034280(void)
{
    s32 watchedWindow;
    s32 state;
    s32 nextState;
    u32 transferActive;
    u8 waitForWindow;

    watchedWindow = -1;
    GBAInit();
    fn_8007B090(0);
    transferActive = 0;
    state = 1;
    waitForWindow = 0;

    for (;;) {
        if (windowGetActiveID() == watchedWindow &&
            (*(u16*)(windowGetKeyInfo() + 4) & 0x20) != 0) {
            if (state == 18) {
                fn_8007AAFC();
            } else {
                if (transferActive != 0) {
                    fn_8007AAA8();
                }
                fn_80166A28(0x3C7);
                winMsgClose(1);
                winMsgOpen(8, 0x44ED, 1, 0);
                winMsgClose(1);
                return 1;
            }
        }

        nextState = fn_8007AB10(state, &transferActive);
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
            sysvarsWaitForTransfer();
            fn_80166A28(0x3C7);
            if (transferActive != 0) {
                fn_8007AAA8();
            }
            winMsgOpen(8, 0x3B91, 1, 0);
            return 0;

        case 17:
            winMsgOpen(8, 0x3B88, 0, 0);
            sysvarsWaitForTransfer();
            state = 18;
            fn_8007AB10(state, &transferActive);
            break;

        case 18:
            winMsgOpen(8, 0x3B8E, 0, 0);
            watchedWindow = windowGetActiveID();
            waitForWindow = 1;
            break;

        case 19:
            fn_80166A28(0x3C7);
            if (transferActive != 0) {
                fn_8007AAA8();
            }
            winMsgOpen(8, 0x3B91, 1, 0);
            return 0;

        case 20:
            fn_8007AAA8();
            fn_80080310(lbl_803A3334, lbl_803F7A30 + 0x2388,
                        0x1040);
            return 2;

        case 21:
            if (transferActive != 0) {
                fn_8007AAA8();
            }
            fn_80166A28(0x3C7);
            winMsgClose(1);
            winMsgOpen(8, 0x44ED, 1, 0);
            winMsgClose(1);
            return 1;
        }
    }
}

/* fn_800345A4 - 0x800345A4 | size: 0x164 */
void fn_800345A4(void* unused, u8* sprite) {
    extern u32 lbl_802E61D8[];
    extern void winSpriteSetDisp(u8*, u8);
    extern u32 fn_8012A5B0(s32, s32, s32);
    u32 value;
    s16 id;

    value = fn_8012A5B0(0, 0xE, 0);
    id = *(s16*)(sprite + 6);
    switch (id) {
    case 0x7BC:
    case 0x7F1:
        winSpriteSetDisp(sprite, value < lbl_802E61D8[3]);
        break;
    case 0x7F0:
    case 0x805:
        winSpriteSetDisp(sprite, value >= lbl_802E61D8[2]
                              && value < lbl_802E61D8[3]);
        break;
    case 0x7EF:
    case 0x804:
        winSpriteSetDisp(sprite, value >= lbl_802E61D8[1]
                              && value < lbl_802E61D8[2]);
        break;
    case 0x7EE:
    case 0x803:
        winSpriteSetDisp(sprite, value >= lbl_802E61D8[0]
                              && value < lbl_802E61D8[1]);
        break;
    }
}

/* fn_80034708 - 0x80034708 | size: 0xB0 */
#pragma push
#pragma peephole off
void fn_80034708(void* unused, u8* sprite) {
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
#pragma pop

/* fn_800347B8 - 0x800347B8 | size: 0xC */
void fn_800347B8(void) {
    extern u8 lbl_8047A440;

    lbl_8047A440 = 1;
}

/* fn_800347C4 - 0x800347C4 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_800347C4(void) {
    extern void fn_80166A28(u32);

    fn_80166A28(0x26);
}
#pragma pop

/* fn_800347E8 - 0x800347E8 | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_800347E8(void) {
    extern void fn_80166A28(u32);

    fn_80166A28(0x26);
}
#pragma pop

/* fn_8003480C - 0x8003480C | size: 0x24 */
#pragma push
#pragma scheduling off
void fn_8003480C(void) {
    extern void fn_80166A28(u32);

    fn_80166A28(0x26);
}
#pragma pop

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
    s8 region;
    s8 variant;
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

void fn_80034830(u8 enabled, u8 selection,
                 const SysvarsOpponentSource* source)
{
    extern u8 lbl_803A3334[];
    extern u8* lbl_8047A430;
    extern void GScharCpy(void*, const void*);
    SysvarsOpponentSnapshot* history;
    SysvarsOpponentSnapshot* destination;
    s32 moveIndex;
    s32 i;

    history = (SysvarsOpponentSnapshot*)(lbl_8047A430 + 0x41E4);
    for (i = 0; i < 10; i++) {
        if (history[i].trainerType == lbl_803A3334[8] &&
            history[i].region == (s8)lbl_803A3334[0x24] &&
            history[i].variant == (s8)lbl_803A3334[0x26]) {
            return;
        }
    }

    for (i = 0; i < 9; i++) {
        history[i] = history[i + 1];
    }

    destination = &history[9];
    destination->trainerType = lbl_803A3334[8];
    destination->region = lbl_803A3334[0x24];
    destination->variant = lbl_803A3334[0x26];
    destination->enabled = enabled;
    destination->selection = selection;
    GScharCpy(destination->name, source->name);
    destination->category = source->category;
    destination->field_14 = source->field_12;
    destination->field_16 = source->field_14;
    destination->field_18 = source->field_16;
    destination->field_1A = source->field_18;
    destination->field_1C = source->field_1C;
    destination->field_1E = source->field_20;
    destination->field_20 = source->field_24;

    for (i = 0; i < 4; i++) {
        moveIndex = source->moveIndex[i];
        if (moveIndex < 0) {
            *(u16*)destination->moves[i].data = 0;
        } else {
            destination->moves[i] =
                *(SysvarsMoveSnapshot*)(lbl_803A3334 + 0x514 +
                                        moveIndex * 0x2A);
        }
    }
}

void fn_80034B5C(u8* valid, u8* selectionOut, u8* valueOut)
{
    extern u8 lbl_803A3334[];
    extern void* lbl_8047A434;
    extern void* heroGetStatus(s32, s32, s32);
    extern u8 pokemonCheckValid(void*);
    extern s32 pokemonGetStatus(void*, s32, s32, s32);
    extern void* fn_80082FE4(void*, s8);
    extern u32 _fadeEffectGetRandom__FUl(u32);
    s32 score[6];
    u8* pokemon;
    u8* layer;
    u8* group;
    s32 maxLevel;
    s32 maxRequired;
    s32 best;
    s32 ties;
    s32 candidate;
    s32 party;
    s32 slot;
    s32 index;
    s32 difference;
    s32 count;

    maxLevel = 1;
    for (party = 0; party < 6; party++) {
        pokemon = heroGetStatus(0, 3, party);
        if (pokemonCheckValid(pokemon) == 1) {
            index = pokemonGetStatus(pokemon, 0, 0x7A, 0);
            if (maxLevel < index) {
                maxLevel = index;
            }
        }
    }

    count = (s8)lbl_803A3334[0x58];
    best = 0;
    ties = 1;
    for (candidate = 0; candidate < count; candidate++) {
        score[candidate] = 0x7FFFFFFF;
        layer = fn_80082FE4(lbl_8047A434, candidate);
        if (layer[0x1C + (s8)lbl_803A3334[0x24] * 0xE] != 0) {
            group = lbl_803A3334 +
                    (s8)lbl_803A3334[0x5E + candidate] * 0x28;
            maxRequired = 0;
            for (slot = 0; slot < 4; slot++) {
                index = (s8)group[0x3B9 + slot];
                if (index >= 0 &&
                    maxRequired < lbl_803A3334[index * 0x2A + 0x517]) {
                    maxRequired = lbl_803A3334[index * 0x2A + 0x517];
                }
            }
            difference = maxLevel - maxRequired;
            if (difference < 0) {
                difference = -difference;
            }
            score[candidate] = difference;
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

    *valid = score[best] != 0x7FFFFFFF;
    if (*valid != 0) {
        *selectionOut = best;
        *valueOut = lbl_803A3334[0x5E + (s8)*selectionOut];
    } else {
        *selectionOut = count - 1;
        *valueOut = lbl_803A3334[0x5B + (s8)*selectionOut];
    }
}

/* fn_80034DC0 - 0x80034DC0 | size: 0x78 */
#pragma push
#pragma peephole off
void fn_80034DC0(u8* arg0, u8* arg1) {
    extern u32 lbl_8047A44C;
    extern void fn_800FB680(s32, s32, u32, u16);
    extern void msgctrlSetValue(s32, s32);
    extern void fn_800FBB34(s32, s32, s32, s32, u32, u16);
    u8 byte;
    s32 mask;
    s32 combined;

    byte = arg0[0x8B];
    mask = -0x100;
    combined = byte | mask;
    fn_800FB680(0, 0, combined, 0x3CC8);
    msgctrlSetValue(0x34, lbl_8047A44C);
    fn_800FBB34(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), combined, 0x3CC9);
}
#pragma pop

/* fn_80034E38 - 0x80034E38 | size: 0xB8 */
#pragma push
#pragma peephole off
void fn_80034E38(u8* arg0, u8* arg1) {
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern void fn_800FB680(s32, s32, u32, u16);
    extern void msgctrlSetValue(s32, u32);
    extern void fn_800FBB34(s32, s32, s32, s32, u32, u16);
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
    fn_800FB680(0, 0, value, 0x3CC7);
    if (lbl_8047A450 <= 0x98967F) {
        msgctrlSetValue(0x34, lbl_8047A450);
    } else {
        msgctrlSetValue(0x34, 0x98967F);
    }
    fn_800FBB34(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), value, 0x3CC9);
}
#pragma pop

/* fn_80034EF0 - 0x80034EF0 | size: 0x94 */
#pragma push
#pragma peephole off
#pragma optimization_level 1
void fn_80034EF0(u8* arg0, u8* arg1) {
    extern void fn_800FB680(s32, s32, u32, u16);
    extern s32 heroGetStatus(s32, s32, s32);
    extern void msgctrlSetValue(s32, s32);
    extern void fn_800FBB34(s32, s32, s32, s32, u32, u16);
    s32 mask;
    s32 value;
    s32 status;
    s16 position;

    value = arg0[0x8B];
    mask = -0x100;
    value |= mask;
    fn_800FB680(0, 0, value, 0x3CC6);
    status = heroGetStatus(0, 0xD, 0);
    msgctrlSetValue(0x34, status);
    position = *(s16*)(arg1 + 0x54) + 2;
    fn_800FBB34(0, 0, position, *(s16*)(arg1 + 0x56), value, 0x3CD3);
}
#pragma pop

/* fn_80034F84 - 0x80034F84 | size: 0x2C */
#pragma push
#pragma optimize_for_size on
void fn_80034F84(void) {
    extern void msgctrlSetValue();
    extern u8 lbl_803A3288[];
#pragma peephole off
    msgctrlSetValue(0x37, lbl_803A3288);
}
#pragma peephole on
#pragma optimize_for_size reset
#pragma pop

/* fn_80034FB0 - 0x80034FB0 | size: 0x4 */
void fn_80034FB0(void) {
}

/* fn_80035C48 - 0x80035C48 | size: 0x128 */
void fn_80035C48(void) {
    extern u8 lbl_803A3288[];
    extern u32 lbl_8047A430;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u8 lbl_8047A440;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u32 lbl_8047A44C;
    extern u8 lbl_8047A454;
    extern u32 lbl_8047A458;
    extern void* fn_80083AF4(s32, s32);
    extern s32 fn_80083BF8(s32);
    extern u32 fn_800FA280(u32);
    extern u32 fn_80113F48(void);
    extern u8* fn_80129280(s32, s32);
    extern u32 fn_801653C4(void);
    extern void fn_80165A20(u32, s32, s32);
    extern void fn_80176E0C(u32, u32, s32, s32);
    extern void GScharLenCpy(void*, u32, s32);
    u8* work;
    u32 soundId;
    u8 resetFlag;

    if (lbl_8047A438 == 0) {
        if (lbl_8047A439 != 0) {
            lbl_8047A458 = 2;
        } else {
            lbl_8047A458 = 1;
        }
        fn_80176E0C(fn_80113F48(), 0x11171800, 0, 0);
        if (lbl_8047A439 != 0) {
            soundId = 0x446;
        } else {
            soundId = 0x4CD;
        }
        if (soundId != fn_801653C4()) {
            fn_80165A20(soundId, 0, 0x7F);
        }
        lbl_8047A454 = 0;
        lbl_8047A449 = 0;
        lbl_8047A444 = 0x21;
        GScharLenCpy(lbl_803A3288, fn_800FA280(0x3CD1), 0x50);
        *(u16*)(lbl_803A3288 + 0x9E) = 0;
        work = fn_80129280(0, 0xD);
        lbl_8047A430 = (u32)work;
        resetFlag = lbl_8047A440;
        lbl_8047A44C = *(u32*)(work + 0x49C8);
        if (resetFlag != 0) {
            if (fn_80083BF8(0) > 0) {
                *(u16*)fn_80083AF4(0, 0) = 0;
            }
            lbl_8047A440 = 0;
        }
    }
}

/* fn_80035D70 - 0x80035D70 | size: 0x30 */
#pragma push
#pragma scheduling off
#pragma optimize_for_size on
void fn_80035D70(void) {
    extern void fn_800FF730(u32);
    extern void _threadSwitch(void);
    extern volatile u8 lbl_8047A439;

    fn_800FF730((lbl_8047A439 = 1, 0x393));
    _threadSwitch();
}
#pragma optimize_for_size reset
#pragma pop

/* fn_80035DA0 - 0x80035DA0 | size: 0x34 */
#pragma push
#pragma scheduling off
#pragma optimize_for_size on
void fn_80035DA0(void) {
    extern void floorLink(s32, s32);
    extern void _threadSwitch(void);
    extern volatile u8 lbl_8047A439;

    floorLink((lbl_8047A439 = 0, 0x393), 0);
    _threadSwitch();
}
#pragma optimize_for_size reset
#pragma pop

/* fn_80035DD4 - 0x80035DD4 | size: 0x30 */
#pragma push
#pragma scheduling off
#pragma optimize_for_size on
void fn_80035DD4(void) {
    extern void fadeSet(f32, u32);
    extern void fadeCheck(u32);
    extern f32 lbl_8047BA30;

    fadeSet(lbl_8047BA30, 3);
    fadeCheck(1);
}
#pragma optimize_for_size reset
#pragma pop
