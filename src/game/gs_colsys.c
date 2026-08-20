/**
 * @file gs_colsys.c
 * @brief GScolsys2 -- surface-type interaction table + a handful of
 *        matched leaf helpers. 0x8010C364-0x8010CBD0 (16 fns, 9 matched).
 *
 * Real matched functions (100%, byte-identical -- do not touch):
 *   fn_8010C364  -- returns 1 after a GStexture-invalidation trigger call
 *   fn_8010C46C, fn_8010C4A0, fn_8010C4D4, fn_8010C508
 *                -- surface-type table (lbl_8035B500) field readers /
 *                   type-vs-type interaction lookup
 *   GScolsy2UtilGetSidePlanePoint -- 3-component dot product
 *   GScolsys2GetObjEnable, GScolsys2SetObjEnable -- active-layer per-triangle visibility
 *                   flag get/set (via lbl_80404C68)
 *   fn_8010CBC0  -- returns the first word of lbl_80404C68
 *
 * Still unmatched (honest TODO stubs, no evidence for real names):
 *   fn_8010C388, _menuFaceBiosGetPtr__FUs, fn_8010C54C, fn_8010C650,
 *   zokuseiGetWazaJoutai, fn_8010C8D0, fn_8010CA30
 *
 * A prior campaign-generation transplant had attached a ~1600-line
 * "GScolsys2_*" fiction block to this file: hand-written bodies for a
 * fully-narrated collision system (WZX mesh loading/relocation, transform
 * building, debug-draw display lists, ground raycasting) under invented
 * names, none of which appear in symbols.txt and none of which are
 * called from anywhere else in the tree. The block also duplicated
 * GScolsys2GetObjEnable/GScolsys2SetObjEnable's real logic under GScolsys2_QueryTriVisible/
 * GScolsys2_SetTriVisible, and claimed several functions (GScolsys2Draw,
 * GScolsys2UnloadCCD, GScolsys2GetCurFloor, ...) whose real addresses
 * (0x8010CBD0-0x8010F6A0, per splits.txt) belong to gs_range_8010CBD0.c,
 * not this unit. That block has been removed as dead fiction; the actual
 * generated scaffold (below, "Generated: 1 pattern-matched + 31 stubs")
 * already carries the correct bodies for this unit's 16 real functions.
 *
 * Note: that generated scaffold also carries stray stubs for
 * 0x8010C220-0x8010C364 and 0x8010CBD0-0x8010E138, which per splits.txt
 * belong to gs_range_80109C88.c and gs_range_8010CBD0.c respectively.
 * Reconciling those is out of scope for this pass and they are left
 * untouched here.
 *
 * Address range: 0x8010C364 - 0x8010CBD0
 */

#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "game/gs_colsys.h"

/* ===== External SDK / engine functions ===== */
extern void  GStextureFree(void);                        /* GStexture trigger (fn_8010C364) */
extern void PSMTXIdentity(Mtx m);
extern void PSMTXCopy(const Mtx src, Mtx dst);
extern void PSMTXConcat(const Mtx left, const Mtx right, Mtx out);
extern void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 angle);
extern void PSMTXScaleApply(const Mtx src, Mtx dst, f32 x, f32 y, f32 z);
extern void PSMTXTransApply(const Mtx src, Mtx dst, f32 x, f32 y, f32 z);

/* ===== Static data tables (data section) ===== */
extern GSColSurfaceType lbl_8035B500[]; /* Surface type table (0x12 entries, 0x2C each) */
extern const Vec lbl_80272020[3];
extern const u32 lbl_80272044[3];

/* ===== SDA globals ===== */

/* Surface type count (sdata at lbl_80478B38) */
extern u32  lbl_80478B38;  /* number of surface types = 0x12 */

/* ===== BSS -- main collision system state ===== */

/** Main collision state: 0x3710 bytes at lbl_80404C68 */
extern GSColSysState lbl_80404C68;

/* =================================================================
 * Convenience macros for accessing the collision state
 * ================================================================= */
#define COL_STATE       (&lbl_80404C68)
#define COL_WZX         (COL_STATE->wzxDataPtr)
#define COL_LAYER_IDX   (COL_STATE->activeLayer)
#define COL_GFX_HANDLE  (COL_STATE->gfxRenderHandle)
#define COL_DISPLIST    (COL_STATE->displayList)

/** Get pointer to the start of collision layer N */
#define COL_LAYER_PTR(n)  ((void*)((u8*)COL_STATE + 4 + (n) * GSCOLSYS_LAYER_SIZE))

/* ===================================================================
 * Generated: 1 pattern-matched + 31 stubs
 * Range: 0x8010C220 - 0x8010E138
 * =================================================================== */

/* 0x8010C220 | 0x4 | void_stub */
void fn_8010C220(void) {
}

/* 0x8010C224 | 0x140 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010C224(void) {
    /* TODO: match -- 320 bytes at 0x8010C224 */
}
#pragma pop

/* 0x8010C364 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
s32 fn_8010C364(void) {
    GStextureFree();
    return 1;
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x8010C388 | 0x74 */
extern void* lbl_80478E70; /* -> face-bios entry count */
extern void* lbl_80478E74; /* -> face-bios entry table */
extern char lbl_80272000[];
extern char lbl_8035B4E8[];
extern void GSlogWrite(char*, char*, ...);

u32 fn_8010C388(u16 idx) {
    u8* entry;
    u16 i = idx;

    if (i >= *(u32*)lbl_80478E70) {
        GSlogWrite(lbl_80272000, lbl_8035B4E8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478E74 + i * 8;
    }
    if (entry == NULL) {
        return 0;
    }
    return (*entry >> 7) & 1;
}

/* 0x8010C3FC | 0x70 */
void* _menuFaceBiosGetPtr__FUs(u16 idx) {
    u8* entry;
    u16 i = idx;

    if (i >= *(u32*)lbl_80478E70) {
        GSlogWrite(lbl_80272000, lbl_8035B4E8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478E74 + i * 8;
    }
    if (entry == NULL) {
        return NULL;
    }
    return *(void**)(entry + 4);
}

/* 0x8010C46C | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u16 fn_8010C46C(u16 typeIndex) {
    if (typeIndex >= lbl_80478B38) {
        return 0;
    }
    return *(u16*)((u8*)(lbl_8035B500 + typeIndex) + 2);
}
#pragma pop

/* 0x8010C4A0 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u8 fn_8010C4A0(u16 typeIndex) {
    if (typeIndex >= lbl_80478B38) {
        return 0;
    }
    return *(u8*)((u8*)(lbl_8035B500 + typeIndex) + 0);
}
#pragma pop

/* 0x8010C4D4 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_8010C4D4(u16 typeIndex) {
    if (typeIndex >= lbl_80478B38) {
        return 0;
    }
    return *(u32*)((u8*)(lbl_8035B500 + typeIndex) + 4);
}
#pragma pop

/* 0x8010C508 | 0x44 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u16 fn_8010C508(u32 typeA, u32 typeB) {
    if (typeA >= lbl_80478B38) {
        return 0;
    }
    if (typeB >= 0x12) {
        return 0;
    }
    return *(u16*)((u8*)lbl_8035B500 + (typeA * 0x2C) + (typeB * 2) + 8);
}
#pragma pop

/* 0x8010C54C | 0x104 */
#pragma push
#pragma optimization_level 4
#pragma dont_inline on
s32 fn_8010C54C(u16 param1, u8 mode) {
    s32 i;
    u16 code1;
    u16 code2;
    u16 result;
    s32 score = 0;

    for (i = 0; i < lbl_80478B38; i++) {
        if (i == 9) {
            continue;
        }
        if (mode == 1) {
            code1 = fn_8010C508((u32)param1, (u32)i);
            result = code1;
            if (result == 0x43) {
                score -= 10;
                continue;
            }
            if (result == 0x42) {
                score -= 10;
                continue;
            }
            if (result == 0x41) {
                score += 10;
                continue;
            }
        } else {
            code2 = fn_8010C508((u32)i, (u32)param1);
            result = code2;
            if (result == 0x43) {
                score += 10;
                continue;
            }
            if (result == 0x42) {
                score += 10;
                continue;
            }
            if (result == 0x41) {
                score -= 10;
                continue;
            }
        }
    }
    return score;
}
#pragma pop

/* 0x8010C650 | 0xFC */
#pragma push
#pragma dont_inline on
u16 fn_8010C650(u16 typeA, u16* arr, u16 count) {
    u16 i;
    u16 countA = 0;
    u16 countB = 0;
    u16 e;
    u16 code;
    u16 result;

    for (i = 0; i < count; i++) {
        if (i == 9) {
            continue;
        }
        e = arr[i];
        code = fn_8010C508((u32)typeA, (u32)e);
        result = code;
        if (result == 0x43) {
            return 0x43;
        }
        if (result == 0x41) {
            countA++;
            continue;
        }
        if (result == 0x42) {
            countB++;
        }
    }
    if (countA == 0 && countB == 0) {
        return 0x3f;
    }
    if (countA == countB) {
        return 0x3f;
    }
    if (countA > countB) {
        return 0x41;
    }
    return 0x42;
}
#pragma dont_inline reset
#pragma pop

/* 0x8010C74C | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma inline_depth(0)
u16 zokuseiGetWazaJoutai(u16 typeA, u16 typeB) {
    return fn_8010C508(typeA, typeB);
}
#pragma pop

/* 0x8010C77C | 0x40 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
#pragma fp_contract on
f32 GScolsy2UtilGetSidePlanePoint(Vec3f* normal, Vec3f* p1, Vec3f* p2) {
    return (normal->x * (p2->x - p1->x))
        + (normal->y * (p2->y - p1->y))
        + (normal->z * (p2->z - p1->z));
}
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma fp_contract off
#pragma pop

/* 0x8010C7BC | 0x88 */
s32 GScolsys2GetObjEnable(s32 triIndex, u32* outResult) {
    u8* base = (u8*)COL_STATE;
    void* wzx = COL_WZX;
    u8* entry;
    s32 result;

    if (wzx == NULL) {
        result = 1;
    } else if (triIndex < 0 || triIndex >= *(u32*)((u8*)wzx + 4)) {
        result = 2;
    } else {
        u8* t = base + COL_LAYER_IDX * GSCOLSYS_LAYER_SIZE;
        t = t + 4;
        t = t + triIndex * GSCOLSYS_TRI_ENTRY_SIZE;
        entry = t;
        result = 0;
    }
    if (result != 0) {
        return result;
    }
    if (*(u16*)(entry + 0x24) & 1) {
        *outResult = 0;
    } else {
        *outResult = 1;
    }
    return 0;
}

/* 0x8010C844 | 0x8C */
s32 GScolsys2SetObjEnable(s32 triIndex, s32 visible) {
    u8* base = (u8*)COL_STATE;
    void* wzx = COL_WZX;
    u8* entry;
    u16 flags;
    s32 result;

    if (wzx == NULL) {
        result = 1;
    } else if (triIndex < 0 || triIndex >= *(u32*)((u8*)wzx + 4)) {
        result = 2;
    } else {
        u8* t = base + COL_LAYER_IDX * GSCOLSYS_LAYER_SIZE;
        t = t + 4;
        t = t + triIndex * GSCOLSYS_TRI_ENTRY_SIZE;
        entry = t;
        result = 0;
    }
    if (result != 0) {
        return result;
    }
    if (visible != 0) {
        flags = *(u16*)(entry + 0x24);
        flags &= 0xFFFE;
        *(u16*)(entry + 0x24) = flags;
    } else {
        flags = *(u16*)(entry + 0x24);
        flags |= 1;
        *(u16*)(entry + 0x24) = flags;
    }
    return 0;
}

/* 0x8010C8D0 | 0x160 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010C8D0(Mtx out, u32 triIndex) {
    typedef struct ColAxes {
        Vec axis[3];
    } ColAxes;
    typedef struct ColAxisOrder {
        u32 index[3];
    } ColAxisOrder;
    u8* base = (u8*)COL_STATE;
    void* wzx = COL_WZX;
    u8* entry;
    ColAxisOrder order;
    Mtx result;
    ColAxes axes;
    Mtx combined;
    Mtx rotation;
    u32 i;
    u32* axisOrder;

    if (wzx == NULL) {
        return 0;
    }
    if (triIndex >= *(u32*)((u8*)wzx + 4)) {
        return 0;
    }

    entry = base + COL_LAYER_IDX * GSCOLSYS_LAYER_SIZE + 4
          + triIndex * GSCOLSYS_TRI_ENTRY_SIZE;
    PSMTXIdentity(out);
    if ((*(u16*)(entry + 0x24) & 1) != 0) {
        axes = *(const ColAxes*)lbl_80272020;
        order = *(const ColAxisOrder*)lbl_80272044;
        PSMTXIdentity(combined);
        for (i = 0, axisOrder = order.index; i < 3; i++, axisOrder++) {
            PSMTXRotAxisRad(rotation, &axes.axis[*axisOrder],
                            *(f32*)(entry + 0xC + *axisOrder * 4));
            PSMTXConcat(rotation, combined, combined);
        }
        PSMTXCopy(combined, result);
        PSMTXConcat(out, result, out);
    }
    return 1;
}
#pragma pop

/* 0x8010CA30 | 0x190 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010CA30(Mtx out, u32 triIndex) {
    typedef struct ColAxes {
        Vec axis[3];
    } ColAxes;
    typedef struct ColAxisOrder {
        u32 index[3];
    } ColAxisOrder;
    u8* base = (u8*)COL_STATE;
    void* wzx = COL_WZX;
    u8* entry;
    ColAxisOrder order;
    Mtx result;
    ColAxes axes;
    Mtx combined;
    Mtx rotation;
    u32 i;
    u32* axisOrder;

    if (wzx == NULL) {
        return 0;
    }
    if (triIndex >= *(u32*)((u8*)wzx + 4)) {
        return 0;
    }

    entry = base + COL_LAYER_IDX * GSCOLSYS_LAYER_SIZE + 4
          + triIndex * GSCOLSYS_TRI_ENTRY_SIZE;
    PSMTXIdentity(out);
    if ((*(u16*)(entry + 0x24) & 1) != 0) {
        PSMTXScaleApply(out, out, *(f32*)(entry + 0x18),
                       *(f32*)(entry + 0x1C), *(f32*)(entry + 0x20));
        axes = *(const ColAxes*)lbl_80272020;
        order = *(const ColAxisOrder*)lbl_80272044;
        PSMTXIdentity(combined);
        for (i = 0, axisOrder = order.index; i < 3; i++, axisOrder++) {
            PSMTXRotAxisRad(rotation, &axes.axis[*axisOrder],
                            *(f32*)(entry + 0xC + *axisOrder * 4));
            PSMTXConcat(rotation, combined, combined);
        }
        PSMTXCopy(combined, result);
        PSMTXConcat(out, result, out);
        PSMTXTransApply(out, out, *(f32*)(entry + 0),
                       *(f32*)(entry + 4), *(f32*)(entry + 8));
    }
    return 1;
}
#pragma pop

/* 0x8010CBC0 | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_8010CBC0(void) {
    return *(u32*)&lbl_80404C68;
}
#pragma pop

/* 0x8010CBD0 | 0x34 */
void* GScolsys2GetCurFloor(void) {
    s32 layer;

    layer = COL_LAYER_IDX;
    if (layer < 0 || layer >= GSCOLSYS_MAX_LAYERS) {
        return NULL;
    }
    return COL_LAYER_PTR(layer);
}

/* 0x8010CC04 | 0x50 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GScolsys2UnloadCCD(void) {
    /* TODO: match -- 80 bytes at 0x8010CC04 */
}
#pragma pop

/* 0x8010CC54 | 0x118 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010CC54(void) {
    /* TODO: match -- 280 bytes at 0x8010CC54 */
}
#pragma pop

/* 0x8010CD6C | 0x98 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010CD6C(void) {
    /* TODO: match -- 152 bytes at 0x8010CD6C */
}
#pragma pop

/* 0x8010CE04 | 0x1E0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010CE04(void) {
    /* TODO: match -- 480 bytes at 0x8010CE04 */
}
#pragma pop

/* 0x8010CFE4 | 0x54 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010CFE4(void) {
    /* TODO: match -- 84 bytes at 0x8010CFE4 */
}
#pragma pop

/* 0x8010D038 | 0x2C */
s32 fn_8010D038(void) {
    s32 layer;

    layer = COL_LAYER_IDX;
    if (layer < 0) {
        return 0;
    }
    COL_LAYER_IDX = layer - 1;
    return 1;
}

/* 0x8010D064 | 0x10C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010D064(void) {
    /* TODO: match -- 268 bytes at 0x8010D064 */
}
#pragma pop

/* 0x8010D170 | 0x9C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010D170(void) {
    /* TODO: match -- 156 bytes at 0x8010D170 */
}
#pragma pop

/* 0x8010D20C | 0x1BC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010D20C(void) {
    /* TODO: match -- 444 bytes at 0x8010D20C */
}
#pragma pop

/* 0x8010D3C8 | 0x50C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GScolsys2Draw(void) {
    /* TODO: match -- 1292 bytes at 0x8010D3C8 */
}
#pragma pop

/* 0x8010D8D4 | 0x52C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010D8D4(void) {
    /* TODO: match -- 1324 bytes at 0x8010D8D4 */
}
#pragma pop

/* 0x8010DE00 | 0xF0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010DE00(void) {
    /* TODO: match -- 240 bytes at 0x8010DE00 */
}
#pragma pop

/* 0x8010DEF0 | 0x248 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010DEF0(void) {
    /* TODO: match -- 584 bytes at 0x8010DEF0 */
}
#pragma pop
