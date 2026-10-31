#include "dolphin/types.h"

/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

/*
 * Runs gbaCmdUpload22 for a 1-based port on the caller's upload block
 * (gbaCommunication.c passes p + 0x4344) and clears the port's retry
 * counter; returns gbaCmdUpload22's result.
 */
s32 gbaPortUpload22(s32 index, const u8* data)
{
    extern s32 gbaCmdUpload22(s32 chan, const u8* data);
    s32 result;

    result = gbaCmdUpload22(index - 1, data);
    lbl_8047A684[index - 1] = 0;
    return result;
}

void gbaCommHandshakeB(s32 index)
{
    extern void gbaCmdHandshakeB(s32);
    gbaCmdHandshakeB(index - 1);
}
