/** Candidate CARD status range, 0x800B5228 - 0x800B5AE0. */
#include "crt/string.h"
#include "dolphin/os/OSTime.h"
#include "src/dolphin/card_dsp_private.h"

typedef struct CARDStat {
    /* 0x00 */ char fileName[32];
    /* 0x20 */ u32 length;
    /* 0x24 */ u32 time;
    /* 0x28 */ u8 gameName[4];
    /* 0x2C */ u8 company[2];
    /* 0x2E */ u8 bannerFormat;
    /* 0x2F */ u8 _2F;
    /* 0x30 */ u32 iconAddr;
    /* 0x34 */ u16 iconFormat;
    /* 0x36 */ u16 iconSpeed;
    /* 0x38 */ u32 commentAddr;
    /* 0x3C */ u32 offsetBanner;
    /* 0x40 */ u32 offsetBannerTlut;
    /* 0x44 */ u32 offsetIcon[8];
    /* 0x64 */ u32 offsetIconTlut;
    /* 0x68 */ u32 offsetData;
} CARDStat;

extern void __CARDDefaultApiCallback(s32 chan, s32 result);
extern s32 fn_800B4270(CARDControl* card, CARDDirEntry* entry);
extern s32 fn_800B4308(CARDDirEntry* entry);
extern void* __CARDGetDirBlock(CARDControl* card);
extern s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
extern BOOL __CARDIsOpened(CARDControl* card, s32 fileNo);
extern void DeleteCallback(s32 chan, s32 result);

s32 CARDDeleteAsync(s32 chan, const char* fileName, CARDCallback callback)
{
    CARDControl* card;
    s32 fileNo;
    s32 result;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    extern s32 __CARDGetFileNo(CARDControl* card, const char* fileName,
                               s32* fileNo);

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }
    if (__CARDIsOpened(card, fileNo)) {
        return __CARDPutControlBlock(card, -1);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    card->startBlock = entry->startBlock;
    memset(entry, 0xff, sizeof(CARDDirEntry));

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    result = __CARDUpdateDir(chan, DeleteCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

void UpdateIconOffsets(CARDDirEntry* entry, CARDStat* stat)
{
    u32 offset;
    BOOL iconTlut;
    s32 i;

    offset = entry->iconAddr;
    if (offset == 0xFFFFFFFF) {
        stat->bannerFormat = 0;
        stat->iconFormat = 0;
        stat->iconSpeed = 0;
        offset = 0;
    }

    iconTlut = FALSE;
    switch (entry->bannerFormat & 3) {
    case 1:
        stat->offsetBanner = offset;
        offset += 96 * 32;
        stat->offsetBannerTlut = offset;
        offset += 2 * 256;
        break;
    case 2:
        stat->offsetBanner = offset;
        offset += 2 * 96 * 32;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        break;
    default:
        stat->offsetBanner = 0xFFFFFFFF;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        break;
    }

    for (i = 0; i < 8; i++) {
        switch ((entry->iconFormat >> (2 * i)) & 3) {
        case 1:
            stat->offsetIcon[i] = offset;
            offset += 32 * 32;
            iconTlut = TRUE;
            break;
        case 2:
            stat->offsetIcon[i] = offset;
            offset += 2 * 32 * 32;
            break;
        default:
            stat->offsetIcon[i] = 0xFFFFFFFF;
            break;
        }
    }

    if (iconTlut) {
        stat->offsetIconTlut = offset;
        offset += 2 * 256;
    } else {
        stat->offsetIconTlut = 0xFFFFFFFF;
    }
    stat->offsetData = offset;
}

s32 fn_800B5530(s32 chan, s32 fileNo, CARDStat* stat)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if (fileNo < 0 || fileNo >= 127) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result == -10) {
        result = fn_800B4308(entry);
    }

    if (result >= 0) {
        memcpy(stat->gameName, entry->gameName, 4);
        memcpy(stat->company, entry->company, 2);
        stat->length = (u32)entry->length * card->sectorSize;
        memcpy(stat->fileName, entry->fileName, 32);
        stat->time = entry->time;
        stat->bannerFormat = entry->bannerFormat;
        stat->iconAddr = entry->iconAddr;
        stat->iconFormat = entry->iconFormat;
        stat->iconSpeed = entry->animationSpeed;
        stat->commentAddr = entry->commentAddr;
        UpdateIconOffsets(entry, stat);
    }

    return __CARDPutControlBlock(card, result);
}

s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat* stat,
                       CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;

    if (fileNo < 0 || fileNo >= 127 ||
        (stat->iconAddr != 0xFFFFFFFF && stat->iconAddr >= 0x200) ||
        (stat->commentAddr != 0xFFFFFFFF &&
         stat->commentAddr % 0x2000 > 0x1FC0)) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    entry->bannerFormat = stat->bannerFormat;
    entry->iconAddr = stat->iconAddr;
    entry->iconFormat = stat->iconFormat;
    entry->animationSpeed = stat->iconSpeed;
    entry->commentAddr = stat->commentAddr;
    UpdateIconOffsets(entry, stat);

    if (entry->iconAddr == 0xFFFFFFFF) {
        entry->animationSpeed = (entry->animationSpeed & ~3) | 1;
    }

    entry->time = (u32)(OSGetTime() / (*(u32*)0x800000F8 / 4));
    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

s32 fn_800B57D0(s32 chan, s32 fileNo, CARDDirEntry* entry)
{
    CARDControl* card;
    s32 result;
    CARDDirEntry* dirEntry;

    if (fileNo < 0 || fileNo >= 127) {
        return -128;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    dirEntry = &((CARDDirEntry*)__CARDGetDirBlock(card))[fileNo];
    result = fn_800B4270(card, dirEntry);
    if (result == -10) {
        result = fn_800B4308(dirEntry);
    }
    if (result >= 0) {
        memcpy(entry, dirEntry, sizeof(CARDDirEntry));
    }
    return __CARDPutControlBlock(card, result);
}

s32 fn_800B588C(s32 chan, s32 fileNo, CARDDirEntry* input,
                CARDCallback callback)
{
    CARDControl* card;
    CARDDirEntry* dir;
    CARDDirEntry* entry;
    s32 result;
    u8* p;
    s32 i;

    if (fileNo < 0 || fileNo >= 127 ||
        (u8)input->fileName[0] == 0xFF || (u8)input->fileName[0] == 0) {
        return -128;
    }

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = fn_800B4270(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    for (p = (u8*)input->fileName; p < (u8*)&input->time; p++) {
        if (*p != 0) {
            continue;
        }
        while (++p < (u8*)&input->time) {
            *p = 0;
        }
        break;
    }

    if (memcmp(entry->fileName, input->fileName, 32) != 0 ||
        memcmp(entry->gameName, input->gameName, 4) != 0 ||
        memcmp(entry->company, input->company, 2) != 0) {
        for (i = 0; i < 127; i++) {
            if (i != fileNo) {
                CARDDirEntry* entry = &dir[i];

                if ((u8)entry->gameName[0] != 0xFF &&
                    memcmp(entry->gameName, input->gameName, 4) == 0 &&
                    memcmp(entry->company, input->company, 2) == 0 &&
                    memcmp(entry->fileName, input->fileName, 32) == 0) {
                    return __CARDPutControlBlock(card, -7);
                }
            }
        }
        memcpy(entry->fileName, input->fileName, 32);
        memcpy(entry->gameName, input->gameName, 4);
        memcpy(entry->company, input->company, 2);
    }

    entry->time = input->time;
    entry->bannerFormat = input->bannerFormat;
    entry->iconAddr = input->iconAddr;
    entry->iconFormat = input->iconFormat;
    entry->animationSpeed = input->animationSpeed;
    entry->commentAddr = input->commentAddr;
    entry->permission = input->permission;
    entry->copyTimes = input->copyTimes;

    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
