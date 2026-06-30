/**
 * @file gx_texture.h
 * @brief GCN texture format decoder -- converts GCN formats to RGBA8 for OpenGL.
 *
 * GCN textures are stored in a tiled/swizzled layout and use formats not
 * directly supported by OpenGL. This module provides decode functions for
 * each GCN texture format, de-tiling the data and converting to
 * OpenGL-compatible RGBA8 (or compressed DXT1 for CMPR).
 *
 * Supported GCN formats (from gs_texture.h GStextureGetGXFormat switch):
 *   I4, I8, IA4, IA8, RGB565, RGB5A3, RGBA8, CI4, CI8, CMPR
 *
 * References:
 *   - docs/pc_port_design.md Section 5 (Texture Format Translation)
 *   - include/game/gs_texture.h (GStextureHandle, format constants)
 *   - YAGCD (Yet Another GameCube Documentation) -- texture format specs
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_GX_TEXTURE_H
#define PCPORT_GX_TEXTURE_H

#include "gx_shim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Texture tile dimensions per format
 *
 * GCN textures are stored in tiles. The tile size depends on the format:
 *   I4:     8x8 tiles (32 bytes per tile)
 *   I8:     8x4 tiles (32 bytes per tile)
 *   IA4:    8x4 tiles (32 bytes per tile)
 *   IA8:    4x4 tiles (32 bytes per tile)
 *   RGB565: 4x4 tiles (32 bytes per tile)
 *   RGB5A3: 4x4 tiles (32 bytes per tile)
 *   RGBA8:  4x4 tiles (64 bytes per tile -- AR and GB interleaved)
 *   CI4:    8x8 tiles (32 bytes per tile)
 *   CI8:    8x4 tiles (32 bytes per tile)
 *   CMPR:   8x8 macro tiles (4 DXT1 sub-blocks of 4x4 each)
 * ========================================================================= */

/** Texture swizzle modes for fragment shader */
#define GX_TEX_SWIZZLE_RGBA  0  /* Direct RGBA */
#define GX_TEX_SWIZZLE_RRRR  1  /* Intensity (I format) -> replicate R */
#define GX_TEX_SWIZZLE_RRRA  2  /* Intensity+Alpha (IA) -> R=I, A=alpha */
#define GX_TEX_SWIZZLE_111R  3  /* Alpha-only (A8) -> RGB=1, A=value */

/* =========================================================================
 * Decoded texture result
 * ========================================================================= */

typedef struct GXDecodedTexture {
    /** Decoded pixel data (RGBA8, allocated by the decode function) */
    u8* data;

    /** Width and height in texels */
    u16 width;
    u16 height;

    /** Size of the decoded data in bytes */
    u32 dataSize;

    /** OpenGL internal format to use for upload */
    u32 glInternalFormat;   /* e.g., GL_RGBA8, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT */

    /** OpenGL upload format and type */
    u32 glFormat;           /* e.g., GL_RGBA, GL_RED */
    u32 glType;             /* e.g., GL_UNSIGNED_BYTE */

    /** Whether data is compressed (for glCompressedTexImage2D) */
    u8 isCompressed;

    /** Fragment shader swizzle mode for this texture */
    u8 swizzleMode;
} GXDecodedTexture;

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * gx_texture_decode -- Decode a GCN texture to an OpenGL-compatible format.
 *
 * @param srcData     Pointer to the raw GCN texture data (tiled/swizzled).
 * @param width       Texture width in texels.
 * @param height      Texture height in texels.
 * @param format      GCN texture format (GXTexFmt).
 * @param tlutData    Pointer to TLUT palette data (NULL for non-indexed formats).
 * @param tlutFmt     TLUT format (GXTlutFmt, ignored if tlutData is NULL).
 * @param tlutEntries Number of TLUT entries.
 * @param outResult   Pointer to result struct to fill.
 * @return            0 on success, -1 on unsupported format or error.
 *
 * The caller is responsible for freeing outResult->data when done.
 *
 * Decoding pipeline:
 * 1. De-tile: Rearrange from GCN tiled layout to linear row-major order
 * 2. Format convert: Expand to RGBA8 (or DXT1 for CMPR)
 * 3. The result can be uploaded directly via glTexImage2D
 */
s32 gx_texture_decode(const void* srcData, u16 width, u16 height,
                      GXTexFmt format,
                      const void* tlutData, GXTlutFmt tlutFmt,
                      u16 tlutEntries,
                      GXDecodedTexture* outResult);

/**
 * gx_texture_decode_mipmap -- Decode a specific mipmap level.
 *
 * @param srcData     Pointer to the raw GCN mip level data.
 * @param mipWidth    Mip level width.
 * @param mipHeight   Mip level height.
 * @param format      GCN texture format.
 * @param tlutData    TLUT data (for indexed formats).
 * @param tlutFmt     TLUT format.
 * @param tlutEntries Number of TLUT entries.
 * @param outResult   Pointer to result struct to fill.
 * @return            0 on success, -1 on error.
 */
s32 gx_texture_decode_mipmap(const void* srcData, u16 mipWidth, u16 mipHeight,
                             GXTexFmt format,
                             const void* tlutData, GXTlutFmt tlutFmt,
                             u16 tlutEntries,
                             GXDecodedTexture* outResult);

/**
 * gx_texture_free -- Free decoded texture data.
 *
 * @param decoded  Pointer to the decoded texture result struct.
 */
void gx_texture_free(GXDecodedTexture* decoded);

/* =========================================================================
 * Per-format decode functions
 *
 * These are called internally by gx_texture_decode based on the format.
 * They may also be called directly for testing/debugging.
 * ========================================================================= */

/**
 * Decode I4 (4-bit intensity) texture.
 * Tile size: 8x8, output: GL_R8 (swizzle RRRR in shader).
 */
s32 gx_texture_decode_I4(const void* src, u16 w, u16 h,
                         GXDecodedTexture* out);

/**
 * Decode I8 (8-bit intensity) texture.
 * Tile size: 8x4, output: GL_R8 (swizzle RRRR in shader).
 */
s32 gx_texture_decode_I8(const void* src, u16 w, u16 h,
                         GXDecodedTexture* out);

/**
 * Decode IA4 (4-bit intensity + 4-bit alpha) texture.
 * Tile size: 8x4, output: GL_RG8 (swizzle RRRA in shader).
 */
s32 gx_texture_decode_IA4(const void* src, u16 w, u16 h,
                          GXDecodedTexture* out);

/**
 * Decode IA8 (8-bit intensity + 8-bit alpha) texture.
 * Tile size: 4x4, output: GL_RG8 (swizzle RRRA in shader).
 */
s32 gx_texture_decode_IA8(const void* src, u16 w, u16 h,
                          GXDecodedTexture* out);

/**
 * Decode RGB565 (16-bit RGB) texture.
 * Tile size: 4x4, output: GL_RGBA8 (expand to 8 bits per channel).
 */
s32 gx_texture_decode_RGB565(const void* src, u16 w, u16 h,
                             GXDecodedTexture* out);

/**
 * Decode RGB5A3 (16-bit, two-mode: RGB555 opaque or RGBA4443) texture.
 * Tile size: 4x4, output: GL_RGBA8.
 *
 * Format: if MSB=1 -> RGB555 (opaque, A=255)
 *         if MSB=0 -> RGBA4443 (3-bit alpha expanded to 8-bit)
 */
s32 gx_texture_decode_RGB5A3(const void* src, u16 w, u16 h,
                             GXDecodedTexture* out);

/**
 * Decode RGBA8 (32-bit RGBA) texture.
 * Tile size: 4x4 (AR and GB in separate 32-byte cache lines).
 * Output: GL_RGBA8 (de-interleave AR/GB).
 */
s32 gx_texture_decode_RGBA8(const void* src, u16 w, u16 h,
                            GXDecodedTexture* out);

/**
 * Decode CI4 (4-bit color index) texture via TLUT palette.
 * Tile size: 8x8, output: GL_RGBA8 (palette lookup on CPU).
 */
s32 gx_texture_decode_CI4(const void* src, u16 w, u16 h,
                          const void* tlut, GXTlutFmt tlutFmt,
                          GXDecodedTexture* out);

/**
 * Decode CI8 (8-bit color index) texture via TLUT palette.
 * Tile size: 8x4, output: GL_RGBA8 (palette lookup on CPU).
 */
s32 gx_texture_decode_CI8(const void* src, u16 w, u16 h,
                          const void* tlut, GXTlutFmt tlutFmt,
                          GXDecodedTexture* out);

/**
 * Decode CMPR (S3TC/DXT1-like compressed) texture.
 * Tile size: 8x8 (four 4x4 DXT1 sub-blocks with swapped byte order).
 * Output: GL_COMPRESSED_RGBA_S3TC_DXT1_EXT.
 *
 * GCN CMPR is nearly identical to DXT1 but with:
 * - Sub-block order swapped within each 8x8 macro tile
 * - Byte order differences (big-endian -> little-endian swap needed)
 */
s32 gx_texture_decode_CMPR(const void* src, u16 w, u16 h,
                           GXDecodedTexture* out);

/* =========================================================================
 * TLUT (palette) decode helpers
 * ========================================================================= */

/**
 * Decode a single TLUT entry to RGBA8.
 *
 * @param entry   Raw 16-bit TLUT entry (big-endian).
 * @param fmt     TLUT format.
 * @param outR    Output red component.
 * @param outG    Output green component.
 * @param outB    Output blue component.
 * @param outA    Output alpha component.
 */
void gx_tlut_decode_entry(u16 entry, GXTlutFmt fmt,
                          u8* outR, u8* outG, u8* outB, u8* outA);

/**
 * Get the swizzle mode for a given GCN texture format.
 *
 * @param format  GCN texture format.
 * @return        Swizzle mode constant (GX_TEX_SWIZZLE_*).
 */
u8 gx_texture_get_swizzle_mode(GXTexFmt format);

/**
 * Compute the size of a GCN texture in bytes (tiled layout).
 *
 * @param width   Texture width.
 * @param height  Texture height.
 * @param format  GCN texture format.
 * @return        Size in bytes, or 0 for unsupported formats.
 */
u32 gx_texture_compute_size(u16 width, u16 height, GXTexFmt format);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_GX_TEXTURE_H */
