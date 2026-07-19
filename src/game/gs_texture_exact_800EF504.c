/**
 * @file gs_texture_exact_800EF504.c
 * @brief Exact texture image unlock, 0x800EF504 - 0x800EF548.
 */

#include "dolphin/types.h"
#include "dolphin/os/OSCache.h"
#include "game/gs_texture.h"

extern void GXInvalidateTexAll(void);

#pragma push
#pragma scheduling off
u32 GStextureUnlockImage(GStextureHandle* tex)
{
    DCFlushRange(tex->mipData[0], tex->totalSize);
    GXInvalidateTexAll();

    return tex->refCount--;
}
#pragma pop
