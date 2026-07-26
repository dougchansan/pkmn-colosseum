/**
 * @file sdk_range_800BF33C.c
 * @brief dolphin-sdk code, 0x800BF33C - 0x800BF534 (2 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

s32 TRKDispatchMessage(u8* message)
{
    extern void TRKSetBufferPosition(void* message, u32 position);
    extern void MWTRACE(s32 level, const char* format, ...);
    extern const char lbl_8026F818[];
    extern const char lbl_8026F834[];
    extern s32 TRKDoConnect(void*);
    extern s32 TRKDoDisconnect(void*);
    extern s32 fn_800C03B4(void*);
    extern s32 fn_800C035C(void*);
    extern s32 fn_800C0354(void*);
    extern s32 fn_800C034C(void*);
    extern s32 TRKDoReadMemory(void*);
    extern s32 TRKDoWriteMemory(void*);
    extern s32 TRKDoReadRegisters(void*);
    extern s32 SendACK(void*);
    extern s32 TRKDoContinue(void*);
    extern s32 TRKDoStep(void*);
    extern s32 TRKDoStop(void*);
    extern s32 TRKDoSetOption(void*);
    s32 result = 0x500;

    TRKSetBufferPosition(message, 0);
    MWTRACE(1, lbl_8026F818, message[0x14]);
    switch (message[0x14]) {
    case 1:
        result = TRKDoConnect(message);
        break;
    case 2:
        result = TRKDoDisconnect(message);
        break;
    case 3:
        result = fn_800C03B4(message);
        break;
    case 7:
        result = fn_800C035C(message);
        break;
    case 4:
        result = fn_800C0354(message);
        break;
    case 5:
        result = fn_800C034C(message);
        break;
    case 0x10:
        result = TRKDoReadMemory(message);
        break;
    case 0x11:
        result = TRKDoWriteMemory(message);
        break;
    case 0x12:
        result = TRKDoReadRegisters(message);
        break;
    case 0x13:
        result = SendACK(message);
        break;
    case 0x18:
        result = TRKDoContinue(message);
        break;
    case 0x19:
        result = TRKDoStep(message);
        break;
    case 0x1A:
        result = TRKDoStop(message);
        break;
    case 0x17:
        result = TRKDoSetOption(message);
        break;
    }
    MWTRACE(1, lbl_8026F834, result);
    return result;
}

s32 usr_puts_serial(const char* str) {
    extern u32 fn_800C04F4(void);
    extern void fn_800C04E8(u32 state);
    extern void OSReport(char* fmt);
    char buf[2];
    s32 state;
    char ch;
    s32 result;

    result = 0;
    while ((result == 0) && ((ch = *str++) != 0)) {
        state = fn_800C04F4();
        buf[0] = ch;
        buf[1] = 0;
        fn_800C04E8(0);
        OSReport(buf);
        fn_800C04E8(state);
        result = 0;
    }
    return result;
}
