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
#pragma push
#pragma peephole off
void fn_800345A4(void* unused, u8* sprite) {
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
#pragma pop

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
    u32 byte;
    s32 value;
    s32 status;
    s16 position;

    byte = arg0[0x8B];
    mask = -0x100;
    value = byte | mask;
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
#pragma push
#pragma peephole off
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
    extern u32 GSmsgGetGSchar(u32);
    extern u32 fn_80113F48(void);
    extern u8* savedataGetStatus(s32, s32);
    extern u32 fn_801653C4(void);
    extern void fn_80165A20(u32, s32, s32);
    extern void cameraPlayAnime(u32, u32, s32, s32);
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
        cameraPlayAnime(fn_80113F48(), 0x11171800, 0, 0);
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
        GScharLenCpy(lbl_803A3288, GSmsgGetGSchar(0x3CD1), 0x50);
        *(u16*)(lbl_803A3288 + 0x9E) = 0;
        work = savedataGetStatus(0, 0xD);
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
#pragma pop

/* fn_80035D70 - 0x80035D70 | size: 0x30 */
#pragma push
#pragma peephole off
void fn_80035D70(void) {
    extern void fn_800FF730(u32);
    extern void _threadSwitch(void);
    extern u8 lbl_8047A439;
    u32 flag;

    flag = 1;
    lbl_8047A439 = flag;
    fn_800FF730(0x393);
    _threadSwitch();
}
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
