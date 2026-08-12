/**
 * @file gs_gfx_range_800DB098.c
 * @brief Candidate GS render-engine prefix, 0x800DB098-0x800DB890.
 */

#include "dolphin/types.h"

extern void DCFlushRange(void* addr, u32 size);
extern u32 lbl_8047AA80;
extern void fn_800D6A80(u16, s32, u32*, u32*);
extern void jumptable_80315364(void);
extern void fn_800D724C(u32);
extern void fn_800D7268(u32);
extern void fn_800D7284(u32);
extern void fn_800D72A4(u32);
extern void fn_800D72C4(u32);
extern void fn_800D72E4(u32);
extern void fn_800D7304(u32);
extern void fn_800D7328(u32);
extern void fn_800D7344(u32);
extern void fn_800D7360(u32);
extern void fn_800D737C(u32);
extern void fn_800D7398(u32);
extern void fn_800D73C4(u32);
extern void fn_800D73F8(void);
extern void fn_800D740C(void);
extern void fn_800D7420(void);
extern void fn_800D7444(void);
extern void fn_800D7468(void);
extern void fn_800D748C(void);
extern void fn_800D74A0(void);
extern void fn_800D74B4(void);
extern void fn_800D74D0(void);
extern void fn_800D74EC(void);
extern void fn_800D7508(void);
extern void fn_800D7524(void);
extern void fn_800D7540(void);
extern void fn_800D7564(void);
extern void fn_800D7588(void);
extern void fn_800D75AC(void);
extern void fn_800D75D0(void);

#define GS_STATE ((u8*)lbl_8047AA80)
#define GS_U8(offset) (*(u8*)(GS_STATE + (offset)))
#define GS_U16(offset) (*(u16*)(GS_STATE + (offset)))
#define GS_U32(offset) (*(u32*)(GS_STATE + (offset)))

void fn_800DB098(void) {
    u32 capture = GS_U32(0x480);
    u8* cursor = (u8*)GS_U32(0x484);
    u32 writer;
    u32 format;
    s32 i;

    if ((u32)(cursor + 0x68) >
        *(u32*)(capture + 4) + *(u32*)(capture + 8)) {
        *(u8*)(capture + 1) = 1;
        return;
    }

    writer = GS_U32(0x4A8);
    if (writer == (u32)fn_800D75D0) {
        *(u32*)(cursor + 0) = GS_U32(0x4B8);
        *(u32*)(cursor + 4) = GS_U32(0x4BC);
        *(u32*)(cursor + 8) = GS_U32(0x4C0);
        GS_U32(0x484) = (u32)(cursor + 12);
    } else if (writer == (u32)fn_800D75AC ||
               writer == (u32)fn_800D7588) {
        *(u16*)(cursor + 0) = GS_U16(0x4B0);
        *(u16*)(cursor + 2) = GS_U16(0x4B2);
        *(u16*)(cursor + 4) = GS_U16(0x4B4);
        GS_U32(0x484) = (u32)(cursor + 6);
    } else if (writer == (u32)fn_800D7564 ||
               writer == (u32)fn_800D7540) {
        cursor[0] = GS_U8(0x4AC);
        cursor[1] = GS_U8(0x4AD);
        cursor[2] = GS_U8(0x4AE);
        GS_U32(0x484) = (u32)(cursor + 3);
    } else if (writer == (u32)fn_800D7524) {
        *(u32*)(cursor + 0) = GS_U32(0x4B8);
        *(u32*)(cursor + 4) = GS_U32(0x4BC);
        GS_U32(0x484) = (u32)(cursor + 8);
    } else if (writer == (u32)fn_800D7508 ||
               writer == (u32)fn_800D74EC) {
        *(u16*)(cursor + 0) = GS_U16(0x4B0);
        *(u16*)(cursor + 2) = GS_U16(0x4B2);
        GS_U32(0x484) = (u32)(cursor + 4);
    } else if (writer == (u32)fn_800D74D0 ||
               writer == (u32)fn_800D74B4) {
        cursor[0] = GS_U8(0x4AC);
        cursor[1] = GS_U8(0x4AD);
        GS_U32(0x484) = (u32)(cursor + 2);
    } else if (writer == (u32)fn_800D74A0) {
        *(u16*)cursor = GS_U16(0x4B0);
        GS_U32(0x484) = (u32)(cursor + 2);
    } else if (writer == (u32)fn_800D748C) {
        cursor[0] = GS_U8(0x4AC);
        GS_U32(0x484) = (u32)(cursor + 1);
    }

    format = GS_U32(0x4C4);
    if (*(u8*)(*(u32*)(capture + 0xC) + 0x40) == 1) {
        cursor = (u8*)GS_U32(0x484);
        if (format == (u32)fn_800D7468) {
            *(u32*)(cursor + 0) = GS_U32(0x4D4);
            *(u32*)(cursor + 4) = GS_U32(0x4D8);
            *(u32*)(cursor + 8) = GS_U32(0x4DC);
            GS_U32(0x484) = (u32)(cursor + 12);
        } else if (format == (u32)fn_800D7444) {
            *(u16*)(cursor + 0) = GS_U16(0x4CC);
            *(u16*)(cursor + 2) = GS_U16(0x4CE);
            *(u16*)(cursor + 4) = GS_U16(0x4D0);
            GS_U32(0x484) = (u32)(cursor + 6);
        } else if (format == (u32)fn_800D7420) {
            cursor[0] = GS_U8(0x4C8);
            cursor[1] = GS_U8(0x4C9);
            cursor[2] = GS_U8(0x4CA);
            GS_U32(0x484) = (u32)(cursor + 3);
        } else if (format == (u32)fn_800D740C) {
            *(u16*)cursor = GS_U16(0x4CC);
            GS_U32(0x484) = (u32)(cursor + 2);
        } else if (format == (u32)fn_800D73F8) {
            cursor[0] = GS_U8(0x4C8);
            GS_U32(0x484) = (u32)(cursor + 1);
        }
    }

    for (i = 0; i < 2; i++) {
        format = GS_U32(0x4E0 + i * 4);
        if (*(u8*)(*(u32*)(capture + 0xC) + 0x78 + i * 0x1C) == 1) {
            cursor = (u8*)GS_U32(0x484);
            if (format == (u32)fn_800D73C4) {
                cursor[0] = GS_U8(0x4E8 + i * 12);
                cursor[1] = GS_U8(0x4E9 + i * 12);
                cursor[2] = GS_U8(0x4EA + i * 12);
                cursor[3] = GS_U8(0x4EB + i * 12);
                GS_U32(0x484) = (u32)(cursor + 4);
            } else if (format == (u32)fn_800D7398) {
                cursor[0] = GS_U8(0x4E8 + i * 12);
                cursor[1] = GS_U8(0x4E9 + i * 12);
                cursor[2] = GS_U8(0x4EA + i * 12);
                GS_U32(0x484) = (u32)(cursor + 3);
            } else if (format == (u32)fn_800D737C) {
                *(u32*)cursor = GS_U32(0x4F0 + i * 12);
                GS_U32(0x484) = (u32)(cursor + 4);
            } else if (format == (u32)fn_800D7360 ||
                       format == (u32)fn_800D7344) {
                *(u16*)cursor = GS_U16(0x4EC + i * 12);
                GS_U32(0x484) = (u32)(cursor + 2);
            } else if (format == (u32)fn_800D7328) {
                cursor[0] = GS_U8(0x4E8 + i * 12);
                GS_U32(0x484) = (u32)(cursor + 1);
            }
        }
    }

    for (i = 0; i < 8; i++) {
        format = GS_U32(0x500 + i * 4);
        if (*(u8*)(*(u32*)(capture + 0xC) + 0xB0 + i * 0x1C) == 1) {
            cursor = (u8*)GS_U32(0x484);
            if (format == (u32)fn_800D7304) {
                *(u32*)(cursor + 0) = GS_U32(0x528 + i * 16);
                *(u32*)(cursor + 4) = GS_U32(0x52C + i * 16);
                GS_U32(0x484) = (u32)(cursor + 8);
            } else if (format == (u32)fn_800D72E4 ||
                       format == (u32)fn_800D72C4) {
                *(u16*)(cursor + 0) = GS_U16(0x522 + i * 16);
                *(u16*)(cursor + 2) = GS_U16(0x524 + i * 16);
                GS_U32(0x484) = (u32)(cursor + 4);
            } else if (format == (u32)fn_800D72A4 ||
                       format == (u32)fn_800D7284) {
                cursor[0] = GS_U8(0x520 + i * 16);
                cursor[1] = GS_U8(0x521 + i * 16);
                GS_U32(0x484) = (u32)(cursor + 2);
            } else if (format == (u32)fn_800D7268) {
                *(u16*)cursor = GS_U16(0x522 + i * 16);
                GS_U32(0x484) = (u32)(cursor + 2);
            } else if (format == (u32)fn_800D724C) {
                cursor[0] = GS_U8(0x520 + i * 16);
                GS_U32(0x484) = (u32)(cursor + 1);
            }
        }
    }
}

void fn_800DB758(u16 vertCount)
{
    u32 state;
    u32 obj;
    u8* p;
    u16* p16;

    state = lbl_8047AA80;
    if (*(s32*)(state + 0x488) == 7) {
        vertCount = (vertCount & 0x7FFF) << 1;
    }

    obj = *(u32*)(state + 0x480);
    fn_800D6A80(vertCount, *(u32*)(state + 0x488),
                (u32*)(obj + 0x10), (u32*)(obj + 0x14));

    state = lbl_8047AA80;
    switch (*(u32*)(state + 0x488)) {
        case 0:
            *(u8*)*(u32*)(state + 0x484) = 0xB8;
            break;
        case 1:
            *(u8*)*(u32*)(state + 0x484) = 0xA8;
            break;
        case 2:
            *(u8*)*(u32*)(state + 0x484) = 0xB0;
            break;
        case 3:
            *(u8*)*(u32*)(state + 0x484) = 0x90;
            break;
        case 4:
            *(u8*)*(u32*)(state + 0x484) = 0x98;
            break;
        case 5:
            *(u8*)*(u32*)(state + 0x484) = 0xA0;
            break;
        case 6:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
        case 7:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
    }

    state = lbl_8047AA80;
    p = *(u8**)(state + 0x484);
    *(u32*)(state + 0x484) = (u32)(p + 1);
    obj = *(u32*)(state + 0x480);
    *p = (u8)(*p | *(u32*)(*(u32*)(obj + 0xC) + 4));

    state = lbl_8047AA80;
    p16 = *(u16**)(state + 0x484);
    *p16++ = vertCount;
    state = lbl_8047AA80;
    *(u32*)(state + 0x484) = (u32)p16;
}
