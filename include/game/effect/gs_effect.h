/**
 * @file gs_effect.h
 * @brief GSeffect -- Genius Sonority visual effects system for Pokemon Colosseum.
 *
 * The GSeffect system manages all real-time visual effects in the game:
 * particle systems, trail effects, auras, screen filters, and more.
 *
 * Architecture:
 *   - A global table of GSEffectInstance entries is allocated from GSmem.
 *   - Each entry (0x34 bytes) stores the effect's state, function pointers
 *     for start/update/render/stop callbacks, and a user-data pointer.
 *   - Effects are identified by 1-based integer IDs.  ID 0 is invalid.
 *   - A doubly-linked active list tracks which effects are currently running.
 *   - Two per-frame task callbacks (registered via GStaskRegister) drive the
 *     update and render passes at different priorities.
 *
 * Sub-modules (each with their own start/update/render/stop callbacks):
 *   - tracefx:     Trail/trace effects (attack trails, motion trails)
 *   - leaffx:      Leaf particle effects, also handles lightning
 *   - electron:    Electrical arc effects
 *   - filter:      Full-screen colour filter effects
 *   - surfEffect:  Water surface wave effects
 *   - seaEffect:   Ocean/sea effects
 *   - envMap:      Environment-mapped reflections
 *   - blur:        Motion blur post-process
 *   - aura:        Aura glow (Shadow Pokemon aura)
 *   - distortion:  Screen distortion / heat-haze
 *   - billboard:   Billboard sprite particle effects
 *
 * Debug strings:
 *   "GSeffect: Cannot trigger effect - instance uninitialised: effect ID %d."
 *   "tracefxStartEffect: Could not start trail effect!"
 *   "leaffxStartEffect: Could not start leaf effect!"
 *   "auraEffectStart: Could not start aura effect!"
 *   "Failed to create Patchiru texture"  (Jirachi effect)
 *
 * Address ranges:
 *   Core manager (gs_effect.c):    0x80130CE0 - 0x80131500
 *   Trail effects (tracefx.c):     0x80137114 - 0x80137F58
 *   Visual effects (effect_visual.c): 0x801380D4 - 0x801402AC
 *   Generator (generator.c):       0x8017424C - 0x8017572C
 *
 * Global state:   lbl_803635C0 (GSEffectGlobals, 0x18 bytes in .data)
 */
#ifndef GS_EFFECT_H
#define GS_EFFECT_H

#include "dolphin/types.h"

/* -----------------------------------------------------------------------
 * Effect state constants
 * ----------------------------------------------------------------------- */
#define GSEFFECT_STATE_UNINIT    (-1)  /* slot unregistered / free */
#define GSEFFECT_STATE_IDLE       0    /* registered but not running */
#define GSEFFECT_STATE_STOPPING   1    /* stop requested, cleanup pending */
#define GSEFFECT_STATE_ACTIVE     2    /* currently running */

/* Maximum supported effect ID (used for array sizing) */
#define GSEFFECT_MAX_EFFECTS  256

/* -----------------------------------------------------------------------
 * Effect callback signatures
 *
 * All effect callbacks receive the user-data pointer (field 0x24 in the
 * instance) as their first argument.  The "start" callback also receives
 * a second argument: the per-frame tick count from GSgfxGetTickCount().
 * ----------------------------------------------------------------------- */
typedef BOOL (*GSEffectStartFunc)(void* userData, u32 tickCount);
typedef void (*GSEffectStopFunc)(void* userData);
typedef void (*GSEffectUpdateFunc)(void* userData);
typedef void (*GSEffectRenderFunc)(void* userData);

/* -----------------------------------------------------------------------
 * GSEffectInstance -- 0x34 bytes per effect slot.
 *
 * Deduced from GSeffectSetCallbacks (GSEffectRegister) which writes fields at
 * offsets 0x08..0x20, and GSeffect (GSEffectTrigger) which reads
 * the state at 0x04 and calls through 0x10 / 0x14.
 *
 * The linked list uses 0x2C (next) and 0x30 (prev).
 * ----------------------------------------------------------------------- */
typedef struct GSEffectInstance {
    /* 0x00 */ u16               id;            /* 1-based effect ID (matches table index + 1) */
    /* 0x02 */ u16               dataSize;      /* size of user data block */
    /* 0x04 */ s32               state;         /* GSEFFECT_STATE_* */
    /* 0x08 */ GSEffectStartFunc startFunc;     /* called when effect is triggered */
    /* 0x0C */ GSEffectStopFunc  destroyFunc;   /* cleanup / free resources */
    /* 0x10 */ GSEffectStartFunc triggerFunc;   /* trigger entry point (returns success) */
    /* 0x14 */ GSEffectStopFunc  stopFunc;      /* graceful stop callback */
    /* 0x18 */ GSEffectUpdateFunc updateFunc;   /* per-frame logic update */
    /* 0x1C */ GSEffectRenderFunc renderFunc;   /* per-frame draw call */
    /* 0x20 */ void*             extraParam;    /* additional parameter */
    /* 0x24 */ void*             userData;      /* pointer to effect-specific work area */
    /* 0x28 */ u16               memHandle;     /* GSmem handle for userData block */
    /* 0x2A */ u16               pad;
    /* 0x2C */ struct GSEffectInstance* next;   /* next in active/free list */
    /* 0x30 */ struct GSEffectInstance* prev;   /* prev in active/free list */
} GSEffectInstance;

/* -----------------------------------------------------------------------
 * GSEffectGlobals -- master state at lbl_803635C0 (0x18 bytes in .data).
 *
 * Deduced from GSeffectInit (GSEffectInit) which populates this structure,
 * and GSeffectCreate (GSEffectAllocSlot) which manipulates the free/active
 * linked lists.
 * ----------------------------------------------------------------------- */
typedef struct GSEffectGlobals {
    /* 0x00 */ u32               maxEffects;    /* total number of effect slots */
    /* 0x04 */ u16               memHandle;     /* GSmem handle for the instance table */
    /* 0x06 */ u16               pad;
    /* 0x08 */ GSEffectInstance* freeListHead;  /* head of free slot linked list */
    /* 0x0C */ void*             instanceTable; /* base pointer to the GSEffectInstance array */
    /* 0x10 */ GSEffectInstance* activeListHead;/* head of active effect linked list */
    /* 0x14 */ u32               reserved;
} GSEffectGlobals;

/* -----------------------------------------------------------------------
 * TraceFX work structure -- 0xAC bytes per trail effect instance.
 *
 * Deduced from fn_8013735C (tracefxInit) which zeroes 0xAC bytes and
 * populates fields at various offsets.
 * ----------------------------------------------------------------------- */
typedef struct TraceFXWork {
    /* 0x00 */ void*  model;            /* GSpart model pointer */
    /* 0x04 */ u32    pad_04[0x10];     /* internal state */
    /* 0x48 */ f32    startPos[3];      /* starting position (x, y, z) */
    /* 0x54 */ u32    pad_54[3];
    /* 0x60 */ u8     colorR;           /* trail colour red component */
    /* 0x61 */ u8     colorG;           /* trail colour green component */
    /* 0x62 */ u8     colorB;           /* trail colour blue component */
    /* 0x63 */ u8     colorA;           /* trail colour alpha component */
    /* 0x64 */ f32    width;            /* trail width */
    /* 0x68 */ f32    height;           /* trail height */
    /* 0x6C */ f32    depth;            /* trail depth */
    /* 0x70 */ u16    segmentCountA;    /* segment count (primary) */
    /* 0x72 */ u16    segmentCountB;    /* segment count (secondary) */
    /* 0x74 */ u32    randomSeed;       /* PRNG seed for trail variation */
    /* 0x78 */ u32    memHandle2;       /* GSmem handle for vertex buffer 2 */
    /* 0x7C */ u32    memHandle1;       /* GSmem handle for vertex buffer 1 */
    /* 0x80 */ u32    pad_80[0x06];
    /* 0x90 */ f32    scaleX;           /* X scale factor */
    /* 0x94 */ f32    scaleY;           /* Y scale factor */
    /* 0x98 */ f32    scaleZ;           /* Z scale factor */
    /* 0x9C */ f32    endScale;         /* end-of-trail scale */
    /* 0xA0 */ f32    fadeRate;         /* alpha fade rate per frame */
    /* 0xA4 */ u16    pad_A4;
    /* 0xA6 */ u16    lifetime;         /* calculated trail lifetime in frames */
    /* 0xA8 */ u32    flags;            /* trail behavior flags */
} TraceFXWork;

/* -----------------------------------------------------------------------
 * Public API -- Core effect manager (gs_effect.c)
 *
 * The manager's ten functions are all matched at 100% under their real
 * fn_ names (GSeffectInit, _GSeffectDrawAll, _GSeffectUpdateAll, GSeffectStop,
 * GSeffectIsRunning, GSeffect, GSeffectSetCallbacks, GSeffectFree, GSeffectSetup,
 * GSeffectCreate -- see gs_effect.c). Nothing outside gs_effect.c calls them
 * yet, so no prototypes are declared here; effect_visual.c and tracefx.c
 * extern-declare the fn_ names directly where they need them. A prior
 * campaign transplant's invented "GSEffectXxx" wrapper API had no real
 * callers and has been removed along with its prototypes.
 * ----------------------------------------------------------------------- */

/**
 * tracefxStartEffect -- Start a trail/trace effect.
 *
 * Start callback installed into the TraceFX effect descriptor.
 *
 * @param work    TraceFX work/configuration block.
 * @return        1 on success, 0 on failure.
 *
 * Corresponds to fn_80137AA4.
 */
BOOL tracefxStartEffect(u8* work);

/* -----------------------------------------------------------------------
 * tracefxInit, tracefxAddSegment, and tracefxUpdate (invented wrapper
 * names for fn_8013735C, fn_80137D14, and fn_80137F58) were another
 * unreferenced duplicate left by the same transplant and have been
 * removed from tracefx.c; their prototypes are gone from here too.
 * ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 * Public API -- Generator sub-module (generator.c)
 * ----------------------------------------------------------------------- */

/**
 * generatorMain -- Main update function for the script-driven particle
 * generator system.
 *
 * Called per-frame by the script interpreter.  Reads the generator's
 * configuration (velocity vector, emission rate, scale factors, etc.),
 * computes the velocity magnitude with a fast inverse square root,
 * generates rotation matrices for directional emission, and spawns
 * particles accordingly.
 *
 * The generator also handles "psCamera" type generators that drive
 * camera-attached particle effects.
 *
 * @param gen  Pointer to the generator work structure.
 *
 * Corresponds to generateParticle_8017424C (5,344 bytes -- one of the largest functions).
 * Source file confirmed by rodata string: "generator.c"
 */
void generatorMain(void* gen);

#endif /* GS_EFFECT_H */
