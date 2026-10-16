/**
 * @file battle_main.c
 * @brief Main battle loop, fight start/end, and core fight flow control.
 *
 * Address range: 0x801EF02C - 0x801F0058 (27 functions), per
 * config/GC6E01/splits.txt.
 *
 * A prior transplant pass had introduced nine fictional `battle_*` wrapper
 * functions (battle_SetResult, battle_GetResult, battle_SetStatusFlags,
 * battle_GetStatusFlags, battle_IsFightInProgress, battle_FightReset,
 * battle_FightEnd, battle_FightCleanup, battle_FightStart). None of these
 * names appear in config/GC6E01/symbols.txt. The first five duplicated the
 * real, already-matched accessor scaffolds (fightSetEncountId/624/62C/634/63C).
 * The remaining four claimed to be the bodies of real unmatched symbols
 * (fn_801EF5C0, _fightFinalize__FUi14FloorEnterMode, fn_801EF488,
 * _fightInitialize__FUi14FloorEnterMode) but used (void) signatures that
 * contradict the mangled names' implied (u32, FloorEnterMode) parameters,
 * referenced a nonexistent `battle_MainLoop` symbol, and had internally
 * inconsistent bodies (e.g. filling a 3-entry config array but only
 * passing count=2 to fn_800FF4D4) -- consistent with invented bodies
 * rather than real decompilation. All nine have been removed.
 *
 * BSS state variables (SDA21-relative):
 *   lbl_8047B5D0 : u32, battle thread handle
 *   lbl_8047B5D4 : u8,  saved scene ID
 *   lbl_8047B5D5 : u8,  saved VSync mode
 *   lbl_8047B5D6 : u16, battle result code
 *   lbl_8047B5D8 : u16, battle status flags
 *   lbl_8047B5DA : u8,  fight-in-progress flag
 */

#define battleCameraIsSimple battleCameraIsSimple_decl
#include "game/battle/battle.h"
#undef battleCameraIsSimple
#include "game/gs_floor.h"
#include "game/gs_thread.h"


#if !defined(BATTLE_MAIN_EXACT_801EF02C_ONLY) && \
    !defined(BATTLE_MAIN_RESIDUAL_801EF4B0_ONLY) && \
    !defined(BATTLE_MAIN_EXACT_801EF5C0_ONLY) && \
    !defined(BATTLE_MAIN_RESIDUAL_801EFA08_ONLY) && \
    !defined(BATTLE_MAIN_EXACT_801EFFC4_ONLY)
#define BATTLE_MAIN_ALL
#endif

/* =========================================================================
 * External function declarations (auto-generated names)
 * ========================================================================= */

/* CRT */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* OS / Engine */
extern u8   fn_800FF548(void);                      /* check if floor is loaded */
extern void std092_getCurrentRoomID(void);                      /* tick floor */
extern void _threadSwitch(void);                      /* tick render */
extern void GSgetFrameStep(void);                      /* GSgfx tick */
extern void GSlogWrite(const char* fmt, ...);      /* GSlog_Print */

/* Scene management */
extern void menuCloseCustom(s32 objID, s32 arg1, s32 arg2);   /* release scene object */
extern u8   menuIsCheck(s32 objID);                        /* check scene object active */
extern void menuGetKeyInfo(void* padData, s32 port);          /* read pad input */
extern void* fn_80113FB4(void);                            /* check floor loaded */

/* Sound */
extern void soundStop(s32 sndID, s32 volume);    /* soundStop */
extern void std105_soundPlayEffect(s32 sndID, s32 fadeTime, s32 volume); /* sndPlay with fade */
extern void std104_soundSetBackgroundMusic(s32 mode, s32 sndID, s32 volume);     /* sndFade */

/* People / NPC system */
extern u8   GSscene_GetMode(void);                      /* save people state */
extern void GSscene_SetMode(u8 savedState);             /* restore people state */
extern void cameraUpdate(void);                      /* clear people state */

/* VSync management */
extern u8   dbgMenuGetEnable(void);                      /* save VSync mode */
extern void dbgMenuSetEnable(u8 mode);                   /* set VSync mode */
extern void dbgMenuClose(void);                      /* disable VSync */
extern void dbgMenuMain(s32 mode);                  /* force VSync */

/* Battle subsystems */
extern u8   battleCameraIsSimple(void);                    /* battle grid tick 1 */
extern void battleCameraDoSimple(void);                      /* battle grid tick 2 */
extern void battleCameraStartRandom(void);                      /* battle grid cleanup */
extern void fn_801C2F00(void* data, u32 size);      /* battle grid load data */
extern void fn_801C31EC(void);                      /* battle grid release */
extern void fadeCheck(s32 flag);                  /* battle grid set flag */
extern void fadeSet(s32 mode);                  /* battle camera init */

/* Battle scene */
extern void wazaSequenceSysRelease(void);                      /* waza system cleanup */
extern void fn_801DAEF8(s32 count);                 /* waza system init */
extern void fn_801DB088(void);                      /* waza system reset */

/* Battle init / render */
extern void fightActionCreateAndFlowFifo(void* sceneCtx, s32 arg1, s32 arg2,
                         s32 passType, s32 passIdx, void* tbl); /* scene pass config */
extern void fightActionDispFifoAll(void);                      /* scene pass finalize */
extern void fightActionFifoInit(void);                      /* battle scene init */
extern u8   fightFloorIsUseFightTimerAll(s32 arg);                   /* check scene state */

/* Battle state machine */
extern void fightActionFlowKaisiPreSubLoad(void);                      /* battle system init */

/* GBA link */
extern void fightTimerAllInit(void);
extern void fightTimerAllStart(void);

/* Sound check */
extern s32 fightFloorGetStatus(s32 a, s32 b, s32 c, s32 d); /* check/setup sound */

/* Battle Pokemon-slot helpers */
extern void* fightFloorGetValidFightSidePtr(s32 a, u16 b);                    /* get battle slot ptr */
extern void* fightSideGetValidFightTrainerPtr(void* a, u16 b);                  /* get slot member ptr */
extern void* fightTrainerGetStatus(void* a, s32 b, s32 c, s32 d);   /* get checked handle */
extern void  fn_801DA4E8(void* a, void* b);                /* dispatch event */
extern void* fightTrainerGetValidFightOutPokemonPtr(void* a, u16 b);                  /* get item ptr */
extern void* pokemonGetStatus(void* a, s32 b, s32 c, s32 d);   /* check item handle */

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
extern u32 lbl_80279B78[3]; /* function pointers for scene callbacks */

#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_RESIDUAL_801EF4B0_ONLY)

void _fightInitialize__FUi14FloorEnterMode(u32 unused, s32 enterMode)
{
    extern u8 fn_800FF548(void);
    extern void* floorGetCurrentContext(void);
    extern void fn_801EFA08(void);
    extern void* GSthreadCreate(s32 priority, void* userData, u32 stackSize,
                                s32 arg3, s32 arg4, void (*entry)(void));
    extern u8 lbl_8046D730[];
    extern u8 lbl_80375CC8[];
    extern const char lbl_80279C00[];

    if (fn_800FF548() != 0) {
        dbgMenuClose();
        dbgMenuSetEnable(0);
        return;
    }

    GSlogWrite(lbl_80279C00);
    lbl_8047B5DA = 1;
    lbl_8047B5D5 = dbgMenuGetEnable();
    dbgMenuClose();
    dbgMenuSetEnable(0);
    fightActionFifoInit();
    fn_801DAEF8(10);
    lbl_8047B5D4 = GSscene_GetMode();
    fightActionCreateAndFlowFifo(lbl_8046D730, 0, 0, 1, 0, lbl_80375CC8);
    fightActionCreateAndFlowFifo(lbl_8046D730, 0, 0, 3, 5, lbl_80375CC8);
    fightActionCreateAndFlowFifo(lbl_8046D730, 0, 0, 3, 4, lbl_80375CC8);
    fightActionDispFifoAll();
    lbl_8047B5D0 = (u32)GSthreadCreate(0x14, floorGetCurrentContext(), 0x4000, 1, 0,
                                      fn_801EFA08);
}

#endif

/* =========================================================================
 * Implementation
 *
 * battle_FightEnd (_fightFinalize__FUi14FloorEnterMode, 0x801EF374) is
 * recovered below. battle_FightStart (_fightInitialize__FUi14FloorEnterMode,
 * 0x801EF4B0) and battle_MainLoop (fn_801EFA08's outline) remain incomplete;
 * see the file header for why the fictional bodies previously here were removed.
 * ========================================================================= */

/* Forward declarations for converted functions */
void fn_801EFA08(void);

/* #######################################################################
 * Shared battle core / fight flow source (0x801EF02C - 0x801F0058).
 * ####################################################################### */

#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_EXACT_801EF02C_ONLY)

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
void fn_801EF128(void* partyData) {
    typedef struct BattlePartySlot {
        u8 active;
        u8 pad01;
        u16 id;
        u32 value04;
        u32 value08;
    } BattlePartySlot;
    typedef struct BattlePartyMeta {
        u16 id;
        u8 pad02[10];
        u8 active;
        u8 flag;
        u8 index;
        u8 pad0F;
        u16 field10;
        u8 pad12[2];
        s16 field14;
        u16 field16;
    } BattlePartyMeta;
    typedef struct BattlePartyData {
        u32 count;
        BattlePartySlot slots[97];
        BattlePartyMeta metadata[97];
    } BattlePartyData;
    extern void* savedataGetStatus(u32, u32);
    extern u16 fn_801EEF08(u16);
    extern u16 fn_801EEFF4(u16);
    BattlePartyData* data = partyData;
    s32 i;

    if (partyData == NULL) {
        data = savedataGetStatus(0, 0xF);
    }
    data->count = 0;
    for (i = 0; i < 97; i++) {
        BattlePartySlot* slot = &data->slots[i];
        BattlePartyMeta* meta = &data->metadata[i];

        slot->active = 0;
        slot->id = 0;
        slot->value04 = 0;
        slot->value08 = 0;
        meta->active = 0;
        meta->id = 0;
        meta->field14 = -1;
        meta->field16 = fn_801EEF08((u16)i);
        meta->flag = 0;
        meta->field10 = fn_801EEFF4((u16)i);
        meta->index = i;
    }
}

/* 0x801EF1E4 | size: 0x30 | small */
/* Ensure battle data pointer is valid; if NULL, get default via savedataGetStatus. */
void* fn_801EF1E4(void* data) {
    extern void* savedataGetStatus(u32 a, u32 b);
    if (data == NULL) {
        data = savedataGetStatus(0, 0xF);
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
    extern u8 menuIsCheck(s32 objID);
    extern void menuCloseCustom(s32 objID, s32 arg1, s32 arg2);
    typedef struct BattleSceneIdList {
        u32 ids[21];
    } BattleSceneIdList;
    BattleSceneIdList ids;
    s32 objID;
    u8 i;

    ids = *(const BattleSceneIdList*)lbl_80279B84;
    for (i = 0; i < 21; i++) {
        objID = ids.ids[i];
        if (menuIsCheck(objID) == 1) {
            menuCloseCustom(objID, 0, 0);
        }
    }
}

/* 0x801EF374 | size: 0x114 */
void _fightFinalize__FUi14FloorEnterMode(u32 floor, u32 enterMode) {
    typedef struct BattleSceneIdList {
        u32 ids[BATTLE_SCENE_OBJ_COUNT];
    } BattleSceneIdList;
    BattleSceneIdList ids;
    u8 i;
    s32 objID;
    u32 soundID;

    GSthreadTerminateGroup(floorGetCurrentContext());
    GSthreadTerminateGroup(floorGetCurrentContext());

    ids = *(const BattleSceneIdList*)lbl_80279B84;
    for (i = 0; i < BATTLE_SCENE_OBJ_COUNT; i++) {
        objID = ids.ids[i];
        if (menuIsCheck(objID) == 1) {
            menuCloseCustom(objID, 0, 0);
        }
    }

    GSscene_SetMode(lbl_8047B5D4);
    fn_801C31EC();
    wazaSequenceSysRelease();

    soundID = fightFloorGetStatus(0, 0, 0x12, 0);
    if (soundID != 0) {
        soundStop(soundID, 0);
    }
    std104_soundSetBackgroundMusic(1, 0, 0xFF);
    dbgMenuSetEnable(lbl_8047B5D5);
    lbl_8047B5DA = 0;
    GSlogWrite(lbl_80279BD8);
}

/* 0x801EF488 | size: 0x28 | small */
void fn_801EF488(void) {
    cameraUpdate();
    battleCameraStartRandom();
    fn_801DB088();
}

#endif

#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_EXACT_801EF5C0_ONLY)

/* 0x801EF5C0 | size: 0x5C | small */
void fn_801EF5C0(void) {
    extern void fn_800FF4D4(void* data, u8 type);
    u32 data[3];

    data[0] = lbl_80279B78[0];
    data[1] = lbl_80279B78[1];
    data[2] = lbl_80279B78[2];
    lbl_8047B5DA = 0;
    lbl_8047B5D8 = 0;
    lbl_8047B5D6 = 0;
    lbl_8047B5D0 = 0;
    fn_800FF4D4(data, 2);
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 5 functions matched
 * =================================================================== */

/* Address: 0x801EF61C | Size: 0x8 | Pattern: sda_setter */
void fightSetEncountId(u16 val) {
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
u16 std103_battleGetResult(void) {
    return lbl_8047B5D8;
}

/* Address: 0x801EF63C | Size: 0x8 | Pattern: sda_getter */
u8 fightIsBattleActive(void) {
    return lbl_8047B5DA;
}

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
    extern u32 fightEncountGetEnvSndDataId(u16 encountDataIndex);
    extern void std105_soundPlayEffect(u32 r3, u32 r4, u32 r5);
    u16 encountDataIndex;
    u32 r3;

    if (lbl_8047B5E1 == 1) return;
    if (a == 0) return;
    encountDataIndex = fn_801EF624();
    if (encountDataIndex == 0) return;
    r3 = fightEncountGetEnvSndDataId(encountDataIndex);
    if (r3 != 0) {
        std105_soundPlayEffect(r3, 0x3e8, 0xff);
    }
    lbl_8047B5E1 = 1;
    return;
}

/* 0x801EF758 | size: 0x5C | small */
void fn_801EF758(u8 a) {
    extern u8 lbl_8047B5E0;
    extern u16 fn_801EF624(void);
    extern u32 fightEncountGetBgmSndDataId(u16 encountDataIndex);
    extern void std104_soundSetBackgroundMusic(u32 r3, u32 r4, u32 r5);
    u16 encountDataIndex;
    u32 r3;

    if (lbl_8047B5E0 == 1) return;
    if (a == 0) return;
    encountDataIndex = fn_801EF624();
    if (encountDataIndex == 0) return;
    r3 = fightEncountGetBgmSndDataId(encountDataIndex);
    if (r3 != 0) {
        std104_soundSetBackgroundMusic(r3, 0x0, 0xff);
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

    fightFloorGetStatus(0, 0, 0x14, 0);
    r27 = fightFloorGetStatus(0, 0, 0x16, 0);
    r26 = fightFloorGetStatus(0, 0, 0x18, 0);

    for (r28 = 0; (u16)r28 < 2; r28++) {
        r31 = fightFloorGetValidFightSidePtr(0, r28);
        if (r31 == 0) continue;

        for (r30 = 0; (u16)r30 < r27; r30++) {
            r25 = fightSideGetValidFightTrainerPtr(r31, r30);
            if (r25 == 0) continue;

            _h = fightTrainerGetStatus(r25, 0, 0x4c, 0);
            if (_h == 0) continue;
            fn_801DA4E8(_h, arg);

            for (r29 = 0; (u16)r29 < r26; r29++) {
                _h = fightTrainerGetValidFightOutPokemonPtr(r25, r29);
                if (_h == 0) continue;
                _h = pokemonGetStatus(_h, 0, 0xee, 0);
                if (_h == 0) continue;
                fn_801DA4E8(_h, arg);
            }
        }
    }
}

#endif

/* fn_801EF8F4 and fn_801EF95C are auto-inlined into fn_801EFA08 (the next
 * residual island); its wrapper compiles them as static inline so that
 * unit's object holds fn_801EFA08 only. */
#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_EXACT_801EF5C0_ONLY) || \
    defined(BATTLE_MAIN_RESIDUAL_801EFA08_ONLY)
#if defined(BATTLE_MAIN_RESIDUAL_801EFA08_ONLY)
#define BATTLE_MAIN_HELPER static inline
#else
#define BATTLE_MAIN_HELPER
#endif

/* 0x801EF8F4 | size: 0x68 | small */
BATTLE_MAIN_HELPER u8 fn_801EF8F4(u8 useSimpleCamera) {
    u8 previousMode;
    void* data;

    previousMode = battleCameraIsSimple();
    if (useSimpleCamera == 1) {
        battleCameraDoSimple();
    } else {
        battleCameraDoFull();
    }
    std092_getCurrentRoomID();
    data = fn_80113FB4();
    if (data != NULL) {
        fn_801C2F00(data, 0x7B1800);
    }
    return previousMode;
}

/* 0x801EF95C | size: 0xAC | medium */
BATTLE_MAIN_HELPER void fn_801EF95C(void) {
    extern u8 dbgMenuGetEnable(void);
    extern void dbgMenuSetEnable(u8 mode);
    extern u8 fn_8000816C(void);
    extern void dbgMenuMain(s32 mode);
    extern void menuGetKeyInfo(void* padData, s32 port);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    u8 ticks;
    u8 mode;
    u8 input[0x1C];

    mode = dbgMenuGetEnable();
    dbgMenuSetEnable(1);
    if (fn_8000816C() == 1) {
        dbgMenuMain(1);
    } else {
        ticks = 0;
        while (ticks < 4) {
            menuGetKeyInfo(input, 1);
            if (!(*(u16*)input & 0x800)) {
                _threadSwitch();
                ticks += GSgetFrameStep();
            } else {
                break;
            }
        }
        if (ticks < 4) {
            dbgMenuMain(1);
        }
    }
    dbgMenuSetEnable(mode);
}

#endif

#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_RESIDUAL_801EFA08_ONLY)

/* 0x801EFA08 | size: 0x5BC | battle main loop (the battle thread entry). */
void fn_801EFA08(void) {
    extern u8 lbl_80375CC8[];
    extern u8 lbl_80375D30[];
    extern u8 lbl_80375D80[];
    extern u8 lbl_8046D760[];
    extern f32 lbl_8047E508;
    extern void fadeSet(s32 mode, f32 time);
    extern u32 dbgMenuFightGetMsgSpeedToFrame(u32 frames);
    extern void fn_800FF660(void);
    extern void floorSetFadeScript(s32 a, s32 b);
    extern void std138_soundPlayEffectEx(s32 a, s32 b, s32 c, s32 d);
    extern void fn_801EF2D4(void);
    extern u16 std103_battleGetResult(void);
    extern void fightFloorSetTimeOutAllFightResult(s32 arg);
    extern u8 fightFloorIsUseFightTimerCommand(s32 arg);
    extern void fightMenuCloseInfoMenu(s32 arg);
    extern void fightMenuOpenInfoMenu(s32 arg);
    extern void fightTimerCommandTerminate(void);
    extern void fightTimerCommandBlock(void);
    extern void fightTimerCommandStart(void);
    extern void fightTimerCommandInit(void);
    extern u8 fightTimerAllIsOver(void);
    extern void fightTimerAllBlock(void);
    extern void fightTimerAllTerminate(void);
    extern u32 GSgetFrameStep(void);
    u32 status;
    u32 frames;
    u32 elapsed;

    status = fightFloorGetStatus(0, 0, 0x12, 0);
    if (status != 0) {
        std105_soundPlayEffect(status, 1000, 0xFF);
    }
    fightActionFlowKaisiPreSubLoad();
    fadeSet(8, lbl_8047E508);
    fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 2, 0, lbl_80375CC8);
    fn_801EF8F4(1);
    fadeCheck(1);
    fn_801EF95C();

    if (std103_battleGetResult() == 0) {
        fn_801EF8F4(1);
        fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 4, 5, lbl_80375CC8);
        fn_801EF8F4(1);
        fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 4, 4, lbl_80375CC8);
        fn_801EF8F4(1);
        fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 5, 0, lbl_80375CC8);
        fightActionDispFifoAll();
        if (fightFloorIsUseFightTimerAll(0) == 1) {
            fightTimerAllInit();
            fightTimerAllStart();
        }

        while (std103_battleGetResult() == 0) {
            fn_801EF95C();
            if (std103_battleGetResult() != 0) {
                break;
            }
            if (fightFloorIsUseFightTimerAll(0) == 1 && fightTimerAllIsOver() == 1) {
                fn_801EF8F4(1);
                fightMenuOpenInfoMenu(0);
                fightFloorSetTimeOutAllFightResult(0);
                frames = dbgMenuFightGetMsgSpeedToFrame(0xB4);
                if (frames != 0) {
                    elapsed = 0;
                    while (elapsed < frames) {
                        _threadSwitch();
                        elapsed += GSgetFrameStep();
                    }
                }
                break;
            }
            if (fightFloorIsUseFightTimerCommand(0) == 1) {
                fightTimerCommandInit();
                fightTimerCommandStart();
            }
            fn_801EF8F4(0);
            fightMenuOpenInfoMenu(-1);
            fightActionDispFifoAll();
            fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 7, 0, lbl_80375D30);
            fightActionDispFifoAll();
            fightMenuCloseInfoMenu(0);
            battleCameraDoSimple();
            if (fightFloorIsUseFightTimerCommand(0) == 1) {
                fightTimerCommandBlock();
                fightTimerCommandTerminate();
            }
            if (std103_battleGetResult() != 0) {
                break;
            }
            fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0xB, 0, lbl_80375D30);
            if (std103_battleGetResult() != 0) {
                break;
            }
            fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0xD, 0, lbl_80375D30);
            if (std103_battleGetResult() != 0) {
                break;
            }
        }
        fightActionDispFifoAll();
    }

    fn_801EF8F4(1);
    fn_801EF2D4();
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        fightTimerAllBlock();
        fightTimerAllTerminate();
    }
    fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0xE, 0, lbl_80375D80);
    fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0xF, std103_battleGetResult(), lbl_80375D80);
    fightActionDispFifoAll();
    status = fightFloorGetStatus(0, 0, 0x12, 0);
    if (status != 0) {
        soundStop(status, 1000);
    }
    status = fightFloorGetStatus(0, 0, 0x12, 0);
    if (status == 0) {
        std138_soundPlayEffectEx(1, 1000, 1000, 0xFF);
    }
    fadeSet(3, lbl_8047E508);
    fadeCheck(1);
    fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0x10, 0, lbl_80375D80);
    fightActionCreateAndFlowFifo(lbl_8046D760, 0, 0, 0x11, 0, lbl_80375D80);
    fn_800FF660();
    floorSetFadeScript(0, 0);
}
#endif

#if defined(BATTLE_MAIN_ALL) || defined(BATTLE_MAIN_EXACT_801EFFC4_ONLY)

/* 0x801EFFC4 | size: 0x48 | small */
void fn_801EFFC4(u32 count) {
    extern u32 GSgetFrameStep(void);
    u32 i;
    if (count != 0) {
        i = 0;
        while (i < count) {
            _threadSwitch();
            i += GSgetFrameStep();
        }
    }
}

/* 0x801F000C | size: 0x4C | small */
void fightMainWaitFrame(void) {
    extern u32 dbgMenuFightGetMsgSpeedToFrame(void);
    extern u32 GSgetFrameStep(void);
    u32 total;
    u32 elapsed;
    total = dbgMenuFightGetMsgSpeedToFrame();
    if (total != 0) {
        elapsed = 0;
        while (elapsed < total) {
            _threadSwitch();
            elapsed += GSgetFrameStep();
        }
    }
}

#endif
