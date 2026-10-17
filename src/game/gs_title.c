/**
 * @file gs_title.c
 * @brief GStitle -- Title screen, autodemo, and intro sequence.
 *
 * Address range: 0x80020328 - 0x80026000 (~80 functions)
 *
 * This module manages the title screen and pre-game sequences:
 *   - Title logo display and animation
 *   - "Press Start" prompt with blink animation
 *   - Autodemo playback timer and trigger
 *   - New Game / Continue selection
 *   - Intro cutscene orchestration
 *   - Camera fly-through for title background
 *   - Sound effect scheduling for title events
 *
 * Key functions:
 *   fn_80020328  GStitle_Cleanup              -- 0x8C bytes, clean up title resources
 *   fn_800203B4  GStitle_MainLoop             -- 0xE8 bytes, title screen main loop
 *   fn_8002049C  GStitle_Init                 -- 0xF0 bytes, initialize title screen
 *   fn_8002058C  GStitle_ThreadEntry          -- 0x2C bytes, title thread entry point
 *   fn_800205B8  GStitle_SetMode              -- 8 bytes, set title mode (stw + blr)
 *   fn_800205C0  GStitle_SetDemoPtr           -- 8 bytes, set autodemo pointer
 *   fn_800205C8  GStitle_CheckAutodemo        -- 0x44 bytes, check autodemo timer
 *   fn_8002060C  GStitle_ResetTimer           -- 0xC bytes, reset autodemo countdown
 *   fn_80020618  GStitle_RenderFrame          -- 0x304 bytes, render one title frame
 *   fn_8002091C  GStitle_GetState             -- 0x10 bytes, return title state
 *   fn_8002092C  GStitle_AnimateLogo          -- 0x90 bytes, logo bounce/spin
 *   fn_800209BC  GStitle_AnimatePrompt        -- 0x90 bytes, "Press Start" blink
 *   fn_80020A4C  GStitle_AnimateBackground    -- 0x90 bytes, background scroll
 *   fn_80020ADC  GStitle_FadeIn               -- 0x58 bytes, fade from black
 *   fn_80020B34  GStitle_FadeOut              -- 0x58 bytes, fade to black
 *   fn_80020B8C  GStitle_GetFadeState         -- 0x14 bytes, return fade progress
 *   fn_80020BA0  GStitle_HandleInput          -- 0xFC bytes, Start/A button handler
 *   fn_80020C9C  GStitle_MenuSelect           -- 0x200 bytes, New/Continue/Options menu
 *   fn_80020E9C  GStitle_ReturnZero           -- 8 bytes, stub
 *   fn_80020EA4  GStitle_OptionsMenu          -- 0xB0 bytes, options sub-menu
 *   fn_80020F54  GStitle_SaveFileSelect       -- 0x19C bytes, save file selection
 *   fn_800210F0  GStitle_LoadSaveFile         -- 0x4D4 bytes, load save from memcard
 *   fn_800215C4  GStitle_NewGameSetup         -- 0x60 bytes, initialize new game
 *   fn_80021624  GStitle_SetDifficulty        -- 0x20 bytes, set game difficulty
 *   fn_80021644  GStitle_IntroSequence        -- 0x9C bytes, start intro sequence
 *   fn_800216E0  GStitle_SkipIntro            -- 8 bytes, set skip flag
 *   fn_800216E8  GStitle_NameEntry            -- 0x1D4 bytes, player name input screen
 *   fn_800218BC  GStitle_NameEntryInput       -- 0x1E0 bytes, keyboard input handler
 *   fn_80021A9C  GStitle_NameEntryConfirm     -- 0x78 bytes, confirm name
 *   fn_80021B14  (item-use handler)           -- 0x53C bytes, field item use on a party Pokemon (see note at definition)
 *   fn_80022050  GStitle_DrawKeyboard         -- 0x12C bytes, keyboard grid
 *   fn_8002217C  GStitle_DrawNamePreview      -- 0x2FC bytes, live name preview
 *   fn_80022478  (item-use handler)           -- 0x2A8 bytes, field item use dispatch (see note at definition)
 *   fn_80022720  GStitle_CameraFlythrough     -- 0x114 bytes, title camera movement
 *   fn_80022834  GStitle_CameraInterpolate    -- 0x308 bytes, camera spline interp
 *   fn_80022B3C  GStitle_ParticleEffects      -- 0x318 bytes, title particle system
 *   fn_80022E54  GStitle_GetEffectCount       -- 0x90 bytes, count active particles
 *   fn_80022EE4  GStitle_SpawnEffect          -- 0x184 bytes, spawn new particle
 *   fn_80023068  GStitle_UpdateEffects        -- 0x20C bytes, update particle positions
 *   cbForgetWazaSelect__FP7PokemonUsl  GStitle_DestroyEffect        -- 0x7C bytes, remove expired particle
 *   fn_800232F0  GStitle_SoundScheduler       -- 0x470 bytes, title BGM/SE scheduling
 *   fn_80023760  GStitle_CrossfadeBGM         -- 0x208 bytes, BGM crossfade
 *   fn_80023968  GStitle_PlayTitleBGM         -- 0x234 bytes, play title music
 *   fn_80023B9C  GStitle_StopTitleBGM         -- 0x20C bytes, fade out title music
 *   fn_80023DA8  GStitle_GetBGMState          -- 0x3C bytes
 *   fn_80023DE4  GStitle_GetSEState           -- 0x3C bytes
 *   fn_80023E20  GStitle_GetFadeState2        -- 0x3C bytes
 *   fn_80023E5C  GStitle_Nop                  -- 4 bytes, nop
 *   fn_80023E60  GStitle_AutodemoPlayback     -- 0x300 bytes, replay input recording
 *   fn_80024160  GStitle_AutodemoRecord       -- 0x1A8 bytes, record input (debug)
 *   fn_80024308  GStitle_AutodemoSetup        -- 0x130 bytes, setup autodemo
 *
 * fn_800203B4 (GStitle_MainLoop) structure:
 *   while (1) {
 *       if (gTitleState == 0x28) {  // TITLE_STATE_AUTODEMO_TIMEOUT
 *           floorLink(0x39C, 0);  // load autodemo camera
 *           gTitleState = 0x3E8;   // TITLE_STATE_PLAY_AUTODEMO
 *           continue;
 *       }
 *       if (!gIsInitialized) continue;
 *       if (gDemoPtr == NULL) continue;
 *       // Accumulate camera float position
 *       gCamAngle += gCamDelta;
 *       if (gCamPhase < 2 && !gPaused) {
 *           // Check distance threshold for camera movement
 *           if (dist >= threshold[gCamPhase]) {
 *               if (gCamPhase == 0)
 *                   fn_80166AB8(0x46E, 0, 0);  // Play whoosh SE
 *               else
 *                   fn_801669E4(0x46E, 0, 0);  // Play landing SE
 *               gCamPhase++;
 *           }
 *       }
 *       _threadSwitch();  // Frame advance
 *   }
 *
 * fn_8002049C (GStitle_Init):
 *   - Clears stale state flags
 *   - Creates the title thread via GSthreadCreate (priority 0x14, stack 0x2000)
 *   - Initializes camera position from lbl_803A1F88 float constants
 *   - Checks save file existence via fn_800FF548
 *   - Calls fn_8002060C if no save found
 *
 * SDA globals:
 *   lbl_8047A310: Title mode (s32)
 *   lbl_8047A314: Frame counter for autodemo timer
 *   lbl_8047A31C: Current title state (s32)
 *   lbl_8047A320: Initialized flag
 *   lbl_8047A324: Demo data pointer
 *   lbl_8047A328: Pause flag
 *   lbl_8047A32C: Thread completion flag (u8)
 *   lbl_8047A330: Title thread handle
 *   lbl_8047A33C: Camera phase counter
 *   lbl_8047A340: Camera angle accumulator (float)
 *   lbl_8047A344: Camera delta per frame (float)
 *   lbl_8047A350: Autodemo playback pointer
 *   lbl_80478878: Frame cycle counter (wraps at 4)
 *
 * BSS globals:
 *   lbl_803A1F88: Camera position array (4 entries, 4 floats each)
 *
 * Rodata (sdata2 float constants):
 *   lbl_8047B814: 0.0f (zero)
 *   lbl_8047B84C: Camera X start position
 *   lbl_8047B850: Camera Y start position
 *   lbl_8047B854: Camera Z position
 *   lbl_8047B858: Camera target X
 *   lbl_8047B85C: Camera target Y
 *   lbl_8047B860: Camera target Z
 *
 * --------------------------------------------------------------------------
 * XD-decomp cross-reference (TeamOrre/xd-decomp Pokemon XD: Gale of Darkness)
 *   Pokemon XD is the direct sequel built on the same Genius Sonority engine.
 *   XD's `game/menuTitle.cpp` (0x800A3150 - 0x800A423C, 0x10EC bytes) is the
 *   probable original-name analogue of THIS file's title-screen functions.
 *   XD is C++; Colosseum is C. Sizes differ because Colosseum's title is
 *   richer (camera fly-through, intro sequence, autodemo recording).
 *
 *   XD title symbols (selected, by behavior, NOT by byte match):
 *     menuTitleInit              0x104   - title screen initialization
 *     menuTitleMain              0x050   - per-frame main loop
 *     menuTitleFunc              0x0D8   - state dispatch
 *     menuTitle                  0x330   - top-level driver
 *     menuTitleHook              0x074
 *     menuTitleButton            0x07C
 *     menuTitleNormalButton      0x074
 *     menuTitleCursorAnime       0x0DC   - cursor blink/animation (~ fn_80024CDC)
 *     menuTitleCursorAnimeMain   0x07C
 *     menuTitleOpenSelectAnime   0x038
 *     menuTitleCloseSelectAnime  0x038
 *     menuTitlePlayBGM           0x040
 *     menuTitlePlayBGMWait       0x090
 *     menuTitleExit              0x074
 *     menuTitleSetSE             0x028
 *     menuTitleBattleSelectFree  0x034
 *     menuTitleBattlePreReadCheck 0x034
 *     menuTitlePreReadWait       0x030
 *     menuTitleReadWazaSelectMenu 0x038
 *     menuTitleSetStartStatus    0x008
 *     menuTitleClose2            0x054
 *     menuTitleOpenMenu          0x044
 *     menuTitleGetSelect / SetSelect  0x018 each
 *     menuTitlePreReadFightRegWzxData 0x058
 *     menuTitleOptionCtrl        0x0DC   - in separate menuTitleOption.cpp
 *     menuTitleOptionCursorControl 0x16C
 *
 *   HSD (HAL Sonata Driver) helpers used by both games:
 *     HSD_JObjSetupMatrix, HSD_JObjMtxIsDirty, modelIntpJObj*,
 *     cameraGetFrameCount, lightGetFrameCount, _modelLoad
 *
 *   Original-name candidates for our extern fn_XXXXXXXX:
 *     menuItemBiosGetPtr / menuDataBiosGetPtr -- linked-list/menu accessors
 *     fn_800D3088 / fn_800D37CC -- frame timing (returns u32/s32 ticks)
 *     fn_800E0CA0 / GSlerpGetLinearInterpolationVector -- vec3 transform helpers
 *     GSmsgGetGSchar / msgctrlSetValue -- message/window callbacks
 *     winMsgOpen / winMsgClose -- text print + wait
 *     fn_801902E0 -- config flag check (returns u8)
 *     fn_80165A20 / soundStop -- audio sequence control
 *     fadeCheck / fadeSet -- BGM volume/fade
 *
 * --------------------------------------------------------------------------
 */

#include "dolphin/types.h"

#if defined(GS_TITLE_SPLIT)
#pragma force_active on
#endif

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Thread management */
extern void* GSthreadCreate(s32 priority, void* stack, s32 stackSize,
                            s32 flags, s32 p5, void (*entry)(void));
extern void  _threadSwitch(void);
extern void  GSthreadTerminate(void* threadCtx);

/* Scene/camera */
extern void  floorLink(s32 cameraId, s32 mode);
extern u8    fn_800FF548(void);
extern void  std092_getCurrentRoomID(s32 floorId);

/* Sound */
extern void  soundStartRaw(s32 soundId, s32 p2, s32 p3);
extern void  soundStopRaw(s32 soundId, s32 p2, s32 p3);

/* Save system */
extern void  floorSetFadeScript(s32 p1, s32 p2);
extern void  menuCloseCustom(s32 menuId, s32 mode, s32 force);
extern void  winMsgClose(s32 wait);

/* Input */
extern void  fn_801EF644(s32 result);

/* Math */
extern f32   GSmodelGetAnimFrame(void* model);        /* Get distance */
extern f64 sin(f32);

/* Named asm symbols (used in inline asm .inc files) */
extern void menuModelSetMotion(void);
extern u8 menuSubOpenYesNo(s32, s32, s32, s32);
extern void itemParamConvertOrigFormat(void* dst, void* src);
extern u32 itemParamGetRecoverType(void* param);
extern s32 pcboxGetPokemonBoxNbEmptySlot(s32, s8);
extern void GSgfxBeginBackFBCapture(void);

/* =========================================================================
 * SDA globals
 * ========================================================================= */

extern s32   gTitleMode;         /* lbl_8047A310 */
extern s32   gTitleState;        /* lbl_8047A31C */
extern s32   gTitleInitFlag;     /* lbl_8047A320 */
extern void* gAutodemoPtr;       /* lbl_8047A324 */
extern s32   gTitlePaused;       /* lbl_8047A328 */
extern u8    gTitleThreadDone;   /* lbl_8047A32C */
extern void* gTitleThreadHandle; /* lbl_8047A330 */
extern s32   gCameraPhase;      /* lbl_8047A33C */
extern f32   gCameraAngle;      /* lbl_8047A340 */
extern f32   gCameraDelta;      /* lbl_8047A344 */
extern void* gDemoPlaybackPtr;   /* lbl_8047A350 */
extern s32   gFrameCycleCount;   /* lbl_80478878 */

/* =========================================================================
 * Function: GStitle_Init
 * Address:  0x8002049C
 * Size:     0xF0
 * ========================================================================= */

/* External declarations required by fn_8002049C.
 *
 * The sdata2 float anchors (lbl_8047B814..lbl_8047B860) and the sbss/bss
 * anchors (lbl_8047A344, lbl_803A1F88) reference data whose ownership lives
 * outside this translation unit (see split map + symbols.txt).
 * These externs are the standard access pattern for cross-TU sdata2/bss
 * anchors -- they do not create new data, they just name existing symbols.
 */
extern u8 lbl_8047A32C;
extern void* lbl_8047A330;
extern s32 lbl_8047A314;
extern void* lbl_8047A324;
extern s32 lbl_8047A31C;
extern s32 lbl_8047A320;
extern s32 lbl_8047A328;
extern s32 lbl_8047A33C;
extern f32 lbl_8047A340;
extern f32 lbl_8047A344;
extern u32 lbl_80478878;
extern u8 lbl_803A1F88[];
extern f32 lbl_8047B814;
extern f32 lbl_8047B84C;
extern f32 lbl_8047B850;
extern f32 lbl_8047B854;
extern f32 lbl_8047B858;
extern f32 lbl_8047B85C;
extern f32 lbl_8047B860;
extern u32 lbl_8047B808;
extern u32 lbl_8047B80C;
extern void* floorGetCurrentContext(void);

/* Forward declarations for same-TU callees used before their definitions. */
void fn_8002058C(void);
void fn_8002060C(void);
void fn_800216E8(void*, s32, u8*, s16, s32);
extern u8 pokemonIsDarkPokemon(s32);
extern f32 pokemonGetDp(void*);
extern u8 lbl_803A1B90[];

/* fn_8002049C now lives in gs_title_8001EF78.c (the head of this TU). */

/* =========================================================================
 * Function: GStitle_MainLoop
 * Address:  0x800203B4
 * Size:     0xE8
 * ========================================================================= */

/* fn_800203B4 now lives in gs_title_8001EF78.c (the head of this TU). */

/* =========================================================================
 * Function: GStitle_Cleanup
 * Address:  0x80020328
 * Size:     0x8C
 *
 * Cancels active events on slots 0x13, 0x15, 0x16, closes any open
 * message boxes, then waits for the title thread to complete.
 * ========================================================================= */

/* fn_80020328 now lives in gs_title_8001EF78.c (the head of this TU). */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 4 functions matched
 * =================================================================== */

extern u32 lbl_8047A310;
extern u32 lbl_8047A350;
extern u32 lbl_8047A358;

/* Address: 0x800205B8 | Size: 0x8 | Pattern: sda_setter */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800205B8)
void fn_800205B8(u32 val) {
    lbl_8047A310 = val;
}
#endif

/* Address: 0x800205C0 | Size: 0x8 | Pattern: sda_setter */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800205C0)
void fn_800205C0(u32 val) {
    lbl_8047A350 = val;
}
#endif

/* Address: 0x80020E9C | Size: 0x8 | Pattern: return_constant */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80020E9C)
u32 fn_80020E9C(void) { return 0; }
#endif

/* Address: 0x800216E0 | Size: 0x8 | Pattern: sda_setter */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800216E0)
void fn_800216E0(u32 val) {
    lbl_8047A358 = val;
}
#endif

extern void* menuItemBiosGetPtr(u32);
extern u32 msgGetCharsPerFrame(void);

/* Menu table at lbl_80478DDC (count at *lbl_80478DD8), 0x10 per entry. */
typedef struct TitleMenuEntry {
    u32 unk0;
    u32 id;
    u32 item;
    u32 value;
} TitleMenuEntry;

/* Title slot table at lbl_80478DE4 (count at *lbl_80478DE0), 0x28 per slot. */
typedef struct TitleSlot {
    u32 flag;
    u32 msg[9];
} TitleSlot;

/* The index-th visible item of the menu, or NULL. */
static inline u8* titleGetMenuItemAt(u8* menu, s32 index) {
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    u8* result;
    s32 count;
    u8* item;

    item = menuItemBiosGetPtr(*(s16*)(menuDataBiosGetPtr(*(u32*)(menu + 4)) + 4));
    count = 0;
    while (1) {
        if (((u32)*(volatile u8*)item >> 7) & 1) {
            if (index == count) {
                result = item;
                break;
            }
            count++;
        }
        if ((((u32)*(volatile u8*)item >> 6) & 1) == 0) {
            item = menuItemBiosGetPtr(*(s16*)(item + 0x18));
        } else {
            result = NULL;
            break;
        }
    }
    return result;
}

/* The visible item of the menu at the cursor position *indexp. */
static inline u8* titleGetMenuItem(u8* menu, u32* indexp) {
    s32 index = *indexp;

    return titleGetMenuItemAt(menu, index);
}

/* Index of the table entry whose item is the given one, or 0. */
static inline s32 titleFindEntry(u8* item) {
    extern u8* menuItemBiosGetPtr(s32);
    extern u32 lbl_80478DD8;
    extern u32 lbl_80478DDC;
    u32 i;

    for (i = 0; i < *(u32*)lbl_80478DD8; i++) {
        if (item == menuItemBiosGetPtr((*(TitleMenuEntry**)&lbl_80478DDC)[i].item)) {
            return i;
        }
    }
    return 0;
}

/* fn_80023DA8 - 0x80023DA8 | size: 0x3c */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80023DA8)
s32 fn_80023DA8(void) {
    winMsgOpen(2, 0x44c6, 1, 0);
    winMsgClose(1);
    return 1;
}

/* fn_80023DE4 - 0x80023DE4 | size: 0x3c */
s32 fn_80023DE4(void) {
    winMsgOpen(2, 0x4261, 1, 0);
    winMsgClose(1);
    return 1;
}

/* fn_80023E20 - 0x80023E20 | size: 0x3c */
s32 fn_80023E20(void) {
    winMsgOpen(2, 0x426a, 1, 0);
    winMsgClose(1);
    return 1;
}

/* fn_80023E5C - 0x80023E5C | size: 0x4 */
void std018_nop18(void) { }
#endif

/* fn_80023E60 - 0x80023E60 | size: 0x300 */
extern void windowGetCursorToItem(void);
extern void fn_800E0060(void);
extern void fn_800E0000(void);
extern u32 lbl_8047A370;
extern u32 lbl_8047A368;
extern u32 lbl_8047A36C;
extern f32 lbl_80478898;
extern u32 lbl_8047A390;
extern u8 lbl_8047A380;
extern void wmapCursorMove(u8*, void*, u16*, u8*);
extern u8 lbl_803A2058[];
extern u8 lbl_803A204C[];
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80023E60)
s32 wmapMenuUpdate(u8* arg0) {
    extern s32 windowGetCursorToItem(u8*);
    extern u16* windowGetKeyInfo(void);
    extern u8* menuDataBiosGetPtr(u32);
    extern u8* menuItemBiosGetPtr(s32);
    extern void fn_800E0060(f32*, f32*);
    extern f32 fn_800E0000(f32*, f32*);
    extern void std104_soundSetBackgroundMusic(s32, s32, s32);
    u8* da18;
    u8* r104318;
    u16* temp_r5;
    s32 result;
    s32 target;
    u8* p;
    f32 sp14[3];
    f32 sp08[3];

    if ((s32)lbl_8047A370 == 1) {
        return 0;
    }

    da18 = menuDataBiosGetPtr(*(u32*)(arg0 + 4));
    r104318 = (u8*)windowGetCursorToItem(arg0);

    if (r104318 == NULL) {
        arg0[0x95] = 0;
        return 0;
    }

    temp_r5 = windowGetKeyInfo();
    wmapCursorMove(arg0, r104318, temp_r5, da18);

    result = (s32)(s8)arg0[0x95] + (s32)(s8)arg0[0x94];
    target = (s32)lbl_8047A368;
    lbl_8047A36C = result;

    if ((u32)target != (u32)result) {
        lbl_80478898 = 90.0f;
        lbl_8047A370 = 1;

        p = titleGetMenuItemAt(arg0, target);
        if (p != NULL) {
            *(f32*)(lbl_803A2058 + 0) = (f32)(s32)(*(s16*)(p + 2));
            *(f32*)(lbl_803A2058 + 4) = (f32)(s32)(*(s16*)(p + 4));
            *(f32*)(lbl_803A2058 + 8) = 0.0f;
        }

        p = titleGetMenuItemAt(arg0, (s32)lbl_8047A36C);
        if (p != NULL) {
            *(f32*)(lbl_803A204C + 0) = (f32)(s32)(*(s16*)(p + 2));
            *(f32*)(lbl_803A204C + 4) = (f32)(s32)(*(s16*)(p + 4));
            *(f32*)(lbl_803A204C + 8) = 0.0f;
        }

        {
            u8* cursor = (u8*)lbl_8047A390;
            sp14[0] = *(f32*)(lbl_803A204C + 0) - (f32)(s32)(*(s16*)(cursor + 2));
            sp14[1] = *(f32*)(lbl_803A204C + 4) - (f32)(s32)(*(s16*)(cursor + 4));
            sp14[2] = 0.0f;
            if (0.0f != sp14[0] || 0.0f != sp14[1]) {
                fn_800E0060(sp08, sp14);
                sp14[0] = 1.0f;
                sp14[1] = 0.0f;
                sp14[2] = 0.0f;
                if (fn_800E0000(sp14, sp08) < 0.0f) {
                    lbl_8047A380 = 1;
                } else {
                    lbl_8047A380 = 0;
                }
            }
        }

        std104_soundSetBackgroundMusic(0x464, 0, 0xFF);
    }

    return 0;
}

/* fn_80024160 - 0x80024160 | size: 0x1a8 */
extern u32 lbl_8047A368;
extern u32 lbl_80478DF4;
typedef struct TitleMenuLink {
    u8 dirMask;
    u32 from;
    u32 to;
} TitleMenuLink;
extern u32 lbl_80478DF0;
void wmapCursorMove(u8* arg0, void* arg1, u16* arg2, u8* arg3) {
    u8* candidate;
    u8 mask;
    u8 visible_index;
    s32 entry_index;
    u8* active;
    s32 v;

    (void)arg1;
    v = *arg2;
    mask = 0;
    if ((v & 0x1) != 0) mask |= 0x1;
    if ((v & 0x2) != 0) mask |= 0x4;
    if ((v & 0x4) != 0) mask |= 0x8;
    if ((v & 0x8) != 0) mask |= 0x2;

    if (mask != 0) {
        active = titleGetMenuItemAt(arg0, lbl_8047A368);

        entry_index = 0;
        while ((u32)entry_index < *(u32*)lbl_80478DF0) {
            visible_index = 0;
            candidate = menuItemBiosGetPtr(*(s16*)(arg3 + 4));
            while (1) {
                if (candidate == 0) {
                    break;
                }
                if (((u32)*(volatile u8*)candidate >> 7) & 1) {
                    if ((((TitleMenuLink*)lbl_80478DF4)[entry_index].dirMask & mask) == mask) {
                        if (menuItemBiosGetPtr(((TitleMenuLink*)lbl_80478DF4)[entry_index].from) == active) {
                            if (menuItemBiosGetPtr(((TitleMenuLink*)lbl_80478DF4)[entry_index].to) == candidate) {
                                arg0[0x95] = visible_index;
                                return;
                            }
                        }
                    }
                    visible_index++;
                }
                if ((((u32)*(volatile u8*)candidate >> 6) & 1) == 0) {
                    candidate = menuItemBiosGetPtr(*(s16*)(candidate + 0x18));
                } else {
                    break;
                }
            }
            entry_index++;
        }
    }
}

/* fn_80024308 - 0x80024308 | size: 0x130
 *
 * Status: 99.1% matched. Remaining: reg-alloc choice in second branch
 * (f0/f1/f2 ordering: B8C8 in f1 vs f2, stack in f2 vs f0, B8898 in f0 vs f1),
 * plus 2 anonymous @NNN@sda21 f64 magic constants for u32->f32 conversion
 * (CW heuristic; target uses lbl_8047B8D0 name).
 * Second branch: separate f3=(f32)(u32)fn_800D3088() to avoid __cvt_fp2unsigned.
 */
extern u32 lbl_8047A370;
extern f32 lbl_8047A378;
extern f32 lbl_8047A374;
extern f32 lbl_80478898;
extern u32 lbl_8047A36C;
extern u32 lbl_8047A368;
s32 wmapMenuInput(u8* arg0) {
    extern u8* windowGetKeyInfo(void);
    u8* ctx;
    f32 f0;
    f32 f1;
    f32 f2;
    f32 f3;

    ctx = windowGetKeyInfo();
    if (arg0 != 0) {
        if ((s32)lbl_8047A370 != 1) {
            if ((*(u16*)(ctx + 4) & 0x10) != 0) {
                arg0[0x98] = 1;
            }
        }
        if ((*(u16*)(ctx + 4) & 0x20) != 0) {
            arg0[0x98] = 1;
            arg0[0x99] = 1;
        }
    }

    f2 = (f32)(u32)msgGetCharsPerFrame();
    f3 = lbl_8047A378;
    f0 = f3 * f2 + lbl_8047A374;
    lbl_8047A374 = f0;
    f1 = 12.0f;
    if (f0 < f1) {
        lbl_8047A374 = f1;
        lbl_8047A378 = -f3;
    } else {
        f1 = 20.0f;
        if (f0 > f1) {
            lbl_8047A374 = f1;
            lbl_8047A378 = -f3;
        }
    }

    if ((s32)lbl_8047A370 == 1) {
        f3 = (f32)(u32)msgGetCharsPerFrame();
        f1 = lbl_80478898 - 4.5f * f3;
        lbl_80478898 = f1;
        if (f1 < 0.0f) {
            lbl_80478898 = 0.0f;
            lbl_8047A370 = 0;
            lbl_8047A368 = lbl_8047A36C;
        }
    }
    return 0;
}
#endif


/* =========================================================================
 * Stubs for remaining GStitle functions (0x80024438-0x80025F84)
 * ========================================================================= */

extern u32 lbl_80478DDC;
extern u32 std081_flagCheckTrue(void*);

/* 0x4C | fn_80024438 | check_then_call */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80024438)
void fn_80024438(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x80));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_80024484 | check_then_call */
/* fn_80024484 - 0x80024484 | size: 0x4c */
void fn_80024484(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x70));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_800244D0 | check_then_call */
/* fn_800244D0 - 0x800244D0 | size: 0x4c */
void fn_800244D0(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x60));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_8002451C | check_then_call */
/* fn_8002451C - 0x8002451C | size: 0x4c */
void fn_8002451C(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x50));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_80024568 | check_then_call */
/* fn_80024568 - 0x80024568 | size: 0x4c */
void fn_80024568(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x40));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_800245B4 | check_then_call */
/* fn_800245B4 - 0x800245B4 | size: 0x4c */
void fn_800245B4(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x30));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_80024600 | check_then_call */
/* fn_80024600 - 0x80024600 | size: 0x4c */
void fn_80024600(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x20));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x4C | fn_8002464C | check_then_call */
/* fn_8002464C - 0x8002464C | size: 0x4c */
void fn_8002464C(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)std081_flagCheckTrue(*(void**)((u8*)ctx + 0x10));
    if ((result & 0xFF) == 0) {
        winSpriteSetDisp(arg2, 0);
    }
}

/* 0x80024698 | 0x4 -- nop */
void fn_80024698(void) { }

/* 0x8002469C | 0x30 */
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue(s32, void*);
void fn_8002469C(void) {
    msgctrlSetValue(0x37, GSmsgGetGSchar(0x3cdf));
}

/* 0x800246CC | 0x30 */
void fn_800246CC(void) {
    msgctrlSetValue(0x37, GSmsgGetGSchar(0x3ce4));
}
#endif

/* 0x800246FC | 0x330
 *
 * fn_800246FC -- XD analogue: menuTitle (the top-level 0x330 driver).
 *
 * Walks an array of menu slot records at lbl_80478DE4 (count in lbl_80478DE0,
 * stride 0x28). For each slot whose condition passes (slot entry == 0 OR
 * fn_801902E0(entry) returns non-zero), it performs three passes of the
 * linked-list walk + lookup table scan:
 *   case 1/subcase 1 (lbl_8047A370 == 1 && lbl_80478898 > lbl_8047B8D8):
 *       uses lbl_8047A368 as target index; alpha = lbl_8047B8DC * ((t-b)/b)
 *   case 1/subcase 2 (lbl_8047A370 == 1 && lbl_80478898 <= lbl_8047B8D8):
 *       uses lbl_8047A36C as target index; alpha = lbl_8047B8DC * ((b-t)/b)
 *   default (lbl_8047A370 != 1):
 *       uses lbl_8047A368; alpha = 0xFF (fully opaque)
 * After matching, writes alpha to arg1[0x67] and invokes msgctrlSetValue(0x37,..)
 * with a value pulled from lbl_80478DE4 indexed by lbl_802E4F58[table_index].
 *
 * The three inner blocks are the titleGetMenuItem/titleFindEntry inlines.
 *
 * Status: exact.
 */
extern void* menuDataBiosGetPtr(s32);
extern void* menuItemBiosGetPtr(u32);
extern TitleSlot* lbl_80478DE4;
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_8047A368;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A36C;
extern u8 lbl_802E4F58[];
extern u32 lbl_80478DE0;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800246FC)
static inline s32 titleUpdateCursor(u8* menu, u8* cursor) {
    extern f32 lbl_80478898;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A36C;
    extern u32 lbl_8047A370;
    s32 index;
    u8 alpha;

    switch ((s32)lbl_8047A370) {
    case 1:
        if (lbl_80478898 > 45.0f) {
            index = titleFindEntry(titleGetMenuItem(menu, &lbl_8047A368));
            alpha = 255.0f * ((lbl_80478898 - 45.0f) / 45.0f);
        } else {
            index = titleFindEntry(titleGetMenuItemAt(menu, lbl_8047A36C));
            alpha = 255.0f * ((45.0f - lbl_80478898) / 45.0f);
        }
        break;
    default:
        index = titleFindEntry(titleGetMenuItemAt(menu, lbl_8047A368));
        alpha = 0xFF;
        break;
    }
    cursor[0x67] = alpha;
    return index;
}

static inline u32 titleSlotMessage(u32 slot, u32 index) {
    extern u8 lbl_802E4F58[];
    extern TitleSlot* lbl_80478DE4;

    if (index > 9) {
        index = 0;
    }
    return lbl_80478DE4[slot].msg[lbl_802E4F58[index]];
}

/* RULE-EXCEPTION(title-path): walk-helper choice (address vs value) made only for register assignment — see docs/RULE_EXCEPTIONS.md */
void wmapDrawInfo(u8* arg0, u8* arg1) {
    extern u8 lbl_802E4F58[];
    extern u32 lbl_80478DD8;
    extern u32 lbl_80478DE0;
    extern TitleSlot* lbl_80478DE4;
    extern u32 std081_flagCheckTrue(void*);
    extern void* GSmsgGetGSchar(u32);
    extern void msgctrlSetValue(s32, void*);
    u32 slot;
    s32 index;

    for (slot = 0; slot < *(u32*)lbl_80478DE0; slot++) {
        if (lbl_80478DE4[slot].flag == 0 ||
            (u8)std081_flagCheckTrue((void*)lbl_80478DE4[slot].flag) != 0) {
            index = titleUpdateCursor(arg0, arg1);
            if ((u32)index < *(u32*)lbl_80478DD8) {
                msgctrlSetValue(0x37, GSmsgGetGSchar(titleSlotMessage(slot, index)));
                return;
            }
            break;
        }
    }

    msgctrlSetValue(0x37, GSmsgGetGSchar(1));
}

/* 0x80024A2C | 0x178
 *
 * fn_80024A2C -- simplified version of fn_800246FC (one inner block only).
 * Walks the linked list to find the node whose counter-position matches
 * lbl_8047A368, then scans lbl_80478DDC[] for a matching pbVar3 and writes
 * either alpha computed via the fade curve OR 0xFF into arg1[0x67]. If the
 * table lookup succeeds, writes the corresponding entry value to arg1[0x58]
 * (special-cased to 0xC5F1200 when the entry id is 0x66 and fn_801902E0(0x45D)
 * returns non-zero - likely an "override" for a specific menu option).
 *
 * Status: exact.
 */
extern u32 lbl_8047A368;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
void fn_80024A2C(u8* arg0, u8* arg1) {
    s32 index;
    u32 value;

    index = titleFindEntry(titleGetMenuItem(arg0, &lbl_8047A368));
    switch ((s32)lbl_8047A370) {
    case 1:
        arg1[0x67] = 255.0f * (lbl_80478898 / 90.0f);
        break;
    default:
        arg1[0x67] = 0xFF;
        break;
    }

    if ((u32)index < *(u32*)lbl_80478DD8) {
        if ((*(TitleMenuEntry**)&lbl_80478DDC)[index].id == 0x66 && (u8)std081_flagCheckTrue((void*)0x45D) != 0) {
            value = 0x0C5F1200;
        } else {
            value = (*(TitleMenuEntry**)&lbl_80478DDC)[index].value;
        }
        *(u32*)(arg1 + 0x58) = value;
    }
}

/* 0x80024BA4 | 0x138
 *
 * fn_80024BA4 -- same linked-list walk as fn_80024A2C, but without the alpha
 * computation. Just finds the menu item matching lbl_8047A36C and writes the
 * entry payload to arg1[0x58] (with the 0x66 + fn_801902E0(0x45D) override).
 *
 * Likely corresponds to a menu cursor hover-highlight update:
 *   arg0 = menu context, arg1 = cursor/sprite target receiving the entry
 *
 * Status: exact.
 */
extern u32 lbl_8047A36C;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
void fn_80024BA4(u8* arg0, u8* arg1) {
    s32 index;
    u32 value;

    index = titleFindEntry(titleGetMenuItem(arg0, &lbl_8047A36C));
    if ((u32)index < *(u32*)lbl_80478DD8) {
        if ((*(TitleMenuEntry**)&lbl_80478DDC)[index].id == 0x66 && (u8)std081_flagCheckTrue((void*)0x45D) != 0) {
            value = 0x0C5F1200;
        } else {
            value = (*(TitleMenuEntry**)&lbl_80478DDC)[index].value;
        }
        *(u32*)(arg1 + 0x58) = value;
    }
}
#endif

/* 0x80024CDC | 0xE0
 *
 * fn_80024CDC -- XD analogue: menuTitleCursorAnime (0xDC, close size match).
 * Animates a pulsing cursor/icon by:
 *   1. Copying position from lbl_8047A390 (cursor source) plus 8px offset
 *      into arg1[0x50]/arg1[0x52] (dst x/y, only when lbl_8047A390 set).
 *   2. Computing iVar4 = speed * tickCounter + currentAlpha.
 *   3. Clamping iVar4 to [0x40, 0xFF] and flipping lbl_8047A37C (speed)
 *      sign when hitting either rail - classic ping-pong animation.
 *   4. Writing clamped alpha back to arg1[0x67].
 *
 * Status: code exact. `iVar4 +=` keeps the alpha and the converted sum in
 * one variable (retail's r31); a separate `iVar4 = (s32)(... + iVar4)`
 * splits them (99.5%). Linked in gs_title_80023DA8.c, which owns the
 * int->float bias doubles (lbl_8047B8B8 / lbl_8047B8D0) in its pool.
 */
extern u32 msgGetCharsPerFrame(void);
extern u32 lbl_8047A390;
extern f32 lbl_8047A37C;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80024CDC)
void fn_80024CDC(s32 arg0, u8* arg1) {
    f32 fVar2;
    s32 iVar4;
    u32 uVar3;

    if (lbl_8047A390 != 0) {
        *(s16*)(arg1 + 0x50) = *(s16*)((u8*)lbl_8047A390 + 2) + 8;
        *(s16*)(arg1 + 0x52) = *(s16*)((u8*)lbl_8047A390 + 4) + 8;
    }

    iVar4 = *(u8*)(arg1 + 0x67);          /* current alpha (0..255)    */
    uVar3 = msgGetCharsPerFrame();                /* u32 tick counter          */
    fVar2 = lbl_8047A37C;                 /* f32 pulse speed           */
    iVar4 += fVar2 * (f32)uVar3;          /* one web: alpha stays in r31 */

    /* Clamp alpha to [0x40, 0xFF] and flip direction on rail.   */
    /* This produces the classic ping-pong / throb animation.   */
    if (iVar4 < 0x40) {
        iVar4 = 0x40;
        lbl_8047A37C = -fVar2;            /* reverse pulse direction   */
    } else if (0xFF < iVar4) {
        iVar4 = 0xFF;
        lbl_8047A37C = -fVar2;
    }

    *(u8*)(arg1 + 0x67) = (u8)iVar4;
}
#endif

/* 0x80024DBC | 0x170
 *
 * fn_80024DBC -- one of four "twin" functions. Fade-aware position writer.
 *   - case 1 (lbl_8047A370 == 1): call fn_800E0CA0(fade_amt) + GSlerpGetLinearInterpolationVector
 *     to interpolate a float-pair position into local_48[0..1]; convert to int.
 *   - default: linked-list walk to find the menu item at lbl_8047A368,
 *     pull its x (pbVar3+2) and y (pbVar3+4) shorts.
 * Then writes (s16)(base_x + float(x) + offset) to arg1[0x50]/[0x52].
 *
 * The four twin functions differ only in the sign applied to each axis:
 *   fn_80024DBC: +x +y
 *   fn_80024F2C: +x -y
 *   fn_8002509C: -x +y
 *   fn_8002520C: -x -y
 *
 * Status: 96.6% matched. Remaining diffs: @NNN@sda21 anonymous f64 bias
 * constants vs target lbl_8047B8B8@sda21 named refs.
 */
extern f32 fn_800E0CA0(f32);
extern void GSlerpGetLinearInterpolationVector(void*, void*, void*, f32);
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u8 lbl_803A2058[];
extern u8 lbl_803A204C[];
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f32 lbl_8047A374;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80024DBC)
void fn_80024DBC(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar2;
    u8 *pbVar3;
    s32 uVar5;
    s32 unaff_r28;
    s32 uVar4;
    s32 unaff_r31;
    f32 fB8E0;
    f32 local_48[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar3 = menuItemBiosGetPtr(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) {
                        if (((!lbl_803A2058) && (!lbl_803A2058)) && (!lbl_803A2058)) {
                        }
                        goto LAB_80024E94;
                    }
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = menuItemBiosGetPtr(*(s16*)(pbVar3 + 0x18));
            }
            pbVar3 = (u8*)0;
LAB_80024E94:
            if (pbVar3 != 0) {
                unaff_r28 = *(s16*)(pbVar3 + 2);
                unaff_r31 = *(s16*)(pbVar3 + 4);
            }
        }
        break;
    }
    /* (f32)(s32)(s16)x forces sign-extend via extsh before the   */
    /* xoris + stw magic f64 pattern for int-to-float conversion. */
    /* Outer (s16) cast (not (s32)!) avoids an extra pre-store    */
    /* extsh that target doesn't emit.                            */
    fB8E0 = 9.0f;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 + lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 + lbl_8047A374 + fB8E0);
}

/* 0x80024F2C | 0x170
 *
 * fn_80024F2C -- twin of fn_80024DBC, using +offset on x and -offset on y.
 * Same structure: case-1 calls GSlerpGetLinearInterpolationVector to interpolate, default walks list.
 * See fn_80024DBC doc for full semantics.
 *
 * Status: 94.9% matched. Same SDA2 anonymous-constant blocker.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f32 lbl_8047A374;
void fn_80024F2C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar2;
    u8 *pbVar3;
    s32 uVar5;
    s32 unaff_r28;
    s32 uVar4;
    s32 unaff_r31;
    f32 fB8E0;
    f32 local_48[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar3 = menuItemBiosGetPtr(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) {
                        if (((!unaff_r28) && (!unaff_r28)) && (!unaff_r28)) {
                        }
                        goto LAB_80025004;
                    }
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = menuItemBiosGetPtr(*(s16*)(pbVar3 + 0x18));
            }
            pbVar3 = (u8*)0;
LAB_80025004:
            if (pbVar3 != 0) {
                unaff_r28 = *(s16*)(pbVar3 + 2);
                unaff_r31 = *(s16*)(pbVar3 + 4);
            }
        }
        break;
    }
    fB8E0 = 9.0f;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 + lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 - lbl_8047A374 + fB8E0);
}

/* 0x8002509C | 0x170
 *
 * fn_8002509C -- twin of fn_80024DBC. Uses -offset on x, +offset on y.
 * See fn_80024DBC doc.
 *
 * Status: 92.4% matched. Same SDA2 anonymous-constant blocker.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f32 lbl_8047A374;
void fn_8002509C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar2;
    u8 *pbVar3;
    s32 uVar5;
    s32 unaff_r28;
    s32 uVar4;
    s32 unaff_r31;
    f32 fB8E0;
    f32 local_48[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar3 = menuItemBiosGetPtr(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) {
                        if (arg1) {
                            goto LAB_80025174;
                        } else {
                            goto LAB_80025174;
                        }
                    }
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = menuItemBiosGetPtr(*(s16*)(pbVar3 + 0x18));
            }
            pbVar3 = (u8*)0;
LAB_80025174:
            if (pbVar3 != 0) {
                unaff_r28 = *(s16*)(pbVar3 + 2);
                unaff_r31 = *(s16*)(pbVar3 + 4);
            }
        }
        break;
    }
    fB8E0 = 9.0f;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 - lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 + lbl_8047A374 + fB8E0);
}

/* 0x8002520C | 0x170
 *
 * fn_8002520C -- twin of fn_80024DBC. Uses -offset on BOTH x and y.
 * See fn_80024DBC doc.
 *
 * Status: 90.5% matched. Same SDA2 anonymous-constant blocker.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f32 lbl_8047A374;
void fn_8002520C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar2;
    u8 *pbVar3;
    s32 uVar5;
    s32 unaff_r28;
    s32 uVar4;
    s32 unaff_r31;
    f32 fB8E0;
    f32 local_48[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar3 = menuItemBiosGetPtr(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) {
                        if (((!pbVar3) && (!pbVar3)) && (!pbVar3)) {
                        }
                        goto LAB_800252E4;
                    }
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = menuItemBiosGetPtr(*(s16*)(pbVar3 + 0x18));
            }
            pbVar3 = (u8*)0;
LAB_800252E4:
            if (pbVar3 != 0) {
                unaff_r28 = *(s16*)(pbVar3 + 2);
                unaff_r31 = *(s16*)(pbVar3 + 4);
            }
        }
        break;
    }
    fB8E0 = 9.0f;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 - lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 - lbl_8047A374 + fB8E0);
}

/* 0x8002537C | 0x114
 *
 * fn_8002537C -- single-axis position writer. Writes (s16)(x + 0xf) to
 * arg1[0x50] and 0 to arg1[0x52]. Only returns an X coord; Y is always 0,
 * meaning this is likely for horizontal-slider cursors.
 *
 * case 1: GSlerpGetLinearInterpolationVector returns a pair, use local_28[0] (x)
 * default: linked-list walk, read pbVar2[2] (the x short of a menu item)
 *
 * Status: 98.4% matched. Remaining 2 diffs: bne+b vs beq pattern in the
 * counter-match check. CW 1.3 optimizer collapses explicit goto-pair
 * source back into beq.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
void fn_8002537C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar1;
    u8 *pbVar2;
    s32 uVar4;
    s32 unaff_r29;
    s32 uVar3;
    f32 local_28[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_28, lbl_803A2058, lbl_803A204C);
        unaff_r29 = (s32)local_28[0];
        break;
    case 0:
    default:
        uVar3 = (s32)lbl_8047A368;
        if ((u32)uVar3 < *(u32*)lbl_80478DD8) {
            iVar1 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar2 = menuItemBiosGetPtr(*(s16*)(iVar1 + 4));
            uVar4 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar2 >> 7) & 1) {
                    if (uVar3 == uVar4) {
                        if ((pbVar2 && pbVar2) && pbVar2) {
                        }
                        goto LAB_80025450;
                    }
                    uVar4 = uVar4 + 1;
                }
                if (((u32)*(volatile u8*)pbVar2 >> 6) & 1) break;
                pbVar2 = menuItemBiosGetPtr(*(s16*)(pbVar2 + 0x18));
            }
            pbVar2 = (u8*)0;
LAB_80025450:
            if (pbVar2 != 0) {
                unaff_r29 = *(s16*)(pbVar2 + 2);
            }
        }
        break;
    }
    *(s16*)(arg1 + 0x50) = (s16)(unaff_r29 + 0xf);
    *(u16*)(arg1 + 0x52) = 0;
}

/* 0x80025490 | 0x114
 *
 * fn_80025490 -- exact twin of fn_8002537C but returns the Y coord (into
 * arg1[0x52]) with X always 0. Same code structure.
 *
 * case 1: use local_24[1] (local_24[0+4] offset)
 * default: read pbVar2[4] (y short, 2 bytes after x at pbVar2[2])
 *
 * Status: 98.4% matched. Same bne+b vs beq CW arcana.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
inline u32 inline_fn(u32* arg0) {
    return *arg0;
}

void fn_80025490(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* menuItemBiosGetPtr(s32);
    extern u8* menuDataBiosGetPtr(u32);
    extern void GSlerpGetLinearInterpolationVector(void*, void*, void*);
    extern void fn_800E0CA0(f64);
    s32 iVar1;
    u8 *pbVar2;
    s32 uVar4;
    s32 unaff_r29;
    s32 uVar3;
    f32 local_24[4];

    switch ((s32)lbl_8047A370) {
    case 1:
        fn_800E0CA0((f64)lbl_80478898);
        GSlerpGetLinearInterpolationVector(local_24, lbl_803A2058, lbl_803A204C);
        unaff_r29 = (s32)local_24[1];
        break;
    case 0:
    default:
        uVar3 = (s32)lbl_8047A368;
        if ((u32)uVar3 < inline_fn((u32*)lbl_80478DD8)) {
            iVar1 = (s32)menuDataBiosGetPtr(*(u32*)(arg0 + 4));
            pbVar2 = menuItemBiosGetPtr(*(s16*)(iVar1 + 4));
            uVar4 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar2 >> 7) & 1) {
                    if (uVar3 == uVar4) {
                        if (!iVar1 && !iVar1 && !iVar1) {
                        }
                        goto LAB_80025564;
                    }
                    uVar4 = uVar4 + 1;
                }
                if (((u32)*(volatile u8*)pbVar2 >> 6) & 1) break;
                pbVar2 = menuItemBiosGetPtr(*(s16*)(pbVar2 + 0x18));
            }
            pbVar2 = (u8*)0;
LAB_80025564:
            if (pbVar2 != 0) {
                unaff_r29 = *(s16*)(pbVar2 + 4);
            }
        }
        break;
    }
    *(u16*)(arg1 + 0x50) = 0;
    *(s16*)(arg1 + 0x52) = (s16)(unaff_r29 + 0xf);
}

/* 0x800255A4 | 0x18C
 *
 * fn_800255A4 -- title-screen audio/timing update driven by lbl_8047A384.
 *   IF title active (lbl_8047A384 != 0):
 *     fadeSet(lbl_8047B8E4, 3)     -- set BGM volume fade target
 *     soundStop(0x449, 0)            -- play SE cue
 *     while (accumulator < limit) {    -- time-based integration loop
 *       _threadSwitch();                 -- frame advance
 *       accumulator += (f32)fn_800D3088() / (f32)(s32)fn_800D37CC();
 *     }
 *     fn_801653C4(); fn_801656F8(0x7d0, 0)
 *   ELSE:
 *     fadeSet(lbl_8047B8E8, 3)     -- different fade curve
 *     fn_801653C4(); fn_801656F8(0x1f4, 0)
 *   Check sound channels 0xbd and 0xc3; stop them if still active.
 *   If lbl_8047A388/38C particle handles set, free them via GStextureFree.
 *
 * fn_800D37CC returns s32 (signed ticks; negative if paused).
 * fn_800D3088 returns u32 (absolute frame count).
 *
 * Status: 97.5% matched. Remaining 8 diffs are register-numbering
 * (limit ends up in f28 vs target f31) and anonymous @257/@255 bias
 * constants vs named lbl_8047B8B8/lbl_8047B8D0 (unfixable without
 * explicit manual int-to-float idiom). Frame size 0x70 now correct.
 * fn_801653C4 chains into fn_801656F8 (3 args).
 */
extern void fadeSet(f32, s32);
extern void soundStop(void);
extern void fn_800D37CC(void);
extern void std228_soundGetCurrentBackgroundMusic(void);
extern void std137_soundStopAll(void);
extern void fadeCheck(s32);
extern void menuIsCheck(void);
extern void menuClose(void);
extern void GStextureFree(void);
extern u32 lbl_8047A384;
extern u32 lbl_8047A388;
extern u32 lbl_8047A38C;
void worldMapPost(void) {
    extern u32 lbl_8047A384;
    extern u32 lbl_8047A388;
    extern u32 lbl_8047A38C;
    extern u32 msgGetCharsPerFrame(void);
    extern s32 fn_800D37CC(void);
    extern void GStextureFree(u32);
    extern void _threadSwitch(void);
    extern u8 menuIsCheck(s32);
    extern void menuClose(s32);
    extern u32 std228_soundGetCurrentBackgroundMusic(void);
    extern void std137_soundStopAll(u32, s32, s32);
    extern void soundStop(s32, s32);
    extern void fadeCheck(s32);
    extern void fadeSet(f32, s32);
    f32 accum;

    if (lbl_8047A384 != 0) {
        fadeSet(2.0f, 3);
        soundStop(0x449, 0);
        accum = 0.0f;
        while (accum < 1.0f) {
            _threadSwitch();
            accum = accum + (f32)msgGetCharsPerFrame() / (f32)fn_800D37CC();
        }
        std137_soundStopAll(std228_soundGetCurrentBackgroundMusic(), 0x7d0, 0);
    } else {
        fadeSet(0.5f, 3);
        std137_soundStopAll(std228_soundGetCurrentBackgroundMusic(), 0x1f4, 0);
    }
    fadeCheck(1);
    if (menuIsCheck(0xbd) == 1) menuClose(0xbd);
    if (menuIsCheck(0xc3) == 1) menuClose(0xc3);
    if (lbl_8047A384 != 0) {
        if (lbl_8047A388 != 0) GStextureFree(lbl_8047A388);
        if (lbl_8047A38C != 0) GStextureFree(lbl_8047A38C);
    }
}

/* 0x80025730 | 0x280 */
extern void menuItemBiosSetSelectFlag(void);
extern void windowGetActiveID(void);
extern void menuOpenCustom(void);
extern void floorGetPrevFloorID(void);
extern void fn_800D3074(void);
extern void GStextureCreate(void);
extern void menuOpen(void);
extern u32 lbl_80478DDC;
extern u32 lbl_8047A368;
extern u32 lbl_8047A390;
extern u32 lbl_8047A3AC;
extern u32 lbl_80478DD8;
extern u8 lbl_8047A3A8;
extern u32 lbl_8047A388;
extern void worldMapBikeRide(void);
void worldMapMain(void) {
    typedef struct TitleFloorEntry {
        u32 flag;
        u32 floor;
        u32 item;
        u32 pad;
    } TitleFloorEntry;
    extern u32 menuItemBiosGetPtr(u32 item);
    extern void menuItemBiosSetSelectFlag(u32 item, u8 enable);
    extern u8 std081_flagCheckTrue(u32 flag);
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 id, ...);
    extern u32 floorGetPrevFloorID(void);
    extern void fn_800D3074(s32 mode);
    extern u32 GStextureCreate(s32, s32, s32, s32, s32);
    extern void GSgfxBeginBackFBCapture(u32 texture, void (*callback)(void), s32 arg);
    extern void menuOpen(s32 id, s32 arg);
    extern void menuClose(s32 id);
    extern void wmapCaptureDone(void);
    s32 selected;
    u8 enable;
    u32 floor;
    u32 i;
    s32 result;
    u32 count;
    u8 change;
    TitleFloorEntry* entry;

    selected = 0;
    lbl_8047A368 = 0;
    lbl_8047A390 = menuItemBiosGetPtr((*(TitleFloorEntry**)&lbl_80478DDC)[0].item);
    for (i = count = 0; i < **(u32**)&lbl_80478DD8; i++) {
        entry = &(*(TitleFloorEntry**)&lbl_80478DDC)[i];
        if (lbl_8047A3AC == entry->floor) {
            lbl_8047A368 = count;
            selected = count;
            lbl_8047A390 = menuItemBiosGetPtr(entry->item);
        }
        enable = 1;
        if ((*(TitleFloorEntry**)&lbl_80478DDC)[i].flag != 0 &&
            std081_flagCheckTrue((*(TitleFloorEntry**)&lbl_80478DDC)[i].flag) == 0) {
            enable = 0;
        }
        menuItemBiosSetSelectFlag((*(TitleFloorEntry**)&lbl_80478DDC)[i].item, enable);
        if (enable) {
            count++;
        }
    }

    result = menuOpenCustom(0xBD, windowGetActiveID(), &selected, 0, 1, 0);
    if (result < 0) {
        floor = floorGetPrevFloorID();
    } else {
        TitleFloorEntry* entries = *(TitleFloorEntry**)&lbl_80478DDC;
        u32 j;

        for (j = 0; j < **(u32**)&lbl_80478DD8; j++) {
            if ((u32)result == entries[j].item) {
                break;
            }
        }
        if (j >= **(u32**)&lbl_80478DD8) {
            j = 0;
        }
        if (entries[j].floor == 0x66 && std081_flagCheckTrue(0x45D) != 0) {
            floor = 0x7B;
        } else {
            floor = (*(TitleFloorEntry**)&lbl_80478DDC)[j].floor;
        }
    }

    if (floor != floorGetPrevFloorID()) {
        if (floor == 0x7A && floorGetPrevFloorID() == 0xF) {
            change = 0;
        } else if (floor == 0xF && floorGetPrevFloorID() == 0x7A) {
            change = 0;
        } else {
            change = 1;
        }
    } else {
        change = 0;
    }
    if (change == 1) {
        fn_800D3074(2);
        lbl_8047A3A8 = 0;
        GSgfxBeginBackFBCapture(lbl_8047A388 = GStextureCreate(0, 0, 0x44, 0, 0),
                                wmapCaptureDone, 0);
        while (lbl_8047A3A8 == 0) {
            _threadSwitch();
        }
        menuOpen(0xC3, 0);
        menuClose(0xBD);
        worldMapBikeRide();
    }
    floorLink(floor, 0);
}
#endif

/* 0x800259B0 | 0xCC */
extern void std114_mapObjectSetVisibility(void);
extern u32 lbl_8047A368;
extern u32 lbl_8047A36C;
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern f32 lbl_8047A374;
extern f32 lbl_8047A378;
extern f32 lbl_8047A37C;
extern u8 lbl_8047A380;
extern u32 lbl_8047A384;
extern u32 lbl_8047A388;
extern u32 lbl_8047A390;
extern u32 lbl_8047A3AC;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800259B0)
void worldMapPre(void) {
    extern u8 lbl_803A2058[];
    extern u8 lbl_803A204C[];
    extern void std114_mapObjectSetVisibility(s32, s32);
    extern u32 floorGetPrevFloorID(void);
    f32* vec1 = (f32*)lbl_803A2058;
    f32* vec2 = (f32*)lbl_803A204C;
    u32 result;

    lbl_8047A368 = 0;
    lbl_8047A36C = 0;
    lbl_8047A370 = 0;
    lbl_80478898 = 90.0f;
    vec1[0] = 0.0f;
    vec1[1] = 0.0f;
    vec1[2] = 0.0f;
    vec2[0] = 0.0f;
    vec2[1] = 0.0f;
    vec2[2] = 0.0f;
    lbl_8047A374 = 0.0f;
    lbl_8047A378 = 0.5f;
    lbl_8047A37C = 8.0f;
    lbl_8047A380 = 0;
    lbl_8047A384 = 0;
    lbl_8047A388 = 0;
    lbl_8047A390 = 0;
    std114_mapObjectSetVisibility(0x0C6A1000, 0);
    std114_mapObjectSetVisibility(0x0C6A1001, 0);
    std114_mapObjectSetVisibility(0x0C6A1002, 0);
    result = floorGetPrevFloorID();
    lbl_8047A3AC = result;
    if (result == 0x7b) {
        lbl_8047A3AC = 0x66;
    }
}

#if 0
void fn_800259B0_old(void) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A36C;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern f32 lbl_8047A378;
    extern f32 lbl_8047A37C;
    extern u8 lbl_8047A380;
    extern u32 lbl_8047A384;
    extern u32 lbl_8047A388;
    extern u32 lbl_8047A390;
    extern u32 lbl_8047A3AC;
    extern void floorGetPrevFloorID();
    extern void fn_801CB954();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;

    f3 = 90.0f;
    r3 = (u32)lbl_803A2058;
    tmp = 0x0;
    f2 = 0.0f;
    r6 = (u32)lbl_803A2058;
    f1 = 0.5f;
    f0 = 8.0f;
    r3 = (u32)lbl_803A204C;
    r5 = (u32)lbl_803A204C;
    r3 = 0xC6A0000;
    lbl_8047A368 = tmp;
    r3 = r3 + 0x1000;
    r4 = 0x0;
    lbl_8047A36C = tmp;
    lbl_8047A370 = tmp;
    lbl_80478898 = f3;
    *(f32*)((u8*)r6 + 0x0) = f2;
    *(f32*)((u8*)r6 + 0x4) = f2;
    *(f32*)((u8*)r6 + 0x8) = f2;
    *(f32*)((u8*)r5 + 0x0) = f2;
    *(f32*)((u8*)r5 + 0x4) = f2;
    *(f32*)((u8*)r5 + 0x8) = f2;
    lbl_8047A374 = f2;
    lbl_8047A378 = f1;
    lbl_8047A37C = f0;
    lbl_8047A380 = tmp;
    lbl_8047A384 = tmp;
    lbl_8047A388 = tmp;
    lbl_8047A390 = tmp;
    fn_801CB954();
    r3 = 0xC6A0000;
    r4 = 0x0;
    r3 = r3 + 0x1001;
    fn_801CB954();
    r3 = 0xC6A0000;
    r4 = 0x0;
    r3 = r3 + 0x1002;
    fn_801CB954();
    floorGetPrevFloorID();
    lbl_8047A3AC = r3;
    if (r3 == 0x7b) {
        tmp = 0x66;
        lbl_8047A3AC = tmp;
    }
    return;
}
#endif

/* 0x80025A7C | 0x4 -- nop */
void fn_80025A7C(void) { }
#endif

/* 0x80025A80 | 0x19C
 *
 * fn_80025A80(s32 param_1) -> s32 -- cursor quad rendering + decay.
 *   1. Set up GX state (fn_800D9B58/A4C4/A2BC/A1E8/A100/A028/9ED8) for
 *      blended additive rendering with channel configuration.
 *   2. Call fn_80025C1C twice to draw two blended quads:
 *        first at the cursor current position using lbl_8047A3A0 (x scale)
 *        second at offset from lbl_803A204C[0,1] using lbl_8047A3A4 (y scale)
 *   3. Decay both scales:
 *        lbl_8047A3A0 -= lbl_8047B908; clamp >= 0
 *        lbl_8047A3A4 -= lbl_8047B8C8; clamp >= 0
 *   4. Return 1 if either scale is still > 0 (quad still visible), else 0.
 *
 * Used by GSgfxBeginBackFBCapture as a particle callback; re-registered until it
 * returns 0 (fully faded).
 *
 * Status: exact.
 */
extern void gfxSetOrtho(void);
extern void gfxSetBlendMode(void);
extern void gfxSetUpdates(void);
extern void gfxSetZMode(void);
extern void fn_800DA100(void);
extern void gfxSetCullMode(void);
extern void fn_800D9ED8(void);
extern f32 lbl_8047A3A0;
extern u32 lbl_8047A388;
extern f32 lbl_8047A3A4;
extern void wmapDrawCaptureQuad(s32, f32, f32, f32, s32, s32, f32*);
/*
 * RULE-EXCEPTION(title-path): reconstructed stripped function, kept only for
 * the .sdata2 pool order - see docs/RULE_EXCEPTIONS.md. Retail's pool has
 * 320.0f, 240.0f, 640.0f, 480.0f and 1.2f (0x8047B8F0-0x8047B900) ahead of
 * fn_80025A80's own 3.0f, although fn_80025A80 uses 640/480 before 320/240
 * and only fn_80025F84 (which comes later) reads 1.2f. MWCC adds a literal
 * when it first compiles it, so a function compiled before fn_80025A80 used
 * them in that order and the linker stripped it. Its body is the shape of
 * fn_80025F84's capture-centre store and 50 Hz-scaled frame wait; the name
 * is a placeholder. force_active is off so mwld strips it as it did retail's.
 */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80025A80)
#pragma push
#pragma force_active off
void gsTitleStrippedCaptureWait(void)
{
    extern u8 lbl_803A2040[];
    extern s32 fn_800D37CC(void);
    extern u32 msgGetCharsPerFrame(void);
    extern void gfxSetOrtho(f32, f32, f32, f32);
    u32 frames;
    u32 elapsed;
    s32 tick;

    *(f32*)lbl_803A2040 = 320.0f;
    *(f32*)(lbl_803A2040 + 4) = 240.0f;
    *(f32*)(lbl_803A2040 + 8) = 0.0f;
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    frames = 0x78 - (u32)((f32)fn_800D37CC() * 2.0f);
    tick = fn_800D37CC();
    if (tick == 0x32) {
        frames = (f32)frames / 1.2f;
    }
    for (elapsed = 0; elapsed < frames; elapsed += tick) {
        _threadSwitch();
        tick = msgGetCharsPerFrame();
    }
}
#pragma pop
#endif


#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80025A80)
s32 wmapRideOverlay(s32 param_1) {
    extern u8 lbl_803A204C[];
    extern u32 lbl_8047A388;
    extern f32 lbl_8047A3A0;
    extern f32 lbl_8047A3A4;
    extern void wmapDrawCaptureQuad(s32, f32, f32, f32, s32, s32, f32*);
    extern void gfxSetOrtho(f32, f32, f32, f32);
    extern void fn_800D9ED8(s32);
    extern void gfxSetCullMode(s32);
    extern void fn_800DA100(s32, s32, s32, s32, s32, s32);
    extern void gfxSetZMode(s32, s32, s32);
    extern void gfxSetUpdates(s32, s32, s32);
    extern void gfxSetBlendMode(s32, s32, s32);
    f32 local_18[3];

    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    gfxSetCullMode(0);
    fn_800D9ED8(1);

    local_18[0] = 320.0f;
    local_18[1] = 240.0f;
    local_18[2] = 0.0f;
    wmapDrawCaptureQuad(param_1, lbl_8047A3A0, 1.0f, 3.0f, 1, 0, local_18);

    lbl_8047A3A0 -= 2.25f;
    if (lbl_8047A3A0 < 0.0f) lbl_8047A3A0 = 0.0f;

    local_18[0] = 640.0f - (9.0f + ((f32*)lbl_803A204C)[0]);
    local_18[1] = 480.0f - (9.0f + ((f32*)lbl_803A204C)[1]);
    local_18[2] = 0.0f;
    wmapDrawCaptureQuad((s32)lbl_8047A388, lbl_8047A3A4, 1.0f, 2.0f, 1, 1, local_18);

    lbl_8047A3A4 = lbl_8047A3A4 - 4.5f;
    if (lbl_8047A3A4 < 0.0f) lbl_8047A3A4 = 0.0f;

    if (lbl_8047A3A4 <= 0.0f && lbl_8047A3A0 <= 0.0f) {
        return 0;
    }
    return 1;
}

/* 0x80025C1C | 0x358
 *
 * fn_80025C1C(s32 tex, f32 rot, f32 size, f32 uv_ext, s32 flag_a,
 *             s32 flag_b, f32 *pos)  -- GX quad render primitive.
 *
 *   1. Build two vec3s (vec_a, vec_b) based on flag_a (1 = symmetric scale,
 *      else diagonal). Call fn_800E0CA0(rot) + GSlerpGetLinearInterpolationVector to combine them
 *      into a rotated offset vector at result_1.
 *   2. Multiply result_1.x by lbl_8047B8DC (alpha) and store as scaled.
 *   3. Build second vec3 pair based on flag_b, call fn_800E0CA0+090C again,
 *      feed through fn_800E042C to get transformed coords into result_2.
 *   4. Third set: combine pos[0], pos[1] with lbl_8047B8F0/F4 UV corners,
 *      via fn_800E0CA0+090C+fn_800E03B4 for the inverse transform.
 *   5. Set GX render state (TEV stage 3, prim TRIANGLE_FAN count 4):
 *        fn_800D88DC(3), fn_800D888C(4), fn_800D6A00(4),
 *        fn_800D7820(lbl_80314AE8), fn_800D85D4(0, tex), fn_800D67BC(4)
 *   6. Emit 4 vertices forming a textured quad. Each vertex:
 *        GSvecTransform(vert_out, result_2, uv_corner)  -- compute position
 *        fn_800D6680(vert.x, vert.y, vert.z)         -- write pos
 *        fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled)    -- RGBA color
 *        fn_800D59B8(0, u, v)                        -- UV
 *      4 UV corners: (0,0) (1,0) (0,1) (1,1) via lbl_8047B8AC/B8B0.
 *   7. fn_800D6728() to flush the primitive.
 *
 * result_2 is a 3x4 matrix (fn_800E042C builds it, fn_800E03B4 updates it,
 * GSvecTransform applies it), which accounts for retail's 0xA0 frame.
 *
 * Status: exact.
 */
extern void fn_800E042C(void);
extern void fn_800E03B4(void);
extern void gfxSetFlags(void);
extern void gfxClearFlags(void);
extern void gfxSetPrimitive(void);
extern void gfxSetVtxDesc(void);
extern void gfxLoadTexture(void);
extern void gfxBegin(void);
extern void GSvecTransform(void);
extern void gfxVtxPos(void);
extern void fn_800D5CB8(void);
extern void gfxVtxTexCoord(void);
extern void gfxEnd(void);
extern u8 lbl_80314AE8[];
void wmapDrawCaptureQuad(s32 arg_r3, f32 arg_f1, f32 arg_f2, f32 arg_f3,
                 s32 arg_r4, s32 arg_r5, f32 *arg_r6) {
    extern u8 lbl_80314AE8[];
    extern void gfxVtxTexCoord(s32, f32, f32);
    extern void fn_800D5CB8(s32, s32, s32, s32, u8);
    extern void gfxVtxPos(f32, f32, f32);
    extern void gfxEnd(void);
    extern void gfxBegin(s32);
    extern void gfxSetPrimitive(s32);
    extern void gfxSetVtxDesc(u8*);
    extern void gfxLoadTexture(s32, s32);
    extern void gfxClearFlags(s32);
    extern void gfxSetFlags(s32);
    extern void GSvecTransform(f32*, f32 (*)[4], f32*);
    extern void fn_800E03B4(f32 (*)[4], f32*);
    extern void fn_800E042C(f32 (*)[4], f32*);
    extern void GSlerpGetLinearInterpolationVector(f32*, f32*, f32*);
    extern void fn_800E0CA0(f32);
    f32 mtx[3][4];
    f32 result_1[3];
    f32 vec_b[3];
    f32 vec_a[3];
    s32 scaled;

    if ((u8)arg_r4 == 1) {
        vec_b[0] = arg_f2;       vec_b[1] = arg_f2;       vec_b[2] = arg_f2;
        vec_a[0] = 0.0f; vec_a[1] = 0.0f; vec_a[2] = 0.0f;
    } else {
        vec_b[0] = 0.0f; vec_b[1] = 0.0f; vec_b[2] = 0.0f;
        vec_a[0] = arg_f2;       vec_a[1] = arg_f2;       vec_a[2] = arg_f2;
    }
    fn_800E0CA0(arg_f1);
    GSlerpGetLinearInterpolationVector(result_1, vec_b, vec_a);
    scaled = (s32)(255.0f * result_1[0]);

    if ((u8)arg_r5 == 1) {
        vec_b[0] = 1.0f; vec_b[1] = 1.0f; vec_b[2] = 1.0f;
        vec_a[0] = arg_f3;       vec_a[1] = arg_f3;       vec_a[2] = 1.0f;
    } else {
        vec_b[0] = arg_f3;       vec_b[1] = arg_f3;       vec_b[2] = 1.0f;
        vec_a[0] = 1.0f; vec_a[1] = 1.0f; vec_a[2] = 1.0f;
    }
    fn_800E0CA0(arg_f1);
    GSlerpGetLinearInterpolationVector(result_1, vec_b, vec_a);
    fn_800E042C(mtx, result_1);

    vec_b[0] = 320.0f; vec_b[1] = 240.0f; vec_b[2] = 0.0f;
    vec_a[0] = arg_r6[0]; vec_a[1] = arg_r6[1]; vec_a[2] = 0.0f;
    fn_800E0CA0(arg_f1);
    GSlerpGetLinearInterpolationVector(result_1, vec_b, vec_a);
    fn_800E03B4(mtx, result_1);

    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(lbl_80314AE8);
    gfxLoadTexture(0, arg_r3);
    gfxBegin(4);

    vec_b[0] = (-320.0f); vec_b[1] = (-240.0f); vec_b[2] = 0.0f;
    GSvecTransform(vec_a, mtx, vec_b);
    gfxVtxPos(vec_a[0], vec_a[1], vec_a[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled);
    gfxVtxTexCoord(0, 0.0f, 0.0f);

    vec_b[0] = 320.0f; vec_b[1] = (-240.0f); vec_b[2] = 0.0f;
    GSvecTransform(vec_a, mtx, vec_b);
    gfxVtxPos(vec_a[0], vec_a[1], vec_a[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled);
    gfxVtxTexCoord(0, 1.0f, 0.0f);

    vec_b[0] = (-320.0f); vec_b[1] = 240.0f; vec_b[2] = 0.0f;
    GSvecTransform(vec_a, mtx, vec_b);
    gfxVtxPos(vec_a[0], vec_a[1], vec_a[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled);
    gfxVtxTexCoord(0, 0.0f, 1.0f);

    vec_b[0] = 320.0f; vec_b[1] = 240.0f; vec_b[2] = 0.0f;
    GSvecTransform(vec_a, mtx, vec_b);
    gfxVtxPos(vec_a[0], vec_a[1], vec_a[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled);
    gfxVtxTexCoord(0, 1.0f, 1.0f);

    gfxEnd();
}
#endif

/* 0x80025F74 | 0x10 */
extern u8 lbl_8047A3A8;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80025F74)
s32 wmapCaptureDone(void) {
    lbl_8047A3A8 = 1;
    return 0;
}
#endif

/* 0x80025F84 | 0x3EC
 *
 * fn_80025F84 -- title splash setup (called once when title first shown).
 *   1. Activate 3 sprite handles 0xC6A1000/1001/1002 (fn_801CB954 activate=1).
 *   2. Pick an intro texture based on fn_801902E0(0x3E5):
 *        ret == 1: use 0xC6B1000 variant
 *        else:     use 0xC6C1000 variant
 *      Load it via fn_801CBA0C + fn_801CB954(activate=0).
 *   3. Set up TEV/sprite chain via fn_80113F48 + GSresGetResource composition:
 *        GSmodelSetBoundCheck, GSmodelSetShadowFlags(2), GSmodelSetShadowLight, GSmodelSetShadowSurface(1, &obj_c),
 *        GSmodelSetShadowTextureSize(0x280, 0x1E0)  -- viewport 640x480
 *        GSmodelLinkToGSparticleBank, GSmodelSetGSparticleLinkAttachMode(4)
 *      Activates tex handle, binds 0xC6A1000 slot 0, pushes animation.
 *      Stores handle in lbl_8047A384 (title active flag).
 *   4. Per lbl_8047A380 (debug flag?), pick one of two frame-sequence tables
 *      (lbl_80478DEC + idx*0x10), read 2 u32s, advance index (wrap > 9).
 *   5. Reset cursor scales: lbl_8047A3A4 = lbl_8047A3A0 = lbl_8047B8A8 (0).
 *   6. Register fn_80025A80 as a particle callback (GSgfxBeginBackFBCapture).
 *   7. Start BGM via cameraPlayAnime, then run two timing delay loops
 *      (_threadSwitch + fn_800D3088 accumulator vs 1 or 0xAE target).
 *   8. Store title origin coords to lbl_803A2040[0..2] and start the final
 *      fade-in loop using __cvt_fp2unsigned for FP-to-int conversion.
 *
 * Status: 99.1% matched (#pragma optimization_level 2). Remaining diffs:
 * register allocation (frame_a/b in table section, first timing loop delay),
 * and anonymous f64 bias constants (lbl_8047B8B8/B8D0, unfixable per CW heuristic).
 */
extern void std112_modelGetResourceID(void);
extern void std093_getCurrentGroupID(void);
extern void GSresGetResource(void);
extern void GSmodelSetBoundCheck(void);
extern void GSmodelSetShadowFlags(void);
extern void GSmodelSetShadowLight(void);
extern void GSmodelSetShadowSurface(void);
extern void GSmodelSetShadowTextureSize(void);
extern void GSmodelLinkToGSparticleBank(void);
extern void GSmodelSetGSparticleLinkAttachMode(void);
extern void std166_mapObjectAttachToObject(void);
extern void std115_mapObjectPlayAnimation(void);
extern void std104_soundSetBackgroundMusic(void);
extern void cameraPlayAnime(void);
extern void __cvt_fp2unsigned(void);
extern u8 lbl_8047A380;
extern u32 lbl_8047A384;
extern u32 lbl_8047A394;
extern u32 lbl_80478DEC;
extern u32 lbl_8047A398;
extern f32 lbl_8047A3A4;
extern f32 lbl_8047A3A0;
extern u32 lbl_8047A38C;
extern u8 lbl_803A2040[];
extern u32 lbl_8047A39C;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80025F84)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimization_level 2) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimization_level 2
void worldMapBikeRide(void) {
    extern u8 lbl_803A2040[];
    extern u32 lbl_80478DEC;
    extern u8 lbl_8047A380;
    extern u32 lbl_8047A384;
    extern u32 lbl_8047A38C;
    extern u32 lbl_8047A394;
    extern u32 lbl_8047A398;
    extern u32 lbl_8047A39C;
    extern f32 lbl_8047A3A0;
    extern f32 lbl_8047A3A4;
    extern u32 __cvt_fp2unsigned(f64);
    extern u32 msgGetCharsPerFrame(void);
    extern s32 fn_800D37CC(void);
    extern void GSgfxBeginBackFBCapture(u32, void*, s32);
    extern void GSmodelSetGSparticleLinkAttachMode(u32, s32);
    extern void GSmodelLinkToGSparticleBank(u32, u32);
    extern void GSmodelSetBoundCheck(u32, s32);
    extern void GSmodelSetShadowTextureSize(s32, s32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, s32, void*);
    extern void GSmodelSetShadowFlags(u32, s32);
    extern u32 GStextureCreate(s32, s32, s32, s32, s32);
    extern void _threadSwitch(void);
    extern u32 GSresGetResource(u32, u32);
    extern u32 std093_getCurrentGroupID(void);
    extern void std104_soundSetBackgroundMusic(s32, s32, s32);
    extern void cameraPlayAnime(u32, u32, s32, s32);
    extern u8 std081_flagCheckTrue(s32);
    extern void std166_mapObjectAttachToObject(u32, u32, s32);
    extern void std115_mapObjectPlayAnimation(u32, u32, s32, s32);
    extern void std114_mapObjectSetVisibility(u32, s32);
    extern u32 std112_modelGetResourceID(u32);
    extern s32 wmapRideOverlay(s32);
    u32 obj_c;
    u32 obj_d;
    u32 obj_e;
    u32 obj_b;
    u32 obj_a;
    u32 start;
    s32 tick;
    u32 tex;
    u32 elapsed;
    u32 frame_b;
    u32 frame_a;
    u32 delay;
    u32 wait2;

    std114_mapObjectSetVisibility(0xC6A1000, 1);
    std114_mapObjectSetVisibility(0xC6A1001, 1);
    std114_mapObjectSetVisibility(0xC6A1002, 1);
    if (std081_flagCheckTrue(0x3e5) == 1) {
        tex = std112_modelGetResourceID(0xC6B1000);
        std114_mapObjectSetVisibility(tex, 0);
        start = 0;
    } else {
        tex = std112_modelGetResourceID(0xC6C1000);
        std114_mapObjectSetVisibility(tex, 0);
        start = 0;
    }

    obj_a = GSresGetResource(std093_getCurrentGroupID(), tex);
    GSmodelSetBoundCheck(obj_a, 0);
    obj_b = GSresGetResource(std093_getCurrentGroupID(), tex);
    obj_c = GSresGetResource(std093_getCurrentGroupID(), 0xC6A1002);
    obj_d = GSresGetResource(std093_getCurrentGroupID(), 0xC6A1603);
    {
        u32 obj_c_local = obj_c;
        GSmodelSetShadowFlags(obj_b, 2);
        GSmodelSetShadowLight(obj_b, obj_d);
        GSmodelSetShadowSurface(obj_b, 1, &obj_c_local);
    }
    GSmodelSetShadowTextureSize(0x280, 0x1e0);
    obj_e = GSresGetResource(std093_getCurrentGroupID(), 0xC831400);
    GSmodelLinkToGSparticleBank(GSresGetResource(std093_getCurrentGroupID(), tex), obj_e);
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(std093_getCurrentGroupID(), tex), 4);
    std114_mapObjectSetVisibility(tex, 1);
    std166_mapObjectAttachToObject(tex, 0xC6A1000, 0);
    std115_mapObjectPlayAnimation(tex, start, 0, 1);
    std104_soundSetBackgroundMusic(0x449, 0, 0xff);
    lbl_8047A384 = tex;

    {
        if (lbl_8047A380 == 0) {
            u32 idx;
            u32 off;
            u32 base;

            idx = lbl_8047A394;
            off = idx << 4;
            base = (u32)lbl_80478DEC; /* RULE-EXCEPTION(title-path): no-op cast for register order - see docs/RULE_EXCEPTIONS.md */
            frame_a = *(u32*)(base + off + 41 * 4);
            frame_b = *(u32*)(base + off + 42 * 4);
            lbl_8047A394 = idx + 1;
            if ((s32)lbl_8047A394 > 9) lbl_8047A394 = 0;
        } else {
            u32 idx;
            u32 off;
            u32 base;

            idx = lbl_8047A398;
            off = idx << 4;
            base = (u32)lbl_80478DEC; /* RULE-EXCEPTION(title-path): no-op cast for register order - see docs/RULE_EXCEPTIONS.md */
            frame_a = *(u32*)(base + off + 1 * 4);
            frame_b = *(u32*)(base + off + 2 * 4);
            lbl_8047A398 = idx + 1;
            if ((s32)lbl_8047A398 > 9) lbl_8047A398 = 0;
        }
        lbl_8047A3A4 = 90.0f;
        lbl_8047A3A0 = 90.0f;
        lbl_8047A38C = GStextureCreate(0, 0, 0x44, 0, 0);
        GSgfxBeginBackFBCapture(lbl_8047A38C, (void*)wmapRideOverlay, 0);
        cameraPlayAnime(std093_getCurrentGroupID(), frame_b, 0, 0);

        delay = 1;
        tick = fn_800D37CC();
        if (tick == 0x32) {
            delay = (f32)delay / 1.2f;
            if (delay < 1) delay = 1;
        }
        for (elapsed = 0; elapsed < delay; elapsed = elapsed + tick) {
            _threadSwitch();
            tick = msgGetCharsPerFrame();
        }
        std115_mapObjectPlayAnimation(0xC6A1000, frame_a, 0, 0);
        wait2 = 0xae;
        tick = fn_800D37CC();
        if (tick == 0x32) {
            wait2 = (f32)wait2 / 1.2f;
            if (wait2 < 1) wait2 = 1;
        }
        for (elapsed = 0; elapsed < wait2; elapsed = elapsed + tick) {
            _threadSwitch();
            tick = msgGetCharsPerFrame();
        }
    }

    *(f32*)lbl_803A2040 = 320.0f;
    *(f32*)(lbl_803A2040 + 4) = 240.0f;
    *(f32*)(lbl_803A2040 + 8) = 0.0f;
    lbl_8047A39C = GSresGetResource(std093_getCurrentGroupID(), lbl_8047A384);
    lbl_8047A3A4 = 90.0f;

    wait2 = 0x78 - (u32)((f32)fn_800D37CC() * 2.0f);
    if (wait2 != 0) {
        tick = fn_800D37CC();
        if (tick == 0x32) {
            wait2 = (f32)wait2 / 1.2f;
            if (wait2 < 1) wait2 = 1;
        }
    }
    for (elapsed = 0; elapsed < wait2; elapsed = elapsed + tick) {
        _threadSwitch();
        tick = msgGetCharsPerFrame();
    }
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_8002058C - 0x8002058C | size: 0x2c */
extern u8 lbl_8047A32C;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_8002058C)
void fn_8002058C(void) {
    lbl_8047A32C = 0;
    fn_801EF644(-1);
    lbl_8047A32C = 1;
    for (;;) {
        _threadSwitch();
    }
}
#endif

/* fn_800205C8 - 0x800205C8 | size: 0x44 */
extern void fn_800FB680(u32 a, u32 b, s32 c, u32 d);
extern u32 lbl_8047A350;
extern u32 lbl_80478880;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800205C8)
void fn_800205C8(u8* obj) {
    s32 mask = (s32)-0x100;
    u32 b = obj[0x8b];
    fn_800FB680(0, 0, b | mask, (&lbl_80478880)[lbl_8047A350]);
}
#endif

/* fn_8002060C - 0x8002060C | size: 0xc */
extern u32 lbl_8047A350;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_8002060C)
void fn_8002060C(void) {
    lbl_8047A350 = 0;
}
#endif

/* fn_80020618 - 0x80020618 | size: 0x304 */
extern void* windowGetKeyInfo(void);
extern s32 gamedatasaveGetStatus(s32, s32);
extern void fn_80166CC0(u32);
extern void fn_800F78A4(s32, s32, s32, s32, s32);
extern void gamedatasaveSetStatus(s32, s32, s32);
extern u32 lbl_8047B878;
extern u8 lbl_803A1FC8[];
extern u32 lbl_8047B880;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80020618)
/* Exact (2026-09-30). The option state is read straight out of
 * lbl_803A1FC8 at each use, not through a pointer local: retail rebuilds the
 * address per region (and caches it in r29 only where it uses it twice),
 * while one pointer keeps it in r31 throughout and drops a saved register.
 * The idle test compares the constant against the timer in that order, and
 * the sine phase is built in two steps, so the constant multiplies the timer
 * before the speed scales the product. */
s32 fn_80020618(void) {
    typedef struct TitleOptionState {
        s32 selection;
        s32 unk04;
        s32 counter;
        s32 toggle;
        f32 output;
        f32 unk14;
        f32 amplitude;
        f32 delta;
        f32 timer;
        f32 phase;
        f32 speed;
    } TitleOptionState;
    u8* keyInfo;
    f32 tickScale;
    f32 phase;
    s32 value;

    keyInfo = windowGetKeyInfo();
    tickScale = (f32)(s32)fn_800D37CC();
    ((TitleOptionState*)lbl_803A1FC8)->delta = (f32)(u32)msgGetCharsPerFrame() / tickScale;
    ((TitleOptionState*)lbl_803A1FC8)->phase += ((TitleOptionState*)lbl_803A1FC8)->delta;
    if (((TitleOptionState*)lbl_803A1FC8)->phase >= 1.0f) {
        ((TitleOptionState*)lbl_803A1FC8)->phase = 0.0f;
    }

    if (gamedatasaveGetStatus(0, 9) == 1) {
        if (0.0f != ((TitleOptionState*)lbl_803A1FC8)->timer) {
            ((TitleOptionState*)lbl_803A1FC8)->timer += ((TitleOptionState*)lbl_803A1FC8)->delta;
            if (((TitleOptionState*)lbl_803A1FC8)->timer >= 1.0f) {
                ((TitleOptionState*)lbl_803A1FC8)->timer = 0.0f;
                ((TitleOptionState*)lbl_803A1FC8)->counter = 0;
            }
        }
    } else {
        ((TitleOptionState*)lbl_803A1FC8)->timer += ((TitleOptionState*)lbl_803A1FC8)->delta;
        if (((TitleOptionState*)lbl_803A1FC8)->timer >= 1.0f) {
            ((TitleOptionState*)lbl_803A1FC8)->timer = 0.0f;
            ((TitleOptionState*)lbl_803A1FC8)->counter++;
            if ((((TitleOptionState*)lbl_803A1FC8)->counter % 2) == 0) {
                ((TitleOptionState*)lbl_803A1FC8)->toggle ^= 1;
                if (((TitleOptionState*)lbl_803A1FC8)->toggle != 0) {
                    ((TitleOptionState*)lbl_803A1FC8)->amplitude = 0.0f;
                } else {
                    ((TitleOptionState*)lbl_803A1FC8)->amplitude = 0.20943952f;
                }
            }
        }
    }

    phase = 6.2831855f * ((TitleOptionState*)lbl_803A1FC8)->timer;
    phase = phase * ((TitleOptionState*)lbl_803A1FC8)->speed;
    ((TitleOptionState*)lbl_803A1FC8)->output = ((TitleOptionState*)lbl_803A1FC8)->amplitude * (f32)sin(phase);

    value = *(u16*)(keyInfo + 6);
    if (value & 2) {
        if (((TitleOptionState*)lbl_803A1FC8)->selection < 2) {
            ((TitleOptionState*)lbl_803A1FC8)->selection++;
        }
    } else if (value & 1) {
        if (((TitleOptionState*)lbl_803A1FC8)->selection > 0) {
            ((TitleOptionState*)lbl_803A1FC8)->selection--;
        }
    } else {
        keyInfo = windowGetKeyInfo();
        switch (((TitleOptionState*)lbl_803A1FC8)->selection) {
        case 0:
            if (*(u16*)(keyInfo + 6) & 4) {
                fn_80166CC0(1);
            }
            if (*(u16*)(keyInfo + 6) & 8) {
                fn_80166CC0(0);
            }
            break;
        case 1:
            if (*(u16*)(keyInfo + 6) & 4) {
                if (gamedatasaveGetStatus(0, 9) != 0) {
                    fn_800F78A4(1, 0, 0xFF, 0x1E, 0);
                }
                gamedatasaveSetStatus(0, 9, 0);
                ((TitleOptionState*)lbl_803A1FC8)->counter = 1;
                ((TitleOptionState*)lbl_803A1FC8)->timer = 1.0f;
                ((TitleOptionState*)lbl_803A1FC8)->toggle = 1;
            }
            if (*(u16*)(keyInfo + 6) & 8) {
                gamedatasaveSetStatus(0, 9, 1);
                ((TitleOptionState*)lbl_803A1FC8)->amplitude = 0.0f;
            }
            break;
        }
    }
    return 0;
}
#endif

/* fn_8002091C - 0x8002091C | size: 0x10 */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_8002091C)
s32 fn_8002091C(void) {
    return *(s32*)lbl_803A1FC8;
}

/* fn_8002092C - 0x8002092C | size: 0x90 */
void fn_8002092C(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(6.2831855f * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = 0.1f * f2 + 1.1f;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 2) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}

/* fn_800209BC - 0x800209BC | size: 0x90 */
void fn_800209BC(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(6.2831855f * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = 0.1f * f2 + 1.1f;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}

/* fn_80020A4C - 0x80020A4C | size: 0x90 */
void fn_80020A4C(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(6.2831855f * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = 0.1f * f2 + 1.1f;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 0) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}

/* fn_80020ADC - 0x80020ADC | size: 0x58 */
extern u32 GSsndGetOutputMode(void);
void fn_80020ADC(void* r3, u8* r4) {
    if (GSsndGetOutputMode() != 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}

/* fn_80020B34 - 0x80020B34 | size: 0x58 */
void fn_80020B34(void* r3, u8* r4) {
    if (GSsndGetOutputMode() == 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}

/* fn_80020B8C - 0x80020B8C | size: 0x14 */
void fn_80020B8C(void* r3, u8* r4) {
    *(f32*)(r4 + 0x70) = *(f32*)(lbl_803A1FC8 + 0x10);
}

/* fn_80020BA0 - 0x80020BA0 | size: 0xfc */
extern void windowDrawSprite(s32, s32, void*, s32, s32);
extern u8 lbl_802EF0A8[];
void fn_80020BA0(void* arg0, u8* arg1) {
    s16* ptr;

    if (GSsndGetOutputMode() == 0) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FF0);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x900C);
    }
    windowDrawSprite((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);

    if (gamedatasaveGetStatus(0, 9) == 1) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FD4);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FB8);
    }
    windowDrawSprite((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);
}

/* fn_80020C9C - 0x80020C9C | size: 0x200 */
extern void winSeqCheckMove(void);
extern void fn_801D04E8(void);
extern void winMsgOpen(s32, s32, s32, s32);
extern void fn_8001E074(void);
extern void fn_801D0748(void);
extern void winMsgClose(s32);
extern void menuCloseCustom(s32, s32, s32);
void fn_80020C9C(void) {
    extern void _threadSwitch(void);
    extern u8 winSeqCheckMove(s32);
    extern void fn_800205C0(s32);
    extern u32 GSsndGetOutputMode(void);
    extern s32 gamedatasaveGetStatus(s32, s32);
    extern s32 menuOpen(s32, s32);
    extern void fn_80166CC0(u32);
    extern void gamedatasaveSetStatus(s32, s32, s32);
    extern u8 fn_801D04E8(void);
    extern void winMsgOpen(s32, s32, s32, s32);
    extern u8 fn_8001E074(s32, s32, s32, s32);
    extern void fn_801D0748(s32, s32, s32);
    extern void winMsgClose(s32);
    extern void menuCloseCustom(s32, s32, s32);

    s32 state;
    s32 active;
    u32 region;
    s32 fmt;
    s32* sp;

    state = 0;
    active = 1;
    while (winSeqCheckMove(0xAA) != 0) {
        _threadSwitch();
    }
    fn_800205C0(0);

    *(s32*)((u8*)&lbl_803A1FC8 + 0x0) = 0;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x14) = 0.0f;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x10) = 0.0f;
    *(s32*)((u8*)&lbl_803A1FC8 + 0x4) = 0;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x20) = 0.0f;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x28) = 3.0f;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x18) = 0.20943952f;
    *(s32*)((u8*)&lbl_803A1FC8 + 0x8) = 0;
    *(s32*)((u8*)&lbl_803A1FC8 + 0xc) = 0;

    do {
        switch (state) {
        case 0:
            region = GSsndGetOutputMode();
            fmt = gamedatasaveGetStatus(0, 9);
            menuOpen(0x7F, 0);
            menuOpen(0x80, 0);
            menuOpen(0x7C, 0);
            menuOpen(0x7D, 0);
            menuOpen(0x7E, 0);
            sp = (s32*)&lbl_803A1FC8;
            do {
                if (menuOpen(0x7B, 1) < 0) {
                    fn_80166CC0(region);
                    gamedatasaveSetStatus(0, 9, fmt);
                    break;
                }
            } while (sp[0] != 2);
            state = 0x64;
            break;
        case 0x64:
            *(s32*)&lbl_803A1FC8 = -1;
            if (gamedatasaveGetStatus(0, 9) != fmt) {
                if (fn_801D04E8() != 0) {
                    winMsgOpen(1, 0x3D82, 1, 0);
                    if ((s8)menuSubOpenYesNo(0, 0x3C, 0xAA, 1) == 0) {
                        fn_801D0748(7, 2, 0);
                    }
                    winMsgClose(1);
                }
            }
            menuCloseCustom(0x80, 0, 1);
            active = 0;
            break;
        }
    } while (active != 0);
}
#endif

/* fn_80020EA4 - 0x80020EA4 | size: 0xb0 */
extern u32 lbl_8047A360;
extern u32 lbl_8047A364;
extern u32 lbl_802E4EF0[];
extern u32 lbl_802E4ED8[];
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80020EA4)
void fn_80020EA4(u8* r3, u8* r4) {
    u8* r31;
    r31 = r3;
    if ((s32)(&lbl_8047A360)[1] == 4) {
        if (fn_8002091C() >= 0) {
            fn_800FB680(0, -8, r31[0x8b] | (s32)-0x100, lbl_802E4EF0[fn_8002091C()]);
        }
    } else if ((s32)(&lbl_8047A360)[1] != 5) {
        if ((s32)lbl_8047A360 >= 0) {
            fn_800FB680(0, -8, r31[0x8b] | (s32)-0x100,
                        lbl_802E4ED8[(s32)lbl_8047A360]);
        }
    }
}
#endif

/* fn_80020F54 - 0x80020F54 | size: 0x19c */
extern void* menuModelRender(void*);
extern void fn_800D61E4(void);
extern s32 fn_801EF214(void);
extern void fn_801021F8(u32, u32);
extern u8 lbl_803A1FF8[];
extern u8 lbl_80314F98[];
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80020F54)
void fn_80020F54(u8* arg0, u8* arg1) {
    extern void gfxSetFlags(s32);
    extern void gfxClearFlags(s32);
    extern void gfxSetPrimitive(s32);
    extern void gfxSetVtxDesc(u8*);
    extern void gfxLoadTexture(s32, void*);
    extern void gfxBegin(s32);
    extern void fn_800D61E4(s32, s32);
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void gfxVtxTexCoord(s32, f32, f32);
    extern void gfxEnd(void);

    void* model;
    s16 code;

    code = *(s16*)(arg1 + 6);
    switch ((s32)code) {
    case 0x90E:
        if ((s32)lbl_8047A360 >= 0) {
            lbl_8047A360 = (s32)(s8)arg0[0x95];
        }
        break;
    case 0x2D2:
        model = menuModelRender(lbl_803A1FF8);
        if (model != 0) {
            gfxSetFlags(3);
            gfxClearFlags(4);
            gfxSetPrimitive(7);
            gfxSetVtxDesc(lbl_80314F98);
            gfxLoadTexture(0, model);
            gfxBegin(2);
            fn_800D61E4(0, 0);
            fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
            gfxVtxTexCoord(0, 0.0f, 0.0f);
            fn_800D61E4(*(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56));
            fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
            gfxVtxTexCoord(0, 1.0f, 1.0f);
            gfxEnd();
        }
        /* FALL THROUGH */
    case 0x2CD:
    case 0x2D1:
        if (fn_801EF214() == 0) {
            ((s8*)arg1)[4] &= ~2;
        }
        break;
    case 0x912:
    case 0x913:
    case 0x914:
    case 0x915:
    case 0x916:
    case 0x917:
        if ((s32)(&lbl_8047A360)[1] == 5) {
            fn_801021F8(0xAA, 0);
        } else {
            fn_801021F8(0xAA, 1);
        }
        break;
    }
}
#endif

/* fn_800210F0 - 0x800210F0 | size: 0x4d4
 *
 * GStitle_LoadSaveFile: the title menu loop. Exact (2026-09-30). The shapes
 * that decide its registers, each read off the target:
 *  - 0x8047A360 is one 8-byte object (symbols.txt), so the menu result and
 *    the mode beside it are indexed, as the neighbouring functions already
 *    spell them: retail keeps the base in r30 and stores the mode at
 *    0x4(r30), where two @sda21 symbols would address each word separately;
 *  - the loop tests its flag at the bottom (do/while), so the switch is
 *    entered directly instead of jumping to the test first;
 *  - the inner menu switch lays out case 1 after cases 2 and 3;
 *  - the three message values are their own object, not the address-taken
 *    work variables: those are passed to fn_8018F4C8 by address, so reusing
 *    them puts the values in memory, while retail holds them in r27/r28
 *    across the calls. As an array their elements outrank the state web and
 *    take r28, leaving state r27; as separate scalars state takes r28 and
 *    every "li" for the mode stores merges with it (26 lines);
 *  - the confirmation answer is s8, the return type of menuSubOpenYesNo, and
 *    is its own variable rather than state: assigning it to state splits that
 *    web into a second register (6 lines).
 * The name buffer is 0x20 bytes, which is what puts the saved registers at
 * 0x3c and makes the frame 0x50.
 */
extern void menuColosseumBattle(void);
extern void floorRequestChange(void);
extern void fn_8006A718(void);
extern void std113_mapObjectUnk_8018db68(u32);
extern void fn_8010A420(void*);
extern void menuNameEntryOpen(void);
extern void menuNameEntryGetLastName(void);
extern void savedataCreate(void);
extern void fn_800056E4(void);
extern void fn_800056EC(void);
extern void heroMoveSyncWithHero(void);
extern void menuModelInit(void);
extern void fn_8010A010(void);
extern void peopleInfoBiosGetPtr(void);
extern void fn_8018F4C8(void);
extern void fn_80005748(void);
extern void fn_801EF274(void);
extern void fn_80113FE8(void);
extern u32 lbl_8047A35C;
extern u32 lbl_8047A358;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800210F0)
s32 fn_800210F0(void)
{
    extern u8 lbl_802EF0A8[];
    extern u8 lbl_803A1FF8[];
    extern s32 windowGetActiveID(void);
    extern s32 menuOpen(s32, s32);
    extern s32 menuOpenCustom(s32, ...);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void menuCloseCustom(s32, s32, s32);
    extern s32 menuColosseumBattle(s32);
    extern void fn_800205B8(u32);
    extern u32 gamedatasaveGetStatus(s32, s32);
    extern u16 fn_8006A718(s32);
    extern u8 fn_801D04E8(void);
    extern s32 floorRequestChange(s32);
    extern void menuModelInit(void*, s16, s16);
    extern s32 menuModelSetMotion(void*, u32);
    extern void fn_8010A010(void*, u32);
    extern void* peopleInfoBiosGetPtr(u32);
    extern void fn_8018F4C8(void*, s32, s32*, void*);
    extern s32 menuNameEntryOpen(s32, s32);
    extern void menuNameEntryGetLastName(void*);
    extern void savedataCreate(u32, const void*);
    extern void std104_soundSetBackgroundMusic(s32, s32, s32);
    extern void fn_800056E4(u8);
    extern void fn_800056EC(f32);
    extern void floorLink(s32, s32);
    extern s32 fn_80005748(void);
    extern s32 fn_801EF214(void);
    extern s32 fn_801EF274(void);
    s32 state;
    s32 loop;
    s32 result;
    s32 workA;
    s32 workB;
    s8 answer;
    s32 msgVals[3];
    u8 nameBuf[0x20];

    state = 0;
    loop = 1;
    (&lbl_8047A360)[1] = 1;

    do {
        switch (state) {
        case 0:
            state = 1;
            break;

        case 1:
            (&lbl_8047A360)[1] = 1;
            menuOpen(0x7F, 0);
            result = menuOpenCustom(0xAA, windowGetActiveID(),
                                    &lbl_8047A360, 0, 1, 0);
            if (result < 0) {
                lbl_8047A360 = -1;
            }
            if (result == 0) {
                state = 5;
                break;
            }

            switch ((s32)lbl_8047A360) {
            case 0:
                break;
            case 2:
                menuColosseumBattle(0);
                loop = 0;
                break;
            case 3:
                menuColosseumBattle(3);
                loop = 0;
                break;
            case 1:
                state = 2;
                break;
            case 4:
                (&lbl_8047A360)[1] = 4;
                state = 4;
                break;
            default:
                fn_800205B8(1);
                floorRequestChange(0x384);
                state = 0x64;
                break;
            }
            break;

        case 5:
            if (gamedatasaveGetStatus(0, 4) == 0) {
                result = 0x3C3A;
            } else if ((u16)fn_8006A718(0) == 1) {
                result = 0x44C9;
            } else {
                result = 0x3C35;
            }
            winMsgOpen(2, result, 1, 1);
            (&lbl_8047A360)[1] = 5;
            answer = (s8)menuSubOpenYesNo(0, 0x3C, 0xA6, 1);
            winMsgClose(1);
            if (answer == 0) {
                menuCloseCustom(0xAA, 0, 1);
                menuCloseCustom(0x7A, 0, 1);
                menuCloseCustom(0x7F, 0, 1);
                std113_mapObjectUnk_8018db68(lbl_8047A35C);
                fn_8010A420(lbl_803A1FF8);
                std104_soundSetBackgroundMusic(1, 0x3E8, 0xFF);
                menuNameEntryOpen(0, 0);
                menuNameEntryGetLastName(nameBuf);
                savedataCreate(0, nameBuf);
                fn_800056E4(1);
                fn_800056EC(0.0f);
                heroMoveSyncWithHero();
                floorLink(0x3A0, 0);
                state = 0x64;
            } else {
                state = 1;
            }
            break;

        case 2:
            fn_800056E4(0);
            if (gamedatasaveGetStatus(0, 4) != 0) {
                state = 3;
                break;
            }
            if ((u8)fn_801D04E8() == 0) {
                winMsgOpen(2, 0x44EA, 1, 0);
            } else {
                winMsgOpen(2, 0x444B, 1, 0);
            }
            winMsgClose(1);
            state = 1;
            break;

        case 3:
            menuCloseCustom(0xAA, 0, 1);
            menuCloseCustom(0x7F, 0, 1);
            menuModelInit(lbl_803A1FF8,
                          *(s16*)(lbl_802EF0A8 + 0x4EFE),
                          *(s16*)(lbl_802EF0A8 + 0x4F00));
            fn_8010A010(lbl_803A1FF8, 0xF70400);
            fn_8018F4C8(peopleInfoBiosGetPtr(0xF70400), 1, &workA, &workB);
            menuModelSetMotion(lbl_803A1FF8, workA);
            msgVals[0] = fn_80005748();
            msgVals[1] = fn_801EF214();
            msgVals[2] = fn_801EF274();
            msgctrlSetValue(0x4C, (void*)msgVals[0]);
            msgctrlSetValue(0x2F, (void*)msgVals[2]);
            msgctrlSetValue(0x30, (void*)msgVals[1]);
            menuOpen(0x16, 0);
            winMsgOpen(1, 0x3C22, 0, 1);
            answer = (s8)menuSubOpenYesNo(0, 0x3C, 0xD6, 1);
            if (answer == 0) {
                heroMoveSyncWithHero();
                fn_80113FE8();
                state = 0x64;
                fn_800056E4(1);
            } else if (answer == 1) {
                state = 1;
                fn_800056E4(1);
            } else {
                state = 1;
                fn_800056E4(1);
            }
            menuCloseCustom(0x16, 0, 1);
            winMsgClose(1);
            fn_8010A420(lbl_803A1FF8);
            break;

        case 4:
            menuCloseCustom(0x7F, 0, 1);
            fn_80020C9C();
            (&lbl_8047A360)[1] = 1;
            state = 1;
            break;

        case 0x64:
            result = (s32)lbl_8047A360;
            if (result >= 5 || result < 0) {
                result = 0;
            }
            lbl_8047A358 = result;
            loop = 0;
            break;

        default:
            break;
        }
    } while (loop != 0);

    menuCloseCustom(0xAA, 0, 1);
    menuCloseCustom(0x7A, 0, 1);
    menuCloseCustom(0x7F, 0, 1);
    std113_mapObjectUnk_8018db68(lbl_8047A35C);
    return lbl_8047A360;
}
#endif

/* fn_800215C4 - 0x800215C4 | size: 0x60 */
extern u32 lbl_8047A35C;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800215C4)
void fn_800215C4(void) {
    menuCloseCustom(0xaa, 0, 1);
    menuCloseCustom(0x7a, 0, 1);
    menuCloseCustom(0x7f, 0, 1);
    std113_mapObjectUnk_8018db68(lbl_8047A35C);
    fn_8010A420(lbl_803A1FF8);
}
#endif

/* fn_80021624 - 0x80021624 | size: 0x20 */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80021624)
void fn_80021624(void) {
    fn_800210F0();
}
#endif

/* fn_80021644 - 0x80021644 | size: 0x9c */
extern void GSscene_SetMode(void);
extern u32 lbl_8047A35C;
extern u32 lbl_8047A358;
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80021644)
void fn_80021644(void) {
    extern void std104_soundSetBackgroundMusic(s32, s32, s32);
    extern void* std093_getCurrentGroupID(void);
    extern u32 std112_modelGetResourceID(s32);
    extern void GSresGetResource(void*, u32);
    extern void cameraPlayAnime(s32, s32, s32, s32);
    extern void GSscene_SetMode(s32);
    void* handle;

    if (fn_800FF548() == 0) {
        std104_soundSetBackgroundMusic(0x446, 0, 0xff);
        handle = std093_getCurrentGroupID();
        lbl_8047A35C = std112_modelGetResourceID(0x0FFE1000);
        GSresGetResource(handle, lbl_8047A35C);
        cameraPlayAnime(0x632, 0x0FFF1800, 0, 1);
        GSscene_SetMode(4);
        lbl_8047A360 = lbl_8047A358;
        (&lbl_8047A360)[1] = 0;
    }
}
#endif

/* fn_800216E8 - 0x800216E8 | size: 0x1d4 */
extern void fn_800F96E4(void*, s32, void*);
extern void* pokemonBiosGetNicknamePtr(s32);
extern u8 lbl_80266C7C[];
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800216E8)
void fn_800216E8(void* arg0, s32 arg1, u8* arg2, s16 arg3, s32 arg4) {
    struct TitleNameCand {
        s32 code;
        s32 msg;
        s32 link;
    };
    struct TitleNameTable {
        struct TitleNameCand e[21];
    } table;
    /* RULE-EXCEPTION(user-approved): separate locals for the second search, declared first for register allocation - see docs/RULE_EXCEPTIONS.md */
    u8* cand2;
    struct TitleNameCand* entry2;
    s32 code2;
    s32 j;
    u8* cand;
    struct TitleNameCand* entry;
    s32 i;
    s32 code;
    s32 link;
    s16 x;
    s16 y;
    s32 msg;
    void* name;

    table = *(struct TitleNameTable*)lbl_80266C7C;

    if (arg3 <= 0) {
        fn_800F96E4(arg0, arg1 + 1, (void*)0x4261);
        return;
    }

    entry = table.e;
    for (i = 0; i < 21; entry++, i++) {
        code = entry->code;
        cand = arg2;
        for (j = 0; j < arg3; cand += 8, j++) {
            if (code == *(s32*)cand) {
                break;
            }
        }
        if (j < arg3) {
            break;
        }
    }

    if (i >= 21) {
        fn_800F96E4(arg0, arg1 + 1, (void*)0x4261);
        return;
    }

    msg = table.e[i].msg;
    link = table.e[i].link;
    x = *(s16*)(arg2 + j * 8 + 4);
    y = *(s16*)(arg2 + j * 8 + 6);

    if (link >= 0) {
        i++;
        entry2 = &table.e[i];
        for (; i < 21; entry2++, i++) {
            if (entry2->link >= 0 && link != entry2->link) {
                code2 = entry2->code;
                cand2 = arg2;
                for (j = 0; j < arg3; cand2 += 8, j++) {
                    if (code2 == *(s32*)cand2) {
                        break;
                    }
                }
                if (j < arg3) {
                    break;
                }
            }
        }
        if (i < 21) {
            msg = 0x4201;
        }
    }

    name = pokemonBiosGetNicknamePtr(arg4);
    msgctrlSetValue(0x32, name);
    msgctrlSetValue(0x2F, (void*)(s32)x);
    msgctrlSetValue(0x30, (void*)(s32)y);
    fn_800F96E4(arg0, arg1 + 1, (void*)msg);
}

#if !defined(GS_TITLE_800216E8_ONLY)
/* fn_800218BC - 0x800218BC | size: 0x1e0 */
extern void fn_80014118(s32, void*, void*);
extern s32 pokemonCheckValid(s32);
extern s32 fn_800141BC(void*, s32);
extern void fn_80014198(s32);
extern u8 lbl_803A1B90[];
extern u32 lbl_8047B8A0;
s32 fn_800218BC(u32 arg0, u32* arg1)
{
    extern f32 pokemonGetDp(void*);
    extern void winMsgOpen(s32, s32, s32, s32);
    extern void winMsgClose(s32);
    f32 thresh;
    void* sp_c;
    s32 sp_8;
    s32 count;
    s32 idx;
    s32 ok;
    s32 ctr;
    u8* p;
    s32 i;

    p = lbl_803A1B90;
    for (count = *((s32*) (p + 0x40)), ctr = count, i = 0; ctr > 0; ctr--) {
        if (arg0 == (*((u16*) p))) {
            thresh = thresh;
            break;
        }
        p += 8;
        i++;
    }

    if (i >= count) {
        winMsgOpen(2, 0x426A, 1, 0);
        winMsgClose(1);
        return 1;
    }
    i = 0;
    thresh = *((f32*) (&lbl_8047B8A0));
    do {
        fn_80014118(i, &sp_c, &sp_8);
        if (((u8) pokemonCheckValid((s32) sp_c)) != 0) {
            if (pokemonIsDarkPokemon((s32) sp_c) != 0) {
                if (pokemonGetDp(sp_c) > thresh) {
                    break;
                }
            }
        }
        i++;
    } while (i < 6);

    if (i >= 6) {
        winMsgOpen(2, 0x4261, 1, 0);
        winMsgClose(1);
        return 1;
    }
    idx = fn_800141BC((void*) arg0, 1);
    if (idx >= 0) {
        fn_80014118(idx, &sp_c, &sp_8);
        ok = 0;
        if (pokemonIsDarkPokemon((s32) sp_c) != 0) {
            if (pokemonGetDp(sp_c) > (*((f32*) (&lbl_8047B8A0)))) {
                ok = 1;
                ok = 1;
            }
        }
        if (ok == 0) {
            winMsgOpen(2, 0x4261, 1, 0);
            winMsgClose(1);
        }
    }
    fn_80014198(idx);
    if ((idx >= 0) && (ok != 0)) {
        *arg1 = 1;
        return 2;
    }
    return 1;
}
#endif /* !GS_TITLE_800216E8_ONLY */
#endif

/* fn_80021A9C - 0x80021A9C | size: 0x78 */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80021A9C)
s32 fn_80021A9C(u32 r3, u32* r4) {
    u8* r5;
    s32 count;

    r5 = lbl_803A1B90;
    count = *(s32*)(lbl_803A1B90 + 0x40);
    for (; count > 0; count--) {
        if (r3 == (u32)*(u16*)r5) {
            *r4 = 0;
            return 2;
        }
        r5 += 8;
    }
    winMsgOpen(2, 0x426a, 1, 0);
    winMsgClose(1);
    return 1;
}
#endif

/* fn_80021B14 - 0x80021B14 | size: 0x53c
 *
 * Field item-use handler: dispatches on itemParamGetRecoverType() of the
 * current item, applies it to the chosen party Pokemon and reports through
 * the message window; returns 0 when the item was used (and sets *arg1).
 *
 * STATUS (99.82%, one instruction): the 0/1 `result` of the 3..11 case.
 * Retail builds it in r0 at a join and returns it with `mr r3,r0`; a u8
 * result gives `clrlwi r3,r0,24` and any 32-bit result is coalesced into r3.
 * Ruled out: every result/return type, declaration position, if/else /
 * goto / ternary / shared-return forms, whole-case and tail inline helpers,
 * C++, peephole and compiler version. The only other function with this
 * join is fn_8010A210, where it comes from a u8 result fed by two
 * expansions of a multi-return inline (faceKeyEqual), so the original
 * probably used a shared inline helper here too (fn_80022478 ends the same
 * way). Details: Working/refs/upstream-matches-leftovers.tsv.
 */
extern void fn_80142EF8(void);
extern void fn_801431AC(void);
extern s32 fn_80014110(void);
extern s32 fn_80121ADC(s32, s32);
extern s16 fn_80144574(void*, s32, s32, u16, s32);
extern void soundStart(s32, s32, s32, s32);
extern void fn_8001D378(void);
extern u8 lbl_80478890[8];
extern u8 lbl_80266DB0[];
extern u8 lbl_80266D78[];
extern s32 fn_80023968(u32, u32*);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80021B14)
s32 fn_80021B14(u32 arg0, u32* arg1) {
    typedef struct TitleEffectEntry {
        s32 code;
        s16 x;
        s16 y;
    } TitleEffectEntry;
    typedef struct TitleSpecialEntry {
        s32 code;
        s32 message;
    } TitleSpecialEntry;
    typedef struct TitleSpecialTable {
        TitleSpecialEntry entries[7];
    } TitleSpecialTable;
    typedef struct TitleBgmTable {
        u16 entries[5];
    } TitleBgmTable;
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    extern s32 fn_800141BC(void*, s32);
    extern u8 fn_80121ADC(s32, s32);
    u8 item_param[0x10];
    u8 result;

    itemParamConvertOrigFormat(item_param, lbl_80478890);
    switch ((s32)itemParamGetRecoverType(item_param)) {
    case 0: {
        char message[0x84];
        TitleEffectEntry effects[32];
        TitleSpecialTable special;
        TitleBgmTable bgm;
        s32 slot;
        s32 data;
        s32 pokemon;
        s32 effectCount;
        s32 text;
        s32 code;
        s32 specialIndex;
        s32 effectIndex;
        TitleEffectEntry* effect;
        s32 bgmIndex;
        s32 y;
        s32 x;
        TitleSpecialEntry* entry;
        s32 owner;

        slot = fn_80014110();
        fn_80014118(slot, &pokemon, &data);
        if (fn_80121ADC(pokemon, 0x3E)) {
            msgctrlSetValue(0x32, (void*)pokemonBiosGetNicknamePtr(pokemon));
            winMsgOpen(2, 0x424D, 1, 0);
            winMsgClose(1);
            return 1;
        }

        effectCount = fn_80144574(effects, pokemon, data, arg0, 0);
        if ((s16)effectCount > 0) {
            bgm = *(TitleBgmTable*)lbl_80266DB0;
            for (bgmIndex = 0; bgmIndex < 5; bgmIndex++) {
                if (arg0 == bgm.entries[bgmIndex]) {
                    break;
                }
            }
            soundStart(bgmIndex < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
            fn_8001D378();
        }

        owner = pokemon;
        special = *(TitleSpecialTable*)lbl_80266D78;
        if ((s16)effectCount <= 0) {
            fn_800F96E4(message, 0x41, (void*)0x4261);
        } else {
            entry = special.entries;
            for (specialIndex = 0; specialIndex < 7; entry++, specialIndex++) {
                code = entry->code;
                effect = effects;
                for (effectIndex = 0; effectIndex < (s16)effectCount; effect++, effectIndex++) {
                    if (code == effect->code) {
                        break;
                    }
                }
                if (effectIndex < (s16)effectCount) {
                    break;
                }
            }

            if (specialIndex >= 7) {
                fn_800F96E4(message, 0x41, (void*)0x4261);
            } else {
                text = special.entries[specialIndex].message;
                x = effects[effectIndex].x;
                y = effects[effectIndex].y;
                msgctrlSetValue(0x32, (void*)pokemonBiosGetNicknamePtr(owner));
                msgctrlSetValue(0x2F, (void*)x);
                msgctrlSetValue(0x30, (void*)y);
                fn_800F96E4(message, 0x41, (void*)text);
            }
        }

        msgctrlSetValue(0x4D, message);
        winMsgOpen(2, 0xE0, 1, 0);
        winMsgClose(1);
        if ((s16)effectCount > 0) {
            *arg1 = 1;
            return 0;
        }
        return 1;
    }
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11: {
        u8 name[0x84];
        s32 data;
        TitleBgmTable bgm;
        u8 effects[0x100];
        s32 effect_count;
        s32 slot;
        s32 pokemon;
        s32 i;

        slot = fn_800141BC((void*)arg0, 1);
        if (slot >= 0) {
            fn_80014118(slot, &pokemon, &data);
            if (fn_80121ADC(pokemon, 0x3E) == 0) {
                effect_count = fn_80144574(effects, pokemon, data, arg0, 0);
                if ((s16)effect_count > 0) {
                    bgm = *(TitleBgmTable*)lbl_80266DB0;
                    for (i = 0; i < 5; i++) {
                        if (arg0 == bgm.entries[i]) {
                            break;
                        }
                    }
                    soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
                    fn_8001D378();
                }
                fn_800216E8(name, 0x40, effects, effect_count, pokemon);
                msgctrlSetValue(0x4D, name);
                winMsgOpen(2, 0xE0, 1, 0);
                winMsgClose(1);
            } else {
                msgctrlSetValue(0x32, (void*)pokemonBiosGetNicknamePtr(pokemon));
                winMsgOpen(2, 0x424D, 1, 0);
                winMsgClose(1);
                effect_count = 0;
            }
        }
        fn_80014198(slot);
        if (slot >= 0 && (s16)effect_count > 0) {
            switch (arg0) {
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
                *arg1 = 0;
                break;
            default:
                *arg1 = 1;
                break;
            }
            result = 0;
        } else {
            result = 1;
        }
        return result;
    }
    case 21:
        return fn_80023968(arg0, arg1);
    default:
        winMsgOpen(2, 0x426A, 1, 0);
        winMsgClose(1);
        return 1;
    }
}
#endif

/* fn_80022050 - 0x80022050 | size: 0x12c */
extern s32 pcboxGetNbPokemonBox(void);
extern s32 fn_801347E8(s32, s8);
extern void fn_800140FC(s32*, s32*);
extern s32 fightTrainerIsSelectedItemBall(s32);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80022050)
s32 fn_80022050(s32 arg0, s32* arg1) {
    s32 sp14;
    s32 sp10;
    s32 spC;
    s32 sp8;
    s32 iVar4;
    s32 iVar3;
    s8 cVar1;
    s8 cVar2;

    iVar3 = 0;
    iVar4 = 0;
    do {
        fn_80014118(iVar4, &spC, &sp8);
        if ((u8)pokemonCheckValid(spC) != 0) {
            iVar3 = iVar3 + 1;
        }
        iVar4 = iVar4 + 1;
    } while (iVar4 < 6);
    if (iVar3 >= 6) {
        cVar1 = pcboxGetNbPokemonBox();
        iVar3 = 0;
        while (iVar3 < cVar1) {
            cVar2 = pcboxGetPokemonBoxNbEmptySlot(0, (s8)iVar3);
            if (cVar2 > 0) {
                break;
            }
            iVar3 = iVar3 + 1;
        }
        if (iVar3 >= cVar1) {
            winMsgOpen(2, 0x4416, 1, 0);
            winMsgClose(1);
            return 1;
        }
    }
    fn_800140FC(&sp14, &sp10);
    if ((u8)fightTrainerIsSelectedItemBall(sp10) != 0) {
        winMsgOpen(2, 0x426d, 1, 0);
        winMsgClose(1);
        return 1;
    }
    *arg1 = 0;
    return 0;
}
#endif

/* fn_8002217C - 0x8002217C | size: 0x2fc */
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_8002217C)
s32 fn_8002217C(u32 wazaDataId, u32* result) {
    typedef struct TitleEffectEntry {
        s32 code;
        s16 x;
        s16 y;
    } TitleEffectEntry;
    typedef struct TitleSpecialEntry {
        s32 code;
        s32 message;
    } TitleSpecialEntry;
    typedef struct TitleSpecialTable {
        TitleSpecialEntry entries[7];
    } TitleSpecialTable;
    typedef struct TitleBgmTable {
        u16 entries[5];
    } TitleBgmTable;
    TitleEffectEntry effects[33];
    char message[0x84];
    TitleSpecialTable special;
    TitleBgmTable bgm;
    s32 slot;
    s32 pokemon;
    s32 data;
    s16 effectCount;
    s32 soundId;
    s32 text;
    s32 code;
    s32 specialIndex;
    s32 effectIndex;
    TitleEffectEntry* effect;
    s32 bgmIndex;
    s32 y;
    void* nickname;
    s32 x;
    TitleSpecialEntry* entry;
    s32 owner;

    slot = fn_80014110();
    fn_80014118(slot, &pokemon, &data);
    if ((u8)fn_80121ADC(pokemon, 0x3E)) {
        nickname = pokemonBiosGetNicknamePtr(pokemon);
        msgctrlSetValue(0x32, nickname);
        winMsgOpen(2, 0x424D, 1, 0);
        winMsgClose(1);
        return 1;
    }

    effectCount =
        fn_80144574(effects, pokemon, data, wazaDataId, 0);
    if (effectCount > 0) {
        bgm = *(TitleBgmTable*)lbl_80266DB0;
        for (bgmIndex = 0; bgmIndex < 5; bgmIndex++) {
            if (wazaDataId == bgm.entries[bgmIndex]) {
                break;
            }
        }
        soundId = bgmIndex < 5 ? 0x466 : 0x465;
        soundStart(soundId, 0, 0xFF, 0);
        fn_8001D378();
    }

    owner = pokemon;
    special = *(TitleSpecialTable*)lbl_80266D78;
    if (effectCount <= 0) {
        fn_800F96E4(message, 0x41, (void*)0x4261);
    } else {
        entry = special.entries;
        for (specialIndex = 0; specialIndex < 7; entry++, specialIndex++) {
            code = entry->code;
            effect = effects;
            for (effectIndex = 0; effectIndex < effectCount; effect++, effectIndex++) {
                if (code == effect->code) {
                    break;
                }
            }
            if (effectIndex < effectCount) {
                break;
            }
        }

        if (specialIndex >= 7) {
            fn_800F96E4(message, 0x41, (void*)0x4261);
        } else {
            text = special.entries[specialIndex].message;
            x = effects[effectIndex].x;
            y = effects[effectIndex].y;
            msgctrlSetValue(0x32, (void*)pokemonBiosGetNicknamePtr(owner));
            msgctrlSetValue(0x2F, (void*)x);
            msgctrlSetValue(0x30, (void*)y);
            fn_800F96E4(message, 0x41, (void*)text);
        }
    }

    msgctrlSetValue(0x4D, message);
    winMsgOpen(2, 0xE0, 1, 0);
    winMsgClose(1);
    if (effectCount > 0) {
        *result = 1;
        return 0;
    }
    return 1;
}

/* fn_80022478 - 0x80022478 | size: 0x2a8
 *
 * Field item-use handler with the same 0/1 result tail as fn_80021B14
 * (retail `mr r3,r0`, ours `clrlwi`); see the STATUS note there.
 */
extern void jumptable_802E4F00();
extern u8 lbl_80478888[8];
extern s32 fn_80023068(u32, u32*);
extern s32 fn_800232F0(u32, u32*);
extern s32 fn_80023760(u32, u32*);
s32 fn_80022478(u32 arg0, u32* arg1) {
    typedef struct TitleBgmList {
        u16 entries[5];
    } TitleBgmList;
    u8 name[0x84];
    s32 data;
    s16 effect_count;
    u8 item_param[0x10];
    u8 effects[0x100];
    s32 pokemon;
    s32 slot;
    TitleBgmList bgm;
    s32 i;
    u8 result;

    itemParamConvertOrigFormat(item_param, lbl_80478888);
    switch (itemParamGetRecoverType(item_param)) {
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        slot = fn_800141BC((void*)arg0, 1);
        if (slot >= 0) {
            fn_80014118(slot, &pokemon, &data);
            if ((u8)fn_80121ADC(pokemon, 0x3E) == 0) {
                effect_count = fn_80144574(effects, pokemon, data, (u16)arg0, 0);
                if (effect_count > 0) {
                    bgm = *(TitleBgmList*)lbl_80266DB0;
                    for (i = 0; i < 5; i++) {
                        if (arg0 == bgm.entries[i]) {
                            break;
                        }
                    }
                    soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
                    fn_8001D378();
                }
                fn_800216E8(name, 0x40, (u8*)effects, effect_count, pokemon);
                msgctrlSetValue(0x4D, name);
                winMsgOpen(2, 0xE0, 1, 0);
                winMsgClose(1);
            } else {
                msgctrlSetValue(0x32, (void*)pokemonBiosGetNicknamePtr(pokemon));
                winMsgOpen(2, 0x424D, 1, 0);
                winMsgClose(1);
                effect_count = 0;
            }
        }
        fn_80014198(slot);
        if (slot >= 0 && effect_count > 0) {
            switch (arg0) {
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
                *arg1 = 0;
                break;
            default:
                *arg1 = 1;
                break;
            }
            result = 0;
        } else {
            result = 1;
        }
        return result;
    case 1:
        return fn_80023760(arg0, arg1);
    case 2:
        return fn_800232F0(arg0, arg1);
    case 19:
    case 20:
        return fn_80023068(arg0, arg1);
    case 21:
        return fn_80023968(arg0, arg1);
    default:
        winMsgOpen(2, 0x426A, 1, 0);
        winMsgClose(1);
        return 1;
    }
}

/* fn_80022720 - 0x80022720 | size: 0x114
 *
 * fn_80022720(u32 arg0, u32* arg1) -> s32  -- error-report dispatcher for
 * name-entry validation. Loads a 10-entry u16+u32 table starting at
 * lbl_80266C54 (each entry: 2-byte id, 2-byte pad, 4-byte msg id), then
 * sequentially compares arg0 against the 5 header u16s (offsets 0, 8, 16,
 * 24, 32) to find a matching row index. If no match, defaults to row 5
 * (the generic error).
 *
 * Source form uses Ghidra comma-chain idiom so iVar1 increments per
 * failed compare:
 *   iVar1 = 0;
 *   if (!(a != h0 && (iVar1=1, a != h1) && (iVar1=2, a != h2) && ...))
 *     iVar1 = 5;
 *
 * Calls winMsgOpen(2, msg_from_row_iVar1, 1, 0) then winMsgClose(1) to
 * print the error message, writes 0 to *arg1, returns 0.
 *
 * Status: 70.5% matched. Blocker: target asm batches ALL 10 lwz loads
 * first THEN ALL stw stores; our CW compile interleaves load-store-load-
 * store. This is CW 1.3 scheduler behavior we have not been able to
 * force from source (tried u32* src, temp-var pair hoist, array init
 * syntax, scheduling pragmas).
 */
extern u8 lbl_80266C54[];
typedef struct TitleMsgEnt {
    u16 id;
    u32 msg;
} TitleMsgEnt;
typedef struct TitleMsgTable {
    TitleMsgEnt e[5];
} TitleMsgTable;
s32 fn_80022720(u32 arg0, u32* arg1) {
    TitleMsgTable t;
    s32 i;

    t = *(TitleMsgTable*)lbl_80266C54;
    i = 0;
    for (i = 0; i < 5; i++) {
        if (arg0 == t.e[i].id) {
            break;
        }
    }
    winMsgOpen(2, t.e[i].msg, 1, 0);
    winMsgClose(1);
    *arg1 = 0;
    return 0;
}
#pragma optimization_level 4

/* fn_80022834 - 0x80022834 | size: 0x308 */
extern void itemDataBiosGetPtr(void);
extern void itemDataBiosGetWazaMachineNo(void);
extern void itemDataBiosGetWazaIDByWazaMachineNo(void);
extern void itemDataBiosGetHidenMachineNo(void);
extern void pokemonBiosGetPokemonWazaDataId(void);
extern void pokemonBiosGetPokemonDataId(s32);
extern void pokemonDataBiosGetPtr(void);
extern void pokemonDataBiosGetWazaMcn(void);
extern void evolutionWazaLearn(void);
extern void pokemonWazaCreate(void);
extern void pokemonGetSoubiItemSoubiDataId(void);
extern void pokemonGetFriendFormPokemonFriendFilterId(void);
s32 fn_80022834(u32 arg0, u32* arg1) {
    extern void* itemDataBiosGetPtr(u32);
    extern u32 itemDataBiosGetWazaMachineNo(void*);
    extern u32 itemDataBiosGetWazaIDByWazaMachineNo(u32);
    extern u32 itemDataBiosGetHidenMachineNo(void*);
    extern s32 menuSubOpenYesNo(s32, s32, s32, s32);
    extern u32 pokemonBiosGetPokemonWazaDataId(s32, u16);
    extern void pokemonBiosGetPokemonDataId(s32);
    extern s32 pokemonDataBiosGetPtr(void);
    extern u8 pokemonDataBiosGetWazaMcn(s32, s32);
    extern s32 evolutionWazaLearn(s32, u32, void*, s32, void*, s32);
    extern void pokemonWazaCreate(s32, u32, u32);
    extern u32 pokemonGetSoubiItemSoubiDataId(s32);
    extern void pokemonGetFriendFormPokemonFriendFilterId(s32, u32, s32);
    extern void cbForgetWazaSelect__FP7PokemonUsl(void);

    u8 status;
    s32 i;
    s32 msg;
    void* handle;
    s32 c;
    u16 value16;
    s32 state;
    s32 type_byte;
    s32 action;
    s32 tmp;
    u32 v2;
    s32 effect;
    s32 slot;
    u32 sc;
    s32 sd;
    u8 buf;

    handle = itemDataBiosGetPtr((u16)arg0);
    status = (u8)itemDataBiosGetWazaMachineNo(handle);
    value16 = itemDataBiosGetWazaIDByWazaMachineNo(status);
    soundStart(0x4CB, 0, 0xFF, 0);
    type_byte = itemDataBiosGetHidenMachineNo(handle);
    if ((u8)type_byte != 0xFF) {
        msg = 0x4260;
    } else {
        msg = 0x4265;
    }
    msgctrlSetValue(0x39, (void*)value16);
    winMsgOpen(2, msg, 1, 0);
    winMsgClose(1);
    winMsgOpen(2, 0x426B, 1, 0);
    state = (s8)menuSubOpenYesNo(0, -1, -1, 0);
    winMsgClose(1);
    switch (state) {
    case 0:  action = 0; break;
    case 1:  action = 1; break;
    default: action = 2; break;
    }
    if (action == 1 || action == 2) {
        return 1;
    }

    slot = fn_800141BC((void*)arg0, 1);
    if (slot >= 0) {
        fn_80014118(slot, &sc, &sd);
        c = sc;
        if (pokemonIsDarkPokemon(c) != 0) {
            winMsgOpen(2, 0x424C, 1, 0);
            winMsgClose(1);
            tmp = 0;
        } else {
            v2 = itemDataBiosGetWazaIDByWazaMachineNo(status);
            for (i = 0; i < 4; i++) {
                if ((u16)v2 == (u16)pokemonBiosGetPokemonWazaDataId(c, i)) {
                    break;
                }
            }
            if (i < 4) {
                msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(c));
                msgctrlSetValue(0x39, (void*)(u16)v2);
                winMsgOpen(2, 0x4244, 1, 0);
                winMsgClose(1);
                tmp = 0;
            } else {
                pokemonBiosGetPokemonDataId(c);
                if (pokemonDataBiosGetWazaMcn(pokemonDataBiosGetPtr(), status) == 0) {
                    msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(c));
                    msgctrlSetValue(0x39, (void*)(u16)v2);
                    winMsgOpen(2, 0x423F, 1, 0);
                    winMsgClose(1);
                    tmp = 0;
                } else {
                    tmp = evolutionWazaLearn(c, v2, &buf, 1, cbForgetWazaSelect__FP7PokemonUsl, 0);
                    if (tmp != 0) {
                        pokemonWazaCreate(c, buf, v2 & 0xFFFF);
                        pokemonGetFriendFormPokemonFriendFilterId(c, pokemonGetSoubiItemSoubiDataId(c), 4);
                    }
                }
            }
        }
        effect = tmp;
    }

    fn_80014198(slot);
    if (slot >= 0 && effect != 0) {
        if ((u8)type_byte != 0xFF) {
            *arg1 = 0;
        } else {
            *arg1 = 1;
        }
        return 0;
    }
    return 1;
}

/* fn_80022B3C - 0x80022B3C | size: 0x318 */
extern u16 pokemonDataBiosGetVoice(void);
extern void soundStartSE(s32);
extern s32 GSsndGetStatus(s32);
extern void fn_80121B4C(s32, s32);
extern void pokemonAddDpFormPokemonDpFilterId(s32, s32, s32);
extern u8 lbl_80266C30[];
extern u32 lbl_8047B8A0;
s32 std163_useCologne(s32 arg0, s32 arg1) {
    typedef struct TitleItemMessage {
        u16 item;
        u32 useMessage;
        u32 dpMessage;
    } TitleItemMessage;
    typedef struct TitleItemMessageTable {
        TitleItemMessage entry[3];
    } TitleItemMessageTable;
    extern void pokemonBiosGetPokemonDataId(s32);
    extern void* pokemonDataBiosGetPtr(void);
    extern u16 pokemonDataBiosGetVoice(void*);
    s32 pokemon;
    s32 data;
    TitleItemMessageTable table;
    s32 i;
    s32 entryOffset;
    u32 target;
    s32 wait;
    s32 voice;
    void* bios;
    s32 j;
    void* nickname;

    table = *(TitleItemMessageTable*)lbl_80266C30;
    fn_80014118(arg1, &pokemon, &data);
    if ((u8)pokemonCheckValid(pokemon) == 0) {
        return 1;
    }

    for (i = 0; i < 3; i++) {
        if ((u16)arg0 == table.entry[i].item) {
            break;
        }
    }
    if (i >= 3) {
        return 1;
    }
    nickname = pokemonBiosGetNicknamePtr(pokemon);
    msgctrlSetValue(0x32, nickname);
    msgctrlSetValue(0x2D, (void*)arg0);
    entryOffset = i * sizeof(TitleItemMessage);
    winMsgOpen(2, *(u32*)((u8*)&table.entry[0].useMessage + entryOffset), 1, 0);
    winMsgClose(1);

    if (gamedatasaveGetStatus(0, 9) == 0) {
        for (j = 0; j < 2; j++) {
            fn_800F78A4(1, 0, 0xFF, 0x15, 0);
            for (wait = 0; wait < 0x3C; wait += msgGetCharsPerFrame()) {
                _threadSwitch();
            }
        }
        fn_800F78A4(1, 0, 0xFF, 0x30, 0);
        for (wait = 0; wait < 0x60; wait += msgGetCharsPerFrame()) {
            _threadSwitch();
        }
    }

    target = pokemon;
    if (target == 0) {
        bios = 0;
    } else if ((u8)pokemonCheckValid(target) == 0) {
        bios = 0;
    } else {
        pokemonBiosGetPokemonDataId(target);
        bios = pokemonDataBiosGetPtr();
    }
    if (bios == 0) {
        voice = 0;
    } else {
        voice = pokemonDataBiosGetVoice(bios);
        soundStartSE(voice);
    }
    while (GSsndGetStatus(voice) == 2) {
        _threadSwitch();
    }

    msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
    winMsgOpen(2, 0x3AF7, 1, 0);
    winMsgClose(1);
    if ((u8)fn_80121ADC(pokemon, 0x3E) != 0) {
        fn_80121B4C(pokemon, 0x3E);
        winMsgOpen(2, 0x4277, 1, 0);
        winMsgClose(1);
    }
    if (pokemonGetDp((void*)pokemon) > *(f32*)&lbl_8047B8A0) {
        pokemonAddDpFormPokemonDpFilterId(pokemon, (u16)arg0, 4);
        msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
        winMsgOpen(2, *(u32*)((u8*)&table.entry[0].dpMessage + entryOffset), 1, 0);
        winMsgClose(1);
    }
    return 1;
}
#endif

/* fn_80022E54 - 0x80022E54 | size: 0x90 */
extern void fadeSet(f32, s32);
extern void std017_menuQueueAdd(void*, s32);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80022E54)
s32 fn_80022E54(void* r3, u32* r4) {
    void* r29;
    u32* r30;
    s32 r31;
    u32 sc, sd;
    r29 = r3;
    r30 = r4;
    r31 = fn_800141BC(r29, 1);
    if (r31 >= 0) {
        fn_80014118(r31, &sc, &sd);
    }
    fn_80014198(r31);
    if (r31 >= 0) {
        std017_menuQueueAdd(r29, 0x5960010);
        *r30 = 1;
        return 2;
    }
    return 1;
}

/* fn_80022EE4 - 0x80022EE4 | size: 0x184 */
extern s32 pokemonEvolutionCheck(s32, s32, u16, void*, void*);
extern void menuOffScreenSetDisp(s32);
extern void pokemonEvolutionAll(s32, s32, u16, void*, s32, s32, s32, s32);
s32 fn_80022EE4(u32 arg0, u32* arg1) {
    extern s32 fn_80144574(void*, s32, s32, u16, s32);
    u8 text_buf[0x100];
    s32 slot;
    s32 effect;
    s32 sc;
    s32 sd;
    u16 sp8;
    u32 spC;

    slot = fn_800141BC((void*)arg0, 1);
    if (slot >= 0) {
        fn_80014118(slot, &sc, &sd);
        if ((u8)pokemonIsDarkPokemon(sc) == 0) {
            effect = fn_80144574(text_buf, sc, sd, (u16)arg0, 0);
            if ((s16)effect <= 0) {
                winMsgOpen(2, 0x4261, 1, 0);
                winMsgClose(1);
            }
        } else {
            winMsgOpen(2, 0x424C, 1, 0);
            winMsgClose(1);
        }
    }

    fn_80014198(slot);

    if (slot >= 0 && (s16)effect > 0) {
        effect = pokemonEvolutionCheck(sc, 1, (u16)arg0, &sp8, &spC);
        fadeSet(0.5f, 3);
        fadeCheck(1);
        menuOffScreenSetDisp(0);
        pokemonEvolutionAll(sc, effect, sp8, &spC, 0, 1, 0, 0);
        menuOffScreenSetDisp(1);
        fadeSet(0.5f, 2);
        fadeCheck(1);
        *arg1 = 1;
        return 0;
    }

    return 1;
}
#endif

/* fn_80023068 - 0x80023068 | size: 0x20c */
extern s32 fn_80019B48(s32);
extern void fn_80019B1C(void);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80023068)
s32 fn_80023068(u32 arg0, u32* arg1) {
    typedef struct TitleBgmList {
        u16 entries[5];
    } TitleBgmList;
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    extern s32 fn_800141BC(void*, s32);
    extern u8 fn_80121ADC(s32, s32);
    s32 pokemon;
    s32 slot;
    TitleBgmList bgm;
    u8 name[0x84];
    u8 effects[0x108];
    s32 effect_count;
    s32 mode;
    s32 data;
    s32 blocked;
    s32 i;
    void* nickname;

    blocked = 0;
    do {
        slot = fn_800141BC((void*)arg0, 1);
        if (slot < 0) {
            break;
        }
        fn_80014118(slot, &pokemon, &data);
        if (fn_80121ADC(pokemon, 0x3E) != 0) {
            blocked = 1;
            break;
        }
        mode = fn_80019B48((s8)slot);
        fn_80019B1C();
    } while (mode < 0);

    if (slot >= 0) {
        if (blocked != 0) {
            nickname = pokemonBiosGetNicknamePtr(pokemon);
            msgctrlSetValue(0x32, nickname);
            winMsgOpen(2, 0x424D, 1, 0);
            winMsgClose(1);
            effect_count = 0;
        } else {
            effect_count = fn_80144574(effects, pokemon, data, arg0, mode);
            if ((s16)effect_count > 0) {
                bgm = *(TitleBgmList*)lbl_80266DB0;
                for (i = 0; i < 5; i++) {
                    if (arg0 == bgm.entries[i]) {
                        break;
                    }
                }
                soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
                fn_8001D378();
            }
            fn_800216E8(name, 0x40, effects, effect_count, pokemon);
            msgctrlSetValue(0x4D, name);
            winMsgOpen(2, 0xE0, 1, 0);
            winMsgClose(1);
        }
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)effect_count > 0) {
        *arg1 = 1;
        return 0;
    }
    return 1;
}
#endif

/* cbForgetWazaSelect__FP7PokemonUsl - 0x80023274 | size: 0x7c */
extern s32 fn_80097B04(s32, s32);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_80023274)
s32 cbForgetWazaSelect__FP7PokemonUsl(s32 r3, s32 r4) {
    s32 result;

    fadeSet(0.5f, 3);
    fadeCheck(1);
    result = fn_80097B04(r3, r4);
    if (result >= 4) {
        result = -1;
    }
    fadeSet(0.5f, 2);
    fadeCheck(1);
    return (s8)result;
}
#endif

/* fn_800232F0 - 0x800232F0 | size: 0x470 */
extern s32 pokemonGetStatus(u32, s32, s32, s32);
extern void std130_soundUnk_801653cc(s32, s32, s32);
extern void pokemonBiosGetLevel(void);
extern void winMsgOpenLevelUpFiledStatus(void);
extern void winMsgCloseLevelUpStatus(void);
extern void pokemonSearchWazaDataId(void);
extern void pokemonGetOboeWazaDataId(void);
extern void fn_8011FC74(void);
extern void fn_80106D3C(void);
extern void fn_801069FC(void);
extern void fn_80132A38(void);
extern void fn_8011F4F0(void);
extern void fn_8011F4A8(void);
extern void fn_80105D48(void);
extern void fn_80105C68(void);
extern void fn_801236F8(void);
extern void fn_80123B5C(void);
extern void fn_802600E4(void);
extern void fn_80123D58(void);
extern void fn_80128A64(void);
extern void fn_801C41C8(void);
extern void fn_801C40F0(void);
extern void fn_801096F8(void);
extern void fn_8012805C(void);
extern s32 cbForgetWazaSelect__FP7PokemonUsl(s32, s32);
#if !defined(GS_TITLE_SPLIT) || defined(GS_TITLE_RANGE_800232F0)
s32 fn_800232F0(u32 wazaDataId, u32* result) {
    extern u32 pokemonBiosGetLevel(s32);
    extern s32 winMsgOpenLevelUpFiledStatus(void*, s32);
    extern void winMsgCloseLevelUpStatus(s32);
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetOboeWazaDataId();
    extern s32 evolutionWazaLearn();
    extern void pokemonWazaCreate();

    typedef struct TitleLevelUpStatus {
        u8 mode;
        s16 hp;
        s16 attack;
        s16 defense;
        s16 speed;
        s16 spAttack;
        s16 spDefense;
    } TitleLevelUpStatus;
    extern s32 fn_800141BC(void*, s32);
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    s32 pokemon;
    s32 data;
    u16 evolveTo;
    u32 evolveData;
    u8 learnSlot;
    u8 index;
    s16 oldHp;
    TitleLevelUpStatus delta;
    s16 oldAttack;
    s16 oldDefense;
    s16 oldSpAttack;
    s32 target;
    TitleLevelUpStatus after;
    u8 effects[0x100];
    s16 oldSpDefense;
    s16 oldSpeed;
    s32 slot;
    s32 effectCount;
    u32 move;
    u32 level;
    u32 evolution;
    void* nickname;

    index = 0;
    slot = fn_800141BC((void*)wazaDataId, 1);
    if (slot >= 0) {
        fn_80014118(slot, &pokemon, &data);
        if (pokemonIsDarkPokemon(pokemon) == 0) {
            oldHp = pokemonGetStatus(target = pokemon, 0, 0x87, 0);
            oldAttack = pokemonGetStatus(target, 0, 0x88, 0);
            oldDefense = pokemonGetStatus(target, 0, 0x89, 0);
            oldSpAttack = pokemonGetStatus(target, 0, 0x8A, 0);
            oldSpDefense = pokemonGetStatus(target, 0, 0x8B, 0);
            oldSpeed = pokemonGetStatus(target, 0, 0x8C, 0);
            effectCount = fn_80144574(effects, pokemon, data, wazaDataId, 0);
            if ((s16)effectCount > 0) {
                fn_8001D378();
            } else {
                effectCount = 0;
            }
        } else {
            effectCount = -1;
        }

        switch ((s16)effectCount) {
        case 0:
            winMsgOpen(2, 0x4261, 1, 0);
            winMsgClose(1);
            break;
        case -1:
            winMsgOpen(2, 0x424C, 1, 0);
            winMsgClose(1);
            break;
        default:
            std130_soundUnk_801653cc(0x4CA, 0, 0xFF);
            nickname = pokemonBiosGetNicknamePtr(pokemon);
            msgctrlSetValue(0x32, nickname);
            level = pokemonBiosGetLevel(pokemon);
            msgctrlSetValue(0x2F, (void*)(u8)level);
            winMsgOpen(2, 0x423E, 1, 0);
            winMsgClose(1);

            target = pokemon;
            after.hp = pokemonGetStatus(target, 0, 0x87, 0);
            after.attack = pokemonGetStatus(target, 0, 0x88, 0);
            after.defense = pokemonGetStatus(target, 0, 0x89, 0);
            after.spAttack = pokemonGetStatus(target, 0, 0x8A, 0);
            after.spDefense = pokemonGetStatus(target, 0, 0x8B, 0);
            after.speed = pokemonGetStatus(target, 0, 0x8C, 0);
            delta.hp = after.hp - oldHp;
            delta.attack = after.attack - oldAttack;
            delta.defense = after.defense - oldDefense;
            delta.spAttack = after.spAttack - oldSpAttack;
            delta.spDefense = after.spDefense - oldSpDefense;
            delta.speed = after.speed - oldSpeed;
            delta.mode = 1;
            winMsgOpenLevelUpFiledStatus(&delta, 1);
            after.mode = 0;
            winMsgOpenLevelUpFiledStatus(&after, 1);
            winMsgCloseLevelUpStatus(1);

            level = pokemonBiosGetLevel(pokemon);
            index = 0;
            while ((u16)(move = pokemonGetOboeWazaDataId(pokemon, level, &index)) != 0) {
                if (pokemonSearchWazaDataId(pokemon, move) == -1 &&
                    evolutionWazaLearn(pokemon, move, &learnSlot, 1,
                                       cbForgetWazaSelect__FP7PokemonUsl, 0) != 0) {
                    pokemonWazaCreate(pokemon, learnSlot, move);
                }
                index++;
            }
            evolution = pokemonEvolutionCheck(pokemon, 0, 0, &evolveTo, &evolveData);
            break;
        }
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)effectCount > 0) {
        if ((u16)evolution != 0) {
            fadeSet(0.5f, 3);
            fadeCheck(1);
            menuOffScreenSetDisp(0);
            pokemonEvolutionAll(pokemon, evolution, evolveTo, &evolveData, 0, 1, 1, 0);
            menuOffScreenSetDisp(1);
            fadeSet(0.5f, 2);
            fadeCheck(1);
        }
        *result = 1;
        return 0;
    }
    return 1;
}

/* fn_80023760 - 0x80023760 | size: 0x208 */
s32 fn_80023760(u32 arg0, u32* arg1) {
    typedef struct TitleBgmList {
        u16 entries[5];
    } TitleBgmList;
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    extern s32 fn_800141BC(void*, s32);
    extern u8 fn_80121ADC(s32, s32);
    s32 pokemon;
    u8* text;
    s32 slot;
    u8 name[0x84];
    u8 effects[0x108];
    u16 total;
    TitleBgmList bgm;
    s32 effect_count;
    s32 i;
    s32 result;
    s32 data;

    total = 0;
    fn_800141BC((void*)arg0, 0);
    text = name;
    for (slot = 0; slot < 6; slot++) {
        fn_80014118(slot, &pokemon, &data);
        if ((u8)pokemonCheckValid(pokemon) == 0) {
            continue;
        }
        if ((s32)pokemonGetStatus(pokemon, 0, 0x83, 0) > 0) {
            continue;
        }
        if (fn_80121ADC(pokemon, 0x3E) != 0) {
            continue;
        }
        effect_count = fn_80144574(effects, pokemon, data, arg0, 0);
        if ((s16)effect_count <= 0) {
            continue;
        }
        bgm = *(TitleBgmList*)lbl_80266DB0;
        for (i = 0; i < 5; i++) {
            if (arg0 == bgm.entries[i]) {
                break;
            }
        }
        soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
        fn_8001D378();
        fn_800216E8(name, 0x40, effects, effect_count, pokemon);
        msgctrlSetValue(0x4D, text);
        winMsgOpen(2, 0xE0, 1, 0);
        winMsgClose(1);
        total += effect_count;
    }

    if (total == 0) {
        winMsgOpen(2, 0x4261, 1, 0);
        winMsgClose(1);
        result = -1;
    } else {
        result = 1;
    }
    fn_80014198(-1);
    if (result < 0) {
        return 1;
    }
    *arg1 = 1;
    return 0;
}

/* fn_80023968 - 0x80023968 | size: 0x234 */
extern void* itemDataBiosGetItemEffectParam(void*);
extern void* itemParamGetPtr(void*);
extern u8 itemParamGetPPSelectFlag(void*);
s32 fn_80023968(u32 arg0, u32* arg1) {
    typedef struct TitleBgmList {
        u16 entries[5];
    } TitleBgmList;
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    extern s32 fn_800141BC(void*, s32);
    extern u8 fn_80121ADC(s32, s32);
    extern void* itemDataBiosGetPtr(u16);
    s32 pokemon;
    s32 slot;
    TitleBgmList bgm;
    u8 name[0x84];
    u8 effects[0x108];
    s32 effect_count;
    s32 mode;
    s32 data;
    s32 blocked;
    s32 i;
    void* nickname;

    blocked = 0;
    do {
        slot = fn_800141BC((void*)arg0, 1);
        if (slot < 0) {
            break;
        }
        fn_80014118(slot, &pokemon, &data);
        if (fn_80121ADC(pokemon, 0x3E) != 0) {
            blocked = 1;
            break;
        }
        if (itemParamGetPPSelectFlag(itemParamGetPtr(itemDataBiosGetItemEffectParam(
                itemDataBiosGetPtr(arg0))))) {
            mode = fn_80019B48((s8)slot);
            fn_80019B1C();
        } else {
            mode = 0;
        }
    } while (mode < 0);

    if (slot >= 0) {
        if (blocked != 0) {
            nickname = pokemonBiosGetNicknamePtr(pokemon);
            msgctrlSetValue(0x32, nickname);
            winMsgOpen(2, 0x424D, 1, 0);
            winMsgClose(1);
            effect_count = 0;
        } else {
            effect_count = fn_80144574(effects, pokemon, data, arg0, mode);
            if ((s16)effect_count > 0) {
                bgm = *(TitleBgmList*)lbl_80266DB0;
                for (i = 0; i < 5; i++) {
                    if (arg0 == bgm.entries[i]) {
                        break;
                    }
                }
                soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
                fn_8001D378();
            }
            fn_800216E8(name, 0x40, effects, effect_count, pokemon);
            msgctrlSetValue(0x4D, name);
            winMsgOpen(2, 0xE0, 1, 0);
            winMsgClose(1);
        }
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)effect_count > 0) {
        *arg1 = 1;
        return 0;
    }
    return 1;
}

/* fn_80023B9C - 0x80023B9C | size: 0x20c */
s32 fn_80023B9C(u32 arg0, u32* arg1) {
    typedef struct TitleBgmList {
        u16 entries[5];
    } TitleBgmList;
    extern s32 fn_80144574(void*, s32, s32, u16, u8);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    extern s32 fn_800141BC(void*, s32);
    extern u8 fn_80121ADC(s32, s32);
    s32 pokemon;
    s32 data;
    TitleBgmList bgm;
    u8 effects[0x100];
    u8 name[0x84];
    s32 slot;
    s32 effect_count;
    s32 i;
    void* nickname;

    slot = fn_800141BC((void*)arg0, 1);
    if (slot >= 0) {
        fn_80014118(slot, &pokemon, &data);
        if (fn_80121ADC(pokemon, 0x3E) == 0) {
            effect_count = fn_80144574(effects, pokemon, data, arg0, 0);
            if ((s16)effect_count > 0) {
                bgm = *(TitleBgmList*)lbl_80266DB0;
                for (i = 0; i < 5; i++) {
                    if (arg0 == bgm.entries[i]) {
                        break;
                    }
                }
                soundStart(i < 5 ? 0x466 : 0x465, 0, 0xFF, 0);
                fn_8001D378();
            }
            fn_800216E8(name, 0x40, effects, effect_count, pokemon);
            msgctrlSetValue(0x4D, name);
            winMsgOpen(2, 0xE0, 1, 0);
            winMsgClose(1);
        } else {
            nickname = pokemonBiosGetNicknamePtr(pokemon);
            msgctrlSetValue(0x32, nickname);
            winMsgOpen(2, 0x424D, 1, 0);
            winMsgClose(1);
            effect_count = 0;
        }
    }
    fn_80014198(slot);
    if (slot >= 0 && (s16)effect_count > 0) {
        switch (arg0) {
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
            *arg1 = 0;
            break;
        default:
            *arg1 = 1;
            break;
        }
        return 0;
    }
    return 1;
}
#endif
