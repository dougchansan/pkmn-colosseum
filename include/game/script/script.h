/**
 * script.h - Particle Script (PS) VM definitions
 *
 * Pokemon Colosseum uses a custom bytecode interpreter for scripting
 * NPC behavior, cutscenes, camera control, and event progression.
 * The system is called "PS" (Particle Script) internally.
 *
 * Architecture:
 * - NOT a traditional stack-based or register-based VM
 * - Each script context (PSParticle) carries its own state:
 *   position (x,y,z), rotation, scale, color, velocity, etc.
 * - Opcodes directly manipulate these fields
 * - Bytecode is streamed from a data pointer with a program counter (PC)
 * - The first byte of each instruction is the opcode
 * - Low 3 bits of some opcodes encode which axes (x/y/z) to read args for
 * - Scripts can spawn child scripts and reference "people" (NPC) objects
 *
 * Opcode encoding (for opcodes >= 0x80):
 *   Byte 0: opcode
 *     Bits 0-2: axis flags (bit0=x, bit1=y, bit2=z) for some opcodes
 *     Bits 3-4: addressing mode for some opcodes
 *     Bits 5-7: opcode group/category
 *
 * For opcodes < 0x80:
 *   Bits 5-6: addressing mode (0x40 = object reference mode)
 *   Bits 0-4: delay/wait value (or extended with next byte if bit 5 set)
 *
 * Source files: psinterpret.c, pslist.c, psdisp.c
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include "dolphin/types.h"

/* ======================================================================
 * Constants
 * ====================================================================== */

/** Number of priority-based linked lists for active scripts */
#define PS_NUM_LINK 16

/** Size of a PSParticle struct (0x94 bytes) */
#define PS_PARTICLE_SIZE 0x94

/* ======================================================================
 * Script context flags (PSParticle.flags at offset 0x04)
 * ====================================================================== */

#define PS_FLAG_SCALE_ACTIVE    0x00000001  /* Scale factor (field 0x38) is active */
#define PS_FLAG_FRICTION_ACTIVE 0x00000002  /* Friction/damping (field 0x3C) is active */
#define PS_FLAG_ORBIT           0x00000008  /* Orbital motion mode */
#define PS_FLAG_GRAVITY_ON      0x00000020  /* Gravity subtracted from Y velocity */
#define PS_FLAG_GRAVITY_ALT     0x00000040  /* Alternative gravity mode */
#define PS_FLAG_GRAVITY_BOTH    0x00000060  /* Both gravity flags */
#define PS_FLAG_INVISIBLE       0x00000080  /* Script entity is invisible */
#define PS_FLAG_BILLBOARD       0x00000200  /* Billboard rendering mode */
#define PS_FLAG_OBJ_REF         0x00000400  /* Has object reference (obj/anim index set) */
#define PS_FLAG_PAUSED          0x00000800  /* Execution paused (bit 11) */
#define PS_FLAG_KILLED          0x00010000  /* Marked for destruction */
#define PS_FLAG_ATTACH_CAMERA   0x00008000  /* Attached to camera system */
#define PS_FLAG_NO_DETACH       0x00002000  /* Inherited: don't detach from parent */
#define PS_FLAG_SPECIAL         0x10000000  /* Special processing flag (bit 20 in rlwinm) */

/* ======================================================================
 * Opcodes (0x80-0xFF range, after subtracting 0x80 for jump table index)
 *
 * The jump table at 0x8036BFE0 has 128 entries (0x00-0x7F after subtracting 0x80).
 * Each entry points to a handler within psinterpret_Main.
 *
 * Opcode raw value = jump_table_index + 0x80
 * ====================================================================== */

typedef enum PSOpcode {
    /* --- Position / Transform opcodes --- */
    PS_OP_SET_POSITION      = 0x80,  /* Set absolute position (x,y,z from stream) */
    PS_OP_ADD_POSITION      = 0x81,  /* Add delta to position */
    PS_OP_SET_TARGET_POS    = 0x82,  /* Set target/destination position */
    PS_OP_SET_VELOCITY      = 0x83,  /* Set velocity vector (with optional parent-relative) */

    /* --- Interpolation / Timer opcodes --- */
    PS_OP_SET_LERP_TIMER    = 0x84,  /* Set interpolation timer + target value */
    PS_OP_CLEAR_OBJ_REF     = 0x85,  /* Clear object reference flag */

    /* --- Scale opcodes --- */
    PS_OP_SET_SCALE         = 0x86,  /* Set scale factor; enables/disables scale flag */
    PS_OP_SET_FRICTION      = 0x87,  /* Set friction/damping factor; enables/disables friction flag */

    /* --- Spawn / Child script opcodes (0xA0-0xA7 group) --- */
    PS_OP_SPAWN_SCRIPT      = 0xA0,  /* Spawn child script by direct script ID */
    PS_OP_SPAWN_SCRIPT_TBL  = 0xA1,  /* Spawn child script via table lookup */
    PS_OP_SPAWN_GENERATOR   = 0xA2,  /* Spawn generator (creates NPC/people) */
    PS_OP_SPAWN_GEN_TBL     = 0xA3,  /* Spawn generator via table lookup */
    PS_OP_SPAWN_GEN_FLAGS   = 0xA4,  /* Spawn generator with explicit flags byte */
    PS_OP_SPAWN_GEN_TBL_FL  = 0xA5,  /* Spawn generator via table + flags */

    /* --- Color / Material opcodes (0xA6-0xAB group) --- */
    PS_OP_SET_COLOR1_RND    = 0xA6,  /* Set primary color with random variation */
    PS_OP_SET_COLOR2_RND    = 0xA8,  /* Set secondary color with random variation */

    /* --- Rendering flag opcodes --- */
    PS_OP_SET_INVISIBLE     = 0xAC,  /* Set invisible flag */
    PS_OP_CLEAR_INVISIBLE   = 0xAD,  /* Clear invisible flag */
    PS_OP_SET_GRAVITY       = 0xAE,  /* Set gravity mode (Y velocity subtraction) */
    PS_OP_SET_GRAVITY_ALT   = 0xAF,  /* Set alternative gravity mode */
    PS_OP_SET_GRAVITY_BOTH  = 0xB0,  /* Set both gravity flags */

    /* --- People / NPC opcodes (0xB1 group) --- */
    PS_OP_DETACH_PEOPLE     = 0xB1,  /* Detach from people/NPC object */

    /* --- Alpha / Transparency opcodes --- */
    PS_OP_SET_ALPHA_INTERP  = 0xB2,  /* Set alpha with interpolation timer */

    /* --- Billboard opcodes --- */
    PS_OP_SET_BILLBOARD     = 0xB3,  /* Enable billboard rendering */
    PS_OP_CLEAR_BILLBOARD   = 0xB4,  /* Disable billboard rendering */

    /* --- Rotation / Heading opcodes --- */
    PS_OP_SET_ROT_LERP      = 0xB5,  /* Set rotation with lerp target */
    PS_OP_SET_ROT_SPEED     = 0xB6,  /* Set rotation speed (+ random) */

    /* --- Random motion opcodes --- */
    PS_OP_RANDOM_OFFSET     = 0xB7,  /* Apply random position offset */
    PS_OP_RANDOM_HEADING    = 0xB8,  /* Set heading from random value */

    /* --- Camera-relative opcodes --- */
    PS_OP_CAMERA_LOOKAT     = 0xB9,  /* Camera look-at parameters */
    PS_OP_CAMERA_COLLISION  = 0xBA,  /* Camera collision check with bounds */

    /* --- Spawn with velocity opcodes --- */
    PS_OP_SPAWN_WITH_VEL    = 0xBB,  /* Spawn script inheriting velocity + scale */
    PS_OP_SPAWN_TBL_VEL     = 0xBC,  /* Spawn via table inheriting velocity + scale */

    /* --- Scale velocity --- */
    PS_OP_SCALE_VELOCITY    = 0xBD,  /* Multiply velocity by a factor */

    /* --- Heading random opcodes --- */
    PS_OP_SET_HEADING_RND   = 0xBE,  /* Set heading + random variation */

    /* --- Attach / Camera opcodes (0xC0-0xCF group) --- */
    PS_OP_ATTACH_CAMERA     = 0xC7,  /* Attach to camera tracking system */

    /* --- Random spawn opcodes (0xD0-0xDF group) --- */
    PS_OP_SPAWN_RANDOM      = 0xD4,  /* Spawn script with random table index */

    /* --- Loop / Flow control opcodes (0xF5-0xFF group) --- */
    PS_OP_SET_ROT_ZERO      = 0xF5,  /* Reset rotation speed and acceleration to zero */
    PS_OP_SET_LOOP_COUNT    = 0xF6,  /* Set loop counter + save return address */
    PS_OP_DEC_LOOP          = 0xF7,  /* Decrement loop counter, jump back if nonzero */
    PS_OP_SAVE_PC           = 0xF8,  /* Save current PC as return address */
    PS_OP_GOTO_SAVED        = 0xF9,  /* Jump to saved PC address */
    PS_OP_YIELD             = 0xFA,  /* Yield: set repeat count = 1, end frame */
    PS_OP_TERMINATE         = 0xFB,  /* Same as yield (handler identical) */
} PSOpcode;

/* ======================================================================
 * PSParticle - Script execution context (0x94 bytes)
 *
 * Each active script instance has one of these. The name "Particle"
 * comes from the engine's origin as a particle system, but it's used
 * for all scripted entities: NPCs, cameras, effects, etc.
 * ====================================================================== */

typedef struct PSParticle {
    /* 0x00 */ struct PSParticle* next;  /* Linked list next pointer */
    /* 0x04 */ u32 flags;               /* PS_FLAG_* bitfield */
    /* 0x08 */ u8 bankIndex;            /* Script data bank index */
    /* 0x09 */ u8 animIndex;            /* Animation/object sub-index */
    /* 0x0A */ u8 objRefIndex;          /* Object reference index */
    /* 0x0B */ u8 pad0B;
    /* 0x0C */ u16 lerpTimer;           /* General-purpose lerp countdown */
    /* 0x0E */ u16 color1Timer;         /* Primary color interpolation timer */
    /* 0x10 */ u16 color2Timer;         /* Secondary color interpolation timer */
    /* 0x12 */ u8 color1R;              /* Primary color - red */
    /* 0x13 */ u8 color1G;              /* Primary color - green */
    /* 0x14 */ u8 color1B;              /* Primary color - blue */
    /* 0x15 */ u8 color1A;              /* Primary color - alpha */
    /* 0x16 */ u8 color2R;              /* Secondary color - red */
    /* 0x17 */ u8 color2G;              /* Secondary color - green */
    /* 0x18 */ u8 color2B;              /* Secondary color - blue */
    /* 0x19 */ u8 color2A;              /* Secondary color - alpha */
    /* 0x1A */ u16 waitTimer;           /* Frames to wait before continuing execution */
    /* 0x1C */ u8 loopCounter;          /* Loop iteration counter */
    /* 0x1D */ u8 linkNo;               /* Priority link list number (0-15) */
    /* 0x1E */ u16 scriptId;            /* Script identifier */
    /* 0x20 */ void* scriptData;        /* Pointer to bytecode data start */
    /* 0x24 */ u16 pc;                  /* Program counter (offset from scriptData) */
    /* 0x26 */ u16 savedPC;             /* Saved PC for goto/return */
    /* 0x28 */ u16 loopPC;              /* Saved PC for loop start */
    /* 0x2A */ u16 repeatCount;         /* Number of times to repeat (0 = run, dec on frame end) */
    /* 0x2C */ f32 velocityX;           /* X velocity */
    /* 0x30 */ f32 velocityY;           /* Y velocity */
    /* 0x34 */ f32 velocityZ;           /* Z velocity */
    /* 0x38 */ f32 scaleFactor;         /* Scale multiplier */
    /* 0x3C */ f32 frictionFactor;      /* Friction/damping multiplier */
    /* 0x40 */ f32 positionX;           /* Current X position */
    /* 0x44 */ f32 positionY;           /* Current Y position */
    /* 0x48 */ f32 positionZ;           /* Current Z position */
    /* 0x4C */ f32 lerpValue;           /* Current lerp interpolation value */
    /* 0x50 */ f32 heading;             /* Heading angle (radians) */
    /* 0x54 */ u16 alphaTimer;          /* Alpha interpolation timer */
    /* 0x56 */ u8 alphaMode;            /* Alpha blending mode */
    /* 0x57 */ u8 alphaStart;           /* Alpha interpolation start value */
    /* 0x58 */ u8 alphaEnd;             /* Alpha interpolation end value */
    /* 0x59 */ u8 cameraSlot;           /* Camera attachment slot index */
    /* 0x5A */ u16 sizeXTimer;          /* Size X interpolation timer */
    /* 0x5C */ u16 sizeYTimer;          /* Size Y interpolation timer */
    /* 0x5E */ u16 headingTimer;        /* Heading interpolation timer */
    /* 0x60 */ f32 lerpTarget;          /* Lerp interpolation target value */
    /* 0x64 */ f32 headingSpeed;        /* Heading rotation speed */
    /* 0x68 */ f32 headingAccel;        /* Heading rotation acceleration */
    /* 0x6C */ u16 color1Countdown;     /* Primary color countdown timer */
    /* 0x6E */ u16 color2Countdown;     /* Secondary color countdown timer */
    /* 0x70 */ u8 color1TargetR;        /* Primary color target - red */
    /* 0x71 */ u8 color1TargetG;        /* Primary color target - green */
    /* 0x72 */ u8 color1TargetB;        /* Primary color target - blue */
    /* 0x73 */ u8 color1TargetA;        /* Primary color target - alpha */
    /* 0x74 */ u8 color2TargetR;        /* Secondary color target - red */
    /* 0x75 */ u8 color2TargetG;        /* Secondary color target - green */
    /* 0x76 */ u8 color2TargetB;        /* Secondary color target - blue */
    /* 0x77 */ u8 color2TargetA;        /* Secondary color target - alpha */
    /* 0x78 */ u16 sizeXCountdown;      /* Size X interpolation countdown */
    /* 0x7A */ u16 sizeYCountdown;      /* Size Y interpolation countdown */
    /* 0x7C */ u16 alphaCountdown;      /* Alpha interpolation countdown */
    /* 0x7E */ u8 alphaTargetStart;     /* Alpha target start for interpolation */
    /* 0x7F */ u8 alphaTargetEnd;       /* Alpha target end for interpolation */
    /* 0x80 */ u8 sizeXCurrent;         /* Current size X value */
    /* 0x81 */ u8 sizeYCurrent;         /* Current size Y value */
    /* 0x82 */ u8 sizeXTarget;          /* Target size X value */
    /* 0x83 */ u8 sizeYTarget;          /* Target size Y value */
    /* 0x84 */ u8 sizeXStart;           /* Size X start value for interp */
    /* 0x85 */ u8 sizeYStart;           /* Size Y start value for interp */
    /* 0x86 */ u8 sizeXTargetFinal;     /* Final size X target */
    /* 0x87 */ u8 sizeYTargetFinal;     /* Final size Y target */
    /* 0x88 */ u32 pad88;
    /* 0x8C */ void* peopleObj;         /* Pointer to associated people/NPC object */
    /* 0x90 */ void* parentObj;         /* Pointer to parent/owner object */
} PSParticle;

/* ======================================================================
 * PSList globals (at BSS addresses)
 * ====================================================================== */

/**
 * Global arrays for the script list system.
 * Three arrays of PS_NUM_LINK (16) pointers each:
 *   lbl_80452708: Script data bank pointers (per link)
 *   lbl_80452748: Active flags (1 = has active scripts in this link)
 *   lbl_80452788: Head pointers for each priority link list
 */

/* ======================================================================
 * Camera attachment data (at lbl_80452DC8)
 * Each camera slot holds a pointer to a camera tracking structure.
 * ====================================================================== */

/* ======================================================================
 * Function prototypes - pslist.c
 * ====================================================================== */

/** Get head of active particle list for a given link number.
 *  Asserts that linkNo is in range [0, PS_NUM_LINK). */
PSParticle* pslist_GetHead(s32 linkNo);           /* _psListGetFirst */

/** Unlink a particle from its active list and move to free list.
 *  If parent is NULL, removes from head of the link's list.
 *  If parent is non-NULL, removes from after parent in the list. */
void pslist_Unlink(PSParticle* pp, PSParticle* parent);  /* _psListDelete */

/** Allocate a new particle from the free list and insert into a link list.
 *  If insertAfter is NULL, inserts at the head.
 *  Returns the new particle, or NULL if free list is empty. */
PSParticle* pslist_Alloc(PSParticle* insertAfter, u32 linkNo);  /* fn_80168DAC */

/** Destroy all particles and free all memory. */
void pslist_DestroyAll(void);                     /* fn_80168E88 */

/** Initialize the particle pool with a given count.
 *  Clears all 16 link lists and pre-allocates particles.
 *  Returns -1 on allocation failure. */
s32 pslist_Init(s32 count);                       /* fn_80168F28 */

/** Update visibility flags for all particles sharing a script ID. */
void pslist_UpdateVisibility(PSParticle* pp, u8 visible);  /* fn_80169034 */

/* ======================================================================
 * Function prototypes - psinterpret.c
 * ====================================================================== */

/** Run the script interpreter for all active particles.
 *  Iterates through all 16 link lists, calling psinterpret_Main on each.
 *  Parameter is a bitmask of which links to process (bit 0 = link 15). */
void psinterpret_RunAll(u32 linkMask);            /* psInterpretParticles */

/** Execute one frame of a particle script.
 *  This is the main bytecode interpreter - processes opcodes until
 *  a wait/yield is encountered or the script terminates.
 *  parentCtx is passed when recursively spawning child scripts.
 *  Returns the next particle in the list (for iteration). */
PSParticle* psInterpretParticle0(PSParticle* pp, PSParticle* parentCtx);

/* ======================================================================
 * Helper function prototypes (called by the interpreter)
 * ====================================================================== */

/** Read a u16 value from the bytecode stream and advance the pointer. */
void psReadU16(u8** streamPtr, u16* outValue);    /* getTime */

/** Read a float value from the bytecode stream and advance the pointer. */
u8* psReadFloat(u8* streamPtr, f32* outValue);    /* getFloat */

/** Spawn a new particle script by ID.
 *  Creates a new particle and starts executing the given script. */
PSParticle* psSpawnScript(PSParticle* parent, u8 linkNo, u8 bankIdx, u16 scriptId, void* arg);  /* fn_80169A48 */

/** Spawn a new generator script (creates NPC/people objects). */
PSParticle* psSpawnGenerator(u8 linkNo, u8 bankIdx, u16 scriptId);  /* psCreateGeneratorID */

/** Initialize a particle with default values after spawn. */
void psInitParticle(PSParticle* pp);              /* fn_80172AE0 */

/** Clean up a particle on termination. */
PSParticle* psCleanup(PSParticle* pp);            /* _psListGetNext */

/** Update particle's velocity based on position data. */
void psUpdateVelocity(PSParticle* pp, f32* velocityVec);  /* fn_801729EC */

/** Perform camera collision/bounds check. */
s32 psCameraCollisionCheck(PSParticle* pp, void* camData, f32 paramA, f32 paramB);  /* applyForceJObj */

/** Attach particle to camera tracking system. */
void psCameraAttach(PSParticle* pp, void* camData);  /* setVelToJObj */

/** Process rotation update for orbital motion. */
void psRotationUpdate(PSParticle* pp, f32 param);    /* fn_80172FA8 */

/** Process camera look-at parameters. */
void psCameraLookAt(PSParticle* pp, f32 fov, f32 nearDist, f32 farDist, f32 aspect);  /* fn_801732A0 */

/** Update people/NPC attachment link. */
void psPeopleLinkUpdate(PSParticle* pp);          /* fn_8016A6FC */

/** Attach particle to a people/NPC object (with parent). */
void psPeopleAttach(PSParticle* pp, void* parent);    /* fn_8016A878 */

/** Attach particle to a people/NPC object (standalone). */
void psPeopleAttachStandalone(PSParticle* pp, void* parent);  /* psAttachParticleAppSRT */

/** Detach particle from its people/NPC object. */
void psPeopleDetach(PSParticle* pp);              /* fn_8016A79C */

/** Detach particle standalone. */
void psPeopleDetachStandalone(PSParticle* pp);    /* psAttachGeneratorAppSRT */

/** Initialize generator data for a spawned generator. */
void psGeneratorInit(PSParticle* gen, void* owner);   /* fn_80172930 */

/** Random number generator (0.0 to 1.0). */
f32 psRandom(void);                              /* fn_801ADC7C */

/** Clamp a color byte with random variation. */
u8 psClampColorByte(u8 current);                  /* U8ClampAdd */

/** Update matrices for a people/NPC object. */
void psPeopleUpdateMatrices(void* people);        /* genPosUpdate */

/** Initialize people/NPC attachment data. */
void psSetupMotion(PSParticle* pp, void* owner, void* posData, void* rotData, void* scaleData, void* reserved);  /* HSD_MTXSRT */

#endif /* SCRIPT_H */
