/** Exact PDA-mail row widgets, 0x8004C6C0 - 0x8004CF78. */
#include "dolphin/types.h"
#include "game/cursor_bios.h"

typedef struct PdaMailRowTable {
    s32 values[10];
} PdaMailRowTable;

typedef union PdaMailRowCursor {
    u32 storage;
    u16 packed;
    struct {
        s8 page;
        s8 row;
    } position;
} PdaMailRowCursor;

extern s32 pdaMailGetMailID(s32);
extern u32 mailGetSenderName(s32);
extern u32 mailGetSubject(s32);
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue(s32, void*);
extern s32 saveFlagBitsTest(s32);
extern u32 mailGetAttachFileGroup(s32);
extern void winSpriteSetDisp(void*, s32);
extern const s32 lbl_802672A0[10];
extern const s32 lbl_80267278[10];
extern const s32 lbl_80267250[10];
extern const s32 lbl_80267228[10];
extern const s32 lbl_80267200[10];

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u32 pdaMailDrawRowSender(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    void* gschar;

    table = *(const PdaMailRowTable*)lbl_802672A0;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSenderName(mailId);
    if (message != 0) {
        gschar = GSmsgGetGSchar(message);
        msgctrlSetValue(0x37, gschar);
        *(u32*)(object + 0x4C) = 0xE7;
    } else {
        *(u32*)(object + 0x4C) = 0;
    }
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (saveFlagBitsTest(mailId) != 0) {
            object[0x64] = 0xFF;
            object[0x65] = 0xFF;
            object[0x66] = 0xFF;
        } else {
            object[0x64] = 0xD5;
            object[0x65] = 0xAA;
            object[0x66] = 0x33;
        }
    }
    return 0;
}

u32 pdaMailDrawRowSubject(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    void* gschar;

    table = *(const PdaMailRowTable*)lbl_80267278;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSubject(mailId);
    if (message != 0) {
        gschar = GSmsgGetGSchar(message);
        msgctrlSetValue(0x37, gschar);
        *(u32*)(object + 0x4C) = 0xE7;
    } else {
        *(u32*)(object + 0x4C) = 0;
    }
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (saveFlagBitsTest(mailId) != 0) {
            object[0x64] = 0xFF;
            object[0x65] = 0xFF;
            object[0x66] = 0xFF;
        } else {
            object[0x64] = 0xD5;
            object[0x65] = 0xAA;
            object[0x66] = 0x33;
        }
    }
    return 0;
}

u32 pdaMailDrawRowAttach(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267250;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (mailGetAttachFileGroup(mailId) != 0) {
            visible = 1;
        } else {
            visible = 0;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 pdaMailDrawRowFlagA(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267228;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (saveFlagBitsTest(mailId) != 0) {
            visible = 1;
        } else {
            visible = 0;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 pdaMailDrawRowFlagB(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267200;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (saveFlagBitsTest(mailId) != 0) {
            visible = 0;
        } else {
            visible = 1;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}
#pragma peephole reset
