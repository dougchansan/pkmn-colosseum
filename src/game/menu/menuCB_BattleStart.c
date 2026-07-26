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

typedef struct MenuCBBattleStartState {
    void* menu;
    s32 status;
    u16 menuId;
    u8 padA[0x2E];
    s32 timer;
    u8 pad3C[0x330];
    u8 model[0x74];
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

extern void fn_8005DFC8(void* arg);

#pragma push
#pragma peephole off
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
#pragma pop

void fn_8005E730(void* arg) {
    fn_8005DFC8(arg);
}

#pragma push
#pragma peephole off
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
#pragma pop

s32 menuCBBattleStartGetStatus(void) {
    MenuCBBattleStartState* state = &lbl_803A9A60;
    return state->status;
}

#pragma push
#pragma peephole off
void fn_80061028(s32 status) {
    extern void menuCloseCustom(s32 menuId, s32 arg1, s32 arg2);

    menuCloseCustom(0xBA, 0, 1);
    lbl_803A9A60.status = status;
}
#pragma pop

typedef struct MenuCBBattleStartDrawParams {
    u8 pad0[0x54];
    s16 x;
    s16 y;
} MenuCBBattleStartDrawParams;

#pragma push
#pragma scheduling off
#pragma peephole off
void fn_800608C4(void* context, MenuCBBattleStartDrawParams* params) {
    extern void* menuModelRender(void* model);
    extern void fn_800D88DC(s32 mode);
    extern void fn_800D888C(s32 mode);
    extern void fn_800D6A00(s32 primitive);
    extern void fn_800D7820(void* format);
    extern void fn_800D85D4(s32 index, void* model);
    extern void fn_800D67BC(s32 count);
    extern void fn_800D61E4(s32 x, s32 y);
    extern void fn_800D5CB8(s32 index, s32 red, s32 green, s32 blue, s32 alpha);
    extern void fn_800D59B8(s32 index, f32 x, f32 y);
    extern void fn_800D6728(void);
    extern u8 lbl_80314F98[];
    extern f32 lbl_8047BF60;
    extern f32 lbl_8047BF90;
    void* model;

    model = menuModelRender(lbl_803A9A60.model);
    if (model != 0) {
#pragma scheduling on
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, model);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047BF60, lbl_8047BF60);
        fn_800D61E4(params->x, params->y);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047BF90, lbl_8047BF90);
        fn_800D6728();
    }
}
#pragma pop

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

#pragma push
#pragma peephole off
void fn_800609B4(MenuCBBattleStartSpriteContext* context,
                 MenuCBBattleStartSprite* sprite, f32 xOffset) {
    extern MenuCBBattleStartSpriteEntry lbl_802EF0A8[];
    extern void fn_800FE6D0(s32 x, s32 y);
    extern void spriteSetEnv(void);

    sprite->x = (s16)(lbl_802EF0A8[sprite->tableIndex].x + (s32)xOffset);
    fn_800FE6D0((s16)(context->x + sprite->x),
                (s16)(context->y + sprite->y));
    spriteSetEnv();
}
#pragma pop

typedef struct MenuCBBattleStartMessage {
    u8 pad0[4];
    s8 flags;
} MenuCBBattleStartMessage;

#pragma push
#pragma peephole off
void fn_80061B74(void* context, MenuCBBattleStartMessage* message) {
    switch (lbl_803A9A60.status) {
    case 0:
        message->flags &= ~2;
        break;
    case 1:
        message->flags &= ~2;
        break;
    }
}
#pragma pop

#pragma push
#pragma peephole off
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
#pragma pop

void fn_80060D70(void*, UICmdMsg*, s32, s32);
void fn_80060EF4(void*, UICmdMsg*, s32);
void fn_8006106C(void*, UICmdMsg*, s32, s32, s32);
void fn_80061240(void*, UICmdMsg*, s32, s32);
void fn_80061454(void*, UICmdMsg*, s32, s32);
void fn_800615F4(void*, UICmdMsg*, s32, s32);
void fn_800617E0(void*, UICmdMsg*, s32, s32);
void fn_80061A2C(void*, UICmdMsg*, s32, s32, s32);
void fn_80061BBC(void*, UICmdMsg*, s32, s32, s32);
u8 fn_80061D34(void*, UICmdMsg*, s32, s32, s32);

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
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC1F:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC20:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC21:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC22:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC23:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
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
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC45:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC46:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC47:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC48:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC49:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
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
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC6B:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC6C:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC6D:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC6E:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC6F:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
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
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC91:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC92:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC93:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC94:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
        break;
    case 0xC95:
        fn_80061B74(ctx, (MenuCBBattleStartMessage*)msg);
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

extern u8 fn_80061D34(void*, UICmdMsg*, s32, s32, s32);
extern u8 fn_80069A08(void*, UICmdMsg*, s32, s32);
extern u16 fn_8025D808(s32);
extern u16 fn_8025D89C(s32);
extern s32 fn_8025D9A8(void);
extern s32 fn_8025D9CC(void);
extern s32 fn_8025DA88(void);
extern u16 fn_8025D28C(s32);
extern u16 fn_8025D914(s32);
extern u16 fn_8025DA18(s32);
extern u16 fn_801EF634(void);
extern void* fn_8012AC54(u16);
extern void* fn_800FA280(s32);
extern void fn_80132A38();
extern void fn_800FB680();
extern void fn_800FBB34();
extern void fn_800FE6D0(s32, s32);
extern void fn_800FE4D4(void);
extern void fn_801040F0();
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(void*);
extern void fn_800D85D4(s32, void*);
extern void fn_800D67BC(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D5BA0(s32, u32);
extern void fn_800D59B8(s32, f32, f32);
extern void fn_800D6728(void);
extern void fn_801FCCC4(u16);
extern void fn_801FCC64(void);
extern void fn_801FBD58(void);
extern void fn_801FBD28(void);
extern u16 lbl_80478910[];
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

static void menuCBBattleStartPlace(
    void* context, UICmdMsg* msg, f32 offset)
{
    u8* menu = context;

    msg->field50 =
        *(s16*)&lbl_802EF0A8[msg->cmd * 0x1C + 2] + (s32)offset;
    fn_800FE6D0(*(s16*)(menu + 0x84) + msg->field50,
                *(s16*)(menu + 0x86) + msg->field52);
    fn_800FE4D4();
}

void fn_80060D70(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    s32 expected[2];
    u16 battle_kind;
    f32 scale;

    if (lbl_803A9A60.status != 1) {
        msg->flags4 &= ~2;
        return;
    }
    battle_kind = fn_801EF634();
    if (battle_kind == 2 || battle_kind == 5) {
        expected[0] = 0;
        expected[1] = 1;
    } else if (battle_kind == 3 || battle_kind == 4) {
        expected[0] = 1;
        expected[1] = 0;
    } else {
        expected[0] = 2;
        expected[1] = 2;
    }
    if (lbl_803A9A60.timer >= 6 && kind == expected[player]) {
        scale = *(f32*)((u8*)&lbl_803A9A60 + 0x358 + player * 8);
        msg->alpha67 = (u8)(255.0f * (2.0f - scale));
        msg->scale68 = scale;
        msg->scale6C = scale;
        msg->flags4 |= 2;
    } else {
        msg->flags4 &= ~2;
    }
}

void fn_80060EF4(void* context, UICmdMsg* msg, s32 index)
{
    s32 count = *(s32*)((u8*)lbl_803A9A60.menu + 0xC);
    s32 mode = fn_8025D9A8();

    if (index < 0) {
        if (mode == 1 || index == count) {
            msg->flags4 |= 2;
        } else {
            msg->flags4 &= ~2;
        }
    } else if (mode == 1) {
        msg->flags4 &= ~2;
    } else if ((count == 5 && index == 3) || index == count) {
        msg->flags4 |= 2;
    } else {
        msg->flags4 &= ~2;
    }
}

void fn_8006106C(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    u8* group = (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;
    s16 count;

    if (fn_80061D34(context, msg, player, slot, kind)) {
        menuCBBattleStartPlace(context, msg, *(f32*)(group + 0x3C + slot * 4));
        if (fn_80069A08(context, msg, player, slot)) {
            count = *(s16*)(group + slot * 2);
            if (count != 0) {
                fn_801040F0(0, 0, context, lbl_80478910[count], 0);
                if (lbl_803A9A60.timer == 3) {
                    f32* phase = (f32*)(group + 0x0C + slot * 4);
                    *phase += *(f32*)((u8*)&lbl_803A9A60 + 0x3C);
                    if (*phase >= 1.0f) {
                        *phase = 0.0f;
                        *(s16*)(group + slot * 2) = count - 1;
                    }
                }
            }
        } else {
            count = *(s16*)(group + slot * 2);
            if (count != 0) {
                fn_801040F0(0, 0, context, lbl_80478910[count], 0);
            }
        }
    }
    if (fn_80061D34(context, msg, player, slot, kind)) {
        fn_801040F0(-8, -8, context, 0x40, 0);
    }
}

void fn_80061240(void* context, UICmdMsg* msg, s32 player, s32 slot)
{
    u8* group = (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;
    f32 ratio = *(f32*)(group + 0x6C + slot * 4) /
                *(f32*)(group + 0x9C + slot * 4);
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha = *((u8*)context + 0x8B);
    u32 color;
    s32 end;

    lbl_8047891C = ratio;
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
    if (ratio != 0.0f) {
        color = ((red - 2) << 24) | ((green - 2) << 16) |
                ((blue - 2) << 8) | alpha;
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D7820(lbl_80314E08);
        fn_800D6A00(4);
        fn_800D67BC(4);
        fn_800D61E4(lbl_80478918, lbl_8047891A);
        fn_800D5BA0(0, color);
        end = (s32)(ratio *
            (msg->field54 - lbl_80478918) + lbl_80478918);
        fn_800D61E4(end, lbl_8047891A);
        fn_800D5BA0(0, color);
        fn_800D61E4(msg->field54, msg->field56);
        color = (red << 24) | (green << 16) | (blue << 8) | alpha;
        fn_800D5BA0(0, color);
        fn_800D61E4(end, msg->field56);
        fn_800D5BA0(0, color);
        fn_800D6728();
    }
}

void fn_80061454(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* image;
    s32 valid = 1;
    s32 mode = fn_8025DA88();

    if ((kind == 2 && mode != 2) || (kind != 2 && mode == 2)) {
        valid = 0;
    }
    if (valid) {
        menuCBBattleStartPlace(context, msg,
            *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
        image = *(void**)((u8*)&lbl_803A9A60 + 0x0C + player * 8);
        if (image != NULL) {
            fn_800D88DC(3);
            fn_800D888C(4);
            fn_800D6A00(7);
            fn_800D7820(lbl_80314F98);
            fn_800D85D4(0, image);
            fn_800D67BC(2);
            fn_800D61E4(0, 0);
            fn_800D5CB8(
                0, 0xFF, 0xFF, 0xFF, *((u8*)context + 0x8B));
            fn_800D59B8(0, 0.0f, 0.0f);
            fn_800D61E4(msg->field54, msg->field56);
            fn_800D5CB8(
                0, 0xFF, 0xFF, 0xFF, *((u8*)context + 0x8B));
            fn_800D59B8(0, 1.0f, 1.0f);
            fn_800D6728();
        }
    }
}

void fn_800615F4(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* text;
    u16 pokemon;
    s32 battle_mode = fn_8025D9CC();
    s32 entry_mode = fn_8025DA88();

    if ((kind == 2) != (entry_mode == 2)) {
        return;
    }
    menuCBBattleStartPlace(context, msg,
        *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
    if (battle_mode == 4) {
        if (kind == 0) {
            fn_80132A38(0x34, fn_8025DA18(player) + 1);
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
    pokemon = fn_8025D28C(player);
    text = fn_800FA280(pokemon == 0);
    fn_801FCCC4(pokemon);
    fn_801FCC64();
    fn_801FBD58();
    fn_801FBD28();
    fn_80132A38(0x37, text);
    fn_80132A38(0x4D, text);
    if (kind == 0 && player != 0) {
        fn_800FB680(0, 0,
            0xFFFFFF00 | *((u8*)context + 0x8B), 0xCF);
    }
}

void fn_800617E0(void* context, UICmdMsg* msg, s32 player, s32 kind)
{
    void* text;

    if ((kind == 2) != (fn_8025DA88() == 2)) {
        return;
    }
    menuCBBattleStartPlace(context, msg,
        *(f32*)((u8*)&lbl_803A9A60 + 0x32C + player * 0xC));
    text = fn_8012AC54(fn_8025D914(player));
    if (text == NULL) {
        text = fn_800FA280(1);
    }
    if (fn_8025D9CC() == 4 || player == 0) {
        fn_80132A38(0x37, text);
        fn_80132A38(0x4D, text);
    } else {
        text = (u8*)&lbl_803A9A60 + 0x3C4;
        fn_80132A38(0x37, text);
        fn_80132A38(0x4D, text);
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
        fn_80132A38(0x34, fn_8025DA18(player) + 1);
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

void fn_80061A2C(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    u8* group = (u8*)&lbl_803A9A60 + player * 0xB4;

    if (lbl_803A9A60.status != 1) {
        if (lbl_803A9A60.status == 0) {
            msg->flags4 &= ~2;
        }
        return;
    }
    if (fn_80061D34(context, msg, player, slot, kind)) {
        menuCBBattleStartPlace(context, msg, *(f32*)(group + 0x94 + slot * 4));
        msg->flags4 &= ~2;
        fn_801040F0(0, 0, context, 0x314, 0);
        fn_80061240(context, msg, player, slot);
    } else {
        msg->flags4 &= ~2;
    }
}

void fn_80061BBC(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    u8* group = (u8*)&lbl_803A9A60 + 0x58 + player * 0xB4;

    if (lbl_803A9A60.status != 1) {
        if (lbl_803A9A60.status == 0) {
            msg->flags4 &= ~2;
        }
        return;
    }
    menuCBBattleStartPlace(context, msg, *(f32*)(group + 0x3C + slot * 4));
    if (lbl_803A9A60.timer >= 5 &&
        fn_80061D34(context, msg, player, slot, kind) &&
        *(f32*)(group + 0x84 + slot * 4) == 0.0f) {
        msg->flags4 |= 2;
    } else {
        msg->flags4 &= ~2;
    }
}

u8 fn_80061D34(
    void* context, UICmdMsg* msg, s32 player, s32 slot, s32 kind)
{
    s32 selection;
    s32 mode = fn_8025DA88();
    s32 valid = 1;

    if (lbl_803A9A60.status == 1) {
        selection = fn_8025D808(player);
    } else {
        selection = fn_8025D89C(player);
    }
    if (lbl_803A9A60.status == 0) {
        if (kind == 2) {
            valid = mode == 2;
        } else if (kind == 0) {
            valid = selection < 4 && mode != 2;
        } else {
            valid = selection >= 4 && mode != 2;
        }
    } else if (kind == 2) {
        valid = mode == 2 && selection <= slot;
    } else if (kind == 0) {
        valid = selection < 4 && mode != 2 && selection <= slot;
    } else {
        valid = selection >= 4 && mode != 2 && selection <= slot;
    }
    if (!valid) {
        msg->flags4 &= ~2;
    }
    return valid;
}

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

void _menuCBBattleStartDispTrainerTexCallBack__FlPvl(
    s32 unused, MenuCBBattleStartTrainerTexture* completed)
{
    MenuCBBattleStartTrainerTextureState* state;
    MenuCBBattleStartTrainerTexture* entry;
    u32 imageId;
    u32 resource;
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
            entry = &state->entries[state->current];
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

void menuCBBattleStartTrainerFaceFree(void)
{
    MenuCBBattleStartTrainerTextureState* state;
    MenuCBBattleStartTrainerTexture* entry;
    u32 imageId;
    s32 i;

    if (toolentryTaisenGetBattleType() != 2) {
        imageId = 0x5C3;
    } else {
        imageId = 0x5C4;
    }
    fn_8017B1CC(imageId);
    fn_800F915C(imageId);

    state = (MenuCBBattleStartTrainerTextureState*)&lbl_803A9A60;
    entry = state->entries;
    if (toolentryTaisenGetBattleType() == 2) {
        state->count = 4;
    } else {
        state->count = 2;
    }
    for (i = 0; i < state->count; i++) {
        fn_800F9210(imageId, entry->resource);
        entry++;
    }

    state->active = 0;
    state->current = 0;
    if (toolentryTaisenGetBattleType() == 2) {
        state->count = 4;
    } else {
        state->count = 2;
    }
    state->entries[0].resource = 0;
    state->entries[0].texture = 0;
    state->entries[1].resource = 0;
    state->entries[1].texture = 0;
    state->entries[2].resource = 0;
    state->entries[2].texture = 0;
    state->entries[3].resource = 0;
    state->entries[3].texture = 0;
}
