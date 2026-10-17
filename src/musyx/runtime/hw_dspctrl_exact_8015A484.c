/**
 * @file hw_dspctrl_exact_8015A484.c
 * @brief MusyX hw_dspctrl.c salInitDspCtrl, 0x8015A484 - 0x8015A838.
 *
 * The reference MusyX runtime's salInitDspCtrl (2.0.0 form, salMalloc for
 * every buffer). Retail indexes the voice and studio arrays through the
 * globals on every access (lbl_8047B024 = lbl_8047B024, dspStudio =
 * lbl_80447E60) rather than through cached pointers. No data; the state it
 * sets stays extern.
 */
#include "musyx/runtime/hw_dspctrl.h"

typedef u32 SND_STUDIO_TYPE;

extern u32 lbl_8047AFF0;      /* dspARAMZeroBuffer */
extern void* salMalloc(u32 size);   /* salMalloc */
extern u32 aramGetZeroBuffer(void);         /* aramGetZeroBuffer */
extern void DCFlushRange(void* addr, u32 size);
extern void DCInvalidateRange(void* addr, u32 size);
extern void DCStoreRangeNoSync(void* addr, u32 size);
extern void salActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type);
extern void salInitHRTFBuffer(void);

u32 salInitDspCtrl(u8 numVoices, u8 numStudios, u32 defaultStudioDPL2)
{
    u32 i;
    u32 j;
    u32 itdPtr;

    lbl_8047B05D = numVoices;
    lbl_8047B05C = numStudios;

    lbl_8047AFF0 = aramGetZeroBuffer();
    if ((lbl_8047B010 = salMalloc(1024 * sizeof(u16)))) {
        if ((lbl_8047B01C = salMalloc(160 * sizeof(s32)))) {
            memset(lbl_8047B01C, 0, 160 * sizeof(s32));
            DCFlushRange(lbl_8047B01C, 160 * sizeof(s32));
            if ((lbl_8047B024 = salMalloc(lbl_8047B05D * sizeof(DSPvoice)))) {
                if ((lbl_8047B020 = salMalloc(lbl_8047B05D * 64))) {
                    DCInvalidateRange(lbl_8047B020, lbl_8047B05D * 64);
                    itdPtr = (u32)lbl_8047B020;
                    for (i = 0; i < lbl_8047B05D; ++i) {
                        lbl_8047B024[i].state = 0;
                        lbl_8047B024[i].postBreak = 0;
                        lbl_8047B024[i].startupBreak = 0;
                        lbl_8047B024[i].lastUpdate.pitch = 0xFF;
                        lbl_8047B024[i].lastUpdate.vol = 0xFF;
                        lbl_8047B024[i].lastUpdate.volA = 0xFF;
                        lbl_8047B024[i].lastUpdate.volB = 0xFF;
                        lbl_8047B024[i].pb = salMalloc(sizeof(_PB));
                        memset(lbl_8047B024[i].pb, 0, sizeof(_PB));
                        lbl_8047B024[i].patchData = salMalloc(0x80);
                        lbl_8047B024[i].pb->currHi = ((u32)lbl_8047B024[i].pb >> 16);
                        lbl_8047B024[i].pb->currLo = (u16)(u32)lbl_8047B024[i].pb;
                        lbl_8047B024[i].pb->update.dataHi = ((u32)lbl_8047B024[i].patchData >> 16);
                        lbl_8047B024[i].pb->update.dataLo = ((u16)(u32)lbl_8047B024[i].patchData);
                        lbl_8047B024[i].pb->itd.bufferHi = ((u32)itdPtr >> 16);
                        lbl_8047B024[i].pb->itd.bufferLo = ((u16)itdPtr);
                        lbl_8047B024[i].itdBuffer = (void*)itdPtr;
                        itdPtr += 0x40;
                        lbl_8047B024[i].virtualSampleID = 0xFFFFFFFF;
                        DCStoreRangeNoSync(lbl_8047B024[i].pb, sizeof(_PB));
                        for (j = 0; j < 5; ++j) {
                            lbl_8047B024[i].changed[j] = 0;
                        }
                    }

                    for (i = 0; i < lbl_8047B05C; ++i) {
                        lbl_80447E60[i].state = 0;
                        if (!(lbl_80447E60[i].spb = salMalloc(0x36 /* sizeof(_SPB) */))) {
                            return FALSE;
                        }

                        if (!(lbl_80447E60[i].main[0] = salMalloc(0x3C00))) {
                            return FALSE;
                        }

                        memset(lbl_80447E60[i].main[0], 0, 0x3C00);
                        DCFlushRangeNoSync(lbl_80447E60[i].main[0], 0x3C00);
                        lbl_80447E60[i].main[1] = lbl_80447E60[i].main[0] + 0x1E0;
                        lbl_80447E60[i].auxA[0] = lbl_80447E60[i].main[1] + 0x1E0;
                        lbl_80447E60[i].auxA[1] = lbl_80447E60[i].auxA[0] + 0x1E0;
                        lbl_80447E60[i].auxA[2] = lbl_80447E60[i].auxA[1] + 0x1E0;
                        lbl_80447E60[i].auxB[0] = lbl_80447E60[i].auxA[2] + 0x1E0;
                        lbl_80447E60[i].auxB[1] = lbl_80447E60[i].auxB[0] + 0x1E0;
                        lbl_80447E60[i].auxB[2] = lbl_80447E60[i].auxB[1] + 0x1E0;
                        memset(lbl_80447E60[i].spb, 0, 0x36 /* sizeof(_SPB) */);
                        lbl_80447E60[i].hostDPopSum.l = lbl_80447E60[i].hostDPopSum.r =
                            lbl_80447E60[i].hostDPopSum.s = 0;
                        lbl_80447E60[i].hostDPopSum.lA = lbl_80447E60[i].hostDPopSum.rA =
                            lbl_80447E60[i].hostDPopSum.sA = 0;
                        lbl_80447E60[i].hostDPopSum.lB = lbl_80447E60[i].hostDPopSum.rB =
                            lbl_80447E60[i].hostDPopSum.sB = 0;
                        DCFlushRangeNoSync(lbl_80447E60[i].spb, 0x36);
                    }
                    salActivateStudio(0, 1, defaultStudioDPL2 != FALSE ? 1 : 0);
                    if (!(lbl_8047B018 = salMalloc(0x100))) {
                        return FALSE;
                    }

                    salInitHRTFBuffer();
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
