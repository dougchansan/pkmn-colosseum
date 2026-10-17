/**
 * @file dbgMenu.c
 * @brief Decompiled functions.
 *
 * Address range: 0x80132C6C - 0x8013433C
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

typedef struct DbgMenuWindow {
    u32 field_00;
    u32 key;
    u8 pad_08[0x8C];
    union {
        u16 cursorPosition;
        struct {
            s8 page;
            s8 row;
        } cursor;
    };
} DbgMenuWindow;

#if !defined(DBGMENU_TAIL_80133E6C_ONLY)


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

static inline void dbgMenuInitItems(u32* itemTable, u32 total)
{
    u32 i;
    for (i = 0; i < total; i++) {
        DbgMenuItem* item = &((DbgMenuItem*)(*itemTable))[i];
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

/* 0x80132C6C | 0x310
 *
 * Remaining difference (register choice only): in the 8x-unrolled item
 * reset, retail colours the first copy's item pointer first (r5), then the
 * strength-reduced byte offset (r6), then the counter (r7).  Any plain C
 * form (item as a top-level or block-scope local, reference, param reuse,
 * any declaration order, index/pointer/byte-offset forms, while/for, counter
 * i/count/param, compilers 1.3-2.7, C or C++) emits the same instructions
 * with the pointer coloured LAST (r29): user variables are coloured after
 * the unroller's temps, whatever their scope.  The loop body has no call, so
 * the r29 is not a live-across-call choice.  Only an inlined function's
 * parameter/local is coloured before those temps; a per-item
 * `static inline void reset(DbgMenuItem* item)` called in the loop is
 * exact, but that is a single-use helper with register-choice-only
 * evidence (no extra instruction; the DOL and common_rel have no second
 * expansion or standalone copy of the nine stores), so it is rejected
 * under the helper policy.  dbgMenuInitItems above is the same kind of
 * helper and keeps this unit a CodeCandidate.
 *
 * Lane B15 (2026-09-28), register replay with mwcc-debugger (GC/2.6, same
 * code as GC/1.3 here).  The GPR colouring runs from the highest virtual
 * register down.  A declared local gets a low number: with the in-place
 * form, `item` is r36, below every unroller temp (r40-r55), so it is
 * coloured last (r29).  With the per-item helper, the substituted argument
 * `&table[i]` becomes the front end's temp @25 (r55), which is coloured
 * first (r5).  So retail's item pointer is a front-end temp, not a user
 * variable.  In C, only an inlined call's argument/parameter creates such a
 * temp for a plain pointer.  Also tried without success: `register` on the
 * item, one pointer variable shared by the slot and item loops (52 lines
 * off, the same as in place), a separate item counter (74), and GC/1.0-1.2.5n
 * (3 extra instructions) and 3.0a (178 instructions).
 * XD check: XD's main init (InitAllThreads, trevor403/xd-asm b1087f18) goes
 * GSlightInit, FUN_8028a9b8(0x20), FUN_8012d8e8(0x20), PopulatePads.  It has
 * no (0x10, 0x40, 1, 0x2000) pool call.  XD `_toolentryAlloc__FUl`
 * (0x80231AB4, scope:local, TeamOrre/xd-decomp 4989794e) is a GSalloc
 * wrapper called only from FUN_80232268 and FUN_802325a4.  So XD has no
 * counterpart for the sister-title clause.  The XD JP demo's linker map
 * (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af) lists every
 * function, stripped ones included, in dbgMenuSub.o (dbgMenuMovieTest, the
 * GSmem/frame-rate items) and dbgMenu.o (dbgMenuInit is UNUSED there, size
 * 0xD4).  Neither object lists a pool init with the (count, max, a, b)
 * shape or a per-item reset helper, so the map has no helper name either.
 * Linking lead: with every `#pragma` removed, fn_80132F7C and
 * dbgMenuMovieTest compile exactly under the unit's plain flags (with and
 * without -schedule on).  Once fn_80132C6C is exact, this prefix unit needs
 * no compiler-control pragma.
 * Lane D15 (2026-09-28), IRO trace (GC/2.6, lane B7b's irolog): with the
 * per-item helper the argument is the front end's temp @25 before any IRO
 * phase runs; the unroller then copies the body with `item`/@25 in every
 * copy, and just before RebuildCondExpressions copies 2-9 get fresh temps
 * while copy 1 keeps the original, so the original's kind (user variable
 * or front-end temp) decides the colouring.  No IRO phase before the
 * unroller turns a user variable into a temp.  Tried without success
 * (94 instructions off, identical code): u8* and u32 byte-address forms
 * with casts at each store, pointer-plus-index, a `const` pointer (C and
 * C++), a C++ `DbgMenuItem&` bound to the element, and the assignment used
 * as the first store's lvalue; the no-variable form reloads the table for
 * every store.  The only non-helper form that is exact is C++
 * `DbgMenuItem* const& item = &items[i];` (a reference to const bound to
 * the pointer rvalue makes the front end create the temp).  It changes
 * nothing but register colouring, so it is a judgement call and is not
 * linked; it lives in the C++ prefix candidate
 * dbgMenu_r61_prefix_80132C6C.cpp, where the whole range (with
 * fn_80132F7C and dbgMenuMovieTest, no pragmas) is exact.  The item table
 * (lbl_8047AEBC) has no other reference in the DOL, and
 * tools/find_inline_expansions.py finds the nine-store block only in this
 * function's own unrolled copies, so repeated expansion cannot admit the
 * helper either.
 */
#if 0
asm void fn_80132C6C(void) {
#include "src/game/effect/effect_util_fn_80132C6C.inc"
}
#else
void fn_80132C6C(u32 count, u32 maxPerSlot, u32 arg2, u32 arg3) {
    extern u32 lbl_8047AEB0;
    extern u32 lbl_8047AEB4;
    extern u16 lbl_8047AEB8;
    extern u32 lbl_8047AEBC;
    extern u32 lbl_8047AEC0;
    extern u16 lbl_8047AEC4;
    extern u32 lbl_8047AEC8;
    extern u32 lbl_8047AECC;
    u32 total;
    u32 i;
    DbgMenuSlot* slot;

    if (count == 0 || maxPerSlot == 0) return;

    lbl_8047AEB4 = count;
    lbl_8047AEC0 = maxPerSlot;
    lbl_8047AECC = arg3;
    lbl_8047AEC8 = arg2;

    lbl_8047AEB8 = (u16)_toolentryAlloc__FUl(count * 0x18);
    if (lbl_8047AEB8 == 0) return;
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
    lbl_8047AEC4 = (u16)_toolentryAlloc__FUl(total << 5);
    if (lbl_8047AEC4 == 0) return;
    lbl_8047AEBC = GSmemLock(lbl_8047AEC4);

    dbgMenuInitItems(&lbl_8047AEBC, total);
}
#endif

#if 0
asm void fn_80132F7C(void) {
#include "src/game/effect/effect_util_fn_80132F7C.inc"
}
#else
u32 fn_80132F7C(void) {
    if (menuIsCheck(0xab) & 0xFF) {
        menuClose(0xab);
    } else {
        menuOpenCustom(0xab, 0, 0, 0, 0, 0);
    }
    return 0;
}
#endif

#if 0
asm void dbgMenuMovieTest(void) {
#include "src/game/effect/effect_util_fn_80132FD8.inc"
}
#else
u32 dbgMenuMovieTest(void) {
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
#endif


/* 0x80133050 | 0x3C -- fn_800D37D4(3, 2, 0, 2, 0, 0), return 0 */
u32 fn_80133050(void) {
    fn_800D37D4(3, 2, 0, 2, 0, 0);
    return 0;
}


/* 0x8013308C | 0x3C -- fn_800D37D4(2, 2, 0, 2, 0, 0), return 0 */
u32 fn_8013308C(void) {
    fn_800D37D4(2, 2, 0, 2, 0, 0);
    return 0;
}


/* 0x801330C8 | 0x150 */
#if 0
asm void fn_801330C8(void) {
#include "src/game/effect/effect_util_fn_801330C8.inc"
}
#else
u32 fn_801330C8(void) {
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetOrtho(lbl_8047D0F0, lbl_8047D0F0, lbl_8047D0F4, lbl_8047D0F8);
    gfxSetBlendMode(0, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetCullMode(0);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(0);
    gfxBegin(3);
    gfxVtxPos(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0xFF, 0x80, 0x40, 0);
    gfxVtxPos(*(f32*)lbl_80478AC4, lbl_8047D100, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0x40, 0xFF, 0, 0);
    gfxVtxPos(lbl_8047D104, *(f32*)lbl_80478AC4, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0, 0x40, 0xFF, 0);
    gfxVtxPos(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0xFF, 0x80, 0x40, 0);
    gfxEnd();
    return 0;
}
#endif


/* 0x80133218 | 0x38 -- GSmemOptimize() then print result, return 0 */
/* 0x80133218 | 0x38 -- GSmemOptimize() then print result, return 0 */
u32 dbgMenuGSmemOptimize(void) {
    u32 val = GSmemOptimize();
    GSlogWrite(lbl_80272AB8, val);
    return 0;
}


/* 0x80133250 | 0x2C -- GSmemDumpMap(1, 1), return 0 */
u32 dbgMenuGSmemDispMap(void) {
    GSmemDumpMap(1, 1);
    return 0;
}

#if 0
asm void dbgMenuGSmemCheck(void) {
#include "src/game/effect/effect_util_fn_8013327C.inc"
}
#else
u32 dbgMenuGSmemCheck(void) {
    if ((u32)(GSmemDumpMap(1, 0) & 0xFF) == 1) {
        GSlogWrite(lbl_80272AE0);
    } else {
        GSlogWrite(lbl_80272AF0);
    }
    return 0;
}
#endif


/* 0x5C | dbgMenuGSmemCheck -- GSmemDumpMap(1,0) check then print */
/* 0x801332D8 | 0x28 -- fn_800D3074(2), return 0 */
u32 dbgMenuFrameRate20(void) {
    fn_800D3074(2);
    return 0;
}


/* 0x80133300 | 0x28 -- fn_800D3074(1), return 0 */
u32 dbgMenuFrameRate30(void) {
    fn_800D3074(1);
    return 0;
}


/* 0x80133328 | 36 bytes | call_return_const2 */
/* 0x80133328 | 36 bytes | call_return_const2 */
u32 dbgMenuSendAllMail(void) {
    mailMainSendAllMail();
    return 0;
}

#if 0
asm void dbgMenuSendMail(void) {
#include "src/game/effect/effect_util_dbgMenuSendMail.inc"
}
#else
u32 dbgMenuSendMail(void) {
    extern s32 menuOpen(u32, u32);
    extern s32 mailChkReceiveMail(s32);
    extern void mailAddMailbox(s32);
    s32 slot;
    while ((slot = menuOpen(2, 1)) != -1) {
        if (mailChkReceiveMail(slot) == 0) {
            mailAddMailbox(slot);
            mailMainReceiveStart();
        }
    }
    return 0;
}
#endif


/* 0x801333AC | 0xA4 */
#if 0
asm void dbgMenuColisionDisp(void) {
#include "src/game/effect/effect_util_dbgMenuColisionDisp.inc"
}
#else
u32 dbgMenuColisionDisp(s32 arg) {
    extern void* GSresGetResource(u32, u32);
    u8* ptr;
    ptr = (u8*)GSresGetResource(0, 2);
    if (ptr == 0) return 0;
    switch (arg) {
    case 0xb8:
        if (ptr[0] != 0) {
            ptr[0] = 0;
        } else {
            ptr[0] = 1;
        }
        break;
    case 0xb9:
        if (ptr[1] != 0) {
            ptr[1] = 0;
        } else {
            ptr[1] = 1;
        }
        break;
    }
    return 0;
}
#endif


/* 0x58 | debugMenuColorBarDisp | call_sequence */
#if 0
asm void debugMenuColorBarDisp(void) {
#include "src/game/effect/effect_util_fn_80133450.inc"
}
#else
u32 debugMenuColorBarDisp(void) {
    *(u8*)&lbl_80478820 = 0;
    menuOpenCustom(5, 0, 0, 0, 1, 0);
    menuClose(5);
    *(u8*)&lbl_80478820 = 1;
    return 0;
}
#endif


/* 0x801334A8 | 0x34 -- toggle lbl_8047AED9 (cntlzw/extrwi), call GSmodelSetShadowDebug */
/* 0x801334A8 | 0x34 -- toggle lbl_8047AED9 (cntlzw/extrwi), call GSmodelSetShadowDebug */
u32 debugMenuShadowBorderDisp(void) {
    u32 clz = __cntlzw(lbl_8047AED9);
    u8 val = (u8)((clz >> 5) & 0xFF);
    lbl_8047AED9 = val;
    GSmodelSetShadowDebug(val);
    return 0;
}


/* 0x801334DC | 0x34 -- toggle lbl_8047AED8 (cntlzw/extrwi), call fn_800D4610 */
u32 fn_801334DC(void) {
    u32 clz = __cntlzw(lbl_8047AED8);
    u8 val = (u8)((clz >> 5) & 0xFF);
    lbl_8047AED8 = val;
    fn_800D4610(val);
    return 0;
}

#if 0
asm void debugMenuNodrawArea(void) {
#include "src/game/effect/effect_util_fn_80133510.inc"
}
#else
u32 debugMenuNodrawArea(void) {
    if (menuIsCheck(0x7) & 0xFF) {
        menuClose(0x7);
    } else {
        menuOpenCustom(0x7, 0, 0, 0, 0, 0);
    }
    return 0;
}
#endif

#if 0
asm void fn_8013356C(void) {
#include "src/game/effect/effect_util_fn_8013356C.inc"
}
#else
u32 fn_8013356C(u32 arg1, s32 arg2) {
    extern u32 heroMoveIsMember(s32);
    extern void heroMoveDismissMember(s32);
    extern void fn_8012F1FC(s32);
    s32 r31 = (arg2 != 0) ? -1 : 1;
    if (r31 >= 0) {
        if (heroMoveIsMember(r31) & 0xFF) {
            heroMoveDismissMember(r31);
        } else {
            fn_8012F1FC(r31);
        }
    }
    return 0;
}
#endif

#if 0
asm void debugMenuChangeMemInfo(void) {
#include "src/game/effect/effect_util_fn_801335D4.inc"
}
#else
u32 debugMenuChangeMemInfo(void) {
    if (menuIsCheck(0x4) & 0xFF) {
        menuClose(0x4);
    } else {
        menuOpenCustom(0x4, 0, 0, 0, 0, 0);
    }
    return 0;
}
#endif


/* 0x80133630 | 0x34 -- toggle lbl_8047AED4 (cntlzw>>5), call fn_80101B88 */
u32 fn_80133630(void) {
    u32 clz = __cntlzw(lbl_8047AED4);
    lbl_8047AED4 = clz >> 5;
    fn_80101B88(clz >> 5);
    return 0;
}


/* 0x80133664 | 0x13C */
#if 0
asm void dbgMenuCursor(void) {
#include "src/game/effect/effect_util_dbgMenuCursor.inc"
}
#else
typedef struct DbgMenuKeyInfo {
    u8 pad_00[6];
    u16 flags;
} DbgMenuKeyInfo;


void dbgMenuCursor(DbgMenuWindow* obj) {
    s8 pair[2];
    u16 inputFlags;
    s8 entryCount;
    s8 maxCount;
    u32 flags;

    flags = ((DbgMenuKeyInfo*)windowGetKeyInfo())->flags;
    entryCount = _dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl((u32)obj, NULL);
    maxCount = menuDataBiosGetType(obj->key);
    inputFlags = flags;
    if (entryCount < maxCount) {
        maxCount = entryCount;
    }

    *(u16*)pair = obj->cursorPosition;
    if (inputFlags & 1) {
        pair[1]--;
    } else if (inputFlags & 2) {
        pair[1]++;
    }

    if (pair[1] < 0) {
        pair[0] += pair[1];
        pair[1] = 0;
        if (pair[0] < 0) {
            pair[1] = maxCount - 1;
            pair[0] = entryCount - maxCount;
        }
    } else if (pair[1] >= maxCount) {
        pair[0] += pair[1] - (maxCount - 1);
        pair[1] = maxCount - 1;
        if (pair[0] + pair[1] >= entryCount) {
            pair[0] = 0;
            pair[1] = 0;
        }
    }

    obj->cursorPosition = *(u16*)pair;
}
#endif


/* 0x801337A0 | 0x8 | sda_getter */
u8 dbgMenuGetEnable(void) { return lbl_8047AED0; }


/* 0x801337A8 | 0x8 | sda_setter */
void dbgMenuSetEnable(u8 val) { lbl_8047AED0 = val; }


/* 0x801337B0 | 0x34 -- check menuIsCheck(lbl_80478848) != 0, return 0 or 1 */
u32 dbgMenuIsOpen(void) {
    u8 result = menuIsCheck(lbl_80478848);
    return (result != 0) ? 1 : 0;
}


/* 0x80133810 - 0x80133BE4: dbgMenuMain, _dbgMenuSub__Fl and
 * _dbgMenuGetMenuNum live in dbgMenu_candidate_801337E4.c. */

#if 0
asm u8 _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl(void) {
#include "src/game/effect/effect_util__dbgMenuCheckTerminate__FP14tagWINDOW_WORKl.inc"
}
#else
u32 _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl(void* obj, s32 offset) {
    EffectUtilEntry* result;
    u32 ret;
    s32 index = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, offset);
    {
        EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
        if (fp == NULL) {
            result = NULL;
        } else {
            result = fp(index);
        }
    }
    if (result == NULL) {
        ret = 1;
    } else {
        ret = (result->flags >> 7) & 1;
    }
    return (u8)ret;
}
#endif


/* 0x80133C3C - 0x80133E1C: _dbgMenuGetMenuNo__Fl lives in
 * dbgMenu_candidate_80133C3C.c. */

#if 0
asm void _dbgMenuGetMsgID__FP14tagWINDOW_WORKl(void) {
#include "src/game/effect/effect_util_fn_80133E1C.inc"
}
#else
u32 _dbgMenuGetMsgID__FP14tagWINDOW_WORKl(void* obj, s32 offset) {
    EffectUtilEntry* result;
    s32 index = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, offset);
    {
        EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
        if (fp == NULL) {
            result = NULL;
        } else {
            result = fp(index);
        }
    }
    if (result == NULL) {
        return 0;
    }
    return result->value;
}
#endif


#endif /* !DBGMENU_TAIL_80133E6C_ONLY */

/*
 * 0x80133E6C - 0x8013433C.  The helpers below are defined ahead of
 * _dbgMenuGetItemNo so that -inline auto expands them the way retail does:
 * the item-number lookup inlines itself twice, with the index, window,
 * root-menu and link helpers expanded in the outer two levels and called
 * at the third.  dbgMenu_candidate_80133E6C.c builds only this run
 * (DBGMENU_TAIL_80133E6C_ONLY) with -inline auto,deferred, which emits the
 * definitions in reverse, i.e. in retail's address order, and links it. */

/* 0x80134304 | 0x38 */
u32 debugMenuGetNum__Fv(void) {
    u32 (*fp)(void) = (u32 (*)(void))lbl_80478F88;
    if (fp == NULL) {
        return 0;
    }
    return fp();
}

/* 0x801342B8 | 0x4C */
s32 dbgMenuGetLink__Fl(s32 idx) {
    EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
    EffectUtilEntry* result;
    if (fp == NULL) {
        result = NULL;
    } else {
        result = fp(idx);
    }
    if (result == NULL) {
        return 0;
    }
    return result->link;
}

/* 0x80134274 | 0x44 */
u32 dbgMenuGetRootMenu(void) {
    int new_var;
    extern u32 GSgetVersionMinor(void);
    s32 val = (s32)GSgetVersionMinor();
    if (val == 1) goto _ret2;
    if (val >= 1) goto _chk3;
    new_var = 2;
    goto _ret2;
_chk3:
    if (val >= 3) goto _ret2;
    return 0x115;
_ret2:
    return new_var;
}

/* 0x80134258 | 0x1C */
s32 _dbgMenuGetIndex__FP14tagWINDOW_WORK(void* obj) {
    DbgMenuWindow* window = obj;
    if ((s32)window->key < (s32)lbl_80478848) {
        return -1;
    }
    return window->key - lbl_80478848;
}

/* 0x80134228 | 0x30 */
void* _dbgMenuGetWin__Fl(s32 offset) {
    s32 key;
    if (offset < 0) {
        key = 0;
    } else {
        key = lbl_80478848 + offset;
    }
    return windowSearchID(key);
}

/* 0x80134164 | 0xC4 */
s32 _dbgMenuGetLink__Fl(s32 idx) {
    s32 link;
    if (idx <= 0 || (s32)debugMenuGetNum__Fv() <= idx) {
        return 0;
    }
    link = dbgMenuGetLink__Fl(idx);
    if ((s16)link <= 0 || (s32)debugMenuGetNum__Fv() <= (s16)link) {
        link = 0;
    }
    return link;
}

/* 0x80133E6C | 0x2F8 */
s32 _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(void* obj, s32 offset) {
    s32 index = _dbgMenuGetIndex__FP14tagWINDOW_WORK(obj);
    s32 value;
    DbgMenuWindow* window;

    if (index < 0) {
        return 0;
    }
    if (index == 0) {
        value = dbgMenuGetRootMenu();
    } else {
        window = _dbgMenuGetWin__Fl(index - 1);
        if (window == NULL) {
            value = dbgMenuGetRootMenu();
        } else {
            value = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(
                window, window->cursor.page + window->cursor.row);
        }
        value = (s16)_dbgMenuGetLink__Fl(value);
    }
    return value + offset;
}
