/**
 * @file fsys_file_candidate_8017C414.c
 * @brief FSYS chunked-read step (0x8017C414 - 0x8017C568).
 *
 * Steps the chunked DVD read (fn_8017A624); when the read is complete,
 * advances the active slot's status exactly as the fn_8017A814/fn_8017A95C
 * completion callbacks do and closes its external file.
 *
 * Standalone level-0 code like the rest of fsys (see configure.py; it used
 * to be scored from fsys_file_candidates.c under a local optimization
 * pragma, 93.9%). Exact: the r4/r5 wall it shares with the two callbacks
 * (retail builds the lbl_80453FEC address in r5) comes from
 * OSDisableInterrupts' result being a 64-bit value whose dead low word keeps
 * r4 live; see docs/recon/fsys_8017C414_wall.md.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSManager lbl_80453FEC;
extern s32 fn_8017A624(FSYSSlot* slot);
/*
 * RULE-EXCEPTION(title-path): mismatched SDK prototype, register allocation only
 * -- see docs/RULE_EXCEPTIONS.md. Retail treats OSDisableInterrupts' result as
 * a 64-bit value and keeps its high word (r3): the dead low word stays live in
 * r4 past the call, so the lbl_80453FEC address is built in r5, as in retail.
 */
extern s64 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void GSdvdClose(void* file);

/* Address: 0x8017C414 | size: 0x154 */
s32 fsysSlotStatusFinishRead(FSYSSlot* request)
{
    FSYSSlot* slot;
    u32 enabled;

    /* RULE-EXCEPTION(title-path): dead read, register allocation only -- see
     * docs/RULE_EXCEPTIONS.md (second reference: retail keeps the argument
     * in r29). */
    (void)request;
    if (fn_8017A624(request)) {
        enabled = (u32)(OSDisableInterrupts() >> 32);
        slot = lbl_80453FEC.activeSlot;
        switch (slot->status) {
        case 1: slot->status = 2; break;
        case 3: slot->status = 4; break;
        case 0x65: slot->status = 0x96; break;
        case 0xC8: slot->status = 0xC9; break;
        case 0x12F: slot->status = 0x130; break;
        case 0x190: slot->status = 0x191; break;
        case 2: case 4: case 0x64: case 0xC9: case 0x12D: break;
        default: slot->status = 0x3E8; slot->archiveHandle = 1; break;
        }
        if (slot->tocBuffer) {
            GSdvdClose(slot->tocBuffer);
            slot->tocBuffer = NULL;
        }
        OSRestoreInterrupts(enabled);
    }
    return 1;
}
