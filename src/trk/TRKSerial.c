#include "dolphin/types.h"

/*
 * TRKSerial.c - TRK serial handler initialization and input processing.
 *
 * Sets up the serial communication state used by the debugger nub
 * for receiving messages from the host debugger.
 */

extern void MWTRACE(s32 level, const char* fmt, ...);
extern void fn_800BE464(void* event, s32 type);
extern s32  fn_800BE47C(void* event);
extern s32  fn_800BF1FC(void);  /* TRKProcessInput - returns buffer index or -1 */
extern void* TRKGetBuffer(s32 index);

/* Serial handler state at lbl_803FE7B8 */
extern u8 lbl_803FE7B8[];

/* String table for serial handler at lbl_8026F688 */
extern char lbl_8026F688[];

/*
 * TRKInitializeSerialHandler - Set up the serial handler state.
 *
 * Clears the serial handler fields and logs initialization messages
 * via MWTRACE. The serial handler state is a small structure with:
 *   offset 0x00: current buffer index (set to -1)
 *   offset 0x08: read position (0)
 *   offset 0x0C: write position (0)
 */
s32 TRKInitializeSerialHandler(void) {
    u8* state = lbl_803FE7B8;
    char* strings = lbl_8026F688;

    /* Initialize serial state */
    ((s32*)state)[0] = -1; /* buffer index = none */
    ((s32*)state)[2] = 0;  /* offset 0x08: read pos */
    ((s32*)state)[3] = 0;  /* offset 0x0C: write pos */

    MWTRACE(1, strings + 0x00, 0x40);  /* string at +0x00 */
    MWTRACE(1, strings + 0x24, 0x40);  /* string at +0x24 */
    MWTRACE(1, strings + 0x48, 0x40);  /* string at +0x48 */
    MWTRACE(1, strings + 0x6C, 0x40);  /* string at +0x6C */
    MWTRACE(1, strings + 0x8C, 0x40);  /* string at +0x8C */
    MWTRACE(1, strings + 0xAC, 0x40);  /* string at +0xAC */

    return 0;
}

/*
 * TRKGetInput - Check for and process incoming debugger messages.
 *
 * Calls the low-level input processor. If a complete message was
 * received (buffer index != -1), constructs a message event and
 * posts it to the event queue.
 */
void TRKGetInput(void) {
    s32 bufIdx;

    bufIdx = fn_800BF1FC();

    if (bufIdx != -1) {
        u8 eventBuf[0x10];
        u8* state = lbl_803FE7B8;

        TRKGetBuffer(bufIdx);

        fn_800BE464((void*)eventBuf, 2); /* construct message event */

        /* Store buffer index in event at offset 0x08 */
        ((s32*)eventBuf)[2] = bufIdx;

        /* Reset serial state */
        ((s32*)state)[0] = -1;

        fn_800BE47C((void*)eventBuf); /* post event */
    }
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/*
 * TRKProcessSerialInput - Process a received serial message.
 *
 * Constructs a message event (type 2), resets the serial handler
 * buffer index to -1, and posts the event.
 *
 * 0x800BF14C | size: 0x50
 */
void fn_800BF14C(s32 bufIdx) {
    extern s32 TRKPostEvent(void* event);
    u8 eventBuf[0x10];

    fn_800BE464((void*)eventBuf, 2);
    ((s32*)lbl_803FE7B8)[0] = -1;
    *(s32*)(eventBuf + 0x8) = bufIdx;
    TRKPostEvent((void*)eventBuf);
}

/*
 * TRKSerialDebugPrint - Debug print routine for serial data.
 *
 * Iterates through a string, saving/restoring the connected state
 * around each character for debug output via OSReport.
 *
 * 0x800BF33C | size: 0x88
 */
s32 fn_800BF33C(u8* str) {
    extern u32 fn_800C04F4(void);
    extern void fn_800C04E8(u32 state);
    extern void OSReport(const char* fmt);
    u8 buf[2];
    u8 zero;
    s32 state;
    s8 ch;
    s32 result;

    zero = 0;
    result = 0;
    while ((result == 0) && ((ch = (s8)*str++) != 0)) {
        state = fn_800C04F4();
        buf[0] = ch;
        buf[1] = zero;
        fn_800C04E8(0);
        OSReport((const char*)buf);
        fn_800C04E8(state);
        result = 0;
    }
    return result;
}
