/**
 * Residual source island 0x80055E38-0x80056A80 from the shared menuCB unit.
 */
#define MENUCB_PREFIX_80055E38_ONLY
#include "src/game/menu/menuCB_range_80055E38.c"

extern f32 lbl_8047A57C;
extern f32 lbl_8047A574;
extern u32 lbl_8047A580;
extern u8 lbl_8026768C[];
extern f32 lbl_8047BED0;
extern f32 lbl_8047BED4;
extern f32 lbl_8047BED8;
extern f32 lbl_8047BEB4;
extern void* fn_80104704(u32);
extern f32 lbl_8047A54C;
extern void* lbl_8047A548;
extern f32 lbl_8047A550;
extern f32 lbl_8047A558;
extern f32 lbl_8047BE60;
extern f32 lbl_8047BE64;
extern f32 lbl_8047BE68;
extern f32 lbl_8047BE6C;
extern f32 lbl_8047BE80;
extern f32 lbl_8047BE84;
extern s32 lbl_80267320[6];
extern u8 lbl_80314F98[];
extern void* heroGetStatus(void* hero, s32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern s32 fn_80057DE8(void* pokemon);
extern void* fn_80057F94(void* pokemon);
extern void fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void fn_800D88DC(u32 flags);
extern void fn_800DC0D4(s32, s32, s32, s32, s32);
extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
extern void fn_800DBFD4(s32, s32, s32, s32, s32);
extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void fn_800D7820(void* resource);
extern void fn_800D85D4(s32 slot, void* texture);
extern void fn_800D6A00(s32 mode);
extern void fn_800D67BC(s32 mode);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32 slot, f32 xScale, f32 yScale);
extern void fn_800D6728(void);
extern void fn_800D888C(u32 flags);
extern void* menuSpriteBiosGetPtr(s32 id);
extern void windowDrawSprite2(s32, s32, s16, s16, s32, void*, s32, s32);

typedef struct MenuCBState {
    s32 markKind;
    s32 cursorKind;
} MenuCBState;

typedef struct MenuCBPane {
    u8 pad_00[0x6];
    s16 itemId;
    u8 pad_08[0x44];
    s32 textId;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    u8 pad_58[0x8];
    MenuCBState* state;
    u8 pad_64[0x3];
    u8 alpha;
    u8 pad_68[0x1c];
    s16 originX;
    s16 originY;
    u8 pad_88[0xd];
    s8 boxIndex;
    u8 pad_96;
    s8 previousBoxIndex;
    u8 flag98;
} MenuCBPane;

typedef struct MenuCBLayoutEntry {
    s32 itemId;
    s16 y;
    s16 pad_06;
} MenuCBLayoutEntry;

extern MenuCBLayoutEntry lbl_802E61E8[17];

s32 fn_80056084(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 index;
    s32 partySlot;
    s32 drawFallback;
    void* pokemon;
    void* texture;
    s32 alpha;
    s16 width;
    s16 height;
    s16 x;
    s16 y;
    s16 insetX;
    s16 insetY;
    f32 t;
    f32 scaleS0;
    f32 scaleS1;
    f32 scaleT0;
    f32 scaleT1;

    drawFallback = TRUE;

    for (index = 0; index < 17; index++) {
        if (sprite->itemId == lbl_802E61E8[index].itemId) {
            break;
        }
    }

    if (index >= 17) {
        return 0;
    }

    sprite->y =
        (s16)((s32)(lbl_8047BE80 * lbl_8047A558) + lbl_802E61E8[index].y);
    fn_800FE6D0((s16)(pane->originX + sprite->x),
                (s16)(pane->originY + sprite->y));
    spriteSetEnv();

    texture = NULL;
    partySlot = 0;
    if (sprite->itemId != lbl_80267320[0]) {
        partySlot = 1;
        if (sprite->itemId != lbl_80267320[1]) {
            partySlot = 2;
            if (sprite->itemId != lbl_80267320[2]) {
                partySlot = 3;
                if (sprite->itemId != lbl_80267320[3]) {
                    partySlot = 4;
                    if (sprite->itemId != lbl_80267320[4]) {
                        partySlot = 5;
                        if (sprite->itemId != lbl_80267320[5]) {
                            partySlot = 6;
                        }
                    }
                }
            }
        }
    }
    if (partySlot >= 6) {
        partySlot = -1;
    }

    if (partySlot >= 0) {
        pokemon = heroGetStatus(0, 3, (u16)partySlot);
        if (pokemon != NULL) {
            if ((u8)pokemonCheckValid(pokemon) != 0) {
                texture = fn_80057F94(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (texture != NULL) {
        if (fn_80057DE8(pokemon) != 0) {
            if (lbl_8047A550 < lbl_8047BE6C) {
                t = lbl_8047BE64 * lbl_8047A550;
            } else {
                t = lbl_8047BE60 -
                    (lbl_8047BE64 * (lbl_8047A550 - lbl_8047BE6C));
            }
            alpha = (s32)(lbl_8047BE84 * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A54C < lbl_8047BE60 &&
            lbl_8047A548 == (void*)(u32)partySlot) {
            s32 scaledW;
            s32 scaledH;

            t = lbl_8047BE60 - lbl_8047A54C;
            scaledW = (s32)(t * sprite->width);
            scaledH = (s32)(t * sprite->height);
            width = (s16)scaledW;
            height = (s16)scaledH;
            insetX = (s16)((sprite->width - (s16)scaledW) / 2);
            insetY = (s16)((sprite->height - (s16)scaledH) / 2);
        } else {
            width = sprite->width;
            height = sprite->height;
            insetX = 0;
            insetY = 0;
        }

        x = sprite->width;
        y = sprite->height;
        fn_800D88DC(-0x7ffffffe);
        fn_800DC0D4(0, 0xf, 0xb, 0xa, 8);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        fn_800DC04C(0, 0, 0, 0, 1, 0);

        scaleS0 = lbl_8047BE68 / (f32)GStextureGetXsize(texture);
        scaleS1 = (f32)x / (f32)GStextureGetXsize(texture);
        scaleT0 = lbl_8047BE68 / (f32)GStextureGetYsize(texture);
        scaleT1 = (f32)y / (f32)GStextureGetYsize(texture);

        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, texture);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(insetX, insetY);
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS0, scaleT0);
        fn_800D61E4((s16)(insetX + width), (s16)(insetY + height));
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS1, scaleT1);
        fn_800D6728();
        fn_800D888C(0x80000000);
    } else if (drawFallback != FALSE) {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);

        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane,
                          0x232, 0);
    }

    return 0;
}

void fn_80056854(void) {
    f32 old578;
    u32 tbl1[3];
    u32 tbl2[3];

    old578 = lbl_8047A578;
    if (old578 > lbl_8047BEC0) {
        lbl_8047A57C = lbl_8047A57C + old578;
        if (lbl_8047A57C >= lbl_8047BEB4) {
            lbl_8047A57C = lbl_8047BEB4;
            lbl_8047A578 = lbl_8047BEC0;
        }
    }
    if (lbl_8047A578 < lbl_8047BEC0) {
        lbl_8047A57C = lbl_8047A57C + lbl_8047A578;
        if (lbl_8047A57C <= lbl_8047BED0) {
            lbl_8047A57C = lbl_8047BED0;
            lbl_8047A578 = lbl_8047BEC0;
        }
    }
    if (lbl_8047BEC0 != old578) {
        s32 val;
        u32 idx = lbl_8047A580;
        tbl1[0] = ((u32*)lbl_8026768C)[0];
        tbl1[1] = ((u32*)lbl_8026768C)[1];
        tbl1[2] = ((u32*)lbl_8026768C)[2];
        if ((s32)idx < 0 || (s32)idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl1[idx];
        }
        if (val >= 0) {
            if (lbl_8047BEC0 == lbl_8047A578) {
                fn_80102254((u32)val, 0);
            } else {
                void* obj = fn_80104704((u32)val);
                if (obj != (void*)0) {
                    *(s16*)((u8*)obj + 0x84) = (s16)(s32)(lbl_8047BED4 * lbl_8047A57C);
                }
            }
        }
        idx = lbl_8047A584;
        {
            u32 *src = (u32*)&lbl_8026768C;
            tbl2[0] = src[0];
            tbl2[1] = src[1];
            tbl2[2] = src[2];
        }
        if ((s32)idx < 0 || (s32)idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl2[idx];
        }
        if (val >= 0) {
            void* obj = fn_80104704((u32)val);
            if (obj != (void*)0) {
                *(s16*)((u8*)obj + 0x84) = (s16)(s32)(lbl_8047BED4 * lbl_8047A57C);
                if (old578 > lbl_8047BEC0) {
                    *(s16*)((u8*)obj + 0x84) = *(s16*)((u8*)obj + 0x84) + 0x1a6;
                } else {
                    *(s16*)((u8*)obj + 0x84) = *(s16*)((u8*)obj + 0x84) - 0x1a6;
                }
            }
        }
    }
    lbl_8047A574 = lbl_8047A574 + lbl_8047BED8;
    if (lbl_8047A574 > lbl_8047BEB4) {
        lbl_8047A574 = lbl_8047A574 - lbl_8047BEB4;
    }
    if (lbl_8047A570 < lbl_8047BEB4) {
        lbl_8047A570 = lbl_8047A570 + lbl_8047BED8;
        if (lbl_8047A570 >= lbl_8047BEB4) {
            lbl_8047A570 = lbl_8047BEB4;
        }
    }
}
