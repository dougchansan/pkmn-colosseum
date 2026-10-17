/**
 * @file menu_pda_mail.c
 * @brief PDA Mailbox reader UI, 0x8004B7EC - 0x8004EADC.
 *
 * XD-anchor-backed identity: menuPdaOpen (0x34) and pdaMailGetMailID
 * (0x50) byte-size-match XD's identically-named functions exactly;
 * calls mailGetMailIDInMailbox/mailGetReceiveNumber; owns 9 private
 * widget tables plus the 56-entry mail-list layout table. Positional
 * porting from XD's menuPdaMail* family failed (reordered subset) --
 * further naming needs byte-level comparison. All functions asm-only
 * until matched.
 */
#include "dolphin/types.h"
#include "game/cursor_bios.h"

/* Small PDA-mail state byte pair, shared with pda_range_80037158.c
 * (the sibling PDA-body TU) and initialized by fn_8004B7EC. Only
 * bytes [0] and [1] are touched by this TU's accessors. */
extern u8 lbl_803A6A60[];
extern u16* lbl_8047A500;

/* The linked units carved from this file build only their own functions:
 * MENU_PDA_MAIL_SORT_ONLY the mailbox sort pair (fn_8004BFB0, fn_8004C120),
 * MENU_PDA_MAIL_LIST_ONLY the mailbox list menu (fn_8004D34C),
 * MENU_PDA_MAIL_HIGHLIGHT_ONLY the row highlight callback (fn_8004DA64),
 * MENU_PDA_MAIL_PICKER_ONLY the handle picker input callback (fn_8004DDC0)
 * MENU_PDA_MAIL_ATTACH_ONLY the attachment viewer (fn_8004E9C0)
 * and MENU_PDA_MAIL_SORTLABEL_ONLY the sort label and page arrows
 * (fn_8004C3E4, fn_8004C4A4). */
#if defined(MENU_PDA_MAIL_SORT_ONLY) || defined(MENU_PDA_MAIL_LIST_ONLY) || \
    defined(MENU_PDA_MAIL_HIGHLIGHT_ONLY) || defined(MENU_PDA_MAIL_PICKER_ONLY) || \
    defined(MENU_PDA_MAIL_ATTACH_ONLY) || defined(MENU_PDA_MAIL_SORTLABEL_ONLY)
#define MENU_PDA_MAIL_PARTIAL
#endif

typedef struct PdaMailSceneState {
    s8 selection;
    u8 menuBusy;
    u8 pad02[0x0A];
    f32 angles[3];
    u8 pad18[0x10];
    f32 field28;
    f32 field2C;
    f32 transition;
    f32 modelHeight;
    f32 transitionTarget;
    f32 field3C;
    f32 field40;
    u32 animationIndex;
    u8 savedSelection;
    u8 offscreenHidden;
    u8 exiting;
} PdaMailSceneState;

typedef struct PdaMailOpenConfig {
    u8 first;
    u8 second;
    u8 third;
    u8 pad03;
    u32* message;
    u32* value;
} PdaMailOpenConfig;

extern const f32 lbl_8047BDAC;
extern const f32 lbl_8047BDA0;
extern const f32 lbl_8047BDA8;
extern const f32 lbl_8047BDF4;
extern const f32 lbl_8047BDF8;
extern const f32 lbl_8047BDFC;
extern const f32 lbl_8047BE00;
extern u32 lbl_8047A4F8;
extern u32 lbl_8047A4FC;

extern void menuOffScreenSetPriority(s32 priority);
extern void menuOffScreenSetDisp(s32 visible);
extern void menuOpen(s32 menuId, s32 parameter);
extern u32 windowGetActiveID(void);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuClose(s32 menuId);
extern void menuCloseSync(s32 menuId, s32 flag);
extern void* fn_800F92D4(u32 resourceId);
extern void fn_800E3CC8(void* model, s32 value);
extern void cameraPlayAnime(s32 camera, u32 resourceId, s32 startFrame, s32 flags);
extern void GSmodelSetAnimIndex(void* model, s32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, s32 type);
extern void GSmodelStartAnimation(void* model);
extern u8 GSmodelIsAnimating(void* model);
extern void GSmodelStopAnimation(void* model);
extern void GSmodelSetVisibility(void* model, s32 visible);
extern void GSlightSetAnimRate(void* light, f32 rate);
extern void GSlightSetAnimFrame(void* light, f32 frame);
extern void GSlightSetAnimType(void* light, s32 type);
extern void GSlightStartAnimation(void* light);
extern void GSlightSetActive(void* light, s32 active);
extern void soundStartRaw(s32 soundId, s32 arg1, s32 arg2);
extern void cameraWaitSyncAnime(s32 camera);
extern void fn_8004A47C(void);
extern void fn_8003C7C0(void);
extern void fn_80044630(void);
extern u8 std081_flagCheckTrue(s32 id);
extern void fn_8004C120(void);
extern void fadeSet(s32 type, f32 speed);
extern s8 fadeCheck(s32 type);
extern void fn_800FF660(void);
extern void floorSetFadeScript(s32 a, u32 b);
extern void _threadSwitch(void);
extern void fn_800FB680(s32 x, s32 y, u32 color, s32 msgId);

typedef struct PdaMailAttachmentConfig {
    f32* scroll;
    s32 mailId;
    s32* status;
    s32 y;
    s32 x;
} PdaMailAttachmentConfig;

#ifndef MENU_PDA_MAIL_PARTIAL
#pragma peephole off
void std027_unkMenuSequence27(void)
{
    PdaMailOpenConfig config;
    s32 menuSelection;
    PdaMailSceneState* state = (PdaMailSceneState*) lbl_803A6A60;
    register f32* transitionTarget = &state->transitionTarget;
    register f32* transition = &state->transition;
    s32 phase = 0;
    s32 active;
    void* model;
    void* light;

    state->exiting = 0;
    state->selection = 0;
    state->savedSelection = 0;
    state->menuBusy = 0;
    state->angles[0] = lbl_8047BDAC;
    state->angles[1] = lbl_8047BDAC;
    state->angles[2] = lbl_8047BDAC;
    state->field2C = lbl_8047BDA0;
    state->field28 = lbl_8047BDA0;
    *transition = lbl_8047BDF4;
    state->modelHeight = lbl_8047BDF8;
    *transitionTarget = lbl_8047BDAC;
    state->field3C = lbl_8047BDAC;
    state->field40 = lbl_8047BDAC;
    state->animationIndex = 0;
    state->offscreenHidden = 1;
    state->exiting = 0;

    menuOffScreenSetPriority(0);
    model = fn_800F92D4(0x0C541000);
    light = fn_800F92D4(0x0C541601);
    fn_800E3CC8(model, 1);
    cameraPlayAnime(0x17, 0x0C541800, 0, 0);
    if (model != 0) {
        GSmodelSetAnimIndex(model, 0);
        GSmodelSetAnimFrame(model, lbl_8047BDAC);
        GSmodelSetAnimRate(model, lbl_8047BDA8);
    }
    GSmodelSetAnimType(model, 0);
    GSlightSetAnimRate(light, lbl_8047BDA8);
    GSlightSetAnimFrame(light, lbl_8047BDAC);
    GSlightSetAnimType(light, 0);
    GSlightStartAnimation(light);
    GSmodelStartAnimation(model);
    soundStartRaw(0x448, 0, 0);
    if (model != 0) {
        while (GSmodelIsAnimating(model)) {
            _threadSwitch();
        }
    }
    GSmodelStopAnimation(model);
    cameraWaitSyncAnime(1);
    GSlightSetActive(light, 0);
    GSlightSetActive(fn_800F92D4(0x0C541600), 0);
    GSlightSetActive(fn_800F92D4(0x0C541602), 0);
    fn_800F92D4(0x0C540200);
    fn_800F92D4(0x0C541800);
    fn_8004A47C();

    config.message = &lbl_8047A4FC;
    config.first = 0;
    config.second = 0xFF;
    config.third = 0;
    config.value = &lbl_8047A4F8;
    *config.message = 0x36B2;
    menuOpen(0xF0, 0);
    menuOpen(0x10D, 0);
    menuOpenCustom(0x71, windowGetActiveID(), 0, 0, 0, 1, (s32*) &config);
    GSmodelSetVisibility(model, 0);

    while (!state->exiting) {
        *config.message = 0x36B2;
        menuSelection = phase;
        menuOpen(0x97, 0);
        menuOpen(0x98, 0);
        menuOpen(0x99, 0);
        menuOpen(0x9A, 0);
        if (menuOpenCustom(0x72, windowGetActiveID(), &menuSelection, 0, 1, 0) == -1) {
            phase = -1;
        } else {
            phase = ((s8*)lbl_803A6A60)[0];
        }
        lbl_803A6A60[0] = (s8) phase;
        active = 1;
        state->menuBusy = active;

        switch (phase) {
        case 0:
            *transitionTarget = lbl_8047BDFC;
            active = 1;
            while (active) {
                if (*transition == *transitionTarget) {
                    active = 0;
                } else {
                    _threadSwitch();
                }
            }
            menuClose(0x97);
            menuCloseSync(0x97, 1);
            *config.message = 0x36B4;
            fn_8003C7C0();
            state->menuBusy = 0;
            *transitionTarget = lbl_8047BDAC;
            break;
        case 1:
            *transitionTarget = lbl_8047BDFC;
            active = 1;
            while (active) {
                if (*transition == *transitionTarget) {
                    active = 0;
                } else {
                    _threadSwitch();
                }
            }
            menuClose(0x97);
            menuCloseSync(0x97, 1);
            *config.message = 0x36B5;
            fn_80044630();
            state->menuBusy = 0;
            *transitionTarget = lbl_8047BDAC;
            break;
        case 2:
            *transitionTarget = lbl_8047BDFC;
            active = 1;
            while (active) {
                if (*transition == *transitionTarget) {
                    active = 0;
                } else {
                    _threadSwitch();
                }
            }
            menuClose(0x97);
            menuCloseSync(0x97, 1);
            if (!std081_flagCheckTrue(0x3F0)) {
                phase = 1;
            }
            *config.message = 0x36B3;
            fn_8004C120();
            state->menuBusy = 0;
            *transitionTarget = lbl_8047BDAC;
            break;
        default:
            fadeSet(3, lbl_8047BE00);
            *transitionTarget = lbl_8047BDFC;
            state->field3C = lbl_8047BDF8;
            state->exiting = 1;
            break;
        }
    }

    phase = 0;
    active = 1;
    while (active) {
        switch (phase) {
        case 0:
            if (!fadeCheck(0)) {
                phase = 100;
            } else {
                _threadSwitch();
            }
            break;
        case 100:
            active = 0;
            break;
        }
    }

    if (state->offscreenHidden != 1) {
        menuOffScreenSetDisp(1);
        state->offscreenHidden = 1;
    }
    menuClose(0x97);
    menuCloseSync(0x97, 1);
    menuClose(0xF0);
    menuClose(0x71);
    menuCloseSync(0xF0, 1);
    menuCloseSync(0x71, 1);

    phase = 0;
    active = 1;
    while (active) {
        switch (phase) {
        case 0:
            fadeSet(2, lbl_8047BDA8);
            phase = 1;
            break;
        case 1:
            if (!fadeCheck(0)) {
                phase = 100;
            } else {
                _threadSwitch();
            }
            break;
        case 100:
            active = 0;
            break;
        }
    }
    fn_800FF660();
    floorSetFadeScript(0, 0);
}
#pragma peephole reset

/* PI/6 and 2*PI -- rotation-angle wrap constants shared by the PDA
 * mail-icon spin/animation helpers. */
extern f64 lbl_8047BE28;
extern f64 lbl_8047BE30;

typedef struct PdaMailSpinWork {
    u8 pad00[0x70];
    f32 angle;
} PdaMailSpinWork;

#if 0
asm s32 fn_8004E144(void* window, PdaMailSpinWork* sprite) {
#include "src/game/menu/menu_pda_mail_fn_8004E144.inc"
}
#else
/* Exact farm result; the redundant locals preserve MWCC's register shape. */
s32 fn_8004E144(void* window, PdaMailSpinWork* sprite)
{
    f64 new_var2;
    f64 wrap = lbl_8047BE30;
    f64 new_var;

    wrap = 0;
    new_var = wrap;
    if ((sprite->angle += lbl_8047BE28) >= new_var) {
        new_var2 = wrap;
        sprite->angle -= (wrap, new_var2);
    }
    return 0;
}

#endif

/* pdaMailGetMailID: byte-size-matches XD's pdaMailGetMailID (0x50)
 * exactly (see file header); not yet ported to C in this TU, but its
 * asm-linked symbol/signature is known from the XD reference (takes
 * a mailbox-slot index, returns the mail ID, or -1 out of range). */
extern s32 pdaMailGetMailID(s32 index);

/* mailGetContents (battle_waza.c): "Waza entry get field 0x14 by index". */
extern u32 mailGetContents(s32 idx);

typedef struct PdaMailWindowA {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
    u8 pad08[0x58];
    s32** field_0x60;
} PdaMailWindowA;

typedef struct PdaMailOutA {
    u8 pad00[0x4c];
    u32 field_0x4c;
} PdaMailOutA;

#if 0
asm s32 fn_8004D6AC(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D6AC.inc"
}
#else
s32 fn_8004D6AC(PdaMailWindowA* window, PdaMailOutA* out)
{
    out->field_0x4c = mailGetContents(pdaMailGetMailID(**window->field_0x60));
    return 0;
}
#endif

/* windowGetKeyInfo (gs_event_exec.c): returns the current input-device
 * state pointer; menuButtonNormal (gs_model.c): resets a widget's
 * button sprite to its normal (unpressed) state. */
extern u8* windowGetKeyInfo(void);
extern void menuButtonNormal(void* p);

/* winSpriteSetDisp (gs_worldmap.c): set a window-sprite field-handle's
 * display/visibility value. */
extern void winSpriteSetDisp(void* fieldHandle, s32 value);

typedef struct PdaMailWindowB {
    u8 pad00[0x60];
    s32* field_0x60;
} PdaMailWindowB;

extern u32 mailGetAttachFileGroup(s32 index);
extern s32 fn_8017B2CC(u32 fileHandle);
extern s32 fn_8017B448(u32 fileHandle);
extern u32 fn_8017B4BC(u32 fileHandle, u32 index);
extern u32 fn_8017B5A4();

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8004E440) — see docs/RULE_EXCEPTIONS.md */
static inline s32 pdaMailIsAttachReady(s32 index)
{
    if (fn_8017B2CC(mailGetAttachFileGroup(index)) == 1) {
        return 0;
    }
    return 1;
}

/* RULE-EXCEPTION(user-approved): reconstructed inline helper (fn_8004E440, fn_8004E180, fn_8004E2E0, fn_8004E510) — see docs/RULE_EXCEPTIONS.md */
static inline s32 pdaMailCountAttachItems(s32* state)
{
    u32 object;
    s32 index;
    s32 total;
    s32 count;
    s32 i;

    index = state[1];

    if (!pdaMailIsAttachReady(index)) {
        return -1;
    }
    object = mailGetAttachFileGroup(index);
    total = fn_8017B448(object);
    count = 0;
    for (i = 0; i < total; i++) {
        fn_8017B4BC(object, i);
        if (fn_8017B5A4() == 9) {
            count++;
        }
    }
    return count;
}

#pragma peephole off
s32 fn_8004E440(PdaMailWindowB* window, void* fieldHandle)
{
    s32 count;

    count = pdaMailCountAttachItems(window->field_0x60);
    if (count <= 0) {
        winSpriteSetDisp(fieldHandle, 0);
    } else {
        winSpriteSetDisp(fieldHandle, 1);
    }
    return 0;
}
#pragma peephole reset

typedef struct PdaMailAttachState {
    s32 unused;
    s32 mailIndex;
    s32* selection;
} PdaMailAttachState;

typedef struct PdaMailAttachWindow {
    u8 pad00[0x60];
    PdaMailAttachState* state;
} PdaMailAttachWindow;

#endif /* MENU_PDA_MAIL_PARTIAL */

typedef struct PdaMailWindowC {
    u8 pad00[0x60];
    s32* field_0x60;
    u8 pad64[0x31];
    s8 selection;
} PdaMailWindowC;

typedef struct PdaMailSpriteField {
    u8 pad00[6];
    s16 msgId;
} PdaMailSpriteField;

extern const s32 lbl_802672D8[6];
extern const s32 lbl_802671D0[12];

#ifndef MENU_PDA_MAIL_PARTIAL
s32 fn_8004C5B0(void* unused, PdaMailSpriteField* field)
{
    typedef union PdaMailCursorPosition {
        u32 storage;
        u16 packed;
        struct {
            s8 page;
            s8 row;
        } position;
    } PdaMailCursorPosition;
    PdaMailCursorPosition cursors[2];
    s32 i;

    cursors[1].packed = cursors[0].packed =
        (u16) (cursorBiosGetPos(10) >> 16);
    for (i = 0; i < 12; i++) {
        if (field->msgId == lbl_802671D0[i]) {
            break;
        }
    }
    if (i >= 12) {
        return 0;
    }
    if (i == cursors[1].position.row) {
        winSpriteSetDisp(field, 1);
    } else {
        winSpriteSetDisp(field, 0);
    }
    return 0;
}

#endif /* MENU_PDA_MAIL_PARTIAL */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_HIGHLIGHT_ONLY)
extern void winSpriteSetDisp(void* fieldHandle, s32 value);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8004DA64(PdaMailWindowC* window, PdaMailSpriteField* field)
{
    s32* statePtr = window->field_0x60;
    s32 table[5];
    s32 i;
    u8 visible;

    table[0] = lbl_802672D8[0];
    table[1] = lbl_802672D8[1];
    table[2] = lbl_802672D8[2];
    table[3] = lbl_802672D8[3];
    table[4] = lbl_802672D8[4];
    if (*statePtr != 0) {
        visible = 0;
    } else {
        for (i = 0; i < 5; i++) {
            if (field->msgId == table[i]) {
                break;
            }
        }
        if (i >= 5) {
            return 0;
        }
        if (window->selection == i) {
            visible = 1;
        } else {
            visible = 0;
        }
    }
    winSpriteSetDisp(field, visible);
    return 0;
}
#pragma peephole reset
#endif /* MENU_PDA_MAIL_HIGHLIGHT_ONLY */

#ifndef MENU_PDA_MAIL_PARTIAL

/* mailGetAttachFileGroup (battle_waza.c): "Waza entry get field 0x18 by index". */
extern u32 mailGetAttachFileGroup(s32 idx);

#if 0
asm s32 fn_8004D5EC(PdaMailWindowA* window, void* fieldHandle) {
#include "src/game/menu/menu_pda_mail_fn_8004D5EC.inc"
}
#else
s32 fn_8004D5EC(PdaMailWindowA* window, void* fieldHandle)
{
    u8 flag;
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        flag = 1;
    } else {
        flag = 0;
    }
    winSpriteSetDisp(fieldHandle, flag);
    return 0;
}
#endif

#if 0
asm s32 fn_8004D64C(PdaMailWindowA* window, void* fieldHandle) {
#include "src/game/menu/menu_pda_mail_fn_8004D64C.inc"
}
#else
s32 fn_8004D64C(PdaMailWindowA* window, void* fieldHandle)
{
    u8 flag;
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        flag = 1;
    } else {
        flag = 0;
    }
    winSpriteSetDisp(fieldHandle, flag);
    return 0;
}
#endif

#if 0
asm s32 fn_8004D590(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D590.inc"
}
#else
s32 fn_8004D590(PdaMailWindowA* window, PdaMailOutA* out)
{
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        out->field_0x4c = 0x36B9;
    } else {
        out->field_0x4c = 0;
    }
    return 0;
}
#endif

/* mailGetSenderName (battle_waza.c): "Waza entry get field 0x0C by index".
 * GSmsgGetGSchar/msgctrlSetValue (gs_title.c): message/window callbacks. */
extern u32 mailGetSenderName(s32 idx);
extern void* GSmsgGetGSchar(u32);
extern void msgctrlSetValue(s32, void*);

#if 0
asm s32 fn_8004D6F0(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D6F0.inc"
}
#else
s32 fn_8004D6F0(PdaMailWindowA* window, PdaMailOutA* out)
{
    u32 val = mailGetSenderName(pdaMailGetMailID(**window->field_0x60));
    if (val != 0) {
        void* winPtr = GSmsgGetGSchar(val);
        msgctrlSetValue(0x37, winPtr);
        out->field_0x4c = 0xE7;
    } else {
        out->field_0x4c = 0;
    }
    return 0;
}
#endif

/* mailGetSubject (battle_waza.c): "Waza entry get field 0x10 by index". */
extern u32 mailGetSubject(s32 idx);

#if 0
asm s32 fn_8004D760(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D760.inc"
}
#else
s32 fn_8004D760(PdaMailWindowA* window, PdaMailOutA* out)
{
    u32 val = mailGetSubject(pdaMailGetMailID(**window->field_0x60));
    if (val != 0) {
        void* winPtr = GSmsgGetGSchar(val);
        msgctrlSetValue(0x37, winPtr);
        out->field_0x4c = 0xE7;
    } else {
        out->field_0x4c = 0;
    }
    return 0;
}
#endif

#if 0
asm void fn_8004D8BC(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004D8BC.inc"
}
#else
void fn_8004D8BC(PdaMailWindowA* window)
{
    u8* state = windowGetKeyInfo();
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0
        || (*(u16*) state & 0x10) == 0) {
        menuButtonNormal(window);
    }
}
#endif

/* Mail-list cursor input callback. The high byte of cursorPosition is the
 * mailbox page and the low byte is the row (10 and 11 are auxiliary rows). */
s32 fn_8004CF78(u8* window)
{
    typedef union PdaMailCursorState {
        u32 packed;
        struct {
            s8 page;
            s8 row;
        } position;
    } PdaMailCursorState;
    extern s32 mailGetNbMailInMailbox(void);
    PdaMailCursorState cursor;
    u16 persistedPosition;
    u8* input;
    s32 remaining;

    input = windowGetKeyInfo();
    cursorBiosGetPos(10);
    *(u16*) &cursor.position.page = *(u16*) (window + 0x94);

    if ((*(u16*) (input + 6) & 2) != 0) {
        if (++cursor.position.row > 11) {
            cursor.position.row = 0;
        }
        remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
        if (remaining > 10) {
            remaining = 10;
        } else if (remaining < 0) {
            remaining = 0;
        }
        if (cursor.position.row < 10 && cursor.position.row >= remaining) {
            cursor.position.row = 10;
        }
    }
    if ((*(u16*) (input + 6) & 1) != 0) {
        if (--cursor.position.row < 0) {
            cursor.position.row = 11;
        }
        remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
        if (remaining > 10) {
            remaining = 10;
        } else if (remaining < 0) {
            remaining = 0;
        }
        if (cursor.position.row < 10 && cursor.position.row >= remaining) {
            if (remaining > 0) {
                cursor.position.row = remaining - 1;
            } else {
                cursor.position.row = 11;
            }
        }
    }
    if ((*(u16*) (input + 6) & 8) != 0) {
        if (cursor.position.row < 10) {
            remaining = mailGetNbMailInMailbox();
            remaining = (remaining + 9) / 10;
            if (++cursor.position.page >= remaining) {
                cursor.position.page = 0;
            }
            remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
            if (remaining > 10) {
                remaining = 10;
            } else if (remaining < 0) {
                remaining = 0;
            }
            if (cursor.position.row >= remaining) {
                cursor.position.row = remaining - 1;
            }
        } else {
            cursor.position.row = 11;
        }
    }
    if ((*(u16*) (input + 6) & 4) != 0) {
        if (cursor.position.row < 10) {
            if (--cursor.position.page < 0) {
                remaining = mailGetNbMailInMailbox();
                cursor.position.page = (remaining + 9) / 10 - 1;
            }
            remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
            if (remaining > 10) {
                remaining = 10;
            } else if (remaining < 0) {
                remaining = 0;
            }
            if (cursor.position.row >= remaining) {
                cursor.position.row = remaining - 1;
            }
        } else {
            cursor.position.row = 10;
        }
    }

    persistedPosition = *(u16*) &cursor.position.page;
    cursorBiosSetPos(10, &persistedPosition);
    *(u16*) (window + 0x94) = *(u16*) &cursor.position.page;
    return 0;
}

#endif /* MENU_PDA_MAIL_PARTIAL */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_LIST_ONLY)
/* Mailbox list menu: seed the cursor from the caller's flat index, run the
 * modal list until it is dismissed, and hand back the packed page/row
 * selection (row 10 is the sort button, row 11 the handle picker). */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8004D34C(s32 index)
{
    typedef union PdaMailCursorPosition {
        u32 storage;
        u16 packed;
        struct {
            s8 page;
            s8 row;
        } position;
    } PdaMailCursorPosition;
    extern s32 mailGetNbMailInMailbox(void);
    extern s32 mailGetSortMode(void);
    extern s32 fn_8004DC18(s32 mode);
    extern void fn_8004BFB0(void);
    extern u8 fn_8004DFCC(u8 initialSelection);
    extern u8 fn_801D16C4(void);
    extern void fn_801D167C(u8 handle);
    extern void fn_801D1B10(s32 handle);
    extern s32 lbl_804788E0;
    extern s32 lbl_8047A508;
    extern s32 lbl_8047A50C;
    extern f32 lbl_8047A510;
    extern const f32 lbl_8047BE20;
    extern u8 lbl_802EF0A8[];
    typedef struct PdaMailListArgs {
        f32* scroll;
        s32 x;
        s32 y;
    } PdaMailListArgs;
    PdaMailListArgs args;
    PdaMailCursorPosition cursor;
    PdaMailCursorPosition seed;
    PdaMailCursorPosition reset;
    PdaMailCursorPosition live;
    s32 choice;
    s32 page;
    s32 row;
    s32 remaining;
    s32 sortMode;
    u8 handle;

    if (lbl_804788E0 != 0) {
        lbl_804788E0 = 0;
        lbl_8047A50C = *(s16*)(lbl_802EF0A8 + 0x7772);
        lbl_8047A508 = *(s16*)(lbl_802EF0A8 + 0x7756);
    }
    page = index / 10;
    row = index % 10;
    remaining = mailGetNbMailInMailbox() - page * 10;
    if (remaining > 10) {
        remaining = 10;
    } else if (remaining < 0) {
        remaining = 0;
    }
    if (row >= remaining) {
        row = 10;
    }
    cursor.position.page = (s8)page;
    cursor.position.row = (s8)row;
    seed.packed = cursor.packed;
    cursorBiosSetPos(10, &seed.packed);
    while (1) {
        lbl_8047A510 = lbl_8047BE20;
        args.scroll = &lbl_8047A510;
        args.x = lbl_8047A50C;
        args.y = lbl_8047A508;
        choice = menuOpenCustom(0x73, windowGetActiveID(), 0, 0, 1, 1, &args);
        cursor.packed = *(u16*)&live.packed = cursorBiosGetPos(10) >> 16;
        if (choice == -1) {
            break;
        }
        if (cursor.position.row < 10) {
            break;
        }
        switch (cursor.position.row) {
        case 10:
            sortMode = fn_8004DC18(mailGetSortMode());
            if (sortMode >= 0) {
                fn_801D1B10(sortMode);
                fn_8004BFB0();
                row = 0;
                remaining = mailGetNbMailInMailbox();
                if (remaining > 10) {
                    remaining = 10;
                } else if (remaining < 0) {
                    remaining = 0;
                }
                if (remaining <= 0) {
                    row = 10;
                }
                cursor.position.page = 0;
                cursor.position.row = (s8)row;
                reset.packed = cursor.packed;
                cursorBiosSetPos(10, &reset.packed);
            }
            break;
        case 11:
            handle = fn_8004DFCC(fn_801D16C4());
            if (handle != 0xff) {
                fn_801D167C(handle);
            }
            break;
        }
    }
    menuClose(0x73);
    menuCloseSync(0x73, 1);
    if (choice == -1) {
        return -1;
    }
    return cursor.position.row + cursor.position.page * 10;
}
#pragma peephole reset
#endif /* MENU_PDA_MAIL_LIST_ONLY */

/* mailGetReceiveNumber (XD-named, same address/size): returns the
 * receive-order slot for a given mail ID, or -1 if not found. */
extern s32 mailGetReceiveNumber(s32 mailId);

/* GScharCmp (menuCB_Battle.c): compares two rendered-message buffers. */
extern s32 GScharCmp(void* a, void* b);

extern s32 fn_8004BE90(u16* a, u16* b);
extern s32 fn_8004BF20(u16* a, u16* b);

#ifndef MENU_PDA_MAIL_PARTIAL

/* winSeqSetMenu (gs_event_exec.c): fires a scripted SE/event by (ctx, id). */
extern void winSeqSetMenu(s32 ctx, s32 id);

/* Small widget/state-machine record shared by the phase-triggered SE
 * callbacks below: phase drives a switch (only phases 0 and 3 do
 * anything), guard is a one-shot latch, msgObj is passed straight
 * through to winSeqSetMenu as its first (context) argument. */
typedef struct PdaMailPhaseWidget {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
} PdaMailPhaseWidget;

#if 0
asm s32 fn_8004D928(PdaMailPhaseWidget* w) {
#include "src/game/menu/menu_pda_mail_fn_8004D928.inc"
}
#else
s32 fn_8004D928(PdaMailPhaseWidget* w)
{
    switch (w->phase) {
    case 0:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c2);
            w->guard = 1;
        }
        break;
    case 3:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c6);
            w->guard = 1;
        }
        break;
    }
    return 0;
}
#endif

/* Angle-wrap constants for the two phase-2 float animations below
 * (distinct sdata2 float pair per callback; same idiom as fn_8004E144
 * but single-precision and accessed through window->field_0x60). */
extern f32 lbl_8047BE18;
extern f32 lbl_8047BE1C;
extern f32 lbl_8047BE4C;
extern f32 lbl_8047BE50;

#if 0
asm s32 fn_8004D26C(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004D26C.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local peephole control;
 * see docs/RULE_EXCEPTIONS.md. */
s32 fn_8004D26C(PdaMailWindowA* window)
{
    s32** field = window->field_0x60;
    switch (window->phase) {
    case 0:
        if (window->guard == 0) {
            winSeqSetMenu(window->msgObj, 0x1c2);
            window->guard = 1;
        }
        break;
    case 2: {
        f32 result;
        f32 thresh = lbl_8047BE1C;
        f32 val = *(f32*)*field;
        /* RULE-EXCEPTION(user-approved): no-op copy sets FP web priority;
         * see docs/RULE_EXCEPTIONS.md. */
        val = val;
        result = val + lbl_8047BE18;
        *(f32*)*field = result;
        if (result >= thresh) {
            *(f32*)*field -= thresh;
        }
        break;
    }
    case 3:
        if (window->guard == 0) {
            winSeqSetMenu(window->msgObj, 0x1c6);
            window->guard = 1;
        }
        break;
    }
    return 0;
}
#endif

/* windowGetActiveID/menuOpenCustom/menuClose/menuCloseSync (gs_event_exec.c):
 * modal list-menu open/poll/close idiom -- same call skeleton as the
 * gs_event_exec.c item-quantity-picker (menu_id, input-state,
 * &config, 0, 1, 1, &out), open by id, close by id. */

#endif /* MENU_PDA_MAIL_PARTIAL */

/* mailGetSortMode (battle_waza.c): Waza party mailbox-sort-mode byte
 * getter (0=default/none, 1=ascending, 2=ascending+recent-sort,
 * 3=ascending+alpha-sort). mailGetMailIDInMailbox (battle_waza.c):
 * mail ID by receive-order index. qsort: standard library sort. */
extern s32 mailGetSortMode(void);
extern s32 mailGetMailIDInMailbox(s32 idx);
extern void qsort(void* base, u32 count, u32 size,
                   s32 (*cmp)(const void*, const void*));

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_SORTLABEL_ONLY)

typedef struct PdaMailSortLabelWindow {
    u8 pad00[0x8b];
    u8 color;
} PdaMailSortLabelWindow;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (fn_8004C3E4) — see docs/RULE_EXCEPTIONS.md */
#pragma opt_propagation off
s32 fn_8004C3E4(PdaMailSortLabelWindow* window)
{
    typedef struct MailSortMessageIds {
        u32 values[4];
    } MailSortMessageIds;
    extern const u32 lbl_802672C8[];
    extern u32 GSmsgGetRect(u32 msgId);
    extern void fn_800FB680(s32 x, s32 y, s32 color, u32 msgId);
    MailSortMessageIds messageIds;
    s32 sortMode;
    u32 messageId;

    messageIds = *(const MailSortMessageIds*)lbl_802672C8;
    sortMode = mailGetSortMode();
    if (sortMode < 0 || sortMode >= 4) {
        return 0;
    }

    messageId = messageIds.values[sortMode];
    {
        s32 colorMask = -0x100;
        u32 alpha = window->color;
        s32 color = alpha | colorMask;

        fn_800FB680(0, 0, color, messageId);
        fn_800FB680(GSmsgGetRect(messageId) >> 16, 0,
                     color, 0x36c1);
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop


#pragma peephole off
s32 fn_8004C4A4(u8* context, u8* field)
{
    u8* state;
    s32 pages;

    state = *(u8**) (context + 0x60);
    pages = (mailGetNbMailInMailbox() + 9) / 10;
    if (pages <= 1) {
        winSpriteSetDisp(field, 0);
    } else {
        winSpriteSetDisp(field, 1);
    }

    if (*(s16*) (field + 6) == 0x444) {
        s32 base = *(s32*) (state + 4);
        *(s16*) (field + 0x50) =
            (s16) (16.0f * **(f32**) state + (f32) base);
    } else {
        s32 base = *(s32*) (state + 8);
        *(s16*) (field + 0x50) =
            (s16) (-16.0f * **(f32**) state + (f32) base);
    }
    return 0;
}
#pragma peephole reset

#endif /* MENU_PDA_MAIL_PARTIAL || MENU_PDA_MAIL_SORTLABEL_ONLY */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_SORT_ONLY)
#if 0
asm void fn_8004BFB0(void) {
#include "src/game/menu/menu_pda_mail_fn_8004BFB0.inc"
}
#else
/* Rebuilds the mailbox id buffer in the current sort mode (same arms as
 * fn_8004C120). Each arm keeps its own cursor and counters. */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8004BFB0(void)
{
    extern s32 mailGetNbMailInMailbox(void);
    s32 ascIndex;
    s32 recentIndex;
    s32 alphaCount;
    u16* ascCursor;
    u16* recentCursor;
    s32 alphaIndex;
    u16* descCursor;
    s32 recentCount;
    u16* alphaCursor;
    s32 descIndex;
    u16* output;

    output = lbl_8047A500;
    switch (mailGetSortMode()) {
    case 1:
        ascCursor = output;
        for (ascIndex = 0; ascIndex < mailGetNbMailInMailbox(); ascIndex++) {
            *ascCursor++ = mailGetMailIDInMailbox(ascIndex);
        }
        break;
    case 2:
        recentCount = mailGetNbMailInMailbox();
        recentCursor = output;
        for (recentIndex = 0; recentIndex < recentCount; recentIndex++) {
            *recentCursor++ = mailGetMailIDInMailbox(recentIndex);
        }
        qsort(output, recentCount, sizeof(u16),
              (s32 (*)(const void*, const void*))fn_8004BF20);
        break;
    case 3:
        alphaCount = mailGetNbMailInMailbox();
        alphaCursor = output;
        for (alphaIndex = 0; alphaIndex < alphaCount; alphaIndex++) {
            *alphaCursor++ = mailGetMailIDInMailbox(alphaIndex);
        }
        qsort(output, alphaCount, sizeof(u16),
              (s32 (*)(const void*, const void*))fn_8004BE90);
        break;
    case 0:
    default:
        descCursor = output;
        for (descIndex = mailGetNbMailInMailbox() - 1; descIndex >= 0; descIndex--) {
            *descCursor++ = mailGetMailIDInMailbox(descIndex);
        }
        break;
    }
}
#pragma peephole reset
#endif
#endif /* MENU_PDA_MAIL_SORT_ONLY */

#ifndef MENU_PDA_MAIL_PARTIAL

/* lbl_8047A518: persistent "current mailbox cursor" slot -- read/written
 * across menu-reopen cycles by fn_8004D9C0 below (in/out selection index
 * for the menuOpenCustom modal-list idiom) and (per XD skeleton) by sibling
 * cursor helpers elsewhere in the PDA subsystem. fn_8004E9C0 (defined
 * later in this TU): per-selection SE/animation pump for the mailbox
 * list cursor -- asm-only still, called here only by symbol. */
extern s32 lbl_8047A518;
extern void fn_8004E9C0(s32 mailId);

#if 0
asm s32 fn_8004D9C0(s32 a) {
#include "src/game/menu/menu_pda_mail_fn_8004D9C0.inc"
}
#else
s32 fn_8004D9C0(s32 a)
{
    lbl_8047A518 = a;
    for (;;) {
        s32* cfg = &lbl_8047A518;
        s32 choice = menuOpenCustom(0x74, windowGetActiveID(), 0, 0, 1, 1, (s32*) &cfg);
        if (choice == -1) {
            break;
        }
        {
            s32 mailId = pdaMailGetMailID(lbl_8047A518);
            if (mailGetAttachFileGroup(mailId) != 0) {
                fn_8004E9C0(mailId);
            }
        }
    }
    menuClose(0x74);
    menuCloseSync(0x74, 1);
    return lbl_8047A518;
}
#endif

/* PdaMailOutC: widget/out-record variant used by fn_8004DCC0 -- a
 * halfword "current message id" field at 0x6 (compared against the
 * lookup table below) plus the shared field_0x4c out-slot seen on
 * PdaMailOutA. */
typedef struct PdaMailOutC {
    u8 pad00[6];
    s16 msgId;
    u8 pad08[0x44];
    u32 field_0x4c;
} PdaMailOutC;

extern u32 fn_801D1620(u32 idx);

/* lbl_802672F0 (rodata_80267250.c): shared message-id table; this call
 * site takes a mutable stack COPY of the first 11 (of 12) entries. */
extern const u32 lbl_802672F0[12];

/* fn_80166A50 (gs_event_exec.c/gs_title.c convention): plays an SE by
 * (id, a, b, c). fn_801D1B78/fn_801D1C20/fn_801D228C (battle_waza.c):
 * despite their current unverified shapes there (2.4%/2.4%/1.3% match
 * -- those bodies are still stubs), this call site's actual arg count
 * (1 each) is what matters for our own byte match. */
extern void soundStart(s32 id, s32 a, s32 b, s32 c);
extern s32 fn_801D1B78(s32 mailId);
extern void fn_801D1C20(s32 mailId);
extern void fn_801D228C(u16 mailId);

#if 0
asm s32 fn_8004D7D0(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004D7D0.inc"
}
#else
s32 fn_8004D7D0(PdaMailWindowA* window)
{
    extern s32 mailGetNbMailInMailbox(void);
    s32** field = window->field_0x60;
    u8* state = windowGetKeyInfo();
    s32 index = **field;
    s32 mailId;
    s32 cur = index;
    u16 flags;

    flags = *(u16*) (state + 6);
    if (flags & 0x2) {
        s32 count = mailGetNbMailInMailbox();
        index++;
        if (index >= count) {
            index = 0;
        }
    }
    flags = *(u16*) (state + 6);
    if (flags & 0x1) {
        index--;
        if (index < 0) {
            index = mailGetNbMailInMailbox() - 1;
        }
    }
    if (index != cur) {
        soundStart(0x23, 0, 0xFF, 0);
        **field = index;
    }
    mailId = pdaMailGetMailID(index);
    if (fn_801D1B78(mailId) == 0) {
        fn_801D1C20(mailId);
        fn_801D228C((u16) mailId);
    }
    return 0;
}
#endif

#endif /* MENU_PDA_MAIL_PARTIAL */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_ATTACH_ONLY)
/* Local peephole control (same idiom as the rest of this file) keeps the
 * group test as mr + cmplwi. */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
/* RULE-EXCEPTION(user-approved): local peephole pragma and a plain copy local kept for allocation — see docs/RULE_EXCEPTIONS.md */
void fn_8004E9C0(s32 mailId)
{
    extern u8 lbl_802EF0A8[];
    extern s32 lbl_804788E8;
    extern s32 lbl_8047A530;
    extern s32 lbl_8047A534;
    extern s32 lbl_8047A538;
    extern f32 lbl_8047A53C;
    extern f32 lbl_8047BE48;
    extern void fn_800F915C(u32 group);
    extern void fn_8017B1CC(u32 group);
    extern s32 fn_8017B2CC(u32 group);
    extern void fn_8017B3E4(u32 group);
    extern u32 mailGetAttachFileGroup(s32 mailId);
    PdaMailAttachmentConfig config;
    u32 group;
    s32 file;

    if (lbl_804788E8 != 0) {
        lbl_804788E8 = 0;
        lbl_8047A534 = *(s16*)(lbl_802EF0A8 + 0x8CC6);
        lbl_8047A530 = *(s16*)(lbl_802EF0A8 + 0x8CAA);
    }

    lbl_8047A538 = 0;
    group = mailGetAttachFileGroup(mailId);
    if (group == 0) {
        return;
    }
    file = group;
    fn_8017B3E4(file);
    config.mailId = mailId;
    config.status = &lbl_8047A538;
    config.y = lbl_8047A534;
    config.x = lbl_8047A530;
    lbl_8047A53C = lbl_8047BE48;
    config.scroll = &lbl_8047A53C;
    menuOpenCustom(0x77, windowGetActiveID(), 0, 0, 1, 1, &config);
    while (fn_8017B2CC(file) == 1) {
        _threadSwitch();
    }
    menuClose(0x77);
    menuCloseSync(0x77, 1);
    fn_8017B1CC(file);
    fn_800F915C(group);
}
#pragma peephole reset
#endif /* MENU_PDA_MAIL_ATTACH_ONLY */

#ifndef MENU_PDA_MAIL_PARTIAL
extern u32 fn_80103E68(u32 id);
extern s32 fn_801D1A88(s32 id);
extern s32 fn_801D1ACC(s32 id);
extern s32 fn_801D16F0(s32 id);
extern void fn_80132A38(u32 id, u32 value);
extern u32 fn_800FA280(void);
extern void fn_80109220(u32 object, u32 visible);
extern const s32 lbl_802672A0[10];
extern const s32 lbl_80267278[10];
extern const s32 lbl_80267250[10];
extern const s32 lbl_80267228[10];
extern const s32 lbl_80267200[10];

typedef struct PdaMailRowTable {
    s32 values[10];
} PdaMailRowTable;

typedef union PdaMailRowCursor {
    u32 storage;
    u16 packed;
    struct {
        s8 page;
        s8 row;
    } position;
} PdaMailRowCursor;

u32 fn_8004C6C0(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    void* gschar;

    table = *(const PdaMailRowTable*)lbl_802672A0;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSenderName(mailId);
    if (message != 0) {
        gschar = GSmsgGetGSchar(message);
        msgctrlSetValue(0x37, gschar);
        *(u32*)(object + 0x4C) = 0xE7;
    } else {
        *(u32*)(object + 0x4C) = 0;
    }
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (fn_801D1B78(mailId) != 0) {
            object[0x64] = 0xFF;
            object[0x65] = 0xFF;
            object[0x66] = 0xFF;
        } else {
            object[0x64] = 0xD5;
            object[0x65] = 0xAA;
            object[0x66] = 0x33;
        }
    }
    return 0;
}

u32 fn_8004C8AC(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    void* gschar;

    table = *(const PdaMailRowTable*)lbl_80267278;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSubject(mailId);
    if (message != 0) {
        gschar = GSmsgGetGSchar(message);
        msgctrlSetValue(0x37, gschar);
        *(u32*)(object + 0x4C) = 0xE7;
    } else {
        *(u32*)(object + 0x4C) = 0;
    }
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (fn_801D1B78(mailId) != 0) {
            object[0x64] = 0xFF;
            object[0x65] = 0xFF;
            object[0x66] = 0xFF;
        } else {
            object[0x64] = 0xD5;
            object[0x65] = 0xAA;
            object[0x66] = 0x33;
        }
    }
    return 0;
}

u32 fn_8004CA98(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267250;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (mailGetAttachFileGroup(mailId) != 0) {
            visible = 1;
        } else {
            visible = 0;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 fn_8004CC38(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267228;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (fn_801D1B78(mailId) != 0) {
            visible = 1;
        } else {
            visible = 0;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 fn_8004CDD8(u8* context, u8* object)
{
    PdaMailRowTable table;
    PdaMailRowCursor cursors[2];
    s32 index;
    s32 mailId;
    s32 value;
    u8 visible;

    table = *(const PdaMailRowTable*)lbl_80267200;
    cursors[1].packed = cursors[0].packed = (u16)(cursorBiosGetPos(10) >> 16);
    for (index = 0; index < 10; index++) {
        value = *(s16*)(object + 6);
        if (value == table.values[index]) {
            break;
        }
    }
    if (index >= 10) {
        return 0;
    }
    index += cursors[1].position.page * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId >= 0) {
        if (fn_801D1B78(mailId) != 0) {
            visible = 0;
        } else {
            visible = 1;
        }
    } else {
        visible = 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}


#endif /* MENU_PDA_MAIL_PARTIAL */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_PICKER_ONLY)
extern u8* windowGetKeyInfo(void);
extern void soundStart(s32 id, s32 a, s32 b, s32 c);
extern u32 fn_801D1650(u8 index);
extern void GSsndGetStatus(u32 id);
extern void soundKill(u32 id);
extern void fn_801654E0(u32 id, u32 buffer, u32 size);
extern void soundAttachWork(u32 id, u32 arg1, u32 arg2);
extern s32 fn_801D1618(void);
extern void soundStopRaw(u32 id, u32 arg1, u32 arg2);
extern s32 lbl_8047A520;
extern u8 lbl_8047A524;
extern u32 lbl_8047A528;
extern u32 lbl_8047A52C;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
u32 fn_8004DDC0(u8* context)
{
    u8* input;
    u32 soundId;
    s32 limit;

    input = windowGetKeyInfo();
    if (lbl_8047A520 != 0) {
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            GSsndGetStatus(soundId);
        }
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            soundKill(soundId);
        }
        lbl_8047A524 = *(s8*)(context + 0x95);
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            fn_801654E0(soundId, lbl_8047A52C, 0x10000);
            soundAttachWork(soundId, 0, 0x408);
            soundStart(soundId, 0, 0xFF, 0);
            lbl_8047A528 = 0;
        }
        lbl_8047A520 = 0;
    }

    if ((*(u16*)(input + 4) & 2) != 0) {
        limit = fn_801D1618() + 1;
        if (++*(s8*)(context + 0x95) >= limit) {
            *(s8*)(context + 0x95) = limit - 1;
        }
    }
    if ((*(u16*)(input + 4) & 1) != 0) {
        if (--*(s8*)(context + 0x95) < 0) {
            *(s8*)(context + 0x95) = 0;
        }
    }
    if (lbl_8047A524 != (s8)context[0x95]) {
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            soundStopRaw(soundId, 0, 0);
        }
        lbl_8047A520 = 1;
    }
    return 0;
}
#pragma peephole reset
#endif /* MENU_PDA_MAIL_PICKER_ONLY */

#ifndef MENU_PDA_MAIL_PARTIAL

extern u32 soundPauseMusic(void);
extern u32 GSresAllocResourceAlign(u32 size, u32 align, u32 arg2, u32 group, u32 arg4);
extern void GSresRegisterResource(u32 buffer, u32 arg1, u32 group, u32 arg3);
extern void fn_800F9210(u32 arg0, u32 group);
extern void soundResumeMusic(u32 state);
extern s32 fn_801026A4(u32 menuId, ...);
extern void fn_80102510(u32 menuId);

#pragma peephole off
u8 fn_8004DFCC(u8 initialSelection)
{
    s32 selection;
    u32 state;
    u32 soundId;
    u8 result;
    s32 choice;

    selection = initialSelection;
    state = soundPauseMusic();
    lbl_8047A52C = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
    GSresRegisterResource(lbl_8047A52C, 0, 0x408, 0);
    lbl_8047A524 = initialSelection;
    soundId = fn_801D1650(initialSelection);
    if (soundId != 0) {
        fn_801654E0(soundId, lbl_8047A52C, 0x10000);
        soundAttachWork(soundId, 0, 0x408);
        soundStart(soundId, 0, 0xFF, 0);
        lbl_8047A528 = 0;
    }

    choice = menuOpenCustom(0x76, windowGetActiveID(), &selection, 0, 1, 0);
    if (choice < 0 || choice >= fn_801D1618()) {
        result = 0xFF;
    } else {
        result = choice;
    }
    menuClose(0x76);
    menuCloseSync(0x76, 1);

    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        soundStopRaw(soundId, 0, 0);
    }
    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        GSsndGetStatus(soundId);
    }
    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        soundKill(soundId);
    }
    fn_800F9210(0, 0x408);
    soundResumeMusic(state);
    return result;
}
#pragma peephole reset

extern u32 mailGetAttachFileGroup(s32 index);
extern s32 fn_8017B2CC(u32 fileHandle);
extern s32 fn_8017B448(u32 fileHandle);
extern u32 fn_8017B4BC(u32 fileHandle, u32 index);
extern u32 fn_8017B5A4(void);
extern u32 GSmsgGetRect(s32 messageId);
extern const f32 lbl_8047BE38;
extern const f32 lbl_8047BE3C;

static inline s32 pdaMailCountAttachmentEntries(u32 fileHandle)
{
    s32 count;
    s32 total;
    s32 index;

    if (fn_8017B2CC(fileHandle) == 1) {
        return -1;
    }
    total = fn_8017B448(fileHandle);
    count = 0;
    for (index = 0; index < total; index++) {
        fn_8017B4BC(fileHandle, index);
        if (fn_8017B5A4() == 9) {
            count++;
        }
    }
    return count;
}

#pragma peephole off
s32 fn_8004E180(u8* context, u8* object)
{
    u8* attachmentState;
    u32 fileHandle;
    s32 count;
    s32 current;
    s32 index;
    s32 selectedWidth;
    s32 normalWidth;
    s32 x;

    attachmentState = *(u8**)(context + 0x60);
    current = **(s32**)(attachmentState + 8);
    count = pdaMailCountAttachItems((s32*)attachmentState);
    if (count <= 0) {
        return 0;
    }

    selectedWidth = GSmsgGetRect(0x36CE) >> 16;
    normalWidth = GSmsgGetRect(0x36CF) >> 16;
    x = *(s16*)(object + 0x54) / 2 -
        (selectedWidth + normalWidth * (count - 1)) / 2;
    for (index = 0; index < count; index++) {
        if (index == current) {
            fn_800FB680(x, 0, 0xE6AA00FF, 0x36CE);
            x += selectedWidth;
        } else {
            fn_800FB680(x, 0, 0xAAAAAAFF, 0x36CF);
            x += normalWidth;
        }
    }
    return 0;
}
#pragma peephole reset

#pragma peephole off
s32 fn_8004E2E0(u8* context, u8* object)
{
    u8* attachmentState;
    u32 fileHandle;
    s32 count;

    attachmentState = *(u8**)(context + 0x60);
    count = pdaMailCountAttachItems((s32*)attachmentState);

    if (count < 2) {
        winSpriteSetDisp(object, 0);
    } else {
        winSpriteSetDisp(object, 1);
    }
    if (*(s16*)(object + 6) == 0x507) {
        *(s16*)(object + 0x50) =
            lbl_8047BE38 * **(f32**)attachmentState +
            (f32)*(s32*)(attachmentState + 0xC);
    } else {
        *(s16*)(object + 0x50) =
            lbl_8047BE3C * **(f32**)attachmentState +
            (f32)*(s32*)(attachmentState + 0x10);
    }
    return 0;
}
#pragma peephole reset

extern u8 fn_8017B07C(u32 fileHandle, u32 entry);
extern void gfxSetFlags(u32 mask);
extern void gfxClearFlags(u32 value);
extern void gfxLoadTexture(u32 slot, void* texture);
extern void gfxSetPrimitive(u32 mode);
extern void gfxBegin(u32 count);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5BA0(u32 slot, s32 value);
extern void gfxVtxTexCoord(u32 slot, f32 x, f32 y);
extern void gfxEnd(void);
extern f32 lbl_8047BE48;
extern f32 lbl_8047BE4C;

/* The file entry of the selected attachment, or -1. */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_8004E510) — see docs/RULE_EXCEPTIONS.md */
static inline u32 pdaMailGetAttachEntry(s32* state, s32 selected)
{
    s32 index;
    s32 i;
    u32 entry;
    s32 found;
    s32 count;
    u32 fileHandle;

    index = state[1];
    count = pdaMailCountAttachItems(state);
    if (count <= selected) {
        return -1;
    }
    fileHandle = mailGetAttachFileGroup(index);
    found = 0;
    for (i = 0; i < count; i++) {
        entry = fn_8017B4BC(fileHandle, i);
        if (fn_8017B5A4() == 9) {
            if (found >= selected) {
                break;
            }
            found++;
        }
    }
    return entry;
}

#pragma peephole off
s32 fn_8004E510(u8* context, u8* object)
{
    /* RULE-EXCEPTION(user-approved): codegen-only local for the selected attachment, read before the entry lookup - see docs/RULE_EXCEPTIONS.md */
    s32 selected;
    u8* attachmentState;
    u32 fileHandle;
    u32 entry;
    u32 texture;

    attachmentState = *(u8**)(context + 0x60);
    if (pdaMailCountAttachItems((s32*)attachmentState) <= 0) {
        return 0;
    }

    selected = **(s32**)(attachmentState + 8);
    entry = pdaMailGetAttachEntry((s32*)attachmentState, selected);
    if (entry == -1) {
        return 0;
    }
    fileHandle = mailGetAttachFileGroup(*(s32*)(attachmentState + 4));
    if (fileHandle == 0) {
        return 0;
    }
    if (fn_8017B07C(fileHandle, entry) == 0) {
        return 0;
    }

    texture = (u32)fn_800F92D4(entry);
    if (texture == 0) {
        return 0;
    }
    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxLoadTexture(0, (void*)texture);
    gfxSetPrimitive(7);
    gfxBegin(2);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, -1);
    gfxVtxTexCoord(0, lbl_8047BE48, lbl_8047BE48);
    fn_800D61E4(*(s16*)(object + 0x54), *(s16*)(object + 0x56));
    fn_800D5BA0(0, -1);
    gfxVtxTexCoord(0, lbl_8047BE4C, lbl_8047BE4C);
    gfxEnd();
    return 0;
}
#pragma peephole reset

#endif /* MENU_PDA_MAIL_PARTIAL */

#if !defined(MENU_PDA_MAIL_PARTIAL) || defined(MENU_PDA_MAIL_SORT_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_8004C120(void)
{
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
    extern void fn_800E24B0(u16);
    extern void fn_800E209C(u16);
    extern s32 fn_8004D34C(s32);
    extern s32 fn_8004D9C0(s32);
    s32 ascIndex;
    s32 alphaCount;
    u16* recentCursor;
    s32 recentIndex;
    s32 alphaIndex;
    s32 recentCount;
    u16* alphaCursor;
    s32 descIndex;
    s32 result;
    s32 count;
    u16* output;
    s32 selection;
    u16 allocation;

    selection = 0;
    count = mailGetNbMailInMailbox();
    if (count > 0) {
        allocation = _toolentryAlloc__FUl(count * sizeof(u16));
        lbl_8047A500 = fn_800E27B0(allocation);
        output = lbl_8047A500;

        switch (mailGetSortMode()) {
        case 1:
            for (ascIndex = 0; ascIndex < mailGetNbMailInMailbox(); ascIndex++) {
                *output++ = mailGetMailIDInMailbox(ascIndex);
            }
            break;
        case 2:
            recentCount = mailGetNbMailInMailbox();
            recentCursor = output;
            for (recentIndex = 0; recentIndex < recentCount; recentIndex++) {
                *recentCursor++ = mailGetMailIDInMailbox(recentIndex);
            }
            qsort(output, recentCount, sizeof(u16),
                  (s32 (*)(const void*, const void*))fn_8004BF20);
            break;
        case 3:
            alphaCount = mailGetNbMailInMailbox();
            alphaCursor = output;
            for (alphaIndex = 0; alphaIndex < alphaCount; alphaIndex++) {
                *alphaCursor++ = mailGetMailIDInMailbox(alphaIndex);
            }
            qsort(output, alphaCount, sizeof(u16),
                  (s32 (*)(const void*, const void*))fn_8004BE90);
            break;
        case 0:
        default:
            for (descIndex = mailGetNbMailInMailbox() - 1; descIndex >= 0; descIndex--) {
                *output++ = mailGetMailIDInMailbox(descIndex);
            }
            break;
        }
    } else {
        lbl_8047A500 = NULL;
    }

    while (1) {
        result = fn_8004D34C(selection);
        if (result < 0) {
            break;
        }
        selection = fn_8004D9C0(result);
    }

    if (count > 0) {
        fn_800E24B0(allocation);
        fn_800E209C(allocation);
    }
}
#pragma peephole reset
#endif /* MENU_PDA_MAIL_SORT_ONLY */
