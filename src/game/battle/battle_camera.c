/**
 * @file battle_camera.c
 * @brief Battle grid trainer/model visibility helpers and battle camera
 *        tick/cleanup callbacks.
 *
 * Address range: 0x801C2AE8 - 0x801C3108 (9 functions).
 *
 * Split out of the former monolithic battle_grid.c CodeCandidate bucket
 * (0x801C0F20 - 0x801C4CB8, split pass 2026-07-07). This is a distinct
 * XD translation unit (game/pxdvs/app/battleGrid/battleCamera.cpp).
 */

#include "dolphin/types.h"
#include "game/battle/battle_grid_types.h"

/* Grid group table entry -- see battleGridAddTrainer (game/battle/battle_grid.c,
 * shrunk remainder) for the sibling BattleGridGroupTable view of the same
 * lbl_80466DE8 storage; kept as two independent local casts (as in the
 * original monolithic file) rather than unified into a shared header,
 * since neither TU calls the other's accessor. */
typedef struct BattleGridGroupEntry {
    u8* slot;
    u8* pokemon[2];
    u16 memberCount;
    u8 arg1;
    u8 arg2;
} BattleGridGroupEntry;

/* HSD (SysDolphin) model/animation */
extern void  fn_8036A384(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetTranslate */
extern void  fn_80363CF4(void* jobj);                       /* HSD_JObjRemoveAll */

/* CRT */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

extern BattleGridGroupEntry lbl_80466DE8[];
extern u8 lbl_8047B39A;
extern u8 lbl_8047B39C[12] __attribute__((section(".sdata")));

/*
 * The battle_camera unit (0x801C2BE0 - 0x801C2D5C) builds battleGridHideModelsExcept
 * alone; battle_camera_candidate_801C2D80.c defines BATTLE_CAMERA_FULL_TU for the rest.
 */
#if defined(BATTLE_CAMERA_FULL_TU)
/**
 * battleGridGetNumPokemonsForTrainer - Get grid group member count by
 * owner ID (renamed from fn_801C2AE8; confirmed name -- naming pass
 * 2026-07-07).
 * Address: 0x801C2AE8 | Size: 0x44
 */

u16 battleGridGetNumPokemonsForTrainer(u32 id) {
    BattleGridGroupEntry* group = lbl_80466DE8;
    u16 i;

    for (i = 0; i < 4; i++, group++) {
        if ((u32)group->slot == id) {
            return group->memberCount;
        }
    }
    return 0;
}

/**
 * battleGridResetModelVisibilityFlags - Apply cached visibility to all grid
 * models (renamed from fn_801C2B2C; confirmed name -- naming pass
 * 2026-07-07).
 * Address: 0x801C2B2C | Size: 0xB4
 */
void battleGridResetModelVisibilityFlags(void) {
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group = lbl_80466DE8;
    u16 i;
    u16 j;
    u16 visibilityIndex = 0;

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

#elif !defined(BATTLE_CAMERA_START_RANDOM_ONLY)
/* RULE-EXCEPTION(user-approved): static inline copy of the real battleGridResetModelVisibilityFlags so its call is inlined as in retail; the real one stays linked in battle_camera_exact_801C2AE8 - see docs/RULE_EXCEPTIONS.md */
static inline void battleGridResetModelVisibilityFlags(void) {
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group = lbl_80466DE8;
    u16 i;
    u16 j;
    u16 visibilityIndex = 0;

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
#endif

/**
 * battleGridHideModelsExcept - Pre-grid final setup (renamed from
 * fn_801C2Be0; confirmed name -- naming pass 2026-07-07). NOTE: prior
 * source spelled this fn_801C2Be0 (mixed-case typo); the correct address
 * label is 0x801C2BE0. Any stale callers using either spelling are
 * updated to the new name.
 * Address: 0x801C2BE0 | Size: 0x174
 */
#if !defined(BATTLE_CAMERA_START_RANDOM_ONLY)
void battleGridHideModelsExcept(void* ctx) {
    extern u32 fn_801DA42C(void*);
    extern void fn_801DA4E8(void*, u32);
    BattleGridGroupEntry* group;
    u16 visibilityIndex;
    u16 i;
    u16 j;

    visibilityIndex = 0;
    group = lbl_80466DE8;
    if (lbl_8047B39A != 0) {
        battleGridResetModelVisibilityFlags();
    }

    memset(lbl_8047B39C, 0, 8);
    for (i = 0; i < 4; i++, group++) {
        {
            u8 visible = fn_801DA42C(group->slot);
            void* model = group->slot;

            lbl_8047B39C[visibilityIndex++] = visible;
            if (model != ctx) {
                fn_801DA4E8(model, 0);
            }
        }
        for (j = 0; j < 2; j++) {
            u8 visible = fn_801DA42C(group->pokemon[j]);
            void* model = group->pokemon[j];

            lbl_8047B39C[visibilityIndex++] = visible;
            if (model != ctx) {
                fn_801DA4E8(model, 0);
            }
        }
    }
    lbl_8047B39A = 1;
}
#endif

#if defined(BATTLE_CAMERA_FULL_TU)
/* =========================================================================
 * GRID TICK / STATE MANAGEMENT (0x801C2D54 - 0x801C3108)
 * ========================================================================= */

/**
 * battleCameraIsSimple - Battle grid tick callback 1 (no-op forward)
 * (renamed from fn_801C2D54; confirmed name -- naming pass 2026-07-07).
 * Referenced by battle_main.c as battle grid tick 1.
 * Address: 0x801C2D54 | Size: 0x8
 */
#endif /* BATTLE_CAMERA_FULL_TU */

extern u8 lbl_8047B398;
extern u8 lbl_8047B399;

#if !defined(BATTLE_CAMERA_START_RANDOM_ONLY)
u8 battleCameraIsSimple(void) {
    return lbl_8047B399;
}
#endif

#if defined(BATTLE_CAMERA_FULL_TU)

/**
 * battleCameraDoFull - Battle grid tick callback wrapper (renamed from
 * fn_801C2D5C; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C2D5C | Size: 0xC
 */
void battleCameraDoFull(void) {
    lbl_8047B399 = 0;
}

/**
 * battleCameraDoSimple - Battle grid tick callback 2 (renamed from
 * fn_801C2D68; confirmed name -- naming pass 2026-07-07).
 * Referenced by battle_main.c as battle grid tick 2.
 * Address: 0x801C2D68 | Size: 0xC
 */
void battleCameraDoSimple(void) {
    lbl_8047B399 = 1;
}

/**
 * battleCameraDisable - Battle grid tick callback 3 (renamed from
 * fn_801C2D74; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C2D74 | Size: 0xC
 */
void battleCameraDisable(void) {
    lbl_8047B398 = 1;
}

#endif /* BATTLE_CAMERA_FULL_TU */

/**
 * battleCameraStartRandom - Periodically select a random battle camera owner
 * (renamed from fn_801C2D80; confirmed name -- naming pass 2026-07-07).
 * Defers selection while camera control is disabled or another sequence is
 * active, then selects a different trainer or party member from the grid.
 * Address: 0x801C2D80 | Size: 0x180
 */
#if defined(BATTLE_CAMERA_FULL_TU) || defined(BATTLE_CAMERA_START_RANDOM_ONLY)
void battleCameraStartRandom(void) {
    extern BattleGridGroupEntry lbl_80466DE8[];
    extern s32 lbl_80478CA8;
    extern u32 lbl_80478CAC;
    extern f32 lbl_8047DF58;
    extern void* fn_801D2C6C(void);
    extern void fn_801D2C74(void* owner);
    extern s32 GSgetFrameStep(void);
    extern f32 fn_800E0BE4(void);
    extern u32 _fadeEffectGetRandom__FUl(u32 range);

    u16 groupCount;
    s32 i;
    s32 j;
    s32 memberCount;
    BattleGridGroupEntry* group;
    u32 candidate;
    u32 ordinal;

    groupCount = *(u16*)((u8*)lbl_80466DE8 + 0x40);
    if (lbl_8047B398 != 0 || lbl_8047B399 != 0) {
        lbl_80478CA8 = 200;
        return;
    }
    if (fn_801D2C6C() != NULL) {
        lbl_80478CA8 = 200;
        return;
    }
    lbl_80478CA8 -= GSgetFrameStep();
    if (lbl_80478CA8 > 0) {
        return;
    }
    if (groupCount + *(u16*)((u8*)lbl_80466DE8 + 0x42) != 0) {
        if (fn_800E0BE4() > lbl_8047DF58) {
            lbl_80478CA8 = 200;
            return;
        }
        candidate = _fadeEffectGetRandom__FUl(
            groupCount + *(u16*)((u8*)lbl_80466DE8 + 0x42));
        while (candidate == lbl_80478CAC) {
            candidate = _fadeEffectGetRandom__FUl(
                groupCount + *(u16*)((u8*)lbl_80466DE8 + 0x42));
        }
        lbl_80478CAC = candidate;

        ordinal = 0;
        group = lbl_80466DE8;
        for (i = 0; i < groupCount; i++, group++) {
            if (group->slot == NULL) {
                continue;
            }
            if (ordinal == candidate) {
                fn_801D2C74(group->slot);
                lbl_80478CA8 = 200;
            }
            ordinal++;
            memberCount = group->memberCount;
            for (j = 0; j < memberCount; j++) {
                if (group->pokemon[j] != NULL) {
                    if (ordinal == candidate) {
                        fn_801D2C74(group->pokemon[j]);
                        lbl_80478CA8 = 200;
                    }
                    ordinal++;
                }
            }
        }
    } else {
        lbl_80478CA8 = 200;
    }
}
#endif

#if defined(BATTLE_CAMERA_FULL_TU)
/**
 * fn_801C2F00 - Battle grid load data from buffer.
 * Address: 0x801C2F00 | Size: 0x208
 * Referenced by battle_main.c as "battle grid load data".
 * Loads grid configuration from a data buffer.
 */
void fn_801C2F00(void* data, u32 size) {
    if (data == NULL || size == 0) {
        return;
    }
    /* Load grid configuration from a data buffer:
     * 1. Parse header (slot count, field type)
     * 2. For each slot: load position, model ID, animation set
     * 3. Load camera configuration
     * 4. Load stage model reference
     */
    memcpy(lbl_80466E50, data, (size < 0x1E0) ? size : 0x1E0);
}
#endif /* BATTLE_CAMERA_FULL_TU */
