/**
 * @file fight_gsfloor.c
 * @brief game/pxdvs/app/fight/fightGSfloor.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x802614B4-0x80261B68, 7 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightGSfloor.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);


/* Address: 0x802614B4 | Size: 0x88 | Ghidra import */
int fightGSfloorGetPushDataSize(void)

{
    typedef struct BattleScanContext {
        u8 collectEntries;
        u8 consumeEntries;
        u16 count;
        u8 *entries;
        u8 *nextEntry;
    } BattleScanContext;
    typedef u32 (*BattleScanCallback)(u32, u32, char *);
    extern void fightFloorLoopValidFightTrainer(u32, BattleScanCallback, void *, u32);
    extern void fightFloorLoopValidFightOutPokemon(u32, BattleScanCallback, void *, u32);
    extern u32 _fightGSfloorPokemonCB__FPvUsPv(u32, u32, char *);
    extern u32 _fightGSfloorTrainerCB__FPvUsPv(u32, u32, char *);
    BattleScanContext scan;
    u16 firstCount;
    int total;

    scan.collectEntries = 0;
    scan.consumeEntries = 0;
    scan.count = 0;
    scan.nextEntry = 0;
    fightFloorLoopValidFightTrainer(0, _fightGSfloorTrainerCB__FPvUsPv, &scan, 0);
    firstCount = scan.count;
    scan.count = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightGSfloorPokemonCB__FPvUsPv, &scan, 0);
    total = firstCount * 0x78;
    total += scan.count * 0x7C;
    return total + 0x48;
}

/* Address: 0x8026153C | Size: 0xB8 | Ghidra import */
void fightGSfloorPushData(void *rawOut)
{
    typedef struct BattleScanOutput {
        u16 firstCount;
        u16 secondCount;
        u8 entries[0x44];
    } BattleScanOutput;
    typedef struct BattleScanContext {
        u8 collectEntries;
        u8 consumeEntries;
        u16 count;
        u8 *entries;
        u8 *nextEntry;
    } BattleScanContext;
    typedef u32 (*BattleScanCallback)(u32, u32, char *);
    extern void *battleGridGetPtr(void);
    extern void fightFloorLoopValidFightTrainer(u32, BattleScanCallback, void *, u32);
    extern void fightFloorLoopValidFightOutPokemon(u32, BattleScanCallback, void *, u32);
    extern u32 _fightGSfloorPokemonCB__FPvUsPv(u32, u32, char *);
    extern u32 _fightGSfloorTrainerCB__FPvUsPv(u32, u32, char *);
    BattleScanOutput *out;
    BattleScanContext scan;
    u16 firstCount;
    u16 secondCount;
    u8 *entries;

    out = rawOut;
    entries = out->entries;
    memcpy(entries, battleGridGetPtr(), 0x44);
    scan.collectEntries = 1;
    scan.consumeEntries = 1;
    scan.count = 0;
    scan.entries = entries;
    scan.nextEntry = entries + 0x44;
    fightFloorLoopValidFightTrainer(0, _fightGSfloorTrainerCB__FPvUsPv, &scan, 0);
    firstCount = scan.count;
    scan.count = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightGSfloorPokemonCB__FPvUsPv, &scan, 0);
    secondCount = scan.count;
    out->firstCount = firstCount;
    out->secondCount = secondCount;
    battleCameraSelect(1);
    battleGridReset();
    wazaSequenceSysRelease();
}

typedef struct BattleReplayHeader {
    u16 firstCount;
    u16 secondCount;
    u8 entries[0x44];
} BattleReplayHeader;

typedef struct BattleReplayFirstEntry {
    u32 target;
    u16 modelId;
    u8 attr;
    u8 pad7;
    u32 *modelOut;
    u8 state[0x6C];
} BattleReplayFirstEntry;

typedef struct BattleReplaySecondEntry {
    u32 target;
    u16 modelId;
    u8 attr;
    u8 variant;
    u32 arg;
    u32 *modelOut;
    u8 state[0x6C];
} BattleReplaySecondEntry;

/* Address: 0x802615F4 | Size: 0x114 | Ghidra import */
void fightGSfloorPopData(BattleReplayHeader *header)
{
    extern u32 GSmodelGetVisibility();
    extern void GSmodelPopState();
    extern int battleGridUpdate();
    BattleReplaySecondEntry *secondEntry;
    extern int wazaSequenceSysInit(int);
    extern u32 wazaSequenceTrainerCreate(u16);
    u16 secondCount;
    extern int fightFloorSetShadow();
    u8 *entries;
    u16 firstCount;
    BattleReplayFirstEntry *firstEntry;
    u32 model;
    u32 state;

    entries = header->entries;
    wazaSequenceSysInit(10);
    firstCount = header->firstCount;
    firstEntry = (BattleReplayFirstEntry *)(entries + 0x44);
    secondCount = header->secondCount;
    while (firstCount-- != 0) {
        model = wazaSequenceTrainerCreate(firstEntry->modelId);
        *(u32 *)(firstEntry->target + 0x27C0) = model;
        *firstEntry->modelOut = model;
        state = wazaSequenceOwnerGetModel(model);
        GSmodelPopState(state, firstEntry->state);
        fn_801DA224(model, firstEntry->attr);
        firstEntry++;
    }
    secondEntry = (BattleReplaySecondEntry *)firstEntry;
    while (secondCount-- != 0) {
        model = fn_801DE190(secondEntry->modelId, secondEntry->arg, secondEntry->variant);
        *(u32 *)(secondEntry->target + 0x600) = model;
        *secondEntry->modelOut = model;
        state = wazaSequenceOwnerGetModel(model);
        GSmodelPopState(state, secondEntry->state);
        fn_801DA224(model, secondEntry->attr);
        state = GSmodelGetVisibility(state);
        wazaSequenceOwnerSetVisibility(model, state);
        secondEntry++;
    }
    memcpy(battleGridGetPtr(), entries, 0x44);
    battleCameraSelect(1);
    battleGridUpdate();
    fightFloorSetShadow();
}

/*
 * One row of the battle grid (battleGridGetPtr, 0x44 bytes: four rows and
 * two counts; battle_grid.c's BattleGridGroupEntry): the trainer's sequence
 * and its two Pokemon sequences.
 */
typedef struct FightGSfloorGridRow {
    u32 trainer;
    u32 pokemon[2];
    u32 info;
} FightGSfloorGridRow;

/*
 * Pokemon XD's _fightGSfloorFindBGSeqPtr__FP10battleGridUlb (GXXE01
 * 0x80236984, size 0x80, scope:local; TeamOrre/xd-decomp symbols.txt at
 * 4989794e, body in trevor403/xd-asm b1087f18 code/func_FUN_80236984.s):
 * the address of the grid word that holds a sequence, trainer slots when
 * the flag is set, Pokemon slots otherwise. It makes no calls. XD calls it
 * from both _fightGSfloorPokemonCB and _fightGSfloorTrainerCB, right
 * between the sequence queries and Sequence_GetGSmodel, as here; Colosseum
 * expands it in both, and each expansion keeps its NULL-sequence guard even
 * though the callback has already returned for a NULL sequence (retail
 * 0x80261790 and 0x802618BC). Admitted under the same-engine sister-title
 * clause of docs/CAMPAIGN_OPERATIONS.md. With it both callbacks are exact
 * in natural form (the pokemon callback was 95.6% written in place) and
 * the unit needs none of its former peephole/optimize_for_size pragmas.
 */
static inline u32* fightGSfloorFindBGSeqPtr(FightGSfloorGridRow* grid, u32 seq, u8 isTrainer)
{
    FightGSfloorGridRow* row;
    s32 i;
    s32 j;

    if (seq == 0) {
        return NULL;
    }
    for (i = 0; i < 4; i++) {
        row = &grid[i];
        if (isTrainer == 1) {
            if (row->trainer == seq) {
                return &row->trainer;
            }
        } else {
            for (j = 0; j < 2; j++) {
                if (row->pokemon[j] == seq) {
                    return &row->pokemon[j];
                }
            }
        }
    }
    return NULL;
}

/* Address: 0x80261708 | Size: 0x144 */
u32 _fightGSfloorPokemonCB__FPvUsPv(u32 r3,u32 r4,char *r5)
{
    extern int GSmodelPushState();
    extern u8 wazaSequenceOwnerGetFlag77();
    extern u8 wazaSequenceOwnerGetFlags();
    extern u16 wazaSequenceOwnerGetId();
    extern int wazaSequenceOwnerFree();
    extern u32 wazaSequenceOwnerGet78();
    u32 *entry;
    u32 seq;
    u16 id;

    seq = (u32)pokemonGetStatus(r3, 0, 0xee, 0);
    if (seq == 0) {
        return 1;
    }
    id = wazaSequenceOwnerGetId();
    if (id == 0) {
        return 1;
    }
    if (*r5 != '\0') {
        entry = *(u32 **)(r5 + 8);
        *entry = r3;
        *(short *)(entry + 1) = id;
        *(u8 *)((int)entry + 6) = wazaSequenceOwnerGetFlags(seq);
        *(u8 *)((int)entry + 7) = wazaSequenceOwnerGetFlag77(seq);
        entry[2] = wazaSequenceOwnerGet78(seq);
        entry[3] = (u32)fightGSfloorFindBGSeqPtr(*(FightGSfloorGridRow **)(r5 + 4), seq, 0);
        GSmodelPushState(wazaSequenceOwnerGetModel(seq), entry + 4);
        *(int *)(r5 + 8) = *(int *)(r5 + 8) + 0x7c;
    }
    if (r5[1] != '\0') {
        wazaSequenceOwnerFree(seq);
    }
    *(u16 *)(r5 + 2) = *(u16 *)(r5 + 2) + 1;
    return 1;
}

/* Address: 0x8026184C | Size: 0x108 */
u32 _fightGSfloorTrainerCB__FPvUsPv(u32 r3,u32 r4,char *r5)
{
    extern int GSmodelPushState();
    extern u8 wazaSequenceOwnerGetFlags();
    extern u16 wazaSequenceOwnerGetId();
    extern int wazaSequenceOwnerFree();
    u32 *entry;
    u32 seq;
    u16 id;

    seq = fightTrainerGetStatus(r3, 0, 0x4c, 0);
    if (seq == 0) {
        return 1;
    }
    id = wazaSequenceOwnerGetId();
    if (id == 0) {
        return 1;
    }
    if (*r5 != '\0') {
        entry = *(u32 **)(r5 + 8);
        *entry = r3;
        *(short *)(entry + 1) = id;
        *(u8 *)((int)entry + 6) = wazaSequenceOwnerGetFlags(seq);
        entry[2] = (u32)fightGSfloorFindBGSeqPtr(*(FightGSfloorGridRow **)(r5 + 4), seq, 1);
        GSmodelPushState(wazaSequenceOwnerGetModel(seq), entry + 3);
        *(int *)(r5 + 8) = *(int *)(r5 + 8) + 0x78;
    }
    if (r5[1] != '\0') {
        wazaSequenceOwnerFree(seq);
    }
    *(u16 *)(r5 + 2) = *(u16 *)(r5 + 2) + 1;
    return 1;
}

/* Address: 0x80261954 | Size: 0x17C | Ghidra import */
void fightMenuCloseInfoMenu(u32 wait)
{
    extern void fightFloorLoopValidFightTrainer();
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightFloorIsUseFightTimerAll(u32);
    extern void menuFightCloseCountDown(void);
    extern void menuFightCloseTotalTimer(void);
    extern u8 menuFightCloseCheckCountDown(void);
    extern u8 menuFightCloseCheckTotalTimer(void);
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv();
    u8 done;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
    if (fightFloorIsUseFightTimerCommand(0) == 1) {
        menuFightCloseCountDown();
    }
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        menuFightCloseTotalTimer();
    }
    if ((u8)wait == 1) {
        fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
        do {
            done = 1;
            fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv, &done, 0);
            if (done == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
        fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
        do {
            if ((u8)fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv, 0, 0) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
        if (fightFloorIsUseFightTimerCommand(0) == 1) {
            do {
                if (menuFightCloseCheckCountDown() == 0) {
                    break;
                }
                _threadSwitch();
            } while (1);
        }
        if (fightFloorIsUseFightTimerAll(0) == 1) {
            do {
                if (menuFightCloseCheckTotalTimer() == 0) {
                    break;
                }
                _threadSwitch();
            } while (1);
        }
    }
}

/* Address: 0x80261AD0 | Size: 0x98 | Ghidra import */
void fightMenuOpenInfoMenu(s8 timerMode)
{
    extern void fightFloorLoopValidFightTrainer();
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightFloorIsUseFightTimerAll(u32);
    extern void menuFightOpenCountDown(void);
    extern void menuFightOpenTotalTimer(void);
    extern u32 _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv();
    u8 openStatus;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv, 0, 0);
    openStatus = 1;
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv, &openStatus, 0);
    if (timerMode < 0) {
        if (fightFloorIsUseFightTimerCommand(0) == 1) {
            menuFightOpenCountDown();
        }
    }
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        menuFightOpenTotalTimer();
    }
}
