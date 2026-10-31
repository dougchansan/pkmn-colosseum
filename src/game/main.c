/**
 * @file main.c
 * @brief Main entry point and top-level game initialization for Pokemon Colosseum.
 *
 * Contains main(), the game initialization sequence (mainInit), the main
 * game loop thread (mainGameThread), the early reset/retrace callbacks, and the
 * per-retrace game logic (mainRetraceLogic).
 *
 * Address range: 0x800055E0 - 0x80005FA8
 * Small data:    .sdata2 0x8047B6A0 - 0x8047B6B8 (the 0.0f and 359940.0f
 *                literals and the two int-to-float conversion constants)
 * Source file:   (unknown original filename, likely main.c or game.c)
 */

#include "dolphin/types.h"
#include "dolphin/vi/VI.h"

/* =========================================================================
 * Forward declarations of SDK / engine functions called by this module.
 * Names prefixed with fn_ are auto-generated and not yet fully identified.
 * =========================================================================
 */

/* CRT / libc */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);

/* Dolphin OS */
extern void* OSGetArenaLo(void);
extern void* OSGetArenaHi(void);
extern void  OSSetArenaLo(void* lo);
extern u32   OSGetResetCode(void);
extern void  OSSetErrorHandler(u32 error, void* handler);
extern void  OSResetSystem(int reset, int returnCode, BOOL forceMenu);
extern u64   OSGetTime(void);
extern void  OSLoadContext(void* context);
extern void  OSEnableScheduler(void);

/* --- Game engine subsystem init functions --- */

/* osInstallErrorHandlers: osInstallErrorHandlers - sets up OS error handlers and
 * memory protection regions. Called very early from main(). */
void osInstallErrorHandlers(void);

/* osErrorReportCallback: osErrorReportCallback - custom OS error handler callback.
 * Formats a crash report into a text buffer and spawns a display thread. */
void osErrorReportCallback(u32 error, void* context, ...);

/* osExceptionResume: osExceptionResume - OS exception 8 handler,
 * calls OSLoadContext to resume from the saved context. */
void osExceptionResume(u32 error, void* context);

/* errorReportThread: errorReportThread - thread entry that displays the error
 * report on screen after a crash. */
s32 errorReportThread(void);

/* errorPanic - error printf (file, line, fmt, ...) - OSPanic-style */
void errorPanic(const char* file, s32 line, const char* fmt, ...);

/* --- Heap / memory init (around 0x800E3560) --- */
extern void  GSmemInit(u32 heapId, void* start, void* end); /* GSmem heap init */
extern void  GSmemSetDebugFlag(u32 heapId);                          /* GSmem set default heap */

/* --- Heap allocator (around 0x8009AB60) --- */
extern void* OSInitAlloc(void* start, void* end, u32 numHeaps); /* OSInitAlloc / OSCreateHeap */

/* --- DVD / file system init --- */
extern void fn_800ACA80(void* dvdInfo, u32 priority);  /* DVDInit-related */
extern u32  ARInit(void* stack_index_addr, u32 num_entries);
extern void AIInit(void* stack);
extern void GSscratchInit(u32 arg);
extern void ARQInit(void);                          /* DVDFSInit or DVDSetAutoInvalidation */

/* --- ARAM / audio memory --- */
extern void fn_800AC440(void* arAddr);   /* ARInit / ARQInit */

/* --- Graphics init --- */
extern void GSlogInit(u32 xfbSize, u32 numXfbs);   /* GXInit or framebuffer setup */
extern void fn_800EEDF8(u32 unused);                   /* VI init */

/* --- Game-specific init called from main --- */
extern void mainNop_80167FA4(int argc, char** argv, u32 flag); /* GameArgsParse */

/* mainInit: mainInit - the massive init function called from main.
 * Sets up all subsystems, then enters the main loop. */
void mainInit(void);

/* --- Subsystem init functions called from mainInit --- */
extern void GSthread(u32 numFrames);              /* VIFrameInit or render timing */
extern void GSgfxInit__FP15_GSgfxInitParms(u32 memSize, u32 a, u32 b,   /* GSgfx init */
                         u32 c, u32 d, u32 e);
extern u32  OSGetProgressiveMode(void);                         /* OSGetResetSwitchState (checks warm boot) */
extern void GSgfxSetVideoMode_800D37D4(u32, u32, u32, u32, u32, u32);/* GSgfx video mode config */
extern void GStextureInit(u32 numEntries);               /* GX FIFO init */
extern void GSboundInitPool(u32 numSounds);                 /* Sound system init */
extern void GSmodelInit(u32 maxObjects);                /* GSmem object pool */
extern void GSpartInit(u32 param);                     /* VI callback setup */
extern void GSmaterialInit(u32 bufSize);                   /* Display list buffer init */
extern void GScameraPoolInit(u32 count);                     /* GSgfx light init */
extern void GSlightInit(u32 count);                     /* GSgfx texture init */
extern void gsParticleInit(u32 a, u32 b);                  /* GSmaterial init */
extern void GSparticleInitPool(u32 a, u32 b, u32 c, u32 d);  /* GStexture cache init */
extern void GSinputInit(void);                           /* GSgfx camera/viewport init */

extern void GSinputOpen(u32 padIdx);                    /* PAD motor init per-controller */
extern void GSinputSetClamping(u32 padIdx, u32 param);        /* PAD rumble config */
extern void GSinputSetSmoothingRate(u32 padIdx, u32 param);        /* PAD deadzone config */
extern void GSinputSetSmoothingFunc(u32 padIdx, u32 param);        /* PAD stick config */

extern void GSgappInit(u32 numTasks, u32 numQueues);  /* GSthread init */
extern void GSresInit(u32 maxSteps);                  /* GSthread step limit */
extern void floorSystemStart(u32 a, u32 b, u32 c, u32 d);  /* GSthread pool config */
extern void GScolsys2Init(void);                           /* GSthread scheduler init */
extern void cms_InitThreadPool(u32 maxPads);                   /* PAD system init */
extern void cms_PoolSetParam(void* padTable);                 /* PAD set mapping table */
extern void GSmsgInit(u32 numFloors, u32 numLayers);  /* GSfloor system init */
extern void GSmsgSetCtrlFunc(void* sndTable);                 /* GSfloor sound table */
extern void GSmsgFontOpen(void* relData);                  /* GSfloor register REL data */
extern void GSmsgOpen(void* relData);                  /* GSfloor register REL data (alt) */
extern void cms_RegisterScript(void* relData);                  /* GSfloor register REL data (floor) */
extern void soundSetInitParams(u32 a, u32 b, u32 c, u32 d, u32 e); /* set the five DVD error message IDs */
extern void vtrInit(void);                           /* Save/card system init */
extern void flareCreateTask(void);                           /* GBA communication init */
extern void GSmovieInit(void);                           /* Save data init */
extern void menuInit(u32 param);                      /* Effect system init */
extern void gappLoadMeterInit(u32 param);                      /* GS load meter draw-sync hook init */
extern void GSgfxSetFrameInterval(u32 flag);                       /* GSgfx enable rendering */

extern u32 GSgappCreate(s32 state, u8 priority, void* param, /* GSthread create task */
                       void* func);
extern void GSgappUpdate(void);                           /* GSthread yield / run scheduler */
extern void GSthreadCreate(u32 affinity, u32 priority,      /* GSthread create main thread */
                         u32 stackSize, u32 usesFPU,
                         u32 autoStart, void* entry);

extern s32 GSvtrRegisterGSgapp(u32 taskId);              /* Save task registration */
extern void fadeInit(void);                           /* Script engine init */
extern void GSgappInitPools(u32 numSlots);                   /* Floor/scene loader init */
extern void GSeffectInit(u32 maxEffects);                 /* 3D model/effect loader */
extern u32  GSmemGetFreeTotal(void);                           /* OSGetFreeMemSize-like */
extern void GSlogWrite(const char* fmt, ...);           /* OSReport / debug printf */
extern f32 __cvt_fp2unsigned(f32 volume);                     /* volume clamp/process */

/* --- Game main loop (mainGameThread) and its helpers --- */
void mainGameThread(void);
void mainRegisterSoundListener(void); /* mainRegisterSoundListener - background color / render schedule init */

/* --- Per-frame callback tasks registered by mainInit --- */
void mainRetraceDraw(void);       /* mainRetraceDraw - primary retrace handler */
void mainPadPollTask(void);      /* mainPadPollTask - controller polling task */
void mainPadReadTask(void);    /* mainPadReadTask - controller rumble task */
void mainLogicTask(u32 a, u32 b); /* mainLogicTask - audio retrace callback */
void mainClearExceptionVectors(void);    /* mainClearExceptionVectors - reset button countdown */
void mainRetraceLogic(void);  /* mainRetraceLogic - per-retrace game logic */
void mainCheckResetButton(void); /* mainCheckResetButton - reset button handler */

/* --- Small utility functions near main --- */

/* mainSetPauseFlag: mainSetPauseFlag - sets a byte flag; returns previous value */
extern u8 mainSetPauseFlag(u8 newVal);

/* mainTakePauseFlag: mainTakePauseFlag - reads flag, clears it, returns old */
extern u8 mainTakePauseFlag(void);

/* titleSetFlag80478DC8: titleSetFlag80478DC8 */
extern void titleSetFlag80478DC8(u8 val);

/* gamedataSetPlaytime: gamedataSetPlaytime - if sound system present, set volume */
extern void gamedataSetPlaytime(f32 volume);

/* gamedataGetPlaytime: gamedataGetPlaytime - if sound system present, get volume */
extern f32 gamedataGetPlaytime(void);

/* GSgetVersionMajor: GSgetVersionMajor - returns 1 */
extern u32 GSgetVersionMajor(void);

/* GSgetVersionMinor: GSgetVersionMinor - returns 6 */
extern u32 GSgetVersionMinor(void);

/* --- Miscellaneous engine functions --- */
extern void bootMain(void);    /* Scene/floor update tick */
extern u32  floorSetEnterCallbacks(void* a, u32 b); /* GSthread set frame counter */
extern void* GSresAllocResource(u32 a, u32 b, u32 c, u32 d); /* GSthread alloc work area */

extern u32  savedataGetWorkPtr(void);    /* SoundSystemIsReady */
extern void* savedataGetSection0(void);   /* SoundSystemGetContext */
extern void* gamedatasaveBiosGetPtr(void);   /* SoundGetMixer */
extern void gamedatasaveBiosSetPlaytime(void* mixer, f32 volume); /* SoundSetMasterVolume */
extern f32  gamedatasaveBiosGetPlaytime(void* mixer);  /* SoundGetMasterVolume */

extern u32  savedataGetBuffer(void);     /* RNG get seed */
extern void randSetSeed(u32 seed); /* RNG set seed */
extern void savedataCreate(u32 a, u32 b); /* RNG init */
extern void fightGSfloorRegister(void);    /* Battle system global init */
extern u32  savedataGetStatus(u32 a, u32 b); /* RNG or calendar read */
extern void fn_801909A8(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f); /* Calendar/RTC set */
extern void GSvtrLoadTexture(void);    /* Card system tick */
extern void mainNop_8010C220(void);    /* Effect system tick */
extern void cameraInit(void);    /* World/map system init */
extern void partnerSystemInit(void);    /* 3D model system init */
extern void peopleSystemInit(u32 a);   /* Pokemon data/model init */
extern void darkPokemonStepInit(void);    /* GBA link init */
extern void memcardInit(void);    /* Menu/UI system init */
extern void floorSystemInit(void);    /* Colosseum mode init */
extern void* tableResBiosGetResPtr(u32 id);  /* Load relocatable module (REL) by ID */

extern void* soundSetListener(void* color); /* Set clear/background color */
extern void GSresRegisterResource(void* work, u32 time, u32 numFrames, u32 arg); /* Thread wait/schedule */

extern u32  fn_800A03B4(void);    /* OSGetResetButtonState */
extern int  OSGetResetButtonState(void); /* real symtab name (signed return -> cmpwi) */
extern void VIFlush(void);               /* real symtab name for fn_800AA068 call site */
extern void soundShutdown(void);    /* Screen fade to black */
extern void PADRecalibrate(u32 mask);/* VISetBlack / video blank */
extern void VISetBlack(u32 flag);/* VIFlush */
extern void fn_800AA068(void);    /* VIWaitForRetrace */
extern void OSSetIdleFunction(u32 a, u32 b, u32 c, u32 d); /* OSClearStack or thread cleanup */
extern void VIWaitForRetrace(void);    /* AIReset */

extern void GSthreadExecuteAll(void);    /* GSthread begin frame */
extern u8   dbgGetFlag8047A2A0(void);    /* IsLanguageJapanese or locale check */
extern void peopleMain(u32 a, u32 b); /* Retry/reconnect handler */

extern u8   gbaCommIsReady(u32 padIdx); /* PAD is connected check */
extern void GSinputRead(u32 padIdx); /* PAD disconnect handler */
extern u8 GSinputIsConnected(u32 padIdx); /* PAD connection check */
extern void gbaPollPort(u32 padIdx); /* PAD recalibrate */

extern void gappLoadMeterDraw(u32 mask);   /* GXSetChanCtrl / GX render state */
extern void gappLoadMeterDrawFrame(void);       /* GX render flush */
extern void gappIssueDrawSync(void);       /* GX render end */
extern void GSgfxWaitRetrace(u8 mode);    /* GSgfx set draw mode */
extern void GSgfxSwapBuffers(u32 flag);   /* GSgfx swap buffers */
extern void GSgfxCaptureUpdate(void);       /* World/scene render tick */

extern void vtrUpdate(s32 unk, u32 a, u32 b); /* Save/card per-frame update */

extern u8   vtrIsActive(void);   /* Card system check pending */
extern s32  vtrGetMode(void);   /* Card system get state */
extern void vtrSetMode1(void);   /* Card system process state 2 */
extern void vtrSetMode2(void);   /* Card system process other */
extern void vtrSetMode3(void);   /* Card system finalize */

extern void menuDaemon(void);   /* Particle system update */
extern void GSmsgDaemon(void);   /* GSthread sync / process tasks */

extern u32  std082_flagGetValue(u32 evtId);  /* Event system check state */
extern s32  GSgfxGetVsyncRate(void);       /* GSgfx get frame counter */
extern u32  GSgetFrameStep(void);       /* GSgfx get tick counter */

extern u32  GSinputButtonsChanged(u32 padIdx); /* PAD get buttons pressed */
extern u32  GSinputGetButtons(u32 padIdx); /* PAD get buttons held */
extern void dbgMovieCaptureUpdate(void);       /* Debug menu toggle */

/* --- SDA (small data area) globals used by this module --- */

/* lbl_80478DC0 @sda21 - DVD info structure */
extern u8 lbl_80478DC0;

/* lbl_80478DC8 @sda21 - rumble enabled flag */
extern u8 lbl_80478DC8;

/* lbl_80478DC9 @sda21 - pause/init-complete flag */
extern u8 lbl_80478DC9;

/* lbl_80478DCA @sda21 - reset button latch */
extern u8 lbl_80478DCA;

/* lbl_80478DCC @sda21 - reset countdown timer (int) */
extern s32 lbl_80478DCC;

/* lbl_80478DD0 @sda21 - draw mode byte */
extern u8 lbl_80478DD0;

/* lbl_80478820 @sda21 - unknown flag (checked in VBlank task) */
extern u8 lbl_80478820;

/* lbl_80478990 @sda21 - max error handler index */
extern s32 lbl_80478990;

/* lbl_8047A260 @sda21 - error number for crash display */
extern u32 lbl_8047A260;
/* lbl_8047A264 @sda21 */
extern u32 lbl_8047A264;
/* lbl_8047A268 @sda21 */
extern u32 lbl_8047A268;

/* lbl_80478FB8 @sda21 - pointer to frame counter */
extern u32* lbl_80478FB8;
/* lbl_80478FBC @sda21 - pointer to scene state */
extern void* lbl_80478FBC;

/* lbl_8047B6B8 @sda21 - small string, source file name for error print */
extern char lbl_8047B6B8[8];
/* lbl_8047B6C0 @sda21 - small format string for the error name line */
extern char lbl_8047B6C0[8];

/* --- Data labels referenced by init code --- */
extern u8 lbl_8039A700[];   /* ARAM base for ARInit */
extern u8 lbl_802E1CF0[];   /* PAD mapping table */
extern u8 msgctrlcode[];   /* GSfloor sound table */
extern u8 lbl_8027A500[];   /* Floor/scene REL data (data section start) */
extern u8 lbl_802BD260[];   /* Floor/scene REL data #2 */
extern u8 lbl_802C0CB0[];   /* Floor/scene REL data #3 */
extern u8 lbl_802CF810[];   /* Floor/scene REL data #4 */
extern u8 lbl_80266438[];   /* Background clear color (rodata) */
extern u8 lbl_802663A0[];   /* Exception address map table (rodata) */
extern u8 lbl_803A0700[];   /* Crash report text buffer (BSS) */
extern u8 lbl_803A1700[];   /* Saved OS context for crash (BSS) */
extern u8 lbl_8039E700[];   /* Error display thread stack (BSS) */
extern u8 lbl_802E2888[];   /* Error name string table */

extern const char lbl_80266420[]; /* "OS avail memory: %d\n" */
extern const char lbl_80266448[]; /* "OS_ERROR_SYSTEM_RESET" (error name table base) */

/* OSProtectRange = __OSInitMemoryProtection region setup */
extern void OSProtectRange(u32 region, u32 addr, u32 size, u32 perm);

/* DBIsDebuggerPresent = __DBIsExceptionMarked or similar */
extern s32  DBIsDebuggerPresent(void);

/* OSCreateThread = OSCreateThread */
extern void OSCreateThread(void* thread, void* func, void* arg,
                         void* stackBase, u32 stackSize,
                         u32 priority, u32 detached);
/* OSResumeThread = OSResumeThread */
extern void OSResumeThread(void* thread);
/* fn_800A1E54 = OSSetThreadPriority */
extern void fn_800A1E54(void* thread, u32 prio);

/* va_list for PPC - MetroWerks CW inline */
typedef struct __va_list_struct {
    u8  gpr;
    u8  fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} __va_list_struct;
typedef __va_list_struct va_list[1];
#define va_start(ap, last) ((void)last, __builtin_va_info(&ap))
#define va_end(ap)         ((void)0)

/* vsprintf = vsprintf */
extern int  vsprintf(char* buf, const char* fmt, va_list ap);
/* sprintf = sprintf */
extern int  sprintf(char* buf, const char* fmt, ...);

/* OSGetStackPointer (0x8009BD28) */
extern u32 OSGetStackPointer(void);

/* __va_arg */
extern void* __va_arg(void* ap, u32 type);

/* __OSSetExceptionHandler */
extern void __OSSetExceptionHandler(u32 exception, void* handler);

/* =========================================================================
 *  main()
 *  Address: 0x800055E0, Size: 0xE4
 *
 *  Called by __start() after CRT init. Sets up the Dolphin heap, DVD
 *  subsystem, graphics, and then calls mainInit() which never returns
 *  (it enters the main loop).
 * =========================================================================
 */
int main(int argc, char** argv) {
    void* arenaLo;
    void* arenaHi;
    void* heapStart;

    /* Clear low-memory scratch area at 0x80001800, size 0x1800 */
    memset((void*)0x80001800, 0, 0x1800);

    /* Install OS error handlers before anything else */
    osInstallErrorHandlers(); /* osInstallErrorHandlers */

    /* Set up the main game heap from the OS arena */
    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();

    /*
     * heapStart = arenaLo + 0x00E80000
     * The game reserves ~14.5 MB for the main heap.
     * addis r30, r31, 0xe8 => heapStart = arenaLo + (0xe8 << 16)
     */
    heapStart = (void*)((u32)arenaLo + 0x00E80000);

    /* Initialize the GS memory allocator with heap region [arenaLo, heapStart) */
    GSmemInit(0, arenaLo, heapStart);

    /* Set heap 0 as the default allocation heap */
    GSmemSetDebugFlag(0);

    /* Create OS heap from [heapStart, arenaHi) for system allocations,
     * then move arena low past our heaps */
    OSSetArenaLo(OSInitAlloc(heapStart, OSGetArenaHi(), 1));

    /* Initialize ARAM (audio RAM) */
    ARInit(&lbl_80478DC0, 2);

    /* Initialize DVD filesystem */
    ARQInit();

    /* Initialize AI with stack at lbl_8039A700 + 0x3FF8 */
    AIInit((void*)(lbl_8039A700 + 0x3FF8));

    /* Initialize XFB (external framebuffer): 0x10000 bytes, 1 buffer */
    GSlogInit(0x10000, 1);

    /* Initialize scratch allocator */
    GSscratchInit(0);

    /* Parse command-line arguments (from disc header / apploader) */
    mainNop_80167FA4(argc, argv, 1);

    /* Enter the game initialization and main loop (does not return) */
    mainInit(); /* mainInit */
}

/* =========================================================================
 *  mainSetPauseFlag / mainSetPauseFlag
 *  Address: 0x800056C4, Size: 0x10
 *
 *  Atomically sets the pause flag and returns the previous value.
 * =========================================================================
 */
u8 mainSetPauseFlag(u8 newVal) {
    u8 oldVal = lbl_80478DC9;
    lbl_80478DC9 = newVal;
    return oldVal;
}

/* =========================================================================
 *  mainTakePauseFlag / mainTakePauseFlag
 *  Address: 0x800056D4, Size: 0x10
 *
 *  Reads the pause flag, clears it to 0, returns the old value.
 * =========================================================================
 */
u8 mainTakePauseFlag(void) {
    u8 val = lbl_80478DC9;
    lbl_80478DC9 = 0;
    return val;
}

/* =========================================================================
 *  titleSetFlag80478DC8 / titleSetFlag80478DC8
 *  Address: 0x800056E4, Size: 0x8
 *
 *  Stores a flag controlling whether controller rumble is active.
 * =========================================================================
 */
void titleSetFlag80478DC8(u8 val) {
    lbl_80478DC8 = val;
}

/* =========================================================================
 *  gamedataSetPlaytime / gamedataSetPlaytime
 *  Address: 0x800056EC, Size: 0x5C
 *
 *  If the sound system is initialized, sets the master volume on the
 *  sound mixer. The volume parameter arrives in f1.
 * =========================================================================
 */
void gamedataSetPlaytime(f32 volume) {
    void* mixer;

    /* Check if sound system is ready */
    if (savedataGetWorkPtr() != 0 && savedataGetSection0() != 0) {
        mixer = gamedatasaveBiosGetPtr();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL) {
        gamedatasaveBiosSetPlaytime(mixer, volume);
    }
}

/* =========================================================================
 *  gamedataGetPlaytime / gamedataGetPlaytime
 *  Address: 0x80005748, Size: 0x58
 *
 *  Returns the current master volume from the sound mixer.
 *  Falls back to 0.0f if the sound system is not available.
 * =========================================================================
 */
f32 gamedataGetPlaytime(void) {
    void* mixer;
    f32 volume;

    if (savedataGetWorkPtr() != 0 && savedataGetSection0() != 0) {
        mixer = gamedatasaveBiosGetPtr();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL) {
        volume = gamedatasaveBiosGetPlaytime(mixer);
    } else {
        volume = 0.0f;
    }

    return __cvt_fp2unsigned(volume); /* Some processing/clamping of the volume value */
}

/* =========================================================================
 *  GSgetVersionMajor / GSgetVersionMajor
 *  Address: 0x800057A0, Size: 0x8
 *
 *  Returns the major version number: 1
 * =========================================================================
 */
u32 GSgetVersionMajor(void) {
    return 1;
}

/* =========================================================================
 *  GSgetVersionMinor / GSgetVersionMinor
 *  Address: 0x800057A8, Size: 0x8
 *
 *  Returns the minor version number: 6
 *  (Pokemon Colosseum version 1.6)
 * =========================================================================
 */
u32 GSgetVersionMinor(void) {
    return 6;
}

/* =========================================================================
 *  mainInit / mainInit
 *  Address: 0x800057B0, Size: 0x2FC
 *
 *  Master initialization function. Sets up every game subsystem, then
 *  creates the main game thread and enters an infinite yield loop.
 *
 *  This function never returns.
 * =========================================================================
 */
void mainInit(void) {
    u8 isWarmBoot;
    s32 i;

    /* Create the 20-slot cooperative GS thread (fiber) pool */
    GSthread(0x14);

    /* Initialize GSgfx (see GSgfxInit):
     *   0x6DDD0 = request-buffer size, 0x10 = matrix-stack depth,
     *   0x8 = render-layer pool (8 records of 0x190), 0x20 = pool of 0x18-byte records,
     *   0x1 = TV standard, 0x1E0 = EFB height override */
    GSgfxInit__FP15_GSgfxInitParms(0x6DDD0, 0x10, 0x8, 0x20, 0x1, 0x1E0);

    isWarmBoot = 0;

    /* Check if this is a warm boot (reset from game) */
    if (OSGetProgressiveMode() == 1) {
        /* If reset code's top bit indicates "return to game" */
        BOOL isReset = ((OSGetResetCode() + 0x80000000) == 0) ? TRUE : FALSE;
        if (isReset == TRUE) {
            isWarmBoot = 1;
        }
    }

    /* On warm boot, reconfigure video mode */
    if (isWarmBoot) {
        GSgfxSetVideoMode_800D37D4(1, 2, 0, 2, 1, 0);
    }

    /* Initialize GX command FIFO with 16 entries */
    GStextureInit(0x10);

    /* Initialize sound system with 8 channels */
    GSboundInitPool(0x8);

    /* Initialize GSmem object pool for 64 objects */
    GSmodelInit(0x40);

    /* Initialize VI retrace callback system with 8 callbacks */
    GSpartInit(0x8);

    /* Initialize display list buffer: 1024 bytes */
    GSmaterialInit(0x400);

    /* Initialize lighting system with 32 lights */
    GScameraPoolInit(0x20);

    /* Initialize texture system with 32 textures */
    GSlightInit(0x20);

    /* Initialize material system: 32 materials, 128 max textures */
    gsParticleInit(0x20, 0x80);

    /* Initialize texture cache: 16 slots, 64 entries, 1 bank, 8192 byte cache */
    GSparticleInitPool(0x10, 0x40, 0x1, 0x2000);

    /* Initialize camera/viewport defaults */
    GSinputInit();

    /* Configure all 4 controller pads (indices 1-4) */
    for (i = 1; i <= 4; i++) {
        GSinputOpen(i);        /* Init motor for pad i */
        GSinputSetClamping(i, 2);    /* Set rumble mode */
        GSinputSetSmoothingRate(i, 7);    /* Set analog deadzone */
        GSinputSetSmoothingFunc(i, 1);    /* Set stick mode */
    }

    /* Initialize GSthread system: 16 tasks, 4 queues */
    GSgappInit(0x10, 0x4);

    /* Set thread step limit to 300 */
    GSresInit(0x12C);

    /* Initialize four floor contexts plus 48 floor resources and launch the floor worker */
    floorSystemStart(0x4, 0x10, 0x10, 0x10);

    /* Initialize collision-draw state */
    GScolsys2Init();

    /* Allocate 16 script VM contexts (0x16C bytes each) */
    cms_InitThreadPool(0x10);

    /* Set the script VM native-call dispatch table (12-byte entries) */
    cms_PoolSetParam(lbl_802E1CF0);

    /* Initialize floor/scene system: 2 floors, 5 layers */
    GSmsgInit(0x2, 0x5);

    /* Register floor sound table */
    GSmsgSetCtrlFunc(msgctrlcode);

    /* Register floor REL data tables */
    GSmsgFontOpen(lbl_8027A500); /* Main scene data */
    GSmsgFontOpen(lbl_802BD260); /* Scene data #2 */
    GSmsgFontOpen(lbl_802C0CB0); /* Scene data #3 */
    GSmsgOpen(lbl_802CF810); /* Scene data #4 (alt registration) */

    /* Set the DVD error message IDs: cover open, no disk, wrong disk, retry, fatal */
    soundSetInitParams(0x3BE8, 0x3BEA, 0x3BED, 0x3BEF, 0x3BF2);

    /* Initialize GSvtr texture-display state and create its display task */
    vtrInit();

    /* Initialize battle texture/resource state and create its render callback */
    flareCreateTask();

    /* Initialize the THP movie runtime */
    GSmovieInit();

    /* Initialize 3D effect system: 24 max effects */
    menuInit(0x18);

    /* Install the GS load meter's GX draw-sync callback; GPU perf counters off */
    gappLoadMeterInit(0x4);

    /* Enable rendering pipeline */
    GSgfxSetFrameInterval(1);

    /* ---------------------------------------------------------------
     * Register per-frame callback tasks.
     * GSgappCreate creates a task: (active, taskId, param, func)
     * ---------------------------------------------------------------
     */

    /* Task 0xFF: Primary VBlank handler (mainRetraceDraw) */
    GSgappCreate(1, 0xFF, 0, (void*)mainRetraceDraw);

    /* Task 0x00: Pad rumble update (mainPadReadTask) */
    GSgappCreate(1, 0x00, 0, (void*)mainPadReadTask);

    /* Task 0x01: Pad read/polling (mainPadPollTask) */
    GSgappCreate(1, 0x01, 0, (void*)mainPadPollTask);

    /* Task 0x0A: Audio retrace callback (mainLogicTask) */
    GSvtrRegisterGSgapp(
        GSgappCreate(1, 0x0A, 0, (void*)mainLogicTask));

    /* Task 0xFD: Reset button countdown (mainClearExceptionVectors) */
    GSgappCreate(1, 0xFD, 0, (void*)mainClearExceptionVectors);

    /* Task 0xFE: Main retrace logic (mainRetraceLogic) */
    GSgappCreate(1, 0xFE, 0, (void*)mainRetraceLogic);

    /* Task 0xFF: Reset handler (mainCheckResetButton) - overrides earlier 0xFF? */
    GSgappCreate(1, 0xFF, 0, (void*)mainCheckResetButton);

    /* Mark initialization as complete */
    lbl_80478DC9 = 1;

    /* Initialize script engine */
    fadeInit();

    /* Initialize floor/scene loader with 8 slots */
    GSgappInitPools(0x8);

    /* Initialize model/effect loader: 32 max */
    GSeffectInit(0x20);

    /* Print available memory to debug console */
    {
        u32 freeMem = (u32)GSmemGetFreeTotal();
        GSlogWrite(lbl_80266420, freeMem); /* "OS avail memory: %d\n" */
    }

    /* Create and start the main game loop thread:
     *   affinity=0, priority=1000, stackSize=0x4000,
     *   usesFPU=1, autoStart=1, entry=mainGameThread */
    GSthreadCreate(0, 0x3E8, 0x4000, 1, 1, (void*)mainGameThread);

    /* Infinite yield loop - scheduler takes over from here */
    for (;;) {
        GSgappUpdate(); /* GSthread yield */
    }
}

/* =========================================================================
 *  mainGameThread / mainGameThread
 *  Address: 0x80005AAC, Size: 0x190
 *
 *  The main game loop thread entry point. Runs once per frame:
 *  1. Updates scene state
 *  2. Processes input, RNG seeding from clock
 *  3. Loads REL modules
 *  4. Updates all game subsystems
 *  5. Clears scratch memory
 * =========================================================================
 */
void mainGameThread(void) {
    void* mixer;

    /* Tick the scene/floor system */
    bootMain();

    /* Update frame counter from scene state */
    {
        u32* framePtr = lbl_80478FB8;
        void* sceneState = lbl_80478FBC;
        floorSetEnterCallbacks(sceneState, *framePtr);
    }

    /* Allocate a 2-byte work area (for some per-frame state) */
    {
        void* work = GSresAllocResource(2, 0, 2, 0);
        *(u8*)(work) = 0;
        *((u8*)(work) + 1) = 0;
    }

    /* Clear draw mode flag */
    lbl_80478DD0 = 0;

    /* If sound system is ready, reset master volume to 0.0 */
    if (savedataGetWorkPtr() != 0 && savedataGetSection0() != 0) {
        mixer = gamedatasaveBiosGetPtr();
    } else {
        mixer = NULL;
    }
    if (mixer != NULL) {
        gamedatasaveBiosSetPlaytime(mixer, 0.0f);
    }

    /* Reset rumble flag */
    lbl_80478DC8 = 0;

    /* Load REL modules and register their scene/floor data.
     * tableResBiosGetResPtr loads a relocatable module by ID and returns a pointer
     * to its data, which is then registered with the floor system. */
    GSmsgFontOpen(tableResBiosGetResPtr(1));  /* REL 1 -> register scene data */
    GSmsgOpen(tableResBiosGetResPtr(4));  /* REL 4 -> register scene data (alt) */
    cms_RegisterScript(tableResBiosGetResPtr(7));  /* REL 7 -> register floor data */

    /* Seed the RNG using the low bits of the OS tick counter.
     * OSGetTime returns u64 in r3:r4; we use r4 (low 32 bits).
     * rlwinm r27, r4, 0, 21, 26 => r4 & 0x000007E0 */
    randSetSeed(savedataGetBuffer() + ((u32)OSGetTime() & 0x7E0)); /* add time entropy to seed */
    savedataCreate(0, 0); /* Reset RNG sequence */

    /* Initialize the battle system */
    fightGSfloorRegister();

    /* Set the in-game calendar from RTC values (sec/min/hour/day/month/year) */
    fn_801909A8(savedataGetStatus(0, 4), savedataGetStatus(0, 7), savedataGetStatus(0, 5),
                savedataGetStatus(0, 8), savedataGetStatus(0, 6), savedataGetStatus(0, 9));

    /* Tick save/card system */
    GSvtrLoadTexture();

    /* Tick effect system (no-op stub at this point) */
    mainNop_8010C220();

    /* Initialize background color / render schedule */
    mainRegisterSoundListener();

    /* Initialize world/map system */
    cameraInit();

    /* Initialize 3D model system */
    partnerSystemInit();

    /* Initialize Pokemon model/data system (48 slots) */
    peopleSystemInit(0x30);

    /* Initialize GBA link subsystem */
    darkPokemonStepInit();

    /* Initialize menu/UI system */
    memcardInit();

    /* Initialize Colosseum battle mode */
    floorSystemInit();

    /* Clear low-memory scratch area 0x80001803, size 0x17FD
     * (preserves bytes 0x1800-0x1802 which contain status flags) */
    memset((void*)0x80001803, 0, 0x17FD);
}

/* =========================================================================
 *  mainCheckResetButton / mainCheckResetButton
 *  Address: 0x80005C3C, Size: 0xA8
 *
 *  Checks the reset button state. On first press, latches.
 *  On release (after latch), if init is complete, performs a full
 *  hardware shutdown and calls OSResetSystem.
 * =========================================================================
 */
void mainCheckResetButton(void) {
    if (lbl_80478DCA == 0) {
        /* Button not yet latched - check if pressed */
        if (OSGetResetButtonState() == 1) {
            lbl_80478DCA = 1; /* latch the reset press */
        }
    } else {
        /* Button was latched - wait for release */
        if (OSGetResetButtonState() == 0) {
            /* Released. Only reset if init is complete. */
            if (lbl_80478DC9 == 1) {
                soundShutdown();           /* Fade screen to black */
                PADRecalibrate(0xF0000000); /* VISetBlack */
                VISetBlack(1);          /* VIFlush */
                VIFlush();               /* real symtab: VIFlush (was fn_800AA068) */
                VISetPreRetraceCallback(NULL);        /* Clear pre-retrace callback */
                VISetPostRetraceCallback(NULL);        /* Clear post-retrace callback */
                OSSetIdleFunction(0, 0, 0, 0); /* Clean up threads */
                VIWaitForRetrace();           /* Reset audio */
                OSResetSystem(0, 0, 1);  /* Reset to system menu */
            }
        }
    }
}

/* =========================================================================
 *  mainClearExceptionVectors / mainClearExceptionVectors
 *  Address: 0x80005CE4, Size: 0x9C
 *
 *  On first call (lbl_80478DCC == 0), clears OS exception vectors by
 *  using the table at lbl_802663A0, then clears low-memory scratch.
 *  Sets a countdown timer of 100 frames, decremented each call.
 *  This provides a delay before the game is fully ready.
 * =========================================================================
 */
void mainClearExceptionVectors(void) {
    u32 i;

    if (lbl_80478DCC == 0) {
        /* First-time init: clear exception vector memory regions */
        for (i = 0; i < 0x20; i += 2) {
            u32 start = ((u32*)lbl_802663A0)[i] + 0x15;
            u32 end   = ((u32*)lbl_802663A0)[i + 1] + 0x16;
            memset((void*)(start + 0x80000000), 0, end - start);
        }

        /* Clear scratch memory at 0x80001801 */
        memset((void*)0x80001801, 0, 0x17FF);

        /* Start the 100-frame countdown */
        lbl_80478DCC = 100;
    }

    /* Decrement the countdown timer each frame */
    lbl_80478DCC--;
}

/* =========================================================================
 *  mainRetraceDraw / mainRetraceDraw
 *  Address: 0x80005D80, Size: 0x80
 *
 *  Primary vertical blank handler. Runs each frame:
 *  1. Sets up GX render state
 *  2. Applies the draw mode
 *  3. Flushes GX commands
 *  4. Triggers the world/scene render
 *  5. Checks a flag and calls the save/card update accordingly
 *  6. Finalizes the GX render state
 * =========================================================================
 */
void mainRetraceDraw(void) {
    /* Set render state mask (0xFF00 = enable all channels) */
    gappLoadMeterDraw(0xFF00);

    /* Apply the draw mode from the per-frame flag */
    GSgfxWaitRetrace(lbl_80478DD0);

    /* Flush GX render commands */
    gappLoadMeterDrawFrame();

    /* Clear swap flag */
    GSgfxSwapBuffers(0);

    /* Tick world/scene renderer */
    GSgfxCaptureUpdate();

    /* Update save/card system based on lbl_80478820 flag */
    if (lbl_80478820 == 0) {
        vtrUpdate(0x10, 1, 1);
    } else {
        vtrUpdate(-1, 1, 1);
    }

    /* Finalize GX render state (0xFFFF00 = full state mask) */
    gappLoadMeterDraw(0xFFFF00);

    /* End GX render pass */
    gappIssueDrawSync();
}

/* =========================================================================
 *  mainRetraceLogic / mainRetraceLogic
 *  Address: 0x80005E00, Size: 0x1A8
 *
 *  Per-retrace game logic handler. Checks controller buttons for
 *  debug menu activation, handles save/card state transitions,
 *  and advances the save file's play time by one retrace, capped at
 *  359940 seconds (99:59:00).
 *
 *  The cap is a literal: retail loads it from this TU's own .sdata2 pool
 *  (0x8047B6A4, between the 0.0f literal and the conversion constants), and
 *  an extern reference instead changes the FPR allocation of the clamp.
 * =========================================================================
 */
void mainRetraceLogic(void) {
    void* mixer;
    f32 vol;
    f32 v;
    u32 pressed;
    u32 held;

    gappLoadMeterDraw(0xFF);

    pressed = GSinputButtonsChanged(1);
    held = GSinputGetButtons(1);
    if (held & pressed & 0x10) {
        dbgMovieCaptureUpdate();
    }

    if (vtrIsActive()) {
        pressed = GSinputButtonsChanged(1);
        held = GSinputGetButtons(1);
        if (held & pressed & 0x40) {
            if (vtrGetMode() == 2) {
                vtrSetMode1();
            } else {
                vtrSetMode2();
            }
        }
        pressed = GSinputButtonsChanged(1);
        held = GSinputGetButtons(1);
        if (held & pressed & 0x20) {
            vtrSetMode3();
        }
    }

    menuDaemon();
    GSmsgDaemon();

    if (savedataGetWorkPtr() != 0 && savedataGetSection0() != 0) {
        mixer = gamedatasaveBiosGetPtr();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL && lbl_80478DC8 == 1 && std082_flagGetValue(0x8AE) == 0) {
        vol = gamedatasaveBiosGetPlaytime(mixer);
        v = vol + (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
        if (v >= 359940.0f) {
            v = 359940.0f;
        }
        gamedatasaveBiosSetPlaytime(mixer, v);
    }
}
