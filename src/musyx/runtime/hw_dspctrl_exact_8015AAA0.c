#include "musyx/runtime/hw_dspctrl.h"

void salDeactivateStudio(u32 studio)
{
    lbl_80447E60[(u8) studio].state = 0;
}

u32 salCheckVolErrorAndResetDelta(u16* dspVol, u16* dspDelta,
                                  u16* lastVol, u16 targetVol,
                                  u16* resetFlags, u16 resetMask)
{
    s16 delta;
    s16 steps;

    if (targetVol != *lastVol) {
        delta = (s16) targetVol - (s16) *lastVol;
        if ((s16) delta >= 32 && (s16) delta < 160) {
            steps = (s16) delta >> 5;
            if ((s16) steps < 5) {
                resetFlags[steps] |= resetMask;
            }
            *dspDelta = 1;
            *lastVol += steps << 5;
            return 1;
        }
        if (-32 >= (s16) delta && -160 < (s16) delta) {
            steps = -(s16) delta >> 5;
            if (steps < 5) {
                resetFlags[steps] |= resetMask;
            }
            *dspDelta = 0xFFFF;
            *lastVol -= steps << 5;
            return 1;
        }
        if (targetVol == 0 && (s16) delta > -32) {
            *dspVol = *lastVol = 0;
        }
    }
    *dspDelta = 0;
    return 0;
}

void sal_setup_dspvol(u16* dspDelta, u16* lastVol, u16 volume)
{
    *dspDelta = ((s16) volume - (s16) *lastVol) / 160;
    *lastVol += (s16) *dspDelta * 160;
}

void sal_update_hostplayinfo(DSPvoice* voice)
{
    u32 pitch;
    u32 oldLo;

    if (voice->smp_info.loopLength != 0) {
        return;
    }
    if (voice->pb->srcSelect != 2) {
        pitch = voice->playInfo.pitch << 5;
    } else {
        pitch = 0x200000;
    }
    oldLo = voice->playInfo.posLo;
    voice->playInfo.posLo += pitch << 16;
    if (oldLo > voice->playInfo.posLo) {
        voice->playInfo.posHi += (pitch >> 16) + 1;
    } else {
        voice->playInfo.posHi += pitch >> 16;
    }
}

void DoDepopFade(s32* dspStart, s16* dspDelta, s32* hostSum)
{
    s16 delta;

    if (*hostSum <= -160) {
        if (*hostSum <= -3200) {
            delta = 20;
        } else {
            delta = (s16) (-*hostSum / 160);
        }
        *dspDelta = delta;
    } else if (*hostSum >= 160) {
        if (*hostSum >= 3200) {
            delta = -20;
        } else {
            delta = (s16) (-*hostSum / 160);
        }
        *dspDelta = delta;
    } else {
        *dspDelta = 0;
    }
    *dspStart = *hostSum;
    *hostSum += *dspDelta * 160;
}
