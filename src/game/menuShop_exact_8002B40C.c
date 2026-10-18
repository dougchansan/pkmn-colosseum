#include "dolphin/types.h"

extern u8 lbl_802E4F68[];
extern f32 lbl_8047B984;

typedef struct ShopAngleEntry {
    s32 key;
    s16 position;
    u16 padding;
} ShopAngleEntry;

typedef struct ShopAngleContext {
    u16* state;
    u8 pad_4[8];
    f32* offset;
    u8 pad_10[4];
    s32* offset_enabled;
} ShopAngleContext;

typedef struct ShopAngleOwner {
    u8 pad_0[0x60];
    ShopAngleContext* context;
    u8 pad_64[0x30];
    u16 sprite_id;
} ShopAngleOwner;

typedef struct ShopAngleDrawData {
    u8 pad_0[6];
    s16 key;
    u8 pad_8[0x4A];
    s16 position;
    u8 pad_54[0x1C];
    f32 angle;
} ShopAngleDrawData;

/* RULE-EXCEPTION(user-approved): retained compiler-control pragmas for this exact function; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B40C(ShopAngleOwner* owner, ShopAngleDrawData* draw) {
    u16 sprite_id;
    s8 sprite_bytes[8];
    ShopAngleContext* context;
    ShopAngleEntry* entry;
    s32 index;
    s32 key;
    s32 position;
    s8 sprite_low;
    f32 angle;

    sprite_id = owner->sprite_id;
    context = owner->context;
    *(u16*)sprite_bytes = sprite_id;
    key = draw->key;
    entry = (ShopAngleEntry*)lbl_802E4F68;
    index = 0;
    while (index < 5) {
        if (key == entry->key) {
            break;
        }
        entry++;
        index++;
    }

    if (index >= 5) {
        entry = NULL;
    } else {
        entry = &((ShopAngleEntry*)lbl_802E4F68)[index];
    }
    if (entry == NULL) {
        return 0;
    }

    sprite_low = sprite_bytes[1];
    position = entry->position + sprite_low * 0x1F;
    draw->position = position;
    if (*context->offset_enabled == 0) {
        draw->position += (s16)*context->offset;
    }

    position = (s32)*context->offset + (sprite_low + sprite_bytes[0]) * 0x1F;
    angle = lbl_8047B984 * (f32)position;
    while (angle > 3.1415927f) {
        angle -= 6.2831855f;
    }
    while (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    draw->angle = angle;
    return 0;
}
#pragma pop
