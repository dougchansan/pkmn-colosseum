/**
 * @file gs_title_8001EF78.c
 * @brief Head of the title translation unit, 0x8001EF78 - 0x8002058C: the
 *        title menu tick, button and item-draw callbacks (historically split
 *        as gs_pcbox_*), the title floor coroutine fn_8001FD48, and GStitle's
 *        cleanup, main loop and init (fn_80020328, fn_800203B4, fn_8002049C,
 *        formerly in gs_title.c).
 *
 * These functions share one .sdata2 pool, 0x8047B808-0x8047B868, which this
 * unit owns: fn_800203B4's two-float frame table first, then fn_8001EF78's
 * literals and its two conversion biases, fn_8001F304's, and fn_8002049C's
 * column targets. The pool repeats 0.0f and 1.0f right after it (0x8047B868,
 * gs_title_80020618.c's own pool), which is where the next source file
 * starts. Retail is GC/1.3 with -opt nopeephole, the title code's flag.
 */
#include "dolphin/types.h"
#include "game/menu/menu.h"

typedef struct GSmodel GSmodel;

typedef struct TitleMenuWork {
    u8 pad0[0x10];
    f32 offset[6];     /* 0x10: current column offsets */
    f32 target[6];     /* 0x28: target column offsets */
} TitleMenuWork;

typedef struct TitleMenuItem {
    u8 pad0[4];
    s8 flags;          /* 0x04: bit 1 = shown */
    u8 pad5;
    s16 id;            /* 0x06: menu item id */
    u8 pad8[0x50 - 0x8];
    s16 x;             /* 0x50 */
    u8 pad52[0x67 - 0x52];
    u8 alpha;          /* 0x67 */
} TitleMenuItem;

/* The title model's animation frames at which fn_800203B4 plays sound
 * 0x46E. Retail has them as the pool's first two objects (0x8047B808,
 * 0x8047B80C), which MWCC gives only to file-scope constants defined
 * ahead of the code. */
const f32 lbl_8047B808 = 180.0f;
const f32 lbl_8047B80C = 199.0f;

extern TitleMenuWork lbl_803A1F88;
extern u8 lbl_802EF0A8[];          /* menu item data, 0x1C bytes per item id */
extern s32 lbl_80478878;           /* current title menu page */
extern s32 lbl_8047A310;           /* title mode */
extern s32 lbl_8047A314;           /* paused while fn_8017B1AC reports 5 */
extern s32 lbl_8047A318;           /* paused while fn_8017B1AC reports 4/11 */
extern s32 lbl_8047A31C;           /* title state */
extern s32 lbl_8047A320;           /* set when title mode 1 starts at state 7 */
extern GSmodel* lbl_8047A324;      /* title model */
extern s32 lbl_8047A328;
extern u8 lbl_8047A32C;            /* title thread finished */
extern void* lbl_8047A330;         /* title thread */
extern f32 lbl_8047A334;           /* attract timer */
extern f32 lbl_8047A338;           /* blink phase */
extern s32 lbl_8047A33C;
extern f32 lbl_8047A340;
extern f32 lbl_8047A344;           /* frame step */
extern f32 lbl_8047A348;
extern u8 lbl_8047A34C;

extern s32 fn_800D37CC(void);
extern u32 msgGetCharsPerFrame(void);
extern s32 GSsndGetStatus(s32 id);
extern u32 fn_800F7AF0(s32 pad);
extern u32 fn_800F7BC4(s32 pad);
extern u8* windowGetKeyInfo(void);
extern void dbgMenuSetEnable(s32 enable);
extern void soundStopRaw(s32 id, s32 a, s32 b);
extern void soundStartRaw(s32 id, s32 a, s32 b);
extern void* menuDataBiosGetPtr(u32 id);
extern f64 sin(f64);
extern GSmodel* fn_800F92D4(u32 resourceId);
extern void GSmodelSetAnimIndex(GSmodel* model, s32 index);
extern void GSmodelSetAnimFrame(GSmodel* model, f32 frame);
extern void GSmodelSetAnimRate(GSmodel* model, f32 rate);
extern void GSmodelStartAnimation(GSmodel* model);
extern void GSmodelSetAnimType(GSmodel* model, s32 type);
extern u8 GSmodelIsAnimating(GSmodel* model);
extern f32 GSmodelGetAnimFrame(GSmodel* model);
extern void GSmodelAllPauseAnimation(void);
extern void GSmodelAllUnpauseAnimation(void);
extern void cameraStartAnimation(void);
extern void cameraStopAnimation(void);
extern void cameraPlayAnime(u32 groupId, u32 animationId, s32 frame, u8 loop);
extern void fn_801662E8(s32 a, s32 b);
extern void std104_soundSetBackgroundMusic(s32 id, s32 a, s32 volume);
extern void soundResumeMusic(void);
extern void soundPauseMusic(void);
extern s32 fn_8017B1AC(void);
extern void _threadSwitch(void);
extern s32 menuOpen(s32 id, s32 arg);
extern void fadeSet(s32 mode, f32 time);
extern s8 fadeCheck(s32 wait);
extern s32 fn_801D0748(s32 a, s32 b, s32 c);
extern void savedataCreate(s32 a, s32 b);
extern u32 gamedatasaveGetStatus(s32 a, s32 b);
extern void fn_800216E0(u32 value);
extern void floorLink(s32 floorId, s32 arg);
extern void menuCloseCustom(s32 menuId, s32 mode, s32 force);
extern void winMsgClose(s32 wait);
extern void GSthreadTerminate(void* threadCtx);
extern void* GSthreadCreate(s32 priority, void* stack, s32 stackSize,
                            s32 flags, s32 p5, void (*entry)(void));
extern void* floorGetCurrentContext(void);
extern u8 fn_800FF548(void);
extern void floorSetFadeScript(s32 p1, s32 p2);
void fn_8002058C(void);
void fn_8002060C(void);

/* fn_8001EF78 (0x8001EF78 - 0x8001F1E8): title menu 0x15's tick.
 * - the frame step lbl_8047A344 = fn_800D3088() / fn_800D37CC() (an
 *   unsigned count over a signed rate);
 * - the blink phase lbl_8047A338 advances by step / 2 and wraps to 0 at 1
 *   (fn_8001F304 turns it into PRESS START's alpha);
 * - from title state 0x1E on (except state 0xC8) the attract timer
 *   lbl_8047A334 advances by the step. When fn_801666BC(0x46A) returns 0
 *   the timer is cleared, and unless the state is 0x3E8 the title moves to
 *   state 0x28;
 * - the six menu column offsets in lbl_803A1F88 ease toward their targets
 *   at 4 * distance * step per frame, capped at 6. They snap to the target
 *   once the remaining distance is no more than the step or under 1. */

#define TITLE_ABS(x) ((x) > 0.0f ? (x) : -(x))

/* Ease `cur` toward `tgt` by the frame step, capped at +-6. */
#define TITLE_EASE(cur, tgt)                                                \
    if ((cur) != (tgt)) {                                                   \
        f32 move;                                                           \
        f32 rest;                                                           \
        f32 absMove;                                                        \
        move = (tgt) - (cur);                                               \
        rest = 4.0f * move;                                                 \
        move = rest * step;                                                 \
        if (move > 6.0f) {                                                  \
            move = 6.0f;                                                    \
        }                                                                   \
        if (move <= -6.0f) {                                                \
            move = -6.0f;                                                   \
        }                                                                   \
        (cur) += move;                                                      \
        rest = (tgt) - (cur);                                               \
        if (move > 0.0f) {                                                  \
            absMove = move;                                                 \
        } else {                                                            \
            absMove = -move;                                                \
        }                                                                   \
        if (TITLE_ABS(rest) <= absMove || TITLE_ABS(rest) < 1.0f) {         \
            (cur) = (tgt);                                                  \
        }                                                                   \
    }

void fn_8001EF78(void)
{
    f32 rate;
    s32 i;
    f32 step;
    TitleMenuWork* work;

    rate = fn_800D37CC();
    lbl_8047A344 = msgGetCharsPerFrame() / rate;
    lbl_8047A338 += lbl_8047A344 / 2.0f;
    if (lbl_8047A338 >= 1.0f) {
        lbl_8047A338 = 0.0f;
    }
    if (lbl_8047A31C < 0x1E) {
        return;
    }
    if (lbl_8047A31C == 0xC8) {
        return;
    }
    lbl_8047A334 += lbl_8047A344;
    if (GSsndGetStatus(0x46A) == 0) {
        lbl_8047A334 = 0.0f;
        if (lbl_8047A31C != 0x3E8) {
            lbl_8047A31C = 0x28;
        }
    }
    step = lbl_8047A344;
    work = &lbl_803A1F88;
    for (i = 0; i < 3; i++) {
        TITLE_EASE(work->offset[i], work->target[i])
        TITLE_EASE(work->offset[i + 3], work->target[i + 3])
    }
}

/* fn_8001F1E8 (0x8001F1E8 - 0x8001F304): title menu button callback.
 * Before the title state reaches 4 the callback disables the debug menu and
 * waits for A/START (0x1100) or B (0x200): either one advances the state to
 * 4, plays sound 0x46E and raises lbl_8047A328. From state 4 on it enables
 * the debug menu and marks the item (offset 0x98) when the window key info
 * reports 0x810. -opt nopeephole keeps retail's `andi.` + `cmplwi` pairs. */
void fn_8001F1E8(u8* arg)
{
    u32 a;
    u32 b;
    u8* obj;

    if (lbl_8047A31C < 4) {
        dbgMenuSetEnable(0);
        if (arg == 0) return;
        menuDataBiosGetPtr(*(u32*)(arg + 0x4));
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if ((b & a) & 0x1100) {
            lbl_8047A31C = 4;
            soundStopRaw(0x46e, 0, 0);
            lbl_8047A328 = 1;
        }
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if (((b & a) & 0x200) == 0) return;
        lbl_8047A31C = 4;
        soundStopRaw(0x46e, 0, 0);
        lbl_8047A328 = 1;
        return;
    }
    dbgMenuSetEnable(1);
    if (arg == 0) return;
    menuDataBiosGetPtr(*(u32*)(arg + 0x4));
    obj = windowGetKeyInfo();
    if ((*(u16*)(obj + 0x4) & 0x810) == 0) return;
    *(u8*)(arg + 0x98) = 1;
    soundStartRaw(0x4c2, 0, 0);
}

/* fn_8001F304 (0x8001F304 - 0x8001FD48): title menu item draw callback.
 * winSpriteDraw calls this for every sprite item of the title menus 0x13 and
 * 0x15 (items 0x2C4 and 0xEFA-0xF0D). It picks which items are shown for
 * the current title state and slides the menu columns:
 * - 0xEFA ("PRESS START") is shown while the title state lbl_8047A31C is
 *   0x1E..0x20 and blinks: its alpha is 128 * sin(pi * t * speed) + 200,
 *   clamped to 0..255, with speed 4 in state 0x1F and 2 otherwise;
 * - 0x2C4 is shown for the same states, without the blink;
 * - 0xEFB-0xF0D belong to one of four menu pages (lbl_80478878). An item on
 *   the current page is shown and placed at its menu-data x (the 0x1C-byte
 *   records of lbl_802EF0A8) plus one of the six animated column offsets in
 *   lbl_803A1F88, which fn_8001EF78 eases toward their targets;
 * - any other item is hidden unless lbl_8047A34C is set. */

/* Show the item when the title menu is on `page`, at its menu-data x plus
 * column offset `column`; hide it otherwise. */
#define TITLE_ITEM_ON_PAGE(page, column)                                    \
    if (lbl_80478878 == (page)) {                                           \
        s16 base;                                                           \
        item->flags |= 2;                                                   \
        base = *(s16*)(lbl_802EF0A8 + item->id * 0x1C + 2);                 \
        item->x = base + (s32)lbl_803A1F88.offset[column];                  \
    } else {                                                                \
        item->flags &= ~2;                                                  \
    }                                                                       \
    break;

void fn_8001F304(void* menu, TitleMenuItem* item)
{
    u8 visible;
    f32 speed;
    f32 alpha;

    switch (item->id) {
    case 0xEFA:
        if (lbl_8047A31C < 0x1E || lbl_8047A31C > 0x20) {
            item->flags &= ~2;
            visible = 0;
        } else {
            item->flags |= 2;
            visible = 1;
        }
        if (visible) {
            if (lbl_8047A31C == 0x1F) {
                speed = 4.0f;
            } else {
                speed = 2.0f;
            }
            alpha = 3.1415927f * (lbl_8047A338 * speed);
            alpha = 128.0f * (f32)sin(alpha) + 200.0f;
            if (alpha > 255.0f) {
                alpha = 255.0f;
            }
            if (alpha < 0.0f) {
                alpha = 0.0f;
            }
            item->alpha = alpha;
        }
        break;
    case 0x2C4:
        if (lbl_8047A31C < 0x1E || lbl_8047A31C > 0x20) {
            item->flags &= ~2;
        } else {
            item->flags |= 2;
        }
        break;
    case 0xEFB: TITLE_ITEM_ON_PAGE(0, 3)
    case 0xEFC: TITLE_ITEM_ON_PAGE(0, 4)
    case 0xEFD: TITLE_ITEM_ON_PAGE(0, 0)
    case 0xEFE: TITLE_ITEM_ON_PAGE(0, 1)
    case 0xEFF: TITLE_ITEM_ON_PAGE(1, 3)
    case 0xF00: TITLE_ITEM_ON_PAGE(1, 4)
    case 0xF01: TITLE_ITEM_ON_PAGE(1, 5)
    case 0xF02: TITLE_ITEM_ON_PAGE(1, 0)
    case 0xF03: TITLE_ITEM_ON_PAGE(1, 1)
    case 0xF04: TITLE_ITEM_ON_PAGE(1, 2)
    case 0xF05: TITLE_ITEM_ON_PAGE(2, 3)
    case 0xF06: TITLE_ITEM_ON_PAGE(2, 4)
    case 0xF07: TITLE_ITEM_ON_PAGE(2, 0)
    case 0xF08: TITLE_ITEM_ON_PAGE(2, 1)
    case 0xF09: TITLE_ITEM_ON_PAGE(2, 2)
    case 0xF0A: TITLE_ITEM_ON_PAGE(3, 0)
    case 0xF0B: TITLE_ITEM_ON_PAGE(3, 1)
    case 0xF0C: TITLE_ITEM_ON_PAGE(3, 3)
    case 0xF0D: TITLE_ITEM_ON_PAGE(3, 4)
    default:
        if (lbl_8047A34C == 0) {
            item->flags &= ~2;
        }
        break;
    }
}

/* fn_8001FD48 (0x8001FD48 - 0x80020328): the title floor (floor 900)
 * coroutine. It owns the title state word lbl_8047A31C that fn_800203B4
 * polls, fills the title model/camera globals (lbl_8047A324/33C/340) that
 * fn_800203B4 reads, and finishes by linking to floor 0x3A1. Retail keeps
 * unpeepholed `mr r4,r3; mr r30,r4` copy chains and dead loop-entry
 * branches (-opt nopeephole). */

/* Expanded four times below with only the animation index changing. */
static inline void titleModelPlay(GSmodel* model, s32 index)
{
    if (model != NULL) {
        GSmodelSetAnimIndex(model, index);
        GSmodelSetAnimFrame(model, 0.0f);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelStartAnimation(model);
    }
}

/*
 * Guarded wait for a model animation. The early return survives inlining as
 * `bne loop; b done; b loop`; a plain `if (model != NULL)` compiles to a
 * single beq. floor_event carries the same expansion four times
 * (0x801165A4, 0x80116724, 0x80116750, 0x80116CC4).
 */
static inline void modelWaitAnimation(GSmodel* model)
{
    if (model == NULL) {
        return;
    }
    while (GSmodelIsAnimating(model) != 0) {
        _threadSwitch();
    }
}

void std021_newGameIntroSequence(void)
{
    s32 running;
    GSmodel* model;
    s32 status;

    running = 1;
    lbl_8047A31C = 0;
    lbl_8047A348 = 0.0f;
    model = fn_800F92D4(0x0B541000);
    lbl_8047A340 = 0.0f;
    lbl_8047A324 = model;
    lbl_8047A33C = 0;
    lbl_8047A328 = 0;
    fn_801662E8(0, 0x406);
    std104_soundSetBackgroundMusic(0x46A, 0, 0xFF);

    if (lbl_8047A310 == 1) {
        lbl_8047A31C = 7;
        lbl_8047A310 = 0;
        lbl_8047A320 = 1;
    } else {
        lbl_8047A320 = 0;
    }

    menuOpen(0x15, 0);

    do {
        status = fn_8017B1AC();
        if (status == 11 || status == 4) {
            if (lbl_8047A318 == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                soundPauseMusic();
                lbl_8047A318 = 1;
            }
            _threadSwitch();
            continue;
        }
        if (lbl_8047A318 == 1) {
            cameraStartAnimation();
            fn_801662E8(0, 0x406);
            soundResumeMusic();
            GSmodelAllUnpauseAnimation();
            lbl_8047A318 = 0;
        }

        if (status == 5) {
            if (lbl_8047A314 == 0) {
                GSmodelAllPauseAnimation();
                cameraStopAnimation();
                soundPauseMusic();
                lbl_8047A314 = 1;
            }
            _threadSwitch();
            continue;
        }
        if (lbl_8047A314 == 1) {
            cameraStartAnimation();
            fn_801662E8(0, 0x406);
            soundResumeMusic();
            GSmodelAllUnpauseAnimation();
            lbl_8047A314 = 0;
        }

        switch (lbl_8047A31C) {
        case 0:
            titleModelPlay(model, 0);
            GSmodelSetAnimType(model, 0);
            cameraPlayAnime(0x12, 0x0B551800, 0, 0);
            fadeSet(4, 0.5f);
            if (lbl_8047A31C == 0) {
                lbl_8047A31C = 1;
            }
            break;
        case 1:
            if (GSmodelIsAnimating(model) == 0) {
                lbl_8047A31C = 2;
                break;
            }
            _threadSwitch();
            break;
        case 2:
            titleModelPlay(model, 1);
            GSmodelSetAnimType(model, 0);
            lbl_8047A31C = 3;
            break;
        case 3:
            modelWaitAnimation(model);
            lbl_8047A31C = 8;
            break;
        case 4:
            fadeSet(5, 0.5f);
            lbl_8047A31C = 6;
            break;
        case 5:
            if (fadeCheck(0) == 0) {
                lbl_8047A31C = 6;
                break;
            }
            _threadSwitch();
            break;
        case 6:
            while (lbl_8047A348 < 0.5f) {
                lbl_8047A348 += lbl_8047A344;
                _threadSwitch();
            }
            lbl_8047A31C = 7;
            break;
        case 7:
            cameraPlayAnime(0x12, 0x0B561800, 0, 0);
            titleModelPlay(model, 2);
            GSmodelSetAnimType(model, 1);
            fadeSet(4, 0.5f);
            fadeCheck(1);
            lbl_8047A31C = 0x1E;
            break;
        case 8:
            cameraPlayAnime(0x12, 0x0B561800, 0, 0);
            titleModelPlay(model, 2);
            GSmodelSetAnimType(model, 1);
            lbl_8047A31C = 0x1E;
            break;
        case 0x28:
            lbl_8047A31C = 0x3E8;
            break;
        case 0x1E:
            lbl_8047A31C = 0x20;
            break;
        case 0x20:
            if (menuOpenCustom(MENU_ID(0x13), windowGetActiveID(), NULL, 0,
                               MENU_CURSOR_CHECK(1), 0) < 0) {
                break;
            }
            lbl_8047A31C = 0x1F;
            lbl_8047A348 = 0.0f;
            break;
        case 0x1F:
            while (lbl_8047A348 < 1.0f) {
                lbl_8047A348 += lbl_8047A344;
                _threadSwitch();
            }
            lbl_8047A31C = 0xC8;
            break;
        case 0xC8:
            status = fn_801D0748(1, 2, 0);
            if (status == -1) {
                lbl_8047A31C = 0x20;
                break;
            }
            if (status != 3) {
                savedataCreate(0, 0);
            }
            if (gamedatasaveGetStatus(0, 4) != 0) {
                fn_800216E0(1);
            } else {
                fn_800216E0(0);
            }
            lbl_8047A31C = 0x29;
            break;
        case 0x29:
            floorLink(0x3A1, 0);
            lbl_8047A31C = 0x3E8;
            break;
        case 0xC9:
            break;
        case 0x3E8:
            running = 0;
            break;
        }
    } while (running != 0);
}

/* fn_80020328 (GStitle cleanup): cancels menus 0x13, 0x15 and 0x16, closes
 * any open message box, then waits for the title thread to finish. */
void fn_80020328(void)
{
    menuCloseCustom(0x13, 0, 1);
    menuCloseCustom(0x15, 0, 1);
    menuCloseCustom(0x16, 0, 1);
    winMsgClose(1);

    if (lbl_8047A330 != NULL) {
        for (;;) {
            if (lbl_8047A32C == 1) {
                GSthreadTerminate(lbl_8047A330);
                break;
            }
            _threadSwitch();
        }
    }

    lbl_8047A330 = NULL;
    lbl_8047A32C = 0;
}

/* fn_800203B4 (GStitle main loop): links the autodemo floor on state 0x28,
 * and while the title model runs plays sound 0x46E once each time its
 * animation frame passes 180 and then 199. */
void fn_800203B4(void)
{
    for (;;) {
        if (lbl_8047A31C == 0x28) {
            floorLink(0x39c, 0);
            lbl_8047A31C = 0x3e8;
            continue;
        }

        if (lbl_8047A320 == 0 && lbl_8047A324 != NULL) {
            s32 phase;

            lbl_8047A340 += lbl_8047A344;
            phase = lbl_8047A33C;

            if (phase < 2 && lbl_8047A328 == 0) {
                u32 frames[2];
                f32 threshold;

                /* RULE-EXCEPTION(title-path): the frame table is copied as
                 * words through no-op casts, as retail's lwz/stw copy does -
                 * see docs/RULE_EXCEPTIONS.md */
                frames[0] = *(const u32*)&lbl_8047B808;
                frames[1] = *(const u32*)&lbl_8047B80C;
                threshold = *(f32*)&frames[phase];
                if (GSmodelGetAnimFrame(lbl_8047A324) >= threshold) {
                    if (lbl_8047A33C == 0) {
                        soundStartRaw(0x46e, 0, 0);
                    } else {
                        soundStopRaw(0x46e, 0, 0);
                    }
                    lbl_8047A33C++;
                }
            }
        }

        _threadSwitch();
    }
}

/* fn_8002049C (GStitle init): starts the title thread, resets the frame
 * step and the autodemo model, advances the menu page (0..3) and sets the
 * six menu column offsets and targets. */
void fn_8002049C(void)
{
    void* ctx;

    lbl_8047A32C = 0;
    ctx = floorGetCurrentContext();
    lbl_8047A330 = GSthreadCreate(0x14, ctx, 0x2000, 1, 0, fn_8002058C);
    lbl_8047A314 = 0;
    lbl_8047A344 = 0.0f;
    lbl_8047A324 = NULL;
    floorSetFadeScript(0, 0);
    lbl_80478878 = lbl_80478878 + 1;
    if (lbl_80478878 >= 4) {
        lbl_80478878 = 0;
    }
    {
        u8* base = (u8*)&lbl_803A1F88;
        *(f32*)(base + 0x10) = 300.0f;
        *(f32*)(base + 0x1c) = -300.0f;
        *(f32*)(base + 0x28) = 0.0f;
        *(f32*)(base + 0x34) = 0.0f;
        *(f32*)(base + 0x14) = 400.0f;
        *(f32*)(base + 0x20) = -400.0f;
        *(f32*)(base + 0x2c) = 0.0f;
        *(f32*)(base + 0x38) = 0.0f;
        *(f32*)(base + 0x18) = 500.0f;
        *(f32*)(base + 0x24) = -500.0f;
        *(f32*)(base + 0x30) = 0.0f;
        *(f32*)(base + 0x3c) = 0.0f;
    }
    if (fn_800FF548() == 0) {
        fn_8002060C();
    }
}
