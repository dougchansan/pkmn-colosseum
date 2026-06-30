/**
 * @file gs_npc_interact.c
 * @brief GSnpcInteract -- Overworld NPC interaction and dialog system.
 *
 * Address range: 0x8000D298 - 0x80012858 (~70 functions)
 *
 * This module handles player-NPC interactions in the overworld, including:
 *   - Dialog initiation and text display
 *   - Item give/receive events
 *   - Trainer battle triggers
 *   - NPC sprite/model animation during dialog
 *   - Shop/mart interactions
 *   - Quest/story progression triggers
 *
 * Key functions:
 *   fn_8000D298  GSnpc_InitDialog            -- 0x114 bytes, start NPC dialog
 *   fn_8000D3AC  GSnpc_DialogStateMachine    -- 0x364 bytes, dialog state handler
 *   fn_8000D710  GSnpc_EventDispatch         -- 0x398 bytes, NPC event dispatcher
 *   fn_8000DAA8  GSnpc_Nop                   -- 8-byte stub
 *   fn_8000DAB0  GSnpc_SetAnimation          -- set NPC animation state
 *   fn_8000DAE8  GSnpc_ProcessItemEvent      -- 0x1A0 bytes, item give/take
 *   fn_8000DC88  GSnpc_ValidateItem          -- check item validity
 *   fn_8000DD0C  GSnpc_GetItemSlot           -- get bag slot for item
 *   fn_8000DD30  GSnpc_AddItem               -- add item to player bag
 *   fn_8000DD5C  GSnpc_RemoveItem            -- remove item from bag
 *   fn_8000DD98  GSnpc_GetMoney              -- get player money count
 *   fn_8000DDBC  GSnpc_AddMoney              -- add money to player
 *   fn_8000DDE8  GSnpc_RemoveMoney           -- subtract money from player
 *   fn_8000DE24  GSnpc_CheckFlag             -- check game flag for NPC
 *   fn_8000DEC4  GSnpc_SetFlag               -- set game flag from NPC event
 *   fn_8000DFF0  GSnpc_MartBuy               -- 0x214 bytes, Poke Mart buy logic
 *   fn_8000E204  GSnpc_MartSell              -- 0x88 bytes, Poke Mart sell logic
 *   fn_8000E28C  GSnpc_NopStub               -- 4 bytes, no-op
 *   fn_8000E290  GSnpc_TrainerBattle         -- 0x780 bytes, initiate trainer battle
 *   fn_8000EA10  GSnpc_PostBattleReward      -- 0x324 bytes, prize money & items
 *   fn_8000ED34  GSnpc_QuestUpdate           -- 0x5DC bytes, quest progression
 *   fn_8000F310  GSnpc_GetQuestState         -- 0x4C bytes
 *   fn_8000F35C  GSnpc_SetQuestState         -- 0xA4 bytes
 *   fn_8000F400  GSnpc_HealParty             -- 0x368 bytes, Pokemon Center heal
 *   fn_8000F768  GSnpc_FadeTransition        -- 0x1FC bytes, screen fade during dialog
 *   fn_8000F964  GSnpc_PokemonTrade          -- 0x474 bytes, in-game trade event
 *   fn_8000FDD8  GSnpc_MoveTutor             -- 0x60 bytes, move tutor intro
 *   fn_8000FE38  GSnpc_MoveTutorTeach        -- 0x118 bytes, teach move
 *   fn_8000FF50  GSnpc_NameRater             -- 0x58 bytes, name rater check
 *   fn_8000FFA8  GSnpc_NameRaterRename       -- 0x118 bytes, rename Pokemon
 *   fn_800100C0  GSnpc_DaycareDeposit        -- 0x68 bytes, daycare deposit
 *   _menuFightIsUse__FP16MENU_WAZA_STATUSUs  GSnpc_DaycareWithdraw       -- 0x16C bytes, daycare withdraw
 *   fn_80010294  GSnpc_PurificationChamber   -- 0x1E8 bytes, purification setup
 *   fn_8001047C  GSnpc_ShadowGaugeCheck      -- 0x10C bytes, check purification ready
 *   fn_80010588  GSnpc_PurifyPokemon         -- 0x11C bytes, purify Shadow Pokemon
 *   fn_800106A4  GSnpc_SnagMachineSetup      -- 0x1A0 bytes, snag machine interaction
 *   fn_80010844  GSnpc_GBALinkPrompt         -- 0x15C bytes, GBA link cable prompt
 *   fn_800109A0  GSnpc_ColosseumSignup       -- 0x190 bytes, colosseum registration
 *   fn_80010B30  GSnpc_MtBattleEntry         -- 0x168 bytes, Mt. Battle entry
 *   fn_80010C98  GSnpc_WarpToLocation        -- 0x52C bytes, warp/teleport handler
 *
 * The dialog state machine (fn_8000D3AC) has 5 states:
 *   State 0: Init -- load NPC dialog data, set up text box
 *   State 1: Advance -- wait for player input to advance text
 *   State 2: Choice -- display yes/no or multi-choice prompt
 *   State 3: Close -- animate text box closing
 *   State 4: Cleanup -- restore camera, free resources
 *
 * fn_8000D298 (GSnpc_InitDialog) sets up the NPC sprite for dialog:
 *   - Calls fn_80109220 to set NPC facing direction
 *   - Gets the NPC data from fn_8005D934 (lookup by ID)
 *   - Sets up the text viewport via fn_801040F0
 *   - If the NPC has a special marker (offset +0x4C), renders it
 *     using fn_800FA444/fn_800FB680 for the dialog portrait
 *
 * fn_8000E290 (GSnpc_TrainerBattle) is a key bridge function:
 *   - Gets the overworld data for the NPC
 *   - Looks up the trainer ID
 *   - Dispatches a battle event via fn_8012640C
 *   - On event completion, validates the battle result
 *   - Uses a jump table (jumptable_802E4BB8) to handle different
 *     battle outcome types (win, lose, flee, draw)
 *
 * fn_80010C98 (GSnpc_WarpToLocation) handles map transitions:
 *   - Checks warp destination validity via fn_801F2020
 *   - Gets the destination map name via fn_802037DC
 *   - Loads string message 0x76FB for the warp confirmation dialog
 *   - Waits for player input with a render loop
 *   - On confirmation, triggers the floor transition
 *
 * Rodata references:
 *   jumptable_802E4BB8: Battle outcome jump table (26 entries)
 *   Various Shift-JIS string constants for dialog templates
 *
 * SDA globals:
 *   Many NPC-specific state variables in 0x8047A280-0x8047A2A0 range
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
extern void* menuSubCalcColor(void*, void*);

/* NPC/People system */
extern void  fn_80109220(void* npc, s32 direction);  /* Set NPC facing */
extern void* fn_8005D934(s16 npcId);                  /* Lookup NPC data by ID */

/* Text/dialog system */
extern void  fn_801040F0(s32 p1, s32 p2, void* textCtx, u16 msgId, s32 p5);
extern void  fn_801080CC(void* ctx, s32 state);       /* Set dialog state */
extern void  fn_801081F8(void* ctx, s32 msgId, s32 flags); /* Display message */

/* Rendering */
extern u32   fn_800FA444(void* model);                /* Get model dimensions */
extern void  fn_800FB680(s32 x, s32 y, s32 z, u32 flags, u16 modelId);

/* Map/warp */
extern u8    fn_801F2020(s32 p1, void* warpId, void* outDest);
extern void* fn_802037DC(void* mapData);              /* Get map name string */
extern void  fn_80106D3C(s32 slot, s32 msgId, s32 p3, s32 p4);
extern void  fn_801069FC(s32 slot);                   /* Close message box */

/* Input/frame */
extern u8    fn_801F18DC(s32 controller);             /* Check input ready */
extern u8    fn_801F1700(s32 controller);             /* Check button pressed */
extern u8    fn_80265924(void);                       /* Check A button */
extern u16   fn_801EF634(void);                       /* Get input state */
extern void  _threadSwitch(void);                       /* Frame advance */
extern u32   fn_800F7AF0(s32 slot);                   /* Get render flags */
extern u32   fn_800F7BC4(s32 slot);                   /* Get VSync flags */

/* Battle bridge */
extern void  fn_80132A38(s32 paramId, s32 value);     /* Set battle parameter */
extern void  fn_801F4C14(s32 p1, s32 p2, s32 p3, s32 p4, u16 p5); /* Configure map object */
extern void  fn_801040D0(void* ctx, s32 index);       /* Get participant data */

/* =========================================================================
 * SDA globals
 * ========================================================================= */

/* NPC interaction state variables are scattered across the SDA region.
 * The exact mapping is determined by the lbz/sth instructions that
 * reference r0+offset or r13+offset addressing modes. */

/* =========================================================================
 * Function: GSnpc_InitDialog
 * Address:  0x8000D298
 * Size:     0x114
 *
 * Initializes dialog with an NPC. Takes two parameters:
 *   r3: Dialog context pointer (contains camera/viewport state)
 *   r4: NPC instance pointer (contains NPC ID at +0x06, model at +0x4C)
 *
 * First sets the NPC facing direction toward the player, then looks up
 * the NPC data table entry. If the NPC has a portrait model (offset +0x4C),
 * renders it at a calculated screen position based on the dialog box location.
 * ========================================================================= */

/* =========================================================================
 * Function: GSnpc_DialogStateMachine
 * Address:  0x8000D3AC
 * Size:     0x364
 *
 * The main dialog state machine with 5 states (0-4).
 * State 0 initializes the text box; states 1-2 handle input; state 3-4 close.
 *
 * At state 0, checks byte at offset +0x01 of the context struct. If 0x41,
 * this is a standard dialog; if 0x109, this is a special event dialog.
 * Each dialog type uses different message IDs (0x65, 0xDB0-0xDB2).
 * ========================================================================= */

/* =========================================================================
 * Function: GSnpc_TrainerBattle
 * Address:  0x8000E290
 * Size:     0x780
 *
 * Initiates a trainer battle from an NPC interaction. This is the bridge
 * between the NPC system and the battle system.
 *
 * Uses jumptable_802E4BB8 (26 entries) for dispatching on the NPC's
 * event type ID (0x1235-0x124E). Different event types correspond to:
 *   0x1235: Standard trainer
 *   0x1236: Shadow Pokemon encounter
 *   0x1237: Double battle trainer
 *   0x1238: Colosseum opponent
 *   0x123A: Boss/admin battle
 *
 * For each type, configures the battle participants, field, and rules,
 * then hands off to the battle system via fn_8012640C.
 * ========================================================================= */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 1 functions matched
 * =================================================================== */

extern u8 lbl_8047A2A0;

/* Address: 0x8000DAA8 | Size: 0x8 | Pattern: sda_getter */
u8 fn_8000DAA8(void) {
    return lbl_8047A2A0;
}

/* =========================================================================
 * Stubs for remaining GSnpcInteract functions (0x800111C4-0x80011EA4)
 * ========================================================================= */

/* 0x800111C4 | 0x24 -- small accessor */
extern void fn_80089F78(u32, u32, u32, u32);
#if 0
asm void fn_800111C4(void) {
#include "src/game/gs_npc_interact_fn_800111C4.inc"
}
#else
#pragma peephole off
void fn_800111C4(u32 a, u32 b, u32 c) { fn_80089F78(a, b, c, 0); }
#pragma peephole on
#endif

/* 0x800111E8 | 0x24 -- small accessor */
#if 0
asm void fn_800111E8(void) {
#include "src/game/gs_npc_interact_fn_800111E8.inc"
}
#else
#pragma peephole off
void fn_800111E8(u32 a, u32 b, u32 c) { fn_80089F78(a, b, c, 1); }
#pragma peephole on
#endif

/* 0x7C | fn_8001120C | nullcheck_call_flag */
#pragma peephole off
u32 fn_8001120C(void* obj) {
    extern void fn_80102568();
    if ((u8)fn_80102620(0xff) != 0) fn_80102568(0xff, 0, obj);
    if ((u8)fn_80102620(0x104) != 0) fn_80102568(0x104, 0, obj);
    fn_80102620(0x100);
    return 0;
}
#pragma peephole on

/* 0x80011288 | 0x21C */
extern void fn_8005D8F8();
extern void fn_80102138();
extern s32 fn_801026A4(s32, ...);
#if 1
asm void fn_80011288(void) {
#include "src/game/gs_npc_interact_fn_80011288.inc"
}
#else
void fn_80011288(void) {
    extern void fn_8005D8F8();
    extern void fn_80102138();
    extern void fn_801026A4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r28 = r3;
    r29 = r5;
    tmp = *(u8*)((u8*)r28 + 0x21);
    if (tmp == 0) {
    do {
        r3 = 0x1258;
        r4 = 0x0;
        fn_8005D8F8();
        r3 = 0x1259;
        r4 = 0x0;
        fn_8005D8F8();
        r3 = 0x125a;
        r4 = 0x0;
        fn_8005D8F8();
        r3 = 0x125b;
        r4 = 0x0;
        fn_8005D8F8();
        r31 = r28;
        r30 = 0x0;
        do {
        do {
            tmp = *(u32*)((u8*)r31 + 0x4);
            if ((s32)tmp != 0x47) {
                if ((s32)tmp < 0x47) {
                    if ((s32)tmp != 0x45) {
                        if ((s32)tmp < 0x45) {
                            goto L_8001133C;
                        }
                        if ((s32)tmp >= 0x49) goto L_8001133C;
                        goto L_80011334;
                        }
                    r3 = 0x125b;
                    break;
                        }
                r3 = 0x125a;
                break;
            }
            r3 = 0x1258;
            break;
        L_80011334:
            r3 = 0x1259;
            break;
        L_8001133C:
            r3 = 0x0;
        } while (0);
            if ((s32)r3 != 0) {
                r4 = 0x1;
                fn_8005D8F8();
            }
            r31 = r31 + 0x8;
            r30 = r30 + 0x1;
        } while ((s32)r30 < 4);
        r3 = 0xff;
        r4 = 0x125a;
        fn_80102138();
        if ((s32)r3 == (s32)-0x1) {
            r3 = 0xff;
            r4 = 0x125b;
            fn_80102138();
            if ((s32)r3 == (s32)-0x1) {
                tmp = 0x0;
                *(u32*)(sp + 0x8) = tmp;
        }
        }
        r7 = r29;
        r9 = r28;
        r5 = (u32)sp + 0x8;
        r3 = 0xff;
        r4 = 0x0;
        r6 = 0x0;
        r8 = 0x1;
        fn_801026A4();
        if ((s32)r3 != 0x125a) {
            if ((s32)r3 < 0x125a) {
                if ((s32)r3 != 0x1258) {
                    if ((s32)r3 < 0x1258) {
                        goto L_80011404;
                    }
                    if ((s32)r3 >= 0x125c) goto L_80011404;
                    goto L_800113FC;
                    }
                r4 = 0x47;
                break;
                    }
            r4 = 0x48;
            break;
        }
        r4 = 0x46;
        break;
    L_800113FC:
        r4 = 0x45;
        break;
    L_80011404:
        r4 = -0x1;
    } while (0);
        r3 = -0x1;
        tmp = *(u32*)((u8*)r28 + 0x4);
        if ((s32)r4 == (s32)tmp) {
            r3 = 0x0;
            return;
        }
        tmp = *(u32*)((u8*)r28 + 0xC);
        if ((s32)r4 == (s32)tmp) {
            r3 = 0x1;
            return;
        }
        tmp = *(u32*)((u8*)r28 + 0x14);
        if ((s32)r4 == (s32)tmp) {
            r3 = 0x2;
            return;
        }
        tmp = *(u32*)((u8*)r28 + 0x1C);
        if ((s32)r4 != (s32)tmp) return;
        r3 = 0x3;
        return;
        return;
    }
    r5 = r4;
    r7 = r29;
    r9 = r28;
    r3 = 0x104;
    r4 = 0x0;
    r6 = 0x0;
    r8 = 0x1;
    fn_801026A4();

    return;
}
#endif

/* 0x800114A4 | 0x25C */
extern void fn_8020E204();
extern void fn_8020E1A4();
extern void fn_8001BD80();
extern void fn_80102568();
extern void fn_801F02AC();
extern void fn_802062FC();
extern void fn_802656AC();
extern u32 fn_80010C98(void* npc, u32 warpId, u32 variant);
extern void fn_80011288();
extern s32 fn_801026A4(s32, ...);
#if 1
asm void fn_800114A4(void) {
#include "src/game/gs_npc_interact_fn_800114A4.inc"
}
#else
void fn_800114A4(void) {
    extern u32 fn_80010C98(void* npc, u32 warpId, u32 variant);
    extern void fn_80011288();
    extern void fn_8001BD80();
    extern void fn_80102568();
    extern void fn_80102620();
    extern void fn_801026A4();
    extern void fn_801F02AC();
    extern void fn_802062FC();
    extern void fn_8020E1A4();
    extern void fn_8020E204();
    extern void fn_802656AC();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f8 = 0.0f;

    r24 = r3;
    r25 = r4;
    r26 = r5;
    r27 = r6;
    r28 = r7;
    r3 = r26;
    fn_8020E204();
    fn_8020E1A4();
    tmp = r28 & 0xFF;
    r31 = r3;
    if (tmp == 0) {
        r4 = r27;
        r5 = r24;
        r6 = r25;
        fn_8001BD80();
        r29 = r3;
        r3 = r29;
        return;
    }
    r30 = r27 & 0xFF;
L_80011500:
    r3 = 0xf8;
    r4 = 0x1e;
    ((void(*)(void))fn_801080CC)();
    r9 = r24;
    r10 = r25;
    r3 = 0xf8;
    r4 = 0x0;
    r5 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    r8 = 0x3;
    fn_801026A4();
    tmp = r3;
    r3 = 0xf8;
    r29 = tmp;
    r4 = 0x20;
    ((void(*)(void))fn_801080CC)();
    if ((s32)r29 == (s32)-0x1) {
        r3 = 0xf8;
        r4 = 0x0;
        r5 = 0x1;
        fn_80102568();
        r3 = -0x1;
        return;
    }
    r3 = r25;
    r4 = r24;
    r5 = r29;
    fn_80010C98();
    tmp = r3 & 0xFF;
    if (tmp == 0) goto L_80011500;
    tmp = r27 & 0xFF;
    do {
        if (tmp == 0) break;
        tmp = r31 & 0xFF;
        if (tmp < 2) break;
        r4 = r25;
        r5 = r26;
        r3 = 0xf;
        fn_801F02AC();
        r23 = r3;
        fn_802062FC();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            r3 = r23;
            r4 = r26;
            r5 = 0x0;
            fn_802656AC();
        } else {

            r3 = 0x0;
        }
        r4 = r25;
        r5 = r26;
        r3 = 0x10;
        fn_801F02AC();
        r23 = r3;
        fn_802062FC();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            r3 = r23;
            r4 = r26;
            r5 = 0x0;
            fn_802656AC();
        } else {

            r3 = 0x0;
        }
        r4 = r25;
        r5 = r26;
        r3 = 0xe;
        fn_801F02AC();
        r23 = r3;
        fn_802062FC();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            r3 = r23;
            r4 = r26;
            r5 = 0x0;
            fn_802656AC();
        } else {

            r3 = 0x0;
        }
        tmp = 0x0;
        r3 = (u32)sp + 0x10;
        r4 = 0x0;
        *(u32*)(sp + 0x2C) = tmp;
        r5 = 0x1;
        *(u8*)(sp + 0x31) = r28;
        fn_80011288();
        r23 = r3;
        r3 = 0xff;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0xff;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = 0x104;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0x104;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = 0x100;
        fn_80102620();
        if ((s32)r23 == (s32)-0x1) goto L_80011500;
    } while (0);

    r3 = 0xf8;
    r4 = 0x0;
    r5 = 0x1;
    fn_80102568();

    r3 = r29;

    return;
}
#endif

/* 0x80011700 | 0xBC — clear 4 event flags referenced by input arg */
#if 0
asm void menuFightCloseWaza(void) {
#include "src/game/gs_npc_interact_menuFightCloseWaza.inc"
}
#else
#pragma push
#pragma peephole off
s32 menuFightCloseWaza(s32 arg) {
    if ((u8)fn_80102620(0x4c) != 0) fn_80102568(0x4c, 0, arg);
    if ((u8)fn_80102620(0xf9) != 0) fn_80102568(0xf9, 0, arg);
    if ((u8)fn_80102620(0xfa) != 0) fn_80102568(0xfa, 0, arg);
    if ((u8)fn_80102620(0xf7) != 0) fn_80102568(0xf7, 0, arg);
    return 0;
}
#pragma pop
#endif

/* 0x800117BC | 0x1EC */
extern void _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
extern void fn_80106394();
extern void fn_80106080();
#if 1
asm void fn_800117BC(void) {
#include "src/game/gs_npc_interact_fn_800117BC.inc"
}
#else
void fn_800117BC(void) {
    extern void _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
    extern void fn_80102568();
    extern void fn_80102620();
    extern void fn_801026A4();
    extern void fn_80106080();
    extern void fn_80106394();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f7 = 0.0f;
    f32 f9 = 0.0f;

    r28 = r3;
    r29 = r4;
    r30 = r5;
    tmp = *(u8*)((u8*)r28 + 0x44);
    if (tmp == 0) {
        r31 = 0x4c;

    } else {
        r31 = 0xf7;
    }
L_800117F0:
    r3 = r31;
    r4 = 0x1e;
    ((void(*)(void))fn_801080CC)();
    r3 = r31;
    r5 = r29;
    r7 = r30;
    r9 = r28;
    r4 = 0x0;
    r6 = 0x0;
    r8 = 0x1;
    fn_801026A4();
    r27 = r3;
    do {
        if ((s32)r27 < 0) break;
        r3 = r28;
        r4 = r27 & 0xFFFF;
        _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
        r26 = r3;
        if (r26 == 0) break;
        r3 = 0x4c;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0x4c;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = 0xf9;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0xf9;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = 0xfa;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0xfa;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = 0xf7;
        fn_80102620();
        tmp = r3 & 0xFF;
        if (tmp != 0) {
            r3 = 0xf7;
            r4 = 0x0;
            r5 = 0x1;
            fn_80102568();
        }
        r3 = r26;
        r4 = 0x1;
        r5 = 0x1;
        fn_80106394();
        while (1) {
            r3 = 0x1;
            ((void(*)(void))fn_800F7AF0)();
            r27 = r3;
            r3 = 0x1;
            ((void(*)(void))fn_800F7BC4)();
            tmp = r3 & r27;
            tmp = tmp & 0x00000300;
            if (tmp != 0) break;
            r3 = 0x0;
            ((void(*)(void))fn_801F18DC)();
            tmp = r3 & 0xFF;
            do {
                if (tmp == 0) break;
                r3 = 0x0;
                ((void(*)(void))fn_801F1700)();
                tmp = r3 & 0xFF;
                do {
                    if (tmp != 1) break;
                    ((void(*)(void))fn_80265924)();
                    tmp = r3 & 0xFF;
                    if (tmp != 1) break;
                    tmp = 0x1;
                    break;
                } while (0);

                ((void(*)(void))fn_801EF634)();
                tmp = r3 & 0xFFFF;
                if (tmp != 1) break;
                tmp = 0x1;
                break;
            } while (0);

            tmp = 0x0;

            tmp = tmp & 0xFF;
            if (tmp != 0) break;
            ((void(*)(void))_threadSwitch)();

        }

        r3 = 0x1;
        fn_80106080();
        goto L_800117F0;
    } while (0);

    r3 = r31;
    r4 = 0x20;
    ((void(*)(void))fn_801080CC)();
    r3 = r27;
    return;
}
#endif

/* 0x74 | fn_800119A8 | nullcheck_call_flag */
#pragma peephole off
#pragma peephole off
u32 fn_800119A8(void* obj) {
    if ((u8)fn_80102620(0x4b) != 0) fn_80102568(0x4b, 0, obj);
    if ((u8)fn_80102620(0xf6) != 0) fn_80102568(0xf6, 0, obj);
    return 0;
}
#pragma peephole on
#pragma peephole on

/* 0x80011A1C | 0x130 */
#if 0
asm void fn_80011A1C(void) {
#include "src/game/gs_npc_interact_fn_80011A1C.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80011A1C(u8* obj, s32 a1, s32 a2) {
    u32 sp8;
    s32 ret;    /* r30 */
    u8 locked;  /* r29 */
    s32 ret2;   /* r31 */

    locked = obj[0x16];
    do {
        if (obj[0x17] != 0) {
            ret = fn_801026A4(0xF6, 0, a1, 0, a2, 1, obj);
        } else {
            ret = fn_801026A4(0x4B, 0, a1, 0, a2, 1, obj);
        }
        if (locked != 0) break;
        if (ret != 3) break;
        sp8 = 0;
        if ((u8)fn_80102620(0x4B) != 0) {
            fn_80102568(0x4B, 0, 1);
        }
        if ((u8)fn_80102620(0xF6) != 0) {
            fn_80102568(0xF6, 0, 1);
        }
        ret2 = fn_801026A4(0xFB, 0, &sp8, 0, 1, 0);
        fn_80102568(0xFB, 0, 1);
    } while (ret2 != 0x1207);

    return ret;
}
#pragma pop
#endif

/* 0x78 | fn_80011B4C | generic */
extern u32 fn_80104704(u32 a);
extern u32 fn_801040A0(u32 a);
#pragma peephole off
#pragma peephole off
u32 fn_80011B4C(u32 arg1, u8 arg2) {
    u32 r;
    while (1) {
        if (!(r = fn_80104704(arg1))) return 0;
        if (*(s16*)((u8*)fn_801040A0(r) + 0xc) == 0) return 0;
        if (arg2 != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}
#pragma peephole on
#pragma peephole on

/* 0x80011BC4 | 0xB4 */
extern u32 fn_80103FE4(u32 a);
extern void fn_80166A28(u32 val);
#if 0
asm void fn_80011BC4(void) {
#include "src/game/gs_npc_interact_fn_80011BC4.inc"
}
#else
#pragma peephole off
#pragma peephole off
#pragma peephole off
void fn_80011BC4(u32 arg1, u32 target) {
    u32 ptr;
    u32 state;
    u32 data;
    u32 diff;
    s16 score;
    ptr = fn_80104704(arg1);
    if (!ptr) { return; }
    state = fn_80103FE4(ptr);
    data = fn_801040A0(ptr);
    if (target > *(u32*)(state + 0x20)) {
        diff = target - *(u32*)(state + 0x20);
    } else {
        diff = *(u32*)(state + 0x20) - target;
    }
    score = diff * 100 / *(u32*)(state + 0x1c);
    *(s16*)(data + 0xc) = score;
    if (*(s16*)(data + 0xc) < 0xf) { *(s16*)(data + 0xc) = 0xf; }
    *(u32*)(data + 0x8) = *(u32*)(state + 0x20);
    *(u32*)(state + 0x20) = target;
    *(s16*)(data + 0xe) = 0;
    fn_80166A28(0x4d0);
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#endif

/* 0x78 | fn_80011C78 | generic */
#pragma peephole off
#pragma peephole off
u32 fn_80011C78(u32 arg1, u8 arg2) {
    u32 r;
    while (1) {
        if (!(r = fn_80104704(arg1))) return 0;
        if (*(s16*)((u8*)fn_801040A0(r) + 0x2) == 0) return 0;
        if (arg2 != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}
#pragma peephole on
#pragma peephole on

/* 0x80011CF0 | 0xAC */
#if 0
asm void fn_80011CF0(void) {
#include "src/game/gs_npc_interact_fn_80011CF0.inc"
}
#else
void fn_80011CF0(u32 arg1, s16 target) {
    u32 ptr;
    u32 state;
    u32 data;
    s16 current;
    s32 diff;
    if (!(ptr = fn_80104704(arg1))) { return; }
    state = fn_80103FE4(ptr);
    data = fn_801040A0(ptr);
    current = *(s16*)(state + 0x1a);
    diff = current - (s16)target;
    if (diff < 0) { diff = -diff; }
    *(s16*)(data + 2) = (s16)((diff * 100) / *(s16*)(state + 0x18));
    if (*(s16*)(data + 2) <= 0) { *(s16*)(data + 2) = 1; }
    *(s16*)(data + 0) = current;
    *(s16*)(state + 0x1a) = target;
    *(s16*)(data + 4) = 0;
}
#endif

/* 0x80011D9C | 0xCC — item-kind resolver + dispatch (same switch as fn_800129A8) */
#if 0
asm void fn_80011D9C(void) {
#include "src/game/gs_npc_interact_fn_80011D9C.inc"
}
#else
#pragma peephole off
#pragma peephole off
void fn_80011D9C(s32 id, s32 do_extra) {
    u32 resolved;
    s32 kind;
    kind = 0;
    resolved = fn_80104704(id);
    if (resolved == 0) return;
    switch (id) {
    case 0x45: case 0x46: case 0x49:
        kind = 0x538;
        break;
    case 0x47: case 0x48: case 0x4a:
        kind = 0x540;
        break;
    }
    if (do_extra != 0) {
        fn_80103F74(resolved, kind, 1);
        fn_801081F8((void*)resolved, kind, 0x2d);
    } else {
        fn_80103F74(resolved, kind, 0);
    }
}
#pragma peephole on
#pragma peephole on
#endif

/* 0x80011E68 | 0x3C */
/* Set an NPC's facing direction. */
void fn_80011E68(u32 npcId, u16 direction) {
    extern void* fn_80103FE4(void* obj);
    extern void* fn_80104704(u32 npcId);
    void* npc;

    npc = fn_80104704(npcId);
    if (npc != NULL) {
        u8* obj = (u8*)fn_80103FE4(npc);
        *(u16*)(obj + 0x1A) = direction;
    }
}

/* 0x80011EA4 | 0x9B4 -- GSnpc_WarpToLocation continued */
extern void fn_801091F4();
extern void fn_8001DACC();
extern void fn_8010B9E8();
extern void fn_801F54A4();
extern void fn_800FA280();
extern void fn_80104160();
extern void fn_800D88DC();
extern void fn_800D888C();
extern void fn_800D6A00();
extern void fn_800D7820();
extern void fn_800D67BC();
extern void fn_800D61E4();
extern void fn_800D5BA0();
extern void fn_800D6728();
extern void fn_800C46B0();
extern f64 lbl_8047B730;
extern f32 lbl_8047B720;
extern f64 lbl_8047B738;
extern f32 lbl_8047B718;
extern f32 lbl_8047B724;
extern f32 lbl_8047B71C;
extern f32 lbl_8047B728;
extern f32 lbl_8047B72C;
extern u8 lbl_80314E08[];
#if 1
asm void fn_80011EA4(void) {
#include "src/game/gs_npc_interact_fn_80011EA4.inc"
}
#else
void fn_80011EA4(void) {
    extern u8 lbl_80314E08[];
    extern f32 lbl_8047B718;
    extern f32 lbl_8047B71C;
    extern f32 lbl_8047B720;
    extern f32 lbl_8047B724;
    extern f32 lbl_8047B728;
    extern f32 lbl_8047B72C;
    extern f64 lbl_8047B730;
    extern f64 lbl_8047B738;
    extern void fn_8001DACC();
    extern void fn_800C46B0();
    extern void fn_800D5BA0();
    extern void fn_800D61E4();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800FA280();
    extern void fn_80103FE4();
    extern void fn_801040A0();
    extern void fn_80104160();
    extern void fn_801091F4();
    extern void fn_8010B9E8();
    extern void fn_801F54A4();
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r27 = r3;
    r28 = r4;
    fn_80103FE4();
    tmp = r3;
    r3 = r27;
    r30 = tmp;
    fn_801040A0();
    r31 = r3;
    r3 = r27;
    fn_80103FE4();
    r6 = *(s16*)((u8*)r28 + 0x6);
    r4 = 0x1;
    r5 = *(u8*)((u8*)r3 + 0x16);
    if ((s32)r6 < 0xa8) {
        if ((s32)r6 < 0x9f) {
            if ((s32)r6 == 0x9a) goto L_80011FC0;
            if ((s32)r6 < 0x9a) {
                if ((s32)r6 < 0x99) {
                    goto L_80011FC0;
                }
                if ((s32)r6 < 0x9d) {
                    goto L_80011F8C;
                }
                if ((s32)r6 != 0xa5) {
                    if ((s32)r6 < 0xa5) {
            }
                }
                if ((s32)r6 < 0xa4) {
                }
                goto L_80011FC0;
            }
            if ((s32)r6 < 0x538) {
                if ((s32)r6 < 0x534) {
                    if ((s32)r6 >= 0xaa) goto L_80011FC0;
                }
                goto L_80011FA8;
            }
            if ((s32)r6 >= 0x536) goto L_80011FA8;
        }

    } else {
        if ((s32)r6 < 0x53e) {
            if ((s32)r6 >= 0x53c) goto L_80011F8C;
            goto L_80011FC0;
        }
        if ((s32)r6 >= 0x540) goto L_80011FC0;
        goto L_80011FA8;
    }
L_80011F8C:
    tmp = *(u8*)((u8*)r3 + 0x29);
    if (tmp != 0) {
        r4 = 0x0;
        goto L_80011FC0;
    }
    r4 = 0x1;
    goto L_80011FC0;
L_80011FA8:
    tmp = *(u8*)((u8*)r3 + 0x29);
    if (tmp != 0) {
        r4 = 0x1;

    } else {
        r4 = 0x0;
    }
L_80011FC0:
do {
    if ((s32)r6 < 0x534) {
        if ((s32)r6 != 0xa4) {
            if ((s32)r6 >= 0xa4) break;
            if ((s32)r6 != 0x99) {
                break;
            }
            if ((s32)r6 < 0x53c) {
                if ((s32)r6 >= 0x538) break;
        }
        }

    } else {
        if ((s32)r6 >= 0x540) break;
    }
    if (r5 == 1) {
        r4 = 0x0;
    }
} while (0);
    r3 = r28;
    ((void(*)(void))fn_80109220)();
    r3 = r28;
    fn_801091F4();
    tmp = r3 & 0xFF;
    if (tmp == 0) return;
    tmp = *(s16*)((u8*)r28 + 0x6);
    r3 = -0x100;
    r8 = *(u8*)((u8*)r27 + 0x8B);
    r29 = r8 | r3;
    if ((s32)tmp != 0xae) {
        if ((s32)tmp < 0xae) {
            if ((s32)tmp != 0xa3) {
                if ((s32)tmp < 0xa3) {
                    if ((s32)tmp < 0x9f) {
                        if ((s32)tmp == 0x9a) goto L_8001215C;
                        if ((s32)tmp < 0x9a) return;
                        if ((s32)tmp < 0x9d) return;

                    }
                    if ((s32)tmp == 0xa1) goto L_80012170;
                    if ((s32)tmp < 0xa1) return;

                }
                if ((s32)tmp < 0xaa) {
                    if ((s32)tmp == 0xa5) goto L_8001215C;
                    if ((s32)tmp < 0xa5) return;
                    if ((s32)tmp < 0xa8) return;

                }
                if ((s32)tmp == 0xac) goto L_80012170;
                if ((s32)tmp < 0xac) return;

            }
            if ((s32)tmp == 0x53a) goto L_800123A4;
            if ((s32)tmp < 0x53a) {
                if ((s32)tmp != 0x535) {
                    if ((s32)tmp < 0x535) {
                        if ((s32)tmp == 0x533) goto L_800123CC;
                        if ((s32)tmp >= 0x533) return;
                        if ((s32)tmp < 0x532) return;

                    }
                    if ((s32)tmp != 0x537) return;

                }
                if ((s32)tmp == 0x53e) return;
                if ((s32)tmp < 0x53e) {
                    if ((s32)tmp == 0x53c) return;
                }
                if ((s32)tmp >= 0x53c) goto L_80012680;
                goto L_800123CC;
            }
            if ((s32)tmp >= 0x540) return;
            goto L_80012758;

            tmp = *(u8*)((u8*)r30 + 0x29);
            if (tmp != 2) return;
            r5 = *(u16*)((u8*)r31 + 0x6);
            tmp = 0x43300000;
            *(u32*)(sp + 0x8) = tmp;
            r3 = r27;
            f2 = lbl_8047B730;
            r4 = r28;
            f0 = lbl_8047B720;
            f1 = f1 - f2;
            f1 = f1 / f0;
            fn_8001DACC();
            r3 = r28;
            r4 = 0x0;
            ((void(*)(void))fn_80109220)();
            return;
        L_8001215C:
            r5 = *(u16*)((u8*)r30 + 0x26);
            r3 = r27;
            r4 = r28;
            fn_8010B9E8();
            return;
        L_80012170:
        do {
            r3 = 0x0;
            r4 = 0x0;
            r5 = 0x32;
            r6 = 0x0;
            fn_801F54A4();
            if ((s32)r3 != 0) {
                tmp = *(u8*)((u8*)r30 + 0x16);
                if (tmp != 0) return;
            }
            r3 = *(s16*)((u8*)r31 + 0x2);
            if ((s32)r3 != 0) {
                tmp = *(s16*)((u8*)r31 + 0x4);
                r5 = 0x43300000;
                r6 = *(s16*)((u8*)r31 + 0x0);
                r3 = *(s16*)((u8*)r30 + 0x1A);
                *(u32*)(sp + 0xC) = tmp;
                r3 = r3 - r6;
                f3 = lbl_8047B738;
                f1 = f0 - f3;
                f0 = f0 - f3;
                f2 = f1 / f0;
                *(u32*)(sp + 0x24) = tmp;
                f1 = f1 - f3;
                f0 = f0 - f3;
                f1 = f2 * f1 + f0;
                f0 = (f64)(s32)f1;
                tmp = (s16)r27;
                if ((s32)tmp != 0) break;
                f0 = lbl_8047B718;
                if (f1 <= f0) break;
                r27 = 0x1;
                break;
            }
            r27 = *(s16*)((u8*)r30 + 0x1A);
        } while (0);
            r5 = r29;
            r3 = 0x20;
            r4 = -0x2;
            r6 = 0x195;
            ((void(*)(void))fn_800FB680)();
            r4 = (s16)r27;
            r3 = 0x34;
            ((void(*)(void))fn_80132A38)();
            r3 = 0xcb;
            ((void(*)(void))fn_800FA444)();
            tmp = (u32)r3 >> 16;
            r5 = r29;
            tmp = (s16)tmp;
            r4 = -0x1;
            tmp = 0x18 - tmp;
            r6 = 0xcb;
            r3 = (s16)tmp;
            ((void(*)(void))fn_800FB680)();
            r4 = *(s16*)((u8*)r30 + 0x18);
            r3 = 0x34;
            ((void(*)(void))fn_80132A38)();
            r3 = 0xcb;
            ((void(*)(void))fn_800FA444)();
            r3 = (u32)r3 >> 16;
            tmp = *(s16*)((u8*)r28 + 0x54);
            r3 = (s16)r3;
            r5 = r29;
            tmp = tmp - r3;
            r4 = -0x1;
            r3 = (s16)tmp;
            r6 = 0xcb;
            ((void(*)(void))fn_800FB680)();
            return;

            r4 = *(u8*)((u8*)r30 + 0x17);
            r3 = 0x34;
            ((void(*)(void))fn_80132A38)();
            r3 = 0xcb;
            ((void(*)(void))fn_800FA444)();
            r3 = (u32)r3 >> 16;
            tmp = *(s16*)((u8*)r28 + 0x54);
            r3 = (s16)r3;
            r5 = *(u8*)((u8*)r27 + 0x8B);
            r3 = tmp - r3;
            tmp = -0x100;
            r3 = (s16)r3;
            r4 = -0x1;
            r5 = r5 | tmp;
            r6 = 0xcb;
            ((void(*)(void))fn_800FB680)();
            return;
        }
            }
    r4 = r30;
    r3 = 0x37;
    ((void(*)(void))fn_80132A38)();
    r5 = r29;
    r3 = 0x0;
    r4 = -0x1;
    r6 = 0xe9;
    ((void(*)(void))fn_800FB680)();
    r3 = 0xe9;
    ((void(*)(void))fn_800FA444)();
    tmp = *(u8*)((u8*)r30 + 0x28);
    r3 = (u32)r3 >> 16;
    r27 = (s16)r3;
    do {
        if ((s32)tmp != 1) {
            if ((s32)tmp < 1) {
                if ((s32)tmp < 0) {
                    goto L_80012370;
                }
                goto L_80012370;
                }
            r3 = 0xd67;
            break;
        }
        r3 = 0xd68;
        break;
    L_80012370:
        r3 = 0x0;
    } while (0);

    if (r3 == 0) return;
    fn_800FA280();
    r4 = r3;
    r3 = 0x37;
    ((void(*)(void))fn_80132A38)();
    r3 = r27;
    r5 = r29;
    r4 = -0x1;
    r6 = 0xd0;
    ((void(*)(void))fn_800FB680)();
    return;
L_800123A4:
    r9 = *(u16*)((u8*)r30 + 0x24);
    r7 = r29;
    r5 = *(s16*)((u8*)r28 + 0x54);
    r8 = r27;
    r6 = *(s16*)((u8*)r28 + 0x56);
    r3 = 0x0;
    r4 = 0x0;
    r10 = 0x0;
    fn_80104160();
    return;
L_800123CC:
    r6 = *(s16*)((u8*)r31 + 0x2);
    if ((s32)r6 != 0) {
        tmp = *(s16*)((u8*)r31 + 0x4);
        r5 = 0x43300000;
        r7 = *(s16*)((u8*)r31 + 0x0);
        r3 = *(s16*)((u8*)r30 + 0x1A);
        *(u32*)(sp + 0x2C) = tmp;
        r3 = r3 - r7;
        f3 = lbl_8047B738;
        f1 = f0 - f3;
        f0 = f0 - f3;
        f2 = f1 / f0;
        *(u32*)(sp + 0x14) = tmp;
        f1 = f1 - f3;
        f0 = f0 - f3;
        f31 = f2 * f1 + f0;
    } else {

        r3 = *(s16*)((u8*)r30 + 0x1A);
        tmp = 0x43300000;
        *(u32*)(sp + 0x8) = tmp;
        f1 = lbl_8047B738;
        *(u32*)(sp + 0xC) = tmp;
        f31 = f0 - f1;
    }
    tmp = *(s16*)((u8*)r30 + 0x18);
    r3 = 0x43300000;
    f3 = lbl_8047B738;
    *(u32*)(sp + 0x34) = tmp;
    f30 = f0 - f3;
    if ((s32)r6 != 0) {
        r4 = *(s16*)((u8*)r31 + 0x0);
        do {
            if ((s32)r4 <= 0) {
                r31 = 0x0;
                break;
            }
            f0 = lbl_8047B724;
            f1 = lbl_8047B71C;
            f0 = f0 * f30;
            *(u32*)(sp + 0x34) = tmp;
            f0 = f0 / f1;
            f2 = f2 - f3;
            /* cror eq, lt, eq */;
            if (f2 == f0) {
                r31 = 0x80000000;
                break;
            }
            f0 = lbl_8047B728;
            *(u32*)(sp + 0x34) = tmp;
            f0 = f0 * f30;
            f0 = f0 / f1;
            f1 = f1 - f3;
            /* cror eq, lt, eq */;
            if (f1 == f0) {
                r31 = 0x64640000;
                break;
            }
            r31 = 0x800000;
        } while (0);

        r3 = *(s16*)((u8*)r28 + 0x54);
        tmp = 0x43300000;
        f0 = lbl_8047B72C;
        r31 = r31 | r8;
        r4 = r4 * r3;
        *(u32*)(sp + 0x30) = tmp;
        f1 = lbl_8047B738;
        f2 = f30 - f0;
        r3 = 0x1;
        *(u32*)(sp + 0x34) = tmp;
        f0 = f0 - f1;
        f0 = f2 + f0;
        f0 = f0 / f30;
        f0 = (f64)(s32)f0;
        fn_800D88DC();
        r3 = 0x6;
        fn_800D888C();
        r3 = 0x7;
        fn_800D6A00();
        r3 = (u32)lbl_80314E08;
        r3 = (u32)lbl_80314E08;
        fn_800D7820();
        r3 = 0x2;
        fn_800D67BC();
        r3 = 0x0;
        r4 = 0x0;
        fn_800D61E4();
        r4 = r31;
        r3 = 0x0;
        fn_800D5BA0();
        r4 = *(s16*)((u8*)r28 + 0x56);
        r3 = r30;
        fn_800D61E4();
        r4 = r31;
        r3 = 0x0;
        fn_800D5BA0();
        fn_800D6728();
    }
    f0 = lbl_8047B718;
    /* cror eq, lt, eq */;
    do {
        if (f31 == f0) {
            r9 = 0x0;
            break;
        }
        f0 = lbl_8047B724;
        f1 = lbl_8047B71C;
        f0 = f0 * f30;
        f0 = f0 / f1;
        /* cror eq, lt, eq */;
        if (f31 == f0) {
            r9 = 0x1ad;
            break;
        }
        f0 = lbl_8047B728;
        f0 = f0 * f30;
        f0 = f0 / f1;
        /* cror eq, lt, eq */;
        if (f31 == f0) {
            r9 = 0x1b0;
            break;
        }
        r9 = 0x1b1;
    } while (0);

    r4 = *(s16*)((u8*)r28 + 0x54);
    r3 = 0x43300000;
    f0 = lbl_8047B72C;
    tmp = r9 & 0xFFFF;
    f2 = lbl_8047B738;
    f0 = f30 - f0;
    f1 = f1 - f2;
    f0 = f31 * f1 + f0;
    f0 = f0 / f30;
    f0 = (f64)(s32)f0;
    if (tmp == 0) return;
    r6 = *(s16*)((u8*)r28 + 0x56);
    r7 = r29;
    r8 = r27;
    r3 = 0x0;
    r4 = 0x0;
    r10 = 0x0;
    fn_80104160();
    return;
L_80012680:
    r3 = *(s16*)((u8*)r31 + 0xC);
    if ((s32)r3 != 0) {
        tmp = *(s16*)((u8*)r31 + 0xE);
        r4 = 0x43300000;
        r6 = *(u32*)((u8*)r31 + 0x8);
        tmp = *(u32*)((u8*)r30 + 0x20);
        tmp = tmp - r6;
        f3 = lbl_8047B738;
        f2 = lbl_8047B730;
        f1 = f0 - f3;
        *(u32*)(sp + 0x24) = tmp;
        f0 = f0 - f3;
        f3 = f1 / f0;
        f1 = f1 - f2;
        f0 = f0 - f2;
        f1 = f3 * f1 + f0;
        fn_800C46B0();
    } else {

        r3 = *(u32*)((u8*)r30 + 0x20);
    }
    r11 = *(u32*)((u8*)r30 + 0x1C);
    if (r3 > r11) {
        r3 = r11;
    }
    if (r11 == 0) return;
    tmp = *(s16*)((u8*)r28 + 0x54);
    r7 = r29;
    r6 = *(s16*)((u8*)r28 + 0x56);
    r8 = r27;
    tmp = r3 * tmp;
    r3 = 0x0;
    r4 = 0x0;
    r9 = 0x1ac;
    r10 = 0x0;
    r5 = r11 + tmp;
    tmp = (u32)tmp / (u32)r11;
    r5 = (s16)tmp;
    fn_80104160();
    return;
L_80012758:
    tmp = *(s16*)((u8*)r31 + 0xC);
    if ((s32)tmp != 0) {
        r4 = *(s16*)((u8*)r31 + 0xE);
        r3 = 0x43300000;
        f3 = lbl_8047B738;
        f4 = *(f32*)((u8*)r31 + 0x8);
        *(u32*)(sp + 0x2C) = tmp;
        f2 = f0 - f3;
        f0 = *(f32*)((u8*)r30 + 0x20);
        f0 = f0 - f4;
        f1 = f1 - f3;
        f1 = f2 / f1;
        f2 = f1 * f0 + f4;
    } else {

        f2 = *(f32*)((u8*)r30 + 0x20);
    }
    f3 = *(f32*)((u8*)r30 + 0x1C);
    if (f2 > f3) {
        f2 = f3;
    }
    r3 = *(s16*)((u8*)r28 + 0x54);
    tmp = 0x43300000;
    f0 = lbl_8047B718;
    *(u32*)(sp + 0x20) = tmp;
    f1 = lbl_8047B738;
    f0 = f0 - f1;
    f0 = f2 * f0;
    f0 = f0 / f3;
    f0 = (f64)(s32)f0;
    if (f2 > f0) {
        tmp = (s16)r5;
        if ((s32)tmp == 0) {
            r5 = 0x1;
    }
    }
    r6 = *(s16*)((u8*)r28 + 0x56);
    r7 = r29;
    r8 = r27;
    r3 = 0x0;
    r4 = 0x0;
    r9 = 0x1ab;
    r10 = 0x0;
    fn_80104160();

    return;
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_8000D710 - 0x8000D710 | size: 0x398 */
extern void fn_8012C540(void);
extern void fn_80116D30(void);
extern void fn_8012BCA4(void);
extern void fn_801D23C0(void);
extern void fn_800EC918(void);
extern void fn_800D37CC(void);
extern void fn_8010206C(void);
extern void fn_80102014(void);
extern void fn_801661D0(void);
extern void fn_800D3074(void);
extern void fn_801906A0(void);
extern void fn_800FF560(void);
extern void fn_80130CD8(void);
extern void fn_800FE6F8(void);
extern void fn_800F03D4(void);
extern void GSthreadCreate(void);
extern void fn_800FE6DC(void);
extern void fn_80102510(void);
extern void fn_800EC8DC(void);
extern void fn_8001BDF4(void);
extern void fn_8004BE0C(void);
extern void fn_80018F88(void);
extern void fn_80019070(void);
extern void fn_8012BAD0(void);
extern void fn_801CBAB8(void);
extern void fn_80109764(void);
extern void fn_801660D8(void);
extern void fn_80102038(void);
extern void menuCloseSync(void);
extern void fn_8012BBA8(void);
extern u8 lbl_8047A2A0;
extern u32 lbl_8047B6F8;
extern u32 lbl_8047B6F0;
extern u32 lbl_8047A2A4;
extern u32 lbl_8047A2A8;
extern u32 lbl_8047A2AC;
extern u32 lbl_8047A2B0;
u32 fn_8000DAB0(void); /* forward decl: referenced by fn_8000D710 asm inc */
#if 1
asm void fn_8000D710(void) {
#include "src/game/gs_npc_interact_fn_8000D710.inc"
}
#else
void fn_8000D710(void) { /* TODO */ }
#endif

/* fn_8000DAB0 - 0x8000DAB0 | size: 0x38 */
extern u32 fn_8010264C(u32, u32);
extern void fn_800F0384(u32);
extern u32 lbl_8047A2A4;
extern u32 lbl_8047A2A8;
extern u32 lbl_8047A2B0;
#if 0
asm void fn_8000DAB0(void) {
#include "src/game/gs_npc_interact_fn_8000DAB0.inc"
}
#else
#pragma peephole off
u32 fn_8000DAB0(void) {
    lbl_8047A2B0 = fn_8010264C(lbl_8047A2A4, 1);
    fn_800F0384(lbl_8047A2A8);
    return lbl_8047A2B0;
}
#pragma peephole on
#endif

/* fn_8000DAE8 - 0x8000DAE8 | size: 0x1a0 */
extern void* fn_8001D834(void*, void*);
extern void fn_800FBB34(void);
extern u8 lbl_802E4B98[];
extern u8 lbl_803A1B80[];
#if 1
asm void fn_8000DAE8(void) {
#include "src/game/gs_npc_interact_fn_8000DAE8.inc"
}
#else
void fn_8000DAE8(void) { /* TODO */ }
#endif

/* fn_8000DC88 - 0x8000DC88 | size: 0x84 */
extern void fn_80265B74(void);
extern void fn_801040B8(void);
extern void fn_8026595C(void);
#if 0
asm void fn_8000DC88(void) {
#include "src/game/gs_npc_interact_fn_8000DC88.inc"
}
#else
#pragma peephole off
u32 fn_8000DC88(u8* ptr) {
    extern f64 fn_80265B74(void);
    extern f64 fn_8026595C(void);
    extern void fn_801040B8(u8* a, u32 b, s32 c);
    switch (*(s32*)(ptr + 4)) {
        case 0x10a:
            fn_801040B8(ptr, 0, (s32)fn_80265B74());
            break;
        case 0x10b:
            fn_801040B8(ptr, 0, (s32)fn_8026595C());
            break;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000DD0C - 0x8000DD0C | size: 0x24 */
#if 0
asm void fn_8000DD0C(void) {
#include "src/game/gs_npc_interact_fn_8000DD0C.inc"
}
#else
#pragma peephole off
u32 fn_8000DD0C(void) { return fn_80102620(0x10a); }
#pragma peephole on
#endif

/* fn_8000DD30 - 0x8000DD30 | size: 0x2c */
#if 0
asm void fn_8000DD30(void) {
#include "src/game/gs_npc_interact_fn_8000DD30.inc"
}
#else
#pragma peephole off
void fn_8000DD30(void) { fn_80102568(0x10a, 0, 0); }
#pragma peephole on
#endif

/* fn_8000DD5C - 0x8000DD5C | size: 0x3c */
#if 0
asm void fn_8000DD5C(void) {
#include "src/game/gs_npc_interact_fn_8000DD5C.inc"
}
#else
#pragma peephole off
void fn_8000DD5C(void) { fn_801026A4(0x10a, -1, 0, 0, 0, 0); }
#pragma peephole on
#endif

/* fn_8000DD98 - 0x8000DD98 | size: 0x24 */
#if 0
asm void fn_8000DD98(void) {
#include "src/game/gs_npc_interact_fn_8000DD98.inc"
}
#else
#pragma peephole off
u32 fn_8000DD98(void) { return fn_80102620(0x10b); }
#pragma peephole on
#endif

/* fn_8000DDBC - 0x8000DDBC | size: 0x2c */
#if 0
asm void fn_8000DDBC(void) {
#include "src/game/gs_npc_interact_fn_8000DDBC.inc"
}
#else
#pragma peephole off
void fn_8000DDBC(void) { fn_80102568(0x10b, 0, 0); }
#pragma peephole on
#endif

/* fn_8000DDE8 - 0x8000DDE8 | size: 0x3c */
#if 0
asm void fn_8000DDE8(void) {
#include "src/game/gs_npc_interact_fn_8000DDE8.inc"
}
#else
#pragma peephole off
void fn_8000DDE8(void) { fn_801026A4(0x10b, -1, 0, 0, 0, 0); }
#pragma peephole on
#endif

/* fn_8000DE24 - 0x8000DE24 | size: 0xa0 */
extern void fn_80102ED4(void);
#if 0
asm void fn_8000DE24(void) {
#include "src/game/gs_npc_interact_fn_8000DE24.inc"
}
#else
#pragma push
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
void fn_8000DE24(u8* ptr) {
    extern void fn_80102ED4(u8* a);
    extern u8 fn_801F18DC(s32 a);
    extern u8 fn_801F1700(s32 a);
    extern u8 fn_80265924(void);
    extern u16 fn_801EF634(void);
    u8 flag;
    fn_80102ED4(ptr);
    if (!(u8)fn_801F18DC(0)) goto _zero;
    if ((u8)fn_801F1700(0) == 1 && (u8)fn_80265924() == 1) { flag = 1; goto _check; }
    if ((u16)fn_801EF634() == 1) { flag = 1; goto _check; }
    _zero:
    flag = 0;
    _check:
    if (flag) {
        ptr[0x98] = 1;
        ptr[0x99] = 1;
    }
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma pop
#endif

/* fn_8000DEC4 - 0x8000DEC4 | size: 0x12c */
#if 0
asm void fn_8000DEC4(void) {
#include "src/game/gs_npc_interact_fn_8000DEC4.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8000DEC4(u8* arg1, u8* arg2) {
    extern void* fn_801040D0(u8* a, u32 b);
    extern void fn_80109220(u8* a, u32 b);
    u8* entry;
    s32 i;
    entry = (u8*)fn_801040D0(arg1, 0);
    for (i = 0; i < 2; i++) {
        s32 value;
        s16 npc_id;
        switch (*(s32*)(entry + 4)) {
            case 0x45: value = 0x125C; break;
            case 0x46: value = 0x125E; break;
            case 0x47: value = 0x12BE; break;
            case 0x48: value = 0x125D; break;
            default: value = 0; break;
        }
        npc_id = *(s16*)(arg2 + 6);
        if (value == npc_id) {
            fn_80109220(arg2, 1);
            return;
        }
        switch (*(s32*)(entry + 0xC)) {
            case 0x45: value = 0x125C; break;
            case 0x46: value = 0x125E; break;
            case 0x47: value = 0x12BE; break;
            case 0x48: value = 0x125D; break;
            default: value = 0; break;
        }
        if (value == npc_id) {
            fn_80109220(arg2, 1);
            return;
        }
        entry += 0x10;
    }
    fn_80109220(arg2, 0);
}
#pragma pop
#endif

/* fn_8000DFF0 - 0x8000DFF0 | size: 0x214 */
extern void fn_80105624(void);
#if 1
asm void fn_8000DFF0(void) {
#include "src/game/gs_npc_interact_fn_8000DFF0.inc"
}
#else
void fn_8000DFF0(void) { /* TODO */ }
#endif

/* fn_8000E204 - 0x8000E204 | size: 0x88 */
extern void fn_8005D8B8(void);
extern void fn_801022B8(void);
#if 0
asm void fn_8000E204(void) {
#include "src/game/gs_npc_interact_fn_8000E204.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8000E204(u8* arg1, u8* arg2) {
    extern u32 fn_8005D8B8(s16 val);
    extern s32 fn_801022B8(u32 val);
    extern void fn_80109220(u8* a, u32 b);
    if ((u8)fn_8005D8B8(*(s16*)(arg2 + 6)) != 0) {
        if (*(s16*)(arg2 + 6) == fn_801022B8(*(u32*)(arg1 + 4))) {
            fn_80109220(arg2, 1);
        } else {
            fn_80109220(arg2, 0);
        }
    } else {
        fn_80109220(arg2, 0);
    }
}
#pragma pop
#endif

/* fn_8000E28C - 0x8000E28C | size: 0x4 */
#if 0
asm void fn_8000E28C(void) {
#include "src/game/gs_npc_interact_fn_8000E28C.inc"
}
#else
void fn_8000E28C(void) {}
#endif

/* fn_8000EA10 - 0x8000EA10 | size: 0x324 */
extern u32 fn_801F2A7C(s32 arg);
extern u32 fn_801F986C(u32 warpId, u16 variant);
extern void fn_8012640C(void);
extern void fn_80123FBC(void);
extern void fn_8001D624(void);
extern void jumptable_802E4C20();
#if 1
asm void fn_8000EA10(void) {
#include "src/game/gs_npc_interact_fn_8000EA10.inc"
}
#else
void fn_8000EA10(void) { /* TODO */ }
#endif

/* fn_8000ED34 - 0x8000ED34 | size: 0x5dc */
extern void fn_80102254(void);
extern void fn_800F7920(void);
extern void fn_800F7994(void);
extern void fn_800CE2D8(void);
extern u32 lbl_8047B710;
extern u32 lbl_8047B700;
extern u32 lbl_8047B704;
extern u32 lbl_8047B708;
extern u8 lbl_8047885C[4];
#if 1
asm void fn_8000ED34(void) {
#include "src/game/gs_npc_interact_fn_8000ED34.inc"
}
#else
void fn_8000ED34(void) { /* TODO */ }
#endif

/* fn_8000F310 - 0x8000F310 | size: 0x4c */
#if 0
asm void fn_8000F310(void) {
#include "src/game/gs_npc_interact_fn_8000F310.inc"
}
#else
#pragma peephole off
u32 fn_8000F310(u32 arg) {
    fn_801040D0((void*)arg, 0);
    fn_801040D0((void*)arg, 1);
    fn_801040D0((void*)arg, 2);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000F35C - 0x8000F35C | size: 0xa4 */
extern void jumptable_802E4C80();
#if 1
asm void fn_8000F35C(void) {
#include "src/game/gs_npc_interact_fn_8000F35C.inc"
}
#else
void fn_8000F35C(void) { /* TODO */ }
#endif

/* fn_8000F400 - 0x8000F400 | size: 0x368 */
extern void fn_80205B8C(void);
extern void fn_8011BEB4(void);
extern void fn_800FB8C8(void);
extern void jumptable_802E4CA8();
#if 1
asm void fn_8000F400(void) {
#include "src/game/gs_npc_interact_fn_8000F400.inc"
}
#else
void fn_8000F400(void) { /* TODO */ }
#endif

/* fn_8000F768 - 0x8000F768 | size: 0x1fc */
extern void jumptable_802E4CD8();
#if 1
asm void fn_8000F768(void) {
#include "src/game/gs_npc_interact_fn_8000F768.inc"
}
#else
void fn_8000F768(void) { /* TODO */ }
#endif

/* fn_8000F964 - 0x8000F964 | size: 0x474 */
extern u32 lbl_8047B710;
extern u32 lbl_8047B700;
extern u32 lbl_8047B704;
extern u32 lbl_8047B708;
extern u8 lbl_80478858[4];
#if 1
asm void fn_8000F964(void) {
#include "src/game/gs_npc_interact_fn_8000F964.inc"
}
#else
void fn_8000F964(void) { /* TODO */ }
#endif

/* fn_8000FDD8 - 0x8000FDD8 | size: 0x60 */
extern void fn_80103FFC(void);
extern void* memcpy(void* dst, const void* src, u32 n);
#if 0
asm void fn_8000FDD8(void) {
#include "src/game/gs_npc_interact_fn_8000FDD8.inc"
}
#else
#pragma peephole off
u32 fn_8000FDD8(u8* ptr) {
    extern void* fn_80103FFC(u8* a, u32 size);
    extern void fn_80103FE4(u8* a);
    if ((s8)ptr[1] == 0) {
        void* dst = fn_80103FFC(ptr, 0x48);
        if (dst != NULL) {
            memcpy(dst, *(void**)(ptr + 0x60), 0x48);
        }
    }
    fn_80103FE4(ptr);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000FE38 - 0x8000FE38 | size: 0x118 */
#if 0
asm void fn_8000FE38(void) {
#include "src/game/gs_npc_interact_fn_8000FE38.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8000FE38(u8* arg1) {
    extern void* fn_80105624(void);
    extern u8 fn_801F18DC(s32 a);
    extern u8 fn_801F1700(s32 a);
    extern u8 fn_80265924(void);
    extern u16 fn_801EF634(void);
    void* data;
    u16 flags;
    s32 flag_val;
    u8 flag;
    data = fn_80105624();
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
        if ((u8)fn_801F1700(0) == 1) {
            if ((u8)fn_80265924() == 1) {
                flag = 1;
                goto got_flag;
            }
        }
        if ((u16)fn_801EF634() == 1) {
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
#pragma pop
#endif

/* fn_8000FF50 - 0x8000FF50 | size: 0x58 */
#if 0
asm void fn_8000FF50(void) {
#include "src/game/gs_npc_interact_fn_8000FF50.inc"
}
#else
#pragma peephole off
u32 fn_8000FF50(u8* ptr) {
    extern void* fn_80103FFC(u8* a, u32 size);
    if ((s8)ptr[1] == 0) {
        void* dst = fn_80103FFC(ptr, 0x18);
        if (dst != NULL) {
            memcpy(dst, *(void**)(ptr + 0x60), 0x18);
        }
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000FFA8 - 0x8000FFA8 | size: 0x118 */
extern void jumptable_802E4D2C();
#if 1
asm void fn_8000FFA8(void) {
#include "src/game/gs_npc_interact_fn_8000FFA8.inc"
}
#else
void fn_8000FFA8(void) { /* TODO */ }
#endif

/* fn_800100C0 - 0x800100C0 | size: 0x68 */
#if 0
asm void fn_800100C0(void) {
#include "src/game/gs_npc_interact_fn_800100C0.inc"
}
#else
#pragma peephole off
u32 fn_800100C0(u8* ptr) {
    extern void* fn_801040A0(u8* a);
    extern void* fn_801040D0(u8* a, u32 b);
    void* dst = fn_801040A0(ptr);
    if ((s8)ptr[1] == 0) {
        memcpy(dst, fn_801040D0(ptr, 0), 6);
    }
    return 0;
}
#pragma peephole on
#endif

/* _menuFightIsUse__FP16MENU_WAZA_STATUSUs - 0x80010128 | size: 0x16c */
extern void fn_801FFEC8(void);
extern void fn_802040E8(void);
#if 1
asm void _menuFightIsUse__FP16MENU_WAZA_STATUSUs(void) {
#include "src/game/gs_npc_interact_fn_80010128.inc"
}
#else
void _menuFightIsUse__FP16MENU_WAZA_STATUSUs(void) { /* TODO */ }
#endif

/* fn_80010294 - 0x80010294 | size: 0x1e8 */
extern void fn_80104530(void);
#if 1
asm void fn_80010294(void) {
#include "src/game/gs_npc_interact_fn_80010294.inc"
}
#else
void fn_80010294(void) { /* TODO */ }
#endif

/* fn_8001047C - 0x8001047C | size: 0x10c */
#if 0
asm void fn_8001047C(void) {
#include "src/game/gs_npc_interact_fn_8001047C.inc"
}
#else
#pragma push
#pragma peephole off
void fn_8001047C(u8* arg1) {
    extern void* fn_80103FE4(u8* a);
    extern u32 fn_80104530(u32 val);
    extern void* fn_80205B8C(void* obj);
    extern u32 fn_8012640C(s32 p1, s32 p2, s32 p3, s32 p4, u16 p5, s32 p6);
    extern void fn_80132A38(s32 p1, s32 val);
    extern void fn_801040F0(s32 p1, s32 p2, u8* p3, u16 p4, s32 p5);
    void* participant;
    u32 npc_data;
    s16 idx;
    s32 r30;
    u16 battle_result;
    u16 val;
    s32 temp;
    participant = fn_80103FE4(arg1);
    npc_data = fn_80104530(*(u32*)(arg1 + 4));
    idx = (s16)(npc_data >> 16);
    temp = (s8)(idx & 0xFF);
    if (temp < 0 || temp >= 4) return;
    r30 = temp * 0xc;
    if (*(u32*)((u8*)participant + r30 + 4) == 0) return;
    battle_result = 0;
    if (fn_80205B8C(*(void**)((u8*)participant + 0x40)) != 0) {
        battle_result = (u16)fn_8012640C(0, 0x7f, 0, 0, (s8)(*(u8*)(arg1 + 0x95)), 0);
    }
    val = 0;
    if (battle_result == 0 || battle_result == 0x164) {
        val = 0;
    } else if (battle_result < 0x166) {
        val = 0x5d;
    } else {
        val = *(u16*)((u8*)participant + r30 + 0xc);
    }
    if ((u16)val != 0) {
        fn_801040F0(0, 2, arg1, val, 0);
    }
}
#pragma pop
#endif

/* fn_80010588 - 0x80010588 | size: 0x11c */
#if 0
asm void fn_80010588(void) {
#include "src/game/gs_npc_interact_fn_80010588.inc"
}
#else
#pragma push
#pragma peephole off
void fn_80010588(u8* arg1, u8* arg2) {
    typedef struct {
        u32 unk_00;
        u32 value;
        u32 unk_08;
    } NpcInteractEntry;
    extern NpcInteractEntry* fn_80103FE4(u8* a);
    extern void* fn_801040A0(u8* a);
    extern void fn_800FB680(s32 a, s32 b, s32 c, u32 d);
    NpcInteractEntry* participant;
    void* npc_data;
    s32 idx;
    s16 npc_id;
    s32 offset;
    u32 result;
    participant = fn_80103FE4(arg1);
    npc_data = fn_801040A0(arg1);
    npc_id = *(s16*)(arg2 + 6);
    idx = 0;
    switch (npc_id) {
    case 0xC4:
        idx = 0;
        break;
    case 0xC5:
        idx = 1;
        break;
    case 0xC6:
        idx = 2;
        break;
    case 0xC7:
        idx = 3;
        break;
    }
    if ((s8)*((u8*)npc_data + 2) == idx) {
        fn_801040F0(0, 0, arg1, 0x49, 0);
        fn_801040F0(0, 0, arg1, 0x4A, 0);
    }
    offset = idx * 0xc;
    participant = (NpcInteractEntry*)((u32)participant + offset);
    result = participant->value;
    if (result != 0) {
        fn_80132A38(0x37, result);
        fn_800FB680(0, 0, (s32)menuSubCalcColor(arg1, arg2), 0xE7);
    }
}
#pragma pop
#endif

/* fn_800106A4 - 0x800106A4 | size: 0x1a0 */
#if 0
asm void fn_800106A4(void) {
#include "src/game/gs_npc_interact_fn_800106A4.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800106A4(u8* arg1, u8* arg2) {
    extern u32 fn_80103FE4(u8* a);
    extern u32 fn_801040A0(u8* a);
    extern u32 fn_801040D0(u8* a, s32 b);
    extern void fn_80132A38(s32 p1, s32 val);
    extern void* fn_8001D834(u8* a, u8* b);
    extern void fn_800FB680(s32 a, s32 b, s32 c, u32 d);
    extern void* fn_802037DC(void* a);
    void* participant;
    void* npc_data;
    u32 r30;
    s16 npc_id;
    s32 sub;
    s32 val;
    u8* p1;
    u32 t1;
    u32 t2;
    npc_id = *(s16*)(arg2 + 6);
    r30 = 0;
    switch (npc_id) {
    case 0xb4:
        t1 = fn_80103FE4(arg1);
        fn_80132A38(0x37, (s32)t1);
        r30 = 0xcf;
        break;
    case 0x11cd:
        t2 = fn_80103FE4(arg1);
        fn_80132A38(0x36, (s32)t2);
        r30 = 0x196;
        break;
    case 0xc1:
        participant = (void*)fn_80103FE4(arg1);
        npc_data = (void*)fn_801040A0(arg1);
        sub = (s32)*(u8*)npc_data;
        switch (sub) {
        case 0:
            fn_80132A38(0x36, *(s32*)participant);
            r30 = 0x196;
            break;
        case 1:
            r30 = 0x199;
            break;
        }
        break;
    case 0x11d8:
        r30 = 0x196;
        val = (s32)*(u32*)(arg1 + 4);
        if (val == 0xf7) {
            fn_80132A38(0x36, *(s32*)fn_80103FE4(arg1));
        } else if (val == 0xf8) {
            p1 = (u8*)fn_801040D0(arg1, 1);
            if ((u8)fn_801040D0(arg1, 2) != 0) {
                fn_80132A38(0x36, (s32)fn_802037DC(p1));
            } else {
                r30 = 0x1a9;
            }
        } else {
            return;
        }
        break;
    }
    if (r30 != 0) {
        fn_800FB680(0, -2, (s32)menuSubCalcColor(arg1, arg2), r30);
    }
}
#pragma pop
#endif

/* fn_80010844 - 0x80010844 | size: 0x15c */
extern void fn_80123C54(void);
extern void fn_801204A8(void);
#if 1
asm void fn_80010844(void) {
#include "src/game/gs_npc_interact_fn_80010844.inc"
}
#else
void fn_80010844(void) { /* TODO */ }
#endif

/* fn_800109A0 - 0x800109A0 | size: 0x190 */
extern u8 lbl_80478850[8];
#if 1
asm void fn_800109A0(void) {
#include "src/game/gs_npc_interact_fn_800109A0.inc"
}
#else
void fn_800109A0(void) { /* TODO */ }
#endif

/* fn_80010B30 - 0x80010B30 | size: 0x168 */
#pragma push
#pragma peephole off
#pragma peephole off
u32 fn_80010B30(u8* arg) {
    extern void* fn_80103FFC(u8* a, u32 size);
    extern void* fn_80103FE4(u8* a);
    extern s32 fn_801022B8(u32 val);
    extern u8* fn_801046C8(u8* a, s32 id);
    extern void fn_8005D8F8(s32 id, s32 flag);
    void* entry;
    void* participant;
    s32 trainer_id;
    u8* r;
    if ((s8)arg[1] == 0) {
        entry = fn_80103FFC(arg, 0x18);
        if (entry != NULL) {
            memcpy(entry, *(void**)(arg + 0x60), 0x18);
        }
        if (*(u8*)((u8*)entry + 0x16) != 0) {
            r = fn_801046C8(arg, 0xB6);
            *(s32*)(r + 0x4C) = 0x13D;
            r = fn_801046C8(arg, 0xB8);
            *(s32*)(r + 0x4C) = 0x140;
            fn_8005D8F8(0xB8, 1);
        } else {
            r = fn_801046C8(arg, 0xB6);
            *(s32*)(r + 0x4C) = 0x13F;
            r = fn_801046C8(arg, 0xB8);
            *(s32*)(r + 0x4C) = 0;
            fn_8005D8F8(0xB8, 0);
        }
    }
    participant = fn_80103FE4(arg);
    trainer_id = fn_801022B8(*(u32*)(arg + 4));
    switch (trainer_id) {
    case 0xB5:
        *(s32*)(arg + 0x80) = 0;
        break;
    case 0xB6:
        if (*(u8*)((u8*)participant + 0x16) != 0) {
            *(s32*)(arg + 0x80) = 1;
        } else {
            *(s32*)(arg + 0x80) = 3;
        }
        break;
    case 0xB7:
        *(s32*)(arg + 0x80) = 2;
        break;
    case 0xB8:
        *(s32*)(arg + 0x80) = 3;
        break;
    default:
        *(s32*)(arg + 0x80) = -1;
        break;
    }
    return 0;
}
#pragma peephole on
#pragma pop

/* fn_80010C98 - 0x80010C98 | size: 0x52c */
extern u32 fn_80207BF4(void* arg);
extern u8 fn_801F8C00(u32 warpId, u32 arg);
extern s32 fn_80203848(u32 arg);
extern void fn_800DD970(const char* fmt, ...);
extern u8 lbl_80266788[];
extern u8 lbl_802E4B78[];
#if 0
asm void fn_80010C98(void) {
#include "src/game/gs_npc_interact_fn_80010C98.inc"
}
#else
#pragma peephole off
u32 fn_80010C98(void* npc, u32 warpId, u32 variant) {
#define WAIT_FOR_DIALOG(waitLabel, checkLabel, haveLabel, doneLabel) \
    goto checkLabel; \
waitLabel: \
    advance = fn_801F18DC(0); \
    if (advance != 0) { \
        if ((fn_801F1700(0) == 1) && (fn_80265924() == 1)) { \
            advance = 1; \
            goto haveLabel; \
        } else if (fn_801EF634() == 1) { \
            advance = 1; \
            goto haveLabel; \
        } \
    } \
    advance = 0; \
haveLabel: \
    if (advance != 0) { \
        goto doneLabel; \
    } \
    _threadSwitch(); \
checkLabel: \
    inputFlags = fn_800F7AF0(1); \
    maskedFlags = fn_800F7BC4(1); \
    maskedFlags &= inputFlags; \
    if ((maskedFlags & 0x300) == 0) { \
        goto waitLabel; \
    } \
doneLabel:

    void* linkedNpc;
    u32 inputFlags;
    u32 maskedFlags;
    u8 relation;
    u8 advance;
    u8 kind;

    relation = fn_801F2020(0, npc, &linkedNpc);
    if (relation == 1) {
        fn_80132A38(0xD, (s32)fn_802037DC(npc));
        fn_80106D3C(1, 0x76FB, 1, 0);
        WAIT_FOR_DIALOG(waitRelationOne, checkRelationOne, haveRelationOne, doneRelationOne);
        fn_801069FC(1);
        return 0;
    }
    if (relation == 2) {
        fn_801F4C14(0, 0, 0x57, 0, (u16)fn_80207BF4(linkedNpc));
        fn_80132A38(0xD, (s32)fn_802037DC(linkedNpc));
        fn_80132A38(0xE, (s32)fn_802037DC(npc));
        fn_80106D3C(1, 0x761F, 1, 0);
        WAIT_FOR_DIALOG(waitRelationTwo, checkRelationTwo, haveRelationTwo, doneRelationTwo);
        fn_801069FC(1);
        return 0;
    }
    if (warpId == 0) {
        warpId = fn_801F2A7C(0);
    }
    if (warpId == 0) {
        return 0;
    }
    variant = fn_801F986C(warpId, (u16)variant);
    if (variant == 0) {
        return 0;
    }
    kind = fn_801F8C00(warpId, variant);
    if (kind == 1) {
        fn_80132A38(0xD, fn_80203848(variant));
        fn_80106D3C(1, 0x76FE, 1, 0);
        WAIT_FOR_DIALOG(waitKindOne, checkKindOne, haveKindOne, doneKindOne);
        fn_801069FC(1);
        return 0;
    }
    if (kind == 2) {
        fn_80132A38(0xD, fn_80203848(variant));
        fn_80106D3C(1, 0x76FC, 1, 0);
        WAIT_FOR_DIALOG(waitKindTwo, checkKindTwo, haveKindTwo, doneKindTwo);
        fn_801069FC(1);
        return 0;
    }
    if (kind == 3) {
        fn_80132A38(0xD, fn_80203848(variant));
        fn_80106D3C(1, 0x76FD, 1, 0);
        WAIT_FOR_DIALOG(waitKindThree, checkKindThree, haveKindThree, doneKindThree);
        fn_801069FC(1);
        return 0;
    }
    if (kind == 0) {
        return 1;
    }
    fn_800DD970((const char*)lbl_80266788, (const char*)lbl_802E4B78);
#undef WAIT_FOR_DIALOG
    return 0;
}
#pragma peephole on
#endif

/* fn_8001120C - 0x8001120C | size: 0x7c -- already decompiled above */
/* fn_800119A8 - 0x800119A8 | size: 0x74 -- already decompiled above */
/* fn_80011B4C - 0x80011B4C | size: 0x78 -- already decompiled above */
/* fn_80011C78 - 0x80011C78 | size: 0x78 -- already decompiled above */
