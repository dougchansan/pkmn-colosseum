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
void battleGridGetDistance(void);
void battleGridGetNormalisedScale(void);

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
    s32 i;
    BattleGridSceneWork* sceneWork;

    memset(lbl_80467030, 0, 0x20);
    memset(lbl_80466E50, 0, 0x1E0);

    /* Initialize 4 BattleGridSlot entries with default values */
    sceneWork = (BattleGridSceneWork*)lbl_80466E50;
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        BattleGridSceneSlot* slot = &sceneWork->slots[i];
        slot->active = 0;
        slot->jobj = NULL;
        slot->rotationY = 0.0f;
        slot->scale = 1.0f;
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
    /* Full grid setup with model loading:
     * 1. Initialize grid state
     * 2. Set up stage model (battle colosseum arena)
     * 3. Place position markers for all 4 slots
     * 4. Initialize camera to default battle overhead view
     */
    fn_801C3114();
    battleGridUpdate();
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
    extern void HSD_AObjInterpretAnim(void* ctx, f32 posX, f32 posZ);
    /* Main battle grid setup:
     * 1. Load stage model from FDAT
     * 2. Set up position transforms for all 4 battle slots
     * 3. Configure lighting (ambient + 2 directional)
     * 4. Configure shadow rendering
     * 5. Set up battle camera default overhead view
     * 6. Initialize model animation system
     */
    HSD_AObjInterpretAnim((void*)lbl_80466E50, 0.0f, 0.0f);
    battleGridGetDistance();
    battleGridGetNormalisedScale();
}

/**
 * battleGridGetDistance - Load models for all grid positions (renamed
 * from fn_801C3A64; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3A64 | Size: 0x11C
 * Loads Pokemon and trainer models into each active grid slot.
 */
void battleGridGetDistance(void) {
    s32 i;
    BattleGridSceneWork* state = (BattleGridSceneWork*)lbl_80466E50;

    /* Load Pokemon and trainer models into each active grid slot */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        BattleGridSceneSlot* slot = &state->slots[i];
        s32 active = slot->active;

        if (active != 0) {
            /* Model is already loaded or should be loaded from battle data */
            void* jobj = slot->jobj;
            if (jobj != NULL) {
                f32 x = slot->posX;
                f32 y = slot->posY;
                f32 z = slot->posZ;
                fn_8036A384(jobj, x, y, z);
            }
        }
    }
}

/**
 * battleGridGetNormalisedScale - Update all grid positions (renamed from
 * fn_801C3B80; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C3B80 | Size: 0x118
 * Recalculates world-space positions for all grid slots
 * (e.g., after a Pokemon switch or camera change).
 */
void battleGridGetNormalisedScale(void) {
    s32 i;
    BattleGridSceneWork* state = (BattleGridSceneWork*)lbl_80466E50;

    /* Recalculate world-space positions for all grid slots */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        BattleGridSceneSlot* slot = &state->slots[i];
        s32 active = slot->active;

        if (active == 0) {
            continue;
        }

        {
            void* jobj = slot->jobj;
            if (jobj != NULL) {
                f32 x = slot->posX;
                f32 y = slot->posY;
                f32 z = slot->posZ;
                f32 scale = slot->scale;

                fn_8036A384(jobj, x, y, z);
                fn_8036A478(jobj, scale, scale, scale);
            }
        }
    }
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
