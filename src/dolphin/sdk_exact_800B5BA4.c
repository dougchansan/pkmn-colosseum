#include "src/dolphin/card_dsp_private.h"

extern s32 __CARDGetStatusEx(s32 chan, s32 fileNo, CARDDirEntry* entry);
extern s32 __CARDSetStatusExAsync(s32 chan, s32 fileNo, CARDDirEntry* entry,
                       CARDCallback callback);

s32 CARDGetAttributes(s32 chan, s32 fileNo, u8* attr)
{
    s32 result;
    CARDDirEntry entry;

    result = __CARDGetStatusEx(chan, fileNo, &entry);
    if (result == 0) {
        *attr = entry.permission;
    }
    return result;
}

s32 CARDSetAttributes(s32 chan, s32 fileNo, u8 attr, CARDCallback callback)
{
    s32 result;
    CARDDirEntry entry;

    result = __CARDGetStatusEx(chan, fileNo, &entry);
    if (result < 0) {
        return result;
    }
    entry.permission = attr;
    return __CARDSetStatusExAsync(chan, fileNo, &entry, callback);
}
