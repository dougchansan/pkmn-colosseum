/**
 * @file gs_floor_data.c
 * @brief GSfloor -- Floor-data table walkers and small state helpers.
 *
 * This unit spans 0x800FF0A0-0x800FF788 (15 functions, 0x6E8 bytes total)
 * and sits between the GStexture/resource layer (fn_800F7274 etc.) and
 * gs_floor.c's GSfloorOpen (0x800FF788+). It contains only:
 *   - 4 floor-resource table walkers (fn_800FF0A0, fn_800FF178,
 *     fn_800FF2A0, fn_800FF3C0). fn_800FF0A0 is exact pure C; the other
 *     three remain honest TODO stubs. No rodata string references exist
 *     for any of the four, and no evidence supports semantic names.
 *     fn_800FF0A0 calls fn_800F7274 only; the other three call nothing.
 *   - 11 small floor-state accessors/helpers (fn_800FF4D4 through
 *     fn_800FF784), all matched at 100% -- must stay byte-identical.
 *
 * A prior campaign-generation transplant had attached a large orphan
 * block to this file: "GSfloorLoadCallback"/"GSfloorRegisterResHandlers"-
 * style wrappers for 0x80101910-0x80101B90, "GSfloorGetState"-style
 * duplicates of the already-matched 0x800FF5xx/6xx/7xx helpers below
 * (referencing gsFloorState/gsFloorNextState/gsFloorCurrentId, which are
 * `static` inside gs_floor.c and not link-visible here), and a full set
 * of floorRead*PreFunc(void) definitions for 0x8011432C-0x80115024 with
 * the wrong signature. That range and those names are fiction for this
 * unit. The real floorRead*PreFunc family (signature
 * `void* name(u32 resId, u32 loadMode, u32 dataSize)`, string-proven)
 * lives in gs_field_resource.c (GFL/Particle/WZX/PKX/Tex) and
 * gs_field_world.c (Map/Script/Font/Msg/Normal). The orphan block has
 * been removed; see the gs_field_resource.c reconciliation this pass
 * completes (commit 9f528cd5).
 *
 * Address range: 0x800FF0A0 - 0x800FF788
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"

/* ===== External engine functions used by the table walkers below ===== */
extern void  memcpy(void* dst, const void* src, u32 n);
extern u32   fn_800F7274(u16 handle);
extern void  floorSetFadeScript(u32 a, u32 b);

/* ===== SDA/SBSS globals (floor context / resource pool state) ===== */
extern u32 lbl_80478B18;
extern u32 lbl_8047ACA8;
extern u32 lbl_8047ACB0;
extern u32 lbl_8047ACB4;
extern u32 lbl_8047ACB8;
extern u32 lbl_8047ACC0;
extern u32 lbl_8047ACC4;
extern u32 lbl_8047ACC8;
extern GSFloorResource* lbl_8047ACCC;
extern u32 lbl_8047ACD8;
extern u32 lbl_8047ACDC;
extern u32 lbl_8047ACE0;
extern GSFloorResHandler lbl_80404918[];

#if !defined(GS_FLOOR_DATA_EXACT_800FF0A0_ONLY) && \
    !defined(GS_FLOOR_DATA_RESIDUAL_800FF178_ONLY)
#define GS_FLOOR_DATA_ALL
#endif

#if defined(GS_FLOOR_DATA_ALL) || \
    defined(GS_FLOOR_DATA_EXACT_800FF0A0_ONLY)

/* 0x800FF0A0 | 0xD8 */
void fn_800FF0A0(u32 callback) {
    GSFloorResource* resource;
    u32 remaining;

    resource = (GSFloorResource*)lbl_8047ACB0;
    remaining = lbl_8047ACC0;
    while (remaining-- != 0) {
        if ((s32)resource->active == 3 &&
            resource->callback == (void*)callback) {
            if (resource != NULL) {
                goto found;
            }
            goto found;
        }
        resource++;
    }
    resource = NULL;

found:
    if (resource != NULL) {
        if ((s32)resource->status == 1 && resource->modelHandle != NULL) {
            fn_800F7274(resource->textureHandle);
        }
        resource->active = 0;
        if (resource->prev != NULL) {
            resource->prev->next = resource->next;
        }
        if (resource->next != NULL) {
            resource->next->prev = resource->prev;
        }
        if (lbl_8047ACCC == resource) {
            lbl_8047ACCC = resource->next;
        }
        resource->prev = NULL;
        resource->next = NULL;
    }
}

#endif

#if defined(GS_FLOOR_DATA_ALL) || \
    defined(GS_FLOOR_DATA_RESIDUAL_800FF178_ONLY)

/* 0x800FF178 | 0x128 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF178(void) {
    /* TODO: match -- 296 bytes at 0x800FF178 */
}
#pragma pop

/* 0x800FF2A0 | 0x120 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF2A0(void) {
    /* TODO: match -- 288 bytes at 0x800FF2A0 */
}
#pragma pop

/* 0x800FF3C0 | 0x114 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF3C0(void) {
    /* TODO: match -- 276 bytes at 0x800FF3C0 */
}
#pragma pop

/* 0x800FF4D4 | 0x58 */
void fn_800FF4D4(void* data, u8 typeId) {
    GSFloorResHandler* handler;

    if (lbl_8047ACE0 < 0x18) {
        handler = &lbl_80404918[lbl_8047ACE0];
        handler->typeId = typeId;
        memcpy(&handler->reserved, data, 0xC);
        lbl_8047ACE0++;
    }
}

/* 0x800FF52C | 0x14 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_800FF52C(void) {
    return lbl_8047ACC4 != 0;
}
#pragma pop

/* 0x800FF540 | 0x8 | sda_getter */
u32 fn_800FF540(void) { return lbl_8047ACC4; }

/* 0x800FF548 | 0xC | sda_deref_getter */
u8 fn_800FF548(void) {
    return *(u8*)((u8*)lbl_8047ACC8 + 0xB);
}

/* 0x800FF554 | 0xC | sda_deref_getter */
u8 fn_800FF554(void) {
    return *(u8*)((u8*)lbl_8047ACC8 + 0xA);
}

/* 0x800FF560 | 0xC | sda_deref_getter */
u32 fn_800FF560(void) {
    return *(u32*)((u8*)lbl_8047ACC8 + 0x4);
}

/* 0x800FF56C | 0x20 */
u32 fn_800FF56C(void) {
    void* floorData;

    floorData = ((GSFloorContext*)lbl_8047ACC8)->floorDataEntry;
    if (floorData != NULL) {
        return *(u32*)((u8*)floorData + 0xC);
    }
    return 0;
}

/* 0x800FF58C | 0xD4 */
void fn_800FF58C(u32 floorId) {
    GSFloorContext* currentFloor;
    GSFloorResource* resource;
    u32 remaining;

    floorSetFadeScript(0x05960009, 0x05960008);
    currentFloor = (GSFloorContext*)lbl_8047ACC8;
    if ((s32)lbl_8047ACD8 == 2) {
        resource = (GSFloorResource*)((u8*)lbl_8047ACB0 +
            (lbl_8047ACB4 * sizeof(GSFloorResource)));
        remaining = lbl_8047ACB8;
        while (remaining-- != 0) {
            if ((s32)resource->active != 0 &&
                (s32)resource->status == 1 &&
                resource->floorId == currentFloor->floorId) {
                fn_800F7274(resource->textureHandle);
            }
            resource++;
        }
        currentFloor->isActive = 5;
        *(volatile u32*)&lbl_80478B18 = (u32)-1;
    }
    *(volatile u32*)&lbl_8047ACDC = 3;
    *(volatile u32*)&lbl_80478B18 = floorId;
}

/* 0x800FF660 | 0xD0 */
void fn_800FF660(void) {
    GSFloorContext* currentFloor;
    GSFloorResource* resource;
    u32 remaining;

    floorSetFadeScript(0x05960009, 0x05960008);
    if (lbl_8047ACC4 != 0) {
        currentFloor = (GSFloorContext*)lbl_8047ACC8;
        if ((s32)lbl_8047ACD8 == 2) {
            resource = (GSFloorResource*)((u8*)lbl_8047ACB0 +
                (lbl_8047ACB4 * sizeof(GSFloorResource)));
            remaining = lbl_8047ACB8;
            while (remaining-- != 0) {
                if ((s32)resource->active != 0 &&
                    (s32)resource->status == 1 &&
                    resource->floorId == currentFloor->floorId) {
                    fn_800F7274(resource->textureHandle);
                }
                resource++;
            }
            currentFloor->isActive = 5;
            lbl_80478B18 = (u32)-1;
        }
        lbl_8047ACDC = 5;
    }
}

/* 0x800FF730 | 0x54 */
void fn_800FF730(u32 floorId) {
    floorSetFadeScript(0x05960009, 0x05960008);
    if (lbl_8047ACC4 < lbl_8047ACA8) {
        *(volatile u32*)&lbl_8047ACD8 = 4;
        *(volatile u32*)&lbl_80478B18 = floorId;
    }
}

/* 0x800FF784 | 0x4 | void_stub */
void fn_800FF784(void) {
}

#endif
