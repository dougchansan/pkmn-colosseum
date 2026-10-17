#include "dolphin/types.h"

extern u32 SamplingRate_8047AA60;
extern void SISetSamplingRate(u32 rate);

void SIRefreshSamplingRate(void) {
    SISetSamplingRate(SamplingRate_8047AA60);
}
