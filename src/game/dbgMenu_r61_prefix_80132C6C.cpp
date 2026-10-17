/**
 * dbgMenu prefix, .text 0x80132C6C-0x80133050: fn_80132C6C (the debug-menu
 * slot/item pools), fn_80132F7C (menu 0xAB toggle) and dbgMenuMovieTest, in
 * address order.  Linked (user decision, 2026-09-28; see the note below).
 *
 * Built as C++ (the .cpp extension; no other flag and no compiler-control
 * pragma).  Evidence that this code is C++: the XD JP demo linker map
 * (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af) lists
 * dbgMenuSub.o, which holds dbgMenuMovieTest, with the mangled static
 * `_dbgMenuMovieTestGetStr__FUl`; Colosseum's own dbgMenu code calls the
 * mangled statics `_dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl` and
 * `_toolentryAlloc__FUl`, and the preceding msgctrl unit has
 * `_msgctrlMakeDigit__FPUslUll`.  The public entry points keep C linkage
 * (dbgMenuMovieTest is unmangled in the XD map).
 *
 * All three functions are exact with GC/1.3 and the unit's normal flags,
 * with and without -schedule on (lane D15, 2026-09-28).
 *
 * fn_80132C6C -- admissibility note.
 * Retail colours the first unrolled copy's item pointer before the
 * unroller's byte offset and before the counter (r5, r6, r7).  A declared
 * pointer local (any scope, C or C++, any form; see dbgMenu.c) is coloured
 * after the unroller's temporaries (r29), 94 instructions off.  The
 * pointer is exact only when the front end holds it in a compiler
 * temporary created before the unroller runs.  Besides an inlined call's
 * argument (a single-use helper, rejected), the one C++ form that does
 * this is a reference to const bound to the pointer rvalue:
 *
 *     DbgMenuItem* const& item = &items[i];
 *
 * GC/2.6 IRO trace (lane B7b's irolog): the front end emits
 * `@22 = &items[i]; item = &@22`, copy propagation folds `*item` to @22,
 * and @22 is coloured first, exactly like the helper's argument temp @25.
 * The plain `DbgMenuItem* item`, `DbgMenuItem* const item` and
 * `DbgMenuItem& item = items[i]` all keep a user variable (94 off).  The
 * reference changes nothing but register colouring, so it is a temporary
 * whose only effect is allocation: a judgement call under the strict
 * acceptance policy.  The user admitted it on 2026-09-28 ("C++ reference
 * bindings" in docs/CAMPAIGN_OPERATIONS.md): in a TU evidenced as C++, a
 * reference to const bound to a computed value is natural C++ source.
 *
 * Linking needs "fn_80132F7C" and "dbgMenuMovieTest" in configure.py's
 * force_active_symbols: they are menu callbacks referenced only from
 * unlinked data, so without them the linker strips both and .text shrinks
 * by 0xC0.
 */

#include "dolphin/types.h"

extern "C" {
#include "game/effect/effect_util_types.h"
}

typedef struct DbgMenuSlot {
    u32 field_00;
    u32 field_04;
    u32 field_08;
    u32 field_0C;
    u32 field_10;
    u8 field_14;
    u8 field_15;
    u8 pad_16[2];
} DbgMenuSlot;

typedef struct DbgMenuItem {
    s32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u32 field_0C;
    s32 field_10;
    u8 field_14;
    u8 pad_15[3];
    u32 field_18;
    u32 field_1C;
} DbgMenuItem;

extern "C" {

extern u32 lbl_8047AEB0;
extern u32 lbl_8047AEB4;
extern u16 lbl_8047AEB8;
extern u32 lbl_8047AEBC;
extern u32 lbl_8047AEC0;
extern u16 lbl_8047AEC4;
extern u32 lbl_8047AEC8;
extern u32 lbl_8047AECC;

/* 0x80132C6C | 0x310 */
void fn_80132C6C(u32 count, u32 maxPerSlot, u32 arg2, u32 arg3)
{
    u32 total;
    u32 i;
    DbgMenuSlot* slot;

    if (count == 0 || maxPerSlot == 0) {
        return;
    }

    lbl_8047AEB4 = count;
    lbl_8047AEC0 = maxPerSlot;
    lbl_8047AECC = arg3;
    lbl_8047AEC8 = arg2;

    lbl_8047AEB8 = (u16)_toolentryAlloc__FUl(count * sizeof(DbgMenuSlot));
    if (lbl_8047AEB8 == 0) {
        return;
    }
    lbl_8047AEB0 = GSmemLock(lbl_8047AEB8);

    for (i = 0; i < lbl_8047AEB4; i++) {
        slot = &((DbgMenuSlot*)lbl_8047AEB0)[i];
        slot->field_00 = 0;
        slot->field_04 = 0;
        slot->field_08 = 0;
        slot->field_0C = 0;
        slot->field_10 = 0;
        slot->field_14 = 0;
        slot->field_15 = 0;
    }

    total = lbl_8047AEB4 * lbl_8047AEC0;
    lbl_8047AEC4 = (u16)_toolentryAlloc__FUl(total * sizeof(DbgMenuItem));
    if (lbl_8047AEC4 == 0) {
        return;
    }
    lbl_8047AEBC = GSmemLock(lbl_8047AEC4);

    for (i = 0; i < total; i++) {
        DbgMenuItem* const& item = &((DbgMenuItem*)lbl_8047AEBC)[i];
        item->field_00 = -1;
        item->field_04 = 0;
        item->field_06 = 0;
        item->field_08 = 0;
        item->field_0C = 0;
        item->field_10 = -1;
        item->field_14 = 0;
        item->field_18 = 0;
        item->field_1C = 0;
    }
}

/* 0x80132F7C | 0x5C */
u32 fn_80132F7C(void)
{
    if (menuIsCheck(0xab) & 0xFF) {
        menuClose(0xab);
    } else {
        menuOpenCustom(0xab, 0, 0, 0, 0, 0);
    }
    return 0;
}

/* 0x80132FD8 | 0x78 */
u32 dbgMenuMovieTest(void)
{
    u8 buf[0x18];
    s32 id;

    while ((u32)(GSmovieIsActive() & 0xFF) == 1) {
        GSmovieClose();
        _threadSwitch();
    }
    id = menuOpen(2, 1);
    if (id != -1) {
        sprintf(buf, lbl_80272AA8, lbl_8047D0E8, id);
        GSmovieOpen(buf, 0);
    }
    return 0;
}

}
