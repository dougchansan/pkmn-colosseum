#include "dolphin/types.h"

extern u32 SIGetType(s32 channel);
extern u32 SIDecodeType(u32 type);

u32 SIProbe(s32 channel) {
    return SIDecodeType(SIGetType(channel));
}
