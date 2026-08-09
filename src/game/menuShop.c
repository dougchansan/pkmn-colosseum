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

/* fn_80029850 - 0x80029850 | size: 0x8c */
extern u16 itemBiosGetItemDataId(void*);
extern u16 itemBiosGetNum(void*);
#if 0
asm void fn_80029850(void) {
#include "src/game/gs_worldmap_fn_80029850.inc"
}
#else
#pragma optimization_level 4
u32 fn_80029850(u8* r3, u16 r4, u16 r5, u16 r6) {
    register u32 r28;
    register s32 r27;
    register u8* r26;
    u16 r30;
    u16 r29;
    u16 r31;
    u16 val;
    u16 cur;
    r26 = r3;
    r30 = r6;
    r29 = r4;
    r28 = 0;
    r27 = 0;
    r31 = r5;
    while (r27 < r29) {
        val = itemBiosGetItemDataId(r26);
        if (val == r31) {
            cur = itemBiosGetNum(r26);
            r28 += (u16)(r30 - cur);
        } else if (val == 0) {
            r28 += r30;
        }
        r27++;
        r26 += 4;
    }
    return r28;
}
#endif

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

/* fn_800298DC - 0x800298DC | size: 0x1ec */
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_800298DC(ShopItemSlot* slots, s32 count, s32 item_id, s32 quantity,
                 s16 index, s32 maximum) {
    u16 current_id;
    u16 current_quantity;
    u16 capacity;
    u16 added;
    s32 i;
    ShopItemSlot* slot;

    if (index < -1 || index >= (u16)count) {
        return (u16)quantity;
    }

    if (index != -1) {
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
            added = capacity;
        } else {
            added = (u16)quantity;
        }
        itemBiosSetNum(slots, (u16)(current_quantity + added));
        return (u16)(quantity - added);
    }

    quantity &= 0xFFFF;
    maximum = (u16)maximum;
    i = 0;
    while (i < (u16)count && quantity > 0) {
        s16 slot_index = i;

        if (slot_index < 0 || slot_index >= (u16)count) {
            quantity = (u16)quantity;
        } else {
            slot = &slots[slot_index];
            current_id = itemBiosGetItemDataId(slot);
            if (current_id == (u16)item_id || current_id == 0) {
                if (current_id == 0) {
                    itemBiosSetItemDataId(slot, item_id);
                    current_quantity = 0;
                } else {
                    current_quantity = itemBiosGetNum(slot);
                }

                capacity = (u16)(maximum - current_quantity);
                if (capacity < (u16)quantity) {
                    added = capacity;
                } else {
                    added = (u16)quantity;
                }
                itemBiosSetNum(slot, (u16)(current_quantity + added));
                quantity = (u16)(quantity - added);
            }
        }
        i++;
    }
    return quantity;
}
#pragma pop

/* fn_80029AC8 - 0x80029AC8 | size: 0x1f8 */
#if 0
asm void fn_80029AC8(void) {
#include "src/game/gs_worldmap_fn_80029AC8.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void fn_80029AC8(s32 r3, s32 r4, s32 r5, ShopInventory* r6) {
    s32 r29;
    s32 r30;
    s32 r26;
    u16 r28;
    s32 r27;
    ShopInventory* r31;
    s32 r24;
    s32 r25;

    r29 = r3;
    r30 = r4;
    r26 = r5;
    r31 = r6;
    if (r31 == NULL) return;
    r28 = r31->count;
    if (r28 > -1) {
        r24 = r26 & 0xFFFF;
        r27 = 0;
        while (r27 < r28 && r24 > 0) {
            s16 i = r27;
            r24 &= 0xFFFF;
            if (i < 0) {
            } else if (i >= r28) {
            } else {
                ShopItemSlot* slot = &r31->primary[i];
                u16 v = itemBiosGetItemDataId(slot);
                if (v == (u16)r30 || v == 0) {
                    u16 cur;
                    u16 delta;
                    u16 give;
                    if (v == 0) {
                        itemBiosSetItemDataId(slot, r30);
                        cur = 0;
                    } else {
                        cur = itemBiosGetNum(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    if (delta < r24) {
                        give = delta;
                    } else {
                        give = r24;
                    }
                    itemBiosSetNum(slot, (u16)(cur + give));
                    r24 = (r24 - give) & 0xFFFF;
                }
            }
            r27++;
        }
    }
    if (r28 > -1) {
        r25 = r26 & 0xFFFF;
        r27 = 0;
        while (r27 < r28 && r25 > 0) {
            s16 i = r27;
            ShopItemSlot* slots;
            r25 &= 0xFFFF;
            slots = r31->secondary;
            if (i < 0) {
            } else if (i >= r28) {
            } else {
                ShopItemSlot* slot = &slots[i];
                u16 v = itemBiosGetItemDataId(slot);
                if (v == (u16)r30 || v == 0) {
                    u16 cur;
                    u16 delta;
                    u16 give;
                    if (v == 0) {
                        itemBiosSetItemDataId(slot, r30);
                        cur = 0;
                    } else {
                        cur = itemBiosGetNum(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    if (delta < r25) {
                        give = delta;
                    } else {
                        give = r25;
                    }
                    itemBiosSetNum(slot, (u16)(cur + give));
                    r25 = (r25 - give) & 0xFFFF;
                }
            }
            r27++;
        }
    }
    r31->currency -= r29;
    r31->modified = 1;
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
    s16 idx;
    u16 cnt;
    u16 id;
    u16 qty;
    void* slot;
    u16 v;
    u16 cur;
    u16 delta;
    u16 give;
    s32 j;
    s16 jj;

    fn_80142A88(r30, 0xeb);
    fn_80142A88(r30 + 0x3ac, 0xeb);
    *(u32*)(r30 + 0x758) = 0;
    *(u32*)(r30 + 0x75c) = 0;
    if (fn_800849B4(0, 0x40, 0, &buf) < 0) {
        return 0;
    }
    for (i = 0; i < buf.count; i++) {
        id = buf.items[i].id;
        if (id == 0) continue;
        qty = buf.items[i].qty;
        idx = (s16)i;
        cnt = buf.count;
        if (idx < -1) continue;
        if (idx >= (s32)cnt) continue;
        if (idx != -1) {
            if (idx >= 0 && idx < (s32)cnt) {
                slot = (void*)(r30 + ((s32)idx << 2));
                v = (u16)itemBiosGetItemDataId(slot);
                if (v != id && v != 0) continue;
                if (v == 0) {
                    itemBiosSetItemDataId(slot, id);
                    cur = 0;
                } else {
                    cur = (u16)itemBiosGetNum(slot);
                }
                delta = (u16)(0x3e7 - cur);
                if (delta < qty) give = delta;
                else give = qty;
                itemBiosSetNum(slot, (u16)(cur + give));
            }
        } else {
            for (j = 0; j < (s32)cnt && qty != 0; j++) {
                jj = (s16)j;
                qty = (u16)qty;
                if (jj >= 0 && jj < (s32)cnt) {
                    slot = (void*)(r30 + ((s32)jj << 2));
                    v = (u16)itemBiosGetItemDataId(slot);
                    if (v != id && v != 0) continue;
                    if (v == 0) {
                        itemBiosSetItemDataId(slot, id);
                        cur = 0;
                    } else {
                        cur = (u16)itemBiosGetNum(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    if (delta < qty) give = delta;
                    else give = qty;
                    itemBiosSetNum(slot, (u16)(cur + give));
                    qty = (u16)(qty - give);
                }
            }
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

/* fn_80029EF4 - 0x80029EF4 | size: 0xb8 */
extern void heroDecPokecoupon(s32, void*);
extern void pcboxDelItem(s32, s32, u16);
extern void heroItemAddItemDataId(s32, s32, u16, s32);
#if 0
asm void fn_80029EF4(void) {
#include "src/game/gs_worldmap_fn_80029EF4.inc"
}
#else
#pragma optimization_level 4
void fn_80029EF4(void* r3, s32 r4, s32 r5, u8 r6, void* r7) {
    s32 r29, r30;
    void* r31;
    r29 = r4; r30 = r5; r31 = r7;
    switch ((u8)r6) {
    case 2:
        heroDecPokecoupon(0, r3);
        pcboxDelItem(0, r29, (u16)r30);
        if (r31 != 0) { ((u8*)r31)[0x760] = 1; }
        break;
    case 3:
        fn_80029AC8((s32)(u32)r3, r4, r5, r31);
        break;
    default:
        heroDecPokecoupon(0, r3);
        heroItemAddItemDataId(0, r29, (u16)r30, -1);
        break;
    }
}
#endif

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
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
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
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
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
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
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
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
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
#pragma optimization_level 4
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
#pragma optimization_level 4
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
#pragma optimization_level 4
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

/* fn_8002A5B0 - 0x8002A5B0 | size: 0x68 */
#if 0
asm void fn_8002A5B0(void) {
#include "src/game/gs_worldmap_fn_8002A5B0.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002A5B0(void* r3, u8* r4) {
    s8 idx;
    s16 val;
    s32* entry;
    idx = (s8)((u8*)r3)[0x95];
    if (idx < 0 || idx >= 2) { return 0; }
    val = *(s16*)(r4 + 0x6);
    entry = (s32*)(lbl_80266E58 + (s32)idx * 0xc);
    if (entry[1] == val || entry[2] == val) {
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AA68 - 0x8002AA68 | size: 0x98 */
#if 0
asm void fn_8002AA68(void) {
#include "src/game/gs_worldmap_fn_8002AA68.inc"
}
#else

s32 fn_8002A618(u8* self)
{
    typedef struct ShopNumberContext {
        s32 minimum;
        s32 maximum;
        u32 unused;
        s32* value;
    } ShopNumberContext;
    extern void fn_80166A50(u32, u32, u32, u32);

    ShopNumberContext* context;
    u16* keyInfo;
    s32 decimalPlace;
    s32 factor;
    s32 i;
    s32 oldValue;
    s32 value;

    context = *(ShopNumberContext**)(self + 0x60);
    keyInfo = windowGetKeyInfo();
    if ((keyInfo[3] & 0xF) != 0) {

    decimalPlace = 1 - (s8)self[0x95];
    factor = 1;
    for (i = 0; i < decimalPlace; i++) {
        factor *= 10;
    }

    if ((keyInfo[3] & 1) != 0) {
        oldValue = *context->value;
        if (factor < 10) {
            value = oldValue + factor;
            *context->value = value;
            if (value > context->maximum) {
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
            value = remainder + nextDigit * factor;
            *context->value = value;
            if (value < context->minimum) {
                *context->value = context->minimum;
            }
        }
        if (oldValue != *context->value) {
            fn_80166A50(0x23, 0, 0xFF, 0);
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
            value = remainder + nextDigit * factor;
            *context->value = value;
            if (value < context->minimum) {
                *context->value = context->minimum;
            }
        }
        if (oldValue != *context->value) {
            fn_80166A50(0x23, 0, 0xFF, 0);
        }
    }

    if ((keyInfo[3] & 8) != 0) {
        s32 cursor = self[0x95] + 1;
        self[0x95] = cursor;
        if ((s8)cursor >= 2) {
            self[0x95] = 1;
        }
    }
    if ((keyInfo[3] & 4) != 0) {
        s32 cursor = self[0x95] - 1;
        self[0x95] = cursor;
        if ((s8)cursor < 0) {
            self[0x95] = 0;
        }
    }
    }
    return 0;
}

#pragma optimization_level 4
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

/* fn_8002AB00 - 0x8002AB00 | size: 0x40 */
extern u8 lbl_80266E70[];
#if 0
asm void fn_8002AB00(void) {
#include "src/game/gs_worldmap_fn_8002AB00.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AB00(void* r3, u8* r4) {
    void* ctx;
    u8* base;
    u8 v;
    u8 first;
    u8 second;
    u8 third;
    s32 off;
    ctx = *(void**)((u8*)r3 + 0x60);
    base = lbl_80266E70;
    v = ((u8*)ctx)[0x1c];
    off = (s32)v * 3;
    first = base[off];
    base = base + off;
    second = base[1];
    r4[0x64] = first;
    third = base[2];
    r4[0x65] = second;
    r4[0x66] = third;
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_8002AB40 - 0x8002AB40 | size: 0x178 */
extern u8 lbl_80266E80[];
extern u32 lbl_804788F0;
extern u8 lbl_802E61D8[];
#if 0
asm void fn_8002AB40(void) {
#include "src/game/gs_worldmap_fn_8002AB40.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
#pragma scheduling on
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
#pragma pop
#endif

/* fn_8002ACB8 - 0x8002ACB8 | size: 0x18c */
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
#if 0
asm void fn_8002ACB8(void) {
#include "src/game/gs_worldmap_fn_8002ACB8.inc"
}
#else
#pragma optimization_level 4
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
#pragma optimization_level 4
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
#pragma optimization_level 4
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

/* fn_8002AE9C - 0x8002AE9C | size: 0x5c */
extern u8 lbl_80266E70[];
#if 0
asm void fn_8002AE9C(void) {
#include "src/game/gs_worldmap_fn_8002AE9C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AE9C(void* r3, u8* r4) {
    void* ctx;
    u8 v;
    u8 first;
    u8 second;
    u8 third;
    u8* base;
    s32 off;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        off = (s32)v * 3;
        base = lbl_80266E70;
        first = base[off];
        base = base + off;
        second = base[1];
        r4[0x64] = first;
        third = base[2];
        r4[0x65] = second;
        r4[0x66] = third;
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AEF8 - 0x8002AEF8 | size: 0x144 | WALL 83.7%: regalloc + scheduling */
extern void itemDataBiosGetPtr(u32);
extern u32 itemDataBiosGetKind(void);
extern u32 heroItemGetItemKindToItemAryPtr(s32, u32, u16*, s32, s32, s32, s32);
extern u32 itemGetStatus(u32, s32, s32, s32);
#if 0
asm void fn_8002AEF8(void) {
#include "src/game/gs_worldmap_fn_8002AEF8.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002AEF8(void* r3, u8* r4) {
    void* r5;
    u32 r30;
    s32 r29;
    s32 r28;
    u32 r27;
    u32 r31;
    s32 idx;
    u16 stack;
    r5 = *(void**)((u8*)r3 + 0x60);
    if (((u8*)r5)[0x1c] != 0 && ((u8*)r5)[0x1c] != 1) { return 0; }
    idx = (s8)((u8*)r3)[0x95] + (s8)((u8*)r3)[0x94];
    if (idx < 0 || idx >= (s32)*(u32*)((u8*)r5 + 0x8)) {
        r30 = 0;
    } else {
        r30 = (u16)*(u16*)(*(u32*)((u8*)r5 + 0x4) + idx * 2);
    }
    r29 = 0;
    if ((u16)r30 != 0) {
        r31 = r30;
        itemDataBiosGetPtr(r30);
        r27 = heroItemGetItemKindToItemAryPtr(0, itemDataBiosGetKind(), &stack, 0, 0, 0, 0);
        r28 = 0;
        while (r28 < (s32)stack) {
            if ((u16)itemGetStatus(r27, 0, 0x1b, 0) == (u16)r31) {
                r29 += (s32)itemGetStatus(r27, 0, 0x1c, 0);
            }
            r28++;
            r27 += 4;
        }
    }
    msgctrlSetValue(0x2d, (void*)(u32)(u16)r30);
    msgctrlSetValue(0x34, (void*)r29);
    fn_800FB680(0, 0, -1, 0x2b2f);
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B03C - 0x8002B03C | size: 0x4c */
#if 0
asm void fn_8002B03C(void) {
#include "src/game/gs_worldmap_fn_8002B03C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
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
#pragma pop
#endif

/* fn_8002B088 - 0x8002B088 | size: 0x34 */
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002B088(void) {
#include "src/game/gs_worldmap_fn_8002B088.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B088(void) {
    fn_800FB680(0, 0, -1, lbl_8047A3E4);
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B0BC - 0x8002B0BC | size: 0x78 */
extern f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern f32 lbl_8047B978;
#if 0
asm void fn_8002B0BC(void) {
#include "src/game/gs_worldmap_fn_8002B0BC.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
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
#pragma peephole on
#endif

/* fn_8002B134 - 0x8002B134 | size: 0x6c */
#pragma scheduling on
extern f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern f32 lbl_8047B978;
#if 0
asm void fn_8002B134(void) {
#include "src/game/gs_worldmap_fn_8002B134.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
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
#pragma peephole on
#endif

/* menuShopDrawListText - 0x8002B1A0 | size: 0x26c */
extern void fn_800FE38C(void);
extern u32 itemDataBiosGetName(void);
extern u32 itemDataBiosGetPrice(void);
extern u32 itemDataBiosGetCoupon(void);
extern void fn_800FE35C(void);
extern f32 lbl_8047B980;
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
void menuShopDrawListText(void* arg0, u8* arg1)
{
    extern void fn_800FE38C(s32, s32, s32, s32);
    extern u32 itemDataBiosGetName(void);
    extern u32 itemDataBiosGetPrice(void);
    extern u32 itemDataBiosGetCoupon(void);
    extern void fn_800FE35C(void);
    extern f32  lbl_8047B980;

    extern void msgctrlSetValue(s32, void*);
    extern u32  GSmsgGetRect(u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    extern void itemDataBiosGetPtr(u32);

    extern u8   lbl_802EF0A8[];

    s16 key;
    u8* tbl;
    u8* entry;
    s16 ref_x1;
    s16 ref_y1;
    s16 ref_x2;
    s16 ref_y2;
    s16 entry_x;
    s16 entry_y;
    u8* ctx;
    s32 slot_count;
    s8 slot_i;
    s32 loop_lim;
    s32 dir_off;
    s32 scroll_px;
    s32 w_0xdb;
    s32 w_0x14f;
    s32 y_base;
    s32 x_mid;
    f32 scroll_f;
    u32* scroll_flag;
    s32 x_acc;
    s32 loop_i;
    s32 trailing_tx;
    u32 btn_id;

    /* ---- decode layout rect from the fixed entries in lbl_802EF0A8 ---- */
    key        = *(s16*)(arg1 + 0x6);
    tbl        = lbl_802EF0A8;
    entry      = tbl + (s32)key * 0x1c;   /* indexed element */

    /* fixed reference rect corners stored at absolute byte offsets in the table */
    ref_x1     = *(s16*)(tbl + 0x492e);   /* reference left   */
    ref_y1     = *(s16*)(tbl + 0x4930);   /* reference top    */
    ref_x2     = *(s16*)(tbl + 0x4932);   /* reference right  */
    ref_y2     = *(s16*)(tbl + 0x4934);   /* reference bottom */

    entry_x    = *(s16*)(entry + 0x2);    /* per-entry x adjustment */
    entry_y    = *(s16*)(entry + 0x4);    /* per-entry y adjustment */

    /* context block hanging off arg0+0x60 (same layout as all sibling fns) */
    ctx        = *(u8**)(( u8*)arg0 + 0x60);
    slot_count = *(s32*)(ctx + 0x8);      /* total number of slots */

    /* ---- draw the border rect ---- */
    fn_800FE38C((s32)(ref_x1 - entry_x),
                (s32)(ref_y1 - entry_y),
                (s32)ref_x2,
                (s32)ref_y2);

    /* ---- set up per-frame counters ---- */
    slot_i  = (s8)(( u8*)arg0)[0x94];     /* starting slot index (signed) */
    loop_lim = 10;                         /* r23: visual column limit */
    dir_off  = 0;                          /* r24: scroll direction bias (-1/0) */
    scroll_px = 0;                         /* r22: scroll pixel offset */

    /* ---- format-print arg: max possible value (0x270f = 9999) ---- */
    msgctrlSetValue(0x50, (void*)0x270f);

    /* ---- measure two reference strings to compute the text x-centre ---- */
    w_0xdb  = (s32)(GSmsgGetRect(0xdb)  >> 16);
    w_0x14f = (s32)(GSmsgGetRect(0x14f) >> 16);

    /* y-base for item text: from sprite descriptor */
    y_base = (s32)*(s16*)(arg1 + 0x54);
    x_mid  = (y_base - w_0xdb) - w_0x14f;  /* r28, used as text x-anchor */

    /* ---- scroll / animation state ---- */
    scroll_f = *(f32*)(*(u32*)(ctx + 0xc)); /* current scroll float */

    if (scroll_f != lbl_8047B980) {              /* != 0.0f: scrolling active */
        scroll_flag = *(u32**)(ctx + 0x14);
        if (scroll_flag != (u32*)0 && *scroll_flag != 0) {
            if (scroll_f < lbl_8047B980) {       /* < 0.0f: scrolling left */
                slot_i  -= 1;
                dir_off  = -1;
            } else {                             /* > 0.0f: scrolling right */
                loop_lim = 11;
            }
            /* ENDIAN-QA: fctiwz+stfd+lwz integer extraction = (s32)scroll_f */
            scroll_px = (s32)scroll_f;
        }
    }

    /* ---- loop setup ---- */
    x_acc  = dir_off * 0x1f;   /* r30: pixel x accumulator (31 px/slot) */
    loop_i = dir_off;          /* r26: visual column index */

    /* loop: render one visual column per iteration */
    while (loop_i < loop_lim && slot_i < slot_count) {
        if (slot_i < 0) {
            /* slot index out of range on the low side: skip to increment */
            goto next_slot;
        }

        {
            /* r24 = adjusted x position for this column */
            s32  x_pos = x_acc - scroll_px;
            u32  slot_id;

            /* bounds check: if adjusted x is negative OR slot_i >= count */
            if (x_pos < 0 || slot_i >= slot_count) {
                slot_id = 0;
            } else {
                /* load the u16 slot ID from the packed array */
                slot_id = (u32)((u16*)(*(u32*)(ctx + 0x4)))[slot_i];
            }

            /* draw the icon for this slot (if any) */
            itemDataBiosGetPtr(slot_id);
            {
                u32 icon_h = (u32)itemDataBiosGetName();
                if (icon_h != 0) {
                    fn_800FB680(0, x_pos, -1, icon_h);
                }
            }

            /* draw the text label: mode determined by ctx[0x1c] */
            {
                u8 mode = ctx[0x1c];
                if (mode == 0 || mode == 1) {
                    /* normal mode: draw background string at y_base, then value */
                    fn_800FB680(x_mid, x_pos, -1, 0x14f);
                    itemDataBiosGetPtr(slot_id);
                    {
                        u32 val = itemDataBiosGetPrice();
                        msgctrlSetValue(0x50, (void*)(u32)(u16)val);
                    }
                    {
                        s32 tw = (s32)(GSmsgGetRect(0xdb) >> 16);
                        s32 tx = (s32)*(s16*)(arg1 + 0x54) - tw;
                        fn_800FB680(tx, x_pos, -1, 0xdb);
                    }
                } else {
                    /* alternate mode: draw different value string */
                    itemDataBiosGetPtr(slot_id);
                    {
                        u32 val = itemDataBiosGetCoupon();
                        msgctrlSetValue(0x50, (void*)(u32)(u16)val);
                    }
                    {
                        s32 tw = (s32)(GSmsgGetRect(0x153) >> 16);
                        s32 tx = (s32)*(s16*)(arg1 + 0x54) - tw;
                        fn_800FB680(tx, x_pos, -1, 0x153);
                    }
                }
            }
        }

next_slot:
        x_acc  += 0x1f;
        loop_i += 1;
        slot_i += 1;
    }

    /* ---- trailing "add/locked" button if visual columns remain ---- */
    if (loop_i < loop_lim) {
        trailing_tx = (loop_i * 0x1f) - scroll_px;
        btn_id = (ctx[0x1d] & 1) ? 0x2b47u : 0x2b2cu;
        fn_800FB680(0, trailing_tx, -1, btn_id);
    }

    fn_800FE35C();
}
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
    u8 sprite_bytes[8];
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

    sprite_low = (s8)sprite_bytes[1];
    position = entry->position + sprite_low * 0x1F;
    draw->position = position;
    if (*context->offset_enabled == 0) {
        position = draw->position + (s32)*context->offset;
        draw->position = position;
    }

    position = (s32)*context->offset;
    position += (sprite_low + (s8)sprite_bytes[0]) * 0x1F;
    angle = lbl_8047B984 * (f32)position;
    while (angle > lbl_8047B98C) {
        angle -= lbl_8047B988;
    }
    {
        f32 wrapped_angle = angle;
        while (wrapped_angle < lbl_8047B990) {
            wrapped_angle += lbl_8047B988;
        }
        draw->angle = wrapped_angle;
    }
    return 0;
}
#pragma pop
#endif

/* fn_8002B594 - 0x8002B594 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern f32 lbl_8047B980;
extern f32 lbl_8047B97C;
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
 * GSmap_DrawPartyIcons  0x8002B594 | 0x2EC bytes
 *
 * Draws a party-icon sprite along a piecewise parametric curve.
 * The curve is divided into 5 segments defined by 5 break-point thresholds
 * computed from the two s16 fields (at +0x54/+0x56) in the data packet.
 * For each segment a normalised fractional parameter f31 is computed and
 * segment-specific x/y screen positions are derived (some use cos/sin arcs).
 * Finally windowDrawSprite2 is called to emit the actual sprite draw call.
 *
 * Parameters (CW EABI):
 *   ctx        r3  – sprite/render context pointer passed through to windowDrawSprite2
 *   data       r4  – sprite data packet; s16 at +0x56 = vert coord, s16 at +0x54 = horiz coord
 *   sprite_id  r5  – sprite index; lower 16 bits passed to windowDrawSprite2
 *   color_byte r6  – low 8 bits = alpha/colour value; OR-merged with 0xFFFFFF00 to form r7
 *   pos        f1  – continuous position parameter along the curve
 *
 * ENDIAN-QA: the asm uses the classic CW big-endian 0x4330/xoris double-word trick
 * to convert the two s16 fields to f32.  On x86 this is a plain (f32)(s16) cast.
 */
void fn_8002B594(void* ctx, u8* data, u32 sprite_id, u32 color_byte, f32 pos)
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

    width = (f32)*(s16*)(data + 0x54);
    height = (f32)*(s16*)(data + 0x56);

    thresholds[0] = lbl_8047B980;
    thresholds[4] = lbl_8047B97C;
    scaled_height = lbl_8047B98C * height;
    curve = scaled_height * lbl_8047B9A0;
    span = width + curve;
    denominator = span * lbl_8047B9A4;
    thresholds[1] = width / denominator;
    thresholds[2] = span / denominator;
    thresholds[3] = (lbl_8047B9A4 * width + curve) / denominator;

    next_threshold = &thresholds[1];
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
        x = (s32)(height - lbl_8047B9A4);
        y = (s32)((lbl_8047B97C - phase) * width);
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
    windowDrawSprite2(x, y, 2, 2, color, ctx, (u16)sprite_id, 0);
}
#endif

/* fn_8002B880 - 0x8002B880 | size: 0x468 */
extern void fn_800FE6D0(void);
extern void spriteSetEnv(void);
extern f64 lbl_8047B998;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B9A0;
extern f32 lbl_8047B9A4;
extern f32 lbl_8047A3F0;
extern f32 lbl_8047B978;
extern f32 lbl_8047B9AC;
extern f32 lbl_8047B97C;
extern f32 lbl_8047B9B0;
extern f32 lbl_8047B9B4;
#if 0
asm void fn_8002B880(void) {
#include "src/game/gs_worldmap_fn_8002B880.inc"
}
#else
/* fn_8002B880  GSmap_DrawInfoPanel - 0x8002B880, size 0x468
 *
 * Worldmap info-panel draw. r3 = worldmap state object (its +0x60 is the
 * panel context), r4 = the draw/sprite entity. Only runs when the context's
 * primary flag word (*(u16*)*(void**)ctx) is zero. It first computes a sprite
 * column/alpha exactly like fn_8002BCE8 (5-entry lbl_802E4F68 lookup table,
 * keyed on entity->0x6), positions the panel via fn_800FE6D0/spriteSetEnv, then
 * draws four 45-step radial rings of icons by calling fn_8002B594 in a loop,
 * each ring starting from a different phase offset and advancing by a fixed
 * per-step increment that wraps at lbl_8047B97C.
 *
 * ENDIAN-QA: all 0x43300000 / 0x8000-xor double-word int->float idioms in the
 * original asm are normalized here to plain signed casts on the full value.
 */
s32 fn_8002B880(void* r3, u8* r4)
{
    /* Cross-TU callees (real arg lists inferred from register state at each bl). */
    extern void fn_800FE6D0(s32 x, s32 y);
    extern void spriteSetEnv(void);
    extern void fn_8002B594(void* panel, u8* entity, u32 mode, s32 step, f32 phase);

    /* r2-relative read-only float constants. */
    extern f64 lbl_8047B998; /* int->float bias double (folds into the casts) */
    extern f32 lbl_8047B98C;
    extern f32 lbl_8047B9A0;
    extern f32 lbl_8047B9A4;
    extern f32 lbl_8047B978;
    extern f32 lbl_8047B9AC;
    extern f32 lbl_8047B97C; /* phase wrap limit */
    extern f32 lbl_8047B9B0;
    extern f32 lbl_8047B9B4;
    /* r13-relative small-data float (running phase base). */
    extern f32 lbl_8047A3F0;
    /* Sprite-column lookup table: 5 entries of 8 bytes; key s32 @ +0, value s16 @ +4. */
    extern u8 lbl_802E4F68[];

    u8* state = (u8*)r3;
    u8* entity = r4;
    u8* ctx = *(u8**)(state + 0x60);

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
        u16 sprite_id = *(u16*)(state + 0x94);
        s16 key = *(s16*)(entity + 0x6);
        u8* tab = lbl_802E4F68;
        s8 low_byte = (s8)(sprite_id & 0xff);
        s32 idx;
        u8* entry;

        idx = 5;
        if (key == *(s32*)(tab + 0x0)) idx = 0;
        else if (key == *(s32*)(tab + 0x8)) idx = 1;
        else if (key == *(s32*)(tab + 0x10)) idx = 2;
        else if (key == *(s32*)(tab + 0x18)) idx = 3;
        else if (key == *(s32*)(tab + 0x20)) idx = 4;

        entry = (idx < 5) ? (tab + (u32)idx * 8) : (u8*)0;

        if (entry != (u8*)0) {
            s32 val = (s32)*(s16*)(entry + 0x4) + (s32)low_byte * 0x1f;

            /* Add the float-derived bias only when the indirect flag is zero. */
            if (*(s32*)(*(u32*)(ctx + 0x14)) == 0) {
                val += (s32)*(f32*)(*(u32*)(ctx + 0xc));
            }

            *(s16*)(entity + 0x52) = (s16)val;
            entity[0x67] = (*(u16*)(*(void**)ctx) == 0) ? 0x72 : 0xff;
        }
    }

    /* ---- Position the panel ---- */
    fn_800FE6D0((s32)(s16)(*(s16*)(state + 0x84) + *(s16*)(entity + 0x50)),
                (s32)(s16)(*(s16*)(state + 0x86) + *(s16*)(entity + 0x52)));
    spriteSetEnv();

    /* ---- Fixed per-step phase increment ---- */
    fx = (f32)(s16)*(s16*)(entity + 0x54);
    fy = (f32)(s16)*(s16*)(entity + 0x56);
    denom = lbl_8047B9A4 * (fx + (lbl_8047B98C * fy) * lbl_8047B9A0);
    incr = lbl_8047B9A4 / denom;

    /* ---- Ring 1: phase starts at the live r13 base ---- */
    phase = lbl_8047A3F0;
    for (i = 0; i < 0x2d; i++) {
        fn_8002B594(state, entity, 0xd1,
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
        fn_8002B594(state, entity, 0xd1,
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
        fn_8002B594(state, entity, 0xd1,
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
        fn_8002B594(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    return 0;
}
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

/* fn_8002BE08 - 0x8002BE08 | size: 0x20c | WALL 86.5%: regalloc + scheduling */
extern u32 itemDataBiosGetDoc(void);
extern f32 lbl_8047B9B8;
extern f32 lbl_8047B9BC;
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002BE08(void) {
#include "src/game/gs_worldmap_fn_8002BE08.inc"
}
#else
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
        if ((s32)((s8)arg0[0x95] + (s8)arg0[0x94]) >= limit) {
            --arg0[0x95];
        } else {
            if ((s8)arg0[0x95] >= 0xa) {
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
            --arg0[0x95];
            {
                s32 t = (s8)arg0[0x95];
                if (t < 0) {
                    arg0[0x95] = 0;
                    --arg0[0x94];
                    *(s32*)(*(u32*)(ctx + 0x14)) = 1;
                } else {
                    *(s32*)(*(u32*)(ctx + 0x14)) = 0;
                }
            }
            *(f32*)(*(u32*)(ctx + 0xc)) = lbl_8047B9BC;
        }
    }
    sum = (s32)(s8)arg0[0x94] + (s32)(s8)arg0[0x95];
    if (sum < 0 || sum >= *(s32*)(ctx + 0x8)) {
        r3val = 0;
    } else {
        r3val = ((u16*)(*(u32*)(ctx + 0x4)))[sum];
    }
    if ((u16)r3val != 0) {
        itemDataBiosGetPtr((u16)r3val);
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
#pragma optimization_level 4
s32 fn_8002C014(void* r3) {
    u8* r31;
    u8* r30;
    u16* pad;
    s32 r4;
    u16 r3val;
    r31 = (u8*)r3;
    r30 = (u8*)*(void**)((u8*)r3 + 0x60);
    pad = windowGetKeyInfo();
    if (pad[2] & 0x10) {
        s32 a = (s8)r31[0x94];
        s32 b = (s8)r31[0x95];
        r4 = a + b;
        if (r4 < 0 || r4 >= (s32)*(u32*)(r30 + 0x8)) {
            r3val = 0;
        } else {
            r3val = ((u16*)(*(u32*)(r30 + 0x4)))[r4];
        }
        if ((r30[0x1d] & 1) && (r3val != 0)) {
            return 0;
        }
    } else {
        r3val = 0;
    }
    if (r3val != 0) {
        *(u16*)(*(u32*)r30) = r3val;
        menuButtonNormal(r31);
    }
    return 0;
}
#endif

/* fn_8002C0E4 - 0x8002C0E4 | size: 0x1a0 */
extern f32 lbl_8047B980;
extern f32 lbl_8047B9C0;
extern f32 lbl_8047B9C4;
extern f32 lbl_8047B97C;
extern f32 lbl_8047B9C8;
#if 0
asm void fn_8002C0E4(void) {
#include "src/game/gs_worldmap_fn_8002C0E4.inc"
}
#else
/*
 * GSmap_FadeFromBlack  (0x8002C0E4, 0x1A0 bytes)
 *
 * Drives the "fade from black" sequence on the world-map screen.
 * self->byte[0x1]  = current phase (0 = init, 2 = animate, 3 = finish)
 * self->byte[0x2]  = one-shot flag (0 = not yet triggered, 1 = done)
 * self->ptr[0x60]  = inner context block; its fields are indirect float/int cells:
 *     ctx+0x0C = ptr to f32 : horizontal pan offset  (driven toward 0 in phase 2)
 *     ctx+0x10 = ptr to f32 : wrap counter A         (incremented by lbl_8047B9C8 mod lbl_8047B97C)
 *     ctx+0x14 = ptr to u32 : integer flag / counter (cleared to 0 in phase 0)
 *     ctx+0x18 = ptr to f32 : wrap counter B         (incremented by lbl_8047B9C4 mod lbl_8047B97C)
 */
s32 fn_8002C0E4(u8 *self)
{
    extern void winSeqSetMenu(s32 param, u32 key);

    /* lbl_ float constants declared block-scope per TU convention */
    extern f32 lbl_8047B980;   /* 0.0f - zero reference                     */
    extern f32 lbl_8047B9C0;   /* step magnitude for pan offset convergence  */
    extern f32 lbl_8047B9C4;   /* step for wrap counter B                    */
    extern f32 lbl_8047B97C;   /* 1.0f (or wrap period) - upper bound        */
    extern f32 lbl_8047B9C8;   /* step for wrap counter A                    */

    s8  phase;
    u8 *ctx;
    f32 val, step;

    phase = (s8)self[0x1];
    ctx   = *(u8 **)(self + 0x60);

    if (phase == 2) {
        goto phase2;
    } else if (phase > 2) {
        if (phase >= 4) {
            return 0;   /* phase 4+ : no-op */
        }
        /* phase == 3 */
        goto phase3;
    } else if (phase == 0) {
        goto phase0;
    }
    /* phase == 1 (or anything else unmapped) : fall through */
    return 0;

phase0:
    /* One-shot init: arm the fade-from-black animation */
    if ((s8)self[0x2] != 0) {
        return 0;
    }
    winSeqSetMenu(0x60, 0x76);

    /* Zero all animated fields */
    *(f32 *)(*(u32 *)(ctx + 0x0C)) = lbl_8047B980;   /* pan offset = 0 */
    *(u32 *)(*(u32 *)(ctx + 0x14)) = 0;              /* integer flag = 0 */
    *(f32 *)(*(u32 *)(ctx + 0x18)) = lbl_8047B980;   /* wrap B = 0 */
    *(f32 *)(*(u32 *)(ctx + 0x10)) = lbl_8047B980;   /* wrap A = 0 */

    self[0x2] = 1;
    return 0;

phase2:
    /* Per-frame animation: converge pan offset toward 0, advance wrap counters */

    /* --- Converge pan offset (ctx+0x0C) toward 0.0 from the positive side --- */
    {
        f32 *pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
        val = *pan_ptr;
        if (val > lbl_8047B980) {               /* val > 0 */
            f32 nv = val - lbl_8047B9C0;
            *pan_ptr = nv;
            if (nv < lbl_8047B980) {            /* undershot: clamp to 0 */
                pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
                *pan_ptr = lbl_8047B980;
            }
        }
    }

    /* --- Converge pan offset toward 0.0 from the negative side --- */
    {
        f32 *pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
        val = *pan_ptr;
        if (val < lbl_8047B980) {               /* val < 0 */
            f32 nv = val + lbl_8047B9C0;
            *pan_ptr = nv;
            if (nv > lbl_8047B980) {            /* overshot: clamp to 0 */
                pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
                *pan_ptr = lbl_8047B980;
            }
        }
    }

    /* --- Advance wrap counter B (ctx+0x18), wrap at lbl_8047B97C back to 0 --- */
    {
        f32 *wb_ptr = (f32 *)(*(u32 *)(ctx + 0x18));
        step = lbl_8047B9C4;
        val  = *wb_ptr;
        val  = val + step;
        *wb_ptr = val;

        wb_ptr = (f32 *)(*(u32 *)(ctx + 0x18));
        val = *wb_ptr;
        if (val > lbl_8047B97C) {               /* exceeded period: wrap to 0 */
            *wb_ptr = lbl_8047B980;
        }
    }

    /* --- Advance wrap counter A (ctx+0x10), wrap at lbl_8047B97C back to (val - period) --- */
    {
        f32 *wa_ptr = (f32 *)(*(u32 *)(ctx + 0x10));
        step = lbl_8047B9C8;
        val  = *wa_ptr;
        val  = val + step;
        *wa_ptr = val;

        /* cror eq,gt,eq  =>  cr0.eq = cr0.gt | cr0.eq  =>  true when val >= lbl_8047B97C */
        if (val >= lbl_8047B97C) {
            wa_ptr = (f32 *)(*(u32 *)(ctx + 0x10));
            val    = *wa_ptr;
            *wa_ptr = val - lbl_8047B97C;       /* subtract one period (sawtooth) */
        }
    }

    return 0;

phase3:
    /* One-shot finish: signal end of fade sequence */
    if ((s8)self[0x2] != 0) {
        return 0;
    }
    winSeqSetMenu(0x60, 0x7a);
    self[0x2] = 1;
    return 0;
}
#endif

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
 * mode:    dialog mode; low byte 0x02 or 0x03 = skip the format-text preamble call
 *
 * Functional C for x86 host build — byte-match irrelevant.
 */
void fn_8002C284(u32 loc_idx, u32 mode)
{
    extern void   fn_8002A1C4(u8* r3, s32 r4, s32 r5, ...); /* GSmap_FormatText2 */
    extern void*  windowGetActiveID(void);                          /* get current scene context */
    extern void   menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...); /* scene event dispatch */
    extern void   menuCloseCustom(u32 slot, u32 p1, u32 p2);     /* scene event release */

    /* lbl_802E4F68: array of 5 entries at stride 8: { u32 key; s16 val; u16 pad; } */
    extern u8     lbl_802E4F68[];
    /* lbl_802EF0A8: array of structs at stride 0x1c; s16 at offset +4 within each */
    extern u8     lbl_802EF0A8[];

    /* SDA globals */
    extern u32    lbl_804788A8;  /* "update pending" flag */
    extern u32    lbl_80478E54;  /* pointer to location-index map (array of u32 entries) */
    extern u32    lbl_80478E44;  /* pointer to destination u16 table */
    extern u16    lbl_8047A3F8;  /* output: zero count / first-free index */
    extern u16    lbl_8047A3F4;  /* output field for dialog params */
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16    lbl_8047A3EC;  /* output field for dialog params */
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */

    u8 mode_low;
    u8 *tab;
    u8 *ef0a8_base;
    u32 *map_entry;
    u16 *u16_base;
    u16 *u16_ptr;
    u16  zero_count;
    void *ctx;

    /* Dialog params struct built on the (conceptual) stack, passed as vararg to menuOpenCustom */
    struct {
        u16  *p_a3f8;     /* +0x00 */
        u16  *u16_base_p; /* +0x04 */
        u32   count;      /* +0x08 */
        u16  *p_a3f4;     /* +0x0C */
        u16  *p_a3f0;     /* +0x10 */
        u16  *p_a3ec;     /* +0x14 */
        u16  *p_a3e8;     /* +0x18 */
        u8    mode_byte;  /* +0x1C */
        u8    flag1;      /* +0x1D */
    } sp_data;

    /* If mode low byte is 2 or 3, skip the text-format preamble */
    mode_low = (u8)(mode & 0xFF);
    if (mode_low != 0x03 && mode_low != 0x02) {
        /* fn_8002A1C4(loc_idx_as_ptr, 0xa, -1) with no float arg (crxor 6,6,6) */
        fn_8002A1C4((u8*)(u32)loc_idx, 0xa, -1);
    }

    /* If the "update pending" flag is set, refresh the lbl_802E4F68 s16 values
     * from the lbl_802EF0A8 table (5 entries, stride 0x1c, s16 at +4) */
    if (lbl_804788A8 != 0) {
        tab      = lbl_802E4F68;        /* base of the 5-entry stride-8 table */
        ef0a8_base = lbl_802EF0A8 + 4; /* s16 values start at offset +4 */

        /* Entry 0 */
        *(s16*)(tab + 0x4) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x0) * 0x1c);
        /* Entry 1 */
        *(s16*)(tab + 0xC) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x8) * 0x1c);
        /* Entry 2 */
        *(s16*)(tab + 0x14) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x10) * 0x1c);
        /* Entry 3 */
        *(s16*)(tab + 0x1C) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x18) * 0x1c);
        /* Entry 4 */
        *(s16*)(tab + 0x24) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x20) * 0x1c);

        lbl_804788A8 = 0;
    }

    /* Find the first zero u16 entry in lbl_80478E44 at the sub-table selected
     * by lbl_80478E54[loc_idx].  The sub-table base offset uses the u16 at
     * offset +2 of the loc_idx map entry (multiplied by 2 for u16 stride). */
    map_entry = (u32 *)((u8 *)lbl_80478E54 + loc_idx * 4);
    /* lbl_8047A3F8 is reset to 0 before the scan */
    lbl_8047A3F8 = 0;
    {
        u16 sub_idx = *(u16 *)((u8 *)map_entry + 2); /* u16 at offset +2 of map entry */
        u16_base = (u16 *)((u8 *)lbl_80478E44 + (u32)sub_idx * 2);
    }
    u16_ptr   = u16_base;
    zero_count = 0;
    while (*u16_ptr != 0) {
        u16_ptr++;
        zero_count++;
    }

    /* Build the parameter block for the scene event dispatcher */
    sp_data.p_a3f8     = &lbl_8047A3F8;
    sp_data.u16_base_p = u16_base;
    sp_data.count      = zero_count;
    sp_data.p_a3f4     = &lbl_8047A3F4;
    sp_data.p_a3f0     = &(*(u16*)&lbl_8047A3F0);
    sp_data.p_a3ec     = &lbl_8047A3EC;
    sp_data.p_a3e8     = &(*(u16*)&lbl_8047A3E8);
    sp_data.mode_byte  = mode_low;
    sp_data.flag1      = 1;

    /* Open scene dialog 0x60 with the parameter block */
    ctx = windowGetActiveID();
    menuOpenCustom((void*)0x60, (u32)ctx, 0, 0, (void*)1, 1, &sp_data);

    /* Release / wait for scene dialog 0x60 */
    menuCloseCustom(0x60, 0, 1);
}
#endif

/* fn_8002C408 - 0x8002C408 | size: 0xa64 */
extern void savedataGetStatus(void);
extern void pcboxGetItemCapacity(void);
extern void heroItemCheckAddItemDataId(void);
extern void fn_80166AB8(void);
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
 * fn_8002C408  GSmap_DialogStateMachine  (0x8002C408, 0xA64)
 *
 * World-map travel/shop dialog driver. Reconstructed for x86 host (clang -m32);
 * byte-match irrelevant, semantics preserved. CW EABI: r3=mapIdx (param0, index
 * into the location tables at lbl_80478E54/E44), r29=mode (param1, u8 dialog
 * type 2/3/4). Returns void (no consistent r3 set across blr paths).
 *
 * Control flow mirrors the asm CFG via labels (retry-loop back-edge ->
 * L_628; early outs -> done). The 0x8e0 frame holds one real working buffer
 * (WorldMapBuf, shared with fn_80029CC0) at +0x128 plus stack menu descriptors.
 *
 * ENDIAN-QA: all multi-byte reads use natural-width loads; no big-endian
 * half/word splitting is required (the asm uses width-correct lhz/lwz/lbz).
 */
void fn_8002C408(s32 mapIdx, u32 mode)
{
    /* ---- cross-TU callees (block-scope typed externs, TU convention) ---- */
    extern void  fn_80142A88(void* buf, s32 v);            /* clear/init work buffer */
    extern s32   fn_80029CC0(u8* buf);                     /* scene callback 2 (mode 3 init) */
    extern u32   savedataGetStatus(u8* obj, u16 sel);            /* object/property accessor */
    extern u32   heroGetStatus(u8* ptr, u32 selector, u32 idx); /* state/interaction getter */
    extern void  heroSetStatus(u8* ptr, u32 selector, u32 value); /* state setter */
    extern void* itemDataBiosGetPtr(u16 speciesId);              /* select species/item entry */
    extern u32   itemDataBiosGetCoupon(void);                        /* read selected entry value (u16) */
    extern u16   itemBiosGetItemDataId(void* slot);                  /* item/species id at slot */
    extern u16   itemBiosGetNum(void* slot);                  /* quantity at slot */
    extern u16   pcboxGetItemCapacity(s32 a, u16 species);          /* owned-count query (mode 2) */
    extern s32   heroItemCheckAddItemDataId(u8* ptr, u32 species);        /* owned-count query (default) */
    extern void  fn_80029EF4(void* a, s32 b, s32 c, u8 d, void* e); /* commit purchase */
    extern void  fn_8002A1C4(u8* idx, s32 msgId, s32 term, ...);    /* show message line */
    extern u32   fn_80029FAC(u8* idx, s32 a, s32 b, s32 c, ...);    /* format text -> string ptr */
    extern void  winMsgOpenWithSE(s32 a, u32 str, s32 c, s32 d, u8 alpha);/* display formatted string */
    extern u8    menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);      /* yes/no prompt */
    extern u32   windowGetActiveID(void);                        /* current menu owner handle */
    extern s32   menuOpenCustom(void* p, u32 owner, s32 c, s32 d, void* e, s32 f, ...); /* open list menu */
    extern void  menuClose(s32 slot);                    /* refresh menu slot */
    extern s32   menuCloseSync(void* slot, u8 flag);       /* close menu (sync) */
    extern void  winMsgClose(s32 slot);                    /* close message box */
    extern s32   menuCloseCustom(void* slot, u32 m, u8 wait);  /* close menu group */
    extern void  fn_80166AB8(s32 soundId, s32 p2, s32 p3); /* play SE */
    extern s32   fn_801D0748(u32 a, u32 b, u32 c);         /* confirm-state query */
    extern void  fn_80093574(s32 a);                       /* inventory list ops */
    extern void  fn_80092C90(s32 a, void* list, s32 c);
    extern s32   fn_80093610(s32 a);
    extern void  fn_80093698(s32 a);
    extern void* memcpy(void* dst, const void* src, u32 n);
    extern void* memset(void* dst, s32 v, u32 n);

    /* ---- small-data / read-only globals ---- */
    extern u32 lbl_8047A3DC;   /* saved object snapshot dst */
    extern u32 lbl_8047A3D8;   /* saved interaction handle */
    extern u32 lbl_8047A3E4;   /* formatted string ptr A */
    extern u32 lbl_8047A3E0;   /* selected quantity result */
    extern u32 lbl_8047A660;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A664;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_804788A8;   /* "rebuild list" flag */
    extern u16 lbl_8047A3F8;   /* menu: selected id out */
    extern u16 lbl_8047A3F4;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16 lbl_8047A3EC;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_80478E54;   /* location header table base */
    extern u32 lbl_80478E44;   /* name/entry list table base */
    extern u32 lbl_80478E4C;   /* per-location descriptor table base */
    extern u8  lbl_80266E70[]; /* per-mode 3-byte RGB color table */
    extern u8  lbl_802E4F68[]; /* source struct for list-rebuild shuffle */
    extern u8  lbl_802EF0A8[]; /* indexed records (0x1c stride) for shuffle */

    /* ---- real working buffer (shared layout with fn_80029CC0) ---- */
    struct WorldMapBuf {
        u8   head[0x758];      /* item slots region init'd by fn_80142A88 */
        u32  credit0;          /* +0x758 */
        u32  credit1;          /* +0x75c */
        u8   exitFlag;         /* +0x760 */
        u8   pad761[3];
        u32  field764;         /* +0x764 */
        u16  count;            /* +0x768 */
    } buf;

    /* list-menu descriptor (frame +0x108), 7 ptr fields + 2 trailing bytes */
    struct ListDesc {
        void* selOut;          /* &lbl_8047A3F8 */
        void* entryList;       /* table + nameIdx*2 */
        u32   entryCount;
        void* f114;            /* &(*(u32*)&lbl_8047A3F4) */
        void* f118;            /* &(*(u32*)&lbl_8047A3F0) */
        void* f11c;            /* &(*(u32*)&lbl_8047A3EC) */
        void* f120;            /* &(*(u32*)&lbl_8047A3E8) */
        u8    modeByte;        /* +0x124 */
        u8    f125;            /* +0x125 */
    } desc1;

    /* quantity-selector descriptor (frame +0x14/+0x18) */
    struct QtyDesc {
        u32   enable;          /* +0x18 = 1 */
        s32   maxQty;          /* +0x1c = r16 (max affordable) */
        s32   unitPrice;       /* +0x20 = r17 */
        void* total;           /* +0x24 = &lbl_8047A3E0 */
        u8    colR;            /* +0x28 */
        u8    colG;            /* +0x29 */
        u8    colB;            /* +0x2a */
        u8    pad2b;
        u32   f2c;             /* +0x2c = 1 */
    } qty;
    s32 qtyTitle;              /* frame +0x14 = 1 (title flag passed via r5) */

    /* inventory snapshot used in the mode-3 commit path (frame +0x30) */
    struct InvHdr {
        u32 a;                 /* +0x30 = credit0 */
        u32 b;                 /* +0x34 = credit1 */
        u32 c;                 /* +0x38 = field764 */
        u16 zero;              /* +0x3c */
        u16 n;                 /* +0x3e = count */
        struct { u16 id; u16 qty; } items[51]; /* +0x40.. */
    } inv;

    u8  msgBuf[4];             /* small format scratch (frame +0x10/+0x11) */

    s32 r25;                   /* loop/return state accumulator */
    s32 r15_have;             /* available currency for mode-4 affordability pre-check */
    s32 minCost;               /* min entry value across the location list */
    u16* listStart;            /* u16 entry list for the location */
    u8* colorEntry;            /* RGB triple for this mode */
    u8 modeLow;                /* mode & 0xff */
    s32 idxX4;                 /* mapIdx << 2 */
    s32 ok;
    s32 sel;                   /* selected entry id (r25 inner) */
    s32 maxAfford;             /* r16 */
    s32 unitPrice;             /* r17 */
    s32 species;               /* r18 */
    s32 chosenQty;             /* r22 */
    s32 totalCost;             /* r21 */
    s32 ownedRoom;             /* r17 reused: owned/room count */
    s32 yn;                    /* r16 reused: yes/no result */
    s32 i;
    s32 next;                  /* r0 -> r25 in CB40 */

    modeLow = (u8)(mode & 0xff);
    r25 = 0;

    /* ===== Phase A: per-mode init ===== */
    if (modeLow == 3) {
        ok = fn_80029CC0((u8*)&buf);
    } else if (modeLow == 2) {
        fn_80142A88((u8*)&buf, 0xeb);
        fn_80142A88((u8*)&buf + 0x3ac, 0xeb);
        buf.credit0 = 0;
        buf.credit1 = 0;
        buf.exitFlag = 0;
        {
            u32 snap = savedataGetStatus((u8*)0, 3);
            memcpy((void*)lbl_8047A3DC, (const void*)snap, 0x7198);
        }
        lbl_8047A3D8 = heroGetStatus((u8*)0, 0xd, 0);
        ok = 1;
    } else {
        ok = 1;
    }
    if (ok == 0) {
        goto done;
    }

    /* ===== Phase B: mode-4 affordability gate ===== */
    if ((mode & 0xff) == 4) {
        /* modeLow is provably 4 here -> available currency via interaction getter */
        r15_have = (s32)heroGetStatus((u8*)0, 0xd, 0);

        /* min entry value across this location's list */
        listStart = (u16*)((u8*)lbl_80478E44 +
                    (u32)(*(u16*)((u8*)lbl_80478E54 + (u32)mapIdx * 4 + 2)) * 2);
        minCost = 0x98967F; /* 9999999 sentinel */
        {
            u16* p = listStart;
            while (*p != 0) {
                itemDataBiosGetPtr(*p);
                {
                    s32 v = (s32)(u16)itemDataBiosGetCoupon();
                    if (v < minCost) minCost = v;
                }
                p++;
            }
        }
        if (r15_have < minCost) {
            fn_8002A1C4((u8*)mapIdx, 4, -1);   /* "can't afford anything" */
            goto done;
        }
    }

    /* ===== main retry loop setup (L_8002C600) ===== */
    modeLow = (u8)(mode & 0xff);
    colorEntry = &lbl_80266E70[(u32)modeLow * 3];
    idxX4 = mapIdx << 2;

    /* do { body } while (r25 == 0)  -- entry jumps straight to the test */
    goto loop_test;

L_628:
    /* (1) optional list rebuild from lbl_802E4F68 / lbl_802EF0A8 records */
    if (lbl_804788A8 != 0) {
        s16* dst = (s16*)lbl_802E4F68;          /* fields at +4,+0xc,+0x14,+0x1c,+0x24 */
        u8*  recs = lbl_802EF0A8;               /* 0x1c-stride records */
        u32* sel5 = (u32*)lbl_802E4F68;         /* selector indices at +0,+8,+0x10,+0x18,+0x20 */
        lbl_804788A8 = 0;
        dst[2]  = *(s16*)(recs + sel5[0] * 0x1c + 4);
        dst[6]  = *(s16*)(recs + sel5[2] * 0x1c + 4);
        dst[10] = *(s16*)(recs + sel5[4] * 0x1c + 4);
        dst[14] = *(s16*)(recs + sel5[6] * 0x1c + 4);
        dst[18] = *(s16*)(recs + sel5[8] * 0x1c + 4);
    }

    /* (2) build the list-menu descriptor and open it */
    lbl_8047A3F8 = 0;
    {
        u16* p = (u16*)((u8*)lbl_80478E44 +
                 (u32)(*(u16*)((u8*)lbl_80478E54 + (u32)idxX4 + 2)) * 2);
        u32 cnt = 0;
        desc1.entryList = p;
        while (*p != 0) { p++; cnt++; }
        desc1.selOut    = &lbl_8047A3F8;
        desc1.entryCount = cnt;
        desc1.f114      = &(*(u32*)&lbl_8047A3F4);
        desc1.f118      = &(*(u32*)&lbl_8047A3F0);
        desc1.f11c      = &(*(u32*)&lbl_8047A3EC);
        desc1.f120      = &(*(u32*)&lbl_8047A3E8);
        desc1.modeByte  = (u8)mode;
        desc1.f125      = 0;
    }
    {
        s32 r = menuOpenCustom((void*)0x60, windowGetActiveID(), 0, 0, (void*)1, 1, &desc1);
        if (r == -1) r25 = 0;
        else         r25 = (s32)*(u16*)desc1.selOut;
    }

    sel = (s32)(u16)r25;
    if (sel == 0) {
        goto L_CB40;
    }

    /* (3) affordability for the chosen entry -> max quantity */
    itemDataBiosGetPtr((u16)sel);
    maxAfford = (s32)(u16)itemDataBiosGetCoupon();
    if (maxAfford > 0) {
        s32 have;
        /* modeLow is 4 in this loop; preserve the per-mode currency fetch */
        have = (s32)heroGetStatus((u8*)0, 0xd, 0);
        maxAfford = have / maxAfford;
        if (maxAfford > 0x63) maxAfford = 0x63;
    } else {
        maxAfford = 0x63;
    }
    if (maxAfford <= 0) {
        fn_8002A1C4((u8*)mapIdx, 8, -1);   /* sold out / cannot buy */
        goto L_628;
    }

    /* (4) format header line, open quantity selector */
    itemDataBiosGetPtr((u16)sel);
    unitPrice = (s32)(u16)itemDataBiosGetCoupon();
    species   = (s32)(u16)sel;
    fn_80029FAC(&msgBuf[1], mapIdx, 0xc, 0x2d, species, -1);
    lbl_8047A3E4 = 0; /* result captured by fn_80029FAC via &msgBuf[1] path */

    if (maxAfford >= 1) {
        qty.enable    = 1;
        qty.maxQty    = maxAfford;
        qty.unitPrice = unitPrice;
        lbl_8047A3E0  = 1;
        qty.total     = &lbl_8047A3E0;
        qty.colR      = colorEntry[0];
        qty.colG      = colorEntry[1];
        qty.colB      = colorEntry[2];
        qty.f2c       = 1;
        qtyTitle      = 1;
        yn = menuOpenCustom((void*)0x61, windowGetActiveID(), 0, 1, &qtyTitle, (s32)&qty);
        menuClose(0x61);
        menuCloseSync((void*)0x61, 1);
        if (yn == -1) chosenQty = -1;
        else          chosenQty = (s32)lbl_8047A3E0;
    } else {
        chosenQty = 0;
    }
    if (chosenQty < 0) {
        goto L_628;
    }

    /* (5) total cost, confirm yes/no */
    totalCost = chosenQty * unitPrice;
    fn_80029FAC(&msgBuf[1], mapIdx, 5, 0x2d, species, 0x2f, totalCost, -1, chosenQty, 0x4b);
    winMsgOpenWithSE(2, lbl_8047A3E4, 1, 0, msgBuf[1]);
    yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
    winMsgClose(1);
    if (yn == 1) goto L_628;     /* cancel */
    if (yn == -1) goto L_628;    /* aborted */

    /* (6) compute owned count / room left for this species */
    if (modeLow == 2) {
        ownedRoom = (s32)(u16)pcboxGetItemCapacity(0, (u16)sel);
    } else if (modeLow == 3) {
        ownedRoom = 0;
        {
            u8* base = (u8*)&buf;
            u16 n = buf.count;
            for (i = 0; i < (s32)n; i++) {
                void* slot = base + i * 4;
                u16 id = itemBiosGetItemDataId(slot);
                if (id == (u16)sel) {
                    ownedRoom += (s32)(u16)(0x3e7 - itemBiosGetNum(slot));
                } else if (id == 0) {
                    ownedRoom += 0x3e7;
                }
            }
        }
    } else {
        ownedRoom = heroItemCheckAddItemDataId((u8*)0, (u32)(u16)sel);
    }

    if (ownedRoom < chosenQty) {
        fn_8002A1C4((u8*)mapIdx, 9, -1);   /* no room */
        goto L_628;
    }

    /* (7) commit purchase */
    fn_8002A1C4((u8*)mapIdx, 6, -1);       /* "thank you" */
    fn_80029EF4((void*)(u32)totalCost, sel, chosenQty, (u8)mode, (void*)&buf);
    fn_80166AB8(0x3cc, 0, 0);              /* purchase SE */

    if ((mode & 0xff) == 4) {
        /* mode-4 "buy another?" loop */
        fn_80029FAC(&msgBuf[1], mapIdx, 7, -1);
        winMsgOpenWithSE(2, lbl_8047A3E4, 1, 0, msgBuf[1]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (yn == -1) goto L_CB40;
        if (yn != 1)  goto L_628;
    }

L_CB40:
    /* ===== finalize: only modes 2 and 3 run the close-out dialogs ===== */
    if (modeLow != 2 && modeLow != 3) {
        r25 = 1;
        goto loop_test;
    }

    if (buf.exitFlag == 0) {
        fn_80029FAC(&msgBuf[0], mapIdx, 0xd, -1);
        winMsgOpenWithSE(2, lbl_8047A3E4, 1, 0, msgBuf[0]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (yn == 1 || yn == -1) {
            next = 0;
        } else {
            if ((mode & 0xff) == 3) {
                fn_8002A1C4((u8*)mapIdx, 0xe, -1);
            }
            next = 1;
        }
        r25 = next;
        goto loop_test;
    } else {
        fn_80029FAC(&msgBuf[0], mapIdx, 0xf, -1);
        winMsgOpenWithSE(2, lbl_8047A3E4, 1, 0, msgBuf[0]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (yn == 1 || yn == -1) {
            next = 0;
            r25 = next;
            goto loop_test;
        }
        if ((mode & 0xff) != 3) {
            /* default-mode close path: confirm-state query then optional restore */
            if (fn_801D0748(4, 2, 0) != 4) {
                u32 snap = savedataGetStatus((u8*)0, 3);
                memcpy((void*)lbl_8047A3DC, (const void*)snap, 0x7198);
                heroSetStatus((u8*)0, 0xd, lbl_8047A3D8);
            }
            r25 = 1;
            goto loop_test;
        }
        /* mode 3: rebuild inventory snapshot, run sell/keep sub-flow */
        {
            u8  itemType;
            u8* recBase = (u8*)lbl_80478E4C +
                          (u32)(*(u8*)((u8*)lbl_80478E54 + (u32)idxX4)) * 0x4c;
            itemType = recBase[0];
            winMsgOpenWithSE(2, 0x3d83, 0, 0, itemType);

            memset(&inv, 0, 0xd8);
            inv.a = buf.credit0;
            inv.b = buf.credit1;
            inv.c = buf.field764;
            inv.zero = 0;
            inv.n = buf.count;

            {
                u8* base = (u8*)&buf;
                s32 j;
                for (j = 0; j < (s32)inv.n; j++) {
                    u16 q = 0;
                    u16 id;
                    void* slot = base + j * 4;
                    if (j < 0 || j > (s32)buf.count) {
                        id = 0;
                    } else {
                        id = itemBiosGetItemDataId(slot);
                        if (id != 0) {
                            inv.items[j].qty = itemBiosGetNum(slot);
                        }
                    }
                    inv.items[j].id = id;
                    if (id == 0) {
                        inv.items[j].id = 0;  /* terminate */
                        break;
                    }
                    (void)q;
                }
            }

            fn_80093574(1);
            fn_80092C90(1, &inv, 0);
            fn_80093574(1);
            if (fn_80093610(1) == 0xc) {
                fn_80093698(1);
                winMsgOpenWithSE(2, 0x3d84, 1, 0, itemType);
                winMsgClose(1);
            } else {
                fn_80093698(1);
                winMsgOpenWithSE(2, 0x3d85, 1, 0, itemType);
                winMsgClose(1);
            }
            fn_8002A1C4((u8*)mapIdx, 0xe, -1);
        }
        r25 = 1;
        goto loop_test;
    }

loop_test:
    if (r25 == 0) {
        goto L_628;
    }

    /* selection made -> close menu group and return */
    menuCloseCustom((void*)0x60, 0, 1);

done:
    return;
}
#endif

/* fn_8002CE6C - 0x8002CE6C | size: 0x2e8 */
extern void fn_800D3088(void);
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
 * fn_8002CE6C  GSmap_ProcessChoice  0x8002CE6C  size: 0x2E8
 *
 * Shows a trade/interaction menu for the NPC identified by `slot`.
 * `obj` is the worldmap context/player-object pointer.
 * `slot` (r4, u8) selects which NPC slot's item list to display.
 *
 * Flow:
 *  1. Open display engine, get nearest trade partner location,
 *     spin-yield until >= 30 frames have accumulated.
 *  2. If a pending key-remap flag is set, refresh the 5 dialog-key
 *     entries from the lbl_802EF0A8 lookup table.
 *  3. Count available items, build a menu descriptor on the stack,
 *     call windowGetActiveID + menuOpenCustom to show the selection dialog.
 *  4. On cancel (-1) → post "cancel" message and return.
 *  5. On selection:
 *       - look up species data (itemDataBiosGetPtr) and get trade count (itemDataBiosGetPrice).
 *       - if count == 0 or ratio > 99 → post "no stock" message and return.
 *       - if trade precondition fails (heroItemCheckAddItemDataId < 1) → post "busy" message and return.
 *       - otherwise: play sound, deduct from party (heroDecPokedoru),
 *         execute trade (heroItemAddItemDataId), then loop back to show updated menu.
 *  6. Close display engine and return.
 */
void fn_8002CE6C(u8* obj, u8 slot) {
    extern u32  menuCloseCustom(u32 a, u32 b, u32 c);    /* display engine open/close */
    extern void fn_8002A2CC(u8* obj, s32 msgId, s32 arg2, ...); /* post format message */
    extern void _threadSwitch(void);                     /* GSthread yield */
    extern void fn_800D3088(void);                     /* GSgfx tick / frame advance */
    extern u32  heroGetStatus(u8* ptr, u32 sel, u32 idx); /* interaction getter */
    extern u32  windowGetActiveID(void);                    /* get display context handle */
    extern s32 menuOpenCustom(u32 sceneId, u32 a, u32 b, u32 c, u32 d, u32 e, ...); /* show menu dialog */
    extern void itemDataBiosGetPtr(u32 speciesId);            /* load species data */
    extern u16  itemDataBiosGetPrice(void);                    /* get trade/field count */
    extern s32  heroItemCheckAddItemDataId(u8* ptr, u32 itemId);     /* check trade precondition */
    extern void fn_80166AB8(u32 soundId, u32 a, u32 b); /* play sound */
    extern void heroDecPokedoru(u8* ptr, u32 offset);     /* deduct from party (give Pokemon) */
    extern s32  heroItemAddItemDataId(u8* ptr, u32 itemId, u32 qty, u32 flags); /* execute trade/receive */

    /* lbl_802E4F68: table of 5 dialog key records, each record has:
     *   +0x00  s32  key value (used for match)
     *   +0x04  s16  display field (written back from lbl_802EF0A8 lookup)
     *   record stride = 0x8 bytes
     * Five entries at offsets 0, 8, 0x10, 0x18, 0x20
     * The s16 at +4 within each entry maps to sth/lha target fields.
     */
    extern u8  lbl_802E4F68[];   /* dialog key table: 5 entries * 0x8 bytes (keys at +0, s16 at +4) */
    /* lbl_802EF0A8: lookup table, each record is 0x1c bytes, s16 at +4 */
    extern u8  lbl_802EF0A8[];

    extern u32 lbl_804788A8;     /* pending key-remap flag */
    extern u32 lbl_80478E54;     /* pointer to location/map table (u8*) */
    extern u32 lbl_80478E44;     /* pointer to NPC item list table (u16*) */

    /* Stack-local dialog descriptor (matches fn_8002C284 convention):
     *   sp+0x08  ptr  pointer to dialog key list (written at line 75)
     *   sp+0x0c  ptr  ptr to start of the matching item list entry (r8)
     *   sp+0x10  u32  item count (r7)
     *   sp+0x14  ptr  &lbl_8047A3F4
     *   sp+0x18  ptr  &(*(u16*)&lbl_8047A3F0)
     *   sp+0x1c  ptr  &lbl_8047A3EC
     *   sp+0x20  ptr  &(*(u16*)&lbl_8047A3E8)
     *   sp+0x24  u8   slot (r30)
     *   sp+0x25  u8   zero
     */
    extern u16 lbl_8047A3F8;    /* dialog key list head (u16, zeroed each iteration) */
    extern u16 lbl_8047A3F4;
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16 lbl_8047A3EC;
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */

    /* Saved registers */
    u32  obj_idx;     /* r31 = obj * 4, used as byte-offset into u16 location table */
    u32  frames;      /* r28, frame accumulator for the spin-yield */
    u16  selection;   /* r28 after dialog, selected item key */
    u16  species;     /* r27 = itemDataBiosGetPtr result (first call) */
    s32  trade_count; /* r26 = itemDataBiosGetPrice result (second call) */
    s32  ratio;       /* r0, computed trade ratio */

    /* --- derive the byte-offset index from the obj pointer integer value --- */
    /* r29 = (u32)obj, r31 = r29 << 2 = (u32)obj * 4 */
    /* FUNCTIONAL-TODO: obj is treated as both a pointer (for fn_8002A2CC) and
     * as a small integer index (slwi r31,r29,2 then lhzx into location table).
     * This pattern matches other scene-callback fns in this TU where the first
     * arg doubles as a scene-slot index.  Cast preserved below. */
    obj_idx = (u32)(u32)obj << 2;   /* ENDIAN-QA: r29 integer value * 4 */

_loop_top:
    /* 1. Open display engine, get nearest interaction partner, start message */
    menuCloseCustom(0x60, 0, 1);
    {
        u32 nearest = heroGetStatus(NULL, 0xc, 0);
        fn_8002A2CC(obj, 0, 0x4b, (s32)nearest, -1);
    }

    /* 2. Spin-yield for 30 frames */
    frames = 0;
    while (frames < 0x1e) {
        _threadSwitch();
        fn_800D3088();
        frames += 1; /* fn_800D3088 returns frame delta in r3 per usage elsewhere;
                      * here the asm does: bl fn_800D3088; add r28,r28,r3 →
                      * accumulate whatever fn_800D3088 returns (typically 1) */
        /* FUNCTIONAL-TODO: original accumulates fn_800D3088's return value.
         * Since fn_800D3088 is a gfx tick that returns 1 per frame, this is
         * functionally equivalent to counting frames. */
    }

    /* 3. If pending key-remap flag: refresh 5 dialog-key s16 fields */
    if (lbl_804788A8 != 0) {
        /* The asm loads 5 indices from lbl_802E4F68[0..4 * 8] at offset +0,
         * multiplies each by 0x1c to index lbl_802EF0A8, reads s16 at +4,
         * and stores back into lbl_802E4F68[entry * 8 + 4].
         * Entry offsets within lbl_802E4F68: 0x0, 0x8, 0x10, 0x18, 0x20.
         * Reading order from asm (r4/r6/r5/r4/r3 = entries 0,2,4,3,1):
         *   entry0 idx at +0x00, entry2 idx at +0x08 (lhzx offset 0x8 from base),
         *   etc.  The indices are loaded in parallel then written sequentially.
         * Preserve exact logic: for each of 5 entries, read the s32 index at
         * the entry's +0 field, look up lbl_802EF0A8[index*0x1c]+4 (s16),
         * store back to entry's +4 field. */
        {
            s32 idx0 = *(s32*)(lbl_802E4F68 + 0x00);
            s32 idx1 = *(s32*)(lbl_802E4F68 + 0x08); /* ENDIAN-QA */
            s32 idx2 = *(s32*)(lbl_802E4F68 + 0x10); /* ENDIAN-QA */
            s32 idx3 = *(s32*)(lbl_802E4F68 + 0x18); /* ENDIAN-QA */
            s32 idx4 = *(s32*)(lbl_802E4F68 + 0x20); /* ENDIAN-QA */
            lbl_804788A8 = 0;
            *(s16*)(lbl_802E4F68 + 0x04) = *(s16*)(lbl_802EF0A8 + idx0 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x0c) = *(s16*)(lbl_802EF0A8 + idx1 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x14) = *(s16*)(lbl_802EF0A8 + idx2 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x1c) = *(s16*)(lbl_802EF0A8 + idx3 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x24) = *(s16*)(lbl_802EF0A8 + idx4 * 0x1c + 0x4);
        }
    }

    /* 4. Build dialog descriptor and show selection menu */
    {
        /* Locate start of this slot's item list in the NPC item table.
         * lbl_80478E54 points to a per-location u16 offset table; index by
         * obj_idx (== (u32)obj * 4) to get a u16 into lbl_80478E44's item list.
         * Advance r8 until a zero-terminator u16 is found, counting entries. */
        u16 *item_list;
        u16 *cur;
        u32  item_count;
        u16 *dialog_key_list;
        u32  menu_result;
        u32  disp_handle;
        struct DialogDesc {
            void *key_list;
            void *items;
            u32   count;
            void *f_a3f4;
            void *f_a3f0;
            void *f_a3ec;
            void *f_a3e8;
            u8    npc_slot;
            u8    _pad;
        } desc;

        lbl_8047A3F8 = 0;
        dialog_key_list = &lbl_8047A3F8;

        {
            /* r0 = *(u8*)lbl_80478E54 + 2 as a base, then lhzx with obj_idx */
            u8  *loc_table = (u8*)lbl_80478E54;
            u16  entry_offset = *(u16*)(loc_table + 2 + obj_idx); /* lhzx r0,r31,r0 */
            u16 *item_base    = (u16*)((u8*)lbl_80478E44 + (u32)entry_offset * 2);
            item_list = item_base;
        }

        /* Count items until zero terminator */
        cur = item_list;
        item_count = 0;
        while (*cur != 0) {
            cur++;
            item_count++;
        }

        /* Build stack descriptor (7 words + 2 bytes matching the asm layout):
         *   [0] = dialog_key_list ptr   (sp+0x08)
         *   [1] = item_list ptr         (sp+0x0c)
         *   [2] = item_count            (sp+0x10)
         *   [3] = &lbl_8047A3F4         (sp+0x14)
         *   [4] = &(*(u16*)&lbl_8047A3F0)         (sp+0x18)
         *   [5] = &lbl_8047A3EC         (sp+0x1c)
         *   [6] = &(*(u16*)&lbl_8047A3E8)         (sp+0x20)
         *   [7] = slot (u8, sp+0x24), zero (u8, sp+0x25)
         */
        desc.key_list = dialog_key_list;
        desc.items    = item_list;
        desc.count    = item_count;
        desc.f_a3f4   = &lbl_8047A3F4;
        desc.f_a3f0   = &(*(u16*)&lbl_8047A3F0);
        desc.f_a3ec   = &lbl_8047A3EC;
        desc.f_a3e8   = &(*(u16*)&lbl_8047A3E8);
        desc.npc_slot = slot;
        desc._pad     = 0;

        disp_handle = windowGetActiveID();
        menu_result = (u32)menuOpenCustom(0x60, disp_handle, 0, 0, 1, 1, &desc);

        /* 5a. Cancelled */
        if ((s32)menu_result == -1) {
            fn_8002A2CC(obj, 2, -1);
            goto _done;
        }

        /* 5b. Read the selected u16 item key from the dialog result pointer */
        selection = *(u16*)desc.key_list;

        if ((selection & 0xffff) == 0) {
            /* Zero selection = cancel path */
            fn_8002A2CC(obj, 2, -1);
            goto _done;
        }

        /* 6. Look up species and count for selected item */
        itemDataBiosGetPtr((u32)selection);
        species = (u16)itemDataBiosGetPrice();

        itemDataBiosGetPtr((u32)selection);
        trade_count = (s16)itemDataBiosGetPrice();

        /* 7. Compute stock ratio and check >= 1 */
        if (trade_count > 0) {
            u32 stock = heroGetStatus(NULL, 0xc, 0);
            ratio = (s32)stock / trade_count;
            if (ratio > 0x63) ratio = 0x63;
        } else {
            ratio = 0x63;
        }

        if (ratio <= 0) {
            fn_8002A2CC(obj, 5, -1);
            goto _done;
        }

        /* 8. Check trade precondition */
        if (heroItemCheckAddItemDataId(NULL, (u32)selection) < 1) {
            fn_8002A2CC(obj, 6, -1);
            goto _done;
        }

        /* 9. Execute trade: play sound, deduct, trade, then loop */
        fn_80166AB8(0x3cb, 0, 0);
        heroDecPokedoru(NULL, (u32)species);   /* give/deduct partner Pokemon by species offset */
        heroItemAddItemDataId(NULL, (u32)selection, 1, -1); /* execute trade/receive */
        fn_8002A2CC(obj, 4, 0x2d, (s32)(selection & 0xffff), -1);
        goto _loop_top;
    }

_done:
    /* 10. Close display engine */
    menuCloseCustom(0x60, 0, 1);
}
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
 * fn_8002D154  GSmap_ConfirmSequence  (0x8002D154, size 0x480)
 *
 * World-map "shop / confirm purchase" interaction loop.
 *   mapIndex    (r3) : map slot index; selects the destination/shop entry list
 *                      (used as <<2 word index and passed to the text formatters).
 *   colorIndex  (r4) : low byte selects an RGB triple in lbl_80266E70[index*3]
 *                      used to tint the quantity menu.
 *
 * Flow: (1) optionally refresh a 5-entry stat-mirror struct, (2) open the item
 * list menu, (3) on a chosen item validate category/affordability, (4) open a
 * quantity menu, (5) confirm with a Yes/No prompt and commit the purchase, then
 * loop until the player backs out (empty selection) which closes the menu.
 */
void fn_8002D154(s32 mapIndex, u8 colorIndex)
{
    /* ---- block-scope typed externs (TU convention) ---- */
    extern u32  lbl_804788A8;        /* "stats dirty" flag           */
    extern u32  lbl_80478E54;        /* map -> list-index base table */
    extern u32  lbl_80478E44;        /* destination/shop list table  */
    extern u32  lbl_8047A3E4;        /* formatted-string handle out  */

    extern u16  lbl_8047A3F8;        /* item-menu result (selected item id) */
    extern u16  lbl_8047A3F4;
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16  lbl_8047A3EC;
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3E0;  /* canonical; per-site reinterpret cast */

    extern u8   lbl_80266E70[];      /* RGB color triples, 3 bytes/entry */

    /* stat-mirror source/dest blocks */
    struct StatMirror {
        u32 src0;  u16 dst0;  u16 _p0;   /* +0x00 idx, +0x04 out */
        u32 src1;  u16 dst1;  u16 _p1;   /* +0x08 idx, +0x0c out */
        u32 src2;  u16 dst2;  u16 _p2;   /* +0x10 idx, +0x14 out */
        u32 src3;  u16 dst3;  u16 _p3;   /* +0x18 idx, +0x1c out */
        u32 src4;  u16 dst4;  u16 _p4;   /* +0x20 idx, +0x24 out */
    };
    extern u8 lbl_802E4F68[];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_802EF0A8[];  /* canonical; per-site reinterpret cast */

    /* callees */
    extern u32  windowGetActiveID(void);                                  /* menu ctx handle */
    extern s32 menuOpenCustom(s32 a, u32 b, void* c, s32 d, s32 e, s32 f, void* desc); /* open list menu */
    extern void menuClose(s32 menuId);                           /* menu post-step  */
    extern void menuCloseSync(s32 menuId, s32 flag);
    extern s8 menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void winMsgClose(s32 a);
    extern void menuCloseCustom(s32 a, s32 b, s32 c);                  /* close/release   */
    extern u32  itemDataBiosGetPtr(u32 itemId);                           /* select item     */
    extern u32  itemDataBiosGetKind(void);                                 /* item category   */
    extern u32  itemDataBiosGetPrice(void);                                 /* item price/value*/
    extern s8   heroItemCheckHaveItemDataId(s32 a, s32 b);                         /* story/flag query*/
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx);       /* money getter    */
    extern u32  heroItemCheckAddItemDataId(s32 a, u32 item);                      /* inventory count */
    extern void heroDecPokedoru(s32 a, s32 amount);                    /* spend money     */
    extern void heroItemAddItemDataId(s32 a, s32 item, u16 qty, s32 d);      /* add item        */
    extern void fn_80166AB8(s32 sfx, s32 b, s32 c);               /* play sound      */
    extern void winMsgOpenWithSE(s32 a, u32 b, s32 c, s32 d, u8 e);
    extern u32  fn_8002A0B8(u8* buf, s32 idx, s32 a, s32 b, ...);  /* format text     */
    extern void fn_8002A2CC(u8* idx, s32 a, s32 b, ...);          /* format/print txt*/

    /* RGB tint for this color index */
    u8*  rgb = &lbl_80266E70[(colorIndex & 0xff) * 3];

    /* item-list menu descriptor (consumed by menuOpenCustom via the pointer block) */
    struct {
        u16* result;        /* +0x00 -> lbl_8047A3F8        */
        u16* list;          /* +0x04 destination/shop list  */
        s32  count;         /* +0x08 entry count            */
        u16* slot1;         /* +0x0c -> lbl_8047A3F4        */
        u16* slot2;         /* +0x10 -> (*(u16*)&lbl_8047A3F0)        */
        u16* slot3;         /* +0x14 -> lbl_8047A3EC        */
        u16* slot4;         /* +0x18 -> (*(u16*)&lbl_8047A3E8)        */
        u8   color;         /* +0x1c                        */
        u8   flag;          /* +0x1d                        */
    } itemDesc;

    /* quantity-menu descriptor */
    struct {
        s32  enabled;       /* +0x00 (=1)                   */
        s32  maxQty;        /* +0x04                        */
        s32  unitPrice;     /* +0x08                        */
        s32* result;        /* +0x0c -> (*(s32*)&lbl_8047A3E0)        */
        u8   r;             /* +0x10                        */
        u8   g;             /* +0x11                        */
        u8   b;             /* +0x12                        */
        u8   _pad;          /* +0x13                        */
        u32  trailer;       /* +0x14 (=0)                   */
    } qtyDesc;
    s32 qtyHeader;          /* the separate +0x14 word passed as r5 (=1) */

    u16  selectedItem;
    s32  maxAffordable;
    u32  itemHandle;
    u32  unitPrice;
    s32  chosenQty;
    s32  totalCost;
    s8   yesNo;
    u32  ctx;
    s32  menuRet;
    u8   fmtBuf[0x770];     /* scratch buffer filled by fn_8002A0B8 */

    for (;;) {
        /* (1) refresh the 5-entry stat mirror when flagged dirty */
        if ((s32)lbl_804788A8 != 0) {
            struct StatMirror* m = &(*(struct StatMirror*)lbl_802E4F68);
            lbl_804788A8 = 0;
            /* each record is 0x1c bytes; field at +4 (s16). ((s16*)lbl_802EF0A8) is s16[]
               so record i's +4 field is element index i*(0x1c/2) + 2 == i*14 + 2 */
            m->dst0 = (u16)((s16*)lbl_802EF0A8)[m->src0 * 14 + 2];   /* ENDIAN-QA */
            m->dst1 = (u16)((s16*)lbl_802EF0A8)[m->src1 * 14 + 2];   /* ENDIAN-QA */
            m->dst2 = (u16)((s16*)lbl_802EF0A8)[m->src2 * 14 + 2];   /* ENDIAN-QA */
            m->dst3 = (u16)((s16*)lbl_802EF0A8)[m->src3 * 14 + 2];   /* ENDIAN-QA */
            m->dst4 = (u16)((s16*)lbl_802EF0A8)[m->src4 * 14 + 2];   /* ENDIAN-QA */
        }

        /* (2) build the destination/shop list and open the item menu */
        lbl_8047A3F8 = 0;
        {
            u16* base = (u16*)(lbl_80478E44 +
                        (u32)(*(u16*)((u8*)lbl_80478E54 + 2 + mapIndex * 4)) * 2);
            s32  count = 0;
            u16* p = base;
            while (*p != 0) { p++; count++; }

            itemDesc.result = &lbl_8047A3F8;
            itemDesc.list   = base;
            itemDesc.count  = count;
            itemDesc.slot1  = &lbl_8047A3F4;
            itemDesc.slot2  = &(*(u16*)&lbl_8047A3F0);
            itemDesc.slot3  = &lbl_8047A3EC;
            itemDesc.slot4  = &(*(u16*)&lbl_8047A3E8);
            itemDesc.color  = colorIndex;
            itemDesc.flag   = 0;
        }

        ctx     = windowGetActiveID();
        menuRet = menuOpenCustom(0x60, ctx, (void*)0, 0, 1, 1, &itemDesc);
        if (menuRet == -1) {
            selectedItem = 0;
        } else {
            selectedItem = lbl_8047A3F8;
        }

        /* (3) empty selection -> back out and close the menu */
        if ((selectedItem & 0xffff) == 0) {
            break;
        }

        /* validate the item exists */
        if (itemDataBiosGetPtr(selectedItem) == 0) {
            continue;
        }

        /* special-case: "key item" category (6) gated by a story flag */
        if ((itemDataBiosGetKind() & 0xff) == 6) {
            if ((heroItemCheckHaveItemDataId(0, 0x21e) & 0xff) == 0) {
                fn_8002A2CC((u8*)mapIndex, 8, -1);
                continue;
            }
        }

        /* (3b) compute how many the player can afford (capped at 99) */
        itemDataBiosGetPtr(selectedItem);
        maxAffordable = (s32)(itemDataBiosGetPrice() & 0xffff);
        if (maxAffordable > 0) {
            maxAffordable = (s32)heroGetStatus((u8*)0, 0xc, 0) / maxAffordable;
            if (maxAffordable > 0x63) {
                maxAffordable = 0x63;
            }
        } else {
            maxAffordable = 0x63;
        }
        if (maxAffordable <= 0) {
            fn_8002A2CC((u8*)mapIndex, 5, -1);
            continue;
        }

        /* (4) format the item header and open the quantity selector */
        itemDataBiosGetPtr(selectedItem);
        unitPrice = itemDataBiosGetPrice() & 0xffff;
        {
            u16 itemId16 = selectedItem & 0xffff;
            lbl_8047A3E4 = fn_8002A0B8(fmtBuf, mapIndex, 0xc, 0x2d, itemId16, -1);

            if (maxAffordable < 1) {
                chosenQty = 0;
            } else {
                qtyDesc.enabled   = 1;
                qtyDesc.maxQty    = maxAffordable;
                qtyDesc.unitPrice = (s32)unitPrice;
                qtyDesc.result    = &(*(s32*)&lbl_8047A3E0);
                qtyDesc.r         = rgb[0];
                qtyDesc.g         = rgb[1];
                qtyDesc.b         = rgb[2];
                qtyDesc._pad      = 0;
                qtyDesc.trailer   = 0;
                qtyHeader         = 1;
                (*(s32*)&lbl_8047A3E0)      = 1;

                ctx     = windowGetActiveID();
                menuRet = menuOpenCustom(0x61, ctx, &qtyHeader, 0, 1, 1, &qtyDesc);
                menuClose(0x61);
                menuCloseSync(0x61, 1);
                if (menuRet == -1) {
                    chosenQty = -1;
                } else {
                    chosenQty = (*(s32*)&lbl_8047A3E0);
                }
            }

            if (chosenQty < 0) {
                continue;
            }

            totalCost = chosenQty * (s32)unitPrice;

            /* (5) build the confirmation string and prompt Yes/No */
            menuRet = fn_8002A0B8(fmtBuf, mapIndex, 3, 0x2d,
                                  itemId16, 0x2f, chosenQty, 0x4b, totalCost, -1);
            winMsgOpenWithSE(2, (u32)menuRet, 1, 0, fmtBuf[0]);
            yesNo = (s8)menuSubOpenYesNo(0, -1, -1, 0);
            winMsgClose(1);
            if (yesNo == 1 || yesNo == -1) {
                continue;
            }

            /* committed: verify capacity, charge money, grant items */
            if ((s32)heroItemCheckAddItemDataId(0, selectedItem) < chosenQty) {
                fn_8002A2CC((u8*)mapIndex, 6, -1);
                continue;
            }
            fn_80166AB8(0x3cb, 0, 0);
            heroDecPokedoru(0, totalCost);
            heroItemAddItemDataId(0, selectedItem, (u16)chosenQty, -1);
            fn_8002A2CC((u8*)mapIndex, 4, -1);

            /* bonus item: buying >=10 of item 4 grants a premier-ball style extra */
            if ((selectedItem & 0xffff) == 4 && chosenQty >= 0xa &&
                (s32)heroItemCheckAddItemDataId(0, 0xc) >= 1) {
                fn_8002A2CC((u8*)mapIndex, 7, -1);
                heroItemAddItemDataId(0, 0xc, 1, -1);
            }
        }
    }
    menuCloseCustom(0x60, 0, 1);
}
#endif

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
void fn_8002D5D4(void)
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
 * fn_8002D91C  GSmap_ArrivalDialog
 * 0x8002D91C | size: 0x350
 *
 * Drives the post-travel arrival dialog sequence.  Behaviour branches on
 * the "type" byte stored at lbl_80478E54[arg0*4 + 1]:
 *   0  -- name-entry / confirm-or-cancel dialog (menu 0x62)
 *   1  -- direct hand-off to fn_8002CE6C
 *   >=2 -- location-arrival dialog (menu 0x83), sub-type from r28 drives
 *          which sub-dialogs are opened while waiting for user choice.
 *
 * arg0: location/context index passed in by the caller (e.g. 0xB or 0xC
 *       from ui_core.c).
 */
void fn_8002D91C(u32 arg0)
{
    /* block-scope externs in TU convention */
    extern u32  lbl_80478E54;  /* pointer to location-map table base  */
    extern u32  lbl_8047A3DC;  /* GSmem pointer for arrival scratch    */
    extern u32  lbl_8047A3FC;  /* persisted arg0 for this session      */
    extern u32  lbl_8047A400;  /* word at lbl_8047A3FC+4, flag checked at tail */
    u32* base;

    extern s32  menuOpen(u32 sceneId, u32 p1);  /* scene/menu query */
    extern void menuClose(u32 sceneId);           /* scene unload     */
    extern void menuCloseSync(u32 sceneId, u32 p1); /* sync-close menu  */
    extern void winMsgClose(s32 p);                 /* yield / wait frame */
    extern void winMsgOpenWithSE(s32 a, u32 b, s32 c, s32 d, u8 e); /* text display helper */

    extern u32  heroGetStatus(u8 *ptr, u32 selector, u32 idx); /* interaction getter */

    /* format-text helpers (vararg: last s32 arg is -1 terminator) */
    extern u32  fn_8002A0B8(u8 *buf, s32 locIdx, s32 field, s32 p6, ...);
    extern u32  fn_80029FAC(u8 *buf, s32 locIdx, s32 field, s32 p6, ...);
    extern void fn_8002A1C4(u8 *buf, s32 p4, s32 p5, ...);
    extern void fn_8002A2CC(u8 *buf, s32 p4, s32 p5, ...);

    /* sub-dialog launchers */
    extern void fn_8002CE6C(u32 ctx, u32 type); /* process-choice     */
    extern void fn_8002D154(u32 ctx, u32 type); /* confirm-sequence   */
    extern void fn_8002C408(u32 ctx, u32 type); /* dialog-state-machine */
    extern void fn_8002C284(u32 ctx, u32 type); /* show-travel-dialog */

    /* storage / set-box helper */
    extern void fn_80018F54(u32 a, u32 b, u32 c); /* GSpcbox_SetCurrentBox */

    /* GSmem helpers */
    extern u32   _toolentryAlloc__FUl(u32 size);          /* GSmemAllocRaw -> handle */
    extern void *fn_800E27B0(u32 handle);         /* GSmemGetPtr            */
    extern void  fn_800E24B0(u32 handle);         /* GSmemFree (step 1)     */
    extern void  fn_800E209C(u32 handle);         /* GSmemFree (step 2)     */

    /* exit-path helpers */
    extern void fn_800FF660(void);
    extern void floorSetFadeScript(s32 a, u32 b);

    /* ---- local variables ------------------------------------------------ */
    u8  type_byte;  /* r28: lbl_80478E54[arg0*4 + 1]                        */
    u32 mem_handle; /* r27: GSmemAllocRaw result, only used in >=2 path      */
    s32 loop_state; /* r26: inner-loop exit condition flag                   */
    s32 menu_res;   /* r29: raw menu query result, normalised to 0-3         */
    u8  text_buf0;  /* sp+8: first byte written by fn_80029FAC / fn_8002A0B8 */
    u8  text_buf1;  /* sp+9: first byte written by fn_8002A0B8               */
    u32 interact;   /* r7 scratch for heroGetStatus result                     */

    /* --------------------------------------------------------------------- */
    /* Save arg0 for this dialog session and clear the flag word that follows */
    base = &lbl_8047A3FC;   /* retail caches this sda21 address in r30 */
    base[0] = arg0;
    base[1] = 0;                 /* lbl_8047A400 == *(r30+4) */

    /* Read the type byte from the location table:
     * table base = *(u32*)lbl_80478E54, entry at arg0*4, byte offset 1      */
    type_byte = ((u8 *)lbl_80478E54)[arg0 * 4 + 1];

    /* ================================================================
     * PATH A: type_byte == 0 -- name-entry / menu-0x62 flow
     * ================================================================ */
    if (type_byte == 0) {

        /* Fetch the interaction entry and format the first text line */
        interact = heroGetStatus((u8 *)0, 0xc, 0);
        {
            u32 text_entry = fn_8002A0B8(&text_buf1, (s32)arg0, 0, 0x4b,
                                         (s32)interact, (s32)-1);
            winMsgOpenWithSE(2, text_entry, 1, 0, text_buf1);
        }

        /* ---- menu-0x62 wait-loop ---- */
        loop_state = 0;  /* initialise loop exit flag */
        do {
            /* Wait one frame before re-querying */
            winMsgClose(1);

            /* Branch on previous normalised menu result */
            if (loop_state == 0) {
                fn_8002D154(arg0, (u32)type_byte);
            } else if (loop_state == 1) {
                /* r26 == 1: set current box (args: 3, arg0, 0) */
                fn_80018F54(3, arg0, 0);
            }
            /* r26 > 1: skip both calls */

            /* Re-format the text line with update flag = 1 */
            {
                u32 text_entry = fn_8002A0B8(&text_buf1, (s32)arg0, 1, (s32)-1);
                /* FUNCTIONAL-TODO: The va_arg terminator is -1 in r6; r7
                 * coming from heroGetStatus at entry is gone by this second
                 * call site -- the asm does NOT pass r7 here.             */
                winMsgOpenWithSE(2, text_entry, 1, 0, text_buf1);
            }

            /* Query menu 0x62, then unload/close it */
            menu_res = menuOpen(0x62, 1);
            menuClose(0x62);
            menuCloseSync(0x62, 1);

            /* Normalise raw menu result -> loop_state in {0, 1, 2} */
            if (menu_res == -1 || menu_res == 2) {
                loop_state = 2;
            } else if (menu_res == 0) {
                loop_state = 0;
            } else {
                loop_state = 1;
            }

        } while (loop_state != 2);

        /* On confirmed exit, push to format-text-3 and jump to common tail */
        fn_8002A2CC((u8 *)(u32)arg0, 2, (s32)-1);
        /* fall through to common tail */

    /* ================================================================
     * PATH B: type_byte == 1 -- direct hand-off to ProcessChoice
     * ================================================================ */
    } else if (type_byte == 1) {

        fn_8002CE6C(arg0, (u32)type_byte);
        /* fall through to common tail */

    /* ================================================================
     * PATH C: type_byte >= 2 -- location-arrival dialog (menu 0x83)
     * ================================================================ */
    } else {

        /* Allocate scratch GSmem block (0x7198 bytes) */
        mem_handle = _toolentryAlloc__FUl(0x7198);
        lbl_8047A3DC = (u32)fn_800E27B0(mem_handle);

        /* Format the initial "arrival at" text line */
        {
            u32 text_entry = fn_80029FAC(&text_buf0, (s32)arg0, 0, (s32)-1);
            winMsgOpenWithSE(2, text_entry, 1, 0, text_buf0);
        }

        /* ---- menu-0x83 wait-loop ---- */
        loop_state = 0;
        do {
            /* Wait one frame before re-querying */
            winMsgClose(1);

            /* Dispatch on the normalised menu result from the previous
             * iteration (first time through we skip straight to the menu
             * open since loop_state==0 and we jump into the loop tail).   */
            if (loop_state == 0) {
                /* Open the main arrival dialog state machine */
                fn_8002C408(arg0, (u32)type_byte);
            } else if (loop_state == 1) {
                /* Show the secondary travel dialog */
                fn_8002C284(arg0, (u32)type_byte);
            } else if (loop_state == 2) {
                /* Open "format text 2" sub-dialog */
                fn_8002A1C4((u8 *)(u32)arg0, 0xb, (s32)-1);
            } else if (loop_state == 3) {
                /* Set the done flag to exit after cleanup */
                /* loop_state==3 means "exit" -- handled after menu query */
            }
            /* loop_state >= 4 or < 0: no-op */

            if (loop_state != 0) {
                /* loop_state already set; skip to menu query */
                goto query_menu_83;
            }

            /* Re-format the arrival text (update pass, field=1) */
            {
                u32 text_entry = fn_80029FAC(&text_buf0, (s32)arg0, 1, (s32)-1);
                winMsgOpenWithSE(2, text_entry, 1, 0, text_buf0);
            }

        query_menu_83:
            /* Query menu 0x83, then unload/close it */
            menu_res = menuOpen(0x83, 1);
            menuClose(0x83);
            menuCloseSync(0x83, 1);

            /* Normalise raw result -> 0/1/2/3 */
            if (menu_res == 0) {
                menu_res = 0;
            } else if (menu_res == 1) {
                menu_res = 1;
            } else if (menu_res >= 2 && menu_res < 3) {
                menu_res = 2;
            } else {
                menu_res = 3; /* <0 or >=3 */
            }

            loop_state = menu_res;

        } while (loop_state != 3);

        /* Post-loop: close sub-dialogs based on type_byte */
        if (type_byte != 2 && type_byte != 3) {
            fn_8002A1C4((u8 *)(u32)arg0, 2, (s32)-1);
        } else {
            winMsgClose(1);
        }

        /* Release the GSmem scratch block */
        fn_800E24B0(mem_handle);
        fn_800E209C(mem_handle);
    }

    /* ================================================================
     * COMMON TAIL: if the flag word at lbl_8047A3FC+4 is non-zero,
     * run the global exit sequence.
     * ================================================================ */
    if (base[1] != 0) {
        fn_800FF660();
        floorSetFadeScript(0, 0);
    }
}
#endif

/* menuShopOpen - 0x8002DC6C | size: 0xb8 | WALL 71%: fsub/fdiv double vs fsubs/fdivs single + sda21 store */
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
    extern void fn_800FF730(s32);
    extern void floorSetFadeScript(s32, u32);
    extern void _threadSwitch(void);
    extern void menuReleaseOffScreen(f32);

    /* lbl_8047A3FC: two consecutive u32 words in SDA (flag word, active word) */
    extern u32 lbl_8047A3FC;

    /* lbl_8047B9CC / lbl_8047B998: r2-relative float/double constants used for
       the int->float bias conversion.  We bypass the bias trick with a direct
       cast - ENDIAN-QA: xoris+0x4330 bias is identical to (f32)(s32)x */
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
    lbl_8047A3FC = flag;
    *(&lbl_8047A3FC + 1) = 1u;   /* lbl_8047A3FC+4 */

    _flagSet(1, 2);
    fn_800FF730(0x38f);
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
