/**
 * @file trk_range_800C4470.c
 * @brief trk code, 0x800C4470 - 0x800C459C (4 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

extern s32 OSDisableInterrupts(void);
extern s32 OSRestoreInterrupts(u32 state);

typedef struct CircleBuffer CircleBuffer;

/* gdev_cc_initialize - 0x800C4470 | size: 0x88 | scope global */
s32 gdev_cc_initialize(u8** comm, void (*callback)(s32)) {
    extern const char lbl_8026FE2C[];
    extern const char lbl_8026FE40[];
    extern u8 lbl_803FF598[];
    extern CircleBuffer lbl_803FFA98;
    extern void MWTRACE(s32 level, const char* format, ...);
    extern void DBInitComm(u8** comm, void (*callback)(s32));
    extern void CircleBufferInitialize(CircleBuffer* circle, u8* buffer, u32 size);

    MWTRACE(1, lbl_8026FE2C);
    DBInitComm(comm, callback);
    MWTRACE(1, lbl_8026FE40);
    CircleBufferInitialize(&lbl_803FFA98, lbl_803FF598, 0x500);
    return 0;
}

/* MWTRACE - 0x800C44F8 | size: 0x50 | scope global */
void MWTRACE(s32 level, const char* format, ...) {
}

/* TRKReleaseMutex_800C4548 - 0x800C4548 | size: 0x24 | scope global */
void TRKReleaseMutex_800C4548(u32* state) {
    OSRestoreInterrupts(*state);
}

/* TRKAcquireMutex_800C456C - 0x800C456C | size: 0x30 | scope global */
void TRKAcquireMutex_800C456C(u32* state) {
    *state = OSDisableInterrupts();
}
