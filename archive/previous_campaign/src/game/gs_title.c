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
 *   fn_80021B14  GStitle_NameEntryDraw        -- 0x53C bytes, render keyboard UI
 *   fn_80022050  GStitle_DrawKeyboard         -- 0x12C bytes, keyboard grid
 *   fn_8002217C  GStitle_DrawNamePreview      -- 0x2FC bytes, live name preview
 *   fn_80022478  GStitle_DrawCaseSwitch       -- 0x2A8 bytes, upper/lower case switch
 *   fn_80022720  GStitle_CameraFlythrough     -- 0x114 bytes, title camera movement
 *   fn_80022834  GStitle_CameraInterpolate    -- 0x308 bytes, camera spline interp
 *   fn_80022B3C  GStitle_ParticleEffects      -- 0x318 bytes, title particle system
 *   fn_80022E54  GStitle_GetEffectCount       -- 0x90 bytes, count active particles
 *   fn_80022EE4  GStitle_SpawnEffect          -- 0x184 bytes, spawn new particle
 *   fn_80023068  GStitle_UpdateEffects        -- 0x20C bytes, update particle positions
 *   fn_80023274  GStitle_DestroyEffect        -- 0x7C bytes, remove expired particle
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
 *           fn_80113828(0x39C, 0);  // load autodemo camera
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
 *     fn_8005D934 / fn_8005DA18 -- linked-list/menu accessors
 *     fn_800D3088 / fn_800D37CC -- frame timing (returns u32/s32 ticks)
 *     fn_800E0CA0 / fn_800E090C -- vec3 transform helpers
 *     fn_800FA280 / fn_80132A38 -- message/window callbacks
 *     fn_80106D3C / fn_801069FC -- text print + wait
 *     fn_801902E0 -- config flag check (returns u8)
 *     fn_80165A20 / soundStop -- audio sequence control
 *     fn_801C40F0 / fn_801C41C8 -- BGM volume/fade
 *
 * --------------------------------------------------------------------------
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* Thread management */
extern void  GSthreadCreate(s32 priority, void* stack, s32 stackSize,
                          s32 flags, s32 p5, void* entry);
extern void  _threadSwitch(void);
extern void  fn_800F05A0(void* threadCtx);

/* Scene/camera */
extern void  fn_80113828(s32 cameraId, s32 mode);
extern u8    fn_800FF548(void);
extern void  fn_800FF56C(s32 floorId);

/* Sound */
extern void  fn_80166AB8(s32 soundId, s32 p2, s32 p3);
extern void  fn_801669E4(s32 soundId, s32 p2, s32 p3);

/* Save system */
extern void  fn_8011288C(s32 p1, s32 p2);

/* Input */
extern void  fn_801EF644(s32 result);

/* Math */
extern f32   GSmodelGetAnimFrame(void);               /* Get distance */
extern f64 sin(f32);

/* Named asm symbols (used in inline asm .inc files) */
extern void menuModelSetMotion(void);
extern u8 menuSubOpenYesNo(s32, s32, s32, s32);
extern void itemParamConvertOrigFormat(void);
extern void itemParamGetRecoverType(void);
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

/* =========================================================================
 * Function: GStitle_MainLoop
 * Address:  0x800203B4
 * Size:     0xE8
 * ========================================================================= */

/* =========================================================================
 * Function: GStitle_Cleanup
 * Address:  0x80020328
 * Size:     0x8C
 *
 * Cancels active events on slots 0x13, 0x15, 0x16, closes any open
 * message boxes, then waits for the title thread to complete.
 * ========================================================================= */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 4 functions matched
 * =================================================================== */

extern u32 lbl_8047A310;
extern u32 lbl_8047A350;
extern u32 lbl_8047A358;

/* Address: 0x800205B8 | Size: 0x8 | Pattern: sda_setter */
void fn_800205B8(u32 val) {
    lbl_8047A310 = val;
}

/* Address: 0x800205C0 | Size: 0x8 | Pattern: sda_setter */
void fn_800205C0(u32 val) {
    lbl_8047A350 = val;
}

/* Address: 0x80020E9C | Size: 0x8 | Pattern: return_constant */
u32 fn_80020E9C(void) { return 0; }

/* Address: 0x800216E0 | Size: 0x8 | Pattern: sda_setter */
void fn_800216E0(u32 val) {
    lbl_8047A358 = val;
}

/* =========================================================================
 * Stubs for remaining GStitle functions (0x80024438-0x80025F84)
 * ========================================================================= */

extern u32 lbl_80478DDC;
extern u32 fn_801902E0(void*);

/* 0x4C | fn_80024438 | check_then_call */
void fn_80024438(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x80));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_80024484 | check_then_call */
/* fn_80024484 - 0x80024484 | size: 0x4c */
void fn_80024484(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x70));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_800244D0 | check_then_call */
/* fn_800244D0 - 0x800244D0 | size: 0x4c */
void fn_800244D0(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x60));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_8002451C | check_then_call */
/* fn_8002451C - 0x8002451C | size: 0x4c */
void fn_8002451C(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x50));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_80024568 | check_then_call */
/* fn_80024568 - 0x80024568 | size: 0x4c */
void fn_80024568(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x40));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_800245B4 | check_then_call */
/* fn_800245B4 - 0x800245B4 | size: 0x4c */
void fn_800245B4(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x30));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_80024600 | check_then_call */
/* fn_80024600 - 0x80024600 | size: 0x4c */
void fn_80024600(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x20));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x4C | fn_8002464C | check_then_call */
/* fn_8002464C - 0x8002464C | size: 0x4c */
void fn_8002464C(u32 arg1, u32 arg2) {
    void* ctx = *(void**)&lbl_80478DDC;
    u32 result = (u32)fn_801902E0(*(void**)((u8*)ctx + 0x10));
    if ((result & 0xFF) == 0) {
        fn_80109220(arg2, 0);
    }
}

/* 0x80024698 | 0x4 -- nop */
#if 0
asm void fn_80024698(void) {
#include "src/game/gs_title_fn_80024698.inc"
}
#else
#pragma optimization_level 4
void fn_80024698(void) { }
#endif

/* 0x8002469C | 0x30 */
extern void* fn_800FA280(u32);
extern void fn_80132A38(s32, void*);
#if 0
asm void fn_8002469C(void) {
#include "src/game/gs_title_fn_8002469C.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling off
void fn_8002469C(void) {
    fn_80132A38(0x37, fn_800FA280(0x3cdf));
}
#endif

/* 0x800246CC | 0x30 */
#if 0
asm void fn_800246CC(void) {
#include "src/game/gs_title_fn_800246CC.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling off
void fn_800246CC(void) {
    fn_80132A38(0x37, fn_800FA280(0x3ce4));
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
 * After matching, writes alpha to arg1[0x67] and invokes fn_80132A38(0x37,..)
 * with a value pulled from lbl_80478DE4 indexed by lbl_802E4F58[table_index].
 *
 * The three inner blocks are deliberate duplicates (not a helper) -- matches
 * target asm exactly via goto LAB_XXX pattern (not break), which emits the
 * bne+b branch pair CW uses when the loop has a jump-table-style exit.
 *
 * Status: 91.6% matched. Remaining diffs: 7 non-volatile reg allocation
 * (uses r25-r31 vs target r27-r31 - we have 2 extra locals) + one extra
 * clrlwi from the (u8)fn_801902E0 cast on ret.
 */
extern void* fn_8005DA18(s32);
extern void* fn_8005D934(u32);
extern u32 lbl_80478DE4;
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern f32 lbl_8047B8D8;
extern u32 lbl_8047A368;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
extern f32 lbl_8047B8DC;
extern u32 lbl_8047A36C;
extern u8 lbl_802E4F58[];
extern u32 lbl_80478DE0;
#if 0
asm void fn_800246FC(void) {
#include "src/game/gs_title_fn_800246FC.inc"
}
#else
#pragma push
#pragma scheduling on
void fn_800246FC(u8* arg0, u8* arg1) {
    extern u8 lbl_802E4F58[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_80478DDC;
    extern u32 lbl_80478DE0;
    extern u32 lbl_80478DE4;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A36C;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047B8D8;
    extern f32 lbl_8047B8DC;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern u32 fn_801902E0(void*);
    extern void* fn_800FA280(u32);
    extern void fn_80132A38(s32, void*);
    u8* node;
    u8* found;
    u32 slot;
    u32 slot_offset;
    s32 table_index;
    s32 count;
    s32 table_offset;
    u32 alpha;

    slot = 0;
    slot_offset = 0;
    while (slot < *(u32*)lbl_80478DE0) {
        if (*(u32*)(lbl_80478DE4 + slot_offset) == 0 ||
            (u8)fn_801902E0((void*)*(u32*)(lbl_80478DE4 + slot_offset)) != 0) {
            switch ((s32)lbl_8047A370) {
            case 1:
                if (lbl_80478898 > lbl_8047B8D8) {
                    table_index = lbl_8047A368;
                    node = fn_8005DA18(*(u32*)(arg0 + 4));
                    node = fn_8005D934(*(s16*)(node + 4));
                    count = 0;
                    while (1) {
                        if ((((u32)*(volatile u8*)node >> 7) & 1) != 0) {
                            if (table_index == count) {
                                found = node;
                                break;
                            }
                            count++;
                        }
                        if ((((u32)*(volatile u8*)node >> 6) & 1) == 0) {
                            node = fn_8005D934(*(s16*)(node + 0x18));
                        } else {
                            found = 0;
                            break;
                        }
                    }
                    table_offset = 0;
                    for (table_index = 0; (u32)table_index < *(u32*)lbl_80478DD8; table_index++) {
                        if (found == fn_8005D934(*(u32*)(lbl_80478DDC + table_offset + 8))) {
                            goto LAB_800247D8;
                        }
                        table_offset += 0x10;
                    }
                    table_index = 0;
                LAB_800247D8:;
                    alpha = (u32)(s32)(lbl_8047B8DC * ((lbl_80478898 - lbl_8047B8D8) / lbl_8047B8D8));
                } else {
                    table_index = lbl_8047A36C;
                    node = fn_8005DA18(*(u32*)(arg0 + 4));
                    node = fn_8005D934(*(s16*)(node + 4));
                    count = 0;
                    while (1) {
                        if ((((u32)*(volatile u8*)node >> 7) & 1) != 0) {
                            if (table_index == count) {
                                found = node;
                                break;
                            }
                            count++;
                        }
                        if ((((u32)*(volatile u8*)node >> 6) & 1) == 0) {
                            node = fn_8005D934(*(s16*)(node + 0x18));
                        } else {
                            found = 0;
                            break;
                        }
                    }
                    table_offset = 0;
                    for (table_index = 0; (u32)table_index < *(u32*)lbl_80478DD8; table_index++) {
                        if (found == fn_8005D934(*(u32*)(lbl_80478DDC + table_offset + 8))) {
                            goto LAB_8002488C;
                        }
                        table_offset += 0x10;
                    }
                    table_index = 0;
                LAB_8002488C:;
                    alpha = (u32)(s32)(lbl_8047B8DC * ((lbl_8047B8D8 - lbl_80478898) / lbl_8047B8D8));
                }
                break;
            default:
                table_index = lbl_8047A368;
                node = fn_8005DA18(*(u32*)(arg0 + 4));
                node = fn_8005D934(*(s16*)(node + 4));
                count = 0;
                while (1) {
                    if ((((u32)*(volatile u8*)node >> 7) & 1) != 0) {
                        if (table_index == count) {
                            found = node;
                            break;
                        }
                        count++;
                    }
                    if ((((u32)*(volatile u8*)node >> 6) & 1) == 0) {
                        node = fn_8005D934(*(s16*)(node + 0x18));
                    } else {
                        found = 0;
                        break;
                    }
                }
                table_offset = 0;
                for (table_index = 0; (u32)table_index < *(u32*)lbl_80478DD8; table_index++) {
                    if (found == fn_8005D934(*(u32*)(lbl_80478DDC + table_offset + 8))) {
                        goto LAB_80024934;
                    }
                    table_offset += 0x10;
                }
                table_index = 0;
            LAB_80024934:;
                alpha = 0xFF;
                break;
            }

            arg1[0x67] = (u8)alpha;
            if ((u32)table_index < *(u32*)lbl_80478DD8) {
                if ((u32)table_index > 9) {
                    table_index = 0;
                }
                fn_80132A38(
                    0x37,
                    fn_800FA280(
                        *(u32*)(lbl_80478DE4 + slot_offset + (((u32)lbl_802E4F58[table_index]) << 2) + 4)
                    )
                );
                return;
            }
        }
        slot_offset += 0x28;
        slot++;
    }

    fn_80132A38(0x37, fn_800FA280(1));
}
#pragma pop
#endif

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
 * Status: 90.7% matched. Remaining diffs: register allocation swap
 * (lbl_8047A368 -> r28 vs target r30) + bne+b vs beq pattern.
 */
extern u32 lbl_8047A368;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern f32 lbl_8047B8A8;
extern f32 lbl_8047B8DC;
#if 0
asm void fn_80024A2C(void) {
#include "src/game/gs_title_fn_80024A2C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
#pragma fp_contract on
void fn_80024A2C(u8* arg0, u8* arg1) {
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    u8* node;
    u8* found;
    u8* ptr;
    s32 index;
    s32 offset;

    index = lbl_8047A368;
    node = fn_8005DA18(*(u32*)(arg0 + 4));
    node = fn_8005D934(*(s16*)(node + 4));
    offset = 0;
    while (1) {
        if ((((u32)*(volatile u8*)node >> 7) & 1) != 0) {
            if (index == offset) {
                found = node;
                break;
            }
            offset++;
        }
        if ((((u32)*(volatile u8*)node >> 6) & 1) == 0) {
            node = fn_8005D934(*(s16*)(node + 0x18));
        } else {
            found = 0;
            break;
        }
    }

    offset = 0;
    for (index = 0; (u32)index < *(u32*)lbl_80478DD8; index++) {
        node = fn_8005D934(*(u32*)(lbl_80478DDC + offset + 8));
        if (found == node) {
            goto LAB_80024aec;
        }
        offset += 0x10;
    }
    index = 0;
LAB_80024aec:
    switch ((s32)lbl_8047A370) {
    case 1:
        arg1[0x67] = lbl_8047B8DC * (lbl_80478898 / lbl_8047B8A8);
        break;
    default:
        arg1[0x67] = 0xFF;
        break;
    }

    if ((u32)index < *(u32*)lbl_80478DD8) {
        u32 val;
        ptr = (u8*)lbl_80478DDC + (index << 4);
        if (*(u32*)(ptr + 4) == 0x66 && (u8)fn_801902E0((void*)0x45D) != 0) {
            val = 0x0C5F1200;
        } else {
            val = *(u32*)(ptr + 0xC);
        }
        *(u32*)(arg1 + 0x58) = val;
    }
}
#pragma pop
#endif

/* 0x80024BA4 | 0x138
 *
 * fn_80024BA4 -- same linked-list walk as fn_80024A2C, but without the alpha
 * computation. Just finds the menu item matching lbl_8047A36C and writes the
 * entry payload to arg1[0x58] (with the 0x66 + fn_801902E0(0x45D) override).
 *
 * Likely corresponds to a menu cursor hover-highlight update:
 *   arg0 = menu context, arg1 = cursor/sprite target receiving the entry
 *
 * Status: 85.8% matched. Remaining diffs: pbVar3 saved to r29 non-volatile
 * (target keeps it in r3 volatile via call-return chaining - CW-scheduler
 * quirk we cannot force from source).
 */
extern u32 lbl_8047A36C;
extern u32 lbl_80478DDC;
extern u32 lbl_80478DD8;
#if 0
asm void fn_80024BA4(void) {
#include "src/game/gs_title_fn_80024BA4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
void fn_80024BA4(s32 arg0, u8* arg1) {
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A36C;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern u32 fn_801902E0(s32);
    /* Declaration order: iVar6->r30, iVar29->r29, uVar7->r28 (shared with iVar6 dead).
     * iVar2->r28 (overlaps uVar7 but not iVar6). pbVar3 saved for loop-1. */
    s32 iVar6;
    s32 iVar29;
    u32 uVar7;
    s32 iVar2;
    u8* pbVar3;
    u8* pbVar4;

    iVar6 = (s32)lbl_8047A36C;
    pbVar3 = fn_8005D934(*(s16*)((u8*)fn_8005DA18(*(u32*)(arg0 + 4)) + 4));
    iVar29 = 0;
    while (1) {
        if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
            if (iVar6 != iVar29) {
                iVar29 = iVar29 + 1;
            } else {
                goto LAB_80024c20;
            }
        }
        if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
        pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));
    }
    iVar29 = 0;
LAB_80024c20:
    uVar7 = 0;
    iVar2 = 0;
    for (; uVar7 < *(u32*)lbl_80478DD8; uVar7++) {
        pbVar4 = fn_8005D934(*(u32*)(lbl_80478DDC + iVar2 + 8));
        if ((u8*)iVar29 == pbVar4) goto LAB_80024c64;
        iVar2 = iVar2 + 0x10;
    }
    uVar7 = 0;
LAB_80024c64:
    if (uVar7 < *(u32*)lbl_80478DD8) {
        u8 *entry = (u8*)lbl_80478DDC + (uVar7 << 4);
        u32 val;
        if (*(u32*)(entry + 4) == 0x66 && (u8)fn_801902E0(0x45D) != 0) {
            val = 0x0C5F1200;
        } else {
            val = *(u32*)(entry + 0xC);
        }
        *(u32*)(arg1 + 0x58) = val;
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
 * Status: 99.3% matched. Remaining diffs: 8 register-allocation choices
 * (target reuses r31 across bVar1 to iVar4 lifetime, we use r0 for iVar4)
 * plus 2 anonymous @NNN@sda21 constants (target binds to named lbl_8047B8D0
 * / lbl_8047B8B8). Both are deep CW internal heuristics; source-level
 * tricks cannot force them.
 */
extern u32 fn_800D3088(void);
extern u32 lbl_8047A390;
extern f64 lbl_8047B8D0;
extern f64 lbl_8047B8B8;
extern f32 lbl_8047A37C;
#if 0
asm void fn_80024CDC(void) {
#include "src/game/gs_title_fn_80024CDC.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma fp_contract on
void fn_80024CDC(s32 arg0, u8* arg1) {
    u8 bVar1;
    f32 fVar2;
    s32 iVar4;
    u32 uVar3;

    if (lbl_8047A390 != 0) {
        *(s16*)(arg1 + 0x50) = *(s16*)((u8*)lbl_8047A390 + 2) + 8;
        *(s16*)(arg1 + 0x52) = *(s16*)((u8*)lbl_8047A390 + 4) + 8;
    }

    bVar1 = *(u8*)(arg1 + 0x67);          /* current alpha (0..255)    */
    uVar3 = fn_800D3088();                /* u32 tick counter          */
    fVar2 = lbl_8047A37C;                 /* f32 pulse speed           */
    /* (f32)(u32)uVar3 -> unsigned int-to-float magic (no xor).  */
    /* (f32)(s32)bVar1 -> signed int-to-float (xoris + stw).     */
    iVar4 = (s32)(fVar2 * (f32)(u32)uVar3 + (f32)(s32)bVar1);

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
 *   - case 1 (lbl_8047A370 == 1): call fn_800E0CA0(fade_amt) + fn_800E090C
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
extern void fn_800E090C(void*, void*, void*, f32);
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u8 lbl_803A2058[];
extern u8 lbl_803A204C[];
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f64 lbl_8047B8B8;
extern f32 lbl_8047A374;
extern f32 lbl_8047B8E0;
#if 0
asm void fn_80024DBC(void) {
#include "src/game/gs_title_fn_80024DBC.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
void fn_80024DBC(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern f64 lbl_8047B8B8;
    extern f32 lbl_8047B8E0;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar3 = fn_8005D934(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) goto LAB_80024E94;
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));
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
    fB8E0 = lbl_8047B8E0;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 + lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 + lbl_8047A374 + fB8E0);
}
#endif

/* 0x80024F2C | 0x170
 *
 * fn_80024F2C -- twin of fn_80024DBC, using +offset on x and -offset on y.
 * Same structure: case-1 calls fn_800E090C to interpolate, default walks list.
 * See fn_80024DBC doc for full semantics.
 *
 * Status: 94.9% matched. Same SDA2 anonymous-constant blocker.
 */
extern u32 lbl_8047A370;
extern f32 lbl_80478898;
extern u32 lbl_80478DD8;
extern u32 lbl_8047A368;
extern f64 lbl_8047B8B8;
extern f32 lbl_8047A374;
extern f32 lbl_8047B8E0;
#if 0
asm void fn_80024F2C(void) {
#include "src/game/gs_title_fn_80024F2C.inc"
}
#else
void fn_80024F2C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern f64 lbl_8047B8B8;
    extern f32 lbl_8047B8E0;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar3 = fn_8005D934(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) goto LAB_80025004;
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));
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
    fB8E0 = lbl_8047B8E0;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 + lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 - lbl_8047A374 + fB8E0);
}
#endif

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
extern f64 lbl_8047B8B8;
extern f32 lbl_8047A374;
extern f32 lbl_8047B8E0;
#if 0
asm void fn_8002509C(void) {
#include "src/game/gs_title_fn_8002509C.inc"
}
#else
void fn_8002509C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern f64 lbl_8047B8B8;
    extern f32 lbl_8047B8E0;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar3 = fn_8005D934(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) goto LAB_80025174;
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));
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
    fB8E0 = lbl_8047B8E0;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 - lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 + lbl_8047A374 + fB8E0);
}
#endif

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
extern f64 lbl_8047B8B8;
extern f32 lbl_8047A374;
extern f32 lbl_8047B8E0;
#if 0
asm void fn_8002520C(void) {
#include "src/game/gs_title_fn_8002520C.inc"
}
#else
void fn_8002520C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern f32 lbl_8047A374;
    extern f64 lbl_8047B8B8;
    extern f32 lbl_8047B8E0;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_48, lbl_803A2058, lbl_803A204C);
        unaff_r28 = (s32)local_48[0];
        unaff_r31 = (s32)local_48[1];
        break;
    case 0:
    default:
        uVar4 = (s32)lbl_8047A368;
        if ((u32)uVar4 < *(u32*)lbl_80478DD8) {
            iVar2 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar3 = fn_8005D934(*(s16*)(iVar2 + 4));
            uVar5 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar3 >> 7) & 1) {
                    if (uVar4 == uVar5) goto LAB_800252E4;
                    uVar5 = uVar5 + 1;
                }
                if (((u32)*(volatile u8*)pbVar3 >> 6) & 1) break;
                pbVar3 = fn_8005D934(*(s16*)(pbVar3 + 0x18));
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
    fB8E0 = lbl_8047B8E0;
    *(s16*)(arg1 + 0x50) = (s16)((f32)(s32)(s16)unaff_r28 - lbl_8047A374 + fB8E0);
    *(s16*)(arg1 + 0x52) = (s16)((f32)(s32)(s16)unaff_r31 - lbl_8047A374 + fB8E0);
}
#endif

/* 0x8002537C | 0x114
 *
 * fn_8002537C -- single-axis position writer. Writes (s16)(x + 0xf) to
 * arg1[0x50] and 0 to arg1[0x52]. Only returns an X coord; Y is always 0,
 * meaning this is likely for horizontal-slider cursors.
 *
 * case 1: fn_800E090C returns a pair, use local_28[0] (x)
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
#if 0
asm void fn_8002537C(void) {
#include "src/game/gs_title_fn_8002537C.inc"
}
#else
void fn_8002537C(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_28, lbl_803A2058, lbl_803A204C);
        unaff_r29 = (s32)local_28[0];
        break;
    case 0:
    default:
        uVar3 = (s32)lbl_8047A368;
        if ((u32)uVar3 < *(u32*)lbl_80478DD8) {
            iVar1 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar2 = fn_8005D934(*(s16*)(iVar1 + 4));
            uVar4 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar2 >> 7) & 1) {
                    if (uVar3 == uVar4) goto LAB_80025450;
                    uVar4 = uVar4 + 1;
                }
                if (((u32)*(volatile u8*)pbVar2 >> 6) & 1) break;
                pbVar2 = fn_8005D934(*(s16*)(pbVar2 + 0x18));
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
#endif

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
#if 0
asm void fn_80025490(void) {
#include "src/game/gs_title_fn_80025490.inc"
}
#else
void fn_80025490(s32 arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
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
        fn_800E090C(local_24, lbl_803A2058, lbl_803A204C);
        unaff_r29 = (s32)local_24[1];
        break;
    case 0:
    default:
        uVar3 = (s32)lbl_8047A368;
        if ((u32)uVar3 < *(u32*)lbl_80478DD8) {
            iVar1 = (s32)fn_8005DA18(*(u32*)(arg0 + 4));
            pbVar2 = fn_8005D934(*(s16*)(iVar1 + 4));
            uVar4 = 0;
            while (1) {
                if (((u32)*(volatile u8*)pbVar2 >> 7) & 1) {
                    if (uVar3 == uVar4) goto LAB_80025564;
                    uVar4 = uVar4 + 1;
                }
                if (((u32)*(volatile u8*)pbVar2 >> 6) & 1) break;
                pbVar2 = fn_8005D934(*(s16*)(pbVar2 + 0x18));
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
#endif

/* 0x800255A4 | 0x18C
 *
 * fn_800255A4 -- title-screen audio/timing update driven by lbl_8047A384.
 *   IF title active (lbl_8047A384 != 0):
 *     fn_801C41C8(lbl_8047B8E4, 3)     -- set BGM volume fade target
 *     soundStop(0x449, 0)            -- play SE cue
 *     while (accumulator < limit) {    -- time-based integration loop
 *       _threadSwitch();                 -- frame advance
 *       accumulator += (f32)fn_800D3088() / (f32)(s32)fn_800D37CC();
 *     }
 *     fn_801653C4(); fn_801656F8(0x7d0, 0)
 *   ELSE:
 *     fn_801C41C8(lbl_8047B8E8, 3)     -- different fade curve
 *     fn_801653C4(); fn_801656F8(0x1f4, 0)
 *   Check sound channels 0xbd and 0xc3; stop them if still active.
 *   If lbl_8047A388/38C particle handles set, free them via fn_800EF5A4.
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
extern void fn_801C41C8(f32, s32);
extern void soundStop(void);
extern void fn_800D37CC(void);
extern void fn_801653C4(void);
extern void fn_801656F8(void);
extern void fn_801C40F0(s32);
extern void fn_80102620(void);
extern void fn_80102510(void);
extern void fn_800EF5A4(void);
extern u32 lbl_8047A384;
extern f32 lbl_8047B8E4;
extern f32 lbl_8047B8AC;
extern f64 lbl_8047B8B8;
extern f64 lbl_8047B8D0;
extern f32 lbl_8047B8B0;
extern f32 lbl_8047B8E8;
extern u32 lbl_8047A388;
extern u32 lbl_8047A38C;
#if 0
asm void fn_800255A4(void) {
#include "src/game/gs_title_fn_800255A4.inc"
}
#else
void fn_800255A4(void) {
    extern u32 lbl_8047A384;
    extern u32 lbl_8047A388;
    extern u32 lbl_8047A38C;
    extern f32 lbl_8047B8AC;
    extern f32 lbl_8047B8B0;
    extern f32 lbl_8047B8E4;
    extern f32 lbl_8047B8E8;
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    extern void fn_800EF5A4(u32);
    extern void _threadSwitch(void);
    extern u8 fn_80102620(s32);
    extern void fn_80102510(s32);
    extern u32 fn_801653C4(void);
    extern void fn_801656F8(u32, s32, s32);
    extern void soundStop(s32, s32);
    extern void fn_801C40F0(s32);
    extern void fn_801C41C8(f32, s32);
    f32 limit;
    f32 accum;

    if (lbl_8047A384 != 0) {
        fn_801C41C8(lbl_8047B8E4, 3);
        soundStop(0x449, 0);
        accum = lbl_8047B8AC;
        limit = lbl_8047B8B0;
        while (accum < limit) {
            _threadSwitch();
            accum = accum + (f32)fn_800D3088() / (f32)fn_800D37CC();
        }
        fn_801656F8(fn_801653C4(), 0x7d0, 0);
    } else {
        fn_801C41C8(lbl_8047B8E8, 3);
        fn_801656F8(fn_801653C4(), 0x1f4, 0);
    }
    fn_801C40F0(1);
    if (fn_80102620(0xbd) == 1) fn_80102510(0xbd);
    if (fn_80102620(0xc3) == 1) fn_80102510(0xc3);
    if (lbl_8047A384 != 0) {
        if (lbl_8047A388 != 0) fn_800EF5A4(lbl_8047A388);
        if (lbl_8047A38C != 0) fn_800EF5A4(lbl_8047A38C);
    }
}
#endif

/* 0x80025730 | 0x280 */
extern void fn_8005D8F8(void);
extern void fn_801046B8(void);
extern void fn_801026A4(void);
extern void fn_8011394C(void);
extern void fn_800D3074(void);
extern void GStextureCreate(void);
extern void fn_8010264C(void);
extern u32 lbl_80478DDC;
extern u32 lbl_8047A368;
extern u32 lbl_8047A390;
extern u32 lbl_8047A3AC;
extern u32 lbl_80478DD8;
extern u8 lbl_8047A3A8;
extern u32 lbl_8047A388;
extern void fn_80025F84(void);
#if 0
asm void fn_80025730(void) {
#include "src/game/gs_title_fn_80025730.inc"
}
#else
void fn_80025730(void) {
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A388;
    extern u32 lbl_8047A390;
    extern u8 lbl_8047A3A8;
    extern u32 lbl_8047A3AC;
    extern void fn_80025F84();
    extern void fn_8005D8F8();
    extern void fn_8005D934();
    extern void fn_800D3074();
    extern void GSgfxBeginBackFBCapture();
    extern void GStextureCreate();
    extern void fn_80102510();
    extern void fn_8010264C();
    extern void fn_801026A4();
    extern void fn_801046B8();
    extern void fn_8011394C();
    extern void fn_801902E0();
    extern void fn_80025F74();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = 0x0;
    r3 = lbl_80478DDC;
    *(u32*)(sp + 0x8) = tmp;
    lbl_8047A368 = tmp;
    r3 = *(u32*)((u8*)r3 + 0x8);
    fn_8005D934();
    r28 = 0x0;
    lbl_8047A390 = r3;
    r31 = r28 << 4;
    r29 = 0x0;
    while (1) {
        r3 = lbl_80478DD8;
        tmp = *(u32*)((u8*)r3 + 0x0);
        if (r29 >= tmp) break;
        tmp = lbl_80478DDC;
        r3 = lbl_8047A3AC;
        r4 = tmp + r31;
        tmp = *(u32*)((u8*)r4 + 0x4);
        if (r3 == tmp) {
            lbl_8047A368 = r28;
            r3 = *(u32*)((u8*)r4 + 0x8);
            fn_8005D934();
            lbl_8047A390 = r3;
        }
        r3 = lbl_80478DDC;
        r30 = 0x1;
        r3 = *(u32*)(r3 + r31);
        if (r3 != 0) {
            fn_801902E0();
            tmp = r3 & 0xFF;
            if (tmp == 0) {
                r30 = 0x0;
        }
        }
        r3 = lbl_80478DDC;
        tmp = r31 + 0x8;
        r4 = r30;
        r3 = *(u32*)(r3 + tmp);
        fn_8005D8F8();
        tmp = r30 & 0xFF;
        if (tmp != 0) {
            r28 = r28 + 0x1;
        }
        r31 = r31 + 0x10;
        r29 = r29 + 0x1;

    }
    fn_801046B8();
    r4 = r3;
    r5 = (u32)sp + 0x8;
    r3 = 0xbd;
    r6 = 0x0;
    r7 = 0x1;
    r8 = 0x0;
    fn_801026A4();
    if ((s32)r3 < 0) {
        fn_8011394C();
        r30 = r3;

    } else {
        r4 = lbl_80478DD8;
        r7 = 0x0;
        r6 = lbl_80478DDC;
        r5 = *(u32*)((u8*)r4 + 0x0);
        r4 = r6;
        ctr_fn = (void(*)(void))r5;
        if (r5 > 0) {
            do {
                tmp = *(u32*)((u8*)r4 + 0x8);
                if (r3 == tmp) break;
                r4 = r4 + 0x10;
                r7 = r7 + 0x1;
            } while (--ctr != 0);
        }
        if (r7 >= r5) {
            r7 = 0x0;
        }
        r31 = r7 << 4;
        r3 = r6 + r31;
        tmp = *(u32*)((u8*)r3 + 0x4);
        do {
        if (tmp != 0x66) break;
            r3 = 0x45d;
            fn_801902E0();
            tmp = r3 & 0xFF;
            if (tmp == 0) break;
            r30 = 0x7b;
            break;
        } while (0);

        tmp = lbl_80478DDC;
        r3 = tmp + r31;
        r30 = *(u32*)((u8*)r3 + 0x4);
    }
    fn_8011394C();
    if (r30 != r3) {
        do {
            if (r30 != 0x7a) break;
            fn_8011394C();
            if (r3 != 0xf) break;
            tmp = 0x0;
            goto L_80025910;
        } while (0);

        do {
            if (r30 != 0xf) break;
            fn_8011394C();
            if (r3 != 0x7a) break;
            tmp = 0x0;
            break;
        } while (0);

        tmp = 0x1;
        goto L_80025910;
    }
    tmp = 0x0;
L_80025910:
    tmp = tmp & 0xFF;
    if (tmp == 1) {
        r3 = 0x2;
        fn_800D3074();
        tmp = 0x0;
        r3 = 0x0;
        lbl_8047A3A8 = tmp;
        r4 = 0x0;
        r5 = 0x44;
        r6 = 0x0;
        r7 = 0x0;
        GStextureCreate();
        r4 = (u32)fn_80025F74;
        lbl_8047A388 = r3;
        r4 = (u32)fn_80025F74;
        r5 = 0x0;
        GSgfxBeginBackFBCapture();
        while (1) {
            tmp = lbl_8047A3A8;
            if (tmp != 0) break;
            ((void(*)(void))_threadSwitch)();

        }
        r3 = 0xc3;
        r4 = 0x0;
        fn_8010264C();
        r3 = 0xbd;
        fn_80102510();
        fn_80025F84();
    }
    r3 = r30;
    r4 = 0x0;
    ((void(*)(void))fn_80113828)();
    return;
}
#endif

/* 0x800259B0 | 0xCC */
extern void fn_801CB954(void);
extern f32 lbl_8047B8A8;
extern f32 lbl_8047B8AC;
extern f32 lbl_8047B8E8;
extern f32 lbl_8047B8EC;
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
#if 0
asm void fn_800259B0(void) {
#include "src/game/gs_title_fn_800259B0.inc"
}
#else
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
void fn_800259B0(void) {
    extern u8 lbl_803A2058[];
    extern u8 lbl_803A204C[];
    extern void fn_801CB954(s32, s32);
    extern u32 fn_8011394C(void);
    f32* vec1 = (f32*)lbl_803A2058;
    f32* vec2 = (f32*)lbl_803A204C;
    u32 result;

    lbl_8047A368 = 0;
    lbl_8047A36C = 0;
    lbl_8047A370 = 0;
    lbl_80478898 = lbl_8047B8A8;
    vec1[0] = lbl_8047B8AC;
    vec1[1] = lbl_8047B8AC;
    vec1[2] = lbl_8047B8AC;
    vec2[0] = lbl_8047B8AC;
    vec2[1] = lbl_8047B8AC;
    vec2[2] = lbl_8047B8AC;
    lbl_8047A374 = lbl_8047B8AC;
    lbl_8047A378 = lbl_8047B8E8;
    lbl_8047A37C = lbl_8047B8EC;
    lbl_8047A380 = 0;
    lbl_8047A384 = 0;
    lbl_8047A388 = 0;
    lbl_8047A390 = 0;
    fn_801CB954(0x0C6A1000, 0);
    fn_801CB954(0x0C6A1001, 0);
    fn_801CB954(0x0C6A1002, 0);
    result = fn_8011394C();
    lbl_8047A3AC = result;
    if (result == 0x7b) {
        lbl_8047A3AC = 0x66;
    }
}
#endif

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
    extern f32 lbl_8047B8A8;
    extern f32 lbl_8047B8AC;
    extern f32 lbl_8047B8E8;
    extern f32 lbl_8047B8EC;
    extern void fn_8011394C();
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

    f3 = lbl_8047B8A8;
    r3 = (u32)lbl_803A2058;
    tmp = 0x0;
    f2 = lbl_8047B8AC;
    r6 = (u32)lbl_803A2058;
    f1 = lbl_8047B8E8;
    f0 = lbl_8047B8EC;
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
    fn_8011394C();
    lbl_8047A3AC = r3;
    if (r3 == 0x7b) {
        tmp = 0x66;
        lbl_8047A3AC = tmp;
    }
    return;
}
#endif

/* 0x80025A7C | 0x4 -- nop */
#if 0
asm void fn_80025A7C(void) {
#include "src/game/gs_title_fn_80025A7C.inc"
}
#else
#pragma optimization_level 4
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
 * Status: 90.6% matched. Remaining diffs: FP register swaps in the
 * call-arg plumbing between fn_80025C1C calls.
 */
extern void fn_800D9B58(void);
extern void fn_800DA4C4(void);
extern void fn_800DA2BC(void);
extern void fn_800DA1E8(void);
extern void fn_800DA100(void);
extern void fn_800DA028(void);
extern void fn_800D9ED8(void);
extern f32 lbl_8047B8AC;
extern f32 lbl_8047B8F8;
extern f32 lbl_8047B8FC;
extern f32 lbl_8047B8F0;
extern f32 lbl_8047B8F4;
extern f32 lbl_8047A3A0;
extern f32 lbl_8047B8B0;
extern f32 lbl_8047B904;
extern f32 lbl_8047B908;
extern f32 lbl_8047B8E0;
extern u32 lbl_8047A388;
extern f32 lbl_8047A3A4;
extern f32 lbl_8047B8E4;
extern f32 lbl_8047B8C8;
extern void fn_80025C1C(f32, f32, f32, s32, s32, s32, f32*);
#if 0
asm void fn_80025A80(void) {
#include "src/game/gs_title_fn_80025A80.inc"
}
#else
s32 fn_80025A80(s32 param_1) {
    extern u8 lbl_803A204C[];
    extern u32 lbl_8047A388;
    extern f32 lbl_8047A3A0;
    extern f32 lbl_8047A3A4;
    extern f32 lbl_8047B8AC;
    extern f32 lbl_8047B8B0;
    extern f32 lbl_8047B8C8;
    extern f32 lbl_8047B8E0;
    extern f32 lbl_8047B8E4;
    extern f32 lbl_8047B8F0;
    extern f32 lbl_8047B8F4;
    extern f32 lbl_8047B8F8;
    extern f32 lbl_8047B8FC;
    extern f32 lbl_8047B904;
    extern f32 lbl_8047B908;
    extern void fn_80025C1C(f32, f32, f32, s32, s32, s32, f32*);
    extern void fn_800D9B58(f32, f32, f32, f32);
    extern void fn_800D9ED8(s32);
    extern void fn_800DA028(s32);
    extern void fn_800DA100(s32, s32, s32, s32, s32, s32);
    extern void fn_800DA1E8(s32, s32, s32);
    extern void fn_800DA2BC(s32, s32, s32);
    extern void fn_800DA4C4(s32, s32, s32);
    f32 local_18[3];

    fn_800D9B58(lbl_8047B8AC, lbl_8047B8AC, lbl_8047B8F8, lbl_8047B8FC);
    fn_800DA4C4(1, 6, 7);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    fn_800DA028(0);
    fn_800D9ED8(1);

    local_18[0] = lbl_8047B8F0;
    local_18[1] = lbl_8047B8F4;
    local_18[2] = lbl_8047B8AC;
    fn_80025C1C(lbl_8047A3A0, lbl_8047B8B0, lbl_8047B904, param_1, 1, 0, local_18);

    lbl_8047A3A0 = lbl_8047A3A0 - lbl_8047B908;
    if (lbl_8047A3A0 < lbl_8047B8AC) lbl_8047A3A0 = lbl_8047B8AC;

    local_18[0] = lbl_8047B8F8 - (lbl_8047B8E0 + ((f32*)lbl_803A204C)[0]);
    local_18[1] = lbl_8047B8FC - (lbl_8047B8E0 + ((f32*)lbl_803A204C)[1]);
    local_18[2] = *(volatile f32*)&lbl_8047B8AC;
    fn_80025C1C(lbl_8047A3A4, lbl_8047B8B0, lbl_8047B8E4, (s32)lbl_8047A388, 1, 1, local_18);

    lbl_8047A3A4 = lbl_8047A3A4 - lbl_8047B8C8;
    if (lbl_8047A3A4 < lbl_8047B8AC) lbl_8047A3A4 = lbl_8047B8AC;

    if (lbl_8047A3A4 <= *(volatile f32*)&lbl_8047B8AC && lbl_8047A3A0 <= lbl_8047B8AC) {
        return 0;
    }
    return 1;
}
#endif

/* 0x80025C1C | 0x358
 *
 * fn_80025C1C(f32 rot, f32 size, f32 uv_ext, s32 tex, s32 flag_a,
 *             s32 flag_b, f32 *pos)  -- GX quad render primitive.
 *
 *   1. Build two vec3s (vec_a, vec_b) based on flag_a (1 = symmetric scale,
 *      else diagonal). Call fn_800E0CA0(rot) + fn_800E090C to combine them
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
 *        fn_800DFF98(vert_out, result_2, uv_corner)  -- compute position
 *        fn_800D6680(vert.x, vert.y, vert.z)         -- write pos
 *        fn_800D5CB8(0, 0xff, 0xff, 0xff, scaled)    -- RGBA color
 *        fn_800D59B8(0, u, v)                        -- UV
 *      4 UV corners: (0,0) (1,0) (0,1) (1,1) via lbl_8047B8AC/B8B0.
 *   7. fn_800D6728() to flush the primitive.
 *
 * Status: 94.8% matched. Remaining diffs are FP register allocation inside
 * the vertex emission loop (4 similar call chains get slightly different
 * register assignments per iteration).
 */
extern void fn_800E042C(void);
extern void fn_800E03B4(void);
extern void fn_800D88DC(void);
extern void fn_800D888C(void);
extern void fn_800D6A00(void);
extern void fn_800D7820(void);
extern void fn_800D85D4(void);
extern void fn_800D67BC(void);
extern void fn_800DFF98(void);
extern void fn_800D6680(void);
extern void fn_800D5CB8(void);
extern void fn_800D59B8(void);
extern void fn_800D6728(void);
extern f32 lbl_8047B8AC;
extern f32 lbl_8047B8DC;
extern f32 lbl_8047B8B0;
extern f32 lbl_8047B8F0;
extern f32 lbl_8047B8F4;
extern u8 lbl_80314AE8[];
extern f32 lbl_8047B90C;
extern f32 lbl_8047B910;
#if 0
asm void fn_80025C1C(void) {
#include "src/game/gs_title_fn_80025C1C.inc"
}
#else
void fn_80025C1C(f32 arg_f1, f32 arg_f2, f32 arg_f3,
                 s32 arg_r3, s32 arg_r4, s32 arg_r5, f32 *arg_r6) {
    extern u8 lbl_80314AE8[];
    extern f32 lbl_8047B8AC;
    extern f32 lbl_8047B8B0;
    extern f32 lbl_8047B8DC;
    extern f32 lbl_8047B8F0;
    extern f32 lbl_8047B8F4;
    extern f32 lbl_8047B90C;
    extern f32 lbl_8047B910;
    extern void fn_800D59B8(s32, f32, f32);
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D6680(f32, f32, f32);
    extern void fn_800D6728(void);
    extern void fn_800D67BC(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(u8*);
    extern void fn_800D85D4(s32, s32);
    extern void fn_800D888C(s32);
    extern void fn_800D88DC(s32);
    extern void fn_800DFF98(f32*, f32*, f32*);
    extern void fn_800E03B4(f32*, f32*);
    extern void fn_800E042C(f32*, f32*);
    extern void fn_800E090C(f32*, f32*, f32*);
    extern void fn_800E0CA0(f32);
    f32 vec_a[3];
    f32 vec_b[3];
    f32 result_1[3];
    f32 result_2[3];
    f32 vert_out[3];
    s32 scaled;

    if ((u8)arg_r4 == 1) {
        vec_a[0] = lbl_8047B8AC; vec_a[1] = lbl_8047B8AC; vec_a[2] = lbl_8047B8AC;
        vec_b[0] = arg_f2;       vec_b[1] = arg_f2;       vec_b[2] = arg_f2;
    } else {
        vec_a[0] = arg_f2;       vec_a[1] = arg_f2;       vec_a[2] = arg_f2;
        vec_b[0] = lbl_8047B8AC; vec_b[1] = lbl_8047B8AC; vec_b[2] = lbl_8047B8AC;
    }
    fn_800E0CA0(arg_f1);
    fn_800E090C(result_1, vec_b, vec_a);
    scaled = (s32)(lbl_8047B8DC * result_1[0]);

    if ((u8)arg_r5 == 1) {
        vec_a[0] = arg_f3;       vec_a[1] = arg_f3;       vec_a[2] = lbl_8047B8B0;
        vec_b[0] = lbl_8047B8B0; vec_b[1] = lbl_8047B8B0; vec_b[2] = lbl_8047B8B0;
    } else {
        vec_a[0] = lbl_8047B8B0; vec_a[1] = lbl_8047B8B0; vec_a[2] = lbl_8047B8B0;
        vec_b[0] = arg_f3;       vec_b[1] = arg_f3;       vec_b[2] = lbl_8047B8B0;
    }
    fn_800E0CA0(arg_f1);
    fn_800E090C(result_1, vec_b, vec_a);
    fn_800E042C(result_2, result_1);

    {
        f32 bf0 = lbl_8047B8F0, bf4 = lbl_8047B8F4, bac = lbl_8047B8AC;
        vec_b[0] = bf0; vec_b[1] = bf4; vec_b[2] = bac;
        vec_a[0] = arg_r6[0]; vec_a[1] = arg_r6[1]; vec_a[2] = bac;
    }
    fn_800E0CA0(arg_f1);
    fn_800E090C(result_1, vec_b, vec_a);
    fn_800E03B4(result_2, result_1);

    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D6A00(4);
    fn_800D7820(lbl_80314AE8);
    fn_800D85D4(0, arg_r3);
    fn_800D67BC(4);

    vec_b[0] = lbl_8047B90C; vec_b[1] = lbl_8047B910; vec_b[2] = lbl_8047B8AC;
    fn_800DFF98(vert_out, result_2, vec_b);
    fn_800D6680(vert_out[0], vert_out[1], vert_out[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, (s32)(u8)scaled);
    fn_800D59B8(0, lbl_8047B8AC, lbl_8047B8AC);

    vec_b[0] = lbl_8047B8F0; vec_b[1] = lbl_8047B910; vec_b[2] = lbl_8047B8AC;
    fn_800DFF98(vert_out, result_2, vec_b);
    fn_800D6680(vert_out[0], vert_out[1], vert_out[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, (s32)(u8)scaled);
    fn_800D59B8(0, lbl_8047B8B0, lbl_8047B8AC);

    vec_b[0] = lbl_8047B90C; vec_b[1] = lbl_8047B8F4; vec_b[2] = lbl_8047B8AC;
    fn_800DFF98(vert_out, result_2, vec_b);
    fn_800D6680(vert_out[0], vert_out[1], vert_out[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, (s32)(u8)scaled);
    fn_800D59B8(0, lbl_8047B8AC, lbl_8047B8B0);

    vec_b[0] = lbl_8047B8F0; vec_b[1] = lbl_8047B8F4; vec_b[2] = lbl_8047B8AC;
    fn_800DFF98(vert_out, result_2, vec_b);
    fn_800D6680(vert_out[0], vert_out[1], vert_out[2]);
    fn_800D5CB8(0, 0xff, 0xff, 0xff, (s32)(u8)scaled);
    fn_800D59B8(0, lbl_8047B8B0, lbl_8047B8B0);

    fn_800D6728();
}
#endif

/* 0x80025F74 | 0x10 */
extern u8 lbl_8047A3A8;
#if 0
asm void fn_80025F74(void) {
#include "src/game/gs_title_fn_80025F74.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80025F74(void) {
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
 *   3. Set up TEV/sprite chain via fn_80113F48 + fn_800F9318 composition:
 *        fn_800E3C94, fn_800E9108(2), fn_800E8FE8, fn_800E900C(1, &obj_c),
 *        fn_800E8FA0(0x280, 0x1E0)  -- viewport 640x480
 *        fn_800E3C08, fn_800E3C00(4)
 *      Activates tex handle, binds 0xC6A1000 slot 0, pushes animation.
 *      Stores handle in lbl_8047A384 (title active flag).
 *   4. Per lbl_8047A380 (debug flag?), pick one of two frame-sequence tables
 *      (lbl_80478DEC + idx*0x10), read 2 u32s, advance index (wrap > 9).
 *   5. Reset cursor scales: lbl_8047A3A4 = lbl_8047A3A0 = lbl_8047B8A8 (0).
 *   6. Register fn_80025A80 as a particle callback (GSgfxBeginBackFBCapture).
 *   7. Start BGM via fn_80176E0C, then run two timing delay loops
 *      (_threadSwitch + fn_800D3088 accumulator vs 1 or 0xAE target).
 *   8. Store title origin coords to lbl_803A2040[0..2] and start the final
 *      fade-in loop using fn_800C46B0 for FP-to-int conversion.
 *
 * Status: 99.1% matched (#pragma optimization_level 2). Remaining diffs:
 * register allocation (frame_a/b in table section, first timing loop delay),
 * and anonymous f64 bias constants (lbl_8047B8B8/B8D0, unfixable per CW heuristic).
 */
extern void fn_801CBA0C(void);
extern void fn_80113F48(void);
extern void fn_800F9318(void);
extern void fn_800E3C94(void);
extern void fn_800E9108(void);
extern void fn_800E8FE8(void);
extern void fn_800E900C(void);
extern void fn_800E8FA0(void);
extern void fn_800E3C08(void);
extern void fn_800E3C00(void);
extern void fn_801CB61C(void);
extern void fn_801CB834(void);
extern void fn_80165A20(void);
extern void fn_80176E0C(void);
extern void fn_800C46B0(void);
extern u8 lbl_8047A380;
extern u32 lbl_8047A384;
extern u32 lbl_8047A394;
extern u32 lbl_80478DEC;
extern u32 lbl_8047A398;
extern f32 lbl_8047B8A8;
extern f32 lbl_8047A3A4;
extern f32 lbl_8047A3A0;
extern u32 lbl_8047A38C;
extern f32 lbl_8047B914;
extern f32 lbl_8047B918;
extern u8 lbl_803A2040[];
extern f32 lbl_8047B8F0;
extern f32 lbl_8047B8F4;
extern f32 lbl_8047B8AC;
extern u32 lbl_8047A39C;
extern f64 lbl_8047B8B8;
extern f32 lbl_8047B8E4;
extern f64 lbl_8047B8D0;
extern f32 lbl_8047B900;
#if 0
asm void fn_80025F84(void) {
#include "src/game/gs_title_fn_80025F84.inc"
}
#else
#pragma optimization_level 2
void fn_80025F84(void) {
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
    extern f32 lbl_8047B8A8;
    extern f32 lbl_8047B8E4;
    extern f32 lbl_8047B8F0;
    extern f32 lbl_8047B8F4;
    extern f32 lbl_8047B900;
    extern f32 lbl_8047B914;
    extern f32 lbl_8047B918;
    extern u32 fn_800C46B0(f64);
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    extern void GSgfxBeginBackFBCapture(u32, void*, s32);
    extern void fn_800E3C00(u32, s32);
    extern void fn_800E3C08(u32, u32);
    extern void fn_800E3C94(u32, s32);
    extern void fn_800E8FA0(s32, s32);
    extern void fn_800E8FE8(u32, u32);
    extern void fn_800E900C(u32, s32, void*);
    extern void fn_800E9108(u32, s32);
    extern u32 GStextureCreate(s32, s32, s32, s32, s32);
    extern void _threadSwitch(void);
    extern u32 fn_800F9318(u32, u32);
    extern u32 fn_80113F48(void);
    extern void fn_80165A20(s32, s32, s32);
    extern void fn_80176E0C(u32, u32, s32, s32);
    extern u8 fn_801902E0(s32);
    extern void fn_801CB61C(u32, u32, s32);
    extern void fn_801CB834(u32, u32, s32, s32);
    extern void fn_801CB954(u32, s32);
    extern u32 fn_801CBA0C(u32);
    extern s32 fn_80025A80(s32);
    u32 obj_a, obj_b, obj_c, obj_d, obj_e;
    u32 frame_a, frame_b;
    u32 tex;
    u32 elapsed;
    u32 delay;
    s32 tick;

    fn_801CB954(0xC6A1000, 1);
    fn_801CB954(0xC6A1001, 1);
    fn_801CB954(0xC6A1002, 1);
    if (fn_801902E0(0x3e5) == 1) {
        tex = fn_801CBA0C(0xC6B1000);
        fn_801CB954(tex, 0);
        frame_a = 0;
    } else {
        tex = fn_801CBA0C(0xC6C1000);
        fn_801CB954(tex, 0);
        frame_a = 0;
    }

    obj_a = fn_800F9318(fn_80113F48(), tex);
    fn_800E3C94(obj_a, 0);
    obj_b = fn_800F9318(fn_80113F48(), tex);
    obj_c = fn_800F9318(fn_80113F48(), 0xC6A1002);
    obj_d = fn_800F9318(fn_80113F48(), 0xC6A1603);
    {
        u32 obj_c_local = obj_c;
        fn_800E9108(obj_b, 2);
        fn_800E8FE8(obj_b, obj_d);
        fn_800E900C(obj_b, 1, &obj_c_local);
    }
    fn_800E8FA0(0x280, 0x1e0);
    obj_e = fn_800F9318(fn_80113F48(), 0xC831400);
    fn_800E3C08(fn_800F9318(fn_80113F48(), tex), obj_e);
    fn_800E3C00(fn_800F9318(fn_80113F48(), tex), 4);
    fn_801CB954(tex, 1);
    fn_801CB61C(tex, 0xC6A1000, 0);
    fn_801CB834(tex, frame_a, 0, 1);
    fn_80165A20(0x449, 0, 0xff);
    lbl_8047A384 = tex;

    {
        s32 idx, frame_offset;
        if (lbl_8047A380 == 0) {
            idx = (s32)lbl_8047A394 + 1;
            frame_offset = (s32)lbl_80478DEC + (s32)lbl_8047A394 * 0x10;
            frame_a = *(u32*)(frame_offset + 0xA4);
            frame_b = *(u32*)(frame_offset + 0xA8);
            lbl_8047A394 = idx;
            if (idx > 9) lbl_8047A394 = 0;
        } else {
            idx = (s32)lbl_8047A398 + 1;
            frame_offset = (s32)lbl_80478DEC + (s32)lbl_8047A398 * 0x10;
            frame_a = *(u32*)(frame_offset + 4);
            frame_b = *(u32*)(frame_offset + 8);
            lbl_8047A398 = idx;
            if (idx > 9) lbl_8047A398 = 0;
        }
        lbl_8047A3A4 = lbl_8047B8A8;
        lbl_8047A3A0 = lbl_8047B8A8;
        lbl_8047A38C = GStextureCreate(0, 0, 0x44, 0, 0);
        GSgfxBeginBackFBCapture(lbl_8047A38C, (void*)fn_80025A80, 0);
        fn_80176E0C(fn_80113F48(), frame_b, 0, 0);

        delay = 1;
        tick = fn_800D37CC();
        if (tick == 0x32) {
            delay = fn_800C46B0((f64)lbl_8047B914);
            if (delay < 1) delay = 1;
        }
        for (elapsed = 0; elapsed < delay; elapsed = elapsed + tick) {
            _threadSwitch();
            tick = fn_800D3088();
        }
        fn_801CB834(0xC6A1000, frame_a, 0, 0);
        delay = 0xae;
        tick = fn_800D37CC();
        if (tick == 0x32) {
            delay = fn_800C46B0((f64)lbl_8047B918);
            if (delay < 1) delay = 1;
        }
        for (elapsed = 0; elapsed < delay; elapsed = elapsed + tick) {
            _threadSwitch();
            tick = fn_800D3088();
        }
    }

    *(f32*)lbl_803A2040 = lbl_8047B8F0;
    *(f32*)(lbl_803A2040 + 4) = lbl_8047B8F4;
    *(f32*)(lbl_803A2040 + 8) = lbl_8047B8AC;
    lbl_8047A39C = fn_800F9318(fn_80113F48(), lbl_8047A384);
    lbl_8047A3A4 = lbl_8047B8A8;

    delay = 0x78 - fn_800C46B0((f64)((f32)fn_800D37CC() * lbl_8047B8E4));
    if (delay != 0) {
        tick = fn_800D37CC();
        if (tick == 0x32) {
            delay = fn_800C46B0((f64)((f32)delay / lbl_8047B900));
            if (delay < 1) delay = 1;
        }
    }
    for (elapsed = 0; elapsed < delay; elapsed = elapsed + tick) {
        _threadSwitch();
        tick = fn_800D3088();
    }
}
#endif

/* ===== Phase 2 recovery stubs ===== */

/* fn_8002058C - 0x8002058C | size: 0x2c */
extern u8 lbl_8047A32C;
#if 0
asm void fn_8002058C(void) {
#include "src/game/gs_title_fn_8002058C.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
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
#if 0
asm void fn_800205C8(void) {
#include "src/game/gs_title_fn_800205C8.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_800205C8(u8* obj) {
    s32 mask = (s32)-0x100;
    u32 b = obj[0x8b];
    fn_800FB680(0, 0, b | mask, (&lbl_80478880)[lbl_8047A350]);
}
#pragma pop
#endif

/* fn_8002060C - 0x8002060C | size: 0xc */
extern u32 lbl_8047A350;
#if 0
asm void fn_8002060C(void) {
#include "src/game/gs_title_fn_8002060C.inc"
}
#else
#pragma optimization_level 4
void fn_8002060C(void) {
    lbl_8047A350 = 0;
}
#endif

/* fn_80020618 - 0x80020618 | size: 0x304 */
extern void fn_80105624(void);
extern s32 fn_80135168(s32, s32);
extern void fn_80166CC0(void);
extern void fn_800F78A4(s32, s32, s32, s32, s32);
extern void fn_80135030(void);
extern u32 lbl_8047B878;
extern u8 lbl_803A1FC8[];
extern u32 lbl_8047B880;
extern u32 lbl_8047B868;
extern u32 lbl_8047B86C;
extern u32 lbl_8047B870;
extern f32 lbl_8047B874;
#if 0
asm void fn_80020618(void) {
#include "src/game/gs_title_fn_80020618.inc"
}
#else
void fn_80020618(void) { /* TODO */ }
#endif

/* fn_8002091C - 0x8002091C | size: 0x10 */
#if 0
asm void fn_8002091C(void) {
#include "src/game/gs_title_fn_8002091C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002091C(void) {
    return *(s32*)lbl_803A1FC8;
}
#endif

/* fn_8002092C - 0x8002092C | size: 0x90 */
extern f32 lbl_8047B874;
extern f32 lbl_8047B88C;
extern f32 lbl_8047B888;
#if 0
asm void fn_8002092C(void) {
#include "src/game/gs_title_fn_8002092C.inc"
}
#else
#pragma optimization_level 4
#pragma fp_contract on
void fn_8002092C(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(lbl_8047B874 * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = lbl_8047B88C * f2 + lbl_8047B888;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 2) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}
#endif

/* fn_800209BC - 0x800209BC | size: 0x90 */
extern f32 lbl_8047B874;
extern f32 lbl_8047B88C;
extern f32 lbl_8047B888;
#if 0
asm void fn_800209BC(void) {
#include "src/game/gs_title_fn_800209BC.inc"
}
#else
#pragma optimization_level 4
void fn_800209BC(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(lbl_8047B874 * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = lbl_8047B88C * f2 + lbl_8047B888;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}
#endif

/* fn_80020A4C - 0x80020A4C | size: 0x90 */
extern f32 lbl_8047B874;
extern f32 lbl_8047B88C;
extern f32 lbl_8047B888;
#if 0
asm void fn_80020A4C(void) {
#include "src/game/gs_title_fn_80020A4C.inc"
}
#else
#pragma optimization_level 4
void fn_80020A4C(void* r3, u8* r4) {
    f32 f2;
    f32 f0;
    f2 = (f32)sin(lbl_8047B874 * *(f32*)(lbl_803A1FC8 + 0x24));
    f0 = lbl_8047B88C * f2 + lbl_8047B888;
    *(f32*)(r4 + 0x6c) = f0;
    *(f32*)(r4 + 0x68) = f0;
    if (*(s32*)lbl_803A1FC8 == 0) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}
#endif

/* fn_80020ADC - 0x80020ADC | size: 0x58 */
extern u32 fn_80166C74(void);
#if 0
asm void fn_80020ADC(void) {
#include "src/game/gs_title_fn_80020ADC.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
void fn_80020ADC(void* r3, u8* r4) {
    if (fn_80166C74() != 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}
#endif

/* fn_80020B34 - 0x80020B34 | size: 0x58 */
#if 0
asm void fn_80020B34(void) {
#include "src/game/gs_title_fn_80020B34.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
void fn_80020B34(void* r3, u8* r4) {
    if (fn_80166C74() == 1) {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) | 2;
    } else {
        *(s8*)(r4 + 4) = *(s8*)(r4 + 4) & ~2;
    }
}
#endif

/* fn_80020B8C - 0x80020B8C | size: 0x14 */
#if 0
asm void fn_80020B8C(void) {
#include "src/game/gs_title_fn_80020B8C.inc"
}
#else
#pragma optimization_level 4
void fn_80020B8C(void* r3, u8* r4) {
    *(f32*)(r4 + 0x70) = *(f32*)(lbl_803A1FC8 + 0x10);
}
#endif

/* fn_80020BA0 - 0x80020BA0 | size: 0xfc */
extern void fn_801040F0(s32, s32, void*, s32, s32);
extern s32 fn_80135168(s32, s32);
extern u8 lbl_802EF0A8[];
#if 0
asm void fn_80020BA0(void) {
#include "src/game/gs_title_fn_80020BA0.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
void fn_80020BA0(void* arg0, u8* arg1) {
    s16* ptr;

    if (fn_80166C74() == 0) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FF0);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x900C);
    }
    fn_801040F0((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);

    if (fn_80135168(0, 9) == 1) {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FD4);
    } else {
        ptr = (s16*)(lbl_802EF0A8 + 0x8FB8);
    }
    fn_801040F0((s16)(ptr[1] - *(s16*)(arg1 + 0x50)), (s16)(ptr[2] - *(s16*)(arg1 + 0x52)), arg0, 0x192, 0);
}
#endif

/* fn_80020C9C - 0x80020C9C | size: 0x200 */
extern void fn_801070F4(void);
extern void fn_801D04E8(void);
extern void fn_80106D3C(s32, s32, s32, s32);
extern void fn_8001E074(void);
extern void fn_801D0748(void);
extern void fn_801069FC(s32);
extern void fn_80102568(s32, s32, s32);
extern u32 lbl_8047B86C;
extern u32 lbl_8047B890;
extern u32 lbl_8047B870;
#if 0
asm void fn_80020C9C(void) {
#include "src/game/gs_title_fn_80020C9C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
void fn_80020C9C(void) {
    extern void _threadSwitch(void);
    extern u8 fn_801070F4(s32);
    extern void fn_800205C0(s32);
    extern u32 fn_80166C74(void);
    extern s32 fn_80135168(s32, s32);
    extern s32 fn_8010264C(s32, s32);
    extern void fn_80166CC0(u32);
    extern void fn_80135030(s32, s32, s32);
    extern u8 fn_801D04E8(void);
    extern void fn_80106D3C(s32, s32, s32, s32);
    extern u8 fn_8001E074(s32, s32, s32, s32);
    extern void fn_801D0748(s32, s32, s32);
    extern void fn_801069FC(s32);
    extern void fn_80102568(s32, s32, s32);

    s32 state;
    s32 active;
    u32 region;
    s32 fmt;
    s32* sp;

    state = 0;
    active = 1;
    while (fn_801070F4(0xAA) != 0) {
        _threadSwitch();
    }
    fn_800205C0(0);

    *(s32*)((u8*)&lbl_803A1FC8 + 0x0) = 0;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x14) = *(f32*)&lbl_8047B86C;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x10) = *(f32*)&lbl_8047B86C;
    *(s32*)((u8*)&lbl_803A1FC8 + 0x4) = 0;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x20) = *(f32*)&lbl_8047B86C;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x28) = *(f32*)&lbl_8047B890;
    *(f32*)((u8*)&lbl_803A1FC8 + 0x18) = *(f32*)&lbl_8047B870;
    *(s32*)((u8*)&lbl_803A1FC8 + 0x8) = 0;
    *(s32*)((u8*)&lbl_803A1FC8 + 0xc) = 0;

    do {
        switch (state) {
        case 0:
            region = fn_80166C74();
            fmt = fn_80135168(0, 9);
            fn_8010264C(0x7F, 0);
            fn_8010264C(0x80, 0);
            fn_8010264C(0x7C, 0);
            fn_8010264C(0x7D, 0);
            fn_8010264C(0x7E, 0);
            sp = (s32*)&lbl_803A1FC8;
            do {
                if (fn_8010264C(0x7B, 1) < 0) {
                    fn_80166CC0(region);
                    fn_80135030(0, 9, fmt);
                    break;
                }
            } while (sp[0] != 2);
            state = 0x64;
            break;
        case 0x64:
            *(s32*)&lbl_803A1FC8 = -1;
            if (fn_80135168(0, 9) != fmt) {
                if (fn_801D04E8() != 0) {
                    fn_80106D3C(1, 0x3D82, 1, 0);
                    if ((s8)menuSubOpenYesNo(0, 0x3C, 0xAA, 1) == 0) {
                        fn_801D0748(7, 2, 0);
                    }
                    fn_801069FC(1);
                }
            }
            fn_80102568(0x80, 0, 1);
            active = 0;
            break;
        }
    } while (active != 0);
}
#pragma pop
#endif

/* fn_80020EA4 - 0x80020EA4 | size: 0xb0 */
extern u32 lbl_8047A360;
extern u32 lbl_802E4EF0[];
extern u32 lbl_802E4ED8[];
#if 0
asm void fn_80020EA4(void) {
#include "src/game/gs_title_fn_80020EA4.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_80020EA4(u8* r3, u8* r4) {
    u8* r31;
    u8 byte;
    r31 = r3;
    if ((s32)(&lbl_8047A360)[1] == 4) {
        if (fn_8002091C() >= 0) {
            byte = r31[0x8b];
            fn_800FB680(0, -8, byte | (s32)-0x100, lbl_802E4EF0[fn_8002091C()]);
        }
    } else if ((s32)(&lbl_8047A360)[1] != 5) {
        if ((s32)lbl_8047A360 >= 0) {
            byte = r31[0x8b];
            fn_800FB680(0, -8, byte | (s32)-0x100, lbl_802E4ED8[(s32)lbl_8047A360]);
        }
    }
}
#pragma pop
#endif

/* fn_80020F54 - 0x80020F54 | size: 0x19c */
extern void* fn_80109934(void*);
extern void fn_800D61E4(void);
extern s32 fn_801EF214(void);
extern void fn_801021F8(u32, u32);
extern u8 lbl_803A1FF8[];
extern u8 lbl_80314F98[];
extern u32 lbl_8047B898;
extern u32 lbl_8047B89C;
#if 0
asm void fn_80020F54(void) {
#include "src/game/gs_title_fn_80020F54.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
void fn_80020F54(u8* arg0, u8* arg1) {
    extern void fn_800D88DC(s32);
    extern void fn_800D888C(s32);
    extern void fn_800D6A00(s32);
    extern void fn_800D7820(u8*);
    extern void fn_800D85D4(s32, void*);
    extern void fn_800D67BC(s32);
    extern void fn_800D61E4(s32, s32);
    extern void fn_800D5CB8(s32, s32, s32, s32, s32);
    extern void fn_800D59B8(s32, f32, f32);
    extern void fn_800D6728(void);

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
        model = fn_80109934(lbl_803A1FF8);
        if (model != 0) {
            fn_800D88DC(3);
            fn_800D888C(4);
            fn_800D6A00(7);
            fn_800D7820(lbl_80314F98);
            fn_800D85D4(0, model);
            fn_800D67BC(2);
            fn_800D61E4(0, 0);
            fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_800D59B8(0, *(f32*)&lbl_8047B898, *(f32*)&lbl_8047B898);
            fn_800D61E4(*(s16*)(arg1 + 0x54), *(s16*)(arg1 + 0x56));
            fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
            fn_800D59B8(0, *(f32*)&lbl_8047B89C, *(f32*)&lbl_8047B89C);
            fn_800D6728();
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
#pragma pop
#endif

/* fn_800210F0 - 0x800210F0 | size: 0x4d4 */
extern void fn_8005CEE8(void);
extern void fn_800FF58C(void);
extern void fn_8006A718(void);
extern void fn_801CB9D8(u32);
extern void fn_8010A420(void*);
extern void fn_80029760(void);
extern void fn_80029638(void);
extern void fn_80128E38(void);
extern void fn_800056E4(void);
extern void fn_800056EC(void);
extern void fn_80130054(void);
extern void fn_8010A5BC(void);
extern void fn_8010A010(void);
extern void fn_8018F6F4(void);
extern void fn_8018F4C8(void);
extern void menuModelSetMotion(void);
extern void fn_80005748(void);
extern void fn_801EF274(void);
extern void fn_80113FE8(void);
extern u32 lbl_8047A35C;
extern u32 lbl_8047B898;
extern u32 lbl_8047A358;
extern void menuModelSetMotion(void);
extern u8 menuSubOpenYesNo(s32, s32, s32, s32);
#if 0
asm void fn_800210F0(void) {
#include "src/game/gs_title_fn_800210F0.inc"
}
#else
void fn_800210F0(void) { /* TODO */ }
#endif

/* fn_800215C4 - 0x800215C4 | size: 0x60 */
extern u32 lbl_8047A35C;
#if 0
asm void fn_800215C4(void) {
#include "src/game/gs_title_fn_800215C4.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
void fn_800215C4(void) {
    fn_80102568(0xaa, 0, 1);
    fn_80102568(0x7a, 0, 1);
    fn_80102568(0x7f, 0, 1);
    fn_801CB9D8(lbl_8047A35C);
    fn_8010A420(lbl_803A1FF8);
}
#pragma peephole on
#endif

/* fn_80021624 - 0x80021624 | size: 0x20 */
#if 0
asm void fn_80021624(void) {
#include "src/game/gs_title_fn_80021624.inc"
}
#else
#pragma optimization_level 4
void fn_80021624(void) {
    fn_800210F0();
}
#endif

/* fn_80021644 - 0x80021644 | size: 0x9c */
extern void GSscene_SetMode(void);
extern u32 lbl_8047A35C;
extern u32 lbl_8047A358;
#if 0
asm void fn_80021644(void) {
#include "src/game/gs_title_fn_80021644.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
void fn_80021644(void) {
    extern void fn_80165A20(s32, s32, s32);
    extern void* fn_80113F48(void);
    extern u32 fn_801CBA0C(s32);
    extern void fn_800F9318(void*, u32);
    extern void fn_80176E0C(s32, s32, s32, s32);
    extern void GSscene_SetMode(s32);
    void* handle;

    if (fn_800FF548() == 0) {
        fn_80165A20(0x446, 0, 0xff);
        handle = fn_80113F48();
        lbl_8047A35C = fn_801CBA0C(0x0FFE1000);
        fn_800F9318(handle, lbl_8047A35C);
        fn_80176E0C(0x632, 0x0FFF1800, 0, 1);
        GSscene_SetMode(4);
        lbl_8047A360 = lbl_8047A358;
        (&lbl_8047A360)[1] = 0;
    }
}
#pragma peephole on
#endif

/* fn_800216E8 - 0x800216E8 | size: 0x1d4 */
extern void fn_800F96E4(void*, s32, void*);
extern void* fn_8011F4F0(s32);
extern u8 lbl_80266C7C[];
#if 0
asm void fn_800216E8(void) {
#include "src/game/gs_title_fn_800216E8.inc"
}
#else
#pragma optimization_level 4
void fn_800216E8(void* arg0, s32 arg1, u8* arg2, s16 arg3, s32 arg4) {
    struct TitleNameCand {
        s32 code;
        s32 msg;
        s32 link;
    } table[21];
    s32 i;
    s32 j;
    s32 msg;
    s32 link;
    s16 x;
    s16 y;

    for (i = 0; i < 21; i++) {
        table[i] = ((struct TitleNameCand*)lbl_80266C7C)[i];
    }

    if (arg3 <= 0) {
        fn_800F96E4(arg0, arg1 + 1, (void*)0x4261);
        return;
    }

    for (i = 0; i < 21; i++) {
        for (j = 0; j < arg3; j++) {
            if (table[i].code == *(s32*)(arg2 + j * 8)) {
                break;
            }
        }
        if (j >= arg3) {
            break;
        }
    }

    if (i >= 21) {
        fn_800F96E4(arg0, arg1 + 1, (void*)0x4261);
        return;
    }

    link = table[i].link;
    msg = table[i].msg;
    x = *(s16*)(arg2 + j * 8 + 4);
    y = *(s16*)(arg2 + j * 8 + 6);

    if (link >= 0) {
        for (i = i + 1; i < 21; i++) {
            if (table[i].link >= 0 && table[i].link != link) {
                for (j = 0; j < arg3; j++) {
                    if (table[i].code == *(s32*)(arg2 + j * 8)) {
                        break;
                    }
                }
                if (j >= arg3) {
                    msg = 0x4201;
                    break;
                }
            }
        }
    }

    fn_80132A38(0x32, (void*)fn_8011F4F0(arg4));
    fn_80132A38(0x2F, (void*)(s32)x);
    fn_80132A38(0x30, (void*)(s32)y);
    fn_800F96E4(arg0, arg1 + 1, (void*)msg);
}
#endif

/* fn_800218BC - 0x800218BC | size: 0x1e0 */
extern void fn_80014118(s32, void*, void*);
extern s32 fn_80123FBC(s32);
extern u8 fn_8011FC74(s32);
extern f32 fn_8011FC14(void*);
extern s32 fn_800141BC(void*, s32);
extern void fn_80014198(s32);
extern u8 lbl_803A1B90[];
extern u32 lbl_8047B8A0;
#if 0
asm void fn_800218BC(void) {
#include "src/game/gs_title_fn_800218BC.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_800218BC(u32 arg0, u32* arg1) {
    extern f32 fn_8011FC14(void*);
    extern void fn_80106D3C(s32, s32, s32, s32);
    extern void fn_801069FC(s32);
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
    for (count = *(s32*)(p + 0x40), ctr = count, i = 0; ctr > 0; ctr--) {
        if (arg0 == *(u16*)p) break;
        p += 8;
        i++;
    }
    if (i >= count) {
        fn_80106D3C(2, 0x426A, 1, 0);
        fn_801069FC(1);
        return 1;
    }

    thresh = *(f32*)&lbl_8047B8A0;
    for (i = 0; i < 6; i++) {
        fn_80014118(i, &sp_c, &sp_8);
        if ((u8)fn_80123FBC((s32)sp_c) != 0) {
            if (fn_8011FC74((s32)sp_c) != 0) {
                if (fn_8011FC14(sp_c) > thresh) break;
            }
        }
    }
    if (i >= 6) {
        fn_80106D3C(2, 0x4261, 1, 0);
        fn_801069FC(1);
        return 1;
    }

    idx = fn_800141BC((void*)arg0, 1);
    if (idx >= 0) {
        fn_80014118(idx, &sp_c, &sp_8);
        ok = 0;
        if (fn_8011FC74((s32)sp_c) != 0) {
            if (fn_8011FC14(sp_c) > *(f32*)&lbl_8047B8A0) ok = 1;
        }
        if (ok == 0) {
            fn_80106D3C(2, 0x4261, 1, 0);
            fn_801069FC(1);
        }
    }
    fn_80014198(idx);
    if (idx >= 0 && ok != 0) {
        *arg1 = 1;
        return 2;
    }
    return 1;
}
#pragma pop
#endif

/* fn_80021A9C - 0x80021A9C | size: 0x78 */
#if 0
asm void fn_80021A9C(void) {
#include "src/game/gs_title_fn_80021A9C.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
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
    fn_80106D3C(2, 0x426a, 1, 0);
    fn_801069FC(1);
    return 1;
}
#endif

/* fn_80021B14 - 0x80021B14 | size: 0x53c */
extern void fn_80142EF8(void);
extern void fn_801431AC(void);
extern void fn_80014110(void);
extern s32 fn_80121ADC(s32, s32);
extern s16 fn_80144574(void*, s32, s32, u16, s32);
extern void fn_80166A50(s32, s32, s32, s32);
extern void fn_8001D378(void);
extern u8 lbl_80478890[8];
extern u8 lbl_80266DB0[];
extern u8 lbl_80266D78[];
extern void fn_80023968(void);
#if 0
asm void fn_80021B14(void) {
#include "src/game/gs_title_fn_80021B14.inc"
}
#else
void fn_80021B14(void) { /* TODO */ }
#endif

/* fn_80022050 - 0x80022050 | size: 0x12c */
extern s32 fn_801347E0(void);
extern s32 fn_801347E8(s32, s8);
extern void fn_800140FC(s32*, s32*);
extern s32 fn_801F7EF0(s32);
extern void fn_80023968(void);
#if 0
asm void fn_80022050(void) {
#include "src/game/gs_title_fn_80022050.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
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
        if ((u8)fn_80123FBC(spC) != 0) {
            iVar3 = iVar3 + 1;
        }
        iVar4 = iVar4 + 1;
    } while (iVar4 < 6);
    if (iVar3 >= 6) {
        cVar1 = fn_801347E0();
        iVar3 = 0;
        while (iVar3 < cVar1) {
            cVar2 = pcboxGetPokemonBoxNbEmptySlot(0, (s8)iVar3);
            if (cVar2 > 0) {
                break;
            }
            iVar3 = iVar3 + 1;
        }
        if (iVar3 >= cVar1) {
            fn_80106D3C(2, 0x4416, 1, 0);
            fn_801069FC(1);
            return 1;
        }
    }
    fn_800140FC(&sp14, &sp10);
    if ((u8)fn_801F7EF0(sp10) != 0) {
        fn_80106D3C(2, 0x426d, 1, 0);
        fn_801069FC(1);
        return 1;
    }
    *arg1 = 0;
    return 0;
}
#endif

/* fn_8002217C - 0x8002217C | size: 0x2fc */
#if 0
asm void fn_8002217C(void) {
#include "src/game/gs_title_fn_8002217C.inc"
}
#else
void fn_8002217C(void) { /* TODO */ }
#endif

/* fn_80022478 - 0x80022478 | size: 0x2a8 */
extern void jumptable_802E4F00();
extern u8 lbl_80478888[8];
extern s32 fn_80023068(u32, u32*);
extern void fn_800232F0(void);
extern s32 fn_80023760(u32, u32*);
#if 0
asm void fn_80022478(void) {
#include "src/game/gs_title_fn_80022478.inc"
}
#else
#pragma push
#pragma peephole off
s32 fn_80022478(u32 arg0, u32* arg1) {
    u8 state_buf[0x110];
    u8 text_buf[0x100];
    u8 name_buf[0x40];
    s32 sel;
    s32 slot;
    s32 effect;
    s32 sc;
    s32 sd;
    s32 sound_id;

    ((void(*)(u8*, u8*))fn_80142EF8)(state_buf, lbl_80478888);
    sel = ((s32(*)(u8*))fn_801431AC)(state_buf);
    if ((u32)sel > 0x15) {
        fn_80106D3C(2, 0x426A, 1, 0);
        fn_801069FC(1);
        return 1;
    }

    switch (sel) {
    case 1:
        return ((s32 (*)(u32, u32*))fn_80023760)(arg0, arg1);
    case 2:
        return ((s32 (*)(u32, u32*))fn_800232F0)(arg0, arg1);
    case 19:
    case 20:
        return ((s32 (*)(u32, u32*))fn_80023068)(arg0, arg1);
    case 21:
        return ((s32 (*)(u32, u32*))fn_80023968)(arg0, arg1);
    case 0:
    case 8:
    case 9:
    case 18:
        fn_80106D3C(2, 0x426A, 1, 0);
        fn_801069FC(1);
        return 1;
    default:
        slot = fn_800141BC((void*)arg0, 1);
        if (slot >= 0) {
            fn_80014118(slot, &sc, &sd);
            if (((u8)fn_80121ADC(sc, 0x3E) & 0xFF) == 0) {
                effect = fn_80144574(text_buf, sc, sd, (u16)arg0, 0);
                if ((s16)effect > 0) {
                    {
                        u16* bgm_table = (u16*)lbl_80266DB0;
                        if (arg0 == bgm_table[0] ||
                            arg0 == bgm_table[1] ||
                            arg0 == bgm_table[2] ||
                            arg0 == bgm_table[3] ||
                            arg0 == bgm_table[4]) {
                            sound_id = 0x466;
                        } else {
                            sound_id = 0x465;
                        }
                    }
                    fn_80166A50(sound_id, 0, 0xFF, 0);
                    fn_8001D378();
                }
                ((void(*)(void*, s32, u8*, s16, s32))fn_800216E8)(name_buf, 0x40, (u8*)sd, effect, sc);
                fn_80132A38(0x4D, name_buf);
                fn_80106D3C(2, 0xE0, 1, 0);
                fn_801069FC(1);
            } else {
                fn_80132A38(0x32, (void*)fn_8011F4F0(sc));
                fn_80106D3C(2, 0x424D, 1, 0);
                fn_801069FC(1);
                effect = 0;
            }
        } else {
            effect = 0;
        }

        fn_80014198(slot);
        if (slot >= 0 && (s16)effect > 0) {
            if (arg0 >= 0x27 && arg0 < 0x2C) {
                *arg1 = 0;
            } else {
                *arg1 = 1;
            }
            return 0;
        }
        return 1;
    }
}
#pragma pop
#endif

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
 * Calls fn_80106D3C(2, msg_from_row_iVar1, 1, 0) then fn_801069FC(1) to
 * print the error message, writes 0 to *arg1, returns 0.
 *
 * Status: 70.5% matched. Blocker: target asm batches ALL 10 lwz loads
 * first THEN ALL stw stores; our CW compile interleaves load-store-load-
 * store. This is CW 1.3 scheduler behavior we have not been able to
 * force from source (tried u32* src, temp-var pair hoist, array init
 * syntax, scheduling pragmas).
 */
extern u8 lbl_80266C54[];
extern s32 fn_80023068(u32, u32*);
extern void fn_800232F0(void);
#if 0
asm void fn_80022720(void) {
#include "src/game/gs_title_fn_80022720.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling off
s32 fn_80022720(u32 arg0, u32* arg1) {
    s32 iVar1;
    u32 *src;
    u32 t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
    u32 buf[10];

    src = (u32*)lbl_80266C54;
    iVar1 = 0;
    t0 = src[0]; t1 = src[1]; t2 = src[2]; t3 = src[3]; t4 = src[4];
    t5 = src[5]; t6 = src[6]; t7 = src[7]; t8 = src[8]; t9 = src[9];
    buf[0] = t0; buf[1] = t1; buf[2] = t2; buf[3] = t3; buf[4] = t4;
    buf[5] = t5; buf[6] = t6; buf[7] = t7; buf[8] = t8; buf[9] = t9;
    if ((((arg0 != *(u16*)((u8*)buf + 0x00)) && (iVar1 = 1, arg0 != *(u16*)((u8*)buf + 0x08))) &&
         (iVar1 = 2, arg0 != *(u16*)((u8*)buf + 0x10))) &&
        ((iVar1 = 3, arg0 != *(u16*)((u8*)buf + 0x18)) &&
         (iVar1 = 4, arg0 != *(u16*)((u8*)buf + 0x20)))) {
        iVar1 = 5;
    }
    fn_80106D3C(2, (s32)*(u32*)((u8*)buf + iVar1 * 8 + 4), 1, 0);
    fn_801069FC(1);
    *arg1 = 0;
    return 0;
}
#endif

/* fn_80022834 - 0x80022834 | size: 0x308 */
extern void fn_800232F0(void);
extern void fn_801440A0(void);
extern void fn_80143F24(void);
extern void fn_80143EF0(void);
extern void fn_80143E88(void);
extern void fn_8011F228(void);
extern void fn_8011F5C8(s32);
extern void fn_8011E778(void);
extern void fn_8011E2AC(void);
extern void fn_802600E4(void);
extern void fn_80123D58(void);
extern void fn_80123090(void);
extern void fn_80122370(void);
#if 0
asm void fn_80022834(void) {
#include "src/game/gs_title_fn_80022834.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80022834(u32 arg0, u32* arg1) {
    extern void* fn_801440A0(u32);
    extern u32 fn_80143F24(void*);
    extern u32 fn_80143EF0(u32);
    extern u32 fn_80143E88(void*);
    extern s32 menuSubOpenYesNo(s32, s32, s32, s32);
    extern u32 fn_8011F228(s32, u32);
    extern void fn_8011F5C8(s32);
    extern s32 fn_8011E778(void);
    extern u8 fn_8011E2AC(s32, s32);
    extern s32 fn_802600E4(s32, u32, void*, s32, void*, s32);
    extern void fn_80123D58(s32, u32, u32);
    extern u32 fn_80123090(s32);
    extern void fn_80122370(s32, u32, s32);
    extern void fn_80023274(void);

    s32 type_byte;
    s32 slot;
    s32 state;
    s32 effect;
    s32 msg;
    void* handle;
    u16 value16;
    u8 status;
    s32 c;
    s32 tmp;
    u32 v2;
    s32 i;
    s32 action;
    s32 sc;
    s32 sd;
    u8 buf;

    handle = fn_801440A0((u16)arg0);
    status = (u8)fn_80143F24(handle);
    value16 = fn_80143EF0(status);
    fn_80166A50(0x4CB, 0, 0xFF, 0);
    type_byte = fn_80143E88(handle);
    if ((u8)type_byte != 0xFF) {
        msg = 0x4260;
    } else {
        msg = 0x4265;
    }
    fn_80132A38(0x39, (void*)value16);
    fn_80106D3C(2, msg, 1, 0);
    fn_801069FC(1);
    fn_80106D3C(2, 0x426B, 1, 0);
    state = (s8)menuSubOpenYesNo(0, -1, -1, 0);
    fn_801069FC(1);
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
        if (fn_8011FC74(c) != 0) {
            fn_80106D3C(2, 0x424C, 1, 0);
            fn_801069FC(1);
            tmp = 0;
        } else {
            v2 = fn_80143EF0(status);
            for (i = 0; i < 4; i++) {
                if ((u16)v2 == (u16)fn_8011F228(c, (u16)i)) {
                    break;
                }
            }
            if (i < 4) {
                fn_80132A38(0x32, fn_8011F4F0(c));
                fn_80132A38(0x39, (void*)(u16)v2);
                fn_80106D3C(2, 0x4244, 1, 0);
                fn_801069FC(1);
                tmp = 0;
            } else {
                fn_8011F5C8(c);
                if (fn_8011E2AC(fn_8011E778(), status) == 0) {
                    fn_80132A38(0x32, fn_8011F4F0(c));
                    fn_80132A38(0x39, (void*)(u16)v2);
                    fn_80106D3C(2, 0x423F, 1, 0);
                    fn_801069FC(1);
                    tmp = 0;
                } else {
                    tmp = fn_802600E4(c, v2, &buf, 1, fn_80023274, 0);
                    if (tmp != 0) {
                        fn_80123D58(c, buf, (u16)v2);
                        fn_80122370(c, fn_80123090(c), 4);
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
#endif

/* fn_80022B3C - 0x80022B3C | size: 0x318 */
extern s32 fn_8011E15C(void);
extern void fn_80166A28(s32);
extern s32 fn_801666BC(s32);
extern void fn_80121B4C(s32, s32);
extern void fn_8011F910(s32, s32, s32);
extern u8 lbl_80266C30[];
extern u32 lbl_8047B8A0;
#if 0
asm void fn_80022B3C(void) {
#include "src/game/gs_title_fn_80022B3C.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling off
s32 fn_80022B3C(s32 arg0, s32 arg1) {
    s32 sp8;
    s32 spC;
    s32 sp10;
    s32 sp14;
    s32 sp18;
    s32 sp1C;
    s32 sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 temp_r28;
    s32 temp_r29;
    s32 temp_r30;
    s32 temp_r31;
    s32 result;
    s32 i;
    s32 count;
    f32 f0;
    void* data_ptr;
    f32 f1;

    data_ptr = &lbl_80266C30;
    temp_r30 = arg0;
    
    /* Copy data from structure */
    sp10 = *(s32*)data_ptr;
    sp14 = *(s32*)((u8*)data_ptr + 4);
    sp18 = *(s32*)((u8*)data_ptr + 8);
    sp1C = *(s32*)((u8*)data_ptr + 0xC);
    sp20 = *(s32*)((u8*)data_ptr + 0x10);
    sp24 = *(s32*)((u8*)data_ptr + 0x14);
    sp28 = *(s32*)((u8*)data_ptr + 0x18);
    sp2C = *(s32*)((u8*)data_ptr + 0x1C);
    
    fn_80014118(temp_r30, &spC, &sp8);
    result = fn_80123FBC(spC);
    
    if ((result & 0xFF000000) == 0) {
        return 1;
    }
    
    /* Determine particle type */
    temp_r29 = 0;
    result = temp_r30 & 0xFFFF;
    
    if (result == (u16)sp10) {
        temp_r29 = 0;
    } else if (result == (u16)sp1C) {
        temp_r29 = 1;
    } else if (result == (u16)sp28) {
        temp_r29 = 2;
    } else {
        temp_r29 = 3;
    }
    
    if (temp_r29 >= 3) {
        return 1;
    }
    
    /* Play particle sound effects */
    fn_8011F4F0(spC);
    fn_80132A38(0x32, (void*)result);
    fn_80132A38(0x2D, (void*)temp_r30);
    
    result = *(s32*)((u8*)(&sp10) + temp_r29 * 0xC);
    fn_80106D3C(2, result, 1, 0);
    fn_801069FC(1);
    
    /* Check if particle system is available */
    if (fn_80135168(0, 9) == 0) {
        /* Wait for particle system with timeout */
        for (i = 0; i < 2; i++) {
            fn_800F78A4(1, 0, 0xFF, 0x15, 0);
            count = 0;
            
            while (count < 0x3C) {
                _threadSwitch();
                count += fn_800D3088();
            }
        }
        
        /* Additional wait */
        fn_800F78A4(1, 0, 0xFF, 0x30, 0);
        count = 0;
        
        while (count < 0x60) {
            _threadSwitch();
            count += fn_800D3088();
        }
    }
    
    /* Validate and process particle */
    temp_r29 = spC;
    if (temp_r29 == 0) {
        return 0;
    }
    
    result = fn_80123FBC(temp_r29);
    if ((result & 0xFF000000) == 0) {
        return 0;
    }
    
    fn_8011F5C8(temp_r29);
    fn_8011E778();
    
    result = fn_8011E15C();
    if (result == 0) {
        temp_r29 = 0;
    } else {
        temp_r29 = result & 0xFFFF;
        fn_80166A28(temp_r29);
    }
    
    /* Wait for particle completion */
    while (fn_801666BC(temp_r29) == 2) {
        _threadSwitch();
    }
    
    /* Update particle state */
    fn_8011F4F0(spC);
    result = fn_80121ADC(spC, 0x3E);
    
    if ((result & 0xFF000000) != 0) {
        fn_80121B4C(spC, 0x3E);
        fn_80106D3C(2, 0x4277, 1, 0);
        fn_801069FC(1);
    }
    
    /* Final particle processing */
    f1 = fn_8011FC14((void*)spC);
    f0 = lbl_8047B8A0;
    
    if (f1 > f0) {
        fn_8011F910(spC, temp_r30 & 0xFFFF, 4);
        fn_8011F4F0(spC);
        result = fn_80121ADC(spC, 0x3E);
        
        if ((result & 0xFF000000) != 0) {
            result = *(s32*)((u8*)(&sp10) + temp_r29 * 0xC);
            fn_80106D3C(2, result, 1, 0);
            fn_801069FC(1);
        }
    }
    
    return 1;
}
#endif

/* fn_80022E54 - 0x80022E54 | size: 0x90 */
extern void fn_800190D8(void*, s32);
#if 0
asm void fn_80022E54(void) {
#include "src/game/gs_title_fn_80022E54.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
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
        fn_800190D8(r29, 0x5960010);
        *r30 = 1;
        return 2;
    }
    return 1;
}
#pragma pop
#endif

/* fn_80022EE4 - 0x80022EE4 | size: 0x184 */
extern s32 fn_80128A64(s32, s32, u16, void*, void*);
extern void fn_801096F8(s32);
extern void fn_8012805C(s32, s32, u16, void*, s32, s32, s32, s32);
extern f32 lbl_8047B8A4;
#if 0
asm void fn_80022EE4(void) {
#include "src/game/gs_title_fn_80022EE4.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
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
        if ((u8)fn_8011FC74(sc) == 0) {
            effect = fn_80144574(text_buf, sc, sd, (u16)arg0, 0);
            if ((s16)effect <= 0) {
                fn_80106D3C(2, 0x4261, 1, 0);
                fn_801069FC(1);
            }
        } else {
            fn_80106D3C(2, 0x424C, 1, 0);
            fn_801069FC(1);
        }
    }

    fn_80014198(slot);

    if (slot >= 0 && (s16)effect > 0) {
        effect = fn_80128A64(sc, 1, (u16)arg0, &sp8, &spC);
        fn_801C41C8(lbl_8047B8A4, 3);
        fn_801C40F0(1);
        fn_801096F8(0);
        fn_8012805C(sc, effect, sp8, &spC, 0, 1, 0, 0);
        fn_801096F8(1);
        fn_801C41C8(lbl_8047B8A4, 2);
        fn_801C40F0(1);
        *arg1 = 1;
        return 0;
    }

    return 1;
}
#endif

/* fn_80023068 - 0x80023068 | size: 0x20c */
extern s32 fn_80019B48(s32);
extern void fn_80019B1C(void);
#if 0
asm void fn_80023068(void) {
#include "src/game/gs_title_fn_80023068.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80023068(u32 arg0, u32* arg1) {
    extern s32 fn_80144574(void*, s32, s32, u16, s32);
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    u8 name_buf[0x84];
    u8 text_buf[0x108];
    s32 blocked;
    s32 mode;
    s32 slot;
    s32 sc;
    s32 sd;
    s32 effect;

    blocked = 0;
    for (;;) {
            { extern s32 fn_800141BC(void*, s32);
              slot = fn_800141BC((void*)arg0, 1);
            }
        if (slot < 0) break;
        
        fn_80014118(slot, &sc, &sd);
        
        { extern u8 fn_80121ADC(s32, s32);
          if (((u8)fn_80121ADC(sc, 0x3E) & 0xFF) == 0) {
              mode = fn_80019B48((s8)slot);
              fn_80019B1C();
              if (mode >= 0) break;
          } else {
              blocked = 1;
              break;
          }
        }
    }

    if (slot >= 0) {
        if (((s8)blocked & 0xFF) != 0) {
            fn_80132A38(0x32, (void*)fn_8011F4F0(sc));
            fn_80106D3C(2, 0x424D, 1, 0);
            fn_801069FC(1);
            effect = 0;
        } else {
            { extern s32 fn_80144574(void*, s32, s32, u16, s32);
              effect = fn_80144574(text_buf, sc, sd, (u16)arg0, (u8)mode);
            }
            
            if ((s16)effect > 0) {
                s32 sound_id;
                {
                    u16* bgm_table = (u16*)lbl_80266DB0;
                    if (arg0 == bgm_table[0] ||
                        arg0 == bgm_table[1] ||
                        arg0 == bgm_table[2] ||
                        arg0 == bgm_table[3] ||
                        arg0 == bgm_table[4]) {
                        sound_id = 0x466;
                    } else {
                        sound_id = 0x465;
                    }
                }
                fn_80166A50(sound_id, 0, 0xFF, 0);
                fn_8001D378();
            }
            
            { extern void fn_800216E8(void*, s32, void*, s32, s32);
              fn_800216E8(name_buf, 0x40, text_buf, effect, sc);
            }
            fn_80132A38(0x4D, name_buf);
            fn_80106D3C(2, 0xE0, 1, 0);
            fn_801069FC(1);
        }
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)effect > 0) {
        *arg1 = 1;
        return 0;
    }
    return 1;
}
#endif

/* fn_80023274 - 0x80023274 | size: 0x7c */
extern s32 fn_80097B04(s32, s32);
extern f32 lbl_8047B8A4;
#if 0
asm void fn_80023274(void) {
#include "src/game/gs_title_fn_80023274.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80023274(s32 r3, s32 r4) {
    s32 result;

    fn_801C41C8(lbl_8047B8A4, 3);
    fn_801C40F0(1);
    result = fn_80097B04(r3, r4);
    if (result >= 4) {
        result = -1;
    }
    fn_801C41C8(lbl_8047B8A4, 2);
    fn_801C40F0(1);
    return (s8)result;
}
#pragma peephole on
#endif

/* fn_800232F0 - 0x800232F0 | size: 0x470 */
extern s32 fn_8012640C(u32, s32, s32, s32);
extern void fn_80165668(void);
extern void fn_8011F4A8(void);
extern void fn_80105D48(void);
extern void fn_80105C68(void);
extern void fn_80123B5C(void);
extern void fn_801236F8(void);
extern f32 lbl_8047B8A4;
#if 0
asm void fn_800232F0(void) {
#include "src/game/gs_title_fn_800232F0.inc"
}
#else
void fn_800232F0(void) { /* TODO */ }
#endif

/* fn_80023760 - 0x80023760 | size: 0x208 */
#if 0
asm void fn_80023760(void) {
#include "src/game/gs_title_fn_80023760.inc"
}
#else
#pragma optimization_level 4
s32 fn_80023760(u32 arg0, u32* arg1) {
    u8 buf[0x80];
    u16 entries[5];
    s32 total;
    s32 slot;
    s32 count;
    s32 effect;
    u32 sc;
    u32 sd;
    u32 species;
    u32 msg;

    total = 0;
    fn_800141BC((void*)arg0, 0);
    for (slot = 0; slot < 6; slot++) {
        fn_80014118(slot, &sc, &sd);
        if ((u8)fn_80123FBC(sc) != 0) {
            if ((s32)fn_8012640C(sc, 0, 0x83, 0) <= 0) {
                if ((u8)fn_80121ADC(sc, 0x3E) == 0) {
                    effect = fn_80144574(buf, sc, sd, (u16)arg0, 0);
                    if ((s16)effect > 0) {
                        memcpy(entries, lbl_80266DB0, sizeof(entries));
                        count = 0;
                        if (arg0 == entries[0]) {
                            count = 0;
                        } else if (arg0 == entries[1]) {
                            count = 1;
                        } else if (arg0 == entries[2]) {
                            count = 2;
                        } else if (arg0 == entries[3]) {
                            count = 3;
                        } else if (arg0 == entries[4]) {
                            count = 4;
                        } else {
                            count = 5;
                        }
                        if (count < 5) {
                            species = 0x466;
                        } else {
                            species = 0x465;
                        }
                        fn_80166A50(species, 0, 0xFF, 0);
                        fn_8001D378();
                        fn_800216E8(buf, 0x40, (u8*)sd, effect, sc);
                        fn_80132A38(0x4D, buf);
                        fn_80106D3C(2, 0xE0, 1, 0);
                        fn_801069FC(1);
                        total = (u16)(total + effect);
                    }
                }
            }
        }
    }
    if ((u16)total == 0) {
        fn_80106D3C(2, 0x4261, 1, 0);
        fn_801069FC(1);
        effect = -1;
    } else {
        effect = 1;
    }
    fn_80014198(-1);
    if (effect < 0) {
        return 1;
    }
    *arg1 = 1;
    return 0;
}
#endif

/* fn_80023968 - 0x80023968 | size: 0x234 */
extern void fn_80143DFC(void);
extern void fn_80143A94(void);
extern void fn_801437B8(void);
#if 0
asm void fn_80023968(void) {
#include "src/game/gs_title_fn_80023968.inc"
}
#else
void fn_80023968(void) { /* TODO */ }
#endif

/* fn_80023B9C - 0x80023B9C | size: 0x20c */
#if 0
asm void fn_80023B9C(void) {
#include "src/game/gs_title_fn_80023B9C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80023B9C(u32 arg0, u32* arg1) {
    extern void fn_800216E8(void*, s32, void*, s32, s32);
    u32 sc;
    u32 sd;
    volatile u16 entries[5];
    u8 text_buf[0x100];
    u8 name_buf[0x84];
    register u32* out;
    register u32 species;
    register s32 slot;
    register s32 effect;
    register s32 match_index;
    s32 sound_id;
    void* msg;

    species = arg0;
    out = arg1;
    slot = fn_800141BC((void*)species, 1);
    if (slot >= 0) {
        fn_80014118(slot, &sc, &sd);
        if ((u8)fn_80121ADC(sc, 0x3E) == 0) {
            effect = fn_80144574(text_buf, sc, sd, (u16)species, 0);
            if ((s16)effect > 0) {
                *(u32*)&entries[0] = *(u32*)(lbl_80266DB0 + 0);
                *(u32*)&entries[2] = *(u32*)(lbl_80266DB0 + 4);
                entries[4] = *(u16*)(lbl_80266DB0 + 8);

                match_index = 0;
                if ((((species != entries[0]) && (match_index = 1, species != entries[1])) &&
                     (match_index = 2, species != entries[2])) &&
                    ((match_index = 3, species != entries[3]) &&
                     (match_index = 4, species != entries[4]))) {
                    match_index = 5;
                }
                if (match_index < 5) {
                    sound_id = 0x466;
                } else {
                    sound_id = 0x465;
                }
                fn_80166A50(sound_id, 0, 0xFF, 0);
                fn_8001D378();
            }
            fn_800216E8(name_buf, 0x40, text_buf, effect, sc);
            fn_80132A38(0x4D, name_buf);
            fn_80106D3C(2, 0xE0, 1, 0);
            fn_801069FC(1);
        } else {
            msg = fn_8011F4F0(sc);
            fn_80132A38(0x32, msg);
            fn_80106D3C(2, 0x424D, 1, 0);
            fn_801069FC(1);
            effect = 0;
        }
    }
    fn_80014198(slot);

    if (slot >= 0 && (s16)effect > 0) {
        if ((s32)species >= 0x2C) goto species_store_one;
        if ((s32)species >= 0x27) goto species_store_zero;
        goto species_store_one;
species_store_zero:
        *out = 0;
        goto ret_zero;
species_store_one:
        *out = 1;
ret_zero:
        return 0;
    }

    return 1;
}
#pragma pop
#endif

/* fn_80023DA8 - 0x80023DA8 | size: 0x3c */
#if 0
asm void fn_80023DA8(void) {
#include "src/game/gs_title_fn_80023DA8.inc"
}
#else
#pragma scheduling off
#pragma optimization_level 4
s32 fn_80023DA8(void) {
    fn_80106D3C(2, 0x44c6, 1, 0);
    fn_801069FC(1);
    return 1;
}
#endif

/* fn_80023DE4 - 0x80023DE4 | size: 0x3c */
#if 0
asm void fn_80023DE4(void) {
#include "src/game/gs_title_fn_80023DE4.inc"
}
#else
#pragma scheduling off
#pragma optimization_level 4
s32 fn_80023DE4(void) {
    fn_80106D3C(2, 0x4261, 1, 0);
    fn_801069FC(1);
    return 1;
}
#endif

/* fn_80023E20 - 0x80023E20 | size: 0x3c */
#if 0
asm void fn_80023E20(void) {
#include "src/game/gs_title_fn_80023E20.inc"
}
#else
#pragma scheduling off
#pragma optimization_level 4
s32 fn_80023E20(void) {
    fn_80106D3C(2, 0x426a, 1, 0);
    fn_801069FC(1);
    return 1;
}
#endif

/* fn_80023E5C - 0x80023E5C | size: 0x4 */
#if 0
asm void fn_80023E5C(void) {
#include "src/game/gs_title_fn_80023E5C.inc"
}
#else
#pragma optimization_level 4
void fn_80023E5C(void) { }
#endif

/* fn_80023E60 - 0x80023E60 | size: 0x300 */
extern void fn_80104318(void);
extern void fn_800E0060(void);
extern void fn_800E0000(void);
extern u32 lbl_8047A370;
extern u32 lbl_8047A368;
extern u32 lbl_8047A36C;
extern f32 lbl_8047B8A8;
extern f32 lbl_80478898;
extern f64 lbl_8047B8B8;
extern f32 lbl_8047B8AC;
extern u32 lbl_8047A390;
extern f32 lbl_8047B8B0;
extern u8 lbl_8047A380;
extern void fn_80024160(u8*, void*, u16*, u8*);
extern u8 lbl_803A2058[];
extern u8 lbl_803A204C[];
#if 0
asm void fn_80023E60(void) {
#include "src/game/gs_title_fn_80023E60.inc"
}
#else
#pragma push
#pragma scheduling on
#pragma peephole off
#pragma optimization_level 4
s32 fn_80023E60(u8* arg0) {
    extern s32 fn_80104318(u8*);
    extern u16* fn_80105624(void);
    extern u8* fn_8005DA18(u32);
    extern u8* fn_8005D934(s32);
    extern void fn_800E0060(f32*, f32*);
    extern f32 fn_800E0000(f32*, f32*);
    extern void fn_80165A20(s32, s32, s32);
    extern f64 lbl_8047B8B8;
    /* nonvol alloc: r31=arg0, r30=DA18 result (called first), r29=104318 result (called second) */
    u8* r104318;
    u8* da18;
    u16* temp_r5;
    s32 result;
    s32 target;
    s32 found;
    u8* p;
    f32 sp14[3];
    f32 sp08[3];
    f32 cmpval;

    if ((s32)lbl_8047A370 == 1) {
        return 0;
    }

    da18 = fn_8005DA18(*(u32*)(arg0 + 4));
    r104318 = (u8*)fn_80104318(arg0);

    if (r104318 == NULL) {
        arg0[0x95] = 0;
        return 0;
    }

    temp_r5 = fn_80105624();
    fn_80024160(arg0, r104318, temp_r5, da18);

    result = (s32)(s8)arg0[0x95] + (s32)(s8)arg0[0x94];
    target = (s32)lbl_8047A368;
    lbl_8047A36C = result;

    if ((u32)target != (u32)result) {
        lbl_80478898 = lbl_8047B8A8;
        lbl_8047A370 = 1;

        p = fn_8005DA18(*(u32*)(arg0 + 4));
        p = fn_8005D934(*(s16*)(p + 4));
        found = 0;
        while (1) {
            if (((u32)*(volatile u8*)p >> 7) & 1) {
                if (target != found) goto SKIP1;
                goto MATCH1;
SKIP1:
                found = found + 1;
            }
            if (((u32)*(volatile u8*)p >> 6) & 1) break;
            p = fn_8005D934(*(s16*)(p + 0x18));
        }
        p = NULL;
MATCH1:
        if (p != NULL) {
            *(f32*)(lbl_803A2058 + 0) = (f32)(s32)(*(s16*)(p + 2));
            *(f32*)(lbl_803A2058 + 8) = lbl_8047B8AC;
            *(f32*)(lbl_803A2058 + 4) = (f32)(s32)(*(s16*)(p + 4));
        }

        target = (s32)lbl_8047A36C;
        p = fn_8005DA18(*(u32*)(arg0 + 4));
        p = fn_8005D934(*(s16*)(p + 4));
        found = 0;
        while (1) {
            if (((u32)*(volatile u8*)p >> 7) & 1) {
                if (target != found) goto SKIP2;
                goto MATCH2;
SKIP2:
                found = found + 1;
            }
            if (((u32)*(volatile u8*)p >> 6) & 1) break;
            p = fn_8005D934(*(s16*)(p + 0x18));
        }
        p = NULL;
MATCH2:
        if (p != NULL) {
            *(f32*)(lbl_803A204C + 0) = (f32)(s32)(*(s16*)(p + 2));
            *(f32*)(lbl_803A204C + 8) = lbl_8047B8AC;
            *(f32*)(lbl_803A204C + 4) = (f32)(s32)(*(s16*)(p + 4));
        }

        {
            u8* r5 = (u8*)lbl_8047A390;
            f32 ref_x = *(f32*)(lbl_803A204C + 0);
            f32 ref_y = *(f32*)(lbl_803A204C + 4);
            cmpval = lbl_8047B8AC;
            sp14[0] = ref_x - (f32)(s32)(*(s16*)(r5 + 2));
            sp14[1] = ref_y - (f32)(s32)(*(s16*)(r5 + 4));
            if (cmpval != sp14[0] || cmpval != sp14[1]) {
                fn_800E0060(sp08, sp14);
                sp14[1] = lbl_8047B8AC;
                sp14[0] = lbl_8047B8B0;
                sp14[2] = lbl_8047B8AC;
                if (fn_800E0000(sp14, sp08) < lbl_8047B8AC) {
                    lbl_8047A380 = 1;
                } else {
                    lbl_8047A380 = 0;
                }
            }
        }

        fn_80165A20(0x464, 0, 0xFF);
    }

    return 0;
}
#pragma pop
#endif

/* fn_80024160 - 0x80024160 | size: 0x1a8 */
extern u32 lbl_8047A368;
extern u32 lbl_80478DF4;
extern u32 lbl_80478DF0;
#if 0
asm void fn_80024160(u8* arg0, void* arg1, u16* arg2, u8* arg3) {
#include "src/game/gs_title_fn_80024160.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
void fn_80024160(u8* arg0, void* arg1, u16* arg2, u8* arg3) {
    s32 chain_index;
    s32 active_index;
    s32 offset;
    s32 visible_index;
    u8 mask;
    u8* active;
    s32 entry_index;
    u8* current;
    u8* candidate;
    s32 v;

    (void)arg1;
    v = *arg2;
    mask = 0;
    if ((v & 0x1) != 0) mask |= 0x1;
    if ((v & 0x2) != 0) mask |= 0x4;
    if ((v & 0x4) != 0) mask |= 0x8;
    if ((v & 0x8) != 0) mask |= 0x2;

    if (mask != 0) {
        active_index = lbl_8047A368;
        current = fn_8005DA18(*(u32*)(arg0 + 4));
        current = fn_8005D934(*(s16*)(current + 4));
        chain_index = 0;
        while (1) {
            if (((u32)*(volatile u8*)current >> 7) & 1) {
                if (active_index == chain_index) {
                    active = current;
                    break;
                }
                chain_index++;
            }
            if ((((u32)*(volatile u8*)current >> 6) & 1) == 0) {
                current = fn_8005D934(*(s16*)(current + 0x18));
            } else {
                active = 0;
                break;
            }
        }

        mask = (u8)mask;
        entry_index = 0;
        offset = 0;
        while ((u32)entry_index < *(u32*)lbl_80478DF0) {
            visible_index = 0;
            candidate = fn_8005D934(*(s16*)(arg3 + 4));
            while (1) {
                if (candidate == 0) {
                    break;
                }
                if (((u32)*(volatile u8*)candidate >> 7) & 1) {
                    if ((*(u8*)(lbl_80478DF4 + offset) & mask) == mask) {
                        if (fn_8005D934(*(u32*)(lbl_80478DF4 + offset + 4)) == active) {
                            if (fn_8005D934(*(u32*)(lbl_80478DF4 + offset + 8)) == candidate) {
                                arg0[0x95] = (u8)visible_index;
                                return;
                            }
                        }
                    }
                    visible_index++;
                }
                if ((((u32)*(volatile u8*)candidate >> 6) & 1) == 0) {
                    candidate = fn_8005D934(*(s16*)(candidate + 0x18));
                } else {
                    break;
                }
            }
            offset += 0xC;
            entry_index++;
        }
    }
}
#pragma pop
#endif

/* fn_80024308 - 0x80024308 | size: 0x130
 *
 * Status: 99.1% matched. Remaining: reg-alloc choice in second branch
 * (f0/f1/f2 ordering: B8C8 in f1 vs f2, stack in f2 vs f0, B8898 in f0 vs f1),
 * plus 2 anonymous @NNN@sda21 f64 magic constants for u32->f32 conversion
 * (CW heuristic; target uses lbl_8047B8D0 name).
 * Second branch: separate f3=(f32)(u32)fn_800D3088() to avoid __cvt_fp2unsigned.
 */
extern u32 lbl_8047A370;
extern f64 lbl_8047B8D0;
extern f32 lbl_8047A378;
extern f32 lbl_8047A374;
extern u32 lbl_8047B8C0;
extern u32 lbl_8047B8C4;
extern f32 lbl_8047B8C8;
extern f32 lbl_80478898;
extern f32 lbl_8047B8AC;
extern u32 lbl_8047A36C;
extern u32 lbl_8047A368;
#if 0
asm void fn_80024308(void) {
#include "src/game/gs_title_fn_80024308.inc"
}
#else
#pragma push
#pragma optimization_level 2
#pragma scheduling on
#pragma fp_contract on
#pragma peephole off
s32 fn_80024308(u8* arg0) {
    extern u8* fn_80105624(void);
    u8* ctx;
    f32 f0;
    f32 f1;
    f32 f2;
    f32 f3;

    ctx = fn_80105624();
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

    f2 = (f32)(u32)fn_800D3088();
    f3 = lbl_8047A378;
    f0 = f3 * f2 + lbl_8047A374;
    lbl_8047A374 = f0;
    f1 = *(f32*)&lbl_8047B8C0;
    if (f0 < f1) {
        lbl_8047A374 = f1;
        lbl_8047A378 = -f3;
    } else {
        f1 = *(f32*)&lbl_8047B8C4;
        if (f0 > f1) {
            lbl_8047A374 = f1;
            lbl_8047A378 = -f3;
        }
    }

    if ((s32)lbl_8047A370 == 1) {
        f3 = (f32)(u32)fn_800D3088();
        f1 = lbl_80478898 - lbl_8047B8C8 * f3;
        lbl_80478898 = f1;
        if (f1 < lbl_8047B8AC) {
            lbl_80478898 = lbl_8047B8AC;
            lbl_8047A370 = 0;
            lbl_8047A368 = lbl_8047A36C;
        }
    }
    return 0;
}
#pragma pop
#endif
