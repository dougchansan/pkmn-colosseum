/**
 * @file sdk_range_800BF33C.c
 * @brief dolphin-sdk code, 0x800BF33C - 0x800BF534 (2 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 *
 * Linked (lane D18) with TRKDispatchMessage's switch table
 * (.data 0x80313770 - 0x803137DC).
 */
#include "dolphin/types.h"

/* usr_puts_serial (0x800BF33C) comes first in the binary, then
 * TRKDispatchMessage (0x800BF3C4). */
s32 usr_puts_serial(const char* str) {
    extern u32 GetTRKConnected(void);
    extern void SetTRKConnected(u32 state);
    extern void OSReport(char* fmt);
    char buf[2];
    s32 state;
    char ch;
    s32 result;

    result = 0;
    while ((result == 0) && ((ch = *str++) != 0)) {
        state = GetTRKConnected();
        buf[0] = ch;
        buf[1] = 0;
        SetTRKConnected(0);
        OSReport(buf);
        SetTRKConnected(state);
        result = 0;
    }
    return result;
}

s32 TRKDispatchMessage(u8* message)
{
    extern void TRKSetBufferPosition(void* message, u32 position);
    extern void MWTRACE(s32 level, const char* format, ...);
    extern const char lbl_8026F818[];
    extern const char lbl_8026F834[];
    extern s32 TRKDoConnect(void*);
    extern s32 TRKDoDisconnect(void*);
    extern s32 TRKDoReset(void*);
    extern s32 TRKDoOverride(void*);
    extern s32 TRKDoVersions(void*);
    extern s32 TRKDoSupportMask(void*);
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
        result = TRKDoReset(message);
        break;
    case 7:
        result = TRKDoOverride(message);
        break;
    case 4:
        result = TRKDoVersions(message);
        break;
    case 5:
        result = TRKDoSupportMask(message);
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
