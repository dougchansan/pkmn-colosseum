/**
 * @file gs_model.c
 * @brief GSmodel -- 3D model management system.
 *
 * This module sits between GSfloor (0x80101910) and GScolsys (0x8010C220)
 * in the link order, managing 3D model loading, animation, and resource
 * lifecycle for the Genius Sonority engine.
 *
 * Decompiled from 145 functions in range 0x80101910 - 0x8010C220.
 *
 * Selected functions:
 *   fn_80101910 (GSmodel_FindLoadedResource)
 *   fn_801019F8 (GSmodel_ClearResourceTable)
 *   fn_80101A28 (GSmodel_GetResourceCount)
 *   fn_80101A4C (GSmodel_GetResourceByIndex)
 *   fn_80101A70 (GSmodel_GetResourceHandle)
 *   fn_80101A9C (GSmodel_SetResourceActive)
 *   fn_80101AC4 (GSmodel_AllocModelSlot)
 *   fn_80101B34 (GSmodel_FreeModelSlot)
 *   fn_80101B90 (GSmodel_LoadFromFSYS)
 *   fn_80101D8C (GSmodel_SetupJoints)
 *   fn_80102004 (GSmodel_GetJointCount)
 *   fn_801028B0 (GSmodel_AttachAnimation)
 *   fn_80102F38 (GSmodel_UpdateAnimation)
 *   fn_801074D4 (GSmodel_RenderModel)
 *   fn_80108580 (GSmodel_BuildDisplayList)
 *   fn_80108C14 (GSmodel_ProcessSkinning)
 *
 * Architecture:
 *   - Model data is stored in a BSS table at lbl_80402518 (0x2400 bytes)
 *   - Each model slot is 0x48 bytes (table has 0x20 * 3 = 0x60 slots)
 *   - Models reference GSmem handles for their data
 *   - Animation is driven by per-frame update calls
 *   - Joint hierarchy is used for skeletal animation
 *   - Display list caching for static geometry
 *
 * Code patterns observed:
 *   - fn_80101910: Unrolled loop searching 4 slots per iteration
 *     (stride 0x48, checks offset 0x44 for ref count, offset 0x40
 *      for resource ID match). Uses bdnz with ctr=0x20.
 *   - fn_801019F8: memset(lbl_80402518, 0, 0x2400) to clear table
 *   - Many functions access lbl_8047AA80 (GSgfx state pointer)
 *     indicating tight integration with the renderer
 *   - Float operations (fmr, fadds, fmuls) for animation interpolation
 *
 * No debug strings reference this range directly -- the module operates
 * silently, using asserts only through GSgfx/GSmem error paths.
 *
 * Address range: 0x80101910 - 0x8010C220 (44KB, 145 functions)
 */

#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  fn_800DD970(const char* fmt, ...);         /* OSReport / GSlog */

/* GSmem */
extern u16   fn_800E3534(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void* fn_800E24B0(u16 handle);                   /* GSmemLock */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */

/* GSgfx state */
extern u8 lbl_8047AA80[];  /* GSgfx state pointer (via sda21) */

/* Matrix math */
extern void  fn_800A2D38(void);                         /* MTXIdentity */
extern void  fn_800A2D64(void* mtxA, void* mtxB);      /* MTXConcat */
extern void  fn_800A37CC(void* mtx, void* vec, void* out); /* MTXMultVec3 */

/* Model resource table (BSS) */
extern u8 lbl_80402518[];  /* model resource table -- 0x2400 bytes */

/* Global model system state block */
extern u8 lbl_80404ACC[];

typedef struct GSModelStateHeader {
    u16 count;          /* 0x00 */
    u16 entryHandle;    /* 0x02 */
    u32 unk_04;         /* 0x04 */
    void* entries;      /* 0x08 */
} GSModelStateHeader;

#define GS_MODEL_STATE ((GSModelStateHeader*)lbl_80404ACC)

/* Resource index table */
extern u32 lbl_80478B20;   /* max resource index (sda21) */
extern u8  lbl_80315690[]; /* resource table, 8-byte entries */

/* Additional externs used by various functions */
extern void  fn_800E4BF4(u32);     /* GSmem release/unref */
extern void  fn_800D2738(void);
extern void* fn_8005DA18(void);    /* linked list head */
extern void* fn_8005D934(s16 idx); /* node by index */
extern void* fn_8005D7F8(s32);
extern u16   fn_8005D798(void*, s32);
extern void* fn_8005D858(s32);
extern int   fn_80166A28(u16);
extern s32   fn_800F037C(void);    /* poll/yield -- 0 if pending */
extern void  _threadSwitch(void);    /* yield */
extern u32   fn_800BE31C(void);    /* rand or tick */
extern u32   fn_800B8FD8(void*);   /* register fn, returns handle */
extern void  fn_800BD91C(s32, s32);
extern void  fn_800B8C58(s32);
extern void  GSgfxBeginBackFBCapture(u32, void*, void*);
extern u32   GStextureCreate(s32, s32, s32, s32, s32);
extern u32   fn_800EC1BC(u32);
extern void  fn_800ECCA8(u32, u32);
extern void  fn_800EC9DC(u32, f32);
extern void  fn_800EC990(u32);
extern void  fn_801DB100(u32);
extern u32   OSGetTick(void);

/* ===================================================================
 * Constants
 * =================================================================== */

/** Size of the model resource table in bytes */
#define GSMODEL_TABLE_SIZE      0x2400

/** Size of one model resource slot */
#define GSMODEL_SLOT_SIZE       0x48

/** Maximum number of model slots (0x2400 / 0x48 = ~0x80, but used as 0x60) */
#define GSMODEL_MAX_SLOTS       0x60

/** Number of slots checked per unrolled iteration */
#define GSMODEL_UNROLL_COUNT    3

/* ===================================================================
 * Model resource slot structure
 * =================================================================== */

/**
 * Each model resource occupies a 0x48-byte slot in the table.
 * Inferred from disassembly access patterns.
 */
typedef struct GSModelSlot {
    /* 0x00 */ u32  flags;          /**< slot state flags */
    /* 0x04 */ u16  memHandle;      /**< GSmem handle for model data */
    /* 0x06 */ u16  animHandle;     /**< GSmem handle for animation data */
    /* 0x08 */ void* dataPtr;       /**< resolved pointer to model data */
    /* 0x0C */ void* animPtr;       /**< resolved pointer to animation */
    /* 0x10 */ u32  jointCount;     /**< number of joints in skeleton */
    /* 0x14 */ void* jointTable;    /**< pointer to joint hierarchy */
    /* 0x18 */ f32  animFrame;      /**< current animation frame */
    /* 0x1C */ f32  animSpeed;      /**< playback speed multiplier */
    /* 0x20 */ f32  animEnd;        /**< animation end frame */
    /* 0x24 */ u32  animFlags;      /**< loop, pingpong, etc. */
    /* 0x28 */ void* displayList;   /**< cached GX display list */
    /* 0x2C */ u32  dlSize;         /**< display list size */
    /* 0x30 */ void* skinWeights;   /**< vertex skinning weight table */
    /* 0x34 */ u32  vertexCount;    /**< number of vertices */
    /* 0x38 */ void* materialPtr;   /**< pointer to material data */
    /* 0x3C */ void* texturePtr;    /**< pointer to texture data */
    /* 0x40 */ u32  resourceId;     /**< resource ID for lookup */
    /* 0x44 */ u32  refCount;       /**< reference count (0 = free) */
} GSModelSlot;

/* ==================================================================
 * fn_80101910 -- GSmodel_FindLoadedResource
 *
 * Search the model resource table for a loaded resource matching
 * the resource ID in r3->offset_0x40. Uses an unrolled loop
 * checking 3 consecutive slots per iteration (via bdnz, ctr=0x20).
 *
 * If found, decrements the reference count and returns 1.
 * If not found after 0x60 slots, returns 1 (all searched).
 *
 * From disassembly (0x80101910, 0xE8 bytes):
 *   lis r4, lbl_80402518@ha
 *   li r0, 0x20              ; 32 iterations
 *   addi r4, r4, lbl_80402518@l
 *   mtctr r0
 *   ; ... unrolled: check slot, slot+0x48, slot+0x90, advance by 0xD8
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSmodel_FindLoadedResource(void* query) {
    /* TODO: match -- 232 bytes at 0x80101910 */
}
#pragma pop

/* ==================================================================
 * fn_801019F8 -- GSmodel_ClearResourceTable
 *
 * Clear the entire model resource table by zeroing 0x2400 bytes.
 *
 * From disassembly (0x801019F8, 0x30 bytes):
 *   lis r3, lbl_80402518@ha
 *   li r4, 0x0
 *   addi r3, r3, lbl_80402518@l
 *   li r5, 0x2400
 *   bl memset
 * ================================================================== */
void GSmodel_ClearResourceTable(void) {
    memset(lbl_80402518, 0, GSMODEL_TABLE_SIZE);
}

/* ==================================================================
 * fn_801028B0 -- GSmodel_AttachAnimation
 *
 * Attach an animation resource to a loaded model. Sets up the
 * joint hierarchy traversal and initializes playback state.
 * 1572 bytes -- one of the larger functions.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSmodel_AttachAnimation(void* model, void* animData, f32 startFrame) {
    /* TODO: match -- 1572 bytes at 0x801028B0 */
}
#pragma pop

/* ==================================================================
 * fn_80102F38 -- GSmodel_UpdateAnimation
 *
 * Advance animation playback by one frame. Handles looping, speed
 * scaling, and blend transitions. 1356 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSmodel_UpdateAnimation(void* model) {
    /* TODO: match -- 1356 bytes at 0x80102F38 */
}
#pragma pop

/* ==================================================================
 * fn_801074D4 -- GSmodel_RenderModel
 *
 * Render a model using its display list and current transform.
 * Largest function in this module at 2468 bytes. Sets up GX state,
 * binds textures and materials, then executes the display list.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSmodel_RenderModel(void* model) {
    /* TODO: match -- 2468 bytes at 0x801074D4 */
}
#pragma pop

/* ==================================================================
 * fn_80108580 -- GSmodel_BuildDisplayList
 *
 * Build a GX display list for a model's static geometry. The
 * resulting display list is cached in the model slot for fast
 * re-rendering. 1684 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSmodel_BuildDisplayList(void* model) {
    /* TODO: match -- 1684 bytes at 0x80108580 */
}
#pragma pop

/* ==================================================================
 * fn_80108C14 -- GSmodel_ProcessSkinning
 *
 * Compute skinned vertex positions from the joint hierarchy and
 * vertex weights. 1504 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSmodel_ProcessSkinning(void* model) {
    /* TODO: match -- 1504 bytes at 0x80108C14 */
}
#pragma pop

/* ===== Small accessor / utility functions ===== */

/* fn_80101A28 */
u32 GSmodel_GetResourceCount(void) {
    fn_800D2738();
    return 1;
}

/* fn_80101A4C */
void* GSmodel_GetResourceByIndex(u32 index) {
    fn_800E4BF4(0);
    return (void*)1;
}

/* fn_80102004 */
u32 GSmodel_GetJointCount(void* model) {
    return *(u32*)(lbl_80404ACC + 0x94);
}

/* ===================================================================
 * Generated: 4 pattern-matched + 131 stubs
 * Range: 0x80101910 - 0x8010C220
 * =================================================================== */

extern u32 lbl_8047ACF0;
extern u32 lbl_8047ACF4;  /* function pointer for callback */
extern u32 lbl_8047ACF8;  /* saved tick */
extern u32 lbl_8047ACEC;  /* tick counter */
extern u32 lbl_8047ACE8;  /* tick base */
extern u8 lbl_80478B28;   /* max slot byte */
extern u8 lbl_8047AD20;
extern u8 lbl_8047AD21;
extern u8 lbl_8047AD22;
extern u8 lbl_8047AD23;
extern u8 lbl_8047AD24;
extern f32 lbl_8047AD2C;
extern f32 lbl_8047AD30;
extern f32 lbl_8047AD34;
extern f32 lbl_8047AD38;
extern f32 lbl_8047AD3C;
extern u32 lbl_8047AD28;
extern u16 lbl_8047AD18;  /* GSmem handle */
extern u8* lbl_8047AD1C;  /* object pool pointer */
extern f32 lbl_8047CDC0;  /* sdata2: float constant */
extern f32 lbl_8047CDC4;  /* sdata2: float constant */
extern u16 lbl_8047CDE0;  /* sdata2: */
extern u16 lbl_8047CDE4;  /* sdata2: */
extern f32 lbl_8047CD80;  /* sdata2: float constant */
extern f32 lbl_8047CD84;  /* sdata2: float constant */
extern f32 lbl_8047CD88;  /* sdata2: float constant */
extern f32 lbl_8047CD8C;  /* sdata2: float constant */
extern f32 lbl_8047CD90;  /* sdata2: float constant */
extern f32 lbl_8047CD94;  /* sdata2: float constant */
extern f64 lbl_8047CD98;  /* sdata2: double constant */
extern f32 lbl_8047CE3C;  /* sdata2: float constant */
extern f32 lbl_8047CE50;  /* sdata2: float constant */
extern f32 lbl_8047CE5C;  /* sdata2: float constant */
extern f32 lbl_8047CE70;  /* sdata2: float constant */
extern u8  lbl_80404A98[];  /* table for display */
extern u8  lbl_80271E10[];  /* format string */
extern u8  lbl_80271E4C[];  /* format string */
extern u8  lbl_80271EE8[];  /* format string */
extern u8  lbl_80271F18[];  /* format string */
extern u8  lbl_8035B060[];  /* module name string */
extern u8  lbl_8035B3F0[];  /* module name string */

/* Additional external functions (not already declared above) */
extern void fn_800BF74C(void);
extern void fn_800D9ED8(s32);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D9B58(f32, f32, f32, f32);
extern void fn_800DA4C4(s32, s32, s32);
extern void fn_800DA2BC(s32, s32, s32);
extern void fn_800DA1E8(s32, s32, s32);
extern void fn_800DA028(s32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D6728(void);

/* Forward declarations for functions defined later in this TU */
extern u8    fn_80109718(u8 param);
extern void  fn_80104828(void* ptr, u32 flags);
extern void* fn_80104704(s32 param);
extern s32   fn_80109884(void);
extern void  fn_80109220(void* node, u32 enable);
extern void  fn_801043A4(s32 param);
extern void  fn_801045A8(void* p, u8 flags);
extern void  fn_80104160(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8    fn_80109664(u8 param);
extern void  fn_801096AC(f32 f1, f32 f2);
extern u8    fn_8010977C(u32 param);
extern void  fn_80109764(void);
extern u32   fn_801046B8(void);
extern void* fn_80105624(void);
extern void* fn_8005D830(u32 idx);
extern void* fn_801046C8(void* head, s32 key);
extern void  fn_801026A4(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
extern u8    fn_801096E8(u8 val);
extern u8    fn_801096F8(u8 val);
extern u32   fn_800D3088(void);
extern u8    lbl_80404B68[];  /* scratch table for fn_80107F38, fn_801081F8 */
extern u8    lbl_80404B8C[];  /* scratch table for fn_801080CC */
extern u8    lbl_8047AD10;     /* resource request gate byte (sda21) — authoritative decl, use as-is */

/* 0x801019F8 | 0x30 */
void fn_801019F8(void) {
    memset(lbl_80402518, 0, 0x2400);
}

/* 0x80101A70 | 0x2C */
void* fn_80101A70(u32 index) {
    struct Entry {
        void* start;
        void* end;
    };
    struct Entry* table;
    if (index >= lbl_80478B20) { return (void*)0; }
    table = (struct Entry*)lbl_80315690;
    return table[index].end;
}

/* 0x80101A9C | 0x28 */
/* Returns value at entry[index].field0 (lwzx from offset 0) in the 8-byte table */
void* fn_80101A9C(u32 index) {
    if (index >= lbl_80478B20) { return (void*)0; }
    return *(void**)(lbl_80315690 + index * 8);
}

/* 0x80101AC4 | 0x70 */
extern u32 fn_800E0C54(void);  /* random or tick */
u32 fn_80101AC4(u32 param) {
    u32 r30 = param;
    u32 r31 = (u32)fn_80101A70(r30);
    r30 = (u32)fn_80101A9C(r30);
    if (r31 != r30) {
        u32 tick = fn_800E0C54();
        u32 range = r31 - r30;
        s32 mod = (s32)(u16)tick % (s32)(range + 1);
        return r30 + (u32)mod;
    }
    return r31;
}

/* 0x80101B34 | 0x54 */
void fn_80101B34(u32 param) {
    u32 r31 = param;
    if ((u16)r31 == 3) {
        lbl_8047ACF8 = fn_800BE31C();
    }
    {
        void (*r12)(u32) = (void (*)(u32))lbl_8047ACF4;
        if (r12) {
            r12(r31);
        }
    }
}

/* 0x80101B88 | 0x8 | sda_setter */
void fn_80101B88(u32 val) { lbl_8047ACF0 = val; }

/* 0x80101B90 | 0x1CC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80101B90(void) {
    /* TODO: match -- 460 bytes at 0x80101B90 */
}
#pragma pop

/* 0x80101D5C | 0x30 */
void fn_80101D5C(void) {
    if ((s32)lbl_8047ACF0 != 0) {
        fn_800B8C58(3);
    }
}

/* 0x80101D8C | 0x22C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80101D8C(void) {
    /* TODO: match -- 556 bytes at 0x80101D8C */
}
#pragma pop

/* 0x80101FB8 | 0x4C */
void fn_80101FB8(u8 param) {
    if (param < 1) { param = 1; }
    lbl_80478B28 = param;
    lbl_8047ACF4 = fn_800B8FD8((void*)fn_80101B34);
    fn_800BD91C(0x22, 0x16);
}

/* 0x80102014 | 0x24 */
#pragma push
#pragma scheduling off
void fn_80102014(void) {
    fn_80109718(0);
}
#pragma pop

/* 0x80102038 | 0x34 */
#pragma push
#pragma scheduling off
void fn_80102038(f32 f1) {
    f32 f2;
    f2 = f1;
    fn_801096AC(lbl_8047CDC0, f2);
    fn_80109664(1);
    fn_80109764();
}
#pragma pop

/* 0x8010206C | 0x54 */
void fn_8010206C(f32 param) {
    f32 f31 = param;
    fn_8010977C(1);
    fn_801096F8(1);
    fn_801096E8(0);
    fn_801096AC(lbl_8047CDC4, f31);
}

/* 0x801020C0 | 0x78 */
#pragma push
#pragma peephole off
s32 fn_801020C0(void) {
    s32 r31 = 0;
    void* r3 = fn_8005DA18();
    if (r3 == (void*)0) { return 0; }
    {
        s32 r0 = (s16)*(s16*)((u8*)r3 + 0x4);
        do {
            void* node = fn_8005D934((s16)r0);
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                r31++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret_r31;
            }
            r0 = (s16)*(s16*)((u8*)node + 0x18);
        } while (1);
    }
_ret_r31:
    return r31;
}
#pragma pop

/* 0x80102138 | 0xC0 */
#pragma push
#pragma peephole off
s32 fn_80102138(void* unused, u32 param) {
    u32 r29 = param;
    void* r3 = fn_8005DA18();
    if (r3 == (void*)0) { return -3; }
    {
        s32 r31 = (s16)*(s16*)((u8*)r3 + 0x4);
        s32 r30 = 0;
    loop:
        {
            void* node = fn_8005D934((s16)r31);
            if ((u32)(s16)r31 == (u32)r29) {
                if (((u32)*(volatile u8*)node >> 7) & 1) {
                    return r30;
                }
                return -1;
            }
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                r30++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret_m2;
            }
            r31 = (s16)*(s16*)((u8*)node + 0x18);
            goto loop;
        }
    _ret_m2:
        return -2;
    }
}
#pragma pop

/* 0x801021F8 | 0x5C */
void fn_801021F8(void* p, u32 val) {
    u32 r30 = val;
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) { return; }
    {
        void* r31 = *(void**)((u8*)r3 + 0x20);
        while (r31 != (void*)0) {
            fn_80109220(r31, r30);
            r31 = *(void**)r31;
        }
    }
}

/* 0x80102254 | 0x64 */
#pragma push
#pragma peephole off
void fn_80102254(void* p, u32 enable) {
    u32 r31 = enable;
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) { return; }
    if ((u8)r31 != 0) {
        s8 r0 = (s8)(*(u8*)r3 | 2);
        *(s8*)r3 = r0;
    } else {
        s8 r0 = (s8)(*(u8*)r3 & ~2);
        *(s8*)r3 = r0;
    }
}
#pragma pop

/* 0x801022B8 | 0xE0 */
#pragma push
#pragma peephole off
void* fn_801022B8(void* p, u32 target) {
    void* r29 = p;
    void* r3 = fn_80104704((s32)p);
    s32 r31;
    if (r3 != (void*)0) {
        s8 r4 = (s8)*(u8*)((u8*)r3 + 0x95);
        s8 r0 = (s8)*(u8*)((u8*)r3 + 0x94);
        r31 = (s32)r4 + (s32)r0;
    } else {
        r31 = -1;
    }
    if (r31 == -1) { return (void*)0; }
    { extern void* fn_8005DA18(void*); r3 = fn_8005DA18(r29); }
    if (r3 == (void*)0) { return (void*)0; }
    {
        s32 r30 = (s16)*(s16*)((u8*)r3 + 0x4);
        s32 r29b = 0;
        void* result = (void*)0;
        do {
            void* node = fn_8005D934((s16)r30);
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                if (r29b == r31) {
                    return (void*)(s32)(s16)r30;
                }
                r29b++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret0_loop;
            }
            r30 = (s16)*(s16*)((u8*)node + 0x18);
        } while (1);
    _ret0_loop:
        return (void*)0;
        (void)result;
    }
    (void)r29;
}
#pragma pop

/* 0x80102398 | 0x4C */
#pragma push
#pragma peephole off
s32 fn_80102398(void* p, u32 val) {
    u32 r31 = val;
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) goto ret_m1;
    *(s8*)((u8*)r3 + 0x95) = (s8)r31;
    goto ret0;
ret_m1:
    return -1;
ret0:
    return 0;
}
#pragma pop

/* 0x801023E4 | 0x44 */
s32 fn_801023E4(void* p) {
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) goto ret_m1;
    {
        s8 r4 = (s8)*(u8*)((u8*)r3 + 0x95);
        s8 r0 = (s8)*(u8*)((u8*)r3 + 0x94);
        return (s32)r4 + (s32)r0;
    }
ret_m1:
    return -1;
}

/* 0x80102428 | 0x98 */
#pragma push
#pragma peephole off
s32 menuCloseSync(void* p, u8 flag) {
    void* r31 = p;
    if ((u8)flag != 0) {
        goto loop;
    loop: {
            void* r3 = fn_80104704((s32)r31);
            if (r3 != (void*)0) goto step2;
            return 0;
        step2:
            if ((u32)fn_800F037C() != 0) goto do_yield;
            fn_800DD970((const char*)lbl_80271E10, (const char*)lbl_8035B060, r31);
            goto ret0;
        do_yield:
            _threadSwitch();
            goto loop;
        }
    } else {
        void* r3 = fn_80104704((s32)r31);
        s32 r0 = (s32)r3;
        s32 neg = -r0;
        return (u32)(neg | r0) >> 31;
    }
ret0:
    return 0;
}
#pragma pop

/* 0x801024C0 | 0x28 */
#pragma push
#pragma scheduling off
void fn_801024C0(void) {
    fn_80104828(0, 4);
}
#pragma pop

/* 0x801024E8 | 0x28 */
#pragma push
#pragma scheduling off
void fn_801024E8(void) {
    fn_80104828(0, 4);
}
#pragma pop

/* 0x80102510 | 0x58 */
#pragma push
#pragma peephole off
void fn_80102510(s32 p) {
    s32 r31 = p;
    if (r31 == 0) {
        r31 = (s32)fn_801046B8();
    }
    {
        void* r3 = fn_80104704(r31);
        if (r3 != (void*)0) {
            fn_80104828(r3, 0);
            fn_80104704(r31);
        }
    }
}
#pragma pop

/* 0x80102568 | 0xB8 */
s32 fn_80102568(void* p, u32 mode, u8 wait) {
    void* r29 = p;
    void* r30 = (void*)mode;
    u8 r31 = (u8)wait;
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) { return 1; }
    fn_80104828(r29, (u32)r30);
    if ((u8)r31 != 0) {
        while (1) {
            r3 = fn_80104704((s32)r29);
            if (r3 == (void*)0) { return 0; }
            if (fn_800F037C() != 0) {
                _threadSwitch();
                continue;
            }
            fn_800DD970((const char*)lbl_80271E10, (const char*)lbl_8035B060, r29);
            return 0;
        }
    }
    r3 = fn_80104704((s32)r29);
    return 0;
}

/* 0x80102620 | 0x2C */
s32 fn_80102620(s32 param) {
    s32 r = (s32)fn_80104704(param);
    return (u32)((-r) | r) >> 31;
}

/* 0x8010264C | 0x58 */
#pragma push
#pragma peephole off
void fn_8010264C(void* p, void* q) {
    void* r30 = p;
    void* r31 = q;
    fn_801026A4(r30, fn_801046B8(), 0, 0, r31, 0);
}
#pragma pop

/* 0x801026A4 | 0x1C4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801026A4(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...) {
    /* TODO: match -- 452 bytes at 0x801026A4 */
    (void)p; (void)r4; (void)r5; (void)r6; (void)r7; (void)r8;
}
#pragma pop

/* 0x80102868 | 0x48 */
#pragma push
#pragma peephole off
void fn_80102868(void* p, s16 a, s16 b) {
    s16 r30 = a;
    s16 r31 = b;
    void* r3 = fn_80104704((s32)p);
    if (r3 == (void*)0) { return; }
    *(s16*)((u8*)r3 + 0x84) = r30;
    *(s16*)((u8*)r3 + 0x86) = r31;
}
#pragma pop

/* 0x80102ED4 | 0x64 */
#pragma push
#pragma peephole off
void fn_80102ED4(void* p) {
    void* r31;
    if ((r31 = p) == (void*)0) { return; }
    {
        void* base = fn_80105624();
        u16 r3 = *(u16*)((u8*)base + 0x4);
        s32 bits = (s32)r3;
        if (bits & 0x10) {
            *(u8*)((u8*)r31 + 0x98) = 1;
        }
        if (bits & 0x20) {
            *(u8*)((u8*)r31 + 0x98) = 1;
            *(u8*)((u8*)r31 + 0x99) = 1;
        }
    }
}
#pragma pop

/* 0x80103484 | 0x58 */
#pragma push
#pragma peephole off
void fn_80103484(void* p, void* q) {
    s32 r31 = (s32)q;
    void* r3 = fn_8005DA18();
    void* r3c = fn_8005D7F8((s32)(*(u8*)((u8*)r3 + 0x0) & 0x7));
    if (r3c == (void*)0) { return; }
    {
        u32 v = (u32)(u16)fn_8005D798(r3c, r31);
        if (v == 0) { return; }
        fn_80166A28((u16)v);
    }
}
#pragma pop

/* 0x801034DC | 0x138 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801034DC(void) {
    /* TODO: match -- 312 bytes at 0x801034DC */
}
#pragma pop

/* 0x80103614 | 0x2E4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80103614(void) {
    /* TODO: match -- 740 bytes at 0x80103614 */
}
#pragma pop

/* 0x801038F8 | 0x2B0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801038F8(void) {
    /* TODO: match -- 688 bytes at 0x801038F8 */
}
#pragma pop

/* 0x80103BA8 | 0x108 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80103BA8(void) {
    /* TODO: match -- 264 bytes at 0x80103BA8 */
}
#pragma pop

/* 0x80103CB0 | 0x10 */
u8 fn_80103CB0(void) {
    return lbl_80404ACC[0x92];
}

/* 0x80103CC0 | 0x18 */
u8 fn_80103CC0(u8 val) {
    u8 old = lbl_80404ACC[0x92];
    lbl_80404ACC[0x92] = val;
    return old;
}

/* 0x80103CD8 | 0x190 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80103CD8(void) {
    /* TODO: match -- 400 bytes at 0x80103CD8 */
}
#pragma pop

/* 0x80103E68 | 0x44 -- read and maybe lookup from table */
#pragma push
#pragma peephole off
u32 fn_80103E68(u16 idx) {
    struct { volatile u16 a; u8 _pad[2]; volatile u16 b; } sp;
    u16 r3 = lbl_8047CDE4;
    sp.a = r3;
    if (idx >= 0xc) {
        sp.b = r3;
    } else {
        sp.b = ((u16*)lbl_80404A98)[idx];
    }
    return (u32)sp.b << 16;
}
#pragma pop

/* 0x80103EAC | 0x48 */
#pragma push
#pragma peephole off
u32 fn_80103EAC(u16 idx, u16* out) {
    u16 g = lbl_8047CDE0;
    u32 n = idx & 0xFFFFu;
    volatile u16 sp[3];
    sp[2] = g;
    if (n >= 0xc) {
        return (u32)g << 16;
    }
    {
        u16 old = ((u16*)lbl_80404A98)[n];
        sp[0] = old;
        ((u16*)lbl_80404A98)[n] = *out;
        return (u32)old << 16;
    }
}
#pragma pop

/* 0x80103EF4 | 0x80 */
void fn_80103EF4(void) {
    if (0 >= 0xc) { return; }
    {
        u8* p = lbl_80404A98;
        u8* q = p + 0xc;
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 0;
        p[4] = 0; p[5] = 0; p[6] = 0; p[7] = 0;
        p[8] = 0; p[9] = 0; p[10] = 0; p[11] = 0;
        q[0] = 0; q[1] = 0; q[2] = 0; q[3] = 0;
        q[4] = 0; q[5] = 0; q[6] = 0; q[7] = 0;
        q[8] = 0; q[9] = 0; q[10] = 0; q[11] = 0;
    }
}

/* 0x80103F74 | 0x70 */
#pragma push
#pragma peephole off
void fn_80103F74(void* head, u16 key, u32 data) {
    u32 r29 = data;
    if (head == (void*)0) { return; }
    {
        void* r30 = *(void**)((u8*)head + 0x1c);
        s32 r31 = (s32)(u16)key;
        while (r30 != (void*)0) {
            s32 r0 = (s32)*(s16*)((u8*)r30 + 0x6);
            if (r0 == r31) {
                fn_80109220(r30, r29);
            }
            r30 = *(void**)r30;
        }
    }
}
#pragma pop

/* 0x80103FE4 | 0x18 */
void* fn_80103FE4(void* ptr) {
    if (ptr) {
        return *(void**)((u8*)ptr + 0xB0);
    }
    return (void*)0;
}

/* 0x80103FFC | 0xA4 */
#pragma push
#pragma peephole off
void* fn_80103FFC(void* p, s32 size) {
    s32 r31 = size;
    void* r30 = p;
    if (r30 == (void*)0) { return (void*)0; }
    if (*(u16*)((u8*)r30 + 0xac) != 0) {
        fn_800E24B0(*(u16*)((u8*)r30 + 0xac));
        fn_800E209C(*(u16*)((u8*)r30 + 0xac));
        *(u32*)((u8*)r30 + 0xb0) = 0;
    }
    if (r31 <= 0) { return (void*)0; }
    {
        u16 h = fn_800E3534((u32)r31);
        *(u16*)((u8*)r30 + 0xac) = h;
        if (*(u16*)((u8*)r30 + 0xac) != 0) {
            void* ptr = fn_800E27B0(*(u16*)((u8*)r30 + 0xac));
            *(void**)((u8*)r30 + 0xb0) = ptr;
        } else {
            return (void*)0;
        }
    }
    return *(void**)((u8*)r30 + 0xb0);
}
#pragma pop

/* 0x801040A0 | 0x18 */
void* fn_801040A0(void* ptr) {
    if (ptr) {
        return (void*)((u8*)ptr + 0x9C);
    }
    return (void*)0;
}

/* 0x801040B8 | 0x18 */
void fn_801040B8(void* ptr, u32 idx, u32 val) {
    if (ptr == (void*)0) { return; }
    ((u32*)((u8*)ptr + 0x60))[idx] = val;
}

/* 0x801040D0 | 0x20 */
u32 fn_801040D0(void* ptr, u32 idx) {
    if (ptr) {
        return ((u32*)((u8*)ptr + 0x60))[idx];
    }
    return 0;
}

/* 0x801040F0 | 0x70 */
/* fn_8005D858 already declared above */
void fn_801040F0(void* p, void* a, void* b, u16 key, u32 data) {
    void* r27 = p;
    void* r28 = a;
    void* r29 = b;
    u16 r30 = key;
    u32 r31 = data;
    if ((u16)r30 != 0) {
        void* r6 = fn_8005D858((s32)(u16)r30);
        fn_80104160(r27, r28, *(s16*)((u8*)r6 + 0xc), *(s16*)((u8*)r6 + 0xe), (s32)r29, (s32)(u16)r30, (s32)r31, -1);
    }
}

/* 0x80104160 | 0x1B8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104160(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10) {
    /* TODO: match -- 440 bytes at 0x80104160 */
    (void)r3; (void)r4; (void)r5; (void)r6; (void)r7; (void)r8; (void)r9; (void)r10;
}
#pragma pop

/* 0x80104318 | 0x8C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u8* fn_80104318(u8* arg) {
#pragma optimization_level 4
#pragma peephole off
    void* node;
    s32 idx;
    { extern void* fn_8005DA18(void*); node = fn_8005DA18(*(void**)(arg + 0x4)); }
    node = fn_8005D934(*(s16*)((u8*)node + 0x4));
    idx = 0;
    while (1) {
        if (((u32)*(volatile u8*)node >> 7) & 1) {
            if ((s8)*(s8*)(arg + 0x95) == idx) {
                return (u8*)node;
            }
            idx = idx + 1;
        }
        if (((u32)*(volatile u8*)node >> 6) & 1) {
            break;
        }
        node = fn_8005D934(*(s16*)((u8*)node + 0x18));
    }
    return (u8*)0;
}
#pragma pop

/* 0x801043A4 | 0x12C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801043A4(s32 param) {
    /* TODO: match -- 300 bytes at 0x801043A4 */
    (void)param;
}
#pragma pop

/* shared model-table lookup, inlined by the find-and-act helpers below */
static inline void* mdl_find(s32 param) {
    void* r;
    if (param <= 0) { return (void*)0; }
    r = *(void**)((u8*)lbl_80404ACC + 0xc);
    while (r != (void*)0) {
        if (*(s32*)((u8*)r + 0x4) == param) { return r; }
        r = *(void**)((u8*)r + 0x10);
    }
    return (void*)0;
}

/* 0x801044D0 | 0x60 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_801044D0(s32 param, u16* val) {
#pragma optimization_level 2
    void* node = mdl_find(param);
    if (node != (void*)0) {
        *(u16*)((u8*)node + 0x94) = *val;
        return 1;
    }
    return 0;
}
#pragma pop

/* 0x80104530 | 0x78 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104530(void) {
    /* TODO: match -- 120 bytes at 0x80104530 */
}
#pragma pop

/* 0x801045A8 | 0x110 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801045A8(void* p, u8 flags) {
    /* TODO: match -- 272 bytes at 0x801045A8 */
    (void)p; (void)flags;
}
#pragma pop

/* 0x801046B8 | 0x10 */
u32 fn_801046B8(void) {
    return *(u32*)(lbl_80404ACC + 0x4);
}

/* 0x801046C8 | 0x3C */
#pragma push
#pragma scheduling off
void* fn_801046C8(void* head, s32 key) {
    if (head == (void*)0) { return (void*)0; }
    {
        void* r3 = *(void**)((u8*)head + 0x1c);
        s32 r4 = (u16)key;
        while (r3 != (void*)0) {
            s16 r0 = *(s16*)((u8*)r3 + 0x6);
            if ((s32)r0 == r4) { return r3; }
            r3 = *(void**)r3;
        }
        return (void*)0;
    }
}
#pragma pop

/* 0x80104704 | 0x48 */
void* fn_80104704(s32 param) {
    if (param <= 0) { return (void*)0; }
    {
        void* r4 = *(void**)((u8*)lbl_80404ACC + 0xc);
        while (r4 != (void*)0) {
            s32 r0 = *(s32*)((u8*)r4 + 0x4);
            if (r0 == param) { return r4; }
            r4 = *(void**)((u8*)r4 + 0x10);
        }
        return (void*)0;
    }
}

/* 0x8010474C | 0xDC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010474C(void* obj) {
#pragma optimization_level 4
#pragma peephole off
    void* h;
    void* nx;
    { extern void* fn_8005DA18(void*); h = fn_8005DA18(*(void**)((u8*)obj + 0x4)); }
    if (*(u32*)((u8*)h + 0x14) != 0) {
        *(u8*)((u8*)obj + 0x1) = 5;
        (*(void (*)(void*))*(u32*)((u8*)h + 0x14))(obj);
    }
    if (obj != (void*)0) {
        if (*(void**)((u8*)obj + 0x14) == (void*)0) {
            *(void**)((u8*)lbl_80404ACC + 0xc) = *(void**)((u8*)obj + 0x10);
        } else {
            *(void**)((u8*)*(void**)((u8*)obj + 0x14) + 0x10) = *(void**)((u8*)obj + 0x10);
        }
        nx = *(void**)((u8*)obj + 0x10);
        if (nx != (void*)0) {
            *(void**)((u8*)nx + 0x14) = *(void**)((u8*)obj + 0x14);
        }
        { extern void fn_8010925C(void* head); fn_8010925C((u8*)obj + 0x1c); }
        { extern void fn_8010925C(void* head); fn_8010925C((u8*)obj + 0x20); }
        if (*(u16*)((u8*)obj + 0xac) != 0) {
            fn_800E24B0(*(u16*)((u8*)obj + 0xac));
            fn_800E209C(*(u16*)((u8*)obj + 0xac));
            *(u32*)((u8*)obj + 0xb0) = 0;
            *(u16*)((u8*)obj + 0xac) = 0;
        }
        *(u8*)((u8*)obj + 0x0) = 0;
        *(u32*)((u8*)obj + 0x4) = 0;
    }
}
#pragma pop

/* 0x80104828 | 0x26C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104828(void* ptr, u32 flags) {
    /* TODO: match -- 620 bytes at 0x80104828 */
}
#pragma pop

/* 0x80104A94 | 0x20C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104A94(void) {
    /* TODO: match -- 524 bytes at 0x80104A94 */
}
#pragma pop

/* 0x80104CA0 | 0x1E0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104CA0(void) {
    /* TODO: match -- 480 bytes at 0x80104CA0 */
}
#pragma pop

/* 0x80104E80 | 0x474 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80104E80(void) {
    /* TODO: match -- 1140 bytes at 0x80104E80 */
}
#pragma pop

/* 0x801052F4 | 0x11C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801052F4(void) {
    /* TODO: match -- 284 bytes at 0x801052F4 */
}
#pragma pop

/* 0x80105410 | 0xA8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105410(u16 count) {
#pragma optimization_level 4
#pragma peephole off
    extern u8 lbl_80271EC4[];
    extern void fn_80109358(void);
    u32 size;
    u16 handle;
    void* ptr;

    memset(lbl_80404ACC, 0, 0x9c);
    size = (u16)count * 0xb4;
    handle = fn_800E3534(size);
    GS_MODEL_STATE->entryHandle = handle;
    if ((u16)handle == 0) {
        fn_800DD970((const char*)lbl_80271EC4);
    } else {
        ptr = fn_800E27B0((u16)handle);
        GS_MODEL_STATE->entries = ptr;
        GS_MODEL_STATE->count = count;
        memset(ptr, 0, size);
        fn_80109358();
    }
}
#pragma pop

/* 0x801054B8 | 0x16C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801054B8(void) {
    /* TODO: match -- 364 bytes at 0x801054B8 */
}
#pragma pop

/* 0x80105624 | 0x10 */
void* fn_80105624(void) {
    return (void*)(lbl_80404ACC + 0x10);
}

/* 0x80105634 | 0x298 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105634(void) {
    /* TODO: match -- 664 bytes at 0x80105634 */
}
#pragma pop

/* 0x801058CC | 0x170 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801058CC(void) {
    /* TODO: match -- 368 bytes at 0x801058CC */
}
#pragma pop

/* 0x80105A3C | 0x1F4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105A3C(void) {
    /* TODO: match -- 500 bytes at 0x80105A3C */
}
#pragma pop

/* 0x80105C30 | 0x38 */
void fn_80105C30(void* p) {
    void* r31 = fn_801040A0(p);
    void* r3 = fn_80105624();
    *(u32*)((u8*)r31 + 0x8) = *(u16*)((u8*)r3 + 0x4);
}

/* 0x80105C68 | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105C68(void) {
    /* TODO: match -- 224 bytes at 0x80105C68 */
}
#pragma pop

/* 0x80105D48 | 0x134 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105D48(void) {
    /* TODO: match -- 308 bytes at 0x80105D48 */
}
#pragma pop

/* 0x80105E7C | 0x134 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105E7C(void) {
    /* TODO: match -- 308 bytes at 0x80105E7C */
}
#pragma pop

/* 0x80105FB0 | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105FB0(void) {
#pragma peephole off
#pragma scheduling off
    if ((u8)fn_80102620(0x10C)) {
        fn_80102568((void*)0x10C, 2, 0);
    }
    lbl_8047AD10 = 0;
}
#pragma pop

/* 0x80105FF8 | 0x88 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80105FF8(void) {
    /* TODO: match -- 136 bytes at 0x80105FF8 */
}
#pragma pop

/* 0x80106080 | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106080(void) {
    /* TODO: match -- 224 bytes at 0x80106080 */
}
#pragma pop

/* 0x80106160 | 0xE4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106160(void) {
    /* TODO: match -- 228 bytes at 0x80106160 */
}
#pragma pop

/* 0x80106244 | 0x150 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106244(void) {
    /* TODO: match -- 336 bytes at 0x80106244 */
}
#pragma pop

/* 0x80106394 | 0x14C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106394(void) {
    /* TODO: match -- 332 bytes at 0x80106394 */
}
#pragma pop

/* 0x801064E0 | 0xD8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801064E0(void) {
    /* TODO: match -- 216 bytes at 0x801064E0 */
}
#pragma pop

/* 0x801065B8 | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801065B8(void) {
    /* TODO: match -- 224 bytes at 0x801065B8 */
}
#pragma pop

/* 0x80106698 | 0x150 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106698(void) {
    /* TODO: match -- 336 bytes at 0x80106698 */
}
#pragma pop

/* 0x801067E8 | 0x14C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801067E8(void) {
    /* TODO: match -- 332 bytes at 0x801067E8 */
}
#pragma pop

/* 0x80106934 | 0xC8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106934(void) {
    /* TODO: match -- 200 bytes at 0x80106934 */
}
#pragma pop

/* 0x801069FC | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801069FC(void) {
    /* TODO: match -- 224 bytes at 0x801069FC */
}
#pragma pop

/* 0x80106ADC | 0x260 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106ADC(void) {
    /* TODO: match -- 608 bytes at 0x80106ADC */
}
#pragma pop

/* 0x80106D3C | 0x25C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106D3C(void) {
    /* TODO: match -- 604 bytes at 0x80106D3C */
}
#pragma pop

/* 0x80106F98 | 0x15C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80106F98(void) {
    /* TODO: match -- 348 bytes at 0x80106F98 */
}
#pragma pop

/* 0x801070F4 | 0x7C */
s32 fn_801070F4(s32 param) {
    void* r3 = fn_80104704(param);
    if (r3 == (void*)0) { return 0; }
    if (*(void**)((u8*)r3 + 0x24) != (void*)0) { return 1; }
    {
        void* node = *(void**)((u8*)r3 + 0x1c);
        while (node != (void*)0) {
            if (*(void**)((u8*)node + 0xc) != (void*)0) {
                if ((u8)*(u8*)((u8*)node + 0x46) == 0) { return 1; }
            }
            node = *(void**)((u8*)node + 0x0);
        }
        return 0;
    }
}

/* 0x80107170 | 0x60 */
s32 fn_80107170(s32 r3, s32 r31) {
    void* r3r = fn_80104704(r3);
    void* result = fn_801046C8(r3r, r31);
    if (result == (void*)0) { goto _ret0; }
    if (*(void**)((u8*)result + 0xc) == (void*)0) { goto _ret0; }
    if ((u8)*(u8*)((u8*)result + 0x46) != 0) { goto _ret0; }
    return 1;
_ret0:
    return 0;
}

/* 0x801071D0 | 0x304 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801071D0(void) {
    /* TODO: match -- 772 bytes at 0x801071D0 */
}
#pragma pop

/* 0x80107E78 | 0x60 */
#pragma push
#pragma peephole off
s32 fn_80107E78(void* r3, s32 r4, u16 r30) {
    void* r31 = fn_801046C8(r3, r4);
    if (r31 == (void*)0) { goto _ret0; }
    {
        void* r3b = fn_8005D830((u32)(u16)r30);
        if (*(void**)((u8*)r31 + 0xc) != r3b) { goto _ret0; }
        return 1;
    }
_ret0:
    return 0;
}
#pragma pop

/* 0x80107ED8 | 0x60 */
#pragma push
#pragma peephole off
s32 fn_80107ED8(s32 r3, u16 r30) {
    void* r31 = fn_80104704(r3);
    if (r31 == (void*)0) { goto _ret0; }
    {
        void* r3b = fn_8005D830((u32)(u16)r30);
        if (*(void**)((u8*)r31 + 0x24) != r3b) { goto _ret0; }
        return 1;
    }
_ret0:
    return 0;
}
#pragma pop

/* 0x80107F38 | 0x194 */
#pragma push
#pragma optimization_level 2
#pragma push
#pragma optimization_level 1
void fn_80107F38(s32 param, u32 key) {
    u32 r28 = key;
    void* r3 = fn_80104704(param);
    if (r3 == (void*)0) { return; }
    {
        u8* r31 = lbl_80404B68;
        u32 r30 = (u32)(u16)r28;
        void* r29 = *(void**)((u8*)r3 + 0x20);
        while (r29 != (void*)0) {
            if (r30 == 0) {
                *(u32*)((u8*)r29 + 0xc) = 0;
                *(u16*)((u8*)r29 + 0x10) = 0;
            } else {
                memset((u8*)r29 + 0xc, 0, 0x3c);
                *(void**)((u8*)r29 + 0xc) = fn_8005D830(r30);
            }
            *(s16*)(r31 + 0x0) = *(s16*)((u8*)r29 + 0x50);
            *(s16*)(r31 + 0x2) = *(s16*)((u8*)r29 + 0x52);
            *(u32*)(r31 + 0x4) = *(u32*)((u8*)r29 + 0x64);
            *(f32*)(r31 + 0xc) = *(f32*)((u8*)r29 + 0x68);
            *(f32*)(r31 + 0x10) = *(f32*)((u8*)r29 + 0x6c);
            *(u8*)(r31 + 0x20) = *(u8*)((u8*)r29 + 0x4);
            *(u32*)(r31 + 0x14) = *(u32*)((u8*)r29 + 0x58);
            *(s16*)(r31 + 0x18) = *(s16*)((u8*)r29 + 0x5c);
            *(s16*)(r31 + 0x1a) = *(s16*)((u8*)r29 + 0x5e);
            *(s16*)(r31 + 0x1c) = *(s16*)((u8*)r29 + 0x60);
            *(s16*)(r31 + 0x1e) = *(s16*)((u8*)r29 + 0x62);
            *(s16*)(r31 + 0x8) = *(s16*)((u8*)fn_8005D934(*(s16*)((u8*)r29 + 0x6)) + 0x2);
            *(s16*)(r31 + 0xa) = *(s16*)((u8*)fn_8005D934(*(s16*)((u8*)r29 + 0x6)) + 0x4);
            r28 = 0;
            while (r28 < fn_800D3088()) {
                fn_801074D4(r31, (u8*)r29 + 0xc);
                r28++;
            }
            *(s16*)((u8*)r29 + 0x50) = *(s16*)(r31 + 0x0);
            *(s16*)((u8*)r29 + 0x52) = *(s16*)(r31 + 0x2);
            *(u32*)((u8*)r29 + 0x64) = *(u32*)(r31 + 0x4);
            *(f32*)((u8*)r29 + 0x68) = *(f32*)(r31 + 0xc);
            *(f32*)((u8*)r29 + 0x6c) = *(f32*)(r31 + 0x10);
            *(u8*)((u8*)r29 + 0x4) = *(u8*)(r31 + 0x20);
            *(u32*)((u8*)r29 + 0x58) = *(u32*)(r31 + 0x14);
            *(s16*)((u8*)r29 + 0x5c) = *(s16*)(r31 + 0x18);
            *(s16*)((u8*)r29 + 0x5e) = *(s16*)(r31 + 0x1a);
            *(s16*)((u8*)r29 + 0x60) = *(s16*)(r31 + 0x1c);
            *(s16*)((u8*)r29 + 0x62) = *(s16*)(r31 + 0x1e);
            r29 = *(void**)r29;
        }
    }
}
#pragma pop
#pragma pop

/* 0x801080CC | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma peephole off
void fn_801080CC(s32 param, u32 key) {
    u32 r28 = key;
    void* r31 = fn_80104704(param);
    if (r31 == (void*)0) { goto _ret80CC; }
    if ((u32)(u16)r28 == 0) {
        *(u32*)((u8*)r31 + 0x24) = 0;
        *(u16*)((u8*)r31 + 0x28) = 0;
    } else {
        memset((u8*)r31 + 0x24, 0, 0x3c);
        *(void**)((u8*)r31 + 0x24) = fn_8005D830((u32)(u16)r28);
    }
    {
        u8* r29 = lbl_80404B8C;
        u8* r30;
        *(s16*)(r29 + 0x0) = *(s16*)((u8*)r31 + 0x84);
        *(s16*)(r29 + 0x2) = *(s16*)((u8*)r31 + 0x86);
        *(u32*)(r29 + 0x4) = *(u32*)((u8*)r31 + 0x88);
        *(u8*)(r29 + 0x20) = *(u8*)r31;
        { extern void* fn_8005DA18(void*);
          *(s16*)(r29 + 0x8) = *(s16*)((u8*)fn_8005DA18(*(void**)((u8*)r31 + 0x4)) + 0x6); }
        { extern void* fn_8005DA18(void*); void* t;
          t = fn_8005DA18(*(void**)((u8*)r31 + 0x4));
          r30 = lbl_80404B8C;
          r28 = 0;
          *(s16*)(r30 + 0xa) = *(s16*)((u8*)t + 0x8); }
        while (r28 < fn_800D3088()) {
            fn_801074D4(r30, (u8*)r31 + 0x24);
            r28++;
        }
        *(s16*)((u8*)r31 + 0x84) = *(s16*)(r29 + 0x0);
        *(s16*)((u8*)r31 + 0x86) = *(s16*)(r29 + 0x2);
        *(u32*)((u8*)r31 + 0x88) = *(u32*)(r29 + 0x4);
        *(u8*)r31 = *(u8*)(r29 + 0x20);
    }
_ret80CC:;
}
#pragma pop

/* 0x801081F8 | 0x320 */
void fn_801081F8(void* r3_arg, u16 r4, u16 r5) {
    u16 r25 = r5;
    if (r3_arg == (void*)0) {
        s32 r29 = (s32)(u16)r4;
        u32 r28 = (u32)(u16)r25;
        s32 r26 = 0;
        u8* r30 = lbl_80404B68;
        s32 r31 = 0;
        while (r26 < 0x168) {
            u8* r27 = lbl_8047AD1C + r31;
            if ((s8)*(u8*)(r27 + 0x4) != 0 && *(s16*)(r27 + 0x6) == (s16)r29) {
                if (r28 == 0) {
                    *(u32*)(r27 + 0xc) = 0;
                    *(u16*)(r27 + 0x10) = 0;
                } else {
                    memset(r27 + 0xc, 0, 0x3c);
                    *(void**)(r27 + 0xc) = fn_8005D830(r28);
                }
                *(s16*)(r30 + 0x0) = *(s16*)(r27 + 0x50);
                *(s16*)(r30 + 0x2) = *(s16*)(r27 + 0x52);
                *(u32*)(r30 + 0x4) = *(u32*)(r27 + 0x64);
                *(f32*)(r30 + 0xc) = *(f32*)(r27 + 0x68);
                *(f32*)(r30 + 0x10) = *(f32*)(r27 + 0x6c);
                *(u8*)(r30 + 0x20) = *(u8*)(r27 + 0x4);
                *(u32*)(r30 + 0x14) = *(u32*)(r27 + 0x58);
                *(s16*)(r30 + 0x18) = *(s16*)(r27 + 0x5c);
                *(s16*)(r30 + 0x1a) = *(s16*)(r27 + 0x5e);
                *(s16*)(r30 + 0x1c) = *(s16*)(r27 + 0x60);
                *(s16*)(r30 + 0x1e) = *(s16*)(r27 + 0x62);
                {
                    void* t;
                    t = fn_8005D934(*(s16*)(r27 + 0x6));
                    *(s16*)(r30 + 0x8) = *(s16*)((u8*)t + 0x2);
                    t = fn_8005D934(*(s16*)(r27 + 0x6));
                    *(s16*)(r30 + 0xa) = *(s16*)((u8*)t + 0x4);
                }
                {
                    u8* r24 = lbl_80404B68;
                    s32 r23 = 0;
                    do {
                        fn_801074D4(r24, r27 + 0xc);
                        r23++;
                    } while ((u32)r23 < fn_800D3088());
                }
                *(s16*)(r27 + 0x50) = *(s16*)(r30 + 0x0);
                *(s16*)(r27 + 0x52) = *(s16*)(r30 + 0x2);
                *(u32*)(r27 + 0x64) = *(u32*)(r30 + 0x4);
                *(f32*)(r27 + 0x68) = *(f32*)(r30 + 0xc);
                *(f32*)(r27 + 0x6c) = *(f32*)(r30 + 0x10);
                *(u8*)(r27 + 0x4) = *(u8*)(r30 + 0x20);
                *(u32*)(r27 + 0x58) = *(u32*)(r30 + 0x14);
                *(s16*)(r27 + 0x5c) = *(s16*)(r30 + 0x18);
                *(s16*)(r27 + 0x5e) = *(s16*)(r30 + 0x1a);
                *(s16*)(r27 + 0x60) = *(s16*)(r30 + 0x1c);
                *(s16*)(r27 + 0x62) = *(s16*)(r30 + 0x1e);
            }
            r31 += 0x78;
            r26++;
        }
    } else {
        void* r30 = fn_801046C8(r3_arg, (s32)r4);
        if (r30 == (void*)0) { return; }
        if ((u32)(u16)r25 == 0) {
            *(u32*)((u8*)r30 + 0xc) = 0;
            *(u16*)((u8*)r30 + 0x10) = 0;
        } else {
            memset((u8*)r30 + 0xc, 0, 0x3c);
            *(void**)((u8*)r30 + 0xc) = fn_8005D830((u32)(u16)r25);
        }
        {
            u8* r31 = lbl_80404B68;
            *(s16*)(r31 + 0x0) = *(s16*)((u8*)r30 + 0x50);
            *(s16*)(r31 + 0x2) = *(s16*)((u8*)r30 + 0x52);
            *(u32*)(r31 + 0x4) = *(u32*)((u8*)r30 + 0x64);
            *(f32*)(r31 + 0xc) = *(f32*)((u8*)r30 + 0x68);
            *(f32*)(r31 + 0x10) = *(f32*)((u8*)r30 + 0x6c);
            *(u8*)(r31 + 0x20) = *(u8*)((u8*)r30 + 0x4);
            *(u32*)(r31 + 0x14) = *(u32*)((u8*)r30 + 0x58);
            *(s16*)(r31 + 0x18) = *(s16*)((u8*)r30 + 0x5c);
            *(s16*)(r31 + 0x1a) = *(s16*)((u8*)r30 + 0x5e);
            *(s16*)(r31 + 0x1c) = *(s16*)((u8*)r30 + 0x60);
            *(s16*)(r31 + 0x1e) = *(s16*)((u8*)r30 + 0x62);
            {
                void* t;
                t = fn_8005D934(*(s16*)((u8*)r30 + 0x6));
                *(s16*)(r31 + 0x8) = *(s16*)((u8*)t + 0x2);
                t = fn_8005D934(*(s16*)((u8*)r30 + 0x6));
                {
                    u8* r25b = lbl_80404B68;
                    s32 r23 = 0;
                    *(s16*)(r25b + 0xa) = *(s16*)((u8*)t + 0x4);
                    do {
                        fn_801074D4(r25b, (u8*)r30 + 0xc);
                        r23++;
                    } while ((u32)r23 < fn_800D3088());
                }
            }
            *(s16*)((u8*)r30 + 0x50) = *(s16*)(r31 + 0x0);
            *(s16*)((u8*)r30 + 0x52) = *(s16*)(r31 + 0x2);
            *(u32*)((u8*)r30 + 0x64) = *(u32*)(r31 + 0x4);
            *(f32*)((u8*)r30 + 0x68) = *(f32*)(r31 + 0xc);
            *(f32*)((u8*)r30 + 0x6c) = *(f32*)(r31 + 0x10);
            *(u8*)((u8*)r30 + 0x4) = *(u8*)(r31 + 0x20);
            *(u32*)((u8*)r30 + 0x58) = *(u32*)(r31 + 0x14);
            *(s16*)((u8*)r30 + 0x5c) = *(s16*)(r31 + 0x18);
            *(s16*)((u8*)r30 + 0x5e) = *(s16*)(r31 + 0x1a);
            *(s16*)((u8*)r30 + 0x60) = *(s16*)(r31 + 0x1c);
            *(s16*)((u8*)r30 + 0x62) = *(s16*)(r31 + 0x1e);
        }
    }
}

/* 0x80108518 | 0x68 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80108518(void* out, u32 idx) {
#pragma optimization_level 4
#pragma peephole off
    if ((u16)idx == 0) {
        *(u32*)((u8*)out + 0x0) = 0;
        *(u16*)((u8*)out + 0x4) = 0;
    } else {
        memset(out, 0, 0x3c);
        *(void**)((u8*)out + 0x0) = fn_8005D830((u32)(u16)idx);
    }
}
#pragma pop

/* 0x801091F4 | 0x2C | nc_getter_s8 -- returns 1 if bit 1 of ptr[0x4] is set */
#pragma push
#pragma peephole off
s32 fn_801091F4(void* ptr) {
    if (ptr == (void*)0) { return 0; }
    {
        s8 r0 = (s8)*((u8*)ptr + 0x4);
        s32 r3 = (s32)r0 & 2;   /* extsb then rlwinm r3, r0, 0, 30, 30 */
        s32 neg = -r3;
        return (u32)(neg | r3) >> 31;
    }
}
#pragma pop

/* 0x80109220 | 0x3C */
#pragma push
#pragma peephole off
#pragma scheduling off
void fn_80109220(void* node, u32 enable) {
    if (node == (void*)0) { return; }
    if ((u8)enable != 0) {
        s8 r0 = (s8)(*(u8*)((u8*)node + 0x4) | 2);
        *(s8*)((u8*)node + 0x4) = r0;
    } else {
        s8 r0 = (s8)(*(u8*)((u8*)node + 0x4) & ~2);
        *(s8*)((u8*)node + 0x4) = r0;
    }
}
#pragma pop

/* 0x8010925C | 0x34 */
#pragma push
#pragma peephole off
void fn_8010925C(void* head) {
    if (head == (void*)0) { return; }
    {
        u8 r0 = 0;
        void* r4;
        void* r5 = head;
        while ((r4 = *(void**)r5) != (void*)0) {
            *(u8*)((u8*)r4 + 0x4) = r0;
            r5 = *(void**)r5;
        }
        *(u32*)head = 0;
    }
}
#pragma pop

/* 0x80109290 | 0xC8 */
#pragma push
#pragma peephole off
void* fn_80109290(void* root) {
    void* r30 = root;
    if (r30 == (void*)0) { return (void*)0; }
    {
        void* r31 = lbl_8047AD1C;
        s32 ctr = 0x168;
        while (ctr-- > 0) {
            s8 flag = (s8)*(u8*)((u8*)r31 + 0x4);
            if (flag == 0) {
                memset(r31, 0, 0x78);
                *(u8*)((u8*)r31 + 0x4) = 7;
                *(f32*)((u8*)r31 + 0x68) = lbl_8047CE3C;
                *(f32*)((u8*)r31 + 0x6c) = lbl_8047CE3C;
                *(s32*)((u8*)r31 + 0x64) = -1;
                /* insert into list rooted at r30 */
                {
                    void* cur = r30;
                    while (*(void**)cur != (void*)0) {
                        cur = *(void**)cur;
                    }
                    *(void**)cur = r31;
                }
                return r31;
            }
            r31 = (void*)((u8*)r31 + 0x78);
        }
        fn_800DD970((const char*)lbl_80271EE8);
        return (void*)0;
    }
}
#pragma pop

/* 0x80109358 | 0x70 */
#pragma push
#pragma peephole off
void fn_80109358(void) {
    u16 h = fn_800E3534(0x10000 - 0x5740);
    lbl_8047AD18 = h;
    if ((u16)h == 0) {
        fn_800DD970((const char*)lbl_80271F18, (const char*)lbl_8035B3F0);
    } else {
        void* ptr = fn_800E27B0((u16)h);
        lbl_8047AD1C = (u8*)ptr;
        memset(ptr, 0, 0x10000 - 0x5740);
    }
}
#pragma pop

/* 0x801093C8 | 0x29C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801093C8(void) {
    /* TODO: match -- 668 bytes at 0x801093C8 */
}
#pragma pop

/* 0x80109664 | 0x48 */
#pragma push
#pragma peephole off
u8 fn_80109664(u8 param) {
    u8 r31 = (u8)param;
    goto check;
loop:
    if (lbl_8047AD24 == 0) { goto done; }
    _threadSwitch();
check:
    if (r31 != 0) { goto loop; }
done:
    return lbl_8047AD24;
}
#pragma pop

/* 0x801096AC | 0x3C */
void fn_801096AC(f32 f1, f32 f2) {
    f32 f3 = lbl_8047AD30;
    f32 f0 = lbl_8047CE50;
    lbl_8047AD24 = 1;
    lbl_8047AD2C = f3;
    lbl_8047AD34 = f1;
    lbl_8047AD30 = f0;
    lbl_8047AD38 = f2;
    lbl_8047AD3C = f0;
    if (f0 != f2) { return; }
    lbl_8047AD38 = lbl_8047CE5C;
    lbl_8047AD3C = lbl_8047CE5C;
}

/* 0x801096E8 | 0x10 */
u8 fn_801096E8(u8 val) {
    u8 old = lbl_8047AD23;
    lbl_8047AD23 = val;
    return old;
}

/* 0x801096F8 | 0x10 */
u8 fn_801096F8(u8 val) {
    u8 old = lbl_8047AD22;
    lbl_8047AD22 = val;
    return old;
}

/* 0x80109708 | 0x8 | sda_getter */
u8 fn_80109708(void) { return lbl_8047AD21; }

/* 0x80109710 | 0x8 | sda_getter */
u32 fn_80109710(void) { return lbl_8047AD28; }

/* 0x80109718 | 0x4C */
#pragma push
#pragma peephole off
u8 fn_80109718(u8 param) {
    u8 r31 = (u8)param;
    goto check;
loop:
    if (lbl_8047AD20 != 0) { goto done; }
    _threadSwitch();
check:
    if (r31 != 0) { goto loop; }
done:
    return (u8)*(volatile u8*)&lbl_8047AD20;
}
#pragma pop

/* 0x80109764 | 0x18 */
void fn_80109764(void) {
    lbl_8047AD24 = 0;
    lbl_8047AD21 = 0;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
}

/* 0x8010977C | 0x94 */
#pragma push
#pragma peephole off
u8 fn_8010977C(u32 param) {
    u32 r31 = param;
    lbl_8047AD24 = 0;
    lbl_8047AD28 = lbl_8047AD28;  /* read */
    lbl_8047AD21 = 1;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
    lbl_8047AD2C = lbl_8047CE50;
    lbl_8047AD30 = lbl_8047CE50;
    lbl_8047AD34 = lbl_8047CE50;
    lbl_8047AD38 = lbl_8047CE50;
    lbl_8047AD3C = lbl_8047CE50;
    GSgfxBeginBackFBCapture(lbl_8047AD28, fn_80109884, (void*)0);
    r31 = (u8)r31;
    goto check;
loop:
    if (lbl_8047AD20 != 0) { goto done; }
    _threadSwitch();
check:
    if (r31 != 0) { goto loop; }
done:
    return (u8)*(volatile u8*)&lbl_8047AD20;
}
#pragma pop

/* 0x80109810 | 0x74 */
void fn_80109810(void) {
    if (lbl_8047AD28 == 0) {
        lbl_8047AD28 = GStextureCreate(0, 0, 0x44, 0, 0);
    }
    lbl_8047AD21 = 0;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
    lbl_8047AD23 = 0;
    lbl_8047AD24 = 0;
    lbl_8047AD38 = lbl_8047CE50;
    lbl_8047AD3C = lbl_8047CE50;
    lbl_8047AD2C = lbl_8047CE50;
    lbl_8047AD30 = lbl_8047CE50;
    lbl_8047AD34 = lbl_8047CE50;
}

/* 0x80109884 | 0x10 */
s32 fn_80109884(void) {
    lbl_8047AD20 = 1;
    return 0;
}

/* 0x80109894 | 0xA0 */
#pragma push
#pragma peephole off
s32 menuModelSetMotion(void* p, u32 val) {
    void* r31 = p;
    if (r31 == (void*)0) { return 0; }
    if (*(u8*)((u8*)r31 + 0x1) != 0) {
        if (*(u8*)((u8*)r31 + 0x4) == 0) {
            *(u32*)((u8*)r31 + 0xc) = val;
        }
    } else {
        if (*(u8*)((u8*)r31 + 0x14) == 0) {
            *(u32*)((u8*)r31 + 0x1c) = val;
            {
                u32 r3 = *(u32*)((u8*)r31 + 0x24);
                if ((u8)fn_800EC1BC(r3) != 0) {
                    fn_800ECCA8(*(u32*)((u8*)r31 + 0x24), *(u32*)((u8*)r31 + 0x1c));
                    fn_800EC9DC(*(u32*)((u8*)r31 + 0x24), lbl_8047CE70);
                    fn_800EC990(*(u32*)((u8*)r31 + 0x24));
                }
            }
        }
    }
    return 1;
}
#pragma pop

/* 0x80109934 | 0x25C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80109934(void) {
    /* TODO: match -- 604 bytes at 0x80109934 */
}
#pragma pop

/* 0x80109B90 | 0x6C */
#pragma push
#pragma peephole off
s32 fn_80109B90(void* obj, u8 wait) {
    void* r30 = obj;
    if (r30 == (void*)0) { return 0; }
    {
        u8 r31 = (u8)wait;
        goto check;
    check:
        if (*(u8*)r30 != 1) { return 0; }
        if (r31 == 0) { goto done; }
        _threadSwitch();
        goto check;
    done:
        return 1;
    }
}
#pragma pop

/* 0x80109BFC | 0x8C */
#pragma push
#pragma peephole off
s32 fn_80109BFC(void* p) {
    void* r31 = p;
    if (r31 == (void*)0) { return 0; }
    *(u8*)((u8*)r31 + 0x1) = 0;
    if (*(u8*)((u8*)r31 + 0x14) != 0) {
        /* has some kind */
        if (*(void**)((u8*)r31 + 0x24) != (void*)0) {
            fn_801DB100(*(u32*)((u8*)r31 + 0x24));
            *(u32*)((u8*)r31 + 0x24) = 0;
        }
    } else {
        if (*(void**)((u8*)r31 + 0x24) != (void*)0) {
            fn_800E4BF4(*(u32*)((u8*)r31 + 0x24));
            *(u32*)((u8*)r31 + 0x24) = 0;
        }
    }
    *(u8*)((u8*)r31 + 0x0) = 0;
    return 1;
}
#pragma pop

/* 0x80109C88 | 0x388 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80109C88(void) {
    /* TODO: match -- 904 bytes at 0x80109C88 */
}
#pragma pop

/* 0x8010A010 | 0x200 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010A010(void) {
    /* TODO: match -- 512 bytes at 0x8010A010 */
}
#pragma pop

/* 0x8010A210 | 0x210 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010A210(void) {
    /* TODO: match -- 528 bytes at 0x8010A210 */
}
#pragma pop

/* 0x8010A420 | 0x19C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010A420(void) {
    /* TODO: match -- 412 bytes at 0x8010A420 */
}
#pragma pop

/* 0x8010A5BC | 0x2D0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010A5BC(void) {
    /* TODO: match -- 720 bytes at 0x8010A5BC */
}
#pragma pop

/* 0x8010A88C | 0x274 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010A88C(void) {
    /* TODO: match -- 628 bytes at 0x8010A88C */
}
#pragma pop

/* 0x8010AB00 | 0x32C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010AB00(void) {
    /* TODO: match -- 812 bytes at 0x8010AB00 */
}
#pragma pop

/* 0x8010AE2C | 0x1F0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010AE2C(void) {
    /* TODO: match -- 496 bytes at 0x8010AE2C */
}
#pragma pop

/* 0x8010B01C | 0x150 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B01C(void) {
    /* TODO: match -- 336 bytes at 0x8010B01C */
}
#pragma pop

/* 0x8010B16C | 0x3F4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B16C(void) {
    /* TODO: match -- 1012 bytes at 0x8010B16C */
}
#pragma pop

/* 0x8010B560 | 0x64 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B560(void) {
    /* TODO: match -- 100 bytes at 0x8010B560 */
}
#pragma pop

/* 0x8010B5C4 | 0x154 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B5C4(void) {
    /* TODO: match -- 340 bytes at 0x8010B5C4 */
}
#pragma pop

/* 0x8010B718 | 0x2D0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B718(void) {
    /* TODO: match -- 720 bytes at 0x8010B718 */
}
#pragma pop

/* 0x8010B9E8 | 0x1D0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010B9E8(void) {
    /* TODO: match -- 464 bytes at 0x8010B9E8 */
}
#pragma pop

/* 0x8010BBB8 | 0x12C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010BBB8(void) {
    /* TODO: match -- 300 bytes at 0x8010BBB8 */
}
#pragma pop

/* 0x8010BCE4 | 0x88 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010BCE4(void) {
    /* TODO: match -- 136 bytes at 0x8010BCE4 */
}
#pragma pop

/* 0x8010BD6C | 0x4B4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010BD6C(void) {
    /* TODO: match -- 1204 bytes at 0x8010BD6C */
}
#pragma pop
