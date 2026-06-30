/**
 * @file main.c
 * @brief Main entry point and top-level game initialization for Pokemon Colosseum.
 *
 * Contains main(), the game initialization sequence (fn_800057B0), the main game
 * loop thread (fn_80005AAC), and various OS-level callback handlers for
 * reset, retrace, and error conditions.
 *
 * Address range: 0x800055E0 - 0x800064C4
 * Source file:   (unknown original filename, likely main.c or game.c)
 */

#include "dolphin/types.h"

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

/* fn_80006250: fn_80006250 - sets up OS error handlers and
 * memory protection regions. Called very early from main(). */
void fn_80006250(void);

/* fn_80006378: fn_80006378 - custom OS error handler callback.
 * Formats a crash report into a text buffer and spawns a display thread. */
void fn_80006378(u32 error, void* context, ...);

/* fn_800064A0: fn_800064A0 - OS exception 8 handler,
 * calls OSLoadContext to resume from the saved context. */
void fn_800064A0(u32 error, void* context);

/* fn_800064C4: fn_800064C4 - thread entry that displays the error
 * report on screen after a crash. */
s32 fn_800064C4(void);

/* fn_800060F0 - error printf (file, line, fmt, ...) - OSPanic-style */
void fn_800060F0(const char* file, s32 line, const char* fmt, ...);

/* --- Heap / memory init (around 0x800E3560) --- */
extern void  fn_800E3568(u32 heapId, void* start, void* end); /* GSmem heap init */
extern void  fn_800E3560(u32 heapId);                          /* GSmem set default heap */

/* --- Heap allocator (around 0x8009AB60) --- */
extern void* fn_8009AB60(void* start, void* end, u32 numHeaps); /* OSInitAlloc / OSCreateHeap */

/* --- DVD / file system init --- */
extern void fn_800ACA80(void* dvdInfo, u32 priority);  /* DVDInit-related */
extern u32  ARInit(void* stack_index_addr, u32 num_entries);
extern void AIInit(void* stack);
extern void GSscratchInit(u32 arg);
extern void fn_800AE5C0(void);                          /* DVDFSInit or DVDSetAutoInvalidation */

/* --- ARAM / audio memory --- */
extern void fn_800AC440(void* arAddr);   /* ARInit / ARQInit */

/* --- Graphics init --- */
extern void fn_800DDF54(u32 xfbSize, u32 numXfbs);   /* GXInit or framebuffer setup */
extern void fn_800EEDF8(u32 unused);                   /* VI init */

/* --- Game-specific init called from main --- */
extern void fn_80167FA4(int argc, char** argv, u32 flag); /* GameArgsParse */

/* fn_800057B0: fn_800057B0 - the massive init function called from main.
 * Sets up all subsystems, then enters the main loop. */
void fn_800057B0(void);

/* --- Subsystem init functions called from fn_800057B0 --- */
extern void GSthread(u32 numFrames);              /* VIFrameInit or render timing */
extern void fn_800D39E0(u32 memSize, u32 a, u32 b,   /* GSgfx init */
                         u32 c, u32 d, u32 e);
extern u32  fn_800A0F58(void);                         /* OSGetResetSwitchState (checks warm boot) */
extern void fn_800D37D4(u32, u32, u32, u32, u32, u32);/* GSgfx video mode config */
extern void fn_800EFFC0(u32 numEntries);               /* GX FIFO init */
extern void fn_80191484(u32 numSounds);                 /* Sound system init */
extern void fn_800E4D3C(u32 maxObjects);                /* GSmem object pool */
extern void fn_800EE880(u32 param);                     /* VI callback setup */
extern void fn_800DF854(u32 bufSize);                   /* Display list buffer init */
extern void fn_800D2AD4(u32 count);                     /* GSgfx light init */
extern void fn_800DD0B8(u32 count);                     /* GSgfx texture init */
extern void fn_80119824(u32 a, u32 b);                  /* GSmaterial init */
extern void fn_80132C6C(u32 a, u32 b, u32 c, u32 d);  /* GStexture cache init */
extern void fn_800F8138(void);                           /* GSgfx camera/viewport init */

extern void fn_800F80B0(u32 padIdx);                    /* PAD motor init per-controller */
extern void fn_800F7E9C(u32 padIdx, u32 param);        /* PAD rumble config */
extern void fn_800F7E40(u32 padIdx, u32 param);        /* PAD deadzone config */
extern void fn_800F7DE4(u32 padIdx, u32 param);        /* PAD stick config */

extern void fn_800FE9B0(u32 numTasks, u32 numQueues);  /* GSthread init */
extern void fn_800F9670(u32 maxSteps);                  /* GSthread step limit */
extern void fn_800FF828(u32 a, u32 b, u32 c, u32 d);  /* GSthread pool config */
extern void fn_8010D170(void);                           /* GSthread scheduler init */
extern void fn_800F7758(u32 maxPads);                   /* PAD system init */
extern void fn_800F75FC(void* padTable);                 /* PAD set mapping table */
extern void fn_800FC528(u32 numFloors, u32 numLayers);  /* GSfloor system init */
extern void fn_800FC518(void* sndTable);                 /* GSfloor sound table */
extern void fn_800FC39C(void* relData);                  /* GSfloor register REL data */
extern void fn_800FC244(void* relData);                  /* GSfloor register REL data (alt) */
extern void fn_800F76E4(void* relData);                  /* GSfloor register REL data (floor) */
extern void fn_80167DC0(u32 a, u32 b, u32 c, u32 d, u32 e); /* Script/event system init */
extern void fn_801E1300(void);                           /* Save/card system init */
extern void fn_801ED740(void);                           /* GBA communication init */
extern void fn_801E1B2C(void);                           /* Save data init */
extern void fn_80103CD8(u32 param);                      /* Effect system init */
extern void fn_80101FB8(u32 param);                      /* Particle system init */
extern void fn_800D3074(u32 flag);                       /* GSgfx enable rendering */

extern void fn_800FE834(u32 active, u32 taskId, u32 param, /* GSthread create task */
                         void* func);
extern void fn_800FE7A0(void);                           /* GSthread yield / run scheduler */
extern void GSthreadCreate(u32 affinity, u32 priority,      /* GSthread create main thread */
                         u32 stackSize, u32 usesFPU,
                         u32 autoStart, void* entry);

extern void fn_801E12A0(void);                           /* Save system post-init */
extern void fn_801C47D0(void);                           /* Script engine init */
extern void fn_80168638(u32 numSlots);                   /* Floor/scene loader init */
extern void fn_80130CE0(u32 maxEffects);                 /* 3D model/effect loader */
extern u32  fn_800E0DDC(void);                           /* OSGetFreeMemSize-like */
extern void fn_800DD970(const char* fmt, ...);           /* OSReport / debug printf */
extern f32 fn_800C46B0(f32 volume);                     /* volume clamp/process */

/* --- Game main loop (fn_80005AAC) and its helpers --- */
void fn_80005AAC(void);
void fn_8000609C(void); /* fn_8000609C - background color / render schedule init */

/* --- Per-frame callback tasks registered by fn_800057B0 --- */
void fn_80005D80(void);       /* fn_80005D80 - primary retrace handler */
void fn_80005FFC(void);      /* fn_80005FFC - controller polling task */
void fn_8000604C(void);    /* fn_8000604C - controller rumble task */
void fn_80005FA8(u32 a, u32 b); /* fn_80005FA8 - audio retrace callback */
void fn_80005CE4(void);    /* fn_80005CE4 - reset button countdown */
void fn_80005E00(void);  /* fn_80005E00 - per-retrace game logic */
void fn_80005C3C(void); /* fn_80005C3C - reset button handler */

/* --- Small utility functions near main --- */

/* fn_800056C4: fn_800056C4 - sets a byte flag; returns previous value */
extern u8 fn_800056C4(u8 newVal);

/* fn_800056D4: fn_800056D4 - reads flag, clears it, returns old */
extern u8 fn_800056D4(void);

/* fn_800056E4: fn_800056E4 */
extern void fn_800056E4(u8 val);

/* fn_800056EC: fn_800056EC - if sound system present, set volume */
extern void fn_800056EC(f64 volume);

/* fn_80005748: fn_80005748 - if sound system present, get volume */
extern f32 fn_80005748(void);

/* fn_800057A0: fn_800057A0 - returns 1 */
extern u32 fn_800057A0(void);

/* fn_800057A8: fn_800057A8 - returns 6 */
extern u32 fn_800057A8(void);

/* --- Miscellaneous engine functions --- */
extern void fn_800366A8(void);    /* Scene/floor update tick */
extern u32  fn_800FF81C(void* a, u32 b); /* GSthread set frame counter */
extern void* fn_800F9544(u32 a, u32 b, u32 c, u32 d); /* GSthread alloc work area */

extern u32  fn_80128E24(void);    /* SoundSystemIsReady */
extern void* fn_80128E04(void);   /* SoundSystemGetContext */
extern void* fn_80135CD0(void);   /* SoundGetMixer */
extern void fn_80135B5C(void* mixer, f64 volume); /* SoundSetMasterVolume */
extern f32  fn_80135C10(void* mixer);  /* SoundGetMasterVolume */

extern u32  fn_80128E2C(void);     /* RNG get seed */
extern void fn_80128E14(u32 seed); /* RNG set seed */
extern void fn_80128E38(u32 a, u32 b); /* RNG init */
extern void fn_801EF5C0(void);    /* Battle system global init */
extern u32  fn_80129280(u32 a, u32 b); /* RNG or calendar read */
extern void fn_801909A8(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f); /* Calendar/RTC set */
extern void fn_801E1274(void);    /* Card system tick */
extern void fn_8010C220(void);    /* Effect system tick */
extern void fn_80179BEC(void);    /* World/map system init */
extern void fn_8013024C(void);    /* 3D model system init */
extern void fn_8018E920(u32 a);   /* Pokemon data/model init */
extern void fn_801ED388(void);    /* GBA link init */
extern void fn_801D0A30(void);    /* Menu/UI system init */
extern void fn_801128A0(void);    /* Colosseum mode init */
extern void* tableResBiosGetResPtr(u32 id);  /* Load relocatable module (REL) by ID */

extern void* fn_801664F0(void* color); /* Set clear/background color */
extern void fn_800F9378(void* work, u32 time, u32 numFrames, u32 arg); /* Thread wait/schedule */

extern u32  fn_800A03B4(void);    /* OSGetResetButtonState */
extern int  OSGetResetButtonState(void); /* real symtab name (signed return -> cmpwi) */
extern void VIFlush(void);               /* real symtab name for fn_800AA068 call site */
extern void fn_80166E44(void);    /* Screen fade to black */
extern void fn_800AAE34(u32 mask);/* VISetBlack / video blank */
extern void fn_800AA204(u32 flag);/* VIFlush */
extern void fn_800AA068(void);    /* VIWaitForRetrace */
extern void fn_800A880C(u32 a);   /* AISetDSPSampleRate */
extern void fn_800A8850(u32 a);   /* AIStopDMA */
extern void fn_800A263C(u32 a, u32 b, u32 c, u32 d); /* OSClearStack or thread cleanup */
extern void fn_800A8FE4(void);    /* AIReset */

extern void fn_800F0448(void);    /* GSthread begin frame */
extern u8   fn_8000DAA8(void);    /* IsLanguageJapanese or locale check */
extern void fn_80181850(u32 a, u32 b); /* Retry/reconnect handler */

extern u8   fn_8008ABA0(u32 padIdx); /* PAD is connected check */
extern void fn_800F7F64(u32 padIdx); /* PAD disconnect handler */
extern u8 fn_800F7EF8(u32 padIdx); /* PAD connection check */
extern void fn_8008AC34(u32 padIdx); /* PAD recalibrate */

extern void fn_80101B90(u32 mask);   /* GXSetChanCtrl / GX render state */
extern void fn_80101D8C(void);       /* GX render flush */
extern void fn_80101D5C(void);       /* GX render end */
extern void fn_800D361C(u8 mode);    /* GSgfx set draw mode */
extern void fn_800D30F0(u32 flag);   /* GSgfx swap buffers */
extern void fn_80175F6C(void);       /* World/scene render tick */

extern void fn_801E0FB4(s32 unk, u32 a, u32 b); /* Save/card per-frame update */

extern u8   fn_801E11E8(void);   /* Card system check pending */
extern s32  fn_801E11E0(void);   /* Card system get state */
extern void fn_801E11B0(void);   /* Card system process state 2 */
extern void fn_801E119C(void);   /* Card system process other */
extern void fn_801E118C(void);   /* Card system finalize */

extern void fn_801028B0(void);   /* Particle system update */
extern void fn_800FBF10(void);   /* GSthread sync / process tasks */

extern u32  fn_801906A0(u32 evtId);  /* Event system check state */
extern s32  fn_800D37CC(void);       /* GSgfx get frame counter */
extern u32  fn_800D3088(void);       /* GSgfx get tick counter */

extern u32  fn_800F7AF0(u32 padIdx); /* PAD get buttons pressed */
extern u32  fn_800F7BC4(u32 padIdx); /* PAD get buttons held */
extern void fn_8000C0DC(void);       /* Debug menu toggle */

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

/* lbl_8047B6A0 @sda21 - float constant 0.0f (used for volume reset) */
extern f32 lbl_8047B6A0;
/* lbl_8047B6A4 @sda21 - float constant (volume cap) */
extern f32 lbl_8047B6A4;
/* lbl_8047B6A8 @sda21 - double constant for int-to-float conversion */
extern f64 lbl_8047B6A8;
/* lbl_8047B6B0 @sda21 - double constant for uint-to-float conversion */
extern f64 lbl_8047B6B0;
/* lbl_8047B6B8 @sda21 - small string, source file name for error print */
extern char lbl_8047B6B8[8];
/* lbl_8047B6C0 @sda21 - small format string for the error name line */
extern char lbl_8047B6C0[8];

/* --- Data labels referenced by init code --- */
extern u8 lbl_8039A700[];   /* ARAM base for ARInit */
extern u8 lbl_802E1CF0[];   /* PAD mapping table */
extern u8 lbl_80363778[];   /* GSfloor sound table */
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

/* fn_8009F488 = __OSInitMemoryProtection region setup */
extern void fn_8009F488(u32 region, u32 addr, u32 size, u32 perm);

/* fn_800A2C58 = __DBIsExceptionMarked or similar */
extern s32  fn_800A2C58(void);

/* fn_800A19CC = OSCreateThread */
extern void fn_800A19CC(void* thread, void* func, void* arg,
                         void* stackBase, u32 stackSize,
                         u32 priority, u32 detached);
/* fn_800A1F94 = OSResumeThread */
extern void fn_800A1F94(void* thread);
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

/* fn_800C8600 = vsprintf */
extern int  fn_800C8600(char* buf, const char* fmt, va_list ap);
/* fn_800C8520 = sprintf */
extern int  fn_800C8520(char* buf, const char* fmt, ...);

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
 *  subsystem, graphics, and then calls fn_800057B0() which never returns
 *  (it enters the main loop).
 * =========================================================================
 */
#pragma push
#pragma scheduling on
#pragma peephole off
int main(int argc, char** argv) {
    void* arenaLo;
    void* arenaHi;
    void* heapStart;

    /* Clear low-memory scratch area at 0x80001800, size 0x1800 */
    memset((void*)0x80001800, 0, 0x1800);

    /* Install OS error handlers before anything else */
    fn_80006250(); /* fn_80006250 */

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
    fn_800E3568(0, arenaLo, heapStart);

    /* Set heap 0 as the default allocation heap */
    fn_800E3560(0);

    /* Create OS heap from [heapStart, arenaHi) for system allocations,
     * then move arena low past our heaps */
    OSSetArenaLo(fn_8009AB60(heapStart, OSGetArenaHi(), 1));

    /* Initialize ARAM (audio RAM) */
    ARInit(&lbl_80478DC0, 2);

    /* Initialize DVD filesystem */
    fn_800AE5C0();

    /* Initialize AI with stack at lbl_8039A700 + 0x3FF8 */
    AIInit((void*)(lbl_8039A700 + 0x3FF8));

    /* Initialize XFB (external framebuffer): 0x10000 bytes, 1 buffer */
    fn_800DDF54(0x10000, 1);

    /* Initialize scratch allocator */
    GSscratchInit(0);

    /* Parse command-line arguments (from disc header / apploader) */
    fn_80167FA4(argc, argv, 1);

    /* Enter the game initialization and main loop (does not return) */
    fn_800057B0(); /* fn_800057B0 */
}
#pragma pop

/* =========================================================================
 *  fn_800056C4 / fn_800056C4
 *  Address: 0x800056C4, Size: 0x10
 *
 *  Atomically sets the pause flag and returns the previous value.
 * =========================================================================
 */
u8 fn_800056C4(u8 newVal) {
    u8 oldVal = lbl_80478DC9;
    lbl_80478DC9 = newVal;
    return oldVal;
}

/* =========================================================================
 *  fn_800056D4 / fn_800056D4
 *  Address: 0x800056D4, Size: 0x10
 *
 *  Reads the pause flag, clears it to 0, returns the old value.
 * =========================================================================
 */
u8 fn_800056D4(void) {
    u8 val = lbl_80478DC9;
    lbl_80478DC9 = 0;
    return val;
}

/* =========================================================================
 *  fn_800056E4 / fn_800056E4
 *  Address: 0x800056E4, Size: 0x8
 *
 *  Stores a flag controlling whether controller rumble is active.
 * =========================================================================
 */
void fn_800056E4(u8 val) {
    lbl_80478DC8 = val;
}

/* =========================================================================
 *  fn_800056EC / fn_800056EC
 *  Address: 0x800056EC, Size: 0x5C
 *
 *  If the sound system is initialized, sets the master volume on the
 *  sound mixer. The volume parameter is a double (f1 on PPC).
 * =========================================================================
 */
void fn_800056EC(f64 volume) {
    void* mixer;

    /* Check if sound system is ready */
    if (fn_80128E24() != 0 && fn_80128E04() != 0) {
        mixer = fn_80135CD0();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL) {
        fn_80135B5C(mixer, volume);
    }
}

/* =========================================================================
 *  fn_80005748 / fn_80005748
 *  Address: 0x80005748, Size: 0x58
 *
 *  Returns the current master volume from the sound mixer.
 *  Falls back to 0.0f if the sound system is not available.
 * =========================================================================
 */
f32 fn_80005748(void) {
    void* mixer;
    f32 volume;

    if (fn_80128E24() != 0 && fn_80128E04() != 0) {
        mixer = fn_80135CD0();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL) {
        volume = fn_80135C10(mixer);
    } else {
        volume = lbl_8047B6A0; /* 0.0f */
    }

    return fn_800C46B0(volume); /* Some processing/clamping of the volume value */
}

/* =========================================================================
 *  fn_800057A0 / fn_800057A0
 *  Address: 0x800057A0, Size: 0x8
 *
 *  Returns the major version number: 1
 * =========================================================================
 */
u32 fn_800057A0(void) {
    return 1;
}

/* =========================================================================
 *  fn_800057A8 / fn_800057A8
 *  Address: 0x800057A8, Size: 0x8
 *
 *  Returns the minor version number: 6
 *  (Pokemon Colosseum version 1.6)
 * =========================================================================
 */
u32 fn_800057A8(void) {
    return 6;
}

/* =========================================================================
 *  fn_800057B0 / fn_800057B0
 *  Address: 0x800057B0, Size: 0x2FC
 *
 *  Master initialization function. Sets up every game subsystem, then
 *  creates the main game thread and enters an infinite yield loop.
 *
 *  This function never returns.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_800057B0(void) {
    u8 isWarmBoot;
    s32 i;

    /* Set render timing to 20Hz (50ms per frame for init) */
    GSthread(0x14);

    /* Initialize the GSgfx graphics state machine:
     *   memSize = 0x6DDD0 (450000 bytes)
     *   fifoSize = 16, matrixStackDepth = 8, lightCount = 32,
     *   useDoubleBuf = 1, displayListSize = 0x1E0 */
    fn_800D39E0(0x6DDD0, 0x10, 0x8, 0x20, 0x1, 0x1E0);

    isWarmBoot = 0;

    /* Check if this is a warm boot (reset from game) */
    if (fn_800A0F58() == 1) {
        /* If reset code's top bit indicates "return to game" */
        BOOL isReset = ((OSGetResetCode() + 0x80000000) == 0) ? TRUE : FALSE;
        if (isReset == TRUE) {
            isWarmBoot = 1;
        }
    }

    /* On warm boot, reconfigure video mode */
    if (isWarmBoot) {
        fn_800D37D4(1, 2, 0, 2, 1, 0);
    }

    /* Initialize GX command FIFO with 16 entries */
    fn_800EFFC0(0x10);

    /* Initialize sound system with 8 channels */
    fn_80191484(0x8);

    /* Initialize GSmem object pool for 64 objects */
    fn_800E4D3C(0x40);

    /* Initialize VI retrace callback system with 8 callbacks */
    fn_800EE880(0x8);

    /* Initialize display list buffer: 1024 bytes */
    fn_800DF854(0x400);

    /* Initialize lighting system with 32 lights */
    fn_800D2AD4(0x20);

    /* Initialize texture system with 32 textures */
    fn_800DD0B8(0x20);

    /* Initialize material system: 32 materials, 128 max textures */
    fn_80119824(0x20, 0x80);

    /* Initialize texture cache: 16 slots, 64 entries, 1 bank, 8192 byte cache */
    fn_80132C6C(0x10, 0x40, 0x1, 0x2000);

    /* Initialize camera/viewport defaults */
    fn_800F8138();

    /* Configure all 4 controller pads (indices 1-4) */
    for (i = 1; i <= 4; i++) {
        fn_800F80B0(i);        /* Init motor for pad i */
        fn_800F7E9C(i, 2);    /* Set rumble mode */
        fn_800F7E40(i, 7);    /* Set analog deadzone */
        fn_800F7DE4(i, 1);    /* Set stick mode */
    }

    /* Initialize GSthread system: 16 tasks, 4 queues */
    fn_800FE9B0(0x10, 0x4);

    /* Set thread step limit to 300 */
    fn_800F9670(0x12C);

    /* Configure thread pool: 4 threads, 16 priority levels each */
    fn_800FF828(0x4, 0x10, 0x10, 0x10);

    /* Initialize scheduler */
    fn_8010D170();

    /* Initialize PAD system for up to 16 pads */
    fn_800F7758(0x10);

    /* Set controller mapping table */
    fn_800F75FC(lbl_802E1CF0);

    /* Initialize floor/scene system: 2 floors, 5 layers */
    fn_800FC528(0x2, 0x5);

    /* Register floor sound table */
    fn_800FC518(lbl_80363778);

    /* Register floor REL data tables */
    fn_800FC39C(lbl_8027A500); /* Main scene data */
    fn_800FC39C(lbl_802BD260); /* Scene data #2 */
    fn_800FC39C(lbl_802C0CB0); /* Scene data #3 */
    fn_800FC244(lbl_802CF810); /* Scene data #4 (alt registration) */

    /* Initialize the script/event system with event ID ranges */
    fn_80167DC0(0x3BE8, 0x3BEA, 0x3BED, 0x3BEF, 0x3BF2);

    /* Initialize save/card system */
    fn_801E1300();

    /* Initialize GBA communication system */
    fn_801ED740();

    /* Initialize save data structures */
    fn_801E1B2C();

    /* Initialize 3D effect system: 24 max effects */
    fn_80103CD8(0x18);

    /* Initialize particle system: 4 emitters */
    fn_80101FB8(0x4);

    /* Enable rendering pipeline */
    fn_800D3074(1);

    /* ---------------------------------------------------------------
     * Register per-frame callback tasks.
     * fn_800FE834 creates a task: (active, taskId, param, func)
     * ---------------------------------------------------------------
     */

    /* Task 0xFF: Primary VBlank handler (fn_80005D80) */
    fn_800FE834(1, 0xFF, 0, (void*)fn_80005D80);

    /* Task 0x00: Pad rumble update (fn_8000604C) */
    fn_800FE834(1, 0x00, 0, (void*)fn_8000604C);

    /* Task 0x01: Pad read/polling (fn_80005FFC) */
    fn_800FE834(1, 0x01, 0, (void*)fn_80005FFC);

    /* Task 0x0A: Audio retrace callback (fn_80005FA8) */
    fn_800FE834(1, 0x0A, 0, (void*)fn_80005FA8);

    /* Post-init for save system */
    fn_801E12A0();

    /* Task 0xFD: Reset button countdown (fn_80005CE4) */
    fn_800FE834(1, 0xFD, 0, (void*)fn_80005CE4);

    /* Task 0xFE: Main retrace logic (fn_80005E00) */
    fn_800FE834(1, 0xFE, 0, (void*)fn_80005E00);

    /* Task 0xFF: Reset handler (fn_80005C3C) - overrides earlier 0xFF? */
    fn_800FE834(1, 0xFF, 0, (void*)fn_80005C3C);

    /* Mark initialization as complete */
    lbl_80478DC9 = 1;

    /* Initialize script engine */
    fn_801C47D0();

    /* Initialize floor/scene loader with 8 slots */
    fn_80168638(0x8);

    /* Initialize model/effect loader: 32 max */
    fn_80130CE0(0x20);

    /* Print available memory to debug console */
    {
        u32 freeMem = (u32)fn_800E0DDC();
        fn_800DD970(lbl_80266420, freeMem); /* "OS avail memory: %d\n" */
    }

    /* Create and start the main game loop thread:
     *   affinity=0, priority=1000, stackSize=0x4000,
     *   usesFPU=1, autoStart=1, entry=fn_80005AAC */
    GSthreadCreate(0, 0x3E8, 0x4000, 1, 1, (void*)fn_80005AAC);

    /* Infinite yield loop - scheduler takes over from here */
    for (;;) {
        fn_800FE7A0(); /* GSthread yield */
    }
}
#pragma pop

/* =========================================================================
 *  fn_80005AAC / fn_80005AAC
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
void fn_80005AAC(void) {
    void* mixer;

    /* Tick the scene/floor system */
    fn_800366A8();

    /* Update frame counter from scene state */
    {
        u32* framePtr = lbl_80478FB8;
        void* sceneState = lbl_80478FBC;
        fn_800FF81C(sceneState, *framePtr);
    }

    /* Allocate a 2-byte work area (for some per-frame state) */
    {
        void* work = fn_800F9544(2, 0, 2, 0);
        *(u8*)(work) = 0;
        *((u8*)(work) + 1) = 0;
    }

    /* Clear draw mode flag */
    lbl_80478DD0 = 0;

    /* If sound system is ready, reset master volume to 0.0 */
    if (fn_80128E24() != 0 && fn_80128E04() != 0) {
        mixer = fn_80135CD0();
    } else {
        mixer = NULL;
    }
    if (mixer != NULL) {
        fn_80135B5C(mixer, (f64)lbl_8047B6A0); /* 0.0f */
    }

    /* Reset rumble flag */
    lbl_80478DC8 = 0;

    /* Load REL modules and register their scene/floor data.
     * tableResBiosGetResPtr loads a relocatable module by ID and returns a pointer
     * to its data, which is then registered with the floor system. */
    fn_800FC39C(tableResBiosGetResPtr(1));  /* REL 1 -> register scene data */
    fn_800FC244(tableResBiosGetResPtr(4));  /* REL 4 -> register scene data (alt) */
    fn_800F76E4(tableResBiosGetResPtr(7));  /* REL 7 -> register floor data */

    /* Seed the RNG using the low bits of the OS tick counter.
     * OSGetTime returns u64 in r3:r4; we use r4 (low 32 bits).
     * rlwinm r27, r4, 0, 21, 26 => r4 & 0x000007E0 */
    fn_80128E14(fn_80128E2C() + ((u32)OSGetTime() & 0x7E0)); /* add time entropy to seed */
    fn_80128E38(0, 0); /* Reset RNG sequence */

    /* Initialize the battle system */
    fn_801EF5C0();

    /* Set the in-game calendar from RTC values (sec/min/hour/day/month/year) */
    fn_801909A8(fn_80129280(0, 4), fn_80129280(0, 7), fn_80129280(0, 5),
                fn_80129280(0, 8), fn_80129280(0, 6), fn_80129280(0, 9));

    /* Tick save/card system */
    fn_801E1274();

    /* Tick effect system (no-op stub at this point) */
    fn_8010C220();

    /* Initialize background color / render schedule */
    fn_8000609C();

    /* Initialize world/map system */
    fn_80179BEC();

    /* Initialize 3D model system */
    fn_8013024C();

    /* Initialize Pokemon model/data system (48 slots) */
    fn_8018E920(0x30);

    /* Initialize GBA link subsystem */
    fn_801ED388();

    /* Initialize menu/UI system */
    fn_801D0A30();

    /* Initialize Colosseum battle mode */
    fn_801128A0();

    /* Clear low-memory scratch area 0x80001803, size 0x17FD
     * (preserves bytes 0x1800-0x1802 which contain status flags) */
    memset((void*)0x80001803, 0, 0x17FD);
}

/* =========================================================================
 *  fn_80005C3C / fn_80005C3C
 *  Address: 0x80005C3C, Size: 0xA8
 *
 *  Checks the reset button state. On first press, latches.
 *  On release (after latch), if init is complete, performs a full
 *  hardware shutdown and calls OSResetSystem.
 * =========================================================================
 */
void fn_80005C3C(void) {
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
                fn_80166E44();           /* Fade screen to black */
                fn_800AAE34(0xF0000000); /* VISetBlack */
                fn_800AA204(1);          /* VIFlush */
                VIFlush();               /* real symtab: VIFlush (was fn_800AA068) */
                fn_800A880C(0);          /* Stop DSP sample rate */
                fn_800A8850(0);          /* Stop DMA */
                fn_800A263C(0, 0, 0, 0); /* Clean up threads */
                fn_800A8FE4();           /* Reset audio */
                OSResetSystem(0, 0, 1);  /* Reset to system menu */
            }
        }
    }
}

/* =========================================================================
 *  fn_80005CE4 / fn_80005CE4
 *  Address: 0x80005CE4, Size: 0x9C
 *
 *  On first call (lbl_80478DCC == 0), clears OS exception vectors by
 *  using the table at lbl_802663A0, then clears low-memory scratch.
 *  Sets a countdown timer of 100 frames, decremented each call.
 *  This provides a delay before the game is fully ready.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005CE4(void) {
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
#pragma pop

/* =========================================================================
 *  fn_80005D80 / fn_80005D80
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
#pragma push
#pragma scheduling off
void fn_80005D80(void) {
    /* Set render state mask (0xFF00 = enable all channels) */
    fn_80101B90(0xFF00);

    /* Apply the draw mode from the per-frame flag */
    fn_800D361C(lbl_80478DD0);

    /* Flush GX render commands */
    fn_80101D8C();

    /* Clear swap flag */
    fn_800D30F0(0);

    /* Tick world/scene renderer */
    fn_80175F6C();

    /* Update save/card system based on lbl_80478820 flag */
    if (lbl_80478820 == 0) {
        fn_801E0FB4(0x10, 1, 1);
    } else {
        fn_801E0FB4(-1, 1, 1);
    }

    /* Finalize GX render state (0xFFFF00 = full state mask) */
    fn_80101B90(0xFFFF00);

    /* End GX render pass */
    fn_80101D5C();
}
#pragma pop

/* =========================================================================
 *  fn_80005E00 / fn_80005E00
 *  Address: 0x80005E00, Size: 0x1A8
 *
 *  Per-retrace game logic handler. Checks controller buttons for
 *  debug menu activation, handles save/card state transitions,
 *  and processes particle effects and sound.
 *
 *  Also handles reading volume from the sound mixer and applying
 *  time-based adjustments.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005E00(void) {
    void* mixer;
    f32 vol;
    f32 v;

    fn_80101B90(0xFF);

    if (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x10) {
        fn_8000C0DC();
    }

    if (fn_801E11E8()) {
        if (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x40) {
            if (fn_801E11E0() == 2) {
                fn_801E11B0();
            } else {
                fn_801E119C();
            }
        }
        if (fn_800F7BC4(1) & fn_800F7AF0(1) & 0x20) {
            fn_801E118C();
        }
    }

    fn_801028B0();
    fn_800FBF10();

    if (fn_80128E24() != 0 && fn_80128E04() != 0) {
        mixer = fn_80135CD0();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL && lbl_80478DC8 == 1 && fn_801906A0(0x8AE) == 0) {
        vol = fn_80135C10(mixer);
        v = vol + (f32)fn_800D3088() / (f32)fn_800D37CC();
        if (v >= lbl_8047B6A4) {
            v = lbl_8047B6A4;
        }
        fn_80135B5C(mixer, v);
    }
}
#pragma pop

/* =========================================================================
 *  fn_80005FA8 / fn_80005FA8
 *  Address: 0x80005FA8, Size: 0x54
 *
 *  Audio retrace callback. Called with (error, context) from the task
 *  system. Updates the audio frame, then checks the locale flag to
 *  potentially trigger a reconnect handler.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005FA8(u32 a, u32 b)
{
    fn_800F0448();
    if (fn_8000DAA8() == 0) {
        fn_80181850(a, b);
    }
}
#pragma pop

/* =========================================================================
 *  fn_80005FFC / fn_80005FFC
 *  Address: 0x80005FFC, Size: 0x50
 *
 *  Controller polling task. Iterates pads 1-4:
 *  if a pad is NOT responding to fn_800F7EF8 (connection check),
 *  it calls fn_8008AC34 to recalibrate that pad.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005FFC(void) {
    s32 i;

    for (i = 1; i <= 4; i++) {
        if (fn_800F7EF8(i) == 0) {
            fn_8008AC34(i);
        }
    }
}
#pragma pop

/* =========================================================================
 *  fn_8000604C / fn_8000604C
 *  Address: 0x8000604C, Size: 0x50
 *
 *  Controller rumble update task. Iterates pads 1-4:
 *  if a pad is NOT responding to fn_8008ABA0 (motor check),
 *  it calls fn_800F7F64 to reset that pad's rumble state.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_8000604C(void) {
    s32 i;

    for (i = 1; i <= 4; i++) {
        if (fn_8008ABA0(i) == 0) {
            fn_800F7F64(i);
        }
    }
}
#pragma pop

/* =========================================================================
 *  InitBackgroundColor / fn_8000609C
 *  Address: 0x8000609C, Size: 0x54
 *
 *  Copies the default background color from rodata (lbl_80266438) into
 *  a local buffer and calls the floor system to set the clear color.
 *  Also configures a render schedule with a 2000-tick timeout.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_8000609C(void) {
    struct BgColor12 { u32 c[3]; } buf;
    buf = *(struct BgColor12*)lbl_80266438;
    fn_800F9378(fn_801664F0(&buf), 0, 0x7D0, 0);
}
#pragma pop

/* =========================================================================
 *  fn_80006250 / fn_80006250
 *  Address: 0x80006250, Size: 0x128
 *
 *  Sets up memory protection regions and installs a custom error handler
 *  (fn_80006378 / fn_80006378) for all relevant OS exception types:
 *    0  = SYSTEM_RESET
 *    1  = MACHINE_CHECK
 *    2  = DSI
 *    3  = ISI
 *    5  = ALIGNMENT
 *    11 = FLOATING_POINT
 *    13 = PERFORMACE_MONITOR
 *    14 = BREAKPOINT
 *    15 = SYSTEM_INTERRUPT
 *    16 = THERMAL_INTERRUPT
 *  If the debugger is not connected (fn_800A2C58 returns 0), also
 *  installs handlers for:
 *    6  = PROGRAM
 *    10 = SYSTEM_CALL
 *    12 = TRACE
 *
 *  Memory protection regions configured:
 *    Region 0: 0x00000000, size 0x80000000, permission 3 (RW)
 *    Region 1: 0x81800000, size 0x01800000, permission 3 (RW)
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80006250(void) {
    /* Set up memory protection regions */
    fn_8009F488(0, 0, 0x80000000, 3);
    fn_8009F488(1, 0x81800000, 0x01800000, 3);

    /* Install custom error handler for standard exceptions */
    OSSetErrorHandler(0, fn_80006378);   /* SYSTEM_RESET */
    OSSetErrorHandler(1, fn_80006378);   /* MACHINE_CHECK */
    OSSetErrorHandler(2, fn_80006378);   /* DSI */
    OSSetErrorHandler(3, fn_80006378);   /* ISI */
    OSSetErrorHandler(5, fn_80006378);   /* ALIGNMENT */
    OSSetErrorHandler(11, fn_80006378);  /* FLOATING_POINT */
    OSSetErrorHandler(13, fn_80006378);  /* PERFORMACE_MONITOR */
    OSSetErrorHandler(14, fn_80006378);  /* BREAKPOINT */
    OSSetErrorHandler(15, fn_80006378);  /* SYSTEM_INTERRUPT */

    /* Store max error handler index */
    lbl_80478990 = 16;
    OSSetErrorHandler(16, fn_80006378);  /* THERMAL_INTERRUPT */

    /* If debugger is not attached, also handle program/trace/syscall */
    if (fn_800A2C58() == 0) {
        OSSetErrorHandler(6, fn_80006378);   /* PROGRAM */
        OSSetErrorHandler(10, fn_80006378);  /* SYSTEM_CALL */
        OSSetErrorHandler(12, fn_80006378);  /* TRACE */
    }
}
#pragma pop

/* =========================================================================
 *  fn_80006378 / fn_80006378
 *  Address: 0x80006378, Size: 0x128
 *
 *  Custom OS error handler. Called on any unhandled exception.
 *  Saves the error number, copies the OS context, formats a crash
 *  report using the error name string table, installs a recovery
 *  exception handler, enables the scheduler, then creates a thread
 *  to display the error on screen.
 * =========================================================================
 */
typedef struct ErrVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} ErrVaList;
typedef ErrVaList ErrVaListArray[1];

typedef struct ErrThread {
    u64 pad[0x318 / 8];
} ErrThread;

#pragma push
#pragma peephole off
void fn_80006378(u32 error, void* context, ...) {
    ErrThread thread;
    ErrVaListArray args;

    __builtin_va_info(&args);
    lbl_8047A264 = *(u32*)__va_arg(args, 1);
    lbl_8047A268 = *(u32*)__va_arg(args, 1);
    memcpy(lbl_803A1700, context, 0x2C8);
    lbl_8047A260 = (u16)error;
    __OSSetExceptionHandler(8, fn_800064A0);
    OSEnableScheduler();
    fn_800A19CC(&thread, fn_800064C4, NULL, lbl_8039E700 + 0x1FFC, 0x2000, 1, 0);
    fn_800A1F94(&thread);
    fn_800A1E54(&thread, 0);
}
#pragma pop

/* =========================================================================
 *  fn_800064A0 / fn_800064A0
 *  Address: 0x800064A0, Size: 0x24
 *
 *  Installed as exception handler 8 (decrementer). Simply loads
 *  the saved context to resume execution in the error display thread.
 * =========================================================================
 */
#pragma push
#pragma scheduling off
void fn_800064A0(u32 error, void* context) {
    OSLoadContext(context);
}
#pragma pop

/* =========================================================================
 *  fn_800064C4 / fn_800064C4
 *  Address: 0x800064C4, Size: 0x16C
 *
 *  Thread entry function that renders the crash report to screen.
 *  Reads the saved error number, context, and error name table to
 *  produce a formatted crash dump visible on the TV output.
 * =========================================================================
 */
#pragma push
#pragma peephole off
s32 fn_800064C4(void) {
    const char* strs = (const char*)lbl_80266448;
    s32 n;
    u32 i;
    u32* sp;
    s32 len;

    n = fn_800C8520((char*)lbl_803A0700, lbl_8047B6C0,
                    ((char**)lbl_802E2888)[lbl_8047A260]);
    len = n + fn_800C8520((char*)lbl_803A0700 + n, strs + 0x1CC,
                       *(u32*)(lbl_803A1700 + 0x84), *(u32*)(lbl_803A1700 + 0x80));
    len += fn_800C8520((char*)lbl_803A0700 + len, strs + 0x1EC,
                       *(u32*)(lbl_803A1700 + 0x198), *(u32*)(lbl_803A1700 + 0x19C));
    len += fn_800C8520((char*)lbl_803A0700 + len, strs + 0x20C,
                       lbl_8047A264, lbl_8047A268);
    len += fn_800C8520((char*)lbl_803A0700 + len, strs + 0x190);

    i = 0;
    sp = (u32*)*(u32*)(lbl_803A1700 + 0x4);
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 0x20) {
        len += fn_800C8520((char*)lbl_803A0700 + len, strs + 0x1B4,
                           sp, sp[0], sp[1]);
        sp = (u32*)sp[0];
    }

    fn_800060F0(lbl_8047B6B8, 0xD5, (char*)lbl_803A0700);
    return 0;
}
#pragma pop

/* ===== Phase 2 recovery stubs ===== */

/* fn_800060F0 - 0x800060F0 | size: 0x160 */
#if 0
asm void fn_800060F0(void) {
#include "src/game/main_fn_800060F0.inc"
}
#else
#pragma push
#pragma peephole off
void fn_800060F0(const char* file, s32 line, const char* fmt, ...) {
    va_list ap;
    int len;
    register u32 i;
    register u32* p;
    register int pos;
    u32 table = (u32)lbl_80266448;

    va_start(ap, fmt);
    len = fn_800C8600((char*)lbl_803A0700, fmt, ap);
    pos = len + fn_800C8520((char*)lbl_803A0700 + len, (const char*)(table + 0x178), file, line);
    pos += fn_800C8520((char*)lbl_803A0700 + pos, (const char*)(table + 0x190));
    for (i = 0, p = (u32*)OSGetStackPointer(); p && (u32)p != 0xFFFFFFFF && i++ < 0x20; p = (u32*)*p) {
        pos += fn_800C8520((char*)lbl_803A0700 + pos, (const char*)(table + 0x1B4), p, p[0], p[1]);
    }
    fn_800060F0(lbl_8047B6B8, 0x196, (char*)lbl_803A0700);
}
#pragma pop
#endif
