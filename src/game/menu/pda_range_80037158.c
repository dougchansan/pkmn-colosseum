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
extern void* fn_80134768(void*, s16);

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
    u8 pad00[6];
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

extern void fn_8003B85C(void* window, s32 enabled);
extern void fn_8003C2B8(PdaSprite* sprite, PdaEvent* event);
extern void fn_80041E48(void* work, s32 mode);
extern void fn_80042658(void* work, s32 mode);
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
        value = *(f32*)&lbl_8047BA58;
        sprite->alpha = (u8)(alpha + 0xff);
        velocity = *(f32*)(lbl_803A654C + 0x50);
        next = *(f32*)(lbl_803A654C + 0x58) + velocity;
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
        value = *(f32*)&lbl_8047BA58;
        sprite->alpha = (u8)(alpha + 0xff);
        velocity = *(f32*)(lbl_803A654C + 0x38);
        next = *(f32*)(lbl_803A654C + 0x40) + velocity;
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
        value = *(f32*)&lbl_8047BA58;
        sprite->alpha = (u8)(alpha + 0xff);
        velocity = *(f32*)(lbl_803A654C + 0x20);
        next = *(f32*)(lbl_803A654C + 0x28) + velocity;
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
        value = *(f32*)&lbl_8047BA58;
        sprite->alpha = (u8)(alpha + 0xff);
        velocity = *(f32*)(lbl_803A654C + 0x8);
        next = *(f32*)(lbl_803A654C + 0x10) + velocity;
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

void fn_800379E8(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[3], 0);
}

void fn_80037BB0(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[2], 0);
}

void fn_80037D78(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[1], 0);
}

void fn_80037F40(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[0], 1);
}

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
    extern void winSeqSetMenu(s32 sequence, s32 menu);

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

void fn_80039F44(void* button)
{
    if (lbl_8047A4B8 < 0) {
        menuButtonNormal(button);
    }
}

s32 fn_8003A6C0(PdaDrawWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267130[3];
    extern s32 lbl_8047A4C8;
    extern void fn_800FB8C8(s32, s32, s16, s16, void*, s32);
    extern void msgctrlSetValue(s32 id, s32 value);
    s32 index;
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

#pragma peephole off
s32 fn_8003AC50(PdaMenuState* state)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);

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

s32 fn_8003AD6C(PdaSelectionWork* work, PdaSprite* sprite)
{
    extern const s32 lbl_80267140[4];
    s32 group;
    s32 found;
    s32 i;

    found = 0;
    group = 0;
    for (i = 0; i < 2 && !found; i++) {
        if (sprite->eventId == lbl_80267140[i]) {
            found = 1;
        }
    }
    if (!found) {
        group = 1;
        for (i = 0; i < 2 && !found; i++) {
            if (sprite->eventId == lbl_80267140[i + 2]) {
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

    winSpriteSetDisp(sprite, group == work->selectedIndex);
    return 0;
}

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
    for (i = 7; i > (s32)lbl_8047A4EC; i--) {
        lbl_802E52C8[i].alpha -= lbl_8047BCC4;
        if (lbl_802E52C8[i].alpha < lbl_8047BC94) {
            lbl_802E52C8[i].alpha = lbl_8047BC94;
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
extern f32 lbl_8047BCF4;
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
extern void fn_8005DA18(u32);
extern u32 fn_80102510();
extern u32 fn_8010264C();
extern u32 fn_801040F0();
extern u8* fn_80105624();
extern u32 fn_80109934();
extern u32 fn_80109B90();
extern u32 fn_8011D8D8();
extern u32 fn_8011D8F4();
extern u32 fn_8011DFE0();
extern u32 fn_801231A4();
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
extern void fn_801EED88(u16);
extern u16 fn_801EEFAC();
extern u32 fn_801FCC7C();
extern u32 fn_801FCCC4();
extern u32 gamedataGetStatus();
extern void menuCloseSync(s32, s32);
extern u32 fn_80018F54();
extern u32 fn_800F915C();

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
            texture = fn_80109934((u8*)&lbl_803A6748 + 0x4C);
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
            state = fn_801EEC74(battleId) != 0;
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
        fn_800FB680(0, 0, (s8)context[0x8B], (void*)messageId);
        break;
    }
}

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
        firstTexture = fn_80109934((u8*)&lbl_803A6818 + 0x7C);
        if (mode == 5) {
            secondTexture = fn_80109934((u8*)&lbl_803A6818 + 0xC4);
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

        fn_8005DA18(*(u32*)(menu + 4));
        keyInfo = fn_80105624();
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

void fn_80044378(u8* context, PdaSprite* sprite)
{
    u8* scene;
    u8* entry;
    s16 messageId;
    s32 i;
    s32 offset;
    f32 y;
    u16 pokemonId;
    u32 model;

    scene = (u8*)&lbl_803A6818;
    if (sprite->messageId == 0x12B2) {
        context[0x8B] = lbl_8047BCA0 * *(f32*)(scene + 0x54);
    } else {
        context[0x8B] = lbl_8047BCA0 * *(f32*)(scene + 0x4C);
    }
    messageId = sprite->eventId;
    switch (messageId) {
    case 0xD46:
    case 0x12B2:
    case 0x31D:
    case 0x31E:
        return;
    case 0x119B:
        if (fn_8004BDEC() == 1 && fn_8004BDFC() >= 1) {
            fn_800492CC(context, sprite);
        }
        return;
    case 0x76D:
        fn_80132A38(0x34, fn_8025FEE4(0));
        return;
    }

    entry = lbl_802EF0A8 + messageId * 0x1C;
    fn_800FE38C(
        *(s16*)(lbl_802EF0A8 + 0x5712) - *(s16*)(entry + 2),
        *(s16*)(lbl_802EF0A8 + 0x5714) - *(s16*)(entry + 4),
        *(s16*)(lbl_802EF0A8 + 0x5716),
        *(s16*)(lbl_802EF0A8 + 0x5718));
    y = *(f32*)(scene + 0x30);
    offset = 0;
    for (i = 0; i < lbl_8047A4E8; i++, offset += 2) {
        if (i >= *(s32*)(scene + 8) - 1 &&
            i <= *(s32*)(scene + 0xC) + 1) {
            model = lbl_8047A4E0;
            if (model != 0) {
                pokemonId = lbl_8047A4E4[i];
                if (pokemonId >= 0x8000) {
                    pokemonId &= 0x3FFF;
                }
                fn_801240C4(model, pokemonId, 0xA,
                             gamedataGetStatus(0, 1));
                fn_8011DFE0(model, fn_8025FDDC(0, pokemonId));
                fn_8011DF90(model, fn_8025FD34(0, pokemonId));
                fn_8011F5C8();
                fn_8011E778();
                model = fn_800FA280(fn_8011E760());
            }
            if (model == 0) {
                model = fn_800FA280(1);
            }
            fn_80132A38(0x37, model);
            fn_800FB680(0, (s32)y - 2, (s8)context[0x8B],
                         (void*)0xE7);
        }
        y += lbl_8047BCF4;
    }
    fn_800FE35C();
}

void fn_8003B478(u8* context)
{
    u8* window;
    PdaListEntry* entry;
    u16 battleId;
    u32 model;
    u32 message;
    s32 battleState;

    window = (u8*)&lbl_803A6748;
    context[0x8B] = lbl_8047BAC0 * *(f32*)(window + 0x44);
    entry = &lbl_8047A4D4[*(u32*)window];
    battleId = entry->battleId;
    fn_801EE614(battleId);
    fn_801EE8F4(battleId);
    if (fn_801EEAD0(battleId) != 0) {
        battleState = fn_801EEC74(battleId) != 0;
    } else {
        battleState = 2;
    }

    model = lbl_8047A4D0;
    fn_801240C4(model, entry->field_00, 0xA, gamedataGetStatus(0, 1));
    fn_801EE750(battleId);
    fn_8011DFE0(model, *(u32*)(window + 0x98));
    if (battleState == 1) {
        fn_8011D8F4(model, 0);
        fn_8011D8D8(model, 0);
    } else {
        fn_8011D8F4(model, 1);
        fn_8011D8D8(model, 0xA);
    }

    if (model == 0 || battleId == 0) {
        message = 0x80;
    } else if (fn_801EE8F4(battleId) != 0) {
        fn_8011DFE0(model, fn_801EE750(battleId));
        message = fn_801231A4(model);
        fn_8011DFE0(model, *(u32*)(window + 0x98));
    } else {
        message = 2;
    }
    message &= 0xFF;
    if (message == 0x80) {
        fn_80132A38(0x37, fn_800FA280(1));
    } else if (message == 0) {
        fn_80132A38(0x37, fn_800FA280(0xD67));
    } else if (message == 1) {
        fn_80132A38(0x37, fn_800FA280(0xD68));
    } else if (message == 2) {
        fn_80132A38(0x37, fn_800FA280(0xD69));
    } else {
        fn_80132A38(0x37, fn_800FA280(0xD68));
    }
    fn_800FB680(0, 0, (s8)context[0x8B], (void*)0xCF);
}

void fn_8004B598(s32 unused, PdaSprite* sprite, s32 messageId)
{
    u32 periods[3];
    s8 selection;
    f32* angles;
    s16* layout;
    u32 active;

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

    selection = (s8)lbl_803A6A60[0];
    if (selection < 0) {
        selection = (s8)lbl_803A6A60[0x48];
    } else {
        lbl_803A6A60[0x48] = selection;
    }
    active = *(u32*)(lbl_803A6A60 + 0x44);
    switch (selection) {
    case 0:
        layout = (s16*)(lbl_802EF0A8 + (active + 0x6E7) * 0x1C);
        break;
    case 1:
        layout = (s16*)(lbl_802EF0A8 + (active + 0x75D) * 0x1C);
        break;
    case 2:
        layout = (s16*)(lbl_802EF0A8 + (active + 0x760) * 0x1C);
        break;
    case 3:
        layout = (s16*)(lbl_802EF0A8 + (active + 0x763) * 0x1C);
        break;
    default:
        layout = (s16*)(lbl_802EF0A8 + 0xC144);
        break;
    }

    angles = (f32*)(lbl_803A6A60 + 0xC);
    angles[active] += *(f32*)(lbl_803A6A60 + 8) *
                      (lbl_8047BDF0 / *(f32*)&periods[active]);
    if (angles[active] > lbl_8047BDF0) {
        angles[active] -= lbl_8047BDF0;
    }
    if (angles[active] < lbl_8047BDAC) {
        angles[active] += lbl_8047BDF0;
    }
    sprite->value = angles[active];
    sprite->field_50 = layout[1];
    sprite->field_52 = layout[2];
}

void fn_8003CF38(void)
{
    u8 temporary[8];
    u8* sortEntries;
    s32 count;
    s32 gap;
    s32 i;
    s32 j;
    u32 allocation;
    u32 handle;

    lbl_8047A4DC = 0x60;
    lbl_8047A4D0 = 0;
    allocation = fn_800E2C04(0x140, 0x20);
    if ((u16)allocation != 0) {
        lbl_8047A4D0 = fn_800E27B0();
    }
    allocation = fn_800E2C04(
        ((lbl_8047A4DC * 4) + 0x1F) & ~0x1F, 0x20);
    if ((u16)allocation != 0) {
        lbl_8047A4D4 = (PdaListEntry*)fn_800E27B0();
    } else {
        lbl_8047A4D4 = 0;
    }
    allocation = fn_800E2C04(
        ((lbl_8047A4DC * 8) + 0x1F) & ~0x1F, 0x20);
    if ((u16)allocation != 0) {
        lbl_8047A4D8 = fn_800E27B0();
    } else {
        lbl_8047A4D8 = 0;
    }

    count = 0;
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        u16 value;
        u16 battleId;

        battleId = i + 1;
        value = fn_801EE248(battleId);
        if (value != 0 &&
            (fn_801EE8F4(battleId) != 0 ||
             fn_801EE614(battleId) != 0)) {
            lbl_8047A4D4[count].field_00 = value;
            lbl_8047A4D4[count].battleId = battleId;
            *(u32*)((u8*)lbl_8047A4D8 + count * 8) =
                fn_801EE0BC();
            *(PdaListEntry*)((u8*)lbl_8047A4D8 + count * 8 + 4) =
                lbl_8047A4D4[count];
            count++;
        }
    }
    lbl_8047A4DC = count;

    sortEntries = (u8*)lbl_8047A4D8;
    for (gap = count / 2; gap > 0; gap /= 2) {
        for (i = gap; i < count; i++) {
            j = i - gap;
            while (j >= 0 &&
                   *(s32*)(sortEntries + j * 8) >
                       *(s32*)(sortEntries + (j + gap) * 8)) {
                memcpy(temporary, sortEntries + j * 8, 8);
                memcpy(sortEntries + j * 8,
                       sortEntries + (j + gap) * 8, 8);
                memcpy(sortEntries + (j + gap) * 8, temporary, 8);
                j -= gap;
            }
        }
    }
    for (i = 0; i < count; i++) {
        lbl_8047A4D4[i] =
            *(PdaListEntry*)(sortEntries + i * 8 + 4);
    }

    handle = fn_800E202C(lbl_8047A4D8);
    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
    lbl_8047A4D8 = 0;
}

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

void fn_8003A520(void)
{
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

void fn_8003B2D8(u8* context)
{
    u8* window;
    u16 battleId;
    u32 message;
    u32 secondary;
    s32 variant;

    extern s32 fn_801EE544(u16, s8*);

    window = (u8*)&lbl_803A6748;
    context[0x8B] = lbl_8047BAC0 * *(f32*)(window + 0x44);
    secondary = 0;
    if (lbl_8047A4D0 == 0) {
        message = 0;
    } else {
        battleId = lbl_8047A4D4[(u16)*(u32*)window].battleId;
        message = fn_801EE544(battleId, (s8*)(window + 0x94));
        variant = *(u8*)(window + 0x94);
        if (variant == 0) {
            message = 0x371F;
        } else if (variant < 3) {
            if (battleId == 0x43) {
                message = 0x12B0;
            } else if (fn_801EEFAC(battleId, 0) == 9) {
                message = fn_801EE328(battleId);
                secondary = 1;
            } else {
                fn_801FCCC4();
                message = fn_801FCC7C();
            }
        }
        if (secondary == 0) {
            message = fn_800FA280(message);
        }
    }
    if (message == 0) {
        message = fn_800FA280(1);
    }
    secondary = fn_8003CE1C(*(u32*)window);
    if (secondary == 0) {
        secondary = fn_800FA280(1);
    }
    fn_80132A38(0x37, secondary);
    fn_800FB680(0, 0, (s8)context[0x8B], (void*)0xE7);
    fn_80132A38(0x37, message);
    fn_800FB680(0xB4, 0, (s8)context[0x8B], (void*)0xE7);
}
/* Readable ports reconstructed from the PDA callback state machines. */
void fn_80037180(u32 unused, u8* p) {
    u32 result;
    if (*(s16*)(p + 6) == 0x36c) {
        result = fn_80005748();
        if (lbl_8047A498 != 0) {
            result = lbl_8047A498;
        }
        lbl_8047A49C = fn_801EF214();
        lbl_8047A4A0 = fn_801EF274();
        fn_80132A38(0x4c, result);
        fn_80132A38(0x2f, fn_801EF274());
        fn_80132A38(0x30, fn_801EF214());
        if ((result = fn_80109934(lbl_803A6498)) != 0) {
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
    } else {
        if ((s32)lbl_8047A49C == 0) {
            *(s8*)(p + 4) = (s8)(p[4] & ~2);
        }
    }
}

void fn_80039F70(u8* ctx) {
    s8 state;
    void* entry;
    u32 seen;
    u32 index;
    u16 count;
    u32 target;
    u32 message;
    f32 f0;
    f32 f1;
    f32 f2;

    state = (s8)ctx[1];
    switch (state) {
    case 0:
        if ((s8)ctx[2] != 0) {
            return;
        }
        fn_801080CC(0x25, 0xb4);
        target = lbl_8047A4A8 + lbl_8047A4AC;
        count = (u16)fn_801347D0(0);
        seen = (u32)-1;
        entry = (void*)0;
        for (index = 0; index < (u32)count; index++) {
            entry = fn_80134768((void*)0, (s16)index);
            if ((u8)fn_801429E8(entry) != 0) {
                seen++;
                if (seen >= target) {
                    break;
                }
            }
        }
        if (index < (u32)count) {
            message = (u32)(u16)fn_80143C68(entry);
        } else {
            message = 0;
        }
        if (message != 0) {
            message = fn_80143F84((void*)fn_801440A0(message));
        } else {
            message = 0x1b68;
        }
        lbl_8047A4B4 = message;
        lbl_8047A4C0 = lbl_8047BAB0;
        lbl_8047A4BC = 0;
        lbl_8047A4B8 = (u32)-1;
        ctx[2] = 1;
        break;
    case 2:
        f2 = lbl_8047A4C0;
        f1 = lbl_8047BAB0;
        if (f2 > f1) {
            f0 = f2 - lbl_8047BABC;
            lbl_8047A4C0 = f0;
            if (f0 < f1) {
                lbl_8047A4C0 = f1;
            }
        }
        f2 = lbl_8047A4C0;
        f1 = lbl_8047BAB0;
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
            fn_801080CC(0x25, 0xb8);
            ctx[2] = 1;
        }
        break;
    }
}

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
    if ((fn_801EEFAC(battleId, 0) & 0xFFFF) == 9) {
        fn_801EE248(battleId);
        value = fn_8011E778();
        if (value != 0) {
            message = fn_800FA280(fn_8011E760());
        }
    } else {
        record = fn_801FCA2C(fn_801FCC3C(fn_801FCCC4()));
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
            while (k >= 0) {
                q = base + (k + gap) * 0x1a;
                if (GScharCmp((u32)(p + 2), (u32)(q + 2)) < 0) {
                    break;
                }
                memcpy(tmp, p, 0x1a);
                memcpy(p, q, 0x1a);
                memcpy(q, tmp, 0x1a);
                p -= gap * 0x1a;
                k -= gap;
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

/* Main summary-screen state machine. */
void fn_80044630(void) {
    extern void  _threadSwitch(void);
    extern u32   fn_80102568(u32 a, u32 b, u32 c);
    extern u32   fn_8010264C(u32 a, u32 b);
    extern void  fn_800E01D0(void* dst, void* src);
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
    extern void* fn_800F9318();
    extern void* fn_80113F48(void);
    extern void  fn_801CB9D8(void* h);
    extern void  fn_801CB954(void* h, s32 b);
    extern void  fn_800E4014(void* a, s32 b);
    extern void  fn_800EC9DC(f32 v);
    extern void  fn_800EC160(void* h, s32 b);
    extern void  fn_800E3C94(void* h, s32 b);
    extern void  fn_800E3BC0(void);
    extern void* fn_801DAC3C(void* h);
    extern void  fn_80176E0C(u32 a, u32 id, u32 c, u32 d);
    extern void  fn_8010A420(void* subobj);
    extern void  fn_801096F8(s32 a);
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
            fn_8010264C(0x35, 0);
            fn_8010264C(0x3b, 0);
            fn_8010264C(0x36, 0);
            fn_8010264C(0x37, 0);
            fn_8010264C(0x38, 0);
            fn_8010264C(0x39, 0);
            *(s32*)(S + 0x1c) = 1;
        } break;

        case 1: {   /* .L_80044750 */
            s32 r;
            r = (s32)fn_8010264C(0x3a, 1);
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
            fn_8010264C(0x96, 0);
            fn_8010264C(0xa1, 0);
            r = (s32)fn_8010264C(0xa2, 1);
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
            fn_8010264C(0x96, 0);
            r = (s32)fn_8010264C(0xa2, 1);
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
            r = (s32)fn_8010264C(0xce, 1);
            if (r < 0) {
                *(s32*)(S + 0x1c) = 0xc;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                fn_80102568(0xce, 0, 0);
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
            fn_80102568(0x35, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
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
                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), cam);
                fn_800EC9DC(lbl_8047BD18);
                fn_800EC160(*(void**)(S + 0x110), 1);
                fn_800E3C94(*(void**)(S + 0x110), 0);

                ((u32*)mtx)[0] = ((const u32*)tbl)[6];
                ((u32*)mtx)[1] = ((const u32*)tbl)[7];
                ((u32*)mtx)[2] = ((const u32*)tbl)[8];
                ((u32*)pos)[0] = ((const u32*)tbl)[9];
                ((u32*)pos)[1] = ((const u32*)tbl)[10];
                ((u32*)pos)[2] = ((const u32*)tbl)[11];
                cam = fn_800F9318(0x17, 0x0D741800);
                *(void**)(S + 0x114) = cam;
                GScameraGetPosition(cam, pos);
                GScameraGetRotation(cam, rot);
                mtx[0] = *(f32*)((u8*)cam + 0x100);
                mtx[1] = *(f32*)((u8*)cam + 0x104);
                mtx[2] = *(f32*)((u8*)cam + 0x108);
                GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
                GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rot);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = persp0;
                *(f32*)(S + 0x140) = persp1;
                *(f32*)(S + 0x144) = persp2;
                *(f32*)(S + 0x148) = persp3;

                fn_80176E0C(0x17, 0x0D741800, 0, 1);
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
                            fn_800E3BC0();
                        *(f32*)(S + 0x64) = lbl_8047BCA8;
                        break;
                    case 2:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 5;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            fn_800E3BC0();
                        break;
                    case 0:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 2;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            fn_800E3BC0();
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
                fn_80102568(0x3C, 0, 1);
                fn_80102568(0x96, 0, 1);
                fn_80102568(0xA2, 0, 1);

                h = *(void**)(S + 0x10C);
                fn_801CB954(h, 0);
                {
                    void* rv = fn_800F9318(fn_80113F48(), h);
                    if (rv != 0)
                        fn_800E4014(rv, 0);
                }
                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = fn_801CBA0C(0x0D171000);
                *(void**)(S + 0x10C) = h;
                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
                fn_800EC9DC(lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = fn_800F9318(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &staging0, &staging1, &staging2, &staging3);

                fn_800E01D0(S218, pos);
                fn_800E01D0(S218 + 0xC, mtx);
                *(f32*)(S218 + 0x18) = staging0;
                *(f32*)(S218 + 0x1C) = staging1;
                *(f32*)(S218 + 0x20) = staging2;
                *(f32*)(S218 + 0x24) = staging3;
                GScameraSetPerspective(h, staging0, staging1, staging2, staging3);

                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rot);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = staging0;
                *(f32*)(S + 0x140) = staging1;
                *(f32*)(S + 0x144) = staging2;
                *(f32*)(S + 0x148) = staging3;

                fn_80176E0C(0x17, 0x0D731800, 0, 1);
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
            fn_80102568(0xa1, 0, 1);
            fn_80102568(0x96, 0, 1);
            fn_80102568(0xa2, 0, 1);

            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
            }

            /* .L_800453AC camera-setup */
            fn_801CB9D8(h);
            *(f32*)(S + 0x6C) = lbl_8047BC94;
            *(f32*)(S + 0x70) = lbl_8047BC94;
            *(f32*)(S + 0x74) = lbl_8047BC94;
            *(f32*)(S + 0x64) = lbl_8047BCF0;

            h = fn_801CBA0C(0x0D171000);
            *(void**)(S + 0x10C) = h;
            *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
            fn_800EC9DC(lbl_8047BD18);

            ((u32*)mtx)[0] = ((const u32*)tbl)[0];
            ((u32*)mtx)[1] = ((const u32*)tbl)[1];
            ((u32*)mtx)[2] = ((const u32*)tbl)[2];
            ((u32*)pos)[0] = ((const u32*)tbl)[3];
            ((u32*)pos)[1] = ((const u32*)tbl)[4];
            ((u32*)pos)[2] = ((const u32*)tbl)[5];
            h = fn_800F9318(0x17, 0x0D731800);
            *(void**)(S + 0x114) = h;

            GScameraGetPosition(h, pos);
            GScameraGetRotation(h, rot);

            mtx[0] = *(f32*)((u8*)h + 0x100);
            mtx[1] = *(f32*)((u8*)h + 0x104);
            mtx[2] = *(f32*)((u8*)h + 0x108);
            GScameraGetPerspective(h, &px0, &px1, &px2, &px3);

            fn_800E01D0(S218, pos);
            fn_800E01D0(S218 + 0xC, mtx);

            *(f32*)(S218 + 0x18) = px0;
            *(f32*)(S218 + 0x1C) = px1;
            *(f32*)(S218 + 0x20) = px2;
            *(f32*)(S218 + 0x24) = px3;
            GScameraSetPerspective(h, px0, px1, px2, px3);

            fn_800E01D0(S118, pos);
            fn_800E01D0(S124, rot);
            fn_800E01D0(S130, mtx);

            *(f32*)(S + 0x13C) = px0;
            *(f32*)(S + 0x140) = px1;
            *(f32*)(S + 0x144) = px2;
            *(f32*)(S + 0x148) = px3;

            fn_80176E0C(0x17, 0x0D731800, 0, 1);
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
            fn_80102568(0x96, 0, 1);
            fn_80102568(0xA2, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
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

                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
                fn_800EC9DC(lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = fn_800F9318(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rotbuf);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &p0, &p1, &p2, &p3);

                fn_800E01D0(S218, pos);
                fn_800E01D0(S218 + 0x0C, mtx);

                *(f32*)(S218 + 0x18) = p0;
                *(f32*)(S218 + 0x1C) = p1;
                *(f32*)(S218 + 0x20) = p2;
                *(f32*)(S218 + 0x24) = p3;
                GScameraSetPerspective(h, p0, p1, p2, p3);

                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rotbuf);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = p0;
                *(f32*)(S + 0x140) = p1;
                *(f32*)(S + 0x144) = p2;
                *(f32*)(S + 0x148) = p3;
                fn_80176E0C(0x17, 0x0D731800, 0, 1);
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
                fn_80102568(0x35, 0, 1);
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
                fn_80102568(0x96, 0, 1);
                fn_80102568(0xA2, 0, 1);
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
            fn_80102568(0x35, 0, 1);
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
        void* rv = fn_800F9318(fn_80113F48(), h);
        if (rv != 0)
            fn_800E4014(rv, 0);
    }
    fn_801CB9D8(h);

        h = *(void**)((u8*)&lbl_803A6818 + 0x150);
    fn_801CB954(h, 0);
    {
        void* rv = fn_800F9318(fn_80113F48(), h);
        if (rv != 0)
            fn_800E4014(rv, 0);
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

    fn_801096F8(1);
        fn_8010A420((u8*)&lbl_803A6818 + 0x7c);
        fn_8010A420((u8*)&lbl_803A6818 + 0xc4);

        h = (void*)fn_800E202C((u32)lbl_8047A4E4);
    if ((u16)(u32)h != 0) {
        fn_800E24B0();
        fn_800E209C((u32)h);
    }
    lbl_8047A4E4 = 0;
}
