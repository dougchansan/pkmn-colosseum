/**
 * @file menuSub.c
 * @brief menuSub TU: 0x8001D718 - 0x8001EF78 (fn_8001D718 .. fn_8001EC08).
 *
 * The retail translation unit, identified by its .sdata2 literal pool
 * 0x8047B7C8-0x8047B808 (0.0f, both int->float doubles, 1.0f, 255.0f, 30.0,
 * 2*pi, 1/32, 15.0). No function outside this range reads it, and the
 * neighbouring pools repeat 0.0f and the conversion double, so the TU
 * boundaries sit at 0x8001D718 (after menuPokemon's fn_8001D624) and
 * 0x8001EF78 (the GStitle TU). It also owns fn_8001E644's colour initializer
 * (.rodata 0x80266C20-0x80266C30) and fn_8001EC08's vertex work area
 * (.bss 0x803A1D60-0x803A1F88). The function names that are known come
 * from the Colosseum map (menuSub.o) and XD's menuSub.cpp.
 *
 * Built with GC/1.3.2, -O4,p and -opt nopeephole. GC/1.3.2 is what keeps
 * fn_8001EC08's pooled .bss offsets out of its store displacements; the rest
 * of the TU is the same under GC/1.3. The GX wrappers are declared once for
 * the whole file. fn_800D61E4 takes s16 coordinates, so fn_8001E58C's int
 * corners are narrowed at the call.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

extern void _threadSwitch(void);
extern void gfxSetFlags(u32 mode);
extern void gfxClearFlags(u32 mode);
extern void gfxSetPrimitive(s32 primitive);
extern void gfxSetVtxDesc(void* desc);
extern void gfxBegin(s32 count);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5CB8(s32 index, u8 r, u8 g, u8 b, u8 a);
extern void gfxVtxTexCoord(s32 index, f32 s, f32 t);
extern void gfxEnd(void);
extern void fn_800D5BA0(s32 index, u32 color);
extern void fn_800D5648(f32 width);
extern void gfxLoadTexture(s32 map, void* texture);
extern void fn_800D848C(s32 id, s32 type, s32 src, void* mtx);
extern void fn_800DC1D4(s32 count);
extern void gfxSetTevStage(s32 stage, s32 coord, s32 map, s32 color, s32 a);
extern void fn_800DC14C(s32 stage, s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800DC0D4(s32 stage, s32 a, s32 b, s32 c, s32 d);
extern void fn_800DC04C(s32 stage, s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800DBFD4(s32 stage, s32 a, s32 b, s32 c, s32 d);
extern u8 lbl_80314E08[];

/* 0x8001D718 | 0xCC */
void fn_8001D718(f32 target) {
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    f32 progress = 0.0f;

    while (progress < target) {
        _threadSwitch();
        progress += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}

/* 0x8001D7E4 | 0x50 */
extern u32 fn_800F7AF0(s32);
extern u32 fn_800F7BC4(s32);
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

typedef struct MenuSubColorContext {
    u8 pad_00[0x88];
    GXColor color;
} MenuSubColorContext;

typedef struct MenuSubColorItem {
    u8 pad_00[0x64];
    GXColor color;
} MenuSubColorItem;

/* menuSubCalcColor - 0x8001D834 | size: 0xB4 */
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

/* 0x8001D8E8 | 0xAC */
u32 menuSubOpenSelect(u8 selection, u32 text, u32 value, s32 x, s32 y, u32 flags) {
    extern u32 windowGetActiveID(void);
    extern void menuOpenCustom(u32, u32, void*, u32, u32, u32, u32, u32, ...);
    extern void menuSetPosition(u32, s16, s16);
    extern void windowCheckCursor(u32, u32);
    extern u32 windowGetValue(u32);
    extern void menuCloseCustom(u32, u32, u32);
    u32 result;

    menuOpenCustom(0xE7, windowGetActiveID(), &flags, 0, 0, 3,
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
    u8 valid;

    valid = pokemonCheckValid(pokemon);
    if (valid == 0) {
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

/* menuSubGetPokemonSexForDisp - 0x8001DA60 | size: 0x6c */
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

/* 0x8001DACC | 0x4DC */
typedef struct MenuSubSprite {
    u8 pad_00[0x54];
    s16 width;
    s16 height;
    u32 textureId;
    s16 cropX;
    s16 cropY;
    s16 cropWidth;
    s16 cropHeight;
} MenuSubSprite;

/* Sprite draw callback: draws the sprite's crop of its texture blended
 * with a second texture (0x00EF1200), the second set of coordinates
 * offset by `shift`. */
s32 fn_8001DACC(void* context, MenuSubSprite* sprite, f32 shift) {
    extern void* lbl_8047AD00;
    extern void* fn_800F92D4(u32 id);
    extern u16 GStextureGetXsize(void* texture);
    extern u16 GStextureGetYsize(void* texture);
    extern void GStextureSetWrap(void* texture, s32 s, s32 t);
    f32 mtx[3][4];
    void* texture;
    void* pattern;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;

    texture = fn_800F92D4(sprite->textureId);
    pattern = fn_800F92D4(0xEF1200);
    if (texture != NULL && pattern != NULL) {
        u0 = (f32)sprite->cropX / (f32)GStextureGetXsize(texture);
        u1 = (f32)(sprite->cropX + sprite->cropWidth) / (f32)GStextureGetXsize(texture);
        v0 = (f32)sprite->cropY / (f32)GStextureGetYsize(texture);
        v1 = (f32)(sprite->cropY + sprite->cropHeight) / (f32)GStextureGetYsize(texture);

        gfxSetFlags(0x80000003);
        gfxClearFlags(4);
        GStextureSetWrap(pattern, 1, 1);
        fn_800D848C(0, 0, 4, mtx);
        fn_800D848C(1, 0, 5, mtx);
        fn_800D848C(2, 0, 6, mtx);
        fn_800DC1D4(3);

        gfxLoadTexture(0, texture);
        gfxSetTevStage(0, 0, 0, 0, 0);
        fn_800DC14C(0, 0, 0, 0, 0, 1);
        fn_800DC0D4(0, 0xF, 8, 0xA, 0xF);
        fn_800DC04C(0, 0, 0, 0, 0, 1);
        fn_800DBFD4(0, 7, 4, 5, 7);

        gfxLoadTexture(1, pattern);
        gfxSetTevStage(1, 0, 1, 1, 0);
        fn_800DC14C(1, 1, 0, 0, 1, 0);
        fn_800DC0D4(1, 0xF, 8, 0xD, 2);
        fn_800DC04C(1, 0, 0, 0, 1, 0);
        fn_800DBFD4(1, 7, 7, 7, 1);

        gfxLoadTexture(2, pattern);
        gfxSetTevStage(2, 0, 2, 2, 0);
        fn_800DC14C(2, 1, 0, 0, 1, 0);
        fn_800DC0D4(2, 0xF, 8, 0xD, 2);
        fn_800DC04C(2, 0, 0, 0, 1, 0);
        fn_800DBFD4(2, 7, 7, 7, 1);

        gfxSetVtxDesc(lbl_8047AD00);
        gfxSetPrimitive(7);
        gfxBegin(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0x40, 0x40, 0xFF);
        gfxVtxTexCoord(0, u0, v0);
        gfxVtxTexCoord(1, shift + u0, shift + v0);
        gfxVtxTexCoord(2, (1.0f - shift) + u0, shift + v0);
        fn_800D61E4(sprite->width, sprite->height);
        fn_800D5CB8(0, 0xFF, 0x40, 0x40, 0xFF);
        gfxVtxTexCoord(0, u1, v1);
        gfxVtxTexCoord(1, shift + u1, shift + v1);
        gfxVtxTexCoord(2, (1.0f - shift) + u1, shift + v1);
        gfxEnd();
        fn_800DC1D4(1);
        gfxClearFlags(0x80000000);
    }
    return 0;
}

/* 0x64 | fn_8001DFA8 | generic_call_check_store */
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

s32 std158_menuOpenCustom43(u32 sp8) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuCloseCustom(s32, s32, s32);
    s32 r31;
    r31 = menuOpenCustom(0x43, windowGetActiveID(), &sp8, 0, 1, 0);
    menuCloseCustom(0x43, 0, 1);
    return r31;
}

s8 menuSubOpenYesNo(u8 menuType, s16 x, s16 y, s32 initialValue) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuSetPosition(s32, s16, s16);
    extern void windowCheckCursor(s32, s32);
    extern u32 windowGetValue(s32);
    extern void menuCloseCustom(s32, s32, s32);
    s8 result;
    s16 activeWindowId;
    s16 windowId;

    if (initialValue != 0) initialValue = 1;
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
    menuOpenCustom((s32)activeWindowId, windowGetActiveID(), &initialValue, 0, 0, 0);
    if (x >= 0 && y >= 0) menuSetPosition((s32)activeWindowId, x, y);
    windowCheckCursor((s32)activeWindowId, 1);
    result = (s8)windowGetValue((s32)activeWindowId);
    menuCloseCustom((s32)activeWindowId, 0, 1);
    return result;
}

s32 std011_dialogDisplayYesNoMenu(void) {
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

void menuSubCloseNumberInput(void) {
    extern void menuClose();
    menuClose(0x2);
}

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

/* 0x8001E3E0 | 0xD4 */
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

/* 0x8001E4B4 | 0xD8 */
void fn_8001E4B4(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f) {
    gfxSetFlags(0x1);
    gfxClearFlags(0x6);
    gfxSetPrimitive(0x3);
    gfxSetVtxDesc(lbl_80314E08);
    gfxBegin(0x3);
    fn_800D61E4((s16)a, (s16)b);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)c, (s16)d);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    fn_800D61E4((s16)e, (s16)f);
    fn_800D5CB8(0x0, 0xff, 0xff, 0xff, 0xff);
    gfxEnd();
}

/* 0x8001E58C | 0xB8 */
void fn_8001E58C(s32 x, s32 y, s32 width, s32 height, u8* color) {
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetPrimitive(7);
    gfxSetVtxDesc(lbl_80314E08);
    width += x;
    height += y;
    gfxBegin(2);
    fn_800D61E4(x, y);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    fn_800D61E4(width, height);
    fn_800D5CB8(0, color[0], color[1], color[2], color[3]);
    gfxEnd();
}

/* 0x8001E644 | 0x454: frame with a translucent box, corner sprites and
 * stretched edge sprites. */
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

void fn_8001E644(s32 x, s32 y, s32 width, s32 height, u8 alpha) {
    extern u8 lbl_80266C20[];
        extern GsPcboxSpriteBios* menuSpriteBiosGetPtr(s32);
    extern GsPcboxItemBios* menuItemBiosGetPtr(s16);
    extern void windowDrawSprite(s16, s16, void*, u16, u32);
    extern void windowDrawSprite2(s16, s16, s16, s16, u32, s32, s32, s32);
    u32 colors[3] = {0x213A44F2, 0x11272BF2, 0xFFFFFF38};
    f32 scale;
    GsPcboxSpriteBios* sprite;
    GsPcboxItemBios* item0;
    GsPcboxItemBios* item1;
    s32 right;
    s16 left;
    s32 i;
    s16 top;
    s16 boxHeight;
    s32 bottom;
    s32 halfW;
    s32 halfH;
    s16 cornerL;
    s16 innerL;
    s16 cornerR;
    s16 cornerT;
    s16 innerT;
    s16 cornerB;
    s32 span;
    s32 span2;

    scale = (f32)alpha / 255.0f;
    ((u8*)colors)[3] = ((u8*)colors)[3] * scale;
    ((u8*)colors)[7] = ((u8*)colors)[7] * scale;
    ((u8*)colors)[11] = ((u8*)colors)[11] * scale;

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetPrimitive(6);
    gfxSetVtxDesc(lbl_80314E08);
    left = x - 10;
    top = y - 10;
    gfxBegin(4);
    fn_800D61E4(left, top);
    fn_800D5BA0(0, colors[0]);
    right = left + (s16)(width + 20);
    fn_800D61E4(right, top);
    fn_800D5BA0(0, colors[0]);
    boxHeight = height + 20;
    bottom = top + boxHeight;
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, colors[1]);
    fn_800D61E4(left, bottom);
    fn_800D5BA0(0, colors[1]);
    gfxEnd();

    gfxSetPrimitive(1);
    gfxSetVtxDesc(lbl_80314E08);
    fn_800D5648(1.0f);
    for (i = 0; i < boxHeight; i += 4) {
        gfxBegin(2);
        fn_800D61E4(left, top + i);
        fn_800D5BA0(0, colors[2]);
        fn_800D61E4(right, top + i);
        fn_800D5BA0(0, colors[2]);
        gfxEnd();
    }

    sprite = menuSpriteBiosGetPtr(0xBB);
    halfW = sprite->width / 2;
    halfH = sprite->height / 2;
    cornerL = x - halfW;
    innerL = x + halfW;
    cornerR = x + width - halfW;
    cornerT = y - halfH - 10;
    innerT = y + halfH - 10;
    sprite = menuSpriteBiosGetPtr(0xB8);
    cornerB = y + height - sprite->height / 2 + 10;

    windowDrawSprite(cornerL, cornerT, 0, 0xBB, 0);
    windowDrawSprite(cornerR, cornerT, 0, 0xBB, 1);
    windowDrawSprite(cornerL, cornerB, 0, 0xB8, 0);
    windowDrawSprite(cornerR, cornerB, 0, 0xB8, 1);

    sprite = menuSpriteBiosGetPtr(0xBA);
    item0 = menuItemBiosGetPtr(0x84);
    item1 = menuItemBiosGetPtr(0x87);
    span = cornerB - innerT;
    windowDrawSprite2(cornerL + (s16)(item1->x - item0->x), innerT, sprite->width, span, -1, 0, 0xBA, 0);
    item0 = menuItemBiosGetPtr(0x85);
    item1 = menuItemBiosGetPtr(0x86);
    windowDrawSprite2(cornerR + (s16)(item1->x - item0->x), innerT, sprite->width, span, -1, 0, 0xBA, 0);

    sprite = menuSpriteBiosGetPtr(0xB7);
    item0 = menuItemBiosGetPtr(0x84);
    item1 = menuItemBiosGetPtr(0x8B);
    span2 = cornerR - innerL;
    windowDrawSprite2(innerL, cornerT + (s16)(item1->y - item0->y), span2, sprite->height, -1, 0, 0xB7, 0);
    item0 = menuItemBiosGetPtr(0x88);
    item1 = menuItemBiosGetPtr(0x8A);
    windowDrawSprite2(innerL, cornerB + (s16)(item1->y - item0->y), span2, sprite->height, -1, 0, 0xB7, 0);
}

/* 0x8001EA98 | 0x170 */
void fn_8001EA98(s32 x, s32 y, s32 width, s32 height) {
    s32 right;
    s32 bottom;

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetPrimitive(7);
    gfxSetVtxDesc(lbl_80314E08);
    gfxBegin(2);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, 0xC0);
    right = x + width + 10;
    bottom = y + height + 10;
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, 0xC0);
    gfxEnd();

    fn_800D5648(1.0f);
    gfxSetPrimitive(2);
    gfxBegin(5);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, -1);
    fn_800D61E4(right, y - 10);
    fn_800D5BA0(0, -1);
    fn_800D61E4(right, bottom);
    fn_800D5BA0(0, -1);
    fn_800D61E4(x - 10, bottom);
    fn_800D5BA0(0, -1);
    fn_800D61E4(x - 10, y - 10);
    fn_800D5BA0(0, -1);
    gfxEnd();
}

typedef struct PCBoxVertex {
    s16 x;
    s16 y;
    u32 color;
} PCBoxVertex;

extern f64 sin(f64);
extern f64 cos(f64);

static PCBoxVertex sInner[32];
static PCBoxVertex sOuter[32];
static PCBoxVertex sCorners[5];


/* RULE-EXCEPTION(title-path): unreferenced helper that only fixes the pooled
 * .bss order -- see docs/RULE_EXCEPTIONS.md. MWCC lays pooled statics out in
 * first-reference order; retail puts the 15.0 ring (sInner) before the 30.0
 * ring (sOuter) although fn_8001EC08 writes sOuter first, so something in the
 * TU referenced sInner earlier. This static function is never called and is
 * dead-stripped at link time. */
static void menuSubRingClear(void) {
    sCorners[0].color = 0;
    sInner[0].color = 0;
    sOuter[0].color = 0;
}

void msgWinDrawBackground(s32 x, s32 y, s32 width, s32 height, u8 color, u8 dimColor) {
    s32 i;

    if (dimColor != 0) {
        color /= 2;
    }

    sCorners[0].x = width + x;
    sCorners[0].y = y + (height + 2);
    sCorners[1].x = width + x;
    sCorners[1].y = y;
    sCorners[2].x = x;
    sCorners[2].y = y;
    sCorners[3].x = x;
    sCorners[3].y = y + (height + 2);
    sCorners[4].x = width / 2 + x;
    sCorners[4].y = height / 2 + y;
    sCorners[4].color = color;

    for (i = 0; i < 32; i++) {

        sOuter[i].x = 30.0 * sin(6.283185307179586 * i / 32.0) + x;
        sOuter[i].y = 30.0 * cos(6.283185307179586 * i / 32.0) + y;
        sOuter[i].color = 0;
        sInner[i].x = 15.0 * sin(6.283185307179586 * i / 32.0) + x;
        sInner[i].y = 15.0 * cos(6.283185307179586 * i / 32.0) + y;
        sInner[i].color = color;
        sOuter[i].x += sCorners[i / 8].x;
        sOuter[i].y += sCorners[i / 8].y;
        sInner[i].x += sCorners[i / 8].x;
        sInner[i].y += sCorners[i / 8].y;
    }

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(lbl_80314E08);
    gfxBegin(0x42);
    for (i = 0; i < 32; i++) {
        fn_800D61E4(sOuter[i].x, sOuter[i].y);
        fn_800D5BA0(0, sOuter[i].color);
        fn_800D61E4(sInner[i].x, sInner[i].y);
        fn_800D5BA0(0, sInner[i].color);
    }
    fn_800D61E4(sOuter[0].x, sOuter[0].y);
    fn_800D5BA0(0, sOuter[0].color);
    fn_800D61E4(sInner[0].x, sInner[0].y);
    fn_800D5BA0(0, sInner[0].color);
    gfxEnd();

    gfxSetPrimitive(5);
    gfxBegin(0x22);
    fn_800D61E4(sCorners[4].x, sCorners[4].y);
    fn_800D5BA0(0, sCorners[4].color);
    for (i = 0; i < 32; i++) {
        fn_800D61E4(sInner[i].x, sInner[i].y);
        fn_800D5BA0(0, sInner[i].color);
    }
    fn_800D61E4(sInner[0].x, sInner[0].y);
    fn_800D5BA0(0, sInner[0].color);
    gfxEnd();
}
