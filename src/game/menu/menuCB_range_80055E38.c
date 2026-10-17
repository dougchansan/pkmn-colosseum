/**
 * @file menuCB_range_80055E38.c
 * @brief colosseum-battle team/status display screens, 0x80055E38 - 0x80057B34.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. menuSetDisp/winSeq screens, pokemon status
 * drawing via windowDrawSprite2. Identity SPECULATIVE (0 XD anchors;
 * structural-family evidence only: distinct .data pool band and static bss
 * 0x803A9768 shared across this range).
 *
 * fn_80056A78 (0x80056A78): trivial sda_getter, ported from the previous
 * campaign's archive/previous_campaign/src/game/menu/menu_status.c.
 * fn_80056A80 (0x80056A80): exact pure-C recovery. Other source-backed
 * functions remain candidates until their own exact islands are proven.
 */
#include "dolphin/types.h"

/* ===== SDA globals ===== */
extern u32 lbl_8047A584;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BEC4;
extern f32 lbl_8047A570;
extern f32 lbl_8047A578;
extern f32 lbl_8047A588;
extern f32 lbl_8047BF00;
extern f32 lbl_8047BF04;
extern f32 lbl_8047BEF4;
extern f32 lbl_8047BF08;
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

#if !defined(MENUCB_PREFIX_80055E38_ONLY) && \
    !defined(MENUCB_TU_80056C54_ONLY) && \
    !defined(MENUCB_PREFIX_RESIDUAL_80055E38_ONLY) && \
    !defined(MENUCB_EXACT_80056A80_ONLY) && \
    !defined(MENUCB_SUFFIX_80056B74_ONLY) && \
    !defined(MENUCB_EXACT_80057144_ONLY) && \
    !defined(MENUCB_EXACT_80057270_ONLY) && \
    !defined(MENUCB_EXACT_80056B74_ONLY) && \
    !defined(MENUCB_EXACT_80057538_ONLY) && \
    !defined(MENUCB_EXACT_80057694_ONLY)
#define MENUCB_RANGE_80055E38_ALL
#endif

/* ===== Function implementations ===== */

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_PREFIX_80055E38_ONLY) || \
    defined(MENUCB_PREFIX_RESIDUAL_80055E38_ONLY)

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

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_PREFIX_80055E38_ONLY)

u32 fn_80056704(void) {
    extern u32 lbl_8047A580;
    extern f32 lbl_8047BEC8;
    extern f32 lbl_8047A57C;
    extern u8 lbl_8026768C[];
    extern void menuSetDisp(void* p, u32 enable);
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
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BEC8;
    return lbl_8047A584;
}

u32 fn_800567AC(void) {
    extern u32 lbl_8047A580;
    extern f32 lbl_8047BECC;
    extern f32 lbl_8047A57C;
    extern u8 lbl_8026768C[];
    extern void menuSetDisp(void* p, u32 enable);
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
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BECC;
    return lbl_8047A584;
}

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_EXACT_80056A80_ONLY)

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

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_EXACT_80056B74_ONLY)

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
u32 fn_80056B74(u32 idx, s32 mode) {
    extern u8 lbl_8026768C[];
    extern s32 lbl_8047A568;
    extern u32 lbl_8047A580;
    extern f32 lbl_8047A57C;
    extern f32 lbl_8047A574;
    extern f32 lbl_8047BEB4;
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
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BEC0;
    lbl_8047A574 = lbl_8047BEC0;
    lbl_8047A570 = lbl_8047BEB4;
    return 1;
}

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_PREFIX_80055E38_ONLY)

u32 fn_80056610(u8* p) {
    extern s32 lbl_8047A568;
    extern void winSeqSetMenu(s32 param, u32 key);
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
#pragma peephole on

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_PREFIX_80055E38_ONLY) || \
    defined(MENUCB_PREFIX_RESIDUAL_80055E38_ONLY)

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

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_PREFIX_80055E38_ONLY)

u32 fn_80056A78(void) {
    return lbl_8047A584;
}

u32 fn_800566B4(void) {
    return !(lbl_8047A570 >= lbl_8047BEC4);
}

void fn_800566D8(u32 a) {
    lbl_8047A56C = a;
    lbl_8047A570 = lbl_8047BEC0;
}

u32 fn_800566E8(void) {
    return lbl_8047BEC0 != lbl_8047A578;
}

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80056C54(u8* a, u8* b, u32 c) {
    extern f32 lbl_8047A58C;
    extern u8 lbl_80314F98[];
    extern u8 pokemonCheckValid(u8* pokemon);
    extern void fn_800FE6D0(s16, s16);
    extern void spriteSetEnv(void);
    extern s32 fn_80057DE8(void*);
    extern void* fn_80057F94(void*);
    extern u16 GStextureGetXsize(void*);
    extern u16 GStextureGetYsize(void*);
    extern void gfxSetVtxDesc(void*);
    extern void gfxLoadTexture(s32, void*);
    extern void gfxSetPrimitive(s32);
    extern void gfxBegin(s32);
    extern void gfxVtxPos2s16(s16, s16);
    extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
    extern void gfxVtxTexCoord(s32, f32, f32);
    extern void gfxEnd(void);
    extern void gfxSetFlags(u32);
    extern void gfxClearFlags(u32);
    extern void fn_800DC0D4(s32, s32, s32, s32, s32);
    extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
    extern void fn_800DBFD4(s32, s32, s32, s32, s32);
    extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
    extern void* menuSpriteBiosGetPtr(s32);
    extern void windowDrawSprite2(s32, s32, s16, s16, s32, void*, s32, s32);
    s16 h;
    void* texture;
    s32 alpha;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 w;
    u8* pokemon;
    f32 t;
    f32 s0;
    f32 t1;
    f32 t0;
    f32 s1;

    *(s16*)(a + 0x84) = (s16)*(f32*)(lbl_803A9768 + 0x27c);
    *(s16*)(a + 0x86) = (s16)*(f32*)(lbl_803A9768 + 0x280);
    fn_800FE6D0((s16)(*(s16*)(a + 0x84) + *(s16*)(b + 0x50)),
                (s16)(*(s16*)(a + 0x86) + *(s16*)(b + 0x52)));
    spriteSetEnv();

    pokemon = lbl_803A9768 + c * 0x138 + 8;
    if (pokemonCheckValid(pokemon) != 0) {
        texture = fn_80057F94(pokemon);
        if (texture != NULL) {
            if (fn_80057DE8(pokemon) != 0) {
                if (lbl_8047A58C < 0.5f) {
                    t = 2.0f * lbl_8047A58C;
                } else {
                    t = 1.0f - 2.0f * (lbl_8047A58C - 0.5f);
                }
                alpha = (s32)(255.0f * t);
            } else {
                alpha = 0;
            }

            if (lbl_8047A588 < 1.0f) {
                t = 1.0f - lbl_8047A588;
                width = (s16)(t * *(s16*)(b + 0x54));
                height = (s16)(t * *(s16*)(b + 0x56));
                x = (s16)((*(s16*)(b + 0x54) - width) / 2);
                y = (s16)((*(s16*)(b + 0x56) - height) / 2);
            } else {
                width = *(s16*)(b + 0x54);
                x = 0;
                height = *(s16*)(b + 0x56);
                y = 0;
            }

            h = *(s16*)(b + 0x56);
            w = *(s16*)(b + 0x54);
            gfxSetFlags(0x80000002);
            fn_800DC0D4(0, 0xf, 0xb, 0xa, 8);
            fn_800DC14C(0, 0, 0, 0, 1, 0);
            fn_800DBFD4(0, 7, 7, 7, 4);
            fn_800DC04C(0, 0, 0, 0, 1, 0);
            s0 = 0.0f / GStextureGetXsize(texture);
            s1 = w / (f32)GStextureGetXsize(texture);
            t0 = 0.0f / GStextureGetYsize(texture);
            t1 = h / (f32)GStextureGetYsize(texture);
            gfxSetVtxDesc(lbl_80314F98);
            gfxLoadTexture(0, texture);
            gfxSetPrimitive(7);
            gfxBegin(2);
            gfxVtxPos2s16(x, y);
            gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
            gfxVtxTexCoord(0, s0, t0);
            gfxVtxPos2s16((s16)(x + width), (s16)(y + height));
            gfxVtxColor4u8(0, 0x3c, 0xc, 0xff, alpha);
            gfxVtxTexCoord(0, s1, t1);
            gfxEnd();
            gfxClearFlags(0x80000000);
            return;
        }
        {
            s16 drawW = *(s16*)((u8*)menuSpriteBiosGetPtr(0x232) + 0xc);

            windowDrawSprite2(0, 0, drawW,
                              *(s16*)((u8*)menuSpriteBiosGetPtr(0x232) + 0xe),
                              -1, a, 0x232, 0);
        }
    }
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop

#pragma scheduling off
void fn_80057094(s16* a, s16* b) {
    *a = (s16)(s32)*(f32*)(lbl_803A9768 + 0x27c);
    *b = (s16)(s32)*(f32*)(lbl_803A9768 + 0x280);
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling on; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling on

#pragma scheduling off
u32 fn_800570D0(u8* a, u8* b) {
    s32 c;
    c = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1);
    fn_80056C54(a, b, (u32)(c % 2));
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling on; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling on

#pragma scheduling off
u32 fn_80057114(u8* a, u8* b) {
    fn_80056C54(a, b, *(u32*)(lbl_803A9768 + 0x278));
    return 0;
}

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_EXACT_80057144_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (scheduling on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma scheduling on
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
u32 fn_80057144(u8* ctx, u8* p) {
    s32 variant;
    s32 table[7][2];
    s32 shown;

    *(Tbl14*)table = *(Tbl14*)lbl_802676B4;
    variant = *(s32*)(lbl_803A9768 + 4);
    if (*(s16*)(p + 6) == table[*(s32*)lbl_803A9768][variant != 0 ? 1 : 0]) {
        shown = 1;
    } else {
        shown = 0;
    }
    winSpriteSetDisp(p, shown);
    *(s16*)(ctx + 0x84) = *(f32*)(lbl_803A9768 + 0x27c);
    *(s16*)(ctx + 0x86) = *(f32*)(lbl_803A9768 + 0x280);
    return 0;
}
#pragma pop

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_EXACT_80057270_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

/* RULE-EXCEPTION(user-approved): local peephole off / scheduling on for pcBoxGetCursorPokemon — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma scheduling on
void* pcBoxGetCursorPokemon(void) {
    extern s32 fn_80055194(u32*, s32);
    extern s32 fn_80058F08(u32*, s32);
    extern s32 fn_80056A78(void);
    extern u8 pokemonCheckValid(void*);
    extern void* heroGetStatus(void*, u32, u32);
    extern void* getPokemon__5PCBOXFScSc(void*, s32, s32);
    u32 index;
    s8* window;
    void* pokemon;

    pokemon = lbl_803A9768 +
              *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8;
    if (pokemonCheckValid(pokemon) != 0) {
        return pokemon;
    }

    window = windowSearchID(0x94);
    if (window != 0) {
        if (fn_80058F08(&index, window[0x95]) != 0) {
            return 0;
        }
        pokemon = heroGetStatus(0, 3, (u16)index);
        if (pokemon == 0) {
            return 0;
        }
        if (pokemonCheckValid(pokemon) != 0) {
            return pokemon;
        }
        return 0;
    }

    window = windowSearchID(0x93);
    if (window == 0) {
        return 0;
    }
    if (fn_80055194(&index, window[0x95]) != 0) {
        return 0;
    }
    pokemon = getPokemon__5PCBOXFScSc(0, (s8)fn_80056A78(), (s8)index);
    if (pokemon == 0) {
        return 0;
    }
    if (pokemonCheckValid(pokemon) != 0) {
        return pokemon;
    }
    return 0;
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

u32 fn_800573C0(void) {
    s32 state;

    if (*(f32*)(lbl_803A9768 + 0x288) <= 0.0f) {
        state = *(s32*)lbl_803A9768;
        if (state == 0 || state == 3) {
            return 0;
        }
    }
    return 1;
}

void fn_80057400(void) {
    s32 value;

    value = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1);
    *(u32*)(lbl_803A9768 + 0x278) = value;
    if (value > 1) {
        *(u32*)(lbl_803A9768 + 0x278) = 0;
    }
}

u32 fn_80057428(void) {
    return !(lbl_8047A588 >= 0.8f);
}

void fn_8005744C(void) {
    lbl_8047A588 = 0.0f;
}

void fn_80057458(u8* src) {
    s32 next = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1) % 2;
    Tbl78* dstState = (Tbl78*)(lbl_803A9768 + next * 0x138 + 8);
    Tbl78* srcState = (Tbl78*)src;

    *dstState = *srcState;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
#pragma optimization_level 4
void fn_800574A8(void) {
    pokemonInit((u8*)(lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8));
}
#pragma pop

u8* fn_800574E0(void) {
    return lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8;
}

void fn_800574FC(u8* src) {
    Tbl78* dstState;
    Tbl78* srcState;
    u32 slot;

    slot = *(u32*)(lbl_803A9768 + 0x278);
    dstState = (Tbl78*)(lbl_803A9768 + slot * 0x138 + 8);
    srcState = (Tbl78*)src;
    *dstState = *srcState;
}

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_EXACT_80057538_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
u32 fn_80057538(void) {
    s32 state;
    s32 variant;
    u32 table[7][2];

    state = *(s32*)lbl_803A9768;
    if (state != 6) {
        if (*(s32*)(lbl_80267698 + state * 4) <= 0) {
            return 1;
        }
        *(Tbl14*)table = *(Tbl14*)lbl_802676B4;
        variant = *(s32*)(lbl_803A9768 + 4);
        if (fn_80107170(0xa0, (u16)table[state][variant != 0 ? 1 : 0]) != 0) {
            return 0;
        }
        return 1;
    }
    if (fn_80107170(0xa0, 0x76e) != 0) {
        return 0;
    }
    return fn_80107170(0xa0, 0x77e) == 0;
}
#pragma pop

#endif

#if defined(MENUCB_RANGE_80055E38_ALL) || \
    defined(MENUCB_SUFFIX_80056B74_ONLY) || \
    defined(MENUCB_EXACT_80057694_ONLY) || \
    defined(MENUCB_TU_80056C54_ONLY)

u32 fn_80057694(void) {
    return *(u32*)(lbl_803A9768 + 4);
}

void fn_800576A4(u32 a) {
    *(u32*)(lbl_803A9768 + 4) = a;
}

u32 fn_800576B4(void) {
    return *(u32*)(lbl_803A9768 + 0);
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_800576C4(s32 state) {
    extern void* windowSearchID(s32);
    s32 count;
    void* object;
    u32 table[7][2];

    *(s32*)lbl_803A9768 = state;
    object = windowSearchID(0xa0);
    count = *(s32*)(lbl_80267698 + state * 4);
    if (count > 0 && object != 0) {
        s32 variant;
        *(Tbl14*)table = *(Tbl14*)lbl_802676B4;
        variant = *(s32*)(lbl_803A9768 + 4);
        fn_801081F8(object,
                    (u16)table[*(s32*)lbl_803A9768][variant != 0 ? 1 : 0],
                    (u16)count);
    }
    if (state == 2 || state == 4) {
        if (count > 0) {
            fn_801081F8(object, 0x76e, (u16)count);
        }
    }
    if (state == 6) {
        fn_801081F8(object, 0x76e, 0xe5);
        fn_801081F8(object, 0x77e, 0xec);
    }
}
#pragma pop

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80057830(s32 x, s32 y, s32 reset) {
    if (reset != 0) {
        *(f32*)(lbl_803A9768 + 0x27c) = (f32)x;
        *(f32*)(lbl_803A9768 + 0x280) = (f32)y;
        *(f32*)(lbl_803A9768 + 0x294) = (f32)x;
        *(f32*)(lbl_803A9768 + 0x298) = (f32)y;
        *(f32*)(lbl_803A9768 + 0x28c) = (f32)x;
        *(f32*)(lbl_803A9768 + 0x290) = (f32)y;
        *(f32*)(lbl_803A9768 + 0x284) = 1.0f;
        *(f32*)(lbl_803A9768 + 0x288) = 0.0f;
    } else {
        *(f32*)(lbl_803A9768 + 0x28c) =
            *(f32*)(lbl_803A9768 + 0x27c);
        *(f32*)(lbl_803A9768 + 0x290) =
            *(f32*)(lbl_803A9768 + 0x280);
        *(f32*)(lbl_803A9768 + 0x294) = (f32)x;
        *(f32*)(lbl_803A9768 + 0x298) = (f32)y;
        *(f32*)(lbl_803A9768 + 0x284) = 0.0f;
        *(f32*)(lbl_803A9768 + 0x288) = 0.16666667f;
    }
}
#pragma pop

void fn_80057948(void) {
    extern f32 lbl_8047A58C;

    if (*(f32*)(lbl_803A9768 + 0x288) > 0.0f) {
        *(f32*)(lbl_803A9768 + 0x284) += *(f32*)(lbl_803A9768 + 0x288);
        if (*(f32*)(lbl_803A9768 + 0x284) >= 1.0f) {
            *(f32*)(lbl_803A9768 + 0x284) = 1.0f;
            *(f32*)(lbl_803A9768 + 0x288) = 0.0f;
        }
        *(f32*)(lbl_803A9768 + 0x27c) =
            *(f32*)(lbl_803A9768 + 0x284) *
                (*(f32*)(lbl_803A9768 + 0x294) - *(f32*)(lbl_803A9768 + 0x28c)) +
            *(f32*)(lbl_803A9768 + 0x28c);
        *(f32*)(lbl_803A9768 + 0x280) =
            *(f32*)(lbl_803A9768 + 0x284) *
                (*(f32*)(lbl_803A9768 + 0x298) - *(f32*)(lbl_803A9768 + 0x290)) +
            *(f32*)(lbl_803A9768 + 0x290);
    }
    lbl_8047A58C += 1.0f / 60.0f;
    if (lbl_8047A58C > 1.0f) {
        lbl_8047A58C -= 1.0f;
    }
    if (lbl_8047A588 < 1.0f) {
        lbl_8047A588 += 1.0f / 60.0f;
        if (lbl_8047A588 > 1.0f) {
            lbl_8047A588 = 1.0f;
        }
    }
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80057A08(void) {
    return windowSearchID(0xa0) != 0;
}

void fn_80057A38(void) {
    menuCloseCustom(0xa0, 2, 1);
}
#pragma pop

/* The slot reset is an inlined helper: its loop counter is an inline local,
 * which is what puts the counter in r31 ahead of the slot pointer. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_80057A64) — see docs/RULE_EXCEPTIONS.md */
static inline void fn_80057A64_initSlots(u32 b) {
    s32 i;

    *(u32*)(lbl_803A9768 + 0x278) = i = 0;
    *(u32*)(lbl_803A9768 + 4) = b;
    for (; i < 2; i++) {
        pokemonInit(lbl_803A9768 + i * 0x138 + 8);
    }
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_80057A64(u8* state, u32 b) {
    extern f32 lbl_8047A58C;
    extern void menuOpenCustom(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);

    fn_80057A64_initSlots(b);
    if (state != 0) {
        *(u32*)lbl_803A9768 = 3;
        *(Tbl78*)(lbl_803A9768 + 8) = *(Tbl78*)state;
    }
    lbl_8047A58C = 0.0f;
    lbl_8047A588 = 1.0f;
    menuOpenCustom(0xa0, 0x1f, 0, 0, 0, 0);
}

#endif
