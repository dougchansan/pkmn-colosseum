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
extern void fn_80166AB8(s32 soundId, s32 arg1, s32 arg2);
extern void cameraWaitSyncAnime(s32 camera);
extern void fn_8004A47C(void);
extern void fn_8003C7C0(void);
extern void fn_80044630(void);
extern u8 fn_801902E0(s32 id);
extern void fn_8004C120(void);
extern void fadeSet(s32 type, f32 speed);
extern s8 fadeCheck(s32 type);
extern void fn_800FF660(void);
extern void floorSetFadeScript(s32 a, u32 b);
extern void _threadSwitch(void);

typedef struct PdaMailAttachmentConfig {
    f32* scroll;
    s32 mailId;
    s32* status;
    s32 y;
    s32 x;
} PdaMailAttachmentConfig;

#pragma peephole off
void fn_8004B7EC(void)
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
    fn_80166AB8(0x448, 0, 0);
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

    config.first = 0;
    config.second = 0xFF;
    config.third = 0;
    config.message = &lbl_8047A4FC;
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
            phase = state->selection;
        }
        state->selection = (s8) phase;
        state->menuBusy = 1;

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
            if (!fn_801902E0(0x3F0)) {
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

#if 0
asm u8 fn_8004BDEC(void) {
#include "src/game/menu/menu_pda_mail_fn_8004BDEC.inc"
}


#else
u8 fn_8004BDEC(void)
{
    return lbl_803A6A60[0];
}
#endif

#if 0
asm u8 fn_8004BDFC(void) {
#include "src/game/menu/menu_pda_mail_fn_8004BDFC.inc"
}
#else
u8 fn_8004BDFC(void)
{
    return lbl_803A6A60[1];
}
#endif

#if 0
asm void fn_8004BDB8(s8 a, s8 b) {
#include "src/game/menu/menu_pda_mail_fn_8004BDB8.inc"
}
#else
#pragma peephole off
void fn_8004BDB8(s8 a, s8 b)
{
    if (a >= 0) {
        *(s8*) &lbl_803A6A60[1] = a;
    }
    if (b < 0) {
        return;
    }
    *(s8*) &lbl_803A6A60[1] = b;
}
#pragma peephole reset
#endif

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

#if 0
asm void fn_8004E89C(void* widget) {
#include "src/game/menu/menu_pda_mail_fn_8004E89C.inc"
}
#else
#pragma peephole off
void fn_8004E89C(void* widget)
{
    u8* state = windowGetKeyInfo();
    if (!(*(u16*) state & 0x10)) {
        menuButtonNormal(widget);
    }
}
#pragma peephole reset
#endif

/* winSpriteSetDisp (gs_worldmap.c): set a window-sprite field-handle's
 * display/visibility value. */
extern void winSpriteSetDisp(void* fieldHandle, s32 value);

typedef struct PdaMailWindowB {
    u8 pad00[0x60];
    s32* field_0x60;
} PdaMailWindowB;

#pragma peephole off
s32 fn_8004E440(PdaMailWindowB* window, void* fieldHandle)
{
    extern u32 mailGetAttachFileGroup(s32 index);
    extern s32 fn_8017B2CC(u32 fileHandle);
    extern s32 fn_8017B448(u32 fileHandle);
    extern u32 fn_8017B4BC(u32 fileHandle, u32 index);
    extern u32 fn_8017B5A4();
    s32 total;
    s32 count;
    s32 index;
    u32 object;
    s32 loaded;

    index = window->field_0x60[1];
    if (fn_8017B2CC(mailGetAttachFileGroup(index)) == 1) {
        loaded = 0;
    } else {
        loaded = 1;
    }
    if (loaded == 0) {
        count = -1;
    } else {
        object = mailGetAttachFileGroup(index);
        total = fn_8017B448(object);
        index = count = 0;
        while (index < total) {
            fn_8017B4BC(object, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            index++;
        }
    }
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

#pragma peephole off
s32 fn_8004E790(PdaMailAttachWindow* window)
{
    extern u32 mailGetAttachFileGroup(s32 index);
    extern s32 fn_8017B2CC(u32 fileHandle);
    extern s32 fn_8017B448(u32 fileHandle);
    extern u32 fn_8017B4BC(u32 fileHandle, u32 index);
    extern u32 fn_8017B5A4();
    u8* input;
    s32 total;
    s32 count;
    s32 index;
    u32 object;
    PdaMailAttachState* state;
    s32 loaded;
    s32 selection;

    state = window->state;
    input = windowGetKeyInfo();
    selection = *state->selection;
    index = state->mailIndex;
    count = mailGetAttachFileGroup(index);
    if (fn_8017B2CC(count) == 1) {
        loaded = 0;
    } else {
        loaded = 1;
    }
    if (loaded == 0) {
        count = -1;
    } else {
        object = mailGetAttachFileGroup(index);
        total = fn_8017B448(object);
        count = 0;
        index = count;
        while (index < total) {
            fn_8017B4BC(object, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
            index++;
        }
    }
    if (count < 2) {
        return 0;
    }
    if ((*(u16*) (input + 6) & 8) != 0) {
        selection++;
        if (selection >= count) {
            selection = 0;
        }
    }
    if ((*(u16*) (input + 6) & 4) != 0) {
        selection--;
        if (selection < 0) {
            selection = count - 1;
        }
    }
    *state->selection = selection;
    return 0;
}
#pragma peephole reset

#if 0
asm s32 fn_8004DB34(PdaMailWindowB* window, void* fieldHandle) {
#include "src/game/menu/menu_pda_mail_fn_8004DB34.inc"
}
#else
#pragma scheduling off
s32 fn_8004DB34(PdaMailWindowB* window, void* fieldHandle)
{
    if (*window->field_0x60 != 0) {
        winSpriteSetDisp(fieldHandle, 0);
    } else {
        winSpriteSetDisp(fieldHandle, 1);
    }
    return 0;
}
#pragma scheduling reset
#endif

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

#pragma peephole off
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
#pragma peephole reset

#pragma peephole off
#pragma scheduling off
s32 fn_8004DA64(PdaMailWindowC* window, PdaMailSpriteField* field)
{
    s32* statePtr = window->field_0x60;
    const s32* values = lbl_802672D8;
    s32 state = *statePtr;
    s32 value0 = values[0];
    s32 value1 = values[1];
    s32 value2 = values[2];
    s32 value3 = values[3];
    s32 value4 = values[4];
    s32 i;
    u8 visible;

    if (state != 0) {
        visible = 0;
    } else {
        i = 0;
        if (field->msgId != value0) {
            i = 1;
            if (field->msgId != value1) {
                i = 2;
                if (field->msgId != value2) {
                    i = 3;
                    if (field->msgId != value3) {
                        i = 4;
                        if (field->msgId != value4) {
                            i = 5;
                        }
                    }
                }
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
#pragma scheduling reset
#pragma peephole reset

/* mailGetAttachFileGroup (battle_waza.c): "Waza entry get field 0x18 by index". */
extern u32 mailGetAttachFileGroup(s32 idx);

#if 0
asm s32 fn_8004D5EC(PdaMailWindowA* window, void* fieldHandle) {
#include "src/game/menu/menu_pda_mail_fn_8004D5EC.inc"
}
#else
#pragma peephole off
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
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004D64C(PdaMailWindowA* window, void* fieldHandle) {
#include "src/game/menu/menu_pda_mail_fn_8004D64C.inc"
}
#else
#pragma peephole off
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
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004D590(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D590.inc"
}
#else
#pragma peephole off
s32 fn_8004D590(PdaMailWindowA* window, PdaMailOutA* out)
{
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0) {
        out->field_0x4c = 0x36B9;
    } else {
        out->field_0x4c = 0;
    }
    return 0;
}
#pragma peephole reset
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
#pragma peephole off
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
#pragma peephole reset
#endif

/* mailGetSubject (battle_waza.c): "Waza entry get field 0x10 by index". */
extern u32 mailGetSubject(s32 idx);

#if 0
asm s32 fn_8004D760(PdaMailWindowA* window, PdaMailOutA* out) {
#include "src/game/menu/menu_pda_mail_fn_8004D760.inc"
}
#else
#pragma peephole off
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
#pragma peephole reset
#endif

#if 0
asm void fn_8004D8BC(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004D8BC.inc"
}
#else
#pragma peephole off
void fn_8004D8BC(PdaMailWindowA* window)
{
    u8* state = windowGetKeyInfo();
    if (mailGetAttachFileGroup(pdaMailGetMailID(**window->field_0x60)) != 0
        || (*(u16*) state & 0x10) == 0) {
        menuButtonNormal(window);
    }
}
#pragma peephole reset
#endif

/* fn_800FF730 (gs_floor_data.c): floor-transition trigger; floorSetFadeScript
 * (gs_field_colquery.c, GSfield_IsTransitioning): floor resource-alloc
 * helper; _threadSwitch: cooperative thread yield. */
extern void fn_800FF730(s32 floorId);
extern void floorSetFadeScript(s32 a, u32 b);
extern void _threadSwitch(void);

#if 0
asm void menuPdaOpen(void) {
#include "src/game/menu/menu_pda_mail_menuPdaOpen.inc"
}
#else
#pragma scheduling off
void menuPdaOpen(void)
{
    fn_800FF730(0x392);
    floorSetFadeScript(0, 0);
    _threadSwitch();
}
#pragma scheduling reset
#endif

/* mailGetNbMailInMailbox (battle_waza.c): Waza-party active-effect count getter. */
extern s32 mailGetNbMailInMailbox(void);

/* Mail-ID lookup table (halfword mail IDs), indexed by receive-order
 * slot; sda21-addressed pointer variable (matches XD's pdaMailGetMailID
 * exactly per file header). */
extern u16* lbl_8047A500;

#if 0
asm s32 pdaMailGetMailID(s32 index) {
#include "src/game/menu/menu_pda_mail_pdaMailGetMailID.inc"
}
#else
/* dont_inline: every earlier call site in this TU only sees the forward
 * `extern` prototype (this definition comes after them in file order),
 * so they naturally keep a real `bl pdaMailGetMailID`. Call sites placed
 * AFTER this definition (e.g. fn_8004D9C0) would otherwise get auto-
 * inlined by -inline; retail keeps a real call there too, so pin it. */
#pragma peephole off
#pragma dont_inline on
s32 pdaMailGetMailID(s32 index)
{
    extern s32 mailGetNbMailInMailbox(void);
    if (index < 0 || index >= mailGetNbMailInMailbox()) {
        return -1;
    }
    return lbl_8047A500[index];
}
#pragma dont_inline reset
#pragma peephole reset
#endif

/* Mail-list cursor input callback. The high byte of cursorPosition is the
 * mailbox page and the low byte is the row (10 and 11 are auxiliary rows). */
#pragma scheduling on
#pragma peephole off
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
#pragma peephole reset
#pragma scheduling reset

/* mailGetReceiveNumber (XD-named, same address/size): returns the
 * receive-order slot for a given mail ID, or -1 if not found. */
extern s32 mailGetReceiveNumber(s32 mailId);

/* GScharCmp (menuCB_Battle.c): compares two rendered-message buffers. */
extern s32 GScharCmp(void* a, void* b);

#if 0
asm s32 fn_8004BE90(u16* a, u16* b) {
#include "src/game/menu/menu_pda_mail_fn_8004BE90.inc"
}
#else
s32 fn_8004BE90(u16* a, u16* b)
{
    s32 idA = *a;
    s32 idB = *b;
    s32 new_var2;
    s32 cmp;
    s32* new_var;
    void* msgA = GSmsgGetGSchar(mailGetSubject(idA));
    void* msgB = GSmsgGetGSchar(mailGetSubject(idB));
    cmp = GScharCmp(msgA, msgB);
    if (cmp != 0) {
        new_var = &cmp;
        return *new_var;
    }
    new_var2 = mailGetReceiveNumber(idA);
    return mailGetReceiveNumber(idB) - new_var2;
}
#endif

#if 0
asm s32 fn_8004BF20(u16* a, u16* b) {
#include "src/game/menu/menu_pda_mail_fn_8004BF20.inc"
}
#else
s32 fn_8004BF20(u16* a, u16* b)
{
    s32 idA = *a;
    s32 idB = *b;
    s32 cmp;
    void* msgA = GSmsgGetGSchar(mailGetSenderName(idA));
    s32 new_var;
    void* msgB = GSmsgGetGSchar(mailGetSenderName(idB));
    cmp = GScharCmp(msgA, msgB);
    if (cmp != 0) {
        return cmp;
    }
    new_var = mailGetReceiveNumber(idA);
    return mailGetReceiveNumber(idB) - new_var;
}
#endif

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
#pragma peephole off
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
#pragma scheduling reset
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004DB80(PdaMailPhaseWidget* w) {
#include "src/game/menu/menu_pda_mail_fn_8004DB80.inc"
}
#else
#pragma peephole off
s32 fn_8004DB80(PdaMailPhaseWidget* w)
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
#pragma scheduling reset
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004DF34(PdaMailPhaseWidget* w) {
#include "src/game/menu/menu_pda_mail_fn_8004DF34.inc"
}
#else
#pragma peephole off
s32 fn_8004DF34(PdaMailPhaseWidget* w)
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
#pragma scheduling reset
#pragma peephole reset
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
/* WALL: W1 register-letter (f1/f2 swap between the float value temp
 * and the threshold local, same class as fn_8004E144) -- compound
 * assignment-in-condition, split statements, and swapping which
 * constant gets a named local all tried; switch dispatch/case layout,
 * SE-call bodies, and store-vs-compare ordering all byte-match.
 * Parked at 99.6% after 3 attempts. */
#pragma peephole off
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
        f32 thresh = lbl_8047BE1C;
        if ((*(f32*)*field += lbl_8047BE18) >= thresh) {
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
#pragma scheduling reset
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004E8E0(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004E8E0.inc"
}
#else
/* WALL: same class as fn_8004D26C (W1 register-letter f1/f2 swap).
 * Parked at 99.6% (see fn_8004D26C for attempts). */
#pragma peephole off
s32 fn_8004E8E0(PdaMailWindowA* window)
{
    s32** field = window->field_0x60;
    switch (window->phase) {
    case 0:
        if (window->guard == 0) {
            winSeqSetMenu(0x77, 0x86);
            window->guard = 1;
        }
        break;
    case 2: {
        f32 thresh = lbl_8047BE4C;
        if ((*(f32*)*field += lbl_8047BE50) >= thresh) {
            *(f32*)*field -= thresh;
        }
        break;
    }
    case 3:
        if (window->guard == 0) {
            winSeqSetMenu(0x77, 0x8a);
            window->guard = 1;
        }
        break;
    }
    return 0;
}
#pragma scheduling reset
#pragma peephole reset
#endif

/* windowGetActiveID/menuOpenCustom/menuClose/menuCloseSync (gs_event_exec.c):
 * modal list-menu open/poll/close idiom -- same call skeleton as the
 * gs_event_exec.c item-quantity-picker (menu_id, input-state,
 * &config, 0, 1, 1, &out), open by id, close by id. */
extern u32 windowGetActiveID(void);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuClose(s32 menuId);
extern void menuCloseSync(s32 menuId, s32 flag);

/* mailGetSortMode (battle_waza.c): Waza party mailbox-sort-mode byte
 * getter (0=default/none, 1=ascending, 2=ascending+recent-sort,
 * 3=ascending+alpha-sort). mailGetMailIDInMailbox (battle_waza.c):
 * mail ID by receive-order index. qsort: standard library sort. */
extern s32 mailGetSortMode(void);
extern s32 mailGetMailIDInMailbox(s32 idx);
extern void qsort(void* base, u32 count, u32 size,
                   s32 (*cmp)(const void*, const void*));

typedef struct PdaMailSortLabelWindow {
    u8 pad00[0x8b];
    u8 color;
} PdaMailSortLabelWindow;

#pragma push
#pragma peephole off
#pragma optimization_level 1
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
#pragma pop

extern const f32 lbl_8047BE08;
extern const f32 lbl_8047BE0C;

#pragma fp_contract on
#pragma optimization_level 4
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
            (s16) (lbl_8047BE08 * **(f32**) state + (f32) base);
    } else {
        s32 base = *(s32*) (state + 8);
        *(s16*) (field + 0x50) =
            (s16) (lbl_8047BE0C * **(f32**) state + (f32) base);
    }
    return 0;
}
#pragma peephole reset
#pragma fp_contract reset

#if 0
asm void fn_8004BFB0(void) {
#include "src/game/menu/menu_pda_mail_fn_8004BFB0.inc"
}
#else
/* WALL: W1 register-letter (retail keeps the mail-id buffer pointer in
 * a single register r31 shared across all 4 switch arms with no r28
 * companion; every source shape tried here -- shared top-level pointer,
 * per-case-local pointer, block-scoped locals -- allocates an extra
 * callee-saved register (r28) not present in target) + a redundant
 * clrlwi mask before each halfword store that the target elides.
 * Best reached 76.4% after 3 source-shape attempts. */
void fn_8004BFB0(void)
{
    extern s32 mailGetNbMailInMailbox(void);
    u16* buf = lbl_8047A500;
    u8 mode = (u8) mailGetSortMode();

    switch (mode) {
    case 1: {
        s32 i = 0;
        u16 count = (u16) mailGetNbMailInMailbox();
        for (; i < count; i++) {
            *buf++ = mailGetMailIDInMailbox(i);
        }
        break;
    }
    case 2: {
        u16 count = (u16) mailGetNbMailInMailbox();
        s32 i = 0;
        for (; i < count; i++) {
            *buf++ = mailGetMailIDInMailbox(i);
        }
        qsort(lbl_8047A500, count, 2, (void*) fn_8004BF20);
        break;
    }
    case 3: {
        u16 count = (u16) mailGetNbMailInMailbox();
        s32 i = 0;
        for (; i < count; i++) {
            *buf++ = mailGetMailIDInMailbox(i);
        }
        qsort(lbl_8047A500, count, 2, (void*) fn_8004BE90);
        break;
    }
    default: {
        s32 i;
        for (i = mailGetNbMailInMailbox() - 1; i >= 0; i--) {
            *buf++ = mailGetMailIDInMailbox(i);
        }
        break;
    }
    }
}
#endif

#if 0
asm s32 fn_8004DC18(s32 a) {
#include "src/game/menu/menu_pda_mail_fn_8004DC18.inc"
}
#else
#pragma peephole off
s32 fn_8004DC18(s32 a)
{
    s32 out = 0;
    s32 choice = menuOpenCustom(0x75, windowGetActiveID(), &a, 0, 1, 1, &out);
    if (choice != -1 && choice != a) {
        out = 1;
    }
    menuClose(0x75);
    menuCloseSync(0x75, 1);
    if (choice < 0 || choice >= 4) {
        return -1;
    }
    return choice;
}
#pragma peephole reset
#endif

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
#pragma peephole off
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
#pragma peephole reset
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

#if 0
asm s32 fn_8004DCC0(void* unused, PdaMailOutC* window) {
#include "src/game/menu/menu_pda_mail_fn_8004DCC0.inc"
}
#else
#pragma peephole off
s32 fn_8004DCC0(void* unused, PdaMailOutC* window)
{
    s32 table[11];
    s32 i;

    table[0] = (s32) lbl_802672F0[0];
    table[1] = (s32) lbl_802672F0[1];
    table[2] = (s32) lbl_802672F0[2];
    table[3] = (s32) lbl_802672F0[3];
    table[4] = (s32) lbl_802672F0[4];
    table[5] = (s32) lbl_802672F0[5];
    table[6] = (s32) lbl_802672F0[6];
    table[7] = (s32) lbl_802672F0[7];
    table[8] = (s32) lbl_802672F0[8];
    table[9] = (s32) lbl_802672F0[9];
    table[10] = (s32) lbl_802672F0[10];

    for (i = 0; i < 11; i++) {
        if (window->msgId == table[i]) {
            break;
        }
    }
    if (i >= 11) {
        return 0;
    }
    {
        u32 result = fn_801D1620((u8) i);
        if (result != 0) {
            window->field_0x4c = result;
        } else {
            window->field_0x4c = 0x36CD;
        }
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80166A50 (gs_event_exec.c/gs_title.c convention): plays an SE by
 * (id, a, b, c). fn_801D1B78/fn_801D1C20/fn_801D228C (battle_waza.c):
 * despite their current unverified shapes there (2.4%/2.4%/1.3% match
 * -- those bodies are still stubs), this call site's actual arg count
 * (1 each) is what matters for our own byte match. */
extern void fn_80166A50(s32 id, s32 a, s32 b, s32 c);
extern s32 fn_801D1B78(s32 mailId);
extern void fn_801D1C20(s32 mailId);
extern void fn_801D228C(u16 mailId);

#if 0
asm s32 fn_8004D7D0(PdaMailWindowA* window) {
#include "src/game/menu/menu_pda_mail_fn_8004D7D0.inc"
}
#else
#pragma peephole off
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
        fn_80166A50(0x23, 0, 0xFF, 0);
        **field = index;
    }
    mailId = pdaMailGetMailID(index);
    if (fn_801D1B78(mailId) == 0) {
        fn_801D1C20(mailId);
        fn_801D228C((u16) mailId);
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_800FB680/GSmsgGetRect (gs_title.c-family text helpers): draw a
 * message at (x,y,color,msgId) / measure a message's rendered width
 * (packed into the high halfword of the return value). */
extern void fn_800FB680(s32 x, s32 y, u32 color, s32 msgId);
extern u32 GSmsgGetRect(s32 msgId);

#if 0
asm s32 fn_8004C2D8(void* ctx, void* p) {
#include "src/game/menu/menu_pda_mail_fn_8004C2D8.inc"
}
#else
#pragma peephole off
s32 fn_8004C2D8(u8* ctx, u8* p)
{
    extern s32 mailGetNbMailInMailbox(void);
    s32 count;
    s32 pages;
    u32 color = (u32) ctx[0x8b] | 0xe66e0000u;
    count = mailGetNbMailInMailbox();
    if ((pages = (count + 9) / 10) <= 0) {
        pages = 1;
    }
    msgctrlSetValue(0x34, (void*) pages);
    fn_800FB680(*(s16*) (p + 0x54) - (s32) (GSmsgGetRect(0xca) >> 16), 0, color, 0xca);
    return 0;
}
#pragma peephole reset
#endif

#if 0
asm s32 fn_8004C36C(void* ctx, void* p) {
#include "src/game/menu/menu_pda_mail_fn_8004C36C.inc"
}
#else
#pragma peephole off
s32 fn_8004C36C(u8* ctx, u8* p)
{
    u32 color = (u32) ctx[0x8b] | 0xe66e0000u;
    msgctrlSetValue(0x34, (void*) ((s8) ctx[0x94] + 1));
    fn_800FB680(*(s16*) (p + 0x54) - (s32) (GSmsgGetRect(0xca) >> 16), 0, color, 0xca);
    return 0;
}
#pragma peephole reset
#endif

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

    fn_8017B3E4(group);
    config.mailId = mailId;
    config.status = &lbl_8047A538;
    config.y = lbl_8047A534;
    config.x = lbl_8047A530;
    lbl_8047A53C = lbl_8047BE48;
    config.scroll = &lbl_8047A53C;
    menuOpenCustom(0x77, windowGetActiveID(), 0, 0, 1, 1, &config);
    while (fn_8017B2CC(group) == 1) {
        _threadSwitch();
    }
    menuClose(0x77);
    menuCloseSync(0x77, 1);
    fn_8017B1CC(group);
    fn_800F915C(group);
}

extern u32 fn_80103E68(u32 id);
extern s32 fn_8004BE40(s32 index);
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

static inline s32 pdaMailFindTableEntry(const s32* table, s32 value)
{
    if (value == table[0]) {
        return 0;
    } else if (value == table[1]) {
        return 1;
    } else if (value == table[2]) {
        return 2;
    } else if (value == table[3]) {
        return 3;
    } else if (value == table[4]) {
        return 4;
    } else if (value == table[5]) {
        return 5;
    } else if (value == table[6]) {
        return 6;
    } else if (value == table[7]) {
        return 7;
    } else if (value == table[8]) {
        return 8;
    } else if (value == table[9]) {
        return 9;
    }
    return 10;
}

u32 fn_8004C6C0(u8* context, u8* object)
{
    s32 table[10];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    u16 page;
    u32 i;

    for (i = 0; i < 10; i++) {
        table[i] = lbl_802672A0[i];
    }
    page = fn_80103E68(10) >> 16;
    value = *(s16*)(object + 6);
    index = pdaMailFindTableEntry(table, value);
    if (index >= 10) {
        return 0;
    }
    index += (s8)(page >> 8) * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSenderName(mailId);
    if (message != 0) {
        msgctrlSetValue(0x37, GSmsgGetGSchar(message));
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
    s32 table[10];
    s32 index;
    s32 mailId;
    s32 value;
    u32 message;
    u16 page;
    u32 i;

    for (i = 0; i < 10; i++) {
        table[i] = lbl_80267278[i];
    }
    page = fn_80103E68(10) >> 16;
    value = *(s16*)(object + 6);
    index = pdaMailFindTableEntry(table, value);
    if (index >= 10) {
        return 0;
    }
    index += (s8)(page >> 8) * 10;
    mailId = pdaMailGetMailID(index);
    message = mailGetSubject(mailId);
    if (message != 0) {
        msgctrlSetValue(0x37, GSmsgGetGSchar(message));
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
    s32 table[10];
    s32 index;
    s32 mailId;
    s32 value;
    u16 page;
    u32 i;
    u8 visible;

    for (i = 0; i < 10; i++) {
        table[i] = lbl_80267250[i];
    }
    page = fn_80103E68(10) >> 16;
    value = *(s16*)(object + 6);
    index = pdaMailFindTableEntry(table, value);
    if (index >= 10) {
        return 0;
    }
    index += (s8)(page >> 8) * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId < 0) {
        visible = 0;
    } else {
        visible = mailGetAttachFileGroup(mailId) != 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 fn_8004CC38(u8* context, u8* object)
{
    s32 table[10];
    s32 index;
    s32 mailId;
    s32 value;
    u16 page;
    u32 i;
    u8 visible;

    for (i = 0; i < 10; i++) {
        table[i] = lbl_80267228[i];
    }
    page = fn_80103E68(10) >> 16;
    value = *(s16*)(object + 6);
    index = pdaMailFindTableEntry(table, value);
    if (index >= 10) {
        return 0;
    }
    index += (s8)(page >> 8) * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId < 0) {
        visible = 0;
    } else {
        visible = fn_801D1B78(mailId) != 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

u32 fn_8004CDD8(u8* context, u8* object)
{
    s32 table[10];
    s32 index;
    s32 mailId;
    s32 value;
    u16 page;
    u32 i;
    u8 visible;

    for (i = 0; i < 10; i++) {
        table[i] = lbl_80267200[i];
    }
    page = fn_80103E68(10) >> 16;
    value = *(s16*)(object + 6);
    index = pdaMailFindTableEntry(table, value);
    if (index >= 10) {
        return 0;
    }
    index += (s8)(page >> 8) * 10;
    mailId = pdaMailGetMailID(index);
    if (mailId < 0) {
        visible = 0;
    } else {
        visible = fn_801D1B78(mailId) == 0;
    }
    winSpriteSetDisp(object, visible);
    return 0;
}

extern u8* fn_80105624(void);
extern u32 fn_801D1650(u32 index);
extern void fn_801666BC(u32 id);
extern void fn_80166B18(u32 id);
extern void fn_801654E0(u32 id, u32 buffer, u32 size);
extern void fn_80166B3C(u32 id, u32 arg1, u32 arg2);
extern s32 fn_801D1618(void);
extern void fn_801669E4(u32 id, u32 arg1, u32 arg2);
extern s32 lbl_8047A520;
extern u8 lbl_8047A524;
extern u32 lbl_8047A528;
extern u32 lbl_8047A52C;

u32 fn_8004DDC0(u8* context)
{
    u8* input;
    u32 soundId;
    s32 limit;
    s8 selection;

    input = fn_80105624();
    if (lbl_8047A520 != 0) {
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            fn_801666BC(soundId);
        }
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            fn_80166B18(soundId);
        }
        lbl_8047A524 = context[0x95];
        soundId = fn_801D1650(context[0x95]);
        if (soundId != 0) {
            fn_801654E0(soundId, lbl_8047A52C, 0x10000);
            fn_80166B3C(soundId, 0, 0x408);
            fn_80166A50(soundId, 0, 0xFF, 0);
            lbl_8047A528 = 0;
        }
        lbl_8047A520 = 0;
    }

    if ((*(u16*)(input + 4) & 2) != 0) {
        limit = fn_801D1618();
        selection = context[0x95] + 1;
        context[0x95] = selection;
        if (selection >= limit + 1) {
            context[0x95] = limit;
        }
    }
    if ((*(u16*)(input + 4) & 1) != 0) {
        selection = context[0x95] - 1;
        context[0x95] = selection;
        if (selection < 0) {
            context[0x95] = 0;
        }
    }
    if (lbl_8047A524 != (s8)context[0x95]) {
        soundId = fn_801D1650(lbl_8047A524);
        if (soundId != 0) {
            fn_801669E4(soundId, 0, 0);
        }
        lbl_8047A520 = 1;
    }
    return 0;
}

extern u32 fn_8016557C(void);
extern u32 fn_800F9418(u32 size, u32 align, u32 arg2, u32 group, u32 arg4);
extern void fn_800F9378(u32 buffer, u32 arg1, u32 group, u32 arg3);
extern void fn_800F9210(u32 arg0, u32 group);
extern void fn_80165548(u32 state);
extern s32 fn_801026A4(u32 menuId, ...);
extern u32 fn_801046B8(void);
extern void fn_80102510(u32 menuId);

u8 fn_8004DFCC(u8 initialSelection)
{
    u32 selection;
    u32 state;
    u32 soundId;
    s32 result;

    selection = initialSelection;
    state = fn_8016557C();
    lbl_8047A52C = fn_800F9418(0x10000, 0x20, 0, 0x408, 0);
    fn_800F9378(lbl_8047A52C, 0, 0x408, 0);
    lbl_8047A524 = initialSelection;
    soundId = fn_801D1650(initialSelection);
    if (soundId != 0) {
        fn_801654E0(soundId, lbl_8047A52C, 0x10000);
        fn_80166B3C(soundId, 0, 0x408);
        fn_80166A50(soundId, 0, 0xFF, 0);
        lbl_8047A528 = 0;
    }

    result = fn_801026A4(0x76, fn_801046B8(), &selection, 0, 1, 0);
    if (result < 0 || result >= fn_801D1618()) {
        result = 0xFF;
    }
    fn_80102510(0x76);
    menuCloseSync(0x76, 1);

    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        fn_801669E4(soundId, 0, 0);
    }
    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        fn_801666BC(soundId);
    }
    soundId = fn_801D1650(lbl_8047A524);
    if (soundId != 0) {
        fn_80166B18(soundId);
    }
    fn_800F9210(0, 0x408);
    fn_80165548(state);
    return result;
}

extern u32 mailGetAttachFileGroup(s32 index);
extern s32 fn_8017B2CC(u32 fileHandle);
extern s32 fn_8017B448(u32 fileHandle);
extern u32 fn_8017B4BC(u32 fileHandle, u32 index);
extern u32 fn_8017B5A4(void);
extern u32 GSmsgGetRect(s32 messageId);
extern const f32 lbl_8047BE38;
extern const f32 lbl_8047BE3C;

s32 fn_8004E180(u8* context, u8* object)
{
    u8* attachmentState;
    u32 fileHandle;
    s32 count;
    s32 current;
    s32 total;
    s32 index;
    s16 selectedWidth;
    s16 normalWidth;
    s32 x;

    attachmentState = *(u8**)(context + 0x60);
    count = *(u32*)(attachmentState + 4);
    current = **(s32**)(attachmentState + 8);
    fileHandle = mailGetAttachFileGroup(count);
    if (fn_8017B2CC(fileHandle) == 1) {
        count = -1;
    } else {
        total = fn_8017B448(fileHandle);
        count = 0;
        for (index = 0; index < total; index++) {
            fn_8017B4BC(fileHandle, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
        }
    }
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

s32 fn_8004E2E0(u8* context, u8* object)
{
    u8* attachmentState;
    u32 fileHandle;
    s32 count;
    s32 total;
    s32 index;

    attachmentState = *(u8**)(context + 0x60);
    count = *(u32*)(attachmentState + 4);
    fileHandle = mailGetAttachFileGroup(count);
    if (fn_8017B2CC(fileHandle) == 1) {
        count = -1;
    } else {
        total = fn_8017B448(fileHandle);
        count = 0;
        for (index = 0; index < total; index++) {
            fn_8017B4BC(fileHandle, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
        }
    }

    fn_80109220((u32)object, count >= 2);
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

extern u8 fn_8017B07C(u32 fileHandle, u32 entry);
extern void fn_800D88DC(u32 mask);
extern void fn_800D888C(u32 value);
extern void fn_800D85D4(u32 slot, void* texture);
extern void fn_800D6A00(u32 mode);
extern void fn_800D67BC(u32 count);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5BA0(u32 slot, s32 value);
extern void fn_800D59B8(u32 slot, f32 x, f32 y);
extern void fn_800D6728(void);
extern f32 lbl_8047BE48;
extern f32 lbl_8047BE4C;

s32 fn_8004E510(u8* context, u8* object)
{
    u8* attachmentState;
    u32 fileHandle;
    u32 entry;
    u32 texture;
    u32 selected;
    s32 count;
    s32 total;
    s32 index;

    attachmentState = *(u8**)(context + 0x60);
    fileHandle = mailGetAttachFileGroup(*(u32*)(attachmentState + 4));
    if (fn_8017B2CC(fileHandle) == 1) {
        count = -1;
    } else {
        total = fn_8017B448(fileHandle);
        count = 0;
        for (index = 0; index < total; index++) {
            fn_8017B4BC(fileHandle, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
        }
    }
    if (count <= 0) {
        return 0;
    }

    selected = **(u32**)(attachmentState + 8);
    fileHandle = mailGetAttachFileGroup(*(u32*)(attachmentState + 4));
    if (fn_8017B2CC(fileHandle) == 1) {
        count = -1;
    } else {
        total = fn_8017B448(fileHandle);
        count = 0;
        for (index = 0; index < total; index++) {
            fn_8017B4BC(fileHandle, index);
            if (fn_8017B5A4() == 9) {
                count++;
            }
        }
    }
    if (count <= (s32)selected) {
        return 0;
    }

    entry = -1;
    count = 0;
    for (index = 0; index < total; index++) {
        entry = fn_8017B4BC(fileHandle, index);
        if (fn_8017B5A4() == 9) {
            if ((u32)count >= selected) {
                break;
            }
            count++;
        }
    }
    if (entry == -1 || fileHandle == 0 ||
        fn_8017B07C(fileHandle, entry) == 0) {
        return 0;
    }

    texture = (u32)fn_800F92D4(entry);
    if (texture == 0) {
        return 0;
    }
    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D85D4(0, (void*)texture);
    fn_800D6A00(7);
    fn_800D67BC(2);
    fn_800D61E4(0, 0);
    fn_800D5BA0(0, -1);
    fn_800D59B8(0, lbl_8047BE48, lbl_8047BE48);
    fn_800D61E4(*(s16*)(object + 0x54), *(s16*)(object + 0x56));
    fn_800D5BA0(0, -1);
    fn_800D59B8(0, lbl_8047BE4C, lbl_8047BE4C);
    fn_800D6728();
    return 0;
}

void fn_8004C120(void)
{
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
    extern void fn_800E24B0(u16);
    extern void fn_800E209C(u16);
    extern s32 fn_8004D34C(s32);
    s32 selection = 0;
    s32 count = mailGetNbMailInMailbox();
    u16 allocation = 0;
    u16* output;
    s32 i;

    if (count > 0) {
        allocation = _toolentryAlloc__FUl(count * sizeof(u16));
        lbl_8047A500 = fn_800E27B0(allocation);
        output = lbl_8047A500;

        switch (mailGetSortMode()) {
        case 1:
            for (i = 0; i < mailGetNbMailInMailbox(); i++) {
                *output++ = mailGetMailIDInMailbox(i);
            }
            break;
        case 2:
        {
            s32 sortCount = mailGetNbMailInMailbox();
            u16* sortedOutput = output;
            for (i = 0; i < sortCount; i++) {
                *sortedOutput++ = mailGetMailIDInMailbox(i);
            }
            qsort(output, sortCount, sizeof(u16),
                  (s32 (*)(const void*, const void*))fn_8004BF20);
            break;
        }
        case 3:
        {
            s32 sortCount = mailGetNbMailInMailbox();
            u16* sortedOutput = output;
            for (i = 0; i < sortCount; i++) {
                *sortedOutput++ = mailGetMailIDInMailbox(i);
            }
            qsort(output, sortCount, sizeof(u16),
                  (s32 (*)(const void*, const void*))fn_8004BE90);
            break;
        }
        default:
            for (i = mailGetNbMailInMailbox() - 1; i >= 0; i--) {
                *output++ = mailGetMailIDInMailbox(i);
            }
            break;
        }
    } else {
        lbl_8047A500 = NULL;
    }

    while (fn_8004D34C(selection) >= 0) {
        selection = fn_8004D9C0(selection);
    }

    if (count > 0) {
        fn_800E24B0(allocation);
        fn_800E209C(allocation);
    }
}
