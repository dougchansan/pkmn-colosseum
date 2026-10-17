/**
 * @file gs_event_exec.c
 * @brief GSeventExec -- Scene scripting and event execution system.
 *
 * Address range: 0x80012858 - 0x80015000 (~35 functions)
 *
 * This module implements the event execution engine that drives scripted
 * sequences in the game. Events are triggered by NPC interactions, story
 * progression flags, item pickups, and location-based triggers.
 *
 * The event system uses a bytecode-like command format where each event
 * is a sequence of operations (set flag, display text, move NPC, play
 * animation, trigger battle, warp player, etc.).
 *
 * Key functions:
 *   fn_80012858  GSevent_InitExecutor         -- 0x150 bytes, initialize event state
 *   fn_800129A8  GSevent_RunStep              -- 0x1EC bytes, execute one event step
 *   fn_80012B94  GSevent_ProcessCommand       -- 0x18C bytes, decode & dispatch command
 *   fn_80012D20  GSevent_EvalCondition        -- 0xF8 bytes, evaluate flag conditions
 *   fn_80012E18  GSevent_SetFlag              -- 0x198 bytes, set game flag from event
 *   fn_80012FB0  GSevent_DisplayText          -- 0x2EC bytes, show event text box
 *   fn_8001329C  GSevent_MoveNPC              -- 0x3CC bytes, script NPC movement
 *   fn_80013668  GSevent_PlayAnimation        -- 0xDC bytes, trigger NPC animation
 *   fn_80013744  GSevent_Nop                  -- 8 bytes, no-op command
 *   itemStorageWithdraw  GSevent_WaitFrames           -- 0x168 bytes, wait N frames
 *   fn_800138B4  GSevent_FadeScreen           -- 0x164 bytes, screen fade in/out
 *   bagItemToss  GSevent_CameraControl        -- 0x3E4 bytes, camera movement script
 *   bagItemGive  GSevent_PlaySound            -- 0x184 bytes, play sound effect
 *   bagItemUse  GSevent_PlayMusic            -- 0x17C bytes, change BGM
 *   fn_800140FC  GSevent_StopMusic            -- 0x14 bytes, stop current BGM
 *   fn_80014110  GSevent_ReturnTrue           -- 8 bytes, stub returns 1
 *   itemUseGetTargetPokemon  GSevent_CheckPartyState      -- 0x80 bytes, check party condition
 *   itemUseClosePartyMenu  GSevent_GetPartySize         -- 0x24 bytes, return party count
 *   itemUseOpenPartyMenu  GSevent_GiveItem             -- 0x78 bytes, give item to player
 *   fn_80014234  GSevent_GivePokemon          -- 0xE8 bytes, give Pokemon to player
 *   fn_8001431C  GSevent_CheckBag             -- 0x7C bytes, check bag for item
 *   fn_80014398  GSevent_BranchOnFlag         -- 0x1B8 bytes, conditional branch
 *   fn_80014550  GSevent_UnlockLocation       -- 0x24 bytes, unlock map location
 *   fn_80014574  GSevent_CutsceneSequence     -- 0x4D4 bytes, full cutscene playback
 *   menuPocket2NumCtrl  GSevent_SetWeather           -- 0x9C bytes, change weather effect
 *   fn_80014AE4  GSevent_SetTimeOfDay         -- 0xBC bytes, set time/lighting
 *   fn_80014BA0  GSevent_TriggerBattle        -- 0x98 bytes, start battle from event
 *   fn_80014C38  GSevent_ShadowEncounter      -- 0xE4 bytes, Shadow Pokemon event
 *   fn_80014D1C  GSevent_PurificationEvent    -- 0x134 bytes, purification ceremony
 *   fn_80014E50  GSevent_ColosseumMatch       -- 0xF8 bytes, colosseum match setup
 *   fn_80014F48  GSevent_MtBattleFloor        -- 0xD4 bytes, Mt. Battle progression
 *
 * Event command format (reconstructed):
 *   Each command is a variable-length structure:
 *     byte 0:    Command opcode (0x00-0x3F)
 *     byte 1:    Flags / subcommand
 *     bytes 2-3: Parameter count
 *     bytes 4+:  Command-specific parameters
 *
 * The event executor (fn_800129A8) is a loop that:
 *   1. Reads the next command from the event bytecode stream
 *   2. Dispatches to the appropriate handler via fn_80012B94
 *   3. If the handler returns "wait", yields until next frame
 *   4. If the handler returns "done", advances to next command
 *   5. If the handler returns "branch", jumps to the target offset
 *
 * Rodata references:
 *   lbl_80266788: Shift-JIS format string for NPC dialog ("%s: [dialog text]")
 *   lbl_802667B0-802668AC: Various event-related string templates
 *
 * SDA globals:
 *   Variables in 0x8047A2A0-0x8047A2D0 range hold event execution state:
 *   - Current event ID
 *   - Program counter within event
 *   - Stack depth for nested events
 *   - Wait counter for timed events
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Flag system */
extern void fn_80190048(s32 flagId, s32 value);    /* GSflagSet */
extern s32  fn_80190118(s32 flagId);                /* GSflagGet */

/* Text system */
extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
extern void winMsgClose(s32 slot);

/* Sound system */
extern void soundStartRaw(s32 soundId, s32 p2, s32 p3);   /* Play SE */
extern void soundStopRaw(s32 soundId, s32 p2, s32 p3);   /* Play BGM */

/* Scene/camera */
extern void floorLink(s32 cameraId, s32 mode);
extern u8   fn_800FF548(void);                     /* Scene transition check */
extern void std092_getCurrentRoomID(s32 floorId);              /* Floor load */

/* Frame control */
extern void _threadSwitch(void);                     /* Frame advance */
extern void GSthreadTerminate(void* threadCtx);          /* Resume thread */
extern void GSthreadCreate(s32 priority, void* stack, s32 stackSize,
                         s32 flags, s32 p5, void* entry);

/* =========================================================================
 * Data tables
 * ========================================================================= */

/* sSummaryPageEntries: Event/page handler table
 * Array of structures, each 0x4C bytes:
 *   +0x00: Event type ID (s32)
 *   +0x04: Handler function pointer (or -1 for default)
 *   +0x08: Parameter block pointer
 *   +0x0C-0x48: Type-specific data
 *
 * This table is referenced heavily by fn_80015050 and fn_800150E4
 * which index into it using: entry = table + (slotIndex * 0x4C)
 */

#if !defined(GS_EVENT_EXEC_ONLY_80014A48) && !defined(GS_EVENT_EXEC_ONLY_80014574)
/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

extern u32 lbl_8047A2EC;

#if !defined(GS_EVENT_EXEC_ONLY_80012D20) && !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)
/* Address: 0x80013744 | Size: 0x8 | Pattern: return_constant */
u32 fn_80013744(void) { return 1; }

/* Address: 0x80014110 | Size: 0x8 | Pattern: sda_getter */
u32 fn_80014110(void) {
    return lbl_8047A2EC;
}
#endif /* !GS_EVENT_EXEC_ONLY_80012D20 */

/* ===== Phase 2 recovery stubs ===== */

/* fn_800129A8 - 0x800129A8 | size: 0x1ec */
extern u8*  windowGetFreeWork(void);
extern void* windowAllocMemory(void*, s32);
extern void* windowGetAllocPtr(void*);
extern u8   windowSearchID(s32);
extern void fn_80103F74(s32, s32, s32);
extern void soundStopSE(s32);
extern void* memcpy(void* dst, const void* src, u32 n);
#if !defined(GS_EVENT_EXEC_ONLY_80012D20) && !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)
s32 fn_800129A8(u8* ctx) {
    u8* p;
    s32 id;
    s32 kind;
    void* buf;
    p = windowGetFreeWork();
    if ((s32)(s8)ctx[1] == 0) {
        buf = windowAllocMemory(ctx, 0x30);
        if (buf != 0) memcpy(buf, *(void**)(ctx + 0x60), 0x30);
    }
    windowGetAllocPtr(ctx);
    if ((s32)(s8)ctx[1] == 0) {
        id = *(s32*)(ctx + 4);
        kind = 0;
        if (windowSearchID(id) != 0) {
            if (id == 0x49) kind = 0x538;
            else if (id >= 0x49) { if (id < 0x4b) kind = 0x540; }
            else if (id >= 0x47) kind = 0x540;
            else if (id >= 0x45) kind = 0x538;
            fn_80103F74(id, kind, 0);
        }
        id = *(s32*)(ctx + 4);
        kind = 0;
        if (windowSearchID(id) != 0) {
            if (id == 0x49) kind = 0x539;
            else if (id >= 0x49) { if (id < 0x4b) kind = 0x541; }
            else if (id >= 0x47) kind = 0x541;
            else if (id >= 0x45) kind = 0x539;
            fn_80103F74(id, kind, 0);
        }
    }
    if (*(s16*)(p + 2) != 0) {
        *(s16*)(p + 4) += 1;
        if (*(s16*)(p + 4) > *(s16*)(p + 2)) {
            *(s16*)(p + 2) = 0;
        }
    }
    if (*(s16*)(p + 0xC) != 0) {
        *(s16*)(p + 0xE) += 1;
        if (*(s16*)(p + 0xE) > *(s16*)(p + 0xC)) {
            *(s16*)(p + 0xC) = 0;
            soundStopSE(0x4d0);
        }
    }
    *(u16*)(p + 6) += 1;
    *(u16*)(p + 6) = (u16)(*(u16*)(p + 6) % 1200);
    return 0;
}
#endif /* !GS_EVENT_EXEC_ONLY_80012D20 */

/* fn_80012B94 - 0x80012B94 | size: 0x18c */
extern void* windowGetParam(void*, s32);
extern s32   GSmsgGetRect(s32);
extern void  GSmsgPrintColor(s32, s32, s32, s32);
extern void fn_8001E644(s32, s32, s32, s32, u8);
extern void windowDrawPanel(s32, s32, s32, s32);
extern void windowDrawSprite(s32, s16, void*, s32, s32);
#if !defined(GS_EVENT_EXEC_ONLY_80012D20) && !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)
s32 fn_80012B94(u8* ctx) {
    u8   cmd;
    u8*  arr;
    s32  count;
    s32  cap;
    u8*  iter;
    s32  i;
    s32  max_hi;
    s32  sum_lo;
    s32  rng;
    s32  val;
    s32  delay;
    s32  acc;
    cmd   = (u8)(s32)windowGetParam(ctx, 0);
    arr   = (u8*)windowGetAllocPtr(ctx);
    count = (s32)(s8)(s32)windowGetParam(ctx, 2);
    cap   = (s32)(u8)menuDataBiosGetType(*(s32*)(ctx + 4));
    if (count > cap) count = cap;

    max_hi = 0;
    sum_lo = 0;
    iter = arr;
    i = 0;
    while (i < count) {
        rng = GSmsgGetRect(*(s32*)iter);
        if (max_hi < (s32)((u32)rng >> 16)) {
            max_hi = (s32)((u32)rng >> 16);
        }
        sum_lo += (s32)(rng & 0xFFFF);
        iter += 4;
        i++;
        sum_lo += 2;
    }

    if ((s32)cmd == 0x7f) {
        windowDrawPanel(0, 0, max_hi + 0x20, sum_lo);
    } else if ((s32)cmd < 2 && (s32)cmd >= 0) {
        fn_8001E644(0, 0, max_hi + 0x20, sum_lo, ctx[0x8B]);
    }

    iter = arr;
    acc = 1;
    i = 0;
    while (i < count) {
        val = *(s32*)iter;
        if (val != 0) {
            rng = GSmsgGetRect(val);
            GSmsgPrintColor(0x20, acc, -1, val);
            delay = (s32)(rng & 0xFFFF) + 2;
        } else {
            delay = 0x14;
        }
        if ((s32)(s8)ctx[0x95] == i) {
            windowDrawSprite(0x20, (s16)acc, ctx, 0x157, 0);
        }
        acc += delay;
        iter += 4;
        i++;
    }
    return 0;
}
#endif /* !GS_EVENT_EXEC_ONLY_80012D20 */

#if !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)
/* fn_80012D20 - 0x80012D20 | size: 0xf8 */
extern void winSeqSetMenu(s32, s32);
/* RULE-EXCEPTION(user-approved): local peephole and optimization-level pragmas; fn_80012D20 needs both (94.9% with peephole only, 92.9% with level 1 only) while the other four functions in its unit do not - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma push
#pragma optimization_level 1
s32 fn_80012D20(u8* arg) {
    s32   size;
    void* src;
    u8*   ctx = arg;
    void* buf;
    switch ((s32)(s8)ctx[1]) {
    case 0:
        if ((s32)(s8)ctx[2] == 0) {
            src  = windowGetParam(arg, 1);
            size = (s32)windowGetParam(ctx, 2) << 2;
            buf  = windowAllocMemory(ctx, size);
            if (buf != 0) {
                memcpy(buf, src, size);
            }
            winSeqSetMenu(*(s32*)(ctx + 4), 0x26);
            ctx[2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[2] == 0) {
            winSeqSetMenu(*(s32*)(ctx + 4), 0x2a);
            ctx[2] = 1;
        }
        break;
    case 5:
    default:
        break;
    }
    return 0;
}
#pragma pop
#pragma pop

/* fn_80012E18 - 0x80012E18 | size: 0x198 */
extern u8* windowGetKeyInfo(void);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma; redundant 16-bit key mask chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
static inline void eventStepCursor(s8* p, s32 keys) {
    s32 k;
    if ((k = keys & 0xFFFF) & 1) {
        (*p)--;
    } else if (k & 2) {
        (*p)++;
    }
}
s32 fn_80012E18(u8* ctx) {
    u16 bits;
    s8 v1;
    s8 v2;
    s8 cur[2];

    bits = *(u16*)(windowGetKeyInfo() + 6);
    v1 = (s8)(s32)windowGetParam(ctx, 2);
    v2 = (s8)menuDataBiosGetType(*(s32*)(ctx + 4));
    /* Retail collapses both bounds to the smaller value. */
    if (v2 < v1) {
        v1 = v2;
    } else {
        v2 = v1;
    }

    /* cur[0], cur[1]: the bytes at 0x94 and 0x95. */
    *(u16*)cur = *(u16*)(ctx + 0x94);
    eventStepCursor(&cur[1], bits);

    if (cur[1] < 0) {
        cur[0] += cur[1];
        cur[1] = 0;
        if (cur[0] < 0) {
            cur[1] = v2 - 1;
            cur[0] = v1 - v2;
        }
    } else if (cur[1] >= v2) {
        cur[0] += cur[1] - (v2 - 1);
        cur[1] = v2 - 1;
        if (cur[0] + cur[1] >= v1) {
            cur[0] = 0;
            cur[1] = 0;
        }
    }

    *(u16*)(ctx + 0x94) = *(u16*)cur;
    *(s32*)(ctx + 0x80) = cur[0] + cur[1];
    return 0;
}
#pragma pop
/* menuPanelCursorDecimalInput - 0x80012FB0 | size: 0x2ec */
extern void windowSetParam(void*, s32, s32);
#if 0
asm void menuPanelCursorDecimalInput(void) {
#include "src/game/gs_event_exec_fn_80012FB0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 menuPanelCursorDecimalInput(u8* ctx) {
    u16 bits;
    u8* state;
    u8* cursor;
    s32 delta;
    s32 mode;
    s32 radix;
    s64 signed_sum;
    s32 pos;
    s32 i;
    s32 limit;
    s32 value;

    cursor = windowGetFreeWork();
    delta = 0;
    value = (s32)windowGetParam(ctx, 0);
    mode = (s32)windowGetParam(ctx, 1);
    if (mode == 2) {
        limit = 8;
        radix = 0x10;
    } else {
        limit = 0xA;
        radix = 0xA;
    }

    state = windowGetKeyInfo();
    bits = *(u16*)(state + 6);
    if (bits & 8) {
        *(s32*)cursor = *(s32*)cursor - 1;
    } else if (bits & 4) {
        *(s32*)cursor = *(s32*)cursor + 1;
    }

    if (*(s32*)cursor < 0) {
        *(s32*)cursor = 0;
        value = 0;
    }
    if (*(s32*)cursor >= limit) {
        *(s32*)cursor = limit - 1;
    }

    if (bits & 1) {
        delta = 1;
        pos = *(s32*)cursor;
        for (i = 0; i < pos; i++) {
            delta *= radix;
        }
    } else if (bits & 2) {
        delta = -1;
        pos = *(s32*)cursor;
        for (i = 0; i < pos; i++) {
            delta *= radix;
        }
    }

    if (mode == 0) {
        signed_sum = (s64)value + (s64)delta;
        if (signed_sum < (s64)(-0x7FFFFFFF - 1)) {
            signed_sum = (s64)(-0x7FFFFFFF - 1);
        }
        if (signed_sum > (s64)0x7FFFFFFF) {
            signed_sum = (s64)0x7FFFFFFF;
        }
    } else {
        signed_sum = (u64)(u32)value;
        signed_sum = signed_sum + (s64)delta;
        if (signed_sum < 0) {
            signed_sum = 0;
        }
        if (signed_sum > (s64)0xFFFFFFFFU) {
            signed_sum = (s64)0xFFFFFFFFU;
        }
    }

    value = signed_sum & 0xFFFFFFFF;
    *(s32*)(ctx + 0x80) = value;
    windowSetParam(ctx, 0, value);
    return 0;
}
#pragma pop
#endif

/* fn_8001329C - 0x8001329C | size: 0x3cc */
extern void msgWinDrawBackground(s32, s32, s32, s32, s32, s32);
extern void msgctrlSetValue(s32, s32);
extern void jumptable_802E4D90();
#if 0
asm void fn_8001329C(void) {
#include "src/game/gs_event_exec_fn_8001329C.inc"
}
#else
/* Message colour: ctx byte 0x8B in the low byte, upper bits set. Built as
 * a 64-bit OR, which is why retail loads -0x100 into a register. */
static inline s32 eventMsgColor(u8* ctx) {
    return (s64)ctx[0x8B] | -0x100;
}

/* Draws the optional prefix label and the count message for one counter. */
static inline void eventDrawCount(u8* ctx, u8* tgt, u8* data, s32 value) {
    s32 width;
    if (data[0] != 0) {
        width = GSmsgGetRect(0x1A0);
        GSmsgPrintColor((*(s16*)(tgt + 0x54) - 0x20) - (s32)(s16)((u32)width >> 16), 0, eventMsgColor(ctx), 0x1A0);
    }
    msgctrlSetValue(0x34, value);
    width = GSmsgGetRect(0xC9);
    GSmsgPrintColor(*(s16*)(tgt + 0x54) - (s32)(s16)((u32)width >> 16), 0, eventMsgColor(ctx), 0xC9);
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8001329C(u8* ctx, u8* tgt) {
    u8* data;

    data = windowGetParam(ctx, 0);

    switch (*(s16*)(tgt + 6)) {
    case 0x92:
        if (*(s32*)(ctx + 4) == 0x107) {
            fn_8001E644(0, 0, *(s16*)(tgt + 0x54), *(s16*)(tgt + 0x56), ctx[0x8B]);
        } else {
            msgWinDrawBackground(0, 0, *(s16*)(tgt + 0x54), *(s16*)(tgt + 0x56), ctx[0x8B], 1);
        }
        break;
    case 0x93:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0x2));
        break;
    case 0x94:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0x4));
        break;
    case 0x95:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0x6));
        break;
    case 0x96:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0x8));
        break;
    case 0x97:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0xA));
        break;
    case 0x98:
        eventDrawCount(ctx, tgt, data, *(s16*)(data + 0xC));
        break;
    }
    return 0;
}
#pragma pop
#endif

/* menuPanelCtrlLvUp - 0x80013668 | size: 0xdc */
extern u8 winSeqIsCheck(s32, s32);
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 menuPanelCtrlLvUp(u8* ctx) {
    s32 flag = 0;
    switch ((s32)(s8)ctx[1]) {
    case 0:
        if ((s32)(s8)ctx[2] == 0) flag = 1;
        if ((u8)winSeqIsCheck(*(s32*)(ctx + 4), 0x2a) == 1) flag = 1;
        if ((u8)flag != 0) {
            winSeqSetMenu(*(s32*)(ctx + 4), 0x26);
            ctx[2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[2] == 0) {
            winSeqSetMenu(*(s32*)(ctx + 4), 0x2a);
            ctx[2] = 1;
        }
        break;
    case 5:
    default:
        break;
    }
    return 0;
}
#pragma pop
#endif /* !GS_EVENT_EXEC_ONLY_8001374C */

#if !defined(GS_EVENT_EXEC_ONLY_80012D20)
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#if defined(GS_EVENT_EXEC_ONLY_8001374C)
extern void msgctrlSetValue(s32, s32);
#endif
/* itemStorageWithdraw - 0x8001374C | size: 0x168 */
extern void* heroItemGetItemKindToItemAryPtr(u32, u8, void*, u16*, s32, s32);
extern void* heroHizukiItemGetItemAryPtr(u32, void*, s32, s32, s32);
extern u8  fn_801429E8(void*);
extern s32 itemBiosGetItemDataId(void*);
extern s32 heroItemAddItemDataId(u32, s32, s32, s32);
extern void fn_8012959C(u32, s32, s32, s16);
extern u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918
extern u32 lbl_8047A2F8;
/* Item data id of the target_n-th usable entry in the list for page entry
 * entry_idx; 0 when there are not that many. */
static inline s32 eventItemListFindDataId(s32 entry_idx, s32 target_n) {
    s32   buf[4];
    u8*   entry;
    s32   flag;
    void* list;
    s32   i;
    s32   idx;
    entry = (u8*)sSummaryPageEntries;
    entry += entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = itemBiosGetItemDataId(list);
                return idx;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma push
#pragma peephole off
#endif
s32 itemStorageWithdraw(s32 entry_idx, s32 target_n, s32* out) {
    s32 idx;
    idx = eventItemListFindDataId(entry_idx, target_n);
    if (heroItemAddItemDataId(lbl_8047A2F8, idx, 1, -1) > 0) {
        winMsgOpen(2, 0x4263, 1, 0);
        winMsgClose(1);
        *out = 0;
        return 0;
    }
    fn_8012959C(lbl_8047A2F8, idx, 1, (s16)target_n);
    msgctrlSetValue(0x2d, (u16)idx);
    winMsgOpen(2, 0x4268, 1, 0);
    winMsgClose(1);
    *out = 0;
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma pop
#endif

/* fn_800138B4 - 0x800138B4 | size: 0x164 */
extern u8 fn_80129718(u32, s32);
extern void fn_80129650(u32, s32, s32, s32);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_8001374C carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma push
#pragma peephole off
#endif
s32 fn_800138B4(s32 entry_idx, s32 target_n, s32* out) {
    s32   buf[4];
    u8*   entry;
    s32   flag;
    s32   idx;
    s32   i;
    void* list;
    entry = (u8*)sSummaryPageEntries;
    entry += entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = itemBiosGetItemDataId(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    if (fn_80129718(lbl_8047A2F8, idx) == 0) {
        winMsgOpen(2, 0x425F, 1, 0);
        winMsgClose(1);
        *out = 0;
        return 2;
    }
    fn_80129650(lbl_8047A2F8, idx, 1, -1);
    msgctrlSetValue(0x2d, (u16)idx);
    winMsgOpen(2, 0x4264, 1, 0);
    winMsgClose(1);
    *out = 1;
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma pop
#endif

/* bagItemToss - 0x80013A18 | size: 0x3e4 */
extern s32 itemBiosGetNum(void*);
extern u32 windowGetActiveID(void);
extern s32 menuOpenCustom(s32, u32, void*, s32, s32, s32, ...);
extern void menuClose(s32);
extern void menuCloseSync(s32, s32);
extern u32 lbl_80266BD8[];
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2FC;
#if 0
asm void fn_80013A18(void) {
#include "src/game/gs_event_exec_fn_80013A18.inc"
}
#else
typedef struct EventMenuArg {
    u8 bytes[3];
    u8 pad;
    s32 menuId;
    s32 quantity;
    s32 one;
    s32 zero;
} EventMenuArg;

typedef struct EventConfirmArg {
    u8 bytes[3];
    u8 pad;
    s32* values;
    s32 mode;
} EventConfirmArg;

/* Same scan as eventItemListFindDataId over the list a page entry's kind
 * selects; returns the held count / the item data id of the target_n-th
 * usable entry, or 0. */
static inline s32 eventItemListGetNum(s32* kind, s32 target_n) {
    u16   count;
    void* list;
    s32   i;
    s32   idx;
    if (*kind >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)*kind, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < count) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                return itemBiosGetNum(list);
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    return 0;
}

static inline s32 eventItemListGetDataId(s32* kind, s32 target_n) {
    u16   count;
    void* list;
    s32   i;
    s32   idx;
    if (*kind >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)*kind, &count, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < count) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = itemBiosGetItemDataId(list);
                return idx;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    return 0;
}

/* Digits of the quantity picker: 3 when the pocket holds more than 100. */
/* RULE-EXCEPTION(user-approved): single-use inline helper (bagItemToss) - see docs/RULE_EXCEPTIONS.md */
static inline s32 eventMaxChoice(s32* kind) {
    u16 max_count;

    heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)*kind, 0, &max_count, 0, 0);
    if ((s32)max_count > 0x64) {
        return 3;
    }
    return 2;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (bagItemToss) - see docs/RULE_EXCEPTIONS.md */
static inline s32 eventGetDataId(s32* kind, s32 target_n) {
    return eventItemListGetDataId(kind, target_n);
}

/* Opens a menu without a config and returns its choice, or -1 when it is
 * cancelled or out of [0, count). */
/* RULE-EXCEPTION(user-approved): single-use inline helper (bagItemToss) - see docs/RULE_EXCEPTIONS.md */
static inline s32 eventMenuSelect(s32 menuId, void* arg, s32 count) {
    s32 ret;
    u32 parent;

    /* RULE-EXCEPTION(user-approved): scalar carrier - the parent window id
     * is held in a named local, chosen for register allocation - see
     * docs/RULE_EXCEPTIONS.md */
    parent = windowGetActiveID();
    ret = menuOpenCustom(menuId, parent, 0, 0, 1, 1, arg);
    menuClose(menuId);
    menuCloseSync(menuId, 1);
    if (ret == -1) {
        return -1;
    }
    if (ret < 0 || ret >= count) {
        return -1;
    }
    return ret;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_8001374C carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma push
#pragma peephole off
#endif
s32 bagItemToss(s32 entry_idx, s32 target_n, s32* out) {
    /* RULE-EXCEPTION(user-approved): declaration order, the named row
     * offset and the function-scope confirm-colour pointer chosen for
     * register allocation - see docs/RULE_EXCEPTIONS.md */
    u8* entry2;
    s32 result;
    s32* kind;
    s32 values[6];
    s32 quantity;
    s32 menu_id;
    s32 menu_config;
    EventConfirmArg confirm_arg;
    s32 offset;
    s32 choice;
    s32 menuResult;
    s32 idx;
    EventMenuArg menu_arg;
    s32 max_choice;

    values[0] = lbl_80266BD8[0];
    values[1] = lbl_80266BD8[1];
    values[2] = lbl_80266BD8[2];
    values[3] = lbl_80266BD8[3];
    values[4] = lbl_80266BD8[4];
    values[5] = lbl_80266BD8[5];
    lbl_8047A2DC = 0x2B1C;
    /* RULE-EXCEPTION(user-approved): page row walked to its kind field with a
     * pointer pre-increment (one register for the row and &kind, as retail)
     * - see docs/RULE_EXCEPTIONS.md */
    offset = entry_idx * 0x4C;
    kind = (s32*)(sSummaryPageEntries + offset);

    max_choice = eventMaxChoice(++kind);
    quantity = (u16)eventItemListGetNum(kind, target_n);
    if (quantity < 1) {
        choice = 0;
    } else {
        if (max_choice == 2) {
            menu_config = 1;
            menu_id = 0x5B;
        } else {
            menu_config = 2;
            menu_id = 0x5C;
        }
        {
            u8* entry = sSummaryPageEntries;
            entry += offset;
            menu_arg.bytes[0] = entry[0];
            menu_arg.bytes[1] = entry[1];
            menu_arg.bytes[2] = entry[2];
        }
        menu_arg.menuId = menu_id;
        menu_arg.one = 1;
        menu_arg.quantity = quantity;
        menu_arg.zero = 0;
        lbl_8047A2FC = 1;
        menuResult = menuOpenCustom(menu_id, windowGetActiveID(), &menu_config, 0, 1, 1, &menu_arg);
        menuClose(menu_id);
        menuCloseSync(menu_id, 1);
        if (menuResult == -1) {
            choice = -1;
        } else {
            choice = lbl_8047A2FC;
        }
    }
    if (choice < 0) {
        *out = 0;
        return 0;
    }

    idx = eventGetDataId(kind, target_n) & 0xFFFF;
    msgctrlSetValue(0x2D, idx);
    msgctrlSetValue(0x2F, choice);

    lbl_8047A2DC = 0x2B1D;
    entry2 = sSummaryPageEntries;
    entry2 += offset;
    confirm_arg.bytes[0] = entry2[0];
    confirm_arg.bytes[1] = entry2[1];
    confirm_arg.bytes[2] = entry2[2];
    confirm_arg.values = values;
    confirm_arg.mode = 2;

    result = eventMenuSelect(0x5A, &confirm_arg, 2);

    if (result == 1 || result == -1) {
        *out = 0;
        return 0;
    }

    msgctrlSetValue(0x2D, idx);
    msgctrlSetValue(0x2F, choice);
    winMsgOpen(2, 0x4269, 1, 0);
    winMsgClose(1);
    *out = choice;
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma pop
#endif
#endif

/* bagItemGive - 0x80013DFC | size: 0x184 */
extern void itemDataBiosGetPtr(u32);
extern u8   itemDataBiosGetHidenMachineNo(void);
extern u8   itemDataBiosGetKind(void);
extern s32  menuPokemonOpenItemGive(u8, u8, s32, s32);
extern void menuPokemonClose(void);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_8001374C carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma push
#pragma peephole off
#endif
s32 bagItemGive(s32 entry_idx, s32 target_n, s32* out) {
    s32   buf[4];
    u8*   entry;
    s32   flag;
    s32   idx;
    s32   i;
    void* list;
    s32   x;
    entry = (u8*)sSummaryPageEntries;
    entry += entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = itemBiosGetItemDataId(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    itemDataBiosGetPtr(idx);
    if (itemDataBiosGetHidenMachineNo() != 0xFF) {
        msgctrlSetValue(0x2d, (u16)idx);
        winMsgOpen(2, 0x4262, 1, 0);
        winMsgClose(1);
        *out = 0;
        return 0;
    }
    itemDataBiosGetPtr((u16)idx);
    x = itemDataBiosGetKind();
    menuCloseCustom(0x59, 0, 1);
    lbl_8047A2EC = menuPokemonOpenItemGive((u8)x, (u8)target_n, idx, 0);
    menuPokemonClose();
    if ((s32)lbl_8047A2EC < 0) {
        return 3;
    }
    *out = 0;
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma pop
#endif

/* bagItemUse - 0x80013F80 | size: 0x17c */
extern u32 lbl_8047A2E0;
typedef s32 (*MusicFp)(u16, s32*);
extern MusicFp itemDataBiosGetFieldUseFunc(void);
extern MusicFp itemDataBiosGetBattleUseFunc(void);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_8001374C carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma push
#pragma peephole off
#endif
s32 bagItemUse(s32 entry_idx, s32 target_n, s32* out) {
    s32   buf[4];
    u8*   entry;
    s32   flag;
    s32   idx;
    s32   i;
    void* list;
    MusicFp fp;
    s32   r;
    s32   retval;
    u16   id;
    entry = (u8*)sSummaryPageEntries;
    entry += entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = itemBiosGetItemDataId(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    id = (u16)idx;
    itemDataBiosGetPtr(id);
    if ((s32)lbl_8047A2E0 == 0) {
        fp = itemDataBiosGetFieldUseFunc();
    } else {
        fp = itemDataBiosGetBattleUseFunc();
    }
    if (fp == 0) {
        winMsgOpen(2, 0x4261, 1, 0);
        winMsgClose(1);
        *out = 0;
        return 3;
    }
    r = fp(id, out);
    switch (r) {
    case 0: retval = 0; break;
    case 1: retval = 3; break;
    case 2: retval = 4; break;
    }
    return retval;
}
#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#pragma pop
#endif
#endif /* !GS_EVENT_EXEC_ONLY_800141BC */

#if !defined(GS_EVENT_EXEC_ONLY_8001374C)
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
/* fn_800140FC - 0x800140FC | size: 0x14 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2F4;
void fn_800140FC(u32* out1, u32* out2) {
    *out1 = lbl_8047A2F8;
    *out2 = lbl_8047A2F4;
}

/* itemUseGetTargetPokemon - 0x80014118 | size: 0x80 */
extern s32 fightTrainer_GetFightPokemonPtr(u32, u16);
extern s32 fightPokemonGetPokemonPtr(void);
extern s32 heroGetStatus(u32, s32, u16);
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F4;
extern u32 lbl_8047A2F8;
void itemUseGetTargetPokemon(s32 arg, s32* out1, s32* out2) {
    s32 val;
    s32 other;
    if ((s32)lbl_8047A2E0 == 1) {
        other = fightTrainer_GetFightPokemonPtr(lbl_8047A2F4, (u16)arg);
        val   = fightPokemonGetPokemonPtr();
    } else {
        val   = heroGetStatus(lbl_8047A2F8, 3, (u16)arg);
        other = 0;
    }
    *out1 = val;
    *out2 = other;
}

/* itemUseClosePartyMenu - 0x80014198 | size: 0x24 */
extern u32 lbl_8047A2EC;
void itemUseClosePartyMenu(u32 val) {
    lbl_8047A2EC = val;
    menuPokemonClose();
}
#else
/* itemUseOpenPartyMenu..fn_80014550 also build alone for the linked
 * gs_event_exec_r47_prefix unit, whose -opt nopeephole stands in for the
 * file's per-function `peephole off` push/pop pairs. */
extern void msgctrlSetValue(s32, s32);
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F4;
#endif /* !GS_EVENT_EXEC_ONLY_800141BC */

/* itemUseOpenPartyMenu - 0x800141BC | size: 0x78 */
extern s32 menuPokemonOpenItemUse(s32 mode, u8 a, u16 b, u32 p);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_800141BC carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma push
#pragma peephole off
#endif
s32 itemUseOpenPartyMenu(s32 arg0, s32 arg1) {
    s32 mode;
    s32 result;
    menuCloseCustom(0x59, 0, 1);
    if ((s32)lbl_8047A2E0 == 1) {
        mode = 4;
    } else {
        mode = 3;
    }
    result = menuPokemonOpenItemUse(mode, (u8)arg1, arg0, lbl_8047A2F4);
    if (arg1 == 0) {
        result = -1;
    }
    return result;
}
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma pop
#endif

/* fn_80014234 - 0x80014234 | size: 0xe8 */
/* The four-entry table at 0x80266B58 (src/game/data/rodata_802666B0.c). */
typedef struct {
    s32 key;
    u8* inner;
    s32 count;
} EvTbl;

extern const EvTbl lbl_80266B58[];

typedef struct {
    s32 _0;
    s32 species_a;
    s32 species_b;
} EvEntry;

/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_800141BC carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma push
#pragma peephole off
#endif
s32 fn_80014234(u8* ctx, u8* tgt) {
    u8* p;
    s32 idx;
    s32 slot;
    s32 species;
    EvEntry* e;

    p    = *(u8**)(ctx + 0x60);
    for (idx = 0; idx < 4; idx++) {
        if (lbl_80266B58[idx].key == *(s32*)(p + 4)) break;
    }
    if (idx >= 4) return 0;
    slot = (s8)ctx[0x95];
    if (slot < 0 || slot >= lbl_80266B58[idx].count) return 0;
    e = (EvEntry*)lbl_80266B58[idx].inner;
    e += slot;
    species = *(s16*)(tgt + 0x6);
    if (e->species_a == species || e->species_b == species) {
        tgt[0x67] = 0xFF;
    } else {
        tgt[0x67] = 0;
    }
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma pop
#endif
/* fn_8001431C - 0x8001431C | size: 0x7c */
extern u32 lbl_8047A2FC;
#if 0
asm void fn_8001431C(void) {
#include "src/game/gs_event_exec_fn_8001431C.inc"
}
#else
s32 fn_8001431C(void* arg0, u8* arg1) {
    void* p = *(void**)((u8*)arg0 + 0x60);
    s32 count = *(s32*)((u8*)p + 0x10);
    s32 rand;
    if (count <= 0) return 0;
    msgctrlSetValue(0x50, count * (s32)lbl_8047A2FC);
    rand = GSmsgGetRect(0x151);
    GSmsgPrintColor(*(s16*)(arg1 + 0x54) - (s32)(s16)((u32)rand >> 16), 0, -1, 0x151);
    return 0;
}
#endif

/* fn_80014398 - 0x80014398 | size: 0x1b8 */
extern void GSmsgPrintRight(s32, s32, s16, s16, s32, s32);
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the GS_EVENT_EXEC_ONLY_800141BC carve skips it and uses the unit's -opt nopeephole) - see docs/RULE_EXCEPTIONS.md */
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma push
#pragma peephole off
#endif
s32 fn_80014398(u8* ctx, u8* arg1) {
    u8* data;
    s32 idx;
    u8* entry;
    s32 j;
    s32 i;
    s32 power;
    s32 digit;

    data = *(u8**)(ctx + 0x60);
    for (idx = 0; idx < 4; idx++) {
        if (lbl_80266B58[idx].key == *(s32*)(data + 4)) break;
    }
    if (idx >= 4) return 0;
    entry = lbl_80266B58[idx].inner;
    j = 0;
    while (j < lbl_80266B58[idx].count) {
        if (*(s16*)(arg1 + 6) == *(s32*)entry) break;
        j++;
        entry += 0xc;
    }
    if (j >= lbl_80266B58[idx].count) return 0;
    power = 1;
    for (i = 0; i < lbl_80266B58[idx].count - j - 1; i++) {
        power *= 10;
    }
    digit = (s32)lbl_8047A2FC / power;
    digit %= 10;
    msgctrlSetValue(0x34, digit);
    GSmsgPrintRight(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), -1, 0xC9);
    return 0;
}
#if !defined(GS_EVENT_EXEC_ONLY_800141BC)
#pragma pop
#endif
/* fn_80014550 - 0x80014550 | size: 0x24 */
s32 fn_80014550(u8* src_struct, u8* dst) {
    u8* src = *(u8**)(src_struct + 0x60);
    dst[0x64] = src[0];
    dst[0x65] = src[1];
    dst[0x66] = src[2];
    return 0;
}

#endif /* !GS_EVENT_EXEC_ONLY_8001374C */
#endif /* !GS_EVENT_EXEC_ONLY_80012D20 */

#endif /* !GS_EVENT_EXEC_ONLY_80014A48 && !GS_EVENT_EXEC_ONLY_80014574 */

/* fn_80014574 also builds alone for the gs_event_exec_r47_80014574_o3 unit. */
#ifdef GS_EVENT_EXEC_ONLY_80014574
extern u8* windowGetKeyInfo(void);

typedef struct {
    s32 key;
    u8* inner;
    s32 count;
} EvTbl;

extern const EvTbl lbl_80266B58[];
#endif

#if !defined(GS_EVENT_EXEC_ONLY_80014A48) && !defined(GS_EVENT_EXEC_ONLY_80012D20) && !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)
/* fn_80014574 - 0x80014574 | size: 0x4d4 */
extern void soundStart(s32, s32, s32, s32);
extern u32 lbl_8047A2FC;
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014574(u8* ctx) {
    u8* p;
    u8* keys;
    EvTbl* tbl;
    s32 idx;
    s32 count;
    s32 place;
    s32 i;
    s32 digit;
    s32 lower;
    s32 max_digit;
    s32 value;

    p = *(u8**)(ctx + 0x60);
    keys = windowGetKeyInfo();
    if (*(u16*)(keys + 6) & 0xF) {
        for (idx = 0; idx < 4; idx++) {
            if (*(s32*)(p + 4) == lbl_80266B58[idx].key) break;
        }
        if (idx >= 4) {
            return 0;
        }

        tbl = (EvTbl*)lbl_80266B58;
        count = tbl[idx].count;
        place = 1;
        for (i = 0; i < count - (s8)ctx[0x95] - 1; i++) {
            place *= 10;
        }

        if (*(u16*)(keys + 6) & 1) {
            if (*(s32*)(p + 0xC) != *(s32*)(p + 8)) {
                if (place < 10) {
                    value = (s32)lbl_8047A2FC + place;
                    lbl_8047A2FC = value;
                    if (value > *(s32*)(p + 8)) {
                        lbl_8047A2FC = *(s32*)(p + 0xC);
                    }
                    soundStart(0x23, 0, 0xFF, 0);
                } else {
                    digit = ((s32)lbl_8047A2FC / place) % 10;
                    lower = (s32)lbl_8047A2FC - digit * place;
                    for (max_digit = 9; max_digit >= 0; max_digit--) {
                        if (lower + place * max_digit <= *(s32*)(p + 8)) break;
                    }
                    digit++;
                    if (digit > max_digit) {
                        digit = 0;
                    }
                    if ((s32)(lbl_8047A2FC = lower + digit * place) < *(s32*)(p + 0xC)) {
                        lbl_8047A2FC = *(s32*)(p + 0xC);
                    } else {
                        soundStart(0x23, 0, 0xFF, 0);
                    }
                }
            }
        }

        if (*(u16*)(keys + 6) & 2) {
            if (*(s32*)(p + 0xC) != *(s32*)(p + 8)) {
                if (place < 10) {
                    value = (s32)lbl_8047A2FC - place;
                    lbl_8047A2FC = value;
                    if (value < *(s32*)(p + 0xC)) {
                        lbl_8047A2FC = *(s32*)(p + 8);
                    }
                    soundStart(0x23, 0, 0xFF, 0);
                } else {
                    digit = ((s32)lbl_8047A2FC / place) % 10;
                    lower = (s32)lbl_8047A2FC - digit * place;
                    for (max_digit = 9; max_digit >= 0; max_digit--) {
                        if (lower + place * max_digit <= *(s32*)(p + 8)) break;
                    }
                    digit--;
                    if (digit < 0) {
                        digit = max_digit;
                    }
                    if ((s32)(lbl_8047A2FC = lower + digit * place) < *(s32*)(p + 0xC)) {
                        lbl_8047A2FC = *(s32*)(p + 0xC);
                    } else {
                        soundStart(0x23, 0, 0xFF, 0);
                    }
                }
            }
        }

        if (*(u16*)(keys + 6) & 8) {
            if (++((s8*)ctx)[0x95] >= count) {
                ((s8*)ctx)[0x95] = count - 1;
            }
        }

        if (*(u16*)(keys + 6) & 4) {
            if (--((s8*)ctx)[0x95] < 0) {
                ((s8*)ctx)[0x95] = 0;
            }
        }
    }
    return 0;
}
#pragma pop

#endif /* !GS_EVENT_EXEC_ONLY_80014A48 */

#if !defined(GS_EVENT_EXEC_ONLY_80014574) && !defined(GS_EVENT_EXEC_ONLY_80012D20) && !defined(GS_EVENT_EXEC_ONLY_8001374C) && !defined(GS_EVENT_EXEC_ONLY_800141BC)

/* menuPocket2NumCtrl..fn_80014F48 also build alone for the linked
 * gs_event_exec_r47_suffix unit. */
#ifdef GS_EVENT_EXEC_ONLY_80014A48
extern void GSmsgPrintColor(s32, s32, s32, s32);
extern void winSeqSetMenu(s32, s32);
extern u8* windowGetKeyInfo(void);
#endif
/* menuPocket2NumCtrl - 0x80014A48 | size: 0x9c */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 menuPocket2NumCtrl(u8* ctx) {
    u8* p = *(u8**)(ctx + 0x60);
    switch ((s32)(s8)ctx[0x1]) {
    case 0:
        if ((s32)(s8)ctx[0x2] == 0) {
            winSeqSetMenu(*(s32*)(p + 0x4), 0x6e);
            ctx[0x2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[0x2] == 0) {
            winSeqSetMenu(*(s32*)(p + 0x4), 0x72);
            ctx[0x2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop

/* fn_80014AE4 - 0x80014AE4 | size: 0xbc */
extern s32 lbl_80266BC4[];
extern f32 lbl_8047B744;
extern f32 lbl_8047A2C0;
extern f32 lbl_8047B740;
#if 0
asm void fn_80014AE4(void) {
#include "src/game/gs_event_exec_fn_80014AE4.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; scheduling on; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling on
#pragma peephole off
s32 fn_80014AE4(u8* ctx, u8* tgt) {
    s32 tbl[5];
    s32 idx;
    void* p = *(void**)(ctx + 0x60);
    s32 base = *(s32*)((u8*)p + 0x8);
    tbl[0] = lbl_80266BC4[0];
    tbl[1] = lbl_80266BC4[1];
    tbl[2] = lbl_80266BC4[2];
    tbl[3] = lbl_80266BC4[3];
    tbl[4] = lbl_80266BC4[4];
    idx = (s32)(s8)ctx[0x95] + (5 - base);
    if (idx < 0 || idx >= 5) return 0;
    if (*(s16*)(tgt + 0x6) != tbl[idx]) {
        tgt[0x67] = 0;
        return 0;
    }
    {
        f32 f = lbl_8047B740 * (lbl_8047B744 - lbl_8047A2C0);
        tgt[0x67] = f;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80014BA0 - 0x80014BA0 | size: 0x98 */
extern s32 lbl_80266BB0[];
#if 0
asm void fn_80014BA0(void) {
#include "src/game/gs_event_exec_fn_80014BA0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014BA0(u8* ctx, u8* tgt) {
    s32 tbl[5];
    s32 idx;
    u8 value;
    u8* p;

    p = *(u8**)(ctx + 0x60);
    tbl[0] = lbl_80266BB0[0];
    tbl[1] = lbl_80266BB0[1];
    tbl[2] = lbl_80266BB0[2];
    tbl[3] = lbl_80266BB0[3];
    tbl[4] = lbl_80266BB0[4];
    idx = (s32)(s8)ctx[0x95] + (5 - *(s32*)(p + 8));
    if (idx < 0 || idx >= 5) {
        return 0;
    }
    if (*(s16*)(tgt + 6) == tbl[idx]) {
        value = 0xFF;
    } else {
        value = 0;
    }
    tgt[0x67] = value;
    return 0;
}
#pragma pop
#endif

/* fn_80014C38 - 0x80014C38 | size: 0xe4 */
extern s32 lbl_80266B9C[];
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014C38(u8* ctx, u8* tgt) {
    u8* p;
    s32 idx;
    s32 base;
    u8* inner;
    s32 flag;
    s32 keys[5];
    p = *(u8**)(ctx + 0x60);
    idx = 0;
    keys[0] = lbl_80266B9C[0];
    keys[1] = lbl_80266B9C[1];
    keys[2] = lbl_80266B9C[2];
    keys[3] = lbl_80266B9C[3];
    keys[4] = lbl_80266B9C[4];
    for (; idx < 5; idx++) {
        if (*(s16*)(tgt + 0x6) == keys[idx]) break;
    }
    base = *(s32*)(p + 0x8);
    idx = idx - (5 - base);
    if (idx < 0 || idx >= base) return 0;
    inner = *(u8**)(p + 0x4) + idx * 0xC;
    flag  = (*(u16*)(inner + 0x8)) & 1;
    GSmsgPrintColor(0, 0, flag ? 0x7f7f7fff : -1, *(s32*)inner);
    return 0;
}
#pragma pop
/* fn_80014D1C - 0x80014D1C | size: 0x134 */
typedef struct { s32 key; s16 a; s16 b; s32 flag; } PurEntry;
extern PurEntry lbl_802E4DB0[];
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014D1C(u8* ctx, u8* tgt) {
    u8* p;
    s32 idx;
    s32 count;
    s32 delta;
    u8* walk;

    p = *(u8**)(ctx + 0x60);
    if (*(s16*)(tgt + 6) != 0x223) {
        tgt[0x64] = p[0];
        tgt[0x65] = p[1];
        tgt[0x66] = p[2];
    }
    count = *(s32*)(p + 8);
    if (count < 5) {
        walk = (u8*)lbl_802E4DB0;
        for (idx = 0; idx < 8; idx++) {
            if (*(s16*)(tgt + 6) == *(s32*)walk) break;
            walk += 0xc;
        }
        if (idx < 8) {
            delta = 5 - count;
            *(s16*)(tgt + 0x52) = (s16)(delta * 0x1F + lbl_802E4DB0[idx].a);
            if (lbl_802E4DB0[idx].flag != 0) {
                *(s16*)(tgt + 0x56) = (s16)(lbl_802E4DB0[idx].b - delta * 0x1F);
            }
        }
    }
    return 0;
}
#pragma pop

/* fn_80014E50 - 0x80014E50 | size: 0xf8 */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014E50(u8* ctx) {
    u8* state;
    u8* p;
    s32 slot;
    s32 count;
    s32 new_slot;
    u8* inner;

    p     = *(u8**)(ctx + 0x60);
    state = windowGetKeyInfo();
    count = *(s32*)(p + 8);

    if ((*(volatile u16*)(state + 6) & 2) != 0) {
        slot = *(s8*)(ctx + 0x95);
        new_slot = slot + 1;
        if (new_slot >= count) {
            new_slot = count - 1;
        }
        inner = *(u8**)(p + 4);
        if ((*(u16*)(inner + new_slot * 0xC + 8) & 1) != 0) {
            new_slot++;
            if (new_slot >= count) {
                new_slot = slot;
            }
        }
        *(s8*)(ctx + 0x95) = new_slot;
    }
    if ((*(volatile u16*)(state + 6) & 1) != 0) {
        slot = *(s8*)(ctx + 0x95);
        new_slot = slot - 1;
        if (new_slot < 0) {
            new_slot = 0;
        }
        inner = *(u8**)(p + 4);
        if ((*(u16*)(inner + new_slot * 0xC + 8) & 1) != 0) {
            new_slot--;
            if (new_slot < 0) {
                new_slot = slot;
            }
        }
        *(s8*)(ctx + 0x95) = new_slot;
    }
    return 0;
}
#pragma pop

/* fn_80014F48 - 0x80014F48 | size: 0xd4 */
extern f32 lbl_8047B748;
extern f32 lbl_8047B74C;
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80014F48(u8* ctx) {
    f32 tmp;
    switch ((s32)(s8)ctx[1]) {
    case 0:
        if ((s32)(s8)ctx[2] == 0) {
            winSeqSetMenu(0x5a, 0x66);
            lbl_8047A2C0 = lbl_8047B748;
            ctx[2] = 1;
        }
        break;
    case 2:
        tmp = lbl_8047A2C0 + lbl_8047B74C;
        lbl_8047A2C0 = tmp;
        if (tmp > lbl_8047B744) {
            lbl_8047A2C0 = lbl_8047B748;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[2] == 0) {
            winSeqSetMenu(0x5a, 0x6a);
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop

#endif /* !GS_EVENT_EXEC_ONLY_80014574 */
