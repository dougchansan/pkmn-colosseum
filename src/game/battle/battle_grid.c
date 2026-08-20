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
 * battleGridResetModelVisibilityFlags - Apply the cached per-model
 * visibility flags to every grid slot, then drop the cache.
 *
 * The retail TU keeps an out-of-line copy of this helper at 0x801C2B2C
 * (decompiled in game/battle/battle_camera.c) and, with -inline auto,
 * also inlines it into battleGrid_Init below -- 0x801C3114 has no `bl`
 * to it, but carries the whole loop verbatim. Reproducing that needs a
 * real call the compiler can inline: hand-expanding the body at the call
 * site schedules the group pointer and the shared zero constant
 * differently and does not match. Declared file-local here so the object
 * keeps only the inlined instance and never redefines the 0x801C2B2C
 * symbol.
 */
static inline void battleGridResetModelVisibilityFlags(void) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern u8 lbl_8047B39A;
    extern u8 lbl_8047B39C[12] __attribute__((section(".sdata")));
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group;
    u16 i;
    u16 j;
    u16 visibilityIndex;

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

    memset(&lbl_80466DE8, 0, 0x44);
    lbl_80478CAC = -1;
    lbl_80478CA8 = 200;
    lbl_8047B399 = 0;
    battleGridResetModelVisibilityFlags();
}

/**
 * battleGridGetMaxPokemonField - Highest field/level value among the
 * Pokemon currently occupying the grid, or 0 when no trainer is set up.
 *
 * Inferred inline helper: it owns no retail address, because -inline
 * auto expands it into every call site -- directly in
 * battleGridGetBaseDistance and battleGridApplyFloorScale, and through
 * those into battleGrid_Setup, battleGridGetDistance,
 * battleGridGetNormalisedScale and battleGridUpdate (which carries two
 * separate expansions of it). The expansion is what writes the group
 * pointer straight into its callee-saved register
 * (`addi rN, r3, lbl_80466DE8@l`); the same code written flat in a
 * caller costs an extra `mr` and one instruction more than retail.
 *
 * The declaration order below is not arbitrary. It is the order that
 * makes battleGridGetDistance and battleGridGetNormalisedScale come out
 * byte-exact, and those two are compiled as separate units under
 * different flags (-O4,s and MW 1.2.5n) from the -O4,p unit that scores
 * battleGrid_Setup -- two independent confirmations of the same order.
 */
static inline s32 battleGridGetMaxPokemonField(void) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 fn_801DAC24(void*);
    u16 i;
    u16 j;
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u8** pokemon;
    s32 maxField = -2;

    if (lbl_80466DE8.count == 0) {
        return 0;
    }
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
    return maxField;
}

typedef struct FloorData {
    u8 pad_00[8];
    u32 resourceId;
} FloorData;

typedef struct ModelList {
    void** models;
} ModelList;

/**
 * battleGridGetBaseDistance - Grid spacing for the largest Pokemon class
 * currently on the field. Inline helper behind battleGridGetDistance,
 * battleGridGetNormalisedScale and battleGridUpdate.
 */
static inline f32 battleGridGetBaseDistance(void) {
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    f32 distance;

    switch (battleGridGetMaxPokemonField()) {
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
    return distance;
}

/**
 * battleGridApplyFloorScale - Scale every floor model for the current
 * Pokemon class and hand the same scale to the camera. Inlined into
 * battleGrid_Setup and battleGridUpdate.
 */
static inline void battleGridApplyFloorScale(void) {
    extern const f32 lbl_8047DF5C;
    extern const f32 lbl_8047DF60;
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern char lbl_802757FC[];
    extern FloorData* floorDataBiosGetCurrentPtr(void);
    extern void* fn_80113F48(void);
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern void* GSresGetResource(void*, u32);
    extern ModelList* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern u32 floorReadMakeModelResID(u32);
    extern void GSmodelSetScale(void*, f32*);
    extern void cameraSetOffsetScale(f32*);
    FloorData* floor;
    ModelList* modelList;
    void* resourceBase;
    void* resource;
    s32 maxField;
    u32 modelId;
    u32 modelIndex;
    f32 scaleValue;
    f32 scale[3];

    maxField = battleGridGetMaxPokemonField();

    floor = floorDataBiosGetCurrentPtr();
    resourceBase = fn_80113F48();
    modelIndex = 0;
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
            for (; modelList->models[modelIndex] != NULL; modelIndex++) {
                void* model =
                    GSresGetResource(resourceBase, modelId | modelIndex);
                if (model != NULL) {
                    GSmodelSetScale(model, scale);
                }
            }
        }
    }
    cameraSetOffsetScale(scale);
}

/**
 * fn_801C31EC / battleGrid_Setup - Full grid setup with model loading.
 * Address: 0x801C31EC | Size: 0x244
 * Referenced by battle_main.c (battle_FightEnd calls this for cleanup).
 */
void fn_801C31EC(void) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 lbl_80478CA8;
    extern s32 lbl_80478CAC;
    extern u8 lbl_8047B399;

    memset(&lbl_80466DE8, 0, 0x44);
    lbl_80478CAC = -1;
    lbl_80478CA8 = 200;

    battleGridApplyFloorScale();

    lbl_8047B399 = 0;
    battleGridResetModelVisibilityFlags();
}
/**
 * battleGridPlaceModel - Point one grid object's model at a position and
 * rotation, then stamp its facing byte. Inlined at every call site.
 */
static inline void battleGridPlaceModel(u8* obj, f32* pos, f32* rot,
                                        u8 mirrored) {
    extern void* fn_801DAC3C(void*);
    extern void GSmodelSetPosition(void*, f32*);
    extern void GSmodelSetRotation(void*, f32*);
    void* model;
    s8 facing;

    if (obj != NULL) {
        model = fn_801DAC3C(obj);
        if (model != NULL) {
            GSmodelSetPosition(model, pos);
            GSmodelSetRotation(model, rot);
            facing = 1;
            if (mirrored != 0) {
                facing = -1;
            }
            obj[0x76] = facing;
        }
    }
}

/**
 * battleGridUpdate - Lay out the whole grid: floor scale, then a
 * position/rotation pass over the four slots and their Pokemon.
 * Address: 0x801C3430 | Size: 0x634
 *
 * 98.79%, not yet exact. Instruction count (401), stack layout, control
 * flow, both scan expansions and the whole placement loop agree with
 * retail. Two things are left. Retail folds the early-out test and the
 * first scan's own count test into a single load and compare off one
 * long-lived table register; we re-materialise the address and re-read
 * the count for the scan, costing four instructions. Threading the
 * table through the helpers as a parameter expresses that but scores
 * worse (98.51), as do reading the count through the group pointer,
 * through the global directly, checking before or after the group
 * assignment, wrapping the body in `if (count != 0)`, and expanding the
 * base-distance switch here instead of calling the helper. The other is
 * `lfd f1, @300` where retail has `lfd f1, lbl_8047DF98`: that is the
 * int-to-float conversion literal mwcc emits for us as a fresh .sdata2
 * entry, against a target that references the pooled one.
 */
void battleGridUpdate(void) {
    extern BattleGridGroupTable lbl_80466DE8;
    extern const f32 lbl_8047DF78;
    extern const f32 lbl_8047DF7C;
    extern const f32 lbl_8047DF80;
    extern const f32 lbl_8047DF84;
    extern const f32 lbl_8047DF88;
    extern const f32 lbl_8047DF8C;
    extern const f32 lbl_8047DF90;
    extern void clear__5GSvecFv(f32*);
    extern void GSvecCopy(f32*, f32*);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupTable* table;
    BattleGridGroupEntry* group;
    f32 distance;
    f32 negDistance;
    f32 slotXMirror;
    f32 modelXMirror;
    f32 slotX;
    f32 modelX;
    f32 depthScale;
    f32 soloOffset;
    f32 pairOffset;
    u16 i;
    f32 pos[3];
    f32 rot[3];
    f32 slotPos[3];
    f32 pairPos[3];

    table = &lbl_80466DE8;
    group = table->entries;
    if (table->count == 0) {
        return;
    }

    distance = battleGridGetBaseDistance();
    battleGridApplyFloorScale();

    clear__5GSvecFv(slotPos);
    clear__5GSvecFv(pos);
    clear__5GSvecFv(rot);

    negDistance = -distance;
    slotX = lbl_8047DF78 * distance;
    slotXMirror = lbl_8047DF78 * negDistance;
    modelXMirror = lbl_8047DF7C * negDistance;
    modelX = lbl_8047DF7C * distance;
    depthScale = lbl_8047DF80 * distance;
    soloOffset = lbl_8047DF84 * distance;
    pairOffset = lbl_8047DF88 * distance;

    for (i = 0; i < 4; i++, group++) {
        if (group->slot != NULL) {
            f32 depth;

            if (group->arg1 != 0) {
                slotPos[0] = slotXMirror;
                pos[0] = modelXMirror;
                rot[1] = lbl_8047DF8C;
            } else {
                slotPos[0] = slotX;
                pos[0] = modelX;
                rot[1] = lbl_8047DF90;
            }

            depth = depthScale * (f32)(s8)group->arg2;
            slotPos[2] = depth;
            pos[2] = depth;

            switch (group->memberCount) {
            case 0:
                break;
            case 1:
                slotPos[2] = depth + soloOffset;
                if (group->pokemon[0] != NULL) {
                    battleGridPlaceModel(group->pokemon[0], pos, rot,
                                         group->arg1);
                } else {
                    battleGridPlaceModel(group->pokemon[1], pos, rot,
                                         group->arg1);
                }
                break;
            case 2:
                GSvecCopy(pairPos, pos);
                if (group->arg1 != 0) {
                    pairPos[2] = pos[2] - pairOffset;
                    battleGridPlaceModel(group->pokemon[0], pairPos, rot,
                                         group->arg1);
                    pairPos[2] = pos[2] + pairOffset;
                    battleGridPlaceModel(group->pokemon[1], pairPos, rot,
                                         group->arg1);
                } else {
                    pairPos[2] = pos[2] - pairOffset;
                    battleGridPlaceModel(group->pokemon[1], pairPos, rot,
                                         group->arg1);
                    pairPos[2] = pos[2] + pairOffset;
                    battleGridPlaceModel(group->pokemon[0], pairPos, rot,
                                         group->arg1);
                }
                break;
            }

            battleGridPlaceModel(group->slot, slotPos, rot, group->arg1);
            fn_801DA4E8(group->slot, 1);
        }
    }
}
/**
 * battleGridGetDistance (renamed from fn_801C3A64; confirmed name --
 * naming pass 2026-07-07).
 * Address: 0x801C3A64 | Size: 0x11C
 * Chooses the battle-grid distance from the largest active Pokemon class.
 */
f32 battleGridGetDistance(u8 side) {
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DF78;
    extern const f32 lbl_8047DF7C;
    s32 maxField;
    f32 distance;

    maxField = battleGridGetMaxPokemonField();

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
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DFA0;
    s32 maxField;
    f32 scaleValue;

    maxField = battleGridGetMaxPokemonField();

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
