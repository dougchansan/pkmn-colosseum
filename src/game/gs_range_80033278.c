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
extern s32 fn_8007AB10(s32, s32*);
extern void fn_80166A28(u32);
extern void winMsgClose(s32);
extern void winMsgOpen(s32, u32, s32, s32);
extern void _threadSwitch(void);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);
extern void fn_80080310(void*, const void*, u32);
extern u8 lbl_803A3334[];
extern u8 lbl_803F7A30[];
extern const f32 lbl_8047B9F8;
extern const f32 lbl_8047BA28;

extern u8 lbl_803A3278[];

extern u8 lbl_8047A439;
extern void* lbl_8047A434;
extern u32 lbl_8047A444;
extern s8 lbl_8047A448;

extern f32 lbl_8047BA18;

extern char lbl_80266FAC[];
extern char lbl_80266FB8[];
extern char lbl_8047BA20[];

extern void fn_8007C764(s32);
extern void fn_8007C7A8(s32);

extern s32 menuOpen(s32, s32);
extern void menuClose(s32);
extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, ...);

extern s32 fn_80034280(void);

extern void GScharCpy(void*, const void*);
extern void* fightFloorDataBiosGetPtr(u16);
extern void* fightFloorDataBiosGetName(void*);
extern void* GSmsgGetGSchar(void*);

extern void fn_8007CB54(void*);

/*
 * The repository elsewhere declares this as two fixed 32-bit arguments.
 * Keeping it non-variadic is important on PPC because variadic calls affect
 * the generated CR setup.
 */
extern void msgctrlSetValue(u32, u32);

extern void winMsgOpen(s32, u32, s32, s32);
extern void winMsgClose(s32);

extern void __assert(const char*, s32, const char*);

extern void fn_8007CAB0(void);
extern void* fn_800836AC(s32, void*, s32);
extern void fn_8007C7EC(void);

extern u8* fn_80082EA4(void*, s32, s32, s32);
extern u8* fn_80082FE4(void*, s32);

extern void fn_8007C450(s32, s32, s32, s32, s32);
extern void fn_8007C414(void);

extern void fn_80082CF0(void*, void*, s32);
extern void fn_80082BA4(void*, void*, s32);
extern void fn_80082960(void*, void*, s32);

extern void fn_80166A28(u32);

extern void _threadSwitch(void);
extern s32 fn_800D37CC(void);
extern u32 fn_800D3088(void);

extern void fn_801D0AFC(s32);
extern void* savedataGetStatus(s32, s32);
extern void* heroBiosGetPokemonPtr(void*, u8);
extern void pokemonBiosSetItemDataId(void*, u16);

extern u32 fn_80032ED8(void*, s32, s32);
extern s32 windowGetActiveID(void);

extern void fn_8007C300(s32, s32);

extern u8 fn_80082738(void*, void*, s32);
extern void fn_8007C634(void);
extern void fn_80082650(void*);

extern void fn_80166AB8(s32, s32, s32);
extern void heroAddPokecoupon(s32, s32);

extern s32 heroItemAddItemDataId(s32, u16, s32, s32);
extern u16 pcboxDelItem(s32, u16, s32);

extern void fn_80165668(s32, s32, s32);
extern void fn_800832C8(s32, void*, s32);

#pragma push
#pragma peephole off
static void sysvarsWaitForTransfer(void)
{
    f32 progress;

    progress = lbl_8047B9F8;
    while (progress < lbl_8047BA28) {
        _threadSwitch();
        progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}

s32 _sysvarsProcessData__FP16sysvarsFuncEntryPc(
    void* unusedEntry,
    char* unusedText)
{
    extern u8 lbl_8047BA1C;
    u8* base;
    u8* name;
    u16* partyItems;
    u8* sv;

    s32 state;
    s32 action;
    s32 secondary;
    s32 empty;
    s32 i;
    s32 j;
    s32 slot;
    s32 region;
    s32 menuResult;
    s32 choice;

    u8 valid;
    u8* ptr;

    u16 item;
    u32 message;

    (void)unusedEntry;
    (void)unusedText;

    base = lbl_803A3278;

    name = base + 0x10;
    partyItems = (u16*)(base + 0xB0);
    sv = base + 0xBC;

    /*
     * Original:
     *   fn_8007C764(lbl_8047A439 == 0);
     *   fn_8007C7A8(1);
     *   menuOpen(0xA6, 1);
     */
    fn_8007C764(lbl_8047A439 == 0);

    fn_8007C7A8(1);
    state = menuOpen(0xA6, 1);
    fn_8007C7A8(0);

    if (state != 0) {
        return 1;
    }

    if (lbl_8047A439 != 0) {
        return 2;
    }

restart:

    state = fn_80034280();

    if (state == 0) {
        return 0;
    }

    if (state == 1) {
        return 2;
    }

    /*
     * sv[0..3] is a state/mode field.
     */
    if (*(s32*)sv == 1) {
        lbl_8047A444 = *(u32*)(sv + 0xAFC);

        if (*(u16*)(sv + 0xB00) != 0) {
            GScharCpy(name, sv + 0xB00);
        } else {
            ptr = fightFloorDataBiosGetPtr((u16)lbl_8047A444);

            if (ptr != NULL) {
                ptr = fightFloorDataBiosGetName(ptr);
                ptr = GSmsgGetGSchar(ptr);
                GScharCpy(name, ptr);
            } else {
                GScharCpy(name, &lbl_8047BA1C);
            }
        }

        fn_8007CB54(name);

        msgctrlSetValue(0x4D, (u32)name);

        winMsgOpen(8, 0x3B55, 1, 0);
        winMsgClose(1);

        return 2;
    }

    if (*(s32*)sv != 0) {
        __assert(lbl_80266FAC, 0x6C8, lbl_80266FB8);
    }

    fn_8007CAB0();

    lbl_8047A434 = fn_800836AC(0, sv, 1);

    fn_8007C7EC();

    if (lbl_8047A434 == NULL) {
        __assert(lbl_80266FAC, 0x6DB, lbl_8047BA20);

        winMsgOpen(8, 0x3B91, 1, 0);

        return 0;
    }

    /*
     * Determine which path is currently possible.
     *
     * 2 = failure/message path
     * 3 = alternate availability path
     * 4 = normal path
     */
    action = 4;

    slot = (s8)(sv[0x58] - 1);

    ptr = fn_80082EA4(
        lbl_8047A434,
        slot,
        sv[0x24],
        sv[0x26]);

    if (ptr[0x0C] != 0) {
        action = 2;
    } else {
        secondary = 4;
        empty = -1;

        i = 0;

        while (i < (s8)sv[0x58]) {
            ptr = fn_80082EA4(
                lbl_8047A434,
                (s8)i,
                sv[0x24],
                sv[0x26]);

            if (ptr[0x0C] == 0) {
                empty = i;
                break;
            }

            i++;
        }

        switch (empty) {
        case -1:
            secondary = 2;
            break;

        case 1:
        case 2:
            j = 0;

            while ((s8)j < empty) {
                region = (s8)sv[0x24];

                /*
                 * The target assembly literally contains a self-compare
                 * here (cmpw r21,r21). Preserve this odd shape initially;
                 * it may be the result of an optimized source condition.
                 */
                if (region == region &&
                    (s8)sv[0x5E + j] < 0) {
                    valid = 0;
                } else {
                    ptr = fn_80082FE4(lbl_8047A434, j);

                    if (ptr[0x1C + region * 0x0E] != 0) {
                        valid = 0;
                    } else {
                        valid = 1;
                    }
                }

                if (valid != 0) {
                    secondary = 3;
                    break;
                }

                j++;
            }

            break;
        }

        if (secondary != 4) {
            action = secondary;
        }
    }

    /*
     * Alternate availability/error path.
     */
    if (action == 3) {
        slot = 0;

        while ((s8)slot < (s8)sv[0x58]) {
            region = (s8)sv[0x24];

            /*
             * The second instance also compiles to a self-compare in
             * the original assembly.
             */
            if (region == region &&
                (s8)sv[0x5E + slot] < 0) {
                valid = 0;
            } else {
                ptr = fn_80082FE4(lbl_8047A434, slot);

                if (ptr[0x1C + region * 0x0E] != 0) {
                    valid = 0;
                } else {
                    valid = 1;
                }
            }

            if (valid != 0) {
                break;
            }

            slot++;
        }

        /*
         * Message chosen from the byte at lbl_8047A434 +
         * 0x1E + region.
         *
         * The assembly contains a real 8-entry jump table here.
         */
        switch (((u8*)lbl_8047A434)[
            0x1E + (s8)sv[0x24]]) {
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

        msgctrlSetValue(
            0x4D,
            (u32)(sv + 0x28 + (s8)slot * 0x10));

        msgctrlSetValue(
            0x57,
            (u32)(sv + 0x28 +
                  ((s8)slot + 1) * 0x10));

        fn_8007C450(
            sv[8],
            slot,
            sv[0x24],
            sv[0x26],
            6);

        winMsgOpen(8, 0x3B5B, 1, 0);
        winMsgClose(1);

        fn_8007C414();

        return 2;
    }

    /*
     * action == 2 (and any other non-normal value).
     */
    if (action != 4) {
        winMsgOpen(8, 0x3B58, 1, 0);
        winMsgClose(1);

        return 2;
    }

    /*
     * Locate first empty candidate.
     */
    slot = -1;
    i = 0;

    while (i < (s8)sv[0x58]) {
        ptr = fn_80082EA4(
            lbl_8047A434,
            (s8)i,
            sv[0x24],
            sv[0x26]);

        if (ptr[0x0C] == 0) {
            slot = i;
            break;
        }

        i++;
    }

    lbl_8047A448 = (s8)slot;

    winMsgClose(1);

    fn_80082CF0(
        lbl_8047A434,
        sv,
        (u8)lbl_8047A448);

    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        sv[0x26],
        1);

    fn_80166A28(0x4C5);

    SYSVARS_WAIT_EFFECT();

    winMsgOpen(8, 0x3B6D, 1, 0);
    winMsgClose(1);

    SYSVARS_RESTORE_PARTY_ITEMS();

first_menu:

    /*
     * NOTE OFFSET 0x5B HERE.
     */
    menuResult = fn_80032ED8(
        sv + 0x3AC +
            (s8)sv[0x5B + (s8)lbl_8047A448] *
                0x28,
        8,
        0);

    /*
     * This call really is variadic in terms of generated ABI:
     * the target has a crclr immediately before menuOpenCustom.
     */
    menuOpenCustom(
        0xA6,
        windowGetActiveID(),
        0,
        0,
        0,
        1,
        name);

    fn_8007C300(
        sv[8],
        (u8)lbl_8047A448);

    if (menuResult != 2) {
        SYSVARS_RESTORE_PARTY_ITEMS();

        winMsgOpen(8, 0x3B70, 0, 0);

        choice = menuOpen(0xA9, 1);

        menuClose(0xA9);
        winMsgClose(1);

        if (choice == 0) {
            goto first_menu;
        }

        fn_8007C450(
            sv[8],
            (u8)lbl_8047A448,
            sv[0x24],
            sv[0x26],
            7);

        fn_80166A28(0x4C6);

        SYSVARS_WAIT_EFFECT();

        fn_8007CAB0();

        if (fn_80082738(
                lbl_8047A434,
                sv,
                (u8)lbl_8047A448) != 0) {

            fn_8007C414();
            fn_8007C634();

            fn_8007CAB0();
            fn_80082650(lbl_8047A434);
        }

        fn_8007C414();
        fn_8007C7EC();

        return 2;
    }

    /*
     * First menu accepted.
     */
    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        sv[0x26],
        2);

    SYSVARS_WAIT_EFFECT();

    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        sv[0x26],
        4);

    fn_80166A28(0x3C6);

    SYSVARS_WAIT_EFFECT();

    fn_80166AB8(0x3CC, 0, 0);

    heroAddPokecoupon(0, 0x32);

    winMsgOpen(8, 0x3B72, 1, 0);

    /*
     * Check whether the second stage is available.
     */
    valid = 1;

    if ((s8)sv[
            0x5E + (s8)lbl_8047A448] < 0) {

        valid = 0;
    } else {
        i = 0;

        while (i < (s8)sv[0x5A]) {
            ptr = fn_80082EA4(
                lbl_8047A434,
                (u8)lbl_8047A448,
                sv[0x24],
                (s8)i);

            if (ptr[0x0C] == 0) {
                valid = 0;
                break;
            }

            i++;
        }
    }

    if (valid == 0) {
        goto post_validate;
    }

    fn_80082BA4(
        lbl_8047A434,
        sv,
        (u8)lbl_8047A448);

    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        -1,
        1);

    fn_80166A28(0x4C5);

    SYSVARS_WAIT_EFFECT();

    winMsgOpen(8, 0x3B74, 1, 0);
    winMsgClose(1);

    /*
     * NOTE OFFSET 0x5E HERE, unlike the first menu's 0x5B.
     */
    menuResult = fn_80032ED8(
        sv + 0x3AC +
            (s8)sv[
                0x5E + (s8)lbl_8047A448] *
                0x28,
        9,
        0);

    menuOpenCustom(
        0xA6,
        windowGetActiveID(),
        0,
        0,
        0,
        1,
        name);

    fn_8007C300(
        sv[8],
        (u8)lbl_8047A448);

    /*
     * Second menu cancelled/rejected.
     */
    if (menuResult != 2) {
        fn_8007C450(
            sv[8],
            (u8)lbl_8047A448,
            sv[0x24],
            -1,
            7);

        fn_80166A28(0x4C6);

        SYSVARS_WAIT_EFFECT();

        fn_80082960(
            lbl_8047A434,
            sv,
            (u8)lbl_8047A448);

        fn_8007C450(
            sv[8],
            (u8)lbl_8047A448,
            sv[0x24],
            sv[0x26],
            8);

        fn_80166A28(0x4C6);

        SYSVARS_WAIT_EFFECT();

        fn_8007CAB0();

        if (fn_80082738(
                lbl_8047A434,
                sv,
                (u8)lbl_8047A448) != 0) {

            fn_8007C414();
            fn_8007C634();

            fn_8007CAB0();
            fn_80082650(lbl_8047A434);
        }

        fn_8007C414();
        fn_8007C7EC();

        winMsgOpen(8, 0x3B78, 1, 0);

        fn_8007C414();

        SYSVARS_RESTORE_PARTY_ITEMS();

        winMsgOpen(8, 0x3B7C, 0, 0);

        choice = menuOpen(0xA9, 1);

        menuClose(0xA9);
        winMsgClose(1);

        if (choice == 0) {
            goto restart;
        }

        return 2;
    }

    /*
     * Second menu accepted.
     */
    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        -1,
        2);

    SYSVARS_WAIT_EFFECT();

    fn_8007C450(
        sv[8],
        (u8)lbl_8047A448,
        sv[0x24],
        -1,
        4);

    fn_80166A28(0x3C6);

    SYSVARS_WAIT_EFFECT();

    fn_80166AB8(0x3CC, 0, 0);

    /*
     * This branch awards 80, versus the earlier branch's 50.
     */
    heroAddPokecoupon(0, 0x50);

    winMsgOpen(8, 0x3B76, 1, 0);

    item = *(u16*)(
        sv +
        0x64 +
        (s8)lbl_8047A448 * 2);

    valid = 1;

    if (item == 0) {
        valid = 0;
    } else if (
        heroItemAddItemDataId(
            0,
            item,
            1,
            -1) != 0) {

        if (pcboxDelItem(
                0,
                item,
                1) != 0) {

            valid = 0;
        }
    }

    if (valid != 0) {
        msgctrlSetValue(
            0x2D,
            item);

        winMsgOpen(
            8,
            0x4273,
            1,
            0);
    }

    fn_8007C414();

post_validate:

    valid = 0;

    if ((s8)sv[
            0x61 + (s8)lbl_8047A448] >= 0) {

        valid = 1;

        i = 0;

        while ((s8)i <
               (s8)((u8*)lbl_8047A434)[0x1C]) {

            j = 0;

            while (j < (s8)sv[0x5A]) {
                ptr = fn_80082EA4(
                    lbl_8047A434,
                    (u8)lbl_8047A448,
                    i,
                    (s8)j);

                if (ptr[0x0C] == 0) {
                    valid = 0;
                    goto validation_done;
                }

                j++;
            }

            i++;
        }
    }

validation_done:

    if (valid == 0) {
        fn_8007C414();

        SYSVARS_RESTORE_PARTY_ITEMS();

        winMsgOpen(8, 0x3B7C, 0, 0);

        choice = menuOpen(0xA9, 1);

        menuClose(0xA9);
        winMsgClose(1);

        if (choice == 0) {
            goto restart;
        }

        return 2;
    }

    fn_80165668(
        0x4CF,
        0,
        0xFF);

    fn_800832C8(
        0,
        sv,
        (u8)lbl_8047A448);

    msgctrlSetValue(
        0x4D,
        (u32)(sv + 0x0A));

    msgctrlSetValue(
        0x57,
        (u32)(
            sv +
            0x28 +
            (s8)lbl_8047A448 * 0x10));

    winMsgOpen(
        8,
        0x3B7A,
        1,
        0);

    winMsgClose(1);

    return 0;
}

/* Run the GBA transfer UI until it finishes or the user cancels it. */
s32 fn_80034280(void)
{
    s32 state;
    s32 nextState;
    s32 transferActive;
    u8 waitForWindow;
    s32 watchedWindow;

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
            /* fallthrough */
        case 19:
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

        case 21:
            if (transferActive != 0) {
                fn_8007AAA8();
            }
            fn_80166A28(0x3C7);
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
            fn_8007AAA8();
            fn_80080310(lbl_803A3334, lbl_803F7A30 + 0x2388,
                        0x1040);
            return 2;
        }
    }
}
#pragma pop

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

/*
 * Record the current opponent in the recent-opponent history. Nothing happens
 * if this trainer/region/variant triple is already listed; otherwise the ten
 * slots shift down by one and the new record is written into the last slot.
 *
 * The new record is filled through a byte cursor: after the header it walks
 * the four 0x2A-byte move slots, which is how the retail code addresses them
 * (base advancing by 0x2A, move payload at displacement 0x22).
 */
#pragma push
#pragma peephole off
void fn_80034830(u8 enabled, s32 selection,
                 SysvarsOpponentSource* source)
{
    extern u8 lbl_803A3334[];
    extern SysvarsSaveWork* lbl_8047A430;
    extern void GScharCpy(void*, const void*);
    u8* destination;
    s32 moveIndex;
    s32 i;
    u8 isNew;

    isNew = 1;
    for (i = 0; i < 10; i++) {
        if (lbl_8047A430->history[i].trainerType == lbl_803A3334[8] &&
            lbl_8047A430->history[i].region == (s8)lbl_803A3334[0x24] &&
            lbl_8047A430->history[i].variant == (s8)lbl_803A3334[0x26]) {
            isNew = 0;
            break;
        }
    }

    if (isNew) {
        for (i = 0; i < 9; i++) {
            lbl_8047A430->history[i] = lbl_8047A430->history[i + 1];
        }

        destination = (u8*)&lbl_8047A430->history[9];
        destination[0] = lbl_803A3334[8];
        destination[1] = lbl_803A3334[0x24];
        destination[2] = lbl_803A3334[0x26];
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
                    *(SysvarsMoveSnapshot*)(lbl_803A3334 + 0x514 +
                                            moveIndex * 0x2A);
            }
            destination += 0x2A;
        }
    }
}
#pragma pop

#pragma push
#pragma peephole off
void fn_80034B5C(u8* valid, s8* selectionOut, u8* valueOut)
{
    extern u8 lbl_803A3334[];
    extern void* lbl_8047A434;
    extern void* heroGetStatus(s32, s32, u16);
    extern u8 pokemonCheckValid(void*);
    extern u32 pokemonGetStatus(void*, s32, s32, s32);
    extern void* fn_80082FE4(void*, s8);
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
    region = lbl_803A3334[0x24];
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

    for (candidate = 0; candidate < (s8)lbl_803A3334[0x58]; candidate++) {
        score[candidate] = 0x7FFFFFFF;
        layer = fn_80082FE4(lbl_8047A434, candidate);
        if (layer[0x1C + (s8)region * 0xE] != 0) {
            group = lbl_803A3334 + (s8)lbl_803A3334[0x5E + candidate] * 0x28;
            maxRequired = 0;
            for (slot = 0; slot < 4; slot++) {
                index = (s8)group[0x3B9 + slot];
                if (index >= 0) {
                    required = lbl_803A3334[index * 0x2A + 0x517];
                    if (maxRequired < required) {
                        maxRequired = required;
                    }
                }
            }
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
        *valueOut = lbl_803A3334[0x5E + *selectionOut];
    } else {
        *selectionOut = (s8)(lbl_803A3334[0x58] - 1);
        *valueOut = lbl_803A3334[0x5B + *selectionOut];
    }
}
#pragma pop

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

/* fn_80034FB4 - 0x80034FB4 | size: 0xC94 */
#pragma push
#pragma peephole off
void fn_80034FB4(void)
{
    extern u8 lbl_803A3278[];
    extern u32 lbl_8047A430;
    extern void* lbl_8047A434;
    extern u8 lbl_8047A438;
    extern u8 lbl_8047A439;
    extern u32 lbl_8047A43C;
    extern u32 lbl_8047A444;
    extern u8 lbl_8047A449;
    extern u8 lbl_8047A44A;
    extern u32 lbl_8047A44C;
    extern u32 lbl_8047A450;
    extern u8 lbl_8047A454;
    extern s32 lbl_8047A458;
    extern const f32 lbl_8047B9FC;
    extern const f32 lbl_8047BA00;
    extern u8 lbl_8047BA1C;
    extern s32 _sysvarsProcessData__FP16sysvarsFuncEntryPc(void);
    extern void fn_8003258C(void);
    extern void fn_800327FC(void);
    extern void* fn_80082EA4(void*, s8, u8, u8);
    extern void* fn_800836AC(void*, void*, s32);
    extern u8 fn_800FF52C(void);
    extern void fn_800FF660(void);
    extern s32 menuOpen(s32, s32);
    extern void menuClose(s32);
    extern u8 menuIsCheck(s32);
    extern s32 menuOpenCustom(s32, ...);
    extern void* savedataGetStatus(s32, s32);
    extern void* heroBiosGetPokemonPtr(void*, u8);
    extern u16 pokemonBiosGetItemDataId(void*);
    extern void pokemonBiosSetItemDataId(void*, u16);
    extern void fn_801D0AFC(s32);
    extern void fn_80165668(u32, s32, u32);
    extern void fadeSet(f32, u32);
    extern void fadeCheck(u32);
    extern void floorLink(u32, s32);
    extern void GScharCpy(void*, const void*);
    extern void* fightFloorDataBiosGetPtr(u16);
    extern u32 fightFloorDataBiosGetName(void*);
    extern void* GSmsgGetGSchar(u32);
    extern void msgctrlSetValue(s32, u32);
    u8* work;
    u8* menuData;
    u8* name;
    u8* floorName;
    u8* clear;
    u8* history;
    u16* heldItems;
    void* save;
    void* pokemon;
    void* entry;
    s32 result;
    s32 index;
    u32 amount;
    u32 historyIndex;
    u32 historyCount;
    u8 value;
    s8 selection;
    u8 valid;
    u8 duplicate;
    f32 progress;

    work = lbl_803A3278;
    lbl_8047A438 = 1;
    menuData = work + 0xBC;
    name = work + 0x10;
    floorName = menuData + 0xB00;

    while (lbl_8047A458 > 0) {
        switch (lbl_8047A458) {
        case 1:
            if (lbl_8047A454 == 1) {
                fadeSet(lbl_8047BA00, 2);
            }
            result = menuOpen(0xA4, 1);
            lbl_8047A454 = 1;
            fadeCheck(1);
            switch (result) {
            case -1:
                lbl_8047A458 = 2;
                break;
            case 1:
                lbl_8047A458 = 3;
                break;
            case 0:
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
                fn_80166A28(0x3C7);
                winMsgOpen(8, 0x3B54, 1, 0);
                winMsgClose(1);
            } else {
                fadeSet(lbl_8047BA00, 3);
                fadeCheck(1);
                menuClose(0xA4);
            }
            break;

        case 2:
            fadeSet(lbl_8047BA00, 2);
            menuOpenCustom(0xA6, windowGetActiveID(), 0, 0, 0, 1, name);
            fadeCheck(1);
            save = savedataGetStatus(0, 2);
            heldItems = (u16*)(work + 0xB0);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                heldItems[index] = pokemonBiosGetItemDataId(pokemon);
            }
            while (lbl_8047A458 == 2) {
                lbl_8047A458 = _sysvarsProcessData__FP16sysvarsFuncEntryPc();
            }
            fn_801D0AFC(0);
            save = savedataGetStatus(0, 2);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                pokemonBiosSetItemDataId(pokemon, heldItems[index]);
            }
            if (lbl_8047A439 != 0) {
                lbl_8047A458 = 0;
            }
            if (lbl_8047A458 != 0) {
                fn_80166A28(0x3C7);
                fadeSet(lbl_8047BA00, 3);
                fadeCheck(1);
                menuClose(0xA6);
            }
            break;

        case 3:
            save = savedataGetStatus(0, 2);
            heldItems = (u16*)(work + 0xB0);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                heldItems[index] = pokemonBiosGetItemDataId(pokemon);
            }
            lbl_8047A450 = 0;
            lbl_8047A44A = 0;
            lbl_8047A43C = 0;
            clear = work;
            work[0] = 0;
            clear[1] = 0;
            clear[2] = 0;
            clear[3] = 0;
            clear[4] = 0;
            clear[5] = 0;
            clear[6] = 0;
            clear[7] = 0;
            clear[8] = 0;
            clear[9] = 0;
            clear[10] = 0;
            clear[11] = 0;
            clear[12] = 0;
            clear[13] = 0;
            clear[14] = 0;
            if (menuIsCheck(0xA5) != 0) {
                menuClose(0xA5);
            }
            fadeSet(lbl_8047BA00, 2);
            menuOpen(0xA5, 1);
            fadeCheck(1);
            lbl_8047A458 = 5;
            break;

        case 4:
            if (lbl_8047A449 != 0) {
                fn_801D0AFC(0);
            }
            if (lbl_8047A450 > lbl_8047A44C) {
                fn_80165668(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B8D, 1, 0);
                winMsgClose(1);
                progress = lbl_8047B9F8;
                while (progress < lbl_8047B9FC) {
                    _threadSwitch();
                    progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
            }
            winMsgOpen(8, 0x3B54, 1, 1);
            winMsgClose(1);
            clear = work;
            lbl_8047A43C = 0;
            work[0] = 0;
            clear[1] = 0;
            clear[2] = 0;
            clear[3] = 0;
            clear[4] = 0;
            clear[5] = 0;
            clear[6] = 0;
            clear[7] = 0;
            clear[8] = 0;
            clear[9] = 0;
            clear[10] = 0;
            clear[11] = 0;
            clear[12] = 0;
            clear[13] = 0;
            clear[14] = 0;
            save = savedataGetStatus(0, 2);
            heldItems = (u16*)(work + 0xB0);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                pokemonBiosSetItemDataId(pokemon, heldItems[index]);
            }
            lbl_8047A458 = 0;
            break;

        case 5:
            result = fn_80034280();
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
            switch (*(s32*)menuData) {
            case 1:
                lbl_8047A458 = 7;
                continue;
            case 0:
                break;
            default:
                continue;
            }
            save = savedataGetStatus(0, 0xD);
            lbl_8047A434 = fn_800836AC(save, menuData, 0);
            if (lbl_8047A434 != 0) {
                entry = fn_80082EA4(lbl_8047A434, (s8)(menuData[0x58] - 1),
                                    menuData[0x24], menuData[0x26]);
                result = 4;
                if (*(u8*)((u8*)entry + 0xC) != 0) {
                    duplicate = 0;
                    historyCount = lbl_8047A43C;
                    history = work;
                    if ((s32)historyCount > 0) {
                        do {
                            if (history[0] == menuData[8] &&
                                (s8)history[1] == (s8)menuData[0x24] &&
                                (s8)history[2] == (s8)menuData[0x26]) {
                                duplicate = 1;
                                break;
                            }
                            history += 3;
                        } while (--historyCount != 0);
                    }
                    if (duplicate == 0) {
                        historyIndex = lbl_8047A43C;
                        history = work + historyIndex * 3;
                        history[0] = menuData[8];
                        history[1] = menuData[0x24];
                        history[2] = menuData[0x26];
                        lbl_8047A43C = historyIndex + 1;
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
            case -1:
                lbl_8047A458 = 5;
                continue;
            case 1:
                lbl_8047A458 = 5;
                continue;
            case 0:
            case 2:
            default:
                break;
            }
            if (lbl_8047A450 > lbl_8047A44C) {
                fn_80165668(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B8D, 1, 0);
                winMsgClose(1);
                progress = lbl_8047B9F8;
                while (progress < lbl_8047B9FC) {
                    _threadSwitch();
                    progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
            }
            if (lbl_8047A449 != 0) {
                fn_801D0AFC(0);
            }
            save = savedataGetStatus(0, 2);
            heldItems = (u16*)(work + 0xB0);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                pokemonBiosSetItemDataId(pokemon, heldItems[index]);
            }
            result = 1;
            lbl_8047A458 = result;
            if (result != 0) {
                fadeSet(lbl_8047BA00, 3);
                fadeCheck(1);
                menuClose(0xA5);
            }
            break;

        case 7:
            lbl_8047A444 = *(u32*)(menuData + 0xAFC);
            if (*(u16*)floorName != 0) {
                GScharCpy(name, floorName);
            } else {
                entry = fightFloorDataBiosGetPtr((u16)lbl_8047A444);
                if (entry != 0) {
                    GScharCpy(name, GSmsgGetGSchar(fightFloorDataBiosGetName(entry)));
                } else {
                    GScharCpy(name, &lbl_8047BA1C);
                }
            }
            msgctrlSetValue(0x4D, (u32)name);
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
            fn_80034B5C(&valid, (void*)&selection, &value);
            fn_80034830(valid, selection,
                        (SysvarsOpponentSource*)(menuData + 0x3AC +
                                                 (s8)value * 0x28));
            amount = lbl_8047A450 + 1;
            if (amount > 0x98967F) {
                amount = 0x98967F;
            }
            msgctrlSetValue(0x2F, amount);
            winMsgOpen(8, 0x3B5E, 1, 0);
            winMsgClose(1);
            if (lbl_8047A450 == 0) {
                fn_801D0AFC(0);
            }
            lbl_8047A458 = 11;
            break;

        case 11:
            fn_800327FC();
            break;

        case 12:
            lbl_8047A44A = 0;
            if (lbl_8047A450 > lbl_8047A44C) {
                fn_80165668(0x4CE, 0, 0xFF);
                amount = lbl_8047A450;
                amount = amount <= 0x98967F ? amount : 0x98967F;
                lbl_8047A44C = amount;
                *(u32*)((u8*)lbl_8047A430 + 0x49C8) = amount;
                winMsgOpen(8, 0x3B75, 1, 0);
                winMsgClose(1);
            }
            clear = work;
            lbl_8047A43C = 0;
            work[0] = 0;
            clear[1] = 0;
            clear[2] = 0;
            clear[3] = 0;
            clear[4] = 0;
            clear[5] = 0;
            clear[6] = 0;
            clear[7] = 0;
            clear[8] = 0;
            clear[9] = 0;
            clear[10] = 0;
            clear[11] = 0;
            clear[12] = 0;
            clear[13] = 0;
            clear[14] = 0;
            fn_801D0AFC(0);
            winMsgOpen(8, 0x3BD7, 1, 0);
            result = menuOpen(0xA9, 1);
            menuClose(0xA9);
            winMsgClose(1);
            switch (result) {
            case 0:
                lbl_8047A458 = 3;
                continue;
            case -1:
                lbl_8047A458 = 1;
                break;
            case 1:
                lbl_8047A458 = 1;
                break;
            case 2:
            default:
                break;
            }
            progress = lbl_8047B9F8;
            while (progress < lbl_8047B9FC) {
                _threadSwitch();
                progress += (f32)fn_800D3088() / (f32)fn_800D37CC();
            }
            save = savedataGetStatus(0, 2);
            heldItems = (u16*)(work + 0xB0);
            for (index = 0; index < 6; index++) {
                pokemon = heroBiosGetPokemonPtr(save, (u8)index);
                pokemonBiosSetItemDataId(pokemon, heldItems[index]);
            }
            if (lbl_8047A458 != 0) {
                fadeSet(lbl_8047BA00, 3);
                fadeCheck(1);
                menuClose(0xA5);
            }
            break;

        case 13:
            fn_8003258C();
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

    if (fn_800FF52C() != 0) {
        fn_800FF660();
    } else {
        floorLink(0x80, 0);
    }
    lbl_8047A438 = 0;
}
#pragma pop

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
#pragma peephole off
void fn_80035DA0(void) {
    extern void floorLink(u32, s32);
    extern void _threadSwitch(void);
    extern u8 lbl_8047A439;

    lbl_8047A439 = 0;
    floorLink(0x393, 0);
    _threadSwitch();
}
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
