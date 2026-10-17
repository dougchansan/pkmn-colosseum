#include "dolphin/types.h"

/*
 * TRKComm.c - TRK communication table initialization.
 *
 * Sets up the function pointer table (gDBCommTable) that provides
 * an abstraction layer for debugger communication. Supports three
 * backends: DDH (EXI/AMC), GDEV (USB Gecko), and UDP (BBA network).
 */

extern void OSReport(const char* fmt, ...);
extern void OSEnableScheduler(void);
extern void TRKLoadContext(void* ctx, u32 exceptionID);

extern s32 Hu_IsStub(void);
extern s32 AMC_IsStub(void);

/* Communication backends */
extern void ddh_cc_initialize(void);
extern void ddh_cc_initinterrupts(void);
extern void ddh_cc_shutdown(void);
extern void ddh_cc_peek(void);
extern void ddh_cc_read(void);
extern void ddh_cc_write(void);
extern void ddh_cc_open(void);
extern void ddh_cc_close(void);
extern void ddh_cc_pre_continue(void);
extern void ddh_cc_post_stop(void);

extern void gdev_cc_initialize(void);
extern void gdev_cc_initinterrupts(void);
extern void gdev_cc_shutdown(void);
extern void gdev_cc_peek(void);
extern void gdev_cc_read(void);
extern void gdev_cc_write(void);
extern void gdev_cc_open(void);
extern void gdev_cc_close(void);
extern void gdev_cc_pre_continue(void);
extern void gdev_cc_post_stop(void);

extern void udp_cc_initialize(void);
extern void udp_cc_shutdown(void);
extern void udp_cc_peek(void);
extern void udp_cc_read(void);
extern void udp_cc_write(void);
extern void udp_cc_open(void);
extern void udp_cc_close(void);
extern void udp_cc_pre_continue(void);
extern void udp_cc_post_stop(void);

/* Global comm table - 10 function pointers */
extern u32 gDBCommTable[];

/* BBA flag */
extern u8 TRK_Use_BBA;

/* String table for comm messages */
extern char EndofProgramInstruction[];

/* TRKEXICallBack function pointer type */
typedef void (*TRKEXICallBackFunc)(s32 chan, void* ctx);
extern void TRKEXICallBack(s32 chan, void* ctx);

/*
 * TRKInitializeIntDrivenUART - Set up interrupt-driven UART communication.
 *
 * Calls the comm table's initialize function with the EXI callback,
 * then calls initInterrupts to enable interrupt-based reception.
 *
 * Parameters:
 *   baud - baud rate / configuration
 *   polarity - polarity setting
 *   pad - padding parameter (unused)
 *   pendingPtr - pointer to the input pending flag
 *
 * Returns 0 on success.
 */
s32 TRKInitializeIntDrivenUART(s32 baud, s32 polarity, s32 pad, void* pendingPtr) {
    typedef s32 (*InitFunc)(void* callback, void* pending);
    typedef s32 (*InitIntFunc)(void);

    InitFunc initFunc = (InitFunc)gDBCommTable[0]; /* offset 0x00: initialize */
    InitIntFunc initIntFunc;

    initFunc((void*)TRKEXICallBack, pendingPtr);

    initIntFunc = (InitIntFunc)gDBCommTable[6]; /* offset 0x18: open/initinterrupts */
    initIntFunc();

    return 0;
}

/*
 * InitMetroTRKCommTable - Initialize the communication function table.
 *
 * Based on the channel parameter:
 *   0: DDH (AMC/EXI) - standard serial debug
 *   1: GDEV (USB Gecko) - USB debug
 *   2: UDP (BBA) - network debug
 *
 * Populates gDBCommTable with the appropriate backend function pointers.
 * Returns 0 on success, 1 on failure (unknown channel).
 */
s32 InitMetroTRKCommTable(s32 channel) {
    s32 result = 1;
    char* strings = (char*)EndofProgramInstruction;

    OSReport(strings + 0x08, channel);

    /* Clear BBA flag */
    TRK_Use_BBA = 0;

    if (channel == 2) {
        /* UDP/BBA mode */
        OSReport(strings + 0x20);

        TRK_Use_BBA = 1;

        gDBCommTable[0] = (u32)udp_cc_initialize;
        gDBCommTable[6] = (u32)udp_cc_open;
        gDBCommTable[7] = (u32)udp_cc_close;
        gDBCommTable[4] = (u32)udp_cc_read;
        gDBCommTable[5] = (u32)udp_cc_write;
        gDBCommTable[2] = (u32)udp_cc_shutdown;
        gDBCommTable[3] = (u32)udp_cc_peek;
        gDBCommTable[8] = (u32)udp_cc_pre_continue;
        gDBCommTable[9] = (u32)udp_cc_post_stop;
        gDBCommTable[1] = 0;

        result = 0;
    } else if (channel == 1) {
        /* GDEV/USB Gecko mode */
        OSReport(strings + 0x38);

        Hu_IsStub();

        gDBCommTable[0] = (u32)gdev_cc_initialize;
        gDBCommTable[6] = (u32)gdev_cc_open;
        gDBCommTable[7] = (u32)gdev_cc_close;
        gDBCommTable[4] = (u32)gdev_cc_read;
        gDBCommTable[5] = (u32)gdev_cc_write;
        gDBCommTable[2] = (u32)gdev_cc_shutdown;
        gDBCommTable[3] = (u32)gdev_cc_peek;
        gDBCommTable[8] = (u32)gdev_cc_pre_continue;
        gDBCommTable[9] = (u32)gdev_cc_post_stop;
        gDBCommTable[1] = (u32)gdev_cc_initinterrupts;

        result = Hu_IsStub(); /* result from Hu_IsStub() call */
    } else if (channel == 0) {
        /* DDH/AMC/EXI mode */
        OSReport(strings + 0x5C);

        AMC_IsStub();

        gDBCommTable[0] = (u32)ddh_cc_initialize;
        gDBCommTable[6] = (u32)ddh_cc_open;
        gDBCommTable[7] = (u32)ddh_cc_close;
        gDBCommTable[4] = (u32)ddh_cc_read;
        gDBCommTable[5] = (u32)ddh_cc_write;
        gDBCommTable[2] = (u32)ddh_cc_shutdown;
        gDBCommTable[3] = (u32)ddh_cc_peek;
        gDBCommTable[8] = (u32)ddh_cc_pre_continue;
        gDBCommTable[9] = (u32)ddh_cc_post_stop;
        gDBCommTable[1] = (u32)ddh_cc_initinterrupts;

        result = AMC_IsStub(); /* result from AMC_IsStub() call */
    } else {
        /* Unknown channel */
        OSReport(strings + 0x80, channel);
        OSReport(strings + 0xAC);
        OSReport(strings + 0xDC);
    }

    return result;
}

/*
 * TRKEXICallBack - EXI interrupt callback for TRK.
 *
 * Called when the EXI controller signals data availability.
 * Enables the OS scheduler and then loads the saved CPU context
 * to enter the TRK interrupt handler as exception 0x500.
 */
void TRKEXICallBack(s32 chan, void* ctx) {
    OSEnableScheduler();
    TRKLoadContext(ctx, 0x500);
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* TRKCommGetState - 0x800C39A0 | size: 0x10 */
/*
 * TRKCommGetState - Return the TRK comm state byte.
 * 0x800C39A0 | size: 0x10
 */
u8 TRKCommGetState(void) {
    extern u8 lbl_803FED70[];
    return lbl_803FED70[0];
}

/*
 * TRKCommSetState - Set the TRK comm state byte.
 * 0x800C39B0 | size: 0xC
 */
void TRKCommSetState(u8 state) {
    extern u8 lbl_803FED70[];
    lbl_803FED70[0] = state;
}

/*
 * TRKCommSendReset - Send a reset (0xD3) command via TRK comm.
 *
 * Returns 0 on success, 2 on partial, 1 on error or init failure.
 *
 * 0x800C39BC | size: 0x84
 */
u32 __close_console(u32 arg) {
    extern s32 GetTRKConnected(void);
    extern u32 TRKCloseFile(u32 cmd, u32 param);
    s32 initOk;
    u32 result;

    initOk = GetTRKConnected();
    if (initOk == 0) {
        return 1;
    }

    result = TRKCloseFile(0xD3, arg) & 0xFF;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

/*
 * TRKCommSendRead - Send a read (0xD0) command via TRK comm.
 *
 * Checks comm state and init, then sends the read command with
 * the address from *addrPtr and the given length. Stores the
 * returned value back to *addrPtr.
 *
 * 0x800C3A40 | size: 0xBC
 */
u32 TRKCommSendRead(u32 unused, u32 length, u32* addrPtr) {
    extern u8 TRKCommGetState(void);
    extern s32 GetTRKConnected(void);
    extern u32 TRKAccessFile(u32 cmd, u32 dir, u32* addrBuf, u32 len);
    u32 addrBuf;
    u32 result;

    if ((TRKCommGetState() & 0xFF) == 0) {
        return 1;
    }

    if (GetTRKConnected() == 0) {
        return 1;
    }

    addrBuf = *addrPtr;
    result = TRKAccessFile(0xD0, 1, &addrBuf, length);
    *addrPtr = addrBuf;

    switch (result & 0xFF) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

/*
 * TRKCommSendWrite - Send a write (0xD1) command via TRK comm.
 *
 * Same pattern as TRKCommSendRead but uses command 0xD1 with
 * direction 0 (write).
 *
 * 0x800C3AFC | size: 0xBC
 */
u32 TRKCommSendWrite(u32 unused, u32 length, u32* addrPtr) {
    extern u8 TRKCommGetState(void);
    extern s32 GetTRKConnected(void);
    extern u32 TRKAccessFile(u32 cmd, u32 dir, u32* addrBuf, u32 len);
    u32 addrBuf;
    u32 result;

    if ((TRKCommGetState() & 0xFF) == 0) {
        return 1;
    }

    if (GetTRKConnected() == 0) {
        return 1;
    }

    addrBuf = *addrPtr;
    result = TRKAccessFile(0xD1, 0, &addrBuf, length);
    *addrPtr = addrBuf;

    switch (result & 0xFF) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}
