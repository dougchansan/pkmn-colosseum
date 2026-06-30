/**
 * @file gx_texture.c
 * @brief GCN texture format decoder -- stub implementations.
 *
 * Decodes GCN tiled/swizzled texture data into linear RGBA8 suitable for
 * upload to OpenGL via glTexImage2D.
 *
 * References:
 *   - docs/pc_port_design.md Section 5 (Texture Format Translation)
 *   - YAGCD -- GCN texture format documentation
 *   - Dolphin Emulator TextureDecoder_Common.cpp for reference
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "gx_texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* OpenGL constants (used in result structs before GL headers available) */
#define GL_R8                     0x8229
#define GL_RG8                    0x822B
#define GL_RGBA8                  0x8058
#define GL_RED                    0x1903
#define GL_RG                     0x8227
#define GL_RGBA                   0x1908
#define GL_UNSIGNED_BYTE          0x1401
#define GL_UNSIGNED_SHORT_5_6_5   0x8363
#define GL_RGB565_CONST           0x8D62
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1

/* =========================================================================
 * Helper: big-endian 16-bit read
 * ========================================================================= */
static u16 read_be16(const u8* p) {
    return (u16)((p[0] << 8) | p[1]);
}

static u32 read_be32(const u8* p) {
    return ((u32)p[0] << 24) |
           ((u32)p[1] << 16) |
           ((u32)p[2] << 8) |
           (u32)p[3];
}

static u8 expand_5_to_8(u32 value) {
    return (u8)((value << 3) | (value >> 2));
}

static u8 expand_6_to_8(u32 value) {
    return (u8)((value << 2) | (value >> 4));
}

static void decode_rgb565(u16 value, u8 outRgba[4]) {
    outRgba[0] = expand_5_to_8((value >> 11) & 0x1Fu);
    outRgba[1] = expand_6_to_8((value >> 5) & 0x3Fu);
    outRgba[2] = expand_5_to_8(value & 0x1Fu);
    outRgba[3] = 0xFF;
}

static void decode_dxt1_block(const u8* srcBlock,
                              u8* dstRgba,
                              u32 dstStride) {
    u16 color0 = read_be16(srcBlock + 0);
    u16 color1 = read_be16(srcBlock + 2);
    u32 indices = read_be32(srcBlock + 4);
    u8 palette[4][4];
    u32 y;
    u32 x;

    decode_rgb565(color0, palette[0]);
    decode_rgb565(color1, palette[1]);

    if (color0 > color1) {
        for (x = 0; x < 3u; ++x) {
            palette[2][x] =
                (u8)((((u32)palette[0][x] * 2u) + (u32)palette[1][x]) / 3u);
            palette[3][x] =
                (u8)(((u32)palette[0][x] + ((u32)palette[1][x] * 2u)) / 3u);
        }
        palette[2][3] = 0xFF;
        palette[3][3] = 0xFF;
    } else {
        for (x = 0; x < 3u; ++x) {
            palette[2][x] =
                (u8)((((u32)palette[0][x]) + (u32)palette[1][x]) / 2u);
            palette[3][x] = 0u;
        }
        palette[2][3] = 0xFF;
        palette[3][3] = 0x00;
    }

    for (y = 0; y < 4u; ++y) {
        for (x = 0; x < 4u; ++x) {
            /* GameCube/S3TC packs the 4x4 selector word MSB-first: pixel (0,0)
             * uses the top two bits. (Reading LSB-first diagonally flips each
             * block -- invisible on gradients, but speckles sharp edges.) */
            u32 code = (indices >> (2u * (15u - ((y * 4u) + x)))) & 0x3u;
            u8* dst = dstRgba + (y * dstStride) + (x * 4u);

            dst[0] = palette[code][0];
            dst[1] = palette[code][1];
            dst[2] = palette[code][2];
            dst[3] = palette[code][3];
        }
    }
}

/* =========================================================================
 * TLUT (palette) decode
 * ========================================================================= */

void gx_tlut_decode_entry(u16 entry, GXTlutFmt fmt,
                          u8* outR, u8* outG, u8* outB, u8* outA) {
    /* Decode a single 16-bit big-endian TLUT palette entry to RGBA bytes.
     * Mirrors decode_title.py tlut_rgba() / _rgb565() / _rgb5a3(). */
    switch (fmt) {
        case GX_TL_IA8: {
            u8 i = (u8)((entry >> 8) & 0xFFu);
            *outR = i; *outG = i; *outB = i;
            *outA = (u8)(entry & 0xFFu);
            break;
        }
        case GX_TL_RGB565:
            *outR = expand_5_to_8((entry >> 11) & 0x1Fu);
            *outG = expand_6_to_8((entry >> 5) & 0x3Fu);
            *outB = expand_5_to_8(entry & 0x1Fu);
            *outA = 0xFF;
            break;
        case GX_TL_RGB5A3:
        default:
            if (entry & 0x8000u) {
                /* RGB555, opaque */
                *outR = expand_5_to_8((entry >> 10) & 0x1Fu);
                *outG = expand_5_to_8((entry >> 5) & 0x1Fu);
                *outB = expand_5_to_8(entry & 0x1Fu);
                *outA = 0xFF;
            } else {
                /* RGB4A3 */
                u32 a3 = (entry >> 12) & 0x07u;
                *outR = (u8)(((entry >> 8) & 0x0Fu) * 0x11u);
                *outG = (u8)(((entry >> 4) & 0x0Fu) * 0x11u);
                *outB = (u8)((entry & 0x0Fu) * 0x11u);
                *outA = (u8)((a3 << 5) | (a3 << 2) | (a3 >> 1));
            }
            break;
    }
}

u8 gx_texture_get_swizzle_mode(GXTexFmt format) {
    switch (format) {
        case GX_TF_I4:
        case GX_TF_I8:
            return GX_TEX_SWIZZLE_RRRR;
        case GX_TF_IA4:
        case GX_TF_IA8:
            return GX_TEX_SWIZZLE_RRRA;
        default:
            return GX_TEX_SWIZZLE_RGBA;
    }
}

u32 gx_texture_compute_size(u16 width, u16 height, GXTexFmt format) {
    /* TODO: Phase 3d -- Compute tiled texture size
     *
     * Round width and height up to tile boundaries, then compute:
     *   numTiles = ceilDiv(width, tileW) * ceilDiv(height, tileH)
     *   size = numTiles * bytesPerTile
     *
     * Tile dimensions by format:
     *   I4:     8x8, 32 bytes/tile  -> 4 bpp
     *   I8:     8x4, 32 bytes/tile  -> 8 bpp
     *   IA4:    8x4, 32 bytes/tile  -> 8 bpp
     *   IA8:    4x4, 32 bytes/tile  -> 16 bpp
     *   RGB565: 4x4, 32 bytes/tile  -> 16 bpp
     *   RGB5A3: 4x4, 32 bytes/tile  -> 16 bpp
     *   RGBA8:  4x4, 64 bytes/tile  -> 32 bpp
     *   CI4:    8x8, 32 bytes/tile  -> 4 bpp
     *   CI8:    8x4, 32 bytes/tile  -> 8 bpp
     *   CMPR:   8x8, 32 bytes/tile  -> 4 bpp (compressed)
     */

    u32 tileW = 0, tileH = 0, bytesPerTile = 0;

    switch (format) {
        case GX_TF_I4:     tileW = 8; tileH = 8; bytesPerTile = 32; break;
        case GX_TF_I8:     tileW = 8; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_IA4:    tileW = 8; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_IA8:    tileW = 4; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_RGB565: tileW = 4; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_RGB5A3: tileW = 4; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_RGBA8:  tileW = 4; tileH = 4; bytesPerTile = 64; break;
        case GX_TF_C4:     tileW = 8; tileH = 8; bytesPerTile = 32; break;
        case GX_TF_C8:     tileW = 8; tileH = 4; bytesPerTile = 32; break;
        case GX_TF_CMPR:   tileW = 8; tileH = 8; bytesPerTile = 32; break;
        default: return 0;
    }

    u32 tilesX = (width + tileW - 1) / tileW;
    u32 tilesY = (height + tileH - 1) / tileH;
    return tilesX * tilesY * bytesPerTile;
}

/* =========================================================================
 * Per-format decode stubs
 * ========================================================================= */

s32 gx_texture_decode_I4(const void* src, u16 w, u16 h,
                         GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 7u) / 8u;
    u32 tilesY = ((u32)h + 7u) / 8u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 8u; ++row) {
                u32 dstY = (tileY * 8u) + row;
                u32 colPair;

                if (dstY >= h) {
                    continue;
                }

                for (colPair = 0; colPair < 4u; ++colPair) {
                    u8 packed = tileSrc[(row * 4u) + colPair];
                    u32 n;

                    for (n = 0u; n < 2u; ++n) {
                        u32 dstX = (tileX * 8u) + (colPair * 2u) + n;
                        u8 intensity;
                        u8* dstPixel;

                        if (dstX >= w) {
                            continue;
                        }
                        intensity = (u8)(((n == 0u) ? (packed >> 4) :
                                          (packed & 0x0Fu)) * 0x11u);
                        dstPixel =
                            out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                        dstPixel[0] = intensity;
                        dstPixel[1] = intensity;
                        dstPixel[2] = intensity;
                        dstPixel[3] = 0xFFu;
                    }
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_I8(const void* src, u16 w, u16 h,
                         GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 7u) / 8u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 8u; ++col) {
                    u32 dstX = (tileX * 8u) + col;
                    u8 intensity;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }

                    intensity = tileSrc[(row * 8u) + col];
                    dstPixel = out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    dstPixel[0] = intensity;
                    dstPixel[1] = intensity;
                    dstPixel[2] = intensity;
                    dstPixel[3] = 0xFF;
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_IA4(const void* src, u16 w, u16 h,
                          GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 7u) / 8u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 8u; ++col) {
                    u32 dstX = (tileX * 8u) + col;
                    u8 packed;
                    u8 intensity;
                    u8 alpha;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }
                    packed = tileSrc[(row * 8u) + col];
                    intensity = (u8)((packed >> 4) * 0x11u);
                    alpha = (u8)((packed & 0x0Fu) * 0x11u);
                    dstPixel =
                        out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    dstPixel[0] = intensity;
                    dstPixel[1] = intensity;
                    dstPixel[2] = intensity;
                    dstPixel[3] = alpha;
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_IA8(const void* src, u16 w, u16 h,
                          GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 3u) / 4u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 4u; ++col) {
                    u32 dstX = (tileX * 4u) + col;
                    const u8* srcPixel;
                    u8 alpha;
                    u8 intensity;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }
                    srcPixel = tileSrc + (((row * 4u) + col) * 2u);
                    alpha = srcPixel[0];
                    intensity = srcPixel[1];
                    dstPixel =
                        out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    dstPixel[0] = intensity;
                    dstPixel[1] = intensity;
                    dstPixel[2] = intensity;
                    dstPixel[3] = alpha;
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_RGB565(const void* src, u16 w, u16 h,
                             GXDecodedTexture* out) {
    (void)src;

    /* TODO: Phase 3d -- Decode RGB565 (16-bit RGB)
     *
     * Tile layout: 4x4 texels per tile, 16 bits per texel
     * Each tile is 32 bytes
     *
     * For each texel (big-endian u16):
     *   r = ((val >> 11) & 0x1F) * 255 / 31
     *   g = ((val >> 5) & 0x3F) * 255 / 63
     *   b = (val & 0x1F) * 255 / 31
     *   a = 255
     *
     * Output: GL_RGBA8 (expand to full RGBA for simplicity)
     */

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * h * 4;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0xFF, out->dataSize);
    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_RGB5A3(const void* src, u16 w, u16 h,
                             GXDecodedTexture* out) {
    /* RGB5A3: 4x4 texels per tile, 32 bytes/tile, big-endian u16 per texel.
     * MSB set -> RGB555 opaque; MSB clear -> RGB4A3 (3-bit alpha + 4:4:4). */
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 3u) / 4u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc = srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 4u; ++col) {
                    u32 dstX = (tileX * 4u) + col;
                    u32 k = (row * 4u) + col;
                    u16 v;
                    u8 r, g, b, a;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }

                    v = (u16)(((u16)tileSrc[k * 2u] << 8) | tileSrc[(k * 2u) + 1u]);
                    if (v & 0x8000u) {
                        u8 r5 = (u8)((v >> 10) & 0x1Fu);
                        u8 g5 = (u8)((v >> 5) & 0x1Fu);
                        u8 b5 = (u8)(v & 0x1Fu);
                        r = (u8)((r5 << 3) | (r5 >> 2));
                        g = (u8)((g5 << 3) | (g5 >> 2));
                        b = (u8)((b5 << 3) | (b5 >> 2));
                        a = 0xFFu;
                    } else {
                        u8 a3 = (u8)((v >> 12) & 0x7u);
                        u8 r4 = (u8)((v >> 8) & 0xFu);
                        u8 g4 = (u8)((v >> 4) & 0xFu);
                        u8 b4 = (u8)(v & 0xFu);
                        r = (u8)((r4 << 4) | r4);
                        g = (u8)((g4 << 4) | g4);
                        b = (u8)((b4 << 4) | b4);
                        a = (u8)((a3 << 5) | (a3 << 2) | (a3 >> 1));
                    }

                    dstPixel = out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    dstPixel[0] = r;
                    dstPixel[1] = g;
                    dstPixel[2] = b;
                    dstPixel[3] = a;
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_RGBA8(const void* src, u16 w, u16 h,
                            GXDecodedTexture* out) {
    /* RGBA8 ("RGBA32"): 4x4 texels per tile, 64 bytes per tile. The tile is
     * two 32-byte groups: first 32 bytes are AR pairs (A,R per texel), next 32
     * are GB pairs (G,B per texel). Texel index k = row*4 + col, stride 2. */
    const u8* srcBytes = (const u8*)src;
    u32 tilesX = ((u32)w + 3u) / 4u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* arData = srcBytes + (((tileY * tilesX) + tileX) * 64u);
            const u8* gbData = arData + 32u;
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 4u; ++col) {
                    u32 dstX = (tileX * 4u) + col;
                    u32 k = (row * 4u) + col;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }

                    dstPixel = out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    dstPixel[0] = arData[(k * 2u) + 1u]; /* R */
                    dstPixel[1] = gbData[(k * 2u) + 0u]; /* G */
                    dstPixel[2] = gbData[(k * 2u) + 1u]; /* B */
                    dstPixel[3] = arData[(k * 2u) + 0u]; /* A */
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_CI4(const void* src, u16 w, u16 h,
                          const void* tlut, GXTlutFmt tlutFmt,
                          GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    const u8* tlutBytes = (const u8*)tlut;
    u32 tilesX = ((u32)w + 7u) / 8u;
    u32 tilesY = ((u32)h + 7u) / 8u;
    u32 tileY;
    u32 tileX;

    /* CI4: 8x8 texels per tile, 32 bytes per tile, 4 bits per texel.
     * High nibble = even column, low nibble = odd column (same as I4).
     * Each nibble is a palette index into the TLUT. */
    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 srcIndex = 0u;
            u32 row;

            for (row = 0; row < 8u; ++row) {
                u32 dstY = (tileY * 8u) + row;
                u32 col;

                for (col = 0; col < 8u; col += 2u) {
                    u8 byteVal = tileSrc[srcIndex++];
                    u8 idx0 = (u8)((byteVal >> 4) & 0x0Fu);
                    u8 idx1 = (u8)(byteVal & 0x0Fu);
                    u32 dstX0 = (tileX * 8u) + col;
                    u32 dstX1 = dstX0 + 1u;

                    if (dstY >= h) {
                        continue;
                    }

                    if (dstX0 < w) {
                        u8* dstPixel =
                            out->data + ((((u32)dstY * (u32)w) + dstX0) * 4u);
                        if (tlutBytes) {
                            gx_tlut_decode_entry(
                                read_be16(tlutBytes + ((u32)idx0 * 2u)),
                                tlutFmt, &dstPixel[0], &dstPixel[1],
                                &dstPixel[2], &dstPixel[3]);
                        } else {
                            u8 g = (u8)(idx0 * 0x11u);
                            dstPixel[0] = g; dstPixel[1] = g;
                            dstPixel[2] = g; dstPixel[3] = 0xFF;
                        }
                    }
                    if (dstX1 < w) {
                        u8* dstPixel =
                            out->data + ((((u32)dstY * (u32)w) + dstX1) * 4u);
                        if (tlutBytes) {
                            gx_tlut_decode_entry(
                                read_be16(tlutBytes + ((u32)idx1 * 2u)),
                                tlutFmt, &dstPixel[0], &dstPixel[1],
                                &dstPixel[2], &dstPixel[3]);
                        } else {
                            u8 g = (u8)(idx1 * 0x11u);
                            dstPixel[0] = g; dstPixel[1] = g;
                            dstPixel[2] = g; dstPixel[3] = 0xFF;
                        }
                    }
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_CI8(const void* src, u16 w, u16 h,
                          const void* tlut, GXTlutFmt tlutFmt,
                          GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    const u8* tlutBytes = (const u8*)tlut;
    u32 tilesX = ((u32)w + 7u) / 8u;
    u32 tilesY = ((u32)h + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    /* CI8: 8x4 texels per tile, 32 bytes per tile, 1 byte index per texel
     * (same tile walk as I8). Each byte is a palette index into the TLUT. */
    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                srcBytes + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= h) {
                    continue;
                }

                for (col = 0; col < 8u; ++col) {
                    u32 dstX = (tileX * 8u) + col;
                    u8 idx;
                    u8* dstPixel;

                    if (dstX >= w) {
                        continue;
                    }

                    idx = tileSrc[(row * 8u) + col];
                    dstPixel = out->data + ((((u32)dstY * (u32)w) + dstX) * 4u);
                    if (tlutBytes) {
                        gx_tlut_decode_entry(
                            read_be16(tlutBytes + ((u32)idx * 2u)),
                            tlutFmt, &dstPixel[0], &dstPixel[1],
                            &dstPixel[2], &dstPixel[3]);
                    } else {
                        dstPixel[0] = idx; dstPixel[1] = idx;
                        dstPixel[2] = idx; dstPixel[3] = 0xFF;
                    }
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

s32 gx_texture_decode_CMPR(const void* src, u16 w, u16 h,
                           GXDecodedTexture* out) {
    const u8* srcBytes = (const u8*)src;
    u32 macroTilesX = (w + 7u) / 8u;
    u32 macroTilesY = (h + 7u) / 8u;
    u32 macroY;
    u32 macroX;
    u32 subBlock;

    out->width = w;
    out->height = h;
    out->dataSize = (u32)w * (u32)h * 4u;
    out->data = (u8*)malloc(out->dataSize);
    if (!out->data) return -1;
    memset(out->data, 0, out->dataSize);

    for (macroY = 0; macroY < macroTilesY; ++macroY) {
        for (macroX = 0; macroX < macroTilesX; ++macroX) {
            const u8* macroSrc =
                srcBytes + (((macroY * macroTilesX) + macroX) * 32u);

            for (subBlock = 0; subBlock < 4u; ++subBlock) {
                u32 blockX = (macroX * 8u) + ((subBlock & 1u) * 4u);
                u32 blockY = (macroY * 8u) + ((subBlock >> 1u) * 4u);
                u8 blockPixels[4 * 4 * 4];
                u32 row;

                decode_dxt1_block(macroSrc + (subBlock * 8u),
                                  blockPixels,
                                  4u * 4u);

                for (row = 0; row < 4u; ++row) {
                    u32 dstY = blockY + row;

                    if (dstY >= h || blockX >= w) {
                        continue;
                    }

                    memcpy(out->data + (((dstY * (u32)w) + blockX) * 4u),
                           blockPixels + (row * 4u * 4u),
                           ((blockX + 4u) <= w ? 4u : (u32)w - blockX) * 4u);
                }
            }
        }
    }

    out->glInternalFormat = GL_RGBA8;
    out->glFormat = GL_RGBA;
    out->glType = GL_UNSIGNED_BYTE;
    out->isCompressed = 0;
    out->swizzleMode = GX_TEX_SWIZZLE_RGBA;
    return 0;
}

/* =========================================================================
 * Main decode dispatch
 * ========================================================================= */

s32 gx_texture_decode(const void* srcData, u16 width, u16 height,
                      GXTexFmt format,
                      const void* tlutData, GXTlutFmt tlutFmt,
                      u16 tlutEntries,
                      GXDecodedTexture* outResult) {
    (void)tlutEntries;

    if (!srcData || !outResult || width == 0 || height == 0) return -1;

    memset(outResult, 0, sizeof(*outResult));

    switch (format) {
        case GX_TF_I4:
            return gx_texture_decode_I4(srcData, width, height, outResult);
        case GX_TF_I8:
            return gx_texture_decode_I8(srcData, width, height, outResult);
        case GX_TF_IA4:
            return gx_texture_decode_IA4(srcData, width, height, outResult);
        case GX_TF_IA8:
            return gx_texture_decode_IA8(srcData, width, height, outResult);
        case GX_TF_RGB565:
            return gx_texture_decode_RGB565(srcData, width, height, outResult);
        case GX_TF_RGB5A3:
            return gx_texture_decode_RGB5A3(srcData, width, height, outResult);
        case GX_TF_RGBA8:
            return gx_texture_decode_RGBA8(srcData, width, height, outResult);
        case GX_TF_C4:
            return gx_texture_decode_CI4(srcData, width, height,
                                         tlutData, tlutFmt, outResult);
        case GX_TF_C8:
            return gx_texture_decode_CI8(srcData, width, height,
                                         tlutData, tlutFmt, outResult);
        case GX_TF_CMPR:
            return gx_texture_decode_CMPR(srcData, width, height, outResult);
        default:
            printf("[gx_texture] Unsupported texture format: 0x%02X\n", format);
            return -1;
    }
}

s32 gx_texture_decode_mipmap(const void* srcData, u16 mipWidth, u16 mipHeight,
                             GXTexFmt format,
                             const void* tlutData, GXTlutFmt tlutFmt,
                             u16 tlutEntries,
                             GXDecodedTexture* outResult) {
    /* TODO: Phase 3d -- Mipmap decode
     *
     * Same as gx_texture_decode but for a single mip level.
     * The caller provides the mip-level dimensions and data pointer
     * (offset from the base texture data using GStextureHandle::mipOffsets).
     *
     * Upload with: glTexImage2D(GL_TEXTURE_2D, mipLevel, ...)
     */

    return gx_texture_decode(srcData, mipWidth, mipHeight, format,
                             tlutData, tlutFmt, tlutEntries, outResult);
}

void gx_texture_free(GXDecodedTexture* decoded) {
    if (decoded && decoded->data) {
        free(decoded->data);
        decoded->data = (u8*)0;
        decoded->dataSize = 0;
    }
}


#endif /* __MWERKS__ */
