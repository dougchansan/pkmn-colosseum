/**
 * @file main_tail.c
 * @brief Matching retrace callbacks and OS error handlers.
 *
 * Address range: 0x80005FA8 - 0x80006630
 */

#include "dolphin/types.h"

extern void* memcpy(void* dst, const void* src, u32 size);
extern void OSSetErrorHandler(u32 error, void* handler);
extern void OSLoadContext(void* context);
extern void OSEnableScheduler(void);
extern void GSthreadExecuteAll(void);
extern u8 fn_8000DAA8(void);
extern void fn_80181850(u32 a, u32 b);
extern u8 fn_800F7EF8(u32 padIdx);
extern void fn_8008AC34(u32 padIdx);
extern u8 fn_8008ABA0(u32 padIdx);
extern void fn_800F7F64(u32 padIdx);
extern void* fn_801664F0(void* color);
extern void GSresRegisterResource(void* work, u32 time, u32 numFrames, u32 arg);
extern void fn_8009F488(u32 region, u32 addr, u32 size, u32 perm);
extern s32 fn_800A2C58(void);
extern void OSCreateThread(void* thread, void* func, void* arg,
                           void* stackBase, u32 stackSize,
                           u32 priority, u32 detached);
extern void OSResumeThread(void* thread);
extern void fn_800A1E54(void* thread, u32 prio);
extern void __OSSetExceptionHandler(u32 exception, void* handler);
extern void* __va_arg(void* ap, u32 type);
extern u32 OSGetStackPointer(void);

void fn_800060F0(const char* file, s32 line, const char* fmt, ...);
void fn_80006378(u32 error, void* context, ...);
void fn_800064A0(u32 error, void* context);
s32 fn_800064C4(void);

extern s32 lbl_80478990;
extern u32 lbl_8047A260;
extern u32 lbl_8047A264;
extern u32 lbl_8047A268;
extern u8 lbl_80266438[];
extern u8 lbl_803A0700[];
extern u8 lbl_803A1700[];
extern u8 lbl_8039E700[];
extern u8 lbl_802E2888[];
extern const char lbl_80266448[];
extern char lbl_8047B6B8[8];
extern char lbl_8047B6C0[8];

typedef struct __va_list_struct {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} __va_list_struct;
typedef __va_list_struct va_list[1];
#define va_start(ap, last) ((void)last, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

extern int vsprintf(char* buf, const char* fmt, va_list ap);
extern int sprintf(char* buf, const char* fmt, ...);

/* =========================================================================
 *  fn_80005FA8 / fn_80005FA8
 *  Address: 0x80005FA8, Size: 0x54
 *
 *  Audio retrace callback. Called with (error, context) from the task
 *  system. Updates the audio frame, then checks the locale flag to
 *  potentially trigger a reconnect handler.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005FA8(u32 a, u32 b)
{
    GSthreadExecuteAll();
    if (fn_8000DAA8() == 0) {
        fn_80181850(a, b);
    }
}
#pragma pop

/* =========================================================================
 *  fn_80005FFC / fn_80005FFC
 *  Address: 0x80005FFC, Size: 0x50
 *
 *  Controller polling task. Iterates pads 1-4:
 *  if a pad is NOT responding to fn_800F7EF8 (connection check),
 *  it calls fn_8008AC34 to recalibrate that pad.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005FFC(void) {
    s32 i;

    for (i = 1; i <= 4; i++) {
        if (fn_800F7EF8(i) == 0) {
            fn_8008AC34(i);
        }
    }
}
#pragma pop

/* =========================================================================
 *  fn_8000604C / fn_8000604C
 *  Address: 0x8000604C, Size: 0x50
 *
 *  Controller rumble update task. Iterates pads 1-4:
 *  if a pad is NOT responding to fn_8008ABA0 (motor check),
 *  it calls fn_800F7F64 to reset that pad's rumble state.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_8000604C(void) {
    s32 i;

    for (i = 1; i <= 4; i++) {
        if (fn_8008ABA0(i) == 0) {
            fn_800F7F64(i);
        }
    }
}
#pragma pop

/* =========================================================================
 *  InitBackgroundColor / fn_8000609C
 *  Address: 0x8000609C, Size: 0x54
 *
 *  Copies the default background color from rodata (lbl_80266438) into
 *  a local buffer and calls the floor system to set the clear color.
 *  Also configures a render schedule with a 2000-tick timeout.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_8000609C(void) {
    struct BgColor12 { u32 c[3]; } buf;
    buf = *(struct BgColor12*)lbl_80266438;
    GSresRegisterResource(fn_801664F0(&buf), 0, 0x7D0, 0);
}
#pragma pop

/* fn_800060F0 - 0x800060F0 | size: 0x160 */
#pragma push
#pragma peephole off
void fn_800060F0(const char* file, s32 line, const char* fmt, ...) {
    va_list ap;
    int len;
    register u32 i;
    register u32* p;
    register int pos;
    u32 table = (u32)lbl_80266448;

    va_start(ap, fmt);
    len = vsprintf((char*)lbl_803A0700, fmt, ap);
    pos = len + sprintf((char*)lbl_803A0700 + len, (const char*)(table + 0x178), file, line);
    pos += sprintf((char*)lbl_803A0700 + pos, (const char*)(table + 0x190));
    for (i = 0, p = (u32*)OSGetStackPointer(); p && (u32)p != 0xFFFFFFFF && i++ < 0x20; p = (u32*)*p) {
        pos += sprintf((char*)lbl_803A0700 + pos, (const char*)(table + 0x1B4), p, p[0], p[1]);
    }
    fn_800060F0(lbl_8047B6B8, 0x196, (char*)lbl_803A0700);
}
#pragma pop

/* =========================================================================
 *  fn_80006250 / fn_80006250
 *  Address: 0x80006250, Size: 0x128
 *
 *  Sets up memory protection regions and installs a custom error handler
 *  (fn_80006378 / fn_80006378) for all relevant OS exception types:
 *    0  = SYSTEM_RESET
 *    1  = MACHINE_CHECK
 *    2  = DSI
 *    3  = ISI
 *    5  = ALIGNMENT
 *    11 = FLOATING_POINT
 *    13 = PERFORMACE_MONITOR
 *    14 = BREAKPOINT
 *    15 = SYSTEM_INTERRUPT
 *    16 = THERMAL_INTERRUPT
 *  If the debugger is not connected (fn_800A2C58 returns 0), also
 *  installs handlers for:
 *    6  = PROGRAM
 *    10 = SYSTEM_CALL
 *    12 = TRACE
 *
 *  Memory protection regions configured:
 *    Region 0: 0x00000000, size 0x80000000, permission 3 (RW)
 *    Region 1: 0x81800000, size 0x01800000, permission 3 (RW)
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80006250(void) {
    /* Set up memory protection regions */
    fn_8009F488(0, 0, 0x80000000, 3);
    fn_8009F488(1, 0x81800000, 0x01800000, 3);

    /* Install custom error handler for standard exceptions */
    OSSetErrorHandler(0, fn_80006378);   /* SYSTEM_RESET */
    OSSetErrorHandler(1, fn_80006378);   /* MACHINE_CHECK */
    OSSetErrorHandler(2, fn_80006378);   /* DSI */
    OSSetErrorHandler(3, fn_80006378);   /* ISI */
    OSSetErrorHandler(5, fn_80006378);   /* ALIGNMENT */
    OSSetErrorHandler(11, fn_80006378);  /* FLOATING_POINT */
    OSSetErrorHandler(13, fn_80006378);  /* PERFORMACE_MONITOR */
    OSSetErrorHandler(14, fn_80006378);  /* BREAKPOINT */
    OSSetErrorHandler(15, fn_80006378);  /* SYSTEM_INTERRUPT */

    /* Store max error handler index */
    lbl_80478990 = 16;
    OSSetErrorHandler(16, fn_80006378);  /* THERMAL_INTERRUPT */

    /* If debugger is not attached, also handle program/trace/syscall */
    if (fn_800A2C58() == 0) {
        OSSetErrorHandler(6, fn_80006378);   /* PROGRAM */
        OSSetErrorHandler(10, fn_80006378);  /* SYSTEM_CALL */
        OSSetErrorHandler(12, fn_80006378);  /* TRACE */
    }
}
#pragma pop

/* =========================================================================
 *  fn_80006378 / fn_80006378
 *  Address: 0x80006378, Size: 0x128
 *
 *  Custom OS error handler. Called on any unhandled exception.
 *  Saves the error number, copies the OS context, formats a crash
 *  report using the error name string table, installs a recovery
 *  exception handler, enables the scheduler, then creates a thread
 *  to display the error on screen.
 * =========================================================================
 */
typedef struct ErrVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} ErrVaList;
typedef ErrVaList ErrVaListArray[1];

typedef struct ErrThread {
    u64 pad[0x318 / 8];
} ErrThread;

#pragma push
#pragma peephole off
void fn_80006378(u32 error, void* context, ...) {
    ErrThread thread;
    ErrVaListArray args;

    __builtin_va_info(&args);
    lbl_8047A264 = *(u32*)__va_arg(args, 1);
    lbl_8047A268 = *(u32*)__va_arg(args, 1);
    memcpy(lbl_803A1700, context, 0x2C8);
    lbl_8047A260 = (u16)error;
    __OSSetExceptionHandler(8, fn_800064A0);
    OSEnableScheduler();
    OSCreateThread(&thread, fn_800064C4, NULL, lbl_8039E700 + 0x1FFC, 0x2000, 1, 0);
    OSResumeThread(&thread);
    fn_800A1E54(&thread, 0);
}
#pragma pop

/* =========================================================================
 *  fn_800064A0 / fn_800064A0
 *  Address: 0x800064A0, Size: 0x24
 *
 *  Installed as exception handler 8 (decrementer). Simply loads
 *  the saved context to resume execution in the error display thread.
 * =========================================================================
 */
#pragma push
#pragma scheduling off
void fn_800064A0(u32 error, void* context) {
    OSLoadContext(context);
}
#pragma pop

/* =========================================================================
 *  fn_800064C4 / fn_800064C4
 *  Address: 0x800064C4, Size: 0x16C
 *
 *  Thread entry function that renders the crash report to screen.
 *  Reads the saved error number, context, and error name table to
 *  produce a formatted crash dump visible on the TV output.
 * =========================================================================
 */
#pragma push
#pragma peephole off
s32 fn_800064C4(void) {
    const char* strs = (const char*)lbl_80266448;
    s32 n;
    u32 i;
    u32* sp;
    s32 len;

    n = sprintf((char*)lbl_803A0700, lbl_8047B6C0,
                    ((char**)lbl_802E2888)[lbl_8047A260]);
    len = n + sprintf((char*)lbl_803A0700 + n, strs + 0x1CC,
                       *(u32*)(lbl_803A1700 + 0x84), *(u32*)(lbl_803A1700 + 0x80));
    len += sprintf((char*)lbl_803A0700 + len, strs + 0x1EC,
                       *(u32*)(lbl_803A1700 + 0x198), *(u32*)(lbl_803A1700 + 0x19C));
    len += sprintf((char*)lbl_803A0700 + len, strs + 0x20C,
                       lbl_8047A264, lbl_8047A268);
    len += sprintf((char*)lbl_803A0700 + len, strs + 0x190);

    i = 0;
    sp = (u32*)*(u32*)(lbl_803A1700 + 0x4);
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 0x20) {
        len += sprintf((char*)lbl_803A0700 + len, strs + 0x1B4,
                           sp, sp[0], sp[1]);
        sp = (u32*)sp[0];
    }

    fn_800060F0(lbl_8047B6B8, 0xD5, (char*)lbl_803A0700);
    return 0;
}
#pragma pop
