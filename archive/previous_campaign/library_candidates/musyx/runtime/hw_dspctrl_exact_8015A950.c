/**
 * @file hw_dspctrl_exact_8015A950.c
 * @brief MusyX hw_dspctrl.c salActivateStudio, 0x8015A950 - 0x8015AAA0.
 *
 * The reference MusyX runtime's salActivateStudio: retail re-derives
 * &dspStudio[studio] (lbl_80447E60) after every call instead of caching a
 * pointer, as the reference's direct dspStudio[studio] accesses do.
 */
#include "musyx/runtime/hw_dspctrl.h"

typedef u32 SND_STUDIO_TYPE;

void salActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type)
{
    memset(lbl_80447E60[studio].main[0], 0, 0x3C00);
    DCFlushRangeNoSync(lbl_80447E60[studio].main[0], 0x3C00);
    memset(lbl_80447E60[studio].spb, 0, 0x36 /* sizeof(_SPB) */);
    lbl_80447E60[studio].hostDPopSum.l = lbl_80447E60[studio].hostDPopSum.r =
        lbl_80447E60[studio].hostDPopSum.s = 0;
    lbl_80447E60[studio].hostDPopSum.lA = lbl_80447E60[studio].hostDPopSum.rA =
        lbl_80447E60[studio].hostDPopSum.sA = 0;
    lbl_80447E60[studio].hostDPopSum.lB = lbl_80447E60[studio].hostDPopSum.rB =
        lbl_80447E60[studio].hostDPopSum.sB = 0;
    DCFlushRangeNoSync(lbl_80447E60[studio].spb, 0x36 /* sizeof(_SPB) */);
    memset(lbl_80447E60[studio].auxA[0], 0, 0x780);
    DCFlushRangeNoSync(lbl_80447E60[studio].auxA[0], 0x780);
    memset(lbl_80447E60[studio].auxB[0], 0, 0x780);
    DCFlushRangeNoSync(lbl_80447E60[studio].auxB[0], 0x780);
    lbl_80447E60[studio].voiceRoot = NULL;
    lbl_80447E60[studio].alienVoiceRoot = NULL;
    lbl_80447E60[studio].state = 1;
    lbl_80447E60[studio].isMaster = isMaster;
    lbl_80447E60[studio].numInputs = 0;
    lbl_80447E60[studio].type = type;
    lbl_80447E60[studio].auxAHandler = lbl_80447E60[studio].auxBHandler = NULL;
}
