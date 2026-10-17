/**
 * @file menuFight_exact_8000F964.c
 * @brief menuFightButtonSecretWazaTop .. menuFightCtrlSecretMain, 0x8000F964 - 0x8000FFA8.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c): the
 * secret-move top window and the secret/ball window controls. GC/1.3
 * -O4,p with the TU's unit-wide -opt nopeephole, no pragmas.
 *
 * RULE-EXCEPTION(user-approved): constant_import (temporary) redirects this
 * carve's own 0.0f, pi/4, 3pi/4 and int-to-float bias literals to the TU's
 * shared .sdata2 pool (lbl_8047B700/704/708/710); see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* renamed symbols referenced by asm incs (symbolmap port) */
extern u32 _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
extern void* menuSubCalcColor(void*, void*);

/* NPC/People system */
extern void  winSpriteSetDisp(void* npc, s32 direction);  /* Set NPC facing */
extern void* menuItemBiosGetPtr(s16 npcId);                  /* Lookup NPC data by ID */

/* Text/dialog system */
extern void  windowDrawSprite();
extern void  winSeqSetMenu(void* ctx, s32 state);       /* Set dialog state */
extern void  fn_801081F8(void* ctx, s32 msgId, s32 flags); /* Display message */

/* Rendering */
extern u32   GSmsgGetRect();                           /* Get model dimensions */
extern void  GSmsgPrintColor();

/* Map/warp */
extern u8    fightFloorCheckFightActionFightOutPokemonIrekaeSelect(s32 p1, void* warpId, void* outDest);
extern void* fightOutPokemonGetNicknamePtr(void* mapData);              /* Get map name string */
extern void  winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
extern void  winMsgClose(s32 slot);                   /* Close message box */

/* Input/frame */
extern u8    fn_801F18DC(s32 controller);             /* Check input ready */
extern u8    fightFloorIsUseFightTimerCommand(s32 controller);             /* Check button pressed */
extern u8    fightTimerCommandIsOver(void);                       /* Check A button */
extern u16   std103_battleGetResult(void);                       /* Get input state */
extern void  _threadSwitch(void);                       /* Frame advance */
extern u32   GSinputButtonsChanged(s32 slot);                   /* Get render flags */
extern u32   fn_800F7BC4(s32 slot);                   /* Get VSync flags */

/* Battle bridge */
extern void menuClose();
extern f64 atan2(f64, f64);

/* menuFightButtonSecretWazaTop - 0x8000F964 | size: 0x474 */
extern f64 lbl_8047B710;
extern f32 lbl_8047B700;
extern f32 lbl_8047B704;
extern f32 lbl_8047B708;
extern u8 lbl_80478858[4];
/* One 0xC-byte secret-move row in the window's allocated copy. */
typedef struct MenuFightSecretWazaEntry {
    u32 id;
    u32 used;
    u32 pad8;
} MenuFightSecretWazaEntry;

void menuFightButtonSecretWazaTop(u8* ctx) {
    extern u8* windowGetKeyInfo(void);
    extern u8* windowGetAllocPtr(u8* a);
    extern void menuSetDisp(u32, s32);
    extern u8 menuIsCheck(s32);
    extern s8 GSinputGetRightStickYData(s32, s32);
    extern s8 GSinputGetRightStickXData(s32, s32);
    extern s32 menuOpenCustom(s32, ...);
    u8* flags;
    MenuFightSecretWazaEntry* entries;
    s32 selected;
    s32 activeMenu;
    s32 targetMenu;
    s8 sx;
    s8 sy;
    f32 fx;
    f32 fy;
    f32 angle;
    u16 bits;
    u8 pressed;

    flags = windowGetKeyInfo();
    entries = (MenuFightSecretWazaEntry*)windowGetAllocPtr(ctx);
    selected = -1;
    activeMenu = 0;
    targetMenu = 0;
    menuSetDisp(*(u32*)(ctx + 4), 1);
    if ((u8)menuIsCheck(0xF9) != 0) {
        activeMenu = 0xF9;
    } else if ((u8)menuIsCheck(0xFA) != 0) {
        activeMenu = 0xFA;
    }

    bits = 0;
    sx = GSinputGetRightStickYData(1, 0);
    sy = GSinputGetRightStickXData(1, 0);
    if ((sy < 0 ? -sy : sy) > 0x20 || (sx < 0 ? -sx : sx) > 0x20) {
        fy = sy;
        fx = sx;
        angle = atan2(fy, fx);
        if ((angle > 0.0f ? angle : -angle) < 0.7853982f) {
            bits |= 2;
        } else if ((angle > 0.0f ? angle : -angle) > 2.3561945f) {
            bits |= 1;
        }
        if (0.7853982f < (angle > 0.0f ? angle : -angle) &&
            (angle > 0.0f ? angle : -angle) < 2.3561945f) {
            if (angle < 0.0f) {
                bits |= 4;
            } else {
                bits |= 8;
            }
        }
    }
    if (bits != 0) {
        if (bits & 1) {
            selected = 0;
        } else if (bits & 8) {
            selected = 1;
        } else if (bits & 2) {
            selected = 2;
        } else if (bits & 4) {
            selected = 3;
        }
        if (entries[selected].used == 0) {
            selected = -1;
        }
        if (selected >= 0) {
            ctx[0x98] = 1;
            *(s32*)(ctx + 0x80) = selected;
        }
    } else if (*(u16*)(flags + 4) & 0x200) {
        ctx[0x98] = 1;
        ctx[0x99] = 1;
        *(s32*)(ctx + 0x80) = -1;
    } else if (*(u16*)flags & 0x400) {
        menuSetDisp(*(u32*)(ctx + 4), 0);
        bits = *(u16*)flags;
        if (bits & 1) {
            *(s32*)lbl_80478858 = 0;
        } else if (bits & 8) {
            *(s32*)lbl_80478858 = 1;
        } else if (bits & 2) {
            *(s32*)lbl_80478858 = 2;
        } else if (bits & 4) {
            *(s32*)lbl_80478858 = 3;
        } else {
            *(s32*)lbl_80478858 = -1;
        }
        if (entries[*(s32*)lbl_80478858].used == 0) {
            *(s32*)lbl_80478858 = -1;
        }
        if (*(s32*)lbl_80478858 < 0) {
            targetMenu = 0xF9;
        } else {
            targetMenu = 0xFA;
        }
    }

    if ((u8)fn_801F18DC(0) != 0) {
        if ((u8)fightFloorIsUseFightTimerCommand(0) == 1 && (u8)fightTimerCommandIsOver() == 1) {
            pressed = 1;
            goto check_pressed;
        }
        if ((u16)std103_battleGetResult() == 1) {
            pressed = 1;
            goto check_pressed;
        }
    }
    pressed = 0;
check_pressed:
    if (pressed != 0) {
        ctx[0x98] = 1;
        ctx[0x99] = 1;
    }

    if (activeMenu != targetMenu) {
        if (activeMenu != 0) {
            menuClose(activeMenu);
        }
        if (ctx[0x98] == 0) {
            if (targetMenu == 0xF9) {
                menuOpenCustom(0xF9, *(u32*)(ctx + 4), 0, 0, 0, 1, *(u32*)(ctx + 4));
                menuSetDisp(0xF9, 1);
                menuSetDisp(0xFA, 0);
            } else if (targetMenu == 0xFA) {
                menuOpenCustom(0xFA, *(u32*)(ctx + 4), 0, 0, 0, 2, *(u32*)(ctx + 4), lbl_80478858);
                menuSetDisp(0xFA, 1);
                menuSetDisp(0xF9, 0);
            }
        }
    }
}

/* menuFightCtrlSecretWazaTop - 0x8000FDD8 | size: 0x60 */
extern void windowAllocMemory(void);
extern void* memcpy(void* dst, const void* src, u32 n);
u32 menuFightCtrlSecretWazaTop(u8* ptr) {
    extern void* windowAllocMemory(u8* a, u32 size);
    extern void windowGetAllocPtr(u8* a);
    if ((s8)ptr[1] == 0) {
        void* dst = windowAllocMemory(ptr, 0x48);
        if (dst != NULL) {
            memcpy(dst, *(void**)(ptr + 0x60), 0x48);
        }
    }
    windowGetAllocPtr(ptr);
    return 0;
}

/* menuFightButtonSecretMain - 0x8000FE38 | size: 0x118 */
void menuFightButtonSecretMain(u8* arg1) {
    extern void* windowGetKeyInfo(void);
    extern u8 fn_801F18DC(s32 a);
    extern u8 fightFloorIsUseFightTimerCommand(s32 a);
    extern u8 fightTimerCommandIsOver(void);
    extern u16 std103_battleGetResult(void);
    void* data;
    u16 flags;
    s32 flag_val;
    u8 flag;
    data = windowGetKeyInfo();
    flags = *(u16*)((u8*)data + 4);
    flag_val = -1;
    if (flags & (1 << 4)) {
        flag_val = 0;
    } else if (flags & (1 << 5)) {
        flag_val = 2;
    } else if (flags & (1 << 11)) {
        flag_val = 3;
    } else if (flags & (1 << 9)) {
        arg1[0x98] = 1;
        arg1[0x99] = 1;
        *(s32*)(arg1 + 0x80) = -1;
    }
    if (flag_val >= 0) {
        arg1[0x98] = 1;
        *(s32*)(arg1 + 0x80) = flag_val;
    }
    if ((u8)fn_801F18DC(0) != 0) {
        if ((u8)fightFloorIsUseFightTimerCommand(0) == 1) {
            if ((u8)fightTimerCommandIsOver() == 1) {
                flag = 1;
                goto got_flag;
            }
        }
        if ((u16)std103_battleGetResult() == 1) {
            flag = 1;
            goto got_flag;
        }
    }
    flag = 0;
got_flag:
    if (flag) {
        arg1[0x98] = 1;
        arg1[0x99] = 1;
    }
}

/* menuFightCtrlSecretMain - 0x8000FF50 | size: 0x58 */
u32 menuFightCtrlSecretMain(u8* ptr) {
    extern void* windowAllocMemory(u8* a, u32 size);
    if ((s8)ptr[1] == 0) {
        void* dst = windowAllocMemory(ptr, 0x18);
        if (dst != NULL) {
            memcpy(dst, *(void**)(ptr + 0x60), 0x18);
        }
    }
    return 0;
}

