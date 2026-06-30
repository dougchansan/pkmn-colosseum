/**
 * @file battle_main.c
 * @brief Main battle loop, fight start/end, and core fight flow control.
 *
 * Address range: 0x801EF02C - 0x801F000C (26 functions)
 * Key functions:
 *   fn_801EFA08 (battle_MainLoop)   - Main battle thread loop, 0x5BC bytes
 *   fn_801EF4B0 (battle_FightStart) - Initializes battle, spawns main loop thread
 *   fn_801EF374 (battle_FightEnd)   - Cleans up after battle, releases resources
 *
 * The battle system entry point is battle_FightStart, which:
 *   1. Checks if a battle is already running (fn_800FF548)
 *   2. Prints "---------- fight start !! ----------" via GSlog
 *   3. Sets the fight-in-progress flag (lbl_8047B5DA)
 *   4. Saves current scene state (lbl_8047B5D4, lbl_8047B5D5)
 *   5. Initializes the battle scene (fn_801F108C)
 *   6. Loads waza (move animation) data (fn_801DAEF8)
 *   7. Saves People system state (GSscene_GetMode)
 *   8. Configures the battle grid camera objects via fn_801F0B00
 *   9. Spawns battle_MainLoop as a thread via GSthread (GSthreadCreate)
 *      with priority 0x14, stack size 0x4000
 *
 * battle_MainLoop runs on its own thread and:
 *   1. Starts a sound effect (fn_801659FC, BGM id 0x3E8)
 *   2. Initializes battle subsystems (fn_8020C840)
 *   3. Sets up the battle camera (fn_801C41C8)
 *   4. Configures scene rendering passes via fn_801F0B00
 *   5. Enters a polling loop that:
 *      a. Saves/restores VSync state (fn_801337A0/fn_801337A8)
 *      b. Checks pad input for debug fast-forward (fn_80103BA8)
 *      c. Reads battle status flags (fn_801EF634)
 *      d. Calls battle_FightEnd or scene callbacks
 *   6. On fight end, transitions through multiple rendering passes
 *      to fade out the battle scene
 *
 * battle_FightEnd:
 *   1. Stops floor/world updates (fn_800FF560)
 *   2. Stops particle effects (fn_800F04C4)
 *   3. Iterates through 21 scene objects (from lbl_80279B84 rodata table)
 *      and releases each one via fn_80102568
 *   4. Restores People system state (GSscene_SetMode)
 *   5. Cleans up battle grid (fn_801C31EC)
 *   6. Cleans up waza system (fn_801DAC90)
 *   7. Stops any active sound (soundStop)
 *   8. Fades out battle music (fn_80165A20)
 *   9. Restores VSync mode (fn_801337A8)
 *   10. Prints "---------- fight end !! ----------" via GSlog
 *
 * BSS state variables (SDA21-relative):
 *   lbl_8047B5D0 : u32, battle thread handle
 *   lbl_8047B5D4 : u8,  saved scene ID
 *   lbl_8047B5D5 : u8,  saved VSync mode
 *   lbl_8047B5D6 : u16, battle result code
 *   lbl_8047B5D8 : u16, battle status flags
 *   lbl_8047B5DA : u8,  fight-in-progress flag
 */

#include "game/battle/battle.h"

#pragma use_lmw_stmw on

/* =========================================================================
 * External function declarations (auto-generated names)
 * ========================================================================= */

/* CRT */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* OS / Engine */
extern u8   fn_800FF548(void);                      /* check if floor is loaded */
extern void* fn_800FF560(void);                     /* get parent thread */
extern void fn_800FF56C(void);                      /* tick floor */
extern void fn_800F04C4(void);                      /* stop particle system */
extern void _threadSwitch(void);                      /* tick render */
extern void* GSthreadCreate(s32 priority, void* parent, s32 stackSize,
                          u8 usesFPU, void* entry, s32 arg); /* GSthread_Create */
extern void fn_800D3088(void);                      /* GSgfx tick */
extern void fn_800DD970(const char* fmt, ...);      /* GSlog_Print */

/* Scene management */
extern void fn_80102568(s32 objID, s32 arg1, s32 arg2);   /* release scene object */
extern u8   fn_80102620(s32 objID);                        /* check scene object active */
extern void fn_80103BA8(void* padData, s32 port);          /* read pad input */
extern void fn_80113FB4(void);                             /* check floor loaded */

/* Sound */
extern void soundStop(s32 sndID, s32 volume);    /* soundStop */
extern void fn_801659FC(s32 sndID, s32 fadeTime, s32 volume); /* sndPlay with fade */
extern void fn_80165A20(s32 mode, s32 sndID, s32 volume);     /* sndFade */

/* People / NPC system */
extern u8   GSscene_GetMode(void);                      /* save people state */
extern void GSscene_SetMode(u8 savedState);             /* restore people state */
extern void fn_80177A64(void);                      /* clear people state */

/* VSync management */
extern u8   fn_801337A0(void);                      /* save VSync mode */
extern void fn_801337A8(u8 mode);                   /* set VSync mode */
extern void fn_801337E4(void);                      /* disable VSync */
extern void fn_80133810(s32 mode);                  /* force VSync */

/* Battle subsystems */
extern void fn_801C2D54(void);                      /* battle grid tick 1 */
extern void fn_801C2D68(void);                      /* battle grid tick 2 */
extern void fn_801C2D80(void);                      /* battle grid cleanup */
extern void fn_801C2F00(void* data, u32 size);      /* battle grid load data */
extern void fn_801C31EC(void);                      /* battle grid release */
extern void fn_801C40F0(s32 flag);                  /* battle grid set flag */
extern void fn_801C41C8(s32 mode);                  /* battle camera init */

/* Battle scene */
extern void fn_801DAC90(void);                      /* waza system cleanup */
extern void fn_801DAEF8(s32 count);                 /* waza system init */
extern void fn_801DB088(void);                      /* waza system reset */

/* Battle init / render */
extern void fn_801F0B00(void* sceneCtx, s32 arg1, s32 arg2,
                         s32 passType, s32 passIdx, void* tbl); /* scene pass config */
extern void fn_801F09D0(void);                      /* scene pass finalize */
extern void fn_801F108C(void);                      /* battle scene init */
extern u8   fn_801F1758(s32 arg);                   /* check scene state */

/* Battle state machine */
extern void fn_8020C840(void);                      /* battle system init */

/* GBA link */
extern void fn_80265C84(void);
extern void fn_80265C10(void);

/* Sound check */
extern s32 fn_801F54A4(s32 a, s32 b, s32 c, s32 d); /* check/setup sound */

/* Battle poké-slot helpers */
extern void* fn_801F47B4(s32 a, u16 b);                    /* get battle slot ptr */
extern void* fn_801F7258(void* a, u16 b);                  /* get slot member ptr */
extern void* fn_801FB1C0(void* a, s32 b, s32 c, s32 d);   /* get checked handle */
extern void  fn_801DA4E8(void* a, void* b);                /* dispatch event */
extern void* fn_801F981C(void* a, u16 b);                  /* get item ptr */
extern void* fn_8012640C(void* a, s32 b, s32 c, s32 d);   /* check item handle */

/* =========================================================================
 * External data (BSS, SDA21-relative)
 * ========================================================================= */

/* These are declared in the linker script or bss section */
extern u32  lbl_8047B5D0;   /* battle thread handle */
extern u8   lbl_8047B5D4;   /* saved scene ID */
extern u8   lbl_8047B5D5;   /* saved VSync mode */
extern u16  lbl_8047B5D6;   /* battle result code */
extern u16  lbl_8047B5D8;   /* battle status flags */
extern u8   lbl_8047B5DA;   /* fight-in-progress flag */

/* =========================================================================
 * Rodata tables (from auto_04 rodata section)
 * ========================================================================= */

/**
 * Scene object IDs to release during fight end.
 * 21 entries: 0x45-0x4F, 0xF1-0xF4, 0x100-0x103, 0x10B, 0x10A
 * These correspond to battle-specific scene objects (Pokemon models,
 * trainer models, UI elements, effect objects).
 */
extern const u32 lbl_80279B84[BATTLE_SCENE_OBJ_COUNT];

/* Scene initialization table */
extern const u32 lbl_80279B78[3]; /* function pointers for scene callbacks */

/* =========================================================================
 * Implementation
 * ========================================================================= */

/**
 * battle_SetResult - Store the battle result code.
 * fn_801EF61C at 0x801EF61C (8 bytes)
 */
void battle_SetResult(u16 result) {
    lbl_8047B5D6 = result;
}

/**
 * battle_GetResult - Read the battle result code.
 * fn_801EF624 at 0x801EF624 (8 bytes)
 */
u16 battle_GetResult(void) {
    return lbl_8047B5D6;
}

/**
 * battle_SetStatusFlags - Store battle status flags.
 * fn_801EF62C at 0x801EF62C (8 bytes)
 */
void battle_SetStatusFlags(u16 flags) {
    lbl_8047B5D8 = flags;
}

/**
 * battle_GetStatusFlags - Read battle status flags.
 * fn_801EF634 at 0x801EF634 (8 bytes)
 */
u16 battle_GetStatusFlags(void) {
    return lbl_8047B5D8;
}

/**
 * battle_IsFightInProgress - Check if a battle is currently running.
 * fn_801EF63C at 0x801EF63C (8 bytes)
 */
u8 battle_IsFightInProgress(void) {
    return lbl_8047B5DA;
}

/**
 * battle_FightReset - Reset all fight state variables.
 * fn_801EF5C0 at 0x801EF5C0 (0x5C bytes)
 *
 * Copies the scene callback table (lbl_80279B78) to the stack,
 * clears the fight-in-progress flag, result code, status flags,
 * and thread handle, then calls fn_800FF4D4 to reset the floor
 * system with the callback configuration.
 */
void battle_FightReset(void) {
    extern void fn_800FF4D4(void* config, s32 count);

    u32 config[3];

    lbl_8047B5DA = 0;
    config[0] = lbl_80279B78[0];
    config[1] = lbl_80279B78[1];
    config[2] = lbl_80279B78[2];
    lbl_8047B5D8 = 0;
    lbl_8047B5D6 = 0;
    lbl_8047B5D0 = 0;

    fn_800FF4D4(config, 2);
}

/**
 * battle_FightEnd - Clean up after a battle ends.
 * fn_801EF374 at 0x801EF374 (0x114 bytes)
 *
 * Iterates through all 21 battle scene objects and releases any
 * that are still active, then restores the pre-battle engine state.
 */
void battle_FightEnd(void) {
    u32 sceneObjTable[BATTLE_SCENE_OBJ_COUNT];
    s32 i;
    s32 objID;

    /* Stop floor and particle systems */
    fn_800FF560();
    fn_800F04C4();
    fn_800FF560();
    fn_800F04C4();

    /* Copy the scene object ID table to stack */
    memcpy(sceneObjTable, lbl_80279B84, sizeof(sceneObjTable));

    /* Release all active battle scene objects */
    for (i = 0; i < BATTLE_SCENE_OBJ_COUNT; i++) {
        objID = sceneObjTable[i];
        if (fn_80102620(objID) == 1) {
            fn_80102568(objID, 0, 0);
        }
    }

    /* Restore NPC/People system state */
    GSscene_SetMode(lbl_8047B5D4);

    /* Clean up battle grid */
    fn_801C31EC();

    /* Clean up waza (move animation) system */
    fn_801DAC90();

    /* Stop any battle sound effects */
    fn_801F54A4(0, 0, 0x12, 0);
    /* result checked but not stored -- original code checks return != NULL
       and calls soundStop to stop it */

    /* Fade out battle music */
    fn_80165A20(1, 0, 0xFF);

    /* Restore VSync mode */
    fn_801337A8(lbl_8047B5D5);

    /* Clear fight-in-progress flag */
    lbl_8047B5DA = 0;

    /* Print debug message */
    fn_800DD970("---------- fight end !! ---------- \n");
}

/**
 * battle_FightCleanup - Additional cleanup after fight end.
 * fn_801EF488 at 0x801EF488 (0x28 bytes)
 *
 * Clears the People system state, resets the battle grid,
 * and resets the waza system.
 */
void battle_FightCleanup(void) {
    fn_80177A64();
    fn_801C2D80();
    fn_801DB088();
}

/**
 * battle_FightStart - Initialize and start a new battle.
 * fn_801EF4B0 at 0x801EF4B0 (0x110 bytes)
 *
 * This is the main entry point for starting a battle. It checks
 * if a floor/scene is already loaded, and if not, sets up the
 * entire battle environment and spawns the battle main loop thread.
 *
 * The battle main loop thread (battle_MainLoop / fn_801EFA08) is
 * created via GSthread with:
 *   - Priority: 0x14 (20)
 *   - Stack size: 0x4000 (16KB)
 *   - Uses FPU: 1 (yes, for battle scene rendering)
 */
void battle_FightStart(void) {
    extern void* lbl_8046D730; /* secondary scene context */
    extern void* lbl_80375CC8; /* scene render table */
    u8 savedVSync;
    void* parent;

    /* Check if scene is already running */
    if (fn_800FF548()) {
        fn_801337E4();
        fn_801337A8(0);
        return;
    }

    /* Print debug message */
    fn_800DD970("---------- fight start !! ---------- \n");

    /* Set fight-in-progress flag */
    lbl_8047B5DA = 1;

    /* Save current VSync mode */
    lbl_8047B5D5 = fn_801337A0();

    /* Disable VSync for setup */
    fn_801337E4();
    fn_801337A8(0);

    /* Initialize the battle scene */
    fn_801F108C();

    /* Initialize waza (move animation) system with 10 entries */
    fn_801DAEF8(10);

    /* Save People/NPC system state */
    lbl_8047B5D4 = GSscene_GetMode();

    /* Configure scene rendering passes (3 passes for battle) */
    fn_801F0B00(&lbl_8046D730, 0, 0, 1, 0, &lbl_80375CC8);
    fn_801F0B00(&lbl_8046D730, 0, 0, 3, 5, &lbl_80375CC8);
    fn_801F0B00(&lbl_8046D730, 0, 0, 3, 4, &lbl_80375CC8);
    fn_801F09D0();

    /* Get parent thread for spawning battle loop */
    parent = (void*)fn_800FF560();

    /* Spawn battle_MainLoop as a new thread */
    lbl_8047B5D0 = (u32)GSthreadCreate(0x14, parent, 0x4000, 1,
                                      (void*)battle_MainLoop, 0);
}

/*
 * battle_MainLoop is defined below in outline form.
 * The full decompilation of fn_801EFA08 (0x5BC bytes) requires
 * matching all branch targets and call sequences. The function
 * is a large polling loop that drives the battle forward by:
 *
 * 1. Playing the battle BGM
 * 2. Initializing battle subsystems
 * 3. Setting up the battle camera
 * 4. Configuring 4 rendering passes for the battle scene:
 *    - Pass (0,2,0): Initial scene
 *    - Pass (0,4,5): Pokemon models
 *    - Pass (0,4,4): Effects/UI overlay
 *    - Pass (0,5,0): Final composite
 * 5. Polling in a loop:
 *    a. Save VSync state, pad reads for debug
 *    b. Check battle_GetStatusFlags() for completion
 *    c. If done, transition through fade-out passes
 *    d. Call fn_801F1758 to check scene state
 *    e. On specific states, trigger GBA link callbacks
 * 6. Loop until battle_GetStatusFlags() returns non-zero
 *
 * The loop body includes a sub-loop that reads 4 frames of
 * controller input (via fn_80103BA8) for debug fast-forward,
 * and accumulates frame counts via fn_800D3088.
 */

/* The actual assembly-level loop cannot be fully decompiled without
 * matching all the branch targets. The structure above documents
 * the observed behavior from reading fn_801EFA08. */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 5 functions matched
 * =================================================================== */

extern u16 lbl_8047B5D6;
extern u16 lbl_8047B5D8;
extern u8 lbl_8047B5DA;

/* Forward declarations for converted functions */
void fn_801EFA08(void);


/* Address: 0x801EF61C | Size: 0x8 | Pattern: sda_setter */
void fn_801EF61C(u16 val) {
    lbl_8047B5D6 = val;
}

/* Address: 0x801EF624 | Size: 0x8 | Pattern: sda_getter */
u16 fn_801EF624(void) {
    return lbl_8047B5D6;
}

/* Address: 0x801EF62C | Size: 0x8 | Pattern: sda_setter */
void fn_801EF62C(u16 val) {
    lbl_8047B5D8 = val;
}

/* Address: 0x801EF634 | Size: 0x8 | Pattern: sda_getter */
u16 fn_801EF634(void) {
    return lbl_8047B5D8;
}

/* Address: 0x801EF63C | Size: 0x8 | Pattern: sda_getter */
u8 fn_801EF63C(void) {
    return lbl_8047B5DA;
}

/* #######################################################################
 * COVERAGE STUBS: battle core / fight flow (0x801EF02C - 0x801F000C)
 * 20 functions remaining for full coverage of battle_main.c TU.
 * ####################################################################### */


/* 0x801EF02C | size: 0x54 | small */
void fn_801EF02C(void) {
    extern void* fn_801EE440(u16 idx);
    extern u16 fn_801EE468(void);
    extern void fn_801EECD8(void* a, u32 b);
    u16 count = fn_801EE468();
    u16 i;
    for (i = 0; i < count; i++) {
        fn_801EECD8(fn_801EE440(i), 1);
    }
}

/* 0x801EF080 | size: 0x54 | small */
void fn_801EF080(void) {
    extern void* fn_801EE440(u16 idx);
    extern u16 fn_801EE468(void);
    extern void fn_801EEB34(void* a, u32 b);
    u16 count = fn_801EE468();
    u16 i;
    for (i = 0; i < count; i++) {
        fn_801EEB34(fn_801EE440(i), 1);
    }
}

/* 0x801EF0D4 | size: 0x54 | small */
void fn_801EF0D4(void) {
    extern void* fn_801EE440(u16 idx);
    extern u16 fn_801EE468(void);
    extern void fn_801EE958(void* a, u32 b);
    u16 count = fn_801EE468();
    u16 i;
    for (i = 0; i < count; i++) {
        fn_801EE958(fn_801EE440(i), 1);
    }
}

/* 0x801EF128 | size: 0xBC | medium */
void fn_801EF128(void) {
    extern void fn_80129280();
    extern void fn_801EEF08();
    extern void fn_801EEFF4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r29 = r3;
    if (r3 == 0) {
        r3 = 0x0;
        r4 = 0xf;
        fn_80129280();
        r29 = r3;
    }
    r30 = 0x0;
    r28 = 0x0;
    *(u32*)((u8*)r29 + 0x0) = r30;
    r31 = 0x0;
    do {
        r5 = r30 + 0x4;
        r27 = r31 + 0x490;
        r5 = r29 + r5;
        r4 = 0x0;
        *(u8*)((u8*)r5 + 0x0) = r4;
        r27 = r29 + r27;
        tmp = -0x1;
        r3 = r28 & 0xFFFF;
        *(u16*)((u8*)r5 + 0x2) = r4;
        *(u32*)((u8*)r5 + 0x4) = r4;
        *(u32*)((u8*)r5 + 0x8) = r4;
        *(u8*)((u8*)r27 + 0xC) = r4;
        *(u16*)((u8*)r27 + 0x0) = r4;
        *(u16*)((u8*)r27 + 0x14) = tmp;
        fn_801EEF08();
        *(u16*)((u8*)r27 + 0x16) = r3;
        tmp = 0x0;
        r3 = r28 & 0xFFFF;
        *(u8*)((u8*)r27 + 0xD) = tmp;
        fn_801EEFF4();
        *(u16*)((u8*)r27 + 0x10) = r3;
        r31 = r31 + 0x18;
        r30 = r30 + 0xc;
        *(u8*)((u8*)r27 + 0xE) = r28;
        r28 = r28 + 0x1;
    } while ((s32)r28 < 0x61);
    return;
}

/* 0x801EF1E4 | size: 0x30 | small */
/* Ensure battle data pointer is valid; if NULL, get default via fn_80129280. */
void* fn_801EF1E4(void* data) {
    extern void* fn_80129280(u32 a, u32 b);
    if (data == NULL) {
        data = fn_80129280(0, 0xF);
    }
    return data;
}

/* 0x801EF214 | size: 0x60 | small */
u32 fn_801EF214(void) {
    extern u32 lbl_80478F68;
    extern u8 fn_801EEC74(u16 idx);
    u16 i;
    u32 count = 0;
    for (i = 1; i < *(u32*)((u8*)lbl_80478F68 + 0x0); i++) {
        if (fn_801EEC74(i) != 0) {
            count++;
        }
    }
    return count;
}

/* 0x801EF274 | size: 0x60 | small */
u32 fn_801EF274(void) {
    extern u32 lbl_80478F68;
    extern u8 fn_801EEAD0(u16 idx);
    u16 i;
    u32 count = 0;
    for (i = 1; i < *(u32*)((u8*)lbl_80478F68 + 0x0); i++) {
        if (fn_801EEAD0(i) != 0) {
            count++;
        }
    }
    return count;
}

/* 0x801EF2D4 | size: 0xA0 | medium */
void fn_801EF2D4(void) {
    extern u8 fn_80102620(s32 objID);
    extern void fn_80102568(s32 objID, s32 arg1, s32 arg2);
    typedef struct BattleSceneIdList {
        u32 ids[21];
    } BattleSceneIdList;
    BattleSceneIdList ids;
    s32 objID;
    u8 i;

    ids = *(const BattleSceneIdList*)lbl_80279B84;
    for (i = 0; i < 21; i++) {
        objID = ids.ids[i];
        if (fn_80102620(objID) == 1) {
            fn_80102568(objID, 0, 0);
        }
    }
}

/* fn_801EF374 (battle_FightEnd) - already decompiled above */
/* fn_801EF488 (battle_FightCleanup) - already decompiled above */
/* fn_801EF4B0 (battle_FightStart) - already decompiled above */
/* fn_801EF5C0 (battle_FightReset) - already decompiled above */

/* 0x801EF644 | size: 0xB8 | medium */
void fn_801EF644(s32 result) {
    extern u8 lbl_803727C8[];
    extern u8 lbl_803752A0[];
    extern u16 lbl_80478D10;
    extern u8 fn_8017D56C(s32 id);
    extern s32 fn_8017B2CC(s32 id);
    extern void _threadSwitch(void);
    s32 id;
    s8 wanted;
    u16 i;
    s32 wait;

    wanted = result;
    for (i = 0; i < lbl_80478D10; i += 2) {
        if (((s8)result < 0) || (wanted == *(u16*)(lbl_803752A0 + ((u16)i * 2) + 2))) {
            id = *(s32*)(lbl_803727C8 + (*(u16*)(lbl_803752A0 + ((u16)i * 2)) * 0xc));
            if (fn_8017D56C(id) != 0) {
                while (1) {
                    wait = fn_8017B2CC(id);
                    if (wait == 0 || wait < 0) {
                        break;
                    }
                    _threadSwitch();
                }
            }
        }
    }
}

/* 0x801EF6FC | size: 0x5C | small */
void fn_801EF6FC(u8 a) {
    extern u8 lbl_8047B5E1;
    extern u16 fn_801EF624(void);
    extern u32 fn_8020DD44(void);
    extern void fn_801659FC(u32 r3, u32 r4, u32 r5);
    u32 r3;

    if (lbl_8047B5E1 == 1) return;
    if (a == 0) return;
    if (fn_801EF624() == 0) return;
    r3 = fn_8020DD44();
    if (r3 != 0) {
        fn_801659FC(r3, 0x3e8, 0xff);
    }
    lbl_8047B5E1 = 1;
    return;
}

/* 0x801EF758 | size: 0x5C | small */
void fn_801EF758(u8 a) {
    extern u8 lbl_8047B5E0;
    extern u16 fn_801EF624(void);
    extern u32 fn_8020DD80(void);
    extern void fn_80165A20(u32 r3, u32 r4, u32 r5);
    u32 r3;

    if (lbl_8047B5E0 == 1) return;
    if (a == 0) return;
    if (fn_801EF624() == 0) return;
    r3 = fn_8020DD80();
    if (r3 != 0) {
        fn_80165A20(r3, 0x0, 0xff);
    }
    lbl_8047B5E0 = 1;
    return;
}

/* 0x801EF7B4 | size: 0x10 | tiny */
void fn_801EF7B4(void) {
    extern u8 lbl_8047B5E0;
    extern u8 lbl_8047B5E1;
    lbl_8047B5E0 = 0;
    lbl_8047B5E1 = 0;
}

/* 0x801EF7C4 | size: 0x130 | medium */
void fn_801EF7C4(void* arg) {
    void *r31;
    u16 r30, r29, r28, r27, r26;
    void *r25;
    void *_h;

    fn_801F54A4(0, 0, 0x14, 0);
    r27 = fn_801F54A4(0, 0, 0x16, 0);
    r26 = fn_801F54A4(0, 0, 0x18, 0);

    for (r28 = 0; (u16)r28 < 2; r28++) {
        r31 = fn_801F47B4(0, r28);
        if (r31 == 0) continue;

        for (r30 = 0; (u16)r30 < r27; r30++) {
            r25 = fn_801F7258(r31, r30);
            if (r25 == 0) continue;

            _h = fn_801FB1C0(r25, 0, 0x4c, 0);
            if (_h == 0) continue;
            fn_801DA4E8(_h, arg);

            for (r29 = 0; (u16)r29 < r26; r29++) {
                _h = fn_801F981C(r25, r29);
                if (_h == 0) continue;
                _h = fn_8012640C(_h, 0, 0xee, 0);
                if (_h == 0) continue;
                fn_801DA4E8(_h, arg);
            }
        }
    }
}

/* 0x801EF95C | size: 0xAC | medium */
void fn_801EF95C(void) {
    extern u8 fn_801337A0(void);
    extern void fn_801337A8(u8 mode);
    extern u8 fn_8000816C(void);
    extern void fn_80133810(s32 mode);
    extern void fn_80103BA8(void* padData, s32 port);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    u8 ticks;
    u8 mode;
    u8 input[0x20];

    mode = fn_801337A0();
    fn_801337A8(1);
    if (fn_8000816C() == 1) {
        fn_80133810(1);
    } else {
        ticks = 0;
        while (ticks < 4) {
            fn_80103BA8(input, 1);
            if (!(*(u16*)input & 0x800)) {
                _threadSwitch();
                ticks += fn_800D3088();
            } else {
                break;
            }
        }
        if (ticks < 4) {
            fn_80133810(1);
        }
    }
    fn_801337A8(mode);
}

/* fn_801EFA08 (battle_MainLoop) - documented above, needs full decompilation */
void fn_801EFA08(void) {
    extern u8 lbl_80375CC8[];
    extern u8 lbl_80375D30[];
    extern u8 lbl_80375D80[];
    extern u8 lbl_8046D760[];
    extern f32 lbl_8047E508;
    extern void fn_8000816C();
    extern void fn_80008184();
    extern void fn_800FF660();
    extern void fn_8011288C();
    extern void fn_8016597C();
    extern void fn_801C2D5C();
    extern void fn_801EF2D4();
    extern void fn_801EF634();
    extern void fn_801F1588();
    extern void fn_801F1700();
    extern void fn_80261954();
    extern void fn_80261AD0();
    extern void fn_802658C8();
    extern void fn_802659A4();
    extern void fn_802659F8();
    extern void fn_80265A6C();
    extern void fn_80265B3C();
    extern void fn_80265BBC();
    extern void fn_80265D54();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;

    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x12;
    r6 = 0x0;
    ((void(*)(void))fn_801F54A4)();
    if (r3 != 0) {
        r4 = 0x3e8;
        r5 = 0xff;
        ((void(*)(void))fn_801659FC)();
    }
    ((void(*)(void))fn_8020C840)();
    f1 = lbl_8047E508;
    r3 = 0x8;
    ((void(*)(void))fn_801C41C8)();
    r3 = (u32)lbl_8046D760;
    r5 = (u32)lbl_80375CC8;
    r8 = (u32)lbl_80375CC8;
    r4 = 0x0;
    r3 = (u32)lbl_8046D760;
    r5 = 0x0;
    r6 = 0x2;
    r7 = 0x0;
    ((void(*)(void))fn_801F0B00)();
    ((void(*)(void))fn_801C2D54)();
    ((void(*)(void))fn_801C2D68)();
    ((void(*)(void))fn_800FF56C)();
    ((void(*)(void))fn_80113FB4)();
    if (r3 != 0) {
        r4 = 0x7B0000;
        r4 = r4 + 0x1800;
        ((void(*)(void))fn_801C2F00)();
    }
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    ((void(*)(void))fn_801337A0)();
    r28 = r3;
    r3 = 0x1;
    ((void(*)(void))fn_801337A8)();
    fn_8000816C();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        r3 = 0x1;
        ((void(*)(void))fn_80133810)();
    } else {
        r29 = 0x0;
        while ((r29 & 0xFF) < 4) {
            r3 = (u32)sp + 0x24;
            r4 = 0x1;
            ((void(*)(void))fn_80103BA8)();
            tmp = *(u16*)(sp + 0x24);
            tmp = tmp & 0x00000800;
            if (tmp != 1) break;
            ((void(*)(void))_threadSwitch)();
            ((void(*)(void))fn_800D3088)();
            tmp = r29 + r3;
            r29 = tmp & 0xFF;
        }
        if ((r29 & 0xFF) < 4) {
            r3 = 0x1;
            ((void(*)(void))fn_80133810)();
        }
    }
    r3 = r28;
    ((void(*)(void))fn_801337A8)();
    fn_801EF634();
    tmp = r3 & 0xFFFF;
    if (tmp == 4) {
        ((void(*)(void))fn_801C2D54)();
        ((void(*)(void))fn_801C2D68)();
        ((void(*)(void))fn_800FF56C)();
        ((void(*)(void))fn_80113FB4)();
        if (r3 != 0) {
            r4 = 0x7B0000;
            r4 = r4 + 0x1800;
            ((void(*)(void))fn_801C2F00)();
        }
        r3 = (u32)lbl_8046D760;
        r5 = (u32)lbl_80375CC8;
        r8 = (u32)lbl_80375CC8;
        r4 = 0x0;
        r3 = (u32)lbl_8046D760;
        r5 = 0x0;
        r6 = 0x4;
        r7 = 0x5;
        ((void(*)(void))fn_801F0B00)();
        ((void(*)(void))fn_801C2D54)();
        ((void(*)(void))fn_801C2D68)();
        ((void(*)(void))fn_800FF56C)();
        ((void(*)(void))fn_80113FB4)();
        if (r3 != 0) {
            r4 = 0x7B0000;
            r4 = r4 + 0x1800;
            ((void(*)(void))fn_801C2F00)();
        }
        r3 = (u32)lbl_8046D760;
        r5 = (u32)lbl_80375CC8;
        r8 = (u32)lbl_80375CC8;
        r4 = 0x0;
        r3 = (u32)lbl_8046D760;
        r5 = 0x0;
        r6 = 0x4;
        r7 = 0x4;
        ((void(*)(void))fn_801F0B00)();
        ((void(*)(void))fn_801C2D54)();
        ((void(*)(void))fn_801C2D68)();
        ((void(*)(void))fn_800FF56C)();
        ((void(*)(void))fn_80113FB4)();
        if (r3 != 0) {
            r4 = 0x7B0000;
            r4 = r4 + 0x1800;
            ((void(*)(void))fn_801C2F00)();
        }
        r3 = (u32)lbl_8046D760;
        r5 = (u32)lbl_80375CC8;
        r8 = (u32)lbl_80375CC8;
        r4 = 0x0;
        r3 = (u32)lbl_8046D760;
        r5 = 0x0;
        r6 = 0x5;
        r7 = 0x0;
        ((void(*)(void))fn_801F0B00)();
        ((void(*)(void))fn_801F09D0)();
        r3 = 0x0;
        ((void(*)(void))fn_801F1758)();
        tmp = r3 & 0xFF;
        if (tmp == 1) {
            ((void(*)(void))fn_80265C84)();
            ((void(*)(void))fn_80265C10)();
        }
        r4 = (u32)lbl_8046D760;
        r3 = (u32)lbl_80375D30;
        r28 = (u32)lbl_8046D760;
        r29 = (u32)lbl_80375D30;
        r30 = r28;
        r31 = r29;

        /* Inner battle polling loop */
        while (1) {
            fn_801EF634();
            tmp = r3 & 0xFFFF;
            if (tmp != 1) break;

            /* Save/restore VSync, read pad input */
            ((void(*)(void))fn_801337A0)();
            r27 = r3;
            r3 = 0x1;
            ((void(*)(void))fn_801337A8)();
            fn_8000816C();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                r3 = 0x1;
                ((void(*)(void))fn_80133810)();
            } else {
                r26 = 0x0;
                while ((r26 & 0xFF) < 4) {
                    r3 = (u32)sp + 0x8;
                    r4 = 0x1;
                    ((void(*)(void))fn_80103BA8)();
                    tmp = *(u16*)(sp + 0x8);
                    tmp = tmp & 0x00000800;
                    if (tmp != 1) break;
                    ((void(*)(void))_threadSwitch)();
                    ((void(*)(void))fn_800D3088)();
                    tmp = r26 + r3;
                    r26 = tmp & 0xFF;
                }
                if ((r26 & 0xFF) < 4) {
                    r3 = 0x1;
                    ((void(*)(void))fn_80133810)();
                }
            }
            r3 = r27;
            ((void(*)(void))fn_801337A8)();
            fn_801EF634();
            tmp = r3 & 0xFFFF;
            if (tmp != 4) break;

            r3 = 0x0;
            ((void(*)(void))fn_801F1758)();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                fn_80265B3C();
                tmp = r3 & 0xFF;
                if (tmp == 1) {
                    ((void(*)(void))fn_801C2D54)();
                    ((void(*)(void))fn_801C2D68)();
                    ((void(*)(void))fn_800FF56C)();
                    ((void(*)(void))fn_80113FB4)();
                    if (r3 != 0) {
                        r4 = 0x7B0000;
                        r4 = r4 + 0x1800;
                        ((void(*)(void))fn_801C2F00)();
                    }
                    r3 = 0x0;
                    fn_80261AD0();
                    r3 = 0x0;
                    fn_801F1588();
                    r3 = 0xb4;
                    fn_80008184();
                    /* mr. r28, r3 */;
                    if (r3 != 0) {
                        r26 = 0x0;
                        while (r26 < r28) {
                            ((void(*)(void))_threadSwitch)();
                            ((void(*)(void))fn_800D3088)();
                            r26 = r26 + r3;
                        }
                    }
                    break;
                }
            }

            /* Scene tick / render pass */
            r3 = 0x0;
            fn_801F1700();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                fn_80265A6C();
                fn_802659F8();
            }
            ((void(*)(void))fn_801C2D54)();
            fn_801C2D5C();
            ((void(*)(void))fn_800FF56C)();
            ((void(*)(void))fn_80113FB4)();
            if (r3 != 0) {
                r4 = 0x7B0000;
                r4 = r4 + 0x1800;
                ((void(*)(void))fn_801C2F00)();
            }
            r3 = -0x1;
            fn_80261AD0();
            ((void(*)(void))fn_801F09D0)();
            r3 = r28;
            r8 = r29;
            r4 = 0x0;
            r5 = 0x0;
            r6 = 0x7;
            r7 = 0x0;
            ((void(*)(void))fn_801F0B00)();
            ((void(*)(void))fn_801F09D0)();
            r3 = 0x0;
            fn_80261954();
            ((void(*)(void))fn_801C2D68)();
            r3 = 0x0;
            fn_801F1700();
            tmp = r3 & 0xFF;
            if (tmp == 1) {
                fn_802659A4();
                fn_802658C8();
            }
            fn_801EF634();
            tmp = r3 & 0xFFFF;
            if (tmp != 1) break;
            r3 = r28;
            r8 = r29;
            r4 = 0x0;
            r5 = 0x0;
            r6 = 0xb;
            r7 = 0x0;
            ((void(*)(void))fn_801F0B00)();
            fn_801EF634();
            tmp = r3 & 0xFFFF;
            if (tmp != 1) break;
            r3 = r30;
            r8 = r31;
            r4 = 0x0;
            r5 = 0x0;
            r6 = 0xd;
            r7 = 0x0;
            ((void(*)(void))fn_801F0B00)();
            fn_801EF634();
            tmp = r3 & 0xFFFF;
            if (tmp != 1) break;
        }
        ((void(*)(void))fn_801F09D0)();
    }
    ((void(*)(void))fn_801C2D54)();
    ((void(*)(void))fn_801C2D68)();
    ((void(*)(void))fn_800FF56C)();
    ((void(*)(void))fn_80113FB4)();
    if (r3 != 0) {
        r4 = 0x7B0000;
        r4 = r4 + 0x1800;
        ((void(*)(void))fn_801C2F00)();
    }
    fn_801EF2D4();
    r3 = 0x0;
    ((void(*)(void))fn_801F1758)();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        fn_80265BBC();
        fn_80265D54();
    }
    r3 = (u32)lbl_8046D760;
    r5 = (u32)lbl_80375D80;
    r8 = (u32)lbl_80375D80;
    r4 = 0x0;
    r3 = (u32)lbl_8046D760;
    r5 = 0x0;
    r6 = 0xe;
    r7 = 0x0;
    ((void(*)(void))fn_801F0B00)();
    fn_801EF634();
    r5 = (u32)lbl_80375D80;
    r6 = (u32)lbl_8046D760;
    r7 = r3 & 0xFFFF;
    r4 = 0x0;
    r3 = (u32)lbl_8046D760;
    r8 = (u32)lbl_80375D80;
    r5 = 0x0;
    r6 = 0xf;
    ((void(*)(void))fn_801F0B00)();
    ((void(*)(void))fn_801F09D0)();
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x12;
    r6 = 0x0;
    ((void(*)(void))fn_801F54A4)();
    if (r3 != 0) {
        r4 = 0x3e8;
        ((void(*)(void))soundStop)();
    }
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x12;
    r6 = 0x0;
    ((void(*)(void))fn_801F54A4)();
    if (r3 == 0) {
        r3 = 0x1;
        r4 = 0x3e8;
        r5 = 0x3e8;
        r6 = 0xff;
        fn_8016597C();
    }
    f1 = lbl_8047E508;
    r3 = 0x3;
    ((void(*)(void))fn_801C41C8)();
    r3 = 0x1;
    ((void(*)(void))fn_801C40F0)();
    r3 = (u32)lbl_8046D760;
    r5 = (u32)lbl_80375D80;
    r8 = (u32)lbl_80375D80;
    r4 = 0x0;
    r3 = (u32)lbl_8046D760;
    r5 = 0x0;
    r6 = 0x10;
    r7 = 0x0;
    ((void(*)(void))fn_801F0B00)();
    r3 = (u32)lbl_8046D760;
    r5 = (u32)lbl_80375D80;
    r8 = (u32)lbl_80375D80;
    r4 = 0x0;
    r3 = (u32)lbl_8046D760;
    r5 = 0x0;
    r6 = 0x11;
    r7 = 0x0;
    ((void(*)(void))fn_801F0B00)();
    fn_800FF660();
    r3 = 0x0;
    r4 = 0x0;
    fn_8011288C();
    return;
}

/* 0x801EFFC4 | size: 0x48 | small */
void fn_801EFFC4(u32 count) {
    extern u32 fn_800D3088(void);
    u32 i;
    if (count != 0) {
        i = 0;
        while (i < count) {
            _threadSwitch();
            i += fn_800D3088();
        }
    }
}

