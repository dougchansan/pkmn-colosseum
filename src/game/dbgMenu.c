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

/* 0x80132C6C | 0x310 */
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
    lbl_8047AEB0 = fn_800E27B0(lbl_8047AEB8);

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
    lbl_8047AEBC = fn_800E27B0(lbl_8047AEC4);

    dbgMenuInitItems(&lbl_8047AEBC, total);
}
#endif

#if 0
asm void fn_80132F7C(void) {
#include "src/game/effect/effect_util_fn_80132F7C.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 fn_80132F7C(void) {
    if (menuIsCheck(0xab) & 0xFF) {
        menuClose(0xab);
    } else {
        menuOpenCustom(0xab, 0, 0, 0, 0, 0);
    }
    return 0;
}
#pragma scheduling off
#endif

#if 0
asm void dbgMenuMovieTest(void) {
#include "src/game/effect/effect_util_fn_80132FD8.inc"
}
#else
#pragma scheduling on
u32 dbgMenuMovieTest(void) {
    u8 buf[0x18];
    s32 id;
    while ((u32)(fn_801E1874() & 0xFF) == 1) {
        fn_801E1810();
        _threadSwitch();
    }
    id = menuOpen(2, 1);
    if (id != -1) {
        sprintf(buf, lbl_80272AA8, lbl_8047D0E8, id);
        fn_801E189C(buf, 0);
    }
    return 0;
}
#pragma scheduling off
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
#pragma push
#pragma scheduling on
u32 fn_801330C8(void) {
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D9B58(lbl_8047D0F0, lbl_8047D0F0, lbl_8047D0F4, lbl_8047D0F8);
    fn_800DA4C4(0, 6, 7);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA028(0);
    fn_800D6A00(4);
    fn_800D7820(0);
    fn_800D67BC(3);
    fn_800D6680(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    fn_800D5CB8(0, 0xFF, 0x80, 0x40, 0);
    fn_800D6680(*(f32*)lbl_80478AC4, lbl_8047D100, lbl_8047D0F0);
    fn_800D5CB8(0, 0x40, 0xFF, 0, 0);
    fn_800D6680(lbl_8047D104, *(f32*)lbl_80478AC4, lbl_8047D0F0);
    fn_800D5CB8(0, 0, 0x40, 0xFF, 0);
    fn_800D6680(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    fn_800D5CB8(0, 0xFF, 0x80, 0x40, 0);
    fn_800D6728();
    return 0;
}
#pragma pop
#endif


/* 0x80133218 | 0x38 -- fn_800E1544() then print result, return 0 */
u32 dbgMenuGSmemOptimize(void) {
    u32 val = fn_800E1544();
    GSlogWrite(lbl_80272AB8, val);
    return 0;
}


/* 0x80133250 | 0x2C -- fn_800E0E14(1, 1), return 0 */
u32 dbgMenuGSmemDispMap(void) {
    fn_800E0E14(1, 1);
    return 0;
}

#if 0
asm void dbgMenuGSmemCheck(void) {
#include "src/game/effect/effect_util_fn_8013327C.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 dbgMenuGSmemCheck(void) {
    if ((u32)(fn_800E0E14(1, 0) & 0xFF) == 1) {
        GSlogWrite(lbl_80272AE0);
    } else {
        GSlogWrite(lbl_80272AF0);
    }
    return 0;
}
#pragma scheduling off
#endif


/* 0x5C | dbgMenuGSmemCheck -- fn_800E0E14(1,0) check then print */
/* 0x801332D8 | 0x28 -- fn_800D3074(2), return 0 */
#pragma optimization_level 4
#pragma scheduling on
u32 dbgMenuFrameRate20(void) {
    fn_800D3074(2);
    return 0;
}
#pragma scheduling off


/* 0x80133300 | 0x28 -- fn_800D3074(1), return 0 */
#pragma optimization_level 4
#pragma scheduling on
u32 dbgMenuFrameRate30(void) {
    fn_800D3074(1);
    return 0;
}
#pragma scheduling off


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
#pragma optimization_level 4
#pragma scheduling on
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
#pragma scheduling off
#endif


/* 0x801333AC | 0xA4 */
#if 0
asm void dbgMenuColisionDisp(void) {
#include "src/game/effect/effect_util_dbgMenuColisionDisp.inc"
}
#else
#pragma optimization_level 4
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
#pragma optimization_level 4
#pragma scheduling on
u32 debugMenuNodrawArea(void) {
    if (menuIsCheck(0x7) & 0xFF) {
        menuClose(0x7);
    } else {
        menuOpenCustom(0x7, 0, 0, 0, 0, 0);
    }
    return 0;
}
#pragma scheduling off
#endif

#if 0
asm void fn_8013356C(void) {
#include "src/game/effect/effect_util_fn_8013356C.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 fn_8013356C(u32 arg1, s32 arg2) {
    extern u32 heroMoveIsMember(s32);
    extern void heroMoveDismissMember(s32);
    extern void fn_8012F1FC(s32);
    s32 r31 = (arg2 == 0) ? 1 : -1;
    if (r31 >= 0) {
        if (heroMoveIsMember(r31) & 0xFF) {
            heroMoveDismissMember(r31);
        } else {
            fn_8012F1FC(r31);
        }
    }
    return 0;
}
#pragma scheduling off
#endif

#if 0
asm void debugMenuChangeMemInfo(void) {
#include "src/game/effect/effect_util_fn_801335D4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
u32 debugMenuChangeMemInfo(void) {
    if (menuIsCheck(0x4) & 0xFF) {
        menuClose(0x4);
    } else {
        menuOpenCustom(0x4, 0, 0, 0, 0, 0);
    }
    return 0;
}
#pragma scheduling off
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

#pragma push
#pragma scheduling on
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
#pragma pop
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


/* 0x801337E4 | 0x2C -- set lbl_8047AED1 = 0, call menuClose(lbl_80478848) */
#if 0
asm void dbgMenuClose(void) {
#include "src/game/effect/effect_util_dbgMenuClose.inc"
}
#else
void dbgMenuClose(void) {
    lbl_8047AED1 = 0;
    menuClose(lbl_80478848);
}
#endif


/* 0x80133810 | 0x94 */
#if 0
asm void dbgMenuMain(void) {
#include "src/game/effect/effect_util_dbgMenuMain.inc"
}
#else
#pragma push
#pragma peephole off
void dbgMenuMain(u8 flag) {
    extern s32 _dbgMenuSub__Fl(u32);
    u32 (*fp)(void);
    u32 result;
    fp = (u32 (*)(void))lbl_80478F88;
    if (fp == NULL) {
        result = 0;
    } else {
        result = fp();
    }
    if (result == 0) {
        return;
    }
    if (lbl_8047AED0 == 0) {
        return;
    }
    if (menuIsCheck(lbl_80478848)) {
        return;
    }
    lbl_8047AED1 = flag;
    do {
        if (_dbgMenuSub__Fl(0) < 0) {
            return;
        }
    } while (lbl_8047AED1 == 1);
}
#pragma pop
#endif


static inline s32 dbgMenuGetMenuNumber(s32 key) {
    return _dbgMenuGetMenuNo__Fl(key);
}

static inline s32 dbgMenuOpenWindow(s32 sceneId, s32 key, s32* savedSelection) {
    return menuOpenCustom(sceneId, key, savedSelection, 0, 1, 0);
}

static inline s32 dbgMenuInvokeCallback(
    EffectUtilEntryCallback callback, s32 valueIndex, DbgMenuWindow* window) {
    return callback(valueIndex, window->cursor.page + window->cursor.row);
}

static inline s32 dbgMenuGetCursorPage(DbgMenuWindow* window) {
    return window->cursor.page;
}

static inline s32 dbgMenuGetCursorIndex(DbgMenuWindow* window) {
    return window->cursor.page + window->cursor.row;
}

static inline s32 dbgMenuGetValidatedLink(s32 valueIndex) {
    EffectUtilCountFunc countFunc;
    EffectUtilEntryFunc entryFunc;
    EffectUtilEntry* entry;
    s32 count;
    s32 link;

    if (valueIndex <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= valueIndex)) {
        return 0;
    }
    entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
    entry = entryFunc == NULL ? NULL : entryFunc(valueIndex);
    link = entry == NULL ? 0 : entry->link;
    if ((s16)link <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= (s16)link)) {
        return 0;
    }
    return link;
}

/* 0x801338A4 | 0x2AC */
#if 0
asm void _dbgMenuSub__Fl(void) {
#include "src/game/effect/effect_util__dbgMenuSub__Fl.inc"
}
#else
#pragma push
#pragma scheduling on
s32 _dbgMenuSub__Fl(s32 offset) {
    s32 prevOffset;
    s32 result;
    s32 sceneId;
    s32 key;
    s32 valueIndex;
    s32 link;
    s32 callbackResult;
    s32* outValue;
    DbgMenuWindow* obj;
    EffectUtilEntry* entry;
    EffectUtilEntryFunc entryFunc;
    EffectUtilEntryCallback callback;

    prevOffset = offset - 1;
    result = 0;
    sceneId = (s32)lbl_80478848 + offset;

retry:
    if (offset < 0) {
        key = 0;
    } else {
        key = (s32)lbl_80478848 + prevOffset;
    }
    outValue = (s32*)(lbl_8047AEDC + dbgMenuGetMenuNumber(key) * sizeof(s32));
    valueIndex = dbgMenuOpenWindow(sceneId, key, outValue);
    obj = windowSearchID(sceneId);
    if (obj != NULL) {
        *outValue = obj->cursor.page + obj->cursor.row;
    } else {
        *outValue = 0;
    }

    if (valueIndex == -1) {
        if (offset == 0) {
            result = -1;
        }
    } else {

    valueIndex = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, dbgMenuGetCursorIndex(obj));
    link = dbgMenuGetValidatedLink(valueIndex);

    if ((s16)link != 0) {
        entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
        if (entryFunc == NULL) {
            entry = NULL;
        } else {
            entry = entryFunc(valueIndex);
        }
        if (entry == NULL) {
            callback = NULL;
        } else {
            callback = entry->callback;
        }
        if (callback != NULL) {
            callbackResult = dbgMenuInvokeCallback(callback, valueIndex, obj);
        } else {
            callbackResult = 1;
        }
        if (callbackResult == 0) {
            menuClose(lbl_80478848);
            return 1;
        }
        if (callbackResult == -1 || (s16)link == 1) {
            goto retry;
        }
        if (_dbgMenuSub__Fl(offset + 1) != 1) {
            goto retry;
        }
        return 1;
    }

    menuClose(lbl_80478848);
    entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
    if (entryFunc == NULL) {
        entry = NULL;
    } else {
        entry = entryFunc(valueIndex);
    }
    if (entry == NULL) {
        callback = NULL;
    } else {
        callback = entry->callback;
    }
    if (callback != NULL) {
        callback(valueIndex, dbgMenuGetCursorIndex(obj));
    }
    return 1;
    }

    menuClose(sceneId);
    return result;
}
#pragma pop
#endif


/* 0x80133B50 | 0x94 */
#if 0
asm void _dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl(void) {
#include "src/game/effect/effect_util_fn_80133B50.inc"
}
#else
u32 _dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl(u32 arg0, u32* outMax) {
    u32 index;
    u32 value;
    u32 max;
    s32 done;

    if (outMax != NULL) {
        *outMax = 0;
    }
    index = 0;
    do {
        value = _dbgMenuGetMsgID__FP14tagWINDOW_WORKl((void*)arg0, index);
        if (outMax != NULL) {
            value = GSmsgGetRect(value) >> 16;
            max = *outMax;
            if ((s32)max < (s32)value) {
                *outMax = value;
            }
        }
        done = _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl((void*)arg0, index++);
    } while (done == 0);
    return index;
}
#endif

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


/* 0x80133C3C | 0x1E0 */
#if 0
asm void _dbgMenuGetMenuNo__Fl(void) {
#include "src/game/effect/effect_util__dbgMenuGetMenuNo__Fl.inc"
}
#else
s32 _dbgMenuGetMenuNo__Fl(s32 key) {
    s32 count;
    s32 index;
    s32 selected;
    u8* obj;
    EffectUtilEntry* entry;
    EffectUtilCountFunc countFunc;
    EffectUtilEntryFunc entryFunc;

    obj = (u8*)windowSearchID(key);
    if (obj != NULL) {
        selected = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, (s8)obj[0x94] + (s8)obj[0x95]);
        if (selected > 0) {
            countFunc = (EffectUtilCountFunc)lbl_80478F88;
            if (countFunc != NULL && countFunc() > selected) {
                entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
                entry = entryFunc != NULL ? entryFunc(selected) : NULL;
                selected = entry != NULL ? entry->link : 0;
                if (selected > 0) {
                    countFunc = (EffectUtilCountFunc)lbl_80478F88;
                    if (countFunc == NULL || countFunc() <= selected) {
                        selected = 0;
                    }
                } else {
                    selected = 0;
                }
            } else {
                selected = 0;
            }
        } else {
            selected = 0;
        }
        count = 0;
        for (index = 0; index < (s16)selected; index++) {
            entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
            entry = entryFunc != NULL ? entryFunc(index) : NULL;
            if (entry == NULL || (entry->flags & 0x80)) {
                count++;
            }
        }
    } else {
        count = 0;
        for (index = 0; index < (s32)dbgMenuGetRootMenu(); index++) {
            entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
            entry = entryFunc != NULL ? entryFunc(index) : NULL;
            if (entry == NULL || (entry->flags & 0x80)) {
                count++;
            }
        }
    }
    return count;
}
#endif

#if 0
asm void _dbgMenuGetMsgID__FP14tagWINDOW_WORKl(void) {
#include "src/game/effect/effect_util_fn_80133E1C.inc"
}
#else
#pragma peephole off
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
#pragma peephole on
#endif

static inline s32 dbgMenuGetIndexInline(DbgMenuWindow* window) {
    u32 key = window->key;
    if ((s32)key < (s32)lbl_80478848) {
        return -1;
    }
    return key - lbl_80478848;
}

static inline DbgMenuWindow* dbgMenuGetWinInline(s32 offset) {
    s32 mask = offset >> 31;
    s32 key = (s32)lbl_80478848 + offset;
    return windowSearchID(key & ~mask);
}

static inline s32 dbgMenuGetRootInline(void) {
    u32 fn_800057A8();
    s32 result;
    s32 value = fn_800057A8();

    if (value != 1) {
        if (value < 1) {
            result = 2;
        } else if (value < 3) {
            return 0x115;
        }
    }
    return result;
}

static inline s32 dbgMenuResolveLink(s32 link) {
    if (link <= 0 || (s32)debugMenuGetNum__Fv() <= link) {
        return 0;
    }
    link = dbgMenuGetLink__Fl(link);
    if ((s16)link <= 0 || (s32)debugMenuGetNum__Fv() <= (s16)link) {
        return 0;
    }
    return link;
}

/* 0x80133E6C | 0x2F8 */
#if 0
asm void _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(void) {
#include "src/game/effect/effect_util__dbgMenuGetItemNo__FP14tagWINDOW_WORKl.inc"
}
#else
static inline void dbgMenuReadIndexInline(DbgMenuWindow* window, s32* index) {
    *index = dbgMenuGetIndexInline(window);
}

#pragma push
#pragma scheduling on
s32 _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(void* obj, s32 offset) {
    s32 rel;
    s32 value;
    s32 baseValue;
    s32 linked;
    DbgMenuWindow* window = obj;
    DbgMenuWindow* prev;
    DbgMenuWindow* prior;

    dbgMenuReadIndexInline(window, &rel);
    if (rel < 0) {
        return 0;
    }
    do {
        if (rel == 0) {
            value = dbgMenuGetRootInline();
            break;
        }

        prev = dbgMenuGetWinInline(rel - 1);
        if (prev == NULL) {
            value = dbgMenuGetRootInline();
        } else {
            baseValue = prev->cursor.page + prev->cursor.row;
            rel = dbgMenuGetIndexInline(prev);
            if (rel < 0) {
                value = 0;
            } else if (rel == 0) {
                value = dbgMenuGetRootInline() + baseValue;
            } else {
                prior = dbgMenuGetWinInline(rel - 1);
                if (prior == NULL) {
                    linked = dbgMenuGetRootInline();
                } else {
                    linked = dbgMenuGetCursorPage(prior) + prior->cursor.row;
                    rel = _dbgMenuGetIndex__FP14tagWINDOW_WORK(prior);
                    if (rel < 0) {
                        linked = 0;
                    } else if (rel == 0) {
                        linked += dbgMenuGetRootMenu();
                    } else {
                        DbgMenuWindow* earlier = _dbgMenuGetWin__Fl(rel - 1);
                        if (earlier == NULL) {
                            linked += dbgMenuGetRootMenu();
                        } else {
                            linked += (s16)_dbgMenuGetLink__Fl(_dbgMenuGetItemNo__FP14tagWINDOW_WORKl(
                                earlier, earlier->cursor.page + earlier->cursor.row));
                        }
                    }
                }

                linked = dbgMenuResolveLink(linked);
                value = (s16)linked + baseValue;
            }
        }

        value = dbgMenuGetValidatedLink(value);
    } while (0);
    return value + offset;
}
#pragma pop
#endif


/* 0x80134164 | 0xC4 */
#if 0
asm void _dbgMenuGetLink__Fl(void) {
#include "src/game/effect/effect_util_fn_80134164.inc"
}
#else
s32 _dbgMenuGetLink__Fl(s32 idx) {
    s32 count;
    EffectUtilEntry* result;

    if (idx <= 0) { return 0; }

    {
        s32 (*fp)(void) = (s32 (*)(void))lbl_80478F88;
        if (fp == NULL) { count = 0; } else { count = fp(); }
    }
    if (count <= idx) { return 0; }

    {
        EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
        if (fp == NULL) { result = NULL; } else { result = fp(idx); }
    }
    if (result == NULL) { idx = 0; } else { idx = result->link; }

    if ((s16)idx <= 0) {
        idx = 0;
    } else {
        s32 (*fp)(void) = (s32 (*)(void))lbl_80478F88;
        if (fp == NULL) { count = 0; } else { count = fp(); }
        if (count <= (s16)idx) { idx = 0; }
    }
    return idx;
}
#endif


/* 0x80134228 | 0x30 -- saturate add: max(0, lbl_80478848 + arg) then windowSearchID */
/* extern void* windowSearchID(s32 key); -- forward-declared K&R style above */
#pragma push
#pragma optimization_level 1
void* _dbgMenuGetWin__Fl(s32 offset) {
    s32 key;
    s32 mask;
    mask = offset >> 31;
    key = (s32)lbl_80478848 + offset;
    return windowSearchID(key & ~mask);
}
#pragma pop


/* 0x80134258 | 0x1C -- get relative key from obj->0x04, return (key - lbl_80478848), or -1 */
s32 _dbgMenuGetIndex__FP14tagWINDOW_WORK(void* obj) {
    u32 val = *(u32*)((u8*)obj + 0x04);
    s32 result = -1;

    if ((s32)val >= (s32)lbl_80478848) {
        result = (s32)(val - lbl_80478848);
    }
    return result;
}

#if 0
asm void dbgMenuGetRootMenu(void) {
#include "src/game/effect/effect_util_fn_80134274.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma scheduling on
u32 dbgMenuGetRootMenu(void) {
    int new_var;
    extern u32 fn_800057A8(void);
    s32 val = (s32)fn_800057A8();
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
#pragma pop
#endif

#if 0
asm void dbgMenuGetLink__Fl(void) {
#include "src/game/effect/effect_util_fn_801342B8.inc"
}
#else
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
#endif


/* 0x80134304 | 0x38 */
#if 0
asm u32 debugMenuGetNum__Fv(void) {
#include "src/game/effect/effect_util_debugMenuGetNum__Fv.inc"
}
#else
u32 debugMenuGetNum__Fv(void) {
    u32 (*fp)(void) = (u32 (*)(void))lbl_80478F88;
    if (fp == NULL) {
        return 0;
    }
    return fp();
}
#endif
