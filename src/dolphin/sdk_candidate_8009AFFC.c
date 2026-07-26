/* Residual Dolphin audio-system range. */
#include "dolphin/types.h"

extern u8 DSPInitCode[128];
extern void* OSGetArenaHi(void);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void DCFlushRange(void* address, u32 size);
extern u32 OSGetTick(void);

void __OSInitAudioSystem(void)
{
    volatile u16* regs = (volatile u16*)0xCC005000;
    u16 status;
    u32 start;

    memcpy((u8*)OSGetArenaHi() - sizeof(DSPInitCode), (void*)0x81000000,
           sizeof(DSPInitCode));
    memcpy((void*)0x81000000, DSPInitCode, sizeof(DSPInitCode));
    DCFlushRange((void*)0x81000000, sizeof(DSPInitCode));

    regs[9] = 0x43;
    regs[5] = 0x8AC;
    regs[5] |= 1;
    while (regs[5] & 1) {
    }
    regs[0] = 0;
    while ((((u32)regs[2] << 16) | regs[3]) & 0x80000000) {
    }

    *(volatile u32*)&regs[16] = 0x01000000;
    *(volatile u32*)&regs[18] = 0;
    *(volatile u32*)&regs[20] = 0x20;

    status = regs[5];
    while (!(status & 0x20)) {
        status = regs[5];
    }
    regs[5] = status;

    start = OSGetTick();
    while ((s32)(OSGetTick() - start) < 0x892) {
    }

    *(volatile u32*)&regs[16] = 0x01000000;
    *(volatile u32*)&regs[18] = 0;
    *(volatile u32*)&regs[20] = 0x20;

    status = regs[5];
    while (!(status & 0x20)) {
        status = regs[5];
    }
    regs[5] = status;

    regs[5] &= ~0x800;
    while (regs[5] & 0x400) {
    }
    regs[5] &= ~4;

    status = regs[2];
    while (!(status & 0x8000)) {
        status = regs[2];
    }
    (void)(((u32)status << 16) | regs[3]);

    regs[5] |= 4;
    regs[5] = 0x8AC;
    regs[5] |= 1;
    while (regs[5] & 1) {
    }

    memcpy((void*)0x81000000, (u8*)OSGetArenaHi() - sizeof(DSPInitCode),
           sizeof(DSPInitCode));
}

#include "src/dolphin/sdk_range_8009AFD0.c"
