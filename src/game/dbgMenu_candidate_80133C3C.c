/** Exact debug-menu entry counter, 0x80133C3C - 0x80133E1C. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

typedef struct DbgMenuWindow {
    u32 field_00;
    u32 key;
    u8 pad_08[0x8C];
    struct {
        s8 page;
        s8 row;
    } cursor;
} DbgMenuWindow;

/* Inline copy of dbgMenuGetRootMenu (0x80134274); retail expands it here. */
/* RULE-EXCEPTION(user-approved): inline copy of the real dbgMenuGetRootMenu — see docs/RULE_EXCEPTIONS.md */
static inline u32 dbgMenuRootMenuInline(void)
{
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

/* Inline copy of _dbgMenuGetLink__Fl (0x80134164), with debugMenuGetNum
 * and dbgMenuGetLink expanded as retail does. */
/* RULE-EXCEPTION(user-approved): inline copy of the real dbgMenuGetLink__Fl — see docs/RULE_EXCEPTIONS.md */
static inline s32 dbgMenuGetLinkInline(s32 idx)
{
    EffectUtilCountFunc countFunc;
    EffectUtilEntryFunc entryFunc;
    EffectUtilEntry* entry;
    s32 count;

    if (idx <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= idx)) {
        return 0;
    }
    entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
    entry = entryFunc == NULL ? NULL : entryFunc(idx);
    idx = entry == NULL ? 0 : entry->link;
    if ((s16)idx <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= (s16)idx)) {
        idx = 0;
    }
    return idx;
}

/* Same test as _dbgMenuCheckTerminate, on an item number. */
static inline u8 dbgMenuIsTerminateInline(s32 index)
{
    EffectUtilEntryFunc fp = (EffectUtilEntryFunc)lbl_80478F8C;
    EffectUtilEntry* entry;
    u32 ret;

    if (fp == NULL) {
        entry = NULL;
    } else {
        entry = fp(index);
    }
    if (entry == NULL) {
        ret = 1;
    } else {
        ret = (entry->flags >> 7) & 1;
    }
    return ret;
}

s32 _dbgMenuGetMenuNo__Fl(s32 key)
{
    DbgMenuWindow* window = windowSearchID(key);
    s32 index;
    s32 count;
    s32 num;

    if (window != NULL) {
        num = (s16)dbgMenuGetLinkInline(
            _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(window, window->cursor.page + window->cursor.row));
        count = 0;
        for (index = 0; index < num; index++) {
            if (dbgMenuIsTerminateInline(index)) {
                count++;
            }
        }
    } else {
        count = 0;
        for (index = 0; index < (s32)dbgMenuRootMenuInline(); index++) {
            if (dbgMenuIsTerminateInline(index)) {
                count++;
            }
        }
    }
    return count;
}
