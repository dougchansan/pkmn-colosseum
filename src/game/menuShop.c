/**
 * @file menuShop.c
 * @brief In-game shop menu: item list draw/scroll, purchase flow, story-flag
 *        gated availability, and travel-dialog state machine helpers that
 *        share this address range.
 *
 * Split from the former game/gs_worldmap.c CodeCandidate bucket
 * (0x80026370-0x80030170); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit (0x80029850-0x8002DD24). This
 * range was originally mislabeled as world-map code; it is actually the
 * XD-era menuShop.cpp translation unit. The naming pass for this segment
 * did not complete, so most functions remain fn_-named.
 */

#include "dolphin/types.h"

/* Cross-TU: declared file-scope in game/menuNameEntry.c (the segment this
 * file was split from originally shared one translation unit with); redeclared
 * here since fn_8002BE08 and fn_8002C014 in this file use it without a local
 * block-scope extern of their own. */
extern u16* windowGetKeyInfo(void);
extern u32 GSmsgGetRect(u32 id);

/* Linked single-function islands include this file with one of these
 * defined to build just that function:
 *   menuShop_candidate_8002A5B0.c -> fn_8002A618
 *   menuShop_candidate_8002AB00.c -> fn_8002AB00
 *   menuShop_candidate_8002AE9C.c -> fn_8002AE9C, fn_8002AEF8 */
#if defined(MENUSHOP_CANDIDATE_8002A5B0_ONLY) || \
    defined(MENUSHOP_CANDIDATE_8002AB00_ONLY) || \
    defined(MENUSHOP_CANDIDATE_8002AE9C_ONLY) || \
    defined(MENUSHOP_ISLAND_8002BCE8_ONLY) || \
    defined(MENUSHOP_ISLAND_8002B880_ONLY) || \
    defined(MENUSHOP_ISLAND_8002CE6C_ONLY) || \
    defined(MENUSHOP_ISLAND_80029850_ONLY) || \
    defined(MENUSHOP_ISLAND_80029EF4_ONLY)
#define MENUSHOP_ISLAND_ONLY
#endif

#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_80029850_ONLY)

/* fn_80029850 - 0x80029850 | size: 0x8c */
extern u16 itemBiosGetItemDataId(void*);
extern u16 itemBiosGetNum(void*);
#if 0
asm void fn_80029850(void) {
#include "src/game/gs_worldmap_fn_80029850.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
u32 fn_80029850(u8* slot, u16 count, u16 item_id, u16 maximum) {
    u32 space;
    s32 i;
    u16 id;

    space = 0;
    i = 0;
    while (i < count) {
        id = itemBiosGetItemDataId(slot);
        if (id == item_id) {
            space += (u16)(maximum - itemBiosGetNum(slot));
        } else if (id == 0) {
            space += maximum;
        }
        i++;
        slot += 4;
    }
    return space;
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_80029850_ONLY) || defined(MENUSHOP_ISLAND_80029EF4_ONLY)
typedef struct ShopItemSlot {
    u16 item_id;
    u16 quantity;
} ShopItemSlot;

void itemBiosSetNum(void*, u16);

typedef struct ShopInventory {
    ShopItemSlot primary[235];
    ShopItemSlot secondary[235];
    s32 currency;
    u32 field_75C;
    u8 modified;
    u8 pad_761[3];
    u32 field_764;
    u16 count;
} ShopInventory;

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_80029850_ONLY)
/* fn_800298DC - 0x800298DC | size: 0x1ec */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 4; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma peephole off
static inline s32 shopAddToSlot(ShopItemSlot* slots, s32 count, s32 item_id, s32 quantity,
                                s16 index, s32 maximum) {
    u16 current_id;
    u16 current_quantity;
    u16 capacity;
    u16 added;

    if (index < 0 || index >= (u16)count) {
        return (u16)quantity;
    }
    slots += index;
    current_id = itemBiosGetItemDataId(slots);
    if (current_id != (u16)item_id && current_id != 0) {
        return (u16)quantity;
    }
    if (current_id == 0) {
        itemBiosSetItemDataId(slots, item_id);
        current_quantity = 0;
    } else {
        current_quantity = itemBiosGetNum(slots);
    }
    capacity = (u16)(maximum - current_quantity);
    added = (capacity < (u16)quantity) ? capacity : (u16)quantity;
    itemBiosSetNum(slots, (u16)(current_quantity + added));
    return (u16)((u16)quantity - added);
}

/* The explicit-index path: same steps as shopAddToSlot, but the incoming
 * quantity is only narrowed where it is compared, and the amount added
 * reuses the maximum (it is not needed again). */
static inline s32 shopAddToSlotOnce(ShopItemSlot* slots, s32 count, s32 item_id,
                                    s32 quantity, s16 index, s32 maximum) {
    u16 current_id;
    u16 current_quantity;
    u16 capacity;

    if (index < 0 || index >= (u16)count) {
        return (u16)quantity;
    }
    slots += index;
    current_id = itemBiosGetItemDataId(slots);
    if (current_id != (u16)item_id && current_id != 0) {
        return (u16)quantity;
    }
    if (current_id == 0) {
        itemBiosSetItemDataId(slots, item_id);
        current_quantity = 0;
    } else {
        current_quantity = itemBiosGetNum(slots);
    }
    capacity = (u16)(maximum - current_quantity);
    if (capacity < (u16)quantity) {
        maximum = capacity;
    } else {
        maximum = quantity;
    }
    maximum &= 0xFFFF;
    itemBiosSetNum(slots, (u16)(current_quantity + maximum));
    return (u16)(quantity - maximum);
}

/* The shop's own copy of fn_800298DC: quantity and maximum are u16, so a
 * slot step narrows the running quantity once on entry. */
static inline s32 shopFillSlot(ShopItemSlot* slots, u16 count, s32 item_id,
                               u16 quantity, s16 index, u16 maximum) {
    u16 current_id;
    u16 current_quantity;
    u16 capacity;
    u16 added;

    if (index < 0 || index >= count) {
        return quantity;
    }
    slots += index;
    current_id = itemBiosGetItemDataId(slots);
    if (current_id != (u16)item_id && current_id != 0) {
        return quantity;
    }
    if (current_id == 0) {
        itemBiosSetItemDataId(slots, item_id);
        current_quantity = 0;
    } else {
        current_quantity = itemBiosGetNum(slots);
    }
    capacity = maximum - current_quantity;
    added = (capacity < quantity) ? capacity : quantity;
    itemBiosSetNum(slots, current_quantity + added);
    return (u16)(quantity - added);
}

/* RULE-EXCEPTION(user-approved): inline copy of the real fn_800298DC used by fn_80029AC8/fn_80029CC0 — see docs/RULE_EXCEPTIONS.md */
static inline u16 shopAddItem(ShopItemSlot* slots, u16 count, s32 item_id,
                              s32 quantity, s16 index, u16 maximum) {
    s32 i;

    if (index < -1 || index >= count) {
        return quantity;
    }
    if (index != -1) {
        return shopFillSlot(slots, count, item_id, quantity, index, maximum);
    }
    quantity = (u16)quantity;
    for (i = 0; i < count && quantity > 0; i++) {
        quantity = (u16)quantity;
        quantity = shopFillSlot(slots, count, item_id, quantity, i, maximum);
    }
    return quantity;
}

/* RULE-EXCEPTION(user-approved): second inline copy of fn_800298DC, taking the
 * item id as u16, used only by fn_80029CC0 — see docs/RULE_EXCEPTIONS.md */
static inline u16 shopAddItemId(ShopItemSlot* slots, u16 count, u16 item_id,
                              s32 quantity, s16 index, u16 maximum) {
    s32 i;

    if (index < -1 || index >= count) {
        return quantity;
    }
    if (index != -1) {
        return shopFillSlot(slots, count, item_id, quantity, index, maximum);
    }
    quantity = (u16)quantity;
    for (i = 0; i < count && quantity > 0; i++) {
        quantity = (u16)quantity;
        quantity = shopFillSlot(slots, count, item_id, quantity, i, maximum);
    }
    return quantity;
}

s32 fn_800298DC(ShopItemSlot* slots, s32 count, s32 item_id, s32 quantity,
                 s16 index, s32 maximum) {
    s32 i;

    if (index < -1 || index >= (u16)count) {
        return (u16)quantity;
    }
    if (index != -1) {
        return shopAddToSlotOnce(slots, count, item_id, quantity, index, maximum);
    }
    quantity &= 0xFFFF;
    maximum = (u16)maximum;
    for (i = 0; i < (u16)count && quantity > 0; i++) {
        quantity = shopAddToSlot(slots, count, item_id, quantity, i, maximum);
    }
    return quantity;
}
#pragma pop

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY)
/* fn_80029AC8 - 0x80029AC8 | size: 0x1f8 */
#if 0
asm void fn_80029AC8(void) {
#include "src/game/gs_worldmap_fn_80029AC8.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_80029AC8(s32 price, s32 item_id, s32 quantity, ShopInventory* inventory) {
    s32 count;

    if (inventory == NULL) {
        return;
    }
    count = inventory->count;
    shopAddItem(inventory->primary, count, item_id, quantity, -1, 999);
    shopAddItem(inventory->secondary, count, item_id, quantity, -1, 999);
    inventory->currency -= price;
    inventory->modified = 1;
}
#pragma pop
#endif

/* fn_80029CC0 - 0x80029CC0 | size: 0x234 */
extern void fn_80142A88(void*, s32);
extern s32 fn_800849B4(s32, s32, s32, void*);
typedef struct WorldMapEntry {
    u16 id;
    u16 qty;
} WorldMapEntry;
typedef struct WorldMapBuf {
    u32 a;
    u32 b;
    u32 c;
    u16 d;
    u16 count;
    WorldMapEntry items[48];
} WorldMapBuf;
#if 0
asm void fn_80029CC0(void) {
#include "src/game/gs_worldmap_fn_80029CC0.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
#pragma scheduling on
s32 fn_80029CC0(u8* r30) {
    WorldMapBuf buf;
    s32 i;

    fn_80142A88(r30, 0xeb);
    fn_80142A88(r30 + 0x3ac, 0xeb);
    *(u32*)(r30 + 0x758) = 0;
    *(u32*)(r30 + 0x75c) = 0;
    if (fn_800849B4(0, 0x40, 0, &buf) < 0) {
        return 0;
    }
    for (i = 0; i < buf.count; i++) {
        if (buf.items[i].id != 0) {
            shopAddItemId((ShopItemSlot*)r30, buf.count, buf.items[i].id, buf.items[i].qty, i, 999);
        }
    }
    *(u32*)(r30 + 0x758) = *(u32*)((u8*)&buf + 0);
    *(u32*)(r30 + 0x75c) = *(u32*)((u8*)&buf + 4);
    *(u8*)(r30 + 0x760) = 0;
    *(u32*)(r30 + 0x764) = *(u32*)((u8*)&buf + 8);
    *(u16*)(r30 + 0x768) = buf.count;
    return 1;
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_80029EF4_ONLY)
#if defined(MENUSHOP_ISLAND_80029EF4_ONLY)
void fn_80029AC8(s32 price, s32 item_id, s32 quantity, ShopInventory* inventory);
#endif
/* fn_80029EF4 - 0x80029EF4 | size: 0xb8 */
extern void heroDecPokecoupon(s32, void*);
extern void pcboxDelItem(s32, s32, u16);
extern void heroItemAddItemDataId(s32, s32, u16, s32);
#if 0
asm void fn_80029EF4(void) {
#include "src/game/gs_worldmap_fn_80029EF4.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80029EF4(void* price, s32 item_id, s32 quantity, s32 currency, ShopInventory* inventory) {
    switch ((u8)currency) {
    case 2:
        heroDecPokecoupon(0, price);
        pcboxDelItem(0, item_id, quantity);
        if (inventory != NULL) {
            inventory->modified = 1;
        }
        break;
    case 3:
        fn_80029AC8((s32)price, item_id, quantity, inventory);
        break;
    default:
        heroDecPokecoupon(0, price);
        heroItemAddItemDataId(0, item_id, quantity, -1);
        break;
    }
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY)
/* fn_80029FAC - 0x80029FAC | size: 0x10c | WALL 97%: slwi scheduling */
extern void* __va_arg(void*, s32);
extern u32 lbl_80478E54;
extern u32 lbl_80478E4C;
typedef struct WorldMapVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} WorldMapVaList;
typedef WorldMapVaList WorldMapVaListArray[1];
#if 0
asm void fn_80029FAC(void) {
#include "src/game/gs_worldmap_fn_80029FAC.inc"
}
#else
u32 fn_80029FAC(u8* r3, s32 r4, s32 r5, s32 r6, ...) {
    WorldMapVaListArray list;
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
    map = (u8*)((((((((((((lbl_80478E54 & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu);
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
#endif

/* fn_8002A0B8 - 0x8002A0B8 | size: 0x10c | WALL 97%: slwi scheduling */
extern u32 lbl_80478E54;
extern u32 lbl_80478E3C;
#if 0
asm void fn_8002A0B8(void) {
#include "src/game/gs_worldmap_fn_8002A0B8.inc"
}
#else
u32 fn_8002A0B8(u8* r3, s32 r4, s32 r5, s32 r6, ...) {
    WorldMapVaListArray list;
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
    map = (u8*)(((((((lbl_80478E54 & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu) & 0xFFFFFFFFFFFFFFFFu);
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
#endif

/* fn_8002A1C4 - 0x8002A1C4 | size: 0x108 | WALL 97%: slwi scheduling */
extern void winMsgOpenWithSE(s32, u32, s32, s32, u8);
extern u32 lbl_80478E54;
extern u32 lbl_80478E4C;
#if 0
asm void fn_8002A1C4(void) {
#include "src/game/gs_worldmap_fn_8002A1C4.inc"
}
#else
void fn_8002A1C4(u8* r3, s32 r4, s32 r5, ...) {
    WorldMapVaListArray list;
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
#endif

/* fn_8002A2CC - 0x8002A2CC | size: 0x108 | WALL 97%: slwi scheduling */
extern u32 lbl_80478E54;
extern u32 lbl_80478E3C;
#if 0
asm void fn_8002A2CC(void) {
#include "src/game/gs_worldmap_fn_8002A2CC.inc"
}
#else
void fn_8002A2CC(u8* r3, s32 r4, s32 r5, ...) {
    WorldMapVaListArray list;
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
#endif

/* fn_8002A3D4 - 0x8002A3D4 | size: 0x2c */
#if 0
asm void fn_8002A3D4(void) {
#include "src/game/gs_worldmap_fn_8002A3D4.inc"
}
#else
s32 fn_8002A3D4(void* r3, u8* r4) {
    void* ctx;
    ctx = *(void**)((u8*)r3 + 0x60);
    r4[0x64] = ((u8*)ctx)[0x10];
    r4[0x65] = ((u8*)ctx)[0x11];
    r4[0x66] = ((u8*)ctx)[0x12];
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_8002A400 - 0x8002A400 | size: 0x8c */
#if 0
asm void fn_8002A400(void) {
#include "src/game/gs_worldmap_fn_8002A400.inc"
}
#else
s32 fn_8002A400(void* r3, u8* r4) {
    u8* r30;
    void* r31;
    u32 id;
    u32 ret;
    r30 = r4;
    r31 = *(void**)((u8*)r3 + 0x60);
    msgctrlSetValue(0x50, (void*)(*(s32*)((u8*)r31 + 0x8) * *(s32*)(*(u32*)((u8*)r31 + 0xc))));
    if (*(s32*)((u8*)r31 + 0x14) != 0) {
        id = 0x153;
    } else {
        id = 0x151;
    }
    ret = GSmsgGetRect(id);
    fn_800FB680((s32)*(s16*)(r30 + 0x54) - (s32)(ret >> 16), 0, -1, id);
    return 0;
}
#endif

/* fn_8002A48C - 0x8002A48C | size: 0x124 */
extern void fn_800FB8C8(s32, s32, s16, s16, s32, s32);
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

extern u8 lbl_80266E58[];
#if 0
asm void fn_8002A48C(void) {
#include "src/game/gs_worldmap_fn_8002A48C.inc"
}
#else
s32 fn_8002A48C(ShopMenuOwner* owner, ShopDrawData* draw) {
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
    fn_800FB8C8(0, 0, draw->x, draw->y, -1, 0xC9);
    return 0;
}
#endif

/* fn_8002AA68 - 0x8002AA68 | size: 0x98 */
#if 0
asm void fn_8002AA68(void) {
#include "src/game/gs_worldmap_fn_8002AA68.inc"
}
#endif
#endif /* !MENUSHOP_ISLAND_ONLY */

/* fn_8002A618 - 0x8002A618 | size: 0x450 */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_CANDIDATE_8002A5B0_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8002A618(u8* self)
{
    typedef struct ShopNumberContext {
        s32 minimum;
        s32 maximum;
        u32 unused;
        s32* value;
    } ShopNumberContext;
    extern void soundStart(u32, u32, u32, u32);

    ShopNumberContext* context;
    u16* keyInfo;
    s32 decimalPlace;
    s32 factor;
    s32 keys;
    s32 i;
    s32 oldValue;
    s32 value;

    context = *(ShopNumberContext**)(self + 0x60);
    keyInfo = windowGetKeyInfo();
    keys = keyInfo[3];
    if ((keys & 0xF) != 0) {

    decimalPlace = 1 - (s8)self[0x95];
    factor = 1;
    for (i = 0; i < decimalPlace; i++) {
        factor *= 10;
    }

    if ((keys & 1) != 0) {
        oldValue = *context->value;
        if (factor < 10) {
            *context->value = oldValue + factor;
            if (*context->value > context->maximum) {
                *context->value = context->minimum;
            }
        } else {
            s32 quotient;
            s32 digit;
            s32 maximumDigit;
            s32 remainder;
            s32 nextDigit;

            quotient = oldValue / factor;
            digit = quotient % 10;
            remainder = oldValue - digit * factor;
            for (maximumDigit = 9; maximumDigit >= 0; maximumDigit--) {
                if (remainder + maximumDigit * factor <= context->maximum) {
                    break;
                }
            }
            nextDigit = digit + 1;
            if (nextDigit > maximumDigit) {
                nextDigit = 0;
            }
            if ((*context->value = remainder + nextDigit * factor) < context->minimum) {
                *context->value = context->minimum;
            }
        }
        if (oldValue != *context->value) {
            soundStart(0x23, 0, 0xFF, 0);
        }
    }

    if ((keyInfo[3] & 2) != 0) {
        oldValue = *context->value;
        if (factor < 10) {
            value = oldValue - factor;
            *context->value = value;
            if (value < context->minimum) {
                *context->value = context->maximum;
            }
        } else {
            s32 quotient;
            s32 digit;
            s32 remainder;
            s32 maximumDigit;
            s32 nextDigit;

            quotient = oldValue / factor;
            digit = quotient % 10;
            remainder = oldValue - digit * factor;
            for (maximumDigit = 9; maximumDigit >= 0; maximumDigit--) {
                if (remainder + maximumDigit * factor <= context->maximum) {
                    break;
                }
            }
            nextDigit = digit - 1;
            if (nextDigit < 0) {
                nextDigit = maximumDigit;
            }
            if ((*context->value = remainder + nextDigit * factor) < context->minimum) {
                *context->value = context->minimum;
            }
        }
        if (oldValue != *context->value) {
            soundStart(0x23, 0, 0xFF, 0);
        }
    }

    if ((keyInfo[3] & 8) != 0) {
        if ((s8)++self[0x95] >= 2) {
            self[0x95] = 1;
        }
    }
    if ((keyInfo[3] & 4) != 0) {
        if ((s8)--self[0x95] < 0) {
            self[0x95] = 0;
        }
    }
    }
    return 0;
}
#pragma pop
#endif

#if !defined(MENUSHOP_ISLAND_ONLY)
#if 0
s32 fn_8002AA68(void* r3) {
    u8* r31;
    s8 state;
    r31 = (u8*)r3;
    state = (s8)r31[1];
    switch (state) {
    case 0:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu((void*)0x61, 0x7e);
            r31[2] = 1;
        }
        break;
    case 3:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu((void*)0x61, 0x82);
            r31[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

#endif /* !MENUSHOP_ISLAND_ONLY */

/* fn_8002AB00 - 0x8002AB00 | size: 0x40 */
extern const u8 lbl_80266E70[];
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_CANDIDATE_8002AB00_ONLY)
s32 fn_8002AB00(void* r3, u8* r4) {
    u8* ctx;
    const u8* base;
    s32 off;

    ctx = *(u8**)((u8*)r3 + 0x60);
    base = lbl_80266E70;
    off = ctx[0x1c] * 3;
    r4[0x64] = base[off];
    base += off;
    r4[0x65] = base[1];
    r4[0x66] = base[2];
    r4[0x67] = 0xff;
    return 0;
}
#endif

#if !defined(MENUSHOP_ISLAND_ONLY)
#if 0
/* fn_8002AB40 - 0x8002AB40 | size: 0x178 */
extern u8 lbl_80266E80[];
extern u32 lbl_804788F0;
extern u8 lbl_802E61D8[];
#if 0
asm void fn_8002AB40(void) {
#include "src/game/gs_worldmap_fn_8002AB40.inc"
}
#else
s32 fn_8002AB40(void* r3, u8* r4) {
    u8* ctx;
    u32 table[4];
    u32 value;
    s32 idx;
    u32* limit;

    ctx = *(u8**)((u8*)r3 + 0x60);
    table[0] = *(u32*)(lbl_80266E80 + 0x0);
    table[1] = *(u32*)(lbl_80266E80 + 0x4);
    table[2] = *(u32*)(lbl_80266E80 + 0x8);
    table[3] = *(u32*)(lbl_80266E80 + 0xC);

    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }

    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        r4[0x67] = 0;
        return 0;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)heroGetStatus(0, 0xE, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            value = *(u32*)(ctx + 0x77C);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)heroGetStatus(0, 0xE, 0);
        break;
    }

    idx = lbl_804788F0 - 1;
    limit = (u32*)(lbl_802E61D8 + idx * 4);
    while (idx >= 0) {
        if (*limit <= value) {
            break;
        }
        limit--;
        idx--;
    }
    if (idx < 0) {
        idx = 0;
    }
    if (idx >= 4) {
        idx = 3;
    }

    if ((s32)*(s16*)(r4 + 0x6) == (s32)table[idx]) {
        r4[0x67] = 0xFF;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002ACB8 - 0x8002ACB8 | size: 0x18c */
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
#if 0
asm void fn_8002ACB8(void) {
#include "src/game/gs_worldmap_fn_8002ACB8.inc"
}
#else
s32 fn_8002ACB8(void* r3, u8* r4) {
    u8* ctx;
    u32 value;
    u32 text_id;
    s32 x;

    ctx = *(u8**)((u8*)r3 + 0x60);

    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }

    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        msgctrlSetValue(0x50, heroGetStatus(0, 0xC, 0));
        text_id = 0x151;
        x = (s32)*(s16*)(r4 + 0x54) - (s32)(s16)(GSmsgGetRect(text_id) >> 16);
        fn_800FB680(x, 0, -1, text_id);
        goto done;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)heroGetStatus(0, 0xD, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            if ((s32)*(volatile u32*)&lbl_8047A660 > 0) {
                *(u32*)(ctx + 0x778) += *(volatile u32*)&lbl_8047A660;
                *(u32*)(ctx + 0x77C) += *(volatile u32*)&lbl_8047A660;
                lbl_8047A660 = 0;
            }
            if ((s32)lbl_8047A664 > 0) {
                *(u32*)(ctx + 0x778) = 0;
                *(u32*)(ctx + 0x77C) = 0;
                lbl_8047A664 = 0;
            }
            value = *(u32*)(ctx + 0x778);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)heroGetStatus(0, 0xD, 0);
        break;
    }

    msgctrlSetValue(0x50, (void*)value);
    text_id = 0x153;
    x = (s32)*(s16*)(r4 + 0x54) - (s32)(s16)(GSmsgGetRect(text_id) >> 16);
    fn_800FB680(x + 6, 0, -1, text_id);
done:
    return 0;
}
#endif

/* fn_8002AE44 - 0x8002AE44 | size: 0x24 */
#if 0
asm void fn_8002AE44(void) {
#include "src/game/gs_worldmap_fn_8002AE44.inc"
}
#else
s32 fn_8002AE44(void* r3, u8* r4) {
    void* ctx;
    ctx = *(void**)((u8*)r3 + 0x60);
    if (((u8*)ctx)[0x1d] & 1) {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AE68 - 0x8002AE68 | size: 0x34 */
#if 0
asm void fn_8002AE68(void) {
#include "src/game/gs_worldmap_fn_8002AE68.inc"
}
#else
s32 fn_8002AE68(void* r3, u8* r4) {
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        r4[0x67] = 0xcc;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif
#endif

#endif /* !MENUSHOP_ISLAND_ONLY */

/* Item `index` of the shop list in `ctx`, or 0 when out of range. */
static inline u32 shopListItem(u8* ctx, s32 index) {
    if (index < 0 || index >= *(s32*)(ctx + 0x8)) {
        return 0;
    }
    return (*(u16**)(ctx + 0x4))[index];
}

#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_CANDIDATE_8002AE9C_ONLY)
/* fn_8002AE9C - 0x8002AE9C | size: 0x5c */
extern const u8 lbl_80266E70[];
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
s32 fn_8002AE9C(void* r3, u8* r4) {
    u8* ctx;
    const u8* base;
    s32 off;
    u8 mode;

    ctx = *(u8**)((u8*)r3 + 0x60);
    mode = ctx[0x1c];
    if (mode == 0 || mode == 1) {
        base = lbl_80266E70;
        off = mode * 3;
        r4[0x64] = base[off];
        base += off;
        r4[0x65] = base[1];
        r4[0x66] = base[2];
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}

/* fn_8002AEF8 - 0x8002AEF8 | size: 0x144 */
extern void itemDataBiosGetPtr(u32);
extern u32 itemDataBiosGetKind(void);
extern u32 heroItemGetItemKindToItemAryPtr(s32, u32, u16*, s32, s32, s32);
extern u32 itemGetStatus(u32, s32, s32, s32);

/* Total quantity of `item` across the hero's inventory pocket for its kind. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8002AEF8) — see docs/RULE_EXCEPTIONS.md */
static inline s32 shopCountOwned(u32 item) {
    u32 entry;
    s32 i;
    s32 owned;
    u16 entryCount;
    u16 id;

    owned = 0;
    if ((u16)item == 0) {
        return owned;
    }
    itemDataBiosGetPtr(item);
    entry = heroItemGetItemKindToItemAryPtr(0, itemDataBiosGetKind(), &entryCount, 0, 0, 0);
    for (i = 0; i < entryCount; i++, entry += 4) {
        id = itemGetStatus(entry, 0, 0x1b, 0);
        if (id == (u16)item) {
            owned += itemGetStatus(entry, 0, 0x1c, 0);
        }
    }
    return owned;
}

s32 fn_8002AEF8(void* r3, u8* r4) {
    u8* ctx;
    u32 item;
    s32 owned;

    ctx = *(u8**)((u8*)r3 + 0x60);
    if (ctx[0x1c] == 0 || ctx[0x1c] == 1) {
        item = shopListItem(ctx, (s8)((u8*)r3)[0x95] + (s8)((u8*)r3)[0x94]);
        if ((u16)item != 0) {
            owned = shopCountOwned(item);
            msgctrlSetValue(0x2d, (void*)(u32)(u16)item);
            msgctrlSetValue(0x34, (void*)owned);
            fn_800FB680(0, 0, -1, 0x2b2f);
        }
    }
    return 0;
}
#pragma pop
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif

#if !defined(MENUSHOP_ISLAND_ONLY)

#if 0
/* fn_8002B03C - 0x8002B03C | size: 0x4c */
#if 0
asm void fn_8002B03C(void) {
#include "src/game/gs_worldmap_fn_8002B03C.inc"
}
#else
s32 fn_8002B03C(void* r3) {
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        fn_800FB680(0, 0, -1, 0x2b2e);
    }
    return 0;
}
#endif

/* fn_8002B088 - 0x8002B088 | size: 0x34 */
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002B088(void) {
#include "src/game/gs_worldmap_fn_8002B088.inc"
}
#else
s32 fn_8002B088(void) {
    fn_800FB680(0, 0, -1, lbl_8047A3E4);
    return 0;
}
#endif

/* fn_8002B0BC - 0x8002B0BC | size: 0x78 */
extern const f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern const f32 lbl_8047B978;
#if 0
asm void fn_8002B0BC(void) {
#include "src/game/gs_worldmap_fn_8002B0BC.inc"
}
#else
s32 fn_8002B0BC(void* r3, u8* r4) {
    u16 hv;
    u8* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(u8**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] + 0xa < *(s32*)(ctx + 0x8) + 1) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}
#endif

/* fn_8002B134 - 0x8002B134 | size: 0x6c */
extern const f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern const f32 lbl_8047B978;
#if 0
asm void fn_8002B134(void) {
#include "src/game/gs_worldmap_fn_8002B134.inc"
}
#else
s32 fn_8002B134(void* r3, u8* r4) {
    u16 hv;
    void* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(void**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] > 0) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}
#endif
#endif

/* menuShopDrawListText - 0x8002B1A0 | size: 0x26c */
extern void fn_800FE38C(void);
extern u32 itemDataBiosGetName(void);
extern u32 itemDataBiosGetPrice(void);
extern u32 itemDataBiosGetCoupon(void);
extern void fn_800FE35C(void);
extern const f32 lbl_8047B980;
#if 0
asm void menuShopDrawListText(void) {
#include "src/game/gs_worldmap_menuShopDrawListText.inc"
}
#else
/*
 * GSmap_DrawWeatherOverlay  0x8002B1A0  size: 0x26C
 *
 * Draws the scrollable slot list overlay on the world-map screen.
 * arg0 = UI/map object (this), arg1 = sprite descriptor for layout info.
 * Iterates up to r23 visual columns, skipping slots outside [0, ctx->count),
 * drawing a header icon and either a normal or alternate text string per slot,
 * then draws a trailing "add" or "locked" button if visual space remains.
 */
#pragma push
#pragma peephole off
s32 menuShopDrawListText(void* arg0, u8* arg1)
{
    extern void fn_800FE38C(s32, s32, s32, s32);
    extern u32 itemDataBiosGetName(void);
    extern u32 itemDataBiosGetPrice(void);
    extern u32 itemDataBiosGetCoupon(void);
    extern void fn_800FE35C(void);
    extern const f32 lbl_8047B980;

    extern void msgctrlSetValue(s32, void*);
    extern u32 GSmsgGetRect(u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    extern void itemDataBiosGetPtr(u32);

    extern u8 lbl_802EF0A8[];

    u8* tbl;
    u8* entry;
    u8* ctx;
    s32 slot_count;
    s32 slot_i;
    s32 loop_lim;
    s32 dir_off;
    s32 scroll_px;
    s32 x_mid;
    f32 scroll_f;
    s32 x_acc;
    s32 loop_i;
    s32 x_pos;
    u32 slot_id;
    u32 icon;
    u32 value;
    u8 mode;

    tbl = lbl_802EF0A8;
    ctx = *(u8**)((u8*)arg0 + 0x60);
    entry = tbl + *(s16*)(arg1 + 0x6) * 0x1c;
    dir_off = 0;
    loop_lim = 10;
    scroll_px = 0;

    /* draw the border rect */
    fn_800FE38C(*(s16*)(tbl + 0x492e) - *(s16*)(entry + 0x2),
                *(s16*)(tbl + 0x4930) - *(s16*)(entry + 0x4),
                *(s16*)(tbl + 0x4932), *(s16*)(tbl + 0x4934));

    slot_i = (s8)((u8*)arg0)[0x94];
    slot_count = *(s32*)(ctx + 0x8);
    msgctrlSetValue(0x50, (void*)0x270f);
    x_mid = GSmsgGetRect(0xdb) >> 16;
    x_mid = *(s16*)(arg1 + 0x54) - x_mid - (GSmsgGetRect(0x14f) >> 16);

    /* scroll animation */
    scroll_f = **(f32**)(ctx + 0xc);
    if (lbl_8047B980 != scroll_f && **(s32**)(ctx + 0x14) != 0) {
        if (scroll_f < lbl_8047B980) {
            slot_i--;
            dir_off = -1;
        } else {
            loop_lim = 11;
        }
        scroll_px = (s32)scroll_f;
    }

    x_acc = dir_off * 0x1f;
    for (loop_i = dir_off; loop_i < loop_lim && slot_i < slot_count;
         x_acc += 0x1f, loop_i++, slot_i++) {
        if (slot_i < 0) {
            continue;
        }

        x_pos = x_acc - scroll_px;
        slot_id = shopListItem(ctx, slot_i);

        itemDataBiosGetPtr(slot_id);
        icon = itemDataBiosGetName();
        if (icon != 0) {
            fn_800FB680(0, x_pos, -1, icon);
        }

        mode = ctx[0x1c];
        if (mode == 0 || mode == 1) {
            fn_800FB680(x_mid, x_pos, -1, 0x14f);
            itemDataBiosGetPtr(slot_id);
            value = itemDataBiosGetPrice();
            msgctrlSetValue(0x50, (void*)(u16)value);
            fn_800FB680(*(s16*)(arg1 + 0x54) - (GSmsgGetRect(0xdb) >> 16),
                        x_pos, -1, 0xdb);
        } else {
            itemDataBiosGetPtr(slot_id);
            value = itemDataBiosGetCoupon();
            msgctrlSetValue(0x50, (void*)(u16)value);
            fn_800FB680(*(s16*)(arg1 + 0x54) - (GSmsgGetRect(0x153) >> 16),
                        x_pos, -1, 0x153);
        }
    }

    /* trailing "add"/"locked" button if visual columns remain */
    if (loop_i < loop_lim) {
        fn_800FB680(0, loop_i * 0x1f - scroll_px, -1,
                    (ctx[0x1d] & 1) ? 0x2b47 : 0x2b2c);
    }

    fn_800FE35C();
    return 0;
}
#pragma pop
#endif

/* fn_8002B40C - 0x8002B40C | size: 0x188 */
extern u8 lbl_802E4F68[];
extern f64 lbl_8047B998;
extern f32 lbl_8047B984;
extern f32 lbl_8047B988;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B990;
#if 0
asm void fn_8002B40C(void) {
#include "src/game/gs_worldmap_fn_8002B40C.inc"
}
#else
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
#endif

/* menuShopDrawMarqueeDot - 0x8002B594 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern const f32 lbl_8047B980;
extern const f32 lbl_8047B97C;
extern f64 lbl_8047B998;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B9A0;
extern f32 lbl_8047B9A4;
extern f32 lbl_8047B9A8;
f64 cos(f64);
f64 sin(f64);
void windowDrawSprite2(s32, s32, s32, s32, s32, void*, u32, s32);
#if 0
asm void fn_8002B594(void) {
#include "src/game/gs_worldmap_fn_8002B594.inc"
}
#else
/*
 * menuShopDrawMarqueeDot  0x8002B594 | 0x2EC bytes
 *
 * Draws one marquee dot along the outline of a window item: a stadium-shaped
 * path (two straight sides and two semicircular ends) of the item's size.
 * The curve is divided into 5 segments defined by 5 break-point thresholds
 * computed from the two s16 fields (at +0x54/+0x56) in the data packet.
 * For each segment a normalised fractional parameter f31 is computed and
 * segment-specific x/y screen positions are derived (some use cos/sin arcs).
 * Finally windowDrawSprite2 is called to emit the actual sprite draw call.
 *
 * Parameters (CW EABI):
 *   ctx        r3  - sprite/render context pointer passed through to windowDrawSprite2
 *   data       r4  - sprite data packet; s16 at +0x56 = vert coord, s16 at +0x54 = horiz coord
 *   sprite_id  r5  - sprite index; lower 16 bits passed to windowDrawSprite2
 *   color_byte r6  - low 8 bits = alpha/colour value; OR-merged with 0xFFFFFF00 to form r7
 *   pos        f1  - continuous position parameter along the curve
 *
 * ENDIAN-QA: the asm uses the classic CW big-endian 0x4330/xoris double-word trick
 * to convert the two s16 fields to f32.  On x86 this is a plain (f32)(s16) cast.
 */
#pragma push
#pragma peephole off
void menuShopDrawMarqueeDot(void* ctx, u8* data, u32 sprite_id, u32 color_byte, f32 pos)
{
    f32 thresholds[5];
    f32* next_threshold;
    f32 phase;
    f32 width;
    f32 height;
    f32 scaled_height;
    f32 curve;
    f32 span;
    f32 denominator;
    f32 angle;
    f32 cosine;
    f32 sine;
    s32 color;
    s32 segment;
    s32 x;
    s32 y;
    f32 radial;
    f32 remaining;

    width = (f32)*(s16*)(data + 0x54);
    height = (f32)*(s16*)(data + 0x56);

    scaled_height = lbl_8047B98C * height;
    curve = scaled_height * lbl_8047B9A0;
    thresholds[0] = lbl_8047B980;
    span = width + curve;
    denominator = lbl_8047B9A4 * span;
    next_threshold = &thresholds[1];
    thresholds[4] = lbl_8047B97C;
    next_threshold[0] = width / denominator;
    next_threshold[1] = span / denominator;
    next_threshold[2] = (lbl_8047B9A4 * width + curve) / denominator;

    segment = 0;
    while (segment < 4) {
        if (thresholds[segment] <= pos && next_threshold[segment] > pos) {
            break;
        }
        segment++;
    }

    phase = (pos - thresholds[segment]) /
            (next_threshold[segment] - thresholds[segment]);

    if (segment == 0) {
        x = (s32)(phase * width);
        y = 0;
    }

    if (segment == 1) {
        angle = lbl_8047B98C * phase - lbl_8047B9A8;
        cosine = cos(angle);
        radial = height - lbl_8047B9A4;
        radial *= cosine;
        x = (s32)(radial * lbl_8047B9A0 + width);
        sine = sin(angle);
        radial = height - lbl_8047B9A4;
        radial *= sine;
        radial *= lbl_8047B9A0;
        y = (s32)(height * lbl_8047B9A0 + radial);
    }

    if (segment == 2) {
        remaining = lbl_8047B97C - phase;
        x = (s32)(remaining * width);
        y = (s32)(height - lbl_8047B9A4);
    }

    if (segment == 3) {
        angle = lbl_8047B98C * phase + lbl_8047B9A8;
        cosine = cos(angle);
        radial = height - lbl_8047B9A4;
        radial *= cosine;
        x = (s32)(radial * lbl_8047B9A0);
        sine = sin(angle);
        radial = height - lbl_8047B9A4;
        radial *= sine;
        radial *= lbl_8047B9A0;
        y = (s32)(height * lbl_8047B9A0 + radial);
    }

    color = -0x100;
    color |= (u8)color_byte;
    windowDrawSprite2(x, y, 2, 2, color, ctx, sprite_id & 0xFFFF, 0);
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_8002B880_ONLY)
/* menuShopDrawMarqueeFrame - 0x8002B880 | size: 0x468 */
extern void fn_800FE6D0(void);
extern void spriteSetEnv(void);
extern f64 lbl_8047B998;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B9A0;
extern f32 lbl_8047B9A4;
extern f32 lbl_8047A3F0;
extern const f32 lbl_8047B978;
extern const f32 lbl_8047B9AC;
extern const f32 lbl_8047B97C;
extern f32 lbl_8047B9B0;
extern f32 lbl_8047B9B4;
#if 0
asm void fn_8002B880(void) {
#include "src/game/gs_worldmap_fn_8002B880.inc"
}
#else
/* menuShopDrawMarqueeFrame - 0x8002B880, size 0x468
 *
 * Draw callback for window item 0x0286: an animated marquee border. r3 = worldmap state object (its +0x60 is the
 * panel context), r4 = the draw/sprite entity. Only runs when the context's
 * primary flag word (*(u16*)*(void**)ctx) is zero. It first computes a sprite
 * column/alpha exactly like fn_8002BCE8 (5-entry lbl_802E4F68 lookup table,
 * keyed on entity->0x6), positions the panel via fn_800FE6D0/spriteSetEnv, then
 * draws four 45-step radial rings of icons by calling menuShopDrawMarqueeDot in a loop,
 * each ring starting from a different phase offset and advancing by a fixed
 * per-step increment that wraps at lbl_8047B97C.
 *
 * ENDIAN-QA: all 0x43300000 / 0x8000-xor double-word int->float idioms in the
 * original asm are normalized here to plain signed casts on the full value.
 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma push
#pragma opt_common_subs off
s32 menuShopDrawMarqueeFrame(u8* state, u8* entity)
{
    /* Cross-TU callees (real arg lists inferred from register state at each bl). */
    extern void fn_800FE6D0(s32 x, s32 y);
    extern void spriteSetEnv(void);
    extern void menuShopDrawMarqueeDot(void* panel, u8* entity, u32 mode, s32 step, f32 phase);

    /* r2-relative read-only float constants. */
    extern f64 lbl_8047B998; /* int->float bias double (folds into the casts) */
    extern f32 lbl_8047B98C;
    extern f32 lbl_8047B9A0;
    extern f32 lbl_8047B9A4;
    extern const f32 lbl_8047B978;
    extern const f32 lbl_8047B9AC;
    extern const f32 lbl_8047B97C; /* phase wrap limit */
    extern f32 lbl_8047B9B0;
    extern f32 lbl_8047B9B4;
    /* r13-relative small-data float (running phase base). */
    extern f32 lbl_8047A3F0;
    /* Sprite-column lookup table: 5 entries of 8 bytes; key s32 @ +0, value s16 @ +4. */
    extern u8 lbl_802E4F68[];

    u8* ctx = *(u8**)(state + 0x60);

    f32 scaledY;
    f32 phase;
    f32 incr;
    f32 denom;
    f32 fx;
    f32 fy;
    s32 i;

    /* Bail unless the context's primary flag word is clear. */
    if (*(u16*)(*(void**)ctx) != 0) {
        return 0;
    }

    /* ---- Sprite column / alpha (mirrors fn_8002BCE8) ---- */
    {
        u16 sprite_id;
        u8 sprite_bytes[8];
        s32 index;
        s32 key;
        s32 position;
        u8 alpha;
        u8* entry;

        sprite_id = *(u16*)(state + 0x94);
        *(u16*)sprite_bytes = sprite_id;
        key = *(s16*)(entity + 0x6);
        entry = lbl_802E4F68;
        index = 0;
        while (index < 5) {
            if (key == *(s32*)entry) {
                break;
            }
            entry += 8;
            index++;
        }
        if (index >= 5) {
            entry = NULL;
        } else {
            entry = lbl_802E4F68 + index * 8;
        }
        if (entry != NULL) {
            position = *(s16*)(entry + 4) + (s8)sprite_bytes[1] * 0x1F;
            if (**(s32**)(ctx + 0x14) == 0) {
                position += (s32)**(f32**)(ctx + 0xc);
            }
            if (**(u16**)ctx == 0) {
                alpha = 0x72;
            } else {
                alpha = 0xFF;
            }
            *(s16*)(entity + 0x52) = position;
            entity[0x67] = alpha;
        }
    }

    /* ---- Position the panel ---- */
    fn_800FE6D0((s32)(s16)(*(s16*)(state + 0x84) + *(s16*)(entity + 0x50)),
                (s32)(s16)(*(s16*)(state + 0x86) + *(s16*)(entity + 0x52)));
    spriteSetEnv();

    /* ---- Fixed per-step phase increment ---- */
    fx = (f32)(s16)*(s16*)(entity + 0x54);
    fy = (f32)(s16)*(s16*)(entity + 0x56);
    scaledY = lbl_8047B98C * fy;
    denom = lbl_8047B9A4 * (fx + scaledY * lbl_8047B9A0);
    incr = lbl_8047B9A4 / denom;

    /* ---- Ring 1: phase starts at the live r13 base ---- */
    phase = lbl_8047A3F0;
    for (i = 0; i < 0x2d; i++) {
        menuShopDrawMarqueeDot(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 2: phase starts at base + B9B0 ---- */
    phase = lbl_8047B9B0 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        menuShopDrawMarqueeDot(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 3: phase starts at base + B9A0 ---- */
    phase = lbl_8047B9A0 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        menuShopDrawMarqueeDot(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 4: phase starts at base + B9B4 ---- */
    phase = lbl_8047B9B4 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        menuShopDrawMarqueeDot(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    return 0;
}
#pragma pop
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_8002BCE8_ONLY)
#if defined(MENUSHOP_ISLAND_8002BCE8_ONLY)
/* RULE-EXCEPTION(user-approved): the island states the full file's compile
 * state at this point (optimization_level 4, scheduling on) because its unit
 * builds only these functions — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047B980;
#endif
/* fn_8002BCE8 - 0x8002BCE8 | size: 0x120 */
typedef struct ShopPositionEntry {
    s32 key;
    s16 position;
    u16 padding;
} ShopPositionEntry;

typedef struct ShopPositionContext {
    u16* state;
    u8 pad_4[8];
    f32* offset;
    u8 pad_10[4];
    s32* offset_enabled;
} ShopPositionContext;

typedef struct ShopPositionOwner {
    u8 pad_0[0x60];
    ShopPositionContext* context;
    u8 pad_64[0x30];
    u16 sprite_id;
} ShopPositionOwner;

typedef struct ShopPositionDrawData {
    u8 pad_0[6];
    s16 key;
    u8 pad_8[0x4A];
    s16 position;
    u8 pad_54[0x13];
    u8 alpha;
} ShopPositionDrawData;

extern u8 lbl_802E4F68[];
#if 0
asm void fn_8002BCE8(void) {
#include "src/game/gs_worldmap_fn_8002BCE8.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002BCE8(ShopPositionOwner* owner, ShopPositionDrawData* draw) {
    u16 sprite_id;
    u8 sprite_bytes[8];
    ShopPositionContext* context;
    ShopPositionEntry* entry;
    s32 index;
    s32 key;
    s32 position;
    u8 alpha;

    sprite_id = owner->sprite_id;
    context = owner->context;
    *(u16*)sprite_bytes = sprite_id;
    key = draw->key;
    entry = (ShopPositionEntry*)lbl_802E4F68;
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
        entry = &((ShopPositionEntry*)lbl_802E4F68)[index];
    }
    if (entry == NULL) {
        return 0;
    }

    position = entry->position + (s8)sprite_bytes[1] * 0x1F;
    if (*context->offset_enabled == 0) {
        position += (s32)*context->offset;
    }

    if (*context->state == 0) {
        alpha = 0x72;
    } else {
        alpha = 0xFF;
    }
    draw->position = position;
    draw->alpha = alpha;
    return 0;
}
#pragma pop
#endif

/* fn_8002BE08 - 0x8002BE08 | size: 0x20c */
extern u32 itemDataBiosGetDoc(void);
extern f32 lbl_8047B9B8;
extern f32 lbl_8047B9BC;
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002BE08(void) {
#include "src/game/gs_worldmap_fn_8002BE08.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; scheduling on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma scheduling on
u32 fn_8002BE08(u8* arg0) {
    u8* ctx;
    u16* state;
    s32 sum;
    u32 r3val;
    s32 limit;

    ctx = *(u8**)(arg0 + 0x60);
    state = windowGetKeyInfo();
    if (lbl_8047B980 != *(f32*)(*(u32*)(ctx + 0xc))) {
        return 0;
    }
    limit = *(s32*)(ctx + 0x8) + 1;
    if ((state[2] | state[4]) & 0x2) {
        ++arg0[0x95];
        sum = (s8)arg0[0x95];
        if (sum + (s8)arg0[0x94] >= limit) {
            --arg0[0x95];
        } else {
            if (sum >= 0xa) {
                ++arg0[0x94];
                --arg0[0x95];
                *(s32*)(*(u32*)(ctx + 0x14)) = 1;
            } else {
                *(s32*)(*(u32*)(ctx + 0x14)) = 0;
            }
            *(f32*)(*(u32*)(ctx + 0xc)) = lbl_8047B9B8;
        }
    }
    if ((state[2] | state[4]) & 0x1) {
        if ((s8)arg0[0x95] > 0 || (s8)arg0[0x94] > 0) {
            if ((s8)--arg0[0x95] < 0) {
                    arg0[0x95] = 0;
                    --arg0[0x94];
                    *(s32*)(*(u32*)(ctx + 0x14)) = 1;
                } else {
                    *(s32*)(*(u32*)(ctx + 0x14)) = 0;
                }
            *(f32*)(*(u32*)(ctx + 0xc)) = lbl_8047B9BC;
        }
    }
    r3val = shopListItem(ctx, (s32)(s8)arg0[0x94] + (s32)(s8)arg0[0x95]);
    if ((u16)r3val != 0) {
        itemDataBiosGetPtr(r3val);
        r3val = itemDataBiosGetDoc();
    } else {
        u8 b = ctx[0x1c];
        if (b == 0 || b == 1) {
            r3val = 0x2b2d;
        } else if (ctx[0x1d] & 1) {
            r3val = 0x2b46;
        } else {
            r3val = 0x2b37;
        }
    }
    lbl_8047A3E4 = r3val;
    return 0;
}
#pragma pop
#endif

/* fn_8002C014 - 0x8002C014 | size: 0xd0 */
extern void menuButtonNormal(void*);
#if 0
asm void fn_8002C014(void) {
#include "src/game/gs_worldmap_fn_8002C014.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8002C014(void* r3) {
    u8* owner;
    u8* ctx;
    u16* keys;
    u16 item;
    u16 sel;
    s32 index;

    owner = (u8*)r3;
    ctx = *(u8**)(owner + 0x60);
    keys = windowGetKeyInfo();
    item = 0;
    if (keys[2] & 0x10) {
        index = (s8)owner[0x94] + (s8)owner[0x95];
        if (index < 0 || index >= *(s32*)(ctx + 0x8)) {
            sel = 0;
        } else {
            sel = (*(u16**)(ctx + 0x4))[index];
        }
        item = sel;
        if ((ctx[0x1d] & 1) && sel != 0) {
            return 0;
        }
    }
    if (item != 0) {
        **(u16**)ctx = item;
    }
    menuButtonNormal(owner);
    return 0;
}
#pragma pop
#endif

/* fn_8002C0E4 - 0x8002C0E4 | size: 0x1a0 */
extern const f32 lbl_8047B980;
extern const f32 lbl_8047B9C0;
extern const f32 lbl_8047B9C4;
extern const f32 lbl_8047B97C;
extern const f32 lbl_8047B9C8;
#if 0
asm void fn_8002C0E4(void) {
#include "src/game/gs_worldmap_fn_8002C0E4.inc"
}
#else
/*
 * GSmap_FadeFromBlack  (0x8002C0E4, 0x1A0 bytes)
 *
 * Drives the "fade from black" sequence on the world-map screen.
 * The lbl_8047B9xx floats are .sdata2 literals, so they are declared const:
 * the compiler then reuses one load across the pointer stores, as retail does.
 * self->byte[0x1]  = current phase (0 = init, 2 = animate, 3 = finish)
 * self->byte[0x2]  = one-shot flag (0 = not yet triggered, 1 = done)
 * self->ptr[0x60]  = inner context block; its fields are indirect float/int cells:
 *     ctx+0x0C = ptr to f32 : horizontal pan offset  (driven toward 0 in phase 2)
 *     ctx+0x10 = ptr to f32 : wrap counter A         (incremented by lbl_8047B9C8 mod lbl_8047B97C)
 *     ctx+0x14 = ptr to u32 : integer flag / counter (cleared to 0 in phase 0)
 *     ctx+0x18 = ptr to f32 : wrap counter B         (incremented by lbl_8047B9C4 mod lbl_8047B97C)
 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8002C0E4(u8* self)
{
    extern void winSeqSetMenu(s32 param, u32 key);
    u8* ctx;
    f32 val;
    f32 nv;

    ctx = *(u8**)(self + 0x60);
    switch ((s8)self[0x1]) {
    case 0:
        if ((s8)self[0x2] == 0) {
            winSeqSetMenu(0x60, 0x76);
            **(f32**)(ctx + 0x0C) = lbl_8047B980;
            **(s32**)(ctx + 0x14) = 0;
            **(f32**)(ctx + 0x18) = lbl_8047B980;
            **(f32**)(ctx + 0x10) = lbl_8047B980;
            self[0x2] = 1;
        }
        break;
    case 2:
        val = **(f32**)(ctx + 0x0C);
        if (val > lbl_8047B980) {
            nv = val - lbl_8047B9C0;
            **(f32**)(ctx + 0x0C) = nv;
            if (nv < lbl_8047B980) {
                **(f32**)(ctx + 0x0C) = lbl_8047B980;
            }
        }
        val = **(f32**)(ctx + 0x0C);
        if (val < lbl_8047B980) {
            nv = val + lbl_8047B9C0;
            **(f32**)(ctx + 0x0C) = nv;
            if (nv > lbl_8047B980) {
                **(f32**)(ctx + 0x0C) = lbl_8047B980;
            }
        }
        **(f32**)(ctx + 0x18) = **(f32**)(ctx + 0x18) + lbl_8047B9C4;
        if (**(f32**)(ctx + 0x18) > lbl_8047B97C) {
            **(f32**)(ctx + 0x18) = lbl_8047B980;
        }
        val = **(f32**)(ctx + 0x10);
        nv = val + lbl_8047B9C8;
        **(f32**)(ctx + 0x10) = nv;
        if (nv >= lbl_8047B97C) {
            **(f32**)(ctx + 0x10) = **(f32**)(ctx + 0x10) - lbl_8047B97C;
        }
        break;
    case 3:
        if ((s8)self[0x2] == 0) {
            winSeqSetMenu(0x60, 0x7a);
            self[0x2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_8002CE6C_ONLY)
/* Opens the shop item list (menu 0x60) for location `loc`; returns the
 * chosen item id, or 0 if the menu was cancelled. */
typedef struct ShopWork {
    u8 head[0x758];
    u32 credit0;
    u32 credit1;
    u8 exitFlag;
    u8 pad761[3];
    u32 field764;
    u16 count;
} ShopWork;

typedef struct ShopListMenu {
    u16* selection;
    u16* list;
    s32 count;
    u16* p_a3f4;
    void* p_a3f0;
    u16* p_a3ec;
    void* p_a3e8;
    u8 mode;
    u8 flag;
    ShopWork work;
} ShopListMenu;

static inline u32 shopOpenItemList(ShopListMenu* params, u32 loc, u8 mode, u8 flag)
{
    extern u32  windowGetActiveID(void);
    extern s32  menuOpenCustom(u32 sceneId, u32 a, u32 b, u32 c, u32 d, u32 e, ...);
    extern u32  lbl_80478E54;
    extern u32  lbl_80478E44;
    extern u16  lbl_8047A3F8;
    extern u16  lbl_8047A3F4;
    extern f32  lbl_8047A3F0;
    extern u16  lbl_8047A3EC;
    extern f32  lbl_8047A3E8;
    u16* list;
    s32 count;
    u16* p;

    lbl_8047A3F8 = 0;
    count = 0;
    params->selection = &lbl_8047A3F8;
    list = (u16*)lbl_80478E44 + ((u16*)lbl_80478E54)[loc * 2 + 1];
    p = list;
    while (*p != 0) {
        p++;
        count++;
    }
    params->count = count;
    params->list = list;
    params->p_a3f4 = &lbl_8047A3F4;
    params->p_a3f0 = &lbl_8047A3F0;
    params->p_a3ec = &lbl_8047A3EC;
    params->p_a3e8 = &lbl_8047A3E8;
    params->mode = mode;
    params->flag = flag;
    if (menuOpenCustom(0x60, windowGetActiveID(), 0, 0, 1, 1, params) == -1) {
        return 0;
    }
    return *params->selection;
}

/* Travel-dialog form of shopOpenItemList: reads the location entry through a
 * pointer, sets flag 1 and ignores the selection. */
/* RULE-EXCEPTION(user-approved): single-call inline helper — see docs/RULE_EXCEPTIONS.md */
static inline void shopOpenTravelList(ShopListMenu* params, u32 loc, u8 mode)
{
    extern u32  windowGetActiveID(void);
    extern s32  menuOpenCustom(u32 sceneId, u32 a, u32 b, u32 c, u32 d, u32 e, ...);
    extern u32  lbl_80478E54;
    extern u32  lbl_80478E44;
    extern u16  lbl_8047A3F8;
    extern u16  lbl_8047A3F4;
    extern f32  lbl_8047A3F0;
    extern u16  lbl_8047A3EC;
    extern f32  lbl_8047A3E8;
    u16* list;
    s32 count;
    u16* p;
    u8* entry;

    lbl_8047A3F8 = 0;
    count = 0;
    params->selection = &lbl_8047A3F8;
    entry = (u8*)lbl_80478E54;
    entry += loc * 4;
    list = (u16*)lbl_80478E44 + *(u16*)(entry + 2);
    p = list;
    while (*p != 0) {
        p++;
        count++;
    }
    params->count = count;
    params->list = list;
    params->p_a3f4 = &lbl_8047A3F4;
    params->p_a3f0 = &lbl_8047A3F0;
    params->p_a3ec = &lbl_8047A3EC;
    params->p_a3e8 = &lbl_8047A3E8;
    params->mode = mode;
    params->flag = 1;
    menuOpenCustom(0x60, windowGetActiveID(), 0, 0, 1, 1, params);
}

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY)
/* fn_8002C284 - 0x8002C284 | size: 0x184 */
extern void menuCloseCustom(void);
extern u32 lbl_804788A8;
extern u16 lbl_8047A3F8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u16 lbl_8047A3F4;
extern u16 lbl_8047A3EC;
#if 0
asm void fn_8002C284(void) {
#include "src/game/gs_worldmap_fn_8002C284.inc"
}
#else
/*
 * fn_8002C284  GSmap_ShowTravelDialog  0x8002C284 | 0x184 bytes
 *
 * Shows the "Travel to <location>?" confirmation dialog for the world map.
 *
 * loc_idx: world-map location index (indexes into lbl_80478E54 table)
 * mode:    dialog mode; 0x02 or 0x03 = skip the format-text preamble call
 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002C284(u32 loc_idx, u8 mode)
{
    extern void   fn_8002A1C4(u8* r3, s32 r4, s32 r5, ...); /* GSmap_FormatText2 */
    extern void*  windowGetActiveID(void);
    extern void   menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
    extern void   menuCloseCustom(u32 slot, u32 p1, u32 p2);
    extern u8     lbl_802E4F68[];
    extern u8     lbl_802EF0A8[];
    extern u32    lbl_804788A8;
    extern u32    lbl_80478E54;
    extern u32    lbl_80478E44;
    extern u16    lbl_8047A3F8;
    extern u16    lbl_8047A3F4;
    extern f32    lbl_8047A3F0;
    extern u16    lbl_8047A3EC;
    extern f32    lbl_8047A3E8;

    u8* tab;
    u8* base;
    ShopListMenu params;

    if (mode != 0x03 && mode != 0x02) {
        fn_8002A1C4((u8*)loc_idx, 0xa, -1);
    }
    if ((s32)lbl_804788A8 != 0) {
        tab = lbl_802E4F68;
        base = lbl_802EF0A8 + 4;
        *(s16*)(tab + 0x4) = *(s16*)(base + *(u32*)(tab + 0x0) * 0x1c);
        *(s16*)(tab + 0xC) = *(s16*)(base + *(u32*)(tab + 0x8) * 0x1c);
        *(s16*)(tab + 0x14) = *(s16*)(base + *(u32*)(tab + 0x10) * 0x1c);
        *(s16*)(tab + 0x1C) = *(s16*)(base + *(u32*)(tab + 0x18) * 0x1c);
        *(s16*)(tab + 0x24) = *(s16*)(base + *(u32*)(tab + 0x20) * 0x1c);
        lbl_804788A8 = 0;
    }
    shopOpenTravelList(&params, loc_idx, mode);
    menuCloseCustom(0x60, 0, 1);
}
#pragma pop
#endif

/* fn_8002C408 - 0x8002C408 | size: 0xa64 */
extern void savedataGetStatus(void);
extern void pcboxGetItemCapacity(void);
extern void heroItemCheckAddItemDataId(void);
extern void soundStartRaw(void);
extern void fn_80093574(void);
extern void fn_80092C90(void);
extern void fn_80093610(void);
extern void fn_80093698(void);
extern void fn_801D0748(void);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 n);
extern u32 lbl_8047A3DC;
extern u32 lbl_8047A3D8;
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u32 lbl_804788A8;
extern u32 lbl_8047A3E4;
extern u32 lbl_8047A3E0;
extern u32 lbl_80478E4C;
#if 0
asm void fn_8002C408(void) {
#include "src/game/gs_worldmap_fn_8002C408.inc"
}
#else
/*
 * Money available to a shop of type `mode`: the PC-box shop (3) uses the
 * credits held in its work block, folding in pending deposits
 * (lbl_8047A660) and resets (lbl_8047A664); every other shop uses the
 * hero's money.
 */
static inline u32 shopGetMoney(u8 mode, ShopWork* work)
{
    extern u32 heroGetStatus(u8* ptr, u32 selector, u32 idx);
    extern u32 lbl_8047A660;
    extern u32 lbl_8047A664;

    switch (mode) {
    case 2:
        return heroGetStatus(NULL, 0xd, 0);
    case 3:
        if (work != NULL) {
            if ((s32)lbl_8047A660 > 0) {
                work->credit0 += lbl_8047A660;
                work->credit1 += lbl_8047A660;
                lbl_8047A660 = 0;
            }
            if ((s32)lbl_8047A664 > 0) {
                work->credit0 = 0;
                work->credit1 = 0;
                lbl_8047A664 = 0;
            }
            return work->credit0;
        }
        return 0;
    default:
        return heroGetStatus(NULL, 0xd, 0);
    }
}

/*
 * fn_8002C408 - coupon shop / PC-box shop loop for location `mapIdx`.
 * Shop types 2 and 3 work on a copy of the save state held in the menu
 * work block; type 4 first checks the player can afford the cheapest item.
 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_8002C408(s32 mapIdx, u32 mode)
{
    extern void  fn_80142A88(void* buf, s32 v);
    extern s32   fn_80029CC0(u8* buf);
    extern u32   savedataGetStatus(u8* obj, u16 sel);
    extern u32   heroGetStatus(u8* ptr, u32 selector, u32 idx);
    extern void  heroSetStatus(u8* ptr, u32 selector, u32 value);
    extern void  itemDataBiosGetPtr(u32 id);
    extern u32   itemDataBiosGetCoupon(void);
    extern u16   itemBiosGetItemDataId(void* slot);
    extern u16   itemBiosGetNum(void* slot);
    extern u32   pcboxGetItemCapacity(s32 a, u32 item);
    extern s32   heroItemCheckAddItemDataId(u8* ptr, u32 item);
    extern void  fn_80029EF4(void* a, s32 b, s32 c, s32 d, void* e);
    extern void  fn_8002A1C4(u8* idx, s32 msgId, s32 term, ...);
    extern u32   fn_80029FAC(u8* idx, s32 a, s32 b, s32 c, ...);
    extern void  winMsgOpenWithSE(s32 a, u32 str, s32 c, s32 d, u8 e);
    extern s8    menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern u32   windowGetActiveID(void);
    extern s32   menuOpenCustom(s32 a, u32 b, void* c, s32 d, s32 e, s32 f, ...);
    extern void  menuClose(s32 slot);
    extern void  menuCloseSync(s32 slot, s32 flag);
    extern void  winMsgClose(s32 slot);
    extern void  menuCloseCustom(s32 a, s32 b, s32 c);
    extern void  soundStartRaw(s32 soundId, s32 p2, s32 p3);
    extern s32   fn_801D0748(u32 a, u32 b, u32 c);
    extern void  fn_80093574(s32 a);
    extern void  fn_80092C90(s32 a, void* list, s32 c);
    extern s32   fn_80093610(s32 a);
    extern void  fn_80093698(s32 a);
    extern void* memcpy(void* dst, const void* src, u32 n);
    extern void* memset(void* dst, int v, u32 n);
    extern u32 lbl_8047A3DC;
    extern u32 lbl_8047A3D8;
    extern u32 lbl_8047A3E4;
    extern u32 lbl_8047A3E0;
    extern u32 lbl_804788A8;
    extern u32 lbl_80478E54;
    extern u32 lbl_80478E44;
    extern u32 lbl_80478E4C;
    extern const u8 lbl_80266E70[];
    extern u8  lbl_802E4F68[];
    extern u8  lbl_802EF0A8[];

    u8 type;
    u8 kind;
    s32 done;
    s32 ok;
    u32 money;
    s32 cheapest;
    u32 cost;
    u16* list;
    u16* p;
    s32 count;
    s32 i;
    s32 n;
    u16 price;
    u16 id;
    s32 qty;
    s32 total;
    s32 answer;
    s32 room;
    s32 ret;
    ShopWork* work;
    u8* slot;
    u8 name;
    u8 se;
    u8 se2;
    s32 header;
    struct {
        s32 enabled;
        s32 max;
        s32 price;
        u32* result;
        u8 r;
        u8 g;
        u8 b;
        u8 pad;
        u32 trailer;
    } qtyParams;
    struct {
        u32 credit0;
        u32 credit1;
        u32 field764;
        u16 zero;
        u16 count;
        struct {
            u16 id;
            u16 num;
        } items[50];
    } inv;
    ShopListMenu params;

    type = mode;
    done = 0;
    switch (type) {
    case 2:
        work = &params.work;
        fn_80142A88(work, 0xeb);
        fn_80142A88(work->head + 0x3ac, 0xeb);
        params.work.credit0 = 0;
        params.work.credit1 = 0;
        params.work.exitFlag = 0;
        memcpy((void*)lbl_8047A3DC, (void*)savedataGetStatus(NULL, 3), 0x7198);
        lbl_8047A3D8 = heroGetStatus(NULL, 0xd, 0);
        ok = 1;
        break;
    case 3:
        ok = fn_80029CC0((u8*)&params.work);
        break;
    default:
        ok = 1;
        break;
    }
    if (ok == 0) {
        return;
    }
    if ((u8)mode == 4) {
        money = shopGetMoney(type, &params.work);
        cheapest = 0x98967F;
        list = (u16*)lbl_80478E44 + *(u16*)(lbl_80478E54 + 2 + mapIdx * 4);
        p = list;
        count = 0;
        while (*p != 0) {
            p++;
            count++;
        }
        for (; *list != 0; list++) {
            itemDataBiosGetPtr(*list);
            cost = (u16)itemDataBiosGetCoupon();
            if ((s32)cost < cheapest) {
                cheapest = cost;
            }
        }
        if ((s32)money < cheapest) {
            fn_8002A1C4((u8*)mapIdx, 4, -1);
            return;
        }
    }
    kind = mode;
    work = &params.work;
    while (done == 0) {
        if ((s32)lbl_804788A8 != 0) {
            for (i = 0; i < 5; i++) {
                *(s16*)(lbl_802E4F68 + i * 8 + 4) = *(s16*)(lbl_802EF0A8 + *(s32*)(lbl_802E4F68 + i * 8) * 0x1c + 4);
            }
            lbl_804788A8 = 0;
        }
        done = shopOpenItemList(&params, mapIdx, mode, 0);
        if ((u16)done == 0) {
            goto confirm;
        }
        itemDataBiosGetPtr(done);
        n = (u16)itemDataBiosGetCoupon();
        if (n > 0) {
            n = (s32)shopGetMoney(type, work) / n;
            if (n > 0x63) {
                n = 0x63;
            }
        } else {
            n = 0x63;
        }
        if (n <= 0) {
            fn_8002A1C4((u8*)mapIdx, 8, -1);
            continue;
        }
        itemDataBiosGetPtr(done);
        price = itemDataBiosGetCoupon();
        id = done;
        lbl_8047A3E4 = fn_80029FAC(&se, mapIdx, 0xc, 0x2d, id, -1);
        if (n < 1) {
            qty = 0;
        } else {
            qtyParams.enabled = 1;
            qtyParams.max = n;
            qtyParams.price = price;
            lbl_8047A3E0 = 1;
            qtyParams.result = &lbl_8047A3E0;
            qtyParams.r = lbl_80266E70[kind * 3];
            qtyParams.g = lbl_80266E70[kind * 3 + 1];
            qtyParams.b = lbl_80266E70[kind * 3 + 2];
            qtyParams.trailer = 1;
            header = 1;
            ret = menuOpenCustom(0x61, windowGetActiveID(), &header, 0, 1, 1, &qtyParams);
            menuClose(0x61);
            menuCloseSync(0x61, 1);
            if (ret == -1) {
                qty = -1;
            } else {
                qty = lbl_8047A3E0;
            }
        }
        if (qty < 0) {
            continue;
        }
        total = qty * price;
        winMsgOpenWithSE(2, fn_80029FAC(&se, mapIdx, 5, 0x2d, id, 0x2f, qty, 0x4b, total, -1), 1, 0, se);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer == 1 || answer == -1) {
            continue;
        }
        switch (type) {
        case 2:
            room = (u16)pcboxGetItemCapacity(0, done);
            break;
        case 3:
            if (work != NULL) {
                room = 0;
                slot = (u8*)work;
                for (i = 0; i < work->count; i++) {
                    if (itemBiosGetItemDataId(slot) == (u16)done) {
                        room += (u16)(999 - itemBiosGetNum(slot));
                    } else if (itemBiosGetItemDataId(slot) == 0) {
                        room += 999;
                    }
                    slot += 4;
                }
            } else {
                room = 0;
            }
            break;
        default:
            room = heroItemCheckAddItemDataId(NULL, done);
            break;
        }
        if (room < qty) {
            fn_8002A1C4((u8*)mapIdx, 9, -1);
            continue;
        }
        fn_8002A1C4((u8*)mapIdx, 6, -1);
        fn_80029EF4((void*)total, done, qty, mode, work);
        soundStartRaw(0x3cc, 0, 0);
        if ((u8)mode != 4) {
            continue;
        }
        winMsgOpenWithSE(2, fn_80029FAC(&se, mapIdx, 7, -1), 1, 0, se);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer != -1 && answer != 1) {
            continue;
        }
    confirm:
        if (kind == 2 || kind == 3) {
            if (work->exitFlag == 0) {
                winMsgOpenWithSE(2, fn_80029FAC(&se2, mapIdx, 0xd, -1), 1, 0, se2);
                answer = menuSubOpenYesNo(0, -1, -1, 0);
                winMsgClose(1);
                if (answer == 1 || answer == -1) {
                    done = 0;
                } else {
                    if ((u8)mode == 3) {
                        fn_8002A1C4((u8*)mapIdx, 0xe, -1);
                    }
                    done = 1;
                }
            } else {
                winMsgOpenWithSE(2, fn_80029FAC(&se2, mapIdx, 0xf, -1), 1, 0, se2);
                answer = menuSubOpenYesNo(0, -1, -1, 0);
                winMsgClose(1);
                if (answer == 1 || answer == -1) {
                    done = 0;
                } else if ((u8)mode == 3) {
                    name = *(u8*)(lbl_80478E4C + *(u8*)(lbl_80478E54 + mapIdx * 4) * 0x4c);
                    winMsgOpenWithSE(2, 0x3d83, 0, 0, name);
                    memset(&inv, 0, sizeof(inv));
                    inv.credit0 = work->credit0;
                    inv.credit1 = work->credit1;
                    inv.field764 = work->field764;
                    inv.zero = 0;
                    inv.count = work->count;
                    for (i = 0; i < inv.count; i++) {
                        if (i < 0 || i > work->count) {
                            id = 0;
                        } else {
                            slot = (u8*)work + i * 4;
                            id = itemBiosGetItemDataId(slot);
                            if (id != 0) {
                                inv.items[i].num = itemBiosGetNum(slot);
                            }
                        }
                        inv.items[i].id = id;
                        if (inv.items[i].id == 0) {
                            inv.items[i].num = 0;
                            break;
                        }
                    }
                    fn_80093574(1);
                    fn_80092C90(1, &inv, 0);
                    fn_80093574(1);
                    if (fn_80093610(1) != 0xc) {
                        fn_80093698(1);
                        winMsgOpenWithSE(2, 0x3d85, 1, 0, name);
                        winMsgClose(1);
                    } else {
                        fn_80093698(1);
                        winMsgOpenWithSE(2, 0x3d84, 1, 0, name);
                        winMsgClose(1);
                    }
                    fn_8002A1C4((u8*)mapIdx, 0xe, -1);
                    done = 1;
                } else {
                    if (fn_801D0748(4, 2, 0) != 4) {
                        memcpy((void*)savedataGetStatus(NULL, 3), (void*)lbl_8047A3DC, 0x7198);
                        heroSetStatus(NULL, 0xd, lbl_8047A3D8);
                    }
                    done = 1;
                }
            }
        } else {
            done = 1;
        }
    }
    menuCloseCustom(0x60, 0, 1);
}
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY) || defined(MENUSHOP_ISLAND_8002CE6C_ONLY)
/* fn_8002CE6C - 0x8002CE6C | size: 0x2e8 */
extern void msgGetCharsPerFrame(void);
extern void heroDecPokedoru(void);
extern u32 lbl_804788A8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
#if 0
asm void fn_8002CE6C(void) {
#include "src/game/gs_worldmap_fn_8002CE6C.inc"
}
#else
/*
 * fn_8002CE6C - shop purchase loop for location `loc` (also the message
 * target passed to fn_8002A2CC). Shows the shop's item list, and for each
 * chosen item checks the price against the player's money (capped at 99
 * affordable), checks bag space, then charges, adds the item and loops.
 */
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
void fn_8002CE6C(u8* loc, u8 mode)
{
    extern u32  menuCloseCustom(u32 a, u32 b, u32 c);
    extern void fn_8002A2CC(u8* obj, s32 msgId, s32 arg2, ...);
    extern void _threadSwitch(void);
    extern s32  msgGetCharsPerFrame(void);
    extern u32  heroGetStatus(u8* ptr, u32 sel, u32 idx);
    extern u32  windowGetActiveID(void);
    extern s32  menuOpenCustom(u32 sceneId, u32 a, u32 b, u32 c, u32 d, u32 e, ...);
    extern void itemDataBiosGetPtr(u32 itemId);
    extern u32  itemDataBiosGetPrice(void);
    extern s32  heroItemCheckAddItemDataId(u8* ptr, u32 itemId);
    extern void soundStartRaw(u32 soundId, u32 a, u32 b);
    extern void heroDecPokedoru(u8* ptr, u32 amount);
    extern s32  heroItemAddItemDataId(u8* ptr, u32 itemId, u32 qty, u32 flags);
    extern u8   lbl_802E4F68[];
    extern u8   lbl_802EF0A8[];
    extern u32  lbl_804788A8;
    extern u32  lbl_80478E54;
    extern u32  lbl_80478E44;
    extern u16  lbl_8047A3F8;
    extern u16  lbl_8047A3F4;
    extern f32  lbl_8047A3F0;
    extern u16  lbl_8047A3EC;
    extern f32  lbl_8047A3E8;

    s32 frames;
    s32 i;
    u32 item;
    u16 cost;
    u16 price;
    s32 afford;
    ShopListMenu params;

loop:
    menuCloseCustom(0x60, 0, 1);
    fn_8002A2CC(loc, 0, 0x4b, heroGetStatus(NULL, 0xc, 0), -1);
    frames = 0;
    while (frames < 0x1e) {
        _threadSwitch();
        frames += msgGetCharsPerFrame();
    }
    if ((s32)lbl_804788A8 != 0) {
        for (i = 0; i < 5; i++) {
            *(s16*)(lbl_802E4F68 + i * 8 + 4) = *(s16*)(lbl_802EF0A8 + *(s32*)(lbl_802E4F68 + i * 8) * 0x1c + 4);
        }
        lbl_804788A8 = 0;
    }
    item = shopOpenItemList(&params, (u32)loc, mode, 0);
    if ((u16)item == 0) {
        fn_8002A2CC(loc, 2, -1);
    } else {
        itemDataBiosGetPtr(item);
        cost = itemDataBiosGetPrice();
        itemDataBiosGetPtr(item);
        price = itemDataBiosGetPrice();
        if ((s32)price > 0) {
            afford = (s32)heroGetStatus(NULL, 0xc, 0) / (s32)price;
            if (afford > 0x63) {
                afford = 0x63;
            }
        } else {
            afford = 0x63;
        }
        if (afford <= 0) {
            fn_8002A2CC(loc, 5, -1);
        } else if (heroItemCheckAddItemDataId(NULL, item) < 1) {
            fn_8002A2CC(loc, 6, -1);
        } else {
            soundStartRaw(0x3cb, 0, 0);
            heroDecPokedoru(NULL, cost);
            heroItemAddItemDataId(NULL, item, 1, -1);
            fn_8002A2CC(loc, 4, 0x2d, item & 0xffff, -1);
            goto loop;
        }
    }
    menuCloseCustom(0x60, 0, 1);
}
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
#endif

/* fn_8002D154 - 0x8002D154 | size: 0x480 */
extern void heroItemCheckHaveItemDataId(void);
extern u32 lbl_804788A8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002D154(void) {
#include "src/game/gs_worldmap_fn_8002D154.inc"
}
#else
/*
 * fn_8002D154 - shop buy loop for location `mapIndex`. Shows the item list,
 * validates the chosen item (key items need flag 0x21e), caps the
 * affordable count at 99, opens the quantity menu tinted with RGB triple
 * `colorIndex` from lbl_80266E70, confirms with Yes/No, then charges the
 * total and adds the items. Buying 10+ of item 4 adds a bonus item 0xc.
 */
/* Unit price of `item` (selects the item record, then reads its price). */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8002D154) — see docs/RULE_EXCEPTIONS.md */
static inline u16 shopGetItemPrice(u32 item)
{
    extern u32  itemDataBiosGetPtr(u32 itemId);
    extern u32  itemDataBiosGetPrice(void);

    itemDataBiosGetPtr(item);
    return itemDataBiosGetPrice();
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
void fn_8002D154(s32 mapIndex, u8 colorIndex)
{
    extern u32  lbl_804788A8;
    extern u32  lbl_80478E54;
    extern u32  lbl_80478E44;
    extern u32  lbl_8047A3E4;
    extern u16  lbl_8047A3F8;
    extern u16  lbl_8047A3F4;
    extern f32  lbl_8047A3F0;
    extern u16  lbl_8047A3EC;
    extern f32  lbl_8047A3E8;
    extern u32  lbl_8047A3E0;
    extern const u8 lbl_80266E70[];
    extern u8   lbl_802E4F68[];
    extern u8   lbl_802EF0A8[];
    extern u32  windowGetActiveID(void);
    extern s32  menuOpenCustom(s32 a, u32 b, void* c, s32 d, s32 e, s32 f, ...);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 flag);
    extern s8   menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void winMsgClose(s32 a);
    extern void menuCloseCustom(s32 a, s32 b, s32 c);
    extern u32  itemDataBiosGetPtr(u32 itemId);
    extern u8   itemDataBiosGetKind(void);
    extern u32  itemDataBiosGetPrice(void);
    extern u8   heroItemCheckHaveItemDataId(s32 a, s32 b);
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx);
    extern s32  heroItemCheckAddItemDataId(s32 a, u32 item);
    extern void heroDecPokedoru(s32 a, s32 amount);
    extern void heroItemAddItemDataId(s32 a, s32 item, u16 qty, s32 d);
    extern void soundStartRaw(s32 sfx, s32 b, s32 c);
    extern void winMsgOpenWithSE(s32 a, u32 b, s32 c, s32 d, u8 e);
    extern u32  fn_8002A0B8(u8* buf, s32 idx, s32 a, s32 b, ...);
    extern void fn_8002A2CC(u8* idx, s32 a, s32 b, ...);

    s32 i;
    u32 item;
    s32 n;
    s32 price;
    u16 id;
    s32 answer;
    s32 ret;
    u8 se;
    s32 header;
    struct {
        s32 enabled;
        s32 max;
        s32 price;
        u32* result;
        u8 r;
        u8 g;
        u8 b;
        u8 pad;
        u32 trailer;
    } qtyParams;
    ShopListMenu params;

    for (;;) {
        if ((s32)lbl_804788A8 != 0) {
            for (i = 0; i < 5; i++) {
                *(s16*)(lbl_802E4F68 + i * 8 + 4) = *(s16*)(lbl_802EF0A8 + *(s32*)(lbl_802E4F68 + i * 8) * 0x1c + 4);
            }
            lbl_804788A8 = 0;
        }
        item = shopOpenItemList(&params, mapIndex, colorIndex, 0);
        if ((u16)item == 0) {
            break;
        }
        if (itemDataBiosGetPtr(item) == 0) {
            continue;
        }
        if (itemDataBiosGetKind() == 6 && heroItemCheckHaveItemDataId(0, 0x21e) == 0) {
            fn_8002A2CC((u8*)mapIndex, 8, -1);
            continue;
        }
        n = shopGetItemPrice(item);
        if (n > 0) {
            n = (s32)heroGetStatus(NULL, 0xc, 0) / n;
            if (n > 0x63) {
                n = 0x63;
            }
        } else {
            n = 0x63;
        }
        if (n <= 0) {
            fn_8002A2CC((u8*)mapIndex, 5, -1);
            continue;
        }
        price = shopGetItemPrice(item);
        id = item;
        lbl_8047A3E4 = fn_8002A0B8(&se, mapIndex, 0xc, 0x2d, id, -1);
        if (n < 1) {
            n = 0;
        } else {
            qtyParams.enabled = 1;
            qtyParams.max = n;
            qtyParams.price = price;
            lbl_8047A3E0 = 1;
            qtyParams.result = &lbl_8047A3E0;
            qtyParams.r = lbl_80266E70[colorIndex * 3];
            qtyParams.g = lbl_80266E70[colorIndex * 3 + 1];
            qtyParams.b = lbl_80266E70[colorIndex * 3 + 2];
            qtyParams.trailer = 0;
            header = 1;
            ret = menuOpenCustom(0x61, windowGetActiveID(), &header, 0, 1, 1, &qtyParams);
            menuClose(0x61);
            menuCloseSync(0x61, 1);
            if (ret == -1) {
                n = -1;
            } else {
                n = lbl_8047A3E0;
            }
        }
        if (n < 0) {
            continue;
        }
        price = n * price;
        winMsgOpenWithSE(2, fn_8002A0B8(&se, mapIndex, 3, 0x2d, id, 0x2f, n, 0x4b, price, -1), 1, 0, se);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer == 1 || answer == -1) {
            continue;
        }
        if (heroItemCheckAddItemDataId(0, item) < n) {
            fn_8002A2CC((u8*)mapIndex, 6, -1);
            continue;
        }
        soundStartRaw(0x3cb, 0, 0);
        heroDecPokedoru(0, price);
        heroItemAddItemDataId(0, item, n, -1);
        fn_8002A2CC((u8*)mapIndex, 4, -1);
        if ((u16)item == 4 && n >= 0xa && heroItemCheckAddItemDataId(0, 0xc) >= 1) {
            fn_8002A2CC((u8*)mapIndex, 7, -1);
            heroItemAddItemDataId(0, 0xc, 1, -1);
        }
    }
    menuCloseCustom(0x60, 0, 1);
}
#if !defined(NO_LOCAL_PRAGMAS_MENUSHOP_ISLAND_8002CE6C)
#endif
#endif

#endif /* guard split */
#if !defined(MENUSHOP_ISLAND_ONLY)
/* fn_8002D5D4 - 0x8002D5D4 | size: 0x348 */
extern u32 lbl_8047A3FC;
extern u32 lbl_80478E54;
extern u32 lbl_8047A3DC;
#if 0
asm void fn_8002D5D4(void) {
#include "src/game/gs_worldmap_fn_8002D5D4.inc"
}
#else
typedef struct ShopLocationEntry {
    u8 field_0;
    u8 type;
    u8 field_2[2];
} ShopLocationEntry;
typedef u8 ShopLocationArgument;

static inline s32 shopQueryMenu(s32 menu)
{
    s32 result = menuOpen(menu, 1);

    menuClose(menu);
    menuCloseSync(menu, 1);
    return result;
}

static inline void shopNormalizeMenu62(s32 result, s32* selection)
{
    if (result == -1 || result == 2) {
        *selection = 2;
    } else if (result == 0) {
        *selection = 0;
    } else {
        *selection = 1;
    }
}

static inline s32 shopNormalizeMenu83(s32 result)
{
    switch (result) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    default:
        return 3;
    }
}

static inline u32 shopMemoryAlloc(void)
{
    return _toolentryAlloc__FUl(0x7198);
}

/*
 * fn_8002D5D4  GSmap_CancelTravel  0x8002D5D4 | size: 0x348
 *
 * Implements the "cancel travel" / travel-confirmation state machine for the
 * world map.  Dispatches on the NPC-state byte at lbl_80478E54[lbl_8047A3FC*4 + 1]:
 *   0 -> show initial location name dialog (menu 0x62), loop until confirmed/cancelled
 *   1 -> hand off to fn_8002CE6C (alternate confirm sequence)
 *   else -> alloc a GSmem block for an extended dialog, run menu 0x83 loop
 *
 * No parameters (the state index lives in lbl_8047A3FC).
 * On exit, frees the GSmem block and optionally fires a story event.
 *
 * Callee conventions used here:
 *   fn_8002A0B8 / fn_80029FAC : vararg text formatters
 *       (u8* outBuf, s32 locIdx, s32 p2, s32 first_va, ..., -1 terminator)
 *   winMsgOpenWithSE               : (s32 kind, u32 tableVal, s32 p3, s32 p4, u8 fmtId)
 *   menuOpen               : (s32 menuId, s32 flag) -> s32 result
 *   menuClose               : (s32 menuId)
 *   menuCloseSync             : (s32 menuId, s32 flag)
 *   _toolentryAlloc__FUl / GSmemAllocRaw : (u32 size) -> u16 handle
 *   fn_800E27B0 / GSmemGetPtr   : (u16 handle) -> void*
 *   fn_800E24B0 / GSmemLock     : (u16 handle)
 *   fn_800E209C / GSmemFree     : (u16 handle)
 */
void std026_unkMenuSequence26(void)
{
    ShopLocationEntry* location_entry;
    u32 location_offset;
    s32 location;
    u8 type;
    s32 menu_result;
    s32 done;
    u32 memory;
    s32 selection;
    u8 text1;
    u8 text0;

    location_offset = lbl_8047A3FC;
    location = location_offset;
    location_entry = (ShopLocationEntry*)lbl_80478E54;
    location_entry += location_offset;
    type = location_entry->type;

    switch (type) {
    case 0: {
        u32 value = heroGetStatus(NULL, 0xC, 0);
        u32 message = fn_8002A0B8(&text1, location, 0, 0x4B, value, -1);
        winMsgOpenWithSE(2, message, 1, 0, text1);
        while ((menu_result = shopQueryMenu(0x62),
                shopNormalizeMenu62(menu_result, &selection), selection != 2)) {
            winMsgClose(1);
            switch (selection) {
            case 0:
                fn_8002D154(location, type);
                break;
            case 1:
                fn_80018F54(3, location, 0);
                break;
            }
            message = fn_8002A0B8(&text1, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text1);
        }

        fn_8002A2CC((ShopLocationArgument*)location, 2, -1);
        break;
    }
    case 1:
        fn_8002CE6C((ShopLocationArgument*)location, type);
        break;
    default: {
        u32 message;

        done = 0;
        memory = shopMemoryAlloc();
        lbl_8047A3DC = fn_800E27B0(memory);
        message = fn_80029FAC(&text0, location, 0, -1);
        winMsgOpenWithSE(2, message, 1, 0, text0);
        while ((menu_result = shopNormalizeMenu83(shopQueryMenu(0x83))) != 3) {
            winMsgClose(1);
            switch (menu_result) {
            case 0:
                fn_8002C408(location, type);
                break;
            case 1:
                fn_8002C284(location, type);
                break;
            case 2:
                fn_8002A1C4((ShopLocationArgument*)location, 0xB, -1);
                break;
            case 3:
                done = 1;
                break;
            }
            if (done != 0) {
                break;
            }
            message = fn_80029FAC(&text0, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text0);
        }

        if (type != 3 && type != 2) {
            fn_8002A1C4((ShopLocationArgument*)location, 2, -1);
        } else {
            winMsgClose(1);
        }
        fn_800E24B0(memory);
        fn_800E209C(memory);
        break;
    }
    }

    if ((s32)*(&lbl_8047A3FC + 1) != 0) {
        fn_800FF660();
        floorSetFadeScript(0, 0);
    }
}
#endif

/* fn_8002D91C - 0x8002D91C | size: 0x350 */
extern u32 lbl_80478E54;
extern u32 lbl_8047A3DC;
#if 0
asm void fn_8002D91C(void) {
#include "src/game/gs_worldmap_fn_8002D91C.inc"
}
#else
/*
 * fn_8002D91C  GSmap_ArrivalDialog  0x8002D91C | size: 0x350
 *
 * fn_8002D5D4 for an explicit location: records it in lbl_8047A3FC (and
 * clears the story-event word after it) before running the same dialog.
 */
void fn_8002D91C(u32 arg0)
{
    ShopLocationEntry* location_entry;
    s32 location;
    u8 type;
    s32 menu_result;
    s32 done;
    u32 memory;
    s32 selection;
    u8 text1;
    u8 text0;

    location = arg0;
    location_entry = (ShopLocationEntry*)lbl_80478E54;
    location_entry += arg0;
    lbl_8047A3FC = arg0;
    *(&lbl_8047A3FC + 1) = 0;
    type = location_entry->type;

    switch (type) {
    case 0: {
        u32 value = heroGetStatus(NULL, 0xC, 0);
        u32 message = fn_8002A0B8(&text1, location, 0, 0x4B, value, -1);
        winMsgOpenWithSE(2, message, 1, 0, text1);
        while ((menu_result = shopQueryMenu(0x62),
                shopNormalizeMenu62(menu_result, &selection), selection != 2)) {
            winMsgClose(1);
            switch (selection) {
            case 0:
                fn_8002D154(location, type);
                break;
            case 1:
                fn_80018F54(3, location, 0);
                break;
            }
            message = fn_8002A0B8(&text1, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text1);
        }

        fn_8002A2CC((ShopLocationArgument*)location, 2, -1);
        break;
    }
    case 1:
        fn_8002CE6C((ShopLocationArgument*)location, type);
        break;
    default: {
        u32 message;

        done = 0;
        memory = shopMemoryAlloc();
        lbl_8047A3DC = fn_800E27B0(memory);
        message = fn_80029FAC(&text0, location, 0, -1);
        winMsgOpenWithSE(2, message, 1, 0, text0);
        while ((menu_result = shopNormalizeMenu83(shopQueryMenu(0x83))) != 3) {
            winMsgClose(1);
            switch (menu_result) {
            case 0:
                fn_8002C408(location, type);
                break;
            case 1:
                fn_8002C284(location, type);
                break;
            case 2:
                fn_8002A1C4((ShopLocationArgument*)location, 0xB, -1);
                break;
            case 3:
                done = 1;
                break;
            }
            if (done != 0) {
                break;
            }
            message = fn_80029FAC(&text0, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text0);
        }

        if (type != 3 && type != 2) {
            fn_8002A1C4((ShopLocationArgument*)location, 2, -1);
        } else {
            winMsgClose(1);
        }
        fn_800E24B0(memory);
        fn_800E209C(memory);
        break;
    }
    }

    if ((s32)*(&lbl_8047A3FC + 1) != 0) {
        fn_800FF660();
        floorSetFadeScript(0, 0);
    }
}
#endif

/* Travel-dialog state at lbl_8047A3FC: the location the dialog was opened
 * for and whether a travel request is pending. */
typedef struct ShopTravelState {
    u32 location;
    u32 active;
} ShopTravelState;

/* menuShopOpen - 0x8002DC6C | size: 0xb8 */
extern void mailMainReceiveTerminate(void);
extern u32 fn_800D37CC(void);
extern void menuCreateOffScreen(f32);
extern void _flagSet(s32, s32);
extern void menuReleaseOffScreen(f32);
extern f64 lbl_8047B998;
extern f32 lbl_8047B9CC;
#if 0
asm void menuShopOpen(void) {
#include "src/game/gs_worldmap_menuShopOpen.inc"
}
#else
/*
 * menuShopOpen  GSmap_SetStoryFlag  0x8002DC6C  size: 0xB8
 *
 * Sets a story-progression flag on the worldmap state, kicks the scene
 * fade/timer system, issues a wait-for-dialog yield, then samples the
 * scene timer a second time to feed the post-yield fade curve.
 *
 * Parameters:
 *   flag  -- story/destination flag value stored to lbl_8047A3FC (r3 -> r31)
 *
 * int-to-float pattern:
 *   xoris r3,r3,0x8000 + lis r0,0x4330 stacked into a f64 then
 *   fsubs lbl_8047B998(r2) bias => plain (f32)(s32)fn_800D37CC()
 */
void menuShopOpen(u32 flag)
{
    extern void mailMainReceiveTerminate(void);
    extern u32  fn_800D37CC(void);
    extern void menuCreateOffScreen(f32);
    extern void _flagSet(s32, s32);
    extern void std098_mapWarpToMenuRoom(s32);
    extern void floorSetFadeScript(s32, u32);
    extern void _threadSwitch(void);
    extern void menuReleaseOffScreen(f32);

    /* lbl_8047B9CC / lbl_8047B998: r2-relative float/double constants used for
       the int->float bias conversion.  We bypass the bias trick with a direct
       cast - ENDIAN-QA: xoris+0x4330 bias is identical to (f32)(s32)x */
    ShopTravelState* state;
    f32 t;

    mailMainReceiveTerminate();

    /* pre-yield timer sample -> fade-in parameter */
    t = (f32)(s32)fn_800D37CC(); /* ENDIAN-QA: lbl_8047B9CC / (bias_cvt(D37CC())) */
    {
        extern f32 lbl_8047B9CC;
        t = lbl_8047B9CC / t;
    }
    menuCreateOffScreen(t);

    /* store flag and mark slot active */
    state = (ShopTravelState*)&lbl_8047A3FC;
    state->location = flag;
    state->active = 1;

    _flagSet(1, 2);
    std098_mapWarpToMenuRoom(0x38f);
    floorSetFadeScript(0, 0);
    _threadSwitch();   /* GSthreadYield / vsync yield */

    /* post-yield timer sample -> fade-out parameter */
    t = (f32)(s32)fn_800D37CC();
    {
        extern f32 lbl_8047B9CC;
        t = lbl_8047B9CC / t;
    }
    menuReleaseOffScreen(t);
}
#endif
#endif /* !MENUSHOP_ISLAND_ONLY */
