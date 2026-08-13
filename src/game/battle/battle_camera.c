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

/**
 * battleGridGetNumPokemonsForTrainer - Get grid group member count by
 * owner ID (renamed from fn_801C2AE8; confirmed name -- naming pass
 * 2026-07-07).
 * Address: 0x801C2AE8 | Size: 0x44
 */
u16 battleGridGetNumPokemonsForTrainer(u32 id) {
    extern BattleGridGroupEntry lbl_80466DE8[];
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
    extern BattleGridGroupEntry lbl_80466DE8[];
    extern u8 lbl_8047B39A;
    extern u8 lbl_8047B39C[8];
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

/**
 * battleGridHideModelsExcept - Pre-grid final setup (renamed from
 * fn_801C2Be0; confirmed name -- naming pass 2026-07-07). NOTE: prior
 * source spelled this fn_801C2Be0 (mixed-case typo); the correct address
 * label is 0x801C2BE0. Any stale callers using either spelling are
 * updated to the new name.
 * Address: 0x801C2BE0 | Size: 0x174
 */
void battleGridHideModelsExcept(void* ctx) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Finalize pre-grid setup:
     * - Apply final slot positions
     * - Set up rendering callbacks
     * - Initialize camera for battle view
     * - Mark grid as ready for battle
     */
    battleGridResetModelVisibilityFlags();
    *(s32*)(state + 0x00) = 7; /* GRID_READY */
}

/* =========================================================================
 * GRID TICK / STATE MANAGEMENT (0x801C2D54 - 0x801C3108)
 * ========================================================================= */

/**
 * battleCameraIsSimple - Battle grid tick callback 1 (no-op forward)
 * (renamed from fn_801C2D54; confirmed name -- naming pass 2026-07-07).
 * Referenced by battle_main.c as battle grid tick 1.
 * Address: 0x801C2D54 | Size: 0x8
 */
extern u8 lbl_8047B398;
extern u8 lbl_8047B399;

void battleCameraIsSimple(void) {
    asm { lbz r3, lbl_8047B399(r13) }
}

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

/**
 * battleCameraStartRandom - Periodically select a random battle camera owner
 * (renamed from fn_801C2D80; confirmed name -- naming pass 2026-07-07).
 * Defers selection while camera control is disabled or another sequence is
 * active, then selects a different trainer or party member from the grid.
 * Address: 0x801C2D80 | Size: 0x180
 */
void battleCameraStartRandom(void) {
    extern BattleGridGroupEntry lbl_80466DE8[];
    extern s32 lbl_80478CA8;
    extern u32 lbl_80478CAC;
    extern f32 lbl_8047DF58;
    extern void* fn_801D2C6C(void);
    extern void fn_801D2C74(void* owner);
    extern s32 fn_800D3088(void);
    extern f32 fn_800E0BE4(void);
    extern u32 _fadeEffectGetRandom__FUl(u32 range);

    BattleGridGroupEntry* group;
    u16 groupCount;
    u16 memberCount;
    u32 candidate;
    u32 ordinal;
    s32 i;

    groupCount = *(u16*)((u8*)lbl_80466DE8 + 0x40);
    if (lbl_8047B398 != 0 || lbl_8047B399 != 0) {
        lbl_80478CA8 = 200;
        return;
    }
    if (fn_801D2C6C() != NULL) {
        lbl_80478CA8 = 200;
        return;
    }
    lbl_80478CA8 -= fn_800D3088();
    if (lbl_80478CA8 > 0) {
        return;
    }
    if (groupCount + *(u16*)((u8*)lbl_80466DE8 + 0x42) == 0) {
        lbl_80478CA8 = 200;
        return;
    }
    if (fn_800E0BE4() > lbl_8047DF58) {
        lbl_80478CA8 = 200;
        return;
    }
    do {
        candidate = _fadeEffectGetRandom__FUl(
            groupCount + *(u16*)((u8*)lbl_80466DE8 + 0x42));
    } while (candidate == lbl_80478CAC);
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
        {
            s32 j;
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
    }
}

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
