#ifndef GAME_GS_TEXTURE_INIT_GX_H
#define GAME_GS_TEXTURE_INIT_GX_H

/* The GX object setup shared by GStextureCreate and GStextureLoad
 * (find_inline_expansions.py block 0x800EFE00 0x800EFFA0 scores 0.981 at
 * GStextureCreate 0x800EFB60, same two calls), with the GStextureGetGXformat
 * expansion it carries. Included by the GStexture candidate and its carve. */

#include "dolphin/types.h"
#include "game/gs_texture.h"

extern void GXInitTlutObj(void* tlutObj, void* data, u32 format, u32 entries); /* GXInitTlutObj */
extern void GXInitTexObj(void* texObj, void* data, u16 width, u16 height, u32 format,
                        u32 wrapS, u32 wrapT, u32 mipmap);                   /* GXInitTexObj */

/*
 * GStextureGetGXformat (0x800EF3E0, gs_texture_getters_exact_800EF3E0.c) is
 * defined earlier in retail GStexture.cpp and expanded inline in both
 * GStextureCreate and GStextureLoad: each carries its exact compare tree with
 * the 0xA0 case folded to 0x01, i.e. the call GStextureGetGXformat(tex, 1).
 * This is a C99 inline definition, so it emits no symbol here; the external
 * definition stays in the getters unit. The const-qualified parameter is what
 * makes MWCC read tex->format again for the TLUT switch that follows, as both
 * retail expansions do (with a plain GStextureHandle* the two reads merge).
 */
inline s32 GStextureGetGXformat(const GStextureHandle* tex, u8 alpha)
{
    u32 format = tex->format;

    switch (format) {
    case 0x00:
        return 0x08;
    case 0x01:
        return 0x09;
    case 0x30:
        return 0x0A;
    case 0x40:
        return 0x00;
    case 0x41:
        return 0x02;
    case 0x42:
        return 0x01;
    case 0x43:
        return 0x03;
    case 0x44:
        return 0x04;
    case 0x45:
        return 0x06;
    case 0x90:
        return 0x05;
    case 0xB0:
        return 0x0E;
    case 0xA0:
        if (alpha != 0) {
            return 0x01;
        }
        return 0x27;
    default:
        return -1;
    }
}


/*
 * Shared tail of GStextureCreate and GStextureLoad: the target expands the
 * same GX format / TLUT setup sequence in both functions.
 */
static inline void textureInitGXObjects(GStextureHandle* tex)
{
    s32 gxFormat;
    u32 tlutEntries;
    u32 tlutFormat;

    tlutEntries = 0;
    gxFormat = GStextureGetGXformat(tex, 1);

    if (tex->tlutData != NULL) {
        switch (tex->format) {
        case 0x00:
            tlutEntries = 0x10;
            break;
        case 0x01:
            tlutEntries = 0x100;
            break;
        case 0x30:
            tlutEntries = 0x400;
            break;
        }

        /* Retail leaves the GX TLUT format unset for an unknown TLUT format. */
        switch (tex->tlutFormat) {
        case 1:
            tlutFormat = 0;
            break;
        case 2:
            tlutFormat = 1;
            break;
        case 3:
            tlutFormat = 2;
            break;
        }

        GXInitTlutObj(tex->gxTlutObj, tex->tlutData, tlutFormat, tlutEntries);
    }

    GXInitTexObj(tex->gxTexObj, tex->mipData[0], tex->width, tex->height, gxFormat, 0, 0,
                tex->mipLevels > 1);
    tex->dirty = 1;
}

#endif /* GAME_GS_TEXTURE_INIT_GX_H */
