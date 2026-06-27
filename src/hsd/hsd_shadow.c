/**
 * @file hsd_shadow.c
 * @brief HSD Shadow system - shadow map rendering and application.
 *
 * Address range: 0x801B019C - 0x801B16C0
 * Contains the HSD shadow rendering system: shadow map generation,
 * projection matrix computation, shadow texture binding, and
 * the main shadow rendering dispatch.
 *
 * "Proposed:" names from symbols.txt: HSD_ShadowFunc1-10, HSD_ShadowMain
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"

/* NOTE: fn_801B06D4 is already in hsd_pobj.c as a simple getter */

/* ========================================================================= */
/*  Shadow setup functions                                                   */
/* ========================================================================= */

/* Address: 0x801B019C | Size: 0x204 | Proposed: HSD_ShadowFunc1 */
/* Shadow map initialization and projection matrix setup */
void fn_801B019C(void) {
}

/* Address: 0x801B03A0 | Size: 0x68 | Proposed: HSD_ShadowFunc2 */
/* Shadow texture coordinate generation */
s32 fn_801B03A0(void* arg0) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDB8;
    s32 result;

    if (arg0 == NULL) {
        __assert(lbl_802752C0, 0x37D, &lbl_8047DDB8);
    }
    result = 0;
    if (*(f32*)((u8*)arg0 + 0x40) > *(f32*)((u8*)arg0 + 0x44)) {
        if (*(f32*)((u8*)arg0 + 0x4C) > *(f32*)((u8*)arg0 + 0x48)) {
            result = 1;
        }
    }
    return result;
}

/* Address: 0x801B0408 | Size: 0xD8 | Proposed: HSD_ShadowFunc3 */
/* Shadow TEV stage configuration */
void fn_801B0408(void) {
}

/* Address: 0x801B04E0 | Size: 0x1F4 | Proposed: HSD_ShadowFunc4 */
/* Shadow map projection computation */
void fn_801B04E0(void) {
}

/* ========================================================================= */
/*  Shadow state management                                                  */
/* ========================================================================= */

/* Address: 0x801B06DC | Size: 0x60 */
/* Shadow state setter / initializer */
void fn_801B06DC(void* arg0) {
    extern void* fn_800E202C(void*);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(void*);
    void* saved;
    void** slot;
    void* obj;

    obj = *(void**)((u8*)arg0 + 8);
    slot = *(void***)((u8*)obj + 0x58);
    if (*(void* volatile*)slot != NULL) {
        saved = fn_800E202C(*slot);
        fn_800E24B0();
        fn_800E209C(saved);
        *slot = NULL;
    }
}

/* Address: 0x801B073C | Size: 0x98 */
/* Shadow map texture bind */
void fn_801B073C(void) {
}

/* Address: 0x801B07D4 | Size: 0xAC */
/* Shadow light direction setup */
void fn_801B07D4(void) {
}

/* ========================================================================= */
/*  Main shadow rendering                                                    */
/* ========================================================================= */

/* Address: 0x801B0880 | Size: 0x218 | Proposed: HSD_ShadowFunc5 */
/* Shadow pass setup - configures GX for shadow map rendering */
void fn_801B0880(void) {
}

/* Address: 0x801B0A98 | Size: 0x140 | Proposed: HSD_ShadowFunc6 */
/* Shadow receiver configuration */
void fn_801B0A98(void) {
}

/* Address: 0x801B0BD8 | Size: 0x2E0 | Proposed: HSD_ShadowFunc7 */
/* Shadow caster traversal and rendering */
void fn_801B0BD8(void) {
}

/* Address: 0x801B0EB8 | Size: 0x66C | Proposed: HSD_ShadowMain */
/* Main shadow system entry point - orchestrates shadow map gen and apply */
void fn_801B0EB8(void) {
}

/* Address: 0x801B1524 | Size: 0x19C | Proposed: HSD_ShadowFunc9 */
/* Shadow cleanup / restore GX state */
void fn_801B1524(void) {
}

/* Address: 0x801B16C0 | Size: 0x70 | Proposed: HSD_ShadowFunc10 */
/* Shadow finalize - restore render state after shadow pass */
void fn_801B16C0(void* arg0) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern void fn_800B962C(u32, u32, u32, u32);
    extern void fn_800B96F8(u32, u32, u32, u32);
    void* obj;
    u8* shadow;

    if (arg0 == NULL) {
        __assert(lbl_802752C0, 0x10C, &lbl_8047DDCC);
    }
    obj = *(void**)((u8*)arg0 + 8);
    shadow = *(u8**)((u8*)obj + 0x58);
    fn_800B962C(0, 0, *(u16*)(shadow + 4), *(u16*)(shadow + 6));
    fn_800B96F8(*(u16*)(shadow + 4), *(u16*)(shadow + 6), 0x20, 0);
}
