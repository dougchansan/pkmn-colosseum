#ifndef GAME_BATTLE_H
#define GAME_BATTLE_H

#include "dolphin/types.h"

/**
 * @file battle.h
 * @brief Pokemon Colosseum Battle Engine - Core structures and declarations.
 *
 * The battle engine spans 0x801C3108 - 0x801F000C (~180KB, ~525 functions)
 * and is organized into the following subsystems:
 *
 *   0x801C3108 - 0x801C53BC : Battle Grid (scene layout, 36 functions)
 *   0x801C53BC - 0x801D7230 : Battle Logic (damage, types, status, ~205 functions)
 *   0x801D7230 - 0x801E03D4 : Waza/Move Animation System (~108 functions)
 *   0x801E03D4 - 0x801EF02C : Battle State Machine (~150 functions) [battle_logic.c]
 *   0x801EF02C - 0x801F000C : Battle Core / Fight Flow (26 functions) [battle_main.c]
 *
 * The battle_logic.c TU contains all interleaved battle logic functions
 * (formerly split across battle_move.c, battle_type.c, battle_status.c,
 * battle_damage.c, battle_ai.c, and battle_shadow.c). Link order analysis
 * confirmed these were originally compiled as a single translation unit.
 *
 * Colosseum uses exclusively Double Battles with up to 4 Pokemon on the
 * field at once (2 per side). The engine is built on Gen III mechanics
 * with additions for Shadow Pokemon (Hyper Mode, Shadow Rush, snagging).
 *
 * Key data structures live in BSS:
 *   lbl_8046D500 (0x230 bytes) : Battle state machine context
 *   lbl_8046AC60 (0x100 bytes) : Battle transfer / communication context
 *   lbl_8046A440 (0xA20 bytes) : Battle timer / round tracking context
 *   lbl_80467030 (0x20  bytes) : Battle camera context
 *   lbl_80466E50 (0x1E0 bytes) : Battle scene animation context
 *
 * The battle state machine uses a large switch-case (jumptable at
 * jumptable_803751B8) with 13+ states managing the flow from
 * encounter setup through move selection, execution, and resolution.
 */

/* =========================================================================
 * Forward declarations
 * ========================================================================= */

/* Pokemon data structure size is 0xE0 (224) bytes, indexed via mulli rN, rN, 0xE0 */
struct BattlePokemon;

/* =========================================================================
 * Constants
 * ========================================================================= */

/* Battle positions: Colosseum is always Double Battle */
#define BATTLE_POS_PLAYER_LEFT   0
#define BATTLE_POS_PLAYER_RIGHT  1
#define BATTLE_POS_ENEMY_LEFT    2
#define BATTLE_POS_ENEMY_RIGHT   3
#define BATTLE_NUM_POSITIONS     4

/* Number of Pokemon per side in a Double Battle */
#define BATTLE_POKEMON_PER_SIDE  2
#define BATTLE_TOTAL_POKEMON     4

/* Pokemon types (Gen III type IDs) */
#define TYPE_NORMAL    0
#define TYPE_FIGHTING  1
#define TYPE_FLYING    2
#define TYPE_POISON    3
#define TYPE_GROUND    4
#define TYPE_ROCK      5
#define TYPE_BUG       6
#define TYPE_GHOST     7
#define TYPE_STEEL     8
#define TYPE_FIRE      9
#define TYPE_WATER    10
#define TYPE_GRASS    11
#define TYPE_ELECTRIC 12
#define TYPE_PSYCHIC  13
#define TYPE_ICE      14
#define TYPE_DRAGON   15
#define TYPE_DARK     16
#define TYPE_SHADOW   17  /* Colosseum-exclusive Shadow type */
#define TYPE_COUNT    18

/* Type effectiveness multipliers (stored as fixed-point: 0=immune, 5=0.5x, 10=1x, 20=2x) */
#define TYPE_EFF_IMMUNE     0
#define TYPE_EFF_NOT_VERY   5
#define TYPE_EFF_NORMAL    10
#define TYPE_EFF_SUPER     20

/* Status conditions (primary - mutually exclusive) */
#define STATUS_NONE       0x00
#define STATUS_SLEEP      0x07  /* Bits 0-2: sleep counter */
#define STATUS_POISON     0x08
#define STATUS_BURN       0x10
#define STATUS_FREEZE     0x20
#define STATUS_PARALYSIS  0x40
#define STATUS_TOXIC      0x80

/* Volatile status (can stack) */
#define VSTATUS_CONFUSION   (1 << 0)
#define VSTATUS_FLINCH      (1 << 1)
#define VSTATUS_ATTRACT     (1 << 2)
#define VSTATUS_FOCUS       (1 << 3)
#define VSTATUS_CHARGE      (1 << 4)

/* Move categories */
#define MOVE_CAT_PHYSICAL  0
#define MOVE_CAT_SPECIAL   1
#define MOVE_CAT_STATUS    2

/* Shadow Pokemon states */
#define SHADOW_NORMAL      0
#define SHADOW_HYPER_MODE  1

/* Battle mode constants (from menuToolBattle.c assert string) */
#define BATTLEMODE_BATTLEYAMA100  0  /* Story mode battle */

/* Battle grid model slots (from fn_801C3D64 / fn_801C3F10) */
#define GRID_SLOT_POKEMON  0
#define GRID_SLOT_TRAINER  1

/* Battle state machine states (from jumptable_803751B8, 13 entries) */
#define BATTLE_STATE_INIT          0
#define BATTLE_STATE_INTRO         1
#define BATTLE_STATE_SEND_OUT      2
#define BATTLE_STATE_COMMAND_SEL   3
#define BATTLE_STATE_TARGET_SEL    4
#define BATTLE_STATE_TURN_ORDER    5
#define BATTLE_STATE_EXECUTE_MOVE  6
#define BATTLE_STATE_APPLY_DAMAGE  7
#define BATTLE_STATE_CHECK_FAINT   8
#define BATTLE_STATE_SWITCH_IN     9
#define BATTLE_STATE_END_TURN     10
#define BATTLE_STATE_RESULT       11
#define BATTLE_STATE_CLEANUP      12

/* Waza (move) sequence entry types (from wazaSequenceEntryStart error strings) */
#define WAZA_ENTRY_PARTICLE  0
#define WAZA_ENTRY_MODEL     1
#define WAZA_ENTRY_CAMERA    2
#define WAZA_ENTRY_SOUND     3

/* Scene object IDs used by battle fight-end cleanup (from lbl_80279B84 rodata table) */
/* These are the 21 scene object indices cleaned up during fight end */
#define BATTLE_SCENE_OBJ_COUNT  21

extern const char lbl_80279BD8[];

/* =========================================================================
 * Structures
 * ========================================================================= */

/**
 * Battle camera state, stored at lbl_80467030.
 * Controls the 3D camera during battle (pan, zoom, rotation).
 * Size: 0x20 bytes.
 */
typedef struct BattleCameraState {
    void*  pCameraObj;       /* 0x00: Camera scene object pointer */
    void*  pCameraObj2;      /* 0x04: Secondary camera object */
    u16    frameCounter;     /* 0x08: Frame counter for animations */
    u16    cameraMode;       /* 0x0A: Current camera behavior mode */
    u16    padding_0C;       /* 0x0C: Camera mode subcategory */
    u16    padding_0E;       /* 0x0E */
    f32    currentAngle;     /* 0x10: Current rotation angle */
    f32    targetAngle;      /* 0x14: Target rotation angle */
    f32    maxAngle;         /* 0x18: Maximum angle bound */
    u32    reserved;         /* 0x1C */
} BattleCameraState;

/**
 * Battle grid entry for a single position on the field.
 * Used by battleGridUpdate (was fn_801C3430) for scene layout.
 */
typedef struct BattleGridSlot {
    void*  pModel;           /* 0x00: HSD JObj pointer for the model */
    u8     side;             /* 0x04: 0 = player side, 1 = enemy side */
    u8     position;         /* 0x05: 0 = left, 1 = right */
    u8     occupied;         /* 0x06: 1 if a Pokemon is in this slot */
    u8     padding;          /* 0x07 */
} BattleGridSlot;

/**
 * Stat stage modifiers for a Pokemon in battle.
 * Range: -6 to +6 (stored as signed bytes).
 */
typedef struct StatStages {
    s8 attack;
    s8 defense;
    s8 spAttack;
    s8 spDefense;
    s8 speed;
    s8 accuracy;
    s8 evasion;
    s8 padding;
} StatStages;

/**
 * Per-Pokemon battle state.
 * This is the runtime representation of a Pokemon during battle.
 * Size: 0xE0 (224) bytes based on mulli indexing in disassembly.
 */
typedef struct BattlePokemon {
    /* 0x00 */ u16  species;
    /* 0x02 */ u16  currentHP;
    /* 0x04 */ u16  maxHP;
    /* 0x06 */ u16  attack;
    /* 0x08 */ u16  defense;
    /* 0x0A */ u16  spAttack;
    /* 0x0C */ u16  spDefense;
    /* 0x0E */ u16  speed;
    /* 0x10 */ u8   level;
    /* 0x11 */ u8   type1;
    /* 0x12 */ u8   type2;
    /* 0x13 */ u8   ability;
    /* 0x14 */ u32  statusCondition;
    /* 0x18 */ u32  volatileStatus;
    /* 0x1C */ StatStages statStages;
    /* 0x24 */ u16  moves[4];
    /* 0x2C */ u8   movePP[4];
    /* 0x30 */ u8   moveMaxPP[4];
    /* 0x34 */ u16  heldItem;
    /* 0x36 */ u8   gender;
    /* 0x37 */ u8   nature;
    /* 0x38 */ u32  experience;
    /* 0x3C */ u32  personalityValue;
    /* 0x40 */ u16  trainerID;
    /* 0x42 */ u16  secretID;
    /* 0x44 */ u8   friendship;
    /* 0x45 */ u8   isShadow;
    /* 0x46 */ u8   shadowMode;          /* 0 = normal, 1 = Hyper Mode */
    /* 0x47 */ u8   padding_47;
    /* 0x48 */ u16  shadowGauge;         /* Heart gauge for purification */
    /* 0x4A */ u16  shadowGaugeMax;
    /* 0x4C */ u8   evHP, evAtk, evDef, evSpAtk, evSpDef, evSpd;
    /* 0x52 */ u8   ivHP, ivAtk, ivDef, ivSpAtk, ivSpDef, ivSpd;
    /* 0x58 */ u8   padding_58[0x88];    /* remaining fields TBD */
} BattlePokemon;

/**
 * Turn action selected by a player or AI for one Pokemon.
 */
typedef struct TurnAction {
    u8   actionType;        /* 0 = fight, 1 = switch, 2 = item, 3 = run, 4 = call (shadow) */
    u8   moveIndex;         /* Index into BattlePokemon.moves[] */
    u8   targetSlot;        /* Target battle position */
    u8   priority;          /* Move priority level */
    u16  moveID;            /* Move ID for this action */
    u16  speedValue;        /* Effective speed for turn ordering */
} TurnAction;

/**
 * Move data entry from the move table.
 * Based on Gen III move data structure.
 */
typedef struct MoveData {
    u8   effect;
    u8   basePower;
    u8   type;
    u8   accuracy;
    u8   pp;
    u8   effectChance;
    u8   target;
    s8   priority;
    u8   flags;           /* contact, sound-based, etc. */
    u8   category;        /* physical / special / status */
    u8   padding[2];
} MoveData;

/**
 * Type effectiveness table entry.
 * In Gen III, type matchups are stored as a flat table of
 * (attacking_type, defending_type, effectiveness) triples,
 * terminated by 0xFF.
 */
typedef struct TypeMatchup {
    u8 attackType;
    u8 defendType;
    u8 effectiveness;     /* TYPE_EFF_* constant */
} TypeMatchup;

/**
 * Battle fight flow state, stored at sda21 offsets.
 * Small state variables controlling the overall fight lifecycle.
 *   lbl_8047B5D0 (u32) : Thread handle for the battle main loop
 *   lbl_8047B5D4 (u8)  : Saved scene ID before battle
 *   lbl_8047B5D5 (u8)  : Saved VSync mode before battle
 *   lbl_8047B5D6 (u16) : Battle result code (win/lose/draw)
 *   lbl_8047B5D8 (u16) : Battle status flags
 *   lbl_8047B5DA (u8)  : Fight-in-progress flag
 */

/* =========================================================================
 * Pre-Grid / Field Setup (0x801C01C8 - 0x801C3108)
 * [hsd/hsd_aobj_range_801C01C8.c, game/battle/battle_camera.c]
 *
 * battle_grid.c (0x801C0F20 - 0x801C4CB8) was split (2026-07-07) into 5
 * true XD translation units: hsd/hsd_aobj_range_801C01C8.c (extended),
 * game/battle/battle_camera.c, game/battle/battle_grid.c (shrunk),
 * game/effect/fade.c, and game/effect/fade_effect.c.
 * ========================================================================= */

/* fn_801C01C8 */ void* fn_801C01C8(void* callback);
/* fn_801C021C */ void* fn_801C021C(void* callback);
/* fn_801C0270 */ s32  fn_801C0270(void);
/* fn_801C027C */ void fn_801C027C(void* obj, f32 frame);
/* fn_801C028C */ void fn_801C028C(void* ctx);                   /* 0xC94 pre-grid state machine */
/* 0x801C0F20-0x801C25E4: the HSD sysdolphin ForeachAnim dispatch family
 * (JObj/DObj/LObj/CObjForeachAnim), NOT battle-grid code. Real signatures
 * use HSD_TypeMask/HSD_ForeachArgType/HSD_ForeachArg, local to
 * hsd/hsd_aobj_range_801C01C8.c. Old "pre-grid" descriptions were fiction. */
/* HSD_AObjRemove (was fn_801C25E4) */ void HSD_AObjRemove(void* ctx, s32 mode);
/* HSD_AObjLoadDesc (was fn_801C2670) */ void HSD_AObjLoadDesc(void* ctx, s32 objType, s32 param);
/* HSD_AObjInterpretAnim (was fn_801C27F4) */ void HSD_AObjInterpretAnim(void* ctx, f32 posX, f32 posZ);
/* HSD_AObjReqAnim */ void HSD_AObjReqAnim(void* obj, f32 value);
/* HSD_AObjInvokeCallBacks */ void HSD_AObjInvokeCallBacks(void);
/* HSD_AObjInitEndCallBack (was fn_801C2A60) */ void HSD_AObjInitEndCallBack(void);
/* HSD_AObjClearFlags (was fn_801C2A74) */ s32  HSD_AObjClearFlags(s32 slot);
/* HSD_AObjSetFlags (was fn_801C2A90) */ s32  HSD_AObjSetFlags(s32 slot);
/* HSD_AObjGetAllocData */ void* HSD_AObjGetAllocData(void);
/* HSD_AObjInitAllocData (was fn_801C2AB8) */ void HSD_AObjInitAllocData(s32 slot, s32 animState);
/* battleGridGetNumPokemonsForTrainer (was fn_801C2AE8) */ u16 battleGridGetNumPokemonsForTrainer(u32 id);
/* battleGridResetModelVisibilityFlags (was fn_801C2B2C) */ void battleGridResetModelVisibilityFlags(void);
/* battleGridHideModelsExcept (was fn_801C2Be0) */ void battleGridHideModelsExcept(void* ctx);

/* Grid tick / state (0x801C2D54 - 0x801C3108) [game/battle/battle_camera.c] */
/* battleCameraIsSimple (was fn_801C2D54) */ void battleCameraIsSimple(void);                        /* grid tick 1 */
/* battleCameraDoFull (was fn_801C2D5C) */ void battleCameraDoFull(void);                        /* grid tick wrapper */
/* battleCameraDoSimple (was fn_801C2D68) */ void battleCameraDoSimple(void);                        /* grid tick 2 */
/* battleCameraDisable (was fn_801C2D74) */ void battleCameraDisable(void);                        /* grid tick 3 */
/* battleCameraStartRandom (was fn_801C2D80) */ void battleCameraStartRandom(void);                        /* grid cleanup 0x180 */
/* fn_801C2F00 */ void fn_801C2F00(void* data, u32 size);        /* grid load data 0x208 */

/* =========================================================================
 * Battle Grid Core (0x801C3108 - 0x801C53BC)
 * [game/battle/battle_grid.c, game/effect/fade.c, game/effect/fade_effect.c]
 * ========================================================================= */

/* battleGridGetPtr (was fn_801C3108) */ void* battleGridGetPtr(void);                       /* battleGrid_GetState */
/* fn_801C3114 */ void fn_801C3114(void);                        /* battleGrid_Init */
/* fn_801C31EC */ void fn_801C31EC(void);                        /* battleGrid_Setup */
/* battleGridUpdate (was fn_801C3430) */ void battleGridUpdate(void);                        /* battleGridSetup 0x634 */
/* battleGridGetDistance (was fn_801C3A64) */ void battleGridGetDistance(void);                        /* battleGridLoadModels 0x11C */
/* battleGridGetNormalisedScale (was fn_801C3B80) */ void battleGridGetNormalisedScale(void);                        /* battleGridUpdatePositions */
/* battleGridRemovePokemon (was fn_801C3C98) */ void battleGridRemovePokemon(u8* pokemon);
/* battleGridReplacePokemon (was fn_801C3D64) */ void battleGridReplacePokemon(u8* pokemon, u8* replacement);
/* battleGridAddPokemon (was fn_801C3E3C) */ void battleGridAddPokemon(u8* slot, u8* pokemon);
/* battleGridReplaceTrainer (was fn_801C3F10) */ void battleGridReplaceTrainer(u8* trainer, u8* replacement);
/* battleGridAddTrainer (was fn_801C3FBC) */ void battleGridAddTrainer(u8* slot, u8 arg1, u8 arg2);
/* fadeEffectDokuStop (was fn_801C4078) */ void* fadeEffectDokuStop(s32 slot);
/* fadeEffectDokuStart */ void fadeEffectDokuStart(void);
/* fadeCheck */ void fadeCheck(s32 flag);                    /* grid set rendering flag */
/* fadeSetEX */ void fadeSetEX(s32 mode, void* callback, s32 flags, f32 a, f32 b);
/* fadeSet */ void fadeSet(s32 mode);                    /* battle camera init */
/* fadeSetFunction__FPFv_vbUsf (was fn_801C423C) */ void* fadeSetFunction__FPFv_vbUsf(void (*callback)(void), u8 mode, u32 arg, f32 value);
/* fadeSetFunctionOnly (was fn_801C431C) */ void fadeSetFunctionOnly(s32 arg0);
/* _fadeSnapshot__Fv (was fn_801C432C) */ void _fadeSnapshot__Fv(void);
typedef struct GStexture GStexture;
/* myBackFB__FP9GStextureUlPv (was fn_801C43E4) */ void* myBackFB__FP9GStextureUlPv(GStexture* texture, u32 size, void* userData);
/* fadeDaemon (was fn_801C43F4) */ void fadeDaemon(s32 seqType, f32 param1, f32 param2); /* 0x3DC */
/* fadeInit */ s32  fadeInit(void);
/* fadeEffectHookFunction_Doku_Init (was fn_801C4814) */ f32  fadeEffectHookFunction_Doku_Init(s32 slot);
/* fadeEffectHookFunction_trainer_Init (symbol swap fix: was named fadein_Init) */ f32  fadeEffectHookFunction_trainer_Init(s32 slot);
/* fadeEffectHookFunction_fadein_Init (symbol swap fix: was named trainer_Init) */ f32  fadeEffectHookFunction_fadein_Init(s32 slot);
/* fadeEffectHookFunction_fadeout_in_Init */ void fadeEffectHookFunction_fadeout_in_Init(s32 slot, f32 x);
/* fadeEffectHookFunction_carde_Init */ void fadeEffectHookFunction_carde_Init(s32 slot, f32 y);
/* fadeEffectHookFunction_boss_Init */ void fadeEffectHookFunction_boss_Init(s32 slot, f32 z);
/* fadeEffectHookFunction_yoko_or_tate_or_ball_Init */ void fadeEffectHookFunction_yoko_or_tate_or_ball_Init(s32 slot, f32 x, f32 y, f32 z);
/* fadeEffectHookFunction_ball_Init */ f32  fadeEffectHookFunction_ball_Init(s32 slot);
/* fadeEffectHookFunction_yoko_or_tate_Init */ void fadeEffectHookFunction_yoko_or_tate_Init(s32 slot, f32 rotation);
/* fadeEffectHookFunction_tate_Init */ f32  fadeEffectHookFunction_tate_Init(s32 slot);
/* fadeEffectHookFunction_yoko_Init */ void fadeEffectHookFunction_yoko_Init(s32 slot, f32 scale);
/* fadeEffectHookFunction_Doku (was fn_801C4A44) */ void fadeEffectHookFunction_Doku(s32 slot, f32 x, f32 y, f32 z, f32 rot, f32 scale);
/* fadeEffectHookFunction_carde (was fn_801C4C98) */ f32  fadeEffectHookFunction_carde(void);
/* fn_801C4CB8 */ void fn_801C4CB8(void);                        /* grid full render 0x704 (game/effect/fade_range_801C4CB8.c) */

/* =========================================================================
 * Battle Scene (0x801C53BC - 0x801D1338) [battle_scene.c]
 * Camera, Pokemon model, animation, effects, transitions
 * ========================================================================= */

/* Camera control */
/* fn_801C53BC */ void fn_801C53BC(void* ctx, s32 arg1, s32 arg2, s32 arg3, f32 arg4);
/* fn_801C54FC */ f32  fn_801C54FC(void);
/* fn_801C5530 */ void fn_801C5530(f32 fov, f32 minFov, f32 maxFov);
/* fn_801C55D8 */ void fn_801C55D8(f32 x, f32 y, f32 z, f32 speed);
/* fn_801C5748 */ void fn_801C5748(void* target, f32 speed);
/* fn_801C5898 */ void fn_801C5898(void* camObj, void* target, f32 speed, f32 fov, f32 nearClip, f32 farClip);
/* fn_801C5B60 */ void fn_801C5B60(f32 zoomTarget, f32 zoomSpeed);
/* fn_801C5D60 */ void fn_801C5D60(f32 angle, f32 radius, f32 height);
/* fn_801C5ED0 */ f32  fn_801C5ED0(void);
/* fn_801C5F6C */ f32  fn_801C5F6C(void);
/* fn_801C6008 */ void fn_801C6008(u8 transition, void* target, f32 speed, f32 zoom);
/* fn_801C6274 */ void fn_801C6274(f32 t, f32 ease);
/* fn_801C63B8 */ u8   fn_801C63B8(void);
/* fn_801C63C0 */ void fn_801C63C0(void* target, void* params, f32 t, f32 speed, f32 zoom, f32 blend);
/* fn_801C6688 */ void fn_801C6688(f32 angle);
/* fn_801C673C */ void fn_801C673C(void);
/* fn_801C6760 */ void fn_801C6760(f32 intensity, f32 duration);
/* fn_801C680C */ void fn_801C680C(s32 arg0);
/* _fadeEffect_AdjustParms__Fv */ u8   _fadeEffect_AdjustParms__Fv(void);
/* fn_801C6908 */ void fn_801C6908(void);
/* fn_801C6928 */ u8   fn_801C6928(void);

/* Scene layout */
/* fn_801C6934 */ void fn_801C6934(void* stageModel, f32 scale);
/* fn_801C6AE8 */ void fn_801C6AE8(s32 row, s32 col, u8 type);
/* fn_801C6D78 */ void fn_801C6D78(s32 slot, f32 x, f32 y, f32 z);
/* _fadeFluidSetShockSub__FUlUlf */ void* _fadeFluidSetShockSub__FUlUlf(s32 slot);
/* fn_801C6EE4 */ void fn_801C6EE4(s32 slot, void* modelData);
/* fadeFluidCalcParms */ s32  fadeFluidCalcParms(void);
/* fn_801C70FC */ void fn_801C70FC(s32 slot, u8 visible);
/* fadeFluidInit */ void fadeFluidInit(u32 columns, u32 rows, f32 cellSize,
                                       f32 calcStep, f32 waveLimit,
                                       f32 timeStep);                 /* updateAnimations 0x43C */
/* fn_801C75EC */ void fn_801C75EC(s32 slot);
/* fn_801C7630 */ f32  fn_801C7630(s32 slot);
/* fn_801C766C */ void fn_801C766C(s32 slot, f32 frame);

/* Pokemon model placement & animation */
/* fn_801C7730 */ s32  fn_801C7730(s32 side, s32 slot);          /* PlacePokemon 0xDCC */
/* _fnDistanceSortFunc__FPCvPCv */ s32  _fnDistanceSortFunc__FPCvPCv(s32 slot);
/* fn_801C852C */ f32  fn_801C852C(s32 species);
/* fn_801C8628 */ void fn_801C8628(s32 slot);
/* fn_801C8688 */ void fn_801C8688(s32 slot, s32 animIdx);
/* fn_801C8804 */ s32  fn_801C8804(s32 slot);
/* fn_801C8834 */ void fn_801C8834(s32 slot);
/* fn_801C89F8 */ void fn_801C89F8(s32 side, s32 slot);          /* EncounterSequence 0x3D8 */
/* fn_801C8DD0 */ s32  fn_801C8DD0(s32 slot);
/* fn_801C8E14 */ s32  fn_801C8E14(s32 pokemonID, s32 animIdx, s16 moveID, u8 animType);

/* Scene state & effects */
/* fn_801C9910 */ void fn_801C9910(void);
/* fn_801C9B6C */ void fn_801C9B6C(s32 transitionType);
/* fn_801C9C9C */ s32  fn_801C9C9C(void);
/* fn_801C9F5C */ void fn_801C9F5C(void);
/* fn_801CA358 */ u8   fn_801CA358(void);
/* fn_801CA5C4 */ void fn_801CA5C4(s32 effectID, f32 x, f32 y, f32 z);

/* Scene transitions & mega-functions */
/* fn_801CBF64 */ void fn_801CBF64(void);                        /* complex anim controller 0x41C */
/* fn_801CC380 */ void fn_801CC380(void);                        /* mega-function A 0x1784 */
/* fn_801CDB04 */ void fn_801CDB04(void);                        /* mega-function B 0x181C */
/* fn_801CF9C8 */ void fn_801CF9C8(void);
/* fn_801CFD08 */ void fn_801CFD08(void);                        /* transition controller 0x378 */
/* fn_801D0DB0 */ void fn_801D0DB0(void);                        /* rendering main loop 0x588 */

/* =========================================================================
 * Waza / Move Animation System (0x801D1338 - 0x801E03D4) [battle_waza.c]
 * ========================================================================= */

/* Waza data access */
/* fn_801D1338 */ u16  fn_801D1338(void);
/* fn_801D1364 */ void* fn_801D1364(u16 handle, s32 idx);
/* fn_801D139C */ u32  fn_801D139C(s32 idx);
/* fn_801D13E4 */ u32  fn_801D13E4(s32 idx);
/* fn_801D142C */ u32  fn_801D142C(s32 idx);

/* Waza animation controllers */
/* fn_801D1734 */ void fn_801D1734(s32 attackerSlot, s32 targetSlot, s32 moveID);
/* fn_801D1864 */ void fn_801D1864(s32 attackerSlot, s32 targetSlot, s32 moveID, s32 cameraMode);
/* fn_801D2D28 */ void fn_801D2D28(void);

/* Waza animation state machines */
/* battleCameraStartWaza */ void battleCameraStartWaza(void* owner, void* sequence); /* dispatcher 0x3E0 */
/* fn_801D349C */ void fn_801D349C(void);                        /* state machine A 0xAE0 */
/* fn_801D3F7C */ void fn_801D3F7C(void);                        /* state machine B 0x548 */
/* _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif */ void _wazaSequenceCameraDoFOV__FP13ModelSequenceP24wazaSequenceCameraParamsif(void);                        /* state machine C 0x514 */
/* fn_801D49D8 */ void fn_801D49D8(void);                        /* state machine D 0x3C8 */
/* fn_801D624C */ void fn_801D624C(void);                        /* mega-function 0x818 */

/* Core waza sequence API */
/* fn_801D7464 */ void fn_801D7464(void);                        /* wazaSequenceLoad 0x730 */
/* fn_801D7B94 */ void fn_801D7B94(void);                        /* wazaSequenceUpdate 0x2C4 */
/* fn_801D7E58 */ void fn_801D7E58(void* entry);                 /* wazaSequenceEntryStop */
/* fn_801D81CC */ void fn_801D81CC(void* entry);                 /* wazaSequenceEntryUpdate */
/* fn_801D84F4 */ void fn_801D84F4(void);                        /* wazaSequenceEntryStart */
/* _wazaSequenceEffectEntryStart */ u8 _wazaSequenceEffectEntryStart(void* entry);       /* wazaSequenceStartEntry */
/* fn_801D8B38 */ void fn_801D8B38(void* entry);                 /* _wazaSequenceParticleEntryStart */
/* _wazaSequenceModelEntryStart */ u8 _wazaSequenceModelEntryStart(void* entry);                 /* _wazaSequenceModelEntryStart */
/* fn_801D9950 */ void fn_801D9950(void* owner, f32* scale, s32 selector);    /* wazaSequencePokemonMotionStart */

/* Waza system lifecycle */
/* wazaSequenceSysRelease */ void wazaSequenceSysRelease(void);                        /* waza system cleanup */
/* fn_801DADC0 */ void fn_801DADC0(void* context);                /* waza partial reset */
/* fn_801DAEF8 */ void fn_801DAEF8(s32 count);                   /* waza system init */
/* wazaSequenceSysGetResID */ BOOL wazaSequenceSysGetResID(void);                        /* waza get initialized */
/* fn_801DB088 */ void fn_801DB088(void);                        /* waza system reset */

/* Waza extended functions */
/* fn_801DFC30 */ void fn_801DFC30(void);                        /* waza master controller 0x7A4 */

/* =========================================================================
 * Battle Core / Fight Flow (0x801EF02C - 0x801F000C)
 * ========================================================================= */

/* fn_801EF02C */ void battle_InitSlots(void);
/* fn_801EF080 */ void battle_ResetSlotAnimations(void);
/* fn_801EF0D4 */ void battle_ResetSlotEffects(void);
/* fn_801EF128 */ void battle_SetupParty(void* partyData);
/* fn_801EFA08 */ void battle_MainLoop(void);   /* 0x5BC bytes - the main battle loop */

/* =========================================================================
 * Battle State Machine (0x801E03D4 - 0x801EF02C)
 * ========================================================================= */

/* fn_801E03D4 */ s32  battleStateMachine_Main(void);  /* 13-state switch via jumptable_803751B8 */
/* fn_801E6684 */ void battleStateMachine_Animate(void);   /* ~6.8KB, PS-heavy animation */
/* fn_801E810C */ void battleStateMachine_MoveExec(void);  /* ~6.8KB, move execution */
/* fn_801E9B98 */ void battleStateMachine_Resolve(void);   /* ~6.8KB, damage resolution */

/* =========================================================================
 * Damage Calculation (decompiled from state machine internals)
 * ========================================================================= */

/**
 * Gen III damage formula:
 *   damage = ((2 * level / 5 + 2) * power * atk / def) / 50 + 2
 *   damage *= STAB modifier (1.5x if move type matches pokemon type)
 *   damage *= type effectiveness
 *   damage *= random factor (85-100) / 100
 *   damage *= critical hit (2x)
 *   damage *= other modifiers (abilities, items, weather, etc.)
 */
s32  battle_CalcDamage(BattlePokemon* attacker, BattlePokemon* defender,
                       const MoveData* move, u8 isCritical);
u8   battle_GetTypeEffectiveness(u8 attackType, u8 defType1, u8 defType2);
BOOL battle_IsSTAB(BattlePokemon* attacker, u8 moveType);
s32  battle_ApplyStatStage(s32 baseStat, s8 stage);
u8   battle_CalcCriticalHit(BattlePokemon* attacker, const MoveData* move);
s32  battle_GetRandomDamageFactor(void);

/* =========================================================================
 * Type Effectiveness
 * ========================================================================= */

u8   battle_CalcTypeMatchup(u8 atkType, u8 defType);

/* =========================================================================
 * Turn Order
 * ========================================================================= */

void battle_DetermineTurnOrder(TurnAction actions[], s32 count);
s32  battle_ComparePriority(const TurnAction* a, const TurnAction* b);

/* =========================================================================
 * Status Effects
 * ========================================================================= */

void battle_ApplyStatusDamage(BattlePokemon* pokemon);
BOOL battle_CheckStatusPreventsMove(BattlePokemon* pokemon);
void battle_TickStatusCounters(BattlePokemon* pokemon);
void battle_TryInflictStatus(BattlePokemon* target, u32 status, u8 chance);

/* =========================================================================
 * Move Execution
 * ========================================================================= */

s32  battle_ExecuteMove(s32 attackerSlot, s32 targetSlot, u16 moveID);
BOOL battle_CheckAccuracy(BattlePokemon* attacker, BattlePokemon* defender,
                          const MoveData* move);
void battle_ApplyMoveEffect(BattlePokemon* target, const MoveData* move);

/* =========================================================================
 * Shadow Pokemon
 * ========================================================================= */

BOOL battle_IsShadowPokemon(BattlePokemon* pokemon);
void battle_EnterHyperMode(BattlePokemon* pokemon);
void battle_ExitHyperMode(BattlePokemon* pokemon);
s32  battle_CalcShadowRushDamage(BattlePokemon* attacker, BattlePokemon* defender);
BOOL battle_CanSnag(BattlePokemon* target);
void battle_ProcessSnagging(s32 targetSlot);
void battle_CallPokemon(BattlePokemon* pokemon);

/* =========================================================================
 * AI
 * ========================================================================= */

void battle_AIChooseAction(s32 trainerSlot, TurnAction* outAction);
s32  battle_AIEvaluateMove(s32 aiSlot, s32 targetSlot, u16 moveID);

#endif /* GAME_BATTLE_H */
