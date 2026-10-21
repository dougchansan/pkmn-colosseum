/**
 * @file gs_texture_exact_800EFD14.c
 * @brief GStextureClaimHandle / GStextureLoad, 0x800EFD14 - 0x800EFFC0.
 *
 * Function-boundary carve of the GStexture candidate
 * (gs_texture_candidate_800EF5FC.c): no jump table (GStextureGetGXformat's
 * switch expands to a compare tree), no pooled constant, no data. The GX
 * setup is the repeated-expansion helper shared with GStextureCreate
 * (game/gs_texture_init_gx.h). GC/1.3 -O4,p like the TU, no pragmas.
 */
#include "game/gs_texture_init_gx.h"

/* Claim tex for the memory handle unless it is already in use. */
void GStextureClaimHandle(GStextureHandle* tex, u16 handle)
{
    if (tex == NULL || tex->inUse != 0) {
        return;
    }
    tex->inUse = 1;
    tex->memHandle = handle;
}

/* Relocate a texture image loaded from a file and build its GX objects. */
GStextureHandle* GStextureLoad(GStextureHandle* tex)
{
    s32 i;

    for (i = 0; i < (s32)tex->mipLevels; i++) {
        tex->mipData[i] = (u8*)tex + (u32)tex->mipData[i];
    }
    for (i = tex->mipLevels; i < 8; i++) {
        tex->mipData[i] = NULL;
    }
    if (tex->tlutData != NULL) {
        tex->tlutData = (u8*)tex + (u32)tex->tlutData;
    }

    textureInitGXObjects(tex);
    tex->memHandle = 0;
    return tex;
}
