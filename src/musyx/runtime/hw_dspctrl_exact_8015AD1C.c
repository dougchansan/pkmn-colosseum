/**
 * @file hw_dspctrl_exact_8015AD1C.c
 * @brief MusyX hw_dspctrl.c HandleDepopVoice and SortVoices,
 *        0x8015AD1C - 0x8015B250.
 *
 * AddDpop is the reference's static helper; retail expands it nine times
 * in HandleDepopVoice (HandleDepopVoice). salBuildCommandList (0x8015B250)
 * follows in hw_dspctrl_exact_8015B250.c.
 */
#include "musyx/runtime/hw_dspctrl.h"

static inline void AddDpop(s32* sum, s16 delta) {
    *sum += delta;
    *sum = (*sum > 0x7FFFFF)
               ? 0x7FFFFF
               : (*sum < -0x7FFFFF ? -0x7FFFFF : *sum);
}

void HandleDepopVoice(DSPstudioinfo* stp, DSPvoice* dsp_vptr) {
    _PB* pb;

    dsp_vptr->postBreak = 0;
    dsp_vptr->pb->state = 0;
    pb = dsp_vptr->pb;

    AddDpop(&stp->hostDPopSum.l, (s16)pb->dpop.aL);
    AddDpop(&stp->hostDPopSum.r, (s16)pb->dpop.aR);

    if ((pb->mixerCtrl & 0x04) != 0) {
        AddDpop(&stp->hostDPopSum.s, (s16)pb->dpop.aS);
    }

    if ((pb->mixerCtrl & 0x01) != 0) {
        AddDpop(&stp->hostDPopSum.lA, (s16)pb->dpop.aAuxAL);
        AddDpop(&stp->hostDPopSum.rA, (s16)pb->dpop.aAuxAR);

        if ((pb->mixerCtrl & 0x14) != 0) {
            AddDpop(&stp->hostDPopSum.sA, (s16)pb->dpop.aAuxAS);
        }
    }

    if ((pb->mixerCtrl & 0x12) != 0) {
        AddDpop(&stp->hostDPopSum.lB, (s16)pb->dpop.aAuxBL);
        AddDpop(&stp->hostDPopSum.rB, (s16)pb->dpop.aAuxBR);

        if ((pb->mixerCtrl & 0x04) != 0) {
            AddDpop(&stp->hostDPopSum.sB, (s16)pb->dpop.aAuxBS);
        }
    }
}

void SortVoices(DSPvoice** voices, s32 l, s32 r) {
    s32 i;
    s32 last;
    DSPvoice* tmp;

    if (l >= r) {
        return;
    }

    tmp = voices[l];
    voices[l] = voices[(l + r) / 2];
    voices[(l + r) / 2] = tmp;
    last = l;
    i = l + 1;

    for (; i <= r; ++i) {
        if (voices[i]->prio < voices[l]->prio) {
            last += 1;
            tmp = voices[last];
            voices[last] = voices[i];
            voices[i] = tmp;
        }
    }

    tmp = voices[l];
    voices[l] = voices[last];
    voices[last] = tmp;
    SortVoices(voices, l, last - 1);
    SortVoices(voices, last + 1, r);
}

