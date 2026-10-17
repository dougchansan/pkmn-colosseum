/**
 * @file menu_exact_80103BA8.c
 * @brief menu engine, 0x80103BA8-0x80103E68: menuGetKeyInfo, the
 *        enable-port accessors and menuInit.
 *
 * Function-boundary carve of the menu TU (see src/game/menu.c), built with
 * the TU's flags (configure.py). Text only: none of these functions uses a
 * pooled literal or inlines a function of the TU. The window-system work,
 * the port-state bytes and the texture group stay extern.
 */
#include "dolphin/types.h"

typedef struct MenuKeyInfo {
    u16 keys;
    u16 previousKeys;
    u16 pressedKeys;
    u16 repeatKeys;
    u16 heldKeys;
    s8 repeatTimer[16];
} MenuKeyInfo;

/* Window system work (WINDOW_SYS_WORK). */
typedef struct MenuSystem {
    u8 pad_00[0x10];
    MenuKeyInfo keyInfo;        /* keys of the active window's port(s) */
    MenuKeyInfo portKeyInfo[4]; /* per-port state, updated every frame */
    u8 enablePort;
} MenuSystem;

extern MenuSystem lbl_80404ACC;
extern u32 lbl_8047AD00;   /* menu texture-resource group, created once */
extern u8 lbl_8047AD04[4]; /* input type per port: 0 none, 1 GC pad, 2 AGB, 3 AGB lost */
extern u8 lbl_8047AD08[4]; /* the same, one frame earlier */
extern u8 lbl_803156E0[];
extern u8 lbl_803254E0[];
extern u8 lbl_803357E0[];

extern void* memset(void* dst, int val, u32 size);
extern MenuKeyInfo* windowGetPortKeyInfo(u8 ports);
extern void windowInit(u16 count);
extern void cursorBiosInit(void);
extern void menuOffScreenInit(void);
extern void menuFaceInit(u32 count);
extern u32 gfxVtxDescAlloc(void);
extern void gfxVtxDescSetAttr(u32, u32, u32, u32, u32, u32, u32, u32);
extern void* GStextureLoad(void* data);
extern void GSresRegisterResource(void* texture, u32, u32 resourceId, u32);

/* 0x80103BA8 | 0x108 */
void menuGetKeyInfo(MenuKeyInfo* out, s32 port) {
    MenuKeyInfo info;
    u8 mask;

    switch (port) {
    case 1: mask = 1; break;
    case 2: mask = 2; break;
    case 3: mask = 4; break;
    case 4: mask = 8; break;
    default: mask = 0; break;
    }
    if (mask != 0) {
        info = *windowGetPortKeyInfo(mask);
    } else {
        memset(&info, 0, sizeof(info));
    }
    *out = info;
}

/* 0x80103CB0 | 0x10 */
u8 menuGetEnablePort(void) {
    return lbl_80404ACC.enablePort;
}

/* 0x80103CC0 | 0x18 */
u8 menuSetEnablePort(u8 enable) {
    u8 old = lbl_80404ACC.enablePort;
    lbl_80404ACC.enablePort = enable;
    return old;
}

/* 0x80103CD8 | 0x190 */
void menuInit(u16 windowCount) {
    void* texture;
    s32 port;

    windowInit(windowCount);
    cursorBiosInit();
    menuOffScreenInit();
    menuFaceInit(0x18);
    lbl_80404ACC.enablePort = 1;
    for (port = 0; port < 4; port++) {
        lbl_8047AD04[port] = 0;
        lbl_8047AD08[port] = 0;
    }

    if (lbl_8047AD00 == 0) {
        lbl_8047AD00 = gfxVtxDescAlloc();
        gfxVtxDescSetAttr(lbl_8047AD00, 1, 0, 0, 3, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047AD00, 4, 0, 6, 10, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047AD00, 6, 0, 8, 4, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047AD00, 7, 0, 8, 4, 0, 0, 0);
        gfxVtxDescSetAttr(lbl_8047AD00, 8, 0, 8, 4, 0, 0, 0);
    }

    texture = GStextureLoad(lbl_803156E0);
    GSresRegisterResource(texture, 0, 0x31A1200, 0);
    texture = GStextureLoad(lbl_803254E0);
    GSresRegisterResource(texture, 0, 0x6221200, 0);
    texture = GStextureLoad(lbl_803357E0);
    GSresRegisterResource(texture, 0, 0x6F71200, 0);
}
