/**
 * @file pda_range_80037158.c
 * @brief PDA subsystem body, 0x80037158 - 0x8004B7EC.
 *
 * Colosseum's PDA: the People/party-select 3D screen (5 private
 * widget tables), a shared PDA scene/camera block, and the mail-fetch
 * helpers. A reduced/reordered subset of XD's PDA (ReliveHall and
 * PdaSearcher clusters were cut). Internal boundaries are fuzzy --
 * split further only with evidence. All functions asm-only until
 * matched.
 */
#include "dolphin/types.h"

extern s8 lbl_8047A470;
extern void* lbl_8047A480;
extern s8 lbl_8047A490;
extern f32 lbl_8047A478;
extern f32 lbl_8047A484;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern f32 lbl_8047A494;
extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4B8;
extern s32 lbl_8047A4B4;
extern s32 lbl_8047A4B0;
extern u32 lbl_8047A498;
extern u32 lbl_8047A49C;
extern u32 lbl_8047A4A0;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern u8 lbl_803A6498[];
extern u8 lbl_80314F98[];
extern void* pcboxGetItem(void*, s16);

extern f32 lbl_8047BA58;
extern f32 lbl_8047BA5C;
extern f32 lbl_8047BAB0;
extern f32 lbl_8047BABC;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA64;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA70;
extern f32 lbl_8047BA74;
extern f32 lbl_8047BA78;
extern f32 lbl_8047BAC0;

typedef struct PdaModelWindow {
    u8 pad00[0x8];
    s32 field_08;
    s32 field_0C;
    s32 field_10;
    u8 pad14[0x4];
    f32 field_18;
    u8 pad1C[0xC];
    f32 field_28;
    u8 pad2C[0x18];
    f32 alphaScale;
    u8 pad48[0x4C];
    s8 variant;
    u8 pad95[0xB];
} PdaModelWindow; /* lbl_803A6748, size 0xA0 */

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

typedef struct PdaEvent {
    u8 pad00[0x6];
    s16 messageId;
} PdaEvent;

typedef struct PdaMenuState {
    u8 pad00;
    s8 mode;
    s8 menuSet;
} PdaMenuState;

typedef struct PdaSelectionWork {
    u8 pad00[4];
    void* menu;
    u8 pad08[0x8d];
    s8 selectedIndex;
} PdaSelectionWork;

typedef struct PdaKeyInfo {
    u8 pad00[4];
    u16 trigger;
    u16 buttons;
} PdaKeyInfo;

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

typedef struct PdaOrbitPoint {
    f32 angle;
    f32 pad04[6];
    f32 alpha;
    f32 pad20;
} PdaOrbitPoint;

typedef struct PdaDrawWork {
    u8 pad00[0x88];
    void* drawData;
    u8 pad8C[9];
    s8 selectedPage;
} PdaDrawWork;

extern PdaModelWindow lbl_803A6748;
extern u8 lbl_803A67FC[];
extern PdaSceneWork lbl_803A6818;

extern s32 pdaSnagListDetailKey(void* window, s32 enabled);
extern void pdaSnagListDrawDetail(PdaSprite* sprite, PdaEvent* event);
extern s32 pdaMemoUpdateScroll(void* work, s32 mode);
extern s32 pdaModelViewerControl(void* work, s32 mode);
extern void pdaReadPadInput(void* scene);
extern void GSscene_SetMode(s32 mode);
extern void menuButtonNormal(void* button);
extern void winSpriteSetDisp(void* sprite, s32 disp);
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);

#if !defined(PDA_RANGE_EXACT_80037174_ONLY) && !defined(PDA_SAVE_80038A0C_ONLY) && !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
void pdaNop_80037158(void)
{
}

void pdaTryLock(void)
{
    u8 flag;

    flag = lbl_8047A470;
    lbl_8047A470 = 1;
    /* RULE-EXCEPTION(user-approved): preserve retail's dead computation. See docs/RULE_EXCEPTIONS.md. */
    flag = 1 - flag;
}
#endif /* !PDA_80039128_ONLY */
#endif

#if !defined(PDA_RANGE_80037158_ONLY)
#if !defined(PDA_SAVE_80038A0C_ONLY) && !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
void pdaUnlock(void)
{
    lbl_8047A470 = 0;
}

/* Retail places this callback immediately after pdaUnlock. */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void pcMenuDrawPurifiedCount(u32 unused, u8* p)
{
    extern u32 gfxVtxTexCoord();
    extern u32 gfxVtxColor4u8();
    extern u32 gfxVtxPos2s16();
    extern u32 gfxEnd();
    extern u32 gfxBegin();
    extern u32 gfxSetPrimitive();
    extern u32 gfxSetVtxDesc();
    extern u32 gfxLoadTexture();
    extern u32 gfxClearFlags();
    extern u32 gfxSetFlags();
    extern u32 menuModelRender();
    extern u32 fn_80132A38();
    extern u32 gamedataGetPlaytime(void);
    extern u32 darkPokemonCountFlag14(void);
    extern u32 darkPokemonCountFlag12(void);
    u32 result;

    switch (*(s16*)(p + 6)) {
    case 0x36c:
        result = gamedataGetPlaytime();
        if (lbl_8047A498 != 0) {
            result = lbl_8047A498;
        }
        lbl_8047A49C = darkPokemonCountFlag14();
        lbl_8047A4A0 = darkPokemonCountFlag12();
        fn_80132A38(0x4c, result);
        fn_80132A38(0x2f, darkPokemonCountFlag12());
        fn_80132A38(0x30, darkPokemonCountFlag14());
        if ((result = menuModelRender(lbl_803A6498)) != 0) {
            gfxSetFlags(3);
            gfxClearFlags(4);
            gfxSetPrimitive(7);
            gfxSetVtxDesc(lbl_80314F98);
            gfxLoadTexture(0, result);
            gfxBegin(2);
            gfxVtxPos2s16(0, 0);
            gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0xff);
            gfxVtxTexCoord(0, lbl_8047BA58, lbl_8047BA58);
            gfxVtxPos2s16(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
            gfxVtxColor4u8(0, 0xff, 0xff, 0xff, 0xff);
            gfxVtxTexCoord(0, lbl_8047BA5C, lbl_8047BA5C);
            gfxEnd();
        }
        break;
    default:
        if ((s32)lbl_8047A49C == 0) {
            *(s8*)(p + 4) = (s8)(p[4] & ~2);
        }
        break;
    }
}
#pragma peephole reset

void pcMenuListDrawItem(void)
{
}

void pcMenuListDrawItem2(void)
{
}

void pcMenuListDrawItem3(void)
{
}

void pcMenuListDrawItem4(void)
{
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void pcMenuTopDrawItem(void* window, PdaSprite* sprite)
{
    extern u8 lbl_80314E08[];
    extern void gfxVtxColor1u32(s32 index, u32 color);
    extern void gfxVtxPos2s16(s32 x, s32 y);
    extern void gfxEnd(void);
    extern void gfxBegin(s32 count);
    extern void gfxSetPrimitive(s32 primitive);
    extern void gfxSetVtxDesc(void* data);
    extern void gfxClearFlags(s32 mode);
    extern void gfxSetFlags(s32 mode);

    (void)window;
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetVtxDesc(lbl_80314E08);
    gfxSetPrimitive(6);
    gfxBegin(4);
    gfxVtxPos2s16(0, 0);
    gfxVtxColor1u32(0, 0x003B6DFF);
    gfxVtxPos2s16(sprite->x, 0);
    gfxVtxColor1u32(0, 0x489DECFF);
    gfxVtxPos2s16(sprite->x, sprite->y);
    gfxVtxColor1u32(0, 0x489DECFF);
    gfxVtxPos2s16(0, sprite->y);
    gfxVtxColor1u32(0, 0x003B6DFF);
    gfxEnd();
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
void pcMenuDrawExit(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A64EC[];
    extern u8 lbl_803A654C[];
    s32 value;

    value = (s32)*(f32*)(lbl_803A654C + 0x58);
    if ((s16)value < -0xff) {
        value = -0xff;
    }
    sprite->alpha = (u8)(value + 0xff);
    sprite->field_50 = (s16)(*(f32*)(lbl_803A64EC + 0x54) + *(f32*)(lbl_803A654C + 0x58));
    if (context->selectedIndex == 3) {
        sprite->colorR = 0xff;
        sprite->colorG = 0xff;
        sprite->colorB = 0xff;
    } else {
        sprite->colorR = 0x46;
        sprite->colorG = 0x8f;
        sprite->colorB = 0xb4;
    }
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
void pcMenuDrawSave(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A64EC[];
    extern u8 lbl_803A654C[];
    s32 value;

    value = (s32)*(f32*)(lbl_803A654C + 0x40);
    if ((s16)value < -0xff) {
        value = -0xff;
    }
    sprite->alpha = (u8)(value + 0xff);
    sprite->field_50 = (s16)(*(f32*)(lbl_803A64EC + 0x3c) + *(f32*)(lbl_803A654C + 0x40));
    if (context->selectedIndex == 2) {
        sprite->colorR = 0xff;
        sprite->colorG = 0xff;
        sprite->colorB = 0xff;
    } else {
        sprite->colorR = 0x46;
        sprite->colorG = 0x8f;
        sprite->colorB = 0xb4;
    }
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
void pcMenuDrawItemStorage(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A64EC[];
    extern u8 lbl_803A654C[];
    s32 value;

    value = (s32)*(f32*)(lbl_803A654C + 0x28);
    if ((s16)value < -0xff) {
        value = -0xff;
    }
    sprite->alpha = (u8)(value + 0xff);
    sprite->field_50 = (s16)(*(f32*)(lbl_803A64EC + 0x24) + *(f32*)(lbl_803A654C + 0x28));
    if (context->selectedIndex == 1) {
        sprite->colorR = 0xff;
        sprite->colorG = 0xff;
        sprite->colorB = 0xff;
    } else {
        sprite->colorR = 0x46;
        sprite->colorG = 0x8f;
        sprite->colorB = 0xb4;
    }
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
void pcMenuDrawPokemonStorage(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A64EC[];
    extern u8 lbl_803A654C[];
    s32 value;

    value = (s32)*(f32*)(lbl_803A654C + 0x10);
    if ((s16)value < -0xff) {
        value = -0xff;
    }
    sprite->alpha = (u8)(value + 0xff);
    sprite->field_50 = (s16)(*(f32*)(lbl_803A64EC + 0xc) + *(f32*)(lbl_803A654C + 0x10));
    if (context->selectedIndex == 0) {
        sprite->colorR = 0xff;
        sprite->colorG = 0xff;
        sprite->colorB = 0xff;
    } else {
        sprite->colorR = 0x46;
        sprite->colorG = 0x8f;
        sprite->colorB = 0xb4;
    }
}
#pragma peephole reset

void pcMenuListDrawText(void)
{
    s32 messageId;

    switch (lbl_8047A490) {
    case 0:
        messageId = 0x1b5b;
        break;
    case 1:
        messageId = 0x1b5c;
        break;
    case 2:
        messageId = 0x1b5d;
        break;
    case 3:
        messageId = 0x1b5e;
        break;
    default:
        messageId = 0x1b59;
        break;
    }
    GSmsgPrintColor(-4, 0, -1, (void*)messageId);
}

#if !defined(PDA_RANGE_EXACT_80037174_ONLY)
void menu1FDrawItem(void* window, PdaSprite* sprite)
{
    sprite->value = lbl_8047BA74 - lbl_8047A478;
}
#endif

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling off; opt_propagation off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling off
#pragma opt_propagation off
void pcMenuListDrawText2(void)
{
    void* data = lbl_8047A480;

    GSmsgPrintColor(0, 0, -1, data);
}
#pragma opt_propagation reset
#pragma scheduling reset
#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_SAVE_80038A0C_ONLY && !PDA_DRIVER_8003842C_ONLY */

#if !defined(PDA_RANGE_EXACT_80037174_ONLY)
#if !defined(PDA_SAVE_80038A0C_ONLY) && !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
/* Four copies of the same fade-in step, one per 0x18-byte record in
   lbl_803A654C. Two shapes here are load-bearing: 0.0f as a literal, because
   retail reloads the constant instead of reusing the compare's copy, and the
   embedded `velocity =`, which orders the record load ahead of the velocity
   load. */
#pragma peephole off
void pcMenuDrawItem(void* window, volatile PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x58);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x58) +
               (velocity = *(f32*)(lbl_803A654C + 0x50));
        *(f32*)(lbl_803A654C + 0x58) = next;
        *(f32*)(lbl_803A654C + 0x50) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x4c) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x58) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x54);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x54) +
                                  *(f32*)(lbl_803A654C + 0x58));
    }
}
#pragma peephole reset

#pragma peephole off
void pcMenuDrawItem2(void* window, volatile PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x40);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x40) +
               (velocity = *(f32*)(lbl_803A654C + 0x38));
        *(f32*)(lbl_803A654C + 0x40) = next;
        *(f32*)(lbl_803A654C + 0x38) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x34) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x40) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x3c);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x3c) +
                                  *(f32*)(lbl_803A654C + 0x40));
    }
}
#pragma peephole reset

#pragma peephole off
void pcMenuDrawItem3(void* window, volatile PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x28);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x28) +
               (velocity = *(f32*)(lbl_803A654C + 0x20));
        *(f32*)(lbl_803A654C + 0x28) = next;
        *(f32*)(lbl_803A654C + 0x20) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x1c) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x28) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x24);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x24) +
                                  *(f32*)(lbl_803A654C + 0x28));
    }
}
#pragma peephole reset

#pragma peephole off
void pcMenuDrawItem4(void* window, volatile PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x10);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x10) +
               (velocity = *(f32*)(lbl_803A654C + 0x8));
        *(f32*)(lbl_803A654C + 0x10) = next;
        *(f32*)(lbl_803A654C + 0x8) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x4) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x10) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0xc);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0xc) +
                                  *(f32*)(lbl_803A654C + 0x10));
    }
}
#pragma peephole reset

/* |v|. Retail's shape -- `> 0` first -- is what emits the two-way
   branch rather than an inverted single one. */
static inline f32 pdaOrbitAbs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

static inline void pdaUpdateOrbitSprite(PdaSprite* sprite, f32 baseAngle,
                                        s32 updateTarget)
{
    extern f32 lbl_802E52A8[4];
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern f64 cos(f64 angle);
    extern f64 sin(f64 angle);
    f32 angle;
    f32 step;
    f32 distance;
    f32 magnitude;
    s8 direction;

    if (updateTarget) {
        lbl_8047A488 = lbl_802E52A8[lbl_8047A47C];
    }
    angle = lbl_8047A484 - baseAngle;
    if (angle < 0.0f) {
        angle += 6.28318548f;
    }
    if (angle >= 6.28318548f) {
        angle -= 6.28318548f;
    }

    windowGetKeyInfo();
    step = lbl_8047BA64 * lbl_8047A494;
    if (lbl_8047A484 != lbl_8047A488) {
        if (lbl_8047A488 - lbl_8047A484 < 0.0f) {
            step = -step;
        }
        lbl_8047A48C = step;
        lbl_8047A484 += step;
        if (lbl_8047A484 >= 6.28318548f) {
            lbl_8047A484 -= 6.28318548f;
        }
        if (lbl_8047A484 < 0.0f) {
            lbl_8047A484 += 6.28318548f;
        }

        distance = pdaOrbitAbs(lbl_8047A484 - lbl_8047A488);
        magnitude = pdaOrbitAbs(step);
        if (distance < magnitude) {
            lbl_8047A484 = lbl_8047A488;
            lbl_8047A48C = 0.0f;
        }
    } else if (lbl_8047A47D != lbl_8047A47C) {
        direction = lbl_8047A47D - lbl_8047A47C;
        if (direction > 0) {
            lbl_8047A48C = -step;
        } else if (direction < 0) {
            lbl_8047A48C = step;
        }
        lbl_8047A47D = lbl_8047A47C;
    }

    sprite->field_50 =
        (s16)(lbl_8047BA6C * (f32)sin(angle) + lbl_8047BA68);
    sprite->field_52 =
        (s16)(lbl_8047BA6C * (f32)cos(angle) + lbl_8047BA70);
}

#pragma peephole off
void menu20DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[3], 0);
}
#pragma peephole reset

#pragma peephole off
void menu22DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[2], 0);
}
#pragma peephole reset

#pragma peephole off
void menu21DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[1], 0);
}
#pragma peephole reset

#pragma peephole off
void menu23DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[0], 1);
}
#pragma peephole reset

/* The redundant expressions preserve MWCC's exact register/scheduling shape. */
void menu1EDrawItem(void* window, PdaSprite* sprite)
{
    f32 new_var;
    int new_var4;
    f32 value;
    f32 new_var2;
    float new_var3;

    new_var2 = (0, lbl_8047A494);
    new_var3 = lbl_8047BA78 * new_var2;
    if (((!lbl_8047A478) && (!lbl_8047A478)) && (!lbl_8047A478)) {
    }
    value = lbl_8047A478 + new_var3;
    lbl_8047A478 = value;
    new_var = value;
    if (new_var4 = new_var > lbl_8047BA60) {
        lbl_8047A478 = lbl_8047BA60;
        lbl_8047A478 = value - lbl_8047A478;
    }
    sprite->value = lbl_8047A478;
}

static inline u8 pdaAllStopped(void)
{
    extern u8 lbl_803A654C[];
    f32* p = (f32*)lbl_803A654C;
    s32 i;

    for (i = 0; i < 4; i++, p += 6) {
        if (lbl_8047BA58 != p[4]) {
            return 0;
        }
    }
    return 1;
}

void pcMenuDrawItem5(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    extern f32 lbl_802E5288[][2];
    extern s8 lbl_8047A47C;
    s8 index;

    if (pdaAllStopped()) {
        sprite->flags |= 2;
    } else {
        sprite->flags &= ~2;
    }
    index = context->selectedIndex;
    lbl_8047A47C = index;
    sprite->field_50 = (s16)lbl_802E5288[index][0];
    *(s16*)((u8*)sprite + 0x52) = (s16)lbl_802E5288[index][1];
}

s32 pcMenuListCtrl(PdaMenuState* state)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);

    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1b, 0xc4);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1b, 0xc8);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}

s32 pcMenuListCtrl2(PdaMenuState* state)
{

    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1a, 0xbc);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1a, 0xc0);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}

s32 pcMenuCursor(PdaSelectionWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 menuGetSelectItemNum();
    PdaKeyInfo* keyInfo;
    s32 itemCount;

    keyInfo = windowGetKeyInfo();
    if (keyInfo->buttons & 2) {
        itemCount = menuGetSelectItemNum(work->menu);
        itemCount = (s8)itemCount;
        if ((s8)++work->selectedIndex >= itemCount) {
            work->selectedIndex = itemCount - 1;
        }
    }
    if (keyInfo->buttons & 1) {
        if ((s8)--work->selectedIndex < 0) {
            work->selectedIndex = 0;
        }
    }
    return 0;
}

s32 itemStorageSelectAction(s32 value)
{
    extern u32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menuId, ...);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 wait);
    s32 parameter = value;
    s32 choices[4] = { 0, 1, 2, 3 };
    s32 choice;

    choice = menuOpenCustom(0x24, windowGetActiveID(), &parameter, 0, 1, 0);
    menuClose(0x24);
    menuCloseSync(0x24, 1);
    if (choice < 0 || choice >= 4) {
        return 4;
    }
    return choices[choice];
}

s32 itemStorageListDrawItem(void* window, PdaSprite* sprite)
{
    s32 messageId;

    (void)window;

    if (lbl_8047A4B0 == 0) {
        messageId = 0x1b6d;
    } else {
        messageId = 0x1b6e;
    }
    sprite->messageId = messageId;
    return 0;
}

s32 itemStorageListDrawSprite(void* window, void* sprite)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern void* pcboxGetItem(s32 box, s16 slot);
    extern u8 itemSlotIsValid(void* item);
    s32 count;
    s32 slot;
    s32 slotCount;
    u16 boundedSlotCount;
    s32 threshold;
    s32 display;

    (void)window;
    count = 0;
    threshold = lbl_8047A4A8 + 8;
    slotCount = pcboxGetNbItemSlot(0);
    boundedSlotCount = slotCount;
    for (slot = 0; slot < boundedSlotCount; slot++) {
        if (itemSlotIsValid(pcboxGetItem(0, slot))) {
            count++;
        }
    }
    if (threshold < count + 1) {
        display = 1;
    } else {
        display = 0;
    }
    winSpriteSetDisp(sprite, display);
    return 0;
}

void pcCloseAllMenus(void)
{
    extern void menuCloseCustom(s32 slot, s32 arg1, s32 arg2);

    menuCloseCustom(0x19, 0, 1);
    menuCloseCustom(0x1a, 0, 1);
    menuCloseCustom(0x1b, 0, 1);
    menuCloseCustom(0x18, 0, 1);
    menuCloseCustom(0x1e, 0, 1);
    menuCloseCustom(0x1f, 0, 1);
    menuCloseCustom(0x20, 0, 1);
    menuCloseCustom(0x21, 0, 1);
    menuCloseCustom(0x22, 0, 1);
    menuCloseCustom(0x23, 0, 1);
    menuCloseCustom(0x1d, 0, 1);
}

void pcFrameStepTask(void)
{
    extern s32 GSgfxGetVsyncRate(void);
    extern u32 GSgetFrameStep(void);
    extern void _threadSwitch(void);

    while (1) {
        lbl_8047A494 = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
        _threadSwitch();
    }
}

void pcResetTimer8047A484(void)
{
    lbl_8047A484 = lbl_8047BA58;
}

#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_SAVE_80038A0C_ONLY && !PDA_DRIVER_8003842C_ONLY */
#if !defined(PDA_SAVE_80038A0C_ONLY)
#if !defined(PDA_80039128_ONLY)
/* Re-seed the People screen's two 4-entry widget tables and reset the
   carousel angle back to the table's rest value. */
static inline void pdaResetPeopleTables(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    s32 i;
    u8* src;
    u8* dst;

    lbl_8047A480 = (void*)0x1b5a;
    dst = base + 0xb4;
    src = tbl + 0;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = tbl + 0x60;
    dst = base + 0x54;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)(tbl + 0xe0);
    lbl_8047A484 = *(f32*)(tbl + 0xe0);
    lbl_8047A47D = 0;
}

static inline void pdaResetPeopleTablesFirst(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    s32 i;
    u8* src;

    lbl_8047A480 = (void*)0x1b5a;
    dst = base + 0xb4;
    src = tbl + 0;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = tbl + 0x60;
    dst = base + 0x54;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)(tbl + 0xe0);
    lbl_8047A484 = *(f32*)(tbl + 0xe0);
    lbl_8047A47D = 0;
}

#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_SAVE_80038A0C_ONLY */
static inline void pdaResetPeopleTablesSave(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    u8* src;
    s32 i;

    lbl_8047A480 = (void*)0x1b5a;
    src = tbl + 0;
    dst = base + 0xb4;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = tbl + 0x60;
    dst = base + 0x54;
    for (i = 0; i < 4; dst += 0x18, src += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)(tbl + 0xe0);
    lbl_8047A484 = *(f32*)(tbl + 0xe0);
    lbl_8047A47D = 0;
}

#if !defined(PDA_SAVE_80038A0C_ONLY)
#if !defined(PDA_80039128_ONLY)
/* People screen driver: run the top-level menu, dispatch the four entries,
   and tear the menu down on exit. */
#pragma peephole off
void std024_dialogDisplayPCBoxMenu(void)
{
    extern u8 lbl_802E51C8[];
    extern u8 lbl_802EF0A8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void soundDuck(s32 a, s32 b, s32 c, s32 d);
    extern void soundUnduck(s32 a, s32 b, s32 c);
    extern void floorBattleEndTerminateScripts(void);
    extern void pcBoxStorageMain(void);
    extern void itemStorageRun(void);
    extern s32 pcMenuSave(void);
    extern void menuModelInit(u8* model, s16 a, s16 b);
    extern void menuModelSetMotion(u8* model, s32 motion);
    extern void menuPokemonModelRequestById(u8* model, s32 id);
    extern void menuModelTerm(u8* model);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern void peopleInfoGetMotion(void* info, s32 kind, s32* out0, s32* out1);
    struct {
        u8* base;
        u8* tbl;
    } refs;
    s32 state;
    s32 running;
    s32 choice;
    s32 motion;
    s32 scratch;

    refs.tbl = lbl_802E51C8;
    refs.base = lbl_803A6498;
    state = 0;
    running = 1;
    soundDuck(0x55, 0x1f4, 1, 1);
    do {
        switch (state) {
        case 0:
            pdaResetPeopleTablesFirst(refs.base, refs.tbl);
            menuOpen(0x18, 0);
            menuOpen(0x1e, 0);
            menuOpen(0x1f, 0);
            menuOpen(0x1a, 0);
            menuOpen(0x1b, 0);
            menuOpen(0x19, 0);
            state = 1;
            break;
        case 1:
            lbl_8047A490 = -1;
            menuOpen(0x20, 0);
            menuOpen(0x21, 0);
            menuOpen(0x22, 0);
            menuOpen(0x23, 0);
            choice = menuOpen(0x1d, 1);
            if (choice >= 0) {
                lbl_8047A490 = (s8)choice;
                switch (choice) {
                case 0:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables(refs.base, refs.tbl);
                    state = 0xa;
                    break;
                case 1:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables(refs.base, refs.tbl);
                    state = 0x14;
                    break;
                case 2:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables(refs.base, refs.tbl);
                    state = 0x1e;
                    break;
                case 3:
                    state = 0x3e8;
                    break;
                }
            } else {
                state = 0x3e8;
            }
            break;
        case 0xa:
            pcBoxStorageMain();
            state = 1;
            break;
        case 0x14:
            lbl_8047A480 = (void*)0x1b61;
            itemStorageRun();
            lbl_8047A480 = (void*)0x1b5a;
            state = 1;
            break;
        case 0x1e:
            menuModelInit(refs.base, (s16)(*(s16*)(lbl_802EF0A8 + 0x5fd6) + 0xa),
                          *(s16*)(lbl_802EF0A8 + 0x5fd8));
            menuPokemonModelRequestById(refs.base, 0xf70400);
            peopleInfoGetMotion(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &scratch);
            menuModelSetMotion(refs.base, motion);
            choice = pcMenuSave();
            menuModelTerm(refs.base);
            state = choice;
            break;
        case 0x28:
            state = 0x3e8;
            break;
        case 0x3e8:
            menuCloseCustom(0x1d, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    soundUnduck(0x3e8, 1, 1);
    floorBattleEndTerminateScripts();
}
#pragma peephole reset

#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_SAVE_80038A0C_ONLY */
#if !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
/* Prime the save-overwrite prompt with the current play time and the two
   hero-name message slots. */
static inline void pdaFillSavePrompt(void)
{
    extern u32 gamedataGetPlaytime(void);
    extern u32 darkPokemonCountFlag14(void);
    extern u32 darkPokemonCountFlag12(void);
    extern void msgctrlSetValue(s32 id, u32 value);
    u32 playTime;

    playTime = gamedataGetPlaytime();
    if (lbl_8047A498 != 0) {
        playTime = lbl_8047A498;
    }
    lbl_8047A49C = darkPokemonCountFlag14();
    lbl_8047A4A0 = darkPokemonCountFlag12();
    msgctrlSetValue(0x4c, playTime);
    msgctrlSetValue(0x2f, darkPokemonCountFlag12());
    msgctrlSetValue(0x30, darkPokemonCountFlag14());
}

typedef struct PdaSaveImage {
    u8 bytes[0x1dfd0];
} PdaSaveImage;

/* Save-and-quit flow reached from the People screen's model preview. */
#pragma peephole off
s32 pcMenuSave(void)
{
    extern u8 lbl_802E51C8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern s8 menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern u32 gamedataGetPlaytime(void);
    extern void gamedataSetPlaytime(f32 seconds);
    extern void* memcardAllocBuffer(void);
    extern void memcardFreeBuffer(void* buffer);
    extern s32 memcardWaitTask2(s32 a, s32 b, s32 c);
    extern void* savedataGetStatus(void* buffer, s32 kind);
    extern u32 gamedatasaveGetStatus(void* buffer, s32 kind);
    extern u32 heroGetStatus(void* data, s32 a, s32 b);
    extern void heroSetStatus(s32 a, s32 b, u8 value);
    extern u8 heroMoveIsMember(s32 index);
    extern void* gamedatasaveBiosGetPtr(void* data);
    extern s32 gamedatasaveBiosGetFloorid(void* data);
    extern s32 gamedatasaveBiosGetPrevfloorid(void* data);
    extern u8 gamedatasaveBiosGetFloorposindex(void* data);
    extern void gamedatasaveBiosSetFloorid(void* data, s32 value);
    extern void gamedatasaveBiosSetPrevfloorid(void* data, s32 value);
    extern void gamedatasaveBiosSetFloorposindex(void* data, u8 value);
    extern void floorGetSaveInfo(s32* floorId, s32* prevFloorId, u8* posIndex);
    struct {
        u8* base;
        u8* tbl;
    } refs;
    void* backup;
    u8* slot;
    s32 result;
    u8 saved;
    s32 running;
    s16 state;
    void* data;
    s8 answer;
    s32 prevFloorId;
    s32 floorId;
    u8 posIndex;

    refs.tbl = lbl_802E51C8;
    refs.base = lbl_803A6498;
    running = 1;
    state = 0;
    slot = refs.base + 0x48;
    do {
        switch (state) {
        case 0:
            lbl_8047A480 = (void*)0x3c01;
            menuOpen(0x28, 0);
            state = 1;
            break;
        case 1:
            pdaFillSavePrompt();
            answer = menuSubOpenYesNo(0, 0x3c, 0x118, 0);
            if (answer == 0) {
                state = 2;
            } else if (answer == 1) {
                state = 0x64;
                result = 1;
            } else {
                state = 0x64;
                result = 1;
            }
            break;
        case 2:
            saved = 1;
            backup = memcardAllocBuffer();
            *(PdaSaveImage*)backup = *(PdaSaveImage*)savedataGetStatus(0, 0);
            if (memcardWaitTask2(3, 2, 0) == 3 &&
                gamedatasaveGetStatus(0, 4) != 0) {
                if (heroGetStatus(0, 2, 0) ==
                        heroGetStatus(savedataGetStatus(backup, 2), 2, 0) ||
                    gamedatasaveGetStatus(backup, 4) == 0) {
                    lbl_8047A498 = gamedataGetPlaytime();
                    pdaFillSavePrompt();
                    lbl_8047A480 = (void*)0x1b60;
                    answer = menuSubOpenYesNo(0, 0x3c, 0x118, 1);
                    if (answer != 0) {
                        saved = 0;
                    } else {
                        saved = 1;
                    }
                    lbl_8047A498 = gamedataGetPlaytime() - lbl_8047A498;
                }
            }
            *(PdaSaveImage*)savedataGetStatus(0, 0) = *(PdaSaveImage*)backup;
            gamedataSetPlaytime((f32)(lbl_8047A498 + gamedataGetPlaytime()));
            lbl_8047A498 = 0;
            memcardFreeBuffer(backup);
            if (saved) {
                state = 4;
                result = 1;
            } else {
                state = 0;
            }
            break;
        case 4:
            heroSetStatus(0, 0x18, (u8)(heroMoveIsMember(1) ? 1 : 0));
            savedataGetStatus(0, 0);
            data = gamedatasaveBiosGetPtr(savedataGetStatus(0, 1));
            *(s32*)&refs.base[0x48] = gamedatasaveBiosGetFloorid(data);
            *(s32*)&slot[4] = gamedatasaveBiosGetPrevfloorid(data);
            slot[8] = gamedatasaveBiosGetFloorposindex(data);
            floorGetSaveInfo(&floorId, &prevFloorId, &posIndex);
            gamedatasaveBiosSetFloorid(data, floorId);
            gamedatasaveBiosSetPrevfloorid(data, prevFloorId);
            gamedatasaveBiosSetFloorposindex(data, posIndex);
            memcardWaitTask2(4, 2, 0);
            state = 0x64;
            break;
        case 0x64:
            menuCloseCustom(0x28, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    pdaResetPeopleTablesSave(refs.base, refs.tbl);
    return result;
}
#pragma peephole reset
#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_DRIVER_8003842C_ONLY */

#if !defined(PDA_SAVE_80038A0C_ONLY) && !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
s32 itemStorageMenuDrawItem(PdaSprite* context, PdaSprite* sprite)
{
    extern f32 lbl_803A65B0[][3];
    s32 index;

    index = context->selectedIndex;
    if (index < 0 || index >= 8) {
        index = 0;
    }
    sprite->field_50 = (s16)lbl_803A65B0[index][0];
    sprite->field_52 = (s16)lbl_803A65B0[index][1];
    *(s8*)((u8*)sprite + 0x67) = lbl_803A65B0[index][2];
    return 0;
}

s32 itemStorageMenuCursor(PdaSelectionWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 menuGetSelectItemNum();
    PdaKeyInfo* keyInfo;
    s32 itemCount;

    keyInfo = windowGetKeyInfo();
    if (keyInfo->buttons & 2) {
        itemCount = menuGetSelectItemNum(work->menu);
        itemCount = (s8)itemCount;
        if ((s8)++work->selectedIndex >= itemCount) {
            work->selectedIndex = itemCount - 1;
        }
    }
    if (keyInfo->buttons & 1) {
        if ((s8)--work->selectedIndex < 0) {
            work->selectedIndex = 0;
        }
    }
    return 0;
}

s32 itemStorageListDrawSprite2(void* window, void* sprite)
{
    s32 disp;

    if (lbl_8047A4A8 > 0) {
        disp = 1;
    } else {
        disp = 0;
    }
    winSpriteSetDisp(sprite, disp);
    return 0;
}

s32 itemStorageDrawList(void* work, PdaSprite* sprite)
{
    extern void spriteSetScissor(s32 x1, s32 y1, s32 x2, s32 y2);
    extern void spriteClearScissor(void);
    extern s32 itemStorageDrawItem(void* window, void* sprite);
    s32 x;
    s32 y;

    x = sprite->field_50;
    y = sprite->field_52;
    spriteSetScissor(0x118 - x, 0x8b - y, 0x150, 0x10d);
    itemStorageDrawItem((u8*)work + 0x94, *(void**)((u8*)work + 0x88));
    spriteClearScissor();
    return 0;
}

s32 itemStorageListDrawItem2(void* window, PdaSprite* sprite)
{
    extern s32 lbl_8047A4AC;
    extern s32 lbl_8047A4BC;
    extern f32 lbl_8047A4C0;
    s32 y;

    (void)window;
    y = lbl_8047A4AC * 31 + 0x9a;
    if (lbl_8047A4BC == 0) {
        y += (s32)lbl_8047A4C0;
    }
    sprite->field_52 = (s16)y;
    return 0;
}

s32 itemStorageListDrawSprite3(void* window, PdaSprite* sprite)
{
    extern s32 lbl_8047A4AC;
    extern s32 lbl_8047A4BC;
    extern f32 lbl_8047A4C0;
    s32 y;
    s32 disp;

    (void)window;
    if (lbl_8047A4B8 >= 0) {
        y = (lbl_8047A4B8 - lbl_8047A4A8) * 31 + 0x97;
        if (lbl_8047A4BC != 0) {
            y -= (s32)lbl_8047A4C0;
        }
        if (y + sprite->y < 0x97 || y >= 0x18f) {
            disp = 0;
        } else {
            disp = 1;
        }
        winSpriteSetDisp(sprite, disp);
    } else {
        y = lbl_8047A4AC * 31 + 0x97;
        if (lbl_8047A4BC == 0) {
            y += (s32)lbl_8047A4C0;
        }
        winSpriteSetDisp(sprite, 1);
    }
    sprite->field_52 = (s16)y;
    return 0;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 itemStorageListDrawText(PdaSprite* sprite)
{
    GSmsgPrintColor(0, 0, *(s32*)((u8*)sprite + 0x88), (void*)lbl_8047A4B4);
    return 0;
}
#pragma pop

/* The data id / count of the target-th listed item in PC box 0. */
static inline s32 pdaBoxItemId(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

/* This call site needs a distinct local order to retain its inline registers. */
static inline s32 pdaBoxItemIdFirst(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    s32 slots;
    void* item;
    s32 i;
    s32 found;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

static inline s32 pdaBoxItemNum(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetNum(item);
            }
        }
    }
    return 0;
}

/* PC item list: cursor/page input, item swapping, and caption refresh. */
/* RULE-EXCEPTION(user-approved): allocation shaping and local compiler controls;
   see docs/RULE_EXCEPTIONS.md, fn_80039A84. */
#pragma push
#pragma opt_lifetimes off
#pragma peephole off
s32 itemStorageListKey(void)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern u16 itemBiosGetItemDataId(void* item);
    extern void* itemDataBiosGetPtr(u16 id);
    extern s32 itemDataBiosGetDoc(void* data);
    extern void pcboxSwapItemSlot(s32 box, s16 from, s16 to);
    extern void soundStart(s32 id, s32 a, s32 b, s32 c);
    extern f32 lbl_8047BAB4;
    extern f32 lbl_8047BAB8;
    PdaKeyInfo* keyInfo;
    s32 docSlots;
    s32 docFound;
    s32 docIndex;
    void* docItem;
    s32 docTarget;
    s32 countSlots;
    s32 countTotal;
    s32 countIndex;
    s32 slots;
    s32 found;
    s32 i;
    s32 target;
    s32 max;
    s32 cursor;
    s32 message;
    u16 keys;
    u16 itemId;
    s32 top;
    s32 previousPageTop;
    u8 moved;
    void* item;

    keyInfo = windowGetKeyInfo();
    moved = 0;
    if (lbl_8047BAB0 != lbl_8047A4C0) {
        return 0;
    }
    countTotal = pcboxGetNbItemSlot(0);
    for (target = 0; target < countTotal; target++) {
        itemSlotIsValid(pcboxGetItem(0, (s16)target));
    }
    target = lbl_8047A4A8 + lbl_8047A4AC;
    if (lbl_8047A4B8 < 0) {
        if ((keyInfo->trigger & 0xc0) != 0) {
            itemId = pdaBoxItemIdFirst(target);
            if (itemId != 0) {
                lbl_8047A4B8 = target;
                soundStart(0x3c6, 0, 0xff, 0);
            }
        }
    } else {
        keys = keyInfo->trigger;
        if ((keys & 0xd0) != 0) {
            itemId = pdaBoxItemId(target);
            if (itemId != 0) {
                pcboxSwapItemSlot(0, (s16)lbl_8047A4B8, (s16)target);
                soundStart(0x3c6, 0, 0xff, 0);
            } else {
                soundStart(0x3c7, 0, 0xff, 0);
            }
            lbl_8047A4B8 = -1;
        } else if ((keys & 0x20) != 0) {
            lbl_8047A4B8 = -1;
            soundStart(0x3c7, 0, 0xff, 0);
        }
    }
    countTotal = 0;
    countSlots = pcboxGetNbItemSlot(0);
    for (countIndex = 0; countIndex < countSlots; countIndex++) {
        if ((u8)itemSlotIsValid(pcboxGetItem(0, (s16)countIndex)) != 0) {
            countTotal++;
        }
    }
    max = countTotal + 1;
    if ((keyInfo->buttons & 2) != 0) {
        lbl_8047A4AC++;
        if (lbl_8047A4AC + lbl_8047A4A8 >= max) {
            lbl_8047A4AC = lbl_8047A4AC - 1;
        } else {
            if (lbl_8047A4AC >= 8) {
                lbl_8047A4A8 = lbl_8047A4A8 + 1;
                lbl_8047A4AC = lbl_8047A4AC - 1;
                lbl_8047A4BC = 1;
            } else {
                lbl_8047A4BC = 0;
            }
            lbl_8047A4C0 = lbl_8047BAB4;
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 1) != 0 && moved == 0) {
        if (lbl_8047A4AC > 0 || lbl_8047A4A8 > 0) {
            cursor = lbl_8047A4AC - 1;
            lbl_8047A4AC = cursor;
            if (cursor < 0) {
                lbl_8047A4AC = 0;
                lbl_8047A4A8 = lbl_8047A4A8 - 1;
                lbl_8047A4BC = 1;
            } else {
                lbl_8047A4BC = 0;
            }
            lbl_8047A4C0 = lbl_8047BAB8;
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 8) != 0 && moved == 0) {
        top = lbl_8047A4A8 + 8;
        if (top < max) {
            lbl_8047A4A8 = top;
            if (top + lbl_8047A4AC >= max) {
                lbl_8047A4AC = max - top - 1;
            }
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 4) != 0 && moved == 0) {
        previousPageTop = lbl_8047A4A8 - 8;
        if (previousPageTop >= 0) {
            lbl_8047A4A8 = previousPageTop;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if (lbl_8047A4B8 >= 0) {
        message = 0x1b69;
    } else {
        docTarget = lbl_8047A4A8 + lbl_8047A4AC;
        itemId = pdaBoxItemId(docTarget);
        if (itemId != 0) {
            message = itemDataBiosGetDoc(itemDataBiosGetPtr(itemId));
        } else {
            message = 0x1b68;
        }
    }
    lbl_8047A4B4 = message;
    return 0;
}
#pragma pop

void itemStorageListButton(void* button)
{
    if (lbl_8047A4B8 < 0) {
        menuButtonNormal(button);
    }
}

/* PC item transfer loop: run the box list and move the highlighted stack
   between the bag (mode 0) and the PC (mode 1) until the list is closed. */
#pragma peephole off
void itemStorageTransfer(s32 mode)
{
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    extern void* itemDataBiosGetPtr(s32 id);
    extern u8 itemDataBiosGetKind(void* data);
    extern s32 heroItemCheckAddItemDataId(s32 bag, s32 id);
    extern void heroItemAddItemDataId(s32 bag, s32 id, u16 count, s32 slot);
    extern void pcboxAddItem(s32 box, s32 id, u16 count);
    extern void msgctrlSetValue(s32 id, s32 value);
    extern void winMsgOpen(s32 a, s32 message, s32 b, s32 c);
    extern void winMsgClose(s32 a);
    extern s32 itemStorageConfirm(void);
    extern s32 itemStorageSelectAmount(s32 a, s32 b, s32 c);
    s32 target;
    s32 found;
    s32 slots;
    void* item;
    s32 i;
    s32 itemId;
    s32 num;
    s32 count;
    s32 shortId;

    lbl_8047A4B0 = mode;
    while (1) {
        if (menuOpen(0x25, 1) == -1) {
            break;
        }
        target = lbl_8047A4A8 + lbl_8047A4AC;
        itemId = pdaBoxItemId(target);
        if ((u16)itemId == 0) {
            break;
        }
        switch (mode) {
        case 0:
            itemId = pdaBoxItemId(target);
            lbl_8047A4B4 = 0x1b6a;
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) != 5) {
                num = pdaBoxItemNum(target);
                count = itemStorageSelectAmount(1, 1, (u16)num);
            } else {
                count = 1;
            }
            if (count <= 0) {
                break;
            }
            if (heroItemCheckAddItemDataId(0, itemId) < count) {
                winMsgOpen(2, 0x1b6f, 1, 0);
                winMsgClose(1);
                break;
            }
            heroItemAddItemDataId(0, itemId, (u16)count, -1);
            pcboxAddItem(0, itemId, (u16)count);
            msgctrlSetValue(0x2d, (u16)itemId);
            msgctrlSetValue(0x2f, count);
            winMsgOpen(2, 0x1b70, 1, 0);
            winMsgClose(1);
            break;
        case 1:
            itemId = pdaBoxItemId(target);
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
                winMsgOpen(2, 0x1b72, 1, 0);
                winMsgClose(1);
                break;
            }
            lbl_8047A4B4 = 0x1b6b;
            num = pdaBoxItemNum(target);
            count = itemStorageSelectAmount(1, 1, (u16)num);
            if (count <= 0) {
                break;
            }
            shortId = (u16)itemId;
            msgctrlSetValue(0x2d, shortId);
            msgctrlSetValue(0x2f, count);
            lbl_8047A4B4 = 0x1b6c;
            if (itemStorageConfirm() == 0) {
                break;
            }
            pcboxAddItem(0, itemId, (u16)count);
            msgctrlSetValue(0x2d, shortId);
            msgctrlSetValue(0x2f, count);
            winMsgOpen(2, 0x1b71, 1, 0);
            winMsgClose(1);
            break;
        }
    }
    menuClose(0x25);
    menuCloseSync(0x25, 1);
}
#pragma peephole reset

s32 menuAmountDraw(PdaDrawWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267130[3];
    extern s32 lbl_8047A4C8;
    extern void GSmsgPrintRight(s32, s32, s16, s16, void*, s32);
    extern void msgctrlSetValue(s32 id, s32 value);
    s32 index;
    s32 i;
    s32 divisor;
    s32 ids[3];
    s32 digit;

    ids[0] = lbl_80267130[0];
    ids[1] = lbl_80267130[1];
    ids[2] = lbl_80267130[2];
    for (index = 0; index < 3; index++) {
        if (sprite->eventId == ids[index]) {
            break;
        }
    }
    if (index >= 3) {
        return 0;
    }

    divisor = 1;
    for (i = 0; i < index; i++) {
        divisor *= 10;
    }
    digit = lbl_8047A4C8 / divisor;
    digit %= 10;
    msgctrlSetValue(0x34, digit);
    GSmsgPrintRight(0, 0, sprite->x, sprite->y, work->drawData, 0xC9);
    return 0;
}

typedef struct PdaNumberRange {
    s32 max;
    s32 min;
} PdaNumberRange;

typedef struct PdaNumberWork {
    u8 pad00[0x60];
    PdaNumberRange* range;
    u8 pad64[0x31];
    s8 digitIndex;
} PdaNumberWork;

/* Digit-wise spinner for the PDA's numeric entry field: left/right step the
   selected digit with wraparound inside the field's range, up/down move
   between digits. */
s32 menuAmountKey(PdaNumberWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 lbl_8047A4C8;
    extern void soundStart(s32 id, s32 a, s32 b, s32 c);
    PdaKeyInfo* keyInfo;
    PdaNumberRange* range;
    s32 step;
    s32 i;
    s32 cur;
    s32 digit;
    s32 base;
    s32 limit;
    s32 value;

    range = work->range;
    keyInfo = windowGetKeyInfo();
    if ((keyInfo->buttons & 0xf) != 0) {
        step = 1;
        for (i = 0; i < work->digitIndex; i++) {
            step *= 10;
        }
        if ((keyInfo->buttons & 1) != 0 && range->max != range->min) {
            if (step < 10) {
                value = lbl_8047A4C8 + step;
                lbl_8047A4C8 = value;
                if (value > range->max) {
                    lbl_8047A4C8 = range->min;
                }
                soundStart(0x3c5, 0, 0xff, 0);
            } else {
                cur = lbl_8047A4C8;
                digit = (cur / step) % 10;
                base = cur - digit * step;
                for (limit = 9; limit >= 0; limit--) {
                    if (base + limit * step <= range->max) {
                        break;
                    }
                }
                digit++;
                if (digit > limit) {
                    digit = 0;
                }
                lbl_8047A4C8 = base + digit * step;
                if (lbl_8047A4C8 < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    soundStart(0x3c5, 0, 0xff, 0);
                }
            }
        }
        if ((keyInfo->buttons & 2) != 0 && range->max != range->min) {
            if (step < 10) {
                value = lbl_8047A4C8 - step;
                lbl_8047A4C8 = value;
                if (value < range->min) {
                    lbl_8047A4C8 = range->max;
                }
                soundStart(0x3c5, 0, 0xff, 0);
            } else {
                cur = lbl_8047A4C8;
                digit = (cur / step) % 10;
                base = cur - digit * step;
                for (limit = 9; limit >= 0; limit--) {
                    if (base + limit * step <= range->max) {
                        break;
                    }
                }
                digit--;
                if (digit < 0) {
                    digit = limit;
                }
                lbl_8047A4C8 = base + digit * step;
                if (lbl_8047A4C8 < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    soundStart(0x3c5, 0, 0xff, 0);
                }
            }
        }
        if ((keyInfo->buttons & 8) != 0) {
            if (--work->digitIndex < 0) {
                work->digitIndex = 0;
            }
        }
        if ((keyInfo->buttons & 4) != 0) {
            if (++work->digitIndex >= 3) {
                work->digitIndex = 2;
            }
        }
    }
    return 0;
}

s32 menuAmountCtrl(PdaMenuState* state)
{

    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x26, 0xb4);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x26, 0xb8);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}

s32 itemStorageSelectAmount(s32 arg0, s32 arg1, s32 arg2)
{
    extern s32 lbl_8047A4C8;
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    s32 args[2];
    s32 result;
    s32 value;

    lbl_8047A4C8 = arg0;
    args[0] = arg2;
    args[1] = arg1;
    result = menuOpenCustom(0x26, windowGetActiveID(), 0, 0, 1, 1, args);
    menuClose(0x26);
    menuCloseSync(0x26, 1);
    if (result == -1) {
        value = -1;
    } else {
        value = lbl_8047A4C8;
    }
    return value;
}

s32 itemStorageConfirmDrawSprite(PdaSelectionWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267140[4];
    s32 groups[4];
    s32* q;
    s32* p;
    s32 group;
    s32 found;
    s32 i;
    u8 flag;
    s8 selectedIndex;

    groups[0] = lbl_80267140[0];
    groups[1] = lbl_80267140[1];
    groups[2] = lbl_80267140[2];
    groups[3] = lbl_80267140[3];
    p = groups;
    found = 0;
    for (group = 0; group < 2; p += 2, group++) {
        for (q = p, i = 0; i < 2 && !found; q++, i++) {
            if (sprite->eventId == *q) {
                found = 1;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return 0;
    }

    selectedIndex = work->selectedIndex;
    if (group == (s32)selectedIndex) {
        flag = 1;
    } else {
        flag = 0;
    }
    winSpriteSetDisp(sprite, flag);
    return 0;
}

s32 itemStorageConfirm(void)
{
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    s32 result;
    s32 value;

    result = menuOpen(0x27, 1);
    menuClose(0x27);
    menuCloseSync(0x27, 1);
    if (result == -1) {
        value = 0;
    } else if (result == 0) {
        value = 1;
    } else {
        value = 0;
    }
    return value;
}

void pdaSnagListDrawCount(PdaSprite* sprite)
{
    extern PdaListEntry* lbl_8047A4D4;
    extern void darkPokemonCheckFlag0C(u16 id);
    extern void* darkPokemonGetFlagListValue(u16 id, u8* variant);
    extern s32 darkPokemonSaveGetC(u16 id);
    extern s32 floorGetPlaceName(s32 floor);
    extern void msgctrlSetValue(s32 id, s32 value);
    u16 id;
    void* message;
    u32 value;

    darkPokemonCheckFlag0C(lbl_8047A4D4[(u16)*(u32*)&lbl_803A6748].battleId);
    id = lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    message = darkPokemonGetFlagListValue(id, (u8*)&lbl_803A6748 + 0x94);
    value = floorGetPlaceName(darkPokemonSaveGetC(id));
    if (value == 0) {
        msgctrlSetValue(0x31, 0x18d3);
    } else {
        msgctrlSetValue(0x31, value);
    }
    GSmsgPrintColor(0, 0, (u32)sprite->alphaByte | -0x100LL, message);
}

#pragma scheduling off
void pdaSnagListCursor(void* window)
{
    pdaSnagListDetailKey(window, 1);
}
#pragma scheduling reset

#pragma scheduling off
void pdaSnagListCursor2(void* window)
{
    pdaSnagListDetailKey(window, 0);
}
#pragma scheduling reset

#pragma peephole off
void pdaSnagListDrawItem(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAD8;
    u8* layoutData;
    s16* baseY;
    f32 offset;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_10 > 10) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    baseY = (s16*)((layoutData = lbl_802EF0A8) + 0x5084);
    offset = lbl_803A6748.field_18 /
             -(lbl_8047BAD8 * (f32)(lbl_803A6748.field_10 - 10));
    sprite->field_52 =
        (s16)(*baseY + offset * *(s16*)(layoutData + 0x5088));
}
#pragma peephole reset

#pragma peephole off
void pdaSnagListDrawAnim(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAF0;
    extern f32 lbl_8047BAF4;
    volatile int pad;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_0C >= 10) {
        if (lbl_803A6748.field_10 != lbl_803A6748.field_0C) {
            sprite->flags = sprite->flags | 2;
        } else {
            sprite->flags = sprite->flags & ~2;
            return;
        }
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BAF4 * lbl_803A6748.field_28;
    f0 = lbl_8047BAF4 * f0;
    if (lbl_8047BAF0 && lbl_8047BAF0) {
        /* Preserve MWCC register allocation. */
    }
    f1 = lbl_8047BAF0 * f0;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BAF4 * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x504C));
}
#pragma peephole reset

#pragma peephole off
void pdaSnagListDrawAnim2(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAF0;
    extern f32 lbl_8047BAF4;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_08 > 0) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BAF4 * lbl_803A6748.field_28;
    f0 = lbl_8047BAF4 * f0;
    f1 = lbl_8047BAF0 * f0 + lbl_8047BAF0;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BAF4 * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5014));
}
#pragma peephole reset

#pragma peephole off
void pdaSnagListDetailDrawItem(PdaSprite* sprite, PdaEvent* event)
{
    (void)event;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
}
#pragma peephole reset

void pdaSnagListDrawItem2(PdaSprite* sprite, PdaEvent* event)
{
    switch (event->messageId) {
    case 0x2e2:
    case 0x2e4:
        sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
        break;
    default:
        pdaSnagListDrawDetail(sprite, event);
        break;
    }
}

#pragma peephole off
void pdaSnagListDrawItem3(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    s32 row;
    s16 baseY;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    baseY = *(s16*)(lbl_802EF0A8 + 0x50a0);
    row = *(s32*)&lbl_803A6748 - lbl_803A6748.field_08;
    if (row >= 10) {
        row = 9;
    }
    *(f32*)((u8*)&lbl_803A6748 + 0x38) =
        (f32)(baseY + row * 0x18);
    sprite->field_52 = *(f32*)((u8*)&lbl_803A6748 + 0x30);
}
#pragma peephole reset

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 pdaSnagListGetTrainerName(s32 index)
{
    extern u32 lbl_8047A4D0;
    extern PdaListEntry* lbl_8047A4D4;
    extern s32 GSmsgGetGSchar(s32 message);
    extern void* fightTrainerDataBiosGetPtr();
    extern u16 fightTrainerDataBiosGetKindDataId(void* trainer);
    extern u32 fightTrainerKindDataBiosGetPrefixName(void* kind);
    extern void* fightTrainerKindDataBiosGetPtr(u16 kind);
    extern u8 darkPokemonSaveGet49C(u16 id);
    extern s32 darkPokemonGetFlagListValue(u16 id, s8* variant);
    extern u32 darkPokemonSaveGetC(u16 id);
    extern u16 darkPokemonGetSpecies(u16 id, s32 side);
    u16 battleId;
    void* kindData;
    void* trainer;
    u16 kind;
    s8* variant;
    u16 alternate;
    s32 message;
    if (lbl_8047A4D0 != 0) {
        battleId = lbl_8047A4D4[index].battleId;
        variant = &lbl_803A6748.variant;
        message = darkPokemonGetFlagListValue(battleId, variant);
        switch ((u8)*variant) {
        case 0:
        case 1:
            message = 0x3720;
            break;
        case 2:
            kind = darkPokemonGetSpecies(battleId, 0);
            if (kind == 9) {
                kindData = fightTrainerKindDataBiosGetPtr(darkPokemonSaveGet49C(battleId));
            } else {
                alternate = darkPokemonGetSpecies(battleId, 1);
                if (alternate != 0 && darkPokemonSaveGetC(battleId) != 3) {
                    kind = alternate;
                }
                trainer = fightTrainerDataBiosGetPtr(kind);
                kindData = fightTrainerKindDataBiosGetPtr(
                    fightTrainerDataBiosGetKindDataId(trainer));
            }
            if (battleId == 0x43) {
                message = 0x12C0;
            } else {
                message = fightTrainerKindDataBiosGetPrefixName(kindData);
            }
            break;
        }
        return GSmsgGetGSchar(message);
    }
    return 0;
}
#pragma pop


static inline u16 pda_8003D818_LoadIds(void)
{
    extern u16* lbl_8047A4E4;
    extern u16 memoDataGetPokemonID(u16* data, u32 index);
    extern u16 memoDataGetCount(u16* data);
    u32 index;
    u16 count;

    count = 0;
    for (index = 0; (u16)index < memoDataGetCount(0); index++) {
        lbl_8047A4E4[(u16)index] = memoDataGetPokemonID(0, index);
        count++;
    }
    return count;
}

/* Defined after its caller pdaMemoMainKey, which retail calls rather than inlines. */
u16 pdaMemoBuildList(void);

void pdaSceneEnter0(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaMemoUpdateScroll(work, 0);
}

void pdaSceneEnter1(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaModelViewerControl(work, 1);
}

void pdaSceneEnter2(void* work)
{
    GSscene_SetMode(4);
    pdaReadPadInput(lbl_803A67FC);
    pdaModelViewerControl(work, 0);
}

s32 pdaSceneGetWork(void)
{
    return lbl_803A6818.field_28;
}

void pdaPeopleUpdateAlpha(PdaSprite* sprite, PdaEvent* event)
{
    extern f32 lbl_8047BCA0;
    extern void pdaMemoDrawPokemonDetail(PdaSprite* sprite, PdaEvent* event);

    sprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    switch (event->messageId) {
    case 0x331:
    case 0x759:
    case 0x76a:
    case 0xfbe:
        break;
    default:
        pdaMemoDrawPokemonDetail(sprite, event);
        break;
    }
}

#pragma push
#pragma peephole off
/* RULE-EXCEPTION(user-approved): allocation-shaping temporaries preserve the retail conversion spill order. See docs/RULE_EXCEPTIONS.md. */
void pdaMemoDrawItem(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BCA0;
    f32 numerator;
    f32 negated;
    u8* layoutEntry;
    u8* layoutValue;
    u8* layoutData;
    extern f32 lbl_8047BCF4;
    f32 f0;
    s32 count_m1;
    f32 f3;
    s16 stored;

    alphaSprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    if (lbl_803A6818.field_10 > 10) {
        sprite->flags = sprite->flags | 2;
        do {
        } while (0);
    } else {
        sprite->flags = sprite->flags & ~2;
        if (1 != 0) {
            return;
        }
    }
    layoutData = lbl_802EF0A8;
    f0 = *(f32*)((u8*)&lbl_803A6818 + 0x30);
    negated = -f0;
    layoutValue = (layoutEntry = lbl_802EF0A8 + 0x5788);
    count_m1 = lbl_803A6818.field_10 - 1;
    numerator = negated;
    f3 = numerator / (f32)count_m1;
    f0 = lbl_8047BCF4;
    sprite->field_52 =
        (s16)((f3 / f0) * (stored = *(s16*)layoutValue) +
              *(s16*)(layoutData + 0x5784));
}
#pragma pop

void pdaMemoDrawAnim(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern s32 lbl_804788C0;
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BD0C;
    extern f64 sin(f64 angle);

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    if (lbl_803A6818.currentIndex != lbl_803A6818.field_10 - 1) {
        if (lbl_804788C0 != 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
            return;
        }
    } else {
        sprite->flags &= ~2;
        return;
    }
    sprite->field_52 =
                (s16)(lbl_8047BD0C *
                          (f32)sin(3.1415927f * (2.0f * (2.0f * lbl_803A6818.angle))) +
                      (f32)*(s16*)(lbl_802EF0A8 + 0x57F4));
}

void pdaMemoDrawAnim2(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BCA4;
    extern f32 lbl_8047BCA8;
    extern f32 lbl_8047BD0C;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    if (*(s32*)&lbl_803A6818 != 0) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BCA8 * *(f32*)((u8*)&lbl_803A6818 + 0x40);
    f0 = lbl_8047BCA8 * f0;
    f1 = lbl_8047BCA4 * f0 + lbl_8047BCA4;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BD0C * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x57bc));
}

void menu9FDrawItem(PdaSprite* alphaSprite)
{
    extern PdaOrbitPoint lbl_802E52C8[8];
    extern u32 lbl_8047A4EC;
    extern f32 lbl_8047BCA0;
    extern void pdaDrawSpline(PdaOrbitPoint* point);
    s32 i;

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    for (i = 0; i < 8; i++) {
        pdaDrawSpline(&lbl_802E52C8[i]);
    }
    for (i = (s32)lbl_8047A4EC - 1; i >= 0; i--) {
        lbl_802E52C8[i].alpha -= 10.0f;
        if (lbl_802E52C8[i].alpha < 0.0f) {
            lbl_802E52C8[i].alpha = 0.0f;
        }
    }
    for (i = 7; i > lbl_8047A4EC; i--) {
        lbl_802E52C8[i].alpha -= 10.0f;
        if (lbl_802E52C8[i].alpha < 0.0f) {
            lbl_802E52C8[i].alpha = 0.0f;
        }
    }
}

extern u8 lbl_802EF0A8[];
extern u8 lbl_80314F98[];
extern u8 lbl_80267190[];
extern u8 lbl_803A6A60[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D8;
extern s32 lbl_8047A4DC;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u16 lbl_8047A4E8;
extern s32 lbl_804788C0;
extern s16 lbl_804788C8[2];
extern s16 lbl_804788CC[2];
extern s16 lbl_804788D0[2];
extern f32 lbl_8047BC94;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BAC4;
extern f32 lbl_8047BAC8;
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BCC8;
extern const f32 lbl_8047BCF4;
extern f32 lbl_8047BCF8;
extern f32 lbl_8047BAC0;
extern f32 lbl_8047BDAC;
extern f32 lbl_8047BDF0;

extern u32 gfxVtxTexCoord();
extern u32 gfxVtxColor4u8();
extern u32 gfxVtxPos2s16();
extern u32 gfxEnd();
extern u32 gfxBegin();
extern u32 gfxSetPrimitive();
extern u32 gfxSetVtxDesc();
extern u32 gfxLoadTexture();
extern u32 gfxClearFlags();
extern u32 gfxSetFlags();
extern u32 GSmemGetHandleByPtr();
extern u32 GSmemFree();
extern u32 GSmemUnlock();
extern u32 GSmemLock();
extern u32 GSmemAlloc();
extern u32 fn_800FA280();
extern void* menuDataBiosGetPtr(s32);
extern u32 fn_80102510();
extern u32 fn_8010264C();
extern u32 fn_801040F0();
extern void* windowGetKeyInfo(void*);
extern u32 menuModelRender();
extern u32 fn_80109B90();
extern u32 fn_8011D8D8();
extern u32 fn_8011D8F4();
extern u32 fn_8011DFE0();
extern u32 pokemonGetSex();
extern u32 fn_801240C4();
extern u32 fn_80132A38();
extern u32 soundStartRaw(u32, u32, u32);
extern u32 GSresReleaseById();
extern s32 GSresGroupGetState();
extern u32 fsysStartLoad();
extern u32 darkPokemonSaveGet4A4();
extern u16 darkPokemonSaveGet4A0();
extern u32 darkPokemonSaveGetName();
extern u16 darkPokemonGetFlag1C();
extern u32 darkPokemonSaveGet8();
extern u8 darkPokemonCheckFlag16();
extern u8 darkPokemonCheckFlag12();
extern u8 darkPokemonCheckFlag14();
extern u8 darkPokemonCheckFlag0C(u16);
extern u16 darkPokemonGetSpecies();
extern u32 fightTrainerDataBiosGetStatus03();
extern u32 fightTrainerDataBiosGetPtr();
extern u32 gamedataGetStatus();
extern void menuCloseSync(s32, s32);
extern u32 bagMenuOpen();
extern u32 GSresReleaseOwner();

extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);
extern u8 menuModelCheck(void* work, s32 index);

static inline void pdaDrawModelTexture(PdaSprite* sprite, u8* model)
{
    u32 texture = menuModelRender(model);
    if (texture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxBegin(2);
        gfxVtxPos2s16(0, 0);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BAC4, lbl_8047BAC4);
        gfxVtxPos2s16(sprite->x, sprite->y);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BAC8, lbl_8047BAC8);
        gfxEnd();
    }
}

void pdaSnagListDrawModel(u8* context, PdaSprite* sprite)
{
    u8* model;
    u16 battleId;
    u16 entryId;
    u8* layout;
    u8 registered;
    u8 cleared;
    s32 state;
    s32 messageId;

    switch (sprite->eventId) {
    case 0xD96:
    case 0xD97:
    case 0xD99:
    case 0x308:
    case 0x30B:
        break;
    case 0xD98:
        if (darkPokemonCheckFlag16(lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId) != 0 &&
            menuModelCheck(model = (u8*)&lbl_803A6748 + 0x4C, 0) == 0) {
            pdaDrawModelTexture(sprite, model);
        }
        break;
    default:
        battleId = lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId;
        darkPokemonCheckFlag0C(lbl_8047A4D4[(u16)*(u32*)&lbl_803A6748].battleId);
        layout = lbl_802EF0A8;
        entryId = lbl_8047A4D4[(u16)*(u32*)&lbl_803A6748].battleId;
        darkPokemonGetFlag1C(entryId);
        darkPokemonCheckFlag16(entryId);
        registered = darkPokemonCheckFlag12(entryId);
        cleared = darkPokemonCheckFlag14(entryId);
        if (registered != 0) {
            if (cleared != 0) {
                state = 1;
            } else {
                state = 0;
            }
        } else {
            state = 2;
        }
        switch (state) {
        case 0:
            windowDrawSprite(
                *(s16*)(layout + 0x55FA) - sprite->field_50 - 7,
                *(s16*)(layout + 0x55FC) - sprite->field_52 - 4,
                (PdaSprite*)context, 0x161, 0);
            break;
        case 1:
            windowDrawSprite(
                *(s16*)(layout + 0x55FA) - sprite->field_50,
                *(s16*)(layout + 0x55FC) - sprite->field_52,
                (PdaSprite*)context, 0x160, 0);
            break;
        case 2:
            break;
        }
        registered = darkPokemonCheckFlag16(battleId);
        cleared = darkPokemonCheckFlag14(battleId);
        if (registered != 0) {
            if (cleared != 0) {
                messageId = 0x36E7;
            } else {
                messageId = 0x36E6;
            }
        } else {
            messageId = 0x36E8;
        }
        GSmsgPrintColor(0, 0, context[0x8B] | -0x100, (void*)messageId);
        break;
    }
}

void pdaMemoDrawModelSprite(u8* context, PdaSprite* sprite)
{
    u32 firstTexture;
    u32 secondTexture;
    s32 alpha;
    s32 mode;

    secondTexture = 0;
    if (lbl_804788C0 == 0) {
        return;
    }
    mode = *(s32*)((u8*)&lbl_803A6818 + 0x1C);
    if (mode == 0xC) {
        return;
    }
    switch (mode) {
    case 1:
    case 2:
        lbl_804788C8[0] = lbl_804788CC[0];
        lbl_804788C8[1] = lbl_804788CC[1];
        break;
    case 4:
        lbl_804788C8[0] = 0;
        lbl_804788C8[1] = 0;
        break;
    case 5:
        lbl_804788C8[0] = lbl_804788D0[0];
        lbl_804788C8[1] = lbl_804788D0[1];
        break;
    }
    if (mode == 5) {
        if (*((u8*)&lbl_803A6818 + 0x214) != 0) {
            firstTexture = menuModelRender((u8*)&lbl_803A6818 + 0x7C);
            secondTexture = menuModelRender((u8*)&lbl_803A6818 + 0xC4);
        } else {
            firstTexture = 0;
            secondTexture = 0;
        }
    } else if (*((u8*)&lbl_803A6818 + 0x214) != 0) {
        firstTexture = menuModelRender((u8*)&lbl_803A6818 + 0x7C);
    } else {
        firstTexture = 0;
    }
    alpha = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x5C);
    if (firstTexture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, firstTexture);
        gfxBegin(2);
        gfxVtxPos2s16(lbl_804788C8[0], lbl_804788C8[1]);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BC94, lbl_8047BC94);
        gfxVtxPos2s16((s16)(lbl_804788C8[0] + (s16)sprite->x),
                     (s16)(lbl_804788C8[1] + (s16)sprite->y));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BCBC, lbl_8047BCBC);
        gfxEnd();
    }
    if (secondTexture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, secondTexture);
        gfxBegin(2);
        gfxVtxPos2s16(lbl_804788C8[0], lbl_804788C8[1]);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BC94, lbl_8047BC94);
        gfxVtxPos2s16((s16)(lbl_804788C8[0] + (s16)sprite->x),
                     (s16)(lbl_804788C8[1] + (s16)sprite->y));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BCBC, lbl_8047BCBC);
        gfxEnd();
    }
}

static inline f32 pdaAbsF(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* RULE-EXCEPTION(user-approved): reconstructed inline helpers pdaAbsF/pdaApproach (fn_80040018, pdaMemoUpdateScroll, fn_80044630) — see docs/RULE_EXCEPTIONS.md */
static inline void pdaApproach(f32* current, f32* target, f32 delta)
{
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = lbl_8047BCC4 * step;
    step = scaled * delta;
    if (step > lbl_8047BC98) {
        step = lbl_8047BC98;
    }
    if (step <= lbl_8047BCC8) {
        step = lbl_8047BCC8;
    }
    distance = *target - (*current += step);
    if (pdaAbsF(distance) <= pdaAbsF(step) || pdaAbsF(distance) < lbl_8047BCBC) {
        *current = *target;
    }
}

#pragma peephole off
#define G(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))
#define GI(off) (*(s32*)((u8*)&lbl_803A6818 + (off)))
void pdaUpdateFadeAndSelect(u8* menu)
{
    f32 target;
    f32 current;
    f32 step;
    f32 difference;
    f32 next;

    target = G(0x50);
    current = G(0x4C);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCB8;
        if (target > current) {
            next = current + step;
            G(0x4C) = next;
            if (next > lbl_8047BCBC) {
                G(0x4C) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x4C) = next;
            if (next < lbl_8047BC94) {
                G(0x4C) = lbl_8047BC94;
            }
        }
    }

    target = G(0x58);
    current = G(0x54);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCB8;
        if (target > current) {
            next = current + step;
            G(0x54) = next;
            if (next > lbl_8047BCBC) {
                G(0x54) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x54) = next;
            if (next < lbl_8047BC94) {
                G(0x54) = lbl_8047BC94;
            }
        }
    }

    target = G(0x60);
    current = G(0x5C);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCC0;
        if (target > current) {
            next = current + step;
            G(0x5C) = next;
            if (next > lbl_8047BCBC) {
                G(0x5C) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x5C) = next;
            if (next < lbl_8047BC94) {
                G(0x5C) = lbl_8047BC94;
            }
        }
    }

    if (G(0x1DC) != G(0x1E0)) {
        pdaApproach(&G(0x1DC), &G(0x1E0), G(0x3C));
    }

    if (G(0x50) == G(0x4C) && menu != 0) {
        u8* keyInfo;
        u16 buttons;

        keyInfo = (u8*)windowGetKeyInfo(
            menuDataBiosGetPtr(*(s32*)(menu + 4)));
        buttons = *(u16*)(keyInfo + 4);
        if (lbl_804788C0 != 0) {
            if (buttons & 0x10) {
                soundStartRaw(0x24, 0, 0);
                menu[0x98] = 1;
                GI(0x18) = 0;
            }
            if (buttons & 0x40) {
                soundStartRaw(0x24, 0, 0);
                menu[0x98] = 1;
                GI(0x18) = 1;
            }
        }
        if (buttons & 0x20) {
            soundStartRaw(0x25, 0, 0);
            GI(0x18) = -1;
            menu[0x98] = 1;
            menu[0x99] = 1;
        }
    }
}
#undef G
#undef GI
#pragma peephole reset

/* Load the memo row at byte `offset` into the shared PDA work Pokemon. */
static inline u32 pdaLoadRowPokemon(s32 offset)
{
    extern u32 gamedataGetStatus(s32 a, s32 b);
    extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
    extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
    extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
    extern void pokemonBiosSetRnd(u32 work, u32 rnd);
    extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = *(u16*)((u8*)lbl_8047A4E4 + offset);
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Species-name message for the memo row at byte `offset`, 0 when
   unavailable. */
static inline u32 pdaRowNameMsg(s32 offset)
{
    extern u16 pokemonBiosGetPokemonDataId(u32 work);
    extern void* pokemonDataBiosGetPtr(u16 id);
    extern void* pokemonDataBiosGetName(void* data);
    extern u32 GSmsgGetGSchar(u32 msg);
    u32 work = lbl_8047A4E0;

    if (work != 0) {
        work = pdaLoadRowPokemon(offset);
        if (work != 0) {
            return GSmsgGetGSchar((u32)pokemonDataBiosGetName(
                pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work))));
        }
        return 0;
    }
    return 0;
}

/* Draw the visible slice of the species-name column. */
static inline void pdaDrawNameRows(u8* context, s32 first, f32 y)
{
    extern void msgctrlSetValue(s32 id, u32 value);
    extern u32 GSmsgGetGSchar(u32 msg);
    s32 offset;
    s32 i;
    u32 name;

    for (i = first, offset = first; i < lbl_8047A4E8; i++) {
        if (i >= *(s32*)((u8*)&lbl_803A6818 + 8) - 1 &&
            i <= *(s32*)((u8*)&lbl_803A6818 + 0xC) + 1) {
            name = pdaRowNameMsg(offset);
            if (name == 0) {
                name = GSmsgGetGSchar(1);
            }
            msgctrlSetValue(0x37, name);
            GSmsgPrintColor(0, (s32)y - 2, (u32)context[0x8B] | -0x100LL,
                        (void*)0xE7);
        }
        y += lbl_8047BCF4;
        offset += 2;
    }
}

/* Species-name column of the memo list. */
void pdaMemoDrawSpeciesColumn(u8* context, PdaSprite* sprite)
{
    extern void msgctrlSetValue(s32 id, u32 value);
    extern u16 memoDataGetCount(s32 a);
    extern u32 GSmsgGetGSchar(u32 msg);
    extern s8 pdaStrategyMemoGetSelection0(void);
    extern s8 pdaStrategyMemoGetSelection1(void);
    u8* entry;
    s16 messageId;

    /* retail reads the s16 eventId at +6 here, not messageId at +0x4c;
       0x12B2 is an eventId value -- it appears as a case in the switch below */
    switch (sprite->eventId) {
    case 0x12B2:
        context[0x8B] = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x54);
        break;
    default:
        context[0x8B] = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4C);
        break;
    }
    messageId = sprite->eventId;
    switch (messageId) {
    case 0xD46:
    case 0x12B2:
    case 0x31D:
    case 0x31E:
        break;
    case 0x119B:
        if (pdaStrategyMemoGetSelection0() == 1 && pdaStrategyMemoGetSelection1() >= 1) {
            pdaMemoDrawModelSprite(context, sprite);
        }
        break;
    case 0x76D:
        msgctrlSetValue(0x34, memoDataGetCount(0));
        break;
    default:
        entry = lbl_802EF0A8 + messageId * 0x1C;
        spriteSetScissor(
            *(s16*)(lbl_802EF0A8 + 0x5712) - *(s16*)(entry + 2),
            *(s16*)(lbl_802EF0A8 + 0x5714) - *(s16*)(entry + 4),
            *(s16*)(lbl_802EF0A8 + 0x5716),
            *(s16*)(lbl_802EF0A8 + 0x5718));
        pdaDrawNameRows(context, 0,
                        *(f32*)((u8*)&lbl_803A6818 + 0x30));
        spriteClearScissor();
        break;
    }
}

extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

#pragma peephole off
void pdaSnagListCreatePokemon(u8* context)
{
    u8 caught;
    u32 liveModel;
    u32 msg;
    u32 message;
    u32 rnd;
    u32 model;
    u32 index;
    u16 curBattleId;
    u8 seen;
    s32 battleState;
    u16 species;
    u32 battleId;

    context[0x8B] = lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44);
    index = *(u32*)&lbl_803A6748;
    species = lbl_8047A4D4[index].field_00;
    battleId = lbl_8047A4D4[index].battleId;
    curBattleId = lbl_8047A4D4[(u16)index].battleId;
    model = lbl_8047A4D0;
    darkPokemonGetFlag1C(curBattleId);
    darkPokemonCheckFlag16(curBattleId);
    seen = darkPokemonCheckFlag12(curBattleId);
    caught = darkPokemonCheckFlag14(curBattleId);
    if (seen != 0) {
        if (caught != 0) {
            battleState = 1;
        } else {
            battleState = 0;
        }
    } else {
        battleState = 2;
    }

    pokemonCreate(model, species, 0xA, gamedataGetStatus(0, 1));
    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
    darkPokemonSaveGet8(battleId);
    pokemonBiosSetRnd(model, rnd);
    switch (battleState) {
    case 0:
    case 2:
        pokemonBiosSetDarkpokemonDataId(model, 1);
        pokemonBiosSetDp(model, 0xA);
        break;
    case 1:
        pokemonBiosSetDarkpokemonDataId(model, 0);
        pokemonBiosSetDp(model, 0);
        break;
    }

    if ((liveModel = lbl_8047A4D0) != 0) {
        if ((u16)battleId != 0) {
            if (darkPokemonCheckFlag16(battleId) != 0) {
                pokemonBiosSetRnd(liveModel, darkPokemonSaveGet8(battleId));
                message = pokemonGetSex(liveModel);
                pokemonBiosSetRnd(liveModel, *(u32*)((u8*)&lbl_803A6748 + 0x98));
            } else {
                message = 2;
            }
        } else {
            message = 0x80;
        }
    } else {
        message = 0x80;
    }
    message &= 0xFF;
    if (message == 0x80) {
        msg = GSmsgGetGSchar(1);
    } else {
        switch (message) {
        case 0:
            msg = GSmsgGetGSchar(0xD67);
            break;
        case 1:
            msg = GSmsgGetGSchar(0xD68);
            break;
        case 2:
            msg = GSmsgGetGSchar(0xD69);
            break;
        default:
            msg = GSmsgGetGSchar(0xD68);
            break;
        }
    }
    msgctrlSetValue(0x37, msg);
    {
        u32 alpha = context[0x8B];
        s32 color = alpha | -0x100LL;

        GSmsgPrintColor(0, 0, color, (void*)0xCF);
    }
}
#pragma peephole reset

void pdaTrainerDataSetCursor(s32 unused, PdaSprite* sprite, s32 messageId)
{
    u32 periods[3];
    u8 raw;
    u8 sel;
    s8 selection;
    f32* angles;
    s16* layout;
    f32 step;

    periods[0] = ((u32*)lbl_80267190)[0];
    periods[1] = ((u32*)lbl_80267190)[1];
    periods[2] = ((u32*)lbl_80267190)[2];
    switch (messageId) {
    case 0x6E7:
    case 0x75D:
    case 0x760:
    case 0x763:
        *(u32*)(lbl_803A6A60 + 0x44) = 0;
        break;
    case 0x6E8:
    case 0x75E:
    case 0x761:
    case 0x764:
        *(u32*)(lbl_803A6A60 + 0x44) = 1;
        break;
    case 0x6E9:
    case 0x75F:
    case 0x762:
    case 0x765:
        *(u32*)(lbl_803A6A60 + 0x44) = 2;
        break;
    }

    raw = lbl_803A6A60[0];
    if ((s8)raw < 0) {
        sel = lbl_803A6A60[0x48];
    } else {
        sel = raw;
        lbl_803A6A60[0x48] = raw;
    }
    selection = (s8)sel;
    switch (selection) {
    case 0:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x6E7) * 0x1C);
        break;
    case 1:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x75D) * 0x1C);
        break;
    case 2:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x760) * 0x1C);
        break;
    case 3:
        layout = (s16*)(lbl_802EF0A8 +
                        (*(u32*)(lbl_803A6A60 + 0x44) + 0x763) * 0x1C);
        break;
    default:
        layout = (s16*)(lbl_802EF0A8 + 0xC144);
        break;
    }

    angles = (f32*)(lbl_803A6A60 + 0xC);
    step = *(f32*)(lbl_803A6A60 + 8) *
           (lbl_8047BDF0 / *(f32*)&periods[*(u32*)(lbl_803A6A60 + 0x44)]);
    angles[*(u32*)(lbl_803A6A60 + 0x44)] += step;
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] > lbl_8047BDF0) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] -= lbl_8047BDF0;
    }
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] < lbl_8047BDAC) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] += lbl_8047BDF0;
    }
    sprite->value = angles[*(u32*)(lbl_803A6A60 + 0x44)];
    sprite->field_50 = layout[1];
    sprite->field_52 = layout[2];
}

static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = GSmemAlloc(size, align);

    if ((u16)h != 0) {
        return (void*)GSmemLock(h);
    }
    return NULL;
}

typedef struct PdaSortEntry {
    s32 key;
    PdaListEntry entry;
} PdaSortEntry;

#pragma peephole off
void pdaSnagListBuild(void)
{
    extern u16 darkPokemonSaveGet4A0(u16 id);
    extern u8 darkPokemonCheckFlag16(u16 id);
    extern u16 darkPokemonGetFlag1C(u16 id);
    u8 temporary[8];
    s32 gap;
    u8* lower;
    s32 j;
    u8* sortEntries;
    s32 i;
    s32 count;
    u32 allocation;
    u32 handle;

    lbl_8047A4DC = 0x60;
    lbl_8047A4D0 = 0;
    lbl_8047A4D0 = (u32)pdaAlloc(0x140, 0x20);
    lbl_8047A4D4 =
        (PdaListEntry*)pdaAlloc(((lbl_8047A4DC * 4) + 0x1F) & ~0x1F, 0x20);
    lbl_8047A4D8 =
        (u32)pdaAlloc(((lbl_8047A4DC * 8) + 0x1F) & ~0x1F, 0x20);

    count = 0;
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        u16 value;

        value = darkPokemonSaveGet4A0(i + 1);
        if (value != 0 &&
            (darkPokemonCheckFlag16(i + 1) != 0 ||
             darkPokemonGetFlag1C(i + 1) != 0)) {
            lbl_8047A4D4[count].field_00 = value;
            lbl_8047A4D4[count].battleId = (u16)(i + 1);
            ((PdaSortEntry*)lbl_8047A4D8)[count].key = darkPokemonSaveGet4A4();
            ((PdaSortEntry*)lbl_8047A4D8)[count].entry.field_00 =
                lbl_8047A4D4[count].field_00;
            ((PdaSortEntry*)lbl_8047A4D8)[count].entry.battleId =
                lbl_8047A4D4[count].battleId;
            count++;
        }
    }
    lbl_8047A4DC = count;

    sortEntries = (u8*)lbl_8047A4D8;
    for (gap = count / 2; gap > 0; gap /= 2) {
        for (i = gap; i < count; i++) {
            j = i - gap;
            lower = sortEntries + j * 8;
            while (j >= 0 &&
                   *(s32*)lower >
                       *(s32*)(sortEntries + (j + gap) * 8)) {
                memcpy(temporary, lower, 8);
                memcpy(lower, sortEntries + (j + gap) * 8, 8);
                memcpy(sortEntries + (j + gap) * 8, temporary, 8);
                lower -= gap * 8;
                j -= gap;
            }
        }
    }
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        lbl_8047A4D4[i].field_00 = ((PdaSortEntry*)lbl_8047A4D8)[i].entry.field_00;
        lbl_8047A4D4[i].battleId = ((PdaSortEntry*)lbl_8047A4D8)[i].entry.battleId;
    }

    handle = GSmemGetHandleByPtr(lbl_8047A4D8);
    if ((u16)handle != 0) {
        GSmemUnlock(handle);
        GSmemFree(handle);
    }
    lbl_8047A4D8 = 0;
}
#pragma peephole reset

#pragma peephole off
/* RULE-EXCEPTION(user-approved): local optimization-level pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 2
u32 pdaMemoListKey(u32 unused, s32 mode, u16 buttons)
{
    u8* scene;
    u16 total;
    s32 selected;
    s32 next;
    s32 difference;

    scene = (u8*)&lbl_803A6818;
    *(s32*)(scene + 0x1C) = 2;
    total = lbl_8047A4E8;
    if ((buttons & 2) && total != 0 &&
        *(s32*)scene < (s32)total - 1) {
        *(s32*)scene += 1;
        soundStartRaw(0x23, 0, 0);
        *(f32*)(scene + 0x38) -= lbl_8047BCF4;
        *(s32*)(scene + 8) += 1;
        *(s32*)(scene + 0xC) += 1;
    } else if ((buttons & 1) && total != 0 &&
               *(s32*)scene > 0) {
        *(s32*)scene -= 1;
        soundStartRaw(0x23, 0, 0);
        *(f32*)(scene + 0x38) += lbl_8047BCF4;
        *(s32*)(scene + 8) -= 1;
        *(s32*)(scene + 0xC) -= 1;
    } else if ((buttons & 8) && mode == 0 && total != 0) {
        selected = *(s32*)scene;
        next = selected + 10;
        *(s32*)scene = next;
        if (next > (s32)total - 1) {
            next = total - 1;
            *(f32*)(scene + 0x38) = -(f32)(next * 0x1B);
            *(s32*)scene = next;
            if (selected != next) {
                difference = next - selected;
                *(s32*)(scene + 8) += difference;
                *(s32*)(scene + 0xC) += difference;
            }
        } else {
            *(f32*)(scene + 0x38) -= lbl_8047BCF8;
            *(s32*)(scene + 8) += 10;
            *(s32*)(scene + 0xC) += 10;
            soundStartRaw(0x23, 0, 0);
        }
    } else if ((buttons & 4) && mode == 0 && total != 0) {
        selected = *(s32*)scene;
        next = selected - 10;
        *(s32*)scene = next;
        if (next < 0) {
            *(s32*)scene = 0;
            *(f32*)(scene + 0x38) = lbl_8047BC94;
            if (selected != 0) {
                *(s32*)(scene + 8) = -5;
                if (total >= 5) {
                    *(s32*)(scene + 0xC) = 5;
                } else {
                    *(s32*)(scene + 0xC) = total;
                }
                *(s32*)(scene + 0x10) = total;
            }
        } else {
            *(f32*)(scene + 0x38) += lbl_8047BCF8;
            *(s32*)(scene + 8) -= 10;
            *(s32*)(scene + 0xC) -= 10;
            soundStartRaw(0x23, 0, 0);
        }
    }
    return 0;
}
#pragma pop
#pragma peephole reset

void itemStorageRun(void)
{
    extern s32 itemStorageSelectAction(s32 value);
    extern void menuClose(s32 id);
    extern void menuOpen(s32 id, s32 flag);
    s32 result;

    result = 0;
    while (1) {
        result = itemStorageSelectAction(result);
        if ((u32)(result - 3) <= 1) {
            break;
        }
        switch (result) {
        case 0:
            menuClose(0x19);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            itemStorageTransfer(0);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        case 1:
            menuClose(0x19);
            menuClose(0x1A);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1A, 1);
            menuCloseSync(0x1B, 1);
            fsysStartLoad(0x66F);
            while (GSresGroupGetState(0x66F) == 1) {
                _threadSwitch();
            }
            bagMenuOpen(4, 0, 0);
            GSresReleaseById(0x66F);
            GSresReleaseOwner(0x66F);
            menuOpen(0x1A, 0);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        case 2:
            menuClose(0x19);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            itemStorageTransfer(1);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        }
    }
}


extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

#pragma peephole off
void pdaSnagListDrawEntry(u8* context)
{
    extern s32 darkPokemonGetFlagListValue(u16, s8*);
    u8 special;
    u32 ownerText;
    u8* slot;
    u16 kind;
    u32 text;
    u32 owner;
    u32 caption;
    u16 index;
    u16 trainerId;

    context[0x8B] = lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44);
    special = 0;
    index = (u16)*(u32*)&lbl_803A6748;
    if (lbl_8047A4D0 != 0) {
        slot = (u8*)&lbl_803A6748 + 0x94;
        kind = lbl_8047A4D4[index].battleId;
        text = darkPokemonGetFlagListValue(kind, (s8*)slot);
        switch (*slot) {
        case 0:
            text = 0x371F;
            break;
        case 1:
        case 2:
            if (kind == 0x43) {
                text = 0x12B0;
            } else if ((trainerId = darkPokemonGetSpecies(kind, 0)) == 9) {
                ownerText = darkPokemonSaveGetName(kind);
                special = 1;
            } else {
                text = fightTrainerDataBiosGetStatus03(fightTrainerDataBiosGetPtr(trainerId));
            }
            break;
        }
        switch (special) {
        case 0:
            ownerText = GSmsgGetGSchar(text);
            break;
        default:
            break;
        }
    } else {
        ownerText = special;
    }
    owner = ownerText;
    if (ownerText == 0) {
        owner = GSmsgGetGSchar(1);
    }
    caption = pdaSnagListGetTrainerName(*(u32*)&lbl_803A6748);
    if (caption == 0) {
        caption = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, caption);
    GSmsgPrintColor(0, 0, context[0x8B] | -0x100, (void*)0xE7);
    msgctrlSetValue(0x37, owner);
    GSmsgPrintColor(0xB4, 0, context[0x8B] | -0x100, (void*)0xE7);
}
#pragma peephole reset
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 itemStorageListSeq(u8* ctx)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);
    extern u8 itemSlotIsValid(void* item);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern u16 itemBiosGetItemDataId(void* item);
    extern void* itemDataBiosGetPtr(s32 id);
    extern s32 itemDataBiosGetDoc(void* data);
    s8 state;
    s32 found;
    s32 i;
    void* item;
    s32 target;
    u16 itemId;
    s32 message;
    s32 slots;
    f32 f0;
    f32 f1;
    f32 f2;

    state = (s8)ctx[1];
    switch (state) {
    case 0:
        if ((s8)ctx[2] == 0) {
            winSeqSetMenu(0x25, 0xb4);
            target = lbl_8047A4A8 + lbl_8047A4AC;
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)itemSlotIsValid(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto haveDoc;
                    }
                }
            }
            itemId = 0;
        haveDoc:
            if (itemId != 0) {
                message = itemDataBiosGetDoc(itemDataBiosGetPtr(itemId));
            } else {
                message = 0x1b68;
            }
            lbl_8047A4B4 = message;
            lbl_8047A4C0 = 0.0f;
            lbl_8047A4BC = 0;
            lbl_8047A4B8 = -1;
            ctx[2] = 1;
        }
        break;
    case 2:
        f2 = lbl_8047A4C0;
        f1 = 0.0f;
        if (f2 > f1) {
            f0 = f2 - lbl_8047BABC;
            lbl_8047A4C0 = f0;
            if (f0 < f1) {
                lbl_8047A4C0 = f1;
            }
        }
        f2 = lbl_8047A4C0;
        f1 = 0.0f;
        if (f2 < f1) {
            f0 = f2 + lbl_8047BABC;
            lbl_8047A4C0 = f0;
            if (f0 > f1) {
                lbl_8047A4C0 = f1;
            }
        }
        break;
    case 3:
        if ((s8)ctx[2] == 0) {
            winSeqSetMenu(0x25, 0xb8);
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop

extern void* pokemonDataBiosGetPtr(u16 id);
extern void* pokemonDataBiosGetName(void* data);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);
extern void* fightTrainerPokemonDataBiosGetPtr(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);

/* Caption for the highlighted battle: the Pokemon's species name, or the
   matching dark Pokemon's nickname from the trainer's party. */
static inline u32 pdaBattleCaption(u16 id)
{
    u16 trainerId;
    void* entry;
    void* nick;
    u32 name;

    trainerId = darkPokemonGetSpecies(id, 0);
    if (trainerId == 9) {
        entry = pokemonDataBiosGetPtr(darkPokemonSaveGet4A0(id));
        if (entry != NULL) {
            name = GSmsgGetGSchar((u32)pokemonDataBiosGetName(entry));
        }
    } else {
        entry = fightTrainerPokemonDataBiosGetPtr(
            (void*)fightTrainerDataBiosGetStatus05(fightTrainerDataBiosGetPtr(trainerId)));
        name = GSmsgGetGSchar(
            (u32)fightTrainerPokemonDataBiosGetNickname(entry));
        for (;;) {
            if (id == fightTrainerPokemonDataBiosGetDarkPokemonFlag(entry)) {
                nick = fightTrainerPokemonDataBiosGetNickname(entry);
                if (nick != NULL) {
                    name = GSmsgGetGSchar((u32)nick);
                }
                break;
            }
            entry = (u8*)entry + 0x50;
        }
    }
    return name;
}

void pdaSnagListDrawCaption(u8* ctx)
{
    u32 name;
    u32 msg;

    ctx[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44));
    name = pdaBattleCaption(lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId);
    msg = name;
    if (name == 0) {
        msg = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, msg);
    GSmsgPrintColor(0, 0, ctx[0x8b] | -0x100, (void*)0xcf);
}

/* pdaMemoSortByName (sort by name) follows pdaMemoDrawData, its only caller. */
void pdaMemoSortByName(void);

void pdaMemoSortPairs(u8* arr, s32 count, s32 dir) {
    u8 tmp[4];

    if (dir == 0) {
        u8* a;
        s32 step;
        u8* b;
        s32 j;
        s32 i;
        s32 gap;

        gap = count / 2;
        while (gap > 0) {
            step = gap * 4;
            for (i = gap; i < count; i++) {
                j = i - gap;
                a = arr + j * 4;
                while (j >= 0 &&
                       *(u16*)(a + 2) > *(u16*)((b = arr + (j + gap) * 4) + 2)) {
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= step;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    } else {
        s32 j2;
        s32 i2;
        s32 gap2;
        u8* b2;
        u8* a2;

        gap2 = count / 2;
        while (gap2 > 0) {
            for (i2 = gap2; i2 < count; i2++) {
                j2 = i2 - gap2;
                a2 = arr + j2 * 4;
                while (j2 >= 0 &&
                       *(u16*)(a2 + 2) < *(u16*)((b2 = arr + (j2 + gap2) * 4) + 2)) {
                    memcpy(tmp, a2, 4);
                    memcpy(a2, b2, 4);
                    memcpy(b2, tmp, 4);
                    a2 -= gap2 * 4;
                    j2 -= gap2;
                }
            }
            gap2 = gap2 / 2;
        }
    }
}

/* Main summary-screen state machine. */
#pragma peephole off
/* fn_80044630's pdaApproach: the upper clamp is the 20.0f literal (as an extern read it colours the clamp's float registers in the other order). */
/* RULE-EXCEPTION(user-approved): reconstructed inline helper pda_80044630_Approach (fn_80044630's copy of pdaApproach) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80044630_Approach(f32* current, f32* target, f32 rate)
{
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = lbl_8047BCC4 * step;
    step = scaled * rate;
    if (step > 20.0f) {
        step = 20.0f;
    }
    if (step <= lbl_8047BCC8) {
        step = lbl_8047BCC8;
    }
    distance = *target - (*current += step);
    if (pdaAbsF(distance) <= pdaAbsF(step) || pdaAbsF(distance) < lbl_8047BCBC) {
        *current = *target;
    }
}

void pdaStrategyMemoRun(void) {
    extern void  _threadSwitch(void);
    extern u32   menuCloseCustom(u32 a, u32 b, u32 c);
    extern s32   menuOpen(s32 menu, s32 mode);
    extern void  GSvecCopy(void* dst, void* src);
    extern void  GSscene_SetMode(u32 mode);
    extern void  GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
    extern const f32 lbl_80267150[];

    extern const f32 lbl_8047BCA8;
    extern const f32 lbl_8047BCCC;
    extern const f32 lbl_8047BCF0;
    extern const f32 lbl_8047BD18;

    extern u32   lbl_8047A4E0;

    extern void  pdaModelViewerSetup(void);
    extern void  pdaStrategyMemoUpdate(void);
    extern void  pdaStrategyMemoSetSelection(s32 a, s32 b);
    extern void* std112_modelGetResourceID(u32 id);
    extern void* GSresGetResource();
    extern void* std093_getCurrentGroupID(void);
    extern void  std113_mapObjectUnk_8018db68(void* h);
    extern void  std114_mapObjectSetVisibility(void* h, s32 b);
    extern void  GSmodelSetVisibility(void* a, s32 b);
    extern void  GSmodelSetAnimRate(void* model, f32 rate);
    extern void  GSmodelSet60fpsAnimFlag(void* h, s32 b);
    extern void  GSmodelSetBoundCheck(void* h, s32 b);
    extern void  GSmodelDestroyLinkedParticles(void* model);
    extern void* wazaSequenceOwnerGetModel(void* h);
    extern void  cameraPlayAnime(u32 a, u32 id, u32 c, u32 d);
    extern void  menuModelTerm(void* subobj);
    extern void  menuOffScreenSetDisp(s32 a);
    extern u32   GSmemGetHandleByPtr(u32 flag);
    extern void  GSmemUnlock(void);
    extern void  GSmemFree(u32 h);
    extern void  GScameraGetPosition(void* cam, void* outVec3);
    extern void  GScameraGetRotation(void* cam, void* outVec3);
    extern void  GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);

    u8*  S;
    u8*  S124;
    u8*  S130;
    u8*  S218;
    u8*  S118;
    const f32* tbl;
    s32  running = 1;
    void* h;
    void* handle;
    u32 mem;

    tbl = lbl_80267150;
    pdaModelViewerSetup();
    S = (u8*)&lbl_803A6818;

    S118 = S + 0x118;
    S124 = S + 0x124;
    S130 = S + 0x130;
    S218 = S + 0x218;

    while (1) {
        switch (*(s32*)(S + 0x1C)) {
        case 0: {   /* .L_800446F0 */
            pdaStrategyMemoSetSelection(-1, 1);
            menuOpen(0x35, 0);
            menuOpen(0x3b, 0);
            menuOpen(0x36, 0);
            menuOpen(0x37, 0);
            menuOpen(0x38, 0);
            menuOpen(0x39, 0);
            *(s32*)(S + 0x1c) = 1;
        } break;

        case 1: {   /* .L_80044750 */
            s32 r;
            r = menuOpen(0x3a, 1);
            *(s32*)(S + 0x28) = 0;
            if (r >= 0) {
                if (*(s32*)(S + 0x18) == 1) {
                    *(s32*)(S + 0x1c) = 7;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                } else {
                    *(s32*)(S + 0x1c) = 6;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                }
            } else {
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                *(s32*)(S + 0x1c) = 0xb;
            }
        } break;

        case 2: {   /* .L_800447CC */
            pdaStrategyMemoUpdate();
        } break;

        case 4: {   /* .L_800447D4 */
            s32 r;
            menuOpen(0x96, 0);
            menuOpen(0xa1, 0);
            r = menuOpen(0xa2, 1);
            if (r >= 0)
                break;
            if (*(s32*)(S + 0x1c) == 4) {
                *(s32*)(S + 0x1c) = 9;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 5: {   /* .L_80044830 */
            s32 r;
            menuOpen(0x96, 0);
            r = menuOpen(0xa2, 1);
            if (r >= 0)
                break;
            if (*(s32*)(S + 0x1c) == 5) {
                *(s32*)(S + 0x1c) = 0xa;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 3: {   /* .L_80044880 */
            s32 r;
            *(u8*)(S + 0x158) = 0;
            *(u8*)(S + 0x159) = 0;
            *(u8*)(S + 0x15b) = 0;
            *(u8*)(S + 0x15a) = 0;
            *(u8*)(S + 0x15c) = 0;
            *(u8*)(S + 0x15d) = 0;
            *(u8*)(S + 0x15f) = 0;
            *(u8*)(S + 0x15e) = 0;
            *(s32*)(S + 0x154) = 0;
            *(f32*)(S + 0x1e0) = lbl_8047BCCC;
            *(f32*)(S + 0x1dc) = lbl_8047BCCC;
            *(f32*)(S + 0x1cc) = lbl_8047BC94;
            *(f32*)(S + 0x1d0) = lbl_8047BC94;
            *(f32*)(S + 0x1d4) = lbl_8047BC94;
            *(f32*)(S + 0x1d8) = lbl_8047BC94;
            *(u8*)(S + 0x164) = 0;
            *(u8*)(S + 0x178) = 0;
            *(u8*)(S + 0x18c) = 0;
            *(u8*)(S + 0x1a0) = 0;
            *(u8*)(S + 0x1b4) = 0;
            *(s32*)(S + 0x1e4) = 0;
            r = menuOpen(0xce, 1);
            if (r < 0) {
                *(s32*)(S + 0x1c) = 0xc;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                menuCloseCustom(0xce, 0, 0);
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 6: {   /* .L_80044928 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nval;
            f32 delta;
            f32 rem;
            f32 mag;

            /* ease block A: unk4C toward unk50 (div BCB8) */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x4C) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x4C) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B: unk54 toward unk58 (div BCB8) */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x54) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x54) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C: unk5C toward unk60 (div BCC0) */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x5C) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x5C) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D: unk1DC toward unk1E0 (signed-delta + clamps) */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x14) = 1;
                menuCloseCustom(0x35, 0, 1);
                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
            {
                    void* cam;
                    f32   persp3;
                    f32   persp2;
                    f32   persp1;
                    f32   persp0;
                    f32   rot[3];
                    f32   pos[3];
                    f32   mtx[3];

                    std113_mapObjectUnk_8018db68(h);

                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;

                    cam = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D181000);
                    GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), cam),
                                       lbl_8047BD18);
                    GSmodelSet60fpsAnimFlag(*(void**)(S + 0x110), 1);
                    GSmodelSetBoundCheck(*(void**)(S + 0x110), 0);

                    ((u32*)mtx)[0] = ((const u32*)tbl)[6];
                    ((u32*)mtx)[1] = ((const u32*)tbl)[7];
                    ((u32*)mtx)[2] = ((const u32*)tbl)[8];
                    ((u32*)pos)[0] = ((const u32*)tbl)[9];
                    ((u32*)pos)[1] = ((const u32*)tbl)[10];
                    ((u32*)pos)[2] = ((const u32*)tbl)[11];
                    cam = GSresGetResource(0x17, 0x0D741800);
                    *(void**)(S + 0x114) = cam;
                    GScameraGetPosition(cam, pos);
                    GScameraGetRotation(cam, rot);
                    mtx[0] = *(f32*)((u8*)cam + 0x100);
                    mtx[1] = *(f32*)((u8*)cam + 0x104);
                    mtx[2] = *(f32*)((u8*)cam + 0x108);
                    GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
                    GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
                    GSvecCopy(S118, pos);
                    GSvecCopy(S124, rot);
                    GSvecCopy(S130, mtx);

                    *(f32*)(S + 0x13C) = persp0;
                    *(f32*)(S + 0x140) = persp1;
                    *(f32*)(S + 0x144) = persp2;
                    *(f32*)(S + 0x148) = persp3;

                    cameraPlayAnime(0x17, 0x0D741800, 0, 1);
                    GSscene_SetMode(4);

                    *(s32*)(S + 0x78) = 1;
                    pdaStrategyMemoSetSelection(-1, 2);

                    {
                        s32 sub = *(s32*)(S + 0x28);
                        void* particles;
                        *(f32*)(S + 0x6C) = lbl_8047BC94;
                        *(f32*)(S + 0x70) = lbl_8047BC94;
                        *(f32*)(S + 0x74) = lbl_8047BC94;
                        *(f32*)(S + 0x64) = lbl_8047BCF0;
                        switch (sub) {
                        case 0:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 2;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            break;
                        case 1:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 4;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            *(f32*)(S + 0x64) = lbl_8047BCA8;
                            break;
                        case 2:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 5;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            break;
                        default:
                            break;
                        }
                    }

                    *(f32*)(S + 0x50) = lbl_8047BCBC;
                    *(f32*)(S + 0x58) = lbl_8047BC94;
                    break;
                }
            } else {
                _threadSwitch();
            }
            break;
        }

        case 8: {   /* .L_80044DA0 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;
            f32 staging3;
            f32 staging2;
            f32 staging1;
            f32 staging0;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x3C, 0, 1);
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);

                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
                std113_mapObjectUnk_8018db68(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);
                GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                   lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = GSresGetResource(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &staging0, &staging1, &staging2, &staging3);

                GSvecCopy(S218, pos);
                GSvecCopy(S218 + 0xC, mtx);
                *(f32*)(S218 + 0x18) = staging0;
                *(f32*)(S218 + 0x1C) = staging1;
                *(f32*)(S218 + 0x20) = staging2;
                *(f32*)(S218 + 0x24) = staging3;
                GScameraSetPerspective(h, staging0, staging1, staging2, staging3);

                GSvecCopy(S118, pos);
                GSvecCopy(S124, rot);
                GSvecCopy(S130, mtx);

                *(f32*)(S + 0x13C) = staging0;
                *(f32*)(S + 0x140) = staging1;
                *(f32*)(S + 0x144) = staging2;
                *(f32*)(S + 0x148) = staging3;

                cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 9: {   /* .L_80045178 */
            f32 step;
            f32 cur;
            f32 tgt;
            f32 px3;
            f32 px2;
            f32 px1;
            f32 px0;
            f32 nv;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {

                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0xa1, 0, 1);
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xa2, 0, 1);

                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }

                /* .L_800453AC camera-setup */
                std113_mapObjectUnk_8018db68(h);
                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);
                GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                   lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = GSresGetResource(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &px0, &px1, &px2, &px3);

                GSvecCopy(S218, pos);
                GSvecCopy(S218 + 0xC, mtx);

                *(f32*)(S218 + 0x18) = px0;
                *(f32*)(S218 + 0x1C) = px1;
                *(f32*)(S218 + 0x20) = px2;
                *(f32*)(S218 + 0x24) = px3;
                GScameraSetPerspective(h, px0, px1, px2, px3);

                GSvecCopy(S118, pos);
                GSvecCopy(S124, rot);
                GSvecCopy(S130, mtx);

                *(f32*)(S + 0x13C) = px0;
                *(f32*)(S + 0x140) = px1;
                *(f32*)(S + 0x144) = px2;
                *(f32*)(S + 0x148) = px3;

                cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
            break;
        }

        case 0xA: {   /* .L_80045550 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 up;
            f32 down;
            f32 delta;
            f32 gstep;
            f32 newcur;
            f32 rem;
            f32 arem;

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x4C) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x4C) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x54) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x54) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x5C) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x5C) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {

                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);
                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
            {
                    f32 p3;
                    f32 p2;
                    f32 p1;
                    f32 p0;
                    f32 rotbuf[3];
                    f32 pos[3];
                    f32 mtx[3];

                    std113_mapObjectUnk_8018db68(h);

                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;

                    h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);

                    GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                       lbl_8047BD18);

                    ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                    ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                    ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                    ((u32*)pos)[0] = ((const u32*)tbl)[3];
                    ((u32*)pos)[1] = ((const u32*)tbl)[4];
                    ((u32*)pos)[2] = ((const u32*)tbl)[5];
                    h = GSresGetResource(0x17, 0x0D731800);
                    *(void**)(S + 0x114) = h;

                    GScameraGetPosition(h, pos);
                    GScameraGetRotation(h, rotbuf);

                    mtx[0] = *(f32*)((u8*)h + 0x100);
                    mtx[1] = *(f32*)((u8*)h + 0x104);
                    mtx[2] = *(f32*)((u8*)h + 0x108);
                    GScameraGetPerspective(h, &p0, &p1, &p2, &p3);

                    GSvecCopy(S218, pos);
                    GSvecCopy(S218 + 0x0C, mtx);

                    *(f32*)(S218 + 0x18) = p0;
                    *(f32*)(S218 + 0x1C) = p1;
                    *(f32*)(S218 + 0x20) = p2;
                    *(f32*)(S218 + 0x24) = p3;
                    GScameraSetPerspective(h, p0, p1, p2, p3);

                    GSvecCopy(S118, pos);
                    GSvecCopy(S124, rotbuf);
                    GSvecCopy(S130, mtx);

                    *(f32*)(S + 0x13C) = p0;
                    *(f32*)(S + 0x140) = p1;
                    *(f32*)(S + 0x144) = p2;
                    *(f32*)(S + 0x148) = p3;
                    cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                    GSscene_SetMode(4);

                    *(s32*)(S + 0x78) = 0;
                    *(s32*)(S + 0x14) = 0;
                    *(f32*)(S + 0x50) = lbl_8047BCBC;
                    *(f32*)(S + 0x58) = lbl_8047BC94;

                    break;
                }
            } else {
                _threadSwitch();
            }
            break;
        }

        case 7: {   /* .L_80045918 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                menuCloseCustom(0x35, 0, 1);
                *(s32*)(S + 0x1C) = 3;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 0xC: {   /* .L_80045B1C */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;

            /* ease A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* converge */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 0xD: {   /* .L_80045D38 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            s32 sub;

            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }
            if (*(f32*)(S + 0x60) == *(f32*)(S + 0x5C)) {
                sub = *(s32*)(S + 0x24);
                switch (sub) {
                case 2:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                case 4:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                case 5:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                default:
                    break;
                }
                *(f32*)(S + 0x60) = lbl_8047BCBC;
            }
        } break;

        case 0xE: {   /* .L_80045DE8 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;

            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }
            if (*(f32*)(S + 0x60) == *(f32*)(S + 0x5C)) {
                *(s32*)(S + 0x1C) = *(s32*)(S + 0x24);
            }
        } break;

        case 0xB: {   /* .L_80045E5C */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;

            /* ease A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0x64;
            } else {
                _threadSwitch();
            }
        } break;

        case 0x64: { /* .L_80046040 */
            menuCloseCustom(0x35, 0, 1);
            running = 0;
        } break;

        default:
            break;
        }
        if (running != 0) {
            continue;
        }
        break;
    }

    /* === EXIT / cleanup epilogue (.L_80046054 fall-through .. blr) === */
    handle = *(void**)(S + 0x10C);
    std114_mapObjectSetVisibility(handle, 0);
    {
        void* rv = GSresGetResource(std093_getCurrentGroupID(), handle);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    std113_mapObjectUnk_8018db68(handle);

        handle = *(void**)((u8*)&lbl_803A6818 + 0x150);
    std114_mapObjectSetVisibility(handle, 0);
    {
        void* rv = GSresGetResource(std093_getCurrentGroupID(), handle);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    std113_mapObjectUnk_8018db68(handle);

    if (lbl_8047A4E0 != 0) {
        mem = GSmemGetHandleByPtr(lbl_8047A4E0);
        if ((u16)mem != 0) {
            GSmemUnlock();
            GSmemFree(mem);
        }
        lbl_8047A4E0 = 0;
    }

    menuOffScreenSetDisp(1);
        menuModelTerm((u8*)&lbl_803A6818 + 0x7c);
        menuModelTerm((u8*)&lbl_803A6818 + 0xc4);

        mem = GSmemGetHandleByPtr((u32)lbl_8047A4E4);
    if ((u16)mem != 0) {
        GSmemUnlock();
        GSmemFree(mem);
    }
    lbl_8047A4E4 = 0;
}
#pragma peephole reset

extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 pokemonGetStatus(u32 a, u32 b, s32 id, s32 index);
extern u16 pokemonBiosGetPokemonDataId(u32 work);
extern void* pokemonDataBiosGetPtr(u16 id);
extern u8 pokemonDataBiosGetColor(void* data, s32 index);
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);

/* Reload the scratch Pokemon slot from a memo entry ID. */
static inline u32 pdaLoadPokemon(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Whether a memo entry ID has been seen (the high bit marks an unseen entry). */
static inline BOOL pdaMemoSeen(u16 id)
{
    return (id & 0x8000) == 0;
}

/* As pdaLoadPokemon, but from an entry ID the caller already has in hand. */
static inline u32 pdaLoadPokemonId(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if ((work = lbl_8047A4E0) != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* pdaLoadPokemonId for the lookup helpers, which go on to read the work slot. */
static inline u32 pdaLoadPokemonWork(u32 id)
{
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;
    u32 work;

    if ((work = lbl_8047A4E0) != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Shiny/color variant of the memo entry, or 0 when it cannot be resolved. */
static inline u8 pdaGetPokemonColor(u16 id)
{
    u32 work = pdaLoadPokemonId(id);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetColor(data, 0);
        }
        return 0;
    }
    return 0;
}

/* People-screen: draw the currently highlighted Pokemon's type icons. */
void pdaPeopleDrawTypeIcons(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u16 lbl_802E554C[];
    s32 pokemon;
    u16 type0;
    u16 type1;
    u8 seen;

    pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);

    alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
    seen = (lbl_8047A4E4[lbl_803A6818.currentIndex] & 0x8000) ? 0 : 1;
    if (seen != 0) {
        type0 = pokemonGetStatus(
            0, (u16)pokemonGetStatus(pokemon, 0, 0x6e, 0), 0x16, 0);
        windowDrawSprite(
            (s16)(*(s16*)(lbl_802EF0A8 + 0x5996) - sprite->field_50),
            (s16)(*(s16*)(lbl_802EF0A8 + 0x5998) - sprite->field_52),
            alphaSprite, lbl_802E554C[type0], 0);
        type1 = pokemonGetStatus(
            0, (u16)pokemonGetStatus(pokemon, 0, 0x6e, 0), 0x16, 1);
        if (type0 != type1) {
            windowDrawSprite(
                (s16)(*(s16*)(lbl_802EF0A8 + 0x59b2) - sprite->field_50),
                (s16)(*(s16*)(lbl_802EF0A8 + 0x59b4) - sprite->field_52),
                alphaSprite, lbl_802E554C[type1], 0);
        }
    } else {
        windowDrawSprite((s16)(*(s16*)(lbl_802EF0A8 + 0x5996) - sprite->field_50),
                         (s16)(*(s16*)(lbl_802EF0A8 + 0x5998) - sprite->field_52),
                         alphaSprite, 0x5d, 0);
        windowDrawSprite((s16)(*(s16*)(lbl_802EF0A8 + 0x59b2) - sprite->field_50),
                         (s16)(*(s16*)(lbl_802EF0A8 + 0x59b4) - sprite->field_52),
                         alphaSprite, 0x5d, 0);
    }
}

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

extern void GSvecCopy(void* dst, void* src);
extern void* GSmodelGetBound(void* model);
extern void ObjInfoInit(void* bound, void* out);
extern void GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);
extern void GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
extern void GScameraSetPosition(void* cam, void* pos);
extern void GScameraSetRotation(void* cam, void* rot);
extern void GScameraLookAt(void* cam, void* a, void* b);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetColor(void* light, void* color);
extern void GSlightSetPosition(void* light, void* pos);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetActive(void* light, s32 active);
extern u8 lbl_802E543C[];
extern u8 lbl_802E5448[];
extern u8 lbl_80267180[];
extern f32 lbl_8047BC9C;
extern u8 pdaModelFrameSingle(u8* work);
extern u8 pdaModelFrameCamera(void* work, void* sub);


/* Bring the PDA model/camera/light rig online for the summary screen. */
static inline u8 fn_8003D1FC_setup(u8* sub)
{
    void* model;
    void* cam;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    PdaVec3 color;
    PdaVec3 lightPos;
    PdaVec3 lightTarget;
    PdaVec3 pos;
    PdaVec3 bound;
    f32 dist;

    if (sub == NULL) {
        return 0;
    }
    model = *(void**)((u8*)&lbl_803A6818 + 0xe8);
    if (model == NULL) {
        *sub = 0;
        return 0;
    }
    GSvecCopy(&pos, (u8*)&lbl_803A6818 + 0x1e8);
    GSvecCopy(lbl_802E5448, (u8*)&lbl_803A6818 + 0x1f4);
    persp0 = *(f32*)((u8*)&lbl_803A6818 + 0x200);
    persp1 = *(f32*)((u8*)&lbl_803A6818 + 0x204);
    persp2 = *(f32*)((u8*)&lbl_803A6818 + 0x208);
    persp3 = *(f32*)((u8*)&lbl_803A6818 + 0x20c);
    dist = *(f32*)((u8*)&lbl_803A6818 + 0x210);
    ObjInfoInit(GSmodelGetBound(model), &bound);
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), &persp0, &persp1, &persp2,
                           &persp3);
    set__5GSvecFfff(&pos, lbl_8047BC94, lbl_8047BC94, dist);
    GScameraSetPosition(*(void**)((u8*)&lbl_803A6818 + 0xfc), &pos);
    GScameraSetPerspective(*(void**)((u8*)&lbl_803A6818 + 0xfc), persp0, persp1, persp2,
                           persp3);
    GScameraLookAt(*(void**)((u8*)&lbl_803A6818 + 0xfc), lbl_802E543C, lbl_802E5448);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &pos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)((u8*)&lbl_803A6818 + 0x108), 2);
    GSlightSetColor(*(void**)((u8*)&lbl_803A6818 + 0x108), &color);
    GSlightSetPosition(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightPos);
    GSlightSetTarget(*(void**)((u8*)&lbl_803A6818 + 0x108), &lightTarget);
    GSlightSetActive(*(void**)((u8*)&lbl_803A6818 + 0x108), 1);
    cam = *(void**)((u8*)&lbl_803A6818 + 0x114);
    if (cam != NULL) {
        GScameraSetPosition(cam, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(cam, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(cam, *(f32*)((u8*)&lbl_803A6818 + 0x13c), *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144), *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}

s32 pdaModelViewSetup(void)
{
    switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 8:
    case 0xb:
    case 0xc:
        *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
        break;
    case 5:
        *((u8*)&lbl_803A6818 + 0x214) =
            pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        *((u8*)&lbl_803A6818 + 0x214) =
            fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }
    return 0;
}

/* Color variant of the memo entry; bails out before loading when no work slot exists. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Color (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_8003D4C8_Color(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetColor(data, 0);
            }
        } else {
            return 0;
        }
    }
    return 0;
}

/* pdaLoadPokemonId for the colour filter: tests the work slot, then reads it again into work. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Load (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003D4C8_Load(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if (lbl_8047A4E0 != 0) {
        work = lbl_8047A4E0;
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Copy the seen entries whose colour matches the active colour tab into buf; returns the count. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Filter (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003D4C8_Filter(u16* buf)
{
    u16 count;
    u16 i;
    u32 id;
    u8 color;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            pda_8003D4C8_Load(id);
            color = pda_8003D4C8_Color(id);
            if (color == *(s8*)((u8*)&lbl_803A6818 + 0x159)) {
                buf[count] = id;
                count++;
            }
        }
    }
    return count;
}

/* Rebuild the memo index table, optionally filtered by the active color tab. */
#pragma peephole off
u16 pdaMemoLoadPokemon(void)
{
    extern void pdaMemoBuildNameIndex(void);
    extern void pdaMemoBuildTypeIndex(void);
    extern void pdaMemoDrawData(void);
    u16 total;
    u16 buf[500];
    u16 i;
    u16 count;

    lbl_8047A4E8 = pda_8003D818_LoadIds();
    pdaMemoBuildNameIndex();
    if (*(s8*)((u8*)&lbl_803A6818 + 0x159) != 0) {
        count = pda_8003D4C8_Filter(buf);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf[i];
        }
        lbl_8047A4E8 = count;
    }
    pdaMemoBuildTypeIndex();
    pdaMemoDrawData();
    total = lbl_8047A4E8;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    return total;
}
#pragma peephole reset

typedef struct PdaNameEntry {
    u8 used;
    u8 pad01;
    u16 text[8];
} PdaNameEntry;

typedef struct PdaKanaGroup {
    u32 words[15];
    s32 count;
} PdaKanaGroup;

extern PdaKanaGroup* lbl_802E60B0[];
extern void GScharMakeFromSJIS(u16* dst, u32 sjis);
extern void* pokemonDataBiosGetName(void* data);

/* Name of the memo entry as a wide string, or NULL when unavailable. */
static inline void* pdaGetPokemonName(u16 id)
{
    u32 work = pdaLoadPokemonWork(id);

    if (work != 0) {
        return pokemonDataBiosGetName(
            pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work)));
    }
    return NULL;
}

/* Mark the row's entries used and convert their first characters. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D8CC_Fill (pdaMemoBuildNameIndex) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003D8CC_Fill(PdaNameEntry* tbl, PdaKanaGroup* group)
{
    s32 n;

    for (n = 0; n < group->count; n++) {
        tbl[n].used = 1;
        GScharMakeFromSJIS(tbl[n].text, group->words[n]);
    }
}

/* Clear the kana-row table and fill it from tab's row. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D8CC_KanaSetup (pdaMemoBuildNameIndex) - see docs/RULE_EXCEPTIONS.md */
static inline PdaKanaGroup* pda_8003D8CC_KanaSetup(PdaNameEntry* tbl, s32 tab)
{
    PdaKanaGroup* group;
    s32 n;

    for (n = 0; n < 15; n++) {
        tbl[n].used = 0;
    }
    if (tab != 0) {
        group = &lbl_802E60B0[0][tab - 1];
        pda_8003D8CC_Fill(tbl, group);
    }
    return group;
}

/* Filter the memo index table down to the active kana-row tab. */
#pragma peephole off
void pdaMemoBuildNameIndex(void)
{
    u16 nameBuf[0x80];
    PdaNameEntry tbl[15];
    u16 out[500];
    PdaKanaGroup* group;
    u16 j;
    u16 total;
    u16 i;
    u16 id;
    u16 count;

    if (*(s8*)((u8*)&lbl_803A6818 + 0x158) == 0) {
        return;
    }
    group = pda_8003D8CC_KanaSetup(tbl, *(s8*)((u8*)&lbl_803A6818 + 0x158));
    count = group->count;
    total = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        pdaLoadPokemonId(id = lbl_8047A4E4[i]);
        if ((u32)GSmsgGetText((u8*)nameBuf, 0x100, (u8*)pdaGetPokemonName(id)) !=
            0) {
            for (j = 0; j < count; j++) {
                if (nameBuf[0] == tbl[j].text[0]) {
                    out[total++] = id;
                    break;
                }
            }
        }
    }
    for (i = 0; i < total; i++) {
        lbl_8047A4E4[i] = out[i];
    }
    lbl_8047A4E8 = total;
}
#pragma peephole reset

extern f32 lbl_8047BCFC;
extern f32 lbl_8047BD00;
extern f32 lbl_8047BD04;
extern f32 lbl_8047BD08;
extern u8 lbl_804788C4;

static inline f32 pdaFabs(f32 v)
{
    if (v > lbl_8047BC94) {
        return v;
    }
    return -v;
}

/* fn_800439BC's pdaFabs: retail reloads the 0.0f literal at every compare. */
static inline f32 pda_800439BC_Fabs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

/* Sample the stick and buttons into the PDA's own key-repeat state. */
#pragma peephole off
void pdaReadPadInput(void* scene)
{
    extern s8 GSinputGetLeftStickYData(s32 chan, s32 index);
    extern s8 GSinputGetLeftStickXData(s32 chan, s32 index);
    extern u32 GSinputGetButtons(s32 chan);
    extern s8 GSgetFrameStep(void);
    extern f64 atan2(f64 y, f64 x);
    s8 x;
    s8 y;
    f32 ang;
    s32 i;
    u16 buttons;
    u16 held;
    u16 trig;
    u32 raw;
    u8* work = (u8*)scene;

    buttons = 0;
    *(u16*)(work + 2) = *(u16*)work;
    x = GSinputGetLeftStickYData(1, 0);
    y = GSinputGetLeftStickXData(1, 0);
    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
        ang = atan2((f64)y, (f64)x);
        if (pda_800439BC_Fabs(ang) < lbl_8047BCFC) {
            buttons |= 2;
        } else if (pda_800439BC_Fabs(ang) > lbl_8047BD00) {
            buttons |= 1;
        }
        if (lbl_8047BD04 < pda_800439BC_Fabs(ang) && pda_800439BC_Fabs(ang) < lbl_8047BD08) {
            if (ang < 0.0f) {
                buttons |= 4;
            } else {
                buttons |= 8;
            }
        }
    }
    raw = GSinputGetButtons(1);
    if ((raw & 0x8) != 0) {
        buttons |= 1;
    }
    if ((raw & 0x4) != 0) {
        buttons |= 2;
    }
    if ((raw & 0x1) != 0) {
        buttons |= 4;
    }
    if ((raw & 0x2) != 0) {
        buttons |= 8;
    }
    if ((raw & 0x100) != 0) {
        buttons |= 0x10;
    }
    if ((raw & 0x200) != 0) {
        buttons |= 0x20;
    }
    if ((raw & 0x400) != 0) {
        buttons |= 0x40;
    }
    if ((raw & 0x800) != 0) {
        buttons |= 0x80;
    }
    if ((raw & 0x10) != 0) {
        buttons |= 0x100;
    }
    if ((raw & 0x40) != 0) {
        buttons |= 0x200;
    }
    if ((raw & 0x20) != 0) {
        buttons |= 0x400;
    }
    if ((raw & 0x1000) != 0) {
        buttons |= 0x800;
    }
    held = (*(u16*)(work + 2) ^ 0xffff) & buttons;
    trig = 0;
    for (i = 0; i < 16; i++) {
        u16 mask = 1 << i;
        if ((held & mask) != 0) {
            work[i + 0xa] = 15;
            trig |= mask;
        } else if ((buttons & mask) != 0) {
            work[i + 0xa] -= GSgetFrameStep();
            if ((s8)work[i + 0xa] <= 0) {
                work[i + 0xa] = lbl_804788C4;
                trig |= mask;
            }
        }
    }
    *(u16*)work = buttons;
    *(u16*)(work + 4) = held;
    *(u16*)(work + 6) = trig;
}
#pragma peephole reset

extern f32 lbl_8047BCA4;
extern f32 lbl_8047BCA8;
extern f32 lbl_8047BD10;
extern f32 lbl_8047BD14;
extern u32 GSmsgGetRect(s32 id);

/* Tab captions: pulse the active page's title and centre each caption. */
#pragma peephole off
void pdaModelViewDrawText(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    f32 v;
    s32 tint;

    alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
    switch (sprite->eventId) {
    case 0x6d7:
        alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
        v = lbl_8047BD14 *
                (f32)sin(lbl_8047BCA4 * (lbl_8047BCA8 * lbl_803A6818.angle)) +
            lbl_8047BD10;
        if (v > lbl_8047BCA0) {
            v = lbl_8047BCA0;
        }
        if (v < lbl_8047BC94) {
            v = lbl_8047BC94;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x6d8:
        alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
        v = lbl_8047BD14 *
                (f32)sin(lbl_8047BCA4 * (lbl_8047BCA8 * lbl_803A6818.angle)) +
            lbl_8047BD10;
        if (v > lbl_8047BCA0) {
            v = lbl_8047BCA0;
        }
        if (v < lbl_8047BC94) {
            v = lbl_8047BC94;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x6d9:
        alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
        v = lbl_8047BD14 *
                (f32)sin(lbl_8047BCA4 * (lbl_8047BCA8 * lbl_803A6818.angle)) +
            lbl_8047BD10;
        if (v > lbl_8047BCA0) {
            v = lbl_8047BCA0;
        }
        if (v < lbl_8047BC94) {
            v = lbl_8047BC94;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x10c7:
        if (lbl_803A6818.field_28 == 0) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3717) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3717);
        break;
    case 0x10c8:
        if (lbl_803A6818.field_28 == 1) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3718) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3718);
        break;
    case 0x10c9:
        if (lbl_803A6818.field_28 == 2) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3719) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3719);
        break;
    case 0xfbf:
        break;
    default:
        break;
    }
}
#pragma peephole reset

extern f32 lbl_8047BD18;
extern f32 lbl_8047BD30;
extern f32 lbl_8047BD34;
extern u8 lbl_802E5424[];
extern u8 lbl_802E5430[];
extern void* wazaSequenceOwnerGetModel(void* h);
extern s32 wazaSequenceOwnerGetSizeClass(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void GSmtxIdentity(f32* mtx);
extern void GSmtxTransApply(f32* mtx, void* vec);

/* Frame the two compared models in the PDA's side-by-side camera. */
#pragma peephole off
u8 pdaModelFrameCamera(void* work, void* sub)
{
    extern f64 tan(f64 x);
    f32 mtx0[12];
    f32 mtx1[12];
    PdaVec3 pos0;
    PdaVec3 camPos;
    PdaVec3 pos1;
    PdaVec3 bound0;
    PdaVec3 bound1;
    PdaVec3 lightPos;
    PdaVec3 color;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    void* model0;
    void* model1;
    void* cam;
    f32 height;
    f32 width;
    f32 dist;
    f32 fovy;
    f32 aspect;
    f32 znear;
    f32 zfar;

    if (work == NULL) {
        return 0;
    }
    if (sub == NULL) {
        return 0;
    }
    if (*(u8*)((u8*)work + 0x14) != 0) {
        model0 = wazaSequenceOwnerGetModel(*(void**)((u8*)work + 0x24));
        if (model0 == NULL) {
            return 0;
        }
        wazaSequenceOwnerGetSizeClass(*(void**)((u8*)work + 0x24));
    } else {
        model0 = *(void**)((u8*)work + 0x24);
    }
    if (sub != NULL) {
        model1 = *(void**)((u8*)sub + 0x24);
    }
    if (model0 == NULL) {
        *(u8*)work = 0;
        return 0;
    }
    if (model1 == NULL) {
        return 0;
    }
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)((u8*)work + 0x38), &persp0, &persp1,
                           &persp2, &persp3);
    ObjInfoInit(GSmodelGetBound(model0), &bound0);
    ObjInfoInit(GSmodelGetBound(model1), &bound1);
    width = bound0.x + bound1.x;
    height = (bound0.y >= bound1.y) ? bound0.y : bound1.y;
    {
        f32 ratio = (f32)*(s32*)((u8*)work + 0x2c) / (f32)*(s32*)((u8*)work + 0x30);
        persp0 = lbl_8047BD30;
        persp1 = ratio;
    }
    dist = (height >= width) ? height : width;
    dist = dist / (f32)tan(lbl_8047BD34);
    set__5GSvecFfff(&camPos, lbl_8047BC94, lbl_8047BC94, dist);
    GSvecCopy((u8*)&lbl_803A6818 + 0x1e8, &camPos);
    GSvecCopy((u8*)&lbl_803A6818 + 0x1f4, lbl_802E5430);
    fovy = persp0;
    aspect = persp1;
    znear = persp2;
    zfar = persp3;
    *(f32*)((u8*)&lbl_803A6818 + 0x200) = fovy;
    *(f32*)((u8*)&lbl_803A6818 + 0x204) = aspect;
    *(f32*)((u8*)&lbl_803A6818 + 0x208) = znear;
    *(f32*)((u8*)&lbl_803A6818 + 0x20c) = zfar;
    *(f32*)((u8*)&lbl_803A6818 + 0x210) = dist;
    GScameraSetPerspective(*(void**)((u8*)work + 0x38), fovy, aspect, znear,
                           zfar);
    GScameraSetPosition(*(void**)((u8*)work + 0x38), &camPos);
    *(f32*)(lbl_802E5430 + 4) = height * lbl_8047BD18;
    GScameraLookAt(*(void**)((u8*)work + 0x38), lbl_802E5424, lbl_802E5430);
    GSmodelGetPosition(model0, &pos0);
    pos0.x = -(bound0.x * lbl_8047BD18);
    pos0.y = lbl_8047BC94;
    pos0.z = lbl_8047BC94;
    GSmodelSetPosition(model0, &pos0);
    GSmtxIdentity(mtx0);
    GSmtxTransApply(mtx0, &pos0);
    GSmodelSetMatrix(model0, mtx0);
    GSmodelGetPosition(model1, &pos1);
    pos1.x = bound1.x * lbl_8047BD18;
    pos1.y = lbl_8047BC94;
    pos1.z = lbl_8047BC94;
    GSmodelSetPosition(model1, &pos1);
    GSmtxIdentity(mtx1);
    GSmtxTransApply(mtx1, &pos1);
    GSmodelSetMatrix(model1, mtx1);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &camPos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)((u8*)work + 0x44), 2);
    GSlightSetColor(*(void**)((u8*)work + 0x44), &color);
    GSlightSetPosition(*(void**)((u8*)work + 0x44), &lightPos);
    GSlightSetTarget(*(void**)((u8*)work + 0x44), &pos0);
    GSlightSetActive(*(void**)((u8*)work + 0x44), 1);
    cam = *(void**)((u8*)&lbl_803A6818 + 0x114);
    if (cam != NULL) {
        GScameraSetPosition(cam, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(cam, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(cam, *(f32*)((u8*)&lbl_803A6818 + 0x13c),
                               *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144),
                               *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}



#pragma peephole reset

extern f64 __frsqrte(f64 value);
extern f32 lbl_80478AC0[];
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BD38;
extern f32 lbl_8047BD3C;
extern f32 lbl_8047BD40;
extern f32 lbl_8047BD44;
extern f32 lbl_8047BD48;
extern f32 lbl_8047BD4C;
extern volatile f32 lbl_8047BD68;
extern const f64 lbl_8047BD50;
extern const f64 lbl_8047BD58;
extern u8 lbl_802E540C[];
extern u8 lbl_802E5418[];
extern void memoGetScaleAngle(u16 id, f32* scale, f32* angle);
extern u8 menuModelCheck(void* work, s32 index);
extern void GSmodelCenterNull(void* model);
extern void modelRemoveCenterNull(void* model);
extern s32 GSmodelWalkJObjs(void* model);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartGetTransform(void* part, void* out, s32 a, s32 b);
extern void GSpartFree(void* part);
extern void GSmtxRotateY(f32* mtx, f32 angle);

/* MSL math.h inlines: float classification and the frsqrte-based sqrtf. */
static inline s32 pdaFpClassifyF(f32 value)
{
    switch (*(s32*)&value & 0x7F800000) {
    case 0x7F800000: {
        if (*(s32*)&value & 0x007FFFFF) {
            return 1;
        } else {
            return 2;
        }
        break;
    }
    case 0: {
        if (*(s32*)&value & 0x007FFFFF) {
            return 5;
        } else {
            return 3;
        }
        break;
    }
    }
    return 4;
}

extern f64 lbl_8047BD60;

static inline f32 pdaSqrtf(f32 x)
{
    if (x > lbl_8047BC94) {
        f64 xd = x;
        f64 guess = __frsqrte(xd);
        f64 half = lbl_8047BD50;
        f64 three = lbl_8047BD58;
        guess = half * guess * (three - guess * guess * xd);
        guess = half * guess * (three - guess * guess * xd);
        guess = half * guess * (three - guess * guess * xd);
        return (f32) (xd * guess);
    } else if (x < lbl_8047BD60) {
        return lbl_80478AC0[0];
    } else if (pdaFpClassifyF(x) == 1) {
        return lbl_80478AC0[0];
    }
    return x;
}

/* Frame the highlighted Pokemon's model for the single-model PDA page. */
u8 pdaModelFrameSingle(u8* work)
{
    extern f64 tan(f64 x);
    extern f64 atan(f64 x);
    extern f64 sin(f64 x);
    f32 mtx[12];
    PdaVec3 bound;
    PdaVec3 target;
    PdaVec3 camPos;
    PdaVec3 xform;
    PdaVec3 modelPos;
    PdaVec3 lightPos;
    PdaVec3 color;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    f32 angle;
    f32 scale;
    void* model;
    void* part;
    f32 spread;
    f32 root;
    register f32 radians;
    register f32 halfAngle;
    register f32 fov;
    register f32 scaledFov;
    f32 zoom;
    f32 pitch;

    zoom = lbl_8047BCC0;
    angle = lbl_8047BD38;
    scale = lbl_8047BCBC;
    memoGetScaleAngle(
        pokemonBiosGetPokemonDataId(pdaLoadPokemon(lbl_803A6818.currentIndex)),
        &scale, &angle);
    zoom = zoom * scale;
    if (work == NULL) {
        return 0;
    }
    if (*(void**)(work + 0x34) == NULL) {
        return 0;
    }
    if (menuModelCheck(work, 0) == 1) {
        return 0;
    }
    if (work[0x14] != 0) {
        model = wazaSequenceOwnerGetModel(*(void**)(work + 0x24));
        if (model == NULL) {
            return 0;
        }
        switch (wazaSequenceOwnerGetSizeClass(*(void**)(work + 0x24))) {
        case -2:
            spread = lbl_8047BD3C;
            break;
        case -1:
            spread = lbl_8047BD40;
            break;
        case 0:
            spread = lbl_8047BD44;
            break;
        case 1:
            spread = lbl_8047BD44;
            break;
        case 2:
            spread = lbl_8047BD48;
            break;
        case 3:
            spread = lbl_8047BD4C;
            break;
        }
    } else {
        model = *(void**)(work + 0x24);
    }
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)(work + 0x38), &persp0, &persp1, &persp2,
                           &persp3);
    ObjInfoInit(GSmodelGetBound(model), &bound);
    {
        f32 temp_persp1;
        temp_persp1 = (f32)*(s32*)(work + 0x2c) / (f32)*(s32*)(work + 0x30);
        persp0 = lbl_8047BD30;
        persp1 = temp_persp1;
    }
    GSmodelGetPosition(model, &modelPos);
    root = pdaSqrtf(bound.y * bound.y + bound.x * bound.x);
    radians = lbl_8047BD68;
    halfAngle = lbl_8047BD18;
    fov = persp0;
    scaledFov = radians * fov;
    spread = zoom * (root / spread) /
             (f32)tan(scaledFov * halfAngle);
    spread = spread * *(f32*)((u8*)&lbl_803A6818 + 0x68);
    GScameraSetPerspective(*(void**)(work + 0x38), persp0, persp1, persp2,
                           persp3);
    {
        f32* v = (f32*)((u8*)&lbl_803A6818 + 0x218);
        pitch = (f32)atan(
            pdaSqrtf((v[4] - v[1]) * (v[4] - v[1])) /
            pdaSqrtf((v[3] - v[0]) * (v[3] - v[0]) +
                     (v[5] - v[2]) * (v[5] - v[2])));
    }
    GSmodelGetPosition(model, &modelPos);
    target.x = lbl_8047BC94;
    target.z = lbl_8047BC94;
    target.y = lbl_8047BC94;
    GSmtxIdentity(mtx);
    GSmtxTransApply(mtx, &target);
    GSmtxRotateY(mtx, angle);
    GSmodelSetMatrix(model, mtx);
    GSmodelCenterNull(model);
    part = GSmodelGetPart(model, GSmodelWalkJObjs(model) - 1);
    if (part != NULL) {
        GSpartGetTransform(part, &xform, 0, 0);
        GSpartFree(part);
    }
    modelRemoveCenterNull(model);
    {
        f32 sin_val = (f32)sin(pitch);
        f32 term = spread * sin_val;
        set__5GSvecFfff(&camPos, lbl_8047BC94,
                        term + (lbl_8047BC94 + xform.y), spread);
    }
    GScameraSetPosition(*(void**)(work + 0x38), &camPos);
    *(f32*)(lbl_802E5418 + 0) = lbl_8047BC94;
    *(f32*)(lbl_802E5418 + 4) = xform.y;
    *(f32*)(lbl_802E5418 + 8) = xform.z;
    GScameraLookAt(*(void**)(work + 0x38), lbl_802E540C, lbl_802E5418);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &camPos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)(work + 0x44), 2);
    GSlightSetColor(*(void**)(work + 0x44), &color);
    GSlightSetPosition(*(void**)(work + 0x44), &lightPos);
    GSlightSetTarget(*(void**)(work + 0x44), &target);
    GSlightSetActive(*(void**)(work + 0x44), 1);
    work = *(u8**)((u8*)&lbl_803A6818 + 0x114);
    if (work != NULL) {
        GScameraSetPosition(work, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(work, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(work, *(f32*)((u8*)&lbl_803A6818 + 0x13c),
                               *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144),
                               *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}


extern f32 lbl_8047BD20;
extern f32 lbl_8047BD24;
extern f32 lbl_8047BD28;
extern f32 lbl_8047BD2C;
extern void* GSsplineCreate(s32 a, s32 b, s32 count);
extern void GSsplineAddControlVectorValue(void* spline, void* vec, f32 t);
extern void GSsplineFree(void* spline);

/* Draw one memo stat polyline through a spline fitted to its byte samples. */
#pragma peephole off
void pdaDrawSpline(PdaOrbitPoint* point)
{
    extern u8 lbl_80314E08[];
    extern void gfxSetLineWidth(f32 width);
    extern void gfxVtxColor1u32(s32 index, u32 color);
    u8* p = (u8*)point;
    u8* data;
    void* spline;
    PdaVec3 ctrl;
    PdaVec3 v0;
    PdaVec3 v1;
    u32 count;
    s32 i;
    s32 off;
    f32 step;
    f32 t;
    f32 dx;
    f32 zero;

    data = *(u8**)p;
    if (data == NULL) {
        return;
    }
    count = (u8)(data[0] + 2);
    *(f32*)(p + 4) = (f32)count;
    if (*(s32*)(p + 0x18) < (s32)count) {
        *(s32*)(p + 0x18) = *(s32*)(p + 0x18) + 1;
    } else if (*(s32*)(p + 0x10) < (s32)count) {
        *(s32*)(p + 0x10) = *(s32*)(p + 0x10) + 1;
    } else if (*(s32*)(p + 0x10) < (s32)count + 30) {
        *(s32*)(p + 0x10) = *(s32*)(p + 0x10) + 1;
        if (*(f32*)(p + 0x14) < (f32)count) {
            *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
        }
    } else {
        if (*(f32*)(p + 0xc) < (f32)count) {
            *(f32*)(p + 0xc) = *(f32*)(p + 0xc) + *(f32*)(p + 0x20);
            if (*(f32*)(p + 0x14) < (f32)count) {
                *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
            }
        } else {
            if (*(f32*)(p + 0x14) < (f32)count) {
                *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
            }
        }
    }
    spline = GSsplineCreate(3, 1, (u8)count);
    if (spline == NULL) {
        return;
    }
    step = lbl_8047BCBC / (f32)count;
    ctrl.x = lbl_8047BC94;
    ctrl.y = lbl_8047BC94;
    ctrl.z = *(f32*)(p + 0x1c);
    dx = *(f32*)(p + 4);
    GSsplineAddControlVectorValue(spline, &ctrl, lbl_8047BC94);
    dx = lbl_8047BD20 / dx;
    t = step;
    ctrl.x = ctrl.x + dx;
    for (i = 0; i < (s32)count - 2; i++) {
        ctrl.y = (f32)(s8)data[i + 1];
        ctrl.y = -ctrl.y;
        GSsplineAddControlVectorValue(spline, &ctrl, t);
        t = t + step;
        ctrl.x = ctrl.x + dx;
    }
    ctrl.y = lbl_8047BC94;
    ctrl.x = lbl_8047BD20;
    GSsplineAddControlVectorValue(spline, &ctrl, lbl_8047BCBC);
    off = 0;
    zero = lbl_8047BC94;
    for (i = 0; i < (s32)*(f32*)(p + 0xc); i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        v0.z = zero;
        GSvecCopy((u8*)*(void**)((u8*)spline + 0xc) + off, &v0);
        off += 0xc;
    }
    off = (s32)*(f32*)(p + 0xc) * 0xc;
    for (i = (s32)*(f32*)(p + 0xc); i < (s32)*(f32*)(p + 0x14); i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        v0.z = *(f32*)(p + 0x1c) /
               (f32)((s32)*(f32*)(p + 0x14) - (s32)*(f32*)(p + 0xc)) *
               (f32)(i - (s32)*(f32*)(p + 0xc));
        GSvecCopy((u8*)*(void**)((u8*)spline + 0xc) + off, &v0);
        off += 0xc;
    }
    off = 0;
    for (i = 0; i < *(s32*)(p + 0x18) - 1; i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        GSvecCopy(&v1, (u8*)*(void**)((u8*)spline + 0xc) + (i + 1) * 0xc);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetVtxDesc(lbl_80314E08);
        gfxSetPrimitive(1);
        gfxSetLineWidth(lbl_8047BCA8);
        gfxBegin(2);
        gfxVtxPos2s16((s32)(lbl_8047BD24 + v0.x),
                    (s32)(lbl_8047BD2C * v0.y + lbl_8047BD28));
        gfxVtxColor1u32(0, (u8)(s32)v0.z | 0x361e0500);
        gfxVtxPos2s16((s32)(lbl_8047BD24 + v1.x),
                    (s32)(lbl_8047BD2C * v1.y + lbl_8047BD28));
        gfxVtxColor1u32(0, (u8)(s32)v1.z | 0x361e0500);
        gfxEnd();
        gfxSetLineWidth(lbl_8047BCBC);
        off += 0xc;
    }
    GSsplineFree(spline);
}
#pragma peephole reset

extern u8 lbl_802E53F4[];
extern u8 lbl_802E5400[];
extern void GSmtxMakeYRotation(f32* mtx, f32 angle);
extern void GSmtxRotateX(f32* mtx, f32 angle);
extern void GSmtxMakeTranslate(f32* mtx, void* vec);
extern void GSvecTransform(void* dst, f32* mtx, void* src);

/* Orbit camera for the PDA's rotatable model page. */
u8 pdaModelOrbitCamera(u8* work)
{
    extern f64 tan(f64 x);
    f32 mtx1[12];
    f32 mtx0[12];
    PdaVec3 bound;
    PdaVec3 target;
    PdaVec3 camPos;
    PdaVec3 lightPos;
    PdaVec3 color;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    f32 scale;
    void* model;
    f32* box;
    f32 spread;
    f32 zoom;
    f32 extent;
    f32 boundX;
    f32 boundY;
    f32 dist;
    register f32 zero;
    register f32 half;
    register f32 vertical;

    zoom = lbl_8047BCC0;
    scale = lbl_8047BCBC;
    memoGetScaleAngle(
        pokemonBiosGetPokemonDataId(pdaLoadPokemon(lbl_803A6818.currentIndex)),
        &scale, NULL);
    zoom = zoom * scale;
    if (work == NULL) {
        return 0;
    }
    if (*(void**)(work + 0x34) == NULL) {
        return 0;
    }
    if (work[0] != 2) {
        return 0;
    }
    if (menuModelCheck(work, 0) == 1) {
        return 0;
    }
    if (work[0x14] != 0) {
        model = wazaSequenceOwnerGetModel(*(void**)(work + 0x24));
        switch (wazaSequenceOwnerGetSizeClass(*(void**)(work + 0x24))) {
        case -2:
            spread = lbl_8047BD3C;
            break;
        case -1:
            spread = lbl_8047BD40;
            break;
        case 0:
            spread = lbl_8047BD44;
            break;
        case 1:
            spread = lbl_8047BD44;
            break;
        case 2:
            spread = lbl_8047BD48;
            break;
        case 3:
            spread = lbl_8047BD4C;
            break;
        }
    } else {
        model = *(void**)(work + 0x24);
    }
    if (model == NULL) {
        return 0;
    }
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)(work + 0x38), &persp0, &persp1, &persp2,
                           &persp3);
    box = GSmodelGetBound(model);
    ObjInfoInit(box, &bound);
    {
        f32 ratio = (f32)*(s32*)(work + 0x2c) / (f32)*(s32*)(work + 0x30);
        persp0 = lbl_8047BD30;
        persp1 = ratio;
    }
    boundX = bound.x;
    boundY = bound.y;
    if (boundY >= boundX) {
        extent = boundY;
    } else {
        extent = boundX;
    }
    if (lbl_8047BCBC == scale) {
        dist = extent * zoom / (f32)tan(lbl_8047BD34);
    } else {
        dist = zoom * (extent / spread) / (f32)tan(lbl_8047BD34);
    }
    set__5GSvecFfff(&camPos, lbl_8047BC94, lbl_8047BC94,
                    dist * *(f32*)((u8*)&lbl_803A6818 + 0x64));
    GSmtxMakeYRotation(mtx1, -*(f32*)((u8*)&lbl_803A6818 + 0x70));
    GSmtxRotateX(mtx1, -*(f32*)((u8*)&lbl_803A6818 + 0x6c));
    GSvecTransform(&camPos, mtx1, &camPos);
    GScameraSetPosition(*(void**)(work + 0x38), &camPos);
    GScameraSetPerspective(*(void**)(work + 0x38), persp0, persp1, persp2,
                           persp3);
    zero = lbl_8047BC94;
    half = lbl_8047BD18;
    target.x = zero;
    target.z = zero;
    vertical = box[5] + box[8];
    vertical = -vertical;
    target.y = vertical * half;
    GSmtxIdentity(mtx0);
    GSmtxMakeTranslate(mtx0, &target);
    GSmodelSetMatrix(model, mtx0);
    GScameraLookAt(*(void**)(work + 0x38), lbl_802E53F4, lbl_802E5400);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &camPos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)(work + 0x44), 2);
    GSlightSetColor(*(void**)(work + 0x44), &color);
    GSlightSetPosition(*(void**)(work + 0x44), &lightPos);
    GSlightSetTarget(*(void**)(work + 0x44), &target);
    GSlightSetActive(*(void**)(work + 0x44), 1);
    GSscene_SetMode(4);
    return 1;
}

/* pdaLoadPokemonId for the type filter: tests the work slot, then reads it again into work. */
static inline u32 pdaLoadPokemonSlot(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if (lbl_8047A4E0 != 0) {
        work = lbl_8047A4E0;
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Icon index of one of a memo entry's two types. */
static inline u16 pdaGetTypeIcon(u32 id, s32 slot)
{
    u32 type = (u16)pokemonGetStatus(
        0, (u16)pokemonGetStatus(pdaLoadPokemonSlot(id), 0, 0x6e, 0), 0x16, slot);

    if (type >= 9) {
        type = type - 1;
    }
    return type + 1;
}

/* Copy the seen entries matching the type tabs into buf; returns the count.
 * With both set an entry needs both types, otherwise either one. */
static inline u16 pdaFilterTypes(u16* buf, u16 type0, u16 type1, BOOL both)
{
    u16 i;
    u32 id;
    u16 count;
    u16 a;
    u16 b;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            pdaLoadPokemonSlot(id);
            a = pdaGetTypeIcon(id, 0);
            b = pdaGetTypeIcon(id, 1);
            if (both) {
                if ((type0 == a && type1 == b) || (type0 == b && type1 == a)) {
                    buf[count] = id;
                    count++;
                }
            } else {
                if (type0 == a || type0 == b) {
                    buf[count] = id;
                    count++;
                }
            }
        }
    }
    return count;
}

/* Filter the memo index table down to the active type tabs. */
#pragma peephole off
void pdaMemoBuildTypeIndex(void)
{
    u8* S = (u8*)&lbl_803A6818;
    u16 buf1[500];
    u16 buf0[500];
    u16 type0;
    u16 type1;
    u16 single;
    u16 i;
    u16 count;

    if (*(s8*)(S + 0x15a) == 0 && *(s8*)(S + 0x15b) == 0) {
        return;
    }
    type0 = *(s8*)(S + 0x15a);
    type1 = *(s8*)(S + 0x15b);
    if (type0 != 0 && type1 != 0) {
        single = 0;
    } else {
        if (type0 == 0) {
            type0 = type1;
        }
        single = 1;
    }
    if (single == 0) {
        count = pdaFilterTypes(buf1, type0, type1, TRUE);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf1[i];
        }
        lbl_8047A4E8 = count;
    } else {
        count = pdaFilterTypes(buf0, type0, type1, FALSE);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf0[i];
        }
        lbl_8047A4E8 = count;
    }
}
#pragma peephole reset

typedef struct PdaSortPair {
    u16 index;
    u16 key;
} PdaSortPair;

extern u16 pokemonDataBiosGetWeight(void* data);
extern u16 pokemonDataBiosGetHeight(void* data);


static inline void pdaFree(void* p)
{
    u32 h;

    if (p != NULL) {
        h = GSmemGetHandleByPtr(p);
        if ((u16)h != 0) {
            GSmemUnlock();
            GSmemFree(h);
        }
    }
}

static inline u16 pdaGetWeight(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetWeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

static inline u16 pdaGetHeight(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetHeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

/* Fill the sort pairs with each seen entry's weight; returns the entry count. */
static inline u16 pdaCollectWeights(PdaSortPair* pairs, u16* ids)
{
    u16 i;
    u32 id;
    u16 count;
    u16 key;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            key = pdaGetWeight(id);
            ids[count] = id;
            pairs[count].key = key;
            pairs[count].index = i;
            count++;
        }
    }
    return count;
}

/* Fill the sort pairs with each seen entry's height; returns the entry count. */
static inline u16 pdaCollectHeights(PdaSortPair* pairs, u16* ids)
{
    u16 i;
    u32 id;
    u16 count;
    u16 key;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            key = pdaGetHeight(id);
            ids[count] = id;
            pairs[count].key = key;
            pairs[count].index = i;
            count++;
        }
    }
    return count;
}

/* Sort tab 1: order the seen entries by weight (the sort runs even for an empty list). */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_SortWeight (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003E394_SortWeight(void)
{
    u16 j;
    u16 count;
    PdaSortPair* pairs;
    u16* ids;

    pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
    ids = (u16*)pdaAlloc(0x320, 0x20);
    count = pdaCollectWeights(pairs, ids);
    pdaMemoSortPairs((u8*)pairs, count, 1);
    for (j = 0; j < count; j++) {
        lbl_8047A4E4[j] = ids[pairs[j].index];
    }
    lbl_8047A4E8 = count;
    pdaFree(pairs);
    pdaFree(ids);
}

/* Reorder the memo index table for the active sort tab. */
#pragma peephole off
void pdaMemoDrawData(void)
{
    u16 j;
    u16 count;
    /* RULE-EXCEPTION(user-approved): no-op store, NULL initialisers that every case overwrites (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
    u16* ids = NULL;
    PdaSortPair* pairs = NULL;

    switch (*(s8*)((u8*)&lbl_803A6818 + 0x15c)) {
    case 0:
        pdaMemoSortByName();
        break;
    case 1:
        pda_8003E394_SortWeight();
        break;
    case 2:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectWeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 0);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    case 3:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectHeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 1);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    case 4:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectHeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 0);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    default:
        break;
    }
}
#pragma peephole reset

typedef struct PdaNamePair {
    u16 index;
    u16 name[12];
} PdaNamePair;

/* Sort the memo index table by species name. */
#pragma peephole off
void pdaMemoSortByName(void)
{
    extern s32 GScharCmp(u16* a, u16* b);
    extern void* memcpy(void* dst, const void* src, u32 size);
    u16 i;
    PdaNamePair tmp;
    s32 gap;
    PdaNamePair* q;
    PdaNamePair* pairs;
    s32 k;
    s32 j;
    PdaNamePair* p;
    u16* ids;
    s32 count;

    pairs = (PdaNamePair*)pdaAlloc(0x28a0, 0x20);
    ids = (u16*)pdaAlloc(0x320, 0x20);
    for (i = 0; i < lbl_8047A4E8; i++) {
        u32 id = lbl_8047A4E4[i];
        ids[i] = id;
        GSmsgGetText((u8*)pairs[i].name, 0x18, (u8*)pdaGetPokemonName(id));
        pairs[i].index = i;
    }

    count = lbl_8047A4E8;
    for (gap = count / 2; gap > 0; gap = gap / 2) {
        for (j = gap; j < count; j++) {
            for (k = j - gap, p = &pairs[k];
                 k >= 0 && GScharCmp(p->name, (q = &pairs[k + gap])->name) >= 0;
                 p -= gap, k -= gap) {
                memcpy(&tmp, p, 0x1a);
                memcpy(p, q, 0x1a);
                memcpy(q, &tmp, 0x1a);
            }
        }
    }

    for (i = 0; i < lbl_8047A4E8; i++) {
        lbl_8047A4E4[i] = ids[pairs[i].index];
    }
    pdaFree(pairs);
    pdaFree(ids);
}
#pragma peephole reset

extern PdaOrbitPoint lbl_802E52C8[8];
extern f32 lbl_8047BCAC;
extern s32 spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

/* Anchor a PDA widget to its layout entry and hand the sprite to the drawer. */
static inline void pdaPlaceSprite(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    sprite->field_50 =
        (s16)((s32)*(f32*)((u8*)&lbl_803A6818 + 0x1dc) +
              *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2));
    spriteSetEnv(spriteSetOrigin(
        (s16)(*(s16*)((u8*)alphaSprite + 0x84) + sprite->field_50),
        (s16)(*(s16*)((u8*)alphaSprite + 0x86) + sprite->field_52)));
}

static inline s32 pdaTint(PdaSprite* alphaSprite, s32 tint)
{
    return alphaSprite->alphaByte | tint;
}

/* Per-widget layout and visibility for the PDA's summary page. */
#pragma peephole off
void pdaMemoMainDrawText(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    u8* T = (u8*)lbl_802E52C8;
    u8* S = (u8*)&lbl_803A6818;
    f32 pos;
    f32 stride;
    s8 i;
    s32 off;
    s32 slot;
    s32 msg;

    alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
    switch (sprite->eventId) {
    case 0xd47:
    case 0xd48:
    case 0xd49:
    case 0xd4a:
    case 0xd4b:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd89:
        pdaPlaceSprite(alphaSprite, sprite);
        slot = *(s8*)(S + 0x15f) - *(s8*)(S + 0x1c8);
        if (slot >= 0x1d) {
            slot = 0x1c;
        }
        sprite->field_52 = (s16)(*(s16*)(lbl_802EF0A8 + 0x17b00) +
                                 (s16)(slot * 0x1d));
        break;
    case 0xd8a:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)(S + 0x1c8) > 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(lbl_8047BCA8 *
                      (f32)sin(lbl_8047BCA4 *
                                   (lbl_8047BCA8 *
                                    (lbl_8047BCA8 * lbl_803A6818.angle)) +
                               lbl_8047BCA4) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b1c));
        break;
    case 0xd8b:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)(S + 0x1c9) >= 10) {
            if (*(s8*)(S + 0x160) != *(s8*)(S + 0x1c9)) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
                break;
            }
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(lbl_8047BCA8 *
                      (f32)sin(lbl_8047BCA4 *
                               (lbl_8047BCA8 *
                                (lbl_8047BCA8 * lbl_803A6818.angle))) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b38));
        break;
    case 0xd66:
    case 0xd67:
    case 0xd68:
    case 0xd69:
    case 0xd6a:
    case 0xd6b:
    case 0xd6c:
    case 0xd6d:
    case 0xd6e:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd6f:
        pdaPlaceSprite(alphaSprite, sprite);
        spriteSetScissor(
            *(s16*)(lbl_802EF0A8 + 0x17b52) -
                *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2),
            *(s16*)(lbl_802EF0A8 + 0x17b54) -
                *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 4),
            *(s16*)(lbl_802EF0A8 + 0x17b56),
            *(s16*)(lbl_802EF0A8 + 0x17b58));
        if (*(s8*)(S + 0x15d) != 4) {
            i = 0;
            pos = *(f32*)(S + 0x1d0);
            off = 0;
            stride = lbl_8047BCAC;
            for (i = 0; i < *(s8*)(S + 0x160); i++) {
                if (i >= *(s8*)(S + 0x1c8) - 1 &&
                    i <= *(s8*)(S + 0x1c9) + 1) {
                    GSmsgPrintColor(0, (s32)pos,
                                (alphaSprite->alphaByte | -0x100LL),
                                (void*)*(u32*)((u8*)*(u32**)(T + 0x24c +
                                                             *(s8*)(S + 0x15d) *
                                                                 4) +
                                               off));
                }
                pos = pos + stride;
                off += 4;
            }
        }
        spriteClearScissor();
        break;
    case 0xd79:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ed) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ed);
        break;
    case 0xd7a:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ec) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ec);
        break;
    case 0xd7b:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36eb) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36eb);
        break;
    case 0xd7c:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ea) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ea);
        break;
    case 0xd7d:
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x36e9) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36e9);
        break;
    case 0xd74:
        GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL),
                    (void*)(*(u32**)(T + 0x258))[*(s8*)(S + 0x15c)]);
        break;
    case 0xd75:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((*(u32**)(T + 0x254))[*(s8*)(S + 0x15b)]));
        GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)0xe7);
        break;
    case 0xd76:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((*(u32**)(T + 0x254))[*(s8*)(S + 0x15a)]));
        GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)0xe7);
        break;
    case 0xd77:
        GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL),
                    (void*)(*(u32**)(T + 0x250))[*(s8*)(S + 0x159)]);
        break;
    case 0xd78:
        GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL),
                    (void*)(*(u32**)(T + 0x24c))[*(s8*)(S + 0x158)]);
        break;
    case 0xd54:
        if (*(s8*)(S + 0x15d) == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd55:
        if (*(s8*)(S + 0x15d) == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd56:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd57:
        if (*(s8*)(S + 0x15d) == 3) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd58:
        if (*(s8*)(S + 0x15d) == 4) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd73:
        switch (*(s32*)(S + 0x154)) {
        case 0:
            if (*(s8*)(S + 0x15d) == 4) {
                slot = *(s32*)(S + 0x1e4);
                if (slot == 0) {
                    msg = ((u32*)(T + 0x270))[*(s8*)(S + 0x15d)];
                }
                if (slot == 2) {
                    msg = 0x3712;
                }
                if (slot == 1) {
                    msg = 0x3714;
                }
            } else {
                msg = ((u32*)(T + 0x270))[*(s8*)(S + 0x15d)];
            }
            GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)msg);
            break;
        case 1:
            if (*(s8*)(S + 0x15d) == 3) {
                GSmsgPrintColor(0, 0, (alphaSprite->alphaByte | -0x100LL),
                            (void*)((u32*)(T + 0x25c))[*(s8*)(S + 0x15f)]);
            }
            break;
        default:
            break;
        }
        break;
    case 0xd59:
    case 0xd5a:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags &= ~2;
        } else if (*(s8*)(S + 0x15d) == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5b:
    case 0xd5c:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags &= ~2;
        } else if (*(s8*)(S + 0x15d) == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5d:
    case 0xd5e:
        if (*(s8*)(S + 0x15d) == 2) {
            if (*(s8*)(S + 0x15e) == 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5f:
    case 0xd60:
        if (*(s8*)(S + 0x15d) == 2) {
            if (*(s8*)(S + 0x15e) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd61:
    case 0xd62:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags &= ~2;
        } else if (*(s8*)(S + 0x15d) == 3) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    default:
        break;
    }
}
#pragma peephole reset

extern u32 lbl_8047A4F0;
extern u16 lbl_803A67E8[];
extern u8 pokemonDataBiosGetTokuseiDataId(void* data, s32 slot);
extern void* pokemonTokuseiDataBiosGetPtr(u32 id);
extern void* pokemonTokuseiDataBiosGetName(void* data);
extern u32 pokemonDataBiosGetTypeName(void* data);

/* Write both seeds into the work Pokemon. MWCC evaluates the arguments right to
 * left, so a caller passing both lookups runs the Pokemon seed lookup first. */
static inline void pda_800411FC_Seeds(u32 work, u32 trainerRnd, u32 rnd)
{
    pokemonBiosSetRnd(work, rnd);
    pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
}

/* pdaMemoDrawPokemonDetail's pdaLoadPokemon: rebuild the work Pokemon for memo entry `index`. */
static inline u32 pda_800411FC_Load(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 species;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        pda_800411FC_Seeds(work, memoDataGetPokemonTrainerRndFromID(0, species),
                           memoDataGetPokemonRndFromID(0, species));
        return lbl_8047A4E0;
    }
    return 0;
}

/* Whether the highlighted memo entry has actually been seen. */
static inline u8 pdaEntrySeen(void)
{
    return (lbl_8047A4E4[lbl_803A6818.currentIndex] & 0x8000) ? 0 : 1;
}

/* Species-name message for the highlighted entry, 0 when unavailable. */
static inline u32 pdaCurrentNameMsg(s32 index)
{
    u32 work;

    if (lbl_8047A4E0 != 0) {
        work = pda_800411FC_Load(index);
        if (work != 0) {
            return GSmsgGetGSchar((u32)pokemonDataBiosGetName(
                pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work))));
        }
        return 0;
    }
    return 0;
}

/* Ability ID in the given slot of the highlighted entry. */
static inline u16 pda_800411FC_Ability(s32 slot)
{
    return pokemonDataBiosGetTokuseiDataId(
        pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(
            pda_800411FC_Load(lbl_803A6818.currentIndex))),
        slot);
}

/* Type-name message of the highlighted entry, 0 when unavailable. */
static inline u32 pda_800411FC_TypeName(void)
{
    if (lbl_8047A4E0 != 0) {
        return pokemonDataBiosGetTypeName(pokemonDataBiosGetPtr(
            pokemonBiosGetPokemonDataId(pda_800411FC_Load(lbl_803A6818.currentIndex))));
    }
    return 0;
}

/* Height of the highlighted entry, 0 when unavailable. */
static inline u16 pdaCurrentHeight(void)
{
    u32 work;
    void* data;

    if (lbl_8047A4E0 != 0) {
        work = pda_800411FC_Load(lbl_803A6818.currentIndex);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetHeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

/* Weight of the highlighted entry, 0 when unavailable. */
static inline u16 pdaCurrentWeight(void)
{
    u32 work;
    void* data;

    if (lbl_8047A4E0 != 0) {
        work = pda_800411FC_Load(lbl_803A6818.currentIndex);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetWeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

/* Species panel: name, abilities, height, weight and type for the entry. */
#pragma peephole off
void pdaMemoDrawPokemonDetail(PdaSprite* alphaSprite, PdaEvent* event)
{
    extern void GSmsgPrintRight(s32 x, s32 y, s16 w, s16 h, void* color, s32 msg);
    extern void GSmsgPrintRect(s32 x, s32 y, s16 w, s16 h, void* color, s32 msg);
    PdaSprite* sprite = (PdaSprite*)event;
    u8* layout;
    u32 name;
    u16 tokusei0;
    u16 tokusei1;
    u32 height;
    u32 weight;
    u32 digit;
    s32 i;
    s32 digits;
    u32 msg;
    s16* pos;

    name = pdaCurrentNameMsg(lbl_803A6818.currentIndex);
    if (name == 0) {
        name = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, name);
    GSmsgPrintColor(0, 0, alphaSprite->alphaByte | -0x100, (void*)0xce);
    layout = lbl_802EF0A8;

    if (pdaEntrySeen() != 0) {
        tokusei0 = pda_800411FC_Ability(0);
        msg = GSmsgGetGSchar((u32)pokemonTokuseiDataBiosGetName(
            pokemonTokuseiDataBiosGetPtr(tokusei0)));
        msgctrlSetValue(0x37, msg);
        GSmsgPrintColor(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100, (void*)0xe7);
    } else {
        GSmsgPrintColor(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100, (void*)0x3721);
    }

    if (pdaEntrySeen() != 0) {
        tokusei1 = pda_800411FC_Ability(1);
        if (tokusei1 != 0 && tokusei1 != tokusei0) {
            pos = (s16*)(lbl_802EF0A8 + 0x20b42);
            msgctrlSetValue(0x37,
                            GSmsgGetGSchar((u32)pokemonTokuseiDataBiosGetName(
                                pokemonTokuseiDataBiosGetPtr(tokusei1))));
            GSmsgPrintColor(pos[0] - sprite->field_50 + 4,
                        pos[1] - sprite->field_52,
                        alphaSprite->alphaByte | -0x100, (void*)0xe7);
        }
    } else {
        GSmsgPrintColor(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100, (void*)0x3721);
    }

    layout = lbl_802EF0A8;
    if (pdaEntrySeen() != 0) {
        height = pdaCurrentHeight();
        i = 10;
        lbl_803A67E8[--i] = 0;
        lbl_803A67E8[--i] = 0x27;
        lbl_803A67E8[--i] = 0x27;
        digits = 3;
        for (;;) {
            digit = height % 10;
            height = height / 10;
            if (digits <= 0) {
                if (digit == 0) {
                    break;
                }
            } else {
                digits--;
            }
            lbl_803A67E8[--i] = digit + 0x30;
            if (digits == 1) {
                lbl_803A67E8[--i] = 0x27;
            }
        }
        msgctrlSetValue(0x37, (u32)&lbl_803A67E8[i]);
        GSmsgPrintRight(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a76) - sprite->field_50),
                    *(s16*)(layout + 0x5a78) - sprite->field_52,
                    *(s16*)(layout + 0x5a7a),
                    *(s16*)(layout + 0x5a7c),
                    (void*)(alphaSprite->alphaByte | -0x100), 0xcf);
    } else {
        GSmsgPrintRect(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a76) - sprite->field_50),
                    *(s16*)(layout + 0x5a78) - sprite->field_52,
                    *(s16*)(layout + 0x5a7a),
                    *(s16*)(layout + 0x5a7c),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371b);
    }

    layout = lbl_802EF0A8;
    if (pdaEntrySeen() != 0) {
        weight = pdaCurrentWeight();
        msgctrlSetValue(0x34, weight / 10);
        msgctrlSetValue(0x35, weight % 10);
        GSmsgPrintRect(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a92) - sprite->field_50),
                    *(s16*)(layout + 0x5a94) - sprite->field_52,
                    *(s16*)(layout + 0x5a96),
                    *(s16*)(layout + 0x5a98),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x189c);
    } else {
        GSmsgPrintRect(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a92) - sprite->field_50),
                    *(s16*)(layout + 0x5a94) - sprite->field_52,
                    *(s16*)(layout + 0x5a96),
                    *(s16*)(layout + 0x5a98),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371a);
    }

    layout = lbl_802EF0A8;
    if (pdaEntrySeen() != 0) {
        msgctrlSetValue(0x31, pda_800411FC_TypeName());
        GSmsgPrintRect(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a22) - sprite->field_50),
                    *(s16*)(layout + 0x5a24) - sprite->field_52,
                    *(s16*)(layout + 0x5a26),
                    *(s16*)(layout + 0x5a28),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371e);
    } else {
        msgctrlSetValue(0x31, 0x371c);
        GSmsgPrintRect(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a22) - sprite->field_50),
                    *(s16*)(layout + 0x5a24) - sprite->field_52,
                    *(s16*)(layout + 0x5a26),
                    *(s16*)(layout + 0x5a28),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371e);
    }
}
#pragma peephole reset

extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCD0;
extern f32 lbl_8047BCD4;

/* Ease a value toward its target, snapping once a step would overshoot. */
static inline void pdaEase(f32* cur, f32* dstp)
{
    f32 c;
    f32 dst;
    f32 delta;
    f32 rate;
    f32 moved;
    f32 rest;
    f32 next;
    s8 mode;

    c = *cur;
    dst = *dstp;
    if (c == dst) {
        return;
    }
    delta = dst - c;
    rate = lbl_8047BCC4 * *(f32*)((u8*)&lbl_803A6818 + 0x3c);
    if ((delta > 0.0f ? delta : -delta) >= lbl_8047BCD0) {
        *cur = dst;
        return;
    }
    mode = *(s8*)((u8*)&lbl_803A6818 + 0x48);
    if (mode == 1) {
        rate = rate * (lbl_8047BCD4 / (f32)(s8)lbl_804788C4);
    }
    if (mode == 2) {
        rate = rate * (lbl_8047BCD4 / (f32)(s8)lbl_804788C4);
    }
    moved = delta * rate;
    next = c + moved;
    rest = dst - next;
    *cur = next;
    if ((rest > 0.0f ? rest : -rest) <= (moved > 0.0f ? moved : -moved)) {
        *cur = dst;
    }
}

/* Ramp a 0..1 fade toward its target at a fixed rate. */
static inline void pdaRamp(f32* cur, f32 dst, f32 rate)
{
    f32 step;

    if (*cur == dst) {
        return;
    }
    step = *(f32*)((u8*)&lbl_803A6818 + 0x3c) / rate;
    if (dst > *cur) {
        *cur = *cur + step;
        if (*cur > lbl_8047BCBC) {
            *cur = lbl_8047BCBC;
        }
    } else {
        *cur = *cur - step;
        if (*cur < lbl_8047BC94) {
            *cur = lbl_8047BC94;
        }
    }
}

/* Per-frame scene animation and D-pad handling for the PDA summary page. */
#define G(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))
s32 pdaMemoUpdateScroll(void* work, s32 mode)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u8* S = (u8*)&lbl_803A6818;
    u16 keys;
    f32 dt;
    f32 target;
    f32 current;
    f32 step;
    f32 next;

    (void)work;
    (void)mode;
    keys = *(u16*)(lbl_803A67FC + 6);
    dt = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    G(0x3c) = dt;
    G(0x40) = G(0x40) + dt;
    if (G(0x40) >= lbl_8047BCBC) {
        G(0x40) = lbl_8047BC94;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            (f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            (f32*)((u8*)&lbl_803A6818 + 0x38));
    pdaEase((f32*)(S + 0x1d0), (f32*)(S + 0x1d8));
    target = G(0x50);
    current = G(0x4C);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCB8;
        if (target > current) {
            next = current + step;
            G(0x4C) = next;
            if (next > lbl_8047BCBC) {
                G(0x4C) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x4C) = next;
            if (next < lbl_8047BC94) {
                G(0x4C) = lbl_8047BC94;
            }
        }
    }

    target = G(0x58);
    current = G(0x54);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCB8;
        if (target > current) {
            next = current + step;
            G(0x54) = next;
            if (next > lbl_8047BCBC) {
                G(0x54) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x54) = next;
            if (next < lbl_8047BC94) {
                G(0x54) = lbl_8047BC94;
            }
        }
    }

    target = G(0x60);
    current = G(0x5C);
    if (target != current) {
        step = G(0x3C) / lbl_8047BCC0;
        if (target > current) {
            next = current + step;
            G(0x5C) = next;
            if (next > lbl_8047BCBC) {
                G(0x5C) = lbl_8047BCBC;
            }
        } else {
            next = current - step;
            G(0x5C) = next;
            if (next < lbl_8047BC94) {
                G(0x5C) = lbl_8047BC94;
            }
        }
    }

    if (G(0x1DC) != G(0x1E0)) {
        pdaApproach(&G(0x1DC), &G(0x1E0), G(0x3C));
    }

    if (*(s32*)(S + 0x154) == 0) {
        if (*(s32*)(S + 0x1e4) != 0) {
            return 0;
        }
        if (*(s8*)(S + 0x15d) == 2) {
            if ((keys & 0x4) != 0) {
                if (*(s8*)(S + 0x15e) != 0) {
                    soundStartRaw(0x23, 0, 0);
                }
                *(s8*)(S + 0x15e) = 0;
            } else if ((keys & 0x8) != 0) {
                if (*(s8*)(S + 0x15e) == 0) {
                    soundStartRaw(0x23, 0, 0);
                }
                *(s8*)(S + 0x15e) = 1;
            }
        } else {
            *(s8*)(S + 0x15e) = 0;
        }
        if ((keys & 0x2) != 0) {
            if (*(s8*)(S + 0x15d) >= 4) {
                return 0;
            }
            *(s8*)(S + 0x15d) = *(s8*)(S + 0x15d) + 1;
            soundStartRaw(0x23, 0, 0);
        } else if ((keys & 0x1) != 0) {
            if (*(s8*)(S + 0x15d) <= 0) {
                return 0;
            }
            *(s8*)(S + 0x15d) = *(s8*)(S + 0x15d) - 1;
            soundStartRaw(0x23, 0, 0);
        }
        return 0;
    }

    if ((keys & 0x2) != 0) {
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x160) - 1) {
            *(s8*)(S + 0x15f) = *(s8*)(S + 0x15f) + 1;
            soundStartRaw(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) >= *(s8*)(S + 0x1c9)) {
            *(f32*)(S + 0x1d8) = *(f32*)(S + 0x1d8) - lbl_8047BCAC;
            *(s8*)(S + 0x1c9) = *(s8*)(S + 0x1c9) + 1;
            *(s8*)(S + 0x1c8) = *(s8*)(S + 0x1c8) + 1;
        }
    } else if ((keys & 0x1) != 0) {
        if (*(s8*)(S + 0x15f) > 0) {
            *(s8*)(S + 0x15f) = *(s8*)(S + 0x15f) - 1;
            soundStartRaw(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x1c8)) {
            *(f32*)(S + 0x1d8) = *(f32*)(S + 0x1d8) + lbl_8047BCAC;
            *(s8*)(S + 0x1c8) = *(s8*)(S + 0x1c8) - 1;
            *(s8*)(S + 0x1c9) = *(s8*)(S + 0x1c9) - 1;
        }
    }
    return 0;
}
#undef G

extern f32 lbl_8047BCE0;
extern f32 lbl_8047BCE4;
extern f32 lbl_8047BCE8;
extern f32 lbl_8047BCEC;
extern f32 lbl_8047BCF0;
extern s8 GSinputGetRightStickXData(s32 chan, s32 index);
extern s8 GSinputGetRightStickYData(s32 chan, s32 index);
extern void GSvecAdd(void* dst, void* a, void* b);
extern u8 menuPokemonModelIsLoaded(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void menuPokemonModelRequest(void* work, u32 pokemon);
extern void GSmodelDestroyLinkedParticles(void* model);

/* Step the page down one level, tearing the current model's particles down. */
static inline void pdaPopLevel(u8* S)
{
    void* model;

    *(f32*)(S + 0x6c) = lbl_8047BC94;
    *(f32*)(S + 0x70) = lbl_8047BC94;
    *(f32*)(S + 0x74) = lbl_8047BC94;
    *(f32*)(S + 0x64) = lbl_8047BCF0;
    switch (*(s32*)(S + 0x28)) {
    case 0:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 2;
        *(f32*)(S + 0x60) = lbl_8047BC94;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        break;
    case 1:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 4;
        *(f32*)(S + 0x60) = lbl_8047BC94;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        *(f32*)(S + 0x64) = lbl_8047BCA8;
        break;
    case 2:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 5;
        *(f32*)(S + 0x60) = lbl_8047BC94;
        model = wazaSequenceOwnerGetModel(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        break;
    default:
        break;
    }
}

/* Close the windows belonging to the level we are leaving. */
static inline void pdaCloseLevelWindows(u8* S)
{
    switch (*(s32*)(S + 0x28)) {
    case 0:
        menuCloseCustom(0x3c, 0, 0);
        menuCloseCustom(0x96, 0, 0);
        break;
    case 1:
        menuCloseCustom(0xa1, 0, 0);
        menuCloseCustom(0x96, 0, 0);
        break;
    case 2:
        menuCloseCustom(0x96, 0, 0);
        break;
    default:
        break;
    }
}

/* Per-frame scene update for the PDA's model page. */
#pragma peephole off
s32 pdaModelViewerControl(void* work, s32 mode)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u8* K = lbl_803A67FC;
    PdaVec3 spin;
    s8 stickY;
    s8 stickX;
    PdaVec3 pos;
    u16 keys;
    s32 held;
    s32 prevIndex;
    f32 dt;
    f32 delta;
    f32 moved;
    f32 rest;
    void* model;
    u32 pokemon;
    f32* rot;

    held = 0;
    prevIndex = *(s32*)&lbl_803A6818;
    if (*(s8*)((u8*)&lbl_803A6818 + 0x48) == 1) {
        keys = *(u16*)(K + 6);
        lbl_804788C4 = 3;
    } else if (*(s8*)((u8*)&lbl_803A6818 + 0x48) == 2) {
        keys = *(u16*)(K + 6);
        lbl_804788C4 = 2;
    } else {
        keys = *(u16*)(K + 6);
        lbl_804788C4 = 5;
    }
    if ((*(u16*)K & 3) != 0) {
        held = 1;
    }
    dt = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    *(f32*)((u8*)&lbl_803A6818 + 0x3c) = dt;
    *(f32*)((u8*)&lbl_803A6818 + 0x40) = *(f32*)((u8*)&lbl_803A6818 + 0x40) + dt;
    if (*(f32*)((u8*)&lbl_803A6818 + 0x40) >= lbl_8047BCBC) {
        *(f32*)((u8*)&lbl_803A6818 + 0x40) = lbl_8047BC94;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            (f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            (f32*)((u8*)&lbl_803A6818 + 0x38));
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x4c),
            *(f32*)((u8*)&lbl_803A6818 + 0x50), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x54),
            *(f32*)((u8*)&lbl_803A6818 + 0x58), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x5c),
            *(f32*)((u8*)&lbl_803A6818 + 0x60), lbl_8047BCC0);

    if (*(f32*)((u8*)&lbl_803A6818 + 0x1dc) !=
        *(f32*)((u8*)&lbl_803A6818 + 0x1e0)) {
        delta = *(f32*)((u8*)&lbl_803A6818 + 0x1e0) -
                *(f32*)((u8*)&lbl_803A6818 + 0x1dc);
        moved = lbl_8047BCC4 * delta * *(f32*)((u8*)&lbl_803A6818 + 0x3c);
        if (moved > lbl_8047BC98) {
            moved = lbl_8047BC98;
        }
        if (moved <= lbl_8047BCC8) {
            moved = lbl_8047BCC8;
        }
        *(f32*)((u8*)&lbl_803A6818 + 0x1dc) =
            *(f32*)((u8*)&lbl_803A6818 + 0x1dc) + moved;
        rest = *(f32*)((u8*)&lbl_803A6818 + 0x1e0) -
               *(f32*)((u8*)&lbl_803A6818 + 0x1dc);
        if (moved > lbl_8047BC94) {
        } else {
            moved = -moved;
        }
        if (pdaFabs(rest) <= moved) {
            *(f32*)((u8*)&lbl_803A6818 + 0x1dc) =
                *(f32*)((u8*)&lbl_803A6818 + 0x1e0);
        } else {
            if (rest > lbl_8047BC94) {
            } else {
                rest = -rest;
            }
            if (rest < lbl_8047BCBC) {
                *(f32*)((u8*)&lbl_803A6818 + 0x1dc) =
                    *(f32*)((u8*)&lbl_803A6818 + 0x1e0);
            }
        }
    }

    switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
    case 1:
    case 2:
    case 3:
        ((u8*)&lbl_803A6818)[0x214] = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
        pdaMemoListKey((u32)work, mode, keys);
        break;
    case 4:
        ((u8*)&lbl_803A6818)[0x214] = pdaModelOrbitCamera((u8*)&lbl_803A6818 + 0x7c);
        spin.x = lbl_8047BC94;
        spin.y = lbl_8047BC94;
        spin.z = lbl_8047BC94;
        model = wazaSequenceOwnerGetModel(*(void**)((u8*)&lbl_803A6818 + 0x7c + 0x24));
        if (model == NULL) {
            break;
        }
        GSmodelGetBound(model);
        rot = (f32*)((u8*)&lbl_803A6818 + 0x6c);
        stickY = GSinputGetRightStickXData(1, 1);
        stickX = GSinputGetRightStickYData(1, 1);
        spin.x = *(f32*)((u8*)&lbl_803A6818 + 0x3c) *
                 (lbl_8047BCA4 * (f32)stickX * lbl_8047BCE0);
        spin.y = *(f32*)((u8*)&lbl_803A6818 + 0x3c) *
                 (lbl_8047BCA4 * (f32)stickY * lbl_8047BCE0);
        GSvecAdd(rot, rot, &spin);
        if (*rot >= lbl_8047BCE4) {
            *rot = lbl_8047BCE4 - spin.x;
        }
        if (*rot <= lbl_8047BCE8) {
            *rot = lbl_8047BCE8 - spin.x;
        }
        GSmodelGetPosition(model, &pos);
        pos.x = lbl_8047BC94;
        GSmodelSetPosition(model, &pos);
        if ((GSinputGetButtons(1) & 0x40) != 0) {
            if (*(f32*)((u8*)&lbl_803A6818 + 0x64) <= lbl_8047BCEC) {
                *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) + *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            }
        } else if ((GSinputGetButtons(1) & 0x20) != 0) {
            if (*(f32*)((u8*)&lbl_803A6818 + 0x64) > lbl_8047BCBC) {
                *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) - *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            }
        }
        break;
    case 5:
        ((u8*)&lbl_803A6818)[0x214] = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        ((u8*)&lbl_803A6818)[0x214] = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }

    if (held != 0) {
        if (*(s8*)((u8*)&lbl_803A6818 + 0x48) < 2) {
            *(f32*)((u8*)&lbl_803A6818 + 0x44) = *(f32*)((u8*)&lbl_803A6818 + 0x44) + *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            if (*(f32*)((u8*)&lbl_803A6818 + 0x44) >= lbl_8047BCBC) {
                *(s8*)((u8*)&lbl_803A6818 + 0x48) = 1;
            }
            if (*(f32*)((u8*)&lbl_803A6818 + 0x44) >= lbl_8047BCA8) {
                *(s8*)((u8*)&lbl_803A6818 + 0x48) = 2;
            }
        }
    } else {
        *(f32*)((u8*)&lbl_803A6818 + 0x44) = lbl_8047BC94;
        *(s8*)((u8*)&lbl_803A6818 + 0x48) = 0;
    }

    if (mode != 0) {
        if (*(f32*)((u8*)&lbl_803A6818 + 0x58) ==
            *(f32*)((u8*)&lbl_803A6818 + 0x54)) {
            if ((*(u16*)(K + 6) & 0x4) != 0) {
                if (*(s32*)((u8*)&lbl_803A6818 + 0x28) > 0) {
                    pdaCloseLevelWindows((u8*)&lbl_803A6818);
                    *(s32*)((u8*)&lbl_803A6818 + 0x28) =
                        *(s32*)((u8*)&lbl_803A6818 + 0x28) - 1;
                    soundStartRaw(0x23, 0, 0);
                    pdaPopLevel((u8*)&lbl_803A6818);
                }
            }
            if ((*(u16*)(K + 6) & 0x8) != 0) {
                if (*(s32*)((u8*)&lbl_803A6818 + 0x28) < 2 &&
                    pdaEntrySeen() != 0 &&
                    *(s32*)((u8*)&lbl_803A6818 + 0x20) != 1) {
                    pdaCloseLevelWindows((u8*)&lbl_803A6818);
                    *(s32*)((u8*)&lbl_803A6818 + 0x28) =
                        *(s32*)((u8*)&lbl_803A6818 + 0x28) + 1;
                    soundStartRaw(0x23, 0, 0);
                    pdaPopLevel((u8*)&lbl_803A6818);
                }
            }
            *(f32*)((u8*)&lbl_803A6818 + 0x30) =
                *(f32*)((u8*)&lbl_803A6818 + 0x38);
        }
    }

    if (prevIndex != lbl_803A6818.currentIndex && lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
        }
        *((u8*)&lbl_803A6818 + 0x214) = 0;
        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
        case 0xb:
        case 0xc:
            *((u8*)&lbl_803A6818 + 0x214) =
                pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera(
                (u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
            *((u8*)&lbl_803A6818 + 0x214) =
                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
            break;
        default:
            break;
        }
    }
    return 0;
}
#pragma peephole reset

typedef struct PdaCamBlock {
    f32 v[6];
} PdaCamBlock;

extern f32 lbl_8047BCCC;
extern const f32 lbl_80267150[];
extern void menuOffScreenSetDisp(s32 disp);
extern void std114_mapObjectSetVisibility(void* h, s32 b);
extern void std113_mapObjectUnk_8018db68(void* h);
extern void* std112_modelGetResourceID(u32 id);
extern void* std093_getCurrentGroupID(void);
extern void* GSresGetResource(u32 group, u32 id);
extern void GSmodelSetVisibility(void* model, s32 vis);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GScameraGetPosition(void* cam, void* out);
extern void GScameraGetRotation(void* cam, void* out);
extern void cameraPlayAnime(u32 group, u32 id, s32 c, s32 d);
extern void menuModelInit(void* work, s32 w, s32 h);
extern void* peopleInfoBiosGetPtr(u32 id);
extern void peopleInfoGetMotion(void* info, s32 index, s32* motion, s32* out);
extern void menuModelSetMotion(void* work, s32 motion);
extern void menuPokemonModelRequestById(void* work, u32 id);

/* Build the PDA scene: camera rig, both model slots and the memo index. */
#pragma peephole off
void pdaModelViewerSetup(void)
{
    extern u16 memoDataGetCount(s32 a);
    const PdaCamBlock* tbl = (const PdaCamBlock*)lbl_80267150;
    PdaVec3 rot;
    PdaCamBlock blk;
    s32 motion;
    f32 persp3;
    f32 persp2;
    f32 persp1;
    f32 persp0;
    s32 out8;
    void* res;
    void* cam;
    void* anim;
    u32 pokemon;
    u16 total;
    u8* saved;
    f32 cf0;
    f32 cbc;
    f32 c94;

    menuOffScreenSetDisp(0);
    *(s32*)((u8*)&lbl_803A6818 + 0x110) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x114) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x10c) = 0;
    std114_mapObjectSetVisibility(NULL, 0);
    res = GSresGetResource((u32)std093_getCurrentGroupID(), 0);
    if (res != NULL) {
        GSmodelSetVisibility(res, 0);
    }
    std113_mapObjectUnk_8018db68(NULL);
    *(f32*)((u8*)&lbl_803A6818 + 0x6c) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x70) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x74) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = lbl_8047BCF0;
    anim = std112_modelGetResourceID(0xd171000);
    *(void**)((u8*)&lbl_803A6818 + 0x10c) = anim;
    res = GSresGetResource((u32)std093_getCurrentGroupID(), (u32)anim);
    *(void**)((u8*)&lbl_803A6818 + 0x110) = res;
    GSmodelSetAnimRate(res, lbl_8047BD18);
    blk = *tbl;
    cam = GSresGetResource(0x17, 0xd731800);
    *(void**)((u8*)&lbl_803A6818 + 0x114) = cam;
    GScameraGetPosition(cam, &blk.v[3]);
    GScameraGetRotation(cam, &rot);
    blk.v[0] = *(f32*)((u8*)cam + 0x100);
    blk.v[1] = *(f32*)((u8*)cam + 0x104);
    blk.v[2] = *(f32*)((u8*)cam + 0x108);
    GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
    saved = (u8*)&lbl_803A6818 + 0x218;
    GSvecCopy(saved, &blk.v[3]);
    GSvecCopy(saved + 0xc, &blk.v[0]);
    *(f32*)(saved + 0x18) = persp0;
    *(f32*)(saved + 0x1c) = persp1;
    *(f32*)(saved + 0x20) = persp2;
    *(f32*)(saved + 0x24) = persp3;
    GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
    GSvecCopy((u8*)&lbl_803A6818 + 0x118, &blk.v[3]);
    GSvecCopy((u8*)&lbl_803A6818 + 0x124, &rot);
    GSvecCopy((u8*)&lbl_803A6818 + 0x130, &blk.v[0]);
    *(f32*)((u8*)&lbl_803A6818 + 0x13c) = persp0;
    *(f32*)((u8*)&lbl_803A6818 + 0x140) = persp1;
    *(f32*)((u8*)&lbl_803A6818 + 0x144) = persp2;
    *(f32*)((u8*)&lbl_803A6818 + 0x148) = persp3;
    cameraPlayAnime(0x17, 0xd731800, 0, 1);
    GSscene_SetMode(4);
    *(s32*)((u8*)&lbl_803A6818 + 0x78) = 0;
    menuModelInit((u8*)&lbl_803A6818 + 0x7c, 0x280, 0x1e0);
    menuModelInit((u8*)&lbl_803A6818 + 0xc4, 0x280, 0x1e0);
    lbl_8047A4E0 = 0;
    lbl_8047A4E0 = (u32)pdaAlloc(0x140, 0x20);
    lbl_804788C0 = memoDataGetCount(0);
    lbl_8047A4E4 = (u16*)pdaAlloc(0x400, 0x20);
    cf0 = lbl_8047BCF0;
    c94 = lbl_8047BC94;
    cbc = lbl_8047BCBC;
    *(f32*)((u8*)&lbl_803A6818 + 0x2c) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x30) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x34) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x38) = c94;
    *(s32*)((u8*)&lbl_803A6818 + 0x0) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x28) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x14) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x4) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x20) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x44) = c94;
    *(s8*)((u8*)&lbl_803A6818 + 0x48) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = cf0;
    *(f32*)((u8*)&lbl_803A6818 + 0x68) = cf0;
    *(f32*)((u8*)&lbl_803A6818 + 0x6c) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x70) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x74) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = cf0;
    *(s32*)((u8*)&lbl_803A6818 + 0x1c) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x4c) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x50) = cbc;
    *(f32*)((u8*)&lbl_803A6818 + 0x58) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x54) = cbc;
    *(f32*)((u8*)&lbl_803A6818 + 0x5c) = cbc;
    *(f32*)((u8*)&lbl_803A6818 + 0x60) = cbc;
    lbl_804788C4 = 5;
    *(s8*)((u8*)&lbl_803A6818 + 0x158) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x159) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15b) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15a) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15c) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15d) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15f) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x15e) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x154) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x1e0) = lbl_8047BCCC;
    *(f32*)((u8*)&lbl_803A6818 + 0x1dc) = lbl_8047BCCC;
    *(f32*)((u8*)&lbl_803A6818 + 0x1cc) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d0) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d4) = c94;
    *(f32*)((u8*)&lbl_803A6818 + 0x1d8) = c94;
    *(s8*)((u8*)&lbl_803A6818 + 0x164) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x178) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x18c) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x1a0) = 0;
    *(s8*)((u8*)&lbl_803A6818 + 0x1b4) = 0;
    *(s32*)((u8*)&lbl_803A6818 + 0x1e4) = 0;
    pdaMemoLoadPokemon();
    if (lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
        }
        *((u8*)&lbl_803A6818 + 0x214) = 0;
        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
        case 0xb:
        case 0xc:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
            *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
            break;
        default:
            break;
        }
    }
    menuPokemonModelRequestById((u8*)&lbl_803A6818 + 0xc4, 0xf70400);
    switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 8:
    case 0xb:
    case 0xc:
        *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
        break;
    case 5:
        *((u8*)&lbl_803A6818 + 0x214) = pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }
    peopleInfoGetMotion(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &out8);
    menuModelSetMotion((u8*)&lbl_803A6818 + 0xc4, motion);
    total = lbl_8047A4E8;
    *(f32*)((u8*)&lbl_803A6818 + 0x40) = lbl_8047BC94;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    *(s32*)((u8*)&lbl_803A6818 + 0x14c) = 0;
}
#pragma peephole reset

extern s8 lbl_804788D4[];
extern void* menuDataBiosGetPtr(s32 id);
extern void* windowGetKeyInfo(void* window);
extern void menuPlaySe(s32 id, s32 se);

/* Which saved-view slot the current page/sub-page pair uses. */
static inline s32 pdaViewSlot(u8* L)
{
    s32 slot = 0;

    switch (*(s8*)(L + 9)) {
    case 0:
        break;
    case 1:
        slot = 1;
        break;
    case 2:
        if (*(s8*)(L + 0xa) == 0) {
            slot = 2;
        } else {
            slot = 3;
        }
        break;
    case 3:
        slot = 4;
        break;
    default:
        break;
    }
    return slot;
}

/* Reset the carousel to the top of a freshly rebuilt memo list. */
static inline void pdaResetCarousel(u16 total)
{
    *(s32*)((u8*)&lbl_803A6818 + 0x0) = 0;
    *(f32*)((u8*)&lbl_803A6818 + 0x2c) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x30) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x34) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x38) = lbl_8047BC94;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
}

/* Confirm / cancel handling for the PDA's filter and sort menus. */
#pragma peephole off
void pdaMemoMainKey(void* menu)
{
    u8* S = (u8*)&lbl_803A6818;
    u8* L = (u8*)&lbl_803A6818 + 0x154;
    u16 keys;
    u16 total;
    s32 page;
    s32 slot;
    u8* view;
    f32 delta;
    f32 moved;
    f32 rest;

    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x4c),
            *(f32*)((u8*)&lbl_803A6818 + 0x50), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x54),
            *(f32*)((u8*)&lbl_803A6818 + 0x58), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x5c),
            *(f32*)((u8*)&lbl_803A6818 + 0x60), lbl_8047BCC0);
    if (*(f32*)(L + 0x88) != *(f32*)(L + 0x8c)) {
        delta = *(f32*)(L + 0x8c) - *(f32*)(L + 0x88);
        moved = lbl_8047BCC4 * delta * *(f32*)((u8*)&lbl_803A6818 + 0x3c);
        if (moved > lbl_8047BC98) {
            moved = lbl_8047BC98;
        }
        if (moved <= lbl_8047BCC8) {
            moved = lbl_8047BCC8;
        }
        *(f32*)(L + 0x88) = *(f32*)(L + 0x88) + moved;
        rest = *(f32*)(L + 0x8c) - *(f32*)(L + 0x88);
        if (moved > lbl_8047BC94) {
        } else {
            moved = -moved;
        }
        if (pdaFabs(rest) <= moved) {
            *(f32*)(L + 0x88) = *(f32*)(L + 0x8c);
        } else {
            if (rest > lbl_8047BC94) {
            } else {
                rest = -rest;
            }
            if (rest < lbl_8047BCBC) {
                *(f32*)(L + 0x88) = *(f32*)(L + 0x8c);
            }
        }
    }
    if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4c)) {
        return;
    }
    if (menu == NULL) {
        return;
    }
    keys = *(u16*)((u8*)windowGetKeyInfo(
                       menuDataBiosGetPtr(*(s32*)((u8*)menu + 4))) +
                   4);
    if ((keys & 0x10) != 0) {
        soundStartRaw(0x24, 0, 0);
        page = *(s8*)(L + 9);
        if (page == 4) {
            switch (*(s32*)(L + 0x90)) {
            case 0:
                if (pdaMemoLoadPokemon() != 0) {
                    total = lbl_8047A4E8;
                    *(s32*)(L + 0x90) = 1;
                    pdaResetCarousel(total);
                } else {
                    *(s32*)(L + 0x90) = 2;
                }
                break;
            case 1:
                *(s8*)((u8*)menu + 0x98) = 1;
                *(s8*)((u8*)menu + 0x99) = 1;
                if (lbl_804788C0 != 0) {
                    {
                        u32 pokemon =
                            pdaLoadPokemon(lbl_803A6818.currentIndex);
                        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
                            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
                            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
                        }
                        *((u8*)&lbl_803A6818 + 0x214) = 0;
                        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                        case 0xb:
                        case 0xc:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
                            break;
                        case 5:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c,
                                            (u8*)&lbl_803A6818 + 0xc4);
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case 2:
                *(s32*)(L + 0x90) = 0;
                break;
            default:
                break;
            }
        } else {
            switch (*(s32*)L) {
            case 0:
                if (lbl_8047BCCC == *(f32*)(L + 0x88)) {
                    *(s8*)(L + 0xc) = lbl_804788D4[page];
                    *(s32*)L = 1;
                    *(f32*)(L + 0x8c) = lbl_8047BC94;
                    switch (page) {
                    case 0:
                        L[0xb] = L[4];
                        break;
                    case 1:
                        L[0xb] = L[5];
                        break;
                    case 2:
                        L[0xb] = *(u8*)(L + *(s8*)(L + 0xa) + 6);
                        break;
                    case 3:
                        L[0xb] = L[8];
                        break;
                    default:
                        break;
                    }
                    if (*(s8*)(L + 0xc) >= 10) {
                        if ((s8)(L[0xb] - 9) < 0) {
                            L[0x74] = 0;
                        }
                        L[0x75] = 10;
                        L[0x75] = (s8)(L[0x74] + 10);
                        *(f32*)(L + 0x7c) = (f32)(*(s8*)(L + 0x74) * -0x1d);
                        *(f32*)(L + 0x84) = (f32)(*(s8*)(L + 0x74) * -0x1d);
                    } else {
                        L[0x74] = 0;
                        L[0x75] = L[0xc];
                        *(f32*)(L + 0x7c) = lbl_8047BC94;
                        *(f32*)(L + 0x84) = lbl_8047BC94;
                    }
                    slot = pdaViewSlot(L);
                    view = L + slot * 0x14 + 0x10;
                    if (*(s8*)view != 0) {
                        L[0x74] = view[1];
                        L[0x75] = view[2];
                        *(f32*)(L + 0x78) = *(f32*)(view + 4);
                        *(f32*)(L + 0x7c) = *(f32*)(view + 8);
                        *(f32*)(L + 0x80) = *(f32*)(view + 0xc);
                        *(f32*)(L + 0x84) = *(f32*)(view + 0x10);
                    }
                    menuPlaySe(*(s32*)((u8*)menu + 4), 2);
                }
                break;
            case 1:
                if (lbl_8047BC94 == *(f32*)(L + 0x88)) {
                    switch (page) {
                    case 0:
                        L[4] = L[0xb];
                        break;
                    case 1:
                        L[5] = L[0xb];
                        break;
                    case 2:
                        *(u8*)(L + *(s8*)(L + 0xa) + 6) = L[0xb];
                        break;
                    case 3:
                        L[8] = L[0xb];
                        break;
                    default:
                        break;
                    }
                    slot = pdaViewSlot(L);
                    view = L + slot * 0x14 + 0x10;
                    view[0] = 1;
                    view[1] = L[0x74];
                    view[2] = L[0x75];
                    *(f32*)(view + 4) = *(f32*)(L + 0x78);
                    *(f32*)(view + 8) = *(f32*)(L + 0x7c);
                    *(f32*)(view + 0xc) = *(f32*)(L + 0x80);
                    *(f32*)(view + 0x10) = *(f32*)(L + 0x84);
                    *(s32*)L = 0;
                    *(f32*)(L + 0x8c) = lbl_8047BCCC;
                    menuPlaySe(*(s32*)((u8*)menu + 4), 2);
                }
                break;
            default:
                break;
            }
        }
    }
    if ((keys & 0x20) != 0) {
        soundStartRaw(0x25, 0, 0);
        switch (*(s32*)L) {
        case 0:
            if (lbl_8047BCCC == *(f32*)(L + 0x88)) {
                if (*(s32*)(L + 0x90) != 1) {
                    pdaMemoBuildList();
                    total = lbl_8047A4E8;
                    pdaResetCarousel(total);
                    if (lbl_804788C0 != 0) {
                        {
                        u32 pokemon =
                            pdaLoadPokemon(lbl_803A6818.currentIndex);
                        if (menuPokemonModelIsLoaded((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
                            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
                            menuPokemonModelRequest((u8*)&lbl_803A6818 + 0x7c, pokemon);
                        }
                        *((u8*)&lbl_803A6818 + 0x214) = 0;
                        switch (*(s32*)((u8*)&lbl_803A6818 + 0x1c)) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                        case 0xb:
                        case 0xc:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
                            break;
                        case 5:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                pdaModelFrameCamera((u8*)&lbl_803A6818 + 0x7c,
                                            (u8*)&lbl_803A6818 + 0xc4);
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
                            break;
                        default:
                            break;
                        }
                    }
                    }
                    *((u8*)&lbl_803A6818 + 0x214) =
                        pdaModelFrameSingle((u8*)&lbl_803A6818 + 0x7c);
                    *(s8*)((u8*)menu + 0x98) = 1;
                    *(s8*)((u8*)menu + 0x99) = 1;
                    menuPlaySe(*(s32*)((u8*)menu + 4), 3);
                }
            }
            break;
        case 1:
            if (lbl_8047BC94 == *(f32*)(L + 0x88)) {
                *(s32*)L = 0;
                *(f32*)(L + 0x8c) = lbl_8047BCCC;
                menuPlaySe(*(s32*)((u8*)menu + 4), 2);
            }
            break;
        default:
            break;
        }
    }
}
#pragma peephole reset

/* Reload the memo id table, clear the list filters and refresh the list. */
#pragma peephole off
u16 pdaMemoBuildList(void)
{
    extern u16* lbl_8047A4E4;
    extern u16 lbl_8047A4E8;
    extern void pdaMemoDrawData(void);
    u8* work;

    work = (u8*)&lbl_803A6818;
    lbl_8047A4E8 = pda_8003D818_LoadIds();
    work[0x158] = 0;
    work[0x159] = 0;
    work[0x15b] = 0;
    work[0x15a] = 0;
    work[0x15c] = 0;
    pdaMemoDrawData();
    lbl_803A6818.field_10 = lbl_8047A4E8;
    return lbl_8047A4E8;
}
#pragma peephole reset

typedef struct PdaCryTrail {
    void* data;
    f32 speed;
    u32 voice;
    f32 field_0c;
    s32 offset;
    f32 field_14;
    s32 active;
    f32 alpha;
    f32 field_20;
} PdaCryTrail;

extern u32 lbl_8047A4EC;
extern f32 lbl_8047BD1C;
extern u16 pokemonDataBiosGetVoice(void* data);
extern void* pokemonNakigoeDataBiosGetDataAddress(void* data);
extern s32 GSsndGetStatus(u32 voice);
extern u32 GSinputButtonsChanged(s32 chan);
extern s32 menuOpen(s32 id, s32 flag);

/* Cry voice of the highlighted entry, 0xffff when it cannot be resolved. */
static inline u16 pdaCurrentVoice(s32 index)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemon(index);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetVoice(data);
            }
            return 0xffff;
        }
        return 0xffff;
    }
    return 0xffff;
}

/* Cry sample of the highlighted entry, NULL when it cannot be resolved. */
static inline void* pdaCurrentCry(s32 index)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemon(index);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonNakigoeDataBiosGetDataAddress(data);
            }
            return NULL;
        }
        return NULL;
    }
    return NULL;
}

/* Push a cry onto the ring of trails and age every older entry. */
#define PDA_CRY(n) (((PdaCryTrail*)lbl_802E52C8)[n])
static inline void pdaPushCry(void* data, u32 voice)
{
    s32 i;
    s32 j;

    lbl_8047A4EC = lbl_8047A4EC + 1;
    if (lbl_8047A4EC >= 8) {
        lbl_8047A4EC = 0;
    }
    i = lbl_8047A4EC;
    PDA_CRY(i).data = data;
    PDA_CRY(i).voice = voice;
    PDA_CRY(i).field_0c = lbl_8047BC94;
    PDA_CRY(i).field_14 = lbl_8047BC94;
    PDA_CRY(i).offset = 0;
    PDA_CRY(i).active = 1;
    PDA_CRY(i).alpha = lbl_8047BCA0;
    PDA_CRY(i).field_20 = lbl_8047BD18;
    for (j = i - 1; j >= 0; j--) {
        PDA_CRY(j).offset = (s32)((f32)PDA_CRY(j).offset + PDA_CRY(j).speed);
        PDA_CRY(j).field_20 += lbl_8047BD1C;
    }
    for (j = 7; j > lbl_8047A4EC; j--) {
        PDA_CRY(j).offset = (s32)((f32)PDA_CRY(j).offset + PDA_CRY(j).speed);
        PDA_CRY(j).field_20 += lbl_8047BD1C;
    }
}

/* Fill the cry ring with empty trails. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its loop counter is an inline local, numbered after the trail push's locals, which gives retail's registers for the ring reset - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80046168_ClearCries(void)
{
    s32 k;

    for (k = 0; k < 8; k++) {
        pdaPushCry(NULL, 0);
    }
}

/* 1 while any trail's cry is still playing. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; returning the flag gives retail's shared zero for the flag and the loop counter - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_80046168_AnyPlaying(void)
{
    PdaCryTrail* t;
    s32 i;
    s32 playing;

    t = (PdaCryTrail*)lbl_802E52C8;
    playing = 0;
    for (i = 0; i < 8; i++) {
        if (t->data != NULL) {
            switch (GSsndGetStatus(t->voice)) {
            case 0:
                break;
            case 2:
                playing = 1;
                break;
            case 4:
                break;
            default:
                break;
            }
        }
        t++;
    }
    return playing;
}

/* 1 when the stick or the cancel buttons leave the cry screen. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its early returns give retail's shared join for the cancel test - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_80046168_StickCancel(void)
{
    /* Retail converts the stick values to single precision (fsubs) for this
     * call, so this caller saw atan2 declared with float parameters. */
    extern f64 atan2(f32 y, f32 x);
    extern s8 GSinputGetLeftStickYData(s32 chan, s32 index);
    extern s8 GSinputGetLeftStickXData(s32 chan, s32 index);
    s8 x;
    s8 y;
    f32 ang;

    if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x8) != 0) {
        return 1;
    }
    if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x4) != 0) {
        return 1;
    }
    x = GSinputGetLeftStickYData(1, 0);
    y = GSinputGetLeftStickXData(1, 0);
    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
        ang = atan2(y, x);
        if (pda_800439BC_Fabs(ang) < lbl_8047BCFC) {
            return 1;
        }
        if (pda_800439BC_Fabs(ang) > lbl_8047BD00) {
            return 1;
        }
    }
    return 0;
}

/* Cry-playback thread for the PDA's Pokemon entry. */
#pragma peephole off
void pdaStrategyMemoUpdate(void)
{
    extern void _threadSwitch(void);
    u8* S;
    s32 running;
    u16 voice;

    running = 1;
    pda_80046168_ClearCries();
    S = (u8*)&lbl_803A6818;
    while (1) {
        switch (*(s32*)(S + 0x20)) {
        case 0:
            menuOpen(0x3c, 0);
            menuOpen(0x3f, 0);
            menuOpen(0x3d, 0);
            menuOpen(0x9f, 0);
            menuOpen(0x96, 0);
            if (menuOpen(0xa2, 1) >= 0) {
                if (pdaEntrySeen() != 0) {
                    menuOpen(0x3e, 0);
                    voice = pdaCurrentVoice(lbl_803A6818.currentIndex);
                    soundStartRaw(voice, 0, 0);
                    pdaPushCry(pdaCurrentCry(lbl_803A6818.currentIndex), voice);
                    *(s32*)(S + 0x20) = 1;
                }
            } else {
                if (*(s32*)(S + 0x1c) == 2) {
                    *(s32*)(S + 0x1c) = 8;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                }
                *(s32*)(S + 0x14) = 0;
                running = 0;
            }
            break;
        case 1:
            if (pda_80046168_AnyPlaying() == 0 || pda_80046168_StickCancel() == 1) {
                menuCloseCustom(0x3e, 0, 1);
                *(s32*)(S + 0x20) = 0;
            } else {
                if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x100) != 0) {
                    voice = pdaCurrentVoice(lbl_803A6818.currentIndex);
                    soundStartRaw(voice, 0, 0);
                    pdaPushCry(pdaCurrentCry(lbl_803A6818.currentIndex), voice);
                }
                if ((GSinputGetButtons(1) & GSinputButtonsChanged(1) & 0x200) != 0) {
                    if (*(s32*)(S + 0x1c) == 2) {
                        *(s32*)(S + 0x1c) = 8;
                        menuCloseCustom(0x3e, 0, 1);
                        menuCloseCustom(0x3c, 0, 1);
                        *(f32*)(S + 0x50) = lbl_8047BC94;
                        *(f32*)(S + 0x58) = lbl_8047BCBC;
                    }
                    running = 0;
                }
            }
            _threadSwitch();
            break;
        default:
            break;
        }
        if (running == 0) {
            break;
        }
    }
    *(s32*)(S + 0x20) = 0;
}
#pragma peephole reset

extern f32 lbl_8047BDA0;
extern f32 lbl_8047BDA4;
extern f32 lbl_8047BDA8;
extern f32 lbl_8047BDB0;
extern f32 lbl_8047BDB4;
extern f32 lbl_8047BDB8;

static inline void pda_800495C8_Scanline(PdaSprite* sprite, s16 y, s32 alpha)
{
    extern u8 lbl_80314E08[];
    extern void gfxSetLineWidth(f32 width);
    extern void gfxVtxColor1u32(s32 index, u32 color);
    extern void gfxVtxPos2s16(s16 x, s16 y);
    u32 color;

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetVtxDesc(lbl_80314E08);
    gfxSetPrimitive(1);
    gfxSetLineWidth(1.0f);
    gfxBegin(2);
    gfxVtxPos2s16(0, y);
    color = alpha | 0x00800000;
    gfxVtxColor1u32(0, color);
    gfxVtxPos2s16(sprite->x, y);
    gfxVtxColor1u32(0, color);
    gfxEnd();
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_800495C8_RowLine (pdaTopMenuDrawItem) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_800495C8_RowLine(PdaSprite* sprite, f32 y, s32 alpha)
{
    s16 row = 480.0f * y;

    pda_800495C8_Scanline(sprite, row, alpha);
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_800495C8_Panel (pdaTopMenuDrawItem) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_800495C8_Panel(PdaSprite* sprite, s32 alpha)
{
    extern void gfxVtxColor1u32(s32 index, u32 color);
    extern void gfxVtxPos2s16(s16 x, s16 y);
    u32 color;

    gfxVtxPos2s16(0, 0);
    color = (u8)alpha | 0x03140e00;
    gfxVtxColor1u32(0, color);
    gfxVtxPos2s16(sprite->x, 0);
    gfxVtxColor1u32(0, color);
    gfxVtxPos2s16(0, sprite->y);
    gfxVtxColor1u32(0, color);
    gfxVtxPos2s16(sprite->x, sprite->y);
    gfxVtxColor1u32(0, color);
    gfxEnd();
}

/* Mail-list backdrop: fade panel, scanlines, and the sheared page edge. */
#pragma peephole off
s32 pdaTopMenuDrawItem(void* work, PdaSprite* sprite)
{
    extern u8 lbl_80314E08[];
    extern void gfxVtxColor1u32(s32 index, u32 color);
    extern void gfxVtxPos2s16(s16 x, s16 y);
    u8* G = lbl_803A6A60;
    u8* data;
    f32 top;
    f32 y;
    u8 fade;
    s32 j;
    s32 i;
    s32 alpha;
    f32 step;

    data = *(u8**)((u8*)work + 0x60);
    if (*(f32*)(G + 0x2c) != *(f32*)(G + 0x28)) {
        step = *(f32*)(G + 8) / 0.5f;
        if (*(f32*)(G + 0x2c) > *(f32*)(G + 0x28)) {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) + step;
            if (*(f32*)(G + 0x28) > 1.0f) {
                *(f32*)(G + 0x28) = 1.0f;
            }
        } else {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) - step;
            if (*(f32*)(G + 0x28) < 0.0f) {
                *(f32*)(G + 0x28) = 0.0f;
            }
        }
    }
    pdaSceneGetWork();
    if (*(s8*)(lbl_803A6A60 + 0) == 1) {
        if (lbl_803A6A60[0x49] != 0 && lbl_803A6A60[0x4a] == 0) {
            menuOffScreenSetDisp(0);
            lbl_803A6A60[0x49] = 0;
        }
        if (*(s8*)(lbl_803A6A60 + 1) >= 1) {
            *(f32*)(G + 0x2c) = 0.0f;
            return 0;
        }
        *(f32*)(G + 0x2c) = 1.0f;
    } else {
        if (lbl_803A6A60[0x49] != 1) {
            menuOffScreenSetDisp(1);
            lbl_803A6A60[0x49] = 1;
        }
        *(f32*)(G + 0x2c) = 1.0f;
    }

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetVtxDesc(lbl_80314E08);
    gfxSetPrimitive(4);
    gfxBegin(4);
    alpha = (s32)(255.0f * *(f32*)(G + 0x28));
    pda_800495C8_Panel(sprite, alpha);

    for (i = 0; i < 0x1e0; i += 4) {
        pda_800495C8_Scanline(sprite, i, 0x80);
    }

    top = **(f32**)(data + 8);
    for (j = 0; j < 12; j++) {
        fade = (s32)(255.0f * (1.0f - (f32)j / 12.0f));
        y = top - 4.0f * (f32)j / 480.0f;
        if (y < 0.0f) {
            y += 1.0f;
        }
        /* RULE-EXCEPTION(user-approved): redundant mask on the u8 row alpha, chosen for code generation (pdaTopMenuDrawItem) - see docs/RULE_EXCEPTIONS.md */
        pda_800495C8_RowLine(sprite, y, fade & 0xff);
    }
    return 0;
}
#pragma peephole reset

extern f32 lbl_8047BDD8;
extern f32 lbl_8047BDDC;
extern f32 lbl_8047BDE0;
extern f32 lbl_8047BDE4;
extern u8 std081_flagCheckTrue(s32 id);
extern void menuItemBiosSetSelectFlag(s32 id, s32 flag);

/* Ease the widgets' horizontal offset toward its target: 10 x the gap per
 * second, at most 20 per frame, snapping once the remaining gap is within the
 * step or under 1. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8004B278_EaseX (pdaTopMenuCtrl) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8004B278_EaseX(f32* cur, f32* target)
{
    f32 tgt;
    f32 delta;
    f32 rest;
    f32 moved;

    if (*cur != *target) {
        delta = *target - *cur;
        delta = 10.0f * delta;
        moved = delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > 20.0f) {
            moved = 20.0f;
        }
        if (moved <= -20.0f) {
            moved = -20.0f;
        }
        *cur += moved;
        tgt = *target;
        rest = tgt - *cur;
        moved = moved > 0.0f ? moved : -moved;
        if ((rest > 0.0f ? rest : -rest) <= moved || (rest > 0.0f ? rest : -rest) < 1.0f) {
            *cur = tgt;
        }
    }
}

/* The same easing for the vertical offset. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8004B278_EaseY (pdaTopMenuCtrl) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8004B278_EaseY(f32* cur, f32* target)
{
    f32 delta;
    f32 rest;
    f32 moved;
    f32 speed;
    f32 tgt;

    if (*cur != *target) {
        delta = *target - *cur;
        delta = 10.0f * delta;
        moved = delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > 20.0f) {
            moved = 20.0f;
        }
        if (moved <= -20.0f) {
            moved = -20.0f;
        }
        *cur += moved;
        tgt = *target;
        rest = tgt - *cur;
        if (moved > 0.0f) {
            speed = moved;
        } else {
            speed = -moved;
        }
        if ((rest > 0.0f ? rest : -rest) <= speed || (rest > 0.0f ? rest : -rest) < 1.0f) {
            *cur = tgt;
        }
    }
}

/* Per-frame timing, offset easing and background scroll for the mail screen. */
#pragma peephole off
s32 pdaTopMenuCtrl(u8* work)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u8* data;

    data = *(u8**)(work + 0x60);
    *(f32*)((u8*)&lbl_803A6A60 + 8) = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    if (std081_flagCheckTrue(0x3f0) != 0) {
        menuItemBiosSetSelectFlag(0x438, 1);
    } else {
        menuItemBiosSetSelectFlag(0x438, 0);
    }
    *(f32*)(lbl_803A6A60 + 0x40) =
        0.5f * *(f32*)((u8*)&lbl_803A6A60 + 8) + *(f32*)(lbl_803A6A60 + 0x40);
    if (*(f32*)(lbl_803A6A60 + 0x40) > 1.0f) {
        *(f32*)(lbl_803A6A60 + 0x40) = 0.0f;
    }

    pda_8004B278_EaseX((f32*)(lbl_803A6A60 + 0x30), (f32*)(lbl_803A6A60 + 0x38));
    pda_8004B278_EaseY((f32*)(lbl_803A6A60 + 0x34), (f32*)(lbl_803A6A60 + 0x3c));

    switch (*(s8*)(work + 1)) {
    case 0:
        if (*(s8*)(work + 2) == 0) {
            **(f32**)(data + 8) = 0.0f;
            work[2] = 1;
        }
        break;
    case 2:
        if ((**(f32**)(data + 8) += 0.003125f) >= 1.0f) {
            **(f32**)(data + 8) -= 1.0f;
        }
        break;
    case 3:
        if (*(s8*)(work + 2) == 0) {
            work[2] = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
#pragma peephole reset

typedef struct PdaPairS16 {
    s16 x;
    s16 y;
} PdaPairS16;

/* lbl_802EF0A8 menu item records, 0x1C bytes each. */
typedef struct PdaMenuItem {
    s16 id;
    s16 x;
    s16 y;
    u8 pad06[0x16];
} PdaMenuItem;

typedef struct PdaSpan12 {
    s16 v[12];
} PdaSpan12;

extern PdaPairS16 lbl_8047BD78;
extern PdaPairS16 lbl_8047BD7C;
extern PdaPairS16 lbl_8047BD80;
extern PdaPairS16 lbl_8047BD84;
extern PdaPairS16 lbl_8047BD88;
extern PdaPairS16 lbl_8047BD8C;
extern PdaPairS16 lbl_8047BD90;
extern PdaPairS16 lbl_8047BD94;
extern PdaSpan12 lbl_8026719C;
extern PdaSpan12 lbl_802671B4;

/* Re-point the mail window layout tables at the wide or narrow variant. */
#pragma peephole off
void pdaTrainerDataLayout(void)
{
    PdaSpan12 wide;
    PdaSpan12 narrow;
    PdaPairS16 topWide[2];
    PdaPairS16 topNarrow[2];
    PdaPairS16 botWide[2];
    PdaPairS16 botNarrow[2];

    topWide[0] = lbl_8047BD78;
    topWide[1] = lbl_8047BD7C;
    topNarrow[0] = lbl_8047BD80;
    topNarrow[1] = lbl_8047BD84;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8;

        items[0x437].y = topWide[0].x - 6;
        items[0x438].y = topWide[0].y - 6;
        items[0x439].y = topWide[1].x - 6;
        items[0x43a].y = topWide[1].y - 6;
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8;

        items[0x437].y = topNarrow[0].x - 6;
        items[0x438].y = topNarrow[0].y - 6;
        items[0x439].y = topNarrow[1].x - 6;
        items[0x43a].y = topNarrow[1].y - 6;
    }

    wide = lbl_8026719C;
    narrow = lbl_802671B4;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e7;

        items[0].y = wide.v[0];
        items[1].y = wide.v[1];
        items[2].y = wide.v[2];
        items[0x76].y = wide.v[3];
        items[0x77].y = wide.v[4];
        items[0x78].y = wide.v[5];
        items[0x79].y = wide.v[6];
        items[0x7a].y = wide.v[7];
        items[0x7b].y = wide.v[8];
        items[0x7c].y = wide.v[9];
        items[0x7d].y = wide.v[10];
        items[0x7e].y = wide.v[11];
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e7;

        items[0].y = narrow.v[0];
        items[1].y = narrow.v[1];
        items[2].y = narrow.v[2];
        items[0x76].y = narrow.v[3];
        items[0x77].y = narrow.v[4];
        items[0x78].y = narrow.v[5];
        items[0x79].y = narrow.v[6];
        items[0x7a].y = narrow.v[7];
        items[0x7b].y = narrow.v[8];
        items[0x7c].y = narrow.v[9];
        items[0x7d].y = narrow.v[10];
        items[0x7e].y = narrow.v[11];
    }

    botWide[0] = lbl_8047BD88;
    botWide[1] = lbl_8047BD8C;
    botNarrow[0] = lbl_8047BD90;
    botNarrow[1] = lbl_8047BD94;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e3;

        items[0].y = botWide[0].x - 6;
        items[1].y = botWide[0].y - 6;
        items[2].y = botWide[1].x - 6;
        items[3].y = botWide[1].y - 6;
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e3;

        items[0].y = botNarrow[0].x - 6;
        items[1].y = botNarrow[0].y - 6;
        items[2].y = botNarrow[1].x - 6;
        items[3].y = botNarrow[1].y - 6;
    }
}
#pragma peephole reset

extern PdaPairS16 lbl_8047BD70;
extern PdaPairS16 lbl_8047BD74;
extern PdaPairS16 lbl_8047BD98;
extern PdaPairS16 lbl_8047BD9C;
extern f32 lbl_8047BDC8;
extern f32 lbl_8047BDCC;
extern s32 lbl_804788F0;
extern u32 lbl_802E61D8[];
extern u32 heroGetStatus(s32 a, s32 b, s32 c);
extern void menuSpriteBiosGetPtr(s32 id);
extern u32 menuGetCursorItemID(s32 menu);
extern void pdaTrainerDataAnimItem(void* work, PdaSprite* sprite);

/* Show a header tab and centre it on its measured width plus the tab's wave offset. */
static inline void pda_800499BC_PlaceTab(PdaSprite* sprite, s32 width, s16 wave)
{
    s16 left;

    sprite->flags |= 2;
    left = width - 0x36;
    left += wave;
    sprite->field_50 = left + 0xa8;
}

/* Per-widget update for the mail screen's sprites and captions. */
#pragma peephole off
s32 pdaTrainerDataDraw(void* work, PdaSprite* sprite)
{
    extern u32 gamedataGetPlaytime(void);
    extern f64 sin(f64 x);
    extern void windowDrawSprite2(s16 x, s16 y, s16 w, s16 h, u32 color,
                                  void* work, s32 id, s32 flag);
    PdaPairS16 wave[2];
    s32* rank;
    s32 idx;
    PdaPairS16 icon[2];
    s16* pos;
    s32 i;
    s32 id;
    u16 cursor;
    s32 msg;
    u8* row;
    u32 value;
    s32 n;
    s32 x;
    u8* layout;
    u32 digit;
    s16 left;
    u32 status;

    pdaTrainerDataAnimItem(work, sprite);
    switch (sprite->eventId) {
    case 0x6e1:
        icon[0] = lbl_8047BD70;
        icon[1] = lbl_8047BD74;
        pos = (s16*)(lbl_802EF0A8 + 0xc09e);
        status = heroGetStatus(0, 0xe, 0);
        for (i = lbl_804788F0 - 1; i >= 0; i--) {
            if (lbl_802E61D8[i] <= status) {
                break;
            }
        }
        if (i < 0) {
            i = 0;
        }
        rank = &idx;
        *rank = i;
        if (i > 4) {
            *rank = 0;
        }
        id = ((u16*)icon)[*rank];
        menuSpriteBiosGetPtr(id);
        windowDrawSprite2(
            (s16)((s32)*(f32*)(lbl_803A6A60 + 0x30) +
                  (pos[0] - sprite->field_50)),
            (s16)(pos[1] - sprite->field_52),
            sprite->x, sprite->y, sprite->alpha | -0x100, work, id, 0);
        break;
    case 0x435:
        row = *(u8**)((u8*)work + 0x60);
        sprite->messageId = **(s32**)(row + 4);
        sprite->colorR = row[0];
        sprite->colorG = row[1];
        sprite->colorB = row[2];
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 1:
            if (*(s8*)(lbl_803A6A60 + 1) == 2) {
                sprite->flags &= ~2;
            } else {
                sprite->flags |= 2;
            }
            break;
        }
        break;
    case 0x433:
    case 0x436:
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 1:
            if (*(s8*)(lbl_803A6A60 + 1) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
            break;
        default:
            sprite->flags &= ~2;
            break;
        }
        break;
    case 0x438:
        if (std081_flagCheckTrue(0x3f0) != 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        *(s32*)(lbl_803A6A60 + 0x1c) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x437:
        *(s32*)(lbl_803A6A60 + 0x18) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x43a:
        *(s32*)(lbl_803A6A60 + 0x24) = GSmsgGetRect(sprite->messageId) >> 16;
        cursor = menuGetCursorItemID(*(s32*)((u8*)work + 4));
        if (std081_flagCheckTrue(0x3f0) != 0) {
            switch (cursor) {
            case 0x437:
                lbl_803A6A60[0] = 0;
                break;
            case 0x438:
                lbl_803A6A60[0] = 1;
                break;
            case 0x439:
                lbl_803A6A60[0] = 2;
                break;
            case 0x43a:
                lbl_803A6A60[0] = 3;
                break;
            default:
                break;
            }
        } else {
            switch (cursor) {
            case 0x437:
                lbl_803A6A60[0] = 0;
                break;
            case 0x438:
                lbl_803A6A60[0] = 1;
                break;
            case 0x439:
                lbl_803A6A60[0] = 2;
                break;
            case 0x43a:
                lbl_803A6A60[0] = 3;
                break;
            default:
                break;
            }
        }
        break;
    case 0x439:
        *(s32*)(lbl_803A6A60 + 0x20) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x75d:
    case 0x75e:
    case 0x75f:
    case 0x760:
    case 0x761:
    case 0x762:
    case 0x763:
    case 0x764:
    case 0x765:
        pdaTrainerDataSetCursor((s32)work, sprite, sprite->eventId);
        break;
    case 0x6dd:
    case 0x6e2:
        break;
    case 0x754:
        /* RULE-EXCEPTION(user-approved): scalar reuse of digit for the play time in fn_800499BC - see docs/RULE_EXCEPTIONS.md */
        digit = gamedataGetPlaytime();
        msgctrlSetValue(0x4c, digit);
        break;
    case 0x755:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xd, 0));
        break;
    case 0x756:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xc, 0));
        break;
    case 0x757:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        spriteSetEnv(spriteSetOrigin(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        value = heroGetStatus(0, 2, 0) & 0xffff;
        x = sprite->x - 0x20;
        for (n = 0; n < 5; n++) {
            x -= 0xd;
            digit = value % 10;
            value = value / 10;
            msgctrlSetValue(0x34, digit);
            GSmsgPrintColor(x + 0x24, 0, sprite->alpha | -0x100, (void*)0xca);
        }
        break;
    case 0x777:
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 0:
            msg = 0x36d6;
            break;
        case 1:
            msg = 0x36d7;
            break;
        case 2:
            msg = 0x36d8;
            break;
        case 3:
            msg = 0x36d9;
            break;
        default:
            msg = 1;
            break;
        }
        GSmsgPrintColor(-0x50, -4, sprite->alpha | -0x100, (void*)msg);
        break;
    case 0x6e3:
    case 0x6e4:
    case 0x6e5:
    case 0x6e6:
        wave[0] = lbl_8047BD98;
        wave[1] = lbl_8047BD9C;
        layout = lbl_802EF0A8 + sprite->eventId * 0x1c;
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 0:
            if (sprite->eventId == 0x6e3) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[0].x = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[0].x = 0;
            }
            wave[0].x -= 0x1e;
            break;
        case 1:
            if (sprite->eventId == 0x6e4) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[0].y = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[0].y = 0;
            }
            wave[0].y -= 0x1e;
            break;
        case 2:
            if (sprite->eventId == 0x6e5) {
                wave[1].x = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[1].x = 0;
            }
            wave[1].x -= 0x1e;
            break;
        case 3:
            if (sprite->eventId == 0x6e6) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[1].y = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[1].y = 0;
            }
            wave[1].y -= 0x1e;
            break;
        default:
            break;
        }
        if (sprite->eventId == 0x6e5) {
            sprite->flags |= 2;
            left = *(s32*)(lbl_803A6A60 + 0x20) - 0x36;
            left += wave[1].x;
            sprite->field_50 = left + 0xa8;
        }
        if (sprite->eventId == 0x6e3) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x18), wave[0].x);
        }
        if (sprite->eventId == 0x6e4) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x1c), wave[0].y);
        }
        if (sprite->eventId == 0x6e6) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x24), wave[1].y);
        }
        if (sprite->eventId == 0x6e4) {
            if (std081_flagCheckTrue(0x3f0) != 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}
#pragma peephole reset

extern f32 lbl_8047BDD0;
extern f32 lbl_8047BDD4;
extern f32 lbl_804788D8;

/* Fade the caption with the widgets' horizontal offset, then pulse its alpha
 * and grey level with a sine wave; `k` staggers the phase per caption. */
static inline void pda_8004A7A8_Pulse(PdaSprite* sprite, f32 k)
{
    extern f64 sin(f64 x);
    s32 fade;
    f32 pulse;

    if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
        fade = 0xff;
    } else {
        fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
    }
    sprite->alpha = 0xff - fade;
    pulse = sin(3.1415927f * (2.0f * *(f32*)(lbl_803A6A60 + 0x40)) + lbl_804788D8 * k);
    pulse += 1.0f;
    pulse *= 0.5f;
    pulse *= 256.0f;
    if (pulse > 255.0f) {
        pulse = 255.0f;
    }
    if (pulse < 0.0f) {
        pulse = 0.0f;
    }
    sprite->alpha = pulse;
    sprite->colorR = pulse;
    sprite->colorG = pulse;
    sprite->colorB = pulse;
}

/* Backdrop alpha ramp and the staggered caption pulses on the mail screen. */
#pragma peephole off
void pdaTrainerDataAnimItem(void* work, PdaSprite* sprite)
{
    s32 fade;
    s16* pos;

    switch (sprite->eventId) {
    case 0x6e0:
    case 0x6e2:
    case 0x751:
    case 0x752:
    case 0x753:
    case 0x754:
    case 0x755:
    case 0x756:
    case 0x757:
    case 0x758:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        spriteSetEnv(spriteSetOrigin(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        break;
    case 0x432:
    case 0x433:
    case 0x434:
    case 0x435:
    case 0x436:
    case 0x6dd:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_52 = pos[2] + (s32)*(f32*)(lbl_803A6A60 + 0x34);
        break;
    case 0x437:
    case 0x438:
    case 0x439:
    case 0x43a:
    case 0x777:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6ea:
        pda_8004A7A8_Pulse(sprite, 0.0f);
        break;
    case 0x6eb:
        pda_8004A7A8_Pulse(sprite, 1.0f);
        break;
    case 0x6ec:
        pda_8004A7A8_Pulse(sprite, 2.0f);
        break;
    case 0x6ed:
        pda_8004A7A8_Pulse(sprite, 3.0f);
        break;
    case 0x6ee:
        pda_8004A7A8_Pulse(sprite, 4.0f);
        break;
    case 0x6ef:
        pda_8004A7A8_Pulse(sprite, 0.0f);
        break;
    case 0x6f0:
        pda_8004A7A8_Pulse(sprite, 1.0f);
        break;
    case 0x6f1:
        pda_8004A7A8_Pulse(sprite, 2.0f);
        break;
    case 0x6f2:
        pda_8004A7A8_Pulse(sprite, 3.0f);
        break;
    case 0x6f3:
        pda_8004A7A8_Pulse(sprite, 4.0f);
        break;
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x6f4:
    case 0x6f5:
    case 0x6f6:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6e3:
    case 0x6e4:
    case 0x6e5:
    case 0x6e6:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6f7:
    case 0x6f8:
        sprite->flags &= ~2;
        break;
    default:
        break;
    }
}
#pragma peephole reset

extern f32 lbl_8047BACC;
extern f32 lbl_8047BAD0;
extern f32 lbl_8047BAD4;
extern f32 lbl_8047BAD8;
extern u32 lbl_8047A4D0;
extern s32 lbl_8047A4DC;
extern void darkPokemonSetFlag0C(u16 id, s32 flag);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);

/* Rebuild the preview Pokemon for the selected party entry. */
/* RULE-EXCEPTION(user-approved): reconstructed inline helper pda_8003C7C0_LoadPreview (fn_8003C7C0, fn_8003B85C) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003C7C0_LoadPreview(u32 work, u16 species, u16 entry, u16 rndId)
{
    u8 shadow;
    u8 snagged;
    s32 mode;
    u32 rnd;

    darkPokemonGetFlag1C(entry);
    darkPokemonCheckFlag16(entry);
    shadow = darkPokemonCheckFlag12(entry);
    snagged = darkPokemonCheckFlag14(entry);
    if (shadow != 0) {
        if (snagged != 0) {
            mode = 1;
        } else {
            mode = 0;
        }
    } else {
        mode = 2;
    }
    pokemonCreate(work, species, 10, gamedataGetStatus(0, 1));
    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
    darkPokemonSaveGet8(rndId);
    pokemonBiosSetRnd(work, rnd);
    switch (mode) {
    case 0:
    case 2:
        pokemonBiosSetDarkpokemonDataId(work, 1);
        pokemonBiosSetDp(work, 10);
        break;
    case 1:
        pokemonBiosSetDarkpokemonDataId(work, 0);
        pokemonBiosSetDp(work, 0);
        break;
    default:
        break;
    }
}

/* Magnitude of a scroll step. */
static inline f32 pda_8003B85C_Abs(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* Ease one scroll coordinate toward its target, snapping once the step would overshoot. */
static inline void pda_8003B85C_Ease(f32* cur, f32 t, f32 c)
{
    f32 rate;
    f32 moved;
    f32 rest;
    f32 diff;

    if (c != t) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        diff = t - c;
        moved = diff * rate;
        *cur = c + moved;
        rest = t - *cur;
        if (pda_8003B85C_Abs(rest) <= pda_8003B85C_Abs(moved)) {
            *cur = t;
        }
    }
}

/* Party-list scroll, cursor movement and the selected Pokemon preview. */
#pragma peephole off
s32 pdaSnagListDetailKey(void* window, s32 enabled)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u16* keys;
    s32 held;
    s32 prevIndex;
    s32 idx;
    u16 code;
    u16 species;
    u16 entry;
    u16 rndId;
    u32 work;
    s16 baseY;
    f32 step;
    f32 cur;
    f32 dt;

    keys = windowGetKeyInfo(window);
    held = 0;
    prevIndex = *(s32*)&lbl_803A6748;
    dt = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    *(f32*)((u8*)&lbl_803A6748 + 0x24) = dt;
    *(f32*)((u8*)&lbl_803A6748 + 0x28) = *(f32*)((u8*)&lbl_803A6748 + 0x28) + dt;
    if (*(f32*)((u8*)&lbl_803A6748 + 0x28) >= lbl_8047BAC8) {
        *(f32*)((u8*)&lbl_803A6748 + 0x28) = lbl_8047BAC4;
    }
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x14), *(f32*)((u8*)&lbl_803A6748 + 0x1c), *(f32*)((u8*)&lbl_803A6748 + 0x14));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x18), *(f32*)((u8*)&lbl_803A6748 + 0x20), *(f32*)((u8*)&lbl_803A6748 + 0x18));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x2c), *(f32*)((u8*)&lbl_803A6748 + 0x34), *(f32*)((u8*)&lbl_803A6748 + 0x2c));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x30), *(f32*)((u8*)&lbl_803A6748 + 0x38), *(f32*)((u8*)&lbl_803A6748 + 0x30));

    if (*(f32*)((u8*)&lbl_803A6748 + 0x48) != *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
        step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
        if (*(f32*)((u8*)&lbl_803A6748 + 0x48) > *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
            cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
            if (cur > lbl_8047BAC8) {
                *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
            }
        } else {
            cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
            if (cur < lbl_8047BAC4) {
                *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
            }
        }
    }

    if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
        code = keys[0];
    } else {
        code = keys[3];
    }
    if ((keys[0] & 3) != 0) {
        held = 1;
    }
    if ((code & 2) != 0) {
        if (lbl_8047A4DC != 0) {
            if (*(s32*)&lbl_803A6748 < lbl_8047A4DC - 1) {
                if (enabled != 0) {
                    darkPokemonSetFlag0C(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
                }
                *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 + 1;
                soundStartRaw(0x23, 0, 0);
            }
            if (*(s32*)&lbl_803A6748 >= *(s32*)((u8*)&lbl_803A6748 + 0xc)) {
                *(f32*)((u8*)&lbl_803A6748 + 0x20) = *(f32*)((u8*)&lbl_803A6748 + 0x20) - lbl_8047BAD8;
                *(s32*)((u8*)&lbl_803A6748 + 0xc) = *(s32*)((u8*)&lbl_803A6748 + 0xc) + 1;
                *(s32*)((u8*)&lbl_803A6748 + 0x8) = *(s32*)((u8*)&lbl_803A6748 + 0x8) + 1;
            }
        }
    }
    if ((code & 1) != 0) {
        if (*(s32*)&lbl_803A6748 > 0) {
            if (enabled != 0) {
                darkPokemonSetFlag0C(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
            }
            *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 - 1;
            soundStartRaw(0x23, 0, 0);
        }
        if (*(s32*)&lbl_803A6748 < *(s32*)((u8*)&lbl_803A6748 + 0x8)) {
            *(f32*)((u8*)&lbl_803A6748 + 0x20) = *(f32*)((u8*)&lbl_803A6748 + 0x20) + lbl_8047BAD8;
            *(s32*)((u8*)&lbl_803A6748 + 0x8) = *(s32*)((u8*)&lbl_803A6748 + 0x8) - 1;
            *(s32*)((u8*)&lbl_803A6748 + 0xc) = *(s32*)((u8*)&lbl_803A6748 + 0xc) - 1;
        }
    }

    if (held != 0) {
        if (*(f32*)((u8*)&lbl_803A6748 + 0x3c) < lbl_8047BAC8) {
            *(f32*)((u8*)&lbl_803A6748 + 0x3c) = *(f32*)((u8*)&lbl_803A6748 + 0x3c) + *(f32*)((u8*)&lbl_803A6748 + 0x24);
            if (*(f32*)((u8*)&lbl_803A6748 + 0x3c) >= lbl_8047BAC8) {
                *(s8*)((u8*)&lbl_803A6748 + 0x40) = 1;
            }
        }
    } else {
        *(f32*)((u8*)&lbl_803A6748 + 0x3c) = lbl_8047BAC4;
        *(s8*)((u8*)&lbl_803A6748 + 0x40) = 0;
    }

    if (enabled != 0) {
        if (prevIndex != *(s32*)&lbl_803A6748) {
            if (darkPokemonCheckFlag16(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId) != 0) {
                work = lbl_8047A4D0;
                idx = *(s32*)&lbl_803A6748;
                species = lbl_8047A4D4[idx].field_00;
                entry = lbl_8047A4D4[(u16)idx].battleId;
                rndId = lbl_8047A4D4[idx].battleId;
                pda_8003C7C0_LoadPreview(work, species, entry, rndId);
                menuPokemonModelRequest((u8*)&lbl_803A6748 + 0x4c, lbl_8047A4D0);
            }
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x18) = *(f32*)((u8*)&lbl_803A6748 + 0x20);
        baseY = *(s16*)(lbl_802EF0A8 + 0x50a0);
        idx = *(s32*)&lbl_803A6748 - *(s32*)((u8*)&lbl_803A6748 + 0x8);
        if (idx >= 10) {
            idx = 9;
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x30) = *(f32*)((u8*)&lbl_803A6748 + 0x38) =
            baseY + idx * 0x18;
    }
    return 0;
}
#pragma peephole reset

extern u8 lbl_803A6610[];
extern u32 pokemonCreateRndFit(void* work, s32 a, s32 b, s32 c, s32 d);
extern void winMsgClose(s32 id);
extern void menuModelTerm(void* work);

/* Party-select scene: open the windows, drive the fade, tear it back down. */
#pragma peephole off
void pdaSnagListRun(void)
{
    extern void _threadSwitch(void);
    u32 handle;
    u32 work;
    u8* model;
    u8* tbl;
    u8* base;
    s32 state;
    s32 running;
    s32 idx;
    u16 species;
    u16 entry;
    u16 rndId;
    s32 mode;
    u32 rnd;
    f32 step;
    u8 shadow;
    u8 snagged;
    f32 cur;
    s16* dim;

    state = 0;
    running = 1;
    pdaSnagListBuild();
    *(f32*)((u8*)&lbl_803A6748 + 0x14) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x18) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x1c) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x20) = lbl_8047BAC4;
    *(s32*)((u8*)&lbl_803A6748 + 0x0) = 0;
    *(s32*)((u8*)&lbl_803A6748 + 0x4) = 0;
    *(s32*)((u8*)&lbl_803A6748 + 0x8) = 0;
    *(f32*)((u8*)&lbl_803A6748 + 0x3c) = lbl_8047BAC4;
    *(s8*)((u8*)&lbl_803A6748 + 0x40) = 0;
    *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
    *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
    pokemonCreate((u32)lbl_803A6610, 1, 10, gamedataGetStatus(0, 1));
    *(u32*)((u8*)&lbl_803A6748 + 0x98) =
        pokemonCreateRndFit(lbl_803A6610, -1, -1, 0, 0);
    *(f32*)((u8*)&lbl_803A6748 + 0x28) = lbl_8047BAC4;
    if (lbl_8047A4DC >= 10) {
        *(s32*)((u8*)&lbl_803A6748 + 0xc) = 10;
    } else {
        *(s32*)((u8*)&lbl_803A6748 + 0xc) = lbl_8047A4DC;
    }
    model = (u8*)&lbl_803A6748 + 0x4c;
    *(s32*)((u8*)&lbl_803A6748 + 0x10) = lbl_8047A4DC;
    *(f32*)((u8*)&lbl_803A6748 + 0x34) = *(f32*)((u8*)&lbl_803A6748 + 0x2c) =
        *(s16*)(lbl_802EF0A8 + 0x509e);
    *(f32*)((u8*)&lbl_803A6748 + 0x38) = *(f32*)((u8*)&lbl_803A6748 + 0x30) =
        *(s16*)(lbl_802EF0A8 + 0x50a0);
    dim = (s16*)(lbl_802EF0A8 + 0x17ca6);
    menuModelInit(model, dim[0], dim[1]);

    while (1) {
        switch (state) {
        case 0:
            menuOpen(0x2f, 0);
            menuOpen(0x2e, 0);
            state = 1;
            break;
        case 1:
            menuOpen(0x30, 0);
            menuOpen(0x2d, 0);
            menuOpen(0x2b, 0);
            menuOpen(0x2c, 0);
            if (menuOpen(0x32, 1) >= 0) {
                if (*(s32*)((u8*)&lbl_803A6748 + 0x10) != 0) {
                    state = 2;
                    soundStartRaw(0x24, 0, 0);
                    work = lbl_8047A4D0;
                    idx = *(s32*)&lbl_803A6748;
                    species = lbl_8047A4D4[idx].field_00;
                    entry = lbl_8047A4D4[(u16)idx].battleId;
                    rndId = lbl_8047A4D4[idx].battleId;
                    pda_8003C7C0_LoadPreview(work, species, entry, rndId);
                    menuPokemonModelRequest(model, lbl_8047A4D0);
                    *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
                } else {
                    state = 1;
                }
            } else {
                state = 5;
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
            }
            break;
        case 2:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                state = 3;
                menuCloseCustom(0x2f, 0, 1);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
            } else {
                _threadSwitch();
            }
            break;
        case 3:
            menuOpen(0x33, 0);
            if (menuOpen(0x34, 1) < 0) {
                state = 4;
                soundStartRaw(0x25, 0, 0);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
            }
            break;
        case 4:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                menuCloseCustom(0x33, 0, 1);
                darkPokemonSetFlag0C(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC8;
                state = 0;
            } else {
                _threadSwitch();
            }
            break;
        case 5:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
                    if (cur < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                state = 0x64;
            } else {
                _threadSwitch();
            }
            break;
        case 0x64:
            menuCloseCustom(0x30, 0, 1);
            menuCloseCustom(0x2f, 0, 1);
            menuCloseCustom(0x2d, 0, 1);
            menuCloseCustom(0x2b, 0, 1);
            menuCloseCustom(0x2c, 0, 1);
            menuCloseCustom(0x2e, 0, 1);
            menuCloseCustom(0x32, 0, 1);
            winMsgClose(1);
            running = 0;
            break;
        default:
            break;
        }
        if (running == 0) {
            break;
        }
    }
    menuModelTerm(model);
    if (lbl_8047A4D0 != 0) {
        handle = GSmemGetHandleByPtr(lbl_8047A4D0);
        if ((u16)handle != 0) {
            GSmemUnlock();
            GSmemFree(handle);
        }
        lbl_8047A4D0 = 0;
    }
    if (lbl_8047A4D4 != NULL) {
        handle = GSmemGetHandleByPtr(lbl_8047A4D4);
        if ((u16)handle != 0) {
            GSmemUnlock();
            GSmemFree(handle);
        }
        lbl_8047A4D4 = NULL;
    }
}
#pragma peephole reset

extern f32 lbl_8047BAF8;
extern f32 lbl_8047BAFC;
extern f32 lbl_8047BB00;
extern void* fightTrainerPokemonDataBiosGetPtr(void* data);
extern void* fightTrainerPokemonDataBiosGetNickname(void* entry);
extern u8 fightTrainerPokemonDataBiosGetDarkPokemonFlag(void* entry);

/* The owner caption of a party entry's battle: the trainer-kind or species
 * caption chosen by the entry's variant, or 0 without a battle list. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003C2B8_OwnerText (fn_8003C2B8) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003C2B8_OwnerText(s32 off, u8* slot)
{
    u16 kind;
    u32 caption;
    u32 text;
    u8 special;
    u16 trainerId;

    special = 0;
    if (lbl_8047A4D0 != 0) {
        kind = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
        caption = darkPokemonGetFlagListValue(kind, slot);
        switch (*slot) {
        case 0:
            caption = 0x371f;
            break;
        case 1:
        case 2:
            if (kind == 0x43) {
                caption = 0x12b0;
            } else if ((trainerId = darkPokemonGetSpecies(kind, 0)) == 9) {
                text = darkPokemonSaveGetName(kind);
                special = 1;
            } else {
                caption = fightTrainerDataBiosGetStatus03(fightTrainerDataBiosGetPtr(trainerId));
            }
            break;
        }
        if (special != 0) {
            return text;
        }
        text = GSmsgGetGSchar(caption);
        return text;
    }
    return 0;
}

/* Draw one row per party entry: nickname, owner and battle caption. */
#pragma peephole off
void pdaSnagListDrawDetail(PdaSprite* alphaSprite, PdaEvent* event)
{
    s32 y;
    void* entry;
    u32 name;
    u32 caption;
    u32 owner;
    u32 msg;
    u32 tint;
    u16 id;
    s32 i;
    s32 off;
    s32 mode;
    u8* slot;
    void* nick;
    u16 trainerId;
    u8 registered;
    u8 cleared;
    f32 stride;
    f32 shifted;
    f32 firstX;
    f32 rowY;

    alphaSprite->alphaByte =
        lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44);
    spriteSetScissor(*(s16*)(lbl_802EF0A8 + 0x4fa2) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 2),
                *(s16*)(lbl_802EF0A8 + 0x4fa4) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 4),
                *(s16*)(lbl_802EF0A8 + 0x4fa6),
                *(s16*)(lbl_802EF0A8 + 0x4fa8));
    slot = (u8*)&lbl_803A6748 + 0x94;
    firstX = *(f32*)((u8*)&lbl_803A6748 + 0x14);
    rowY = *(f32*)((u8*)&lbl_803A6748 + 0x18);
    shifted = lbl_8047BAC8 + (firstX - lbl_8047BAF8 - lbl_8047BAFC);
    stride = lbl_8047BAD8;
    for (i = 0, off = 0; i < (s32)lbl_8047A4DC; i++) {
        if (i >= *(s32*)((u8*)&lbl_803A6748 + 0x8) - 1 &&
            i <= *(s32*)((u8*)&lbl_803A6748 + 0xc) + 1) {
            id = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
            trainerId = darkPokemonGetSpecies(id, 0);
            if (trainerId == 9) {
                entry = pokemonDataBiosGetPtr(darkPokemonSaveGet4A0(id));
                if (entry != NULL) {
                    name = GSmsgGetGSchar(
                        (u32)pokemonDataBiosGetName(entry));
                }
            } else {
                entry = fightTrainerPokemonDataBiosGetPtr(
                    (void*)fightTrainerDataBiosGetStatus05(fightTrainerDataBiosGetPtr(trainerId)));
                name = GSmsgGetGSchar(
                    (u32)fightTrainerPokemonDataBiosGetNickname(entry));
                for (;;) {
                    if (id == fightTrainerPokemonDataBiosGetDarkPokemonFlag(entry)) {
                        nick = fightTrainerPokemonDataBiosGetNickname(entry);
                        if (nick != NULL) {
                            name = GSmsgGetGSchar((u32)nick);
                        }
                        break;
                    }
                    entry = (u8*)entry + 0x50;
                }
            }
            msg = name;
            if (name == 0) {
                msg = GSmsgGetGSchar(1);
            }
            owner = pda_8003C2B8_OwnerText(off, slot);
            if (owner == 0) {
                owner = GSmsgGetGSchar(1);
            }
            caption = pdaSnagListGetTrainerName(i);
            if (caption == 0) {
                caption = GSmsgGetGSchar(1);
            }
            id = lbl_8047A4D4[(u16)i].battleId;
            darkPokemonGetFlag1C(id);
            darkPokemonCheckFlag16(id);
            registered = darkPokemonCheckFlag12(id);
            cleared = darkPokemonCheckFlag14(id);
            if (registered != 0) {
                if (cleared != 0) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            } else {
                mode = 2;
            }
            switch (mode) {
            case 0:
                windowDrawSprite(shifted, rowY - lbl_8047BB00,
                                 alphaSprite, 0x161, 0);
                break;
            case 1:
                windowDrawSprite(firstX - lbl_8047BAF8, rowY, alphaSprite, 0x160, 0);
                break;
            case 2:
                break;
            }
            if (darkPokemonCheckFlag0C(*(u16*)((u8*)lbl_8047A4D4 + off + 2)) != 0) {
                tint = 0xe0e01800;
            } else {
                tint = -0x100;
            }
            msgctrlSetValue(0x37, msg);
            y = (s32)rowY;
            GSmsgPrintColor(0, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, caption);
            GSmsgPrintColor(0x89, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, owner);
            GSmsgPrintColor(0x13d, y, tint | alphaSprite->alphaByte, (void*)0xe7);
        }
        rowY = rowY + stride;
        off += 4;
    }
    spriteClearScissor();
}
#pragma peephole reset

#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_SAVE_80038A0C_ONLY && !PDA_DRIVER_8003842C_ONLY */
#if !defined(PDA_DRIVER_8003842C_ONLY)
extern u8 lbl_80267060[];
extern u8 lbl_803A65B0[];
#if !defined(PDA_80039128_ONLY)
typedef struct PdaMenuRecord {
    s32 eventId;
    u8 pad04[0x10];
    u8 enabled;
    u8 pad15[3];
} PdaMenuRecord;

/* Position and tint one of the PDA's eight main menu buttons. */
#pragma peephole off
s32 itemStorageMenuCB(void* work, PdaSprite* sprite)
{
    s32 i;
    u8 enabled;

    (void)work;
    for (i = 0; i < 8; i++) {
        if (sprite->eventId == *(s32*)(i * 0x18 + lbl_80267060)) {
            break;
        }
    }
    if (i >= 8) {
        return 0;
    }
    {
        f32 x = *(volatile f32*)(lbl_803A65B0 + i * 0xc);
        f32* pos = ((f32(*)[3])lbl_803A65B0)[i];
        enabled = ((PdaMenuRecord*)lbl_80267060)[(u8)i].enabled;
        sprite->field_50 = x;
        sprite->field_52 = pos[1];
        sprite->alpha = pos[2];
    }
    if (enabled != 0) {
        if (sprite->eventId == (s32)menuGetCursorItemID(0x24)) {
            sprite->colorR = 0xff;
            sprite->colorG = 0xff;
            sprite->colorB = 0xff;
        } else {
            sprite->colorR = 0x46;
            sprite->colorG = 0x8f;
            sprite->colorB = 0xb4;
        }
    }
    return 0;
}
#pragma peephole reset
#endif /* !PDA_80039128_ONLY */
#endif /* !PDA_DRIVER_8003842C_ONLY */

#if !defined(PDA_SAVE_80038A0C_ONLY) && !defined(PDA_DRIVER_8003842C_ONLY)
#if !defined(PDA_80039128_ONLY)
extern u16 pcboxGetNbItemSlot(s32 box);
extern u16 itemBiosGetItemDataId(void* item);
extern void* itemDataBiosGetPtr(u16 id);
extern void* itemDataBiosGetName(void* data);
extern u8 itemDataBiosGetKind(void* data);
extern u16 itemBiosGetNum(void* item);

/* Draw the visible page of the PC item list. */
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local optimization-level pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 2
s32 itemStorageDrawItem(void* work, PdaSprite* sprite)
{
    s32 start;
    s32 yAdj;
    s32 rows;
    s32 total;
    s32 slots;
    s32 i;
    s32 j;
    s32 found;
    s32 nameX;
    s32 rowY;
    s32 y;
    s32 width;
    u32 itemId;
    u16 count;
    void* item;
    void* name;
    s32 index;

    (void)work;
    start = 0;
    rows = 8;
    yAdj = 0;
    total = 0;
    slots = pcboxGetNbItemSlot(0);
    for (i = 0; i < slots; i++) {
        if ((u8)itemSlotIsValid(pcboxGetItem(0, (s16)i)) != 0) {
            total++;
        }
    }
    index = lbl_8047A4A8;
    msgctrlSetValue(0x34, 0x3e7);
    width = (s16)(GSmsgGetRect(0xca) >> 16);
    width += (s16)(GSmsgGetRect(0x12e) >> 16);
    if (lbl_8047BAB0 != lbl_8047A4C0 && lbl_8047A4BC != 0) {
        if (lbl_8047A4C0 < lbl_8047BAB0) {
            index--;
            start = -1;
        } else {
            rows = 9;
        }
        yAdj = (s32)lbl_8047A4C0;
    }
    rowY = start * 0x1f;
    i = start;
    nameX = 0x11a - width;
    for (; i < rows && index < total; i++, rowY += 0x1f, index++) {
        if (index < 0) {
            continue;
        }
        y = rowY - yAdj;
        itemId = pdaBoxItemIdFirst(index);
        name = itemDataBiosGetName(itemDataBiosGetPtr(itemId));
        if (name != NULL) {
            GSmsgPrintColor(0, y, (s32)sprite, name);
        }
        if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
            continue;
        }
        GSmsgPrintColor(nameX, y, (s32)sprite, (void*)0x12e);
        count = pdaBoxItemNum(index);
        msgctrlSetValue(0x34, count);
        GSmsgPrintColor(0x11a - (s16)(GSmsgGetRect(0xca) >> 16), y, (s32)sprite,
                    (void*)0xca);
    }
    if (i < rows) {
        GSmsgPrintColor(0, i * 0x1f - yAdj, (s32)sprite, (void*)0x1b67);
    }
    return 0;
}
#pragma pop
#pragma peephole reset

#endif /* !PDA_80039128_ONLY */
extern f32 lbl_8047BAA8;

/* Slide each of the eight main-menu buttons right and fade it in, clamping at
   its record's target X and at full alpha. */
/* RULE-EXCEPTION(user-approved): single-use inline helper; its own loop counter numbers after the strength-reduced cursors, which gives retail's r5/r6/r7 - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80039128_Slide(void)
{
    s32 i;

    for (i = 0; i < 8; i++) {
        if ((((f32*)lbl_803A65B0)[i * 3] += 13.333333f) > ((s32*)lbl_80267060)[i * 6 + 3]) {
            ((f32*)lbl_803A65B0)[i * 3] = ((s32*)lbl_80267060)[i * 6 + 3];
        }
        if ((((f32*)lbl_803A65B0)[i * 3 + 2] += 8.333333f) > 255.0f) {
            ((f32*)lbl_803A65B0)[i * 3 + 2] = 255.0f;
        }
    }
}

/* Seed and then advance the main-menu button slide-in animation. */
#if !defined(PDA_80039128_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) for the shared-file compiles; the linked unit pda_candidate_80039128 uses -opt nopeephole - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
#endif
s32 itemStorageMenuCtrl(u8* work)
{
    s32 i;

    switch (*(s8*)(work + 1)) {
    case 0:
        if (*(s8*)(work + 2) == 0) {
            for (i = 0; i < 8; i++) {
                ((f32*)lbl_803A65B0)[i * 3] = ((s32*)lbl_80267060)[i * 6 + 1];
                ((f32*)lbl_803A65B0)[i * 3 + 1] = ((s32*)lbl_80267060)[i * 6 + 2];
                ((f32*)lbl_803A65B0)[i * 3 + 2] = lbl_8047BAA8;
            }
            work[2] = 1;
        }
        break;
    case 2:
        pda_80039128_Slide();
        break;
    case 3:
        if (*(s8*)(work + 2) == 0) {
            work[2] = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
#if !defined(PDA_80039128_ONLY)
#pragma peephole reset
#endif
#endif /* !PDA_SAVE_80038A0C_ONLY && !PDA_DRIVER_8003842C_ONLY */
#endif /* !PDA_RANGE_EXACT_80037174_ONLY */
#endif /* !PDA_RANGE_80037158_ONLY */
