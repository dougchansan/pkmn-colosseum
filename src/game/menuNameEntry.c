/**
 * @file menuNameEntry.c
 * @brief Name-entry menu: player/Pokemon name input keyboard, back-panel
 *        model/ball preview, and the shop-adjacent draw helpers that share
 *        this address range.
 *
 * Split from the former game/gs_worldmap.c CodeCandidate bucket
 * (0x80026370-0x80030170); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit (0x80026370-0x80029850). This
 * range was originally mislabeled as world-map code; it is actually the
 * head of the XD-era menuNameEntry.cpp translation unit.
 */

#include "dolphin/types.h"
#include "game/menu/menu_name_entry.h"

/* ===== Phase 2 recovery stubs ===== */

typedef struct MenuNameEntryObject {
    u8 pad_00[0x8b];
    u8 alpha;
} MenuNameEntryObject;

/* fn_80026370 - 0x80026370 | size: 0x20 */
#if 0
asm void fn_80026370(void) {
#include "src/game/gs_worldmap_fn_80026370.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026370(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}
#endif


/* fn_80026390 - 0x80026390 | size: 0x20 */
#if 0
asm void fn_80026390(void) {
#include "src/game/gs_worldmap_fn_80026390.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026390(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}
#endif

/* fn_800263B0 - 0x800263B0 | size: 0x6c | WALL 92.6%: li vs mr pattern */
extern u8 lbl_80266E18[];
#if 0
asm void fn_800263B0(void) {
#include "src/game/gs_worldmap_fn_800263B0.inc"
}
#else
#pragma optimization_level 4
s32 fn_800263B0(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    u8 r0, r5, r6;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx + 1;
    if (idx >= 2) idx -= 2;
    if (idx < 0 || idx >= 2) {
        r0 = 0xff; r5 = 0xff; r6 = 0xff;
    } else {
        r0 = lbl_80266E18[idx * 0x18 + 0];
        r5 = lbl_80266E18[idx * 0x18 + 1];
        r6 = lbl_80266E18[idx * 0x18 + 2];
    }
    r4[0x64] = r0;
    r4[0x65] = r5;
    r4[0x66] = r6;
    return 0;
}
#endif

/* fn_8002641C - 0x8002641C | size: 0x5c | WALL 91.3%: li vs mr pattern */
#if 0
asm void fn_8002641C(void) {
#include "src/game/gs_worldmap_fn_8002641C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002641C(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    u8 r0, r5, r6;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx;
    if (idx < 0 || idx >= 2) {
        r0 = 0xff; r5 = 0xff; r6 = 0xff;
    } else {
        r0 = lbl_80266E18[idx * 0x18 + 0];
        r5 = lbl_80266E18[idx * 0x18 + 1];
        r6 = lbl_80266E18[idx * 0x18 + 2];
    }
    r4[0x64] = r0;
    r4[0x65] = r5;
    r4[0x66] = r6;
    return 0;
}
#endif

/* fn_80026478 - 0x80026478 | size: 0xa4 */
extern void* heroGetStatus(s32, s32, u32);
extern u8 pokemonCheckValid(void);
extern u8 pokemonGetSex(void*);
#if 0
asm void fn_80026478(void) {
#include "src/game/gs_worldmap_fn_80026478.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026478(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)pokemonCheckValid() == 0) goto L_done;
    if ((u32)(pokemonGetSex(r31) & 0xff) != 1) goto L_done;
    r30 = 0xff;
L_done:
    r4[0x67] = r30;
    return 0;
}
#pragma pop
#endif

/* fn_8002651C - 0x8002651C | size: 0xa4 */
#if 0
asm void fn_8002651C(void) {
#include "src/game/gs_worldmap_fn_8002651C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002651C(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)pokemonCheckValid() == 0) goto L_done2;
    if ((u32)(pokemonGetSex(r31) & 0xff) != 0) goto L_done2;
    r30 = 0xff;
L_done2:
    r4[0x67] = r30;
    return 0;
}
#pragma pop
#endif

/* fn_800265C0 - 0x800265C0 | size: 0x40 */
extern u8 lbl_80266DD8[];
#if 0
asm void fn_800265C0(void) {
#include "src/game/gs_worldmap_fn_800265C0.inc"
}
#else
#pragma optimization_level 4
s32 fn_800265C0(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] != 7) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026600 - 0x80026600 | size: 0x40 */
#if 0
asm void fn_80026600(void) {
#include "src/game/gs_worldmap_fn_80026600.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026600(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] != 8) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026640 - 0x80026640 | size: 0x40 */
#if 0
asm void fn_80026640(void) {
#include "src/game/gs_worldmap_fn_80026640.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026640(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 0xa) goto L_40_then;
    r4[0x67] = 0;
    goto L_40_end;
L_40_then:
    r4[0x67] = 0xff;
L_40_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026680 - 0x80026680 | size: 0x40 */
#if 0
asm void fn_80026680(void) {
#include "src/game/gs_worldmap_fn_80026680.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026680(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 7) goto L_80_then;
    r4[0x67] = 0;
    goto L_80_end;
L_80_then:
    r4[0x67] = 0xff;
L_80_end:
    return 0;
}
#pragma pop
#endif

/* fn_800266C0 - 0x800266C0 | size: 0x40 */
#if 0
asm void fn_800266C0(void) {
#include "src/game/gs_worldmap_fn_800266C0.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_800266C0(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    u8* base;
    ctx = *(void**)((u8*)r3 + 0x60);
    base = lbl_80266DD8;
    base = base + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)base;
    if (entry[1] == 8) goto L_C0_then;
    r4[0x67] = 0;
    goto L_C0_end;
L_C0_then:
    r4[0x67] = 0xff;
L_C0_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026700 - 0x80026700 | size: 0x40 */
#if 0
asm void fn_80026700(void) {
#include "src/game/gs_worldmap_fn_80026700.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026700(void* r3, u8* r4) {
    void* ctx;
    u8* new_var;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    new_var = lbl_80266DD8;
    new_var = new_var + (*(s32*)((u8*)ctx + 0x1c) << 4);
    entry = (s32*)new_var;
    if (entry[1] == 0xa) goto L_700_then;
    r4[0x67] = 0;
    goto L_700_end;
L_700_then:
    r4[0x67] = 0xff;
L_700_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026740 - 0x80026740 | size: 0x90 | active C 65.8% (2026-06-29)
 *
 * The active C below is FAITHFUL, byte-correct logic and measures 65.8% after
 * activation. The inactive asm wrapper measured 96.67% under
 * objdiff (the 96.67 is a pure numeric-vs-named float-reloc disassembler
 * artifact: the .inc emits `lfs f2,-0x7d68(r2)` while the target object carries
 * `lbl_8047B938@sda21`; both resolve to the same sdata2 address).
 * Current active C still needs a permuter attack for byte matching.
 *
 * Levers that DID land (took the draft 72% -> 85.69%):
 *   - `#pragma fp_contract on`  -> `1.0 - 255.0*x` fuses to a single fnmsubs
 *   - declaring `state`/`index` BEFORE `ctx` reserves r5 for `state`, pinning
 *     ctx to r6 to match the target's whole-function allocation (+11%)
 *   - if(state!=7){0}else{...} single trailing `return 0` -> one epilogue li r3,0
 *   - dropping the (u8)/(s32) cast on the r4[0x67] store -> stb truncates with
 *     no redundant clrlwi (peephole off keeps the (s16) extsh on the X store)
 *   - `#pragma scheduling on` (off regresses to 71%)
 *
 * Residual WALL (3 CW reg-alloc / scheduler ties, not source-controllable):
 *   1. table base/index scratch rotation: target keeps base in r3 + idx in r0
 *      (`add r3,r3,r0`); CW emits base->r0 + idx->r3 (`add r3,r0,r3`).
 *   2. X multiply result: target reuses freed r5 (`mulli r5,r0,0x1a`); CW keeps
 *      it in r0 (`mulli r0,r0,0x1a`).
 *   3. float const-load schedule: target interleaves lfs f2/f0 into the X-mul
 *      load-delay slots; CW won't hoist them across the sth store.
 * -> permuter territory. Same wall applies to siblings fn_800267D0/fn_80026860.
 */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 0
asm void fn_80026740(void) {
#include "src/game/gs_worldmap_fn_80026740.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_80026740(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* state-machine entry table: each entry 16 bytes */
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 7) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x48));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}
#pragma pop
#endif

/* fn_800267D0 - 0x800267D0 | size: 0x90 | WALL ~85.7% — sibling of fn_80026740.
 * selector=8, base ptr ctx+0x44. The inactive asm measured 96.67% (mostly
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The C is active for honest
 * decomp progress. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 0
asm void fn_800267D0(void) {
#include "src/game/gs_worldmap_fn_800267D0.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_800267D0(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 8) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x44));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80026860 - 0x80026860 | size: 0x90 */
/* fn_80026860 - 0x80026860 | size: 0x90 | WALL ~85.7% — sibling of fn_80026740.
 * selector=0xa, base ptr ctx+0x40. The inactive asm measured 96.67% (mostly
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The C is active for honest
 * decomp progress. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 0
asm void fn_80026860(void) {
#include "src/game/gs_worldmap_fn_80026860.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_80026860(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 0xa) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x40));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}
#pragma pop
#endif

/* fn_800268F0 - 0x800268F0 | size: 0x254 */
extern void msgctrlSetValue(s32, void*);
extern u32 GSmsgGetRect(u32);
extern void fn_800FB680(s32, s32, s32, u32);
extern s32 GSmsgGetLength(void*);
extern void* GSmsgGetGSchar(u32);
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_800268F0(void) {
#include "src/game/gs_worldmap_fn_800268F0.inc"
}
#else
/*
 * GSmap_DrawLocation0 -- 0x800268F0, size 0x254
 *
 * Draws a world-map location icon of type 0 onto the draw-context canvas.
 * r3 = self (world-map object); r4 = draw-context (canvas u8*).
 *
 * Control flow:
 *   1. Look up state-entry word[1] for the current index in lbl_80266DD8.
 *      If it != 7 → clear r4[0x67] and return.
 *   2. Walk the u16 item list at ctx->0x18, emitting each icon with
 *      msgctrlSetValue + fn_800FB680.
 *   3. If fewer items were drawn than the entry capacity AND r3[0x98]==0:
 *      look up a "next" item through lbl_80266E18[row][col].item_list,
 *      validate it, optionally draw a special computed-X icon.
 *   4. Always set r4[0x67] = 0xff on the way out of the "state==7" branch.
 */
s32 fn_800268F0(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* 16-byte-entry state table */
    extern u8  lbl_80266E18[];   /* 0x18-byte-row, 4-word-col item handle table */
    extern f32 lbl_8047B934;     /* map center X (float) */
    extern f32 lbl_8047B938;     /* map X scale (float) */

    extern void msgctrlSetValue(s32 cmd, void* buf);
    extern u32  GSmsgGetRect(u32 id);
    extern void fn_800FB680(s32 x, s32 y, s32 palette, u32 cmd);
    extern s32  GSmsgGetLength(void* handle);
    extern void* GSmsgGetGSchar(u32 id);

    u8  *self  = (u8*)r3;
    u8  *ctx   = *(u8**)(self + 0x60);
    s32  state_idx;
    u32  *entry_w1_base;
    u32   entry_cap;
    u16  *item_ptr;
    s32   item_count;
    s32   y_cursor;
    u16   buf[2];
    u32   cap2;

    /* entry = lbl_80266DD8 + state_index * 16; word[1] = entry[4] as u32 */
    state_idx  = *(s32*)(ctx + 0x1c);
    entry_w1_base = (u32*)(lbl_80266DD8 + 4);  /* points at word[1] of entry[0] */
    entry_cap = entry_w1_base[state_idx * 4];   /* stride 16 bytes = 4 u32 words */

    if (entry_cap != 7) {
        r4[0x67] = 0;
        return 0;
    }

    /* --- Phase 1: draw existing items ---------------------------------- */
    item_ptr = (u16*)(*(u8**)(ctx + 0x18)); /* walking u16 list */
    item_count = 0;
    y_cursor = 0;

    while (*item_ptr != 0) {
        u16  item_id;
        s32  palette;
        u32  raw_x;
        s32  pos_x;
        s32  delta;
        s32  x_off;

        item_id = *item_ptr;
        palette = (s32)(self[0x8b]) | (s32)(-0x100);

        buf[0] = item_id;
        buf[1] = 0;
        msgctrlSetValue(0x37, buf);

        /* compute X offset: signed upper-16 of GSmsgGetRect(0xce), floor-div */
        raw_x = GSmsgGetRect(0xce);
        pos_x = (s16)(raw_x >> 16);         /* extsh of upper half */
        delta = 0x1b - pos_x;
        x_off = (delta + (s32)((u32)delta >> 31)) >> 1; /* arithmetic floor */

        fn_800FB680(y_cursor + x_off, 0, palette, 0xce);

        y_cursor   += 0x1a;
        item_count += 1;
        item_ptr   += 1;   /* advance by one u16 word */
    }

    /* --- Phase 2: optionally draw "next" item -------------------------- */
    /* Re-read cap (entry[1] for current state) */
    state_idx = *(s32*)(ctx + 0x1c);
    cap2 = entry_w1_base[state_idx * 4];

    if ((u32)item_count >= cap2) goto L_done;
    if (self[0x98] != 0)         goto L_done;

    {
        /* Navigate lbl_80266E18[row][col] to find an item handle */
        s32  map_row  = *(s32*)*(u32**)(ctx + 0x24);  /* **(s32**)(ctx+0x24) */
        s32  item_idx = *(s32*)*(u32**)(ctx + 0x28);  /* **(s32**)(ctx+0x28) */
        s32  map_col  = *(s32*)*(u32**)(ctx + 0x2c);  /* **(s32**)(ctx+0x2c) */

        /* lbl_80266E18: row stride 0x18 bytes (6 u32s?), col stride 4 bytes;
           handle is at +8 within each col cell */
        u32  handle = 0;
        u8  *arr;
        s32  list_count;

        if (map_row < 0 || map_row >= 2) goto L_check_handle;
        if (map_col < 0 || map_col >= 4) goto L_check_handle;

        handle = *(u32*)(lbl_80266E18 + map_row * 0x18 + map_col * 4 + 8);

        list_count = GSmsgGetLength((void*)handle);
        if (item_idx < 0 || item_idx >= list_count) {
            handle = 0;
            goto L_check_handle;
        }
        /* read u16 at item_idx from the handle's array */
        arr = (u8*)GSmsgGetGSchar(handle);
        handle = (u32)*(u16*)(arr + item_idx * 2);

    L_check_handle:
        if ((u16)handle == 0) goto L_done;

        /* Compare against special list 0x2efc */
        {
            u8   *special_arr = (u8*)GSmsgGetGSchar(0x2efc);
            u16   special_id  = *(u16*)(special_arr + 0);
            u32   palette_off;
            f32  *pos_fptr;
            f32   map_pos;
            f32   cx;
            f32   scale;
            s32   ix;
            u8    x_byte;
            u32   pal_word;
            u16   draw_buf[2];
            u32   raw_x2;
            s32   pos_x2;
            s32   delta2;
            s32   x_off2;
            s32   y2;

            if ((u16)handle == special_id) {
                palette_off = 0;
            } else {
                palette_off = 6;
            }

            if (palette_off != 6) goto L_done;

            /* Compute the special icon draw */
            pos_fptr = *(f32**)(ctx + 0x30);
            map_pos = pos_fptr[0];
            cx = lbl_8047B934;
            scale = lbl_8047B938;
            /* computed X byte: truncate-to-int of scale*(center - map_pos) */
            ix = (s32)(scale * (cx - map_pos)); /* fctiwz = truncate */
            x_byte = (u8)(u32)ix;                   /* clrlwi 24 */
            /* palette word: 0x00FF0000 | x_byte -- ENDIAN-QA: big-endian
               halfword where high byte = 0xff, low byte = computed x */
            pal_word = 0x00FF0000u | x_byte;          /* oris rN,r0,0xff */

            draw_buf[0] = (u16)handle;                      /* sth r23, sp+8  */
            draw_buf[1] = 0;                                 /* sth r0, sp+0xa */
            msgctrlSetValue(0x37, draw_buf);

            /* X offset computation (same arithmetic as Phase 1) */
            raw_x2 = GSmsgGetRect(0xce);
            pos_x2 = (s16)(raw_x2 >> 16);
            delta2 = 0x1b - pos_x2;
            x_off2 = (delta2 + (s32)((u32)delta2 >> 31)) >> 1;
            y2 = item_count * 0x1a + x_off2;  /* mulli + add */
            fn_800FB680(y2, 0, (s32)pal_word, 0xce);
        }
    }

L_done:
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026B44 - 0x80026B44 | size: 0x254 */
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_80026B44(void) {
#include "src/game/gs_worldmap_fn_80026B44.inc"
}
#else
/*
 * GSmap_DrawLocation1 -- Draw world-map location icon type 1 (state-8 entry).
 * Identical structure to GSmap_DrawLocation0 (fn_800268F0) except the entry
 * word at lbl_80266DD8[idx*16 + 4] is tested for 0x8 instead of 0x7.
 *
 * r3 = self object pointer
 * r4 = draw-context / output u8 buffer (byte 0x67 = visibility flag)
 *
 * Flow:
 *  1. If the table entry is not 8 → write 0 to r4[0x67] and return.
 *  2. Walk the name string (u16 array at ctx+0x18), rendering each glyph.
 *  3. If char-count < table-count AND object not flagged at +0x98:
 *       look up a special glyph via lbl_80266E18 + sub-tables,
 *       and if it differs from the "current" glyph (0x2efc pool),
 *       draw it at the next x position with an animated alpha byte derived
 *       from a float countdown at ctx+0x30.
 *  4. Write 0xFF to r4[0x67] on the success path.
 */
s32 fn_80026B44(void *r3, u8 *r4)
{
    extern u8  lbl_80266DD8[];   /* state table: 16-byte entries; word at +4 = type */
    extern u8  lbl_80266E18[];   /* glyph-set table: 0x18-byte stride, ptr at +8    */
    extern f32 lbl_8047B934;     /* float constant: animation reference value        */
    extern f32 lbl_8047B938;     /* float constant: animation scale                  */

    extern void   msgctrlSetValue(s32 cmd, void *buf); /* set glyph draw param */
    extern u32    GSmsgGetRect(u32 pool);            /* query pool metrics   */
    extern void   fn_800FB680(s32 x, s32 y, s32 color, u32 glyph_id); /* draw glyph */
    extern s32    GSmsgGetLength(void *tbl);           /* get entry count      */
    extern void  *GSmsgGetGSchar(u32 key);             /* get base pointer     */

    void  *ctx;       /* r30: *(void**)(r3 + 0x60) */
    u16   *name_ptr;  /* r24: pointer into u16 name string */
    s32    char_count;/* r31: number of chars drawn in loop */
    s32    x_acc;     /* r29: accumulated x position */
    s32    color;     /* r23: packed color/alpha word */
    u32    entry_word;/* word from lbl_80266DD8 entry[1] (offset +4 in 16-byte entry) */
    u16    buf[2];    /* r28 -> stack buf at sp+0xc: {glyph_id, 0} */
    u32    special;   /* r23 after glyph-set lookup: glyph handle / u16 id */
    void  *tbl_ptr;   /* pointer into lbl_80266E18 sub-table */
    s32    sub_idx0;  /* r5: first index into lbl_80266E18 rows */
    s32    sub_idx1;  /* r0: second index (column) */
    s32    half_w;    /* width correction: (0x1b - measured_width + 1) / 2 rounded */
    s32    measured;  /* signed half-word from GSmsgGetRect high half */
    u8    *arr;
    s32    count;
    u16    glyph_id;
    u8     anim_byte;
    f32    anim_val;
    u32    cur_word;

    ctx = *(void **)((u8 *)r3 + 0x60);

    /* lbl_80266DD8 is a flat array of 16-byte entries; word at [idx*16 + 4] = type */
    {
        s32 idx = *(s32 *)((u8 *)ctx + 0x1c);
        entry_word = *(u32 *)(lbl_80266DD8 + (u32)(idx << 4) + 4);
    }

    if (entry_word != 0x8) {
        r4[0x67] = 0;
        return 0;
    }

    /* --- Name-string render loop ---------------------------------------- */
    char_count = 0;
    x_acc      = 0;
    name_ptr   = (u16 *)*(u32 *)((u8 *)ctx + 0x18);

    while (*name_ptr != 0) {
        glyph_id = *name_ptr;

        /* color: top 24 bits = 0xFFFFFF, bottom 8 bits = alpha from +0x8B */
        color = (s32)((u8 *)r3)[0x8b] | (s32)(-0x100);

        buf[0] = glyph_id;
        buf[1] = 0;
        msgctrlSetValue(0x37, buf);

        /* measured width: upper 16 bits of GSmsgGetRect return, sign-extended */
        measured = (s32)(s16)(u16)((u32)GSmsgGetRect(0xce) >> 16);

        /* center the glyph: half of (0x1b - measured), rounding toward zero */
        {
            s32 diff = 0x1b - measured;
            half_w   = (diff + ((u32)diff >> 31)) >> 1;
        }

        fn_800FB680(x_acc + half_w, 0, color, 0xce);

        x_acc += 0x1a;
        char_count++;
        name_ptr++;
    }

    /* Re-read entry_word (compiler reloads from table) */
    {
        s32 idx = *(s32 *)((u8 *)ctx + 0x1c);
        entry_word = *(u32 *)(lbl_80266DD8 + (u32)(idx << 4) + 4);
    }

    if ((s32)char_count >= (s32)entry_word) goto L_set_visible;
    if (((u8 *)r3)[0x98] != 0)              goto L_set_visible;

    /* --- Special-glyph lookup ------------------------------------------- */
    /* Three indirection layers from ctx+0x24, +0x28, +0x2c               */
    {
        void **p_a = *(void ***)((u8 *)ctx + 0x24); /* ptr-to-ptr */
        void **p_b = *(void ***)((u8 *)ctx + 0x28);
        void **p_c = *(void ***)((u8 *)ctx + 0x2c);
        sub_idx0 = *(s32 *)p_a;   /* r5 */
        sub_idx1 = *(s32 *)p_c;   /* r0 */
        /* r25 used later as second range value from p_b */
        {
            s32 val_b = *(s32 *)p_b; /* r25 = *p_b[0] */

            if (sub_idx0 < 0 || sub_idx0 >= 2) {
                special = 0;
                goto L_check_special;
            }
            if (sub_idx1 < 0 || sub_idx1 >= 4) {
                special = 0;
                goto L_check_special;
            }

            /* lbl_80266E18: stride 0x18; column stride 4; pointer at +8 in each cell */
            tbl_ptr = *(void **)(lbl_80266E18 + (u32)sub_idx0 * 0x18 + (u32)sub_idx1 * 4 + 8);
            special = (u32)tbl_ptr; /* r23 = the raw pointer / handle */

            count = GSmsgGetLength(tbl_ptr);

            if (val_b < 0 || val_b >= count) {
                special = 0;
                goto L_check_special;
            }

            arr    = (u8 *)GSmsgGetGSchar((u32)tbl_ptr);
            special = (u32)*(u16 *)(arr + (u32)val_b * 2); /* lhzx, zero-extended */

        L_check_special:
            /* clrlwi r0, r23, 16 → zero upper 16 bits → treat as u16 */
            if ((u16)special == 0) goto L_set_visible;

            /* Compare against "current" glyph from pool 0x2efc */
            {
                u16 *cur_base = (u16 *)GSmsgGetGSchar(0x2efc);
                u16  cur_glyph = cur_base[0]; /* lhz r0, 0x0(r3) */
                s32  flag;

                if ((u16)special == cur_glyph)
                    flag = 0;
                else
                    flag = 6;

                if (flag != 6) goto L_set_visible;
            }

            /* --- Animated-alpha final glyph ----------------------------- */
            /* f0 = (lbl_8047B934 - *(f32*)(ctx+0x30)) * lbl_8047B938      */
            /* anim_byte = (u8)(s32)f0 (truncate-toward-zero via fctiwz)   */
            /* fctiwz stores in low 32 bits of the 64-bit FP reg (stfd idiom) */
            {
                f32 *fp_countdown = *(f32 **)((u8 *)ctx + 0x30);
                f32  f0 = (lbl_8047B934 - fp_countdown[0]) * lbl_8047B938;
                s32  int_val = (s32)f0; /* fctiwz = truncate toward zero    */
                anim_byte = (u8)(int_val & 0xFF); /* clrlwi r0,r0,24 */
            }

            /* r23 = anim_byte | 0x00FF0000  (oris r23, r0, 0xff)         */
            /* This packs: bits[23:16]=0xFF, bits[7:0]=anim_byte           */
            /* ENDIAN-QA: On PPC big-endian this is a 32-bit color word where
             * byte[1] (G channel or 2nd byte) = 0xFF and byte[3] = anim_byte.
             * fn_800FB680 treats its 3rd arg as a packed color; preserve the
             * bit pattern exactly.                                          */
            {
                u32 packed_color = ((u32)anim_byte) | 0x00FF0000u; /* ENDIAN-QA */

                buf[0] = (u16)special;
                buf[1] = 0;
                msgctrlSetValue(0x37, buf);

                measured = (s32)(s16)(u16)((u32)GSmsgGetRect(0xce) >> 16);
                {
                    s32 diff = 0x1b - measured;
                    /* final glyph: x = char_count*0x1a + half_w             */
                    /* In PPC: add r3,r0,r3 then mulli r0,r31,0x1a then add  */
                    /* Both orderings are commutative; reconstruct r29 = r31*0x1a */
                    s32 x_pos = (s32)char_count * 0x1a + (diff + ((u32)diff >> 31)) / 2;
                    fn_800FB680(x_pos, 0, (s32)packed_color, 0xce);
                }
            }
        }
    }

L_set_visible:
    r4[0x67] = 0xFF;
    return 0;
}
#endif

/* fn_80026D98 - 0x80026D98 | size: 0x254 */
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_80026D98(void) {
#include "src/game/gs_worldmap_fn_80026D98.inc"
}
#else
/*
 * fn_80026D98  GSmap_DrawLocation2  0x80026D98 | 0x254 bytes
 *
 * Draws a world-map location icon of type 2 (entry[1] == 0xA).
 * Iterates the null-terminated u16 name-string stored at ctx+0x18,
 * centre-draws each character glyph, then optionally draws a
 * species-coloured highlight icon at the accumulated row position.
 *
 * Parameters:
 *   r3  – self (GS object; *(void**)(self+0x60) is the map entry ctx)
 *   r4  – output/render buffer whose byte at +0x67 is updated
 *
 * Returns 0.
 */
s32 fn_80026D98(void* r3, u8* r4)
{
    /* block-scope externs (TU convention) */
    extern u8  lbl_80266DD8[];          /* map entry table; each entry 16 bytes */
    extern u8  lbl_80266E18[];          /* species-colour table; each row 0x18 bytes */
    extern f32 lbl_8047B934;            /* float const: base animation phase    */
    extern f32 lbl_8047B938;            /* float const: animation scale factor  */
    extern void  msgctrlSetValue(s32 cmd, void* buf);
    extern u32   GSmsgGetRect(u32 id);
    extern void  fn_800FB680(s32 x, s32 y, s32 color, u32 id);
    extern s32   GSmsgGetLength(void* ptr);
    extern void* GSmsgGetGSchar(u32 id);

    /* saved registers r23-r31 */
    u32  r23;     /* working value / species entry id / packed color word */
    u16* r24;     /* pointer advancing through the u16 name string        */
    u8*  r25;     /* self pointer (r3), reused as s32 index after loop    */
    u8*  r26;     /* output buffer (r4)                                   */
    u8*  r27;     /* &lbl_80266DD8[4] — word-1 base of entry table        */
    u8*  r28;     /* sp+0xc — inline u16[2] text buffer                  */
    s32  r29;     /* accumulated x position                               */
    u32  r30;     /* ctx pointer (*(void**)(self+0x60))                   */
    s32  r31;     /* character counter                                     */

    /* temporaries */
    s32  r0;
    u16  r6;

    /* scratch buffer on the (virtual) stack; two u16s written at sp+0xc/0xe */
    u16  text_buf[2];

    /* frame-local double storage for fctiwz idiom */
    union { f64 d; struct { u32 hi; u32 lo; } w; } fconv;

    r25 = (u8*)r3;
    r26 = r4;
    r30 = (u32)(*(void**)((u8*)r25 + 0x60));

    /*
     * Load entry[1] for the current map-table slot.
     * lbl_80266DD8 is a flat array of 16-byte entries; ctx+0x1c holds
     * the current entry index.  entry[1] is at byte offset 4.
     */
    r27 = lbl_80266DD8 + 4;             /* r27 = base of entry[1] words */
    r0  = (s32)( *(u32*)((u8*)r30 + 0x1c) ) << 4;
    r0  = (s32)*(u32*)(r27 + (u32)r0);  /* entry[1] for this slot */

    /* DrawLocation2 only operates when the slot type is 0xA */
    if (r0 != 0xa) {
        r26[0x67] = 0;
        return 0;
    }

    /* ---------------------------------------------------------------
     * Phase 1: walk the null-terminated u16 name string at ctx+0x18,
     * centre-drawing each character glyph.
     * --------------------------------------------------------------- */
    r31 = 0;                            /* character counter            */
    r24 = (u16*)(*(u32*)((u8*)r30 + 0x18));   /* name string ptr */
    r29 = 0;                            /* x accumulator                */
    r28 = (u8*)text_buf;               /* &sp+0xc text buffer           */

    goto loop_check;

loop_body:
    /* pack colour: self+0x8b byte OR'd with 0xFFFFFF00 = sign-extend to s32 */
    r0  = (s32)(u8)(r25[0x8b]) | (s32)(-0x100);
    r23 = (u32)r0;

    /* set up the two-element u16 text buffer: {r6, 0} */
    text_buf[0] = r6;
    text_buf[1] = 0;

    msgctrlSetValue(0x37, r28);

    /* get half-width of the glyph: upper s16 of GSmsgGetRect(0xce) */
    r0 = (s32)(s16)(u16)( GSmsgGetRect(0xce) >> 16 );

    /* x offset = r29 + floor((0x1b - half_width) / 2)             */
    r0 = 0x1b - r0;
    /* arithmetic divide-by-2: (r0 + (r0 >> 31)) >> 1 */
    r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;
    r0 = r29 + r0;

    fn_800FB680(r0, 0, (s32)r23, 0xce);

    r29 += 0x1a;
    r31 += 1;
    r24 += 1;     /* advance u16* by one element (+2 bytes) */

loop_check:
    r6 = *r24;
    if (r6 != 0)
        goto loop_body;

    /* ---------------------------------------------------------------
     * Phase 2: check if we need to draw the species highlight icon.
     * Conditions:
     *   (a) character count r31 < entry[1] for this slot
     *   (b) self+0x98 == 0 (not suppressed)
     * --------------------------------------------------------------- */
    r0 = (s32)( *(u32*)((u8*)r30 + 0x1c) ) << 4;
    r0 = (s32)*(u32*)(r27 + (u32)r0);   /* re-read entry[1] */

    if (r31 >= r0)
        goto done_ff;
    if ((u8)r25[0x98] != 0)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 3: resolve species/variant entry id (r23).
     * Uses three pointer-indirected values from ctx+0x24/0x28/0x2c.
     * val_a = **(u32**)(ctx+0x24)   must be [0,2)
     * r25   = **(u32**)(ctx+0x28)   used as index later
     * val_c = **(u32**)(ctx+0x2c)   must be [0,4)
     * species_id = lbl_80266E18[val_a*0x18 + val_c*4 + 8] as u32
     * --------------------------------------------------------------- */
    {
        u32* ptr_a  = *(u32**)((u8*)r30 + 0x24);
        u32* ptr_b  = *(u32**)((u8*)r30 + 0x28);
        u32* ptr_c  = *(u32**)((u8*)r30 + 0x2c);
        s32  val_a  = (s32)*ptr_a;
        s32  val_b  = (s32)*ptr_b;
        s32  val_c  = (s32)*ptr_c;
        s32  count;
        void* arr;

        /* r25 is repurposed to hold val_b from here */
        r25 = (u8*)(u32)val_b;

        if (val_a < 0 || val_a >= 2) {
            r23 = 0;
            goto after_species;
        }
        if (val_c < 0 || val_c >= 4) {
            r23 = 0;
            goto after_species;
        }

        /*
         * Index into lbl_80266E18:
         *   row = lbl_80266E18 + val_a * 0x18
         *   element = *(u32*)(row + val_c*4 + 8)
         */
        r23 = *(u32*)(lbl_80266E18 + (u32)val_a * 0x18 + (u32)val_c * 4 + 8);

        /* bounds-check val_b against the array length */
        count = GSmsgGetLength((void*)r23);
        if (val_b < 0 || val_b >= count) {
            r23 = 0;
            goto after_species;
        }

        /* r23 = array[val_b] as u16 */
        arr  = GSmsgGetGSchar(r23);
        r23  = (u32)*(u16*)((u8*)arr + (u32)(u32)val_b * 2);
    }

after_species:
    /* Skip if species id resolved to 0 */
    if ((u16)r23 == 0)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 4: compare against "currently displayed" species id.
     * GSmsgGetGSchar(0x2efc) → array, lhz [0] = active species u16.
     * If r23 matches → r0=0 (same species); else r0=6 (different).
     * Only draw when r0==6 (NOT the active species).
     * --------------------------------------------------------------- */
    {
        void* cur_arr = GSmsgGetGSchar(0x2efc);
        u16   cur_id  = *(u16*)((u8*)cur_arr);
        if ((u16)r23 == cur_id)
            r0 = 0;
        else
            r0 = 6;
    }

    if (r0 != 6)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 5: compute the animated x position and draw the icon.
     *
     * float at *(f32*)(ctx+0x30):  animation phase
     * packed color: (u8)(lbl_8047B938 * (lbl_8047B934 - phase)) | 0x00FF0000
     * The u16 species id r23 goes into text_buf[0]; text_buf[1] = 0.
     * x = r31 * 0x1a + floor((0x1b - half_width) / 2)
     * --------------------------------------------------------------- */
    {
        f32* fptr = *(f32**)((u8*)r30 + 0x30);
        f32  f0, f1, f2;
        u32  int_val;

        f1 = lbl_8047B934;
        f0 = *fptr;
        f2 = lbl_8047B938;

        f0 = f2 * (f1 - f0);           /* fsubs then fmuls */

        /* fctiwz → store as f64, pick up integer from low word (big-endian +4) */
        fconv.d = (f64)f0;              /* fctiwz truncation: use (s32) cast    */
        int_val = (u32)(s32)f0;         /* ENDIAN-QA: stfd/lwz+4 = lower 32 bits */

        /* clrlwi 24 = mask to 8 bits */
        r0  = (s32)((u8)int_val);
        /* oris r23, r0, 0xff: r23 = r0 | 0x00FF0000 */
        r23 = (u32)r0 | 0x00FF0000u;

        /* set up text buffer with species id */
        text_buf[0] = (u16)(r23 & 0xFFFF);  /* sth r23, 0x8(r1) — only low 16 written */
        text_buf[1] = 0;                      /* sth r0=0, 0xa(r1)                      */

        msgctrlSetValue(0x37, r28);

        /* half-width centering */
        r0 = (s32)(s16)(u16)(GSmsgGetRect(0xce) >> 16);
        r0 = 0x1b - r0;
        r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;

        /* x = r31 * 0x1a + centered_offset */
        r0 = (s32)r31 * 0x1a + r0;

        fn_800FB680(r0, 0, (s32)r23, 0xce);
    }

done_ff:
    r26[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026FEC - 0x80026FEC | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern u8 lbl_802EF0A8[];
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_80026FEC(void) {
#include "src/game/gs_worldmap_fn_80026FEC.inc"
}
#else
/*
 * fn_80026FEC  GSmap_DrawRoute0  (0x80026FEC, size 0x190)
 *
 * Callback for drawing a world-map route segment of type 0.
 *
 * r3  = callback-object ptr  (ctx ptr at +0x60)
 * r4  = node/sprite struct   (s16 key at +0x6; output s16s at +0x50/+0x52/+0x54/+0x56;
 *                             output u8 alpha at +0x67)
 *
 * The function selects a u16 discriminant from a two-element table built from
 * lbl_8047B928/lbl_8047B92C using indices derived from two s32 fields of the
 * context.  If the discriminant matches node->field_06 the route vertex data
 * is unpacked, transformed with a scale-factor + offset, and written back.
 * Otherwise the node's alpha byte is zeroed.
 *
 * Big-endian s16->f32 trick normalised to plain casts. ENDIAN-QA on all four
 * entry fields read via lha + xoris+0x8000 sequences.
 */
s32 fn_80026FEC(void* r3, u8* r4)
{
    /* ---- SDA globals (block-scope externs, TU convention) ---- */
    extern u32 lbl_8047B928;        /* u16 table slot 0+1 packed as u32 */
    extern u32 lbl_8047B92C;        /* u16 table slot 2+3 packed as u32 */
    extern u8  lbl_802EF0A8[];      /* route-vertex data array, stride 0x1c */
    extern f64 lbl_8047B948;        /* 0x4330000080000000 s16->f64 bias constant */
    extern f32 lbl_8047B93C;        /* route scale coefficient A */
    extern f32 lbl_8047B940;        /* route scale coefficient B */
    extern f32 lbl_8047B934;        /* map half-width / max coordinate */
    extern f32 lbl_8047B938;        /* alpha scale factor */

    /* ---- locals ---- */
    u16   sp_table[4];   /* two SDA u32s overlay a 4-element u16 lookup table */
    u16   sel;
    void* ctx;
    s32   val_a;         /* s32 from *(u32**)(ctx+0x28)[0] */
    s32   val_b;         /* s32 from *(u32**)(ctx+0x2c)[0] */
    u8*   entry;         /* pointer into lbl_802EF0A8 at stride 0x1c */
    f32*  scale_ptr;     /* *(f32**)(ctx+0x30), game-state scale float */
    f32   f10;           /* runtime scale = *scale_ptr */
    f32   e2, e4, e6, e8;/* s16 fields from route entry at +0x2, +0x4, +0x6, +0x8 */
    f32   f2, f7, f6, f1, f0; /* intermediate FP */
    f32   f3, f5;
    f32   alpha_f;
    s32   x0, y0, x1, y1;
    s32   alpha_int;
    s16   cmp_val;

    /*
     * Build the 4-entry u16 lookup table from the two SDA u32 words.
     * On big-endian PPC a 32-bit store places the high half at the lower address,
     * so slot 0 = lbl_8047B928 >> 16, slot 1 = lbl_8047B928 & 0xffff, etc.
     * On the x86 host we replicate the same layout explicitly. ENDIAN-QA
     */
    sp_table[0] = (u16)(lbl_8047B928 >> 16); /* ENDIAN-QA */
    sp_table[1] = (u16)(lbl_8047B928 & 0xffff);
    sp_table[2] = (u16)(lbl_8047B92C >> 16);
    sp_table[3] = (u16)(lbl_8047B92C & 0xffff);

    /* Unpack context */
    ctx    = *(void**)((u8*)r3 + 0x60);
    val_a  = *(s32*)(*(u8**)((u8*)ctx + 0x28)); /* *ptr_A */
    val_b  = *(s32*)(*(u8**)((u8*)ctx + 0x2c)); /* *ptr_B */

    /* Select discriminant u16 from the table.
     * Original condition: val_a >= 0xf means the outer check passes.
     * Then val_b must be in [0,3] to index the table; otherwise 0xFFFF sentinel. */
    if (val_a < 0xf) {
        sel = 0xffff;
    } else if (val_b < 0 || val_b >= 4) {
        sel = 0xffff;
    } else {
        sel = sp_table[val_b]; /* lhzx: zero-extended u16 */
    }

    /* Compare against the node's s16 key field (+0x6).
     * lha sign-extends; clrlwi zero-extends sel: comparison is unsigned u16 vs u16. */
    cmp_val = *(s16*)(r4 + 0x6);
    if ((u16)sel != (u16)(u32)cmp_val) {
        /* No match: mark node invisible */
        r4[0x67] = 0;
        return 0;
    }

    /* ---- Matching path: compute and write route vertex data ---- */

    /* Route entry in the vertex array; entry index = cmp_val, stride = 0x1c */
    entry = lbl_802EF0A8 + (s32)cmp_val * 0x1c;

    /* Runtime map scale from the context's float pointer */
    scale_ptr = *(f32**)((u8*)ctx + 0x30);
    f10       = *scale_ptr;          /* lfs f10, 0x0(r5) */

    /*
     * Unpack four s16 fields from the route entry via the big-endian
     * xoris+0x8000 / stfd / lfd / fsubs trick, normalised to plain casts.
     * ENDIAN-QA: the game reads native big-endian s16 fields with lha.
     */
    e6 = (f32)(s32)*(s16*)(entry + 0x6); /* ENDIAN-QA */
    e8 = (f32)(s32)*(s16*)(entry + 0x8); /* ENDIAN-QA */
    e2 = (f32)(s32)*(s16*)(entry + 0x2); /* ENDIAN-QA */
    e4 = (f32)(s32)*(s16*)(entry + 0x4); /* ENDIAN-QA */

    /*
     * Scale pass:
     *   f2 = e6 * f10
     *   f1_tmp = e8 * f10
     *   f6 = lbl_8047B93C * f2
     *   f7 = lbl_8047B93C * f1_tmp
     */
    f2  = e6 * f10;
    f1  = e8 * f10;
    f6  = lbl_8047B93C * f2;
    f7  = lbl_8047B93C * f1;

    /*
     * Compute output coordinates:
     *   x1 = e6 + lbl_8047B93C * e6 * f10
     *      = fadds(f9, f6)   where f9 = e6   [f1 register in asm]
     *   x0 = e2 - lbl_8047B93C * lbl_8047B940 * e6 * f10
     *      = fnmsubs(f6, lbl_8047B940, e2)   [f2 register in asm]
     *   y1 = e8 + lbl_8047B93C * e8 * f10
     *      = fadds(f8, f7)                   [f0 register in asm]
     *   y0 = e4 - lbl_8047B93C * lbl_8047B940 * e8 * f10
     *      = fnmsubs(f7, lbl_8047B940, e4)   [second f2 in asm]
     */
    x1 = (s32)(e6  + f6);
    f0 = e2  - f6  * lbl_8047B940;   /* x0 float */
    x0 = (s32)f0;
    y1 = (s32)(e8  + f7);
    f3 = e4  - f7  * lbl_8047B940;   /* y0 float */
    y0 = (s32)f3;

    /*
     * Alpha scalar:
     *   f5 = lbl_8047B934 - f10           (fsubs f5, f3, f10 in asm, where f3=lbl_8047B934)
     *   alpha = lbl_8047B938 * f5          (fmuls f3, f6, f5 in asm, where f6=lbl_8047B938)
     *   fctiwz -> truncate to int
     */
    f5       = lbl_8047B934 - f10;
    alpha_f  = lbl_8047B938 * f5;
    alpha_int = (s32)alpha_f;         /* fctiwz */

    /* Write vertex outputs */
    *(s16*)(r4 + 0x50) = (s16)x0;
    *(s16*)(r4 + 0x52) = (s16)y0;
    *(s16*)(r4 + 0x54) = (s16)x1;
    *(s16*)(r4 + 0x56) = (s16)y1;

    /* Write alpha byte (lower byte of the integer conversion) */
    r4[0x67] = (u8)(u32)alpha_int;

    return 0;
}
#endif

/* fn_8002717C - 0x8002717C | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_8002717C(void) {
#include "src/game/gs_worldmap_fn_8002717C.inc"
}
#else
/*
 * GSmap_DrawRoute1  (0x8002717C, 0x190 bytes)
 * Checks whether a world-map route entry matches the sprite's expected route ID,
 * then projects the route's four s16 data fields into screen coordinates stored
 * at r4[0x50..0x57].  r4[0x67] receives the projected "alpha/depth" byte or 0.
 *
 * Parameters:
 *   r3 = self / GSmap object (void*)
 *   r4 = sprite / output record (u8*)
 *
 * Big-endian note: lbl_8047B928 and lbl_8047B92C are each a u32 that the PPC
 * code treats as two packed big-endian u16 values on the stack.  On the x86
 * host we reconstruct the same four u16 slots via explicit bit-shifts.
 * Marked ENDIAN-QA below.
 */
s32 fn_8002717C(void* r3, u8* r4)
{
    extern u32 lbl_8047B928;   /* route-ID table word 0  (2 packed u16 big-endian) */
    extern u32 lbl_8047B92C;   /* route-ID table word 1  (2 packed u16 big-endian) */
    extern u8  lbl_802EF0A8[]; /* world-map entry table, stride 0x1c */
    extern f64 lbl_8047B948;   /* int->float bias constant (0x4330000080000000) */
    extern f32 lbl_8047B93C;   /* projection constant K1 */
    extern f32 lbl_8047B940;   /* projection constant K2 */
    extern f32 lbl_8047B934;   /* projection constant K3 */
    extern f32 lbl_8047B938;   /* projection constant K4 */

    u8*  ctx;       /* *(u8**)(r3+0x60) */
    u8*  ptr28;     /* *(u8**)(ctx+0x28) */
    u8*  ptr2c;     /* *(u8**)(ctx+0x2c) */
    s32  val28;     /* *(s32*) ptr28      - first  gating value */
    s32  val2c;     /* *(s32*) ptr2c      - route-slot index  */
    u16  sel;       /* selected route-ID from packed table     */
    s32  loc_id;    /* (s16)(r4+0x6)     - sprite's route ID  */
    u8*  entry;     /* &lbl_802EF0A8[loc_id * 0x1c]           */
    f32* scale_ptr; /* *(f32**)(ctx+0x30)                     */
    f32  scale;     /* *scale_ptr                              */
    f32  e2, e4, e6, e8; /* s16 fields at entry+2,+4,+6,+8   */
    f32  f9, f8;         /* e6*scale, e8*scale before K1      */
    f32  f6, f7;         /* K1*f9*scale,  K1*f8*scale         */
    f32  r_x1, r_y1;     /* projected coordinates             */
    f32  r_x2, r_y2;     /* second pair                       */
    f32  f_depth;        /* K4*(K3-scale)                     */
    s32  i_x1, i_y1, i_x2, i_y2, i_depth;

    ctx    = *(u8**)((u8*)r3 + 0x60);
    ptr28  = *(u8**)(ctx + 0x28);
    val28  = *(s32*)ptr28;
    ptr2c  = *(u8**)(ctx + 0x2c);
    val2c  = *(s32*)ptr2c;

    /* Build the 4-slot u16 table from the two packed u32 globals.
     * PPC stores them big-endian on the stack then reads u16 pairs:
     *   slot 0 = high u16 of lbl_8047B928
     *   slot 1 = low  u16 of lbl_8047B928
     *   slot 2 = high u16 of lbl_8047B92C
     *   slot 3 = low  u16 of lbl_8047B92C
     * ENDIAN-QA: extraction is endian-normalised for x86 host. */
    {
        u16 tab[4];
        tab[0] = (u16)(lbl_8047B928 >> 16);
        tab[1] = (u16)(lbl_8047B928 & 0xFFFF);
        tab[2] = (u16)(lbl_8047B92C >> 16);
        tab[3] = (u16)(lbl_8047B92C & 0xFFFF);

        /* Gate 1: val28 must be >= 15 for the route slot to be meaningful */
        if (val28 < 15) {
            sel = 0xFFFF;
        } else if (val2c < 0 || val2c >= 4) {
            sel = 0xFFFF;
        } else {
            sel = tab[val2c];
        }
    }

    loc_id = (s32)(s16)(*(u16*)(r4 + 0x6));

    if ((u16)sel != (u16)loc_id) {
        r4[0x67] = 0;
        return 0;
    }

    /* Locate the entry record */
    entry     = lbl_802EF0A8 + (u32)((s32)loc_id * 0x1c);
    scale_ptr = *(f32**)(ctx + 0x30);
    scale     = *scale_ptr;

    /* Convert four s16 fields to float (ENDIAN-QA: xoris/lfd trick -> plain cast) */
    e6 = (f32)(s32)(s16)*(s16*)(entry + 0x6); /* ENDIAN-QA */
    e8 = (f32)(s32)(s16)*(s16*)(entry + 0x8); /* ENDIAN-QA */
    e2 = (f32)(s32)(s16)*(s16*)(entry + 0x2); /* ENDIAN-QA */
    e4 = (f32)(s32)(s16)*(s16*)(entry + 0x4); /* ENDIAN-QA */

    /* f9 = e6 * scale, f8 = e8 * scale */
    f9 = e6 * scale;
    f8 = e8 * scale;

    /* f6 = K1 * f9,  f7 = K1 * f8 */
    f6 = lbl_8047B93C * f9;
    f7 = lbl_8047B93C * f8;

    /* Projected coordinates (fnmsubs = -(a*b) + c) */
    r_x1  = e6  + f6;                              /* fadds f1 = f9 + f6 */
    r_x2  = e2  - f6 * lbl_8047B940;               /* fnmsubs f2 = -(f6*K2) + e2 */
    r_y2  = e4  - f7 * lbl_8047B940;               /* fnmsubs f2 = -(f7*K2) + e4 */
    r_y1  = e8  + f7;                               /* fadds f0 = f8 + f7 */
    f_depth = lbl_8047B938 * (lbl_8047B934 - scale); /* fmuls f3 = K4*(K3-scale) */

    /* Truncate to integer */
    i_x2    = (s32)r_x2;   /* fctiwz -> truncate toward zero */
    i_x1    = (s32)r_x1;
    i_y2    = (s32)r_y2;
    i_y1    = (s32)r_y1;
    i_depth = (s32)f_depth;

    /* Store four s16 screen coordinates to sprite record */
    *(s16*)(r4 + 0x50) = (s16)i_x2;
    *(s16*)(r4 + 0x52) = (s16)i_y2;
    *(s16*)(r4 + 0x54) = (s16)i_x1;
    *(s16*)(r4 + 0x56) = (s16)i_y1;

    /* Store depth/alpha byte */
    r4[0x67] = (u8)(s32)i_depth;

    return 0;
}
#endif

/* fn_8002730C - 0x8002730C | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 0
asm void fn_8002730C(void) {
#include "src/game/gs_worldmap_fn_8002730C.inc"
}
#else
/*
 * GSmap_DrawRoute2 -- Draw route line type 2
 * 0x8002730C | size: 0x190
 *
 * Args:  r3 = self/callback context pointer
 *        r4 = render packet / output buffer (u8*)
 *
 * The function selects a route entry from lbl_802EF0A8[] (stride 0x1c)
 * by matching the packet's route ID field (r4[6..7] as s16) against a
 * 4-entry u16 lookup table packed from two SDA globals.  On a match it
 * projects four s16 coordinate fields through a scale + affine transform
 * and writes the results as four s16 pairs into r4[0x50..0x57], then
 * stores a derived byte into r4[0x67].  Always returns 0.
 *
 * Big-endian int->float idiom (0x43300000 / xoris / lfd / fsubs) is
 * rewritten as plain (f32)(s32) casts -- ENDIAN-QA
 */
s32 fn_8002730C(void* r3, u8* r4)
{
    /* SDA globals -- block-scope as per TU convention */
    extern u32 lbl_8047B928;   /* packed u16[2]: table entries [0],[1] */
    extern u32 lbl_8047B92C;   /* packed u16[2]: table entries [2],[3] */
    extern f64 lbl_8047B948;   /* int->float magic const (not used after rewrite) */
    extern f32 lbl_8047B93C;   /* affine scale coefficient A */
    extern f32 lbl_8047B940;   /* affine scale coefficient B */
    extern f32 lbl_8047B934;   /* base value for derived byte computation */
    extern f32 lbl_8047B938;   /* multiplier for derived byte computation */
    /* Route data table: entries are 0x1c (28) bytes each */
    extern u8 lbl_802EF0A8[];

    u16 route_table[4];
    u32 w0;
    u32 w1;
    void* ctx;
    u32* ptr_a;
    u32 val_a;
    u32* ptr_b;
    u32 val_b;
    u16 table_val;
    s16 route_id;
    u8* entry;
    f32 e2;
    f32 e4;
    f32 e6;
    f32 e8;
    f32 scale;
    f32 sx1;
    f32 sx2;
    f32 coeff_a;
    f32 coeff_b;
    f32 fx6;
    f32 fx7;
    f32 proj_x0;
    f32 proj_y0;
    f32 proj_x1;
    f32 proj_y1;
    f32 base_val;
    f32 mul_val;
    f32 derived_f;

    /* Stack-local u16 lookup table packed from two SDA u32 globals.
     * On big-endian PPC: lbl_8047B928 = { u16[0], u16[1] }
     *                    lbl_8047B92C = { u16[2], u16[3] }
     * On the x86 host those u32s are stored LE, so the packed bytes
     * are byte-swapped relative to PPC.  The original code reads them
     * with lhzx on the raw stack words -- which works on PPC because
     * it accesses the big-endian bytes.  On x86 we must byte-swap
     * each u16 when building the table.  *  ENDIAN-QA * 
     * We expose the swap here so host callers see the same logical
     * values the PPC code intended.
     */
    w0 = lbl_8047B928;
    w1 = lbl_8047B92C;
    /* PPC big-endian packing: high halfword = entry[0], low = entry[1] */
    route_table[0] = (u16)(w0 >> 16);
    route_table[1] = (u16)(w0 & 0xFFFF);
    route_table[2] = (u16)(w1 >> 16);
    route_table[3] = (u16)(w1 & 0xFFFF);

    ctx = *(void**)((u8*)r3 + 0x60);
    ptr_a = *(u32**)((u8*)ctx + 0x28);
    val_a = ptr_a[0];           /* deref of ctx->ptr_0x28 */
    ptr_b = *(u32**)((u8*)ctx + 0x2c);
    val_b = ptr_b[0];           /* deref of ctx->ptr_0x2c -- route slot index */

    /* Determine which route ID to match against r4[6..7].
     * If val_a < 15 the slot is considered invalid -> sentinel 0xFFFF.
     * Otherwise use val_b as an index into route_table[0..3];
     * any out-of-range val_b also yields 0xFFFF. */
    if ((s32)val_a < 0xf) {
        table_val = 0xFFFF;
    } else if ((s32)val_b < 0 || (s32)val_b >= 4) {
        table_val = 0xFFFF;
    } else {
        table_val = route_table[val_b];
    }

    /* r4[6..7] holds the route ID as a signed 16-bit integer.
     * The PPC code compares the zero-extended table_val (clrlwi, 16)
     * against the sign-extended field value (lha) as a full word.
     * For a valid table entry (0x0000..0xFFFE) this only matches a
     * non-negative s16; 0xFFFF never matches a valid s16 (it would
     * equal (s32)-1 as s16, but the sentinel is treated as u16=65535
     * and the s16 field is sign-extended -- so cmpw compares 65535 vs
     * a sign-extended value, effectively always a mismatch for 0xFFFF). */
    route_id = *(s16*)(r4 + 0x6);

    if ((s32)(u32)table_val != (s32)route_id) {
        /* mismatch */
        r4[0x67] = 0;
        return 0;
    }

    /* Matched: compute projected coordinates for this route entry. */

    /* Route table entry base */
    entry = lbl_802EF0A8 + (s32)route_id * 0x1c;

    /* s16 coordinate fields from the route entry -- ENDIAN-QA (big-endian s16 on PPC) */
    e2 = (f32)(s16)(*(s16*)(entry + 0x2));   /* entry->x0 */
    e4 = (f32)(s16)(*(s16*)(entry + 0x4));   /* entry->y0 */
    e6 = (f32)(s16)(*(s16*)(entry + 0x6));   /* entry->x1 */
    e8 = (f32)(s16)(*(s16*)(entry + 0x8));   /* entry->y1 */

    /* Scale factor: a float pointer stored at ctx+0x30 */
    scale = *(f32*)(*(u8**)((u8*)ctx + 0x30));   /* **(f32**)(ctx+0x30) */

    /* Intermediate scaled values */
    sx1 = e6 * scale;   /* field_6 * scale */
    sx2 = e8 * scale;   /* field_8 * scale */

    coeff_a = lbl_8047B93C;
    coeff_b = lbl_8047B940;

    fx6 = coeff_a * sx1;   /* A * (field_6 * scale) */
    fx7 = coeff_a * sx2;   /* A * (field_8 * scale) */

    /* Affine projected coordinates (fnmsubs = -(a*b) + c = c - a*b) */
    proj_x0 = e2  - fx6 * coeff_b;          /* field_2  - A*sx1*B */
    proj_y0 = e4  - fx7 * coeff_b;          /* field_4  - A*sx2*B */
    proj_x1 = e6  + fx6;                    /* field_6  + A*sx1  = field_6*(1+A*scale) */
    proj_y1 = e8  + fx7;                    /* field_8  + A*sx2  = field_8*(1+A*scale) */

    /* Derived byte value */
    base_val = lbl_8047B934;
    mul_val  = lbl_8047B938;
    derived_f = mul_val * (base_val - scale);

    /* Write projected coordinates as s16 pairs into the output packet.
     * fctiwz = round-toward-zero (truncate) then take low 32 bits as s32. */
    *(s16*)(r4 + 0x50) = (s16)(s32)proj_x0;
    *(s16*)(r4 + 0x52) = (s16)(s32)proj_y0;
    *(s16*)(r4 + 0x54) = (s16)(s32)proj_x1;
    *(s16*)(r4 + 0x56) = (s16)(s32)proj_y1;

    /* r4[0x67] = low byte of truncated derived float */
    r4[0x67] = (u8)(s32)derived_f;

    return 0;
}
#endif

/* menuNameEntryDraw50Cursor - 0x8002749C | size: 0x158 */
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B940;
extern f32 lbl_8047B938;
#if 0
asm void menuNameEntryDraw50Cursor(void) {
#include "src/game/gs_worldmap_fn_8002749C.inc"
}
#else
s32 menuNameEntryDraw50Cursor(void *self, u8 *r4)
{
    extern u8 lbl_802EF0A8[];
    u8* context;
    u8* entry;
    s32 tileId;
    s32 tileRow;
    f32 x;
    f32 y;
    f32 scale;
    f32 scaledX;
    f32 scaledY;
    s8 columnCount;

    context = *(u8**)((u8*)self + 0x60);
    entry = lbl_802EF0A8 + *(s16*)(r4 + 6) * 0x1C;
    tileId = **(s32**)(context + 0x28);
    x = (f32)*(s16*)(entry + 6);
    scale = **(f32**)(context + 0x30);
    y = (f32)*(s16*)(entry + 8);
    scaledX = lbl_8047B93C * (x * scale);
    scaledY = lbl_8047B93C * (y * scale);

    if (tileId < 0xF) {
        tileRow = **(s32**)(context + 0x2C);
        columnCount = (s8)(lbl_8047B938 * (lbl_8047B934 - scale));
        *(s16*)(r4 + 0x50) =
            (s16)((f32)(**(s32**)(context + 0x38) + tileId * 0x1B) -
                  scaledX * lbl_8047B940);
        *(s16*)(r4 + 0x52) =
            (s16)((f32)(**(s32**)(context + 0x3C) + tileRow * 0x23) -
                  scaledY * lbl_8047B940);
        *(s16*)(r4 + 0x54) = (s16)(x + scaledX);
        *(s16*)(r4 + 0x56) = (s16)(y + scaledY);
    } else {
        columnCount = 0;
    }
    r4[0x67] = columnCount;
    return 0;
}
#endif

/* menuNameEntryDraw50Text - 0x800275F4 | size: 0x14c */
#if 0
asm void menuNameEntryDraw50Text(void) {
#include "src/game/gs_worldmap_fn_800275F4.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryDraw50Text(void* r3) {
    s32 r31;     /* x_acc */
    s32 r30;     /* byte_off */
    u8* r29;     /* inner_ptr */
    s32 r28;     /* y_acc */
    u8* r27;     /* base_ptr */
    u16* r26;    /* pbuf */
    s32 r25;     /* item */
    s32 r24;     /* outer_ctr */
    s32 r23;     /* loc_idx */
    void* r22;   /* self */
    void* r21;   /* entry_ptr */
    u8* ctx;
    u8* arr;
    u16 buf[2];
    u16 r6;
    s32 r0;
    r22 = r3;
    ctx = *(u8**)((u8*)r22 + 0x60);
    r23 = *(s32*)(*(u8**)((u8*)ctx + 0x24));
    r24 = 0;
    r28 = 0;
    r27 = lbl_80266E18 + r23 * 0x18;
    r26 = buf;
    while (r24 < 4) {
        r31 = 0;
        r29 = r27 + 8;
        r30 = 0;
        r25 = 0;
        r6 = 0;
        goto inner_check;
        while (r6 != 0) {
            r26[0] = r6; r26[1] = 0;
            msgctrlSetValue(0x37, r26);
            r0 = (s32)(s16)(u16)((u32)GSmsgGetRect(0xce) >> 16);
            r0 = (0x1b - r0);
            r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;
            fn_800FB680(r31 + r0, r28, (s32)((u8*)r22)[0x8b] | (s32)(-0x100), 0xce);
            r31 += 0x1b;
            r30 += 2;
            r25++;
        inner_check:
            if (r23 < 0 || r23 >= 2) { r6 = 0; continue; }
            if (r24 < 0 || r24 >= 4) { r6 = 0; continue; }
            r21 = *(void**)r29;
            r0 = GSmsgGetLength(r21);
            if (r25 < 0 || r25 >= r0) { r6 = 0; continue; }
            arr = (u8*)GSmsgGetGSchar((u32)r21);
            r6 = *(u16*)(arr + r30);
        }
        r28 += 0x23;
        r27 += 4;
        r24++;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80027740 - 0x80027740 | size: 0x3c */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_80027740(void* r3) {
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    fn_800FB680(0, 0, alpha | 0x509100, 0x2ef5);
    return 0;
}
#pragma pop

/* fn_8002777C - 0x8002777C | size: 0x3c */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_8002777C(void* r3) {
    s32 mask = -0x100;
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    fn_800FB680(0, 0, alpha | mask, 0x2ef3);
    return 0;
}
#pragma pop

/* fn_800277B8 - 0x800277B8 | size: 0x3c */
#pragma push
#pragma peephole off
#pragma optimization_level 1
s32 fn_800277B8(void* r3) {
    s32 mask = -0x100;
    u32 alpha = ((MenuNameEntryObject*)r3)->alpha;
    fn_800FB680(0, 0, alpha | mask, 0x2ef4);
    return 0;
}
#pragma pop

/* menuNameEntryDrawXButtonText - 0x800277F4 | size: 0xb0 */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 menuNameEntryDrawXButtonText(void* r3, u8* r4) {
    u8* r29;
    u8* r30;
    u32* r31;
    s32 index;
    u16 width;
    s16 x;
    u8 alpha;
    s32 mask;
    u32 message;

    r29 = r3;
    r30 = r4;
    index = *(s32*)(*(u8**)(*(u8**)(r29 + 0x60) + 0x24));
    index++;
    if (index >= 2) {
        index -= 2;
    }
    if (index >= 0 && index < 2) {
        r31 = (u32*)(lbl_80266E18 + index * 0x18 + 4);
        width = (u16)GSmsgGetRect(*r31);
        x = *(s16*)(r30 + 0x56);
        x -= (s16)width;
        alpha = r29[0x8b];
        mask = -0x100;
        mask |= alpha;
        message = *r31;
        fn_800FB680(0, x, mask, message);
    }
    return 0;
}
#pragma pop

/* menuNameEntryDrawTitle - 0x800278A4 | size: 0xbc */
extern s32 pokemonBiosGetPokemonDataId(void*);
#if 0
asm void menuNameEntryDrawTitle(void) {
#include "src/game/gs_worldmap_fn_800278A4.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 menuNameEntryDrawTitle(void* r3) {
    u8* r29;
    void* r31;
    void* r30;
    s32 r4;
    s32 mask;
    u32 message;
    r29 = (u8*)r3;
    r31 = *(void**)(r29 + 0x60);
    if (*(s32*)r31 == 2) {
        r30 = heroGetStatus(0, 3, (u16)*(u32*)((u8*)r31 + 0x4));
        if (pokemonCheckValid() != 0) {
            r4 = pokemonBiosGetPokemonDataId(r30);
        } else {
            r4 = 1;
        }
        msgctrlSetValue(0x4e, (void*)(u32)(u16)r4);
    }
    message = *(u32*)(lbl_80266DD8 + (*(s32*)r31 << 4));
    mask = -0x100;
    mask |= r29[0x8b];
    fn_800FB680(0, 0, mask, message);
    return 0;
}
#pragma pop
#endif

/* exchangeDakuon__FUs11DAKUON_MODE - 0x80027960 | size: 0x144 */
extern u32 lbl_8047B920;
#if 0
asm void exchangeDakuon__FUs11DAKUON_MODE(void) {
#include "src/game/gs_worldmap_fn_80027960.inc"
}
#else
u16 exchangeDakuon__FUs11DAKUON_MODE(u16 r26, s32 r27) {
    u32* r29;
    u16* r5;
    s32 r28, r30, r31, r4;
    r28 = 0;
    r31 = 0;
    do {
        u32 ptr;
        r29 = (u32*)lbl_8047B920;
        ptr = r29[r28];
        if (ptr == 0) goto next;
        r30 = GSmsgGetLength((void*)ptr);
        r5 = (u16*)((u8*)GSmsgGetGSchar(r29[r28]) + 2);
        r4 = 1;
        if (r30 > 1) {
            s32 ctr = r30 >> 1;
            do {
                if (*r5 == r26) break;
                r4 += 2;
                r5 += 2;
            } while (--ctr);
        }
        if (r4 < r30) goto found;
    next:
        r28++;
        r31 += 4;
    } while (r28 < 2);

found:
    if (r28 >= 2) r28 = 0;
    if (r28 == r27) return r26;
    if (r28 == 1) {
        r26 = *(r5 - 1);
    }
    if (r27 == 0) return r26;
    r30 = r27 << 2;
    r29 = (u32*)lbl_8047B920;
    r31 = GSmsgGetLength((void*)r29[r27]);
    r5 = (u16*)GSmsgGetGSchar(r29[r27]);
    r4 = 0;
    if (r31 > 0) {
        s32 ctr = (r31 + 1) >> 1;
        do {
            if (*r5 == r26) break;
            r4 += 2;
            r5 += 2;
        } while (--ctr);
    }
    if (r4 < r31) return *(r5 + 1);
    return 0;
}
#endif

/* selectLetter__FP14NAME_ENTRY_ARG - 0x80027AA4 | size: 0x2b4 */
extern void fn_80166A28(void);
#if 0
asm void selectLetter__FP14NAME_ENTRY_ARG(void) {
#include "src/game/gs_worldmap_selectLetter__FP14NAME_ENTRY_ARG.inc"
}
#else
/*
 * selectLetter__FP14NAME_ENTRY_ARG -- GSmap_TravelAnimation (0x80027AA4, size 0x2B4)
 *
 * World-map travel / UI state machine tick.
 * Reads two indirect sub-state counters from the context object,
 * decides which action to perform (0-6), then optionally fires a
 * scheduler/sound event via fn_80166A28 before returning a flag.
 *
 * param r3  - pointer to the world-map controller object ("self")
 * return    - 1 if a "complete / next-scene" transition was triggered,
 *             0 otherwise
 */
s32 selectLetter__FP14NAME_ENTRY_ARG(void* r3) {
    extern s32  GSmsgGetLength(void*);
    extern void* GSmsgGetGSchar(u32);
    extern u16  exchangeDakuon__FUs11DAKUON_MODE(u16, s32);
    extern void fn_80166A28(u32);
    extern u8   lbl_80266DD8[];   /* array of 16-byte entries: [u32 id, u32 limit, ...] */
    extern u8   lbl_80266E18[];   /* 2D array: [2][0x18], each 0x18 entry has sub-arrays of void* at +8 */

    u8*   self;          /* r28 -- self pointer (reused as byte-offset after case 1/2) */
    u32   r29;           /* event code sent to fn_80166A28 (0 = no call) */
    s32   r30;           /* return value */
    u16   r31;           /* current travel-target ID */
    s32   r27;           /* animation sub-counter (from self+0x28 indirect) */
    s32   r5;            /* state phase counter (from self+0x2c indirect) */
    void* ptr_28;        /* *(void**)(self+0x28) */
    void* ptr_2c;        /* *(void**)(self+0x2c) */
    s32   action;        /* the computed action code 0-6 */

    self  = (u8*)r3;
    r30   = 0;
    r29   = 0;

    ptr_28 = *(void**)(self + 0x28);
    ptr_2c = *(void**)(self + 0x2c);
    r27 = *(s32*)((u8*)ptr_28 + 0x0);   /* animation sub-counter */
    r5  = *(s32*)((u8*)ptr_2c + 0x0);   /* state phase */

    /* ---------------------------------------------------------------
     * Determine action code
     * --------------------------------------------------------------- */
    if (r27 < 0xf) {
        /* Look up the travel-target ID from a 2-D table indexed by
         * sub-state (0-1) and phase (0-3). */
        s32   sub_state;  /* 0 or 1 -- from self+0x24 indirect */
        s32   phase;      /* 0..3   -- r5 clamped */
        void* entry_ptr;
        void* list_obj;
        s32   list_count;
        void* list_data;
        u16   looked_up;
        u16   current;

        sub_state = *(s32*)(*(u8**)(self + 0x24));
        /* out-of-range sub_state → return 0 */
        if (sub_state < 0 || sub_state >= 2)
            return 0;
        phase = r5;
        /* out-of-range phase → return 0 */
        if (phase < 0 || phase >= 4)
            return 0;

        /* table entry at lbl_80266E18[sub_state * 0x18 + phase * 4]:
         * the pointer-to-list-object lives at +8 within the entry */
        entry_ptr = (void*)(lbl_80266E18 + (s32)(sub_state * 0x18) + (s32)(phase * 4));
        list_obj  = *(void**)((u8*)entry_ptr + 0x8);

        /* validate r27 (the animation counter) as index into the list */
        list_count = GSmsgGetLength(list_obj);
        if (r27 < 0 || r27 >= list_count)
            return 0;

        list_data = GSmsgGetGSchar((u32)list_obj);
        looked_up = *(u16*)((u8*)list_data + (u32)r27 * 2);
        r31 = looked_up;

        /* if the looked-up ID is zero, use a hard-coded default */
        if ((u16)r31 == 0) {
            void* def_ptr = GSmsgGetGSchar(0x2ef9u);
            r31 = *(u16*)def_ptr;
        }

        /* compare against the "current" reference value */
        current = *(u16*)GSmsgGetGSchar(0x2efcu);
        if ((u16)r31 == current)
            action = 0;
        else
            action = 6;
    } else {
        /* r27 >= 0xf: derive action purely from the phase counter */
        r31 = 0; /* r31 not used by these action codes */
        if (r5 == 3) goto act5;
        if (r5 >= 3) goto act4;
        if (r5 == 0) goto act3;
        goto act4;
act3:
        action = 3;
        goto act_done;
act5:
        action = 5;
        goto act_done;
act4:
        action = 4;
act_done:
        ;
    }

    /* ---------------------------------------------------------------
     * Execute action
     * --------------------------------------------------------------- */
    switch (action) {
    case 1:
    case 2: {
        /* Advance to the previous entry in the travel list (wrap around). */
        u32** count_ptr;
        s32   cur_count;
        s32   idx;
        u16*  array;
        u16   candidate;

        count_ptr = *(u32***)(self + 0x34);
        cur_count = *(s32*)count_ptr;
        idx = cur_count - 1;
        if (idx >= 0) {
            array     = *(u16**)(self + 0x18);
            candidate = array[idx];                     /* lhzx r3, r29, r28 */
            candidate = exchangeDakuon__FUs11DAKUON_MODE(candidate, 1);      /* look up alternate mapping */
            if ((u16)candidate != 0) {
                array[idx] = candidate;
            }
        }
        r29 = 0x24u;
        break;
    }
    case 4: {
        /* Remove the last entry from the travel list. */
        u32** count_ptr;
        s32   cur_count;
        u8    did_remove;

        count_ptr = *(u32***)(self + 0x34);
        cur_count = *(s32*)count_ptr;
        if (cur_count > 0) {
            s32   new_count  = cur_count - 1;
            u16*  array      = *(u16**)(self + 0x18);
            array[new_count] = 0;                       /* zero the slot */
            *(u32*)count_ptr = (u32)new_count;
            did_remove = 1;
        } else {
            did_remove = 0;
        }
        /* clrlwi r0, r0, 24 -- mask to u8 */
        if ((u8)did_remove != 0)
            r29 = 0x25u;
        break;
    }
    case 5:
        /* Signal "next scene / done". */
        r30 = 1;
        break;
    case 3: {
        /* Toggle sub-state (0→1→0). */
        u8**  sub_ptr;
        s32   cur;
        sub_ptr = (u8**)(self + 0x24);
        cur = *(s32*)*sub_ptr;
        cur++;
        if (cur >= 2)
            cur = 0;
        *(s32*)*sub_ptr = cur;
        r29 = 0x27u;
        break;
    }
    case 0: {
        /* action 0: also used as the fall-through entry for case 6.
         * Load the default target ID then fall into the append logic. */
        void* def_ptr = GSmsgGetGSchar(0x2ef9u);
        r31 = *(u16*)def_ptr;
        /* FALL THROUGH */
    }
    /* no break -- falls into case 6 */
    case 6:
    default: {
        /* Append r31 into the travel-list at the current count position,
         * clamping if needed, and advance the count pointer. */
        u32   field_1c;
        u32*  entry;
        u32** count_ptr;
        s32   cur_count;
        s32   insert_idx;
        s32   limit;
        u16*  array;

        field_1c  = *(u32*)(self + 0x1c);
        entry     = (u32*)(lbl_80266DD8 + (s32)(field_1c * 16u));
        count_ptr = *(u32***)(self + 0x34);        /* ptr to the mutable count word */
        cur_count = *(s32*)count_ptr;
        limit     = (s32)entry[1];

        if (cur_count >= limit)
            insert_idx = limit - 1;    /* clamp */
        else
            insert_idx = cur_count;

        array = *(u16**)(self + 0x18);
        array[insert_idx]     = r31;
        array[insert_idx + 1] = 0;

        /* advance count, but not past limit */
        {
            s32 new_count = insert_idx + 1;
            if (new_count >= limit + 1)   /* i.e. new_count > limit */
                new_count = limit;
            *(u32*)count_ptr = (u32)new_count;
        }
        r29 = 0x24u;
        break;
    }
    }

    /* Fire the scheduler/sound event if a code was set. */
    if (r29 != 0)
        fn_80166A28(r29);

    return r30;
}
#endif

/* menuNameEntryCursor - 0x80027D58 | size: 0x3a4 */
extern u16* windowGetKeyInfo(void);
#if 0
asm void menuNameEntryCursor(void) {
#include "src/game/gs_worldmap_fn_80027D58.inc"
}
#else
/* menuNameEntryCursor - GSmap_ArrivalSequence (0x80027D58, 0x3A4 bytes)
 * Overworld map party-slot edit dispatcher. Reads the global input-state
 * object (windowGetKeyInfo), then dispatches on its two bitfields:
 *   state->f4 (u16) bits select one mutually-exclusive action (each returns):
 *     bit 0x40 - cycle a per-slot toggle 0..1 and play SE 0x27
 *     bit 0x10 - commit/leave; if selectLetter__FP14NAME_ENTRY_ARG says "exit" set actor[0x98],
 *                else range-check the route list and set status fields
 *     bit 0x20 - delete the current list entry, play SE 0x25
 *     bit 0x400 - swap the selected slot's species across the two
 *                 reference lists via exchangeDakuon__FUs11DAKUON_MODE, play SE 0x24
 *     bit 0x800 - request exit (set actor[0x98])
 *   state->f6 (u16) bits adjust the two cursor counters (NOT exclusive):
 *     bit 0x8/0x4 - inc/dec the column cursor (ctx->f28), clamp 0..15
 *     bit 0x2/0x1 - inc/dec the row cursor (ctx->f2c), clamp 0..3,
 *                   with a +2/-2 step when the column cursor is at 15
 * Returns 0 in all paths. */
#pragma peephole off
s32 menuNameEntryCursor(void* actor) {
    extern u16* windowGetKeyInfo(void);
    extern void fn_80166A28(s32 se);
    extern s32  selectLetter__FP14NAME_ENTRY_ARG(void* ctx);
    extern u16  exchangeDakuon__FUs11DAKUON_MODE(u16 value, s32 listIndex);
    extern s32  GSmsgGetLength(u32 list);
    extern void* GSmsgGetGSchar(u32 list);
    extern u8 lbl_80266DD8[];  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047B920;  /* canonical; per-site reinterpret cast */

    u16* state;
    u8*  ctx;
    u16  flags4;
    u16  flags6;

    state = windowGetKeyInfo();
    ctx   = *(u8**)((u8*)actor + 0x60);
    flags4 = state[2];              /* *(u16*)(state + 0x4) */

    if (flags4 & 0x40) {
        s32* cell = *(s32**)(ctx + 0x24);
        s32  v = *cell + 1;
        if (v >= 2) v = 0;
        *cell = v;
        fn_80166A28(0x27);
        return 0;
    }

    if (flags4 & 0x10) {
        if (selectLetter__FP14NAME_ENTRY_ARG(ctx) != 0) {
            *(u8*)((u8*)actor + 0x98) = 1;
            return 0;
        }
        {
            u16* list = *(u16**)(ctx + 0x18);
            s32  count = 0;
            s32* entry;
            while (*list != 0) {
                list++;
                count++;
            }
            entry = (s32*)((u8*)((s32*)lbl_80266DD8) + (*(s32*)(ctx + 0x1c) << 4));
            if (count >= entry[1]) {           /* entry+0x4 */
                *(*(s32**)(ctx + 0x28)) = 0xf;
                *(*(s32**)(ctx + 0x2c)) = 3;
            }
        }
        return 0;
    }

    if (flags4 & 0x20) {
        s32  n = *(*(s32**)(ctx + 0x34));
        u8   played = 0;
        if (n > 0) {
            s32  newN = n - 1;
            u16* base = *(u16**)(ctx + 0x18);
            base[newN] = 0;
            *(*(s32**)(ctx + 0x34)) = newN;
            played = 1;
        }
        if (played) {
            fn_80166A28(0x25);
        }
        return 0;
    }

    if (flags4 & 0x400) {
        s32 idx = *(*(s32**)(ctx + 0x34)) - 1;
        if (idx >= 0) {
            u16* slots = *(u16**)(ctx + 0x18);
            u16  cur = slots[idx];
            s32  found = 0;
            s32  li;
            /* find which of the two reference lists contains the current value */
            for (li = 0; li < 2; li++) {
                u32 list = ((u32*)&lbl_8047B920)[li];
                s32 count;
                u16* data;
                s32 r4;
                s32 ctr;
                if (list == 0) {
                    continue;
                }
                count = GSmsgGetLength(list);
                data  = (u16*)((u8*)GSmsgGetGSchar(list) + 2);  /* value half of key/value pairs */
                r4    = 1;
                if (count > 1) {
                    ctr = count >> 1;
                    do {
                        if (*data == cur) {
                            break;
                        }
                        r4   += 2;
                        data += 2;        /* skip to next pair (2 u16 = 4 bytes) */
                    } while (--ctr);
                }
                if (r4 < count) {
                    found = li;
                    break;
                }
            }
            if (found >= 2) {
                found = 0;
            }
            /* cycle the slot's value through the alternate list(s) until a
             * non-zero conversion is produced */
            {
                s32 dst = found;
                for (;;) {
                    u16 converted;
                    dst++;
                    if (dst >= 2) {
                        dst = 0;
                    }
                    converted = exchangeDakuon__FUs11DAKUON_MODE(slots[idx], dst);
                    if ((u16)converted != 0) {
                        slots[idx] = converted;
                        break;
                    }
                }
            }
        }
        fn_80166A28(0x24);
        return 0;
    }

    if (flags4 & 0x800) {
        *(u8*)((u8*)actor + 0x98) = 1;
        return 0;
    }

    /* --- non-exclusive cursor adjustments on state->f6 --- */
    flags6 = state[3];             /* *(u16*)(state + 0x6) */

    if (flags6 & 0x8) {            /* column cursor + */
        s32* colP = *(s32**)(ctx + 0x28);
        s32  col  = *colP + 1;
        if (col >= 0x10) {
            col = 0xf;
        } else {
            fn_80166A28(0x23);
        }
        *colP = col;
    }

    if (flags6 & 0x4) {            /* column cursor - */
        s32* colP = *(s32**)(ctx + 0x28);
        s32  col  = *colP - 1;
        if (col < 0) {
            col = 0;
        } else {
            fn_80166A28(0x23);
        }
        *colP = col;
    }

    if (flags6 & 0x2) {           /* row cursor + (double-step on last column) */
        s32* colP = *(s32**)(ctx + 0x28);
        s32* rowP = *(s32**)(ctx + 0x2c);
        s32  col  = *colP;
        s32  row  = *rowP;
        if (col >= 0xf) {
            if (row == 1) {
                row += 2;
            } else {
                row += 1;
            }
        } else {
            row += 1;
        }
        if (row >= 4) {
            row = 3;
        } else {
            fn_80166A28(0x23);
        }
        *rowP = row;
    }

    if (flags6 & 0x1) {           /* row cursor - (double-step on last column) */
        s32* colP = *(s32**)(ctx + 0x28);
        s32* rowP = *(s32**)(ctx + 0x2c);
        s32  col  = *colP;
        s32  row  = *rowP;
        if (col >= 0xf) {
            if (row == 2) {
                row -= 2;
            } else {
                row -= 1;
            }
        } else {
            row -= 1;
        }
        if (row < 0) {
            row = 0;
        } else {
            fn_80166A28(0x23);
        }
        *rowP = row;
    }

    return 0;
}
#pragma peephole reset
#endif

/* menuNameEntryCtrl - 0x800280FC | size: 0xf4 */
extern void winSeqSetMenu(void*, s32);
extern f32 lbl_8047B930;
extern f32 lbl_8047B950;
extern f32 lbl_8047B934;
#if 0
asm void menuNameEntryCtrl(void) {
#include "src/game/gs_worldmap_fn_800280FC.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryCtrl(void* r3) {
    u8* r30;
    u8* r31;
    f32* fptr;
    f32 f0;
    f32 f1;
    f32 f2;
    s32 state;
    s32 flag;
    u8 one;

    r30 = (u8*)r3;
    state = (s32)(s8)*(volatile u8*)(r30 + 1);
    r31 = *(u8**)(r30 + 0x60);
    switch (state) {
    case 0:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x56);
            one = 1;
            **(f32**)(r31 + 0x30) = lbl_8047B930;
            r30[2] = one;
        }
        break;
    case 2:
        fptr = *(f32**)(r31 + 0x30);
        f0 = lbl_8047B950;
        f2 = *(volatile f32*)fptr;
        f1 = lbl_8047B934;
        f0 = f2 + f0;
        *fptr = f0;
        if (f0 >= f1) {
            fptr = *(f32**)(r31 + 0x30);
            *fptr = *fptr - f1;
        }
        break;
    case 3:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x5a);
            r30[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* menuNameEntryButton - 0x800281F0 | size: 0x4 */
#if 0
asm void menuNameEntryButton(void) {
#include "src/game/gs_worldmap_fn_800281F0.inc"
}
#else
#pragma optimization_level 4
void menuNameEntryButton(void) { }
#endif

/* inputName__FPUsPUsiii - 0x800281F4 | size: 0x250 */
extern void GScharCpy(void*, u8*);
extern void dbgMenuSetEnable(void);
extern void windowGetActiveID(void);
extern void menuOpenCustom(void);
extern void winMsgOpen(void);
extern void fn_8001E074(void);
extern void winMsgClose(s32);
extern void menuClose(void);
extern void menuCloseSync(void);
extern void menuSubOpenYesNo(void);
extern void pcboxSetPokemonBoxName(void);
extern void GScharCmp(void);
extern void menuModelSetMotion(void);
extern void cameraWaitSyncAnime(void);
extern f32 sin(f32);
extern f32 cos(f32);
extern f32 lbl_8047B930;
extern u8 lbl_8047A3D4[4];
extern u8 lbl_8047A3D0[4];
extern u8 lbl_8047A3CC[4];
extern u8 lbl_8047A3C8[4];
extern u8 lbl_8047A3C4[4];
extern u32 lbl_8047A3C0;
extern u32 lbl_8047A3BC;
extern u32 lbl_8047A3B8;
extern u32 lbl_8047A3B4;
extern u32 lbl_8047A3B0;
extern u8 lbl_803A2068[];
#if 0
asm void inputName__FPUsPUsiii(void) {
#include "src/game/gs_worldmap_inputName__FPUsPUsiii.inc"
}
#else
/*
 * GSmap_Init (inputName__FPUsPUsiii) -- World-map name-entry dialog.
 *
 * Opens the GS name-input scene (0x6e), lets the player type a name,
 * and returns 1 if a new name was accepted or 0 if the dialog was cancelled.
 *
 * Parameters (derived from CW EABI register use before write):
 *   r3  existing_name  -- pointer to the current UTF-16 name (u16[]), may be empty
 *   r4  name_buf_in    -- source name buffer passed to GScharCpy for the stack copy
 *   r5  arg2           -- extra context ptr stored in the GS name-input descriptor
 *   r6  arg3           -- extra context ptr stored in the GS name-input descriptor
 *   r7  allow_cancel   -- if 0, YES/CANCEL both exit without saving; if non-zero,
 *                         player must explicitly answer YES to save
 *
 * Returns 1 if the player accepted a new name, 0 otherwise.
 */
s32 inputName__FPUsPUsiii(u16 *existing_name, u8 *name_buf_in, void *arg2, void *arg3, s32 allow_cancel)
{
    /* --- block-scope extern declarations (TU convention) --- */
    extern u8  *GScharCpy(u8 *dst, u8 *src);        /* GS string copy                  */
    extern void dbgMenuSetEnable(s32 mode);                 /* set VSync mode (0=off, 1=on)    */
    extern u32  windowGetActiveID(void);                     /* get current scene handle         */
    extern void menuOpenCustom(s32 sceneId, u32 handle, s32 a, s32 b, s32 c, s32 d, ...); /* open GS scene with descriptor */
    extern void fn_80166A28(u32 arg);                  /* audio/effect trigger             */
    extern void msgctrlSetValue(s32 effect, void *param);  /* UI effect dispatcher             */
    extern void winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4); /* open dialog message */
    extern s8 menuSubOpenYesNo(s32 max, s32 a, s32 b, s32 initial); /* blocking yes/no picker */
    extern void winMsgClose(s32 slot);                 /* close dialog slot               */
    extern void menuClose(s32 sceneId);              /* close GS scene                  */
    extern void menuCloseSync(s32 sceneId, s32 flag);  /* sync-close menu                 */

    /* lbl_* globals accessed in this function */
    extern f32  lbl_8047B930;       /* float constant 0.0f (stfs source)            */
    extern u8 lbl_8047A3D4[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3D0[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3CC[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3C8[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3C4[4];  /* canonical; per-site reinterpret cast */
    extern u32  lbl_8047A3C0;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3BC;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B8;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B4;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B0;       /* GS name-input descriptor field               */
    extern u8   lbl_803A2068[];     /* committed name output buffer                 */

    /*
     * Stack-local name-input descriptor block.
     * sp+0x08 : name buffer (GS Unicode string, dest of GScharCpy)
     * sp+0x20 : saved existing_name ptr
     * sp+0x24 : saved arg2
     * sp+0x28 : saved arg3
     * sp+0x2c..0x50 : addresses of the nine lbl_8047A3xx globals (pointer table)
     *
     * We model sp+0x08 as a local u8 array large enough for a GS name
     * (0x20 - 0x08 = 0x18 bytes precede the saved-pointer area, but the
     * GS name buffer conventionally sits at this slot and may be longer;
     * 0x58 bytes = 0x60 save-area start - 0x08 buf start).
     */
    u8   name_buf[0x58];      /* sp+0x08: working name buffer                     */
    u16 *name_ptr;            /* sp+0x20 shadow: pointer to existing_name         */
    /* sp+0x24/0x28: arg2/arg3 -- accessed only through the descriptor above      */

    s32  done;                /* r29: loop-exit flag                              */
    s32  confirmed;           /* r30: 1 = player typed YES, 0 = NO/cancel        */
    u16 *scan;                /* r3/r24 reused: scan ptr through existing name    */
    s32  existing_len;        /* r25 reused: UTF-16 code-unit count               */
    u16 *loaded;              /* r3 after GSmsgGetGSchar: resource name ptr          */
    s32  match_count;         /* r5: matched code units                           */
    s32  names_differ;        /* r0: 1 = names differ, 0 = same                  */
    u16 *name_to_use;         /* r31: pointer to the name we will commit          */
    s8   yn_result;           /* r24 reused: yes/no answer                        */
    s32  i;                   /* loop counter                                     */

    extern void *GSmsgGetGSchar(u32 id);   /* resource lookup by ID                  */

    /* -----------------------------------------------------------------
     * Prologue: copy the caller's name into the local stack buffer,
     * zero the name-input descriptor globals, and initialise the
     * pointer table in the stack frame.
     * ----------------------------------------------------------------- */
    GScharCpy(name_buf, name_buf_in);

    /* Zero out the five descriptor globals that are explicitly cleared   */
    (*(u32*)lbl_8047A3D4) = 0;
    (*(u32*)lbl_8047A3D0) = 0;
    (*(u32*)lbl_8047A3CC) = 0;
    (*(f32*)lbl_8047A3C8) = lbl_8047B930;   /* 0.0f */
    (*(u32*)lbl_8047A3C4) = 0;

    /* Null the u16 at existing_name[0] (clear first code unit)           */
    existing_name[0] = 0;

    /* Turn off VSync while the name-input UI is open                     */
    dbgMenuSetEnable(0);

    name_ptr = existing_name;
    done      = 0;
    confirmed = 0;   /* FUNCTIONAL-TODO: r30 is written only later; set 0 for safety */

    /* -----------------------------------------------------------------
     * Main loop: keep the name-input scene open until the player makes
     * a final choice (done != 0).
     * ----------------------------------------------------------------- */
    do {
        /* Open the GS name-input scene (scene ID 0x6e).
         * r9 = name_buf (sp+8) is the 7th integer arg; crxor clears
         * cr1.eq so no float args are signalled.                         */
        menuOpenCustom(0x6e, windowGetActiveID(),
                    0, 0, 1, 1,
                    name_buf);      /* r9 = 7th arg = working name buffer */

        /* ---------------------------------------------------------------
         * Count UTF-16 code units in the original existing_name.
         * existing_name[0] was zeroed above, so if it was already empty
         * this loop exits immediately with existing_len = 0.
         * ------------------------------------------------------------- */
        scan         = name_ptr;  /* reload from sp+0x20 shadow */
        existing_len = 0;
        while (scan[0] != 0) {
            scan++;
            existing_len++;
        }

        /* ---------------------------------------------------------------
         * Determine whether the player entered the same name that was
         * already stored (names_differ = 0 → same, 1 → different).
         * If the original name was empty, treat as "same" (r0=0).
         * ------------------------------------------------------------- */
        if (existing_len == 0) {
            names_differ = 0;
        } else {
            /* Load the resource for ID 0x2ef9 (current box name data).   */
            loaded      = (u16 *)GSmsgGetGSchar(0x2ef9);
            match_count = 0;

            /* Compare up to existing_len code units.                     */
            scan = name_ptr;
            for (i = 0; i < existing_len; i++) {
                if (scan[0] != loaded[0]) {
                    break;
                }
                match_count++;
                scan++;
                /* loaded advances implicitly via bdnz; model as pointer: */
                /* loaded++ -- but in asm loaded (r3) is NOT incremented  */
                /* every iteration, only when bdnz fires.                  */
                /* FUNCTIONAL-TODO: The asm uses r24 for scan and r3 for  */
                /* loaded but only advances r24 on match; r3 seems to be  */
                /* the same resource base re-indexed. Conservative: break  */
                /* on mismatch (already done above) or count matches.     */
            }

            /* If all code units matched, names are the same.             */
            names_differ = (match_count < existing_len) ? 1 : 0;
        }

        /* ---------------------------------------------------------------
         * Select which name pointer to pass to the dialog:
         *   names_differ == 0 → use the newly-entered buffer (sp+0x08)
         *   names_differ != 0 → use the original name (existing_name)
         * ------------------------------------------------------------- */
        if (names_differ != 0) {
            name_to_use = name_ptr;             /* existing name differs: show it */
        } else {
            name_to_use = (u16 *)name_buf;      /* same / new: use entered buf    */
        }

        /* ---------------------------------------------------------------
         * Show the "Are you sure?" dialog (message 0x2ef6).
         * ------------------------------------------------------------- */
        fn_80166A28(0x440);
        msgctrlSetValue(0x4d, name_to_use);
        winMsgOpen(2, 0x2ef6, 1, 0);
        yn_result = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);

        /* ---------------------------------------------------------------
         * Interpret the yes/no answer.
         *   yn_result ==  1 → YES  → confirmed = 0 (use the name)
         *   yn_result == -1 → BACK → confirmed = 0 (treat same as yes)
         *   otherwise       → NO   → confirmed = 1 (do NOT save)
         * ------------------------------------------------------------- */
        if (yn_result == 1 || yn_result == -1) {
            confirmed = 0;
        } else {
            confirmed = 1;
        }

        /* ---------------------------------------------------------------
         * Decide whether to exit the loop.
         *   allow_cancel == 0 : always exit after first answer
         *   allow_cancel != 0 : exit only if player said YES (confirmed=0)
         * ------------------------------------------------------------- */
        if (allow_cancel == 0) {
            done = 1;
        } else if (confirmed == 0) {
            done = 1;
        }
        /* else: confirmed==1 and allow_cancel!=0 → loop again            */

    } while (done == 0);

    /* -----------------------------------------------------------------
     * Cleanup: restore VSync and close the name-input scene.
     * ----------------------------------------------------------------- */
    dbgMenuSetEnable(1);
    menuClose(0x6e);
    menuCloseSync(0x6e, 1);

    /* -----------------------------------------------------------------
     * Commit the result.
     * If the player accepted (confirmed==0), copy name_to_use into the
     * output buffer lbl_803A2068 and return 1.
     * Otherwise return 0.
     * ----------------------------------------------------------------- */
    if (confirmed != 0) {
        return 0;
    }
    GScharCpy(lbl_803A2068, (u8 *)name_to_use);
    return 1;
}
#endif

/* menuNameEntrySelectDrawSelText3 - 0x80028444 | size: 0x50 */
#if 0
asm void menuNameEntrySelectDrawSelText3(void) {
#include "src/game/gs_worldmap_fn_80028444.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 menuNameEntrySelectDrawSelText3(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)((u8*)sub + 0x8);
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* menuNameEntrySelectDrawSelText2 - 0x80028494 | size: 0x50 */
#if 0
asm void menuNameEntrySelectDrawSelText2(void) {
#include "src/game/gs_worldmap_fn_80028494.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 menuNameEntrySelectDrawSelText2(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)((u8*)sub + 0x4);
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* menuNameEntrySelectDrawSelText1 - 0x800284E4 | size: 0x50 */
#if 0
asm void menuNameEntrySelectDrawSelText1(void) {
#include "src/game/gs_worldmap_fn_800284E4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 menuNameEntrySelectDrawSelText1(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)sub;
    msgctrlSetValue(0x37, GSmsgGetGSchar((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* menuNameEntrySelectButton - 0x80028534 | size: 0x54 */
#if 0
asm void menuNameEntrySelectButton(void) {
#include "src/game/gs_worldmap_fn_80028534.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
void menuNameEntrySelectButton(void* r3) {
    u8* r31;
    u16* pad;
    r31 = (u8*)r3;
    pad = windowGetKeyInfo();
    if (!(pad[0] & 0x20)) {
        if (pad[2] & 0x10) {
            r31[0x98] = 1;
        }
    }
}
#endif

/* menuNameEntrySelectCtrl - 0x80028588 | size: 0x98 */
#if 0
asm void menuNameEntrySelectCtrl(void) {
#include "src/game/gs_worldmap_menuNameEntrySelectCtrl.inc"
}
#else
#pragma optimization_level 4
s32 menuNameEntrySelectCtrl(void* r3) {
    u8* r31;
    s8 state;
    r31 = (u8*)r3;
    state = (s8)r31[1];
    switch (state) {
    case 0:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu(*(void**)(r31 + 4), 0x56);
            r31[2] = 1;
        }
        break;
    case 3:
        if ((s8)r31[2] == 0) {
            winSeqSetMenu(*(void**)(r31 + 4), 0x5a);
            r31[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* menuNameEntryBackDrawHumanModel - 0x80028620 | size: 0x108 */
extern void* menuModelRender(void*);
extern void fn_800D888C(s32);
extern void fn_800D88DC(s32);
extern void fn_800D7820(void*);
extern void fn_800D85D4(s32, void*);
extern void fn_800D6A00(s32);
extern void fn_800D67BC(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32, f32, f32);
extern void fn_800D6728(void);
extern u8 lbl_803A2094[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;
#if 0
asm void menuNameEntryBackDrawHumanModel(void) {
#include "src/game/gs_worldmap_fn_80028620.inc"
}
#else
#pragma peephole off
#pragma scheduling on
#pragma optimization_level 4
s32 menuNameEntryBackDrawHumanModel(void* r3, u8* r4) {
    void* r31;
    u8* r30;
    r30 = r4;
    r3 = *(void**)((u8*)r3 + 0x60);
    if (*(s32*)r3 == 2) {
        return 0;
    }
    r31 = menuModelRender(lbl_803A2094);
    if (r31 != (void*)0) {
        fn_800D888C(4);
        fn_800D88DC(3);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, r31);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B930, lbl_8047B930);
        fn_800D61E4((s32)*(s16*)(r30 + 0x54), (s32)*(s16*)(r30 + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B934, lbl_8047B934);
        fn_800D6728();
    }
    return 0;
}
#pragma peephole on
#endif

/* menuNameEntryBackDrawPokemonModel - 0x80028728 | size: 0x108 */
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;
#if 0
asm void menuNameEntryBackDrawPokemonModel(void) {
#include "src/game/gs_worldmap_fn_80028728.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 menuNameEntryBackDrawPokemonModel(void* r3, u8* r4) {
    void* r31;
    u8* r30;
    r30 = r4;
    r3 = *(void**)((u8*)r3 + 0x60);
    if (*(s32*)r3 != 2) {
        return 0;
    }
    r31 = menuModelRender(lbl_803A2094);
    if (r31 != (void*)0) {
        fn_800D888C(4);
        fn_800D88DC(3);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, r31);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B930, lbl_8047B930);
        fn_800D61E4((s32)*(s16*)(r30 + 0x54), (s32)*(s16*)(r30 + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B934, lbl_8047B934);
        fn_800D6728();
    }
    return 0;
}
#pragma peephole on
#endif

/* menuNameEntryBackDrawBall - 0x80028830 | size: 0x118 */
extern void* menuSpriteBiosGetPtr(s32);
extern void windowDrawSprite2(s32, s32, s32, s32, u32, void*, s32, s32);
extern f64 lbl_8047B948;
extern f32 lbl_8047B940;
typedef struct WorldMapOverlay {
    s32 active;
    f32 x;
    f32 y;
    f32 scale;
    f32 unused10;
    u32 color;
    u8 alpha;
    u8 pad19[3];
    f32 timer;
    f32 lifetime;
} WorldMapOverlay;
extern WorldMapOverlay lbl_803A20DC[];
#if 0
asm void menuNameEntryBackDrawBall(void) {
#include "src/game/gs_worldmap_fn_80028830.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
#pragma fp_contract on
s32 menuNameEntryBackDrawBall(void* r3) {
    void* r27;
    register s32 r29;
    register s32 r28;
    register WorldMapOverlay* r31;
    register s32 r30;
    f32 sx;
    f32 sy;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;

    r27 = r3;
    r29 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xc);
    r28 = *(s16*)((u8*)menuSpriteBiosGetPtr(0x98) + 0xe);
    r31 = lbl_803A20DC;
    r30 = 0;
    while (r30 < 0x1e) {
        if (r31->active != 0) {
            sx = (f32)r29 * r31->scale;
            sy = (f32)r28 * r31->scale;
            x1 = (s32)(lbl_8047B940 + sx);
            x0 = (s32)(lbl_8047B940 + (r31->x - sx * lbl_8047B940));
            y1 = (s32)(lbl_8047B940 + sy);
            y0 = (s32)(lbl_8047B940 + (r31->y - sy * lbl_8047B940));
            windowDrawSprite2(x0, y0, x1, y1, r31->color | r31->alpha, r27, 0x98, 0);
        }
        r31++;
        r30++;
    }
    return 0;
}
#pragma pop
#endif

/* menuNameEntryBackCtrl - 0x80028948 | size: 0x674 */
extern f32 fn_800E0BE4(void);
extern f32 lbl_8047B958;
extern f32 lbl_8047B95C;
extern f32 lbl_8047B960;
extern f32 lbl_8047B964;
extern f32 lbl_8047B968;
extern f32 lbl_8047B930;
extern f32 lbl_8047B970;
extern f32 lbl_8047B96C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B954;
#if 0
asm void menuNameEntryBackCtrl(void) {
#include "src/game/gs_worldmap_fn_80028948.inc"
}
#else
/*
 * menuNameEntryBackCtrl - GSmap_MainRenderFrame (0x80028948, size 0x674)
 *
 * Particle/overlay state machine that drives the 30-entry overlay table
 * (lbl_803A20DC, WorldMapOverlay[30]) consumed by the renderer menuNameEntryBackDrawBall.
 *
 * Dispatches on the 1-byte mode field at offset 0x01 of the controller
 * object (signed):
 *   mode 0 : prime - clear all 30 overlays, then run 600 spawn/update ticks,
 *            finally latch the "primed" flag at offset 0x02.
 *   mode 2 : per-frame - probabilistically spawn one overlay, then update all.
 *   mode 3 : latch the flag at offset 0x02 (one-shot) and return.
 *   else   : no-op.
 *
 * Each tick: fn_800E0BE4() returns a random f32; if it is <= the spawn
 * threshold (lbl_8047B958) a new overlay is allocated in the first inactive
 * slot (if any of the 30 are free). The update pass ages every active
 * overlay, retires it when its timer reaches its lifetime, and recomputes
 * its per-frame scale and alpha.
 *
 * Big-endian note: the original advanced its timer / treated lbl_8047B934 as
 * both the per-tick increment and the constant 1.0 used in the alpha fade;
 * the same symbol is reused here so the value stays identical to the ROM.
 * The fctiwz->stb on the alpha is a truncate-to-int then low-byte store.
 */
s32 menuNameEntryBackCtrl(void* r3)
{
    extern f32 fn_800E0BE4(void);            /* random f32 source (-> f1) */
    extern f32 lbl_8047B958;                 /* spawn threshold           */
    extern f32 lbl_8047B95C;                 /* x scale                   */
    extern f32 lbl_8047B960;                 /* y scale                   */
    extern f32 lbl_8047B964;                 /* scale base / init scale   */
    extern f32 lbl_8047B968;                 /* scale rand coeff          */
    extern f32 lbl_8047B96C;                 /* lifetime base             */
    extern f32 lbl_8047B970;                 /* lifetime rand coeff       */
    extern f32 lbl_8047B930;                 /* initial timer             */
    extern f32 lbl_8047B934;                 /* timer increment / 1.0     */
    extern f32 lbl_8047B954;                 /* alpha scale               */

    u8* ctl;
    s32 mode;
    s32 i;
    s32 slot;
    WorldMapOverlay* ov;

    ctl = (u8*)r3;
    mode = (s32)(s8)ctl[1];

    if (mode == 0) {
        if ((s32)(s8)ctl[2] != 0) {
            return 0;
        }

        /* clear all 30 overlays */
        for (i = 0; i < 30; i++) {
            lbl_803A20DC[i].active = 0;
        }

        /* 600 priming ticks */
        for (i = 0; i < 0x258; i++) {
            if (fn_800E0BE4() <= lbl_8047B958) {
                /* find first inactive slot */
                for (slot = 0; slot < 30; slot++) {
                    if (lbl_803A20DC[slot].active == 0) {
                        break;
                    }
                }
                if (slot < 30) {
                    ov = &lbl_803A20DC[slot];
                    ov->active = 1;
                    ov->x = lbl_8047B95C * fn_800E0BE4();
                    ov->y = lbl_8047B960 * fn_800E0BE4();
                    ov->scale = lbl_8047B964;
                    ov->unused10 = lbl_8047B968 * fn_800E0BE4() + lbl_8047B964;
                    ov->color = 0xFFFFFF00u;
                    ov->alpha = 0x80;
                    ov->timer = lbl_8047B930;
                    ov->lifetime = lbl_8047B970 * fn_800E0BE4() + lbl_8047B96C;
                }
            }

            /* update pass over all 30 overlays */
            for (slot = 0; slot < 30; slot++) {
                ov = &lbl_803A20DC[slot];
                if (ov->active != 0) {
                    f32 ratio;
                    ov->timer = ov->timer + lbl_8047B934;
                    if (ov->timer >= ov->lifetime) {
                        ov->active = 0;
                    }
                    ratio = ov->timer / ov->lifetime;
                    ov->scale = ov->unused10 * ratio;
                    ov->alpha = (u8)(s32)(lbl_8047B954 * (lbl_8047B934 - ratio));
                }
            }
        }

        ctl[2] = 1;
        return 0;
    }

    if (mode == 2) {
        /* probabilistic single spawn */
        if (fn_800E0BE4() <= lbl_8047B958) {
            for (slot = 0; slot < 30; slot++) {
                if (lbl_803A20DC[slot].active == 0) {
                    break;
                }
            }
            if (slot < 30) {
                ov = &lbl_803A20DC[slot];
                ov->active = 1;
                ov->x = lbl_8047B95C * fn_800E0BE4();
                ov->y = lbl_8047B960 * fn_800E0BE4();
                ov->scale = lbl_8047B964;
                ov->unused10 = lbl_8047B968 * fn_800E0BE4() + lbl_8047B964;
                ov->color = 0xFFFFFF00u;
                ov->alpha = 0x80;
                ov->timer = lbl_8047B930;
                ov->lifetime = lbl_8047B970 * fn_800E0BE4() + lbl_8047B96C;
            }
        }

        /* update pass over all 30 overlays */
        for (slot = 0; slot < 30; slot++) {
            ov = &lbl_803A20DC[slot];
            if (ov->active != 0) {
                f32 ratio;
                ov->timer = ov->timer + lbl_8047B934;
                if (ov->timer >= ov->lifetime) {
                    ov->active = 0;
                }
                ratio = ov->timer / ov->lifetime;
                ov->scale = ov->unused10 * ratio;
                ov->alpha = (u8)(s32)(lbl_8047B954 * (lbl_8047B934 - ratio));
            }
        }

        return 0;
    }

    if (mode == 3) {
        if ((s32)(s8)ctl[2] == 0) {
            ctl[2] = 1;
        }
        return 0;
    }

    return 0;
}
#endif

/* menuNameEntry - 0x80028FBC | size: 0x59c */
extern void pokemonBiosGetNicknamePtr(void);
extern void pcboxGetPokemonBoxName(void);
extern void menuItemBiosGetPtr(void);
extern void menuModelInit(void);
extern void fn_8010A010(void);
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F4C8(void);
extern void fn_80109C88(void);
extern void menuModelCheck(void);
extern void fadeSet(void);
extern void fadeCheck(void);
extern void fn_8010A420(void);
extern void heroSetStatus(void);
extern void pokemonBiosSetNicknamePtr(void);
extern void fn_801349DC(void);
extern void fn_800F9EE4(void);
extern void fn_800FF660(void);
extern void floorSetFadeScript(s32, u32);
extern u32 lbl_804788A0;
extern u8 lbl_80266DC0[];
extern f32 lbl_8047B940;
#if 0
asm void menuNameEntry(void) {
#include "src/game/gs_worldmap_fn_80028FBC.inc"
}
#else
/* menuNameEntry - GSmap_MainUpdate (0x80028FBC, size 0x59C)
 *
 * World-map main update loop. Operates entirely on module globals:
 *   - lbl_803A2068 : the GSmap context block. Layout used here:
 *         +0x00 u16   (cleared by callers)
 *         +0x18 s32   mode      (selector category, range 0..3)
 *         +0x1c s32   subIndex
 *         +0x20 s32   result    (written here, read by callers)
 *         +0x24 s32   flag24
 *         +0x28 s32   asyncMode (1 => render/finalize path active)
 *   - lbl_80266DC0 : map data blob. Header field +0x00 (ptr) and +0x0C (ptr),
 *         followed at +0x18 by an array of 0x10-byte entries; each entry has
 *         a payload pointer at +0x08 and an element count at +0x0C.
 *   - lbl_802EF0A8 : large read-only data blob (cross-TU); five s16 fields are
 *         latched into the lbl_8047A3xx scratch globals on the first frame.
 *   - lbl_804788A0 : "first frame / needs-latch" flag.
 *   - lbl_803A2094 : the world-map menu-model handle.
 *
 * Byte-match is irrelevant; this reproduces the x86 semantics of the loop.
 */
void menuNameEntry(void) {
    /* --- module globals (block-scope typed externs, TU convention) --- */
    extern u8  lbl_803A2068[];     /* GSmap context block            */
    extern u8  lbl_80266DC0[];     /* map data blob                  */
    extern u8  lbl_802EF0A8[];     /* far read-only data blob        */
    extern u8  lbl_803A2094[];     /* menu-model handle              */
    extern u32 lbl_804788A0;       /* first-frame latch flag         */
    extern u32 lbl_8047A3C0;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3BC;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B4;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B0;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047B940;       /* 0.0f constant                  */

    /* --- callees (minimal real signatures inferred from each bl site) --- */
    extern u32  GSmsgGetGSchar(u32 id);                       /* id -> resource ptr        */
    extern u32  heroGetStatus(u8* ptr, u32 selector, u32 idx);
    extern u32  pokemonCheckValid(void);                         /* returns u8 status         */
    extern u32  pokemonBiosGetNicknamePtr(u32 a);
    extern u32  pcboxGetPokemonBoxName(s32 a, s32 b);
    extern void GScharCpy(void* dst, u8* src);           /* copy/build name struct    */
    extern void* menuItemBiosGetPtr(s32 id);                      /* returns struct ptr        */
    extern void menuModelInit(void* handle, s16 a, s16 b);
    extern void fn_8010A010(void* handle, s32 v);
    extern void peopleInfoBiosGetPtr(s32 v);
    extern void fn_8018F4C8(s32 a, s32* outA, s32* outB);
    extern void menuModelSetMotion(void* handle, s32 motion);
    extern void fn_80109C88(void* handle, u32 v);
    extern void menuModelCheck(void* handle, s32 v);
    extern void fn_8010A420(void* handle);
    extern void fadeSet(s32 mode, f32 v);
    extern void fadeCheck(s32 v);
    extern u32  windowGetActiveID(void);                         /* returns context handle    */
    extern s32  menuOpenCustom(s32 id, u32 ctx, s32 a, s32 b, s32 c, s32 d, void* arg);
    extern u32  fn_80166A28(s32 size);
    extern void msgctrlSetValue(s32 id, u32 name);
    extern void winMsgOpen(s32 a, s32 b, s32 c, s32 d);
    extern s32  menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void winMsgClose(s32 a);
    extern void menuClose(s32 id);
    extern void menuCloseSync(s32 id, s32 a);
    extern s32  inputName__FPUsPUsiii(u8* ctx, void* nameBuf, s32 mode, s32 subIndex, s32 last);
    extern void heroSetStatus(s32 a, s32 b, u8* ctx);
    extern void pokemonBiosSetNicknamePtr(u32 v, u8* ctx);
    extern void pcboxSetPokemonBoxName(s32 a, s32 name, u8* ctx);
    extern s32  GScharCmp(u8* ctx, void* nameBuf);
    extern void fn_800FF660(void);
    extern void floorSetFadeScript(s32 a, u32 b);

    u8* ctx;        /* lbl_803A2068 context block          */
    u8* data;       /* lbl_80266DC0 map data blob          */
    s32 mode;       /* ctx +0x18                           */
    s32 subIndex;   /* ctx +0x1c                           */
    s32 r0;         /* generic selection result            */
    u32 sel;        /* selection / pokemon handle          */
    u8  ok;
    s32 nameBuf[8]; /* sp+0x30 local name buffer (was sp[]) */
    s32 entryBuf[4];/* sp+0x20: copy of map header fields   */
    void* mdl;      /* struct ptr from menuItemBiosGetPtr          */
    s32 motOut;     /* fn_8018F4C8 out word @ sp+0xc        */
    s32 motTmp;     /* fn_8018F4C8 out word @ sp+0x8        */

    data = lbl_80266DC0;

    /* First-frame latch: copy fixed display params out of the big data blob. */
    if (lbl_804788A0 != 0) {
        lbl_804788A0 = 0;
        (*(s32*)&lbl_8047A3C0) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fca);
        (*(s32*)&lbl_8047A3BC) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fcc);
        (*(s32*)&lbl_8047A3B8) = (s32)*(s16*)(lbl_802EF0A8 + 0x20ec2);
        (*(s32*)&lbl_8047A3B4) = (s32)*(s16*)(lbl_802EF0A8 + 0x73ba);
        (*(s32*)&lbl_8047A3B0) = (s32)*(s16*)(lbl_802EF0A8 + 0x70fe);
    }

    ctx = lbl_803A2068;
    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);

    /* --- Resolve the current selection (r3) from the mode switch. --- */
    sel = 0;
    switch (mode) {
    case 0:
        sel = GSmsgGetGSchar(*(u32*)(data + 0x0));
        break;
    case 1:
        sel = GSmsgGetGSchar(*(u32*)(data + 0xc));
        break;
    case 2:
        r0 = (s32)heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) == 0) {
            sel = 0;
        } else {
            sel = pokemonBiosGetNicknamePtr((u32)r0);
        }
        break;
    case 3:
        sel = pcboxGetPokemonBoxName(0, (s32)(s8)subIndex);
        break;
    default:
        sel = 0;
        break;
    }

    /* Build the name buffer from the selection; if none, zero the head. */
    if (sel != 0) {
        GScharCpy(nameBuf, (u8*)sel);
        ok = 1;
    } else {
        ok = 0;
    }
    if (ok == 0) {
        *(u16*)nameBuf = 0;
    }

    /* Snapshot four words of the data header into a scratch frame array
       (these are indexed by 'mode' below). */
    mode = *(s32*)(ctx + 0x18);
    entryBuf[0] = *(s32*)(data + 0x88);
    entryBuf[1] = *(s32*)(data + 0x8c);
    entryBuf[2] = *(s32*)(data + 0x90);
    entryBuf[3] = *(s32*)(data + 0x94);
    subIndex = *(s32*)(ctx + 0x1c);

    /* Fetch the model descriptor (0xd3a for mode 2, else 0xd39) and pose it. */
    mdl = menuItemBiosGetPtr((mode == 2) ? 0xd3a : 0xd39);
    menuModelInit(lbl_803A2094, *(s16*)((u8*)mdl + 0x6), *(s16*)((u8*)mdl + 0x8));

    /* --- Per-mode model setup. --- */
    switch (mode) {
    case 0:
    case 1:
    case 3: {
        s32 v = entryBuf[mode];
        if (mode == 3) {
            fn_8010A010(lbl_803A2094, v);
        } else {
            fn_8010A010(lbl_803A2094, v);
            peopleInfoBiosGetPtr(v);
            fn_8018F4C8(1, &motOut, &motTmp);
            menuModelSetMotion(lbl_803A2094, motOut);
        }
        break;
    }
    case 2:
        sel = heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) != 0) {
            fn_80109C88(lbl_803A2094, sel);
        }
        break;
    default:
        break;
    }
    menuModelCheck(lbl_803A2094, 1);

    /* --- Open the primary menu window (id 0x6f) seeded with the context. --- */
    nameBuf[6] = *(s32*)(ctx + 0x18);   /* sp+0x18 */
    nameBuf[7] = *(s32*)(ctx + 0x1c);   /* sp+0x1c */
    menuOpenCustom(0x6f, windowGetActiveID(), 0, 0, 1, 1, &nameBuf[6]);

    /* On async/render frames, prime the transition. */
    ctx = lbl_803A2068;
    if (*(s32*)(ctx + 0x28) == 0) {
        fadeSet(2, lbl_8047B940);
        fadeCheck(1);
    }

    /* --- List/confirm loop over the current mode's entry array. --- */
    {
        s32 idx = *(s32*)(ctx + 0x18);
        u8* entry = (data + 0x18) + idx * 0x10;
        s32 count = *(s32*)(entry + 0xc);
        s32 lastFlag;

        if (count > 0) {
            s32 done = 0;
            while (done == 0) {
                idx = *(s32*)(ctx + 0x18);
                entry = (data + 0x18) + idx * 0x10;
                count = *(s32*)(entry + 0xc);
                {
                    s32* listPtr = *(s32**)(entry + 0x8);
                    s32 accepted = 0;
                    for (;;) {
                        s32 pick;
                        nameBuf[4] = (s32)listPtr; /* sp+0x10 */
                        nameBuf[5] = count;        /* sp+0x14 */
                        pick = menuOpenCustom(0x70, windowGetActiveID(), 0, 0, 1, 1, &nameBuf[4]);
                        if (pick == 0) {
                            fn_80166A28(0x24);
                            accepted = 0;
                            break;
                        }
                        if (pick == -1) {
                            continue;
                        }
                        {
                            u32 choiceName = (u32)GSmsgGetGSchar((u32)listPtr[pick - 1]);
                            s32 ans;
                            fn_80166A28(0x440);
                            msgctrlSetValue(0x4d, choiceName);
                            winMsgOpen(2, 0x2ef6, 1, 0);
                            ans = (s32)(s8)menuSubOpenYesNo(0, -1, -1, 0);
                            winMsgClose(1);
                            if (ans == 1 || ans == -1) {
                                /* yes/cancel sentinel -> not accepted, retry */
                                continue;
                            }
                            /* accepted */
                            menuClose(0x70);
                            menuCloseSync(0x70, 1);
                            accepted = 1;
                            GScharCpy(lbl_803A2068, (u8*)choiceName);
                            done = 1;
                            goto after_inner; /* accepted path completes the list loop */
                        }
                    }
                    /* not accepted: close the sub-window and continue/abort. */
                    menuClose(0x70);
                    menuCloseSync(0x70, 1);
                after_inner:;
                    if (done != 0) {
                        break;
                    }
                    /* Re-run init for the next page; if it reports terminal, stop. */
                    if (inputName__FPUsPUsiii(lbl_803A2068, nameBuf,
                                    *(s32*)(ctx + 0x18), *(s32*)(ctx + 0x1c), 0) != 0) {
                        break;
                    }
                }
            }
        } else {
            /* Empty list: single terminal init pass. */
            inputName__FPUsPUsiii(lbl_803A2068, nameBuf, 0 /*unused*/, *(s32*)(ctx + 0x1c), 1);
        }
        (void)lastFlag;
    }

    /* --- Tear down the menu model and finalize the selection by mode. --- */
    fadeSet(3, lbl_8047B940);
    fadeCheck(1);
    fn_8010A420(lbl_803A2094);
    menuClose(0x6f);
    menuCloseSync(0x6f, 1);

    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);
    switch (mode) {
    case 0:
        break;
    case 1:
        heroSetStatus(0, 0x17, lbl_803A2068);
        break;
    case 2:
        sel = heroGetStatus(0, 3, (u16)subIndex);
        if ((pokemonCheckValid() & 0xff) != 0) {
            pokemonBiosSetNicknamePtr(sel, lbl_803A2068);
        }
        break;
    case 3:
        pcboxSetPokemonBoxName(0, (s32)(s8)subIndex, lbl_803A2068);
        break;
    default:
        break;
    }

    /* result = (name changed) ? 1 : 0 */
    if (GScharCmp(lbl_803A2068, nameBuf) == 0) {
        *(s32*)(lbl_803A2068 + 0x20) = 0;
    } else {
        *(s32*)(lbl_803A2068 + 0x20) = 1;
    }

    /* Async finalize: post the appropriate completion event. */
    if (*(s32*)(ctx + 0x28) != 0) {
        fn_800FF660();
        if (*(s32*)(lbl_803A2068 + 0x24) != 0) {
            floorSetFadeScript(0, 0x05960008);
        } else {
            floorSetFadeScript(0, 0);
        }
    }
}
#endif

/* menuNameEntryCall - 0x80029558 | size: 0xe0 */
extern u8 lbl_803A2068[];
#if 0
asm void menuNameEntryCall(void) {
#include "src/game/gs_worldmap_fn_80029558.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryCall(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _558_eq2;
    if (r29 >= 2) goto _558_ge3;
    if (r29 >= 0) goto _558_done;
    goto _558_fail;
    _558_ge3:
    if (r29 >= 4) goto _558_fail;
    goto _558_r30;
    _558_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _558_done;
    _558_r30:
    if (r30 < 0) goto _558_r30_fail;
    if (r30 < 3) goto _558_done;
    _558_r30_fail:
    r31 = 0;
    goto _558_done;
    _558_fail:
    r31 = 0;
    _558_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x28) = 0;
    menuNameEntry();
    return *(s32*)(ctx + 0x20);
}
#pragma peephole on
#endif

/* menuNameEntryGetLastName - 0x80029638 | size: 0x28 */
#if 0
asm void menuNameEntryGetLastName(void) {
#include "src/game/gs_worldmap_fn_80029638.inc"
}
#else
#pragma scheduling off
#pragma optimization_level 4
void menuNameEntryGetLastName(void* r3) {
    GScharCpy(r3, lbl_803A2068);
}
#pragma scheduling on
#endif

/* menuNameEntryOpenNoFade - 0x80029660 | size: 0x100 */
extern void fn_800FF730(s32);
extern void _threadSwitch(void);
#if 0
asm void menuNameEntryOpenNoFade(void) {
#include "src/game/gs_worldmap_fn_80029660.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryOpenNoFade(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _660_eq2;
    if (r29 >= 2) goto _660_ge3;
    if (r29 >= 0) goto _660_done;
    goto _660_fail;
    _660_ge3:
    if (r29 >= 4) goto _660_fail;
    goto _660_r30;
    _660_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _660_done;
    _660_r30:
    if (r30 < 0) goto _660_r30_fail;
    if (r30 < 3) goto _660_done;
    _660_r30_fail:
    r31 = 0;
    goto _660_done;
    _660_fail:
    r31 = 0;
    _660_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 0;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    floorSetFadeScript(0, 0x05960008);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}
#endif

/* menuNameEntryOpen - 0x80029760 | size: 0xf0 */
#if 0
asm void menuNameEntryOpen(void) {
#include "src/game/gs_worldmap_fn_80029760.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 menuNameEntryOpen(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _760_eq2;
    if (r29 >= 2) goto _760_ge3;
    if (r29 >= 0) goto _760_done;
    goto _760_fail;
    _760_ge3:
    if (r29 >= 4) goto _760_fail;
    goto _760_r30;
    _760_eq2:
    heroGetStatus(0, 3, (u16)r30);
    if ((u8)pokemonCheckValid() == 0) r31 = 0;
    goto _760_done;
    _760_r30:
    if (r30 < 0) goto _760_r30_fail;
    if (r30 < 3) goto _760_done;
    _760_r30_fail:
    r31 = 0;
    goto _760_done;
    _760_fail:
    r31 = 0;
    _760_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 1;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}
#endif
