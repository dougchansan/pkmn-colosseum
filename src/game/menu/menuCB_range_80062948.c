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
extern u8 lbl_803A9F08[];

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
    !defined(MENUCB_RANGE_EXACT_80063D10_ONLY) && \
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

#if defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY)
void fn_80065A48(void*, UICmdMsg*, s32);
extern s32 fn_8006B1D4(void);
extern s32 toolentryTaisenGetPokemonNum(s32);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);
extern s32 fn_8025D9CC(void);
extern s32 fn_800D37CC(void);
extern s32 fn_800D3088(void);
extern f32 lbl_8047BFE8;
extern f32 lbl_8047C010;
extern f32 lbl_8047C014;
extern f32 lbl_8047C018;
#endif

#if defined(MENUCB_RANGE_HEAD_ONLY)
extern u8 fn_8006B1F4(s32, s32);
extern void fn_8006B2A4(s32, s32);
extern u8 fn_8006B3C8(s32);
extern void fn_8006B354(s32);
extern s32 fn_8025DAAC(void);
extern void fn_800FB680(s32, s32, s32, u32);
extern void fn_80063AD4(u8*, UICmdMsg*);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D5648(f32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(void*);
extern void fn_800D67BC(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5BA0(s32, u32);
extern void fn_800D6728(void);
extern void fn_800FE38C(s32, s32, s32, s32);
extern void fn_800FE35C(void);
extern u8 lbl_80314E08[];
extern u32 lbl_8047BFC8;
extern u32 lbl_8047BFCC;
extern f32 lbl_8047BFD0;
extern f32 lbl_8047BFD4;

typedef struct MenuCBBattleEntryContext {
    s32 field_00;
    s32 mode;
} MenuCBBattleEntryContext;

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
        if (result == 0) {
            toolentryCopyHero();
            result = 0xD1;
        } else {
            result = -1;
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

void fn_800637B0(void)
{
    s32 player;
    s32 battleType;
    s32 setting;
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

void fn_800638F4(u8* context, UICmdMsg* msg)
{
    u32 message;

    switch (msg->cmd) {
    case 0xE08:
    case 0xE17:
    case 0x1264:
    case 0x1123:
        fn_80063AD4(context, msg);
        break;
    case 0xE14:
    case 0xE24:
    case 0x126F:
        message = 0x3C21;
        fn_800FB680(0, 0, context[0x8B] | -0x100, message);
        break;
    case 0xE15:
    case 0xE26:
        message = 0x3DB2;
        fn_800FB680(0, 0, context[0x8B] | -0x100, message);
        break;
    case 0xE16:
    case 0xE27:
    case 0x1270:
        message = 0x3DB3;
        fn_800FB680(0, 0, context[0x8B] | -0x100, message);
        break;
    case 0xE25:
        message = 0x3DAE;
        fn_800FB680(0, 0, context[0x8B] | -0x100, message);
        break;
    }
}

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
    MenuCBColor top;
    MenuCBColor bottom;
    MenuCBColor white;
    f32 opacity;
    s32 combinedAlpha;
    s32 y;

    top.value = lbl_8047BFC8;
    bottom.value = lbl_8047BFCC;
    combinedAlpha = context[0x8B] * msg->alpha67 / 65025;
    opacity = (f32)combinedAlpha;
    top.channel.alpha =
        (u8)(s32)((f32)top.channel.alpha * opacity);
    bottom.channel.alpha =
        (u8)(s32)((f32)bottom.channel.alpha * opacity);

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D6A00(6);
    fn_800D7820(lbl_80314E08);
    fn_800D67BC(4);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, top.value);
    fn_800D61E4(msg->s54, 0);
    fn_800D5BA0(0, top.value);
    fn_800D61E4(msg->s54, msg->s56);
    fn_800D5BA0(0, bottom.value);
    fn_800D61E4(0, msg->s56);
    fn_800D5BA0(0, bottom.value);
    fn_800D6728();
    fn_800FE38C(0, 0, msg->s54, msg->s56);

    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D5648(lbl_8047BFD0);
    fn_800D6A00(1);
    fn_800D7820(lbl_80314E08);
    white.value = 0xFFFFFF00;
    white.channel.alpha =
        (u8)(s32)(lbl_8047BFD4 * opacity);
    for (y = 0; y < msg->s56; y += 4) {
        fn_800D67BC(2);
        fn_800D61E4(0, y);
        fn_800D5BA0(0, white.value);
        fn_800D61E4(msg->s54, y);
        fn_800D5BA0(0, white.value);
        fn_800D6728();
    }
    fn_800FE35C();
}
#endif

#if defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY)
static inline void menuCBPokemonEntryAdvancePositions(void)
{
    f32* current;
    f32 target;
    f32 remaining;
    f32 step;
    f32 elapsed;
    s32 player;
    s32 component;
    s32 denominator;
    s32 numerator;

    denominator = fn_800D37CC();
    numerator = fn_800D3088();
    elapsed = (f32)numerator / (f32)denominator;
    *(f32*)&lbl_803A9F08[0xCD88] = elapsed;
    for (player = 0; player < 4; player++) {
        current = (f32*)&lbl_803A9F08[0xCD8C + player * 0x30];
        for (component = 0; component < 6; component++) {
            target = current[component + 6];
            if (current[component] != target) {
                step = lbl_8047C010 *
                    (target - current[component]) * elapsed;
                if (step > lbl_8047C010) {
                    step = lbl_8047C010;
                }
                if (step < lbl_8047C014) {
                    step = lbl_8047C014;
                }
                current[component] += step;
                remaining = target - current[component];
                if (remaining < lbl_8047BFE8) {
                    remaining = -remaining;
                }
                if (step < lbl_8047BFE8) {
                    step = -step;
                }
                if (remaining == step || remaining < lbl_8047C018) {
                    current[component] = target;
                }
            }
        }
    }
}

u8 fn_8006905C(void)
{
    u16 maximum;
    u16 count;
    s32 active_players = 1;
    s32 mode;
    s32 player;
    s32 order;

    mode = fn_8025D9CC();
    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        active_players = 2;
        *(s32*)&lbl_803A9F08[0xCD7C] = mode == 4 ? 4 : mode;
        break;
    case 2:
        active_players = 4;
        *(s32*)&lbl_803A9F08[0xCD7C] = 4;
        break;
    }

    if (*(s32*)&lbl_803A9F08[0xCD7C] != 4) {
        for (player = 1; player < active_players; player++) {
            if (lbl_803A9F08[player + 4] == 0) {
                maximum = fn_8006B1D4();
                count = toolentryTaisenGetPokemonNum(player);
                if (count > maximum) {
                    count = maximum;
                }
                order = toolentryTaisengetEtnryPokemonOrderNum(player);
                if (order == count) {
                    maximum = fn_8006B1D4();
                    count = toolentryTaisenGetPokemonNum(player);
                    if (count > maximum) {
                        count = maximum;
                    }
                    if (order == count) {
                        order--;
                        if (order < 0) {
                            order = 0;
                        }
                        if (*(f32*)&lbl_803A9F08[
                                0xCD8C + player * 0x30 + order * 4] ==
                            lbl_8047BFE8) {
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

void fn_80069220(u8* context)
{
    menuCBPokemonEntryAdvancePositions();
    *(s16*)(context + 0x84) = *(s16*)&lbl_803A9F08[0xCD80];
}

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

void fn_80065628(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player = 3;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
    case 2:
    default:
        player = 3;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 &= ~2;
    }
    fn_80065A48(menu, msg, 3);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    if (msg->cmd == 0xBB1 || msg->cmd == 0xB92 ||
        msg->cmd == 0xB73 || msg->cmd == 0xBD0) {
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
    }
}

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
    default:
        player = 2;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 |= 2;
    }
    fn_80065A48(menu, msg, 2);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    if (msg->cmd == 0xBB1 || msg->cmd == 0xB92 ||
        msg->cmd == 0xB73 || msg->cmd == 0xBD0) {
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
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
    default:
        player = 1;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 &= ~2;
    }
    fn_80065A48(menu, msg, 1);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    if (msg->cmd == 0xBB1 || msg->cmd == 0xB92 ||
        msg->cmd == 0xB73 || msg->cmd == 0xBD0) {
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
    }
}

void fn_80065940(void* menu, UICmdMsg* msg)
{
    u8* color;
    s32 player = 0;

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
    case 2:
    default:
        player = 0;
        break;
    }
    if (*(s32*)(lbl_803A9F08 + 0x154) != 2) {
        msg->flags4 |= 2;
    }
    fn_80065A48(menu, msg, 0);
    color = lbl_802ED9F0 + toolentryTaisenGetBattlePlayerID(player) * 3;
    if (msg->cmd == 0xBB1 || msg->cmd == 0xB92 ||
        msg->cmd == 0xB73 || msg->cmd == 0xBD0) {
        ((u8*)msg)[0x64] = color[0];
        ((u8*)msg)[0x65] = color[1];
        ((u8*)msg)[0x66] = color[2];
    }
}
#endif

/* ===== Function implementations ===== */

#if !defined(MENUCB_RANGE_RESIDUAL_EMPTY_ONLY) && \
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

#if defined(MENUCB_RANGE_RESIDUAL_800697F4_ONLY)
/* Address: 0x80069944 | Size: 0xC4 */
#pragma push
#pragma scheduling off
#pragma peephole off
void menuCBPokemonEntryTexWorkInit(void)
{
    typedef struct PokemonEntryTexWork {
        u8 active;
        u8 _1[3];
        u32 value;
        u8 _8[4];
    } PokemonEntryTexWork;
    PokemonEntryTexWork* entry0;
    PokemonEntryTexWork* entry1;
    PokemonEntryTexWork* entry2;
    PokemonEntryTexWork* entry3;
    PokemonEntryTexWork* entry4;
    PokemonEntryTexWork* entry5;
    u8* group;
    u32 pairs;

    *(u32*) &lbl_803A9F08[0x2C] = 0;
    lbl_803A9F08[0xCD84] = 0;
    group = lbl_803A9F08;
    for (pairs = 0; pairs < 2; pairs++) {
        entry0 = (PokemonEntryTexWork*) &group[0x30];
        entry0->active = 0;
        entry1 = entry0 + 1;
        entry2 = entry0 + 2;
        entry3 = entry0 + 3;
        entry0->value = 0;
        entry4 = entry0 + 4;
        entry5 = entry0 + 5;
        group += 0x48;
        entry1->active = 0;
        entry0 = (PokemonEntryTexWork*) &group[0x30];
        group += 0x48;
        entry1->value = 0;
        entry1 = entry0 + 1;
        entry2->active = 0;
        entry2->value = 0;
        entry2 = entry0 + 2;
        entry3->active = 0;
        entry3->value = 0;
        entry3 = entry0 + 3;
        entry4->active = 0;
        entry4->value = 0;
        entry4 = entry0 + 4;
        entry5->active = 0;
        entry5->value = 0;
        entry5 = entry0 + 5;
        entry0->active = 0;
        entry0->value = 0;
        entry1->active = 0;
        entry1->value = 0;
        entry2->active = 0;
        entry2->value = 0;
        entry3->active = 0;
        entry3->value = 0;
        entry4->active = 0;
        entry4->value = 0;
        entry5->active = 0;
        entry5->value = 0;
    }
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_EXACT_80069A08_ONLY)
/* Address: 0x80069A08 | Size: 0x58 */
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
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);
extern void* toolentryTaisenGetHeroPtr(s32);
extern void* toolentryTaisenGetPokemonPtr(s32, u16);
extern void* heroBiosGetNamePtr(void*);
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue();
extern s32 fn_8025D9CC(void);
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

void fn_80068794(void* context, UICmdMsg* msg, s32 player, s32 slot)
{
    f32* position;
    f32 difference;
    s32 x;
    s32 order;

    order = toolentryTaisengetEtnryPokemonOrderNum(player);
    if (toolentryTaisenGetBattleType() < 2 && player >= 2) {
        return;
    }
    if (order > slot) {
        position = (f32*)&lbl_803A9F08[0xCD8C + player * 0x30 + slot * 4];
        x = *(s16*)&lbl_802EF0A8[msg->cmd * 0x1C + 2] + (s32)position[0];
        msg->s50 = x;
        difference = position[6] - position[0];
        if (difference <= lbl_8047BFE8) {
            difference = -difference;
        }
        ((u8*)msg)[0x67] =
            (u8)(-(lbl_8047C00C * difference - lbl_8047C008));
        msg->flags4 |= 2;
    } else {
        msg->flags4 &= ~2;
    }
}

void fn_800688C4(u8* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* name;
    u32 message;

    if (toolentryTaisenGetBattleType() < 2 && player >= 2) {
        return;
    }
    name = heroBiosGetNamePtr(toolentryTaisenGetHeroPtr(player));
    if (name == NULL) {
        name = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x34, toolentryTaisenGetBattlePlayerID(player) + 1);
    msgctrlSetValue(0x37, name);

    message = 0x30DC;
    if (fn_8025D9CC() != 4 && kind == 2) {
        message = 0x30E6;
    }
    fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], message);
}

void fn_800689FC(void* context, UICmdMsg* msg, s32 player)
{
    u16 item_slot = 0;
    u16 command = (u16)msg->cmd;
    s32 i;

    if (toolentryTaisenGetBattleType() < 2 && player >= 2) {
        return;
    }
    for (i = 0; i < 72; i++) {
        if (lbl_802EDA20[i][0] == command) {
            item_slot = lbl_802EDA20[i][1];
            break;
        }
    }
    if (pokemonGetSoubiItemDataId(
            toolentryTaisenGetPokemonPtr(player, item_slot)) != 0) {
        msg->flags4 |= 2;
    } else {
        msg->flags4 &= ~2;
    }
}

void fn_80068BB0(u8* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* pokemon;
    u16 slot = 0;
    u16 command = (u16)msg->cmd;
    u32 message;
    s32 i;

    if (toolentryTaisenGetBattleType() < 2 && player >= 2) {
        return;
    }
    for (i = 0; i < 72; i++) {
        if (lbl_802EDA20[i][0] == command) {
            slot = lbl_802EDA20[i][1];
            break;
        }
    }
    pokemon = toolentryTaisenGetPokemonPtr(player, slot);
    if (pokemon == NULL) {
        return;
    }
    if (!pokemonCheckValid(pokemon)) {
        msg->flags4 &= ~2;
        return;
    }
    msgctrlSetValue(0x34, pokemonGetStatus(pokemon, 0, 0x7A, 0) & 0xFF);
    message = kind == 0 ? 0x30D4 : 0xD3;
    fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], message);
}

void fn_80068DBC(u8* context, UICmdMsg* msg, s32 player)
{
    void* nickname;
    u16 slot = 0;
    u16 command = (u16)msg->cmd;
    s32 i;

    if (toolentryTaisenGetBattleType() < 2 && player >= 2) {
        return;
    }
    for (i = 0; i < 72; i++) {
        if (lbl_802EDA20[i][0] == command) {
            slot = lbl_802EDA20[i][1];
            break;
        }
    }
    nickname = pokemonBiosGetNicknamePtr(
        toolentryTaisenGetPokemonPtr(player, slot));
    if (nickname == NULL) {
        nickname = GSmsgGetGSchar(1);
    }
    msgctrlSetValue(0x37, nickname);
    fn_800FB680(0, 0, 0xFFFFFF00 | context[0x8B], 0xE9);
}

/* Address: 0x80068F84 | Size: 0xC4 */
#pragma push
#pragma scheduling off
#pragma peephole off
void fn_80068F84(void)
{
    typedef struct PokemonEntryWork {
        u8 active;
        u8 _1[3];
        u32 value;
        u8 _8[4];
    } PokemonEntryWork;
    PokemonEntryWork* entry0;
    PokemonEntryWork* entry1;
    PokemonEntryWork* entry2;
    PokemonEntryWork* entry3;
    PokemonEntryWork* entry4;
    PokemonEntryWork* entry5;
    u8* group;
    u32 pairs;

    *(u32*) &lbl_803A9F08[0x2C] = 0;
    lbl_803A9F08[0xCD84] = 0;
    group = lbl_803A9F08;
    for (pairs = 0; pairs < 2; pairs++) {
        entry0 = (PokemonEntryWork*) &group[0x30];
        entry0->active = 0;
        entry1 = entry0 + 1;
        entry2 = entry0 + 2;
        entry3 = entry0 + 3;
        entry0->value = 0;
        entry4 = entry0 + 4;
        entry5 = entry0 + 5;
        group += 0x48;
        entry1->active = 0;
        entry0 = (PokemonEntryWork*) &group[0x30];
        group += 0x48;
        entry1->value = 0;
        entry1 = entry0 + 1;
        entry2->active = 0;
        entry2->value = 0;
        entry2 = entry0 + 2;
        entry3->active = 0;
        entry3->value = 0;
        entry3 = entry0 + 3;
        entry4->active = 0;
        entry4->value = 0;
        entry4 = entry0 + 4;
        entry5->active = 0;
        entry5->value = 0;
        entry5 = entry0 + 5;
        entry0->active = 0;
        entry0->value = 0;
        entry1->active = 0;
        entry1->value = 0;
        entry2->active = 0;
        entry2->value = 0;
        entry3->active = 0;
        entry3->value = 0;
        entry4->active = 0;
        entry4->value = 0;
        entry5->active = 0;
        entry5->value = 0;
    }
}
#pragma pop
#endif

#if defined(MENUCB_RANGE_EXACT_80065A48_ONLY)
/* Address: 0x80065A48 | Size: 0x1CA4 */
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
