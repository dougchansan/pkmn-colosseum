#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

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

void dbgMenuCursor(DbgMenuWindow* obj)
{
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
