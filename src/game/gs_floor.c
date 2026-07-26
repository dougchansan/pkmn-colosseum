/**
 * @file gs_floor.c
 * @brief GSfloor -- Floor/scene management unit (address range only; not
 *        yet decompiled beyond two small stub functions).
 *
 * A prior recovery pass invented named GSfloor* functions (GSfloorOpen,
 * GSfloorInit, GSfloorThreadMain, GSfloorUpdate, GSfloorLoadParticle,
 * GSfloorFindAndOpen, GSfloorLoadData, GSfloorLoadMain, GSfloorGetCurrentId,
 * GSfloorGetContext, etc.) with fabricated bodies. None of those names
 * appear in config/GC6E01/symbols.txt and none of them were referenced
 * anywhere else in the tree; they have been removed (see the note above
 * the stub scaffold below).
 *
 * What actually remains is the real stub scaffold for this unit's address
 * range, generated from config/GC6E01/splits.txt:
 *   fn_800FF788 | 0x94   (~97% fuzzy match)
 *   fn_800FF81C | 0xC    (100% match)
 *   fn_800FF828 | 0x148  (TODO, not yet matched)
 *   fn_800FF970 | 0x11B4 (TODO, not yet matched)
 *   fn_80100B24 | 0x720  (TODO, not yet matched)
 *   loadParticle | 0xA4  (100% match; ported from archive/previous_campaign
 *                        GSfloorLoadParticle body, default optimization --
 *                        no O0 pragma needed)
 *   fn_801012E8 | 0xB8   (TODO, not yet matched)
 *   fn_801013A0 | 0xDC   (TODO, not yet matched)
 *   fn_8010147C | 0x494  (TODO, not yet matched)
 *
 * Address range: 0x800FF788 - 0x80101910
 */

#include "dolphin/types.h"
#include "game/gs_floor.h"

/* ===== External engine / SDK functions (used by the stub scaffold) ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* GSlog / OSReport */
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 loadParam,
                                      u32 loadParam2, void* callback);
extern void  memcpy(void* dst, const void* src, u32 n);

/* ===== String constants (rodata references) ===== */
extern const char lbl_802717F0[];  /* "GSfloorOpen: cannot find floor %d\n" */
extern const char lbl_802719C4[];  /* "loadParticle(): loading...\n" */
extern const char lbl_802719E0[];  /* "loadParticlePtr(): can't alloc %d bytes of memory\n" */

/* ===================================================================
 * Generated: 0 pattern-matched + 9 stubs
 * Range: 0x800FF788 - 0x80101910
 * =================================================================== */

/* 0x800FF788 | 0x94 */
void fn_800FF788(u32 floorId) {
    extern GSFloorContext* lbl_8047ACC8;
    extern void* lbl_8047ACD0;
    extern u32 lbl_8047ACD4;
    extern s32 lbl_8047ACD8;
    u8* entry;
    GSFloorContext* ctx;
    u32 count;

    ctx = lbl_8047ACC8;
    if (lbl_8047ACD8 != 0) {
        return;
    }

    entry = lbl_8047ACD0;
    for (count = lbl_8047ACD4; count != 0; count--) {
        if (*(u32*)(entry + 0xC) != floorId) {
            goto not_found;
        }
        if (((!entry) && (!entry)) && (!entry)) {
        }
        goto found;
not_found:
        entry += 0x4C;
    }
    entry = NULL;

found:
    if (entry == NULL) {
        GSlogWrite(lbl_802717F0, floorId);
        return;
    }

    ctx->floorDataEntry = entry;
    ctx->floorId = floorId + GSFLOOR_ID_BASE;
    ctx->isActive = 1;
    lbl_8047ACD8 = 1;
}

/* 0x800FF81C | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF81C(void* tablePtr, u32 tableCount) {
    extern void* lbl_8047ACD0;
    extern u32 lbl_8047ACD4;
    lbl_8047ACD0 = tablePtr;
    lbl_8047ACD4 = tableCount;
}
#pragma pop

/* 0x800FF828 | 0x148 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF828(void) {
    /* TODO: match -- 328 bytes at 0x800FF828 */
}
#pragma pop

/* 0x800FF970 | 0x11B4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_800FF970(void) {
    /* TODO: match -- 4532 bytes at 0x800FF970 */
}
#pragma pop

/* 0x80100B24 | 0x720 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80100B24(void) {
    /* TODO: match -- 1824 bytes at 0x80100B24 */
}
#pragma pop

/* 0x80101244 | 0xA4 */
void loadParticle(void* dst, u32 size, void* callback, void* callbackArg) {
    void* buf;

    GSlogWrite(lbl_802719C4);

    buf = GSresAllocResourceAlign((size + 0x1F) & ~0x1F, 0x20,
                                   (u32)callback, (u32)callbackArg, NULL);
    if (buf == NULL) {
        GSlogWrite(lbl_802719E0, size);
        return;
    }

    memcpy(buf, dst, size);
}

/* 0x801012E8 | 0xB8 */
void fn_801012E8(void* archive, u32 resourceArg, u32 callbackArg) {
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern void* fn_800D27FC(void*);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern void fn_80101A28(void);
    void** publicData;
    void* resource;

    if (archive == NULL) {
        GSlogWrite(lbl_802717F0 + 0x300);
        return;
    }
    publicData = HSD_ArchiveGetPublicAddress(archive, lbl_802717F0 + 0x2C8);
    if (publicData == NULL) {
        GSlogWrite(lbl_802717F0 + 0x328);
        return;
    }
    resource = fn_800D27FC(publicData[1]);
    if (resource == NULL) {
        GSlogWrite(lbl_802717F0 + 0x358);
    }
    GSresRegisterResource(resource, resourceArg, callbackArg,
                          (void*)fn_80101A28);
}

/* 0x801013A0 | 0xDC */
void fn_801013A0(void* archive, u32 resourceArg, u32 modelIndex,
                 u32 callbackArg) {
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern void* GSmodelLoad(void*);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern void fn_80101A4C(void);
    void*** publicData;
    void** models;
    void* model;
    u32 index;

    if (archive == NULL) {
        GSlogWrite(lbl_802717F0 + 0x3F4);
        return;
    }
    publicData = HSD_ArchiveGetPublicAddress(archive, lbl_802717F0 + 0x2C8);
    if (publicData == NULL) {
        GSlogWrite(lbl_802717F0 + 0x418);
        return;
    }
    models = *publicData;
    for (index = 0; models[index] != NULL; index++) {
        if (index == modelIndex) {
            model = GSmodelLoad(models[index]);
            if (model == NULL) {
                GSlogWrite(lbl_802717F0 + 0x448, index);
            }
            GSresRegisterResource(model, resourceArg, callbackArg,
                                  (void*)fn_80101A4C);
            return;
        }
    }
}

/* 0x8010147C | 0x494 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010147C(void) {
    /* TODO: match -- 1172 bytes at 0x8010147C */
}
#pragma pop
