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
} PdaModelWindow;

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
} PdaSceneWork;

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

extern s32 fn_8003B85C(void* window, s32 enabled);
extern void fn_8003C2B8(PdaSprite* sprite, PdaEvent* event);
extern s32 fn_80041E48(void* work, s32 mode);
extern s32 fn_80042658(void* work, s32 mode);
extern void fn_800439BC(void* scene);
extern void GSscene_SetMode(s32 mode);
extern void menuButtonNormal(void* button);
extern void winSpriteSetDisp(void* sprite, s32 disp);
extern void fn_800FB680(s32 arg0, s32 arg1, s32 arg2, void* data);

void fn_80037158(void)
{
}

s32 fn_8003715C(void)
{
    s32 flag;

    flag = lbl_8047A470;
    lbl_8047A470 = 1;
    return 1 - flag;
}

void fn_80037174(void)
{
    lbl_8047A470 = 0;
}

void fn_800372F0(void)
{
}

void fn_800372F4(void)
{
}

void fn_800372F8(void)
{
}

void fn_800372FC(void)
{
}

#pragma peephole off
void fn_80037300(void* window, PdaSprite* sprite)
{
    extern u8 lbl_80314E08[];
    extern void fn_800D5BA0(s32 index, u32 color);
    extern void fn_800D61E4(s32 x, s32 y);
    extern void fn_800D6728(void);
    extern void fn_800D67BC(s32 count);
    extern void fn_800D6A00(s32 primitive);
    extern void fn_800D7820(void* data);
    extern void fn_800D888C(s32 mode);
    extern void fn_800D88DC(s32 mode);

    (void)window;
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314E08);
    fn_800D6A00(6);
    fn_800D67BC(4);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, 0x003B6DFF);
    fn_800D61E4(sprite->x, 0);
    fn_800D5BA0(0, 0x489DECFF);
    fn_800D61E4(sprite->x, sprite->y);
    fn_800D5BA0(0, 0x489DECFF);
    fn_800D61E4(0, sprite->y);
    fn_800D5BA0(0, 0x003B6DFF);
    fn_800D6728();
}
#pragma peephole reset

#pragma peephole off
void fn_800373C8(PdaSprite* context, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
void fn_80037468(PdaSprite* context, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
void fn_80037508(PdaSprite* context, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
void fn_800375A8(PdaSprite* context, PdaSprite* sprite)
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

void fn_80037648(void)
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
    fn_800FB680(-4, 0, -1, (void*)messageId);
}

void fn_80038124(void* window, PdaSprite* sprite)
{
    sprite->value = lbl_8047BA74 - lbl_8047A478;
}

#pragma scheduling off
#pragma opt_propagation off
void fn_800376C8(void)
{
    void* data = lbl_8047A480;

    fn_800FB680(0, 0, -1, data);
}
#pragma opt_propagation reset
#pragma scheduling reset

/* Four copies of the same fade-in step, one per 0x18-byte record in
   lbl_803A654C. Two shapes here are load-bearing: 0.0f as a literal, because
   retail reloads the constant instead of reusing the compare's copy, and the
   embedded `velocity =`, which orders the record load ahead of the velocity
   load. */
#pragma peephole off
void fn_800376F8(void* window, volatile PdaSprite* sprite)
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
void fn_800377B4(void* window, volatile PdaSprite* sprite)
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
void fn_80037870(void* window, volatile PdaSprite* sprite)
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
void fn_8003792C(void* window, volatile PdaSprite* sprite)
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

    angle = lbl_8047A484 - baseAngle;
    if (updateTarget) {
        lbl_8047A488 = lbl_802E52A8[lbl_8047A47C];
    }
    if (angle < lbl_8047BA58) {
        angle += lbl_8047BA60;
    }
    if (angle >= lbl_8047BA60) {
        angle -= lbl_8047BA60;
    }

    windowGetKeyInfo();
    step = lbl_8047BA64 * lbl_8047A494;
    if (lbl_8047A484 != lbl_8047A488) {
        if (lbl_8047A488 - lbl_8047A484 < lbl_8047BA58) {
            step = -step;
        }
        lbl_8047A48C = step;
        lbl_8047A484 += step;
        if (lbl_8047A484 >= lbl_8047BA60) {
            lbl_8047A484 -= lbl_8047BA60;
        }
        if (lbl_8047A484 < lbl_8047BA58) {
            lbl_8047A484 += lbl_8047BA60;
        }

        distance = lbl_8047A484 - lbl_8047A488;
        if (distance <= lbl_8047BA58) {
            distance = -distance;
        }
        magnitude = step;
        if (magnitude <= lbl_8047BA58) {
            magnitude = -magnitude;
        }
        if (distance < magnitude) {
            lbl_8047A484 = lbl_8047A488;
            lbl_8047A48C = lbl_8047BA58;
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
void fn_800379E8(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[3], 0);
}
#pragma peephole reset

#pragma peephole off
void fn_80037BB0(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[2], 0);
}
#pragma peephole reset

#pragma peephole off
void fn_80037D78(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[1], 0);
}
#pragma peephole reset

#pragma peephole off
void fn_80037F40(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[0], 1);
}
#pragma peephole reset

/* The redundant expressions preserve MWCC's exact register/scheduling shape. */
void fn_80038138(void* window, PdaSprite* sprite)
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

#pragma peephole off
void fn_80038170(PdaSprite* context, PdaSprite* sprite)
{
    extern u8 lbl_803A654C[];
    extern f32 lbl_802E5288[][2];
    extern s8 lbl_8047A47C;
    f32* state;
    s8 index;
    u8 stopped;

    stopped = 0;
    state = (f32*)lbl_803A654C;
    if (lbl_8047BA58 == state[4]) {
        state += 6;
        if (lbl_8047BA58 == state[4]) {
            state += 6;
            if (lbl_8047BA58 == state[4]) {
                state += 6;
                if (lbl_8047BA58 == state[4]) {
                    stopped = 1;
                }
            }
        }
    }
    if (stopped) {
        sprite->flags |= 2;
    } else {
        sprite->flags &= ~2;
    }
    index = context->selectedIndex;
    lbl_8047A47C = index;
    sprite->field_50 = (s16)lbl_802E5288[index][0];
    *(s16*)((u8*)sprite + 0x52) = (s16)lbl_802E5288[index][1];
}
#pragma peephole reset

#pragma peephole off
s32 fn_80038250(PdaMenuState* state)
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
#pragma peephole reset

#pragma peephole off
s32 fn_800382E8(PdaMenuState* state)
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
#pragma peephole reset

#pragma peephole off
s32 fn_80038380(PdaSelectionWork* work)
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
#pragma peephole reset

#pragma peephole off
s32 fn_80039498(s32 value)
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
#pragma peephole reset

#pragma peephole off
s32 fn_80039548(void* window, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
s32 fn_8003956C(void* window, void* sprite)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern void* pcboxGetItem(s32 box, s16 slot);
    extern u8 fn_801429E8(void* item);
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
        if (fn_801429E8(pcboxGetItem(0, slot))) {
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
#pragma peephole reset

#pragma scheduling off
void fn_800388C4(void)
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
#pragma scheduling reset

void fn_80038990(void)
{
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void _threadSwitch(void);

    while (1) {
        lbl_8047A494 = (f32)fn_800D3088() / (f32)fn_800D37CC();
        _threadSwitch();
    }
}

void fn_80038A00(void)
{
    lbl_8047A484 = lbl_8047BA58;
}

/* Re-seed the People screen's two 4-entry widget tables and reset the
   carousel angle back to the table's rest value. */
static inline void pdaResetPeopleTables(u8* base, u8* tbl)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    u8* src;
    s32 i;

    lbl_8047A480 = (void*)0x1b5a;
    src = tbl + 0;
    dst = base + 0xb4;
    for (i = 0; i < 4; i++, dst += 0x18, src += 0x18) {
        memcpy(dst, src, 0x18);
    }
    src = tbl + 0x60;
    dst = base + 0x54;
    for (i = 0; i < 4; i++, dst += 0x18, src += 0x18) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)(tbl + 0xe0);
    lbl_8047A484 = *(f32*)(tbl + 0xe0);
    lbl_8047A47D = 0;
}

/* People screen driver: run the top-level menu, dispatch the four entries,
   and tear the menu down on exit. */
#pragma peephole off
void fn_8003842C(void)
{
    extern u8 lbl_802E51C8[];
    extern u8 lbl_802EF0A8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void fn_801661D0(s32 a, s32 b, s32 c, s32 d);
    extern void fn_801660D8(s32 a, s32 b, s32 c);
    extern void fn_800FF660(void);
    extern void fn_80058150(void);
    extern void fn_8003A520(void);
    extern s32 fn_80038A0C(void);
    extern void menuModelInit(u8* model, s16 a, s16 b);
    extern void menuModelSetMotion(u8* model, s32 motion);
    extern void fn_8010A010(u8* model, s32 id);
    extern void fn_8010A420(u8* model);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern void fn_8018F4C8(void* info, s32 kind, s32* out0, s32* out1);
    u8* tbl;
    u8* base;
    s32 state;
    s32 running;
    s32 choice;
    s32 motion;
    s32 scratch;

    base = lbl_803A6498;
    tbl = lbl_802E51C8;
    state = 0;
    running = 1;
    fn_801661D0(0x55, 0x1f4, 1, 1);
    do {
        switch (state) {
        case 0:
            pdaResetPeopleTables(base, tbl);
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
                    pdaResetPeopleTables(base, tbl);
                    state = 0xa;
                    break;
                case 1:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables(base, tbl);
                    state = 0x14;
                    break;
                case 2:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables(base, tbl);
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
            fn_80058150();
            state = 1;
            break;
        case 0x14:
            lbl_8047A480 = (void*)0x1b61;
            fn_8003A520();
            lbl_8047A480 = (void*)0x1b5a;
            state = 1;
            break;
        case 0x1e:
            menuModelInit(base, (s16)(*(s16*)(lbl_802EF0A8 + 0x5fd6) + 0xa),
                          *(s16*)(lbl_802EF0A8 + 0x5fd8));
            fn_8010A010(base, 0xf70400);
            fn_8018F4C8(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &scratch);
            menuModelSetMotion(base, motion);
            state = fn_80038A0C();
            fn_8010A420(base);
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
    fn_801660D8(0x3e8, 1, 1);
    fn_800FF660();
}
#pragma peephole reset

/* Prime the save-overwrite prompt with the current play time and the two
   hero-name message slots. */
static inline void pdaFillSavePrompt(void)
{
    extern u32 fn_80005748(void);
    extern u32 fn_801EF214(void);
    extern u32 fn_801EF274(void);
    extern void msgctrlSetValue(s32 id, u32 value);
    u32 playTime;

    playTime = fn_80005748();
    if (lbl_8047A498 != 0) {
        playTime = lbl_8047A498;
    }
    lbl_8047A49C = fn_801EF214();
    lbl_8047A4A0 = fn_801EF274();
    msgctrlSetValue(0x4c, playTime);
    msgctrlSetValue(0x2f, fn_801EF274());
    msgctrlSetValue(0x30, fn_801EF214());
}

typedef struct PdaSaveImage {
    u8 bytes[0x1dfd0];
} PdaSaveImage;

/* Save-and-quit flow reached from the People screen's model preview. */
#pragma peephole off
s32 fn_80038A0C(void)
{
    extern u8 lbl_802E51C8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern s8 menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern u32 fn_80005748(void);
    extern void fn_800056EC(f32 seconds);
    extern void* fn_801D036C(void);
    extern void fn_801D0314(void* buffer);
    extern s32 fn_801D0748(s32 a, s32 b, s32 c);
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
    extern void fn_8011418C(s32* floorId, s32* prevFloorId, u8* posIndex);
    u8* base;
    u8* tbl;
    void* backup;
    u8* slot;
    u8 saved;
    s32 result;
    s32 running;
    s32 state;
    void* data;
    s8 answer;
    s32 prevFloorId;
    s32 floorId;
    u8 posIndex;

    base = lbl_803A6498;
    state = 0;
    tbl = lbl_802E51C8;
    running = 1;
    slot = base + 0x48;
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
            backup = fn_801D036C();
            *(PdaSaveImage*)backup = *(PdaSaveImage*)savedataGetStatus(0, 0);
            if (fn_801D0748(3, 2, 0) == 3 &&
                gamedatasaveGetStatus(0, 4) != 0) {
                if (heroGetStatus(0, 2, 0) ==
                        heroGetStatus(savedataGetStatus(backup, 2), 2, 0) ||
                    gamedatasaveGetStatus(backup, 4) == 0) {
                    lbl_8047A498 = fn_80005748();
                    pdaFillSavePrompt();
                    lbl_8047A480 = (void*)0x1b60;
                    answer = menuSubOpenYesNo(0, 0x3c, 0x118, 1);
                    if (answer != 0) {
                        saved = 0;
                    } else {
                        saved = 1;
                    }
                    lbl_8047A498 = fn_80005748() - lbl_8047A498;
                }
            }
            *(PdaSaveImage*)savedataGetStatus(0, 0) = *(PdaSaveImage*)backup;
            fn_800056EC((f32)(lbl_8047A498 + fn_80005748()));
            lbl_8047A498 = 0;
            fn_801D0314(backup);
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
            *(s32*)&base[0x48] = gamedatasaveBiosGetFloorid(data);
            *(s32*)&slot[4] = gamedatasaveBiosGetPrevfloorid(data);
            slot[8] = gamedatasaveBiosGetFloorposindex(data);
            fn_8011418C(&floorId, &prevFloorId, &posIndex);
            gamedatasaveBiosSetFloorid(data, floorId);
            gamedatasaveBiosSetPrevfloorid(data, prevFloorId);
            gamedatasaveBiosSetFloorposindex(data, posIndex);
            fn_801D0748(4, 2, 0);
            state = 0x64;
            break;
        case 0x64:
            menuCloseCustom(0x28, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    pdaResetPeopleTables(base, tbl);
    return result;
}
#pragma peephole reset

#pragma peephole off
s32 fn_80039004(PdaSprite* context, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
s32 fn_8003907C(PdaSelectionWork* work)
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
#pragma peephole reset

#pragma peephole off
s32 fn_80039604(void* window, void* sprite)
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
#pragma peephole reset

#pragma peephole off
s32 fn_800398D4(void* work, PdaSprite* sprite)
{
    extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
    extern void fn_800FE35C(void);
    extern s32 fn_80039644(void* window, void* sprite);
    s32 x;
    s32 y;

    x = sprite->field_50;
    y = sprite->field_52;
    fn_800FE38C(0x118 - x, 0x8b - y, 0x150, 0x10d);
    fn_80039644((u8*)work + 0x94, *(void**)((u8*)work + 0x88));
    fn_800FE35C();
    return 0;
}
#pragma peephole reset

#pragma peephole off
s32 fn_8003992C(void* window, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
s32 fn_80039970(void* window, PdaSprite* sprite)
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
#pragma peephole reset

#pragma scheduling off
s32 fn_80039A50(PdaSprite* sprite)
{
    fn_800FB680(0, 0, *(s32*)((u8*)sprite + 0x88), (void*)lbl_8047A4B4);
    return 0;
}
#pragma scheduling reset

/* PC item list: cursor/page input, item swapping, and caption refresh. */
#pragma peephole off
s32 fn_80039A84(void)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern u16 itemBiosGetItemDataId(void* item);
    extern void* itemDataBiosGetPtr(u16 id);
    extern s32 itemDataBiosGetDoc(void* data);
    extern void pcboxSwapItemSlot(s32 box, s16 from, s16 to);
    extern void fn_80166A50(s32 id, s32 a, s32 b, s32 c);
    extern f32 lbl_8047BAB4;
    extern f32 lbl_8047BAB8;
    PdaKeyInfo* keyInfo;
    s32 slots;
    s32 found;
    s32 i;
    s32 total;
    s32 target;
    s32 max;
    s32 cursor;
    s32 top;
    s32 message;
    u16 keys;
    u16 itemId;
    u8 moved;
    void* item;

    keyInfo = windowGetKeyInfo();
    moved = 0;
    if (lbl_8047BAB0 != lbl_8047A4C0) {
        return 0;
    }
    slots = pcboxGetNbItemSlot(0);
    for (i = 0; i < slots; i++) {
        fn_801429E8(pcboxGetItem(0, (s16)i));
    }
    target = lbl_8047A4A8 + lbl_8047A4AC;
    if (lbl_8047A4B8 < 0) {
        if ((keyInfo->trigger & 0xc0) != 0) {
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto haveHeld;
                    }
                }
            }
            itemId = 0;
        haveHeld:
            if (itemId != 0) {
                lbl_8047A4B8 = target;
                fn_80166A50(0x3c6, 0, 0xff, 0);
            }
        }
    } else {
        keys = keyInfo->trigger;
        if ((keys & 0xd0) != 0) {
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto haveSwap;
                    }
                }
            }
            itemId = 0;
        haveSwap:
            if (itemId != 0) {
                pcboxSwapItemSlot(0, (s16)lbl_8047A4B8, (s16)target);
                fn_80166A50(0x3c6, 0, 0xff, 0);
            } else {
                fn_80166A50(0x3c7, 0, 0xff, 0);
            }
            lbl_8047A4B8 = -1;
        } else if ((keys & 0x20) != 0) {
            lbl_8047A4B8 = -1;
            fn_80166A50(0x3c7, 0, 0xff, 0);
        }
    }
    total = 0;
    slots = pcboxGetNbItemSlot(0);
    for (i = 0; i < slots; i++) {
        if ((u8)fn_801429E8(pcboxGetItem(0, (s16)i)) != 0) {
            total++;
        }
    }
    max = total + 1;
    if ((keyInfo->buttons & 2) != 0) {
        cursor = lbl_8047A4AC + 1;
        top = lbl_8047A4A8;
        lbl_8047A4AC = cursor;
        if (top + cursor >= max) {
            lbl_8047A4AC = cursor - 1;
        } else {
            if (cursor >= 8) {
                lbl_8047A4A8 = top + 1;
                lbl_8047A4AC = cursor - 1;
                lbl_8047A4BC = 1;
            } else {
                lbl_8047A4BC = 0;
            }
            lbl_8047A4C0 = lbl_8047BAB4;
            moved = 1;
            fn_80166A50(0x3c5, 0, 0xff, 0);
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
            fn_80166A50(0x3c5, 0, 0xff, 0);
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
            fn_80166A50(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 4) != 0 && moved == 0) {
        top = lbl_8047A4A8 - 8;
        if (top >= 0) {
            lbl_8047A4A8 = top;
            fn_80166A50(0x3c5, 0, 0xff, 0);
        }
    }
    if (lbl_8047A4B8 >= 0) {
        message = 0x1b69;
    } else {
        target = lbl_8047A4A8 + lbl_8047A4AC;
        slots = pcboxGetNbItemSlot(0);
        found = -1;
        for (i = 0; i < slots; i++) {
            item = pcboxGetItem(0, (s16)i);
            if ((u8)fn_801429E8(item) != 0) {
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
    }
    lbl_8047A4B4 = message;
    return 0;
}
#pragma peephole reset

void fn_80039F44(void* button)
{
    if (lbl_8047A4B8 < 0) {
        menuButtonNormal(button);
    }
}

/* PC item transfer loop: run the box list and move the highlighted stack
   between the bag (mode 0) and the PC (mode 1) until the list is closed. */
#pragma peephole off
void fn_8003A10C(s32 mode)
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
    extern s32 fn_8003AE84(void);
    extern s32 fn_8003ACE8(s32 a, s32 b, s32 c);
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
        slots = pcboxGetNbItemSlot(0);
        found = -1;
        for (i = 0; i < slots; i++) {
            item = pcboxGetItem(0, (s16)i);
            if ((u8)fn_801429E8(item) != 0) {
                found++;
                if (found >= target) {
                    itemId = itemBiosGetItemDataId(item);
                    goto haveSelected;
                }
            }
        }
        itemId = 0;
    haveSelected:
        if ((u16)itemId == 0) {
            break;
        }
        switch (mode) {
        case 0:
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto haveTake;
                    }
                }
            }
            itemId = 0;
        haveTake:
            lbl_8047A4B4 = 0x1b6a;
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) != 5) {
                slots = pcboxGetNbItemSlot(0);
                found = -1;
                for (i = 0; i < slots; i++) {
                    item = pcboxGetItem(0, (s16)i);
                    if ((u8)fn_801429E8(item) != 0) {
                        found++;
                        if (found >= target) {
                            num = itemBiosGetNum(item);
                            goto haveTakeNum;
                        }
                    }
                }
                num = 0;
            haveTakeNum:
                count = fn_8003ACE8(1, 1, (u16)num);
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
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto havePut;
                    }
                }
            }
            itemId = 0;
        havePut:
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
                winMsgOpen(2, 0x1b72, 1, 0);
                winMsgClose(1);
                break;
            }
            lbl_8047A4B4 = 0x1b6b;
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        num = itemBiosGetNum(item);
                        goto havePutNum;
                    }
                }
            }
            num = 0;
        havePutNum:
            count = fn_8003ACE8(1, 1, (u16)num);
            if (count <= 0) {
                break;
            }
            shortId = (u16)itemId;
            msgctrlSetValue(0x2d, shortId);
            msgctrlSetValue(0x2f, count);
            lbl_8047A4B4 = 0x1b6c;
            if (fn_8003AE84() == 0) {
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

#pragma peephole off
s32 fn_8003A6C0(PdaDrawWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267130[3];
    extern s32 lbl_8047A4C8;
    extern void fn_800FB8C8(s32, s32, s16, s16, void*, s32);
    extern void msgctrlSetValue(s32 id, s32 value);
    s8 index;
    s32 divisor;
    s32 i;

    index = 0;
    if (sprite->eventId != lbl_80267130[0]) {
        index = 1;
        if (sprite->eventId != lbl_80267130[1]) {
            index = 2;
            if (sprite->eventId != lbl_80267130[2]) {
                index = 3;
            }
        }
    }
    if (index >= 3) {
        return 0;
    }

    divisor = 1;
    for (i = 0; i < index; i++) {
        divisor *= 10;
    }
    msgctrlSetValue(0x34, (lbl_8047A4C8 / divisor) % 10);
    fn_800FB8C8(0, 0, sprite->x, sprite->y, work->drawData, 0xC9);
    return 0;
}
#pragma peephole reset

#pragma peephole off
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
#pragma peephole off
s32 fn_8003A7F0(PdaNumberWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 lbl_8047A4C8;
    extern void fn_80166A50(s32 id, s32 a, s32 b, s32 c);
    PdaKeyInfo* keyInfo;
    PdaNumberRange* range;
    s32 step;
    s32 i;
    s32 cur;
    s32 digit;
    s32 base;
    s32 limit;
    s32 value;
    s8 index;

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
                fn_80166A50(0x3c5, 0, 0xff, 0);
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
                value = base + digit * step;
                lbl_8047A4C8 = value;
                if (value < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    fn_80166A50(0x3c5, 0, 0xff, 0);
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
                fn_80166A50(0x3c5, 0, 0xff, 0);
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
                value = base + digit * step;
                lbl_8047A4C8 = value;
                if (value < range->min) {
                    lbl_8047A4C8 = range->min;
                } else {
                    fn_80166A50(0x3c5, 0, 0xff, 0);
                }
            }
        }
        if ((keyInfo->buttons & 8) != 0) {
            index = work->digitIndex - 1;
            work->digitIndex = index;
            if (index < 0) {
                work->digitIndex = 0;
            }
        }
        if ((keyInfo->buttons & 4) != 0) {
            index = work->digitIndex + 1;
            work->digitIndex = index;
            if (index >= 3) {
                work->digitIndex = 2;
            }
        }
    }
    return 0;
}
#pragma peephole reset

s32 fn_8003AC50(PdaMenuState* state)
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
#pragma peephole reset

#pragma peephole off
s32 fn_8003ACE8(s32 arg0, s32 arg1, s32 arg2)
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
#pragma peephole reset

#pragma peephole off
s32 fn_8003AD6C(PdaSelectionWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267140[4];
    s32 groups[4];
    s32 group;
    s32 found;
    s32 i;
    s16 eventId;
    s32 flag;
    s8 selectedIndex;

    groups[0] = lbl_80267140[0];
    groups[1] = lbl_80267140[1];
    groups[2] = lbl_80267140[2];
    groups[3] = lbl_80267140[3];
    found = 0;
    group = 0;
    eventId = sprite->eventId;
    for (i = 0; i < 2 && !found; i++) {
        if ((s32)eventId == groups[i]) {
            found = 1;
        }
    }
    if (!found) {
        group = 1;
        for (i = 0; i < 2 && !found; i++) {
            if ((s32)eventId == groups[i + 2]) {
                found = 1;
            }
        }
        if (!found) {
            group = 2;
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
#pragma peephole reset

#pragma peephole off
s32 fn_8003AE84(void)
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
#pragma peephole reset

#pragma peephole off
void fn_8003AEF0(PdaSprite* sprite)
{
    extern PdaListEntry* lbl_8047A4D4;
    extern void fn_801EED88(u16 id);
    extern void* fn_801EE544(u16 id, u8* variant);
    extern s32 fn_801EEF40(u16 id);
    extern s32 fn_8011396C(s32 floor);
    extern void msgctrlSetValue(s32 id, s32 value);
    u16 id;
    void* message;
    u32 value;
    s32 color;

    fn_801EED88(lbl_8047A4D4[(u16)*(u32*)&lbl_803A6748].battleId);
    id = lbl_8047A4D4[*(u32*)&lbl_803A6748].battleId;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    message = fn_801EE544(id, (u8*)&lbl_803A6748 + 0x94);
    value = fn_8011396C(fn_801EEF40(id));
    if (value == 0) {
        msgctrlSetValue(0x31, 0x18d3);
    } else {
        msgctrlSetValue(0x31, value);
    }
    color = -0x100;
    color |= sprite->alphaByte;
    fn_800FB680(0, 0, color, message);
}
#pragma peephole reset

#pragma scheduling off
void fn_8003B814(void* window)
{
    fn_8003B85C(window, 1);
}
#pragma scheduling reset

#pragma scheduling off
void fn_8003B838(void* window)
{
    fn_8003B85C(window, 0);
}
#pragma scheduling reset

#pragma peephole off
void fn_8003BF54(PdaSprite* alphaSprite, PdaSprite* sprite)
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
void fn_8003C03C(PdaSprite* alphaSprite, PdaSprite* sprite)
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
void fn_8003C13C(PdaSprite* alphaSprite, PdaSprite* sprite)
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
void fn_8003C21C(PdaSprite* sprite, PdaEvent* event)
{
    (void)event;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
}
#pragma peephole reset

void fn_8003C24C(PdaSprite* sprite, PdaEvent* event)
{
    switch (event->messageId) {
    case 0x2e2:
    case 0x2e4:
        sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
        break;
    default:
        fn_8003C2B8(sprite, event);
        break;
    }
}

#pragma peephole off
void fn_8003C728(PdaSprite* alphaSprite, PdaSprite* sprite)
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

#pragma peephole off
s32 fn_8003CE1C(s32 index)
{
    extern u32 lbl_8047A4D0;
    extern PdaListEntry* lbl_8047A4D4;
    extern s32 GSmsgGetGSchar(s32 message);
    extern void* fightTrainerDataBiosGetPtr(u16 id);
    extern u16 fightTrainerDataBiosGetKindDataId(void* trainer);
    extern u32 fightTrainerKindDataBiosGetPrefixName(void* kind);
    extern void* fightTrainerKindDataBiosGetPtr(u16 kind);
    extern u8 fn_801EE174(u16 id);
    extern s32 fn_801EE544(u16 id, s8* variant);
    extern u32 fn_801EEF40(u16 id);
    extern u16 fn_801EEFAC(u16 id, s32 side);
    void* kindData;
    void* trainer;
    u16 battleId;
    u16 kind;
    u16 alternate;
    s32 message;

    if (lbl_8047A4D0 == 0) {
        return 0;
    }

    battleId = lbl_8047A4D4[index].battleId;
    message = fn_801EE544(battleId, &lbl_803A6748.variant);
    switch (lbl_803A6748.variant) {
    case 0:
    case 1:
        message = 0x3720;
        break;
    case 2:
        kind = fn_801EEFAC(battleId, 0);
        if (kind == 9) {
            kindData = fightTrainerKindDataBiosGetPtr(fn_801EE174(battleId));
        } else {
            alternate = fn_801EEFAC(battleId, 1);
            if (alternate != 0 && fn_801EEF40(battleId) != 3) {
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
#pragma peephole reset

#pragma peephole off
void fn_8003D818(void)
{
    extern u16* lbl_8047A4E4;
    extern u16 lbl_8047A4E8;
    extern u16 memoDataGetPokemonID(u16* data, u32 index);
    extern u16 memoDataGetCount(u16* data);
    extern void fn_8003E394(void);
    u8* work;
    u32 count;
    u32 index;

    work = (u8*)&lbl_803A6818;
    count = 0;
    index = count;
    while ((u16)index < memoDataGetCount(0)) {
        lbl_8047A4E4[(u16)index] = memoDataGetPokemonID(0, index);
        count++;
        index++;
    }
    lbl_8047A4E8 = count;
    work[0x158] = 0;
    work[0x159] = 0;
    work[0x15b] = 0;
    work[0x15a] = 0;
    work[0x15c] = 0;
    fn_8003E394();
    lbl_803A6818.field_10 = lbl_8047A4E8;
}
#pragma peephole reset

void fn_80041114(void* work)
{
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80041E48(work, 0);
}

void fn_8004115C(void* work)
{
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80042658(work, 1);
}

void fn_800411A4(void* work)
{
    GSscene_SetMode(4);
    fn_800439BC(lbl_803A67FC);
    fn_80042658(work, 0);
}

s32 fn_800411EC(void)
{
    return lbl_803A6818.field_28;
}

#pragma peephole off
void fn_80041B5C(PdaSprite* sprite, PdaEvent* event)
{
    extern f32 lbl_8047BCA0;
    extern void fn_800411FC(PdaSprite* sprite, PdaEvent* event);

    sprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    switch (event->messageId) {
    case 0x331:
    case 0x759:
    case 0x76a:
    case 0xfbe:
        break;
    default:
        fn_800411FC(sprite, event);
        break;
    }
}
#pragma peephole reset

#pragma peephole off
void fn_80043CD8(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BCF4;
    f32 f0;
    f32 f3;
    s32 count_m1;

    alphaSprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    if (lbl_803A6818.field_10 > 10) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = *(f32*)((u8*)&lbl_803A6818 + 0x30);
    count_m1 = lbl_803A6818.field_10 - 1;
    f3 = -f0 / (f32)count_m1;
    sprite->field_52 = (s16)(f3 / lbl_8047BCF4 *
                                  *(s16*)(lbl_802EF0A8 + 0x5788) +
                              *(s16*)(lbl_802EF0A8 + 0x5784));
}
#pragma peephole reset

#pragma peephole off
void fn_80043DC0(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern s32 lbl_804788C0;
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BCA4;
    extern f32 lbl_8047BCA8;
    extern f32 lbl_8047BD0C;
    extern f64 sin(f64 angle);
    f32 angle;

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    if (lbl_803A6818.currentIndex != lbl_803A6818.field_10 - 1) {
        if (lbl_804788C0 != 0) {
            sprite->flags |= 2;
            angle = lbl_8047BCA8 *
                    (lbl_8047BCA8 * lbl_803A6818.angle);
            sprite->field_52 =
                (s16)(lbl_8047BD0C *
                          (f32)sin(lbl_8047BCA4 * angle) +
                      (f32)*(s16*)(lbl_802EF0A8 + 0x57F4));
            return;
        }
    }
    sprite->flags &= ~2;
}
#pragma peephole reset

#pragma peephole off
void fn_80043EC8(PdaSprite* alphaSprite, PdaSprite* sprite)
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
#pragma peephole reset

#pragma peephole off
void fn_800472CC(PdaSprite* alphaSprite)
{
    extern PdaOrbitPoint lbl_802E52C8[8];
    extern u32 lbl_8047A4EC;
    extern f32 lbl_8047BC94;
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BCC4;
    extern void fn_800473E0(PdaOrbitPoint* point);
    s32 i;

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    for (i = 0; i < 8; i++) {
        fn_800473E0(&lbl_802E52C8[i]);
    }
    for (i = (s32)lbl_8047A4EC - 1; i >= 0; i--) {
        lbl_802E52C8[i].alpha -= lbl_8047BCC4;
        if (lbl_802E52C8[i].alpha < lbl_8047BC94) {
            lbl_802E52C8[i].alpha = lbl_8047BC94;
        }
    }
    for (i = 7; i > lbl_8047A4EC; i--) {
        lbl_802E52C8[i].alpha -= lbl_8047BCC4;
        if (lbl_802E52C8[i].alpha < lbl_8047BC94) {
            lbl_802E52C8[i].alpha = lbl_8047BC94;
        }
    }
}
#pragma peephole reset

extern u8 lbl_802EF0A8[];
extern u8 lbl_80314F98[];
extern u8 lbl_80267190[];
extern u8 lbl_803A6A60[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D8;
extern u32 lbl_8047A4DC;
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

extern u32 fn_800D59B8();
extern u32 fn_800D5CB8();
extern u32 fn_800D61E4();
extern u32 fn_800D6728();
extern u32 fn_800D67BC();
extern u32 fn_800D6A00();
extern u32 fn_800D7820();
extern u32 fn_800D85D4();
extern u32 fn_800D888C();
extern u32 fn_800D88DC();
extern u32 fn_800E202C();
extern u32 fn_800E209C();
extern u32 fn_800E24B0();
extern u32 fn_800E27B0();
extern u32 fn_800E2C04();
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
extern u32 fn_80166AB8(u32, u32, u32);
extern u32 fn_8017B1CC();
extern u32 fn_8017B2CC();
extern u32 fn_8017B3E4();
extern u32 fn_801EE0BC();
extern u16 fn_801EE248();
extern u32 fn_801EE328();
extern u16 fn_801EE614();
extern u32 fn_801EE750();
extern u8 fn_801EE8F4();
extern u8 fn_801EEAD0();
extern u8 fn_801EEC74();
extern u8 fn_801EED88(u16);
extern u16 fn_801EEFAC();
extern u32 fn_801FCC7C();
extern u32 fightTrainerDataBiosGetPtr();
extern u32 gamedataGetStatus();
extern void menuCloseSync(s32, s32);
extern u32 fn_80018F54();
extern u32 fn_800F915C();

#pragma peephole off
void fn_8003AFDC(u8* context, PdaSprite* sprite)
{
    u16 battleId;
    u32 texture;
    s32 state;
    s32 messageId;
    u8* layout;

    switch (sprite->eventId) {
    case 0xD96:
    case 0xD97:
    case 0xD99:
    case 0x308:
    case 0x30B:
        break;
    case 0xD98:
        battleId = lbl_8047A4D4[lbl_803A6748.field_08].battleId;
        if (fn_801EE8F4(battleId) != 0 &&
            fn_80109B90((u8*)&lbl_803A6748 + 0x4C, 0) == 0) {
            texture = menuModelRender((u8*)&lbl_803A6748 + 0x4C);
            if (texture != 0) {
                fn_800D88DC(3);
                fn_800D888C(4);
                fn_800D6A00(7);
                fn_800D7820(lbl_80314F98);
                fn_800D85D4(0, texture);
                fn_800D67BC(2);
                fn_800D61E4(0, 0);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D59B8(0, lbl_8047BAC4, lbl_8047BAC4);
                fn_800D61E4(sprite->x, sprite->y);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D59B8(0, lbl_8047BAC8, lbl_8047BAC8);
                fn_800D6728();
            }
        }
        break;
    default:
        battleId = lbl_8047A4D4[lbl_803A6748.field_08].battleId;
        fn_801EED88(battleId);
        fn_801EE614(battleId);
        fn_801EE8F4(battleId);
        if (fn_801EEAD0(battleId) != 0) {
            if (fn_801EEC74(battleId) != 0) {
                state = 1;
            } else {
                state = 0;
            }
        } else {
            state = 2;
        }
        layout = lbl_802EF0A8;
        if (state == 0) {
            fn_801040F0(
                *(s16*)(layout + 0x55FA) - sprite->field_50 - 7,
                *(s16*)(layout + 0x55FC) - sprite->field_52 - 4,
                context, 0x161, 0);
        } else if (state == 1) {
            fn_801040F0(
                *(s16*)(layout + 0x55FA) - sprite->field_50,
                *(s16*)(layout + 0x55FC) - sprite->field_52,
                context, 0x160, 0);
        }
        if (fn_801EE8F4(battleId) == 0) {
            messageId = 0x36E8;
        } else if (fn_801EEC74(battleId) == 0) {
            messageId = 0x36E6;
        } else {
            messageId = 0x36E7;
        }
        fn_800FB680(0, 0, context[0x8B] | -0x100, (void*)messageId);
        break;
    }
}
#pragma peephole reset

#pragma peephole off
void fn_800492CC(u8* context, PdaSprite* sprite)
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
    if (*((u8*)&lbl_803A6818 + 0x214) != 0) {
        firstTexture = menuModelRender((u8*)&lbl_803A6818 + 0x7C);
        if (mode == 5) {
            secondTexture = menuModelRender((u8*)&lbl_803A6818 + 0xC4);
        }
    } else {
        firstTexture = 0;
    }
    alpha = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x5C);
    if (firstTexture != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, firstTexture);
        fn_800D67BC(2);
        fn_800D61E4(lbl_804788C8[0], lbl_804788C8[1]);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        fn_800D59B8(0, lbl_8047BC94, lbl_8047BC94);
        fn_800D61E4(lbl_804788C8[0] + sprite->x,
                     lbl_804788C8[1] + sprite->y);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        fn_800D59B8(0, lbl_8047BCBC, lbl_8047BCBC);
        fn_800D6728();
    }
    if (secondTexture != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, secondTexture);
        fn_800D67BC(2);
        fn_800D61E4(lbl_804788C8[0], lbl_804788C8[1]);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        fn_800D59B8(0, lbl_8047BC94, lbl_8047BC94);
        fn_800D61E4(lbl_804788C8[0] + sprite->x,
                     lbl_804788C8[1] + sprite->y);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        fn_800D59B8(0, lbl_8047BCBC, lbl_8047BCBC);
        fn_800D6728();
    }
}
#pragma peephole reset

#pragma peephole off
void fn_80040018(u8* menu)
{
    u8* scene;
    f32 target;
    f32 current;
    f32 step;
    f32 difference;

    scene = (u8*)&lbl_803A6818;
    target = *(f32*)(scene + 0x50);
    current = *(f32*)(scene + 0x4C);
    if (target != current) {
        step = *(f32*)(scene + 0x3C) / lbl_8047BCB8;
        if (target > current) {
            current += step;
            if (current > lbl_8047BCBC) {
                current = lbl_8047BCBC;
            }
        } else {
            current -= step;
            if (current < lbl_8047BC94) {
                current = lbl_8047BC94;
            }
        }
        *(f32*)(scene + 0x4C) = current;
    }

    target = *(f32*)(scene + 0x58);
    current = *(f32*)(scene + 0x54);
    if (target != current) {
        step = *(f32*)(scene + 0x3C) / lbl_8047BCB8;
        if (target > current) {
            current += step;
            if (current > lbl_8047BCBC) {
                current = lbl_8047BCBC;
            }
        } else {
            current -= step;
            if (current < lbl_8047BC94) {
                current = lbl_8047BC94;
            }
        }
        *(f32*)(scene + 0x54) = current;
    }

    target = *(f32*)(scene + 0x60);
    current = *(f32*)(scene + 0x5C);
    if (target != current) {
        step = *(f32*)(scene + 0x3C) / lbl_8047BCC0;
        if (target > current) {
            current += step;
            if (current > lbl_8047BCBC) {
                current = lbl_8047BCBC;
            }
        } else {
            current -= step;
            if (current < lbl_8047BC94) {
                current = lbl_8047BC94;
            }
        }
        *(f32*)(scene + 0x5C) = current;
    }

    current = *(f32*)(scene + 0x1DC);
    target = *(f32*)(scene + 0x1E0);
    if (current != target) {
        difference = target - current;
        step = lbl_8047BCC4 * difference * *(f32*)(scene + 0x3C);
        if (step > lbl_8047BC98) {
            step = lbl_8047BC98;
        }
        if (step <= lbl_8047BCC8) {
            step = lbl_8047BCC8;
        }
        current = *(f32*)(scene + 0x1DC) + step;
        difference = target - current;
        *(f32*)(scene + 0x1DC) = current;
        if (step < lbl_8047BC94) {
            step = -step;
        }
        if (difference < lbl_8047BC94) {
            current = -difference;
        } else {
            current = difference;
        }
        if (current <= step || current < lbl_8047BCBC) {
            *(f32*)(scene + 0x1DC) = target;
        }
    }

    if (*(f32*)(scene + 0x50) == *(f32*)(scene + 0x4C) && menu != 0) {
        u8* keyInfo;
        u16 buttons;

        keyInfo = (u8*)windowGetKeyInfo(
            menuDataBiosGetPtr(*(s32*)(menu + 4)));
        buttons = *(u16*)(keyInfo + 4);
        if (lbl_804788C0 != 0) {
            if (buttons & 0x10) {
                fn_80166AB8(0x24, 0, 0);
                menu[0x98] = 1;
                *(s32*)(scene + 0x18) = 0;
            }
            if (buttons & 0x40) {
                fn_80166AB8(0x24, 0, 0);
                menu[0x98] = 1;
                *(s32*)(scene + 0x18) = 1;
            }
        }
        if (buttons & 0x20) {
            fn_80166AB8(0x25, 0, 0);
            *(s32*)(scene + 0x18) = -1;
            menu[0x98] = 1;
            menu[0x99] = 1;
        }
    }
}
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
    extern u32 pokemonBiosGetPokemonDataId(u32 work);
    extern void* pokemonDataBiosGetPtr(u32 id);
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
            fn_800FB680(0, (s32)y - 2, (u32)context[0x8B] | -0x100LL,
                        (void*)0xE7);
        }
        y += lbl_8047BCF4;
        offset += 2;
    }
}

/* Species-name column of the memo list. */
void fn_80044378(u8* context, PdaSprite* sprite)
{
    extern void msgctrlSetValue(s32 id, u32 value);
    extern u16 memoDataGetCount(s32 a);
    extern u32 GSmsgGetGSchar(u32 msg);
    extern s8 fn_8004BDEC(void);
    extern s8 fn_8004BDFC(void);
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
        if (fn_8004BDEC() == 1 && fn_8004BDFC() >= 1) {
            fn_800492CC(context, sprite);
        }
        break;
    case 0x76D:
        msgctrlSetValue(0x34, memoDataGetCount(0));
        break;
    default:
        entry = lbl_802EF0A8 + messageId * 0x1C;
        fn_800FE38C(
            *(s16*)(lbl_802EF0A8 + 0x5712) - *(s16*)(entry + 2),
            *(s16*)(lbl_802EF0A8 + 0x5714) - *(s16*)(entry + 4),
            *(s16*)(lbl_802EF0A8 + 0x5716),
            *(s16*)(lbl_802EF0A8 + 0x5718));
        pdaDrawNameRows(context, 0,
                        *(f32*)((u8*)&lbl_803A6818 + 0x30));
        fn_800FE35C();
        break;
    }
}

#pragma peephole off
void fn_8003B478(u8* context)
{
    u32 index;
    u16 species;
    u16 battleId;
    u16 curBattleId;
    u32 model;
    u32 liveModel;
    u32 rnd;
    u32 message;
    u32 msg;
    s32 battleState;
    u8 seen;
    u8 caught;

    context[0x8B] = lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44);
    index = *(u32*)&lbl_803A6748;
    species = lbl_8047A4D4[index].field_00;
    battleId = lbl_8047A4D4[index].battleId;
    curBattleId = lbl_8047A4D4[(u16)index].battleId;
    model = lbl_8047A4D0;
    fn_801EE614(curBattleId);
    fn_801EE8F4(curBattleId);
    seen = fn_801EEAD0(curBattleId);
    caught = fn_801EEC74(curBattleId);
    if (seen != 0) {
        if (caught != 0) {
            battleState = 1;
        } else {
            battleState = 0;
        }
    } else {
        battleState = 2;
    }

    fn_801240C4(model, species, 0xA, gamedataGetStatus(0, 1));
    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
    fn_801EE750(battleId);
    fn_8011DFE0(model, rnd);
    switch (battleState) {
    case 0:
    case 2:
        fn_8011D8F4(model, 1);
        fn_8011D8D8(model, 0xA);
        break;
    case 1:
        fn_8011D8F4(model, 0);
        fn_8011D8D8(model, 0);
        break;
    }

    liveModel = lbl_8047A4D0;
    if (liveModel != 0) {
        if (battleId != 0) {
            if (fn_801EE8F4(battleId) != 0) {
                fn_8011DFE0(liveModel, fn_801EE750(battleId));
                message = pokemonGetSex(liveModel);
                fn_8011DFE0(liveModel, *(u32*)((u8*)&lbl_803A6748 + 0x98));
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
        msg = fn_800FA280(1);
    } else {
        switch (message) {
        case 0:
            msg = fn_800FA280(0xD67);
            break;
        case 1:
            msg = fn_800FA280(0xD68);
            break;
        case 2:
            msg = fn_800FA280(0xD69);
            break;
        default:
            msg = fn_800FA280(0xD68);
            break;
        }
    }
    fn_80132A38(0x37, msg);
    {
        u32 alpha = context[0x8B];
        s32 color = alpha | -0x100LL;

        fn_800FB680(0, 0, color, (void*)0xCF);
    }
}
#pragma peephole reset

#pragma peephole off
void fn_8004B598(s32 unused, PdaSprite* sprite, s32 messageId)
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
    angles[*(u32*)(lbl_803A6A60 + 0x44)] =
        angles[*(u32*)(lbl_803A6A60 + 0x44)] + step;
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] > lbl_8047BDF0) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] =
            angles[*(u32*)(lbl_803A6A60 + 0x44)] - lbl_8047BDF0;
    }
    if (angles[*(u32*)(lbl_803A6A60 + 0x44)] < lbl_8047BDAC) {
        angles[*(u32*)(lbl_803A6A60 + 0x44)] =
            angles[*(u32*)(lbl_803A6A60 + 0x44)] + lbl_8047BDF0;
    }
    sprite->value = angles[*(u32*)(lbl_803A6A60 + 0x44)];
    sprite->field_50 = layout[1];
    sprite->field_52 = layout[2];
}
#pragma peephole reset

static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = fn_800E2C04(size, align);

    if ((u16)h != 0) {
        return (void*)fn_800E27B0(h);
    }
    return NULL;
}

#pragma peephole off
void fn_8003CF38(void)
{
    u8 temporary[8];
    u8* sortEntries;
    u8* lower;
    s32 count;
    s32 gap;
    s32 i;
    s32 j;
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

        value = fn_801EE248((u16)(i + 1));
        if (value != 0 &&
            (fn_801EE8F4((u16)(i + 1)) != 0 ||
             fn_801EE614((u16)(i + 1)) != 0)) {
            lbl_8047A4D4[count].field_00 = value;
            lbl_8047A4D4[count].battleId = (u16)(i + 1);
            *(u32*)((u8*)lbl_8047A4D8 + count * 8) =
                fn_801EE0BC();
            ((PdaListEntry*)((u8*)lbl_8047A4D8 + count * 8 + 4))->field_00 =
                lbl_8047A4D4[count].field_00;
            ((PdaListEntry*)((u8*)lbl_8047A4D8 + count * 8 + 4))->battleId =
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
        lbl_8047A4D4[i].field_00 =
            ((PdaListEntry*)(sortEntries + i * 8 + 4))->field_00;
        lbl_8047A4D4[i].battleId =
            ((PdaListEntry*)(sortEntries + i * 8 + 4))->battleId;
    }

    handle = fn_800E202C(lbl_8047A4D8);
    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
    lbl_8047A4D8 = 0;
}
#pragma peephole reset

#pragma peephole off
u32 fn_80043728(u32 unused, s32 mode, u16 buttons)
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
        fn_80166AB8(0x23, 0, 0);
        *(f32*)(scene + 0x38) -= lbl_8047BCF4;
        *(s32*)(scene + 8) += 1;
        *(s32*)(scene + 0xC) += 1;
    } else if ((buttons & 1) && total != 0 &&
               *(s32*)scene > 0) {
        *(s32*)scene -= 1;
        fn_80166AB8(0x23, 0, 0);
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
            fn_80166AB8(0x23, 0, 0);
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
            fn_80166AB8(0x23, 0, 0);
        }
    }
    return 0;
}
#pragma peephole reset

void fn_8003A520(void)
{
    extern s32 fn_80039498(s32 value);
    s32 result;

    result = 0;
    while ((u32)(result - 3) > 1) {
        result = fn_80039498(result);
        switch (result) {
        case 0:
            fn_80102510(0x19);
            fn_80102510(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            fn_8003A10C(0);
            fn_8010264C(0x1B, 0);
            fn_8010264C(0x19, 0);
            break;
        case 1:
            fn_80102510(0x19);
            fn_80102510(0x1A);
            fn_80102510(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1A, 1);
            menuCloseSync(0x1B, 1);
            fn_8017B3E4(0x66F);
            while (fn_8017B2CC(0x66F) == 1) {
                _threadSwitch();
            }
            fn_80018F54(4, 0, 0);
            fn_8017B1CC(0x66F);
            fn_800F915C(0x66F);
            fn_8010264C(0x1A, 0);
            fn_8010264C(0x1B, 0);
            fn_8010264C(0x19, 0);
            break;
        case 2:
            fn_80102510(0x19);
            fn_80102510(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            fn_8003A10C(1);
            fn_8010264C(0x1B, 0);
            fn_8010264C(0x19, 0);
            break;
        }
    }
}

#pragma peephole off
void fn_8003B2D8(u8* context)
{
    u8* window;
    u16 battleId;
    u32 message;
    u8 secondary;
    u32 caption;
    s32 variant;

    extern s32 fn_801EE544(u16, s8*);

    window = (u8*)&lbl_803A6748;
    context[0x8B] = lbl_8047BAC0 * *(f32*)(window + 0x44);
    secondary = 0;
    if (lbl_8047A4D0 != 0) {
        battleId = lbl_8047A4D4[(u16)*(u32*)window].battleId;
        message = fn_801EE544(battleId, (s8*)(window + 0x94));
        variant = *(u8*)(window + 0x94);
        switch (variant) {
        case 0:
            message = 0x371F;
            break;
        case 1:
        case 2:
            if (battleId == 0x43) {
                message = 0x12B0;
            } else if (fn_801EEFAC(battleId, 0) == 9) {
                message = fn_801EE328(battleId);
                secondary = 1;
            } else {
                fightTrainerDataBiosGetPtr();
                message = fn_801FCC7C();
            }
            break;
        }
        if (secondary == 0) {
            message = fn_800FA280(message);
        }
    } else {
        message = secondary;
    }
    if (message == 0) {
        message = fn_800FA280(1);
    }
    caption = fn_8003CE1C(*(u32*)window);
    if (caption == 0) {
        caption = fn_800FA280(1);
    }
    fn_80132A38(0x37, caption);
    {
        u32 alpha = context[0x8B];

        fn_800FB680(0, 0, alpha | -0x100LL, (void*)0xE7);
    }
    fn_80132A38(0x37, message);
    {
        u32 alpha = context[0x8B];

        fn_800FB680(0xB4, 0, alpha | -0x100LL, (void*)0xE7);
    }
}
#pragma peephole reset
/* Readable ports reconstructed from the PDA callback state machines. */
#pragma peephole off
void fn_80037180(u32 unused, u8* p) {
    u32 result;
    switch (*(s16*)(p + 6)) {
    case 0x36c:
        result = fn_80005748();
        if (lbl_8047A498 != 0) {
            result = lbl_8047A498;
        }
        lbl_8047A49C = fn_801EF214();
        lbl_8047A4A0 = fn_801EF274();
        fn_80132A38(0x4c, result);
        fn_80132A38(0x2f, fn_801EF274());
        fn_80132A38(0x30, fn_801EF214());
        if ((result = menuModelRender(lbl_803A6498)) != 0) {
            fn_800D88DC(3);
            fn_800D888C(4);
            fn_800D6A00(7);
            fn_800D7820(lbl_80314F98);
            fn_800D85D4(0, result);
            fn_800D67BC(2);
            fn_800D61E4(0, 0);
            fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
            fn_800D59B8(0, lbl_8047BA58, lbl_8047BA58);
            fn_800D61E4(*(s16*)(p + 0x54), *(s16*)(p + 0x56));
            fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
            fn_800D59B8(0, lbl_8047BA5C, lbl_8047BA5C);
            fn_800D6728();
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

s32 fn_80039F70(u8* ctx)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);
    extern u8 fn_801429E8(void* item);
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
                if ((u8)fn_801429E8(item) != 0) {
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

#pragma peephole off
void fn_8003B6D0(u8* ctx) {
    u8* base;
    u32 battleId;
    u32 record;
    u32 message;
    u32 value;
    base = (u8*)&lbl_803A6748;
    ctx[0x8b] = (u8)(lbl_8047BAC0 * *(f32*)(base + 0x44));
    value = *(u32*)base;
    battleId = (u32)(u16)*(u16*)((u8*)lbl_8047A4D4 + value * 4 + 2);
    message = 0;
    if ((u32)(u16)fn_801EEFAC(battleId, 0) == 9U) {
        fn_801EE248(battleId);
        value = fn_8011E778();
        if (value != 0) {
            message = fn_800FA280(fn_8011E760());
        }
    } else {
        record = fn_801FCA2C(fn_801FCC3C(fightTrainerDataBiosGetPtr()));
        message = fn_800FA280(fn_801FCA14(record));
        while (1) {
            value = (u32)(u8)fn_801FC964(record);
            if (battleId == value) {
                value = fn_801FCA14(record);
                if (value != 0) {
                    message = fn_800FA280(value);
                }
                break;
            }
            record += 0x50;
        }
    }
    if (message == 0) { message = fn_800FA280(1); }
    fn_80132A38(0x37, message);
    fn_800FB680(0, 0, (s32)ctx[0x8b] | -0x100, (void*)0xcf);
}
#pragma peephole reset

#pragma peephole off
void fn_8003F040(void) {
    u8 tmp[0x1a];
    u8* base;
    u16* idx;
    s32 count;
    u32 i;
    s32 j;
    s32 gap;
    s32 k;
    u32 src_id;
    u32 lookup;
    u32 ctx;
    u32 allocation;
    u32 a;
    u8* p;
    u8* q;
    u32 handle;

    allocation = fn_800E2C04(0x28a0, 0x20);
    if ((u16)allocation != 0) {
        allocation = fn_800E27B0();
    } else {
        allocation = 0;
    }
    base = (u8*)allocation;

    allocation = fn_800E2C04(0x320, 0x20);
    if ((u16)allocation != 0) {
        allocation = fn_800E27B0();
    } else {
        allocation = 0;
    }
    idx = (u16*)allocation;

    for (i = 0; (u16)i < (u16)(*(u16*)&lbl_8047A4E8); i++) {
        u32 ii = (u16)i;
        src_id = ((u16*)lbl_8047A4E4)[ii];
        idx[ii] = (u16)src_id;
        lookup = src_id;
        ctx = lbl_8047A4E0;
        if (ctx != 0) {
            if (src_id >= 0x8000) {
                lookup = src_id & 0x3FFF;
            }
            allocation = gamedataGetStatus(0, 1);
            fn_801240C4(ctx, lookup & 0xFFFF, 0xa, allocation);
            a = fn_8025FDDC(0, lookup);
            lookup = fn_8025FD34(0, lookup);
            fn_8011DFE0(ctx, a);
            fn_8011DF90(ctx, lookup);
            allocation = lbl_8047A4E0;
        } else {
            allocation = 0;
        }
        if (allocation != 0) {
            fn_8011F5C8();
            fn_8011E778();
            allocation = fn_8011E760();
        } else {
            allocation = 0;
        }
        p = base + ii * 0x1a;
        fn_800F96E4(p + 2, 0x18, (u8*)allocation);
        *(u16*)p = i;
    }

    count = (s32)*(u16*)&lbl_8047A4E8;
    for (gap = count / 2; gap > 0; gap = gap / 2) {
        for (j = gap; j < count; j++) {
            k = j - gap;
            p = base + k * 0x1a;
            /* Retail emits the three memcpys before GScharCmp, i.e. the
             * top-test rotation with the compare in the loop condition. */
            q = base + (k + gap) * 0x1a;
            while (k >= 0 &&
                   GScharCmp((u32)(p + 2), (u32)(q + 2)) >= 0) {
                memcpy(tmp, p, 0x1a);
                memcpy(p, q, 0x1a);
                memcpy(q, tmp, 0x1a);
                p -= gap * 0x1a;
                k -= gap;
                q = base + (k + gap) * 0x1a;
            }
        }
    }

    for (i = 0; (u16)i < (u16)(*(u16*)&lbl_8047A4E8); i++) {
        u32 ii = (u16)i;
        ((u16*)lbl_8047A4E4)[ii] = idx[*(u16*)(base + ii * 0x1a)];
    }

    if (base != 0) {
        handle = fn_800E202C((u32)base);
        if ((u16)handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
    if (idx != 0) {
        handle = fn_800E202C((u32)idx);
        if ((u16)handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
}
#pragma peephole reset

#pragma peephole off
void fn_8003F2DC(u8* arr, s32 count, s32 dir) {
    s32 gap;
    s32 i;
    s32 j;
    u8* a;
    u8* b;
    u8 tmp[4];

    if (dir == 0) {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                a = arr + (i - gap) * 4;
                j = i - gap;
                while (j >= 0) {
                    b = arr + (j + gap) * 4;
                    if (*(u16*)(a + 2) <= *(u16*)(b + 2)) { break; }
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= gap * 4;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    } else {
        gap = count / 2;
        while (gap > 0) {
            for (i = gap; i < count; i++) {
                a = arr + (i - gap) * 4;
                j = i - gap;
                while (j >= 0) {
                    b = arr + (j + gap) * 4;
                    if (*(u16*)(a + 2) >= *(u16*)(b + 2)) { break; }
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= gap * 4;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    }
}
#pragma peephole reset

/* Main summary-screen state machine. */
#pragma peephole off
void fn_80044630(void) {
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

    extern void  fn_80048918(void);
    extern void  fn_80046168(void);
    extern void  fn_8004BDB8(s32 a, s32 b);
    extern void* fn_801CBA0C(u32 id);
    extern void* GSresGetResource();
    extern void* fn_80113F48(void);
    extern void  fn_801CB9D8(void* h);
    extern void  fn_801CB954(void* h, s32 b);
    extern void  GSmodelSetVisibility(void* a, s32 b);
    extern void  GSmodelSetAnimRate(f32 v);
    extern void  GSmodelSet60fpsAnimFlag(void* h, s32 b);
    extern void  GSmodelSetBoundCheck(void* h, s32 b);
    extern void  GSmodelDestroyLinkedParticles(void);
    extern void* fn_801DAC3C(void* h);
    extern void  cameraPlayAnime(u32 a, u32 id, u32 c, u32 d);
    extern void  fn_8010A420(void* subobj);
    extern void  menuOffScreenSetDisp(s32 a);
    extern u32   fn_800E202C(u32 flag);
    extern void  fn_800E24B0(void);
    extern void  fn_800E209C(u32 h);
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

    fn_80048918();
    tbl = lbl_80267150;
    S = (u8*)&lbl_803A6818;

    S118 = S + 0x118;
    S124 = S + 0x124;
    S130 = S + 0x130;
    S218 = S + 0x218;

    while (1) {
        switch (*(s32*)(S + 0x1C)) {
        case 0: {   /* .L_800446F0 */
            fn_8004BDB8(-1, 1);
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
            fn_80046168();
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                step = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (step > lbl_8047BC98)
                    step = lbl_8047BC98;
                if (step <= lbl_8047BCC8)
                    step = lbl_8047BCC8;
                nval = *(f32*)(S + 0x1DC) + step;
                tgt = *(f32*)(S + 0x1E0);
                rem = tgt - nval;
                *(f32*)(S + 0x1DC) = nval;
                if (step <= lbl_8047BC94)
                    step = -step;
                if (rem > lbl_8047BC94)
                    mag = rem;
                else
                    mag = -rem;
                if (mag <= step) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem <= lbl_8047BC94)
                        rem = -rem;
                    if (rem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
                continue;
            }
            *(s32*)(S + 0x14) = 1;
            menuCloseCustom(0x35, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = GSresGetResource(fn_80113F48(), h);
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

                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                cam = fn_801CBA0C(0x0D181000);
                *(void**)(S + 0x10C) = cam;
                *(void**)(S + 0x110) = GSresGetResource(fn_80113F48(), cam);
                GSmodelSetAnimRate(lbl_8047BD18);
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
                fn_8004BDB8(-1, 2);

                {
                    s32 sub = *(s32*)(S + 0x28);
                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;
                    switch (sub) {
                    case 1:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 4;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            GSmodelDestroyLinkedParticles();
                        *(f32*)(S + 0x64) = lbl_8047BCA8;
                        break;
                    case 2:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 5;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            GSmodelDestroyLinkedParticles();
                        break;
                    case 0:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 2;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            GSmodelDestroyLinkedParticles();
                        break;
                    default:
                        break;
                    }
                }

                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
                continue;
            }
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
            } else {
                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x3C, 0, 1);
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);

                h = *(void**)(S + 0x10C);
                fn_801CB954(h, 0);
                {
                    void* rv = GSresGetResource(fn_80113F48(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = fn_801CBA0C(0x0D171000);
                *(void**)(S + 0x10C) = h;
                *(void**)(S + 0x110) = GSresGetResource(fn_80113F48(), h);
                GSmodelSetAnimRate(lbl_8047BD18);

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
            }
        } break;

        case 9: {   /* .L_80045178 */
            f32 step;
            f32 cur;
            f32 tgt;
            f32 delta;
            f32 rem;
            f32 acur;
            f32 arem;
            f32 astep;
            f32 px3;
            f32 px2;
            f32 px1;
            f32 px0;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x4C) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x4C) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x54) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x54) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x5C) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x5C) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            acur = *(f32*)(S + 0x1DC);
            tgt  = *(f32*)(S + 0x1E0);
            if (acur != tgt) {
                delta = tgt - acur;
                astep = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (astep > lbl_8047BC98)
                    astep = lbl_8047BC98;
                if (astep <= lbl_8047BCC8)
                    astep = lbl_8047BCC8;
                acur = *(f32*)(S + 0x1DC) + astep;
                rem  = tgt - acur;
                *(f32*)(S + 0x1DC) = acur;
                if (astep > lbl_8047BC94) {
                } else {
                    astep = -astep;
                }
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= astep) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94)
                        rem = rem;
                    else
                        rem = -rem;
                    if (rem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
                continue;
            }

            *(s32*)(S + 0x1C) = 0;
            menuCloseCustom(0xa1, 0, 1);
            menuCloseCustom(0x96, 0, 1);
            menuCloseCustom(0xa2, 0, 1);

            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = GSresGetResource(fn_80113F48(), h);
                if (rv != 0)
                    GSmodelSetVisibility(rv, 0);
            }

            /* .L_800453AC camera-setup */
            fn_801CB9D8(h);
            *(f32*)(S + 0x6C) = lbl_8047BC94;
            *(f32*)(S + 0x70) = lbl_8047BC94;
            *(f32*)(S + 0x74) = lbl_8047BC94;
            *(f32*)(S + 0x64) = lbl_8047BCF0;

            h = fn_801CBA0C(0x0D171000);
            *(void**)(S + 0x10C) = h;
            *(void**)(S + 0x110) = GSresGetResource(fn_80113F48(), h);
            GSmodelSetAnimRate(lbl_8047BD18);

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
            continue;
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                gstep = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (gstep > lbl_8047BC98)
                    gstep = lbl_8047BC98;
                if (gstep <= lbl_8047BCC8)
                    gstep = lbl_8047BCC8;
                newcur = *(f32*)(S + 0x1DC) + gstep;
                rem    = *(f32*)(S + 0x1E0) - newcur;
                *(f32*)(S + 0x1DC) = newcur;
                if (gstep <= lbl_8047BC94)
                    gstep = -gstep;
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= gstep) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94)
                        arem = rem;
                    else
                        arem = -rem;
                    if (arem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
                continue;
            }

            *(s32*)(S + 0x1C) = 0;
            menuCloseCustom(0x96, 0, 1);
            menuCloseCustom(0xA2, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = GSresGetResource(fn_80113F48(), h);
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

                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = fn_801CBA0C(0x0D171000);
                *(void**)(S + 0x10C) = h;

                *(void**)(S + 0x110) = GSresGetResource(fn_80113F48(), h);
                GSmodelSetAnimRate(lbl_8047BD18);

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

                continue;
            }
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
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
            f32 dlt;
            f32 dstep;
            f32 ncur;
            f32 rem;
            f32 arem;
            f32 dtgt;

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
            cur = *(f32*)(S + 0x1DC);
            dtgt = *(f32*)(S + 0x1E0);
            if (cur != dtgt) {
                dlt = dtgt - cur;
                dstep = lbl_8047BCC4 * dlt;
                dstep = dstep * *(f32*)(S + 0x3C);
                if (dstep > lbl_8047BC98)
                    dstep = lbl_8047BC98;
                if (dstep <= lbl_8047BCC8)
                    dstep = lbl_8047BCC8;
                ncur = *(f32*)(S + 0x1DC) + dstep;
                dtgt = *(f32*)(S + 0x1E0);
                rem = dtgt - ncur;
                *(f32*)(S + 0x1DC) = ncur;
                if (dstep > lbl_8047BC94) {
                } else {
                    dstep = -dstep;
                }
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= dstep) {
                    *(f32*)(S + 0x1DC) = dtgt;
                } else {
                    if (rem <= lbl_8047BC94) {
                        rem = -rem;
                    }
                    if (rem < lbl_8047BCBC) {
                        *(f32*)(S + 0x1DC) = dtgt;
                    }
                }
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
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
    h = *(void**)(S + 0x10C);
    fn_801CB954(h, 0);
    {
        void* rv = GSresGetResource(fn_80113F48(), h);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    fn_801CB9D8(h);

        h = *(void**)((u8*)&lbl_803A6818 + 0x150);
    fn_801CB954(h, 0);
    {
        void* rv = GSresGetResource(fn_80113F48(), h);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    fn_801CB9D8(h);

    if (lbl_8047A4E0 != 0) {
        h = (void*)fn_800E202C(lbl_8047A4E0);
        if ((u16)(u32)h != 0) {
            fn_800E24B0();
            fn_800E209C((u32)h);
        }
        lbl_8047A4E0 = 0;
    }

    menuOffScreenSetDisp(1);
        fn_8010A420((u8*)&lbl_803A6818 + 0x7c);
        fn_8010A420((u8*)&lbl_803A6818 + 0xc4);

        h = (void*)fn_800E202C((u32)lbl_8047A4E4);
    if ((u16)(u32)h != 0) {
        fn_800E24B0();
        fn_800E209C((u32)h);
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
extern u32 pokemonBiosGetPokemonDataId(u32 work);
extern void* pokemonDataBiosGetPtr(u32 id);
extern u8 pokemonDataBiosGetColor(void* data, s32 index);
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);

/* Reload the scratch Pokemon slot from a memo entry ID. */
static inline u32 pdaLoadPokemon(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 species;
    u32 rnd;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        species = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, species);
        return lbl_8047A4E0;
    }
    return 0;
}

/* As pdaLoadPokemon, but from an entry ID the caller already has in hand. */
static inline u32 pdaLoadPokemonId(u16 id)
{
    u32 work = lbl_8047A4E0;
    u32 species = id;
    u32 rnd;

    if (work != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        species = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, species);
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
#pragma peephole off
void fn_80041BD0(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u16 lbl_802E554C[];
    u32 pokemon;
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
#pragma peephole reset

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
extern u8 fn_80047CC0(u8* work);
extern u8 fn_800478B4(void* work, void* sub);


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

s32 fn_8003D1FC(void)
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
        *((u8*)&lbl_803A6818 + 0x214) = fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
        break;
    case 5:
        *((u8*)&lbl_803A6818 + 0x214) =
            fn_800478B4((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        *((u8*)&lbl_803A6818 + 0x214) =
            fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }
    return 0;
}

/* Rebuild the memo index table, optionally filtered by the active color tab. */
#pragma peephole off
u16 fn_8003D4C8(void)
{
    extern u16 memoDataGetCount(s32 a);
    extern u16 memoDataGetPokemonID(s32 a, u16 index);
    extern void fn_8003D8CC(void);
    extern void fn_8003DC54(void);
    extern void fn_8003E394(void);
    u16 buf[500];
    u16 i;
    u16 count;
    u16 total;

    count = 0;
    for (i = 0; i < memoDataGetCount(0); i++) {
        lbl_8047A4E4[i] = memoDataGetPokemonID(0, i);
        count++;
    }
    lbl_8047A4E8 = count;
    fn_8003D8CC();
    if (*(s8*)((u8*)&lbl_803A6818 + 0x159) != 0) {
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                pdaLoadPokemonId(id);
                if (*(s8*)((u8*)&lbl_803A6818 + 0x159) ==
                    pdaGetPokemonColor(id)) {
                    buf[count] = id;
                    count++;
                }
            }
        }
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf[i];
        }
        lbl_8047A4E8 = count;
    }
    fn_8003DC54();
    fn_8003E394();
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
    u32 work = pdaLoadPokemonId(id);

    if (work != 0) {
        return pokemonDataBiosGetName(
            pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work)));
    }
    return NULL;
}

/* Filter the memo index table down to the active kana-row tab. */
#pragma peephole off
void fn_8003D8CC(void)
{
    u16 nameBuf[0x80];
    PdaNameEntry tbl[15];
    u16 out[500];
    PdaKanaGroup* group;
    s32 tab;
    s32 n;
    u16 i;
    u16 j;
    u16 k;
    u16 count;
    u16 total;

    tab = *(s8*)((u8*)&lbl_803A6818 + 0x158);
    if (tab == 0) {
        return;
    }
    for (n = 0; n < 15; n++) {
        tbl[n].used = 0;
    }
    if (tab != 0) {
        group = &lbl_802E60B0[0][tab - 1];
        for (n = 0; n < group->count; n++) {
            tbl[n].used = 1;
            GScharMakeFromSJIS(tbl[n].text, group->words[n]);
        }
    }
    count = group->count;
    total = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        u16 id = lbl_8047A4E4[i];
        pdaLoadPokemonId(id);
        if ((u32)fn_800F96E4((u8*)nameBuf, 0x100, (u8*)pdaGetPokemonName(id)) !=
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

/* Sample the stick and buttons into the PDA's own key-repeat state. */
#pragma peephole off
void fn_800439BC(void* scene)
{
    extern s8 fn_800F7A08(s32 chan, s32 index);
    extern s8 fn_800F7A7C(s32 chan, s32 index);
    extern u32 fn_800F7BC4(s32 chan);
    extern s8 fn_800D3088(void);
    extern f64 atan2(f64 y, f64 x);
    s8 x;
    s8 y;
    f32 ang;
    u16 buttons;
    u16 held;
    u16 trig;
    u32 raw;
    s32 i;
    u8* work = (u8*)scene;

    buttons = 0;
    *(u16*)(work + 2) = *(u16*)work;
    x = fn_800F7A08(1, 0);
    y = fn_800F7A7C(1, 0);
    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
        ang = atan2((f64)y, (f64)x);
        if (pdaFabs(ang) < lbl_8047BCFC) {
            buttons |= 2;
        } else if (pdaFabs(ang) > lbl_8047BD00) {
            buttons |= 1;
        }
        if (lbl_8047BD04 < pdaFabs(ang) && pdaFabs(ang) < lbl_8047BD08) {
            if (ang >= lbl_8047BC94) {
                buttons |= 8;
            } else {
                buttons |= 4;
            }
        }
    }
    raw = fn_800F7BC4(1);
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
            work[i + 0xa] -= fn_800D3088();
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
void fn_80043FA8(PdaSprite* alphaSprite, PdaSprite* sprite)
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
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x3717) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3717);
        break;
    case 0x10c8:
        if (lbl_803A6818.field_28 == 1) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x3718) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3718);
        break;
    case 0x10c9:
        if (lbl_803A6818.field_28 == 2) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x3719) >> 16)) / 2), 0,
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
extern void* fn_801DAC3C(void* h);
extern s32 fn_801DAC24(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void fn_800E064C(f32* mtx);
extern void fn_800E03B4(f32* mtx, void* vec);

/* Frame the two compared models in the PDA's side-by-side camera. */
#pragma peephole off
u8 fn_800478B4(void* work, void* sub)
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
        model0 = fn_801DAC3C(*(void**)((u8*)work + 0x24));
        if (model0 == NULL) {
            return 0;
        }
        fn_801DAC24(*(void**)((u8*)work + 0x24));
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
    height = bound0.y;
    width = bound0.x + bound1.x;
    if (height >= bound1.y) {
    } else {
        height = bound1.y;
    }
    persp0 = lbl_8047BD30;
    persp1 = (f32)*(s32*)((u8*)work + 0x2c) / (f32)*(s32*)((u8*)work + 0x30);
    if (height >= width) {
        dist = height;
    } else {
        dist = width;
    }
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
    fn_800E064C(mtx0);
    fn_800E03B4(mtx0, &pos0);
    GSmodelSetMatrix(model0, mtx0);
    GSmodelGetPosition(model1, &pos1);
    pos1.x = bound1.x * lbl_8047BD18;
    pos1.y = lbl_8047BC94;
    pos1.z = lbl_8047BC94;
    GSmodelSetPosition(model1, &pos1);
    fn_800E064C(mtx1);
    fn_800E03B4(mtx1, &pos1);
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
extern f32 lbl_8047BD68;
extern u8 lbl_802E540C[];
extern u8 lbl_802E5418[];
extern void memoGetScaleAngle(u32 id, f32* scale, f32* angle);
extern u8 menuModelCheck(void* work, s32 index);
extern void GSmodelCenterNull(void* model);
extern void modelRemoveCenterNull(void* model);
extern s32 fn_800EE0E8(void* model);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartGetTransform(void* part, void* out, s32 a, s32 b);
extern void GSpartFree(void* part);
extern void fn_800E032C(f32* mtx, f32 angle);

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

extern f64 lbl_8047BD50;
extern f64 lbl_8047BD58;
extern f64 lbl_8047BD60;

static inline f32 pdaSqrtf(f32 value)
{
    if (value > lbl_8047BC94) {
        f64 guess = __frsqrte(value);
        guess = lbl_8047BD50 * guess *
                (lbl_8047BD58 - value * (guess * guess));
        guess = lbl_8047BD50 * guess *
                (lbl_8047BD58 - value * (guess * guess));
        guess = lbl_8047BD50 * guess *
                (lbl_8047BD58 - value * (guess * guess));
        return (f32)(value * guess);
    }
    if ((f64)value < lbl_8047BD60) {
        return lbl_80478AC0[0];
    }
    if (pdaFpClassifyF(value) == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

/* Frame the highlighted Pokemon's model for the single-model PDA page. */
#pragma peephole off
u8 fn_80047CC0(u8* work)
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
    void* cam;
    f32 spread;
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
        model = fn_801DAC3C(*(void**)(work + 0x24));
        if (model == NULL) {
            return 0;
        }
        switch (fn_801DAC24(*(void**)(work + 0x24))) {
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
    persp0 = lbl_8047BD30;
    persp1 = (f32)*(s32*)(work + 0x2c) / (f32)*(s32*)(work + 0x30);
    GSmodelGetPosition(model, &modelPos);
    zoom = zoom * (pdaSqrtf(bound.y * bound.y + bound.x * bound.x) / spread) /
           (f32)tan(lbl_8047BD68 * persp0 * lbl_8047BD18);
    zoom = zoom * *(f32*)((u8*)&lbl_803A6818 + 0x68);
    GScameraSetPerspective(*(void**)(work + 0x38), persp0, persp1, persp2,
                           persp3);
    pitch = (f32)atan(
        pdaSqrtf((*(f32*)((u8*)&lbl_803A6818 + 0x228) -
                  *(f32*)((u8*)&lbl_803A6818 + 0x21c)) *
                 (*(f32*)((u8*)&lbl_803A6818 + 0x228) -
                  *(f32*)((u8*)&lbl_803A6818 + 0x21c))) /
        pdaSqrtf((*(f32*)((u8*)&lbl_803A6818 + 0x224) -
                  *(f32*)((u8*)&lbl_803A6818 + 0x218)) *
                     (*(f32*)((u8*)&lbl_803A6818 + 0x224) -
                      *(f32*)((u8*)&lbl_803A6818 + 0x218)) +
                 (*(f32*)((u8*)&lbl_803A6818 + 0x22c) -
                  *(f32*)((u8*)&lbl_803A6818 + 0x220)) *
                     (*(f32*)((u8*)&lbl_803A6818 + 0x22c) -
                      *(f32*)((u8*)&lbl_803A6818 + 0x220))));
    GSmodelGetPosition(model, &modelPos);
    target.x = lbl_8047BC94;
    target.z = lbl_8047BC94;
    target.y = lbl_8047BC94;
    fn_800E064C(mtx);
    fn_800E03B4(mtx, &target);
    fn_800E032C(mtx, angle);
    GSmodelSetMatrix(model, mtx);
    GSmodelCenterNull(model);
    part = GSmodelGetPart(model, fn_800EE0E8(model) - 1);
    if (part != NULL) {
        GSpartGetTransform(part, &xform, 0, 0);
        GSpartFree(part);
    }
    modelRemoveCenterNull(model);
    set__5GSvecFfff(&camPos, lbl_8047BC94,
                    zoom * (f32)sin(pitch) + (lbl_8047BC94 + xform.y), zoom);
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

extern f32 lbl_8047BD20;
extern f32 lbl_8047BD24;
extern f32 lbl_8047BD28;
extern f32 lbl_8047BD2C;
extern void* GSsplineCreate(s32 a, s32 b, s32 count);
extern void GSsplineAddControlVectorValue(void* spline, void* vec, f32 t);
extern void GSsplineFree(void* spline);

/* Draw one memo stat polyline through a spline fitted to its byte samples. */
#pragma peephole off
void fn_800473E0(PdaOrbitPoint* point)
{
    extern u8 lbl_80314E08[];
    extern void fn_800D5648(f32 width);
    extern void fn_800D5BA0(s32 index, u32 color);
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
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D7820(lbl_80314E08);
        fn_800D6A00(1);
        fn_800D5648(lbl_8047BCA8);
        fn_800D67BC(2);
        fn_800D61E4((s32)(lbl_8047BD24 + v0.x),
                    (s32)(lbl_8047BD2C * v0.y + lbl_8047BD28));
        fn_800D5BA0(0, (u8)(s32)v0.z | 0x361e0500);
        fn_800D61E4((s32)(lbl_8047BD24 + v1.x),
                    (s32)(lbl_8047BD2C * v1.y + lbl_8047BD28));
        fn_800D5BA0(0, (u8)(s32)v1.z | 0x361e0500);
        fn_800D6728();
        fn_800D5648(lbl_8047BCBC);
        off += 0xc;
    }
    GSsplineFree(spline);
}
#pragma peephole reset

extern u8 lbl_802E53F4[];
extern u8 lbl_802E5400[];
extern void GSmtxMakeYRotation(f32* mtx, f32 angle);
extern void fn_800E0370(f32* mtx, f32 angle);
extern void fn_800E0560(f32* mtx, void* vec);
extern void GSvecTransform(void* dst, f32* mtx, void* src);

/* Orbit camera for the PDA's rotatable model page. */
#pragma peephole off
u8 fn_800484A4(u8* work)
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
    f32 dist;

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
        model = fn_801DAC3C(*(void**)(work + 0x24));
        switch (fn_801DAC24(*(void**)(work + 0x24))) {
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
    persp0 = lbl_8047BD30;
    persp1 = (f32)*(s32*)(work + 0x2c) / (f32)*(s32*)(work + 0x30);
    if (bound.y >= bound.x) {
        extent = bound.y;
    } else {
        extent = bound.x;
    }
    if (lbl_8047BCBC == scale) {
        dist = extent * zoom / (f32)tan(lbl_8047BD34);
    } else {
        dist = zoom * (extent / spread) / (f32)tan(lbl_8047BD34);
    }
    set__5GSvecFfff(&camPos, lbl_8047BC94, lbl_8047BC94,
                    dist * *(f32*)((u8*)&lbl_803A6818 + 0x64));
    GSmtxMakeYRotation(mtx1, -*(f32*)((u8*)&lbl_803A6818 + 0x70));
    fn_800E0370(mtx1, -*(f32*)((u8*)&lbl_803A6818 + 0x6c));
    GSvecTransform(&camPos, mtx1, &camPos);
    GScameraSetPosition(*(void**)(work + 0x38), &camPos);
    GScameraSetPerspective(*(void**)(work + 0x38), persp0, persp1, persp2,
                           persp3);
    target.x = lbl_8047BC94;
    target.z = lbl_8047BC94;
    target.y = -(box[5] + box[8]) * lbl_8047BD18;
    fn_800E064C(mtx0);
    fn_800E0560(mtx0, &target);
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
#pragma peephole reset

/* Icon index of one of a memo entry's two types. */
static inline u16 pdaGetTypeIcon(u16 id, s32 slot)
{
    u16 type = pokemonGetStatus(
        0, (u16)pokemonGetStatus(pdaLoadPokemonId(id), 0, 0x6e, 0), 0x16, slot);

    if (type >= 9) {
        type = type - 1;
    }
    return type + 1;
}

/* Filter the memo index table down to the active type tabs. */
#pragma peephole off
void fn_8003DC54(void)
{
    u8* S = (u8*)&lbl_803A6818;
    u16 buf1[500];
    u16 buf0[500];
    u16 type0;
    u16 type1;
    u16 single;
    u16 i;
    u16 count;
    u16 a;
    u16 b;

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
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                pdaLoadPokemonId(id);
                a = pdaGetTypeIcon(id, 0);
                b = pdaGetTypeIcon(id, 1);
                if ((type0 == a && type1 == b) || (type0 == b && type1 == a)) {
                    buf1[count++] = id;
                }
            }
        }
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf1[i];
        }
        lbl_8047A4E8 = count;
    } else {
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                pdaLoadPokemonId(id);
                a = pdaGetTypeIcon(id, 0);
                b = pdaGetTypeIcon(id, 1);
                if (type0 == a || type0 == b) {
                    buf0[count++] = id;
                }
            }
        }
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
        h = fn_800E202C(p);
        if ((u16)h != 0) {
            fn_800E24B0();
            fn_800E209C(h);
        }
    }
}

static inline u16 pdaGetWeight(u16 id)
{
    u32 work = pdaLoadPokemonId(id);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetWeight(data);
        }
        return 0;
    }
    return 0;
}

static inline u16 pdaGetHeight(u16 id)
{
    u32 work = pdaLoadPokemonId(id);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetHeight(data);
        }
        return 0;
    }
    return 0;
}

/* Reorder the memo index table for the active sort tab. */
#pragma peephole off
void fn_8003E394(void)
{
    PdaSortPair* pairs;
    u16* ids;
    u16 i;
    u16 j;
    u16 count;

    switch (*(s8*)((u8*)&lbl_803A6818 + 0x15c)) {
    case 0:
        fn_8003F040();
        break;
    case 1:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                u16 key = pdaGetWeight(id);
                ids[count] = id;
                pairs[count].key = key;
                pairs[count].index = i;
                count++;
            }
        }
        fn_8003F2DC((u8*)pairs, count, 1);
        for (j = 0; j < count; j++) {
            lbl_8047A4E4[j] = ids[pairs[j].index];
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    case 2:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                u16 key = pdaGetWeight(id);
                ids[count] = id;
                pairs[count].key = key;
                pairs[count].index = i;
                count++;
            }
        }
        if (count != 0) {
            fn_8003F2DC((u8*)pairs, count, 0);
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
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                u16 key = pdaGetHeight(id);
                ids[count] = id;
                pairs[count].key = key;
                pairs[count].index = i;
                count++;
            }
        }
        if (count != 0) {
            fn_8003F2DC((u8*)pairs, count, 1);
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
        count = 0;
        for (i = 0; i < lbl_8047A4E8; i++) {
            u16 id = lbl_8047A4E4[i];
            if ((id & 0x8000) == 0) {
                u16 key = pdaGetHeight(id);
                ids[count] = id;
                pairs[count].key = key;
                pairs[count].index = i;
                count++;
            }
        }
        if (count != 0) {
            fn_8003F2DC((u8*)pairs, count, 0);
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

extern PdaOrbitPoint lbl_802E52C8[8];
extern f32 lbl_8047BCAC;
extern s32 fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

/* Anchor a PDA widget to its layout entry and hand the sprite to the drawer. */
static inline void pdaPlaceSprite(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    sprite->field_50 =
        (s16)((s32)*(f32*)((u8*)&lbl_803A6818 + 0x1dc) +
              *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2));
    spriteSetEnv(fn_800FE6D0(
        (s16)(*(s16*)((u8*)alphaSprite + 0x84) + sprite->field_50),
        (s16)(*(s16*)((u8*)alphaSprite + 0x86) + sprite->field_52)));
}

static inline s32 pdaTint(PdaSprite* alphaSprite, s32 tint)
{
    return alphaSprite->alphaByte | tint;
}

/* Per-widget layout and visibility for the PDA's summary page. */
#pragma peephole off
void fn_8003F464(PdaSprite* alphaSprite, PdaSprite* sprite)
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
        fn_800FE38C(
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
                    fn_800FB680(0, (s32)pos,
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
        fn_800FE35C();
        break;
    case 0xd79:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ed) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ed);
        break;
    case 0xd7a:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ec) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ec);
        break;
    case 0xd7b:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36eb) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36eb);
        break;
    case 0xd7c:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ea) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36ea);
        break;
    case 0xd7d:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36e9) >> 16)) / 2), -2,
                    (alphaSprite->alphaByte | -0x100LL), (void*)0x36e9);
        break;
    case 0xd74:
        fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL),
                    (void*)(*(u32**)(T + 0x258))[*(s8*)(S + 0x15c)]);
        break;
    case 0xd75:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((*(u32**)(T + 0x254))[*(s8*)(S + 0x15b)]));
        fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)0xe7);
        break;
    case 0xd76:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((*(u32**)(T + 0x254))[*(s8*)(S + 0x15a)]));
        fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)0xe7);
        break;
    case 0xd77:
        fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL),
                    (void*)(*(u32**)(T + 0x250))[*(s8*)(S + 0x159)]);
        break;
    case 0xd78:
        fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL),
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
            fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL), (void*)msg);
            break;
        case 1:
            if (*(s8*)(S + 0x15d) == 3) {
                fn_800FB680(0, 0, (alphaSprite->alphaByte | -0x100LL),
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

/* Whether the highlighted memo entry has actually been seen. */
static inline u8 pdaEntrySeen(void)
{
    return (lbl_8047A4E4[lbl_803A6818.currentIndex] & 0x8000) ? 0 : 1;
}

/* Species-name message for the highlighted entry, 0 when unavailable. */
static inline u32 pdaCurrentNameMsg(void)
{
    u32 work = pdaLoadPokemon(lbl_803A6818.currentIndex);

    if (work != 0) {
        return GSmsgGetGSchar((u32)pokemonDataBiosGetName(
            pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work))));
    }
    return 0;
}

/* Bios record for the highlighted entry, NULL when unavailable. */
static inline void* pdaCurrentData(void)
{
    return pokemonDataBiosGetPtr(
        pokemonBiosGetPokemonDataId(pdaLoadPokemon(lbl_803A6818.currentIndex)));
}

/* Height of the highlighted entry, 0 when unavailable. */
static inline u16 pdaCurrentHeight(void)
{
    u32 work = pdaLoadPokemon(lbl_803A6818.currentIndex);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetHeight(data);
        }
        return 0;
    }
    return 0;
}

/* Weight of the highlighted entry, 0 when unavailable. */
static inline u16 pdaCurrentWeight(void)
{
    u32 work = pdaLoadPokemon(lbl_803A6818.currentIndex);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetWeight(data);
        }
        return 0;
    }
    return 0;
}

/* Species panel: name, abilities, height, weight and type for the entry. */
#pragma peephole off
void fn_800411FC(PdaSprite* alphaSprite, PdaEvent* event)
{
    extern void fn_800FB8C8(s32 x, s32 y, s16 w, s16 h, void* color, s32 msg);
    extern void fn_800FBB34(s32 x, s32 y, s16 w, s16 h, void* color, s32 msg);
    PdaSprite* sprite = (PdaSprite*)event;
    u8* layout;
    u32 name;
    u16 tokusei0;
    u16 tokusei1;
    u16 height;
    u16 weight;
    u16 digit;
    s32 i;
    s32 digits;

    layout = lbl_802EF0A8;
    name = pdaCurrentNameMsg();
    if (name == 0) {
        name = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, name);
    fn_800FB680(0, 0, alphaSprite->alphaByte | -0x100LL, (void*)0xce);

    if (pdaEntrySeen() != 0) {
        tokusei0 = pokemonDataBiosGetTokuseiDataId(pdaCurrentData(), 0);
        msgctrlSetValue(0x37,
                        GSmsgGetGSchar((u32)pokemonTokuseiDataBiosGetName(
                            pokemonTokuseiDataBiosGetPtr(tokusei0))));
        fn_800FB680(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100LL, (void*)0xe7);
    } else {
        fn_800FB680(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100LL, (void*)0x3721);
    }

    if (pdaEntrySeen() != 0) {
        tokusei1 = pokemonDataBiosGetTokuseiDataId(pdaCurrentData(), 1);
        if (tokusei1 != 0 && tokusei1 != tokusei0) {
            msgctrlSetValue(0x37,
                            GSmsgGetGSchar((u32)pokemonTokuseiDataBiosGetName(
                                pokemonTokuseiDataBiosGetPtr(tokusei1))));
            fn_800FB680(*(s16*)(layout + 0x20b42) - sprite->field_50 + 4,
                        *(s16*)(layout + 0x20b44) - sprite->field_52,
                        alphaSprite->alphaByte | -0x100LL, (void*)0xe7);
        }
    } else {
        fn_800FB680(*(s16*)(layout + 0x5a5a) - sprite->field_50 + 4,
                    *(s16*)(layout + 0x5a5c) - sprite->field_52,
                    alphaSprite->alphaByte | -0x100LL, (void*)0x3721);
    }

    if (pdaEntrySeen() != 0) {
        height = pdaCurrentHeight();
        lbl_803A67E8[9] = 0;
        lbl_803A67E8[8] = 0x27;
        i = 7;
        lbl_803A67E8[7] = 0x27;
        digits = 3;
        for (;;) {
            digit = (u32)height % 10;
            height = (u32)height / 10;
            if (digits > 0) {
                digits--;
            } else if (digit == 0) {
                break;
            }
            lbl_803A67E8[--i] = digit + 0x30;
            if (digits == 1) {
                lbl_803A67E8[--i] = 0x27;
            }
        }
        msgctrlSetValue(0x37, (u32)&lbl_803A67E8[i]);
        fn_800FB8C8(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a76) - sprite->field_50),
                    *(s16*)(layout + 0x5a78) - sprite->field_52,
                    *(s16*)(layout + 0x5a7a),
                    *(s16*)(layout + 0x5a7c),
                    (void*)(alphaSprite->alphaByte | -0x100), 0xcf);
    } else {
        fn_800FBB34(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a76) - sprite->field_50),
                    *(s16*)(layout + 0x5a78) - sprite->field_52,
                    *(s16*)(layout + 0x5a7a),
                    *(s16*)(layout + 0x5a7c),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371b);
    }

    if (pdaEntrySeen() != 0) {
        weight = pdaCurrentWeight();
        msgctrlSetValue(0x34, weight / 10);
        msgctrlSetValue(0x35, weight - weight / 10 * 10);
        fn_800FBB34(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a92) - sprite->field_50),
                    *(s16*)(layout + 0x5a94) - sprite->field_52,
                    *(s16*)(layout + 0x5a96),
                    *(s16*)(layout + 0x5a98),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x189c);
    } else {
        fn_800FBB34(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a92) - sprite->field_50),
                    *(s16*)(layout + 0x5a94) - sprite->field_52,
                    *(s16*)(layout + 0x5a96),
                    *(s16*)(layout + 0x5a98),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371a);
    }

    if (pdaEntrySeen() != 0) {
        msgctrlSetValue(0x31, pokemonDataBiosGetTypeName(pdaCurrentData()));
        fn_800FBB34(lbl_8047A4F0 +
                        (*(s16*)(layout + 0x5a22) - sprite->field_50),
                    *(s16*)(layout + 0x5a24) - sprite->field_52,
                    *(s16*)(layout + 0x5a26),
                    *(s16*)(layout + 0x5a28),
                    (void*)(alphaSprite->alphaByte | -0x100), 0x371e);
    } else {
        msgctrlSetValue(0x31, 0x371c);
        fn_800FBB34(lbl_8047A4F0 +
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
static inline void pdaEase(f32* cur, f32 dst)
{
    f32 delta;
    f32 rate;
    f32 moved;
    f32 rest;
    s8 mode;

    if (*cur == dst) {
        return;
    }
    delta = dst - *cur;
    rate = lbl_8047BCC4 * *(f32*)((u8*)&lbl_803A6818 + 0x3c);
    if (pdaFabs(delta) >= lbl_8047BCD0) {
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
    *cur = *cur + moved;
    rest = dst - *cur;
    if (moved > lbl_8047BC94) {
    } else {
        moved = -moved;
    }
    if (rest > lbl_8047BC94) {
    } else {
        rest = -rest;
    }
    if (rest <= moved) {
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
#pragma peephole off
#pragma peephole on
s32 fn_80041E48(void* work, s32 mode)
{
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    u16 keys;
    u8* S = (u8*)&lbl_803A6818;
    f32 dt;
    f32 delta;
    f32 moved;
    f32 rest;

    (void)work;
    (void)mode;
    keys = *(u16*)(lbl_803A67FC + 6);
    dt = (f32)fn_800D3088() / (f32)fn_800D37CC();
    *(f32*)(S + 0x3c) = dt;
    *(f32*)(S + 0x40) = *(f32*)(S + 0x40) + dt;
    if (*(f32*)(S + 0x40) >= lbl_8047BCBC) {
        *(f32*)(S + 0x40) = lbl_8047BC94;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            *(f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            *(f32*)((u8*)&lbl_803A6818 + 0x38));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x1d0),
            *(f32*)((u8*)&lbl_803A6818 + 0x1d8));
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x4c),
            *(f32*)((u8*)&lbl_803A6818 + 0x50), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x54),
            *(f32*)((u8*)&lbl_803A6818 + 0x58), lbl_8047BCB8);
    pdaRamp((f32*)((u8*)&lbl_803A6818 + 0x5c),
            *(f32*)((u8*)&lbl_803A6818 + 0x60), lbl_8047BCC0);

    if (*(f32*)(S + 0x1dc) != *(f32*)(S + 0x1e0)) {
        delta = *(f32*)(S + 0x1e0) - *(f32*)(S + 0x1dc);
        moved = lbl_8047BCC4 * delta * *(f32*)(S + 0x3c);
        if (moved > lbl_8047BC98) {
            moved = lbl_8047BC98;
        }
        if (moved <= lbl_8047BCC8) {
            moved = lbl_8047BCC8;
        }
        *(f32*)(S + 0x1dc) = *(f32*)(S + 0x1dc) + moved;
        rest = *(f32*)(S + 0x1e0) - *(f32*)(S + 0x1dc);
        if (moved > lbl_8047BC94) {
        } else {
            moved = -moved;
        }
        if (pdaFabs(rest) <= moved) {
            *(f32*)(S + 0x1dc) = *(f32*)(S + 0x1e0);
        } else {
            if (rest > lbl_8047BC94) {
            } else {
                rest = -rest;
            }
            if (rest < lbl_8047BCBC) {
                *(f32*)(S + 0x1dc) = *(f32*)(S + 0x1e0);
            }
        }
    }

    if (*(s32*)(S + 0x154) == 0) {
        if (*(s32*)(S + 0x1e4) != 0) {
            return 0;
        }
        if (*(s8*)(S + 0x15d) == 2) {
            if ((keys & 0x4) != 0) {
                if (*(s8*)(S + 0x15e) != 0) {
                    fn_80166AB8(0x23, 0, 0);
                }
                *(s8*)(S + 0x15e) = 0;
            } else if ((keys & 0x8) != 0) {
                if (*(s8*)(S + 0x15e) == 0) {
                    fn_80166AB8(0x23, 0, 0);
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
            fn_80166AB8(0x23, 0, 0);
        } else if ((keys & 0x1) != 0) {
            if (*(s8*)(S + 0x15d) <= 0) {
                return 0;
            }
            *(s8*)(S + 0x15d) = *(s8*)(S + 0x15d) - 1;
            fn_80166AB8(0x23, 0, 0);
        }
        return 0;
    }

    if ((keys & 0x2) != 0) {
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x160) - 1) {
            *(s8*)(S + 0x15f) = *(s8*)(S + 0x15f) + 1;
            fn_80166AB8(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) >= *(s8*)(S + 0x1c9)) {
            *(f32*)(S + 0x1d8) = *(f32*)(S + 0x1d8) - lbl_8047BCAC;
            *(s8*)(S + 0x1c9) = *(s8*)(S + 0x1c9) + 1;
            *(s8*)(S + 0x1c8) = *(s8*)(S + 0x1c8) + 1;
        }
    } else if ((keys & 0x1) != 0) {
        if (*(s8*)(S + 0x15f) > 0) {
            *(s8*)(S + 0x15f) = *(s8*)(S + 0x15f) - 1;
            fn_80166AB8(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x1c8)) {
            *(f32*)(S + 0x1d8) = *(f32*)(S + 0x1d8) + lbl_8047BCAC;
            *(s8*)(S + 0x1c8) = *(s8*)(S + 0x1c8) - 1;
            *(s8*)(S + 0x1c9) = *(s8*)(S + 0x1c9) - 1;
        }
    }
    return 0;
}
#pragma peephole off
#pragma peephole reset

extern f32 lbl_8047BCE0;
extern f32 lbl_8047BCE4;
extern f32 lbl_8047BCE8;
extern f32 lbl_8047BCEC;
extern f32 lbl_8047BCF0;
extern s8 fn_800F7994(s32 chan, s32 index);
extern s8 fn_800F7920(s32 chan, s32 index);
extern void GSvecAdd(void* dst, void* a, void* b);
extern u8 fn_8010A210(void* work, u32 pokemon);
extern void menuModelFree(void* work);
extern void fn_80109C88(void* work, u32 pokemon);
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
        model = fn_801DAC3C(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        break;
    case 1:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 4;
        *(f32*)(S + 0x60) = lbl_8047BC94;
        model = fn_801DAC3C(*(void**)(S + 0xa0));
        if (model != NULL) {
            GSmodelDestroyLinkedParticles(model);
        }
        *(f32*)(S + 0x64) = lbl_8047BCA8;
        break;
    case 2:
        *(s32*)(S + 0x1c) = 0xd;
        *(s32*)(S + 0x24) = 5;
        *(f32*)(S + 0x60) = lbl_8047BC94;
        model = fn_801DAC3C(*(void**)(S + 0xa0));
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
s32 fn_80042658(void* work, s32 mode)
{
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
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
    dt = (f32)fn_800D3088() / (f32)fn_800D37CC();
    *(f32*)((u8*)&lbl_803A6818 + 0x3c) = dt;
    *(f32*)((u8*)&lbl_803A6818 + 0x40) = *(f32*)((u8*)&lbl_803A6818 + 0x40) + dt;
    if (*(f32*)((u8*)&lbl_803A6818 + 0x40) >= lbl_8047BCBC) {
        *(f32*)((u8*)&lbl_803A6818 + 0x40) = lbl_8047BC94;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            *(f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            *(f32*)((u8*)&lbl_803A6818 + 0x38));
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
        ((u8*)&lbl_803A6818)[0x214] = fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
        fn_80043728((u32)work, mode, keys);
        break;
    case 4:
        ((u8*)&lbl_803A6818)[0x214] = fn_800484A4((u8*)&lbl_803A6818 + 0x7c);
        spin.x = lbl_8047BC94;
        spin.y = lbl_8047BC94;
        spin.z = lbl_8047BC94;
        model = fn_801DAC3C(*(void**)((u8*)&lbl_803A6818 + 0x7c + 0x24));
        if (model == NULL) {
            break;
        }
        GSmodelGetBound(model);
        rot = (f32*)((u8*)&lbl_803A6818 + 0x6c);
        stickY = fn_800F7994(1, 1);
        stickX = fn_800F7920(1, 1);
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
        if ((fn_800F7BC4(1) & 0x40) != 0) {
            if (*(f32*)((u8*)&lbl_803A6818 + 0x64) <= lbl_8047BCEC) {
                *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) + *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            }
        } else if ((fn_800F7BC4(1) & 0x20) != 0) {
            if (*(f32*)((u8*)&lbl_803A6818 + 0x64) > lbl_8047BCBC) {
                *(f32*)((u8*)&lbl_803A6818 + 0x64) = *(f32*)((u8*)&lbl_803A6818 + 0x64) - *(f32*)((u8*)&lbl_803A6818 + 0x3c);
            }
        }
        break;
    case 5:
        ((u8*)&lbl_803A6818)[0x214] = fn_800478B4((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
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
                    fn_80166AB8(0x23, 0, 0);
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
                    fn_80166AB8(0x23, 0, 0);
                    pdaPopLevel((u8*)&lbl_803A6818);
                }
            }
            *(f32*)((u8*)&lbl_803A6818 + 0x30) =
                *(f32*)((u8*)&lbl_803A6818 + 0x38);
        }
    }

    if (prevIndex != lbl_803A6818.currentIndex && lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (fn_8010A210((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            fn_80109C88((u8*)&lbl_803A6818 + 0x7c, pokemon);
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
                fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = fn_800478B4(
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
extern void fn_801CB954(void* h, s32 b);
extern void fn_801CB9D8(void* h);
extern void* fn_801CBA0C(u32 id);
extern void* fn_80113F48(void);
extern void* GSresGetResource(u32 group, u32 id);
extern void GSmodelSetVisibility(void* model, s32 vis);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GScameraGetPosition(void* cam, void* out);
extern void GScameraGetRotation(void* cam, void* out);
extern void cameraPlayAnime(u32 group, u32 id, s32 c, s32 d);
extern void menuModelInit(void* work, s32 w, s32 h);
extern void* peopleInfoBiosGetPtr(u32 id);
extern void fn_8018F4C8(void* info, s32 index, s32* motion, s32* out);
extern void menuModelSetMotion(void* work, s32 motion);
extern void fn_8010A010(void* work, u32 id);

/* Build the PDA scene: camera rig, both model slots and the memo index. */
#pragma peephole off
void fn_80048918(void)
{
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
    fn_801CB954(NULL, 0);
    res = GSresGetResource((u32)fn_80113F48(), 0);
    if (res != NULL) {
        GSmodelSetVisibility(res, 0);
    }
    fn_801CB9D8(NULL);
    *(f32*)((u8*)&lbl_803A6818 + 0x6c) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x70) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x74) = lbl_8047BC94;
    *(f32*)((u8*)&lbl_803A6818 + 0x64) = lbl_8047BCF0;
    anim = fn_801CBA0C(0xd171000);
    *(void**)((u8*)&lbl_803A6818 + 0x10c) = anim;
    res = GSresGetResource((u32)fn_80113F48(), (u32)anim);
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
    fn_8003D4C8();
    if (lbl_804788C0 != 0) {
        pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);
        if (fn_8010A210((u8*)&lbl_803A6818 + 0x7c, pokemon) == 0) {
            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
            fn_80109C88((u8*)&lbl_803A6818 + 0x7c, pokemon);
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
            *((u8*)&lbl_803A6818 + 0x214) = fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
            break;
        case 5:
            *((u8*)&lbl_803A6818 + 0x214) = fn_800478B4((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
            *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
            break;
        default:
            break;
        }
    }
    fn_8010A010((u8*)&lbl_803A6818 + 0xc4, 0xf70400);
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
        *((u8*)&lbl_803A6818 + 0x214) = fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
        break;
    case 5:
        *((u8*)&lbl_803A6818 + 0x214) = fn_800478B4((u8*)&lbl_803A6818 + 0x7c, (u8*)&lbl_803A6818 + 0xc4);
        *((u8*)&lbl_803A6818 + 0x214) = fn_8003D1FC_setup((u8*)&lbl_803A6818 + 0xc4);
        break;
    default:
        break;
    }
    fn_8018F4C8(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &out8);
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
void fn_80040308(void* menu)
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
        fn_80166AB8(0x24, 0, 0);
        page = *(s8*)(L + 9);
        if (page == 4) {
            switch (*(s32*)(L + 0x90)) {
            case 0:
                if (fn_8003D4C8() != 0) {
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
                        if (fn_8010A210((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
                            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
                            fn_80109C88((u8*)&lbl_803A6818 + 0x7c, pokemon);
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
                                fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
                            break;
                        case 5:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_800478B4((u8*)&lbl_803A6818 + 0x7c,
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
        fn_80166AB8(0x25, 0, 0);
        switch (*(s32*)L) {
        case 0:
            if (lbl_8047BCCC == *(f32*)(L + 0x88)) {
                if (*(s32*)(L + 0x90) != 1) {
                    fn_8003D818();
                    total = lbl_8047A4E8;
                    pdaResetCarousel(total);
                    if (lbl_804788C0 != 0) {
                        {
                        u32 pokemon =
                            pdaLoadPokemon(lbl_803A6818.currentIndex);
                        if (fn_8010A210((u8*)&lbl_803A6818 + 0x7c, pokemon) ==
                            0) {
                            menuModelFree((u8*)&lbl_803A6818 + 0x7c);
                            fn_80109C88((u8*)&lbl_803A6818 + 0x7c, pokemon);
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
                                fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
                            break;
                        case 5:
                            *((u8*)&lbl_803A6818 + 0x214) =
                                fn_800478B4((u8*)&lbl_803A6818 + 0x7c,
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
                        fn_80047CC0((u8*)&lbl_803A6818 + 0x7c);
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
extern s32 fn_801666BC(u32 voice);
extern u32 fn_800F7AF0(s32 chan);
extern s32 menuOpen(s32 id, s32 flag);

/* Cry voice of the highlighted entry, 0xffff when it cannot be resolved. */
static inline u16 pdaCurrentVoice(void)
{
    u32 work = pdaLoadPokemon(lbl_803A6818.currentIndex);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonDataBiosGetVoice(data);
        }
        return 0xffff;
    }
    return 0xffff;
}

/* Cry sample of the highlighted entry, NULL when it cannot be resolved. */
static inline void* pdaCurrentCry(void)
{
    u32 work = pdaLoadPokemon(lbl_803A6818.currentIndex);
    void* data;

    if (work != 0) {
        data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
        if (data != NULL) {
            return pokemonNakigoeDataBiosGetDataAddress(data);
        }
        return NULL;
    }
    return NULL;
}

/* Push a cry onto the ring of trails and age every older entry. */
static inline void pdaPushCry(void* data, u32 voice)
{
    PdaCryTrail* t = (PdaCryTrail*)lbl_802E52C8;
    s32 i;
    s32 j;

    lbl_8047A4EC = lbl_8047A4EC + 1;
    if (lbl_8047A4EC >= 8) {
        lbl_8047A4EC = 0;
    }
    i = lbl_8047A4EC;
    t[i].data = data;
    t[i].voice = voice;
    t[i].field_0c = lbl_8047BC94;
    t[i].field_14 = lbl_8047BC94;
    t[i].offset = 0;
    t[i].active = 1;
    t[i].alpha = lbl_8047BCA0;
    t[i].field_20 = lbl_8047BD18;
    for (j = i - 1; j >= 0; j--) {
        t[j].offset = (s32)((f32)t[j].offset + t[j].speed);
        t[j].field_20 = t[j].field_20 + lbl_8047BD1C;
    }
    for (j = 7; j > (s32)lbl_8047A4EC; j--) {
        t[j].offset = (s32)((f32)t[j].offset + t[j].speed);
        t[j].field_20 = t[j].field_20 + lbl_8047BD1C;
    }
}

/* Cry-playback thread for the PDA's Pokemon entry. */
#pragma peephole off
void fn_80046168(void)
{
    extern void _threadSwitch(void);
    extern f64 atan2(f64 y, f64 x);
    u8* S;
    PdaCryTrail* t;
    s32 running;
    s32 k;
    s32 i;
    s32 playing;
    u8 cancel;
    u16 voice;
    s8 x;
    s8 y;
    f32 ang;

    running = 1;
    for (k = 0; k < 8; k++) {
        pdaPushCry(NULL, 0);
    }
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
                    voice = pdaCurrentVoice();
                    fn_80166AB8(voice, 0, 0);
                    pdaPushCry(pdaCurrentCry(), voice);
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
            t = (PdaCryTrail*)lbl_802E52C8;
            playing = 0;
            for (i = 0; i < 8; i++) {
                if (t[i].data != NULL) {
                    switch (fn_801666BC(t[i].voice)) {
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
            if ((u8)playing != 0) {
                if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x8) != 0) {
                    cancel = 1;
                } else if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x4) != 0) {
                    cancel = 1;
                } else {
                    x = fn_800F7A08(1, 0);
                    y = fn_800F7A7C(1, 0);
                    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
                        ang = atan2((f64)y, (f64)x);
                        if (pdaFabs(ang) < lbl_8047BCFC) {
                            cancel = 1;
                        } else if (pdaFabs(ang) > lbl_8047BD00) {
                            cancel = 1;
                        } else {
                            cancel = 0;
                        }
                    } else {
                        cancel = 0;
                    }
                }
            } else {
                cancel = 1;
            }
            if (cancel == 1) {
                menuCloseCustom(0x3e, 0, 1);
                *(s32*)(S + 0x20) = 0;
            } else {
                if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x100) != 0) {
                    voice = pdaCurrentVoice();
                    fn_80166AB8(voice, 0, 0);
                    pdaPushCry(pdaCurrentCry(), voice);
                }
                if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x200) != 0) {
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

/* Mail-list backdrop: fade panel, scanlines, and the sheared page edge. */
#pragma peephole off
s32 fn_800495C8(void* work, PdaSprite* sprite)
{
    extern u8 lbl_80314E08[];
    extern void fn_800D5648(f32 width);
    extern void fn_800D5BA0(s32 index, u32 color);
    u8* G = lbl_803A6A60;
    u8* data;
    f32 kOne;
    f32 kAlpha;
    f32 top;
    f32 kMin;
    f32 kScale;
    f32 kMul;
    f32 kDiv;
    f32 y;
    s32 i;
    s32 j;
    s32 alpha;
    s32 row;
    u32 color;
    f32 step;

    data = *(u8**)((u8*)work + 0x60);
    if (*(f32*)(G + 0x2c) != *(f32*)(G + 0x28)) {
        step = *(f32*)(G + 8) / lbl_8047BDA8;
        if (*(f32*)(G + 0x2c) > *(f32*)(G + 0x28)) {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) + step;
            if (*(f32*)(G + 0x28) > lbl_8047BDA0) {
                *(f32*)(G + 0x28) = lbl_8047BDA0;
            }
        } else {
            *(f32*)(G + 0x28) = *(f32*)(G + 0x28) - step;
            if (*(f32*)(G + 0x28) < lbl_8047BDAC) {
                *(f32*)(G + 0x28) = lbl_8047BDAC;
            }
        }
    }
    fn_800411EC();
    if (*(s8*)(lbl_803A6A60 + 0) == 1) {
        if (lbl_803A6A60[0x49] != 0 && lbl_803A6A60[0x4a] == 0) {
            menuOffScreenSetDisp(0);
            lbl_803A6A60[0x49] = 0;
        }
        if (*(s8*)(lbl_803A6A60 + 1) >= 1) {
            *(f32*)(G + 0x2c) = lbl_8047BDAC;
            return 0;
        }
        *(f32*)(G + 0x2c) = lbl_8047BDA0;
    } else {
        if (lbl_803A6A60[0x49] != 1) {
            menuOffScreenSetDisp(1);
            lbl_803A6A60[0x49] = 1;
        }
        *(f32*)(G + 0x2c) = lbl_8047BDA0;
    }

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D7820(lbl_80314E08);
    fn_800D6A00(4);
    fn_800D67BC(4);
    alpha = (s32)(lbl_8047BDB0 * *(f32*)(G + 0x28));
    fn_800D61E4(0, 0);
    color = (u8)alpha | 0x03140e00;
    fn_800D5BA0(0, color);
    fn_800D61E4(sprite->x, 0);
    fn_800D5BA0(0, color);
    fn_800D61E4(0, sprite->y);
    fn_800D5BA0(0, color);
    fn_800D61E4(sprite->x, sprite->y);
    fn_800D5BA0(0, color);
    fn_800D6728();

    for (i = 0; i < 0x1e0; i += 4) {
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D7820(lbl_80314E08);
        fn_800D6A00(1);
        fn_800D5648(lbl_8047BDA0);
        fn_800D67BC(2);
        fn_800D61E4(0, (s16)i);
        fn_800D5BA0(0, 0x00800080);
        fn_800D61E4(sprite->x, (s16)i);
        fn_800D5BA0(0, 0x00800080);
        fn_800D6728();
    }

    top = **(f32**)(data + 8);
    kAlpha = lbl_8047BDB0;
    kOne = lbl_8047BDA0;
    kDiv = lbl_8047BDB4;
    kMul = lbl_8047BDB8;
    kScale = lbl_8047BDA4;
    kMin = lbl_8047BDAC;
    for (j = 0; j < 12; j++) {
        alpha = (s32)(kAlpha * (kOne - (f32)j / kDiv));
        y = top - kMul * (f32)j / kScale;
        if (y < kMin) {
            y = y + kOne;
        }
        row = (s32)(kScale * y);
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D7820(lbl_80314E08);
        fn_800D6A00(1);
        fn_800D5648(lbl_8047BDA0);
        fn_800D67BC(2);
        fn_800D61E4(0, row);
        color = (u8)alpha | 0x00800000;
        fn_800D5BA0(0, color);
        fn_800D61E4(sprite->x, row);
        fn_800D5BA0(0, color);
        fn_800D6728();
    }
    return 0;
}
#pragma peephole reset

extern f32 lbl_8047BDD8;
extern f32 lbl_8047BDDC;
extern f32 lbl_8047BDE0;
extern f32 lbl_8047BDE4;
extern u8 fn_801902E0(s32 id);
extern void menuItemBiosSetSelectFlag(s32 id, s32 flag);

/* Per-frame timing, scroll easing and page-flip for the mail list. */
#pragma peephole off
s32 fn_8004B278(u8* work)
{
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    u8* data;
    f32 delta;
    f32 moved;
    f32 rest;
    f32* p;

    data = *(u8**)(work + 0x60);
    *(f32*)((u8*)&lbl_803A6A60 + 8) = (f32)fn_800D3088() / (f32)fn_800D37CC();
    if (fn_801902E0(0x3f0) != 0) {
        menuItemBiosSetSelectFlag(0x438, 1);
    } else {
        menuItemBiosSetSelectFlag(0x438, 0);
    }
    *(f32*)(lbl_803A6A60 + 0x40) =
        lbl_8047BDA8 * *(f32*)((u8*)&lbl_803A6A60 + 8) + *(f32*)(lbl_803A6A60 + 0x40);
    if (*(f32*)(lbl_803A6A60 + 0x40) > lbl_8047BDA0) {
        *(f32*)(lbl_803A6A60 + 0x40) = lbl_8047BDAC;
    }

    if (*(f32*)(lbl_803A6A60 + 0x30) != *(f32*)(lbl_803A6A60 + 0x38)) {
        delta = *(f32*)(lbl_803A6A60 + 0x38) - *(f32*)(lbl_803A6A60 + 0x30);
        moved = lbl_8047BDD8 * delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > lbl_8047BDDC) {
            moved = lbl_8047BDDC;
        }
        if (moved <= lbl_8047BDE0) {
            moved = lbl_8047BDE0;
        }
        *(f32*)(lbl_803A6A60 + 0x30) = *(f32*)(lbl_803A6A60 + 0x30) + moved;
        rest = *(f32*)(lbl_803A6A60 + 0x38) - *(f32*)(lbl_803A6A60 + 0x30);
        if (moved > lbl_8047BDAC) {
        } else {
            moved = -moved;
        }
        if ((rest > lbl_8047BDAC ? rest : -rest) <= moved) {
            *(f32*)(lbl_803A6A60 + 0x30) = *(f32*)(lbl_803A6A60 + 0x38);
        } else {
            if (rest > lbl_8047BDAC) {
            } else {
                rest = -rest;
            }
            if (rest < lbl_8047BDA0) {
                *(f32*)(lbl_803A6A60 + 0x30) = *(f32*)(lbl_803A6A60 + 0x38);
            }
        }
    }

    if (*(f32*)(lbl_803A6A60 + 0x34) != *(f32*)(lbl_803A6A60 + 0x3c)) {
        delta = *(f32*)(lbl_803A6A60 + 0x3c) - *(f32*)(lbl_803A6A60 + 0x34);
        moved = lbl_8047BDD8 * delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > lbl_8047BDDC) {
            moved = lbl_8047BDDC;
        }
        if (moved <= lbl_8047BDE0) {
            moved = lbl_8047BDE0;
        }
        *(f32*)(lbl_803A6A60 + 0x34) = *(f32*)(lbl_803A6A60 + 0x34) + moved;
        rest = *(f32*)(lbl_803A6A60 + 0x3c) - *(f32*)(lbl_803A6A60 + 0x34);
        if (moved > lbl_8047BDAC) {
        } else {
            moved = -moved;
        }
        if ((rest > lbl_8047BDAC ? rest : -rest) <= moved) {
            *(f32*)(lbl_803A6A60 + 0x34) = *(f32*)(lbl_803A6A60 + 0x3c);
        } else {
            if (rest > lbl_8047BDAC) {
            } else {
                rest = -rest;
            }
            if (rest < lbl_8047BDA0) {
                *(f32*)(lbl_803A6A60 + 0x34) = *(f32*)(lbl_803A6A60 + 0x3c);
            }
        }
    }

    switch (*(s8*)(work + 1)) {
    case 0:
        if (*(s8*)(work + 2) == 0) {
            **(f32**)(data + 8) = lbl_8047BDAC;
            work[2] = 1;
        }
        break;
    case 2:
        p = *(f32**)(data + 8);
        *p = *p + lbl_8047BDE4;
        if (*p >= lbl_8047BDA0) {
            p = *(f32**)(data + 8);
            *p = *p - lbl_8047BDA0;
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
void fn_8004A47C(void)
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
    if (fn_801902E0(0x3f0) != 0) {
        *(s16*)(lbl_802EF0A8 + 0x7608) = topWide[0].x - 6;
        *(s16*)(lbl_802EF0A8 + 0x7624) = topWide[0].y - 6;
        *(s16*)(lbl_802EF0A8 + 0x7640) = topWide[1].x - 6;
        *(s16*)(lbl_802EF0A8 + 0x765c) = topWide[1].y - 6;
    } else {
        *(s16*)(lbl_802EF0A8 + 0x7608) = topNarrow[0].x - 6;
        *(s16*)(lbl_802EF0A8 + 0x7624) = topNarrow[0].y - 6;
        *(s16*)(lbl_802EF0A8 + 0x7640) = topNarrow[1].x - 6;
        *(s16*)(lbl_802EF0A8 + 0x765c) = topNarrow[1].y - 6;
    }

    wide = lbl_8026719C;
    narrow = lbl_802671B4;
    if (fn_801902E0(0x3f0) != 0) {
        *(s16*)(lbl_802EF0A8 + 0xc148) = wide.v[0];
        *(s16*)(lbl_802EF0A8 + 0xc164) = wide.v[1];
        *(s16*)(lbl_802EF0A8 + 0xc180) = wide.v[2];
        *(s16*)(lbl_802EF0A8 + 0xce30) = wide.v[3];
        *(s16*)(lbl_802EF0A8 + 0xce4c) = wide.v[4];
        *(s16*)(lbl_802EF0A8 + 0xce68) = wide.v[5];
        *(s16*)(lbl_802EF0A8 + 0xce84) = wide.v[6];
        *(s16*)(lbl_802EF0A8 + 0xcea0) = wide.v[7];
        *(s16*)(lbl_802EF0A8 + 0xcebc) = wide.v[8];
        *(s16*)(lbl_802EF0A8 + 0xced8) = wide.v[9];
        *(s16*)(lbl_802EF0A8 + 0xcef4) = wide.v[10];
        *(s16*)(lbl_802EF0A8 + 0xcf10) = wide.v[11];
    } else {
        *(s16*)(lbl_802EF0A8 + 0xc148) = narrow.v[0];
        *(s16*)(lbl_802EF0A8 + 0xc164) = narrow.v[1];
        *(s16*)(lbl_802EF0A8 + 0xc180) = narrow.v[2];
        *(s16*)(lbl_802EF0A8 + 0xce30) = narrow.v[3];
        *(s16*)(lbl_802EF0A8 + 0xce4c) = narrow.v[4];
        *(s16*)(lbl_802EF0A8 + 0xce68) = narrow.v[5];
        *(s16*)(lbl_802EF0A8 + 0xce84) = narrow.v[6];
        *(s16*)(lbl_802EF0A8 + 0xcea0) = narrow.v[7];
        *(s16*)(lbl_802EF0A8 + 0xcebc) = narrow.v[8];
        *(s16*)(lbl_802EF0A8 + 0xced8) = narrow.v[9];
        *(s16*)(lbl_802EF0A8 + 0xcef4) = narrow.v[10];
        *(s16*)(lbl_802EF0A8 + 0xcf10) = narrow.v[11];
    }

    botWide[0] = lbl_8047BD88;
    botWide[1] = lbl_8047BD8C;
    botNarrow[0] = lbl_8047BD90;
    botNarrow[1] = lbl_8047BD94;
    if (fn_801902E0(0x3f0) != 0) {
        *(s16*)(lbl_802EF0A8 + 0xc0d8) = botWide[0].x - 6;
        *(s16*)(lbl_802EF0A8 + 0xc0f4) = botWide[0].y - 6;
        *(s16*)(lbl_802EF0A8 + 0xc110) = botWide[1].x - 6;
        *(s16*)(lbl_802EF0A8 + 0xc12c) = botWide[1].y - 6;
    } else {
        *(s16*)(lbl_802EF0A8 + 0xc0d8) = botNarrow[0].x - 6;
        *(s16*)(lbl_802EF0A8 + 0xc0f4) = botNarrow[0].y - 6;
        *(s16*)(lbl_802EF0A8 + 0xc110) = botNarrow[1].x - 6;
        *(s16*)(lbl_802EF0A8 + 0xc12c) = botNarrow[1].y - 6;
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
extern void fn_8004A7A8(void* work, PdaSprite* sprite);

/* Per-widget update for the mail screen's sprites and captions. */
#pragma peephole off
s32 fn_800499BC(void* work, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    extern void windowDrawSprite2(s16 x, s16 y, s16 w, s16 h, u32 color,
                                  void* work, s32 id, s32 flag);
    PdaPairS16 wave[2];
    PdaPairS16 icon[2];
    s32 idx;
    s32 i;
    s32 id;
    s32 cursor;
    s32 msg;
    u8* row;
    u32 value;
    u32 status;

    fn_8004A7A8(work, sprite);
    switch (sprite->eventId) {
    case 0x6e1:
        icon[0] = lbl_8047BD70;
        icon[1] = lbl_8047BD74;
        status = heroGetStatus(0, 0xe, 0);
        for (i = lbl_804788F0 - 1; i >= 0; i--) {
            if (lbl_802E61D8[i] <= status) {
                break;
            }
        }
        if (i < 0) {
            i = 0;
        }
        idx = i;
        if (i > 4) {
            idx = 0;
        }
        id = ((u16*)icon)[idx];
        menuSpriteBiosGetPtr(id);
        windowDrawSprite2(
            (s16)((s32)*(f32*)(lbl_803A6A60 + 0x30) +
                  (*(s16*)(lbl_802EF0A8 + 0xc09e) - sprite->field_50)),
            (s16)(*(s16*)(lbl_802EF0A8 + 0xc0a0) - sprite->field_52),
            sprite->x, sprite->y, sprite->alpha | -0x100, work, id, 0);
        break;
    case 0x435:
        row = *(u8**)((u8*)work + 0x60);
        sprite->messageId = **(s32**)(row + 4);
        sprite->colorR = row[0];
        sprite->colorG = row[1];
        sprite->colorB = row[2];
        if (*(s8*)(lbl_803A6A60 + 0) == 1) {
            if (*(s8*)(lbl_803A6A60 + 1) == 2) {
                sprite->flags &= ~2;
            } else {
                sprite->flags |= 2;
            }
        }
        break;
    case 0x433:
    case 0x436:
        if (*(s8*)(lbl_803A6A60 + 0) == 1) {
            if (*(s8*)(lbl_803A6A60 + 1) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x438:
        if (fn_801902E0(0x3f0) != 0) {
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
        if (fn_801902E0(0x3f0) != 0) {
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
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x75d:
    case 0x75e:
    case 0x75f:
    case 0x439:
        *(s32*)(lbl_803A6A60 + 0x20) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x760:
    case 0x761:
    case 0x762:
    case 0x763:
    case 0x764:
    case 0x765:
        fn_8004B598((s32)work, sprite, sprite->eventId);
        break;
    case 0x754:
        msgctrlSetValue(0x4c, fn_80005748());
        break;
    case 0x755:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xd, 0));
        break;
    case 0x756:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xc, 0));
        break;
    case 0x757:
        sprite->field_50 =
            (s16)(*(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2) +
                  (s32)*(f32*)(lbl_803A6A60 + 0x30));
        spriteSetEnv(fn_800FE6D0(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        sprite->field_50 =
            (s16)(*(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2) +
                  (s32)*(f32*)(lbl_803A6A60 + 0x30));
        value = heroGetStatus(0, 2, 0);
        idx = sprite->x - 0x20;
        for (i = 0; i < 5; i++) {
            idx -= 0xd;
            msgctrlSetValue(0x34, value % 10);
            value = value / 10;
            fn_800FB680(idx + 0x24, 0, sprite->alpha | -0x100LL, (void*)0xca);
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
        fn_800FB680(-0x50, -4, sprite->alpha | -0x100, (void*)msg);
        break;
    case 0x6e3:
    case 0x6e4:
    case 0x6e5:
    case 0x6e6:
        wave[0] = lbl_8047BD98;
        wave[1] = lbl_8047BD9C;
        row = lbl_802EF0A8 + sprite->eventId * 0x1c;
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 0:
            if (sprite->eventId == 0x6e3) {
                sprite->field_50 = (s16)(s32)(
                    lbl_8047BDB8 *
                        (f32)sin(lbl_8047BDC8 *
                                 (lbl_8047BDCC *
                                  (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(row + 2));
                wave[0].x = (s16)(s32)(
                    lbl_8047BDB8 *
                    (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                wave[0].x = 0;
                sprite->field_50 = *(s16*)(row + 2);
            }
            wave[0].x = wave[0].x - 0x1e;
            break;
        case 1:
            if (sprite->eventId == 0x6e4) {
                sprite->field_50 = (s16)(s32)(
                    lbl_8047BDB8 *
                        (f32)sin(lbl_8047BDC8 *
                                 (lbl_8047BDCC *
                                  (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(row + 2));
                wave[0].y = (s16)(s32)(
                    lbl_8047BDB8 *
                    (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                wave[0].y = 0;
                sprite->field_50 = *(s16*)(row + 2);
            }
            wave[0].y = wave[0].y - 0x1e;
            break;
        case 2:
            if (sprite->eventId == 0x6e5) {
                wave[1].x = (s16)(s32)(
                    lbl_8047BDB8 *
                    (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                wave[1].x = 0;
                sprite->field_50 = *(s16*)(row + 2);
            }
            wave[1].x = wave[1].x - 0x1e;
            break;
        case 3:
            if (sprite->eventId == 0x6e6) {
                sprite->field_50 = (s16)(s32)(
                    lbl_8047BDB8 *
                        (f32)sin(lbl_8047BDC8 *
                                 (lbl_8047BDCC *
                                  (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(row + 2));
                wave[1].y = (s16)(s32)(
                    lbl_8047BDB8 *
                    (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                wave[1].y = 0;
                sprite->field_50 = *(s16*)(row + 2);
            }
            wave[1].y = wave[1].y - 0x1e;
            break;
        default:
            break;
        }
        if (sprite->eventId == 0x6e5) {
            sprite->flags |= 2;
            sprite->field_50 =
                (s16)((s16)(*(s32*)(lbl_803A6A60 + 0x20) - 0x36) + wave[1].x +
                      0xa8);
        }
        if (sprite->eventId == 0x6e3) {
            sprite->flags |= 2;
            sprite->field_50 =
                (s16)((s16)(*(s32*)(lbl_803A6A60 + 0x18) - 0x36) + wave[0].x +
                      0xa8);
        }
        if (sprite->eventId == 0x6e4) {
            sprite->flags |= 2;
            sprite->field_50 =
                (s16)((s16)(*(s32*)(lbl_803A6A60 + 0x1c) - 0x36) + wave[0].y +
                      0xa8);
        }
        if (sprite->eventId == 0x6e6) {
            sprite->flags |= 2;
            sprite->field_50 =
                (s16)((s16)(*(s32*)(lbl_803A6A60 + 0x24) - 0x36) + wave[1].y +
                      0xa8);
        }
        if (sprite->eventId == 0x6e4) {
            if (fn_801902E0(0x3f0) != 0) {
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

/* Backdrop alpha ramp and the staggered caption pulses on the mail screen. */
#pragma peephole off
void fn_8004A7A8(void* work, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    s32 fade;
    f32 pulse;

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
        sprite->field_50 =
            (s16)(*(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2) +
                  (s32)*(f32*)(lbl_803A6A60 + 0x30));
        spriteSetEnv(fn_800FE6D0(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        break;
    case 0x432:
    case 0x433:
    case 0x434:
    case 0x435:
    case 0x436:
    case 0x6dd:
        sprite->field_52 =
            (s16)(*(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 4) +
                  (s32)*(f32*)(lbl_803A6A60 + 0x34));
        break;
    case 0x437:
    case 0x438:
    case 0x439:
    case 0x43a:
    case 0x777:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6ea:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                         lbl_804788D8 * lbl_8047BDAC);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6eb:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDA0);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6ec:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDCC);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6ed:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDD4);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6ee:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDB8);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6ef:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                             (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                         lbl_804788D8 * lbl_8047BDAC);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6f0:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDA0);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6f1:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDCC);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6f2:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDD4);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6f3:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        pulse = (f32)sin(lbl_8047BDC8 *
                              (lbl_8047BDCC * *(f32*)(lbl_803A6A60 + 0x40)) +
                          lbl_804788D8 * lbl_8047BDB8);
        pulse = pulse + lbl_8047BDA0;
        pulse = pulse * lbl_8047BDA8;
        pulse = pulse * lbl_8047BDD0;
        if (pulse > lbl_8047BDB0) {
            pulse = lbl_8047BDB0;
        }
        if (pulse < lbl_8047BDAC) {
            pulse = lbl_8047BDAC;
        }
        sprite->alpha = (u8)(s32)pulse;
        sprite->colorR = (u8)(s32)pulse;
        sprite->colorG = (u8)(s32)pulse;
        sprite->colorB = (u8)(s32)pulse;
        break;
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x6f4:
    case 0x6f5:
    case 0x6f6:
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
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
        if (*(f32*)(lbl_803A6A60 + 0x30) > lbl_8047BDB0) {
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
extern u32 lbl_8047A4DC;
extern void fn_801EEDEC(u16 id, s32 flag);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);

/* Party-list scroll, cursor movement and the selected Pokemon preview. */
#pragma peephole off
s32 fn_8003B85C(void* window, s32 enabled)
{
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    u16* keys;
    s32 prevIndex;
    s32 held;
    s32 idx;
    u16 code;
    u16 species;
    u16 entry;
    u16 rndId;
    s32 mode;
    u32 rnd;
    f32 rate;
    f32 moved;
    f32 rest;
    f32 step;
    f32 dt;

    keys = windowGetKeyInfo(window);
    held = 0;
    prevIndex = *(s32*)&lbl_803A6748;
    dt = (f32)fn_800D3088() / (f32)fn_800D37CC();
    *(f32*)((u8*)&lbl_803A6748 + 0x24) = dt;
    *(f32*)((u8*)&lbl_803A6748 + 0x28) = *(f32*)((u8*)&lbl_803A6748 + 0x28) + dt;
    if (*(f32*)((u8*)&lbl_803A6748 + 0x28) >= lbl_8047BAC8) {
        *(f32*)((u8*)&lbl_803A6748 + 0x28) = lbl_8047BAC4;
    }
    if (*(f32*)((u8*)&lbl_803A6748 + 0x14) != *(f32*)((u8*)&lbl_803A6748 + 0x1c)) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        moved = (*(f32*)((u8*)&lbl_803A6748 + 0x1c) - *(f32*)((u8*)&lbl_803A6748 + 0x14)) * rate;
        *(f32*)((u8*)&lbl_803A6748 + 0x14) = *(f32*)((u8*)&lbl_803A6748 + 0x14) + moved;
        rest = *(f32*)((u8*)&lbl_803A6748 + 0x1c) - *(f32*)((u8*)&lbl_803A6748 + 0x14);
        if (moved > lbl_8047BAC4) {
        } else {
            moved = -moved;
        }
        if (rest > lbl_8047BAC4) {
        } else {
            rest = -rest;
        }
        if (rest <= moved) {
            *(f32*)((u8*)&lbl_803A6748 + 0x14) = *(f32*)((u8*)&lbl_803A6748 + 0x1c);
        }
    }
    if (*(f32*)((u8*)&lbl_803A6748 + 0x18) != *(f32*)((u8*)&lbl_803A6748 + 0x20)) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        moved = (*(f32*)((u8*)&lbl_803A6748 + 0x20) - *(f32*)((u8*)&lbl_803A6748 + 0x18)) * rate;
        *(f32*)((u8*)&lbl_803A6748 + 0x18) = *(f32*)((u8*)&lbl_803A6748 + 0x18) + moved;
        rest = *(f32*)((u8*)&lbl_803A6748 + 0x20) - *(f32*)((u8*)&lbl_803A6748 + 0x18);
        if (moved > lbl_8047BAC4) {
        } else {
            moved = -moved;
        }
        if (rest > lbl_8047BAC4) {
        } else {
            rest = -rest;
        }
        if (rest <= moved) {
            *(f32*)((u8*)&lbl_803A6748 + 0x18) = *(f32*)((u8*)&lbl_803A6748 + 0x20);
        }
    }
    if (*(f32*)((u8*)&lbl_803A6748 + 0x2c) != *(f32*)((u8*)&lbl_803A6748 + 0x34)) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        moved = (*(f32*)((u8*)&lbl_803A6748 + 0x34) - *(f32*)((u8*)&lbl_803A6748 + 0x2c)) * rate;
        *(f32*)((u8*)&lbl_803A6748 + 0x2c) = *(f32*)((u8*)&lbl_803A6748 + 0x2c) + moved;
        rest = *(f32*)((u8*)&lbl_803A6748 + 0x34) - *(f32*)((u8*)&lbl_803A6748 + 0x2c);
        if (moved > lbl_8047BAC4) {
        } else {
            moved = -moved;
        }
        if (rest > lbl_8047BAC4) {
        } else {
            rest = -rest;
        }
        if (rest <= moved) {
            *(f32*)((u8*)&lbl_803A6748 + 0x2c) = *(f32*)((u8*)&lbl_803A6748 + 0x34);
        }
    }
    if (*(f32*)((u8*)&lbl_803A6748 + 0x30) != *(f32*)((u8*)&lbl_803A6748 + 0x38)) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        moved = (*(f32*)((u8*)&lbl_803A6748 + 0x38) - *(f32*)((u8*)&lbl_803A6748 + 0x30)) * rate;
        *(f32*)((u8*)&lbl_803A6748 + 0x30) = *(f32*)((u8*)&lbl_803A6748 + 0x30) + moved;
        rest = *(f32*)((u8*)&lbl_803A6748 + 0x38) - *(f32*)((u8*)&lbl_803A6748 + 0x30);
        if (moved > lbl_8047BAC4) {
        } else {
            moved = -moved;
        }
        if (rest > lbl_8047BAC4) {
        } else {
            rest = -rest;
        }
        if (rest <= moved) {
            *(f32*)((u8*)&lbl_803A6748 + 0x30) = *(f32*)((u8*)&lbl_803A6748 + 0x38);
        }
    }

    if (*(f32*)((u8*)&lbl_803A6748 + 0x48) != *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
        step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
        if (*(f32*)((u8*)&lbl_803A6748 + 0x48) > *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
            if (*(f32*)((u8*)&lbl_803A6748 + 0x44) > lbl_8047BAC8) {
                *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
            }
        } else {
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
            if (*(f32*)((u8*)&lbl_803A6748 + 0x44) < lbl_8047BAC4) {
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
            if (*(s32*)&lbl_803A6748 < (s32)lbl_8047A4DC - 1) {
                if (enabled != 0) {
                    fn_801EEDEC(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
                }
                *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 + 1;
                fn_80166AB8(0x23, 0, 0);
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
                fn_801EEDEC(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
            }
            *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 - 1;
            fn_80166AB8(0x23, 0, 0);
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
            if (fn_801EE8F4(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId) != 0) {
                idx = *(s32*)&lbl_803A6748;
                species = lbl_8047A4D4[idx].field_00;
                entry = lbl_8047A4D4[(u16)idx].battleId;
                rndId = lbl_8047A4D4[idx].battleId;
                fn_801EE614(entry);
                fn_801EE8F4(entry);
                if (fn_801EEAD0(entry) != 0) {
                    if (fn_801EEC74(entry) != 0) {
                        mode = 1;
                    } else {
                        mode = 0;
                    }
                } else {
                    mode = 2;
                }
                pokemonCreate(lbl_8047A4D0, species, 10,
                              gamedataGetStatus(0, 1));
                rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
                fn_801EE750(rndId);
                pokemonBiosSetRnd(lbl_8047A4D0, rnd);
                switch (mode) {
                case 0:
                case 2:
                    pokemonBiosSetDarkpokemonDataId(lbl_8047A4D0, 1);
                    pokemonBiosSetDp(lbl_8047A4D0, 10);
                    break;
                case 1:
                    pokemonBiosSetDarkpokemonDataId(lbl_8047A4D0, 0);
                    pokemonBiosSetDp(lbl_8047A4D0, 0);
                    break;
                default:
                    break;
                }
                fn_80109C88((u8*)&lbl_803A6748 + 0x4c, lbl_8047A4D0);
            }
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x18) = *(f32*)((u8*)&lbl_803A6748 + 0x20);
        idx = *(s32*)&lbl_803A6748 - *(s32*)((u8*)&lbl_803A6748 + 0x8);
        if (idx >= 10) {
            idx = 9;
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x38) =
            (f32)(*(s16*)(lbl_802EF0A8 + 0x50a0) + idx * 0x18);
        *(f32*)((u8*)&lbl_803A6748 + 0x30) =
            (f32)(*(s16*)(lbl_802EF0A8 + 0x50a0) + idx * 0x18);
    }
    return 0;
}
#pragma peephole reset

extern u8 lbl_803A6610[];
extern u32 pokemonCreateRndFit(void* work, s32 a, s32 b, s32 c, s32 d);
extern void winMsgClose(s32 id);
extern void fn_8010A420(void* work);

/* Party-select scene: open the windows, drive the fade, tear it back down. */
#pragma peephole off
void fn_8003C7C0(void)
{
    extern void _threadSwitch(void);
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
    u32 work;
    u32 rnd;
    u32 handle;
    f32 step;

    state = 0;
    running = 1;
    fn_8003CF38();
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
    *(f32*)((u8*)&lbl_803A6748 + 0x2c) = (f32)*(s16*)(lbl_802EF0A8 + 0x509e);
    *(f32*)((u8*)&lbl_803A6748 + 0x34) = (f32)*(s16*)(lbl_802EF0A8 + 0x509e);
    *(f32*)((u8*)&lbl_803A6748 + 0x30) = (f32)*(s16*)(lbl_802EF0A8 + 0x50a0);
    *(f32*)((u8*)&lbl_803A6748 + 0x38) = (f32)*(s16*)(lbl_802EF0A8 + 0x50a0);
    menuModelInit(model, *(s16*)(lbl_802EF0A8 + 0x17ca6),
                  *(s16*)(lbl_802EF0A8 + 0x17ca8));

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
                    fn_80166AB8(0x24, 0, 0);
                    work = lbl_8047A4D0;
                    idx = *(s32*)&lbl_803A6748;
                    entry = lbl_8047A4D4[(u16)idx].battleId;
                    species = lbl_8047A4D4[idx].field_00;
                    rndId = lbl_8047A4D4[idx].battleId;
                    fn_801EE614(entry);
                    fn_801EE8F4(entry);
                    if (fn_801EEAD0(entry) != 0) {
                        if (fn_801EEC74(entry) != 0) {
                            mode = 1;
                        } else {
                            mode = 0;
                        }
                    } else {
                        mode = 2;
                    }
                    pokemonCreate(work, species, 10, gamedataGetStatus(0, 1));
                    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
                    fn_801EE750(rndId);
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
                    fn_80109C88(model, lbl_8047A4D0);
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
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) < lbl_8047BAC4) {
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
                fn_80166AB8(0x25, 0, 0);
                *(f32*)((u8*)&lbl_803A6748 + 0x48) = lbl_8047BAC4;
            }
            break;
        case 4:
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) !=
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
                if (*(f32*)((u8*)&lbl_803A6748 + 0x48) >
                    *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) < lbl_8047BAC4) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
                    }
                }
            }
            if (*(f32*)((u8*)&lbl_803A6748 + 0x48) ==
                *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
                menuCloseCustom(0x33, 0, 1);
                fn_801EEDEC(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
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
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) > lbl_8047BAC8) {
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
                    }
                } else {
                    *(f32*)((u8*)&lbl_803A6748 + 0x44) =
                        *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
                    if (*(f32*)((u8*)&lbl_803A6748 + 0x44) < lbl_8047BAC4) {
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
    fn_8010A420(model);
    if (lbl_8047A4D0 != 0) {
        handle = fn_800E202C(lbl_8047A4D0);
        if ((u16)handle != 0) {
            fn_800E24B0();
            fn_800E209C(handle);
        }
        lbl_8047A4D0 = 0;
    }
    if (lbl_8047A4D4 != NULL) {
        handle = fn_800E202C(lbl_8047A4D4);
        if ((u16)handle != 0) {
            fn_800E24B0();
            fn_800E209C(handle);
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

/* Draw one row per party entry: nickname, owner and battle caption. */
#pragma peephole off
void fn_8003C2B8(PdaSprite* alphaSprite, PdaEvent* event)
{
    u8* slot;
    void* entry;
    u32 name;
    u32 owner;
    u32 caption;
    u32 msg;
    u32 tint;
    u16 id;
    u16 kind;
    s32 i;
    s32 off;
    s32 mode;
    s32 y;
    u8 special;
    f32 rowY;
    f32 firstX;
    f32 firstY;
    f32 shifted;
    f32 stride;

    alphaSprite->alphaByte =
        lbl_8047BAC0 * *(f32*)((u8*)&lbl_803A6748 + 0x44);
    fn_800FE38C(*(s16*)(lbl_802EF0A8 + 0x4fa2) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 2),
                *(s16*)(lbl_802EF0A8 + 0x4fa4) -
                    *(s16*)(lbl_802EF0A8 + event->messageId * 0x1c + 4),
                *(s16*)(lbl_802EF0A8 + 0x4fa6),
                *(s16*)(lbl_802EF0A8 + 0x4fa8));
    slot = (u8*)&lbl_803A6748 + 0x94;
    firstX = *(f32*)((u8*)&lbl_803A6748 + 0x14);
    firstY = *(f32*)((u8*)&lbl_803A6748 + 0x18);
    i = 0;
    off = 0;
    shifted = lbl_8047BAC8 + (firstX - lbl_8047BAF8 - lbl_8047BAFC);
    rowY = firstY;
    stride = lbl_8047BAD8;
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        if (i >= *(s32*)((u8*)&lbl_803A6748 + 0x8) - 1 &&
            i <= *(s32*)((u8*)&lbl_803A6748 + 0xc) + 1) {
            id = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
            if (fn_801EEFAC(id, 0) == 9) {
                entry = pokemonDataBiosGetPtr(fn_801EE248(id));
                if (entry != NULL) {
                    name = GSmsgGetGSchar(
                        (u32)pokemonDataBiosGetName(entry));
                }
            } else {
                entry = fightTrainerPokemonDataBiosGetPtr(
                    (void*)fn_801FCC3C(fightTrainerDataBiosGetPtr(id)));
                name = GSmsgGetGSchar(
                    (u32)fightTrainerPokemonDataBiosGetNickname(entry));
                while (id != fightTrainerPokemonDataBiosGetDarkPokemonFlag(
                                 entry)) {
                    entry = (u8*)entry + 0x50;
                }
                if (fightTrainerPokemonDataBiosGetNickname(entry) != NULL) {
                    name = GSmsgGetGSchar(
                        (u32)fightTrainerPokemonDataBiosGetNickname(entry));
                }
            }
            msg = name;
            if (name == 0) {
                msg = GSmsgGetGSchar(1);
            }
            special = 0;
            owner = 0;
            if (lbl_8047A4D0 != 0) {
                kind = *(u16*)((u8*)lbl_8047A4D4 + off + 2);
                caption = fn_801EE544(kind, slot);
                if (*(s8*)slot == 0) {
                    caption = 0x371f;
                } else if (*(s8*)slot > 0 && *(s8*)slot < 3) {
                    if (kind == 0x43) {
                        caption = 0x12b0;
                    } else if (fn_801EEFAC(kind, 0) == 9) {
                        fn_801EE328(kind);
                        special = 1;
                    } else {
                        caption = fn_801FCC7C(fightTrainerDataBiosGetPtr(kind));
                    }
                }
                if (special == 0) {
                    owner = GSmsgGetGSchar(caption);
                }
            }
            if (owner == 0) {
                owner = GSmsgGetGSchar(1);
            }
            caption = fn_8003CE1C(i);
            if (caption == 0) {
                caption = GSmsgGetGSchar(1);
            }
            id = lbl_8047A4D4[(u16)i].battleId;
            fn_801EE614(id);
            fn_801EE8F4(id);
            if (fn_801EEAD0(id) != 0) {
                if (fn_801EEC74(id) != 0) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            } else {
                mode = 2;
            }
            switch (mode) {
            case 0:
                windowDrawSprite((s16)(s32)shifted,
                                 (s16)(s32)(rowY - lbl_8047BB00),
                                 alphaSprite, 0x161, 0);
                break;
            case 1:
                windowDrawSprite((s16)(s32)(firstX - lbl_8047BAF8),
                                 (s16)(s32)rowY, alphaSprite, 0x160, 0);
                break;
            default:
                break;
            }
            if (fn_801EED88(*(u16*)((u8*)lbl_8047A4D4 + off + 2)) != 0) {
                tint = 0xe0e01800;
            } else {
                tint = -0x100;
            }
            msgctrlSetValue(0x37, msg);
            y = (s32)rowY;
            fn_800FB680(0, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, owner);
            fn_800FB680(0x89, y, tint | alphaSprite->alphaByte, (void*)0xe7);
            msgctrlSetValue(0x37, caption);
            fn_800FB680(0x13d, y, tint | alphaSprite->alphaByte, (void*)0xe7);
        }
        rowY = rowY + stride;
        off += 4;
    }
    fn_800FE35C();
}
#pragma peephole reset

extern u8 lbl_80267060[];
extern u8 lbl_803A65B0[];

/* Position and tint one of the PDA's eight main menu buttons. */
#pragma peephole off
s32 fn_80038E74(void* work, PdaSprite* sprite)
{
    s32 i;

    (void)work;
    for (i = 0; i < 8; i++) {
        if (sprite->eventId == *(s32*)(lbl_80267060 + i * 0x18)) {
            break;
        }
    }
    if (i >= 8) {
        return 0;
    }
    sprite->field_50 = (s32)*(f32*)(lbl_803A65B0 + i * 0xc);
    sprite->field_52 = (s32)*(f32*)(lbl_803A65B0 + i * 0xc + 4);
    sprite->alpha = (s32)*(f32*)(lbl_803A65B0 + i * 0xc + 8);
    if (lbl_80267060[i * 0x18 + 0x14] != 0) {
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

extern u16 pcboxGetNbItemSlot(s32 box);
extern u16 itemBiosGetItemDataId(void* item);
extern void* itemDataBiosGetPtr(u16 id);
extern void* itemDataBiosGetName(void* data);
extern u8 itemDataBiosGetKind(void* data);
extern u16 itemBiosGetNum(void* item);

/* Draw the visible page of the PC item list. */
#pragma peephole off
s32 fn_80039644(void* work, PdaSprite* sprite)
{
    s32 start;
    s32 rows;
    s32 yAdj;
    s32 total;
    s32 slots;
    s32 i;
    s32 j;
    s8 index;
    s32 found;
    s32 rowY;
    s32 nameX;
    s32 y;
    s32 width;
    u16 itemId;
    u16 count;
    void* item;
    void* name;

    (void)work;
    start = 0;
    rows = 8;
    yAdj = 0;
    total = 0;
    slots = pcboxGetNbItemSlot(0);
    for (i = 0; i < slots; i++) {
        if ((u8)fn_801429E8(pcboxGetItem(0, (s16)i)) != 0) {
            total++;
        }
    }
    index = lbl_8047A4A8;
    msgctrlSetValue(0x34, 0x3e7);
    width = (s16)(GSmsgGetRect(0xca) >> 16) +
            (s16)(GSmsgGetRect(0x12e) >> 16);
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
        slots = pcboxGetNbItemSlot(0);
        found = -1;
        for (j = 0; j < slots; j++) {
            item = pcboxGetItem(0, (s16)j);
            if ((u8)fn_801429E8(item) != 0) {
                found++;
                if (found >= index) {
                    itemId = itemBiosGetItemDataId(item);
                    goto haveItem;
                }
            }
        }
        itemId = 0;
    haveItem:
        name = itemDataBiosGetName(itemDataBiosGetPtr(itemId));
        if (name != NULL) {
            fn_800FB680(0, y, (s32)sprite, name);
        }
        if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
            continue;
        }
        fn_800FB680(nameX, y, (s32)sprite, (void*)0x12e);
        slots = pcboxGetNbItemSlot(0);
        found = -1;
        for (j = 0; j < slots; j++) {
            item = pcboxGetItem(0, (s16)j);
            if ((u8)fn_801429E8(item) != 0) {
                found++;
                if (found >= index) {
                    count = itemBiosGetNum(item);
                    goto haveCount;
                }
            }
        }
        count = 0;
    haveCount:
        msgctrlSetValue(0x34, count);
        fn_800FB680(0x11a - (s16)(GSmsgGetRect(0xca) >> 16), y, (s32)sprite,
                    (void*)0xca);
    }
    if (i < rows) {
        fn_800FB680(0, i * 0x1f - yAdj, (s32)sprite, (void*)0x1b67);
    }
    return 0;
}
#pragma peephole reset

extern f32 lbl_8047BA90;
extern f32 lbl_8047BA94;
extern f32 lbl_8047BA98;
extern f32 lbl_8047BAA8;

/* Seed and then advance the main-menu button slide-in animation. */
#pragma peephole off
s32 fn_80039128(u8* work)
{
    s32 i;

    switch (*(s8*)(work + 1)) {
    case 0:
        if (*(s8*)(work + 2) == 0) {
            for (i = 0; i < 8; i++) {
                *(f32*)(lbl_803A65B0 + i * 0xc) =
                    (f32)*(s32*)(lbl_80267060 + i * 0x18 + 4);
                *(f32*)(lbl_803A65B0 + i * 0xc + 4) =
                    (f32)*(s32*)(lbl_80267060 + i * 0x18 + 8);
                *(f32*)(lbl_803A65B0 + i * 0xc + 8) = lbl_8047BAA8;
            }
            work[2] = 1;
        }
        break;
    case 2:
        for (i = 0; i < 8; i++) {
            *(f32*)(lbl_803A65B0 + i * 0xc) =
                *(f32*)(lbl_803A65B0 + i * 0xc) + lbl_8047BA90;
            if (*(f32*)(lbl_803A65B0 + i * 0xc) >
                (f32)*(s32*)(lbl_80267060 + i * 0x18 + 0xc)) {
                *(f32*)(lbl_803A65B0 + i * 0xc) =
                    (f32)*(s32*)(lbl_80267060 + i * 0x18 + 0xc);
            }
            *(f32*)(lbl_803A65B0 + i * 0xc + 8) =
                *(f32*)(lbl_803A65B0 + i * 0xc + 8) + lbl_8047BA94;
            if (*(f32*)(lbl_803A65B0 + i * 0xc + 8) > lbl_8047BA98) {
                *(f32*)(lbl_803A65B0 + i * 0xc + 8) = lbl_8047BA98;
            }
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
