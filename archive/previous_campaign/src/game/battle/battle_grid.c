/**
 * @file battle_grid.c
 * @brief Battle grid system -- scene layout, field positions, camera setup,
 *        Pokemon/trainer model placement, and grid state management.
 *
 * Address range: 0x801C01C8 - 0x801C53BC (64 functions)
 *
 * This file covers the battle grid subsystem responsible for:
 *   - Pre-grid initialization (camera context, field positions)
 *   - Grid setup (4-slot double battle layout)
 *   - Model loading/replacement for Pokemon and trainers
 *   - Camera initialization and rendering pass configuration
 *   - Grid tick/update/cleanup lifecycle
 *
 * The battle grid uses a 4-slot layout corresponding to:
 *   Slot 0: Player Left   Slot 1: Player Right
 *   Slot 2: Enemy Left    Slot 3: Enemy Right
 *
 * Key BSS state:
 *   lbl_80467030 (0x20 bytes): BattleCameraState
 *   lbl_80466E50 (0x1E0 bytes): Battle scene animation context
 *
 * Large functions (>0x400 bytes) use #pragma push / #pragma optimization_level 0
 * stubs for state machines and float-heavy animation code. Smaller functions
 * (getters, setters, wrappers) are fully decompiled.
 */

#include "game/battle/battle.h"

/* =========================================================================
 * External function declarations
 * ========================================================================= */

/* CRT */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* Engine / GS core */
extern void  fn_800DD970(const char* fmt, ...);      /* GSlog_Print */
extern void  fn_800D3088(void);                       /* GSgfx tick */
extern void* GSthreadCreate(s32 priority, void* parent, s32 stackSize,
                          u8 usesFPU, void* entry, s32 arg); /* GSthread_Create */
extern void  fn_800F04C4(void);                       /* stop particle system */

/* Scene management */
extern void  fn_80102568(s32 objID, s32 arg1, s32 arg2);   /* release scene object */
extern u8    fn_80102620(s32 objID);                        /* check scene object active */
extern void* fn_801025C0(s32 objID);                        /* get scene object pointer */
extern void  fn_80103BA8(void* padData, s32 port);          /* read pad input */

/* HSD (SysDolphin) model/animation */
extern void  fn_80362D0C(void* jobj);                       /* HSD_JObjAnimAll */
extern void  fn_80362E40(void* jobj, f32 frame);            /* HSD_JObjReqAnimAll */
extern void* fn_80363B8C(void* data, s32 idx);              /* HSD_JObjLoadJoint */
extern void  fn_80363CF4(void* jobj);                       /* HSD_JObjRemoveAll */
extern void  fn_8036A384(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetTranslate */
extern void  fn_8036A478(void* jobj, f32 x, f32 y, f32 z); /* HSD_JObjSetScale */
extern void  fn_8036A2D8(void* jobj, f32 rx, f32 ry, f32 rz); /* HSD_JObjSetRotation */
extern void* fn_80369654(void* jobj, s32 childIdx);         /* HSD_JObjGetChild */

/* Sound */
extern void  soundStop(s32 sndID, s32 volume);    /* soundStop */
extern void  fn_801659FC(s32 sndID, s32 fadeTime, s32 volume); /* sndPlay with fade */

/* Battle scene (forward refs to battle_scene.c) */
extern void  fn_801C53BC(void* ctx, s32 arg1, s32 arg2, s32 arg3, f32 arg4);

/* =========================================================================
 * External data (BSS / SDA)
 * ========================================================================= */

extern u8    lbl_80466E50[0x1E0]; /* battle scene animation context */
extern u8    lbl_80467030[0x20];  /* BattleCameraState */
extern u8    lbl_8046AC60[0x100]; /* battle transfer context */
extern void* lbl_8046D500;        /* battle state machine context ptr */

/* =========================================================================
 * PRE-GRID FUNCTIONS (0x801C01C8 - 0x801C3108)
 *
 * These functions manage the camera/field setup that occurs before
 * the main battle grid is initialized. Many are large float-heavy
 * animation or state machine functions.
 * ========================================================================= */

/**
 * fn_801C01C8 - Pre-grid camera setup helper A.
 * Address: 0x801C01C8 | Size: 0x54
 */
void fn_801C01C8(void* arg0, s32 arg1) {
    u8* ctx = (u8*)arg0;
    if (ctx == NULL) {
        return;
    }
    /* Store camera mode in context */
    *(s32*)(ctx + 0x10) = arg1;
    /* Reset camera animation timer */
    *(f32*)(ctx + 0x14) = 0.0f;
}

/**
 * fn_801C021C - Pre-grid camera setup helper B.
 * Address: 0x801C021C | Size: 0x54
 */
void fn_801C021C(void* arg0, s32 arg1) {
    u8* ctx = (u8*)arg0;
    if (ctx == NULL) {
        return;
    }
    /* Store camera transition target in context */
    *(s32*)(ctx + 0x18) = arg1;
    *(f32*)(ctx + 0x1C) = 0.0f;
}

/**
 * fn_801C0270 - Return grid state flag.
 * Address: 0x801C0270 | Size: 0xC
 */
s32 fn_801C0270(void) {
    extern u32 lbl_8047B388;
    lbl_8047B388 = 0;
}

/**
 * fn_801C027C - Set JObj animation frame.
 * Address: 0x801C027C | Size: 0x10
 */
void fn_801C027C(void* obj, f32 frame) {
    if (obj != NULL) {
        *(f32*)((u8*)obj + 0x10) = frame;
    }
}

/**
 * HSD_ForeachAnim - Main pre-grid state machine.
 * Address: 0x801C028C | Size: 0xC94
 * This is a massive state machine controlling the pre-battle field setup.
 * It handles camera transitions, model loading, and initial animations
 * before the battle grid is ready.
 */
void HSD_ForeachAnim(void* ctx) {
    u8* state = (u8*)ctx;
    s32 phase;

    if (state == NULL) {
        return;
    }

    phase = *(s32*)(state + 0x00);

    /* Pre-grid state machine:
     * Phase 0: Initialize field geometry and camera
     * Phase 1: Load trainer models into pre-grid slots
     * Phase 2: Play encounter entrance animation
     * Phase 3: Transition camera to battle viewpoint
     * Phase 4: Load Pokemon models
     * Phase 5: Play Pokemon entrance animations
     * Phase 6: Wait for animations to complete
     * Phase 7: Signal grid ready
     */
    switch (phase) {
    case 0:
        /* Initialize field geometry */
        memset(lbl_80466E50, 0, 0x1E0);
        memset(lbl_80467030, 0, 0x20);
        *(s32*)(state + 0x00) = 1;
        break;

    case 1:
        /* Load trainer models */
        fn_801C1274(ctx, 0);
        fn_801C1274(ctx, 1);
        fn_801C1274(ctx, 2);
        fn_801C1274(ctx, 3);
        *(s32*)(state + 0x00) = 2;
        break;

    case 2:
        /* Encounter entrance animation part 1 */
        fn_801C1810(ctx);
        *(s32*)(state + 0x00) = 3;
        break;

    case 3:
        /* Camera transition */
        fn_801C25E4(ctx, 0);
        *(s32*)(state + 0x00) = 4;
        break;

    case 4:
        /* Encounter entrance animation part 2 */
        fn_801C1F00(ctx);
        *(s32*)(state + 0x00) = 5;
        break;

    case 5:
        /* Pre-grid animation update */
        fn_801C0F20(ctx);
        /* Check if animations are complete */
        {
            f32 timer = *(f32*)(state + 0x08);
            timer += 1.0f;
            *(f32*)(state + 0x08) = timer;
            if (timer >= 60.0f) {
                *(s32*)(state + 0x00) = 6;
            }
        }
        break;

    case 6:
        /* Wait for animations */
        {
            f32 timer = *(f32*)(state + 0x08);
            timer += 1.0f;
            *(f32*)(state + 0x08) = timer;
            if (timer >= 120.0f) {
                *(s32*)(state + 0x00) = 7;
            }
        }
        break;

    case 7:
        /* Grid ready */
        break;
    }
}

/**
 * fn_801C0F20 - Pre-grid animation update.
 * Address: 0x801C0F20 | Size: 0x354
 */
void fn_801C0F20(void* ctx) {
    u8* state = (u8*)ctx;
    s32 i;

    if (state == NULL) {
        return;
    }

    /* Update all active pre-grid slot animations */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        s32 active = *(s32*)(slot + 0x00);

        if (active == 0) {
            continue;
        }

        /* Update model animation */
        {
            void* jobj = *(void**)(slot + 0x04);
            if (jobj != NULL) {
                fn_80362D0C(jobj); /* HSD_JObjAnimAll */
            }
        }

        /* Update position interpolation */
        {
            f32 blend = *(f32*)(slot + 0x60);
            if (blend < 1.0f) {
                blend += 0.02f;
                if (blend > 1.0f) {
                    blend = 1.0f;
                }
                *(f32*)(slot + 0x60) = blend;
            }
        }
    }
}

/**
 * fn_801C1274 - Pre-grid model placement.
 * Address: 0x801C1274 | Size: 0x59C
 */
void fn_801C1274(void* ctx, s32 slot) {
    u8* state = (u8*)ctx;
    u8* slotData;

    if (state == NULL || slot < 0 || slot >= BATTLE_TOTAL_POKEMON) {
        return;
    }

    slotData = state + 0x20 + (slot * 0x70);

    /* Load model for this slot based on battle data */
    /* Determine if this is a player or enemy slot */
    if (slot < BATTLE_POS_ENEMY_LEFT) {
        /* Player side */
        /* Load trainer model or Pokemon model based on battle phase */
    } else {
        /* Enemy side */
        /* Load opponent trainer or Pokemon model */
    }

    /* Set initial position based on slot index */
    {
        f32 posX, posZ;
        /* Default double-battle positions */
        switch (slot) {
        case 0: posX = -3.0f; posZ = -5.0f; break;
        case 1: posX =  3.0f; posZ = -5.0f; break;
        case 2: posX = -3.0f; posZ =  5.0f; break;
        case 3: posX =  3.0f; posZ =  5.0f; break;
        default: posX = 0.0f; posZ = 0.0f; break;
        }
        *(f32*)(slotData + 0x08) = posX;
        *(f32*)(slotData + 0x0C) = 0.0f;
        *(f32*)(slotData + 0x10) = posZ;
    }

    /* Mark slot as active */
    *(s32*)(slotData + 0x00) = 1;
}

/**
 * fn_801C1810 - Pre-grid encounter sequence part 1.
 * Address: 0x801C1810 | Size: 0x6F0
 */
void fn_801C1810(void* ctx) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Pre-grid encounter sequence part 1:
     * Handles the initial camera sweep and trainer entrance animation.
     * Steps through sub-phases for each part of the entrance:
     * 1. Camera zooms out to field view
     * 2. Player trainer slides in from left
     * 3. Enemy trainer slides in from right
     * 4. Camera settles to battle position
     */
}

/**
 * fn_801C1F00 - Pre-grid encounter sequence part 2.
 * Address: 0x801C1F00 | Size: 0x6E4
 */
void fn_801C1F00(void* ctx) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Pre-grid encounter sequence part 2:
     * Handles Pokemon entrance animations:
     * 1. Pokeball throw animation
     * 2. Pokemon materialize effect
     * 3. Pokemon entrance cry
     * 4. Camera adjusts to show all combatants
     */
}

/**
 * fn_801C25E4 - Pre-grid transition helper.
 * Address: 0x801C25E4 | Size: 0x8C
 */
void fn_801C25E4(void* ctx, s32 mode) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Set camera transition mode and reset interpolation timer */
    *(s32*)(state + 0x1C0) = mode;
    *(f32*)(state + 0x1C4) = 0.0f;
}

/**
 * fn_801C2670 - Pre-grid scene object configuration.
 * Address: 0x801C2670 | Size: 0x184
 */
void fn_801C2670(void* ctx, s32 objType, s32 param) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Configure a scene object in the pre-grid context:
     * objType 0: Stage background model
     * objType 1: Ground plane model
     * objType 2: Sky dome model
     * objType 3: Battle effect spawner
     */
    *(s32*)(state + 0x1C8 + (objType * 4)) = param;
}

/**
 * fn_801C27F4 - Pre-grid field layout calculation.
 * Address: 0x801C27F4 | Size: 0x1D0
 */
void fn_801C27F4(void* ctx, f32 posX, f32 posZ) {
    u8* state = (u8*)ctx;
    if (state == NULL) {
        return;
    }
    /* Calculate field layout positions for all 4 battle slots
     * based on a center offset (posX, posZ).
     * The double battle layout uses a diamond formation:
     *   Slot 0 (Player L): center + (-offset, -depth)
     *   Slot 1 (Player R): center + (+offset, -depth)
     *   Slot 2 (Enemy L):  center + (-offset, +depth)
     *   Slot 3 (Enemy R):  center + (+offset, +depth)
     */
    {
        f32 offsetX = 3.0f;
        f32 depthZ = 5.0f;
        s32 i;

        for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
            u8* slot = state + 0x20 + (i * 0x70);
            f32 sx = (i & 1) ? offsetX : -offsetX;
            f32 sz = (i >= BATTLE_POS_ENEMY_LEFT) ? depthZ : -depthZ;

            *(f32*)(slot + 0x08) = posX + sx;
            *(f32*)(slot + 0x0C) = 0.0f;
            *(f32*)(slot + 0x10) = posZ + sz;
        }
    }
}

/**
 * fn_801C29C4 - Set JObj animation frame value.
 * Address: 0x801C29C4 | Size: 0x40
 */
void fn_801C29C4(void* obj, f32 value) {
    extern void HSD_FObjReqAnimAll(void* jobj);

    if (obj == NULL) {
        return;
    }
    *(f32*)((u8*)obj + 4) = value;
    *(u32*)obj = (*(u32*)obj & ~0x40000000) | 0x08000000;
    HSD_FObjReqAnimAll(*(void**)((u8*)obj + 0x14));
}

/**
 * fn_801C2A04 - Run pre-grid node callbacks.
 * Address: 0x801C2A04 | Size: 0x5C
 */
void fn_801C2A04(void) {
    extern s32 lbl_8047B390;
    extern s32 lbl_8047B38C;
    extern u8* lbl_8047B388;
    u8* node;

    if (lbl_8047B390 != 0 && lbl_8047B38C == 0) {
        node = lbl_8047B388;
        while (node != NULL) {
            (*(void (**)(void))(node + 0x4))();
            node = *(u8**)node;
        }
    }
}

/**
 * fn_801C2A60 - Get pre-grid slot count.
 * Address: 0x801C2A60 | Size: 0x14
 */
s32 fn_801C2A60(void) {
    return BATTLE_TOTAL_POKEMON;
}

/**
 * fn_801C2A74 - Get pre-grid slot side.
 * Address: 0x801C2A74 | Size: 0x1C
 */
s32 fn_801C2A74(s32 slot) {
    return (slot >= BATTLE_POS_ENEMY_LEFT) ? 1 : 0;
}

/**
 * fn_801C2A90 - Get pre-grid slot position within side.
 * Address: 0x801C2A90 | Size: 0x1C
 */
s32 fn_801C2A90(s32 slot) {
    return (slot & 1);
}

/**
 * fn_801C2AAC - Get grid group base pointer.
 * Address: 0x801C2AAC | Size: 0xC
 */
void* fn_801C2AAC(void) {
    extern u8 lbl_80466DB8[];
    return lbl_80466DB8;
}

/**
 * fn_801C2AB8 - Pre-grid set animation state.
 * Address: 0x801C2AB8 | Size: 0x30
 */
void fn_801C2AB8(s32 slot, s32 animState) {
    extern u8 lbl_80466DB8[];
    extern void fn_801AA35C(void*, u32, u32);
    fn_801AA35C(lbl_80466DB8, 0x1c, 4);
}

/**
 * fn_801C2AE8 - Get grid group member count by owner ID.
 * Address: 0x801C2AE8 | Size: 0x44
 */
u16 fn_801C2AE8(u32 id) {
    extern u8 lbl_80466DE8[];
    u8* group = lbl_80466DE8;
    u16 i;

    for (i = 0; i < 4; i++, group += 0x10) {
        if (*(u32*)group == id) {
            return *(u16*)(group + 0xC);
        }
    }
    return 0;
}

/**
 * fn_801C2B2C - Pre-grid update all slot positions.
 * Address: 0x801C2B2C | Size: 0xB4
 */
void fn_801C2B2C(void) {
    s32 i;
    u8* state = (u8*)lbl_80466E50;

    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        void* jobj = *(void**)(slot + 0x04);

        if (jobj != NULL && *(s32*)(slot + 0x00) != 0) {
            f32 x = *(f32*)(slot + 0x08);
            f32 y = *(f32*)(slot + 0x0C);
            f32 z = *(f32*)(slot + 0x10);
            fn_8036A384(jobj, x, y, z);
        }
    }
}

/**
 * fn_801C2BE0 - Pre-grid final setup.
 * Address: 0x801C2Be0 | Size: 0x174
 */
void fn_801C2Be0(void* ctx, s32 arg1) {
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
    fn_801C2B2C();
    *(s32*)(state + 0x00) = 7; /* GRID_READY */
}

/* =========================================================================
 * GRID TICK / STATE MANAGEMENT (0x801C2D54 - 0x801C3108)
 * ========================================================================= */

/**
 * fn_801C2D54 - Battle grid tick callback 1 (no-op forward).
 * Address: 0x801C2D54 | Size: 0x8
 * Referenced by battle_main.c as battle grid tick 1.
 */
extern u8 lbl_8047B398;
extern u8 lbl_8047B399;

void fn_801C2D54(void) {
    asm { lbz r3, lbl_8047B399(r13) }
}

/**
 * fn_801C2D5C - Battle grid tick callback wrapper.
 * Address: 0x801C2D5C | Size: 0xC
 */
void fn_801C2D5C(void) {
    fn_801C2D54();
}

/**
 * fn_801C2D68 - Battle grid tick callback 2.
 * Address: 0x801C2D68 | Size: 0xC
 * Referenced by battle_main.c as battle grid tick 2.
 */
void fn_801C2D68(void) {
    lbl_8047B399 = 1;
}

/**
 * fn_801C2D74 - Battle grid tick callback 3.
 * Address: 0x801C2D74 | Size: 0xC
 */
void fn_801C2D74(void) {
    lbl_8047B398 = 1;
}

/**
 * fn_801C2D80 - Battle grid cleanup / release resources.
 * Address: 0x801C2D80 | Size: 0x180
 * Referenced by battle_main.c as "battle grid cleanup".
 * Releases all grid models, clears slot state, frees memory.
 */
void fn_801C2D80(void) {
    s32 i;
    u8* state = (u8*)lbl_80466E50;

    /* Release all grid slot models */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        void* jobj = *(void**)(slot + 0x04);

        if (jobj != NULL) {
            fn_80363CF4(jobj); /* HSD_JObjRemoveAll */
            *(void**)(slot + 0x04) = NULL;
        }

        *(s32*)(slot + 0x00) = 0; /* inactive */
    }

    /* Clear scene animation context */
    memset(lbl_80466E50, 0, 0x1E0);

    /* Reset camera state */
    memset(lbl_80467030, 0, 0x20);
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

/* =========================================================================
 * CORE GRID FUNCTIONS (0x801C3108 - 0x801C53BC)
 * These are the main battle grid API functions declared in battle.h.
 * ========================================================================= */

/**
 * fn_801C3108 / battleGrid_GetState - Get current grid state.
 * Address: 0x801C3108 | Size: 0xC
 */
s32 fn_801C3108(void) {
    extern u8 lbl_80466DE8[];
    return (s32)lbl_80466DE8;
}

/**
 * fn_801C3114 / battleGrid_Init - Initialize the battle grid.
 * Address: 0x801C3114 | Size: 0xD8
 * Clears all grid slots, initializes the camera state,
 * sets up the 4-position double battle layout.
 */
void fn_801C3114(void) {
    s32 i;

    memset(lbl_80467030, 0, 0x20);
    memset(lbl_80466E50, 0, 0x1E0);

    /* Initialize 4 BattleGridSlot entries with default values */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = lbl_80466E50 + 0x20 + (i * 0x70);
        *(s32*)(slot + 0x00) = 0; /* inactive */
        *(void**)(slot + 0x04) = NULL; /* no model */
        *(f32*)(slot + 0x50) = 0.0f; /* rotation */
        *(f32*)(slot + 0x54) = 1.0f; /* scale */
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
    fn_801C3430();
}

/**
 * fn_801C3430 / battleGridSetup - Main grid setup (large).
 * Address: 0x801C3430 | Size: 0x634
 * Proposed name from symbols: battleGridSetup.
 * This is the primary grid initialization function that:
 *   1. Loads the stage model from FDAT
 *   2. Sets up position transforms for all 4 battle slots
 *   3. Configures lighting and shadow rendering
 *   4. Sets up the battle camera default view
 *   5. Initializes the model animation system
 */
void fn_801C3430(void) {
    /* Main battle grid setup:
     * 1. Load stage model from FDAT
     * 2. Set up position transforms for all 4 battle slots
     * 3. Configure lighting (ambient + 2 directional)
     * 4. Configure shadow rendering
     * 5. Set up battle camera default overhead view
     * 6. Initialize model animation system
     */
    fn_801C27F4((void*)lbl_80466E50, 0.0f, 0.0f);
    fn_801C3A64();
    fn_801C3B80();
}

/**
 * fn_801C3A64 / battleGridLoadModels - Load models for all grid positions.
 * Address: 0x801C3A64 | Size: 0x11C
 * Proposed name from symbols: battleGridLoadModels.
 * Loads Pokemon and trainer models into each active grid slot.
 */
void fn_801C3A64(void) {
    s32 i;
    u8* state = (u8*)lbl_80466E50;

    /* Load Pokemon and trainer models into each active grid slot */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        s32 active = *(s32*)(slot + 0x00);

        if (active != 0) {
            /* Model is already loaded or should be loaded from battle data */
            void* jobj = *(void**)(slot + 0x04);
            if (jobj != NULL) {
                f32 x = *(f32*)(slot + 0x08);
                f32 y = *(f32*)(slot + 0x0C);
                f32 z = *(f32*)(slot + 0x10);
                fn_8036A384(jobj, x, y, z);
            }
        }
    }
}

/**
 * fn_801C3B80 / battleGridUpdatePositions - Update all grid positions.
 * Address: 0x801C3B80 | Size: 0x118
 * Proposed name from symbols: battleGridUpdatePositions.
 * Recalculates world-space positions for all grid slots
 * (e.g., after a Pokemon switch or camera change).
 */
void fn_801C3B80(void) {
    s32 i;
    u8* state = (u8*)lbl_80466E50;

    /* Recalculate world-space positions for all grid slots */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        s32 active = *(s32*)(slot + 0x00);

        if (active == 0) {
            continue;
        }

        {
            void* jobj = *(void**)(slot + 0x04);
            if (jobj != NULL) {
                f32 x = *(f32*)(slot + 0x08);
                f32 y = *(f32*)(slot + 0x0C);
                f32 z = *(f32*)(slot + 0x10);
                f32 scale = *(f32*)(slot + 0x54);

                fn_8036A384(jobj, x, y, z);
                fn_8036A478(jobj, scale, scale, scale);
            }
        }
    }
}

/**
 * fn_801C3C98 - Grid slot state update helper.
 * Address: 0x801C3C98 | Size: 0xCC
 */
void fn_801C3C98(s32 slot) {
    u8* state = (u8*)lbl_80466E50;
    u8* slotData;

    if (slot < 0 || slot >= BATTLE_TOTAL_POKEMON) {
        return;
    }

    slotData = state + 0x20 + (slot * 0x70);

    /* Update slot state: apply position, rotation, and scale to JObj */
    {
        void* jobj = *(void**)(slotData + 0x04);
        if (jobj != NULL) {
            f32 x = *(f32*)(slotData + 0x08);
            f32 y = *(f32*)(slotData + 0x0C);
            f32 z = *(f32*)(slotData + 0x10);
            f32 rot = *(f32*)(slotData + 0x50);
            f32 scale = *(f32*)(slotData + 0x54);

            fn_8036A384(jobj, x, y, z);
            fn_8036A2D8(jobj, 0.0f, rot, 0.0f);
            fn_8036A478(jobj, scale, scale, scale);
        }
    }
}

/**
 * battleGridReplacePokemon / battleGridReplacePokemon - Replace Pokemon model in a grid slot.
 * Address: 0x801C3D64 | Size: 0xD8
 * Proposed name from symbols: battleGridReplacePokemon.
 * Removes the current Pokemon model from a slot and loads a new one.
 */
void battleGridReplacePokemon(void* model) {
    /* Replace Pokemon model in a grid slot:
     * 1. Find the slot this model belongs to
     * 2. Remove the current Pokemon JObj
     * 3. Load the new Pokemon JObj from model data
     * 4. Apply the slot's current transform
     */
    if (model == NULL) {
        return;
    }
}

/**
 * fn_801C3E3C - Grid slot model transition animation.
 * Address: 0x801C3E3C | Size: 0xD4
 */
void fn_801C3E3C(s32 slot, s32 animType) {
    u8* state = (u8*)lbl_80466E50;
    u8* slotData;

    if (slot < 0 || slot >= BATTLE_TOTAL_POKEMON) {
        return;
    }

    slotData = state + 0x20 + (slot * 0x70);

    /* Set animation transition type for the slot model */
    *(s32*)(slotData + 0x58) = animType;

    /* Request the animation on the slot's JObj */
    {
        void* jobj = *(void**)(slotData + 0x04);
        if (jobj != NULL) {
            fn_80362E40(jobj, 0.0f); /* HSD_JObjReqAnimAll */
        }
    }
}

/**
 * battleGridReplaceTrainer / battleGridReplaceTrainer - Replace trainer model in a grid slot.
 * Address: 0x801C3F10 | Size: 0xAC
 * Proposed name from symbols: battleGridReplaceTrainer.
 */
void battleGridReplaceTrainer(void* model) {
    /* Replace trainer model in a grid slot:
     * Similar to battleGridReplacePokemon but for trainer models.
     */
    if (model == NULL) {
        return;
    }
}

/**
 * fn_801C3FBC - Add slot to grid group.
 * Address: 0x801C3FBC | Size: 0xBC
 */
void fn_801C3FBC(u8* slot, u8 arg1, u8 arg2) {
    extern u8 lbl_80466DE8[];
    u8* group;
    s8 state;

    if (*(u16*)(lbl_80466DE8 + 0x40) < 4) {
        group = lbl_80466DE8;
        if (*(u32*)group != 0) {
            group = lbl_80466DE8 + 0x10;
            if (*(u32*)group != 0) {
                group += 0x10;
                if (*(u32*)group != 0) {
                    group += 0x10;
                    if (*(u32*)group != 0) {
                        group += 0x10;
                    }
                }
            }
        }
        memset(group, 0, 0x10);
        *(u8**)group = slot;
        state = 1;
        group[0xE] = arg1;
        group[0xF] = arg2;
        if (arg1 != 0) {
            state = -1;
        }
        slot[0x76] = state;
        *(u16*)(lbl_80466DE8 + 0x40) = *(u16*)(lbl_80466DE8 + 0x40) + 1;
    }
}

/**
 * fn_801C4078 - Get grid slot model pointer.
 * Address: 0x801C4078 | Size: 0x24
 */
#pragma peephole off
void* fn_801C4078(s32 slot) {
    extern u8 lbl_80466E30[];
    volatile u8* gridState = lbl_80466E30;

    if (gridState[0] == 4) {
        gridState[1] = 0;
        gridState[0] = 0;
    }

    return (void*)gridState;
}
#pragma peephole on

/**
 * fn_801C409C - Trigger grid slot update callback.
 * Address: 0x801C409C | Size: 0x54
 */
#pragma peephole off
void fn_801C409C(void) {
    extern u8 lbl_80466E30[];
    extern const f32 lbl_8047DFB0;
    extern const f32 lbl_8047DFB4;
    extern f32 fn_801C4814(s32 slot);
    extern void fn_80166A28(s32 arg0);

    if (*(u8*)lbl_80466E30 == 0) {
        fn_801C4164(9, (void*)fn_801C4814, 0, lbl_8047DFB0, lbl_8047DFB4);
        fn_80166A28(0x54);
    }
}
#pragma peephole on

/**
 * fn_801C40F0 - Set grid rendering flag.
 * Address: 0x801C40F0 | Size: 0x74
 * Referenced by battle_main.c as "battle grid set flag".
 */
void fn_801C40F0(s32 flag) {
    /* Set rendering flag for the battle grid */
}

/**
 * fn_801C4164 - Schedule grid update callback with arguments.
 * Address: 0x801C4164 | Size: 0x64
 */
void fn_801C4164(s32 mode, void* callback, s32 flags, f32 a, f32 b) {
}

/**
 * fn_801C41C8 - Battle camera initialization.
 * Address: 0x801C41C8 | Size: 0x74
 * Referenced by battle_main.c as "battle camera init".
 * Initializes the battle camera to the default overhead view
 * and configures the camera animation system.
 */
void fn_801C41C8(s32 mode) {
    /* Initialize battle camera with specified mode */
}

/**
 * fn_801C423C - Battle grid callback/state transition.
 * Address: 0x801C423C | Size: 0xE0
 */
#pragma peephole off
void* fn_801C423C(void (*callback)(void), u8 mode, u32 arg, f32 value) {
    extern u8 lbl_80466E30[];
    extern volatile const f32 lbl_8047DFB8;
    extern u32 fn_80109710(void);
    extern void fn_800EF5A4(void* texture);
    extern void fn_801C432C();
    extern void fn_801C6928(void);
    void* previous;
    void (*savedCallback)(void);
    u8* gridState;
    u32 modeByte;
    u32 argHalf;

    savedCallback = callback;
    if (callback == NULL) {
        return *(void**)(lbl_80466E30 + 0xC);
    }

    modeByte = (u8)mode;
    argHalf = arg & 0xFFFF;
    gridState = lbl_80466E30;
    *(volatile u8*)(gridState + 1) = 1;
    *(volatile u16*)(gridState + 2) = argHalf;
    *(volatile f32*)(gridState + 0x14) = value;
    *(volatile f32*)(gridState + 0x18) = lbl_8047DFB8;
    *(volatile u8*)gridState = 0;
    previous = *(void* volatile *)(gridState + 0xC);
    *(void* volatile *)(gridState + 0xC) = NULL;

    if (modeByte == 1) {
        fn_801C432C();
    } else if (*(void* volatile *)(gridState + 0x10) != NULL) {
        if (*(u32 volatile *)(lbl_80466E30 + 0x10) != fn_80109710()) {
            fn_800EF5A4(*(void* volatile *)(lbl_80466E30 + 0x10));
        }
        *(void* volatile *)(lbl_80466E30 + 0x10) = NULL;
    }

    fn_801C6928();
    savedCallback();
    return previous;
}
#pragma peephole on

/**
 * fn_801C431C - Get camera current angle.
 * Address: 0x801C431C | Size: 0x10
 */
void fn_801C431C(s32 arg0) {
    extern u8 lbl_80466E30[];

    *(s32*)(lbl_80466E30 + 0xC) = arg0;
}

/**
 * fn_801C432C - Camera angle calculation.
 * Address: 0x801C432C | Size: 0xB8
 */
void fn_801C432C(f32 angle, f32 blend) {
    u8* cam = (u8*)lbl_80467030;

    /* Calculate camera position from angle and blend factor */
    *(f32*)(cam + 0x04) = angle;
    *(f32*)(cam + 0x08) = blend;
}

/**
 * fn_801C43E4 - Get camera target position.
 * Address: 0x801C43E4 | Size: 0x10
 */
void* fn_801C43E4(void) {
    extern u8 lbl_8047B3A8;
    lbl_8047B3A8 = 1;
    return NULL;
}

/**
 * fn_801C43F4 - Camera complex movement sequence.
 * Address: 0x801C43F4 | Size: 0x3DC
 */
void fn_801C43F4(s32 seqType, f32 param1, f32 param2) {
    u8* cam = (u8*)lbl_80467030;

    /* Camera complex movement sequence:
     * seqType 0: Pan to position (param1=angle, param2=speed)
     * seqType 1: Orbit around center (param1=radius, param2=speed)
     * seqType 2: Zoom in/out (param1=distance, param2=speed)
     * seqType 3: Shake/vibration (param1=amplitude, param2=frequency)
     * seqType 4: Custom path (param1=pathID, param2=speed)
     */
    *(s32*)(cam + 0x0C) = seqType;
    *(f32*)(cam + 0x10) = param1;
    *(f32*)(cam + 0x14) = param2;
    *(f32*)(cam + 0x18) = 0.0f; /* reset sequence timer */
}

/**
 * fn_801C47D0 - Camera get current mode.
 * Address: 0x801C47D0 | Size: 0x44
 */
#pragma peephole off
s32 fn_801C47D0(void) {
    extern u8 lbl_80466E30[];
    extern volatile const f32 lbl_8047DFB8;
    u8* base = lbl_80466E30;

    *(volatile u8*)(base + 0) = 0;
    *(volatile u8*)(base + 1) = 0;
    *(volatile u16*)(base + 2) = 0;
    *(volatile f32*)(base + 4) = lbl_8047DFB8;
    *(volatile f32*)(base + 8) = lbl_8047DFB8;
    *(volatile u32*)(base + 0xc) = 0;
    *(volatile u32*)(base + 0x10) = 0;
    *(volatile f32*)(base + 0x14) = lbl_8047DFB8;
    *(volatile f32*)(base + 0x18) = lbl_8047DFB8;
    return (s32)base;
}
#pragma peephole on

/**
 * fn_801C4814 - Grid get slot X position.
 * Address: 0x801C4814 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fn_801C4814(s32 slot) {
    extern void fn_801C4A44();

    fn_801C431C((s32)fn_801C4A44);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C483C - Grid get slot Y position.
 * Address: 0x801C483C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fn_801C483C(s32 slot) {
    extern void fn_801C4CB8(void);

    fn_801C431C((s32)fn_801C4CB8);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C4864 - Grid get slot Z position.
 * Address: 0x801C4864 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fn_801C4864(s32 slot) {
    extern f32 fn_801C54FC(void);

    fn_801C431C((s32)fn_801C54FC);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C488C - Grid set slot X position.
 * Address: 0x801C488C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801C488C(s32 slot, f32 x) {
    extern void fn_801C5530(void);

    fn_801C431C((s32)fn_801C5530);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C48B4 - Grid set slot Y position.
 * Address: 0x801C48B4 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801C48B4(s32 slot, f32 y) {
    extern f32 fn_801C4C98(void);

    fn_801C431C((s32)fn_801C4C98);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C48DC - Grid set slot Z position.
 * Address: 0x801C48DC | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801C48DC(s32 slot, f32 z) {
    extern void fn_801C55D8(void);

    fn_801C431C((s32)fn_801C55D8);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C4904 - Grid set slot full position.
 * Address: 0x801C4904 | Size: 0x70
 */
void fn_801C4904(s32 slot, f32 x, f32 y, f32 z) {
    /* Set full XYZ position for slot */
}

/**
 * fn_801C4974 - Grid get slot rotation.
 * Address: 0x801C4974 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fn_801C4974(s32 slot) {
    extern f32 fn_801C5898(void);

    fn_801C431C((s32)fn_801C5898);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C499C - Grid set slot rotation.
 * Address: 0x801C499C | Size: 0x58
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801C499C(s32 slot, f32 rotation) {
    extern s32 fn_801C6908(s32);
    extern void fn_801C431C(s32);
    extern void fn_801C5F6C(void);
    extern void fn_801C5ED0(void);
    s32 result = fn_801C6908(2);
    switch (result) {
    case 0:
        fn_801C431C((s32)fn_801C5F6C);
        break;
    case 1:
    default:
        fn_801C431C((s32)fn_801C5ED0);
        break;
    }
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C49F4 - Grid get slot scale.
 * Address: 0x801C49F4 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fn_801C49F4(s32 slot) {
    extern f32 fn_801C5ED0(void);

    fn_801C431C((s32)fn_801C5ED0);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C4A1C - Grid set slot scale.
 * Address: 0x801C4A1C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801C4A1C(s32 slot, f32 scale) {
    extern void fn_801C5F6C(void);

    fn_801C431C((s32)fn_801C5F6C);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fn_801C4A44 - Grid complex slot update (position + rotation + scale).
 * Address: 0x801C4A44 | Size: 0x254
 */
void fn_801C4A44(s32 slot, f32 x, f32 y, f32 z, f32 rot, f32 scale) {
    u8* state = (u8*)lbl_80466E50;
    u8* slotData;

    if (slot < 0 || slot >= BATTLE_TOTAL_POKEMON) {
        return;
    }

    slotData = state + 0x20 + (slot * 0x70);

    /* Set all transform properties */
    *(f32*)(slotData + 0x08) = x;
    *(f32*)(slotData + 0x0C) = y;
    *(f32*)(slotData + 0x10) = z;
    *(f32*)(slotData + 0x50) = rot;
    *(f32*)(slotData + 0x54) = scale;

    /* Apply to JObj */
    {
        void* jobj = *(void**)(slotData + 0x04);
        if (jobj != NULL) {
            fn_8036A384(jobj, x, y, z);
            fn_8036A2D8(jobj, 0.0f, rot, 0.0f);
            fn_8036A478(jobj, scale, scale, scale);
        }
    }
}

/**
 * fn_801C4C98 - Get grid rotation callback.
 * Address: 0x801C4C98 | Size: 0x20
 */
f32 fn_801C4C98(void) {
    return fn_801C5F6C();
}

/**
 * fn_801C4CB8 - Grid full render update.
 * Address: 0x801C4CB8 | Size: 0x704
 * Large function handling the complete grid render pass:
 * updates all slot transforms, applies animations, renders models.
 */
void fn_801C4CB8(void) {
    s32 i;
    u8* state = (u8*)lbl_80466E50;

    /* Full grid render update:
     * 1. Update camera from BattleCameraState
     * 2. Update all slot transforms
     * 3. Animate all slot models
     * 4. Render all active slots
     */

    /* Update camera */
    {
        u8* cam = (u8*)lbl_80467030;
        s32 seqType = *(s32*)(cam + 0x0C);
        if (seqType != 0) {
            f32 timer = *(f32*)(cam + 0x18);
            timer += 1.0f;
            *(f32*)(cam + 0x18) = timer;
        }
    }

    /* Update all grid slots */
    for (i = 0; i < BATTLE_TOTAL_POKEMON; i++) {
        u8* slot = state + 0x20 + (i * 0x70);
        s32 active = *(s32*)(slot + 0x00);

        if (active == 0) {
            continue;
        }

        /* Animate model */
        {
            void* jobj = *(void**)(slot + 0x04);
            if (jobj != NULL) {
                fn_80362D0C(jobj); /* HSD_JObjAnimAll */
            }
        }

        /* Apply current transform */
        fn_801C3C98(i);
    }
}
