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
 *   fn_8001374C  GSevent_WaitFrames           -- 0x168 bytes, wait N frames
 *   fn_800138B4  GSevent_FadeScreen           -- 0x164 bytes, screen fade in/out
 *   fn_80013A18  GSevent_CameraControl        -- 0x3E4 bytes, camera movement script
 *   fn_80013DFC  GSevent_PlaySound            -- 0x184 bytes, play sound effect
 *   fn_80013F80  GSevent_PlayMusic            -- 0x17C bytes, change BGM
 *   fn_800140FC  GSevent_StopMusic            -- 0x14 bytes, stop current BGM
 *   fn_80014110  GSevent_ReturnTrue           -- 8 bytes, stub returns 1
 *   fn_80014118  GSevent_CheckPartyState      -- 0x80 bytes, check party condition
 *   fn_80014198  GSevent_GetPartySize         -- 0x24 bytes, return party count
 *   fn_800141BC  GSevent_GiveItem             -- 0x78 bytes, give item to player
 *   fn_80014234  GSevent_GivePokemon          -- 0xE8 bytes, give Pokemon to player
 *   fn_8001431C  GSevent_CheckBag             -- 0x7C bytes, check bag for item
 *   fn_80014398  GSevent_BranchOnFlag         -- 0x1B8 bytes, conditional branch
 *   fn_80014550  GSevent_UnlockLocation       -- 0x24 bytes, unlock map location
 *   fn_80014574  GSevent_CutsceneSequence     -- 0x4D4 bytes, full cutscene playback
 *   fn_80014A48  GSevent_SetWeather           -- 0x9C bytes, change weather effect
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
extern void fn_80106D3C(s32 slot, s32 msgId, s32 p3, s32 p4);
extern void fn_801069FC(s32 slot);

/* Sound system */
extern void fn_80166AB8(s32 soundId, s32 p2, s32 p3);   /* Play SE */
extern void fn_801669E4(s32 soundId, s32 p2, s32 p3);   /* Play BGM */

/* Scene/camera */
extern void fn_80113828(s32 cameraId, s32 mode);
extern u8   fn_800FF548(void);                     /* Scene transition check */
extern void fn_800FF56C(s32 floorId);              /* Floor load */

/* Frame control */
extern void _threadSwitch(void);                     /* Frame advance */
extern void fn_800F05A0(void* threadCtx);          /* Resume thread */
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

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

extern u32 lbl_8047A2EC;

/* Address: 0x80013744 | Size: 0x8 | Pattern: return_constant */
u32 fn_80013744(void) { return 1; }

/* Address: 0x80014110 | Size: 0x8 | Pattern: sda_getter */
u32 fn_80014110(void) {
    return lbl_8047A2EC;
}

/* ===== Phase 2 recovery stubs ===== */

/* fn_800129A8 - 0x800129A8 | size: 0x1ec */
extern u8*  fn_801040A0(void);
extern void* fn_80103FFC(void*, s32);
extern void* fn_80103FE4(void*);
extern u8   fn_80104704(s32);
extern void fn_80103F74(s32, s32, s32);
extern void fn_801669BC(s32);
extern void* memcpy(void* dst, const void* src, u32 n);
#pragma push
#pragma peephole off
s32 fn_800129A8(u8* ctx) {
    u8* p;
    s32 id;
    s32 kind;
    void* buf;
    p = fn_801040A0();
    if ((s32)(s8)ctx[1] == 0) {
        buf = fn_80103FFC(ctx, 0x30);
        if (buf != 0) memcpy(buf, *(void**)(ctx + 0x60), 0x30);
    }
    fn_80103FE4(ctx);
    if ((s32)(s8)ctx[1] == 0) {
        id = *(s32*)(ctx + 4);
        kind = 0;
        if (fn_80104704(id) != 0) {
            if (id == 0x49) kind = 0x538;
            else if (id >= 0x49) { if (id < 0x4b) kind = 0x540; }
            else if (id >= 0x47) kind = 0x540;
            else if (id >= 0x45) kind = 0x538;
            fn_80103F74(id, kind, 0);
        }
        id = *(s32*)(ctx + 4);
        kind = 0;
        if (fn_80104704(id) != 0) {
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
            fn_801669BC(0x4d0);
        }
    }
    *(u16*)(p + 6) += 1;
    *(u16*)(p + 6) = (u16)(*(u16*)(p + 6) % 1200);
    return 0;
}
#pragma pop

/* fn_80012B94 - 0x80012B94 | size: 0x18c */
extern void* fn_801040D0(void*, s32);
extern s32   fn_800FA444(s32);
extern void  fn_800FB680(s32, s32, s32, s32);
extern void fn_8001E644(s32, s32, s32, s32, u8);
extern void fn_8001EA98(s32, s32, s32, s32);
extern void fn_801040F0(s32, s16, void*, s32, s32);
#pragma push
#pragma peephole off
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
    cmd   = (u8)(s32)fn_801040D0(ctx, 0);
    arr   = (u8*)fn_80103FE4(ctx);
    count = (s32)(s8)(s32)fn_801040D0(ctx, 2);
    cap   = (s32)(u8)fn_8005D9E4(*(s32*)(ctx + 4));
    if (count > cap) count = cap;

    max_hi = 0;
    sum_lo = 0;
    iter = arr;
    i = 0;
    while (i < count) {
        rng = fn_800FA444(*(s32*)iter);
        if (max_hi < (s32)((u32)rng >> 16)) {
            max_hi = (s32)((u32)rng >> 16);
        }
        sum_lo += (s32)(rng & 0xFFFF);
        iter += 4;
        i++;
        sum_lo += 2;
    }

    if ((s32)cmd == 0x7f) {
        fn_8001EA98(0, 0, max_hi + 0x20, sum_lo);
    } else if ((s32)cmd < 2 && (s32)cmd >= 0) {
        fn_8001E644(0, 0, max_hi + 0x20, sum_lo, ctx[0x8B]);
    }

    iter = arr;
    acc = 1;
    i = 0;
    while (i < count) {
        val = *(s32*)iter;
        if (val != 0) {
            rng = fn_800FA444(val);
            fn_800FB680(0x20, acc, -1, val);
            delay = (s32)(rng & 0xFFFF) + 2;
        } else {
            delay = 0x14;
        }
        if ((s32)(s8)ctx[0x95] == i) {
            fn_801040F0(0x20, (s16)acc, ctx, 0x157, 0);
        }
        acc += delay;
        iter += 4;
        i++;
    }
    return 0;
}
#pragma pop

/* fn_80012D20 - 0x80012D20 | size: 0xf8 */
extern void fn_801080CC(s32, s32);
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
            src  = fn_801040D0(arg, 1);
            size = (s32)fn_801040D0(ctx, 2) << 2;
            buf  = fn_80103FFC(ctx, size);
            if (buf != 0) {
                memcpy(buf, src, size);
            }
            fn_801080CC(*(s32*)(ctx + 4), 0x26);
            ctx[2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[2] == 0) {
            fn_801080CC(*(s32*)(ctx + 4), 0x2a);
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
extern u8* fn_80105624(void);
#pragma push
#pragma peephole off
s32 fn_80012E18(u8* ctx) {
    u8* state;
    u32 bits;
    s32 v1;
    s32 v2;
    s32 maxv, minv;
    u8  hi, lo;
    u16 pair;
    u8  saved_hi, saved_lo;
    state = fn_80105624();
    bits = *(u16*)(state + 6);
    v1 = (s32)(s8)(s32)fn_801040D0(ctx, 2);
    v2 = (s32)(s8)(s32)fn_8005D9E4(*(s32*)(ctx + 4));
    if (v1 < v2) {
        maxv = v2;
        minv = v1;
    } else {
        maxv = v1;
        minv = v2;
    }
    pair = *(u16*)(ctx + 0x94);
    hi = (u8)(pair >> 8);
    lo = (u8)pair;
    if ((bits & 1) != 0) {
        lo = (u8)(lo - 1);
    } else if ((bits & 2) != 0) {
        lo = (u8)(lo + 1);
    }
    if ((s8)lo < 0) {
        saved_hi = hi;
        saved_lo = lo;
        lo = 0;
        hi = (u8)(saved_hi + (s8)saved_lo);
        if ((s8)hi < 0) {
            lo = (u8)(minv - 1);
            hi = (u8)(maxv - minv);
        }
    } else {
        if ((s32)(s8)lo >= minv) {
            saved_lo = lo;
            saved_hi = hi;
            lo = (u8)(minv - 1);
            hi = (u8)(saved_hi + (saved_lo - (minv - 1)));
            if ((s32)(s8)hi + (s32)(s8)lo >= maxv) {
                hi = 0;
                lo = 0;
            }
        }
    }
    *(u16*)(ctx + 0x94) = (u16)((u16)hi << 8 | lo);
    *(s32*)(ctx + 0x80) = (s32)(s8)hi + (s32)(s8)lo;
    return 0;
}
#pragma pop

/* fn_80012FB0 - 0x80012FB0 | size: 0x2ec */
extern void fn_801040B8(void*, s32, s32);
#if 0
asm void fn_80012FB0(void) {
#include "src/game/gs_event_exec_fn_80012FB0.inc"
}
#else
s32 fn_80012FB0(u8* ctx) {
    u8* state;
    u8* cursor;
    s32 value;
    s32 mode;
    s32 limit;
    s32 radix;
    u16 bits;
    s32 delta;
    s32 pos;
    s32 i;
    s64 signed_sum;

    cursor = fn_801040A0();
    value = (s32)fn_801040D0(ctx, 0);
    mode = (s32)fn_801040D0(ctx, 1);
    if (mode == 2) {
        limit = 8;
        radix = 0x10;
    } else {
        limit = 0xA;
        radix = 0xA;
    }

    state = fn_80105624();
    bits = *(u16*)(state + 6);
    if ((bits & 8) != 0) {
        *(s32*)cursor = *(s32*)cursor - 1;
    } else if ((bits & 4) != 0) {
        *(s32*)cursor = *(s32*)cursor + 1;
    }

    if (*(s32*)cursor < 0) {
        *(s32*)cursor = 0;
        value = 0;
    }
    if (*(s32*)cursor >= limit) {
        *(s32*)cursor = limit - 1;
    }

    delta = 0;
    if ((bits & 1) != 0) {
        delta = 1;
        pos = *(s32*)cursor;
        for (i = 0; i < pos; i++) {
            delta *= radix;
        }
    } else if ((bits & 2) != 0) {
        delta = -1;
        pos = *(s32*)cursor;
        for (i = 0; i < pos; i++) {
            delta *= radix;
        }
    }

    if (mode == 0) {
        signed_sum = (s64)value + (s64)delta;
        if (signed_sum < (s64)-0x80000000) {
            signed_sum = (s64)-0x80000000;
        }
        if (signed_sum > (s64)0x7FFFFFFF) {
            signed_sum = (s64)0x7FFFFFFF;
        }
    } else {
        signed_sum = (u32)value + (s64)delta;
        if (signed_sum < 0) {
            signed_sum = 0;
        }
        if (signed_sum > (s64)0xFFFFFFFFU) {
            signed_sum = (s64)0xFFFFFFFFU;
        }
    }

    *(s32*)(ctx + 0x80) = (s32)signed_sum;
    fn_801040B8(ctx, 0, (s32)signed_sum);
    return 0;
}
#endif

/* fn_8001329C - 0x8001329C | size: 0x3cc */
extern void fn_8001EC08(void);
extern void fn_80132A38(s32, s32);
extern void jumptable_802E4D90();
/* Keep this on the asm wrapper until the active-C rewrite compiles cleanly. */
#if 1
asm void fn_8001329C(void) {
#include "src/game/gs_event_exec_fn_8001329C.inc"
}
#else
void fn_8001329C(void) { /* TODO */ }
#endif

/* fn_80013668 - 0x80013668 | size: 0xdc */
extern u8 fn_80107ED8(s32, s32);
#pragma push
#pragma peephole off
s32 fn_80013668(u8* ctx) {
    s32 flag = 0;
    switch ((s32)(s8)ctx[1]) {
    case 0:
        if ((s32)(s8)ctx[2] == 0) flag = 1;
        if ((u8)fn_80107ED8(*(s32*)(ctx + 4), 0x2a) == 1) flag = 1;
        if ((u8)flag != 0) {
            fn_801080CC(*(s32*)(ctx + 4), 0x26);
            ctx[2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[2] == 0) {
            fn_801080CC(*(s32*)(ctx + 4), 0x2a);
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

/* fn_8001374C - 0x8001374C | size: 0x168 */
extern void* fn_80129BC8(u32, u8, void*, s32, s32, s32);
extern void* fn_801297D8(u32, void*, s32, s32, s32);
extern u8  fn_801429E8(void*);
extern s32 fn_80143C68(void*);
extern s32 fn_80129A78(u32, s32, s32, s32);
extern void fn_8012959C(u32, s32, s32, s16);
extern u8 lbl_80266918[];
#define sSummaryPageEntries lbl_80266918
extern u32 lbl_8047A2F8;
#pragma push
#pragma peephole off
#pragma push
#pragma optimization_level 2
s32 fn_8001374C(s32 entry_idx, s32 target_n, s32* out) {
    s32   buf[5];
    u8*   entry;
    s32   flag;
    void* list;
    s32   idx;
    s32   i;
    entry = (u8*)sSummaryPageEntries + entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = fn_80129BC8(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = fn_801297D8(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = fn_80143C68(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    if (fn_80129A78(lbl_8047A2F8, idx, 1, -1) > 0) {
        fn_80106D3C(2, 0x4263, 1, 0);
        fn_801069FC(1);
        *out = 0;
    } else {
        fn_8012959C(lbl_8047A2F8, idx, 1, (s16)target_n);
        fn_80132A38(0x2d, (u16)idx);
        fn_80106D3C(2, 0x4268, 1, 0);
        fn_801069FC(1);
        *out = 0;
    }
    return 0;
}
#pragma pop
#pragma pop

/* fn_800138B4 - 0x800138B4 | size: 0x164 */
extern u8 fn_80129718(u32, s32);
extern void fn_80129650(u32, s32, s32, s32);
#pragma push
#pragma peephole off
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
        list = fn_80129BC8(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = fn_801297D8(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = fn_80143C68(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    if (fn_80129718(lbl_8047A2F8, idx) == 0) {
        fn_80106D3C(2, 0x425F, 1, 0);
        fn_801069FC(1);
        *out = 0;
        return 2;
    }
    fn_80129650(lbl_8047A2F8, idx, 1, -1);
    fn_80132A38(0x2d, (u16)idx);
    fn_80106D3C(2, 0x4264, 1, 0);
    fn_801069FC(1);
    *out = 1;
    return 0;
}
#pragma pop

/* fn_80013A18 - 0x80013A18 | size: 0x3e4 */
extern void fn_80143C50(void);
extern void fn_801046B8(void);
extern void fn_801026A4(void);
extern void fn_80102510(void);
extern void menuCloseSync(void);
extern u8 lbl_80266BD8[];
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2FC;
#if 1
asm void fn_80013A18(void) {
#include "src/game/gs_event_exec_fn_80013A18.inc"
}
#else
void fn_80013A18(void) { /* TODO */ }
#endif

/* fn_80013DFC - 0x80013DFC | size: 0x184 */
extern void fn_801440A0(u16);
extern u8   fn_80143E88(void);
extern u8   fn_80144014(void);
extern s32  fn_8001BCEC(u8, u8, s32, s32);
extern void fn_8001B184(void);
#pragma push
#pragma peephole off
s32 fn_80013DFC(s32 entry_idx, s32 target_n, s32* out) {
    s32   buf[5];
    u8*   entry;
    s32   flag;
    void* list;
    s32   idx;
    s32   i;
    s32   x;
    entry = (u8*)sSummaryPageEntries + entry_idx * 0x4C;
    flag  = *(s32*)(entry + 4);
    if (flag >= 0) {
        list = fn_80129BC8(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = fn_801297D8(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = fn_80143C68(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    fn_801440A0(idx);
    if (fn_80143E88() != 0xFF) {
        fn_80132A38(0x2d, (u16)idx);
        fn_80106D3C(2, 0x4262, 1, 0);
        fn_801069FC(1);
        *out = 0;
        return 0;
    }
    fn_801440A0(idx);
    x = (u8)fn_80144014();
    fn_80102568(0x59, 0, 1);
    lbl_8047A2EC = fn_8001BCEC((u8)x, (u8)target_n, idx, 0);
    fn_8001B184();
    if ((s32)lbl_8047A2EC < 0) {
        return 3;
    }
    *out = 0;
    return 0;
}
#pragma pop

/* fn_80013F80 - 0x80013F80 | size: 0x17c */
extern u32 lbl_8047A2E0;
typedef s32 (*MusicFp)(u16, s32*);
extern MusicFp fn_80143DE4(void);
extern MusicFp fn_80143DCC(void);
#pragma push
#pragma peephole off
s32 fn_80013F80(s32 entry_idx, s32 target_n, s32* out) {
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
        list = fn_80129BC8(lbl_8047A2F8, (u8)flag, buf, 0, 0, 0);
    } else {
        list = fn_801297D8(lbl_8047A2F8, buf, 0, 0, 0);
    }
    idx = -1;
    i = 0;
    while (i < *(u16*)buf) {
        if (fn_801429E8(list)) {
            idx++;
            if (idx >= target_n) {
                idx = fn_80143C68(list);
                goto after;
            }
        }
        i++;
        list = (u8*)list + 4;
    }
    idx = 0;
after:
    id = (u16)idx;
    fn_801440A0(id);
    if ((s32)lbl_8047A2E0 == 0) {
        fp = fn_80143DE4();
    } else {
        fp = fn_80143DCC();
    }
    if (fp == 0) {
        fn_80106D3C(2, 0x4261, 1, 0);
        fn_801069FC(1);
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
#pragma pop

/* fn_800140FC - 0x800140FC | size: 0x14 */
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2F4;
void fn_800140FC(u32* out1, u32* out2) {
    *out1 = lbl_8047A2F8;
    *out2 = lbl_8047A2F4;
}

/* fn_80014118 - 0x80014118 | size: 0x80 */
extern s32 fn_801FCE60(u32, u16);
extern s32 fn_80205BE8(void);
extern s32 fn_8012A5B0(u32, s32, u16);
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F4;
extern u32 lbl_8047A2F8;
#pragma push
#pragma peephole off
void fn_80014118(s32 arg, s32* out1, s32* out2) {
    s32 val;
    s32 other;
    if ((s32)lbl_8047A2E0 == 1) {
        other = fn_801FCE60(lbl_8047A2F4, (u16)arg);
        val   = fn_80205BE8();
    } else {
        val   = fn_8012A5B0(lbl_8047A2F8, 3, (u16)arg);
        other = 0;
    }
    *out1 = val;
    *out2 = other;
}
#pragma pop

/* fn_80014198 - 0x80014198 | size: 0x24 */
extern u32 lbl_8047A2EC;
extern void fn_8001B184(void);
void fn_80014198(u32 val) {
    lbl_8047A2EC = val;
    fn_8001B184();
}

/* fn_800141BC - 0x800141BC | size: 0x78 */
extern s32 fn_8001BD3C(s32 mode, u8 a, u16 b, u32 p);
#pragma push
#pragma peephole off
s32 fn_800141BC(s32 arg0, s32 arg1) {
    s32 mode;
    s32 result;
    fn_80102568(0x59, 0, 1);
    if ((s32)lbl_8047A2E0 == 1) {
        mode = 4;
    } else {
        mode = 3;
    }
    result = fn_8001BD3C(mode, (u8)arg1, arg0, lbl_8047A2F4);
    if (arg1 == 0) {
        result = -1;
    }
    return result;
}
#pragma pop

/* fn_80014234 - 0x80014234 | size: 0xe8 */
extern u8 lbl_80266B58[];

typedef struct {
    s32 key;
    u8* inner;
    s32 count;
} EvTbl;

typedef struct {
    s32 _0;
    s32 species_a;
    s32 species_b;
} EvEntry;

#pragma push
#pragma peephole off
s32 fn_80014234(u8* ctx, u8* tgt) {
    u8* walk;
    u8* p;
    s32 key;
    s32 idx;
    s32 slot;
    s32 species;
    EvEntry* e;

    p    = *(u8**)(ctx + 0x60);
    walk = lbl_80266B58;
    key  = *(s32*)(p + 4);
    for (idx = 0; idx < 4; idx++) {
        if (key == *(s32*)walk) break;
        walk += 0xc;
    }
    if (idx >= 4) return 0;
    slot = (s32)(s8)ctx[0x95];
    if (slot < 0 || slot >= ((EvTbl*)lbl_80266B58)[idx].count) return 0;
    e = &((EvEntry*)((EvTbl*)lbl_80266B58)[idx].inner)[slot];
    species = *(s16*)(tgt + 0x6);
    if (e->species_a == species || e->species_b == species) {
        tgt[0x67] = 0xFF;
    } else {
        tgt[0x67] = 0;
    }
    return 0;
}
#pragma pop

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
    fn_80132A38(0x50, count * (s32)lbl_8047A2FC);
    rand = fn_800FA444(0x151);
    fn_800FB680(*(s16*)(arg1 + 0x54) - (s32)(s16)((u32)rand >> 16), 0, -1, 0x151);
    return 0;
}
#endif

/* fn_80014398 - 0x80014398 | size: 0x1b8 */
extern void fn_800FB8C8(s32, s32, s16, s16, s32, s32);
#pragma push
#pragma peephole off
s32 fn_80014398(u8* ctx, u8* arg1) {
    u8* p = *(u8**)(ctx + 0x60);
    s32 key = *(s32*)(p + 4);
    s32 idx = 0;
    s32 idx2 = 0;
    s32 cnt;
    s32 species;
    u8* inner;
    s32 iv;
    s32 cnt2;
    s32 i;
    s32 tmp;
    EvTbl* tbl = (EvTbl*)lbl_80266B58;

    if ((((key != tbl[0].key) && (idx = 1, key != tbl[1].key)) &&
         (idx = 2, key != tbl[2].key)) &&
        (idx = 3, key != tbl[3].key)) {
        idx = 4;
    }
    if (idx >= 4) return 0;

    inner = tbl[idx].inner;
    cnt   = tbl[idx].count;
    species = *(s16*)(arg1 + 6);
    if (cnt > 0) {
        do {
            if (*(s32*)inner == species) break;
            idx2++;
            inner += 0xC;
        } while (idx2 < cnt);
    }
    if (idx2 >= tbl[idx].count) return 0;

    iv = 1;
    cnt2 = 0;
    if ((cnt - idx2) - 1 > 0) {
        if ((cnt - idx2) - 1 > 8) {
            s32 adj = (cnt - idx2) - 9;
            s32 iters = (adj + 7) >> 3;
            if (adj > 0) {
                for (i = 0; i < iters; i++) {
                    iv  *= 100000000;
                    cnt2 += 8;
                }
            }
        }
        if (cnt2 < (cnt - idx2) - 1) {
            s32 rem = ((cnt - idx2) - 1) - cnt2;
            for (i = 0; i < rem; i++) {
                iv *= 10;
            }
        }
    }
    tmp = (s32)lbl_8047A2FC / iv;
    fn_80132A38(0x34, tmp - (tmp / 10) * 10);
    fn_800FB8C8(0, 0, *(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56), -1, 0xC9);
    return 0;
}
#pragma pop

/* fn_80014550 - 0x80014550 | size: 0x24 */
s32 fn_80014550(u8* src_struct, u8* dst) {
    u8* src = *(u8**)(src_struct + 0x60);
    dst[0x64] = src[0];
    dst[0x65] = src[1];
    dst[0x66] = src[2];
    return 0;
}

/* fn_80014574 - 0x80014574 | size: 0x4d4 */
extern void fn_80166A50(void);
extern u32 lbl_8047A2FC;
#if 1
asm void fn_80014574(void) {
#include "src/game/gs_event_exec_fn_80014574.inc"
}
#else
void fn_80014574(void) { /* TODO */ }
#endif

/* fn_80014A48 - 0x80014A48 | size: 0x9c */
#pragma push
#pragma peephole off
s32 fn_80014A48(u8* ctx) {
    u8* p = *(u8**)(ctx + 0x60);
    switch ((s32)(s8)ctx[0x1]) {
    case 0:
        if ((s32)(s8)ctx[0x2] == 0) {
            fn_801080CC(*(s32*)(p + 0x4), 0x6e);
            ctx[0x2] = 1;
        }
        break;
    case 3:
        if ((s32)(s8)ctx[0x2] == 0) {
            fn_801080CC(*(s32*)(p + 0x4), 0x72);
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
#pragma push
#pragma scheduling off
#pragma peephole off
s32 fn_80014C38(u8* ctx, u8* tgt) {
    u8* p = *(u8**)(ctx + 0x60);
    s32 k0 = lbl_80266B9C[0];
    s32 k1 = lbl_80266B9C[1];
    s32 k2 = lbl_80266B9C[2];
    s32 k3 = lbl_80266B9C[3];
    s32 k4 = lbl_80266B9C[4];
    s32 idx0 = 0;
    s32 species = *(s16*)(tgt + 0x6);
    s32 base;
    s32 idx;
    u8* inner;
    s32 flag;
    if (species != k0) {
        idx0 = 1;
        if (species != k1) {
            idx0 = 2;
            if (species != k2) {
                idx0 = 3;
                if (species != k3) {
                    idx0 = 4;
                    if (species != k4) idx0 = 5;
                }
            }
        }
    }
    base = *(s32*)(p + 0x8);
    idx = idx0 - (5 - base);
    if (idx < 0 || idx >= base) return 0;
    inner = *(u8**)(p + 0x4) + idx * 0xC;
    flag  = (*(u16*)(inner + 0x8)) & 1;
    fn_800FB680(0, 0, flag ? 0x7f7f7fff : -1, *(s32*)inner);
    return 0;
}
#pragma pop

/* fn_80014D1C - 0x80014D1C | size: 0x134 */
typedef struct { s32 key; s16 a; s16 b; s32 flag; } PurEntry;
extern PurEntry lbl_802E4DB0[];
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
#pragma push
#pragma peephole off
s32 fn_80014E50(u8* ctx) {
    u8* state;
    u8* p;
    s32 count;
    s32 slot;
    s32 new_slot;
    u8* inner;

    p     = *(u8**)(ctx + 0x60);
    state = fn_80105624();
    count = *(s32*)(p + 8);

    if ((*(volatile u16*)(state + 6) & 2) != 0) {
        slot  = (s32)(s8)ctx[0x95];
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
        slot  = (s32)(s8)ctx[0x95];
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
#pragma push
#pragma peephole off
s32 fn_80014F48(u8* ctx) {
    f32 tmp;
    switch ((s32)(s8)ctx[1]) {
    case 0:
        if ((s32)(s8)ctx[2] == 0) {
            fn_801080CC(0x5a, 0x66);
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
            fn_801080CC(0x5a, 0x6a);
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
