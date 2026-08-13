/**
 * @file gs_pcbox_range_8001E3E0.c
 * @brief Unidentified scene state machine (low-confidence split segment).
 *
 * Address range: 0x8001E3E0 - 0x80020328 (10 functions)
 *
 * Split from game/gs_pcbox.c. This segment has zero name anchors; it is
 * structurally similar to XD's game/menuPcBoxPokemon.cpp scene state
 * machine (CMenuPokemonLeave::init/loop/main) but this cannot be verified
 * via anchor monotonicity, only via internal self-consistency (single
 * shared .data symbol, disjoint call signature from its neighbors).
 * Kept under an address-based filename per low-confidence handling rules.
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations (shared)
 * ========================================================================= */

/* Pokemon data */
extern void  heroItemGetItemKindToItemAryPtr(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void  heroHizukiItemGetItemAryPtr(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);
extern u16   itemBiosGetNum(void* fieldData);
extern u16   itemDataBiosGetPtr(u16 speciesId);
extern u16   itemDataBiosGetPrice(void);

/* Text formatting */
extern void  fn_8002A0B8(void* outBuf, void* fmt, s32 p3, s32 p4,
                          u16 p5, s32 p6, ...);
extern s32   heroGetStatus(void* partyData, s32 slot, s32 p3);

/* Dialog/rendering */
extern void  winMsgOpenWithSE(s32 p1, void* text, s32 p3, s32 p4, u8 p5);
extern void  winMsgClose(s32 slot);
extern void  winSeqSetMenu(void* ctx, s32 state);
extern void  menuDataBiosGetXY(s16 npcId, u16* outX, u16* outY);
extern void  menuDataBiosSetXY(s16 x, s16 y, s16 z);
extern void* menuDataBiosGetPtr(void* data);

/* 0x8001E3E0 | 0xD4 */
#if 0
asm void fn_8001E3E0(void) {
#include "src/game/gs_pcbox_fn_8001E3E0.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E3E0(void* a, u32* b) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void menuSetPosition();
    extern void windowGetValue();
    extern void windowCheckCursor();
    extern u8* windowSearchID();
    extern void menuClose();
    void* r4_tmp;
    u8* r3;
    s32 r31;
    void* a_save;
    r31 = 0;
    r4_tmp = windowGetActiveID();
    a_save = a;
    menuOpenCustom(0x2, r4_tmp, 0, 0, 0, 0x3, a_save, 0x1, 0);
    menuSetPosition(0x2, 0x32, 0x3c);
    windowCheckCursor(0x2, 0x1);
    windowGetValue(0x2);
    r3 = windowSearchID(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        menuClose(0x2);
    }
    return r31;
}
#pragma peephole reset
#endif

/* 0x8001E4B4 | 0xD8 */
extern u8 lbl_80314E08[];
#if 0
asm void fn_8001E4B4(void) {
#include "src/game/gs_pcbox_fn_8001E4B4.inc"
}
#else
#pragma optimization_level 4
void fn_8001E4B4(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f) {
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D61E4(s32, s32);
    extern void fn_800D6728();
    extern void fn_800D67BC(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(u8*);
    extern void fn_800D888C(s32);
    extern void fn_800D88DC(s32);
    fn_800D88DC(0x1);
    fn_800D888C(0x6);
    fn_800D6A00(0x3);
    fn_800D7820(lbl_80314E08);
    fn_800D67BC(0x3);
    fn_800D61E4((s16)a, (s16)b);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)c, (s16)d);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)e, (s16)f);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D6728();
}
#endif

/* 0x8001E644 | 0x454 */
extern void fn_800D5BA0(void);
extern void fn_800D5648(void);
extern void menuSpriteBiosGetPtr(void);
extern void windowDrawSprite(void);
extern void menuItemBiosGetPtr(void);
extern void windowDrawSprite2(void);
extern u8 lbl_80266C20[];
extern f64 lbl_8047B7D8;
extern f32 lbl_8047B7E4;
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001E644(void) {
#include "src/game/gs_pcbox_fn_8001E644.inc"
}
#else
typedef struct GsPcboxSpriteBios {
    u8 pad_00[0x0C];
    s16 width;
    s16 height;
} GsPcboxSpriteBios;

typedef struct GsPcboxItemBios {
    u8 pad_00[0x02];
    s16 x;
    s16 y;
} GsPcboxItemBios;

void fn_8001E644(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4) {
    extern u8 lbl_80266C20[];
    extern u8 lbl_80314E08[];
    extern f64 lbl_8047B7D8;
    extern f32 lbl_8047B7E0;
    extern f32 lbl_8047B7E4;
    extern void* menuSpriteBiosGetPtr(s32);
    extern void* menuItemBiosGetPtr(s16);
    extern void fn_800D5648(f32);
    extern void fn_800D5BA0(u32, u32);
    extern void fn_800D61E4(s16, s16);
    extern void fn_800D6728(void);
    extern void fn_800D67BC(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(void*);
    extern void fn_800D888C(s32);
    extern void fn_800D88DC(s32);
    extern void windowDrawSprite(s32, s32, void*, u16, u32);
    extern void windowDrawSprite2(s32, s32, s16, s16, s32, s32, s32, s32);
    u32 colors[3];
    f32 alphaScale;
    GsPcboxSpriteBios* sprite;
    GsPcboxSpriteBios* stretchSprite;
    GsPcboxSpriteBios* edgeSprite;
    GsPcboxItemBios* item0;
    GsPcboxItemBios* item1;
    s16 left;
    s16 top;
    s16 right;
    s16 bottom;
    s16 innerLeft;
    s16 innerTop;
    s16 innerRight;
    s16 outerHeight;
    s16 span;
    s16 spriteWidth;
    s16 spriteHeight;
    s32 y;

    colors[0] = ((u32*)lbl_80266C20)[0];
    colors[1] = ((u32*)lbl_80266C20)[1];
    colors[2] = ((u32*)lbl_80266C20)[2];

    alphaScale = (f32)arg4 / lbl_8047B7E4;
    ((u8*)colors)[3] = (u8)((f32)((u8*)colors)[3] * alphaScale);
    ((u8*)colors)[7] = (u8)((f32)((u8*)colors)[7] * alphaScale);
    ((u8*)colors)[11] = (u8)((f32)((u8*)colors)[11] * alphaScale);

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D6A00(6);
    fn_800D7820(lbl_80314E08);

    left = (s16)(arg0 - 0xA);
    top = (s16)(arg1 - 0xA);
    right = left + (s16)(arg2 + 0x14);
    outerHeight = (s16)(arg3 + 0x14);
    bottom = top + outerHeight;

    fn_800D67BC(4);
    fn_800D61E4(left, top);
    fn_800D5BA0(0, colors[0]);
    fn_800D61E4(right, top);
    fn_800D5BA0(0, colors[0]);
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, colors[1]);
    fn_800D61E4(left, bottom);
    fn_800D5BA0(0, colors[1]);
    fn_800D6728();

    fn_800D6A00(1);
    fn_800D7820(lbl_80314E08);
    fn_800D5648(lbl_8047B7E0);
    for (y = top; y < outerHeight + top; y += 4) {
        fn_800D67BC(2);
        fn_800D61E4(left, (s16)y);
        fn_800D5BA0(0, colors[2]);
        fn_800D61E4(right, (s16)y);
        fn_800D5BA0(0, colors[2]);
        fn_800D6728();
    }

    sprite = menuSpriteBiosGetPtr(0xBB);
    spriteWidth = (sprite->width + ((u32)sprite->width >> 31)) >> 1;
    spriteHeight = sprite->height + ((u32)sprite->height >> 31);
    left = (s16)(arg0 - spriteWidth);
    innerLeft = (s16)(arg0 + spriteWidth);
    right = (s16)(arg0 + arg2 - spriteWidth);
    top = (s16)(arg1 - (spriteHeight >> 1) - 0xA);
    innerTop = (s16)(arg1 + spriteHeight);

    sprite = menuSpriteBiosGetPtr(0xB8);
    bottom = (s16)(arg1 + arg3 - ((sprite->height + ((u32)sprite->height >> 31)) >> 1) + 0xA);

    windowDrawSprite(left, top, 0, 0xBB, 0);
    windowDrawSprite(right, top, 0, 0xBB, 1);
    windowDrawSprite(left, bottom, 0, 0xB8, 0);
    windowDrawSprite(right, bottom, 0, 0xB8, 1);

    stretchSprite = menuSpriteBiosGetPtr(0xBA);
    item0 = menuItemBiosGetPtr(0x84);
    item1 = menuItemBiosGetPtr(0x87);
    span = bottom - innerTop;
    windowDrawSprite2(
        left + (s16)(item1->x - item0->x), innerTop, stretchSprite->width,
        span, -1, 0, 0xBA, 0);

    item0 = menuItemBiosGetPtr(0x85);
    item1 = menuItemBiosGetPtr(0x86);
    windowDrawSprite2(
        right + (s16)(item1->x - item0->x), innerTop, stretchSprite->width,
        span, -1, 0, 0xBA, 0);

    edgeSprite = menuSpriteBiosGetPtr(0xB7);
    item0 = menuItemBiosGetPtr(0x84);
    item1 = menuItemBiosGetPtr(0x8B);
    span = right - innerLeft;
    windowDrawSprite2(
        innerLeft, top + (s16)(item1->y - item0->y), span, edgeSprite->height,
        -1, 0, 0xB7, 0);

    item0 = menuItemBiosGetPtr(0x88);
    item1 = menuItemBiosGetPtr(0x8A);
    windowDrawSprite2(
        innerLeft, bottom + (s16)(item1->y - item0->y), span, edgeSprite->height,
        -1, 0, 0xB7, 0);
}
#endif

/* 0x8001EA98 | 0x170 */
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001EA98(void) {
#include "src/game/gs_pcbox_fn_8001EA98.inc"
}
#else
void fn_8001EA98(void) {
    extern u8 lbl_80314E08[];
    extern f32 lbl_8047B7E0;
    extern void fn_800D5648();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r28 = r3;
    r29 = r4;
    r31 = r5;
    r30 = r6;
    r3 = 0x1;
    fn_800D88DC();
    r3 = 0x6;
    fn_800D888C();
    r3 = 0x7;
    fn_800D6A00();
    r3 = (u32)lbl_80314E08;
    r3 = (u32)lbl_80314E08;
    fn_800D7820();
    r3 = 0x2;
    fn_800D67BC();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = 0xc0;
    fn_800D5BA0();
    r31 = r31 + 0xa;
    r30 = r30 + 0xa;
    r31 = r28 + r31;
    r30 = r29 + r30;
    r3 = (s16)r31;
    r4 = (s16)r30;
    fn_800D61E4();
    r3 = 0x0;
    r4 = 0xc0;
    fn_800D5BA0();
    fn_800D6728();
    f1 = lbl_8047B7E0;
    fn_800D5648();
    r3 = 0x2;
    fn_800D6A00();
    r3 = 0x5;
    fn_800D67BC();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r31;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r31;
    r4 = (s16)r30;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r4 = (s16)r30;
    r3 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    r3 = (s16)r3;
    r4 = (s16)tmp;
    fn_800D61E4();
    r3 = 0x0;
    r4 = -0x1;
    fn_800D5BA0();
    fn_800D6728();
    return;
}
#endif

/* 0x8001EC08 | 0x370 */
extern void fn_800CE148(void);
extern void fn_800CDBE0(void);
extern void _savefpr_26(void);
extern void _restfpr_26(void);
extern u8 lbl_803A1D60[];
extern f64 lbl_8047B7F0;
extern f64 lbl_8047B7D0;
extern f64 lbl_8047B7F8;
extern f64 lbl_8047B7E8;
extern f64 lbl_8047B800;
#if 0
asm void fn_8001EC08(void) {
#include "src/game/gs_pcbox_fn_8001EC08.inc"
}
#else
typedef struct PCBoxVertex {
    s16 x;
    s16 y;
    u32 color;
} PCBoxVertex;

void fn_8001EC08(s32 x, s32 y, s32 width, s32 height, u8 color, u8 dimColor) {
    extern u8 lbl_80314E08[];
    extern u8 lbl_803A1D60[];
    extern f64 lbl_8047B7D0;
    extern f64 lbl_8047B7E8;
    extern f64 lbl_8047B7F0;
    extern f64 lbl_8047B7F8;
    extern f64 lbl_8047B800;
    extern f64 sin(f64);
    extern f64 cos(f64);
    extern void fn_800D5BA0(s32, u32);
    extern void fn_800D61E4(s16, s16);
    extern void fn_800D6728(void);
    extern void fn_800D67BC(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(void*);
    extern void fn_800D888C(s32);
    extern void fn_800D88DC(s32);
    PCBoxVertex* corners = (PCBoxVertex*)lbl_803A1D60;
    PCBoxVertex* inner = (PCBoxVertex*)(lbl_803A1D60 + 0x28);
    PCBoxVertex* outer = (PCBoxVertex*)(lbl_803A1D60 + 0x128);
    s32 i;

    if (dimColor != 0) {
        color = (u8)((s32)color / 2);
    }

    corners[0].x = x + width;
    corners[0].y = y + height + 2;
    corners[1].x = x + width;
    corners[1].y = y;
    corners[2].x = x;
    corners[2].y = y;
    corners[3].x = x;
    corners[3].y = y + height + 2;
    corners[4].x = x + width / 2;
    corners[4].y = y + height / 2;
    corners[4].color = color;

    for (i = 0; i < 32; i++) {
        f64 angle = lbl_8047B7F0 * (f64)i * lbl_8047B7F8;
        PCBoxVertex* base = &corners[i / 8];

        outer[i].x = (s16)((f64)x + lbl_8047B7E8 * sin(angle));
        outer[i].y = (s16)((f64)y + lbl_8047B7E8 * cos(angle));
        outer[i].color = 0;
        inner[i].x = (s16)((f64)x + lbl_8047B800 * sin(angle));
        inner[i].y = (s16)((f64)y + lbl_8047B800 * cos(angle));
        inner[i].color = color;
        outer[i].x += base->x;
        outer[i].y += base->y;
        inner[i].x += base->x;
        inner[i].y += base->y;
    }

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D6A00(4);
    fn_800D7820(lbl_80314E08);
    fn_800D67BC(0x42);
    for (i = 0; i < 32; i++) {
        fn_800D61E4(outer[i].x, outer[i].y);
        fn_800D5BA0(0, outer[i].color);
        fn_800D61E4(inner[i].x, inner[i].y);
        fn_800D5BA0(0, inner[i].color);
    }
    fn_800D61E4(outer[0].x, outer[0].y);
    fn_800D5BA0(0, outer[0].color);
    fn_800D61E4(inner[0].x, inner[0].y);
    fn_800D5BA0(0, inner[0].color);
    fn_800D6728();

    fn_800D6A00(5);
    fn_800D67BC(0x22);
    fn_800D61E4(corners[4].x, corners[4].y);
    fn_800D5BA0(0, corners[4].color);
    for (i = 0; i < 32; i++) {
        fn_800D61E4(inner[i].x, inner[i].y);
        fn_800D5BA0(0, inner[i].color);
    }
    fn_800D61E4(inner[0].x, inner[0].y);
    fn_800D5BA0(0, inner[0].color);
    fn_800D6728();
}
#endif

/* 0x8001EF78 | 0x270 */
extern void fn_801666BC(void);
extern f64 lbl_8047B828;
extern f64 lbl_8047B830;
extern f32 lbl_8047B810;
extern f32 lbl_8047A338;
extern f32 lbl_8047B818;
extern f32 lbl_8047A344;
extern f32 lbl_8047B814;
extern u32 lbl_8047A31C;
extern f32 lbl_8047A334;
extern u8 lbl_803A1F88[];
extern f32 lbl_8047B81C;
extern f32 lbl_8047B820;
extern f32 lbl_8047B824;
#if 0
asm void fn_8001EF78(void) {
#include "src/game/gs_pcbox_fn_8001EF78.inc"
}
#else
void fn_8001EF78(void) {
    extern u8 lbl_803A1F88[];
    extern u32 lbl_8047A31C;
    extern f32 lbl_8047A334;
    extern f32 lbl_8047A338;
    extern f32 lbl_8047A344;
    extern f32 lbl_8047B810;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B818;
    extern f32 lbl_8047B81C;
    extern f32 lbl_8047B820;
    extern f32 lbl_8047B824;
    extern f64 lbl_8047B828;
    extern f64 lbl_8047B830;
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void fn_801666BC();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    fn_800D37CC();
    tmp = 0x43300000;
    f1 = lbl_8047B828;
    *(u32*)(sp + 0x8) = tmp;
    f31 = f0 - f1;
    fn_800D3088();
    tmp = 0x43300000;
    f3 = lbl_8047B830;
    *(u32*)(sp + 0x10) = tmp;
    f2 = lbl_8047B810;
    f1 = lbl_8047A338;
    f3 = f0 - f3;
    f0 = lbl_8047B818;
    f3 = f3 / f31;
    f1 = f3 * f2 + f1;
    lbl_8047A344 = f3;
    lbl_8047A338 = f1;
    /* cror eq, gt, eq */;
    if (f1 == f0) {
        f0 = lbl_8047B814;
        lbl_8047A338 = f0;
    }
    tmp = lbl_8047A31C;
    if ((s32)tmp < 0x1e) return;
    if ((s32)tmp == 0xc8) return;
    f0 = lbl_8047A334;
    r3 = 0x46a;
    f0 = f0 + f3;
    lbl_8047A334 = f0;
    fn_801666BC();
    if ((s32)r3 == 0) {
        tmp = lbl_8047A31C;
        f0 = lbl_8047B814;
        lbl_8047A334 = f0;
        if ((s32)tmp != 0x3e8) {
            tmp = 0x28;
            lbl_8047A31C = tmp;
    }
    }
    r3 = (u32)lbl_803A1F88;
    f0 = lbl_8047A344;
    r3 = (u32)lbl_803A1F88;
    tmp = 0x3;
    ctr_fn = (void(*)(void))tmp;
    do {
        f1 = *(f32*)((u8*)r3 + 0x10);
        f2 = *(f32*)((u8*)r3 + 0x28);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x10);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x10) = f2;
            f3 = *(f32*)((u8*)r3 + 0x28);
            f1 = *(f32*)((u8*)r3 + 0x10);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x10) = f3;
        } while (0);
        f1 = *(f32*)((u8*)r3 + 0x1C);
        f2 = *(f32*)((u8*)r3 + 0x34);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x1C);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x1C) = f2;
            f3 = *(f32*)((u8*)r3 + 0x34);
            f1 = *(f32*)((u8*)r3 + 0x1C);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x1C) = f3;
        } while (0);
        r3 = r3 + 0x4;
    } while (--ctr != 0);

    return;
}
#endif

/* 0x8001F1E8 | 0x11C */
extern void dbgMenuSetEnable(void);
extern void fn_801669E4(void);
extern u8* windowGetKeyInfo();
extern void fn_80166AB8(void);
extern u32 lbl_8047A31C;
extern u32 lbl_8047A328;
#pragma peephole off
void fn_8001F1E8(u8* arg) {
    extern u32 lbl_8047A31C;
    extern u32 lbl_8047A328;
    extern u32 fn_800F7AF0(s32);
    extern u32 fn_800F7BC4(s32);
    extern u8* windowGetKeyInfo(void);
    extern void dbgMenuSetEnable(s32);
    extern void fn_801669E4(s32, s32, s32);
    extern void fn_80166AB8(s32, s32, s32);
    extern void* menuDataBiosGetPtr(u32);
    u32 a;
    u32 b;
    u8* obj;

    if ((s32)lbl_8047A31C < 4) {
        dbgMenuSetEnable(0);
        if (arg == 0) return;
        menuDataBiosGetPtr(*(u32*)(arg + 0x4));
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if ((b & a) & 0x1100) {
            lbl_8047A31C = 4;
            fn_801669E4(0x46e, 0, 0);
            lbl_8047A328 = 1;
        }
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if (((b & a) & 0x200) == 0) return;
        lbl_8047A31C = 4;
        fn_801669E4(0x46e, 0, 0);
        lbl_8047A328 = 1;
        return;
    }
    dbgMenuSetEnable(1);
    if (arg == 0) return;
    menuDataBiosGetPtr(*(u32*)(arg + 0x4));
    obj = windowGetKeyInfo();
    if ((*(u16*)(obj + 0x4) & 0x810) == 0) return;
    *(u8*)(arg + 0x98) = 1;
    fn_80166AB8(0x4c2, 0, 0);
}
#pragma peephole reset

/* 0x8001F304 | 0xA44 */
extern u32 lbl_8047A31C;
extern f32 lbl_8047B838;
extern f32 lbl_8047A338;
extern f32 lbl_8047B81C;
extern f32 lbl_8047B83C;
extern f32 lbl_8047B844;
extern f32 lbl_8047B840;
extern f32 lbl_8047B848;
extern f32 lbl_8047B814;
extern u32 lbl_80478878;
extern u8 lbl_802EF0A8[];
extern u8 lbl_8047A34C;
#if 0
asm void fn_8001F304(void) {
#include "src/game/gs_pcbox_fn_8001F304.inc"
}
#else
void fn_8001F304(void* arg0, void* arg1) {
    extern u8 lbl_802EF0A8[];
    extern u8 lbl_803A1F88[];
    extern u32 lbl_80478878;
    extern u32 lbl_8047A31C;
    extern f32 lbl_8047A338;
    extern u8 lbl_8047A34C;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B81C;
    extern f32 lbl_8047B838;
    extern f32 lbl_8047B83C;
    extern f32 lbl_8047B840;
    extern f32 lbl_8047B844;
    extern f32 lbl_8047B848;
    extern void fn_800CE148();
    u8 sp[0x20];
    u32 tmp;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r31;
    f32 f0;
    f32 f1;
    f32 f2;
    f32 f3;
    f32 f5;
    f32 f6;
    f32 f8;
    f32 f9;

    r31 = (u32)arg1;
    tmp = *(s16*)((u8*)r31 + 0x6);
    if ((s32)tmp != 0xf03) {
        if ((s32)tmp < 0xf03) {
            if ((s32)tmp != 0xefd) {
                if ((s32)tmp < 0xefd) {
                    if ((s32)tmp != 0xefa) {
                        if ((s32)tmp < 0xefa) {
                            if ((s32)tmp != 0x2c4) {
                                goto L_8001FD18;
                            }
                            if ((s32)tmp < 0xefc) {
                                goto L_8001F4C8;
                            }
                            if ((s32)tmp == 0xf00) goto L_8001F6F8;
                            if ((s32)tmp < 0xf00) {
                                if ((s32)tmp >= 0xeff) goto L_8001F688;
                                goto L_8001F618;
                            }
                            if ((s32)tmp >= 0xf02) goto L_8001F7D8;
                            goto L_8001F768;
                        }
                        if ((s32)tmp == 0xf09) goto L_8001FAE8;
                        if ((s32)tmp < 0xf09) {
                            if ((s32)tmp == 0xf06) goto L_8001F998;
                            if ((s32)tmp < 0xf06) {
                                if ((s32)tmp >= 0xf05) goto L_8001F928;
                                goto L_8001F8B8;
                            }
                            if ((s32)tmp >= 0xf08) goto L_8001FA78;
                            goto L_8001FA08;
                        }
                        if ((s32)tmp == 0xf0c) goto L_8001FC38;
                        if ((s32)tmp < 0xf0c) {
                            if ((s32)tmp >= 0xf0b) goto L_8001FBC8;
                            goto L_8001FB58;
                        }
                        if ((s32)tmp >= 0xf0e) goto L_8001FD18;
                        goto L_8001FCA8;
                            }
                    tmp = lbl_8047A31C;
                    if ((s32)tmp >= 0x1e) {
                        if ((s32)tmp > 0x20) {
                        }
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r3 = 0x0;
                        tmp = tmp & 0xFFFFFFFD;
                        tmp = (s8)tmp;
                        *(u8*)((u8*)r31 + 0x4) = tmp;

                        } else {
                        tmp = *(u8*)((u8*)r31 + 0x4);
                        r3 = 0x1;
                        tmp = tmp | 0x2;
                        tmp = (s8)tmp;
                        *(u8*)((u8*)r31 + 0x4) = tmp;
                        }
                    tmp = r3 & 0xFF;
                    if (tmp == 0) return;
                    tmp = lbl_8047A31C;
                    f2 = lbl_8047B838;
                    f1 = lbl_8047A338;
                    if ((s32)tmp == 0x1f) {
                        f0 = lbl_8047B81C;
                    } else {

                        f0 = lbl_8047B83C;
                    }
                    f0 = f1 * f0;
                    f1 = f2 * f0;
                    fn_800CE148();
                    f3 = (f32)f1;
                    f2 = lbl_8047B844;
                    f1 = lbl_8047B840;
                    f0 = lbl_8047B848;
                    f1 = f2 * f3 + f1;
                    if (f1 > f0) {
                        f1 = f0;
                    }
                    f0 = lbl_8047B814;
                    if (f1 < f0) {
                        f1 = f0;
                    }
                    f0 = (f64)(s32)f1;
                    *(u8*)((u8*)r31 + 0x67) = tmp;
                    return;
                                }
                tmp = lbl_8047A31C;
                if ((s32)tmp >= 0x1e) {
                    if ((s32)tmp > 0x20) {
                    }
                    tmp = *(u8*)((u8*)r31 + 0x4);
                    tmp = tmp & 0xFFFFFFFD;
                    tmp = (s8)tmp;
                    *(u8*)((u8*)r31 + 0x4) = tmp;
                    return;
                    }
                tmp = *(u8*)((u8*)r31 + 0x4);
                tmp = tmp | 0x2;
                tmp = (s8)tmp;
                *(u8*)((u8*)r31 + 0x4) = tmp;
                return;
            L_8001F4C8:
                tmp = lbl_80478878;
                if ((s32)tmp == 0) {
                    tmp = *(u8*)((u8*)r31 + 0x4);
                    r3 = (u32)lbl_803A1F88;
                    r4 = (u32)lbl_802EF0A8;
                    tmp = tmp | 0x2;
                    r3 = (u32)lbl_803A1F88;
                    r5 = (s8)tmp;
                    tmp = (u32)lbl_802EF0A8;
                    *(u8*)((u8*)r31 + 0x4) = r5;
                    f0 = *(f32*)((u8*)r3 + 0x1C);
                    r3 = *(s16*)((u8*)r31 + 0x6);
                    f0 = (f64)(s32)f0;
                    r3 = r3 * 0x1c;
                    r3 = tmp + r3;
                    r3 = *(s16*)((u8*)r3 + 0x2);
                    tmp = r3 + tmp;
                    tmp = (s16)tmp;
                    *(u16*)((u8*)r31 + 0x50) = tmp;
                    return;
                }
                tmp = *(u8*)((u8*)r31 + 0x4);
                tmp = tmp & 0xFFFFFFFD;
                tmp = (s8)tmp;
                *(u8*)((u8*)r31 + 0x4) = tmp;
                return;
                            }
            tmp = lbl_80478878;
            if ((s32)tmp == 0) {
                tmp = *(u8*)((u8*)r31 + 0x4);
                r3 = (u32)lbl_803A1F88;
                r4 = (u32)lbl_802EF0A8;
                tmp = tmp | 0x2;
                r3 = (u32)lbl_803A1F88;
                r5 = (s8)tmp;
                tmp = (u32)lbl_802EF0A8;
                *(u8*)((u8*)r31 + 0x4) = r5;
                f0 = *(f32*)((u8*)r3 + 0x20);
                r3 = *(s16*)((u8*)r31 + 0x6);
                f0 = (f64)(s32)f0;
                r3 = r3 * 0x1c;
                r3 = tmp + r3;
                r3 = *(s16*)((u8*)r3 + 0x2);
                tmp = r3 + tmp;
                tmp = (s16)tmp;
                *(u16*)((u8*)r31 + 0x50) = tmp;
                return;
            }
            tmp = *(u8*)((u8*)r31 + 0x4);
            tmp = tmp & 0xFFFFFFFD;
            tmp = (s8)tmp;
            *(u8*)((u8*)r31 + 0x4) = tmp;
            return;
            }
        tmp = lbl_80478878;
        if ((s32)tmp == 0) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x10);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F618:
        tmp = lbl_80478878;
        if ((s32)tmp == 0) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x14);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F688:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x1C);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F6F8:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x20);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F768:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x24);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    L_8001F7D8:
        tmp = lbl_80478878;
        if ((s32)tmp == 1) {
            tmp = *(u8*)((u8*)r31 + 0x4);
            r3 = (u32)lbl_803A1F88;
            r4 = (u32)lbl_802EF0A8;
            tmp = tmp | 0x2;
            r3 = (u32)lbl_803A1F88;
            r5 = (s8)tmp;
            tmp = (u32)lbl_802EF0A8;
            *(u8*)((u8*)r31 + 0x4) = r5;
            f0 = *(f32*)((u8*)r3 + 0x10);
            r3 = *(s16*)((u8*)r31 + 0x6);
            f0 = (f64)(s32)f0;
            r3 = r3 * 0x1c;
            r3 = tmp + r3;
            r3 = *(s16*)((u8*)r3 + 0x2);
            tmp = r3 + tmp;
            tmp = (s16)tmp;
            *(u16*)((u8*)r31 + 0x50) = tmp;
            return;
        }
        tmp = *(u8*)((u8*)r31 + 0x4);
        tmp = tmp & 0xFFFFFFFD;
        tmp = (s8)tmp;
        *(u8*)((u8*)r31 + 0x4) = tmp;
        return;
    }
    tmp = lbl_80478878;
    if ((s32)tmp == 1) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F8B8:
    tmp = lbl_80478878;
    if ((s32)tmp == 1) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x18);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F928:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x1C);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001F998:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x20);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FA08:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x10);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FA78:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FAE8:
    tmp = lbl_80478878;
    if ((s32)tmp == 2) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x18);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FB58:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x10);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FBC8:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x14);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FC38:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x1C);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FCA8:
    tmp = lbl_80478878;
    if ((s32)tmp == 3) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        r3 = (u32)lbl_803A1F88;
        r4 = (u32)lbl_802EF0A8;
        tmp = tmp | 0x2;
        r3 = (u32)lbl_803A1F88;
        r5 = (s8)tmp;
        tmp = (u32)lbl_802EF0A8;
        *(u8*)((u8*)r31 + 0x4) = r5;
        f0 = *(f32*)((u8*)r3 + 0x20);
        r3 = *(s16*)((u8*)r31 + 0x6);
        f0 = (f64)(s32)f0;
        r3 = r3 * 0x1c;
        r3 = tmp + r3;
        r3 = *(s16*)((u8*)r3 + 0x2);
        tmp = r3 + tmp;
        tmp = (s16)tmp;
        *(u16*)((u8*)r31 + 0x50) = tmp;
        return;
    }
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;
    return;
L_8001FD18:
    tmp = lbl_8047A34C;
    if (tmp != 0) return;
    tmp = *(u8*)((u8*)r31 + 0x4);
    tmp = tmp & 0xFFFFFFFD;
    tmp = (s8)tmp;
    *(u8*)((u8*)r31 + 0x4) = tmp;

    return;
}
#endif

/* 0x8001FD48 | 0x5E0 */
extern void fn_801662E8(void);
extern void fn_80165A20(void);
extern void fn_8017B1AC(void);
extern void GSmodelAllPauseAnimation(void);
extern void cameraStopAnimation(void);
extern void fn_8016557C(void);
extern void cameraStartAnimation(void);
extern void fn_80165548(void);
extern void GSmodelAllUnpauseAnimation(void);
extern void GSmodelSetAnimIndex(void);
extern void GSmodelSetAnimFrame(void);
extern void GSmodelSetAnimRate(void);
extern void GSmodelStartAnimation(void);
extern void GSmodelSetAnimType(void);
extern void cameraPlayAnime(void);
extern void GSmodelIsAnimating(void);
extern void fn_801D0748(void);
extern void savedataCreate(void);
extern void gamedatasaveGetStatus(void);
extern void fn_800216E0(void);
extern void fn_80113828(void);
extern f32 lbl_8047B814;
extern u32 lbl_8047A31C;
extern f32 lbl_8047A348;
extern f32 lbl_8047A340;
extern u32 lbl_8047A324;
extern u32 lbl_8047A33C;
extern u32 lbl_8047A328;
extern u32 lbl_8047A310;
extern u32 lbl_8047A320;
extern u32 lbl_8047A318;
extern u32 lbl_8047A314;
extern f32 lbl_8047B810;
extern f32 lbl_8047A344;
extern f32 lbl_8047B818;
#if 0
asm void fn_8001FD48(void) {
#include "src/game/gs_pcbox_fn_8001FD48.inc"
}
#else
void fn_8001FD48(void) {
    extern u32 lbl_8047A310;
    extern u32 lbl_8047A314;
    extern u32 lbl_8047A318;
    extern u32 lbl_8047A31C;
    extern u32 lbl_8047A320;
    extern u32 lbl_8047A324;
    extern u32 lbl_8047A328;
    extern u32 lbl_8047A33C;
    extern f32 lbl_8047A340;
    extern f32 lbl_8047A344;
    extern f32 lbl_8047A348;
    extern f32 lbl_8047B810;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B818;
    extern void fn_800216E0();
    extern void GSmodelAllUnpauseAnimation();
    extern void GSmodelAllPauseAnimation();
    extern void GSmodelIsAnimating();
    extern void GSmodelStartAnimation();
    extern void GSmodelSetAnimRate();
    extern void GSmodelSetAnimFrame();
    extern void GSmodelSetAnimType();
    extern void GSmodelSetAnimIndex();
    extern void _threadSwitch();
    extern void fn_800F92D4();
    extern void menuOpen();
    extern void menuOpenCustom();
    extern void windowGetActiveID();
    extern void fn_80113828();
    extern void savedataCreate();
    extern void gamedatasaveGetStatus();
    extern void fn_80165548();
    extern void fn_8016557C();
    extern void fn_80165A20();
    extern void fn_801662E8();
    extern void cameraStartAnimation();
    extern void cameraStopAnimation();
    extern void cameraPlayAnime();
    extern void fn_8017B1AC();
    extern void fadeCheck();
    extern void fadeSet();
    extern void fn_801D0748();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;

    f0 = lbl_8047B814;
    tmp = 0x0;
    r3 = 0xB540000;
    lbl_8047A31C = tmp;
    r3 = r3 + 0x1000;
    r31 = 0x1;
    lbl_8047A348 = f0;
    fn_800F92D4();
    r4 = r3;
    f0 = lbl_8047B814;
    tmp = 0x0;
    r3 = 0x0;
    r30 = r4;
    lbl_8047A340 = f0;
    r4 = 0x406;
    lbl_8047A324 = r30;
    lbl_8047A33C = tmp;
    lbl_8047A328 = tmp;
    fn_801662E8();
    r3 = 0x46a;
    r4 = 0x0;
    r5 = 0xff;
    fn_80165A20();
    tmp = lbl_8047A310;
    if ((s32)tmp == 1) {
        r4 = 0x7;
        r3 = 0x0;
        tmp = 0x1;
        lbl_8047A31C = r4;
        lbl_8047A310 = r3;
        lbl_8047A320 = tmp;
    } else {

        tmp = 0x0;
        lbl_8047A320 = tmp;
    }
    r3 = 0x15;
    r4 = 0x0;
    menuOpen();
    do {
    do {
        fn_8017B1AC();
        r29 = r3;

        if ((s32)r29 == 0xb || (s32)r29 == 4) {

            tmp = lbl_8047A318;
            if ((s32)tmp == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                fn_8016557C();
                tmp = 0x1;
                lbl_8047A318 = tmp;
            }
            _threadSwitch();
            break;
        }
        tmp = lbl_8047A318;
        if ((s32)tmp == 1) {
            cameraStartAnimation();
            r3 = 0x0;
            r4 = 0x406;
            fn_801662E8();
            fn_80165548();
            GSmodelAllUnpauseAnimation();
            tmp = 0x0;
            lbl_8047A318 = tmp;
        }
        if ((s32)r29 == 5) {
            tmp = lbl_8047A314;
            if ((s32)tmp == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                fn_8016557C();
                tmp = 0x1;
                lbl_8047A314 = tmp;
            }
            _threadSwitch();
            break;
        }
        tmp = lbl_8047A314;
        if ((s32)tmp == 1) {
            cameraStartAnimation();
            r3 = 0x0;
            r4 = 0x406;
            fn_801662E8();
            fn_80165548();
            GSmodelAllUnpauseAnimation();
            tmp = 0x0;
            lbl_8047A314 = tmp;
        }
        tmp = lbl_8047A31C;
        if ((s32)tmp != 0x1e) {
            if ((s32)tmp < 0x1e) {
                if ((s32)tmp != 4) {
                    if ((s32)tmp < 4) {
                        if ((s32)tmp != 1) {
                            if ((s32)tmp < 1) {
                                if ((s32)tmp < 0) {
                                    break;
                                }
                                if ((s32)tmp < 3) {
                                    goto L_80020018;
                                }
                                if ((s32)tmp == 7) goto L_8002010C;
                                if ((s32)tmp < 7) {
                                    if ((s32)tmp >= 6) goto L_800200D8;
                                    goto L_800200B0;
                                }
                                if ((s32)tmp >= 9) break;
                                goto L_80020184;
                            }
                            if ((s32)tmp == 0xc8) goto L_80020274;
                            if ((s32)tmp < 0xc8) {
                                if ((s32)tmp == 0x28) goto L_800201E8;
                                if ((s32)tmp < 0x28) {
                                    if ((s32)tmp == 0x20) goto L_80020200;
                                    if ((s32)tmp >= 0x20) break;
                                    goto L_80020240;
                                }
                                if ((s32)tmp >= 0x2a) break;
                                goto L_800202E0;
                            }
                            if ((s32)tmp == 0x3e8) goto L_800202F8;
                            if ((s32)tmp >= 0x3e8) break;
                            break;
                                    }
                        if (r30 != 0) {
                            r3 = r30;
                            r4 = 0x0;
                            GSmodelSetAnimIndex();
                            f1 = lbl_8047B814;
                            r3 = r30;
                            GSmodelSetAnimFrame();
                            f1 = lbl_8047B810;
                            r3 = r30;
                            GSmodelSetAnimRate();
                            r3 = r30;
                            GSmodelStartAnimation();
                        }
                        r3 = r30;
                        r4 = 0x0;
                        GSmodelSetAnimType();
                        r4 = 0xB550000;
                        r3 = 0x12;
                        r4 = r4 + 0x1800;
                        r5 = 0x0;
                        r6 = 0x0;
                        cameraPlayAnime();
                        f1 = lbl_8047B810;
                        r3 = 0x4;
                        fadeSet();
                        tmp = lbl_8047A31C;
                        if ((s32)tmp != 0) break;
                        tmp = 0x1;
                        lbl_8047A31C = tmp;
                        break;
                            }
                    r3 = r30;
                    GSmodelIsAnimating();
                    tmp = r3 & 0xFF;
                    if (tmp == 0) {
                        tmp = 0x2;
                        lbl_8047A31C = tmp;
                        break;
                    }
                    _threadSwitch();
                    break;
                L_80020018:
                    if (r30 != 0) {
                        r3 = r30;
                        r4 = 0x1;
                        GSmodelSetAnimIndex();
                        f1 = lbl_8047B814;
                        r3 = r30;
                        GSmodelSetAnimFrame();
                        f1 = lbl_8047B810;
                        r3 = r30;
                        GSmodelSetAnimRate();
                        r3 = r30;
                        GSmodelStartAnimation();
                    }
                    r3 = r30;
                    r4 = 0x0;
                    GSmodelSetAnimType();
                    tmp = 0x3;
                    lbl_8047A31C = tmp;
                    break;
                                }
                if (r30 == 0) {

                } else {
                    goto L_80020078;
                do {
                        _threadSwitch();
                    L_80020078:
                        r3 = r30;
                        GSmodelIsAnimating();
                        tmp = r3 & 0xFF;
                } while (tmp != 0);
                }
                tmp = 0x8;
                lbl_8047A31C = tmp;
                break;
                }
            f1 = lbl_8047B810;
            r3 = 0x5;
            fadeSet();
            tmp = 0x6;
            lbl_8047A31C = tmp;
            break;
        L_800200B0:
            r3 = 0x0;
            fadeCheck();
            tmp = (s8)r3;
            if ((s32)tmp == 0) {
                tmp = 0x6;
                lbl_8047A31C = tmp;
                break;
            }
            _threadSwitch();
            break;
        L_800200D8:
            f31 = lbl_8047B810;
            while (1) {
                f0 = lbl_8047A348;
                if (f0 >= f31) break;
                f1 = lbl_8047A348;
                f0 = lbl_8047A344;
                f0 = f1 + f0;
                lbl_8047A348 = f0;
                _threadSwitch();

            }
            tmp = 0x7;
            lbl_8047A31C = tmp;
            break;
        L_8002010C:
            r4 = 0xB560000;
            r3 = 0x12;
            r4 = r4 + 0x1800;
            r5 = 0x0;
            r6 = 0x0;
            cameraPlayAnime();
            if (r30 != 0) {
                r3 = r30;
                r4 = 0x2;
                GSmodelSetAnimIndex();
                f1 = lbl_8047B814;
                r3 = r30;
                GSmodelSetAnimFrame();
                f1 = lbl_8047B810;
                r3 = r30;
                GSmodelSetAnimRate();
                r3 = r30;
                GSmodelStartAnimation();
            }
            r3 = r30;
            r4 = 0x1;
            GSmodelSetAnimType();
            f1 = lbl_8047B810;
            r3 = 0x4;
            fadeSet();
            r3 = 0x1;
            fadeCheck();
            tmp = 0x1e;
            lbl_8047A31C = tmp;
            break;
        L_80020184:
            r4 = 0xB560000;
            r3 = 0x12;
            r4 = r4 + 0x1800;
            r5 = 0x0;
            r6 = 0x0;
            cameraPlayAnime();
            if (r30 != 0) {
                r3 = r30;
                r4 = 0x2;
                GSmodelSetAnimIndex();
                f1 = lbl_8047B814;
                r3 = r30;
                GSmodelSetAnimFrame();
                f1 = lbl_8047B810;
                r3 = r30;
                GSmodelSetAnimRate();
                r3 = r30;
                GSmodelStartAnimation();
            }
            r3 = r30;
            r4 = 0x1;
            GSmodelSetAnimType();
            tmp = 0x1e;
            lbl_8047A31C = tmp;
            break;
        L_800201E8:
            tmp = 0x3e8;
            lbl_8047A31C = tmp;
            break;
        }
        tmp = 0x20;
        lbl_8047A31C = tmp;
        break;
    L_80020200:
        windowGetActiveID();
        r4 = r3;
        r3 = 0x13;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        menuOpenCustom();
        if ((s32)r3 < 0) break;
        f0 = lbl_8047B814;
        tmp = 0x1f;
        lbl_8047A31C = tmp;
        lbl_8047A348 = f0;
        break;
    L_80020240:
        f31 = lbl_8047B818;
        while (1) {
            f0 = lbl_8047A348;
            if (f0 >= f31) break;
            f1 = lbl_8047A348;
            f0 = lbl_8047A344;
            f0 = f1 + f0;
            lbl_8047A348 = f0;
            _threadSwitch();

        }
        tmp = 0xc8;
        lbl_8047A31C = tmp;
        break;
    L_80020274:
        r3 = 0x1;
        r4 = 0x2;
        r5 = 0x0;
        fn_801D0748();
        if ((s32)r3 == (s32)-0x1) {
            tmp = 0x20;
            lbl_8047A31C = tmp;
            break;
        }
        if ((s32)r3 != 3) {
            r3 = 0x0;
            r4 = 0x0;
            savedataCreate();
        }
        r3 = 0x0;
        r4 = 0x4;
        gamedatasaveGetStatus();
        if (r3 != 0) {
            r3 = 0x1;
            fn_800216E0();
        } else {

            r3 = 0x0;
            fn_800216E0();
        }
        tmp = 0x29;
        lbl_8047A31C = tmp;
        break;
    L_800202E0:
        r3 = 0x3a1;
        r4 = 0x0;
        fn_80113828();
        tmp = 0x3e8;
        lbl_8047A31C = tmp;
        break;
    L_800202F8:
        r31 = 0x0;
    } while (0);
        ;
    } while ((s32)r31 != 0);
    return;
}
#endif

/* fn_8001E58C - 0x8001E58C | size: 0xb8 */
#if 0
asm void fn_8001E58C(void) {
#include "src/game/gs_pcbox_fn_8001E58C.inc"
}
#else
#pragma optimization_level 4
void fn_8001E58C(s16 x1, s16 y1, s16 x2, s16 y2, u8* color) {
    extern void fn_800D88DC();
    extern void fn_800D888C();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D67BC();
    extern void fn_800D61E4();
    extern void fn_800D5CB8();
    extern void fn_800D6728();
    extern u8 lbl_80314E08[];
    fn_800D88DC(0x1);
    fn_800D888C(0x6);
    fn_800D6A00(0x7);
    fn_800D7820(lbl_80314E08);
    x2 += x1;
    y2 += y1;
    fn_800D67BC(0x2);
    fn_800D61E4(x1, y1);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    fn_800D61E4(x2, y2);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    fn_800D6728();
}
#endif
