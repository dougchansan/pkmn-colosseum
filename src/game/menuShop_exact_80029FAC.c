/** Exact menuShop varargs callbacks, 0x80029FAC - 0x8002A3D4. */
#include "dolphin/types.h"

extern void* __va_arg(void*, s32);
extern u32 lbl_80478E54;
extern u32 lbl_80478E4C;
extern u32 lbl_80478E3C;
extern void winMsgOpenWithSE(s32, u32, s32, s32, u8);

typedef struct ShopVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} ShopVaList;
typedef ShopVaList ShopVaListArray[1];

/* RULE-EXCEPTION(user-approved): code-generation-shaped varargs/table setup;
 * see docs/RULE_EXCEPTIONS.md. */
u32 shopFormatTextA(u8* r3, s32 r4, s32 r5, s32 r6, ...)
{
    ShopVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    u8* table;
    s32 idx;
    s32 offset;
    u32* new_var;

    *(u32*)list = 0x04000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = r4 << 2;
    map = (u8*)((((((((((((lbl_80478E54 & 0xFFFFFFFFFFFFFFFFu) &
                           0xFFFFFFFFFFFFFFFFu) &
                          0xFFFFFFFFFFFFFFFFu) &
                         0xFFFFFFFFFFFFFFFFu) &
                        0xFFFFFFFFFFFFFFFFu) &
                       0xFFFFFFFFFFFFFFFFu) &
                      0xFFFFFFFFFFFFFFFFu) &
                     0xFFFFFFFFFFFFFFFFu) &
                    0xFFFFFFFFFFFFFFFFu) &
                   0xFFFFFFFFFFFFFFFFu) &
                  0xFFFFFFFFFFFFFFFFu) &
                 0xFFFFFFFFFFFFFFFFu);
    r31 = r5 << 2;
    table = (u8*)lbl_80478E4C;
    r30 = 1;
    offset = map[idx] * 0x4c;
    *r3 = table[offset];
    r28 = (u8*)*(volatile u32*)(new_var = &lbl_80478E4C) + offset + 4;
    while (r6 >= 0) {
        if (r30 != 0) {
            r29 = r6;
            r30 = 0;
        } else {
            r30 = 1;
            msgctrlSetValue(r29, (void*)r6);
        }
        r6 = *(s32*)__va_arg(list, 1);
    }
    return *(u32*)(r28 + r31);
}

u32 shopFormatTextB(u8* r3, s32 r4, s32 r5, s32 r6, ...)
{
    ShopVaListArray list;
    s32 r31;
    s32 r30;
    u8 new_var;
    s32 r29;
    u8* r28;
    u8* map;
    u8* table;
    s32 idx;
    s32 offset;

    *(u32*)list = 0x04000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = r4 << 2;
    map = (u8*)(((((((lbl_80478E54 & 0xFFFFFFFFFFFFFFFFu) &
                     0xFFFFFFFFFFFFFFFFu) &
                    0xFFFFFFFFFFFFFFFFu) &
                   0xFFFFFFFFFFFFFFFFu) &
                  0xFFFFFFFFFFFFFFFFu) &
                 0xFFFFFFFFFFFFFFFFu) &
                0xFFFFFFFFFFFFFFFFu);
    r31 = r5 << 2;
    new_var = map[idx];
    table = (u8*)lbl_80478E3C;
    r30 = 1;
    offset = new_var * 0x3c;
    *r3 = table[offset];
    r28 = (u8*)*(volatile u32*)&lbl_80478E3C + offset + 4;
    while (r6 >= 0) {
        if (0 != r30) {
            r29 = r6;
            r30 = 0;
        } else {
            r30 = 1;
            msgctrlSetValue(r29, (void*)r6);
        }
        r6 = *(s32*)__va_arg(list, 1);
    }
    return *(u32*)(r28 + r31);
}

void shopMsgOpenA(u8* r3, s32 r4, s32 r5, ...)
{
    ShopVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    s32 idx;
    u8 r27;

    *(u32*)list = 0x03000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = (s32)r3 << 2;
    map = (u8*)(lbl_80478E54 & 0xFFFFFFFFFFFFFFFFu);
    r31 = r4 << 2;
    r3 = (u8*)lbl_80478E4C + map[idx] * 0x4c;
    r27 = r3[0];
    r28 = r3 + 4;
    r30 = 1;
    while (r5 >= 0) {
        if (r30 != 0) {
            r29 = r5;
            r30 = 0;
        } else {
            r30 = 1;
            msgctrlSetValue(r29, (void*)r5);
        }
        r5 = *(s32*)__va_arg(list, 1);
    }
    winMsgOpenWithSE(2, *(u32*)(r28 + r31), 1, 0, r27);
    winMsgClose(1);
}

void shopMsgOpenB(u8* r3, s32 r4, s32 r5, ...)
{
    ShopVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    s32 idx;
    u8 r27;

    *(u32*)list = 0x03000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = (s32)r3 << 2;
    map = (u8*)lbl_80478E54;
    r31 = r4 << 2;
    r3 = (u8*)lbl_80478E3C + map[idx] * (0x3c & 0xFFFFFFFFFFFFFFFFu);
    r27 = r3[0];
    r28 = r3 + 4;
    r5 = r5;
    r30 = 1;
    while (r5 >= 0) {
        if (r30 != 0) {
            r29 = r5;
            r30 = 0;
        } else {
            r30 = 1;
            msgctrlSetValue(r29, (void*)r5);
        }
        r5 = *(s32*)__va_arg(list, 1);
    }
    winMsgOpenWithSE(2, *(u32*)(r28 + r31), 1, 0, r27);
    winMsgClose(1);
}
