/**
 * @file menuSub.c
 * @brief menuSub -- shared menu sub-widgets (yes/no, number input, color calc).
 *
 * Address range: 0x8001D7E4 - 0x8001E3E0 (13 functions)
 *
 * Split from game/gs_pcbox.c (originally mislabeled -- this range is
 * actually shared menu sub-widget helper code, not the PC box module).
 * Corresponds to XD game/menuSub.cpp.
 */

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

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

/* 0x50 | menuSubKeyWait | multi_call_cond */
extern u32 fn_800F7AF0(s32);
extern u32 fn_800F7BC4(s32);
#pragma peephole off
u32 menuSubKeyWait(void) {
    u32 a;
    u32 b;
    u32 m;
    goto _test;
    do {
        _threadSwitch();
    _test:
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        m = (b & a) & 0x300;
    } while (m == 0);
    return b;
}
#pragma peephole reset

typedef struct MenuSubColorContext {
    u8 pad_00[0x88];
    GXColor color;
} MenuSubColorContext;

typedef struct MenuSubColorItem {
    u8 pad_00[0x64];
    GXColor color;
} MenuSubColorItem;

/* menuSubCalcColor - 0x8001D834 | size: 0xB4 */
#pragma push
#pragma peephole off
u32 menuSubCalcColor(MenuSubColorContext* context, MenuSubColorItem* item)
{
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;

    red = (u8)((item->color.r * context->color.r) / 255);
    green = (u8)((item->color.g * context->color.g) / 255);
    blue = (u8)((item->color.b * context->color.b) / 255);
    alpha = (u8)((item->color.a * context->color.a) / 255);
    return (red << 24) | (green << 16) | (blue << 8) | alpha;
}
#pragma pop

/* 0x8001D8E8 | 0xAC */
extern void windowGetActiveID();
extern void menuSetPosition(void);
extern void windowCheckCursor(void);
extern void windowGetValue(void);
u32 menuSubOpenSelect(u8 selection, u32 text, u32 value, s16 x, s16 y, u32 flags) {
    extern u32 windowGetActiveID(void);
    extern void menuOpenCustom(u32, u32, void*, u32, u32, u32, u32, u32, ...);
    extern void menuSetPosition(u32, s16, s16);
    extern void windowCheckCursor(u32, u32);
    extern u32 windowGetValue(u32);
    extern void menuCloseCustom(u32, u32, u32);
    u32 openFlags;
    u32 result;

    openFlags = flags;
    menuOpenCustom(0xE7, windowGetActiveID(), &openFlags, 0, 0, 3,
                   selection, text, value);
    menuSetPosition(0xE7, x, y);
    windowCheckCursor(0xE7, 1);
    result = windowGetValue(0xE7);
    menuCloseCustom(0xE7, 0, 1);
    return result;
}

/* 0x8001D994 | 0xCC */
extern u16 pokemonBiosGetPokemonDataId(void*);
extern void* pokemonDataBiosGetPtr(u16);
extern void* pokemonDataBiosGetName(void*);
extern void* GSmsgGetGSchar(void*);
extern s32 GScharCmp(void*, void*);
extern u8 pokemonGetSex(void*);
extern u8 pokemonCheckValid(void*);
extern u32 pokemonGetStatus(void*, s32, s32, s32);

u8 menuSubGetPokemonSexForFightDisp(void* pokemon)
{
    void* species;
    void* name;
    u16 species_id;

    if (!pokemonCheckValid(pokemon)) {
        return 0xFF;
    }

    species_id = pokemonBiosGetPokemonDataId(pokemon);
    if (species_id == 0x1D || species_id == 0x20) {
        species = pokemonDataBiosGetPtr(
            (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0));
        if (species != NULL) {
            name = GSmsgGetGSchar(pokemonDataBiosGetName(species));
            if (GScharCmp((void*)pokemonGetStatus(pokemon, 0, 0x77, 0),
                          name) == 0) {
                return 2;
            }
        }
    }
    return pokemonGetSex(pokemon);
}

/* 0x8001DACC | 0x4DC */
extern void fn_800F92D4(void);
extern void GStextureGetXsize(void);
extern void GStextureGetYsize(void);
extern void fn_800D88DC(void);
extern void fn_800D888C(void);
extern void GStextureSetWrap(void);
extern void fn_800D848C(void);
extern void fn_800DC1D4(void);
extern void fn_800D85D4(void);
extern void fn_800DC224(void);
extern void fn_800DC14C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC04C(void);
extern void fn_800DBFD4(void);
extern void fn_800D7820(void);
extern void fn_800D6A00(void);
extern void fn_800D67BC(void);
extern void fn_800D61E4(void);
extern void fn_800D5CB8(void);
extern void fn_800D59B8(void);
extern void fn_800D6728(void);
extern f64 lbl_8047B7D8;
extern f64 lbl_8047B7D0;
extern u32 lbl_8047AD00;
extern f32 lbl_8047B7E0;
#if 0
asm void fn_8001DACC(void) {
#include "src/game/gs_pcbox_fn_8001DACC.inc"
}
#else
void fn_8001DACC(void) {
    extern u32 lbl_8047AD00;
    extern f64 lbl_8047B7D0;
    extern f64 lbl_8047B7D8;
    extern f32 lbl_8047B7E0;
    extern void fn_800D59B8();
    extern void fn_800D5CB8();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D848C();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800DBFD4();
    extern void fn_800DC04C();
    extern void fn_800DC0D4();
    extern void fn_800DC14C();
    extern void fn_800DC1D4();
    extern void fn_800DC224();
    extern void GStextureGetYsize();
    extern void GStextureGetXsize();
    extern void GStextureSetWrap();
    extern void fn_800F92D4();
    u8 sp[0xF0];
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
    f32 f2 = 0.0f;
    f32 f4 = 0.0f;
    f32 f26 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r29 = r4;
    f29 = f1;
    r3 = *(u32*)((u8*)r29 + 0x58);
    fn_800F92D4();
    r4 = 0xEF0000;
    r31 = r3;
    r3 = r4 + 0x1200;
    fn_800F92D4();
    r30 = r3;
    if ((r31 != 0) && (r30 != 0)) {

        r3 = r31;
        GStextureGetXsize();
        tmp = *(s16*)((u8*)r29 + 0x5C);
        r3 = r3 & 0xFFFF;
        r4 = 0x43300000;
        f2 = lbl_8047B7D8;
        r3 = r31;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x44) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f31 = f0 / f2;
        GStextureGetXsize();
        r4 = *(s16*)((u8*)r29 + 0x5C);
        r3 = r3 & 0xFFFF;
        tmp = *(s16*)((u8*)r29 + 0x60);
        r5 = 0x43300000;
        r3 = r31;
        tmp = r4 + tmp;
        f2 = lbl_8047B7D8;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x54) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f30 = f0 / f2;
        GStextureGetYsize();
        tmp = *(s16*)((u8*)r29 + 0x5E);
        r3 = r3 & 0xFFFF;
        r4 = 0x43300000;
        f2 = lbl_8047B7D8;
        r3 = r31;
        f1 = lbl_8047B7D0;
        *(u32*)(sp + 0x64) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f27 = f0 / f2;
        GStextureGetYsize();
        r4 = *(s16*)((u8*)r29 + 0x5E);
        r5 = 0x43300000;
        tmp = *(s16*)((u8*)r29 + 0x62);
        r6 = r3 & 0xFFFF;
        r3 = 0x80000000;
        tmp = r4 + tmp;
        f2 = lbl_8047B7D8;
        f1 = lbl_8047B7D0;
        r3 = r3 + 0x3;
        *(u32*)(sp + 0x74) = tmp;
        f2 = f0 - f2;
        f0 = f0 - f1;
        f26 = f0 / f2;
        fn_800D88DC();
        r3 = 0x4;
        fn_800D888C();
        r3 = r30;
        r4 = 0x1;
        r5 = 0x1;
        GStextureSetWrap();
        r6 = (u32)sp + 0x8;
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x4;
        fn_800D848C();
        r6 = (u32)sp + 0x8;
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x5;
        fn_800D848C();
        r6 = (u32)sp + 0x8;
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x6;
        fn_800D848C();
        r3 = 0x3;
        fn_800DC1D4();
        r4 = r31;
        r3 = 0x0;
        fn_800D85D4();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x1;
        fn_800DC14C();
        r3 = 0x0;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xa;
        r7 = 0xf;
        fn_800DC0D4();
        r3 = 0x0;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x1;
        fn_800DC04C();
        r3 = 0x0;
        r4 = 0x7;
        r5 = 0x4;
        r6 = 0x5;
        r7 = 0x7;
        fn_800DBFD4();
        r4 = r30;
        r3 = 0x1;
        fn_800D85D4();
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x1;
        r4 = 0x1;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC14C();
        r3 = 0x1;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xd;
        r7 = 0x2;
        fn_800DC0D4();
        r3 = 0x1;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC04C();
        r3 = 0x1;
        r4 = 0x7;
        r5 = 0x7;
        r6 = 0x7;
        r7 = 0x1;
        fn_800DBFD4();
        r4 = r30;
        r3 = 0x2;
        fn_800D85D4();
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x2;
        r6 = 0x2;
        r7 = 0x0;
        fn_800DC224();
        r3 = 0x2;
        r4 = 0x1;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC14C();
        r3 = 0x2;
        r4 = 0xf;
        r5 = 0x8;
        r6 = 0xd;
        r7 = 0x2;
        fn_800DC0D4();
        r3 = 0x2;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x1;
        r8 = 0x0;
        fn_800DC04C();
        r3 = 0x2;
        r4 = 0x7;
        r5 = 0x7;
        r6 = 0x7;
        r7 = 0x1;
        fn_800DBFD4();
        r3 = lbl_8047AD00;
        fn_800D7820();
        r3 = 0x7;
        fn_800D6A00();
        r3 = 0x2;
        fn_800D67BC();
        r3 = 0x0;
        r4 = 0x0;
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x40;
        r6 = 0x40;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = f31;
        r3 = 0x0;
        f2 = f27;
        fn_800D59B8();
        f28 = f29 + f27;
        r3 = 0x1;
        f1 = f29 + f31;
        f2 = f28;
        fn_800D59B8();
        f0 = lbl_8047B7E0;
        f2 = f28;
        r3 = 0x2;
        f27 = f0 - f29;
        f1 = f27 + f31;
        fn_800D59B8();
        r3 = *(s16*)((u8*)r29 + 0x54);
        r4 = *(s16*)((u8*)r29 + 0x56);
        fn_800D61E4();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x40;
        r6 = 0x40;
        r7 = 0xff;
        fn_800D5CB8();
        f1 = f30;
        r3 = 0x0;
        f2 = f26;
        fn_800D59B8();
        f28 = f29 + f26;
        r3 = 0x1;
        f1 = f29 + f30;
        f2 = f28;
        fn_800D59B8();
        f2 = f28;
        r3 = 0x2;
        f1 = f27 + f30;
        fn_800D59B8();
        fn_800D6728();
        r3 = 0x1;
        fn_800DC1D4();
        r3 = 0x80000000;
        fn_800D888C();
    }
    r3 = 0x0;
    return;
}
#endif

/* 0x64 | fn_8001DFA8 | generic_call_check_store */
/* fn_8001DFA8 - 0x8001DFA8 | size: 0x64 */
#pragma push
#pragma peephole off
s32 fn_8001DFA8(u32 arg1, u8* arg2) {
    extern u8 menuItemBiosGetSelectFlag(s16);
    if (menuItemBiosGetSelectFlag(*(s16*)(arg2 + 0x6)) != 0) {
        *(u8*)(arg2 + 0x66) = 0xff;
        *(u8*)(arg2 + 0x65) = 0xff;
        *(u8*)(arg2 + 0x64) = 0xff;
    } else {
        *(u8*)(arg2 + 0x66) = 0x80;
        *(u8*)(arg2 + 0x65) = 0x80;
        *(u8*)(arg2 + 0x64) = 0x80;
    }
    return 0;
}
#pragma pop

/* 0x68 | fn_8001E00C | call_sequence */
/* fn_8001E00C - 0x8001E00C | size: 0x68 */
#if 0
asm void fn_8001E00C(void) {
#include "src/game/gs_pcbox_fn_8001E00C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_8001E00C(u32 sp8) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuCloseCustom(s32, s32, s32);
    s32 r31;
    r31 = menuOpenCustom(0x43, windowGetActiveID(), &sp8, 0, 1, 0);
    menuCloseCustom(0x43, 0, 1);
    return r31;
}
#pragma pop
#endif

/* menuSubGetPokemonSexForDisp - 0x8001DA60 | size: 0x6c */
#if 0
asm void menuSubGetPokemonSexForDisp(void) {
#include "src/game/gs_pcbox_fn_8001DA60.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 menuSubGetPokemonSexForDisp(void* a) {
    extern u8 pokemonCheckValid();
    extern u32 pokemonBiosGetPokemonDataId();
    extern u32 pokemonGetSex();
    u32 r3;
    r3 = pokemonCheckValid();
    if ((r3 & 0xFF) == 0) return (s32)0xFF;
    r3 = pokemonBiosGetPokemonDataId(a);
    if ((r3 & 0xFFFF) == 0x1d || (r3 & 0xFFFF) == 0x20) return 0x2;
    return (s32)pokemonGetSex(a);
}
#pragma peephole reset
#endif

/* menuSubOpenYesNo - 0x8001E074 | size: 0x110 */
#if 0
asm void fn_8001E074(void) {
#include "src/game/gs_pcbox_fn_8001E074.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s8 menuSubOpenYesNo(u8 menuType, s16 x, s16 y, s32 initialValue) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuSetPosition(s32, s16, s16);
    extern void windowCheckCursor(s32, s32);
    extern u32 windowGetValue(s32);
    extern void menuCloseCustom(s32, s32, s32);
    s32 value = initialValue;
    s16 activeWindowId;
    s16 windowId;

    if (value != 0) value = 1;
    switch (menuType) {
    case 0:
        windowId = 0x11;
        break;
    case 1:
        windowId = 0x12;
        break;
    case 0x7f:
    default:
        windowId = 0x44;
        break;
    }
    activeWindowId = windowId;
    menuOpenCustom((s32)activeWindowId, windowGetActiveID(), &value, 0, 0, 0);
    if (x >= 0 && y >= 0) menuSetPosition((s32)activeWindowId, x, y);
    windowCheckCursor((s32)activeWindowId, 1);
    windowId = (s8)windowGetValue((s32)activeWindowId);
    menuCloseCustom((s32)activeWindowId, 0, 1);
    return (s8)windowId;
}
#pragma pop
#endif

/* fn_8001E184 - 0x8001E184 | size: 0x7c */
#if 0
asm void fn_8001E184(void) {
#include "src/game/gs_pcbox_fn_8001E184.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_8001E184(void) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void windowCheckCursor();
    extern u32 windowGetValue();
    extern void menuCloseCustom();
    u32 sp8;
    s8 r31;
    sp8 = 0;
    menuOpenCustom(0x12, windowGetActiveID(), &sp8, 0, 0, 0);
    windowCheckCursor(0x12, 0x1);
    r31 = (s8)windowGetValue(0x12);
    menuCloseCustom(0x12, 0x0, 0x1);
    return r31;
}
#pragma pop
#endif

/* menuSubCloseNumberInput - 0x8001E200 | size: 0x24 */
#if 0
asm void menuSubCloseNumberInput(void) {
#include "src/game/gs_pcbox_fn_8001E200.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void menuSubCloseNumberInput(void) {
    extern void menuClose();
    menuClose(0x2);
}
#pragma pop
#endif

/* fn_8001E224 - 0x8001E224 | size: 0xe0 */
#if 0
asm void fn_8001E224(void) {
#include "src/game/gs_pcbox_fn_8001E224.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E224(void* a, u32* b, u8 c, void* d, void* e, u8 f) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void menuSetPosition();
    extern void windowCheckCursor();
    extern void windowGetValue();
    extern u8* windowSearchID();
    extern void menuClose();
    void* r4;
    u8* r3;
    s32 r31;
    u8 c_val;
    r31 = 0;
    r4 = windowGetActiveID();
    c_val = c;
    menuOpenCustom(0x2, r4, 0, 0, 0, 0x3, a, c_val, 0);
    menuSetPosition(0x2, d, e);
    windowCheckCursor(0x2, 0x1);
    windowGetValue(0x2);
    r3 = windowSearchID(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        if (f != 0) menuClose(0x2);
    }
    return r31;
}
#pragma peephole reset
#endif

/* menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs - 0x8001E304 | size: 0xdc */
#if 0
asm void menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(void) {
#include "src/game/gs_pcbox_menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(void* a, u32* b, void* c) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void menuSetPosition();
    extern void windowCheckCursor();
    extern void windowGetValue();
    extern u8* windowSearchID();
    extern void menuClose();
    void* r4_tmp;
    u8* r3;
    s32 r31;
    r31 = 0;
    r4_tmp = windowGetActiveID();
    menuOpenCustom(0x2, r4_tmp, 0, 0, 0, 0x3, a, 0x1, c);
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
