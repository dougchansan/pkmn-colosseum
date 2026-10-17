/**
 * menuCB 0x80055E38 - 0x80056C54 with its own .sdata2 pool, 0x8047BEA0 -
 * 0x8047BEE0 (the constants retail repeats at 0x8047BEE0 for the next unit,
 * menuCB_80056C54.c). Every pool constant is a literal.
 */
#include "dolphin/types.h"

/* ===== SDA globals ===== */
extern u32 lbl_8047A584;
extern f32 lbl_8047A570;
extern f32 lbl_8047A578;
extern f32 lbl_8047A588;
extern u32 lbl_8047A56C;
extern u8 lbl_803A9768[];
extern u8 lbl_80267698[];
extern u8 lbl_802676B4[];

extern void fn_80056C54(u8*, u8*, u32);
extern s32 menuCloseCustom(s32 menuId, s32 mode, s32 wait);
extern void* windowSearchID(s32);
extern u8 fn_80123FBC(void*);
extern void* fn_8012A5B0(void*, u32, u32);
extern void* fn_80134EF0(void*, s32, s32);
extern u8 fn_80107170(u32, u16);
extern void fn_801081F8(void*, u16, u16);
extern void fn_80109220(void*, u32);
extern void winSpriteSetDisp(void*, u32);

typedef struct {
    u32 data[14];
} Tbl14;

typedef struct {
    u32 data[78];
} Tbl78;

extern s32 lbl_8047A568;
extern u32 lbl_8047A580;
extern f32 lbl_8047A574;
extern f32 lbl_8047A57C;
extern u8 lbl_8026768C[];

/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function — see docs/RULE_EXCEPTIONS.md
 * Retail's pool opens with the u32 and then the s32 int-to-float biases ahead of
 * the float literals that fn_80056084 uses first; an unreferenced conversion
 * helper compiled ahead of the range (dead-stripped at link) creates them in
 * that order. Its body rests only on the pool layout. */
f32 menuCBStrippedConvA(u32 a, s32 b) { return (f32)a + (f32)b; }


extern f32 lbl_8047A57C;
extern f32 lbl_8047A574;
extern u32 lbl_8047A580;
extern u8 lbl_8026768C[];
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
extern void gfxSetFlags(u32 flags);
extern void fn_800DC0D4(s32, s32, s32, s32, s32);
extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
extern void fn_800DBFD4(s32, s32, s32, s32, s32);
extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void gfxSetVtxDesc(void* resource);
extern void gfxLoadTexture(s32 slot, void* texture);
extern void gfxSetPrimitive(s32 mode);
extern void gfxBegin(s32 mode);
extern void gfxVtxPos2s16(s16 x, s16 y);
extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
extern void gfxVtxTexCoord(s32 slot, f32 xScale, f32 yScale);
extern void gfxEnd(void);
extern void gfxClearFlags(u32 flags);
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

extern const s32 lbl_80267518[3][30];
extern f32 lbl_8047A570;
extern f32 lbl_8047A574;
extern void* getPokemon__5PCBOXFScSc(void* pcbox, s8 box, s8 slot);


/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u32 fn_80055E38(s32 idx) {
    extern s32 winSeqCheckMove(s32 param);
    extern u8 lbl_8026768C[];
    s32 val;
    u32 tbl[3];
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    if (idx < 0 || idx >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[idx];
    }
    if (val < 0) {
        return 1;
    }
    return (u8)winSeqCheckMove(val) == 0;
}

/* 0x80055EB8 | 0xD0 */
u32 fn_80055EB8(s8* ctx, u8* p) {
    extern s32 fn_80057A08(void);
    extern void* windowSearchID(s32);
    extern s32 fn_80055194(u32*, s32);
    extern void fn_80057094(s16*, s16*);
    extern void winSpriteSetDisp(void*, u32);
    extern u8 lbl_802EF0A8[];
    u8 result;
    u32 out;
    s16 x;
    s16 y;

    result = 0;
    if (fn_80057A08() != 0) {
        ctx = (s8*)windowSearchID(0x93);
        if (ctx != 0) {
            if (fn_80055194(&out, ctx[0x95]) == 0) {
                result = 1;
                fn_80057094(&x, &y);
                *(s16*)(p + 0x50) =
                    (s16)(x + *(s16*)(lbl_802EF0A8 + *(s16*)(p + 6) * 0x1c + 2));
                *(s16*)(p + 0x52) =
                    (s16)(y + *(s16*)(lbl_802EF0A8 + *(s16*)(p + 6) * 0x1c + 4));
            }
        }
    }
    winSpriteSetDisp(p, result);
    return 0;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
static inline s32 fn_80055F88_find(s16 val, u32* ptr) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (val == (s32)*ptr) {
            return i;
        }
        ptr++;
    }
    return -1;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u32 pcBoxDrawBoxName(u8* unused, u8* p) {
    extern u8 lbl_80267680[];
    extern void* pcboxGetPokemonBoxName(void* pcbox, s8 box);
    extern void msgctrlSetValue(u32 id, u32 value);
    extern u32 GSmsgGetRect(u32 val);
    extern void GSmsgPrintColor(s32, s32, s32, s32);
    s32 idx;
    s16 val;
    void* obj;
    s16 width;
    s16 half;
    u32* ptr;

    val = *(s16*)(p + 6);
    ptr = (u32*)lbl_80267680;
    idx = fn_80055F88_find(val, ptr);
    if (idx < 0) {
        return 0;
    }
    obj = pcboxGetPokemonBoxName(0, (s8)idx);
    if (obj == 0) {
        return 0;
    }
    msgctrlSetValue(0x37, (u32)obj);
    width = (s16)(GSmsgGetRect(0xce) >> 16);
    half = *(s16*)(p + 0x54);
    GSmsgPrintColor((s16)(half / 2 - width / 2), 0, -1, 0xce);
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole on

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
static inline s32 menuCBFindPcboxSprite(MenuCBPane* sprite, s32* slot)
{
    const s32 (*row)[30];
    const s32* entry;
    s32 id;
    s32 box;
    s32 col;
    s32 found;
    found = 0;
    id = sprite->itemId;
    for (row = lbl_80267518, box = 0; box < 3; row++, box++) {
        entry = *row;
        for (col = 0; col < 30; entry++, col++) {
            if (id == *entry) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return -1;
    }
    *slot = col;
    return box;
}

s32 fn_80056084(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 box;
    s16 y;
    s32 slot;
    void* texture;
    void* pokemon;
    s32 alpha;
    s32 drawFallback;
    s16 insetX;
    s16 insetY;
    s16 width;
    s16 height;
    s16 x;
    f32 t;
    f32 scaleS0;
    f32 scaleT1;
    f32 scaleT0;
    f32 scaleS1;

    drawFallback = TRUE;
    texture = NULL;
    box = menuCBFindPcboxSprite(sprite, &slot);

    if (box >= 0) {
        pokemon = getPokemon__5PCBOXFScSc(NULL, box, slot);
        if (pokemon != NULL) {
            if (pokemonCheckValid(pokemon) != 0) {
                texture = fn_80057F94(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (drawFallback == FALSE) {
        return 0;
    }

    if (texture != NULL) {
        if (fn_80057DE8(pokemon) != 0) {
            if (lbl_8047A574 < 0.5f) {
                t = 2.0f * lbl_8047A574;
            } else {
                t = 1.0f - 2.0f * (lbl_8047A574 - 0.5f);
            }
            alpha = (s32)(255.0f * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A570 < 1.0f && (s32)lbl_8047A56C == slot) {
            t = 1.0f - lbl_8047A570;
            width = (s16)(t * sprite->width);
            height = (s16)(t * sprite->height);
            insetX = (s16)((sprite->width - width) / 2);
            insetY = (s16)((sprite->height - height) / 2);
        } else {
            width = sprite->width;
            insetX = 0;
            height = sprite->height;
            insetY = 0;
        }

        y = sprite->height;
        x = sprite->width;
        gfxSetFlags(0x80000002);
        fn_800DC0D4(0, 0xf, 0xb, 0xa, 8);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        fn_800DC04C(0, 0, 0, 0, 1, 0);

        scaleS0 = 0.0f / (f32)GStextureGetXsize(texture);
        scaleS1 = (f32)x / (f32)GStextureGetXsize(texture);
        scaleT0 = 0.0f / (f32)GStextureGetYsize(texture);
        scaleT1 = (f32)y / (f32)GStextureGetYsize(texture);

        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxSetPrimitive(7);
        gfxBegin(2);
        gfxVtxPos2s16(insetX, insetY);
        gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
        gfxVtxTexCoord(0, scaleS0, scaleT0);
        gfxVtxPos2s16((s16)(insetX + width), (s16)(insetY + height));
        gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
        gfxVtxTexCoord(0, scaleS1, scaleT1);
        gfxEnd();
        gfxClearFlags(0x80000000);
    } else {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);

        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane,
                          0x232, 0);
    }

    return 0;
}
#pragma pop

/* RULE-EXCEPTION(user-approved): inherited nopeephole mode; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
u32 fn_80056610(u8* p)
{
    s8 state;

    state = (s8)p[1];
    switch (state) {
    case 0:
        if ((s8)p[2] == 0) {
            if (lbl_8047A568 != 0) {
                winSeqSetMenu(*(u32*)(p + 4), 0x107);
            }
            p[2] = 1;
        }
        break;
    case 3:
        if ((s8)p[2] == 0) {
            winSeqSetMenu(*(u32*)(p + 4), 0x10b);
            p[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop

u32 fn_800566B4(void)
{
    return !(lbl_8047A570 >= 0.8f);
}

void fn_800566D8(u32 a)
{
    lbl_8047A56C = a;
    lbl_8047A570 = 0.0f;
}

u32 fn_800566E8(void)
{
    return 0.0f != lbl_8047A578;
}

/* RULE-EXCEPTION(user-approved): inherited nopeephole mode; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
u32 fn_80056704(void)
{
    u32 tbl[3];
    u32 cur;
    s32 val;

    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur - 1;
    lbl_8047A584 = cur;
    if ((s32)cur < 0) {
        lbl_8047A584 = 2;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    cur = lbl_8047A584;
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        menuSetDisp((void*)val, 1);
    }
    lbl_8047A57C = 0.0f;
    lbl_8047A578 = -0.033333335f;
    return lbl_8047A584;
}

u32 fn_800567AC(void)
{
    u32 tbl[3];
    u32 cur;
    s32 val;

    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur + 1;
    lbl_8047A584 = cur;
    if ((s32)cur >= 3) {
        lbl_8047A584 = 0;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    cur = lbl_8047A584;
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        menuSetDisp((void*)val, 1);
    }
    lbl_8047A57C = 0.0f;
    lbl_8047A578 = 0.033333335f;
    return lbl_8047A584;
}
#pragma pop

void fn_80056854(void)
{
    extern void* windowSearchID(u32);
    extern void menuSetDisp(u32, u32);
    f32 old578;
    u32 tbl1[3];
    u32 tbl2[3];

    old578 = lbl_8047A578;
    if (old578 > 0.0f) {
        lbl_8047A57C = lbl_8047A57C + old578;
        if (lbl_8047A57C >= 1.0f) {
            lbl_8047A57C = 1.0f;
            lbl_8047A578 = 0.0f;
        }
    }
    if (lbl_8047A578 < 0.0f) {
        lbl_8047A57C = lbl_8047A57C + lbl_8047A578;
        if (lbl_8047A57C <= -1.0f) {
            lbl_8047A57C = -1.0f;
            lbl_8047A578 = 0.0f;
        }
    }
    if (0.0f != old578) {
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
            if (0.0f == lbl_8047A578) {
                menuSetDisp((u32)val, 0);
            } else {
                void* obj = windowSearchID((u32)val);
                if (obj != (void*)0) {
                    *(s16*)((u8*)obj + 0x84) =
                        (s16)(s32)(-422.0f * lbl_8047A57C);
                }
            }
        }
        idx = lbl_8047A584;
        {
            u32* src = (u32*)&lbl_8026768C;
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
            void* obj = windowSearchID((u32)val);
            if (obj != (void*)0) {
                *(s16*)((u8*)obj + 0x84) =
                    (s16)(s32)(-422.0f * lbl_8047A57C);
                if (old578 > 0.0f) {
                    *(s16*)((u8*)obj + 0x84) =
                        *(s16*)((u8*)obj + 0x84) + 0x1a6;
                } else {
                    *(s16*)((u8*)obj + 0x84) =
                        *(s16*)((u8*)obj + 0x84) - 0x1a6;
                }
            }
        }
    }
    lbl_8047A574 += 0.016666668f;
    if (lbl_8047A574 > 1.0f) {
        lbl_8047A574 = lbl_8047A574 - 1.0f;
    }
    if (lbl_8047A570 < 1.0f) {
        lbl_8047A570 += 0.016666668f;
        if (lbl_8047A570 >= 1.0f) {
            lbl_8047A570 = 1.0f;
        }
    }
}

u32 fn_80056A78(void)
{
    return lbl_8047A584;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_80056A80(void) {
    extern u8 lbl_8026768C[];
    extern void menuCloseSync(s32, s32);
    u32* table;
    s32 i;
    s32 val;
    u32 tbl[3];

    for (i = 0; i < 3; i++) {
        tbl[0] = ((u32*)lbl_8026768C)[0];
        table = (u32*)lbl_8026768C;
        tbl[1] = table[1];
        tbl[2] = table[2];
        if (i < 0 || i >= 3) {
            val = -1;
        } else {
            val = (s32)tbl[i];
        }
        if (val >= 0) {
            menuCloseCustom(val, 2, 0);
        }
    }
    {
        u32 tbl2[3];
        s32 idx;
        tbl2[0] = ((u32*)lbl_8026768C)[0];
        tbl2[1] = ((u32*)lbl_8026768C)[1];
        tbl2[2] = ((u32*)lbl_8026768C)[2];
        idx = (s32)lbl_8047A584;
        if (idx < 0 || idx >= 3) {
            val = -1;
        } else {
            val = (s32)tbl2[idx];
        }
        if (0 <= val) {
            menuCloseSync(val, 1);
        }
    }
}
#pragma peephole on

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
/* RULE-EXCEPTION(user-approved): single-use inline helper for fn_80056B74's menu-id lookup — see docs/RULE_EXCEPTIONS.md */
static inline s32 fn_80056B74_menuId(s32 i) {
    extern u8 lbl_8026768C[];
    u32 tbl[3];

    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    if (i < 0 || i >= 3) {
        return -1;
    }
    return (s32)tbl[i];
}

/* RULE-EXCEPTION(user-approved): local opt_loop_invariants off keeps the table copy inside the loop as retail does — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_loop_invariants off
u32 fn_80056B74(u32 idx, s32 mode) {
    extern u8 lbl_8026768C[];
    extern s32 lbl_8047A568;
    extern u32 lbl_8047A580;
    extern f32 lbl_8047A57C;
    extern f32 lbl_8047A574;
    extern void menuOpenCustom(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);
    extern void menuSetDisp(void* p, u32 enable);
    s32 val;
    s32 i;

    lbl_8047A568 = (u32)mode;
    for (i = 0; i < 3; i++) {
        val = fn_80056B74_menuId(i);
        if (val >= 0) {
            menuOpenCustom(val, 0x1f, 0, 0, 0, 0);
        }
        if ((s32)idx != i) {
            menuSetDisp((void*)val, 0);
        }
    }
    lbl_8047A584 = idx;
    lbl_8047A580 = idx;
    lbl_8047A57C = 0.0f;
    lbl_8047A578 = 0.0f;
    lbl_8047A574 = 0.0f;
    lbl_8047A570 = 1.0f;
    return 1;
}
#pragma pop
#pragma peephole on
