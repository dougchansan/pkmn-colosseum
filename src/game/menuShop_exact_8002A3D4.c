/** Exact menuShop display callbacks, 0x8002A3D4 - 0x8002A5B0.
 * RULE-EXCEPTION(user-approved): inherited scheduling/nopeephole mode; see
 * docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern u32 GSmsgGetRect(u32 id);
extern void msgctrlSetValue(u32 id, s32 value);
extern void GSmsgPrintColor(s32, s32, s32, s32);
extern void GSmsgPrintRight(s32, s32, s16, s16, s32, s32);
extern u8 lbl_80266E58[];

s32 fn_8002A3D4(void* owner, u8* draw)
{
    void* context;

    context = *(void**)((u8*)owner + 0x60);
    draw[0x64] = ((u8*)context)[0x10];
    draw[0x65] = ((u8*)context)[0x11];
    draw[0x66] = ((u8*)context)[0x12];
    draw[0x67] = 0xff;
    return 0;
}

s32 fn_8002A400(void* owner, u8* draw)
{
    u8* savedDraw;
    void* context;
    u32 id;
    u32 bounds;

    savedDraw = draw;
    context = *(void**)((u8*)owner + 0x60);
    msgctrlSetValue(0x50,
                    *(s32*)((u8*)context + 8) *
                        *(s32*)(*(u32*)((u8*)context + 0xc)));
    if (*(s32*)((u8*)context + 0x14) != 0) {
        id = 0x153;
    } else {
        id = 0x151;
    }
    bounds = GSmsgGetRect(id);
    GSmsgPrintColor((s32)*(s16*)(savedDraw + 0x54) - (s32)(bounds >> 16), 0,
                 -1, id);
    return 0;
}

typedef struct ShopDisplayEntry {
    s32 key;
    s32 field_4;
    s32 field_8;
} ShopDisplayEntry;

typedef struct ShopDigitContext {
    u8 pad_0[0xC];
    s32* value;
} ShopDigitContext;

typedef struct ShopMenuOwner {
    u8 pad_0[0x60];
    ShopDigitContext* context;
} ShopMenuOwner;

typedef struct ShopDrawData {
    u8 pad_0[6];
    s16 key;
    u8 pad_8[0x4C];
    s16 x;
    s16 y;
} ShopDrawData;

s32 fn_8002A48C(ShopMenuOwner* owner, ShopDrawData* draw)
{
    ShopDigitContext* context = owner->context;
    ShopDisplayEntry* entry;
    s32 index;
    s32 place;
    s32 divisor;
    s32 value;
    s32 tens;

    entry = (ShopDisplayEntry*)lbl_80266E58;
    index = 0;
    while (index < 2) {
        if (draw->key == entry->key) {
            break;
        }
        index++;
        entry++;
    }
    if (index >= 2) {
        return 0;
    }

    index = 1 - index;
    divisor = 1;
    for (place = 0; place < index; place++) {
        divisor *= 10;
    }

    value = *context->value;
    value /= divisor;
    tens = value / 10 * 10;
    value -= tens;
    msgctrlSetValue(0x34, value);
    GSmsgPrintRight(0, 0, draw->x, draw->y, -1, 0xc9);
    return 0;
}
