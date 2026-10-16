/**
 * @file menuCB_range_80062948.c
 * @brief GBA/controller battle-entry + pokecoupon screens, 0x80062948 - 0x80069A60.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. Nearest XD analog: menuCB_PokemonEntry.cpp
 * universe (identity PARTIAL/SPECULATIVE, Colosseum version much larger).
 * Coherent family: shared static bss 0x803A9F08 spans >20 fns; callees
 * toolentryTaisen*, gbaCommandEntryPokemon, gbaCommandSendWazaText,
 * heroBiosGet/SetPokecoupon(All).
 *
 * fn_80065A48 (below) is reintroduced from the previous campaign's
 * ui_core.c (archive/previous_campaign/src/game/ui/ui_core.c, commits
 * 745775c5 and 9f9727ef) through the current dtk-template pipeline: ported
 * into this unit's split and re-verified against this unit's own compiler
 * flags (GC/1.3, -use_lmw_stmw on, -sdata 8, -sdata2 8), not copied
 * wholesale. Residual head and tail ranges remain target-linked candidates.
 */
#include "dolphin/types.h"

/* ===== External function declarations (fn_80065A48 only) ===== */
extern void fn_8010B9E8();
extern s32  toolentryTaisenGetPokemonNum();
extern s32  toolentryTaisenGetHomePlace();
extern s32  toolentryTaisenGetBattleType();
extern s32  fn_8006B1D4();
extern void fn_80068794();
extern void fn_800688C4();
extern void fn_800689FC();
extern void fn_80068BB0();
extern void fn_80068DBC();
extern void fn_8010B01C();
#if defined(MENUCB_RANGE_RESIDUAL_800697F4_ONLY)
extern void* _menuCBPokemonEntryLoadCallBack__FPv(void*);
#else
extern void _menuCBPokemonEntryLoadCallBack__FPv();
#endif
extern u16 toolentryTaisenGetBattlePlayerID(s32);
extern u8 lbl_802ED9F0[];

/* ===== Rodata / data labels ===== */
#if defined(MENUCB_RANGE_RESIDUAL_800697F4_ONLY)
typedef struct PokemonEntryTexWork {
    u8 active;
    u8 _1[3];
    u32 value;
    u8 _8[4];
} PokemonEntryTexWork;

typedef struct PokemonEntryTexWorkAll {
    u8 _0[0x2C];
    s32 index;
    PokemonEntryTexWork entries[4][6];
    u8 _150[0xCC34];
    u8 complete;
} PokemonEntryTexWorkAll;

extern PokemonEntryTexWorkAll lbl_803A9F08;
#else
extern u8 lbl_803A9F08[];
#endif

typedef struct UICmdMsg {
    u8 _0[4];
    s8 flags4;  /* 0x4 */
    u8 _5;
    s16 cmd;    /* 0x6 */
    u8 _8[0x48];
    s16 s50;
    s16 s52;
    s16 s54;
    s16 s56;
    u8 _58[0xF];
    u8 alpha67;
} UICmdMsg;

#if !defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY) && \
    !defined(MENUCB_RANGE_800643D4_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80063D10_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_800638F4_ONLY) && \
    !defined(MENUCB_RANGE_HEAD_SUFFIX_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80064378_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80065A48_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80068738_ONLY) && \
    !defined(MENUCB_RANGE_RESIDUAL_80068794_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80069048_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_800697C4_ONLY) && \
    !defined(MENUCB_RANGE_RESIDUAL_800697F4_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80069A08_ONLY)
#define MENUCB_RANGE_HEAD_ONLY
#endif

#if defined(MENUCB_RANGE_800643D4_ONLY)
extern u8 lbl_802ED9FC[];
extern u8 lbl_802EDB40[];
extern u8 lbl_802EF0A8[];

/* Reintroduced from the previous campaign's ui_core.c through this range's
 * active dtk split. Retail preserves the mask-building operations emitted
 * with peephole optimization disabled. */
#pragma push
#pragma peephole off
void fn_800643D4(u8* ctx, UICmdMsg* msg)
{
    s32 h;
    s32 fl;
    s32 idx;

    switch (msg->cmd) {
    case 0xB38: {
        void* q;
        u32 t;
        u32 snd;
        s32 mask = -0x100;
        q = (void*) toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        t = pokemonBiosGetNicknamePtr();
        if (t == 0) {
            t = GSmsgGetGSchar(1);
        }
        msgctrlSetValue(0x37, t);
        fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xe7);
        switch ((u8) menuSubGetPokemonSexForDisp(q)) {
        case 0:
            snd = 0xd67;
            break;
        case 1:
            snd = 0xd68;
            break;
        case 2:
        default:
            snd = 0;
            break;
        }
        if (snd != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(snd));
            fn_800FB680(0x5a, 0, (ctx[0x8b] | mask), 0xcf);
        }
        break;
    }
    case 0xB39: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        if ((u32) h != 0) {
            msgctrlSetValue(0x34, (u8) pokemonGetStatus(h, 0, 0x7a, 0));
            fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xd3);
        }
        break;
    }
    case 0xB3A: {
        s32 v;
        s32 mask = -0x100;
        toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        v = pokemonBiosGetHp();
        msgctrlSetValue(0x34, (u16) v);
        fn_800FB680(0, 0, (ctx[0x8b] | mask), 0xd3);
        break;
    }
    case 0xB2C: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_0;
            }
        } else if (cnt < 0x10000) {
        set_zero_0:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = wazaGetStatus(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2D: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_1;
            }
        } else if (cnt < 0x10000) {
        set_zero_1:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = wazaGetStatus(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2E: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_2;
            }
        } else if (cnt < 0x10000) {
        set_zero_2:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = wazaGetStatus(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB2F: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_3;
            }
        } else if (cnt < 0x10000) {
        set_zero_3:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xffff:
            snd = 0x933;
            break;
        case 0xfffe:
            snd = 0x934;
            break;
        case 0:
            break;
        default:
            snd = wazaGetStatus(0, snd, 1, 0);
            break;
        }
        if (snd != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(snd));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xe9);
        }
        break;
    }
    case 0xB30: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_4;
            }
        } else if (cnt < 0x10000) {
        set_zero_4:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            msgctrlSetValue(0x34, pokemonGetStatus(h, 0, 0x80, 0));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB31: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_5;
            }
        } else if (cnt < 0x10000) {
        set_zero_5:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            msgctrlSetValue(0x34, pokemonGetStatus(h, 0, 0x80, 1));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB32: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_6;
            }
        } else if (cnt < 0x10000) {
        set_zero_6:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            msgctrlSetValue(0x34, pokemonGetStatus(h, 0, 0x80, 2));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB33: {
        s32 cnt;
        u32 snd;
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_7;
            }
        } else if (cnt < 0x10000) {
        set_zero_7:
            cnt = 0;
        }
        snd = (u16) cnt;
        switch (snd) {
        case 0xfffe:
        case 0:
            break;
        default:
            msgctrlSetValue(0x34, pokemonGetStatus(h, 0, 0x80, 3));
            fn_800FBB34(0, 0, msg->s54, msg->s56, (ctx[0x8b] | mask),
                        0xdf);
            break;
        }
        break;
    }
    case 0xB34: {
        s32 cnt;
        u32 v;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 0) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 0) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_8;
            }
        } else if (cnt < 0x10000) {
        set_zero_8:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            s32 w;
            wazaDataBiosGetPtr((u16) v);
            w = (u8) wazaDataBiosGetZokuseiDataId();
            switch (w) {
            case 0xfffe:
                break;
            default:
                windowDrawSprite(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
                break;
            }
        }
        break;
    }
    case 0xB35: {
        s32 cnt;
        u32 v;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 1) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 1) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_9;
            }
        } else if (cnt < 0x10000) {
        set_zero_9:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            s32 w;
            wazaDataBiosGetPtr((u16) v);
            w = (u8) wazaDataBiosGetZokuseiDataId();
            switch (w) {
            case 0xfffe:
                break;
            default:
                windowDrawSprite(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
                break;
            }
        }
        break;
    }
    case 0xB36: {
        s32 cnt;
        u32 v;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 2) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 2) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_10;
            }
        } else if (cnt < 0x10000) {
        set_zero_10:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            s32 w;
            wazaDataBiosGetPtr((u16) v);
            w = (u8) wazaDataBiosGetZokuseiDataId();
            switch (w) {
            case 0xfffe:
                break;
            default:
                windowDrawSprite(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
                break;
            }
        }
        break;
    }
    case 0xB37: {
        s32 cnt;
        u32 v;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        cnt = pokemonGetStatus(h, 0, 0x7f, 3) & 0xffff;
        if ((u8) pokemonWazaCheckValid(h, 3) == 0) {
            cnt = 0;
        } else if (cnt < 0xfffe) {
            if (cnt == 0) {
                goto set_zero_11;
            }
        } else if (cnt < 0x10000) {
        set_zero_11:
            cnt = 0;
        }
        v = (u16) cnt;
        if (v == 0xffff) {
            v = 0xa5;
        }
        if (v != 0) {
            s32 w;
            wazaDataBiosGetPtr((u16) v);
            w = (u8) wazaDataBiosGetZokuseiDataId();
            switch (w) {
            case 0xfffe:
                break;
            default:
                windowDrawSprite(0, 0, ctx, ((u16*) lbl_802EDB40)[w], 0);
                break;
            }
        }
        break;
    }
    case 0xB1F:
        windowDrawSprite(
            0, 0, ctx,
            ((u16*) lbl_802ED9FC)[(u16) pokemonGetStatus(
                0,
                (u16) pokemonGetStatus(toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]), 0,
                                  0x6e, 0),
                0x16, 0)],
            0);
        break;
    case 0xB20: {
        u32 v1;
        u32 v2;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        v1 = (u16) pokemonGetStatus(0, (u16) pokemonGetStatus(h, 0, 0x6e, 0), 0x16, 0);
        v2 = (u16) pokemonGetStatus(0, (u16) pokemonGetStatus(h, 0, 0x6e, 0), 0x16, 1);
        if (v1 != v2) {
            windowDrawSprite(0, 0, ctx, ((u16*) lbl_802ED9FC)[v2], 0);
        }
        break;
    }
    case 0xB21: {
        s32 mask = -0x100;
        u32 byte;
        s32 cnt;
        u32 v;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        byte = ctx[0x8b];
        fl = byte | mask;
        v = (u16) pokemonGetSoubiItemDataId(h);
        if (v != 0) {
            msgctrlSetValue(0x2d, v);
            fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0x30da);
        }
        break;
    }
    case 0xB27: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        msgctrlSetValue(0x34, (s16) pokemonGetStatus(h, 0, 0x88, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB28: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        msgctrlSetValue(0x34, (s16) pokemonGetStatus(h, 0, 0x89, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB29: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        msgctrlSetValue(0x34, (s16) pokemonGetStatus(h, 0, 0x8a, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB2A: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        msgctrlSetValue(0x34, (s16) pokemonGetStatus(h, 0, 0x8b, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB2B: {
        s32 mask = -0x100;
        h = toolentryTaisenGetPokemonPtr(0, ((u32*) lbl_803A9F08)[3]);
        fl = ctx[0x8b] | mask;
        msgctrlSetValue(0x34, (s16) pokemonGetStatus(h, 0, 0x8c, 0));
        fn_800FBB34(0, 0, msg->s54, msg->s56, fl, 0xdf);
        break;
    }
    case 0xB1E: {
        u8* p;
        idx = ((u32*) lbl_803A9F08)[3];
        toolentryTaisenGetBattleType();
        p = (u8*) lbl_803A9F08 + (idx * 0xc + 0x30);
        if (p[0] != 0) {
            fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
        }
        break;
    }
    case 0xE32: {
        idx = ((u32*) lbl_803A9F08)[3];
        toolentryTaisenGetBattleType();
        if ((u16) pokemonGetSoubiItemDataId(toolentryTaisenGetPokemonPtr(0, idx)) != 0) {
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
        break;
    }
    case 0x1097:
    case 0x1098:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0x1099:
    case 0x109A:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0x109B:
        fn_80060EF4(ctx, msg, -1);
        break;
    case 0x109C:
    case 0x109D:
        fn_80060EF4(ctx, msg, 3);
        break;
    case 0x109E:
    case 0x109F:
        fn_80060EF4(ctx, msg, 4);
        break;
    case 0x10A0:
    case 0x10A1:
        fn_80060EF4(ctx, msg, 2);
        break;
    case 0x10A2:
    case 0x10A3:
        fn_80060EF4(ctx, msg, 1);
        break;
    case 0x10A4:
        fn_80060EF4(ctx, msg, 0);
        break;
    case 0x10A5: {
        u8* p = lbl_802EF0A8 + 0x20000;
        u32 t;
        s32 mask = -0x100;
        if (fn_8025DAD0() == 0) {
            t = GSmsgGetGSchar(0x3db4);
        } else {
            msgctrlSetValue(0x2f, fn_8006B1D4());
            t = GSmsgGetGSchar(0x3c1e);
        }
        msgctrlSetValue(0x37, t);
        fn_800FBB34(*(s16*) (p - 0x2df2) - msg->s50 - 0x12,
                    *(s16*) (p - 0x2df0) - msg->s52, *(s16*) (p - 0x2dee),
                    *(s16*) (p - 0x2dec), (ctx[0x8b] | mask), 0xcf);
        break;
    }
    }
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY)
void fn_80065A48(void*, UICmdMsg*, s32);
extern s32 fn_8006B1D4(void);
extern s32 toolentryTaisenGetPokemonNum(s32);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);
extern s32 fn_8025D9CC(void);
extern s32 fn_800D37CC(void);
extern s32 GSgetFrameStep(void);
extern f32 lbl_8047BFE8;
extern f32 lbl_8047BFF8;
extern f32 lbl_8047BFFC;
extern f32 lbl_8047C000;
extern f32 lbl_8047C004;
extern f32 lbl_8047C010;
extern f64 atan2(f64, f64);
extern f32 lbl_8047C014;
extern f32 lbl_8047C018;
#endif

#if defined(MENUCB_RANGE_HEAD_ONLY) || \
    defined(MENUCB_RANGE_EXACT_800638F4_ONLY) || \
    defined(MENUCB_RANGE_HEAD_SUFFIX_ONLY)
extern u8 fn_8006B1F4(s32, s32);
extern void fn_8006B2A4(s32, s32);
extern u8 fn_8006B3C8(s32);
extern void fn_8006B354(s32);
extern s32 fn_8025DAAC(void);
extern void fn_800FB680(s32, s32, s32, u32);
extern void fn_80063AD4(u8*, UICmdMsg*);
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void fn_800D5648(f32);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(void*);
extern void gfxBegin(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5BA0(s32, u32);
extern void gfxEnd(void);
extern void fn_800FE38C(s32, s32, s32, s32);
extern void fn_800FE35C(void);
extern u8 lbl_80314E08[];
extern u32 lbl_8047BFC8;
extern u32 lbl_8047BFCC;
extern f32 lbl_8047BFD0;
extern f32 lbl_8047BFD4;

#if defined(MENUCB_RANGE_HEAD_ONLY)
typedef struct MenuCBBattleEntryContext {
    s32 field_00;
    s32 mode;
} MenuCBBattleEntryContext;

extern void menuSetEnablePort(s32 port);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 toolentryTaisenGetControlerType(s32 player);
extern u8 fn_8008ABA0(s32 controller);
extern void _threadSwitch(void);
extern s32 windowGetActiveID(void);
extern s32 menuOpenCustom(s32 menuId, s32 owner, s32 arg2, s32 arg3, s32 arg4, s32 arg5, ...);
extern void menuCloseCustom(s32, s32, s32);
extern u8 fn_800F7EF8(s32);
extern s32 fn_800F7C28(s32);
extern void toolentryCopyHero(void);
extern s32 fn_8025D9A8(void);
extern u16 std103_battleGetResult(void);
extern s32 menuOpen(s32, s32);
extern void msgctrlSetValue(s32 index, s32 value);
extern void winMsgOpen(s32 type, s32 message, s32 a, s32 b);
extern void winMsgClose(s32 window);
extern s32 lbl_8047A5D0;

s32 fn_80062948(MenuCBBattleEntryContext* context)
{
    extern void menuCBBattleStartInit(void*, s32);
    extern s32 fn_8025D9A8(void);
    extern s32 fn_80063060(void*);
    extern s32 fn_80062AB4(void*);
    extern s32 menuOpen(s32, s32);
    extern void menuCloseCustom(s32, s32, s32);
    extern void winSeqSetMenu(s32, s32);
    extern u8 winSeqCheckMove(s32);
    extern void toolentryCopyHero(void);
    extern void menuCBPokemonEntryTexWorkInit(void);
    extern void menuCBBattleStartTrainerFaceFree(void);
    extern void fn_80061028(s32);
    s32 result;
    s32 battleMode;

    menuCBBattleStartInit(context, 1);
    battleMode = fn_8025D9A8();
    toolentryTaisenGetBattleType();

    switch (battleMode) {
    case 0:
    case 1:
        result = fn_80063060(context);
        break;
    case 3:
        menuOpen(0xDF, 0);
        menuOpen(0xBA, 1);
        result = menuOpen(0x106, 1);
        if (context->mode != 2 && result > 0) {
            result++;
        }
        switch (result) {
        case 0:
            toolentryCopyHero();
            result = 0xD1;
            break;
        case -1:
        default:
            result = -1;
            break;
        }
        menuCloseCustom(0x106, 0, 1);
        break;
    default:
        result = fn_80062AB4(context);
        break;
    }

    winSeqSetMenu(0xDF, 0x1C6);
    winSeqSetMenu(0xBA, 0x1C6);
    while (winSeqCheckMove(0xDF)) {
        _threadSwitch();
    }
    while (winSeqCheckMove(0xBA)) {
        _threadSwitch();
    }

    menuCBPokemonEntryTexWorkInit();
    menuCBBattleStartTrainerFaceFree();
    fn_80061028(1);
    menuCloseCustom(0xDF, 0, 1);
    return result;
}

static inline s32 fn_80062AB4_FindController(void)
{
    s32 count;
    s32 controller;
    s32 player;
    s32 home;

    count = toolentryTaisenGetEntryPlayerNum();
    fn_8025D9A8();
    for (player = 0; player < count; player++) {
        home = toolentryTaisenGetHomePlace(player);
        controller = toolentryTaisenGetControlerType(player);
        if (controller != 0 && ((u16)home == 1 || (u16)home == 2)) {
            if (!fn_8008ABA0(controller)) {
                return controller;
            }
        }
    }
    return 2;
}

static inline u8 fn_80062AB4_AllReleased(void)
{
    s32 player;
    s32 home;
    s32 controller;

    for (player = 0; player < 4; player++) {
        home = toolentryTaisenGetHomePlace(player);
        controller = toolentryTaisenGetControlerType(player);
        if (controller != 0 && ((u16)home == 1 || (u16)home == 2)) {
            if (fn_8008ABA0(controller)) {
                return 0;
            }
        }
    }
    return 1;
}

static inline u8 fn_80062AB4_Decided(void)
{
    if (fn_800F7EF8(1)) {
        if (fn_800F7C28(1) == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

static inline void fn_80062AB4_WaitDecided(void)
{
    s32 waiting;

    waiting = 1;
    do {
        if (fn_80062AB4_Decided()) {
            waiting = 0;
        } else {
            _threadSwitch();
        }
    } while (waiting != 0);
}

static inline u16 fn_80062AB4_IsHomeBattle(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        if ((s32)(u16)toolentryTaisenGetHomePlace(0) != 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

s32 fn_80062AB4(MenuCBBattleEntryContext* context)
{
    u16 status;
    u8 abort;
    u16 menuId;
    s32 choice;
    s32 result;
    s32 state;
    s32 keepRunning;
    s32 homeBattle;
    s32 controller;
    s32 waiting;

    status = std103_battleGetResult();
    abort = 0;
    menuSetEnablePort(0);
    menuOpen(0xDF, 0);
    menuOpen(0xBA, 1);

    switch (status) {
    case 1:
        menuSetEnablePort(1);
        if (fn_80062AB4_IsHomeBattle() == 0) {
            controller = fn_80062AB4_FindController();
            msgctrlSetValue(0x30, controller);
            winMsgOpen(2, 0x44DC, 1, 1);
            winMsgClose(1);
            abort = 1;
        } else {
            fn_80062AB4_FindController();
            winMsgOpen(2, 0x44E7, 1, 1);
            fn_80062AB4_WaitDecided();
            winMsgClose(1);
            abort = 1;
        }
        break;
    }

    if (abort) {
        return 0xB3;
    }

    if (context->mode != 2) {
        menuId = 0xD4;
    } else {
        menuId = 0xD5;
    }

    menuSetEnablePort(1);
    winMsgOpen(2, 0x3C20, 1, 1);
    choice = menuOpenCustom(menuId, windowGetActiveID(), 0, 8, 1, 0);
    if (context->mode != 2 && choice > 0) {
        choice++;
    }

    switch (choice) {
    case 0:
        toolentryCopyHero();
        result = 0xD1;
        break;
    case 1:
        result = 0xB5;
        break;
    case 2:
        result = 0xB3;
        break;
    case -1:
    default:
        result = -1;
        break;
    }

    winMsgClose(1);
    if (result == -1 || result == 0xB3) {
        state = 0;
        keepRunning = 1;
        homeBattle = fn_80062AB4_IsHomeBattle();
        do {
            switch (state) {
            case 0:
                if (homeBattle == 0) {
                    state = 1;
                } else {
                    state = 2;
                }
                break;
            case 1:
                menuCloseCustom(menuId, 0, 1);
                winMsgOpen(2, 0x4446, 1, 1);
                waiting = 1;
                do {
                    if (fn_80062AB4_AllReleased()) {
                        waiting = 0;
                    }
                    if (waiting != 0) {
                        _threadSwitch();
                    }
                } while (waiting != 0);
                winMsgClose(1);
                state = 4;
                break;
            case 2:
                menuCloseCustom(menuId, 0, 1);
                winMsgOpen(2, 0x4445, 1, 1);
                waiting = 1;
                do {
                    if (fn_80062AB4_AllReleased()) {
                        waiting = 0;
                    }
                    if (waiting != 0) {
                        _threadSwitch();
                    }
                } while (waiting != 0);
                winMsgClose(1);
                state = 3;
                break;
            case 3:
                menuCloseCustom(menuId, 0, 1);
                winMsgOpen(2, 0x44E2, 1, 1);
                fn_80062AB4_WaitDecided();
                winMsgClose(1);
                state = 4;
                break;
            case 4:
                keepRunning = 0;
                break;
            }
        } while (keepRunning != 0);
    }

    lbl_8047A5D0 = 0;
    return result;
}

s32 fn_80063060(MenuCBBattleEntryContext* context)
{
    extern void fn_8006A7D0(void);
    extern u16 fn_8006AC6C(void);
    extern void fn_8006ADB4(s32);
    extern u32 fn_8006ADEC(void);
    extern void fn_8006B09C(s32);
    extern s32 fn_800886D0(void);
    extern s32 fn_800889E4(s32);
    extern s32 fn_80088D84(void);
    extern s8 menuSubOpenYesNo(s32, s16, s16, s32);
    extern void menuCBPokemonEntryTexWorkInit(void);
    extern u8 fn_80062284(s32);
    extern void menuCBBattleStartTrainerFaceFree(void);
    extern void fn_800637B0(void);
    extern s8 windowGetValue(s32);
    extern void menuSetPosition(s32, s16, s16);
    extern void windowCheckCursor(s32, s32);
    extern void winSeqSetMenu(s32, s32);
    extern u8 winSeqCheckMove(s32);
    extern s32 savedataGetStatus(s32, s32);
    extern s32 heroBiosGetPokecouponAll(s32);
    extern void heroBiosSetPokecouponAll(s32, s32);
    extern s32 heroBiosGetPokecoupon(void);
    extern void heroBiosSetPokecoupon(s32, s32);
    extern void soundStartRaw(s32, s32, s32);
    extern u32 std082_flagGetValue(s32);
    extern u8 fn_801EE398(void);
    extern void fn_8025D06C(void);
    extern s32 fn_8025D164(void);
    extern void fn_8025DAF4(void);
    extern void fn_8025DB2C(void);
    extern s32 fn_8025DB5C(void);
    extern void fn_8025DB80(void);
    extern s32 fn_8025DBB0(void);
    extern void fn_80061028(s32);
    extern void fn_80069C0C(void*);
    extern s16 lbl_80478920;
    extern s16 lbl_80478922;
    s32 state;
    u16 battleType;
    s32 result;
    s32 keepRunning;
    s32 battleMode;
    s32 battleCount;
    u8 pendingSetup;
    u8 openedCustomMenu;
    u8 usedCancelRoute;
    u8 allowDbCleanup;
    u8 allowRestore;
    u8 pendingPostCopy;
    s32 saveStatus;
    s32 coupon;
    s32 onesDigit;
    s32 couponAll;
    s32 answer;

    state = 0;
    battleType = std103_battleGetResult();
    result = -1;
    keepRunning = 1;
    battleMode = fn_8025D9A8();
    battleCount = fn_8025DBB0();
    pendingSetup = 0;
    openedCustomMenu = 0;
    usedCancelRoute = 0;
    allowDbCleanup = 1;
    allowRestore = 1;
    pendingPostCopy = 0;
    saveStatus = savedataGetStatus(0, 2);
    coupon = heroBiosGetPokecoupon();
    couponAll = heroBiosGetPokecouponAll(saveStatus);
    onesDigit = (battleCount + 1) % 10;

    do {
        switch (state) {
        case 0:
            menuOpen(0xDF, 0);
            menuOpen(0xBA, 1);
            switch (battleType) {
            case 2:
            case 5:
                switch (battleMode) {
                case 0:
                    if (battleCount == 7) {
                        fn_8006ADB4(fn_8025D164());
                        pendingSetup = 1;
                        fn_800637B0();
                    }
                    break;
                case 1:
                    if (onesDigit == 0) {
                        s32 base = fn_8025D164();
                        fn_8006ADB4(base + fn_8006ADEC());
                    }
                    if (battleCount + 1 == 100) {
                        pendingSetup = 1;
                    }
                    break;
                }
                if (pendingSetup) {
                    if (battleMode == 1 && std082_flagGetValue(0xAFD) == 0) {
                        pendingPostCopy = fn_801EE398();
                    }
                    state = 5;
                } else if (fn_80062284(0)) {
                    winMsgOpen(2, 0x3C10, 1, 1);
                    winMsgClose(1);
                    fn_8025DB2C();
                    winMsgOpen(2, 0x30DD, 0, 1);
                    state = 1;
                } else {
                    winMsgOpen(2, 0x30DD, 0, 1);
                    state = 1;
                }
                break;
            case 3:
            case 4:
            case 6:
            case 7:
                if (fn_8025DB5C() == 0) {
                    if (battleMode == 1) {
                        result = 0x105;
                    } else {
                        result = 0xAC;
                    }
                    state = 9;
                } else {
                    state = 2;
                }
                break;
            default:
                winMsgOpen(2, 0x3DA4, 0, 1);
                state = 9;
                break;
            }
            break;

        case 1:
            menuOpenCustom(0xEC, windowGetActiveID(), 0, 8, 0, 0);
            menuSetPosition(0xEC, lbl_80478920, lbl_80478922);
            windowCheckCursor(0xEC, 1);
            answer = windowGetValue(0xEC);
            menuCloseCustom(0xEC, 0, 1);
            winMsgClose(1);
            if (answer == 0) {
                openedCustomMenu = 1;
                if (allowDbCleanup) {
                    fn_8025DB80();
                }
                result = 0xD1;
                usedCancelRoute = 0;
                state = 9;
            } else {
                openedCustomMenu = 1;
                if (allowDbCleanup) {
                    fn_8025DB80();
                }
                state = 9;
                usedCancelRoute = 1;
                result = 0xAC;
            }
            break;

        case 4:
            winMsgOpen(2, 0x44E3, 0, 1);
            answer = menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 1);
            winMsgClose(1);
            if (answer == 0) {
                state = 12;
            } else {
                state = 1;
            }
            break;

        case 2:
            msgctrlSetValue(0x30, fn_8025DB5C());
            winMsgOpen(2, 0x3C13, 0, 1);
            answer = menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 0);
            winMsgClose(1);
            if (answer == 0) {
                fn_8025DAF4();
                state = 9;
                result = 0xD1;
            } else {
                state = 3;
            }
            break;

        case 3:
            msgctrlSetValue(0x30, fn_8025DB5C());
            winMsgOpen(2, 0x44DF, 0, 1);
            answer = menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 1);
            winMsgClose(1);
            if (answer == 0) {
                state = 9;
                result = 0xAC;
            } else {
                state = 2;
            }
            break;

        case 5:
            {
                s32 n = fn_8006ADEC();
                msgctrlSetValue(0x30, n);
            }
            soundStartRaw(0x3CC, 0, 0);
            winMsgOpen(2, 0x3C11, 1, 1);
            fn_8006B09C(0);
            fn_8006A7D0();
            {
                s32 entries = fn_8006AC6C();

                if (pendingPostCopy) {
                    state = 8;
                } else {
                    switch (entries) {
                    case 0:
                        state = 6;
                        break;
                    case 1:
                    case 2:
                        result = 0x105;
                        state = 12;
                        break;
                    default:
                        state = 6;
                        break;
                    }
                }
            }
            break;

        case 10:
            winMsgOpen(2, 0x3C23, 0, 1);
            if (menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 0) == 0) {
                result = 0x105;
                state = 12;
            } else {
                state = 11;
            }
            break;

        case 11:
            winMsgOpen(2, 0x3C0F, 0, 1);
            if (menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 1) == 0) {
                result = 0xAC;
                state = 12;
            } else {
                state = 10;
            }
            break;

        case 6:
            winMsgOpen(2, 0x3C03, 0, 1);
            answer = menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 0);
            winMsgClose(1);
            if (answer == 0) {
                result = 0xAC;
                state = 9;
                if (allowRestore) {
                    fn_8025D06C();
                }
                usedCancelRoute = 1;
            } else {
                state = 7;
            }
            break;

        case 7:
            winMsgOpen(2, 0x3C41, 0, 1);
            if (menuSubOpenYesNo(0, lbl_80478920, lbl_80478922, 1) == 0) {
                s32 status = savedataGetStatus(0, 2);

                heroBiosSetPokecoupon(status, coupon);
                heroBiosSetPokecouponAll(status, couponAll);
                state = 12;
            } else {
                state = 6;
            }
            break;

        case 8:
            winMsgOpen(2, 0x3C12, 0, 1);
            winMsgClose(1);
            winSeqSetMenu(0xDF, 0x1C6);
            winSeqSetMenu(0xBA, 0x1C6);
            while (winSeqCheckMove(0xDF)) {
                _threadSwitch();
            }
            while (winSeqCheckMove(0xBA)) {
                _threadSwitch();
            }
            menuCBPokemonEntryTexWorkInit();
            menuCBBattleStartTrainerFaceFree();
            fn_80061028(1);
            menuCloseCustom(0xDF, 0, 1);
            fn_800886D0();
            result = 0x105;
            state = 9;
            break;

        case 9:
            if (usedCancelRoute) {
                if (openedCustomMenu) {
                    if (fn_800889E4(0) < 0) {
                        state = 4;
                        allowDbCleanup = 0;
                    } else {
                        keepRunning = 0;
                        fn_80069C0C(context);
                    }
                } else {
                    if (fn_80088D84() < 0) {
                        state = 6;
                        allowRestore = 0;
                    } else {
                        keepRunning = 0;
                        fn_80069C0C(context);
                    }
                }
            } else {
                keepRunning = 0;
                if (openedCustomMenu) {
                    fn_80069C0C(context);
                }
            }
            break;

        case 12:
            keepRunning = 0;
            break;
        }
    } while (keepRunning != 0);

    winMsgClose(1);
    return result;
}

void fn_800637B0(void)
{
    s32 player;
    u32 setting;
    s32 battleType;
    u8 ready;

    battleType = toolentryTaisenGetBattleType();
    setting = fn_8025DAAC();
    if (fn_8006B1F4(setting, battleType) == 0) {
        fn_8006B2A4(setting, battleType);
    }

    if (fn_8006B3C8(3) == 0) {
        ready = 1;
        for (player = 0; player <= 2; player++) {
            if (fn_8006B1F4(player, 0) == 0) {
                ready = 0;
                break;
            }
            if (fn_8006B1F4(player, 1) == 0) {
                ready = 0;
                break;
            }
        }
        if (ready == 1) {
            fn_8006B354(3);
        }
    }

    if (fn_8006B3C8(5) == 0) {
        ready = 1;
        if (fn_8006B1F4(4, 0) == 0) {
            ready = 0;
        } else if (fn_8006B1F4(4, 1) == 0) {
            ready = 0;
        }
        if (ready == 1) {
            fn_8006B354(5);
        }
    }
}

#endif

#if defined(MENUCB_RANGE_EXACT_800638F4_ONLY)
void fn_800638F4(u8* context, UICmdMsg* msg)
{
    s16 command = msg->cmd;

    switch (command) {
    case 0xE08:
        fn_80063AD4(context, msg);
        return;
    case 0xE17:
        fn_80063AD4(context, msg);
        return;
    case 0x1264:
        fn_80063AD4(context, msg);
        return;
    case 0xE14:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3C21U);
        return;
    case 0xE15:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DB2U);
        return;
    case 0xE16:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DB3U);
        return;
    case 0xE24:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3C21U);
        return;
    case 0xE25:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DAEU);
        return;
    case 0xE26:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DB2U);
        return;
    case 0xE27:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DB3U);
        return;
    case 0x126F:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3C21U);
        return;
    case 0x1270:
        fn_800FB680(0, 0, context[0x8B] | ~0xFF, 0x3DB3U);
        return;
    case 0x1123:
        fn_80063AD4(context, msg);
        return;
    }
}
#endif

#if defined(MENUCB_RANGE_HEAD_SUFFIX_ONLY)
typedef union MenuCBColor {
    u32 value;
    struct {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    } channel;
} MenuCBColor;

void fn_80063AD4(u8* context, UICmdMsg* msg)
{
    MenuCBColor bottom;
    MenuCBColor top;
    f32 opacity;
    s32 combinedAlpha;
    s32 y;

    top.value = lbl_8047BFC8;
    bottom.value = lbl_8047BFCC;
    combinedAlpha = context[0x8B] * msg->alpha67 / 65025;
    opacity = (f32)combinedAlpha;
    top.channel.alpha = (f32)top.channel.alpha * opacity;
    bottom.channel.alpha = (f32)bottom.channel.alpha * opacity;

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetPrimitive(6);
    gfxSetVtxDesc(lbl_80314E08);
    gfxBegin(4);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, top.value);
    fn_800D61E4(msg->s54, 0);
    fn_800D5BA0(0, top.value);
    fn_800D61E4(msg->s54, msg->s56);
    fn_800D5BA0(0, bottom.value);
    fn_800D61E4(0, msg->s56);
    fn_800D5BA0(0, bottom.value);
    gfxEnd();
    fn_800FE38C(0, 0, msg->s54, msg->s56);

    gfxSetFlags(1);
    gfxClearFlags(6);
    fn_800D5648(1.0f);
    gfxSetPrimitive(1);
    gfxSetVtxDesc(lbl_80314E08);
    top.channel.red = 0xFF;
    top.channel.green = 0xFF;
    top.channel.blue = 0xFF;
    top.channel.alpha = 56.0f * opacity;
    for (y = 0; (s16)y < msg->s56; y += 4) {
        gfxBegin(2);
        fn_800D61E4(0, y);
        fn_800D5BA0(0, top.value);
        fn_800D61E4(msg->s54, y);
        fn_800D5BA0(0, top.value);
        gfxEnd();
    }
    fn_800FE35C();
}
#endif
#endif

#if defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY)
/* The 0x80065628 and 0x80065730 wrappers compile only their own callbacks. */
#if !defined(MENUCB_RANGE_80065628_ONLY) && !defined(MENUCB_RANGE_80065730_ONLY)
/* The 0x8006905C wrapper compiles only fn_8006905C. */
/* Single-range wrappers: 0x800676EC (fn_800676EC..fn_80068418),
 * 0x8006905C, 0x80069220 and 0x800693A4 each compile only their own range. */
#if !defined(MENUCB_RANGE_8006905C_ONLY) && !defined(MENUCB_RANGE_80069220_ONLY) && \
    !defined(MENUCB_RANGE_800693A4_ONLY)
void fn_800676EC(u8* context)
{
    extern u32 fn_800F7BC4(s32);
    extern s32 toolentryTaisenGetControlerType(s32);
    extern s32 toolentryTaisenGetPokemonNum(s32);
    extern void fn_800679C0(u8*, s32);
    extern u8 fn_8006905C(void);
    extern void menuButtonNormal(u8*);
    u32 buttons;
    u32 mask;
    s32 selection;

    buttons = fn_800F7BC4(1);
    if ((buttons & 0x20) != 0) {
        if (toolentryTaisenGetControlerType(0) == 1) {
            switch (*(s32*)&lbl_803A9F08[0]) {
            case 0:
                if (lbl_803A9F08[4] == 0) {
                    u16 maxPokemon;

                    buttons = fn_800F7BC4(1);
                    selection = -1;
                    maxPokemon = toolentryTaisenGetPokemonNum(0);
                    if (buttons & 1) {
                        selection = 0;
                    }
                    if (buttons & 8) {
                        selection = 1;
                    }
                    if (buttons & 0x800) {
                        selection = 2;
                    }
                    if (buttons & 4) {
                        selection = 3;
                    }
                    if (buttons & 2) {
                        selection = 4;
                    }
                    if (buttons & 0x400) {
                        selection = 5;
                    }
                    if (maxPokemon <= selection) {
                        selection = -1;
                    }
                    if (selection >= 0 &&
                        selection < (u16)toolentryTaisenGetPokemonNum(0)) {
                        context[0x95] = 0;
                        context[0x98] = 1;
                        *(s32*)&lbl_803A9F08[0xC] = selection;
                    }
                }
                fn_800679C0(context, 1);
                break;
            case 1:
                fn_800679C0(context, 1);
                break;
            case 2:
                buttons = fn_800F7BC4(1);
                mask = 0;
                switch (*(s32*)&lbl_803A9F08[0xC]) {
                case 0: mask = 1; break;
                case 1: mask = 8; break;
                case 2: mask = 0x800; break;
                case 3: mask = 4; break;
                case 4: mask = 2; break;
                case 5: mask = 0x400; break;
                }
                if ((buttons & mask) == 0) {
                    context[0x98] = 1;
                }
                fn_800679C0(context, 1);
                break;
            case 3:
                break;
            }
        }
    } else {
        switch (*(s32*)&lbl_803A9F08[0]) {
        case 0:
            fn_800679C0(context, 0);
            break;
        case 1:
            fn_800679C0(context, 1);
            break;
        case 2:
            fn_800679C0(context, 1);
            context[0x98] = 1;
            break;
        case 3:
            break;
        }
    }

    if (fn_8006905C() != 0) {
        context[0x98] = 1;
        context[0x99] = 1;
    }
    if (lbl_803A9F08[0xCE58] == 0) {
        context[0x98] = 1;
        context[0x99] = 1;
    }
    if (*(s32*)&lbl_803A9F08[0] == 1) {
        menuButtonNormal(context);
    }
}

#if defined(MENUCB_RANGE_800676EC_ONLY)
/* fn_800679C0 sits between fn_800676EC and the GBA entry check in retail. */
void _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(s32 inputDevice, s32 player);
typedef struct MenuCBContext676EC {
    u8 pad_00[0x95];
    u8 state95;
    u8 pad_96[2];
    u8 state98;
    u8 state99;
} MenuCBContext676EC;

typedef struct MenuCBAfe4 {
    u8 pad[4];
    s32 type;
} MenuCBAfe4;

extern u8 lbl_803A9F08[];
extern u32 lbl_802EDB64[];
extern f32 lbl_8047BFE8;
extern f32 lbl_8047BFEC;
extern MenuCBAfe4* fn_8006AFE4(s32);
extern s32 fn_8006B1D4(void);
extern u8 fn_8008ABA0(s32);
extern u32 fn_800F7AF0(void);
extern u32 fn_800F7BC4(s32);
extern void soundStartRaw(u32, u32, u32);
extern s32 fn_8025D9CC(void);
extern s32 toolentryTaisenDeleteEtnryPokemonOrder(s32);
extern u16 toolentryTaisenGetBattlePlayerID(s32);
extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetControlerType(s32);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 toolentryTaisenGetHomePlace(s32);
extern s32 toolentryTaisenGetPokemonNum(s32);
extern s32 toolentryTaisenSetEtnryPokemonOrder(s32, s32);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);

static inline void menuCBSetAnimTrack(s32 player, s32 slot)
{
    f32* track = (f32*)(lbl_803A9F08 + player * 0x30 + 0xCD8C);

    track[slot] = (f32)((5 - slot) * 0x18);
    track[slot + 6] = lbl_8047BFE8;
}

static inline void menuCBEntryCheckLinkLost(int player)
{
    s32 type;
    s32 controller;

    if (fn_8025D9CC() == 4 && lbl_803A9F08[(s32)player + 4] != 0) {
        type = fn_8006AFE4(toolentryTaisenGetControlerType(player))->type;
        if (type == 1 || type == 2) {
            controller = toolentryTaisenGetControlerType(player);
            if (fn_8008ABA0(controller) == 0) {
                *(volatile u8*)&lbl_803A9F08[0xCE58] = 0;
                if (*(volatile s32*)&lbl_803A9F08[0xCE5C] < 0) {
                    *(s32*)&lbl_803A9F08[0xCE5C] = controller;
                }
            }
        }
    }
}

static inline void menuCBEntryPadInput(int player)
{
    s32 controller;
    s32 selection;
    u32 buttons;
    s32 count;
    s32 slot;
    u16 maxPokemon;

    controller = toolentryTaisenGetControlerType(player);
    buttons = fn_800F7BC4(controller) & fn_800F7AF0();

    toolentryTaisenGetBattlePlayerID(player);
    if ((buttons & 0x40) != 0) {
        count = toolentryTaisengetEtnryPokemonOrderNum(player);
        if (count != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
            soundStartRaw(0x25, 0, 0);
        }
    } else if ((buttons & 0xC0F) != 0) {
        selection = -1;
        maxPokemon = toolentryTaisenGetPokemonNum(player);

        if (buttons & 1) {
            selection = 0;
        }
        if (buttons & 8) {
            selection = 1;
        }
        if (buttons & 0x800) {
            selection = 2;
        }
        if (buttons & 4) {
            selection = 3;
        }
        if (buttons & 2) {
            selection = 4;
        }
        if (buttons & 0x400) {
            selection = 5;
        }
        if (maxPokemon <= selection) {
            selection = -1;
        }
        if (selection >= 0) {
            slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
            if (slot >= 0) {
                soundStartRaw(0x3C3, 0, 0);
                menuCBSetAnimTrack(player, slot);
            }
        }
    }
}

static inline void menuCBEntryCpuInput(int player)
{
    s32 selection;
    u32 buttons;
    s32 slot;
    u16 maxPokemon;

    toolentryTaisenGetControlerType(player);
    toolentryTaisenGetBattlePlayerID(player);
    *(f32*)&lbl_803A9F08[0xCE4C] =
        *(f32*)&lbl_803A9F08[0xCE4C] + *(f32*)&lbl_803A9F08[0xCD88];
    if (*(volatile f32*)&lbl_803A9F08[0xCE4C] >= lbl_8047BFEC) {
        *(volatile f32*)&lbl_803A9F08[0xCE4C] = lbl_8047BFE8;
        {
            s32 idx = *(volatile s32*)&lbl_803A9F08[0xCE50];
            buttons = ((volatile u32*)lbl_802EDB64)[idx];
            *(volatile s32*)&lbl_803A9F08[0xCE50] = idx + 1;
        }
    } else {
        buttons = 0;
    }
    if ((buttons & 0xC0F) != 0) {
        selection = -1;
        maxPokemon = toolentryTaisenGetPokemonNum(player);
        if (buttons & 1) {
            selection = 0;
        }
        if (buttons & 8) {
            selection = 1;
        }
        if (buttons & 0x800) {
            selection = 2;
        }
        if (buttons & 4) {
            selection = 3;
        }
        if (buttons & 2) {
            selection = 4;
        }
        if (buttons & 0x400) {
            selection = 5;
        }
        slot = toolentryTaisenSetEtnryPokemonOrder(
            player, (maxPokemon <= selection) ? -1 : selection);
        if (slot >= 0) {
            soundStartRaw(0x3C3, 0, 0);
            menuCBSetAnimTrack(player, slot);
        }
    }
}

void fn_800679C0(MenuCBContext676EC* context, s32 startPlayer)
{
    s32 player;
    s32 entryPlayers;
    s32 type;

    entryPlayers = toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();
    toolentryTaisenGetHomePlace(0);
    toolentryTaisenGetHomePlace(1);
    toolentryTaisenGetHomePlace(2);
    toolentryTaisenGetHomePlace(3);

    player = (startPlayer != 0) ? 1 : 0;

    while (player < entryPlayers) {
        menuCBEntryCheckLinkLost(player);

        if (lbl_803A9F08[(u32)player + 4] == 0) {
            if (fn_8025D9CC() == 4) {
                type = fn_8006AFE4(toolentryTaisenGetControlerType(player))->type;
                if (type == 1 || type == 2) {
                    _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                        toolentryTaisenGetControlerType(player), player);
                } else {
                    menuCBEntryPadInput(player);
                }
            } else if (player == 1) {
                menuCBEntryCpuInput(player);
            } else {
                switch ((u16)toolentryTaisenGetHomePlace(player)) {
                case 0:
                    menuCBEntryPadInput(player);
                    break;
                case 1:
                case 2:
                    _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                        toolentryTaisenGetControlerType(player), player);
                    break;
                default:
                    menuCBEntryPadInput(player);
                    break;
                }
            }

            if ((toolentryTaisenGetControlerType(player) == 1) &&
                ((u16)toolentryTaisenGetHomePlace(player) == 0)) {
                s32 orderCount = toolentryTaisengetEtnryPokemonOrderNum(player);
                u16 pokemonCount = fn_8006B1D4();
                u16 limit = toolentryTaisenGetPokemonNum(player);

                if (orderCount == (u16)((limit < pokemonCount) ? limit : pokemonCount)) {
                    s32 prev = orderCount - 1;
                    if (prev < 0) {
                        prev = 0;
                    }
                    if (lbl_8047BFE8 ==
                        *(f32*)(lbl_803A9F08 + (u32)player * 0x30 + prev * 4 + 0xCD8C)) {
                        context->state95 = 1;
                        context->state98 = 1;
                    }
                }
            }
        }

        player += 1;
    }
}
#endif /* MENUCB_RANGE_800676EC_ONLY */

void _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
    s32 inputDevice, s32 player)
{
    extern u16 toolentryTaisenGetBattlePlayerID(s32);
    extern void fn_8008A9E4(s32, u32*);
    extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);
    extern s32 toolentryTaisenSetEtnryPokemonOrder(s32, s32);
    extern s32 toolentryTaisenDeleteEtnryPokemonOrder(s32);
    extern u16 toolentryTaisenGetEntryPokemonNum(s32);
    extern void gbaCommandEntryPokemon(u32, u8*);
    extern void toolentryTaisenSetEtnryPokemonOrderGBA(
        s32, s32, u32*);
    extern void soundStartRaw(s32, s32, s32);
    u32 order[6];
    f32* track;
    u32 linkStatus;
    u32 command;
    u8 gbaOrder[8];
    s32 count;
    s32 oldCount;
    s32 result;
    int i;

    toolentryTaisenGetBattlePlayerID(player);
    fn_8008A9E4(inputDevice, &linkStatus);
    command = linkStatus & 0xFF000000;
    switch (command) {
    case 0x01000000:
        count = toolentryTaisengetEtnryPokemonOrderNum(player);
        result = toolentryTaisenSetEtnryPokemonOrder(player, count);
        if (result >= 0) {
            soundStartRaw(0x3C3, 0, 0);
            track = (f32*)(lbl_803A9F08 + player * 0x30 + 0xCD8C);
            track[result] = (f32)((5 - result) * 0x18);
            track[result + 6] = lbl_8047BFE8;
        }
        break;
    case 0xFF000000:
        oldCount = toolentryTaisengetEtnryPokemonOrderNum(player);
        if (oldCount != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
            soundStartRaw(0x25, 0, 0);
        }
        break;
    case 0:
        count = toolentryTaisenGetEntryPokemonNum(player);
        gbaCommandEntryPokemon(linkStatus, gbaOrder);
        for (i = 0; i < count; i++) {
            order[i] = gbaOrder[i];
        }
        toolentryTaisenSetEtnryPokemonOrderGBA(player, count, order);
        lbl_803A9F08[player + 4] = 1;
        break;
    case 0x03000000:
        *(volatile u8*)&lbl_803A9F08[0xCE58] = 0;
        if (*(volatile s32*)&lbl_803A9F08[0xCE5C] < 0) {
            *(s32*)&lbl_803A9F08[0xCE5C] = inputDevice;
        }
        break;
    }
}

#endif /* 0x800676EC range: fn_800676EC..EntCheckGBA */

#if !defined(MENUCB_RANGE_8006905C_ONLY)
static inline f32 menuCBPokemonEntryAbsF(f32 value)
{
    return value > 0.0f ? value : -value;
}

static inline void menuCBPokemonEntryAdvancePositions(void)
{
    f32* current;
    f32 remaining;
    f32 step;
    f32 scaled;
    s32 player;
    s32 component;
    f32 denominator;
    u32 numerator;

    denominator = (f32)fn_800D37CC();
    numerator = GSgetFrameStep();
    *(f32*)&lbl_803A9F08[0xCD88] = (f32)numerator / denominator;
    for (player = 0; player < 4; player++) {
        current = (f32*)(lbl_803A9F08 + player * 0x30 + 0xCD8C);
        for (component = 0; component < 6; component++) {
            if (current[component] != current[component + 6]) {
                step = current[component + 6] - current[component];
                scaled = lbl_8047C010 * step;
                step = scaled * *(f32*)&lbl_803A9F08[0xCD88];
                if (step > lbl_8047C010) {
                    step = lbl_8047C010;
                }
                if (step <= lbl_8047C014) {
                    step = lbl_8047C014;
                }
                current[component] += step;
                remaining = current[component + 6] - current[component];
                step = menuCBPokemonEntryAbsF(step);
                if (menuCBPokemonEntryAbsF(remaining) <= step ||
                    menuCBPokemonEntryAbsF(remaining) < lbl_8047C018) {
                    current[component] = current[component + 6];
                }
            }
        }
    }
}

#endif /* !MENUCB_RANGE_8006905C_ONLY */

#if !defined(MENUCB_RANGE_8006905C_ONLY) && !defined(MENUCB_RANGE_80069220_ONLY) && \
    !defined(MENUCB_RANGE_800693A4_ONLY)
typedef struct PokemonEntryInputRepeat {
    u16 current;
    u16 previous;
    u16 pressed;
    u16 repeated;
    u8 pad_08[2];
    s8 timer[16];
} PokemonEntryInputRepeat;

void fn_80068418(PokemonEntryInputRepeat* input, int device)
{
    extern s8 fn_800F7A08(s32, s32);
    extern s8 fn_800F7A7C(s32, s32);
    extern u32 fn_800F7BC4(s32);
    s32 bit;
    f32 angle;
    u16 current;
    u16 pressed;
    u16 repeated;
    s8 horizontal;
    s8 vertical;
    u32 buttons;

    input->previous = input->current;
    current = 0;
    horizontal = fn_800F7A08(device, 0);
    vertical = fn_800F7A7C(device, 0);

    if ((vertical < 0 ? -vertical : vertical) > 32 ||
        (horizontal < 0 ? -horizontal : horizontal) > 32) {
        angle = (f32)atan2((f64)vertical, (f64)horizontal);
        if (menuCBPokemonEntryAbsF(angle) < lbl_8047BFF8) {
            current |= 2;
        } else if (menuCBPokemonEntryAbsF(angle) > lbl_8047BFFC) {
            current |= 1;
        }
        if (lbl_8047C000 < menuCBPokemonEntryAbsF(angle) &&
            menuCBPokemonEntryAbsF(angle) < lbl_8047C004) {
            if (angle < 0.0f) {
                current |= 4;
            } else {
                current |= 8;
            }
        }
    }

    buttons = fn_800F7BC4(device);
    if ((buttons & 0x008) != 0) current |= 0x001;
    if ((buttons & 0x004) != 0) current |= 0x002;
    if ((buttons & 0x001) != 0) current |= 0x004;
    if ((buttons & 0x002) != 0) current |= 0x008;
    if ((buttons & 0x100) != 0) current |= 0x010;
    if ((buttons & 0x200) != 0) current |= 0x020;
    if ((buttons & 0x400) != 0) current |= 0x040;
    if ((buttons & 0x800) != 0) current |= 0x080;
    if ((buttons & 0x010) != 0) current |= 0x100;
    if ((buttons & 0x040) != 0) current |= 0x200;
    if ((buttons & 0x020) != 0) current |= 0x400;
    if ((buttons & 0x1000) != 0) current |= 0x800;

    pressed = (input->previous ^ 0xFFFF) & current;
    repeated = 0;
    for (bit = 0; bit < 16; bit++) {
        u16 mask = 1 << bit;
        if ((pressed & mask) != 0) {
            input->timer[bit] = 15;
            repeated |= mask;
        } else if ((current & mask) != 0) {
            input->timer[bit] -= GSgetFrameStep();
            if (input->timer[bit] <= 0) {
                input->timer[bit] = 5;
                repeated |= mask;
            }
        }
    }
    input->current = current;
    input->pressed = pressed;
    input->repeated = repeated;
}

#endif /* 0x800676EC range: fn_80068418 */

static inline u16 menuCBEntryLimit(s32 player)
{
    u16 maximum = fn_8006B1D4();
    u16 count = toolentryTaisenGetPokemonNum(player);
    return count < maximum ? count : maximum;
}

#if !defined(MENUCB_RANGE_800676EC_ONLY) && !defined(MENUCB_RANGE_80069220_ONLY) && \
    !defined(MENUCB_RANGE_800693A4_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
u8 fn_8006905C(void)
{
    u16 count;
    s32 active_players = 1;
    s32 mode;
    s32 player;
    s32 order;
    s32 index;

    mode = fn_8025D9CC();
    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        if (mode == 4) {
            active_players = 2;
            *(s32*)&lbl_803A9F08[0xCD7C] = 4;
        } else {
            active_players = 2;
            *(s32*)&lbl_803A9F08[0xCD7C] = mode;
        }
        break;
    case 2:
        active_players = 4;
        *(s32*)&lbl_803A9F08[0xCD7C] = 4;
        break;
    }

    if (*(s32*)&lbl_803A9F08[0xCD7C] != 4) {
        for (player = 1; player < active_players; player++) {
            if (lbl_803A9F08[player + 4] == 0) {
                count = menuCBEntryLimit(player);
                order = toolentryTaisengetEtnryPokemonOrderNum(player);
                if (order == count) {
                    order = toolentryTaisengetEtnryPokemonOrderNum(player);
                    if (order == menuCBEntryLimit(player)) {
                        index = order - 1;
                        if (index < 0) {
                            index = 0;
                        }
                        if (((f32*)(lbl_803A9F08 + player * 0x30))[index + 0x3363] == lbl_8047BFE8) {
                            lbl_803A9F08[player + 4] = 1;
                        }
                    }
                }
            }
        }
    }

    for (player = 0; player < active_players; player++) {
        if (lbl_803A9F08[player + 4] == 0) {
            return 0;
        }
    }
    return 1;
}
#pragma pop
#endif /* fn_8006905C */

#if !defined(MENUCB_RANGE_8006905C_ONLY) && !defined(MENUCB_RANGE_800676EC_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#if !defined(MENUCB_RANGE_800693A4_ONLY)
void fn_80069220(u8* context)
{
    menuCBPokemonEntryAdvancePositions();
    *(s16*)(context + 0x84) = *(s32*)&lbl_803A9F08[0xCD80];
}
#endif

#if !defined(MENUCB_RANGE_80069220_ONLY)
void fn_800693A4(void)
{
    menuCBPokemonEntryAdvancePositions();
}

void fn_80069504(void)
{
    menuCBPokemonEntryAdvancePositions();
}

void fn_80069664(void)
{
    menuCBPokemonEntryAdvancePositions();
}
#endif
#pragma pop
#endif /* !MENUCB_RANGE_8006905C_ONLY && !MENUCB_RANGE_800676EC_ONLY */

#endif

#if !defined(MENUCB_RANGE_80065730_ONLY) && !defined(MENUCB_RANGE_8006905C_ONLY) && \
    !defined(MENUCB_RANGE_800676EC_ONLY) && !defined(MENUCB_RANGE_80069220_ONLY) && \
    !defined(MENUCB_RANGE_800693A4_ONLY)
void fn_80065628(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        player = 3;
        break;
    case 2:
        player = 3;
        break;
    default:
        player = 3;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 &= ~2;
    }
    fn_80065A48(menu, msg, 3);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    switch (msg->cmd) {
    case 0xBB1:
    case 0xB92:
    case 0xB73:
    case 0xBD0:
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
        break;
    }
}

#endif

#if !defined(MENUCB_RANGE_80065628_ONLY) && !defined(MENUCB_RANGE_8006905C_ONLY) && \
    !defined(MENUCB_RANGE_800676EC_ONLY) && !defined(MENUCB_RANGE_80069220_ONLY) && \
    !defined(MENUCB_RANGE_800693A4_ONLY)
void fn_80065730(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        player = 1;
        break;
    case 2:
        player = 2;
        break;
    default:
        player = 2;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 |= 2;
    }
    fn_80065A48(menu, msg, 2);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    switch (msg->cmd) {
    case 0xBB1:
    case 0xB92:
    case 0xB73:
    case 0xBD0:
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
        break;
    }
}

void fn_80065838(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        player = 2;
        break;
    case 2:
        player = 1;
        break;
    default:
        player = 1;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 &= ~2;
    }
    fn_80065A48(menu, msg, 1);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    switch (msg->cmd) {
    case 0xBB1:
    case 0xB92:
    case 0xB73:
    case 0xBD0:
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
        break;
    }
}

void fn_80065940(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        player = 0;
        break;
    case 2:
        player = 0;
        break;
    default:
        player = 0;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 |= 2;
    }
    fn_80065A48(menu, msg, 0);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    switch (msg->cmd) {
    case 0xBB1:
    case 0xB92:
    case 0xB73:
    case 0xBD0:
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
        break;
    }
}

#endif
#endif

/* ===== Function implementations ===== */

#if !defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY) && \
    !defined(MENUCB_RANGE_800643D4_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80063D10_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80064378_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80065A48_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80068738_ONLY) && \
    !defined(MENUCB_RANGE_RESIDUAL_80068794_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80069048_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_800697C4_ONLY) && \
    !defined(MENUCB_RANGE_RESIDUAL_800697F4_ONLY) && \
    !defined(MENUCB_RANGE_EXACT_80069A08_ONLY)
#define MENUCB_RANGE_RESIDUAL_EMPTY_ONLY
#endif

#if defined(MENUCB_RANGE_EXACT_80069048_ONLY)
/* Address: 0x80069048 | Size: 0x20 */
s32 menuCBPokemonEntryGetReadFlag(void)
{
    return lbl_803A9F08[0xCD84];
}
#endif

#if defined(MENUCB_RANGE_EXACT_800697C4_ONLY)
/* Address: 0x800697C4 | Size: 0x30 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling on; optimize_for_size on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling on
#pragma optimize_for_size on
#pragma peephole off
void menuCBPokemonEntryLoadTex(void)
{
    fn_8010B01C(0, _menuCBPokemonEntryLoadCallBack__FPv, 0);
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_EXACT_80069A08_ONLY)
/* Address: 0x80069A08 | Size: 0x58 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling on
#pragma peephole off
s32 menuCBPokemonEntryDispPokemonFace(void* ctx, UICmdMsg* msg, s32 group, s32 slot)
{
    typedef struct PokemonEntryTexWork {
        u8 active;
        u8 _1;
        u16 face;
        u8 _4[8];
    } PokemonEntryTexWork;
    typedef struct PokemonEntryTexState {
        u8 _0[0x30];
        PokemonEntryTexWork groups[4][6];
    } PokemonEntryTexState;
    PokemonEntryTexWork* entry;

    entry = &((PokemonEntryTexState*) lbl_803A9F08)->groups[group][slot];
    if (entry->active != 0) {
        fn_8010B9E8(ctx, msg, entry->face);
        return 1;
    }
    return 0;
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_EXACT_80063D10_ONLY)
/* Address: 0x80063D10 | Size: 0x4 */
void fn_80063D10(void)
{
}
#endif

#if defined(MENUCB_RANGE_EXACT_80064378_ONLY)
/* Address: 0x80064378 | Size: 0x5C */
void fn_80064378(u8* ctx, UICmdMsg* msg)
{
    extern void fn_80063AD4(u8*, UICmdMsg*);
    extern void fn_800FB680(s32, s32, s32, u32);

    switch (msg->cmd) {
    case 0xA9E:
        fn_800FB680(0, 0, ctx[0x8B] | -0x100LL, 0x3C1A);
        break;
    case 0xA88:
        fn_80063AD4(ctx, msg);
        break;
    }
}
#endif

#if defined(MENUCB_RANGE_EXACT_80068738_ONLY)
/* Address: 0x80068738 | Size: 0x5C */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling on
#pragma peephole off
s32 fn_80068738(void)
{
    extern void windowGetKeyInfo(void);
    extern void fn_80068418(u8*, s32);
    extern u8 lbl_803A9EA0[];
    u8* entry;
    s32 i;

    windowGetKeyInfo();
    for (i = 0; i < 4; i++) {
        entry = &lbl_803A9EA0[i * 0x1A];
        fn_80068418(entry, i + 1);
    }
    return 0;
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_RESIDUAL_80068794_ONLY)
extern void* toolentryTaisenGetHeroPtr(s32);
extern void* toolentryTaisenGetPokemonPtr(s32, u16);
extern void* heroBiosGetNamePtr(void*);
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue();
extern void fn_800FB680(s32, s32, u32, u32);
extern u16 pokemonGetSoubiItemDataId(void*);
extern u8 pokemonCheckValid(void*);
extern u32 pokemonGetStatus(void*, s32, s32, s32);
extern void* pokemonBiosGetNicknamePtr(void*);
extern u16 lbl_802EDA20[][2];
extern u8 lbl_802EF0A8[];
extern f32 lbl_8047BFE8;
extern f32 lbl_8047C008;
extern f32 lbl_8047C00C;

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
static inline u8 fn_800688C4_IsPlayerActive(s32 player)
{
    u8 active = 1;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        if (player >= 2) {
            active = 0;
        }
        break;
    case 2:
        break;
    }
    return active;
}

static inline u16 fn_800688C4_GetEntrySlot(u16 command)
{
    s32 i;
    u16* entry = lbl_802EDA20[0];

    for (i = 0; i < 72; i++) {
        if (entry[0] == command) {
            return entry[1];
        }
        entry += 2;
    }
    return 0;
}

#if !defined(MENUCB_RANGE_EXACT_800688C4_ONLY) && !defined(MENUCB_RANGE_SUFFIX_80068DBC_ONLY) /* the exact island 0x800688C4-0x80068DBC builds fn_800688C4, fn_800689FC, fn_80068BB0 only */
void fn_80068794(void* context, UICmdMsg* msg, s32 player, s32 slot)
{
    typedef struct EntryPosition {
        f32 current[6];
        f32 target[6];
    } EntryPosition;
    typedef struct EntryWork {
        u8 _0[0xCD8C];
        EntryPosition position[4];
    } EntryWork;
    typedef struct EntryMoveTable {
        u8 _0[2];
        s16 x;
        u8 _4[0x18];
    } EntryMoveTable;
    EntryPosition* position;
    f32 difference;
    s32 order;

    order = toolentryTaisengetEtnryPokemonOrderNum(player);
    if (fn_800688C4_IsPlayerActive(player)) {
        if (order > slot) {
            position = &((EntryWork*) lbl_803A9F08)->position[player];
            msg->s50 = ((EntryMoveTable*) lbl_802EF0A8)[msg->cmd].x +
                       (s32) position->current[slot];
            difference = position->target[slot] - position->current[slot];
            if (difference < 0.0f) {
                difference = -difference;
            }
            msg->alpha67 = -(lbl_8047C00C * difference - lbl_8047C008);
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
    }
}
#endif /* !MENUCB_RANGE_EXACT_800688C4_ONLY */


#if !defined(MENUCB_RANGE_SUFFIX_80068DBC_ONLY)
void fn_800688C4(u8* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* name;

    if (fn_800688C4_IsPlayerActive(player)) {
        name = heroBiosGetNamePtr(toolentryTaisenGetHeroPtr(player));
        if (name == NULL) {
            name = GSmsgGetGSchar(1);
        }
        msgctrlSetValue(0x34, toolentryTaisenGetBattlePlayerID(player) + 1);
        msgctrlSetValue(0x37, name);
        if (fn_8025D9CC() == 4) {
            fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0x30DC);
        } else if (kind == 2) {
            fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0x30E6);
        } else {
            fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0x30DC);
        }
    }
}

void fn_800689FC(void* context, UICmdMsg* msg, s32 player)
{
    if (fn_800688C4_IsPlayerActive(player)) {
        s32 slot = fn_800688C4_GetEntrySlot(msg->cmd);

        if (pokemonGetSoubiItemDataId(toolentryTaisenGetPokemonPtr(player, slot)) != 0) {
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
    }
}

void fn_80068BB0(u8* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* pokemon;
    s32 slot;

    if (fn_800688C4_IsPlayerActive(player)) {
        slot = fn_800688C4_GetEntrySlot(msg->cmd);
        pokemon = toolentryTaisenGetPokemonPtr(player, slot);
        if (pokemon != NULL) {
            if (!pokemonCheckValid(pokemon)) {
                msg->flags4 &= ~2;
            } else {
                msgctrlSetValue(0x34, (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0));
                if (kind == 0) {
                    fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0x30D4);
                } else {
                    fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0xD3);
                }
            }
        }
    }
}
#endif /* !MENUCB_RANGE_SUFFIX_80068DBC_ONLY */

#if !defined(MENUCB_RANGE_EXACT_800688C4_ONLY)
void fn_80068DBC(u8* context, UICmdMsg* msg, s32 player)
{
    void* nickname;
    s32 slot;
    /* RULE-EXCEPTION(user-approved): int copy of player used at both call sites, chosen for register allocation (farm m2c finisher) — see docs/RULE_EXCEPTIONS.md */
    int playerId;

    playerId = player;
    if (fn_800688C4_IsPlayerActive(playerId)) {
        slot = fn_800688C4_GetEntrySlot(msg->cmd);
        nickname = pokemonBiosGetNicknamePtr(toolentryTaisenGetPokemonPtr(playerId, slot));
        if (nickname == NULL) {
            nickname = GSmsgGetGSchar(1);
        }
        msgctrlSetValue(0x37, nickname);
        fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0xE9);
    }
}
#pragma pop

/* Address: 0x80068F84 | Size: 0xC4 */
#pragma push
#pragma peephole off
void fn_80068F84(void)
{
    typedef struct PokemonEntryWork {
        u8 active;
        u8 _1[3];
        u32 value;
        u8 _8[4];
    } PokemonEntryWork;
    typedef struct PokemonEntryGroup {
        PokemonEntryWork entry[6];
    } PokemonEntryGroup;
    typedef struct PokemonEntryArea {
        u8 _0[0x2C];
        u32 field_2C;
        PokemonEntryGroup group[4];
    } PokemonEntryArea;
    PokemonEntryArea* work = (PokemonEntryArea*) lbl_803A9F08;
    s32 i;
    s32 j;

    work->field_2C = 0;
    /* RULE-EXCEPTION(user-approved): zero-derived pointer adjustment retained for instruction scheduling - see docs/RULE_EXCEPTIONS.md */
    work = (PokemonEntryArea*) ((u8*) work + work->field_2C);
    ((u8*) work)[0xCD84] = 0;
    for (i = 0; i < 4; i++) {
        PokemonEntryGroup* group = &work->group[i];
        for (j = 0; j < 6; j++) {
            PokemonEntryWork* entry = &group->entry[j];
            entry->active = 0;
            entry->value = 0;
        }
    }
}
#pragma pop
#endif /* !MENUCB_RANGE_EXACT_800688C4_ONLY */
#endif

#if defined(MENUCB_RANGE_EXACT_80065A48_ONLY)
/* Address: 0x80065A48 | Size: 0x1CA4 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 4; scheduling on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_80065A48(void* ctx, void* arg1, s32 arg2)
{
    UICmdMsg* msg = (UICmdMsg*) arg1;
    s32 i0;
    s32 i1;
    s32 i2;
    s32 i3;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        i0 = 0;
        i1 = 1;
        i2 = 2;
        i3 = 3;
        break;
    case 2:
        i0 = 0;
        i2 = 1;
        i1 = 2;
        i3 = 3;
        break;
    default:
        i0 = 0;
        i2 = 1;
        i1 = 2;
        i3 = 3;
        break;
    }
    switch (msg->cmd) {
    case 0xB74: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB75: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB76: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB77: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB78: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB79: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i0 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB2: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB3: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB4: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB5: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB6: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBB7: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i2 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB93: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB94: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB95: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB96: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB97: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB98: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i1 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD1: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x30;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD2: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x3C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD3: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x48;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD4: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x54;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD5: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x60;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xBD6: {
        s32 ok = 1;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            u8* p = (u8*) lbl_803A9F08 + i3 * 0x48 + 0x6C;
            if (p[0] != 0) {
                fn_8010B9E8(ctx, msg, *(u16*) (p + 2));
            }
        }
        break;
    }
    case 0xB86:
    case 0xB87:
    case 0xB88:
    case 0xB89:
    case 0xB8A:
    case 0xB8B:
        fn_80068DBC(ctx, msg, i0);
        break;
    case 0xB8C:
    case 0xB8D:
    case 0xB8E:
    case 0xB8F:
    case 0xB90:
    case 0xB91:
        fn_80068BB0(ctx, msg, i0, 0);
        break;
    case 0xB80:
    case 0xB81:
    case 0xB82:
    case 0xB83:
    case 0xB84:
    case 0xB85:
        fn_800689FC(ctx, msg, i0);
        break;
    case 0xB3D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB3E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB3F: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB40: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB41: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB42: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i0 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i0);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB43:
        fn_80068794(ctx, msg, i0, 0);
        break;
    case 0xB44:
        fn_80068794(ctx, msg, i0, 1);
        break;
    case 0xB45:
        fn_80068794(ctx, msg, i0, 2);
        break;
    case 0xB46:
        fn_80068794(ctx, msg, i0, 3);
        break;
    case 0xB47:
        fn_80068794(ctx, msg, i0, 4);
        break;
    case 0xB48:
        fn_80068794(ctx, msg, i0, 5);
        break;
    case 0xBC4:
    case 0xBC5:
    case 0xBC6:
    case 0xBC7:
    case 0xBC8:
    case 0xBC9:
        fn_80068DBC(ctx, msg, i2);
        break;
    case 0xBCA:
    case 0xBCB:
    case 0xBCC:
    case 0xBCD:
    case 0xBCE:
    case 0xBCF:
        fn_80068BB0(ctx, msg, i2, 0);
        break;
    case 0xBBE:
    case 0xBBF:
    case 0xBC0:
    case 0xBC1:
    case 0xBC2:
    case 0xBC3:
        fn_800689FC(ctx, msg, i2);
        break;
    case 0xB59: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5A: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i2 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i2);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB5F:
        fn_80068794(ctx, msg, i2, 0);
        break;
    case 0xB60:
        fn_80068794(ctx, msg, i2, 1);
        break;
    case 0xB61:
        fn_80068794(ctx, msg, i2, 2);
        break;
    case 0xB62:
        fn_80068794(ctx, msg, i2, 3);
        break;
    case 0xB63:
        fn_80068794(ctx, msg, i2, 4);
        break;
    case 0xB64:
        fn_80068794(ctx, msg, i2, 5);
        break;
    case 0xBA5:
    case 0xBA6:
    case 0xBA7:
    case 0xBA8:
    case 0xBA9:
    case 0xBAA:
        fn_80068DBC(ctx, msg, i1);
        break;
    case 0xBAB:
    case 0xBAC:
    case 0xBAD:
    case 0xBAE:
    case 0xBAF:
    case 0xBB0:
        fn_80068BB0(ctx, msg, i1, 0);
        break;
    case 0xB9F:
    case 0xBA0:
    case 0xBA1:
    case 0xBA2:
    case 0xBA3:
    case 0xBA4:
        fn_800689FC(ctx, msg, i1);
        break;
    case 0xB4B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4D: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4E: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB4F: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB50: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i1 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i1);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB51:
        fn_80068794(ctx, msg, i1, 0);
        break;
    case 0xB52:
        fn_80068794(ctx, msg, i1, 1);
        break;
    case 0xB53:
        fn_80068794(ctx, msg, i1, 2);
        break;
    case 0xB54:
        fn_80068794(ctx, msg, i1, 3);
        break;
    case 0xB55:
        fn_80068794(ctx, msg, i1, 4);
        break;
    case 0xB56:
        fn_80068794(ctx, msg, i1, 5);
        break;
    case 0xBE3:
    case 0xBE4:
    case 0xBE5:
    case 0xBE6:
    case 0xBE7:
    case 0xBE8:
        fn_80068DBC(ctx, msg, i3);
        break;
    case 0xBE9:
    case 0xBEA:
    case 0xBEB:
    case 0xBEC:
    case 0xBED:
    case 0xBEE:
        fn_80068BB0(ctx, msg, i3, 0);
        break;
    case 0xBDD:
    case 0xBDE:
    case 0xBDF:
    case 0xBE0:
    case 0xBE1:
    case 0xBE2:
        fn_800689FC(ctx, msg, i3);
        break;
    case 0xB67: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 0) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB68: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 1) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB69: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 2) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6A: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6B: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 4) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6C: {
        s32 ok = 1;
        u16 n;
        u16 m;
        switch (toolentryTaisenGetBattleType()) {
        case 0:
        case 1:
            if (i3 >= 2) {
                ok = 0;
            }
            break;
        case 2:
            break;
        }
        if ((u8) ok != 0) {
            n = fn_8006B1D4();
            m = toolentryTaisenGetPokemonNum(i3);
            m = (m < n) ? m : n;
            if ((s32) m > 5) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
        break;
    }
    case 0xB6D:
        fn_80068794(ctx, msg, i3, 0);
        break;
    case 0xB6E:
        fn_80068794(ctx, msg, i3, 1);
        break;
    case 0xB6F:
        fn_80068794(ctx, msg, i3, 2);
        break;
    case 0xB70:
        fn_80068794(ctx, msg, i3, 3);
        break;
    case 0xB71:
        fn_80068794(ctx, msg, i3, 4);
        break;
    case 0xB72:
        fn_80068794(ctx, msg, i3, 5);
        break;
    case 0xB3C:
        fn_800688C4(ctx, msg, i0, 0);
        break;
    case 0xB58:
        fn_800688C4(ctx, msg, i2, 1);
        break;
    case 0xB4A:
        fn_800688C4(ctx, msg, i1, 2);
        break;
    case 0xB66:
        fn_800688C4(ctx, msg, i3, 3);
        break;
    case 0xBF0:
        switch ((u16) toolentryTaisenGetHomePlace(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    case 0xB3B:
        switch ((u16) toolentryTaisenGetHomePlace(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    case 0xB7A:
    case 0xB7B:
    case 0xB7C:
    case 0xB7D:
    case 0xB7E:
    case 0xB7F:
    case 0xBEF:
        switch ((u16) toolentryTaisenGetHomePlace(0)) {
        case 0:
            msg->flags4 |= 2;
            break;
        default:
            msg->flags4 &= ~2;
            break;
        }
        break;
    }
}
#pragma pop
#endif

#undef MENUCB_RANGE_HEAD_ONLY
#undef MENUCB_RANGE_EXACT_80065A48_ONLY
#undef MENUCB_RANGE_TAIL_ONLY
