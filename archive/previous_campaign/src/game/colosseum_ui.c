/**
 * @file colosseum_ui.c
 * @brief General Colosseum UI utility functions.
 *
 * This module contains UI helper routines that serve as common building blocks
 * for the menu/screen system. Functions here include sprite/graphic positioning,
 * text formatting helpers, and state-machine utilities used by the shop, summary
 * screen, and other UI subsystems.
 *
 * These functions form a transitional region between the scene init code and
 * the shop system, providing generic UI framework capabilities.
 *
 * Key behaviors:
 *   - Contains state machine logic invoked by scene_init (fn_8003A520 calls
 *     fn_80039498 and fn_8003A10C internally)
 *   - fn_8003A10C (0x414 bytes) is a significant UI framework function that
 *     dispatches to fn_8003ACE8 and fn_8003AE84
 *   - fn_8003A7F0 (0x460 bytes) is another large state machine
 *   - No BSS references, suggesting these operate on caller-provided data
 *
 * Address range: 0x80039A50 - 0x8003AEF0 (12 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);          /* GSthread yield */
extern void  fn_801026A4(u32 sceneId, u32 a, u32 b, u32 c,
                         u32 d, u32 e, ...);
extern u32   fn_80102568(u32 a, u32 b, u32 c);
extern u32   fn_8010264C(u32 a, u32 b);
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d);
extern void  fn_80132A38(u32 effectId, u32 param);
extern u32   lbl_8047A4B4;

typedef struct ColosseumUiDrawContext {
    u8 unk_00[0x88];
    s32 color; /* 0x88 */
} ColosseumUiDrawContext;

/*
 * Functions in this translation unit (12 total):
 *
 * fn_80039A50  0x034  Small utility
 * fn_80039A84  0x4C0  Large UI state machine
 * fn_80039F44  0x02C  Small accessor
 * fn_80039F70  0x19C  UI helper
 * fn_8003A10C  0x414  UI framework dispatcher (calls fn_8003ACE8, fn_8003AE84)
 * fn_8003A520  0x1A0  UI entry point (calls fn_80039498, fn_8003A10C)
 * fn_8003A6C0  0x130  UI helper
 * fn_8003A7F0  0x460  Large UI state machine
 * fn_8003AC50  0x098  UI accessor
 * fn_8003ACE8  0x19C  UI sub-handler A
 * fn_8003AE84  0x06C  UI sub-handler B
 */


/* 0x80039A50 | size: 0x34 */
/* `peephole off` preserves the target's load/zero scheduling for this leaf wrapper. */
#pragma push
#pragma peephole off
u32 fn_80039A50(ColosseumUiDrawContext* ctx) {
    fn_800FB680(0, 0, ctx->color, lbl_8047A4B4);
    return 0;
}
#pragma pop

/* 0x80039A84 | size: 0x4C0 */
asm void fn_80039A84(void) { nofralloc
    #include "asm/GC6E01/nonmatching/colosseum_ui/fn_80039A84.s"
}


/* 0x8003AD6C | 0x118
 * Check which group (0 or 1) a UI element's ID belongs to,
 * based on lookup table lbl_80267140 (two groups of 2 IDs each).
 * If found in a group, calls fn_80109220 with a flag indicating
 * whether the element's current group matches.
 */
u32 fn_8003AD6C(u8* state, u8* element) {
    extern u8 lbl_80267140[];
    extern void fn_80109220(u8* elem, u32 flag);
    u32 groups[4]; /* 2 groups of 2 u32 IDs */
    s32 found;
    s32 groupIdx;
    s32 i;
    s16 elemId;

    /* Copy the group table from rodata to stack */
    groups[0] = *(u32*)(lbl_80267140 + 0x0);
    groups[1] = *(u32*)(lbl_80267140 + 0x4);
    groups[2] = *(u32*)(lbl_80267140 + 0x8);
    groups[3] = *(u32*)(lbl_80267140 + 0xC);

    found = 0;
    groupIdx = 0;
    elemId = *(s16*)(element + 0x6);

    /* Search group 0 (first 2 entries) */
    for (i = 0; i < 2 && found == 0; i++) {
        if ((s32)elemId == (s32)groups[i]) {
            found = 1;
        }
    }

    /* If not found in group 0, search group 1 */
    if (found == 0) {
        groupIdx = 1;
        for (i = 0; i < 2 && found == 0; i++) {
            if ((s32)elemId == (s32)groups[2 + i]) {
                found = 1;
            }
        }
        if (found == 0) {
            groupIdx = 2;
        }
    }

    if (found == 0) {
        return 0;
    }

    {
        s8 currentGroup = *(s8*)(state + 0x95);
        u32 flag = (groupIdx == (s32)currentGroup) ? 1 : 0;
        fn_80109220(element, flag);
    }
    return 0;
}
