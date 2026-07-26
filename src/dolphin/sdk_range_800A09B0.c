/* Residual middle view of the Dolphin SRAM/system-call range. */
#include "src/dolphin/sdk_range_800A07C4.c"

extern BOOL fn_80098368(s32 chan, void* buffer, s32 length, s32 mode);
extern void WriteSramCallback(s32 chan, OSContext* context);

static inline BOOL WriteSramLocal(void* buffer, u32 offset, u32 size)
{
    BOOL err;
    u32 command;

    if (!EXILock(0, 1, WriteSramCallback)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    command = 0xA0000000 | ((offset << 6) + 0x100);
    err = FALSE;
    err |= !EXIImm(0, &command, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !fn_80098368(0, buffer, size, 1);
    err |= !EXIDeselect(0);
    EXIUnlock(0);
    return !err;
}

BOOL fn_800A09B0(BOOL commit, u32 offset)
{
    SramControl* control = (SramControl*)Scb_803FB840;
    u16* value;

    if (commit) {
        if (offset == 0) {
            if (2 < (control->sram[0x13] & 3)) {
                control->sram[0x13] &= ~3;
            }

            *(u16*)&control->sram[0] = 0;
            *(u16*)&control->sram[2] = 0;
            for (value = (u16*)&control->sram[0x0C];
                 value < (u16*)&control->sram[0x14]; value++) {
                *(u16*)&control->sram[0] += *value;
                *(u16*)&control->sram[2] += ~*value;
            }
        }

        if (offset < control->offset) {
            control->offset = offset;
        }

        control->sync =
            WriteSramLocal(&control->sram[control->offset], control->offset,
                           0x40 - control->offset);
        if (control->sync) {
            control->offset = 0x40;
        }
    }

    control->locked = FALSE;
    OSRestoreInterrupts(control->enabled);
    return control->sync;
}
