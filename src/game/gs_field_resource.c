/**
 * @file gs_field_resource.c
 * @brief GSfield floor resource pre-load callbacks.
 *
 * Decompiled from (attribution FINAL -- registration-table-proven, see note):
 *   floorReadGFLPreFunc                (0x8011432C, string lbl_80272200 + table 0x11)
 *   floorReadBGMPostFunc               (0x801143A0, table 0x04; byte-identical to XD)
 *   floorReadBGMPreFunc                (0x801143A8, table 0x04)
 *   floorReadNotLinkedParticlePostFunc (0x801143EC, table 0x12)
 *   floorReadNotLinkedParticlePreFunc  (0x8011445C, string lbl_80272270 + table 0x12)
 *   floorReadParticlePostFunc          (0x801144D0, table 0x0A; scene_data tail)
 *   floorReadParticlePreFunc           (0x801145C0, string lbl_80272270 + table 0x0A)
 *   floorReadWZXPreFunc                (0x80114634, string lbl_802722B8 + table 0x10)
 *   floorReadPKXPreFunc                (0x801146A4, string lbl_802722F0 + table 0x0F)
 *   floorReadTexPostFunc               (0x80114714, string lbl_80272328 + table 0x09)
 *   floorReadTexPreFunc                (0x80114760, string lbl_8027235C + table 0x09)
 *   floorReadColPostFunc               (0x801147D4, table 0x03)
 *   floorReadColPreFunc                (0x80114808, string lbl_80272394 + table 0x03)
 *   floorReadCameraPostFunc            (0x8011487C, table 0x0C; _registerCamera strings)
 *   floorReadCameraPreFunc             (0x80114948, string lbl_80272428 + table 0x0C)
 *   floorReadObjPostFunc               (0x801149BC, table 0x02)
 *   floorReadObjPreFunc                (0x80114A70, string lbl_80272460 + table 0x02)
 *
 * ATTRIBUTION NOTE: names are pinned by TWO independent evidence systems.
 *   (1) Self-name assert strings in rodata (each pre-func prints its own
 *       name on allocation failure), verified against the target DOL.
 *   (2) The floor resource handler registration table at lbl_8036C2A0
 *       (16-byte entries: size?, type-id, PreFunc, PostFunc), which pairs
 *       every PreFunc with its PostFunc per resource slot. Table entries
 *       decoded from the DOL; layout mirrors Pokemon XD's floorRead
 *       family, where every resource type has a paired Post+Pre handler.
 *   The table resolved the former two-particle-string anomaly: BOTH
 *   0x8011445C and 0x801145C0 reference lbl_80272270, but entry 0x12
 *   pairs 0x8011445C with the NotLinkedParticle PostFunc (0x801143EC)
 *   while entry 0x0A pairs 0x801145C0 with the true ParticlePostFunc
 *   (0x801144D0, the scene_data group-linking one) -- matching XD, where
 *   the two PreFunc bodies are also byte-identical to each other.
 *   The GFL PostFunc (table 0x11) is floorReadGFLPostFunc in gs_field_colquery.c
 *   territory -- rename parked until that fleet-owned file is free.
 *   An earlier first-generation "orphan" recovery block (names that never
 *   paired, wrong attributions) was removed by the 2026-07-01 pass; its
 *   candidate bodies were empty TODO placeholders (nothing adoptable).
 *
 * Each floor archive (FSYS) contains multiple resource types that need
 * to be loaded into memory before the floor becomes active. The pre-func
 * callbacks allocate memory, validate buffer sizes, and set up the
 * loading pipeline.
 *
 * Resource types handled:
 *   - GFL: Floor geometry data (ground mesh, walls)
 *   - Sound: BGM/SE wave data buffers
 *   - Particle: VFX particle system data
 *   - WZX: Walkability/collision mesh
 *   - PKX: Pokemon model data (for overworld encounters)
 *   - Texture: Shared texture packs
 *   - Camera: Pre-set camera angles/positions
 *   - Map: Minimap/area data
 *   - Script: Event script bytecode
 *   - Font: Text rendering data
 *   - Message: Localized string tables
 *   - Normal: Default/miscellaneous data
 *
 * Debug strings:
 *   "floorReadGFLPreFunc(): can't alloc %d bytes of memory"
 *   "ERROR: Over Sound Buffer! snd_res_id=%d buffer size=%d"
 *   "floorReadParticlePreFunc(): can't alloc %d bytes of memory"
 *   "floorReadWZXPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadPKXPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadTexPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadCameraPreFunc: can't alloc %d bytes of memory"
 *   "floorReadMapPreFunc: can't alloc %d bytes of memory"
 *   "floorReadScriptPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadFontPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadMsgPreFunc(): can't alloc %d bytes of memory"
 *   "floorReadNormalPreFunc(): can't alloc %d bytes of memory"
 *
 * Address range: 0x8011432C - 0x80114AE0 (per splits.txt; 0x80114AE0 itself
 * belongs to the next unit, field_range_80114AE0.c).
 */

#include "dolphin/types.h"
#include "game/world/gs_field.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void* GSresAllocResourceAlign(u32 size, u32 alignment,
                          u32 loadParam, u32 loadParam2,
                          void* callback);               /* GSres alloc+load */

/* ===== String constants (rodata) ===== */
extern const char lbl_80272200[]; /* floorReadGFLPreFunc(): can't alloc... */
extern const char lbl_80272238[]; /* "ERROR: Over Sound Buffer! snd_res_id=%d..." */
extern const char lbl_80272270[]; /* "floorReadParticlePreFunc(): can't alloc..." (read by BOTH particle pre-funcs, see header note) */
extern const char lbl_802722B8[]; /* floorReadWZXPreFunc(): can't alloc... */
extern const char lbl_802722F0[]; /* floorReadPKXPreFunc(): can't alloc... */
extern const char lbl_8027235C[]; /* floorReadTexPreFunc(): can't alloc... */
extern const char lbl_80272428[]; /* floorReadCameraPreFunc: can't alloc... */
extern const char lbl_802724E8[]; /* floorReadMapPreFunc: can't alloc... (next unit) */
extern const char lbl_80272520[]; /* floorReadScriptPreFunc(): can't alloc... (next unit) */
extern const char lbl_8027255C[]; /* floorReadFontPreFunc(): can't alloc... (next unit) */
extern const char lbl_80272594[]; /* floorReadMsgPreFunc(): can't alloc... (next unit) */
extern const char lbl_802725CC[]; /* floorReadNormalPreFunc(): can't alloc... (next unit) */
extern const char lbl_802722AC[]; /* "scene_data" */
extern const char lbl_80272328[]; /* floorReadTexPostFunc(): <SJIS text> -- string-proves fn_80114714's name */
extern const char lbl_80272394[]; /* floorReadColPreFunc(): can't alloc... -- string-proves fn_80114808's name */
extern const char lbl_80272460[]; /* floorReadObjPreFunc: can't alloc... -- string-proves fn_80114A70's name */

/* ===== BSS / global state ===== */
extern u32 lbl_8047B0B0;  /* sound buffer size limit */

/* ===== Internal callbacks referenced by pre-funcs ===== */
extern void _unloadFlare__FPvUlUl(void);  /* GFL resource completion callback */
extern void _unloadParticles__FPvUlUl(void);  /* sound resource completion callback */
extern void _unloadColsys__FPvUlUl(void);  /* resource completion callback (Col/...) */

extern void* GSresGetResource();        /* GSres simple alloc */
extern void* GSresRegisterResource();        /* GSres install loader */
extern void  fn_8010CFE4();        /* PKX overlap setup */
extern u32   GStextureLoad();        /* WZX overlap check */
extern void* gsParticleBankLoad();        /* node lookup */

typedef struct HSDArchiveBuffer {
    u8 header[0x60];
    u8 payload[1];
} HSDArchiveBuffer;

/* ===================================================================
 * Range: 0x8011432C - 0x80114AE0
 * =================================================================== */

/* 0x8011432C | 0x74 | floorReadGFLPreFunc (string-proven) */
void* floorReadGFLPreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize;

    alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, loadMode, (void*)_unloadFlare__FPvUlUl);
    if (buf == (void*)0) {
        GSlogWrite(lbl_80272200, dataSize);
    }
    return buf;
}

/* 0x801143A0 | 0x8 | return_const */
u32 floorReadBGMPostFunc(void) { return 0; }

/* 0x801143A8 | 0x44 | sound buffer size check */
void* floorReadBGMPreFunc(u32 unused, u32 sndResId, u32 dataSize) {
    extern void* lbl_8047B0B4;
    u32 alignedSize = (dataSize + 0x1F) & ~0x1F;
    u32 bufferSize = lbl_8047B0B0;

    if (alignedSize > bufferSize) {
        GSlogWrite(lbl_80272238, sndResId, bufferSize);
    }
    return lbl_8047B0B4;
}

/* 0x801143EC | 0x70 */
void* floorReadNotLinkedParticlePostFunc(u32 resId, u32 param) {
    void* result = GSresGetResource(resId, (param & 0x7FFF0000) | 0x400);
    void* node = gsParticleBankLoad(result);
    if (node != (void*)0) {
        GSresRegisterResource(node, resId, param, (void*)_unloadParticles__FPvUlUl);
    }
    return result;
}

/* 0x8011445C | 0x74 | floorReadNotLinkedParticlePreFunc (string + table 0x12) */
void* floorReadNotLinkedParticlePreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize;

    alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, (loadMode & 0x7FFF0000) | 0x400, (void*)0);
    if (buf == (void*)0) {
        GSlogWrite(lbl_80272270, alignedSize);
    }
    return buf;
}

/* 0x801144D0 | 0xF0 */
void* floorReadParticlePostFunc(u32 resId, u32 param) {
    void* nullValue;
    typedef struct FloorData {
        u8 unk0[8];
        u32 resourceFlags;
    } FloorData;
    typedef struct SceneData {
        void** models;
    } SceneData;
    extern FloorData* floorDataBiosGetCurrentPtr(void);
    extern u32 std093_getCurrentGroupID(void);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbol);
    extern void GSmodelLinkToGSparticleBank(void* model, void* bank);
    u32 resourceFlags;
    u32 modelOffset;
    void* result;
    void* particleBank;
    FloorData* floorData;
    SceneData* sceneData;
    u32 i = 0;

    result = GSresGetResource(resId, (param & 0x7FFF0000) | 0x400);
    particleBank = gsParticleBankLoad(result);
    if (particleBank != (void*)0) {
        GSresRegisterResource(particleBank, resId, param,
                              (void*)_unloadParticles__FPvUlUl);
        nullValue = (void*)0;
        floorData = floorDataBiosGetCurrentPtr();
        sceneData = HSD_ArchiveGetPublicAddress(
            GSresGetResource(std093_getCurrentGroupID(), floorData->resourceFlags),
            lbl_802722AC);
        if (sceneData == nullValue) {
            return nullValue;
        }
        if (sceneData->models != nullValue) {
            modelOffset = 0;
            resourceFlags = (floorData->resourceFlags & 0x7FFF0000) | 0x1000;
            for (; *(void**)((u8*)sceneData->models + modelOffset) != nullValue;
                 modelOffset += sizeof(void*), i++) {
                void* model = GSresGetResource(std093_getCurrentGroupID(),
                                                resourceFlags | i);
                if (model != nullValue) {
                    GSmodelLinkToGSparticleBank(model, particleBank);
                }
            }
        }
    }
    return result;
}

/* 0x801145C0 | 0x74 | floorReadParticlePreFunc (table 0x0A, paired with
 * floorReadParticlePostFunc at 0x801144D0). Reads the same lbl_80272270
 * string as floorReadNotLinkedParticlePreFunc -- the two bodies are
 * byte-identical, exactly as in XD. */
void* floorReadParticlePreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize;

    alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, (loadMode & 0x7FFF0000) | 0x400, (void*)0);
    if (buf == (void*)0) {
        GSlogWrite(lbl_80272270, alignedSize);
    }
    return buf;
}

/* 0x80114634 | 0x70 | floorReadWZXPreFunc (string-proven) */
void* floorReadWZXPreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, loadMode, (void*)0);
    if (buf == (void*)0) {
        GSlogWrite(lbl_802722B8, alignedSize);
    }
    return buf;
}

/* 0x801146A4 | 0x70 | floorReadPKXPreFunc (string-proven) */
void* floorReadPKXPreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, loadMode, (void*)0);
    if (buf == (void*)0) {
        GSlogWrite(lbl_802722F0, alignedSize);
    }
    return buf;
}

/* 0x80114714 | 0x4C | floorReadTexPostFunc (string-proven: lbl_80272328 is
 * "floorReadTexPostFunc(): <SJIS text>") */
void* floorReadTexPostFunc(void) {
    void* result = GSresGetResource();
    if (GStextureLoad(result) == 0) {
        GSlogWrite(lbl_80272328);
    }
    return result;
}

/* 0x80114760 | 0x74 | floorReadTexPreFunc (string-proven: lbl_8027235C) */
void* floorReadTexPreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    extern u32 _unloadTexture__FPvUlUl(void);
    void* buf;
    u32 alignedSize;

    alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, loadMode,
                                  (void*)_unloadTexture__FPvUlUl);
    if (buf == (void*)0) {
        GSlogWrite(lbl_8027235C, alignedSize);
    }
    return buf;
}

/* 0x801147D4 | 0x34 | matched, unproven name */
void* floorReadColPostFunc(void) {
    void* result = GSresGetResource();
    fn_8010CFE4();
    return result;
}

/* 0x80114808 | 0x74 | floorReadColPreFunc (string-proven: lbl_80272394) */
void* floorReadColPreFunc(u32 resId, u32 loadMode, u32 dataSize) {
    void* buf;
    u32 alignedSize;

    alignedSize = (dataSize + 0x1F) & ~0x1F;
    buf = GSresAllocResourceAlign(alignedSize, 0x20, resId, loadMode, (void*)_unloadColsys__FPvUlUl);
    if (buf == (void*)0) {
        GSlogWrite(lbl_80272394, alignedSize);
    }
    return buf;
}

/* 0x8011487C | 0xCC | matched, unproven name */
void* floorReadCameraPostFunc(u32 resId, u32 loadMode, u32 dataSize) {
    extern void HSD_ArchiveParse(void* archive, void* buf, u32 size);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* sym);
    extern void* GScameraCreateFromDesc(void* model);
    extern void GSresRegisterResource(void* entry, u32 resId, u32 loadMode, void* callback);
    extern u32 _unloadCamera__FPvUlUl(void);
    const u8* strings;
    HSDArchiveBuffer* archive;
    void* pub;
    void* entry;
    u32 flags;

    flags = loadMode & 0x7FFF0000;
    strings = (const u8*)lbl_80272200;
    archive = GSresGetResource(resId, flags | 0x400);
    HSD_ArchiveParse(archive, archive->payload, dataSize);
    pub = HSD_ArchiveGetPublicAddress(archive, (const char*)(strings + 0xAC));
    if (pub == (void*)0) {
        GSlogWrite((const char*)(strings + 0x1CC));
    } else {
        entry = GScameraCreateFromDesc(*(void**)((u8*)pub + 4));
        if (entry == (void*)0) {
            GSlogWrite((const char*)(strings + 0x200));
        } else {
            GSresRegisterResource(entry, resId, loadMode, (void*)_unloadCamera__FPvUlUl);
        }
    }
    return pub;
}

/* 0x80114948 | 0x74 | floorReadCameraPreFunc (previously proven, matched) */
/* Matches the map pre-load wrapper, but forces the camera load flag. */
void* floorReadCameraPreFunc(void* owner, u32 param, u32 alloc_size) {
    u32 total = ((alloc_size + 0x1F) & ~0x1F) + 0x60;
    HSDArchiveBuffer* archive = GSresAllocResourceAlign(total, 0x20, (u32)owner, (param & 0x7FFF0000) | 0x400, (void*)0);
    if (archive == (void*)0) {
        GSlogWrite(lbl_80272428, total);
        return (void*)0;
    }
    return archive->payload;
}

/* 0x801149BC | 0xB4 | matched, unproven name */
void* floorReadObjPostFunc(u32 resId, u32 loadMode, u32 dataSize) {
    extern void* GSresGetResource(void);
    extern void HSD_ArchiveParse(void* archive, void* buf, u32 size);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* sym);
    extern void GSresRegisterResource(void* entry, u32 resId, u32 flags, u32 cb);
    HSDArchiveBuffer* archive;
    void* pub;
    void* entry;
    u32 flags;
    u32 offset;
    u32 counter;

    counter = 0;
    archive = GSresGetResource();
    HSD_ArchiveParse(archive, archive->payload, dataSize);
    pub = HSD_ArchiveGetPublicAddress(archive, lbl_802722AC);
    if (pub == (void*)0) {
        return (void*)0;
    }
    flags = (loadMode & 0x7FFF0000) | 0x1000;
    if (*(u32*)pub != 0) {
        offset = 0;
        while ((entry = *(void**)(*(u8**)pub + offset)) != 0) {
            GSresRegisterResource(entry, resId, flags | counter, 0);
            offset += 4;
            counter++;
        }
    }
    return pub;
}

/* 0x80114A70 | 0x70 | floorReadObjPreFunc (string-proven: lbl_80272460) */
void* floorReadObjPreFunc(void* owner, u32 param, u32 alloc_size) {
    u32 total = ((alloc_size + 0x1F) & ~0x1F) + 0x60;
    HSDArchiveBuffer* archive = GSresAllocResourceAlign(total, 0x20, (u32)owner, param, (void*)0);
    if (archive == (void*)0) {
        GSlogWrite(lbl_80272460, total);
        return (void*)0;
    }
    return archive->payload;
}

/* NOTE: 0x80114AE0 belongs to the NEXT unit (field_range_80114AE0.c) per
 * splits.txt (this unit ends at 0x80114AE0, exclusive). A definition for
 * it was previously (incorrectly) included here; a cross-unit definition
 * is an invariant hazard, so it has been removed. It remains asm-only in
 * its own unit. */
