/**
 * @file battle_grid.c
 * @brief Battle grid core API -- grid state, setup, model loading, and
 *        position/scale bookkeeping for the 4-slot double battle layout.
 *
 * Address range: 0x801C3108 - 0x801C4078 (11 functions).
 *
 * This is the shrunk remainder of the former monolithic battle_grid.c
 * CodeCandidate bucket (0x801C0F20 - 0x801C4CB8, split pass 2026-07-07):
 * the true XD translation unit game/pxdvs/app/battleGrid/battleGrid.cpp.
 * The HSD library code, battle camera helpers, and fade/fade-effect code
 * that used to live in this file moved out to
 * hsd/hsd_aobj_range_801C01C8.c, game/battle/battle_camera.c,
 * game/effect/fade.c, and game/effect/fade_effect.c respectively.
 *
 * The battle grid uses a 4-slot layout corresponding to:
 *   Slot 0: Player Left   Slot 1: Player Right
 *   Slot 2: Enemy Left    Slot 3: Enemy Right
 */

#include "dolphin/types.h"
#include "game/battle/battle_grid_types.h"

void battleGridUpdate(void);
f32 battleGridGetDistance(u8 side);
void battleGridGetNormalisedScale(f32* scale);

/* CRT */
extern void* memset(void* dst, int val, u32 size);

/* HSD (SysDolphin) model/animation */
extern void  fn_8036A384(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetTranslate */
extern void  fn_8036A478(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetScale */
extern void  fn_8036A2D8(void* jobj, f32 rx, f32 ry, f32 rz); /* HSD_JObjSetRotation */
extern void  fn_80362E40(void* jobj, f32 frame);            /* HSD_JObjReqAnimAll */

/* Grid group entry/table -- see battleGridGetNumPokemonsForTrainer
 * (game/battle/battle_camera.c) for the sibling BattleGridGroupEntry[]
 * view of the same lbl_80466DE8 storage (kept as an independent local
 * type here too, as in the original monolithic file). */
typedef struct BattleGridGroupEntry {
    u8* slot;
    u8* pokemon[2];
    u16 memberCount;
    u8 arg1;
    u8 arg2;
} BattleGridGroupEntry;

typedef struct BattleGridGroupTable {
    BattleGridGroupEntry entries[4];
    u16 count;
    u16 pokemonCount;
} BattleGridGroupTable;

#if !defined(BATTLE_GRID_RESIDUAL_PREFIX_801C3114_ONLY) && \
    !defined(BATTLE_GRID_RESIDUAL_MIDDLE_801C3E3C_ONLY)
#define BATTLE_GRID_ALL
#endif

#if defined(BATTLE_GRID_ALL)

/**
 * battleGridGetPtr - Get current grid state (renamed from fn_801C3108;
 * confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3108 | Size: 0xC
 */
void* battleGridGetPtr(void) {
    extern u8 lbl_80466DE8[];
    return lbl_80466DE8;
}

#endif

#if defined(BATTLE_GRID_ALL) || \
    defined(BATTLE_GRID_RESIDUAL_PREFIX_801C3114_ONLY)

/**
 * fn_801C3114 / battleGrid_Init - Initialize the battle grid.
 * Address: 0x801C3114 | Size: 0xD8
 * Clears all grid slots, initializes the camera state,
 * sets up the 4-position double battle layout.
 */
void fn_801C3114(void) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 lbl_80478CA8;
    extern s32 lbl_80478CAC;
    extern u8 lbl_8047B399;
    extern u8 lbl_8047B39A;
    extern u8 lbl_8047B39C[12] __attribute__((section(".sdata")));
    extern void fn_801DA4E8(void*, u32);
    u16 visibilityIndex;
    u16 j;
    u16 i;
    BattleGridGroupEntry* group = lbl_80466DE8.entries;

    memset(&lbl_80466DE8, 0, 0x44);
    visibilityIndex = 0;
    lbl_80478CAC = -1;
    lbl_80478CA8 = 200;
    lbl_8047B399 = 0;

    if (lbl_8047B39A != 0) {
        for (i = 0; i < 4; i++, group++) {
            fn_801DA4E8(group->slot, lbl_8047B39C[visibilityIndex++]);
            for (j = 0; j < 2; j++) {
                fn_801DA4E8(group->pokemon[j],
                            lbl_8047B39C[visibilityIndex++]);
            }
        }
        lbl_8047B39A = 0;
    }
}

/**
 * fn_801C31EC / battleGrid_Setup - Full grid setup with model loading.
 * Address: 0x801C31EC | Size: 0x244
 * Referenced by battle_main.c (battle_FightEnd calls this for cleanup).
 * Sets up the complete battle field layout including stage model,
 * position markers, and initial camera placement.
 */
void fn_801C31EC(void) {
    typedef struct FloorData {
        u8 pad_00[8];
        u32 resourceId;
    } FloorData;
    typedef struct ModelList {
        void** models;
    } ModelList;
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 lbl_80478CA8;
    extern s32 lbl_80478CAC;
    extern u8 lbl_8047B399;
    extern u8 lbl_8047B39A;
    extern u8 lbl_8047B39C[12] __attribute__((section(".sdata")));
    extern const f32 lbl_8047DF5C;
    extern const f32 lbl_8047DF60;
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern char lbl_802757FC[];
    extern s32 fn_801DAC24(void*);
    extern FloorData* floorDataBiosGetCurrentPtr(void);
    extern void* fn_80113F48(void);
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern void* GSresGetResource(void*, u32);
    extern ModelList* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern u32 floorReadMakeModelResID(u32);
    extern void GSmodelSetScale(void*, f32*);
    extern void cameraSetOffsetScale(f32*);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group;
    FloorData* floor;
    ModelList* modelList;
    void* resourceBase;
    void* resource;
    s32 maxField;
    u32 modelId;
    f32 scaleValue;
    u16 visibilityIndex;
    u16 i;
    u16 j;
    f32 scale[3];

    memset(&lbl_80466DE8, 0, 0x44);
    lbl_80478CAC = -1;
    lbl_80478CA8 = 200;

    maxField = -2;
    group = lbl_80466DE8.entries;
    if (lbl_80466DE8.count == 0) {
        maxField = 0;
    } else {
        for (i = 0; i < 4; i++, group++) {
            if (group->slot != NULL) {
                for (j = 0; j < 2; j++) {
                    if (group->pokemon[j] != NULL) {
                        s32 field = fn_801DAC24(group->pokemon[j]);
                        if (field > maxField) {
                            maxField = field;
                        }
                    }
                }
            }
        }
    }

    floor = floorDataBiosGetCurrentPtr();
    resourceBase = fn_80113F48();
    switch (maxField) {
    case 1:
        scaleValue = lbl_8047DF5C;
        break;
    case 2:
        scaleValue = lbl_8047DF60;
        break;
    case 3:
        scaleValue = lbl_8047DF64;
        break;
    default:
        scaleValue = lbl_8047DF68;
        break;
    }
    set__5GSvecFfff(scale, scaleValue, scaleValue, scaleValue);

    resource = GSresGetResource(fn_80113F48(), floor->resourceId);
    if (resource != NULL) {
        modelList = HSD_ArchiveGetPublicAddress(resource, lbl_802757FC);
        if (modelList != NULL && modelList->models != NULL) {
            modelId = floorReadMakeModelResID(floor->resourceId);
            for (i = 0; modelList->models[i] != NULL; i++) {
                void* model = GSresGetResource(resourceBase, modelId | i);
                if (model != NULL) {
                    GSmodelSetScale(model, scale);
                }
            }
        }
    }
    cameraSetOffsetScale(scale);

    lbl_8047B399 = 0;
    group = lbl_80466DE8.entries;
    visibilityIndex = 0;
    if (lbl_8047B39A != 0) {
        for (i = 0; i < 4; i++, group++) {
            fn_801DA4E8(group->slot, lbl_8047B39C[visibilityIndex++]);
            for (j = 0; j < 2; j++) {
                fn_801DA4E8(group->pokemon[j],
                            lbl_8047B39C[visibilityIndex++]);
            }
        }
        lbl_8047B39A = 0;
    }
}

/**
 * battleGridUpdate - Main grid setup (large) (renamed from fn_801C3430;
 * confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3430 | Size: 0x634
 * This is the primary grid initialization function that:
 *   1. Loads the stage model from FDAT
 *   2. Sets up position transforms for all 4 battle slots
 *   3. Configures lighting and shadow rendering
 *   4. Sets up the battle camera default view
 *   5. Initializes the model animation system
 */
void battleGridUpdate(void) {
    typedef struct FloorData {
        u8 pad_00[8];
        u32 resourceId;
    } FloorData;
    typedef struct ModelList {
        void** models;
    } ModelList;
    typedef struct BattleGridRenderable {
        u8 pad_00[0x76];
        s8 facing;
    } BattleGridRenderable;
    extern BattleGridGroupTable lbl_80466DE8;
    extern const f32 lbl_8047DF5C;
    extern const f32 lbl_8047DF60;
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DF78;
    extern const f32 lbl_8047DF7C;
    extern const f32 lbl_8047DF80;
    extern const f32 lbl_8047DF84;
    extern const f32 lbl_8047DF88;
    extern const f32 lbl_8047DF8C;
    extern const f32 lbl_8047DF90;
    extern char lbl_802757FC[];
    extern s32 fn_801DAC24(void*);
    extern FloorData* floorDataBiosGetCurrentPtr(void);
    extern void* fn_80113F48(void);
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern void* GSresGetResource(void*, u32);
    extern ModelList* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern u32 floorReadMakeModelResID(u32);
    extern void GSmodelSetScale(void*, f32*);
    extern void cameraSetOffsetScale(f32*);
    extern void clear__5GSvecFv(f32*);
    extern void GSvecCopy(f32*, f32*);
    extern void* fn_801DAC3C(void*);
    extern void GSmodelSetPosition(void*, f32*);
    extern void GSmodelSetRotation(void*, f32*);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group;
    FloorData* floor;
    ModelList* modelList;
    void* resourceBase;
    void* model;
    s32 distanceField;
    s32 scaleField;
    u32 modelId;
    f32 distance;
    f32 stageScaleValue;
    f32 scale[3];
    f32 slotPosition[3];
    f32 pokemonPosition[3];
    f32 rotation[3];
    f32 xOffset;
    f32 slotOffset;
    f32 sideScale;
    f32 pokemonSpread;
    f32 leftX;
    f32 rightX;
    f32 leftZ;
    f32 rightZ;
    u16 i;
    u16 j;

    if (lbl_80466DE8.count == 0) {
        return;
    }

    distanceField = -2;
    group = lbl_80466DE8.entries;
    for (i = 0; i < 4; i++, group++) {
        if (group->slot == NULL) {
            continue;
        }
        for (j = 0; j < 2; j++) {
            if (group->pokemon[j] != NULL) {
                s32 field = fn_801DAC24(group->pokemon[j]);
                if (field > distanceField) {
                    distanceField = field;
                }
            }
        }
    }

    switch (distanceField) {
    case -2:
        distance = lbl_8047DF6C;
        break;
    case 1:
        distance = lbl_8047DF64;
        break;
    case 2:
        distance = lbl_8047DF70;
        break;
    case 3:
        distance = lbl_8047DF74;
        break;
    default:
        distance = lbl_8047DF68;
        break;
    }

    scaleField = -2;
    group = lbl_80466DE8.entries;
    for (i = 0; i < 4; i++, group++) {
        if (group->slot == NULL) {
            continue;
        }
        for (j = 0; j < 2; j++) {
            if (group->pokemon[j] != NULL) {
                s32 field = fn_801DAC24(group->pokemon[j]);
                if (field > scaleField) {
                    scaleField = field;
                }
            }
        }
    }

    floor = floorDataBiosGetCurrentPtr();
    resourceBase = fn_80113F48();
    switch (scaleField) {
    case 1:
        stageScaleValue = lbl_8047DF5C;
        break;
    case 2:
        stageScaleValue = lbl_8047DF60;
        break;
    case 3:
        stageScaleValue = lbl_8047DF64;
        break;
    default:
        stageScaleValue = lbl_8047DF68;
        break;
    }
    set__5GSvecFfff(scale, stageScaleValue, stageScaleValue, stageScaleValue);

    model = GSresGetResource(fn_80113F48(), floor->resourceId);
    if (model != NULL) {
        modelList = HSD_ArchiveGetPublicAddress(model, lbl_802757FC);
        if (modelList != NULL && modelList->models != NULL) {
            modelId = floorReadMakeModelResID(floor->resourceId);
            for (i = 0; modelList->models[i] != NULL; i++) {
                model = GSresGetResource(resourceBase, modelId | i);
                if (model != NULL) {
                    GSmodelSetScale(model, scale);
                }
            }
        }
    }
    cameraSetOffsetScale(scale);

    clear__5GSvecFv(slotPosition);
    clear__5GSvecFv(pokemonPosition);
    clear__5GSvecFv(rotation);

    leftX = lbl_8047DF78 * distance;
    rightX = lbl_8047DF78 * -distance;
    leftZ = lbl_8047DF7C * distance;
    rightZ = lbl_8047DF7C * -distance;
    sideScale = lbl_8047DF80 * distance;
    slotOffset = lbl_8047DF84 * distance;
    pokemonSpread = lbl_8047DF88 * distance;

    group = lbl_80466DE8.entries;
    for (i = 0; i < 4; i++, group++) {
        BattleGridRenderable* renderable;
        s8 facing;

        if (group->slot == NULL) {
            continue;
        }

        if (group->arg1 != 0) {
            slotPosition[0] = rightX;
            pokemonPosition[0] = rightZ;
            rotation[1] = lbl_8047DF8C;
        } else {
            slotPosition[0] = leftX;
            pokemonPosition[0] = leftZ;
            rotation[1] = lbl_8047DF90;
        }

        xOffset = sideScale * (s8) group->arg2;
        slotPosition[2] = xOffset;
        pokemonPosition[2] = xOffset;
        facing = (group->arg1 != 0) ? -1 : 1;

        switch (group->memberCount) {
        case 1:
            slotPosition[2] = xOffset + slotOffset;
            renderable = (BattleGridRenderable*) group->pokemon[0];
            if (renderable == NULL) {
                renderable = (BattleGridRenderable*) group->pokemon[1];
            }
            if (renderable != NULL) {
                model = fn_801DAC3C(renderable);
                if (model != NULL) {
                    GSmodelSetPosition(model, pokemonPosition);
                    GSmodelSetRotation(model, rotation);
                    renderable->facing = facing;
                }
            }
            break;
        case 2:
            GSvecCopy(scale, pokemonPosition);
            if (group->arg1 != 0) {
                scale[2] = xOffset - pokemonSpread;
                renderable = (BattleGridRenderable*) group->pokemon[0];
                if (renderable != NULL) {
                    model = fn_801DAC3C(renderable);
                    if (model != NULL) {
                        GSmodelSetPosition(model, scale);
                        GSmodelSetRotation(model, rotation);
                        renderable->facing = facing;
                    }
                }

                scale[2] = xOffset + pokemonSpread;
                renderable = (BattleGridRenderable*) group->pokemon[1];
                if (renderable != NULL) {
                    model = fn_801DAC3C(renderable);
                    if (model != NULL) {
                        GSmodelSetPosition(model, scale);
                        GSmodelSetRotation(model, rotation);
                        renderable->facing = facing;
                    }
                }
            } else {
                scale[2] = xOffset - pokemonSpread;
                renderable = (BattleGridRenderable*) group->pokemon[1];
                if (renderable != NULL) {
                    model = fn_801DAC3C(renderable);
                    if (model != NULL) {
                        GSmodelSetPosition(model, scale);
                        GSmodelSetRotation(model, rotation);
                        renderable->facing = facing;
                    }
                }

                scale[2] = xOffset + pokemonSpread;
                renderable = (BattleGridRenderable*) group->pokemon[0];
                if (renderable != NULL) {
                    model = fn_801DAC3C(renderable);
                    if (model != NULL) {
                        GSmodelSetPosition(model, scale);
                        GSmodelSetRotation(model, rotation);
                        renderable->facing = facing;
                    }
                }
            }
            break;
        }

        renderable = (BattleGridRenderable*) group->slot;
        if (renderable != NULL) {
            model = fn_801DAC3C(renderable);
            if (model != NULL) {
                GSmodelSetPosition(model, slotPosition);
                GSmodelSetRotation(model, rotation);
                renderable->facing = facing;
            }
        }
        fn_801DA4E8(group->slot, 1);
    }
}

/**
 * battleGridGetDistance (renamed from fn_801C3A64; confirmed name --
 * naming pass 2026-07-07).
 * Address: 0x801C3A64 | Size: 0x11C
 * Chooses the battle-grid distance from the largest active Pokemon class.
 */
f32 battleGridGetDistance(u8 side) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 fn_801DAC24(void*);
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DF78;
    extern const f32 lbl_8047DF7C;
    s32 maxField;
    u8** pokemon;
    BattleGridGroupEntry* group;
    f32 distance;
    u16 j;
    u16 i;

    group = (BattleGridGroupEntry*)&lbl_80466DE8;
    maxField = -2;
    if (((BattleGridGroupTable*)group)->count == 0) {
        maxField = 0;
    } else {
        for (i = 0; i < 4; i++, group++) {
            if (group->slot != NULL) {
                pokemon = group->pokemon;
                for (j = 0; j < 2; j++, pokemon++) {
                    if (*pokemon != NULL) {
                        s32 field = fn_801DAC24(*pokemon);
                        if (field > maxField) {
                            maxField = field;
                        }
                    }
                }
            }
        }
    }

    switch (maxField) {
    case -2:
    case -1:
        distance = lbl_8047DF6C;
        break;
    case 1:
        distance = lbl_8047DF64;
        break;
    case 2:
        distance = lbl_8047DF70;
        break;
    case 3:
        distance = lbl_8047DF74;
        break;
    default:
        distance = lbl_8047DF68;
        break;
    }

    if (side != 0) {
        return lbl_8047DF7C * distance;
    }
    return lbl_8047DF78 * distance;
}

/**
 * battleGridGetNormalisedScale (renamed from fn_801C3B80; confirmed name --
 * naming pass 2026-07-07).
 * Address: 0x801C3B80 | Size: 0x118
 * Chooses the model scale from the largest active Pokemon class.
 */
void battleGridGetNormalisedScale(f32* scale) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 fn_801DAC24(void*);
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DFA0;
    s32 maxField;
    u8** pokemon;
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    f32 scaleValue;
    u16 j;
    u16 i;

    maxField = -2;
    if (lbl_80466DE8.count == 0) {
        maxField = 0;
    } else {
        for (i = 0; i < 4; i++, group++) {
            if (group->slot != NULL) {
                pokemon = group->pokemon;
                for (j = 0; j < 2; j++, pokemon++) {
                    if (*pokemon != NULL) {
                        s32 field = fn_801DAC24(*pokemon);
                        if (field > maxField) {
                            maxField = field;
                        }
                    }
                }
            }
        }
    }

    switch (maxField) {
    case -2:
    case -1:
        scaleValue = lbl_8047DF6C;
        break;
    case 1:
        scaleValue = lbl_8047DF64;
        break;
    case 2:
        scaleValue = lbl_8047DF70;
        break;
    case 3:
        scaleValue = lbl_8047DF74;
        break;
    default:
        scaleValue = lbl_8047DF68;
        break;
    }

    scaleValue *= lbl_8047DFA0;
    set__5GSvecFfff(scale, scaleValue, scaleValue, scaleValue);
}

#endif

#if defined(BATTLE_GRID_ALL)

/**
 * battleGridRemovePokemon - Grid slot state update helper (renamed from
 * fn_801C3C98; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3C98 | Size: 0xCC
 */
void battleGridRemovePokemon(u8* pokemon) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;
    u16 j;

    if (pokemon != NULL) {
        for (i = 0; i < 4; i++, group++) {
            for (j = 0; j < 2; j++) {
                if (group->pokemon[j] == pokemon) {
                    group->pokemon[j] = NULL;
                    group->memberCount--;
                    lbl_80466DE8.pokemonCount--;
                    fn_801DA4E8(pokemon, 0);
                    pokemon[0x76] = 0;
                    return;
                }
            }
        }
    }
}

/**
 * battleGridReplacePokemon / battleGridReplacePokemon - Replace Pokemon model in a grid slot.
 * Address: 0x801C3D64 | Size: 0xD8
 * Proposed name from symbols: battleGridReplacePokemon.
 * Removes the current Pokemon model from a slot and loads a new one.
 */
void battleGridReplacePokemon(u8* pokemon, u8* replacement) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern char lbl_80275808[];
    extern void GSlogWrite(const char*, ...);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;
    u16 j;

    if (pokemon == NULL || replacement == NULL) {
        GSlogWrite(lbl_80275808);
        return;
    }

    for (i = 0; i < 4; i++, group++) {
        for (j = 0; j < 2; j++) {
            if (group->pokemon[j] == pokemon) {
                group->pokemon[j] = replacement;
                fn_801DA4E8(pokemon, 0);
                replacement[0x76] = pokemon[0x76];
                pokemon[0x76] = 0;
                return;
            }
        }
    }
}

#endif

#if defined(BATTLE_GRID_ALL) || \
    defined(BATTLE_GRID_RESIDUAL_MIDDLE_801C3E3C_ONLY)

/**
 * battleGridAddPokemon - Grid slot model transition animation (renamed
 * from fn_801C3E3C; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3E3C | Size: 0xD4
 */
void battleGridAddPokemon(u8* slot, u8* pokemon) {
    extern BattleGridGroupTable lbl_80466DE8;
    BattleGridGroupTable* table = &lbl_80466DE8;
    BattleGridGroupEntry* group = table->entries;
    u16 i;
    u16 j;

    if (slot == NULL || pokemon == NULL) {
        return;
    }
    if (table->pokemonCount >= 8) {
        return;
    }
    for (i = 0; i < 4; i++, group++) {
        if (group->slot == slot) {
            if (group->memberCount >= 2) {
                return;
            }
            for (j = 0; j < 2; j++) {
                if (group->pokemon[j] == NULL) {
                    group->pokemon[j] = pokemon;
                    group->memberCount++;
                    table->pokemonCount++;
                    pokemon[0x76] = group->arg1 != 0 ? -1 : 1;
                    return;
                }
            }
        }
    }
}

#endif

#if defined(BATTLE_GRID_ALL)

/**
 * battleGridReplaceTrainer / battleGridReplaceTrainer - Replace trainer model in a grid slot.
 * Address: 0x801C3F10 | Size: 0xAC
 * Proposed name from symbols: battleGridReplaceTrainer.
 */
void battleGridReplaceTrainer(u8* trainer, u8* replacement) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern char lbl_80275830[];
    extern void GSlogWrite(const char*, ...);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;

    if (trainer == NULL || replacement == NULL) {
        GSlogWrite(lbl_80275830);
        return;
    }

    for (i = 0; i < 4; i++, group++) {
        if (group->slot == trainer) {
            group->slot = replacement;
            fn_801DA4E8(trainer, 0);
            replacement[0x76] = trainer[0x76];
            trainer[0x76] = 0;
            return;
        }
    }
}

/**
 * battleGridAddTrainer - Add slot to grid group (renamed from
 * fn_801C3FBC; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3FBC | Size: 0xBC
 */
void battleGridAddTrainer(u8* slot, u8 arg1, u8 arg2) {
    extern BattleGridGroupTable lbl_80466DE8;
    BattleGridGroupEntry* group;
    s8 state;

    if (lbl_80466DE8.count < 4) {
        group = &lbl_80466DE8.entries[0];
        if (group->slot != NULL) {
            group = &lbl_80466DE8.entries[1];
            if (group->slot != NULL) {
                group++;
                if (group->slot != NULL) {
                    group++;
                    if (group->slot != NULL) {
                        group++;
                    }
                }
            }
        }
        memset(group, 0, sizeof(*group));
        group->slot = slot;
        state = 1;
        group->arg1 = arg1;
        group->arg2 = arg2;
        if (arg1 != 0) {
            state = -1;
        }
        slot[0x76] = state;
        lbl_80466DE8.count = lbl_80466DE8.count + 1;
    }
}

#endif
