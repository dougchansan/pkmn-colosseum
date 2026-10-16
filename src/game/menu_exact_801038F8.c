/**
 * @file menu_exact_801038F8.c
 * @brief menu engine, 0x801038F8-0x80103BA8: _menuUpdateKeyInfo, the
 *        per-frame pad/AGB key-state update.
 *
 * Function-boundary carve of the menu TU (see src/game/menu.c), built with
 * the TU's flags (configure.py). Text only: it uses no pooled literal and
 * no jump table (its switches compile to compare trees). Retail calls
 * _menuGetGcKeyInfo and _menuGetAgbKeyInfo rather than inlining them; in
 * this carve they are defined elsewhere, so GC/1.3.2's -inline auto can't
 * expand them either.
 */
#include "dolphin/types.h"

typedef struct tagWINDOW_WORK MenuWindow;

/* Open window (only the field this function reads). */
struct tagWINDOW_WORK {
    u8 pad_00[0x0B];
    u8 keyPort; /* input port(s) the window reads */
};

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
    u16 heldKeys;
    s8 repeatTimer[16];
} MenuKeyInfo;

/* Window system work. */
typedef struct WINDOW_SYS_WORK {
    u8 pad_00[0x0C];
    MenuWindow* head;
    MenuKeyInfo keyInfo;        /* keys of the active window's port(s) */
    MenuKeyInfo portKeyInfo[4]; /* per-port state, updated every frame */
    u8 enablePort;
    s32 lastError;
    u32 activeId;
} MenuSystem;

extern MenuSystem lbl_80404ACC;
extern u8 lbl_8047AD04[4]; /* input type per port: 0 none, 1 GC pad, 2 AGB, 3 AGB lost */
extern u8 lbl_8047AD08[4]; /* the same, one frame earlier */

u32 GSgetFrameStep(void);
MenuWindow* windowSearchID(u32 id);
u32 windowGetActiveID(void);
MenuKeyInfo* windowGetPortKeyInfo(u8 ports);
u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys);
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys);

/* 0x801038F8 | 0x2B0 */
/* Retail ignores the work argument and updates the global directly. */
void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(MenuSystem* work) {
    MenuWindow* window;
    MenuKeyInfo* keyInfo;
    s32 i;
    s32 port;
    u16 keys;
    u16 pressed;
    u16 repeat;
    u16 held;
    u16 bit;
    u8 type;

    for (port = 0; port < 4; port++) {
        keyInfo = &lbl_80404ACC.portKeyInfo[port];
        lbl_8047AD08[port] = lbl_8047AD04[port];
        keys = 0;
        if (_menuGetGcKeyInfo__FlPUs(port, &keys) == 0) {
            if (_menuGetAgbKeyInfo__FlPUs(port, &keys) == 0) {
                keys = 0;
                type = 0;
            } else {
                type = 2;
            }
        } else {
            type = 1;
        }

        /* The case lists are the values the retail compare trees test. */
        switch (type) {
        case 0:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 1:
            default:
                lbl_8047AD04[port] = 0;
                break;
            case 2:
            case 3:
                /* An AGB was unplugged: report it once as key 0x8000. */
                lbl_8047AD04[port] = 3;
                keys = 0x8000;
                break;
            }
            break;
        case 1:
            lbl_8047AD04[port] = 1;
            break;
        case 2:
            switch (lbl_8047AD08[port]) {
            case 0:
            case 2:
            case 3:
            default:
                lbl_8047AD04[port] = 2;
                break;
            case 1:
                lbl_8047AD04[port] = 2;
                break;
            }
            break;
        default:
            lbl_8047AD04[port] = type;
            break;
        }

        keyInfo->previousKeys = keyInfo->keys;
        pressed = (keyInfo->previousKeys ^ 0xFFFF) & keys;
        repeat = 0;
        held = 0;
        for (i = 0; i < 16; i++) {
            bit = 1 << i;
            if (pressed & bit) {
                keyInfo->repeatTimer[i] = 15;
                repeat |= bit;
            } else if (keys & bit) {
                keyInfo->repeatTimer[i] -= GSgetFrameStep();
                if (keyInfo->repeatTimer[i] <= 0) {
                    keyInfo->repeatTimer[i] = 5;
                    repeat |= bit;
                    held |= bit;
                } else {
                    held |= bit & keyInfo->heldKeys;
                }
            }
        }
        if (pressed & 0xF) {
            keyInfo->repeatTimer[0] = 15;
            keyInfo->repeatTimer[1] = 15;
            keyInfo->repeatTimer[2] = 15;
            keyInfo->repeatTimer[3] = 15;
        }
        keyInfo->keys = keys;
        keyInfo->pressedKeys = pressed;
        keyInfo->repeatKeys = repeat;
        keyInfo->heldKeys = held;
    }

    window = windowSearchID(windowGetActiveID());
    if (window == NULL) {
        type = 1;
    } else {
        type = window->keyPort;
    }
    lbl_80404ACC.keyInfo = *windowGetPortKeyInfo(type);
}
