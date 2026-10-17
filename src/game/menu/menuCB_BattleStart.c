/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file menuCB_BattleStart.c
 * @brief menuCB_BattleStart.cpp, 0x8005DFC8 - 0x80062948.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. XD has __sinit_menuCB_BattleStart_cpp at
 * 0x800477F4; includes menuCBBattleStart* + menuCB_BattleResult* locals
 * (XD 0x80046594-0x800477F8). SMOKING GUN: fn_8005DFC8 takes the address of
 * local symbol _menuCBBattleStartDispTrainerTexCallBack__FlPvl (0x800626CC),
 * proving same-TU membership. The battle-entry callback core is reconstructed
 * below while the remaining residual functions stay source candidates.
 */
#include "dolphin/types.h"

/* Wrapper units define one of these to emit only their own functions. */
#if defined(MENUCB_BATTLESTART_8005E7F0_ONLY) || \
    defined(MENUCB_BATTLESTART_80060434_ONLY) || \
    defined(MENUCB_BATTLESTART_80060D70_ONLY) || \
    defined(MENUCB_BATTLESTART_8006106C_ONLY) || \
    defined(MENUCB_BATTLESTART_80061454_ONLY) || \
    defined(MENUCB_BATTLESTART_80061BBC_ONLY) || \
    defined(MENUCB_BATTLESTART_80061D34_ONLY) || \
    defined(MENUCB_BATTLESTART_80062284_ONLY) || \
    defined(MENUCB_BATTLESTART_800626CC_ONLY) || \
    defined(MENUCB_BATTLESTART_80062834_ONLY) || \
    defined(MENUCB_BATTLESTART_8005DFC8_ONLY)
#define MENUCB_BATTLESTART_SPLIT_UNIT
#endif

/* Single-function wrapper units define one of these to emit only their own
 * function from this shared source. */

typedef struct MenuCBBattleStartPlayerView {
    union {
        u16 marker[6];
        f32 transitionValue[3];
    } header;
    f32 reset[6];
    f32 position[6];
    f32 side[6];
    f32 alpha[6];
} MenuCBBattleStartPlayerView;

typedef struct MenuCBBattleStartPlayerLayout {
    MenuCBBattleStartPlayerView view;
    f32 maxHp[6];
    f32 hp[6];
    f32 maxHpDisplay[6];
} MenuCBBattleStartPlayerLayout;

typedef struct MenuCBBattleStartPosition {
    f32 x;
    f32 y;
    f32 z;
} MenuCBBattleStartPosition;

typedef struct MenuCBBattleStartTransitions {
    f32 active[2];
    f32 current[2];
    f32 target[2];
} MenuCBBattleStartTransitions;

typedef struct MenuCBBattleStartModel {
    u8 data[0x48];
    f32 scale;
    u8 pad_4C[4];
    u32 soundId;
    u32 battleId;
    u8 trainerName[0x18];
    u32 trainerPrefix;
} MenuCBBattleStartModel;

typedef struct MenuCBBattleStartState {
    void* menu;
    s32 status;
    u16 menuId;
    u8 padA[0x2E];
    s32 timer;
    f32 deltaTime;
    MenuCBBattleStartTransitions transitions;
    MenuCBBattleStartPlayerLayout players[4];
    MenuCBBattleStartPosition trainerPositions[4];
    f32 field358;
    f32 field35C;
    f32 field360;
    f32 field364;
    u8 field368;
    u8 pad369[3];
    MenuCBBattleStartModel model;
} MenuCBBattleStartState;

typedef struct MenuCBBattleStartButton {
    u8 pad0[0x98];
    u8 finished;
} MenuCBBattleStartButton;

typedef struct MenuCBBattleStartParams {
    u8 pad0[4];
    s32 mode;
} MenuCBBattleStartParams;

typedef struct UICmdMsg {
    u8 pad0[4];
    s8 flags4;
    u8 pad5;
    s16 cmd;
    u8 pad8[0x48];
    s16 field50;
    s16 field52;
    s16 field54;
    s16 field56;
    u8 pad58[0xF];
    u8 alpha67;
    f32 scale68;
    f32 scale6C;
} UICmdMsg;

extern MenuCBBattleStartState lbl_803A9A60;
extern u8 lbl_803A9E40[];

static inline f32 battleStartAbs(f32 x)
{
    return x > 0.0f ? x : -x;
}

static inline void battleStartApproach(f32* current, f32* target, f32 delta)
{
    extern const f32 lbl_8047BF70;
    extern const f32 lbl_8047BF74;
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = lbl_8047BF70 * step;
    step = scaled * delta;
    if (step > lbl_8047BF70) {
        step = lbl_8047BF70;
    }
    if (step <= lbl_8047BF74) {
        step = lbl_8047BF74;
    }
    *current += step;
    distance = *target - *current;
    if (battleStartAbs(distance) <= battleStartAbs(step)) {
        *current = *target;
    }
}

static inline u8 battleStartLoaded(void)
{
    extern u8 menuCBPokemonEntryGetReadFlag(void);
    if (menuCBPokemonEntryGetReadFlag() && *((u8*)&lbl_803A9A60 + 0x34) != 0) {
        return 1;
    }
    return 0;
}

/* Every view, trainer and transition has reached its target. */
static inline u8 battleStartSettled(void)
{
    MenuCBBattleStartPlayerLayout* player;
    MenuCBBattleStartPosition* position;
    s32 i;
    s32 j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 6; j++) {
            player = &lbl_803A9A60.players[i];
            if (player->view.side[j] != player->view.alpha[j]) {
                return 0;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        position = &lbl_803A9A60.trainerPositions[i];
        if (position->y != position->z) {
            return 0;
        }
    }
    for (i = 0; i < 2; i++) {
        if (lbl_803A9A60.transitions.current[i] != lbl_803A9A60.transitions.target[i]) {
            return 0;
        }
    }
    lbl_803A9A60.field368 = 1;
    return 1;
}

/* Count each displayed HP down towards its real value. */
static inline u8 battleStartDrainHp(void)
{
    extern const f32 lbl_8047BF68;
    MenuCBBattleStartPlayerLayout* player;
    f32 step;
    s32 i;
    s32 j;
    u8 done;

    done = 1;
    for (i = 0; i < 4; i++) {
        player = &lbl_803A9A60.players[i];
        for (j = 0; j < 6; j++) {
            if (player->hp[j] != player->maxHp[j]) {
                step = player->maxHpDisplay[j] * lbl_803A9A60.deltaTime;
                step *= lbl_8047BF68;
                player->maxHp[j] -= step;
                if (player->maxHp[j] < player->hp[j]) {
                    player->maxHp[j] = player->hp[j];
                }
                done = 0;
            }
        }
    }
    return done;
}

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8005DFC8_ONLY)
void fn_8005DFC8(void* arg)
{
    extern u8 fn_80069048(void);
    extern u32 msgGetCharsPerFrame(void);
    extern s32 fn_800D37CC(void);
    extern s32 GSsndGetStatus(u32);
    extern s32 toolentryTaisenGetBattleType(void);
    extern u8 menuCBPokemonEntryGetReadFlag(void);
    extern void fn_80060A28(void);
    extern void fn_8017B000();
    extern void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(void);
    extern void winSeqSetMenu(s32 sequence, s32 menu);
    extern const f32 lbl_8047BF60;
    extern const f32 lbl_8047BF64;
    extern const f32 lbl_8047BF68;
    extern const f32 lbl_8047BF6C;
    extern const f32 lbl_8047BF70;
    extern const f32 lbl_8047BF74;
    extern const f32 lbl_8047BF78;
    extern const f32 lbl_8047BF7C;

    MenuCBBattleStartPlayerLayout* player;
    MenuCBBattleStartPosition* position;
    f32* pair;
    f32 delta_time;
    f32 rate;
    f32 diff;
    f32 step;
    int i;
    int j;
    int ready;
    u8 loaded;
    u8* arg_bytes = (u8*) arg;
    u8* base;

#define STATE_U8(off) (*(u8*) ((u8*)&lbl_803A9A60 + (off)))
#define STATE_U32(off) (*(u32*) ((u8*)&lbl_803A9A60 + (off)))
#define STATE_F32(off) (*(f32*) ((u8*)&lbl_803A9A60 + (off)))

    rate = fn_800D37CC();
    lbl_803A9A60.deltaTime = msgGetCharsPerFrame() / rate;
    delta_time = lbl_803A9A60.deltaTime;

    switch (STATE_U32(0x38)) {
    case 0:
        if (!menuCBPokemonEntryGetReadFlag()) {
            break;
        }
        base = (u8*)&lbl_803A9A60;
        if (*(u8*)(base + 0x34) == 0) {
            *(u8*)(base + 0x34) = 0;
            *(u32*)(base + 0x2C) = 0;
            if (toolentryTaisenGetBattleType() == 2) {
                STATE_U32(0x30) = 4;
            } else {
                STATE_U32(0x30) = 2;
            }
            *(u32*)(base + 0x10) = 0;
            *(u32*)(base + 0x0C) = 0;
            *(u32*)(base + 0x18) = 0;
            *(u32*)(base + 0x14) = 0;
            *(u32*)(base + 0x20) = 0;
            *(u32*)(base + 0x1C) = 0;
            *(u32*)(base + 0x28) = 0;
            *(u32*)(base + 0x24) = 0;
            if (toolentryTaisenGetBattleType() == 2) {
                ready = 0;
            } else {
                ready = 1;
            }
            *(u8*)(base + 0x34) = 1;
            fn_8017B000(ready == 0 ? 0x5C4 : 0x5C3, 0,
                        _menuCBBattleStartDispTrainerTexCallBack__FlPvl, 0, 0);
            STATE_U32(0x38) = 1;
        } else {
            STATE_U32(0x38) = 2;
        }
        break;

    case 1:
        if (battleStartLoaded()) {
            STATE_U32(0x38) = 2;
        }
        break;

    case 2:
        fn_80060A28();
        if (battleStartSettled()) {
            if (lbl_803A9A60.status == 0) {
                STATE_U32(0x38) = 3;
                STATE_F32(0x3B8) = lbl_8047BF60;
            } else {
                STATE_U32(0x38) = 4;
            }
        }
        break;

    case 3:
        STATE_F32(0x3B8) += delta_time;
        if (STATE_F32(0x3B8) >= lbl_8047BF64) {
            STATE_U32(0x38) = 0xA;
        }
        break;

    case 0xA:
        if (GSsndGetStatus(STATE_U32(0x3BC)) == 0) {
            STATE_U32(0x38) = 0xB;
        }
        break;

    case 4:
        if (battleStartDrainHp()) {
            STATE_U32(0x38) = 5;
        }
        break;

    case 5:
        lbl_803A9A60.model.scale += delta_time;
        if (lbl_803A9A60.model.scale >= lbl_8047BF6C) {
            STATE_U32(0x38) = 6;
        }
        break;

    case 6:
        for (i = 0; i < 2; i++) {
            pair = &lbl_803A9A60.field358 + i * 2;
            if (pair[0] != pair[1]) {
                battleStartApproach(&pair[0], &pair[1], lbl_803A9A60.deltaTime);
            }
        }
        diff = lbl_803A9A60.field358 - lbl_803A9A60.field35C;
        if (battleStartAbs(diff) <= lbl_8047BF78) {
            STATE_U32(0x38) = 7;
            STATE_F32(0x3B8) = lbl_8047BF60;
        }
        break;

    case 7:
        STATE_F32(0x3B8) += delta_time;
        if (STATE_F32(0x3B8) >= lbl_8047BF7C) {
            STATE_U32(0x38) = 8;
        }
        break;

    case 8:
        STATE_U32(0x38) = 9;
        break;

    case 0xB:
        STATE_U32(0x38) = 0x64;
        winSeqSetMenu(*(u32*) (arg_bytes + 4), 0x1C6);
        arg_bytes[2] = 1;
        break;

    case 9:
    case 0x64:
        break;
    }

#undef STATE_F32
#undef STATE_U32
#undef STATE_U8
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8005E7F0_ONLY)
void fn_8005E690(MenuCBBattleStartButton* button) {
    extern void menuButtonNormal(void* button);

    switch (lbl_803A9A60.status) {
    case 0:
        if (lbl_803A9A60.timer >= 3) {
            menuButtonNormal(button);
        }
        if (lbl_803A9A60.timer == 100) {
            button->finished = 1;
        }
        break;
    case 1:
        if (lbl_803A9A60.timer >= 7) {
            menuButtonNormal(button);
        }
        if (lbl_803A9A60.timer == 9) {
            button->finished = 1;
        }
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8005E7F0_ONLY)
void fn_8005E730(void* arg) {
    fn_8005DFC8(arg);
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8005E7F0_ONLY)
s32 fn_8005E750(MenuCBBattleStartParams* params) {
    extern void menuCBBattleStartInit(void* params, s32 mode);
    extern void menuSetEnablePort(s32 enabled);
    extern void menuOpen(s32 menuId, s32 mode);
    extern void menuCloseCustom(s32 menuId, s32 arg1, s32 arg2);

    menuCBBattleStartInit(params, 0);
    lbl_803A9A60.menuId = 0xBA;
    menuSetEnablePort(0);
    menuOpen(0xDF, 0);
    menuOpen(0xBA, 1);
    menuSetEnablePort(1);
    menuCloseCustom(0xBA, 0, 1);
    lbl_803A9A60.status = 0;

    switch (params->mode) {
    default:
        return 0xC4;
    case 2:
        return 0xC6;
    }
}
#endif

typedef struct MenuCBBattleStartMessageContext {
    u8 pad0[0x8B];
    u8 alpha;
} MenuCBBattleStartMessageContext;

typedef struct MenuCBBattleStartResourceData {
    u8 pad0[0x1D06E];
    s16 screenWidth;
} MenuCBBattleStartResourceData;

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
static inline s32 battleStartMessageWidth(u32 messageId)
{
    extern u32 GSmsgGetRect(u32 messageId);
    return GSmsgGetRect(messageId) >> 16;
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
static inline void battleStartDrawMessage(MenuCBBattleStartMessageContext* context,
                                   s32 x, u32 messageId)
{
    extern void fn_800FB680(s32 x, s32 y, s32 color, u32 messageId);
    s32 color;

    color = context->alpha | 0xFFFFFF00;
    fn_800FB680(x, 0, color, messageId);
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
static inline void battleStartDrawSingle(MenuCBBattleStartMessageContext* context,
                                  s32 screenWidth, u32 messageId)
{
    s32 x;

    x = (screenWidth - battleStartMessageWidth(messageId)) / 2;
    battleStartDrawMessage(context, x, messageId);
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
static inline void battleStartDrawPair(MenuCBBattleStartMessageContext* context,
                                s32 screenWidth, u32 firstMessage,
                                s32 spacing)
{
    const u32 secondMessage = 0x3F3D;
    s32 firstWidth;
    s32 x;

    firstWidth = battleStartMessageWidth(firstMessage) + spacing;
    x = (screenWidth - firstWidth -
         battleStartMessageWidth(secondMessage)) / 2;
    battleStartDrawMessage(context, x, firstMessage);
    battleStartDrawMessage(context, x + firstWidth, secondMessage);
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
void fn_80060434(MenuCBBattleStartMessageContext* context, UICmdMsg* message)
{
    extern s32 toolentryTaisenGetBattleType(void);
    extern s32 fn_8025D9A8(void);
    extern s32 fn_8025DAD0(void);
    extern void msgctrlSetValue(s32 index, s32 value);
    extern u32 GSmsgGetRect(u32 messageId);
    extern void fn_800FB680(s32 x, s32 y, s32 color, u32 messageId);
    extern u8 lbl_802EF0A8[];
    extern u32 lbl_802ED9A0[];
    s32 displayMode;
    s32 variant;
    s32 battleId;
    s16* screenWidth;
    s32 firstWidth;
    s32 x;

    (void)message;
    toolentryTaisenGetBattleType();
    displayMode = fn_8025D9A8();
    variant = fn_8025DAD0();
    battleId = lbl_803A9A60.model.battleId;
    screenWidth = &((MenuCBBattleStartResourceData*)lbl_802EF0A8)->screenWidth;

    switch (lbl_803A9A60.status) {
    case 0:
        switch (displayMode) {
        case 0:
            if (battleId <= 5) {
                msgctrlSetValue(0x2F, battleId + 1);
                fn_800FB680((*screenWidth - (s32)(GSmsgGetRect(0x3F39) >> 16)) / 2, 0,
                            context->alpha | -0x100, 0x3F39);
            } else if (battleId == 6) {
                fn_800FB680((*screenWidth - (s32)(GSmsgGetRect(0x3F3A) >> 16)) / 2, 0,
                            context->alpha | -0x100, 0x3F3A);
            } else if (battleId == 7) {
                fn_800FB680((*screenWidth - (s32)(GSmsgGetRect(0x3F3B) >> 16)) / 2, 0,
                            context->alpha | -0x100, 0x3F3B);
            }
            break;
        case 1:
            msgctrlSetValue(0x2F, battleId + 1);
            fn_800FB680((*screenWidth - (s32)(GSmsgGetRect(0x3F3C) >> 16)) / 2, 0,
                        context->alpha | -0x100, 0x3F3C);
            break;
        case 2:
            x = (*screenWidth - (s32)(GSmsgGetRect(lbl_802ED9A0[variant]) >> 16)) / 2;
            fn_800FB680(x, 0, context->alpha | -0x100, lbl_802ED9A0[variant]);
            break;
        case 3:
            break;
        }
        break;
    case 1:
        switch (displayMode) {
        case 0:
            if (battleId <= 5) {
                msgctrlSetValue(0x2F, battleId + 1);
                firstWidth = (s32)(GSmsgGetRect(0x3F39) >> 16) + 11;
                x = (*screenWidth - (firstWidth + (s32)(GSmsgGetRect(0x3F3D) >> 16))) / 2;
                fn_800FB680(x, 0, context->alpha | -0x100, 0x3F39);
                fn_800FB680(x + firstWidth, 0, context->alpha | -0x100, 0x3F3D);
            } else if (battleId == 6) {
                firstWidth = (s32)(GSmsgGetRect(0x3F3A) >> 16) + 9;
                x = (*screenWidth - (firstWidth + (s32)(GSmsgGetRect(0x3F3D) >> 16))) / 2;
                fn_800FB680(x, 0, context->alpha | -0x100, 0x3F3A);
                fn_800FB680(x + firstWidth, 0, context->alpha | -0x100, 0x3F3D);
            } else if (battleId == 7) {
                firstWidth = (s32)(GSmsgGetRect(0x3F3B) >> 16) + 9;
                x = (*screenWidth - (firstWidth + (s32)(GSmsgGetRect(0x3F3D) >> 16))) / 2;
                fn_800FB680(x, 0, context->alpha | -0x100, 0x3F3B);
                fn_800FB680(firstWidth + x, 0, context->alpha | -0x100, 0x3F3D);
            }
            break;
        case 1:
            firstWidth = (s32)(GSmsgGetRect(0x3F3C) >> 16) + 11;
            x = (*screenWidth - (firstWidth + (s32)(GSmsgGetRect(0x3F3D) >> 16))) / 2;
            msgctrlSetValue(0x2F, battleId + 1);
            fn_800FB680(x, 0, context->alpha | -0x100, 0x3F3C);
            fn_800FB680(firstWidth + x, 0, context->alpha | -0x100, 0x3F3D);
            break;
        case 2:
            firstWidth = (s32)(GSmsgGetRect(lbl_802ED9A0[variant]) >> 16) + 11;
            x = (*screenWidth - (firstWidth + (s32)(GSmsgGetRect(0x3F3D) >> 16))) / 2;
            msgctrlSetValue(0x2F, battleId);
            fn_800FB680(x, 0, context->alpha | -0x100, lbl_802ED9A0[variant]);
            fn_800FB680(firstWidth + x, 0, context->alpha | -0x100, 0x3F3D);
            break;
        case 3:
            break;
        }
        break;
    }
}
#endif

typedef struct MenuCBBattleStartDrawParams {
    u8 pad0[0x54];
    s16 x;
    s16 y;
} MenuCBBattleStartDrawParams;

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060434_ONLY)
void fn_800608C4(void* context, MenuCBBattleStartDrawParams* params) {
    extern void* menuModelRender(void* model);
    extern void gfxSetFlags(s32 mode);
    extern void gfxClearFlags(s32 mode);
    extern void gfxSetPrimitive(s32 primitive);
    extern void gfxSetVtxDesc(void* format);
    extern void gfxLoadTexture(s32 index, void* model);
    extern void gfxBegin(s32 count);
    extern void fn_800D61E4(s32 x, s32 y);
    extern void fn_800D5CB8(s32 index, s32 red, s32 green, s32 blue, s32 alpha);
    extern void gfxVtxTexCoord(s32 index, f32 x, f32 y);
    extern void gfxEnd(void);
    extern u8 lbl_80314F98[];
    extern f32 lbl_8047BF60;
    extern f32 lbl_8047BF90;
    void* model;

    model = menuModelRender(&lbl_803A9A60.model);
    if (model != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, model);
        gfxBegin(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BF60, lbl_8047BF60);
        fn_800D61E4(params->x, params->y);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047BF90, lbl_8047BF90);
        gfxEnd();
    }
}
#endif

typedef struct MenuCBBattleStartSpriteContext {
    u8 pad0[0x84];
    s16 x;
    s16 y;
} MenuCBBattleStartSpriteContext;

typedef struct MenuCBBattleStartSprite {
    u8 pad0[6];
    s16 tableIndex;
    u8 pad8[0x48];
    s16 x;
    s16 y;
} MenuCBBattleStartSprite;

typedef struct MenuCBBattleStartSpriteEntry {
    u8 pad0[2];
    s16 x;
    u8 pad4[0x18];
} MenuCBBattleStartSpriteEntry;

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || \
    defined(MENUCB_BATTLESTART_8006106C_ONLY) || \
    defined(MENUCB_BATTLESTART_80061BBC_ONLY)
#if defined(MENUCB_BATTLESTART_8006106C_ONLY) || \
    defined(MENUCB_BATTLESTART_80061BBC_ONLY)
/*
 * RULE-EXCEPTION(user-approved): the fn_8006106C and fn_80061BBC carves keep a
 * static copy of fn_800609B4 so their calls are inlined as retail's are; the
 * real fn_800609B4 stays in its own unit — see docs/RULE_EXCEPTIONS.md
 */
#define MENUCB_BATTLESTART_SPRITE_LINKAGE static inline
#else
#define MENUCB_BATTLESTART_SPRITE_LINKAGE
#endif
MENUCB_BATTLESTART_SPRITE_LINKAGE
void fn_800609B4(MenuCBBattleStartSpriteContext* context,
                 MenuCBBattleStartSprite* sprite, f32 xOffset) {
    extern MenuCBBattleStartSpriteEntry lbl_802EF0A8[];
    extern void fn_800FE6D0(s16 x, s16 y);
    extern void spriteSetEnv(void);
    MenuCBBattleStartSpriteEntry* entry = &lbl_802EF0A8[sprite->tableIndex];

    sprite->x = (s16)(entry->x + (s32)xOffset);
    fn_800FE6D0(context->x + sprite->x, context->y + sprite->y);
    spriteSetEnv();
}
#endif

typedef struct MenuCBBattleStartMessage {
    u8 pad0[4];
    s8 flags;
} MenuCBBattleStartMessage;

void fn_80061B74(void*, MenuCBBattleStartMessage*, s32, s32, s32);

typedef struct MenuCBBattleStartOrderRow {
    u32 slot[6];
} MenuCBBattleStartOrderRow;

typedef struct MenuCBBattleStartOrderTable {
    MenuCBBattleStartOrderRow row[6];
} MenuCBBattleStartOrderTable;

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
void _menuCBBattleStartSetIndex__Fv(void)
{
    extern s32 toolentryTaisenGetBattleType(void);
    extern u16 toolentryTaisenGetPokemonNum(s32);
    extern u16 toolentryTaisenGetEntryPokemonNum(s32);
    extern const MenuCBBattleStartOrderTable lbl_80267AF8;
    extern const MenuCBBattleStartOrderTable lbl_80267B88;
    s32 battleType = toolentryTaisenGetBattleType();
    u16 count[4] = { 6, 6, 6, 6 };
    MenuCBBattleStartOrderRow* order;
    s32 player;
    u16 slot;
    u16 n;

    order = (MenuCBBattleStartOrderRow*)lbl_803A9E40;
    for (player = 0; player < 4; player++) {
        for (slot = 0; slot < 6; slot++) {
            order[player].slot[slot] = slot;
        }
    }

    switch (lbl_803A9A60.status) {
    case 0:
        for (player = 0; player < 4; player++) {
            count[player] = toolentryTaisenGetPokemonNum(player);
        }
        break;
    case 1:
        for (player = 0; player < 4; player++) {
            count[player] = toolentryTaisenGetEntryPokemonNum(player);
        }
        break;
    }

    switch (battleType) {
    case 0: {
        order = (MenuCBBattleStartOrderRow*)lbl_803A9E40;
        for (player = 0; player < 2; player++, order++) {
            MenuCBBattleStartOrderTable table = lbl_80267AF8;
            s32 row = count[player] - 1;

            *order = table.row[row];
        }
        break;
    }
    case 1: {
        order = (MenuCBBattleStartOrderRow*)lbl_803A9E40;
        for (player = 0; player < 2; player++, order++) {
            MenuCBBattleStartOrderTable table = lbl_80267B88;
            s32 row = count[player] - 1;

            *order = table.row[row];
        }
        break;
    }
    case 2:
        order = (MenuCBBattleStartOrderRow*)lbl_803A9E40;
        for (player = 0; player < 4; player++) {
            n = count[player];
            for (slot = 0; slot < n; slot++) {
                order[player].slot[slot] = slot;
            }
            for (slot = n; slot < 6; slot++) {
                order[player].slot[slot] = n++;
            }
        }
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
/* RULE-EXCEPTION(user-approved): single-function inline helper (menuCBBattleStartInit) — see docs/RULE_EXCEPTIONS.md */
static inline u8 battleStartTypeCompatible(void)
{
    extern s32 toolentryTaisenGetBattleType(void);
    s32 firstBattleType;
    u8 compatible;
    s32 battleType;

    firstBattleType = toolentryTaisenGetBattleType();
    compatible = 1;
    battleType = toolentryTaisenGetBattleType();
    if (firstBattleType == 2) {
        if (battleType != 2) {
            compatible = 0;
        }
    } else if (battleType == 2) {
        compatible = 0;
    }
    return compatible;
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
/* RULE-EXCEPTION(user-approved): single-function inline helper (menuCBBattleStartInit) — see docs/RULE_EXCEPTIONS.md */
static inline void battleStartInitHp(void)
{
    extern void* toolentryTaisenGetEntryPokemonPtr(s32, s32);
    extern u16 pokemonBiosGetMaxHp(void*);
    extern u16 pokemonBiosGetHp(void*);
    extern const f32 lbl_8047BF60;
    extern const f32 lbl_8047BFAC;
    MenuCBBattleStartPlayerLayout* layout;
    void* pokemon;
    s32 slot;
    s32 player;

    for (player = 0; player < 4; player++) {
        layout = &lbl_803A9A60.players[player];
        for (slot = 0; slot < 6; slot++) {
            pokemon =
                toolentryTaisenGetEntryPokemonPtr(player, slot);
            if (pokemon != NULL) {
                layout->maxHp[slot] = pokemonBiosGetMaxHp(pokemon);
                layout->hp[slot] = pokemonBiosGetHp(pokemon);
                layout->maxHpDisplay[slot] =
                    pokemonBiosGetMaxHp(pokemon);
            } else {
                layout->maxHp[slot] = lbl_8047BF60;
                layout->hp[slot] = lbl_8047BF60;
                layout->maxHpDisplay[slot] = lbl_8047BFAC;
            }
        }
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
void menuCBBattleStartInit(void* menu, s32 mode)
{
    extern s32 toolentryTaisenGetBattleType(void);
    extern void* toolentryTaisenGetHeroPtr(s32);
    extern void* heroBiosGetNamePtr(void*);
    extern void GScharCpy(void*, const void*);
    extern void* GSmsgGetGSchar(s32);
    extern s32 toolentryTaisenGetTrainerDataID(s32);
    extern void* fightTrainerDataBiosGetPtr(u16);
    extern u16 fightTrainerDataBiosGetKindDataId(void*);
    extern void* fightTrainerKindDataBiosGetPtr(u16);
    extern u32 fightTrainerKindDataBiosGetPrefixName(void*);
    extern s32 fn_8025DBB0(void);
    extern void _menuCBBattleStartSetIndex__Fv(void);
    extern void fn_80062334(void);
    extern void fn_80068F84(void);
    extern void menuCBPokemonEntryLoadTex(void);
    extern s32 fn_8025D9A8(void);
    extern void std104_soundSetBackgroundMusic(u32, s32, s32);
    extern const u32 lbl_802ED958[];
    extern const u32 lbl_802ED978[];
    extern const f32 lbl_8047BF60;
    void* trainer;
    void* name;
    void* trainerData;
    void* trainerKind;
    s32 battleId;
    u8* dst;

    if (battleStartTypeCompatible() != 0) {
        trainer = toolentryTaisenGetHeroPtr(1);
        name = heroBiosGetNamePtr(trainer);
        dst = lbl_803A9A60.model.trainerName;
        if (dst != NULL) {
            GScharCpy(dst, name);
        } else {
            GScharCpy(dst, GSmsgGetGSchar(1));
        }
        trainerData = fightTrainerDataBiosGetPtr(
            toolentryTaisenGetTrainerDataID(1));
        trainerKind = fightTrainerKindDataBiosGetPtr(
            fightTrainerDataBiosGetKindDataId(trainerData));
        lbl_803A9A60.model.trainerPrefix =
            fightTrainerKindDataBiosGetPrefixName(trainerKind);
    }

    battleId = fn_8025DBB0();
    lbl_803A9A60.model.battleId = battleId;
    lbl_803A9A60.menu = menu;
    lbl_803A9A60.status = mode;
    lbl_803A9A60.timer = 0;
    _menuCBBattleStartSetIndex__Fv();
    fn_80062334();
    lbl_803A9A60.model.scale = lbl_8047BF60;

    switch (mode) {
    case 0:
        fn_80068F84();
        menuCBPokemonEntryLoadTex();
        battleId = fn_8025DBB0();
        switch (fn_8025D9A8()) {
        case 0:
            lbl_803A9A60.model.soundId = lbl_802ED958[battleId];
            break;
        case 1:
            lbl_803A9A60.model.soundId =
                lbl_802ED978[battleId % 10];
            break;
        case 2:
            lbl_803A9A60.model.soundId = 0x3CD;
            break;
        case 3:
            lbl_803A9A60.model.soundId = 0x3CD;
            break;
        default:
            lbl_803A9A60.model.soundId = 0x3CD;
            break;
        }
        std104_soundSetBackgroundMusic(lbl_803A9A60.model.soundId, 0, 0xFF);
        break;
    case 1:
        fn_80068F84();
        menuCBPokemonEntryLoadTex();
        battleStartInitHp();
        lbl_803A9A60.model.soundId = 0x1E;
        std104_soundSetBackgroundMusic(0x1E, 0, 0xFF);
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80062284_ONLY)
s32 fn_80062284(s32 trainer) {
    extern u16 toolentryTaisenGetEntryPokemonNum(s32 trainer);
    extern void* toolentryTaisenGetEntryPokemonPtr(s32 trainer, s32 index);
    extern u8 pokemonCheckValid(void* pokemon);
    extern u8 pokemonGetStatus(void* pokemon, s32 index, s32 status, s32 subindex);
    void* pokemon;
    u16 count;
    s32 i;

    i = toolentryTaisenGetEntryPokemonNum(trainer);
    count = i;
    for (i = 0; i < count; i++) {
        pokemon = toolentryTaisenGetEntryPokemonPtr(trainer, i);
        if (pokemon != 0 && pokemonCheckValid(pokemon) &&
            pokemonGetStatus(pokemon, 0, 0x7B, 0) == 1) {
            return 0;
        }
    }
    return 1;
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
void fn_80062334(void)
{
    extern s32 toolentryTaisenGetBattleType(void);
    extern const f32 lbl_8047BFA4;
    extern const f32 lbl_8047BFA8;
    extern const f32 lbl_8047BFB0;
    extern const f32 lbl_8047BFB4;
    extern const f32 lbl_8047BF68;
    extern const f32 lbl_8047BF70;
    extern const f32 lbl_8047BF90;
    extern const f32 lbl_8047BFB8;
    extern const f32 lbl_8047BF60;
    extern const f32 lbl_8047BFBC;
    extern const f32 lbl_8047BFC0;
    extern const f32 lbl_8047BFC4;
    s32 battleType = toolentryTaisenGetBattleType();
    u32* orderGroups[4];
    f32 forward[6];
    /* RULE-EXCEPTION(user-approved): 7th element is unused; it reproduces retail's stack slot at 0x30 (forward at 0x34) — see docs/RULE_EXCEPTIONS.md */
    f32 reverse[7];
    MenuCBBattleStartPosition* position;
    s32 slot;
    s32 destination;
    f32* rev;
    MenuCBBattleStartState* state;
    f32* fwd;
    s32 player;
    MenuCBBattleStartPlayerView* view;

    orderGroups[0] = (u32*)lbl_803A9E40;
    orderGroups[1] = (u32*)(lbl_803A9E40 + 0x18);
    orderGroups[2] = (u32*)(lbl_803A9E40 + 0x30);
    orderGroups[3] = (u32*)(lbl_803A9E40 + 0x48);
    forward[0] = lbl_8047BFA4;
    forward[1] = lbl_8047BFA8;
    forward[2] = lbl_8047BFB0;
    forward[3] = lbl_8047BFB4;
    forward[4] = lbl_8047BF68;
    forward[5] = lbl_8047BFB8;
    reverse[0] = lbl_8047BFB8;
    reverse[1] = lbl_8047BF68;
    reverse[2] = lbl_8047BFB4;
    reverse[3] = lbl_8047BFB0;
    reverse[4] = lbl_8047BFA8;
    reverse[5] = lbl_8047BFA4;
    state = &lbl_803A9A60;
    fwd = forward;
    rev = reverse;

    for (player = 0; player < 4; player++) {
        view = &state->players[player].view;
        if (battleType == 2) {
            for (slot = 0; slot < 6; slot++) {
                switch (state->status) {
                case 0:
                    view->header.marker[slot] = 3;
                    view->reset[slot] = lbl_8047BF60;
                    break;
                case 1:
                    view->header.marker[slot] = 0;
                    view->reset[slot] = lbl_8047BF60;
                    break;
                }
                if (player < 2) {
                    view->side[slot] = lbl_8047BFBC;
                    view->alpha[slot] = lbl_8047BF60;
                    view->position[slot] = fwd[slot];
                } else {
                    view->side[slot] = lbl_8047BFC0;
                    view->alpha[slot] = lbl_8047BF60;
                    view->position[slot] = rev[slot];
                }
            }
        } else {
            for (slot = 0; slot < 6; slot++) {
                destination = orderGroups[player][slot];
                switch (state->status) {
                case 0:
                    view->header.marker[slot] = 3;
                    view->reset[slot] = lbl_8047BF60;
                    break;
                case 1:
                    view->header.marker[slot] = 0;
                    view->reset[slot] = lbl_8047BF60;
                    break;
                }
                if (player % 2 != 0) {
                    view->side[slot] = lbl_8047BFC0;
                    view->alpha[slot] = lbl_8047BF60;
                    view->position[destination] = reverse[3 + slot % 3];
                } else {
                    view->side[slot] = lbl_8047BFBC;
                    view->alpha[slot] = lbl_8047BF60;
                    view->position[destination] = forward[slot % 3];
                }
            }
        }
    }

    for (player = 0; player < 4; player++) {
        position = &state->trainerPositions[player];
        if (battleType == 2) {
            if (player < 2) {
                position->y = lbl_8047BFBC;
                position->z = lbl_8047BF60;
                position->x = lbl_8047BFC4;
            } else {
                position->y = lbl_8047BFC0;
                position->z = lbl_8047BF60;
                position->x = lbl_8047BFC4;
            }
        } else if (player % 2 != 0) {
            position->y = lbl_8047BFC0;
            position->z = lbl_8047BF60;
            position->x = lbl_8047BFC4;
        } else {
            position->y = lbl_8047BFBC;
            position->z = lbl_8047BF60;
            position->x = lbl_8047BFC4;
        }
    }

    lbl_803A9A60.field368 = 0;
    state->field358 = lbl_8047BF70;
    state->field35C = lbl_8047BF90;
    lbl_803A9A60.field360 = lbl_8047BF70;
    lbl_803A9A60.field364 = lbl_8047BF90;
    lbl_803A9A60.transitions.target[1] = lbl_8047BF60;
    lbl_803A9A60.transitions.current[1] = lbl_8047BFBC;
    lbl_803A9A60.transitions.active[1] = lbl_8047BF60;
    lbl_803A9A60.transitions.target[0] = lbl_8047BF60;
    lbl_803A9A60.transitions.current[0] = lbl_8047BFC0;
    lbl_803A9A60.transitions.active[0] = lbl_8047BF60;
}
#endif

static inline void battleStartInterpolate(f32* current, f32* target, f32 delta)
{
    extern const f32 lbl_8047BF60;
    extern const f32 lbl_8047BF90;
    extern const f32 lbl_8047BF94;
    extern const f32 lbl_8047BF98;
    extern const f32 lbl_8047BF9C;
    f32 step;
    f32 distance;

    step = lbl_8047BF94 * (*target - *current);
    step *= delta;
    if (step > lbl_8047BF98) {
        step = lbl_8047BF98;
    }
    if (step <= lbl_8047BF9C) {
        step = lbl_8047BF9C;
    }

    *current += step;
    distance = *target - *current;
    if (battleStartAbs(distance) <= battleStartAbs(step) ||
        battleStartAbs(distance) < lbl_8047BF90) {
        *current = *target;
    }
}

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT)
void fn_80060A28(void)
{
    extern const f32 lbl_8047BF60;
    MenuCBBattleStartState* state;
    MenuCBBattleStartPlayerView* view;
    MenuCBBattleStartPosition* position;
    s32 player;
    s32 slot;
    s32 index;

    state = &lbl_803A9A60;
    for (player = 0; player < 4; player++) {
        view = &state->players[player].view;
        for (slot = 0; slot < 6; slot++) {
            if (lbl_8047BF60 != view->position[slot]) {
                view->position[slot] -= state->deltaTime;
                if (view->position[slot] < lbl_8047BF60) {
                    view->position[slot] = lbl_8047BF60;
                }
            } else if (view->side[slot] != view->alpha[slot]) {
                battleStartInterpolate(&view->side[slot], &view->alpha[slot],
                                       state->deltaTime);
            }
        }
    }

    for (player = 0; player < 4; player++) {
        position = &state->trainerPositions[player];
        if (lbl_8047BF60 != position->x) {
            position->x -= state->deltaTime;
            if (position->x < lbl_8047BF60) {
                position->x = lbl_8047BF60;
            }
        } else if (position->y != position->z) {
            battleStartInterpolate(&position->y, &position->z,
                                   state->deltaTime);
        }
    }

    for (index = 0; index < 2; index++) {
        if (lbl_8047BF60 != state->transitions.active[index]) {
            /* Retail decrements the slot-indexed entry (slot is 6 here). */
            lbl_803A9A60.transitions.active[slot] -= state->deltaTime;
            if (lbl_803A9A60.transitions.active[slot] < lbl_8047BF60) {
                lbl_803A9A60.transitions.active[slot] = lbl_8047BF60;
            }
        } else if (state->transitions.current[index] !=
                   state->transitions.target[index]) {
            battleStartInterpolate(&state->transitions.current[index],
                                   &state->transitions.target[index],
                                   state->deltaTime);
        }
    }
}
#endif

void fn_80060D70(void*, UICmdMsg*, s32, s32);
void fn_80060EF4(void*, UICmdMsg*, s32);
void fn_8006106C(void*, UICmdMsg*, s32, s32, s32);
void fn_80061240(void*, UICmdMsg*, s32, s32, s32);
void fn_80061454(void*, UICmdMsg*, s32, s32);
void fn_800615F4(void*, UICmdMsg*, s32, s32);
void fn_800617E0(void*, UICmdMsg*, s32, s32);
void fn_80061A2C(void*, UICmdMsg*, s32, s32, s32);
void fn_80061BBC(void*, UICmdMsg*, s32, s32, s32);
u8 fn_80061D34(void*, UICmdMsg*, s32, s32, s32);

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8005E7F0_ONLY)
/* Battle-start command dispatcher. */
void fn_8005E7F0(void* ctx, void* arg1)
{
    UICmdMsg* msg = (UICmdMsg*) arg1;
    u32* tbl = (u32*) lbl_803A9E40;

    switch (msg->cmd) {
    case 0x8A6:
        fn_800608C4(ctx, (MenuCBBattleStartDrawParams*)msg);
        break;
    case 0xC01:
        fn_8006106C(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC02:
        fn_8006106C(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC03:
        fn_8006106C(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC04:
        fn_8006106C(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC05:
        fn_8006106C(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC06:
        fn_8006106C(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xC0D:
        msg->flags4 &= ~2;
        break;
    case 0xC0E:
        msg->flags4 &= ~2;
        break;
    case 0xC0F:
        msg->flags4 &= ~2;
        break;
    case 0xC10:
        msg->flags4 &= ~2;
        break;
    case 0xC11:
        msg->flags4 &= ~2;
        break;
    case 0xC12:
        msg->flags4 &= ~2;
        break;
    case 0xC13:
        fn_80061A2C(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC14:
        fn_80061A2C(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC15:
        fn_80061A2C(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC16:
        fn_80061A2C(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC17:
        fn_80061A2C(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC18:
        fn_80061A2C(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xC19:
        fn_80061BBC(ctx, msg, 0, tbl[0], 2);
        break;
    case 0xC1A:
        fn_80061BBC(ctx, msg, 0, tbl[1], 2);
        break;
    case 0xC1B:
        fn_80061BBC(ctx, msg, 0, tbl[2], 2);
        break;
    case 0xC1C:
        fn_80061BBC(ctx, msg, 0, tbl[3], 2);
        break;
    case 0xC1D:
        fn_80061BBC(ctx, msg, 0, tbl[4], 2);
        break;
    case 0xC1E:
        fn_80061BBC(ctx, msg, 0, tbl[5], 2);
        break;
    case 0xDB5:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[0], 2);
        break;
    case 0xC1F:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[1], 2);
        break;
    case 0xC20:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[2], 2);
        break;
    case 0xC21:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[3], 2);
        break;
    case 0xC22:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[4], 2);
        break;
    case 0xC23:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 0, tbl[5], 2);
        break;
    case 0xC27:
        fn_8006106C(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC28:
        fn_8006106C(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC29:
        fn_8006106C(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC2A:
        fn_8006106C(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC2B:
        fn_8006106C(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC2C:
        fn_8006106C(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xC33:
        msg->flags4 &= ~2;
        break;
    case 0xC34:
        msg->flags4 &= ~2;
        break;
    case 0xC35:
        msg->flags4 &= ~2;
        break;
    case 0xC36:
        msg->flags4 &= ~2;
        break;
    case 0xC37:
        msg->flags4 &= ~2;
        break;
    case 0xC38:
        msg->flags4 &= ~2;
        break;
    case 0xC39:
        fn_80061A2C(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC3A:
        fn_80061A2C(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC3B:
        fn_80061A2C(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC3C:
        fn_80061A2C(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC3D:
        fn_80061A2C(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC3E:
        fn_80061A2C(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xC3F:
        fn_80061BBC(ctx, msg, 1, tbl[6], 2);
        break;
    case 0xC40:
        fn_80061BBC(ctx, msg, 1, tbl[7], 2);
        break;
    case 0xC41:
        fn_80061BBC(ctx, msg, 1, tbl[8], 2);
        break;
    case 0xC42:
        fn_80061BBC(ctx, msg, 1, tbl[9], 2);
        break;
    case 0xC43:
        fn_80061BBC(ctx, msg, 1, tbl[10], 2);
        break;
    case 0xC44:
        fn_80061BBC(ctx, msg, 1, tbl[11], 2);
        break;
    case 0xDB4:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[6], 2);
        break;
    case 0xC45:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[7], 2);
        break;
    case 0xC46:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[8], 2);
        break;
    case 0xC47:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[9], 2);
        break;
    case 0xC48:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[10], 2);
        break;
    case 0xC49:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 1, tbl[11], 2);
        break;
    case 0xC4D:
        fn_8006106C(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC4E:
        fn_8006106C(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC4F:
        fn_8006106C(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC50:
        fn_8006106C(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC51:
        fn_8006106C(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC52:
        fn_8006106C(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xC59:
        msg->flags4 &= ~2;
        break;
    case 0xC5A:
        msg->flags4 &= ~2;
        break;
    case 0xC5B:
        msg->flags4 &= ~2;
        break;
    case 0xC5C:
        msg->flags4 &= ~2;
        break;
    case 0xC5D:
        msg->flags4 &= ~2;
        break;
    case 0xC5E:
        msg->flags4 &= ~2;
        break;
    case 0xC5F:
        fn_80061A2C(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC60:
        fn_80061A2C(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC61:
        fn_80061A2C(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC62:
        fn_80061A2C(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC63:
        fn_80061A2C(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC64:
        fn_80061A2C(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xC65:
        fn_80061BBC(ctx, msg, 2, tbl[12], 2);
        break;
    case 0xC66:
        fn_80061BBC(ctx, msg, 2, tbl[13], 2);
        break;
    case 0xC67:
        fn_80061BBC(ctx, msg, 2, tbl[14], 2);
        break;
    case 0xC68:
        fn_80061BBC(ctx, msg, 2, tbl[15], 2);
        break;
    case 0xC69:
        fn_80061BBC(ctx, msg, 2, tbl[16], 2);
        break;
    case 0xC6A:
        fn_80061BBC(ctx, msg, 2, tbl[17], 2);
        break;
    case 0xDAF:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[12], 2);
        break;
    case 0xC6B:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[13], 2);
        break;
    case 0xC6C:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[14], 2);
        break;
    case 0xC6D:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[15], 2);
        break;
    case 0xC6E:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[16], 2);
        break;
    case 0xC6F:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 2, tbl[17], 2);
        break;
    case 0xC73:
        fn_8006106C(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC74:
        fn_8006106C(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC75:
        fn_8006106C(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC76:
        fn_8006106C(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC77:
        fn_8006106C(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC78:
        fn_8006106C(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xC7F:
        msg->flags4 &= ~2;
        break;
    case 0xC80:
        msg->flags4 &= ~2;
        break;
    case 0xC81:
        msg->flags4 &= ~2;
        break;
    case 0xC82:
        msg->flags4 &= ~2;
        break;
    case 0xC83:
        msg->flags4 &= ~2;
        break;
    case 0xC84:
        msg->flags4 &= ~2;
        break;
    case 0xC85:
        fn_80061A2C(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC86:
        fn_80061A2C(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC87:
        fn_80061A2C(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC88:
        fn_80061A2C(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC89:
        fn_80061A2C(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC8A:
        fn_80061A2C(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xC8B:
        fn_80061BBC(ctx, msg, 3, tbl[18], 2);
        break;
    case 0xC8C:
        fn_80061BBC(ctx, msg, 3, tbl[19], 2);
        break;
    case 0xC8D:
        fn_80061BBC(ctx, msg, 3, tbl[20], 2);
        break;
    case 0xC8E:
        fn_80061BBC(ctx, msg, 3, tbl[21], 2);
        break;
    case 0xC8F:
        fn_80061BBC(ctx, msg, 3, tbl[22], 2);
        break;
    case 0xC90:
        fn_80061BBC(ctx, msg, 3, tbl[23], 2);
        break;
    case 0xDB3:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[18], 2);
        break;
    case 0xC91:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[19], 2);
        break;
    case 0xC92:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[20], 2);
        break;
    case 0xC93:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[21], 2);
        break;
    case 0xC94:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[22], 2);
        break;
    case 0xC95:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg, 3, tbl[23], 2);
        break;
    case 0xDB6:
        fn_8006106C(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDB7:
        fn_8006106C(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDB8:
        fn_8006106C(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDB9:
        msg->flags4 &= ~2;
        break;
    case 0xDBA:
        msg->flags4 &= ~2;
        break;
    case 0xDBB:
        msg->flags4 &= ~2;
        break;
    case 0xDBC:
        fn_80061A2C(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDBD:
        fn_80061A2C(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDBE:
        fn_80061A2C(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDBF:
        fn_80061BBC(ctx, msg, 0, tbl[0], 0);
        break;
    case 0xDC0:
        fn_80061BBC(ctx, msg, 0, tbl[1], 0);
        break;
    case 0xDC1:
        fn_80061BBC(ctx, msg, 0, tbl[2], 0);
        break;
    case 0xDC2:
        fn_8006106C(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDC3:
        fn_8006106C(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDC4:
        fn_8006106C(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDC5:
        msg->flags4 &= ~2;
        break;
    case 0xDC6:
        msg->flags4 &= ~2;
        break;
    case 0xDC7:
        msg->flags4 &= ~2;
        break;
    case 0xDC8:
        fn_80061A2C(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDC9:
        fn_80061A2C(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDCA:
        fn_80061A2C(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDCB:
        fn_80061BBC(ctx, msg, 1, tbl[6], 0);
        break;
    case 0xDCC:
        fn_80061BBC(ctx, msg, 1, tbl[7], 0);
        break;
    case 0xDCD:
        fn_80061BBC(ctx, msg, 1, tbl[8], 0);
        break;
    case 0xDD8:
        fn_8006106C(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDD9:
        fn_8006106C(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDDA:
        fn_8006106C(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDE4:
        fn_8006106C(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDE5:
        fn_8006106C(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDE6:
        fn_8006106C(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDDB:
        msg->flags4 &= ~2;
        break;
    case 0xDDC:
        msg->flags4 &= ~2;
        break;
    case 0xDDD:
        msg->flags4 &= ~2;
        break;
    case 0xDE7:
        msg->flags4 &= ~2;
        break;
    case 0xDE8:
        msg->flags4 &= ~2;
        break;
    case 0xDE9:
        msg->flags4 &= ~2;
        break;
    case 0xDDE:
        fn_80061A2C(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDDF:
        fn_80061A2C(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDE0:
        fn_80061A2C(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDEA:
        fn_80061A2C(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDEB:
        fn_80061A2C(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDEC:
        fn_80061A2C(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDE1:
        fn_80061BBC(ctx, msg, 0, tbl[0], 1);
        break;
    case 0xDE2:
        fn_80061BBC(ctx, msg, 0, tbl[1], 1);
        break;
    case 0xDE3:
        fn_80061BBC(ctx, msg, 0, tbl[2], 1);
        break;
    case 0xDED:
        fn_80061BBC(ctx, msg, 0, tbl[3], 1);
        break;
    case 0xDEE:
        fn_80061BBC(ctx, msg, 0, tbl[4], 1);
        break;
    case 0xDEF:
        fn_80061BBC(ctx, msg, 0, tbl[5], 1);
        break;
    case 0xDF3:
        msg->flags4 &= ~2;
        break;
    case 0xDF4:
        msg->flags4 &= ~2;
        break;
    case 0xDF5:
        msg->flags4 &= ~2;
        break;
    case 0xDFF:
        msg->flags4 &= ~2;
        break;
    case 0xE00:
        msg->flags4 &= ~2;
        break;
    case 0xE01:
        msg->flags4 &= ~2;
        break;
    case 0xDF6:
        fn_80061A2C(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDF7:
        fn_80061A2C(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDF8:
        fn_80061A2C(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xE02:
        fn_80061A2C(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xE03:
        fn_80061A2C(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xE04:
        fn_80061A2C(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDF0:
        fn_8006106C(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDF1:
        fn_8006106C(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDF2:
        fn_8006106C(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xDFC:
        fn_8006106C(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xDFD:
        fn_8006106C(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xDFE:
        fn_8006106C(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDF9:
        fn_80061BBC(ctx, msg, 1, tbl[6], 1);
        break;
    case 0xDFA:
        fn_80061BBC(ctx, msg, 1, tbl[7], 1);
        break;
    case 0xDFB:
        fn_80061BBC(ctx, msg, 1, tbl[8], 1);
        break;
    case 0xE05:
        fn_80061BBC(ctx, msg, 1, tbl[9], 1);
        break;
    case 0xE06:
        fn_80061BBC(ctx, msg, 1, tbl[10], 1);
        break;
    case 0xE07:
        fn_80061BBC(ctx, msg, 1, tbl[11], 1);
        break;
    case 0xDD4:
        fn_80060D70(ctx, msg, 0, 0);
        break;
    case 0xDD5:
        fn_80060D70(ctx, msg, 0, 1);
        break;
    case 0xDD6:
        fn_80060D70(ctx, msg, 1, 0);
        break;
    case 0xDD7:
        fn_80060D70(ctx, msg, 1, 1);
        break;
    case 0x102C:
        fn_80060D70(ctx, msg, 1, 2);
        break;
    case 0x102D:
        fn_80060D70(ctx, msg, 1, 2);
        break;
    case 0xBF1:
    case 0xBF2:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0xBF3:
    case 0xBF4:
        fn_80060EF4(ctx, msg, 6);
        break;
    case 0xBF5:
        fn_80060EF4(ctx, msg, -1);
        break;
    case 0xBF6:
    case 0xBF7:
        fn_80060EF4(ctx, msg, 3);
        break;
    case 0xBF8:
    case 0xBF9:
        fn_80060EF4(ctx, msg, 4);
        break;
    case 0xBFA:
    case 0xBFB:
        fn_80060EF4(ctx, msg, 2);
        break;
    case 0xBFC:
    case 0xBFD:
        fn_80060EF4(ctx, msg, 1);
        break;
    case 0xBFE:
        fn_80060EF4(ctx, msg, 0);
        break;
    case 0xDD0:
        fn_800617E0(ctx, msg, 0, 0);
        break;
    case 0xDD1:
        fn_800617E0(ctx, msg, 1, 0);
        break;
    case 0xDCF:
        fn_800615F4(ctx, msg, 0, 0);
        break;
    case 0xDCE:
        fn_800615F4(ctx, msg, 1, 0);
        break;
    case 0xDD3:
        fn_80061454(ctx, msg, 0, 0);
        break;
    case 0xDD2:
        fn_80061454(ctx, msg, 1, 0);
        break;
    case 0xC26:
        fn_800617E0(ctx, msg, 0, 2);
        break;
    case 0xC4C:
        fn_800617E0(ctx, msg, 1, 2);
        break;
    case 0xC72:
        fn_800617E0(ctx, msg, 2, 2);
        break;
    case 0xC98:
        fn_800617E0(ctx, msg, 3, 2);
        break;
    case 0xC24:
        fn_80061454(ctx, msg, 0, 2);
        break;
    case 0xC4A:
        fn_80061454(ctx, msg, 1, 2);
        break;
    case 0xC70:
        fn_80061454(ctx, msg, 2, 2);
        break;
    case 0xC96:
        fn_80061454(ctx, msg, 3, 2);
        break;
    case 0xC25:
        fn_800615F4(ctx, msg, 0, 2);
        break;
    case 0xC4B:
        fn_800615F4(ctx, msg, 1, 2);
        break;
    case 0xC71:
        fn_800615F4(ctx, msg, 2, 2);
        break;
    case 0xC97:
        fn_800615F4(ctx, msg, 3, 2);
        break;
    case 0xBFF:
        fn_800609B4(ctx, (MenuCBBattleStartSprite*)msg, *(f32*) ((u8*)&lbl_803A9A60 + 0x48));
        break;
    case 0xC00:
        fn_800609B4(ctx, (MenuCBBattleStartSprite*)msg, *(f32*) ((u8*)&lbl_803A9A60 + 0x4c));
        break;
    case 0x1096:
        fn_80060434(ctx, msg);
        break;
    }
}
#endif

extern u8 fn_80061D34(void*, UICmdMsg*, s32, s32, s32);
extern u8 fn_80069A08(void*, UICmdMsg*, s32, s32);
extern u16 toolentryTaisenGetEntryPokemonNum(s32);
extern u16 toolentryTaisenGetPokemonNum(s32);
extern s32 fn_8025D9A8(void);
extern s32 fn_8025D9CC(void);
extern s32 toolentryTaisenGetBattleType(void);
extern u16 toolentryTaisenGetTrainerDataID(s32);
extern u16 toolentryTaisenGetHeroPtr(s32);
extern u16 toolentryTaisenGetBattlePlayerID(s32);
extern u16 std103_battleGetResult(void);
extern void* heroBiosGetNamePtr(u16);
extern void* GSmsgGetGSchar(s32);
extern void msgctrlSetValue();
extern void fn_800FB680();
extern void fn_800FBB34();
extern void fn_800FE6D0(s32, s32);
extern void spriteSetEnv(void);
extern void fn_801040F0();
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(void*);
extern void gfxLoadTexture(s32, void*);
extern void gfxBegin(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D5BA0(s32, u32);
extern void gfxVtxTexCoord(s32, f32, f32);
extern void gfxEnd(void);
extern void fn_801FCCC4(u16);
extern void fn_801FCC64(void);
extern void fn_801FBD58(void);
extern void fn_801FBD28(void);
extern u16 lbl_80478910[4];
extern s16 lbl_80478918;
extern s16 lbl_8047891A;
extern f32 lbl_8047891C;
extern u8 lbl_802EF0A8[];
extern u8 lbl_80314E08[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047BF60;
extern f32 lbl_8047BF68;
extern f32 lbl_8047BF90;
extern f32 lbl_8047BFA8;

static inline void menuCBBattleStartPlace(
    void* context, UICmdMsg* msg, f32 offset)
{
    u8* menu = context;
    u8* entry = &lbl_802EF0A8[msg->cmd * 0x1C];

    msg->field50 = (s16)(*(s16*)(entry + 2) + (s32)offset);
    fn_800FE6D0((s16)(*(s16*)(menu + 0x84) + msg->field50),
                (s16)(*(s16*)(menu + 0x86) + msg->field52));
    spriteSetEnv();
}

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060D70_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
void fn_80060D70(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    extern const u32 lbl_8047BF50;
    extern const u32 lbl_8047BF54;
    extern const f32 lbl_8047BFA0;
    s32 expected[2];
    f32 scale;
    f32 fade;

    expected[0] = lbl_8047BF50;
    expected[1] = lbl_8047BF54;
    if (lbl_803A9A60.status == 1) {
        switch (std103_battleGetResult()) {
        case 2:
        case 5:
            expected[0] = 0;
            expected[1] = 1;
            break;
        case 3:
        case 4:
            expected[0] = 1;
            expected[1] = 0;
            break;
        case 6:
        case 7:
            expected[0] = 2;
            expected[1] = 2;
            break;
        default:
            expected[0] = 2;
            expected[1] = 2;
            break;
        }
        if (lbl_803A9A60.timer >= 6) {
            if (kind == expected[player]) {
                scale = *(f32*)((u8*)&lbl_803A9A60 + 0x358 + player * 8);
                fade = lbl_8047BF90;
                fade -= scale - lbl_8047BF90;
                msg->alpha67 = lbl_8047BFA0 * fade;
                msg->scale68 = scale;
                msg->scale6C = scale;
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        } else {
            msg->flags4 &= ~2;
        }
    } else {
        msg->flags4 &= ~2;
    }
}
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060D70_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
void fn_80060EF4(void* context, UICmdMsg* msg, s32 index)
{
    s32 count = *(s32*)((u8*)lbl_803A9A60.menu + 0xC);
    s32 mode = fn_8025D9A8();
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
    if (index < 0) {
        if (mode == 1) {
            msg->flags4 |= 2;
        } else {
            if (index == count) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        }
    } else if (mode == 1) {
        msg->flags4 &= ~2;
    } else {
        if (count == 5) {
            if (index == 3) {
                msg->flags4 |= 2;
            } else {
                msg->flags4 &= ~2;
            }
        } else if (index == count) {
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
    }
}
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060D70_ONLY)
s32 menuCBBattleStartGetStatus(void) {
    MenuCBBattleStartState* state = &lbl_803A9A60;
    return state->status;
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80060D70_ONLY)
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
void fn_80061028(s32 status) {
    extern void menuCloseCustom(s32 menuId, s32 arg1, s32 arg2);

    menuCloseCustom(0xBA, 0, 1);
    lbl_803A9A60.status = status;
}
#if !defined(MENUCB_BATTLESTART_80060D70_ONLY)
#endif
#endif

typedef struct MenuCBBattleStartGroup {
    s16 count[6];
    f32 phase[12];
    f32 offset[18];
    f32 wait[12];
} MenuCBBattleStartGroup;

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8006106C) — see docs/RULE_EXCEPTIONS.md */
static inline u8* menuCBBattleStartGroupPtr(s32 player)
{
    return (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;
}

/* Shared by fn_8006106C and fn_80061BBC. Read through an inline call, the
 * offset is evaluated before fn_800609B4's inlined body, which numbers its
 * address temp before the sprite-table temps as retail's colouring needs. */
static inline f32 menuCBBattleStartGroupOffset(u8* group, s32 slot)
{
    return ((f32*)(group + 0x3C))[slot];
}

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8006106C_ONLY)
void fn_8006106C(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    extern const f32 lbl_8047BFA4;
    extern u8 menuCBPokemonEntryDispPokemonFace(void*, UICmdMsg*, s32, s32);
    extern void windowDrawSprite(s32 x, s32 y, void* context, s32 id, s32 flags);
    u8* group;

    if (fn_80061D34(context, msg, player, slot, kind)) {
        group = menuCBBattleStartGroupPtr(player);
        fn_800609B4(context, (MenuCBBattleStartSprite*)msg,
                    menuCBBattleStartGroupOffset(group, slot));
        if (menuCBPokemonEntryDispPokemonFace(context, msg, player, slot)) {
            if (((s16*)group)[slot] != 0) {
                windowDrawSprite(0, 0, context, lbl_80478910[((s16*)group)[slot]], 0);
                if (lbl_803A9A60.timer == 3) {
                    ((f32*)(group + 0xC))[slot] += *(f32*)((u8*)&lbl_803A9A60 + 0x3C);
                    if (((f32*)(group + 0xC))[slot] >= lbl_8047BFA4) {
                        ((f32*)(group + 0xC))[slot] = lbl_8047BF60;
                        ((s16*)group)[slot]--;
                    }
                }
            }
        } else if (((s16*)group)[slot] != 0) {
            windowDrawSprite(0, 0, context, lbl_80478910[((s16*)group)[slot]], 0);
        }
    }
    if (fn_80061D34(context, msg, player, slot, kind)) {
        windowDrawSprite(-8, -8, context, 0x40, 0);
    }
}
#endif

static inline void menuCBBattleStartDrawGauge(
    UICmdMsg* msg, u8 r, u8 g, u8 b, u8 a)
{
    u32 color;
    f32 end;

    color = ((u8)(r - 2) << 24) | ((u8)(g - 2) << 16) | ((u8)(b - 2) << 8) | a;
    fn_800D5BA0(0, color);
    fn_800D61E4(end = lbl_8047891C * (msg->field54 - lbl_80478918) + lbl_80478918,
                lbl_8047891A);
    fn_800D5BA0(0, color);
    fn_800D61E4(lbl_80478918, (s16)(msg->field56 - lbl_8047891A));
    color = (r << 24) | (g << 16) | (b << 8) | a;
    fn_800D5BA0(0, color);
    fn_800D61E4(end, (s16)(msg->field56 - lbl_8047891A));
    fn_800D5BA0(0, color);
}

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_8006106C_ONLY)
void fn_80061240(void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    u8* group = (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;
    /* RULE-EXCEPTION(user-approved): alpha read as int and copied to a u8 carrier after the ratio store, chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
    u8 gaugeAlpha;
    int alpha = *((const u8*)context + 0x8B);
    f32 ratio;
    u8 red;
    u8 green;
    u8 blue;

    lbl_8047891C = ratio =
        ((f32*)(group + 0x6C))[slot] / ((f32*)(group + 0x9C))[slot];
    gaugeAlpha = alpha;
    if (ratio <= lbl_8047BFA8) {
        red = 0xA7;
        green = 0x23;
        blue = 0x13;
    } else if (ratio <= lbl_8047BF68) {
        red = 0xC1;
        green = 0xBD;
        blue = 0x16;
    } else {
        red = 5;
        green = 0xB3;
        blue = 0x11;
    }
    if (lbl_8047BF60 != ratio) {
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetVtxDesc(lbl_80314E08);
        gfxSetPrimitive(4);
        gfxBegin(4);
        fn_800D61E4(lbl_80478918, lbl_8047891A);
        menuCBBattleStartDrawGauge(msg, red, green, blue, gaugeAlpha);
        gfxEnd();
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061454_ONLY)
void fn_80061454(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    u8 alpha = *((u8*)context + 0x8B);
    void** imageSlot = (void**)((u8*)&lbl_803A9A60 + 0x0C + player * 8);
    u8 visible = 1;
    void* image;
    s32 battleType = toolentryTaisenGetBattleType();

    if (kind == 2) {
        if (battleType != 2) {
            visible = 0;
        }
    } else if (battleType == 2) {
        visible = 0;
    }
    if (!visible) {
        return;
    }
    menuCBBattleStartPlace(context, msg,
        *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
    image = *imageSlot;
    if (image != NULL) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, image);
        gfxBegin(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BF60, lbl_8047BF60);
        fn_800D61E4(msg->field54, msg->field56);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BF90, lbl_8047BF90);
        gfxEnd();
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061454_ONLY)
void fn_800615F4(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    extern void* fightTrainerDataBiosGetPtr(s32 id);
    extern u16 fightTrainerDataBiosGetKindDataId(void* data);
    extern void* fightTrainerKindDataBiosGetPtr(u16 id);
    extern u32 fightTrainerKindDataBiosGetPrefixName(void* data);
    void* text;
    u16 trainerId;
    u32 nameId;
    s32 battleMode = fn_8025D9CC();
    u8 visible = 1;
    s32 battleType = toolentryTaisenGetBattleType();

    if (kind == 2) {
        if (battleType != 2) {
            visible = 0;
        }
    } else if (battleType == 2) {
        visible = 0;
    }
    if (!visible) {
        return;
    }
    menuCBBattleStartPlace(context, msg,
        *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
    if (battleMode == 4) {
        if (kind == 0) {
            msgctrlSetValue(0x34, toolentryTaisenGetBattlePlayerID(player) + 1);
            if (player == 0) {
                fn_800FBB34(0, 0, msg->field54, msg->field56,
                    0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E9);
            } else {
                fn_800FB680(0, 0,
                    0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E5);
            }
        }
        return;
    }
    trainerId = toolentryTaisenGetTrainerDataID(player);
    fightTrainerKindDataBiosGetPrefixName(fightTrainerKindDataBiosGetPtr(
        fightTrainerDataBiosGetKindDataId(fightTrainerDataBiosGetPtr(trainerId))));
    nameId = *(u32*)((u8*)&lbl_803A9A60 + 0x3DC);
    if (trainerId == 0) {
        text = GSmsgGetGSchar(1);
    } else {
        text = GSmsgGetGSchar(nameId);
    }
    msgctrlSetValue(0x37, text);
    msgctrlSetValue(0x4D, text);
    if (kind == 0 && player != 0) {
        fn_800FB680(0, 0,
            0xFFFFFF00 | *((u8*)context + 0x8B), 0xCF);
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061454_ONLY)
void fn_800617E0(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* text;
    u8 visible = 1;
    s32 battleType = toolentryTaisenGetBattleType();

    if (kind == 2) {
        if (battleType != 2) {
            visible = 0;
        }
    } else if (battleType == 2) {
        visible = 0;
    }
    if (!visible) {
        return;
    }
    menuCBBattleStartPlace(context, msg,
        *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
    text = heroBiosGetNamePtr(toolentryTaisenGetHeroPtr(player));
    if (text == NULL) {
        text = GSmsgGetGSchar(1);
    }
    if (fn_8025D9CC() == 4) {
        msgctrlSetValue(0x37, text);
        msgctrlSetValue(0x4D, text);
    } else if (player == 0) {
        msgctrlSetValue(0x37, text);
        msgctrlSetValue(0x4D, text);
    } else {
        text = (u8*)&lbl_803A9A60 + 0x3C4;
        msgctrlSetValue(0x37, text);
        msgctrlSetValue(0x4D, text);
    }
    if (kind == 0) {
        if (player == 0) {
            fn_800FBB34(0, 0, msg->field54, msg->field56,
                0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E2);
        } else {
            fn_800FB680(0, 0,
                0xFFFFFF00 | *((u8*)context + 0x8B), 0xCE);
        }
    } else {
        msgctrlSetValue(0x34, toolentryTaisenGetBattlePlayerID(player) + 1);
        if (player < 2) {
            fn_800FBB34(0, 0, msg->field54, msg->field56,
                0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E9);
            fn_800FBB34(0, 0x16, msg->field54, msg->field56,
                0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E8);
        } else {
            fn_800FB680(0, 0,
                0xFFFFFF00 | *((u8*)context + 0x8B), 0x30E7);
        }
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061454_ONLY)
void fn_80061A2C(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    extern void windowDrawSprite(s32 x, s32 y, void* context, s32 id, s32 flags);

    switch (lbl_803A9A60.status) {
    case 0:
        msg->flags4 &= ~2;
        break;
    case 1:
        if (fn_80061D34(context, msg, player, slot, kind)) {
            menuCBBattleStartPlace(context, msg,
                *(f32*)((u8*)&lbl_803A9A60 + player * 0xB4 + 0x94 + slot * 4));
            msg->flags4 &= ~2;
            windowDrawSprite(0, 0, context, 0x314, 0);
            fn_80061240(context, msg, player, slot, kind);
        } else {
            msg->flags4 &= ~2;
        }
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061454_ONLY)
void fn_80061B74(void* context, MenuCBBattleStartMessage* message,
                 s32 player, s32 slot, s32 kind) {
    switch (lbl_803A9A60.status) {
    case 0:
        message->flags &= ~2;
        break;
    case 1:
        message->flags &= ~2;
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061BBC_ONLY)
void fn_80061BBC(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    u8* group;

    switch (lbl_803A9A60.status) {
    case 0:
        msg->flags4 &= ~2;
        break;
    case 1:
        group = (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;
        fn_800609B4(context, (MenuCBBattleStartSprite*)msg,
                    menuCBBattleStartGroupOffset(group, slot));
        if (lbl_803A9A60.timer >= 5) {
            if (fn_80061D34(context, msg, player, slot, kind)) {
                if (lbl_8047BF60 == ((f32*)(group + 0x84))[slot]) {
                    msg->flags4 |= 2;
                } else {
                    msg->flags4 &= ~2;
                }
            } else {
                msg->flags4 &= ~2;
            }
        } else {
            msg->flags4 &= ~2;
        }
        break;
    }
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80061D34_ONLY)
u8 fn_80061D34(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    s32 selection;
    s32 mode = toolentryTaisenGetBattleType();
    u8 valid = 1;

    switch (lbl_803A9A60.status) {
    case 0:
        selection = toolentryTaisenGetPokemonNum(player);
        break;
    case 1:
        selection = toolentryTaisenGetEntryPokemonNum(player);
        break;
    }
    if (lbl_803A9A60.status == 0) {
        if (kind == 2) {
            if (mode != 2) {
                msg->flags4 &= ~2;
                valid = 0;
            }
        } else if (kind == 0) {
            if (selection >= 4) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (mode == 2) {
                msg->flags4 &= ~2;
                valid = 0;
            }
        } else {
            if (selection < 4) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (mode == 2) {
                msg->flags4 &= ~2;
                valid = 0;
            }
        }
    } else {
        if (kind == 2) {
            if (mode != 2) {
                msg->flags4 &= ~2;
                valid = 0;
            }
            if (selection <= slot) {
                valid = 0;
            }
        } else if (kind == 0) {
            if (selection >= 4) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (mode == 2) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (selection <= slot) {
                msg->flags4 &= ~2;
                valid = 0;
            }
        } else {
            if (selection < 4) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (mode == 2) {
                msg->flags4 &= ~2;
                valid = 0;
            } else if (selection <= slot) {
                msg->flags4 &= ~2;
                valid = 0;
            }
        }
    }
    return valid;
}
#endif

typedef struct MenuCBBattleStartTrainerTexture {
    void* texture;
    u32 resource;
} MenuCBBattleStartTrainerTexture;

typedef struct MenuCBBattleStartTrainerTextureState {
    u8 pad0[0xC];
    MenuCBBattleStartTrainerTexture entries[4];
    s32 current;
    s32 count;
    u8 active;
} MenuCBBattleStartTrainerTextureState;

extern s32 toolentryTaisenGetBattleType(void);
extern u32 toolentryGetTrainerBicFaceResID(s32, s32);
extern u32 toolentryGetTrainerSamllFaceResID(s32, s32);
extern void* fn_800F92D4(u32);
extern void fn_8017B000(
    u32, u32, void (*)(s32, MenuCBBattleStartTrainerTexture*),
    MenuCBBattleStartTrainerTexture*, u32);
extern void fn_8017B1CC(u32);
extern void fn_800F915C(u32);
extern void fn_800F9210(u32, u32);

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_800626CC_ONLY)
void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(
    s32 unused, MenuCBBattleStartTrainerTexture* completed)
{
    MenuCBBattleStartTrainerTextureState* state;
    MenuCBBattleStartTrainerTexture* entry;
    u32 resource;
    u32 imageId;
    s32 keepLoading;

    keepLoading = 1;
    if (completed != 0) {
        completed->texture = fn_800F92D4(completed->resource);
    }

    state = (MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60;
    do {
        if (state->current == state->count) {
            keepLoading = 0;
        } else {
            if (toolentryTaisenGetBattleType() != 2) {
                if (state->current % 2 != 0) {
                    resource = toolentryGetTrainerBicFaceResID(1, 0);
                } else {
                    resource = toolentryGetTrainerBicFaceResID(0, 1);
                }
            } else if (state->current < 2) {
                resource =
                    toolentryGetTrainerSamllFaceResID(state->current, 1);
            } else {
                resource =
                    toolentryGetTrainerSamllFaceResID(state->current, 0);
            }

            if (toolentryTaisenGetBattleType() != 2) {
                imageId = 0x5C3;
            } else {
                imageId = 0x5C4;
            }
            entry = &((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)
                         ->entries[state->current];
            entry->resource = resource;
            entry->texture = fn_800F92D4(entry->resource);
            if (entry->texture != 0) {
                state->current++;
            } else {
                fn_8017B000(
                    imageId, resource,
                    _menuCBBattleStartDispTrainerTexCallBack__FlPvl,
                    entry, resource);
                keepLoading = 0;
                state->current++;
            }
        }
    } while (keepLoading != 0);
}
#endif

#if !defined(MENUCB_BATTLESTART_SPLIT_UNIT) || defined(MENUCB_BATTLESTART_80062834_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(MENUCB_BATTLESTART_80062834_ONLY)
#endif
#if !defined(MENUCB_BATTLESTART_80062834_ONLY)
#endif
void menuCBBattleStartTrainerFaceFree(void)
{
    MenuCBBattleStartTrainerTextureState* state;
    MenuCBBattleStartTrainerTexture* entries;
    u32 imageId;
    MenuCBBattleStartTrainerTexture* entry;
    s32 i;

    if (toolentryTaisenGetBattleType() != 2) {
        imageId = 0x5C3;
    } else {
        imageId = 0x5C4;
    }
    fn_8017B1CC(imageId);
    fn_800F915C(imageId);

    entries = ((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)->entries;
    entry = entries;
    if (toolentryTaisenGetBattleType() == 2) {
        ((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)->count = 4;
    } else {
        ((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)->count = 2;
    }
    state = (MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60;
    for (i = 0; i < state->count; i++) {
        fn_800F9210(imageId, entry->resource);
        entry++;
    }

    ((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)->active = 0;
    ((MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60)->current = 0;
    if (toolentryTaisenGetBattleType() == 2) {
        state->count = 4;
    } else {
        state->count = 2;
    }
    entries[0].resource = 0;
    entries[0].texture = 0;
    entries[1].resource = 0;
    entries[1].texture = 0;
    entries[2].resource = 0;
    entries[2].texture = 0;
    entries[3].resource = 0;
    entries[3].texture = 0;
}
#if !defined(MENUCB_BATTLESTART_80062834_ONLY)
#endif
#endif
