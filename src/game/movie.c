/**
 * @file movie.c
 * @brief Boot sequence and THP movie scenes (0x80035DD4 - 0x80037158).
 *
 * One retail translation unit, built with GC/1.3.2 -O4,p -opt nopeephole
 * -inline deferred -rostr. It owns .text 0x80035DD4-0x80037158, .rodata
 * 0x80266FE8-0x80267060 (the movie paths and the read-error message),
 * .data 0x802E50E0-0x802E51C8 (the two cue tables), .bss
 * 0x803A3E58-0x803A6498 (the loader and wave-reader threads) and .sdata2
 * 0x8047BA30-0x8047BA58 (its literal pool).
 *
 * -inline deferred makes MWCC generate and emit the functions in reverse
 * definition order, so they are defined here from the highest address
 * down; the string literals and the .sdata2 pool follow that generation
 * order, the static data and .bss follow the definition order.
 *
 * The movie scenes come in (start, play, finish) callback triples, laid
 * out finish/play/start. Nothing in main.dol references them, so the link
 * keeps them with FORCEACTIVE (configure.py).
 *   openingdemo  fn_80035EE4 / fn_80035E04 / fn_80035DD4
 *   staffroll    fn_800361C0 / fn_80035F64 / fn_80035F34
 *   autodemo01   fn_80036360 / fn_80036240 / fn_80036210
 *   (none)       fn_800363B8 / fn_800363B4 / fn_800363B0
 *   gs_logo      fn_800364C8 / fn_80036468 / fn_800363BC
 *   tpc          fn_80036640 / fn_800365E0 / fn_800365B0
 *   (none)       fn_800366A4 / fn_800366A0 / fn_8003669C
 */

#include "dolphin/types.h"
#include "game/movie.h"
#include "dolphin/os/OSThread.h"

/* Dolphin SDK OSReset.h: the reset code of a hot restart. */
#define OS_RESETCODE_RESTART 0x80000000

extern u32 OSGetResetCode(void);
extern u32 OSGetProgressiveMode(void);
extern u32 VIGetDTVStatus(void);
extern void* memset(void* dst, int val, u32 n);

/* ===== THP Player SDK functions ===== */
extern u8   GSmovieIsActive(void);                        /* THPPlayerGetState */
extern void GSmovieOpen(const char* path, u32 loop);  /* THPPlayerOpen */
extern s32  GSmovieGetFrame(void);                        /* THPPlayerGetFrame */
extern void GSmovieClose(void);

/* ===== GS Engine external functions ===== */
extern void _threadSwitch(void);                        /* GSthread yield / step */
extern void std104_soundSetBackgroundMusic(u32 sndId, u32 fade, u32 vol); /* sndPlay (BGM start) */
extern void std138_soundPlayEffectEx(u32, u32, u32, u32);
extern void soundStartSE(u32 soundId);
extern void fn_80165F40(void);
extern void fn_80165F84(void);
extern void fn_801655D4(u32 index);
extern void soundInit(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fadeSet(u32 mode, f32 speed);
extern void fadeCheck(u32 enable);
extern void std078_flagSetFalse(u32 flagId);                   /* GSflagSet */
extern u32  std081_flagCheckTrue(u32 flagId);                   /* GSflagGet */
extern void floorLink(u32 a, u32 b);
extern void floorSetFadeScript(u32 a, u32 b);
extern u32  floorGetPrevFloorID(void);
extern void floorRequestChange(u32 a);
extern void* savedataGetStatus(void* data, u32 kind);
extern void gamedatasaveSetStatus(void* data, u32 id, u32 value);
extern u32  gamedatasaveGetStatus(void* data, u32 id);
extern void heroSetStatus(u32 a, u32 b, u32 c);
extern u32  heroGetStatus(void* hero, u32 selector, u32 index);
extern void winMsgOpen(u32 a, u32 b, u32 c, u32 d);
extern void winMsgClose(u32 id);
extern s8   menuSubOpenYesNo(u8 menuType, s16 x, s16 y, s32 initialValue);
extern s32  fn_801D0748(u32 a, u32 b, u32 c);
extern s32  fn_8017B1AC(void);
extern u32  fn_800F7AF0(u32);
extern u32  fn_800F7BC4(u32);
extern void fn_800F7F64(u32);
extern u32  menuOpen(u32 sceneId, u32 arg);
extern void menuClose(u32 sceneId);
extern void fn_8017B370(u32 arg);
extern s32  fn_8017B2CC(u32 arg);
extern u32  GSgetFrameStep(void);                         /* frames since the last call */
extern s32  fn_800D37CC(void);                         /* frames per second */
extern void fn_800A0FC8(u32 mode);                     /* OSSetProgressiveMode */
extern void fn_800D37D4(u32 a, u32 b, u32 c, u32 d, u32 e, u16 size);
extern s32  _fsysInitTOC(u32 numSlots, u32 param2, u32 param3, u32 param4);
extern u32  GSgappCreate(s32 state, u8 priority, void* param, void* func);
extern void fn_8017AAA4(void);
extern void GSlogWrite(const char* fmt, ...);
extern u32  windowGetActiveID(void);
extern s32  menuOpenCustom(void* menuId, u32 parentId, s32* cursorOut,
                           s32 closeFlags, void* checkCursor, s32 openParam, ...);
extern void menuCloseCustom(u32 id, u32 a, u32 b);
extern void menuSetPosition(u32 id, u32 x, u32 y);
extern u8   windowCheckCursor(u32 id, u32 arg);
extern s32  windowGetValue(u32 id);

typedef struct WindowCursorWork {
    u8 pad[0x95];
    s8 cursor;
} WindowCursorWork;

extern WindowCursorWork* windowSearchID(u32 id);

/* Save data body copied whole by the staff roll (0x1DFD0 bytes). */
typedef struct SavedataBody {
    u8 data[0x1DFD0];
} SavedataBody;

extern SavedataBody* fn_801D036C(void);    /* allocate a save-data backup */
extern void* fn_801D0314(void* ptr);       /* free it */

/* Cross-thread flags. Volatile: the wave reader decrements lbl_8047A460
 * and the loader thread sets lbl_8047A464 while this thread yields, and
 * fn_800364C8 keeps the lbl_8047A468 store ahead of the lbl_8047A460
 * load, which MWCC only preserves for volatile. */
extern volatile s32 lbl_8047A460; /* outstanding wave-reader requests */
extern volatile s32 lbl_8047A464; /* set by the loader thread when ready */
extern volatile u8  lbl_8047A468;
extern s32 lbl_804788B8;          /* progressive-scan choice: -1 unset, 0 off, 1 on */

/* One subtitle/sound cue of a THP movie: fire up to two sounds at `frame`. */
typedef struct MovieCue {
    u16 frame;
    u16 sound0;
    u16 sound1;
} MovieCue;

/*
 * Loader (fn_8003708C) and menu-sound wave-reader threads, their 4 KiB
 * stacks, and the four-word request handed to the wave reader. Retail
 * addresses all five through one base register (offset 0 for the loader
 * thread). With -inline deferred MWCC lays them out in reverse declaration
 * order, which gives retail's order: loader thread, loader stack, request,
 * wave thread, wave stack.
 */
static u8 sWaveStack[0x1000];
static OSThread sWaveThread;
static u32 sWaveArgs[4];
static u8 sMainStack[0x1000];
static OSThread sMainThread;

/*
 * Advance a movie's cue list by at most one cue per frame and return the
 * next cue index. Retail expands this identically in fn_80035E04 (5 cues)
 * and fn_80036240 (33 cues); both expansions keep the inlined early
 * returns as a conditional branch around an unconditional jump to the
 * helper's exit, which the equivalent nested-if form does not produce.
 */
static inline u32 moviePlayCues(const MovieCue* cues, u32 count, u32 index)
{
    u32 next;
    s32 frame;

    next = index;
    if (index >= count) {
        return next;
    }
    frame = GSmovieGetFrame();
    if (frame < 0) {
        return next;
    }
    if (frame >= cues[index].frame) {
        if (cues[index].sound0 != 0) {
            soundStartSE(cues[index].sound0);
        }
        if (cues[index].sound1 != 0) {
            soundStartSE(cues[index].sound1);
        }
        next++;
    }
    return next;
}

/*
 * TRUE after a hot restart. fn_8003686C expands this five times, each
 * time materialising the result (li 1 / li 0) before testing it.
 */
static inline BOOL movieIsRestart(void)
{
    if (OSGetResetCode() == OS_RESETCODE_RESTART) {
        return TRUE;
    }
    return FALSE;
}

static void movieWait(void);
static s8 movieSelectProgressive(void);

/*
 * Progressive-scan prompt: ask, record the choice for fn_800366A8 and
 * confirm it (0x3B51 progressive / 0x3B52 interlaced). fn_8003686C
 * expands this twice, identically (with separate cursor slots on the
 * stack).
 */
static inline void movieAskProgressive(void)
{
    if (movieSelectProgressive() == 0) {
        /* After a hot restart the mode is kept; only switch if needed. */
        if (movieIsRestart() == TRUE) {
            if (OSGetProgressiveMode() == 0) {
                lbl_804788B8 = 1;
            }
        } else {
            lbl_804788B8 = 1;
        }
        winMsgOpen(1, 0x3B51, 1, 1);
        movieWait();
    } else {
        if (movieIsRestart() == TRUE) {
            if (OSGetProgressiveMode() == 1) {
                lbl_804788B8 = 0;
            }
        } else {
            lbl_804788B8 = 0;
        }
        winMsgOpen(1, 0x3B52, 1, 1);
        movieWait();
    }
    winMsgClose(1);
}

/* Menu-sound wave-reader thread: load up to four wave banks. */
void* _menuSoundReadWaveThread__FPv(void* param)
{
    u32* request = param;

    if (request[0] != 0) {
        fn_801655D4(request[0]);
    }
    if (request[1] != 0) {
        fn_801655D4(request[1]);
    }
    if (request[2] != 0) {
        fn_801655D4(request[2]);
    }
    if (request[3] != 0) {
        fn_801655D4(request[3]);
    }
    lbl_8047A460--;
    return NULL;
}

/* Loader thread: build the file TOC and start the main game app. */
void* fn_8003708C(void* param)
{
    _fsysInitTOC(0x40, 0, 0, 0);
    GSgappCreate(1, 0x14, 0, fn_8017AAA4);
    lbl_8047A464 = 1;
    return NULL;
}

/*
 * Boot loader: settle the progressive-scan choice, wait for the file TOC
 * load (fn_8017B2CC), then load the first menu-sound wave bank.
 */
void bootWaitLogo(void)
{
    s32 status;

    lbl_804788B8 = -1;

    if (!movieIsRestart()) {
        if (VIGetDTVStatus() == 1) {
            fn_800F7F64(1);
            if (fn_800F7BC4(1) & 0x200) {
                movieAskProgressive();
            } else if (OSGetProgressiveMode() == 1) {
                movieAskProgressive();
            } else {
                fadeCheck(1);
                movieWait();
            }
        } else {
            fadeCheck(1);
            movieWait();
            fn_800A0FC8(0);
        }
    } else {
        fadeCheck(1);
        movieWait();
    }

    for (;;) {
        status = fn_8017B2CC(0xA);
        if (status < 0) {
            GSlogWrite("読み出しエラー\n");
        }
        if (status == 0) {
            break;
        }
        _threadSwitch();
    }

    soundInit(2, 1, 0x20, 1, 0x10);
    fn_80165F84();

    lbl_8047A460++;
    sWaveArgs[0] = 1;
    sWaveArgs[1] = 0;
    sWaveArgs[2] = 0;
    sWaveArgs[3] = 0;
    OSCreateThread(&sWaveThread, _menuSoundReadWaveThread__FPv, sWaveArgs,
                   sWaveStack + 0xFFC, 0x1000, 0x10, 1);
    OSResumeThread(&sWaveThread);

    while (lbl_8047A460 != 0) {
        _threadSwitch();
    }
}

/* Boot: start the loader thread, run the boot loader, then fade in. */
void fn_800366A8(void)
{
    u16 fadeFrames;

    menuOpen(0x85, 0);
    fadeSet(2, 0.5f);

    if (lbl_8047A464 != 1) {
        OSCreateThread(&sMainThread, fn_8003708C, NULL,
                       sMainStack + 0xFFC, 0x1000, 0x10, 1);
        OSResumeThread(&sMainThread);
    }

    while (lbl_8047A464 == 0) {
        _threadSwitch();
    }

    fn_8017B370(0xA);
    memset((void*)0x80001801, 0, 0x17FF);
    bootWaitLogo();

    lbl_8047A460++;
    sWaveArgs[0] = 2;
    sWaveArgs[1] = 0;
    sWaveArgs[2] = 0;
    sWaveArgs[3] = 0;
    OSCreateThread(&sWaveThread, _menuSoundReadWaveThread__FPv, sWaveArgs,
                   sWaveStack + 0xFFC, 0x1000, 0x10, 1);
    OSResumeThread(&sWaveThread);

    fadeSet(3, 0.5f);
    fadeCheck(1);
    menuClose(0x85);

    if (lbl_804788B8 != -1) {
        /* Retail converts through __cvt_fp2unsigned before narrowing. */
        fadeFrames = (u32)(1.5f * (f32)fn_800D37CC());
        fn_800A0FC8(lbl_804788B8); /* OSSetProgressiveMode */
        /* Retail tests the mode it just passed on as unsigned (cmplwi). */
        if ((u32)lbl_804788B8 == 1) {
            fn_800D37D4(1, 2, 0, 2, 1, fadeFrames);
        } else {
            fn_800D37D4(1, 2, 0, 2, 0, fadeFrames);
        }
    }

    while (lbl_8047A460 != 0) {
        _threadSwitch();
    }
}

/*
 * movieSelectProgressive and movieWait are ordinary static helpers that
 * MWCC auto-inlines into fn_8003686C (twice and seven times) while still
 * emitting an unreferenced copy, which the linker dead-strips. The copies
 * are what order the literal pool: they are generated after fn_800366A4
 * and before fn_800366A8, so their 2.0f, int-to-float doubles and 10.0f
 * precede fn_800366A8's 0.5f and 1.5f, exactly as in retail
 * (0x8047BA30: 0, 2, s32 bias, u32 bias, 10, 0.5, 1.5).
 */

/*
 * Ask "progressive scan?" (message 0x3B50) with a ten-second timeout that
 * restarts whenever the cursor moves; on timeout take the highlighted
 * entry. Every exit sets `answer`: the break after reading the value, or
 * the timeout test.
 */
static s8 movieSelectProgressive(void)
{
    s32 cursorOut;
    f32 time;
    s8 lastCursor;
    s8 cursor;
    s8 answer;

    winMsgOpen(1, 0x3B50, 1, 1);
    cursorOut = 0;
    menuOpenCustom((void*)0x11, windowGetActiveID(), &cursorOut, 0, 0, 0);
    menuSetPosition(0x11, 0x2D, 0xBE);
    time = 0.0f;
    lastCursor = -1;
    while (time < 10.0f) {
        if (windowCheckCursor(0x11, 0) == 0) {
            answer = windowGetValue(0x11);
            break;
        }
        cursor = windowSearchID(0x11)->cursor;
        if (lastCursor != cursor) {
            time = 0.0f;
            lastCursor = cursor;
        }
        _threadSwitch();
        time += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
    if (time >= 10.0f) {
        answer = windowSearchID(0x11)->cursor;
    }
    menuCloseCustom(0x11, 0, 1);
    return answer;
}

/* Yield for two seconds of frame time (frames elapsed over frames/s). */
static void movieWait(void)
{
    f32 time;

    time = 0.0f;
    while (time < 2.0f) {
        _threadSwitch();
        time += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}

void fn_800366A4(void) {}

void fn_800366A0(void) {}

void fn_8003669C(void) {}

/* tpc: start */
void movieTpcStart(void)
{
    lbl_8047A468 = 0;
    fadeSet(2, 0.0f);
    fadeCheck(1);
    GSmovieOpen("movie/tpc.thp", 1);
    memset((void*)0x80001803, 0, 0x17FD);
}

/* tpc: play */
void movieTpcWait(void)
{
    if (lbl_8047A468 != 1) {
        fn_80165F40();
        while ((u8)GSmovieIsActive() == 1) {
            _threadSwitch();
        }
    }
    floorRequestChange(0x39A);
    floorSetFadeScript(0, 0);
    lbl_8047A468 = 1;
}

/* tpc: finish */
void fn_800365B0(void)
{
    fadeSet(3, 0.0f);
    fadeCheck(1);
}

/* gs_logo: start */
void movieGsLogoStart(void)
{
    lbl_8047A468 = 0;

    lbl_8047A460++;
    sWaveArgs[0] = 5;
    sWaveArgs[1] = 4;
    sWaveArgs[2] = 6;
    sWaveArgs[3] = 0;
    OSCreateThread(&sWaveThread, _menuSoundReadWaveThread__FPv, sWaveArgs,
                   sWaveStack + 0xFFC, 0x1000, 0x10, 1);
    OSResumeThread(&sWaveThread);

    while (lbl_8047A460 != 0) {
        _threadSwitch();
    }

    fadeSet(2, 0.0f);
    fadeCheck(1);
    GSmovieOpen("movie/gs_logo.thp", 0);
    std104_soundSetBackgroundMusic(0x04D1, 0, 0x7F);
    memset((void*)0x80001805, 0, 0x17FB);
}

/* gs_logo: play */
void movieGsLogoWait(void)
{
    if (lbl_8047A468 != 1) {
        while ((u8)GSmovieIsActive() == 1) {
            _threadSwitch();
        }
    }
    floorRequestChange(0x384);
    floorSetFadeScript(0, 0x5960008);
    lbl_8047A468 = 1;
}

/* gs_logo: finish */
void fn_800363BC(void)
{
    fadeSet(3, 0.0f);
    fadeCheck(1);

    lbl_8047A460++;
    sWaveArgs[0] = 3;
    sWaveArgs[1] = 7;
    sWaveArgs[2] = 0;
    sWaveArgs[3] = 0;
    OSCreateThread(&sWaveThread, _menuSoundReadWaveThread__FPv, sWaveArgs,
                   sWaveStack + 0xFFC, 0x1000, 0x10, 1);
    OSResumeThread(&sWaveThread);

    while (lbl_8047A460 != 0) {
        _threadSwitch();
    }
}

void fn_800363B8(void) {}

void fn_800363B4(void) {}

void fn_800363B0(void) {}

/* autodemo01: start */
void movieStartAttract(void)
{
    fadeSet(2, 0.0f);
    fadeCheck(1);
    GSmovieOpen("movie/autodemo01.thp", 0);
    std104_soundSetBackgroundMusic(0x0494, 0, 0x7F);
}

/* autodemo01: play, stopped early by A/B/Start (0x1300) */
void fn_80036240(void)
{
    static MovieCue cues[0x21] = {
        { 0x3DF, 0x08B, 0x48B }, { 0x412, 0x08F, 0x000 }, { 0x433, 0x090, 0x000 },
        { 0x43B, 0x474, 0x000 }, { 0x46C, 0x472, 0x000 }, { 0x49D, 0x08B, 0x48B },
        { 0x4D0, 0x47E, 0x000 }, { 0x57C, 0x1AB, 0x000 }, { 0x5BE, 0x1C1, 0x000 },
        { 0x5C8, 0x1C2, 0x000 }, { 0x5E8, 0x1DC, 0x000 }, { 0x605, 0x13A, 0x000 },
        { 0x667, 0x4A4, 0x000 }, { 0x669, 0x48B, 0x000 }, { 0x6CF, 0x1FF, 0x000 },
        { 0x70B, 0x1EC, 0x000 }, { 0x764, 0x22F, 0x48B }, { 0x78D, 0x22B, 0x000 },
        { 0x7DB, 0x07A, 0x000 }, { 0x80D, 0x07B, 0x000 }, { 0x823, 0x07C, 0x000 },
        { 0x842, 0x474, 0x000 }, { 0x86B, 0x42D, 0x000 }, { 0x883, 0x42F, 0x000 },
        { 0x8FB, 0x3E6, 0x000 }, { 0x918, 0x48B, 0x000 }, { 0x98F, 0x3ED, 0x000 },
        { 0x9B6, 0x082, 0x000 }, { 0x9C2, 0x076, 0x000 }, { 0x9D0, 0x3EE, 0x000 },
        { 0xA08, 0x482, 0x000 }, { 0xA3B, 0x487, 0x000 }, { 0xBB8, 0x000, 0x000 },
    };
    u32 cueIndex = 0;
    s32 state;

    while ((u32)(GSmovieIsActive() & 0xFF) == THP_STATE_PLAYING) {
        state = fn_8017B1AC();
        if (state == 11 || state == 5) {
            _threadSwitch();
            continue;
        }
        /* Retail calls fn_800F7AF0 first; MWCC evaluates the right-hand
         * call of an & first. */
        if ((fn_800F7BC4(1) & fn_800F7AF0(1) & 0x1300) != 0) {
            GSmovieClose();
            break;
        }
        cueIndex = moviePlayCues(cues, 0x21, cueIndex);
        _threadSwitch();
    }
    std138_soundPlayEffectEx(1, 1000, 0, 0x7F);
    floorRequestChange(0x384);
    floorSetFadeScript(0, 0x5960008);
}

/* autodemo01: finish */
void fn_80036210(void)
{
    fadeSet(3, 0.0f);
    fadeCheck(1);
}

/* staffroll: start */
void fn_800361C0(void)
{
    fadeSet(2, 0.0f);
    fadeCheck(1);
    GSmovieOpen("movie/staffroll.thp", 0);
    std104_soundSetBackgroundMusic(0x04C9, 0, 0x7F);
}

/*
 * staffroll: play. On the credits floor (0x76) after the game is cleared
 * (flag 0x476), offer to save: back up the save data, mark the clear and
 * ask (0x444C) until the player declines or the save succeeds; a
 * too-weak party asks again (0x3C02). The live save data is restored
 * from the backup after every attempt.
 */
void fn_80035F64(void)
{
    SavedataBody* backup;
    void* status;
    u32 savedHero;
    s32 answer;

    while ((u8)GSmovieIsActive() == 1) {
        _threadSwitch();
    }
    std104_soundSetBackgroundMusic(1, 0, 0x7F);

    if (floorGetPrevFloorID() == 0x76 && (u8)std081_flagCheckTrue(0x476) == 1) {
        std078_flagSetFalse(0x478);
        status = savedataGetStatus(NULL, 1);
        gamedatasaveSetStatus(status, 5, 2);
        gamedatasaveSetStatus(status, 7, 1);
        gamedatasaveSetStatus(status, 8, 1);
        heroSetStatus(0, 0x18, 1);
        backup = fn_801D036C();
        *backup = *(SavedataBody*)savedataGetStatus(NULL, 0);
        do {
            winMsgOpen(2, 0x444C, 1, 0);
            answer = menuSubOpenYesNo(0, 0x3C, 0xAA, 0);
            if (answer != 0) {
                break;
            }
            if (fn_801D0748(3, 2, 0) == 3 && gamedatasaveGetStatus(NULL, 4) != 0) {
                savedHero = heroGetStatus(savedataGetStatus(backup, 2), 2, 0);
                if (savedHero == heroGetStatus(NULL, 2, 0) ||
                    gamedatasaveGetStatus(backup, 4) == 0) {
                    winMsgOpen(2, 0x3C02, 1, 0);
                    answer = menuSubOpenYesNo(0, 0x3C, 0xAA, 1);
                }
            }
            *(SavedataBody*)savedataGetStatus(NULL, 0) = *backup;
        } while (answer != 0 || fn_801D0748(4, 2, 0) != 4);
        winMsgClose(1);
        fn_801D0314(backup);
    }
    floorLink(2, 1);
    floorSetFadeScript(0, 0x5960008);
}

/* staffroll: finish */
void fn_80035F34(void)
{
    fadeSet(3, 0.0f);
    fadeCheck(1);
}

/* openingdemo: start */
void movieOpeningStart(void)
{
    fadeSet(2, 0.0f);
    fadeCheck(1);
    GSmovieOpen("movie/openingdemo.thp", 0);
    std104_soundSetBackgroundMusic(0x0495, 0, 0x7F);
}

/* openingdemo: play */
void fn_80035E04(void)
{
    static MovieCue cues[5] = {
        { 0x13D, 0x1D6, 0x000 }, { 0x576, 0x449, 0x000 }, { 0x5C3, 0x1D6, 0x000 },
        { 0x5D4, 0x449, 0x000 }, { 0xBB8, 0x000, 0x000 },
    };
    u32 cueIndex = 0;

    while ((u32)(GSmovieIsActive() & 0xFF) == THP_STATE_PLAYING) {
        cueIndex = moviePlayCues(cues, 5, cueIndex);
        _threadSwitch();
    }

    std104_soundSetBackgroundMusic(1, 0, 0x7F);
    std078_flagSetFalse(0x08D0);
    floorLink(1, 0);
    floorSetFadeScript(0, 0x05960008);
}

/* openingdemo: finish */
void fn_80035DD4(void)
{
    fadeSet(3, 0.0f);
    fadeCheck(1);
}
