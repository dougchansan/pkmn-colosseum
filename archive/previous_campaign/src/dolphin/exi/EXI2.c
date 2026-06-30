#include "dolphin/types.h"

/*
 * EXI2.c - EXI Channel 2 debug communication backend.
 *
 * Implements the AMC (Application Module Communication) hardware interface
 * for the GameCube debug serial port via EXI channel 2.
 *
 * Functions 0x800CE79C - 0x800CF254 in the original binary.
 *
 * The AMC functions are stubs in release builds (non-NDEV hardware).
 * The EXI2 transfer functions provide low-level byte-at-a-time
 * serial communication for the TRK debugger.
 */

extern void DBGWrite(); /* renamed symbol referenced by asm incs */
extern s32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(s32 enabled);
extern void __OSMaskInterrupts(u32 mask);
extern void __OSUnmaskInterrupts(u32 mask);
extern void __OSSetInterruptHandler(s32 interrupt, void* handler);
extern void OSReport(const char* fmt, ...);

/* Forward declarations for EXI2 internal functions */
extern void fn_800CED58(u32 type, void* buf, u32 len);  /* SIEnablePolling - in SI.c */
extern void DBGRead(u32 type, void* buf, u32 len);  /* SIDisablePolling - in SI.c */
extern void fn_800CEF10(void* resp);  /* SIGetResponse - in SI.c */
extern void fn_800CEC30(void);  /* SISetCommand - in SI.c */
extern void fn_800CEC70(void);  /* SIGetCommand - in SI.c */
extern void fn_800CECAC(void* resp);  /* SITransferCommands - in SI.c */

/* SDA-relative globals */
extern u8  lbl_80478AD0;    /* EXI2 device ID byte */
extern u32 lbl_8047AA30;    /* EXI2 status word */
extern u32 lbl_8047AA34;    /* EXI2 transfer length */
extern u32 lbl_8047AA38;    /* EXI2 callback pointer storage */
extern u8  lbl_8047AA3C;    /* EXI2 pending flag */
extern u32 lbl_8047AA2C;    /* EXI2 interrupt handler ptr */
extern u32 lbl_8047AA28;    /* EXI2 callback function ptr */

/* ========================================================== */
/* AMC stub functions (no-op on retail hardware)              */
/* ========================================================== */

/*
 * fn_800CE79C - AMC_Initialize stub
 * No-op: real AMC hardware not present on retail GameCube.
 */
void fn_800CE79C(void* callback, void* pendingPtr) {
}

/*
 * fn_800CE7A0 - AMC_InitInterrupts stub
 */
void fn_800CE7A0(void) {
}

/*
 * fn_800CE7A4 - AMC_Peek stub. Returns 0 (no data available).
 */
s32 fn_800CE7A4(void) {
    return 0;
}

/*
 * fn_800CE7AC - AMC_Read stub. Returns 0 (success, no data).
 */
s32 fn_800CE7AC(void* buf, s32 size) {
    return 0;
}

/*
 * fn_800CE7B4 - AMC_Write stub. Returns 0 (nothing written).
 */
s32 fn_800CE7B4(void* buf, s32 size) {
    return 0;
}

/*
 * fn_800CE7BC - AMC_PostStop stub
 */
void fn_800CE7BC(void) {
}

/*
 * fn_800CE7C0 - AMC_PreContinue stub
 */
void fn_800CE7C0(void) {
}

/*
 * AMC_IsStub - Returns 1 indicating AMC is a stub (no real hardware).
 */
s32 AMC_IsStub(void) {
    return 1;
}

/*
 * Hu_IsStub - Returns 0 indicating Hu is not present.
 */
s32 Hu_IsStub(void) {
    return 0;
}

/*
 * fn_800CE7D4 - EXI2 post-stop handler stub
 */
void fn_800CE7D4(void) {
}

/*
 * fn_800CE7D8 - EXI2 pre-continue handler stub
 */
void fn_800CE7D8(void) {
}

/* ========================================================== */
/* EXI2 communication functions (asm wrappers)                */
/* ========================================================== */

/*
 * fn_800CEFBC - Low-level EXI byte transfer.
 * Transfers data byte-by-byte over EXI channel 2.
 * 0x800CEFBC | size: 0x298
 */
#if 1
asm void fn_800CEFBC(void) {
#include "src/dolphin/exi/EXI2_fn_800CEFBC.inc"
}
#else
void fn_800CEFBC(void) {}
#endif

/*
 * fn_800CE7DC - EXI2 write/transfer function.
 * Sends data over EXI channel 2 using polling.
 * 0x800CE7DC | size: 0x260
 */
#if 1
asm void fn_800CE7DC(void) {
#include "src/dolphin/exi/EXI2_fn_800CE7DC.inc"
}
#else
void fn_800CE7DC(void) {}
#endif

/*
 * fn_800CEA3C - EXI2 read function.
 * Reads data from EXI channel 2.
 * 0x800CEA3C | size: 0x8C
 */
#if 0
asm void fn_800CEA3C(void) {
#include "src/dolphin/exi/EXI2_fn_800CEA3C.inc"
}
#else
#pragma push
#pragma peephole on
s32 fn_800CEA3C(void* buf, s32 len) {
    s32 enabled;
    enabled = OSDisableInterrupts();
    DBGRead(((lbl_8047AA30 & 0x10000) ? 0x1000 : 0) + 0x1E000, buf,
                (len + 3) & ~3);
    lbl_8047AA34 = 0;
    lbl_8047AA3C = 0;
    OSRestoreInterrupts(enabled);
    return 0;
}
#pragma pop
#endif

/*
 * fn_800CEAC8 - EXI2 peek/status check.
 * Checks for available data on EXI channel 2.
 * 0x800CEAC8 | size: 0x9C
 */
#if 0
asm void fn_800CEAC8(void) {
#include "src/dolphin/exi/EXI2_fn_800CEAC8.inc"
}
#else
#pragma push
#pragma peephole on
u32 fn_800CEAC8(void) {
    s32 enabled;
    u32 resp;
    lbl_8047AA3C = 0;
    if ((s32)lbl_8047AA34 == 0) {
        enabled = OSDisableInterrupts();
        fn_800CECAC(&resp);
        if (resp & 1) {
            fn_800CEF10(&resp);
            resp &= 0x1FFFFFFF;
            if ((resp & 0x1F000000) == 0x1F000000) {
                lbl_8047AA30 = resp;
                lbl_8047AA34 = resp & 0x7FFF;
                lbl_8047AA3C = 1;
            }
        }
        OSRestoreInterrupts(enabled);
    }
    return lbl_8047AA34;
}
#pragma pop
#endif

/*
 * fn_800CEB64 - EXI2 interrupt initialization.
 * Masks/unmasks interrupts and sets up the EXI2 interrupt handler.
 * 0x800CEB64 | size: 0x54
 */
#if 1
asm void fn_800CEB64(void) {
#include "src/dolphin/exi/EXI2_fn_800CEB64.inc"
}
#else
void fn_800CEB64(void) {
    __OSMaskInterrupts(0x18000);
    __OSMaskInterrupts(0x40);
    lbl_8047AA2C = (u32)fn_800CEC70;
    __OSSetInterruptHandler(0x19, (void*)fn_800CEC30);
    __OSUnmaskInterrupts(0x40);
}
#endif

/*
 * fn_800CEBB8 - EXI2 initialize.
 * Sets up the EXI channel 2 hardware for debug communication.
 * 0x800CEBB8 | size: 0x78
 */
#if 1
asm void fn_800CEBB8(void) {
#include "src/dolphin/exi/EXI2_fn_800CEBB8.inc"
}
#else
void fn_800CEBB8(u32* pendingPtr, void* callback) {
    s32 enabled;
    enabled = OSDisableInterrupts();
    lbl_8047AA38 = (u32)&lbl_8047AA3C;
    *pendingPtr = lbl_8047AA38;
    lbl_8047AA28 = (u32)callback;
    __OSMaskInterrupts(0x18000);
    *(volatile u32*)0xCC006828 = 0;
    OSRestoreInterrupts(enabled);
}
#endif
