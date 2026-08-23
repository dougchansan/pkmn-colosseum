#include "dolphin/types.h"
#include "musyx/runtime/reverb.h"

extern u32 ReverbHICreate(_SND_REVHI_WORK* obj, f32 f1, f32 f2, f32 f3, f32 f4,
                          f32 f5, f32 f6);

void sndAuxCallbackPrepareReverbHI(u8* ptr)
{
    ptr[0x1C4] = 0;
    ReverbHICreate(
        (_SND_REVHI_WORK*)ptr,
        *(f32*)(ptr + 0x1C8),
        *(f32*)(ptr + 0x1D0),
        *(f32*)(ptr + 0x1CC),
        *(f32*)(ptr + 0x1D4),
        *(f32*)(ptr + 0x1D8),
        *(f32*)(ptr + 0x1DC)
    );
}
