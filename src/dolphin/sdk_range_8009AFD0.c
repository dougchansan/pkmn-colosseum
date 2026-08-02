/**
 * @file sdk_range_8009AFD0.c
 * @brief dolphin-sdk code, 0x8009AFD0 - 0x8009B290 (3 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef void OSArenaAddress;

extern OSArenaAddress* __OSArenaLo;
volatile u16 AudioDSPRegs[32] : 0xCC005000;

#if !defined(SDK_STOP_AUDIO_ONLY)

void* OSAllocFromArenaLo(u32 size, u32 align) {
    u32 am1 = align - 1;
    u32 mask = ~am1;
    OSArenaAddress* ptr;

    ptr = (OSArenaAddress*)(((u32)__OSArenaLo + am1) & mask);
    size = (u32)ptr + size;
    size += am1;
    __OSArenaLo = (OSArenaAddress*)(mask & size);
    return ptr;
}

#endif

void __OSStopAudioSystem(void) {
    u16 reg16;
    u32 start;

    extern u32 OSGetTick(void);

    AudioDSPRegs[5] = 0x804;
    reg16 = AudioDSPRegs[27];
    AudioDSPRegs[27] = reg16 & ~0x8000;

    reg16 = AudioDSPRegs[5];
    while (reg16 & 0x400) {
        reg16 = AudioDSPRegs[5];
    }
    reg16 = AudioDSPRegs[5];
    while (reg16 & 0x200) {
        reg16 = AudioDSPRegs[5];
    }

    AudioDSPRegs[5] = 0x8AC;
    AudioDSPRegs[0] = 0;
    while ((((u32)AudioDSPRegs[2] << 16) | AudioDSPRegs[3]) & 0x80000000) {
    }

    start = OSGetTick();
    while ((s32)(OSGetTick() - start) < 44) {
    }

    reg16 = AudioDSPRegs[5];
    AudioDSPRegs[5] = reg16 | 1;
    reg16 = AudioDSPRegs[5];
    while (reg16 & 1) {
        reg16 = AudioDSPRegs[5];
    }
}
