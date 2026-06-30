#include "real_content_host.h"
#include "gx_shim.h"
#include "gx_texture.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_dobj.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define PCPORT_FSYS_MAGIC 0x46535953u
#define PCPORT_LZSS_MAGIC 0x4C5A5353u
#define PCPORT_LZSS_HEADER_SIZE 0x10u
#define PCPORT_LZSS_WINDOW_SIZE 0x1000u
#define PCPORT_LZSS_WINDOW_START 0x0FEEu

typedef struct {
    u32 resultOffset;
    u32 keyOffset;
} PCPortArchivePair;

typedef struct {
    u32 maxPosIndex;
    u32 maxNormalIndex;
    u32 maxColorIndex;
    u32 maxTexcoordIndex;
    u32 maxTexcoord1Index;
    u32 maxMatrixIndex;        /* highest GX_VA_PNMTXIDX value seen (skinning) */
    u32 totalSubmittedVertices;
    u32 totalPrimitiveCommands;
} PCPortDisplayListStats;

#define PCPORT_SERIALIZED_POBJ_SIZE    0x18u
#define PCPORT_SERIALIZED_VTXDESC_SIZE 0x18u
#define PCPORT_SERIALIZED_JOINT_SIZE   0x40u
#define PCPORT_SERIALIZED_WOBJ_SIZE    0x14u
#define PCPORT_SERIALIZED_COBJ_PERSPECTIVE_SIZE 0x38u
#define PCPORT_SERIALIZED_MOBJ_SIZE    0x18u
#define PCPORT_SERIALIZED_MATERIAL_SIZE 0x14u
#define PCPORT_SERIALIZED_PEDESC_SIZE  0x0Cu
#define PCPORT_SERIALIZED_TOBJ_SIZE    0x5Cu
#define PCPORT_SERIALIZED_IMAGEDESC_SIZE 0x18u
#define PCPORT_SERIALIZED_TEV_SIZE     0x40u
#define PCPORT_MAX_VTXDESC_ENTRIES     16u
#define PCPORT_MAX_JOINT_PATH          64u

#define PCPORT_TEX_COLORMAP_MASK      (0x0Fu << 16)
#define PCPORT_TEX_COLORMAP_MODULATE  (4u << 16)
#define PCPORT_TEX_COLORMAP_REPLACE   (5u << 16)
#define PCPORT_TEX_COLORMAP_PASS      (6u << 16)

#define PCPORT_GX_TEV_MODULATE 0u
#define PCPORT_GX_TEV_REPLACE  3u
#define PCPORT_GX_TEV_PASSCLR  4u

#define PCPORT_TEV_I8_RAMP_SIG0   0x00000000u
#define PCPORT_TEV_I8_RAMP_SIG1   0x00000101u
#define PCPORT_TEV_I8_RAMP_SIG2   0x8580080Fu
#define PCPORT_TEV_I8_RAMP_SIG3   0x07070707u
#define PCPORT_TEV_I8_RAMP_SIG6   0x00000000u
#define PCPORT_TEV_I8_RAMP_SIG7   0x40000077u
#define PCPORT_TEV_I8_RAMP_SIG11  0x00000004u
#define PCPORT_TEV_I8_RAMP_SIG15  0x3F800000u

static u32 ReadBE32(const u8* data) {
    return ((u32)data[0] << 24) |
           ((u32)data[1] << 16) |
           ((u32)data[2] << 8) |
           (u32)data[3];
}

static u16 ReadBE16(const u8* data) {
    return (u16)(((u16)data[0] << 8) | data[1]);
}

static f32 ReadBEFloat(const u8* data) {
    u32 bits = ReadBE32(data);
    f32 value;

    memcpy(&value, &bits, sizeof(value));
    return value;
}

static void ReadPackedColorRGB(u32 value, u8 outColor[4]) {
    outColor[0] = (u8)((value >> 24) & 0xFFu);
    outColor[1] = (u8)((value >> 16) & 0xFFu);
    outColor[2] = (u8)((value >> 8) & 0xFFu);
    outColor[3] = 0xFFu;
}

static void WriteBE32(u8* data, u32 value) {
    data[0] = (u8)((value >> 24) & 0xFF);
    data[1] = (u8)((value >> 16) & 0xFF);
    data[2] = (u8)((value >> 8) & 0xFF);
    data[3] = (u8)(value & 0xFF);
}

static void WriteBEFloat(u8* data, f32 value) {
    union { f32 f; u32 u; } v;
    v.f = value;
    WriteBE32(data, v.u);
}

static BOOL IsArchiveRangeValid(const PCPortHSDArchive* archive,
                                u32 offset, u32 size) {
    if (archive == NULL || archive->storage == NULL ||
        offset < archive->dataOffset || offset > archive->storageSize) {
        return FALSE;
    }

    return size <= archive->storageSize - offset;
}

static void DecodeI8RampTexture(const u8* src,
                                u16 width,
                                u16 height,
                                const u8 light[4],
                                const u8 dark[4],
                                u8* dstRgba) {
    u32 tilesX = ((u32)width + 7u) / 8u;
    u32 tilesY = ((u32)height + 3u) / 4u;
    u32 tileY;
    u32 tileX;

    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc = src + (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= height) {
                    continue;
                }

                for (col = 0; col < 8u; ++col) {
                    u32 dstX = (tileX * 8u) + col;
                    u32 channel;
                    u8 intensity;
                    u8* dstPixel;

                    if (dstX >= width) {
                        continue;
                    }

                    intensity = tileSrc[(row * 8u) + col];
                    dstPixel = dstRgba + ((((u32)dstY * (u32)width) + dstX) * 4u);
                    for (channel = 0; channel < 3u; ++channel) {
                        u32 darkValue = dark[channel];
                        u32 lightValue = light[channel];

                        dstPixel[channel] = (u8)(darkValue +
                                                 (((lightValue - darkValue) * intensity + 127u) / 255u));
                    }
                    dstPixel[3] = 0xFFu;
                }
            }
        }
    }
}

static void TranslateTextureTevPayload(const PCPortHSDArchive* archive,
                                       u32 tevOffset,
                                       u32 textureFormat,
                                       PCPortTranslatedTev* outTev) {
    const u8* tevData;
    u32 i;

    if (outTev == NULL) {
        return;
    }

    memset(outTev, 0, sizeof(*outTev));

    if (archive == NULL || tevOffset == 0u ||
        !IsArchiveRangeValid(archive, tevOffset, PCPORT_SERIALIZED_TEV_SIZE)) {
        return;
    }

    tevData = archive->storage + tevOffset;
    outTev->archiveOffset = tevOffset;
    outTev->rawWordCount = 16u;
    for (i = 0; i < 16u; ++i) {
        outTev->rawWords[i] = ReadBE32(tevData + (i * 4u));
    }

    if (textureFormat == GX_TF_I8 &&
        outTev->rawWords[0] == PCPORT_TEV_I8_RAMP_SIG0 &&
        outTev->rawWords[1] == PCPORT_TEV_I8_RAMP_SIG1 &&
        outTev->rawWords[2] == PCPORT_TEV_I8_RAMP_SIG2 &&
        outTev->rawWords[3] == PCPORT_TEV_I8_RAMP_SIG3 &&
        outTev->rawWords[6] == PCPORT_TEV_I8_RAMP_SIG6 &&
        outTev->rawWords[7] == PCPORT_TEV_I8_RAMP_SIG7 &&
        outTev->rawWords[11] == PCPORT_TEV_I8_RAMP_SIG11 &&
        outTev->rawWords[15] == PCPORT_TEV_I8_RAMP_SIG15) {
        outTev->kind = PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP;
        ReadPackedColorRGB(outTev->rawWords[4], outTev->rampLight);
        ReadPackedColorRGB(outTev->rawWords[5], outTev->rampDark);
    }
}

static BOOL ResolveTextureCoordIdFromRawSrc(u32 rawSrc, u8* outCoordId) {
    if (outCoordId == NULL) {
        return FALSE;
    }

    if (rawSrc >= 4u && rawSrc <= 11u) {
        *outCoordId = (u8)(rawSrc - 4u);
        return TRUE;
    }

    if (rawSrc <= 7u) {
        *outCoordId = (u8)rawSrc;
        return TRUE;
    }

    return FALSE;
}

static BOOL HasZeroColorRamp(const PCPortTranslatedTexture* texture) {
    if (texture == NULL) {
        return FALSE;
    }

    return texture->tev.rampLight[0] == 0u &&
           texture->tev.rampLight[1] == 0u &&
           texture->tev.rampLight[2] == 0u &&
           texture->tev.rampDark[0] == 0u &&
           texture->tev.rampDark[1] == 0u &&
           texture->tev.rampDark[2] == 0u;
}

static BOOL IsNoTevDirectSampleFormat(u32 textureFormat) {
    switch (textureFormat) {
    case GX_TF_I4:
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_IA8:
    case GX_TF_RGB565:
    case GX_TF_RGB5A3:
    case GX_TF_CMPR:
    case GX_TF_RGBA8:
    case GX_TF_C4:  /* CI4 (palettized) */
    case GX_TF_C8:  /* CI8 (palettized) */
        return TRUE;
    default:
        return FALSE;
    }
}

static u8 ClassifyTextureExpStageKind(const PCPortTranslatedTexture* texture,
                                      u8 coordId) {
    if (texture == NULL) {
        return PCPORT_TEXP_STAGE_NONE;
    }

    if (texture->format == GX_TF_I8 &&
        texture->tev.kind == PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP) {
        return PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE;
    }

    if (texture->format == GX_TF_I8 &&
        coordId == 1u &&
        (texture->flags & 0x0Fu) == 0u &&
        (texture->flags & PCPORT_TEX_COLORMAP_MASK) == PCPORT_TEX_COLORMAP_MODULATE &&
        (texture->flags & 0x00F00000u) == 0x00300000u &&
        HasZeroColorRamp(texture)) {
        return PCPORT_TEXP_STAGE_I8_MASK_MODULATE;
    }

    /* A directly-sampleable format (I/IA/RGB/CI/CMPR) is a plain texture
     * stage unless one of the narrower I8 expression cases above claimed it.
     * A generic TEV only selects a blend mode, which the pipeline applies via
     * tevMode. */
    if (IsNoTevDirectSampleFormat(texture->format) &&
        texture->tev.kind != PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP) {
        return PCPORT_TEXP_STAGE_DIRECT_SAMPLE;
    }

    return PCPORT_TEXP_STAGE_NONE;
}

static u8 ClassifyTextureExpKindFromParsedChain(const PCPortParsedTextureNodeChain* chain) {
    if (chain == NULL || chain->nodeCount == 0u) {
        return PCPORT_TEXTURE_EXP_KIND_NONE;
    }

    if (chain->nodeCount == 1u) {
        if (chain->stageKinds[0] == PCPORT_TEXP_STAGE_DIRECT_SAMPLE) {
            return PCPORT_TEXTURE_EXP_KIND_DIRECT_SAMPLE;
        }
        if (chain->stageKinds[0] == PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE) {
            return PCPORT_TEXTURE_EXP_KIND_I8_RAMP;
        }
        return PCPORT_TEXTURE_EXP_KIND_NONE;
    }

    if (chain->nodeCount == 2u &&
        chain->stageKinds[0] == PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE &&
        chain->stageKinds[1] == PCPORT_TEXP_STAGE_I8_MASK_MODULATE) {
        return PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK;
    }

    return PCPORT_TEXTURE_EXP_KIND_NONE;
}

static BOOL TranslateTextureFromArchiveCommon(const PCPortHSDArchive* archive,
                                              u32 tobjArchiveOffset,
                                              BOOL allowNext,
                                              PCPortTranslatedTexture* outTexture);

BOOL PCPort_ParseTextureNodeChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               u32 maxNodes,
                                               PCPortParsedTextureNodeChain* outChain) {
    u32 currentOffset = tobjArchiveOffset;
    u32 nodeIndex = 0u;

    if (outChain == NULL || maxNodes == 0u || maxNodes > PCPORT_TEXP_STAGE_MAX ||
        !IsArchiveRangeValid(archive, tobjArchiveOffset, PCPORT_SERIALIZED_TOBJ_SIZE)) {
        return FALSE;
    }

    memset(outChain, 0, sizeof(*outChain));
    outChain->headArchiveOffset = tobjArchiveOffset;

    while (currentOffset != 0u && nodeIndex < maxNodes) {
        PCPortTranslatedTexture* node = &outChain->nodes[nodeIndex];

        if (!TranslateTextureFromArchiveCommon(archive,
                                               currentOffset,
                                               TRUE,
                                               node) ||
            !node->hasCoordId) {
            return FALSE;
        }

        outChain->coordIds[nodeIndex] = node->coordId;
        outChain->stageKinds[nodeIndex] = ClassifyTextureExpStageKind(node,
                                                                      node->coordId);
        ++nodeIndex;
        currentOffset = ReadBE32(archive->storage + currentOffset + 0x04);
    }

    if (nodeIndex == 0u || currentOffset != 0u) {
        return FALSE;
    }

    outChain->nodeCount = (u8)nodeIndex;
    outChain->kind = ClassifyTextureExpKindFromParsedChain(outChain);
    return TRUE;
}

static int GetIndexByteCount(u32 attrType) {
    switch (attrType) {
    case GX_INDEX8:
        return 1;
    case GX_INDEX16:
        return 2;
    default:
        return 0;
    }
}

static BOOL ParseSerializedVtxDesc(const PCPortHSDArchive* archive,
                                   u32 offset,
                                   HSD_VtxDescList* outDesc,
                                   u32* outSourceVertexOffset) {
    u32 strideWord;

    if (outDesc == NULL || outSourceVertexOffset == NULL ||
        !IsArchiveRangeValid(archive, offset, PCPORT_SERIALIZED_VTXDESC_SIZE)) {
        return FALSE;
    }

    memset(outDesc, 0, sizeof(*outDesc));
    outDesc->attr = ReadBE32(archive->storage + offset + 0x00);
    outDesc->attr_type = ReadBE32(archive->storage + offset + 0x04);
    outDesc->comp_cnt = ReadBE32(archive->storage + offset + 0x08);
    outDesc->comp_type = ReadBE32(archive->storage + offset + 0x0C);
    strideWord = ReadBE32(archive->storage + offset + 0x10);
    outDesc->frac = (u8)((strideWord >> 24) & 0xFFu);
    outDesc->stride = (u16)(strideWord & 0xFFFFu);
    *outSourceVertexOffset = ReadBE32(archive->storage + offset + 0x14);
    return TRUE;
}

static BOOL ScanDisplayListIndices(const u8* displayList,
                                   u32 displayListCapacity,
                                   const HSD_VtxDescList* verts,
                                   PCPortDisplayListStats* stats,
                                   u32* outConsumedBytes) {
    const u8* cursor = displayList;
    const u8* end = displayList + displayListCapacity;

    if (displayList == NULL || verts == NULL || stats == NULL) {
        return FALSE;
    }

    memset(stats, 0, sizeof(*stats));

    while (cursor < end) {
        u16 vertexCount;
        const HSD_VtxDescList* v;
        u32 i;

        if ((u32)(end - cursor) < 3u) {
            break; /* not enough room for cmd + 2-byte count -> trailing pad, done */
        }

        /* End-of-list: a NOP opcode (0x00) with a zero count is the terminator/
         * trailing padding. */
        if (cursor[0] == 0u && cursor[1] == 0u && cursor[2] == 0u &&
            stats->totalPrimitiveCommands != 0u) {
            break;
        }

        /* Mid-list GX NOP: a single 0x00 padding byte BETWEEN primitives (for
         * 32-byte alignment), distinguished from the 3-zero terminator above by
         * a non-zero following byte. The old code mis-parsed it as a primitive
         * with a garbage vertex count, failed the scan, and the WHOLE PObj was
         * rejected and skipped -- silently dropping an entire body mesh
         * (mania/warugaki's coat/torso link). Skip the pad byte and continue. */
        if (cursor[0] == 0u) {
            cursor += 1;
            continue;
        }

        ++cursor; /* command byte: primitive + vtxfmt */
        vertexCount = (u16)(((u16)cursor[0] << 8) | cursor[1]);
        cursor += 2;
        if (vertexCount == 0) {
            return FALSE;
        }

        stats->totalSubmittedVertices += vertexCount;
        ++stats->totalPrimitiveCommands;

        for (i = 0; i < vertexCount; ++i) {
            for (v = verts; v->attr != GX_VA_NULL; ++v) {
                int indexSize;
                u32 index;

                /* The matrix-index attributes (GX_VA_PNMTXIDX=0 and the eight
                 * GX_VA_TEXnMTXIDX=1..8) are 1-byte DIRECT indices inline in the
                 * display list (not indices into a vertex array): consume the
                 * byte. For the position-matrix index track its max (palette size)
                 * for the skinning pass; the texture-matrix indices are consumed
                 * but not otherwise used yet. A PObj carrying a TEXnMTXIDX (e.g.
                 * Wes's coat) was previously rejected outright -> the mesh dropped. */
                if (v->attr <= GX_VA_TEX7MTXIDX) {
                    if ((u32)(end - cursor) < 1u) {
                        return FALSE;
                    }
                    if (v->attr == GX_VA_PNMTXIDX &&
                        (u32)cursor[0] > stats->maxMatrixIndex) {
                        stats->maxMatrixIndex = cursor[0];
                    }
                    cursor += 1;
                    continue;
                }

                indexSize = GetIndexByteCount(v->attr_type);
                if (indexSize == 0 || (u32)(end - cursor) < (u32)indexSize) {
                    return FALSE;
                }

                if (indexSize == 1) {
                    index = cursor[0];
                } else {
                    index = (u32)(((u16)cursor[0] << 8) | cursor[1]);
                }
                cursor += indexSize;

                switch (v->attr) {
                case GX_VA_POS:
                    if (index > stats->maxPosIndex) {
                        stats->maxPosIndex = index;
                    }
                    break;
                case GX_VA_NRM:
                    if (index > stats->maxNormalIndex) {
                        stats->maxNormalIndex = index;
                    }
                    break;
                case GX_VA_CLR0:
                    if (index > stats->maxColorIndex) {
                        stats->maxColorIndex = index;
                    }
                    break;
                case GX_VA_TEX0:
                    if (index > stats->maxTexcoordIndex) {
                        stats->maxTexcoordIndex = index;
                    }
                    break;
                case GX_VA_TEX1:
                    if (index > stats->maxTexcoord1Index) {
                        stats->maxTexcoord1Index = index;
                    }
                    break;
                default:
                    return FALSE;
                }
            }
        }
    }

    if (outConsumedBytes != NULL) {
        *outConsumedBytes = (u32)(cursor - displayList);
    }

    return stats->totalPrimitiveCommands != 0u;
}

static BOOL TranslateVertexArray(const PCPortHSDArchive* archive,
                                 HSD_VtxDescList* desc,
                                 u32 sourceOffset,
                                 u32 elementCount,
                                 PCPortTranslatedPObj* outPObj) {
    size_t totalBytes;
    u8* data;
    u32 i;

    if (archive == NULL || desc == NULL || outPObj == NULL ||
        elementCount == 0 || desc->stride == 0) {
        return FALSE;
    }

    totalBytes = (size_t)elementCount * (size_t)desc->stride;
    if (totalBytes == 0 ||
        !IsArchiveRangeValid(archive, sourceOffset, (u32)totalBytes)) {
        return FALSE;
    }

    data = (u8*)malloc(totalBytes);
    if (data == NULL) {
        return FALSE;
    }

    memcpy(data, archive->storage + sourceOffset, totalBytes);

    switch (desc->attr) {
    case GX_VA_POS:
        if (desc->comp_type != GX_F32 ||
            (desc->comp_cnt != GX_POS_XY && desc->comp_cnt != GX_POS_XYZ)) {
            free(data);
            return FALSE;
        }

        for (i = 0; i < elementCount; ++i) {
            u8* dst = data + ((size_t)i * desc->stride);
            const u8* src = archive->storage + sourceOffset + ((size_t)i * desc->stride);
            f32 x = ReadBEFloat(src + 0);
            f32 y = ReadBEFloat(src + 4);
            f32 z = 0.0f;

            memcpy(dst + 0, &x, sizeof(x));
            memcpy(dst + 4, &y, sizeof(y));
            if (desc->comp_cnt == GX_POS_XYZ) {
                z = ReadBEFloat(src + 8);
                memcpy(dst + 8, &z, sizeof(z));
            }

            if (i == 0 || x < outPObj->minPosition[0]) {
                outPObj->minPosition[0] = x;
            }
            if (i == 0 || y < outPObj->minPosition[1]) {
                outPObj->minPosition[1] = y;
            }
            if (i == 0 || z < outPObj->minPosition[2]) {
                outPObj->minPosition[2] = z;
            }
            if (i == 0 || x > outPObj->maxPosition[0]) {
                outPObj->maxPosition[0] = x;
            }
            if (i == 0 || y > outPObj->maxPosition[1]) {
                outPObj->maxPosition[1] = y;
            }
            if (i == 0 || z > outPObj->maxPosition[2]) {
                outPObj->maxPosition[2] = z;
            }
        }

        outPObj->positionData = data;
        desc->vertex = data;
        return TRUE;

    case GX_VA_NRM:
        if (desc->comp_type != GX_F32 || desc->comp_cnt != GX_NRM_XYZ) {
            free(data);
            return FALSE;
        }

        for (i = 0; i < elementCount; ++i) {
            u8* dst = data + ((size_t)i * desc->stride);
            const u8* src = archive->storage + sourceOffset + ((size_t)i * desc->stride);
            f32 x = ReadBEFloat(src + 0);
            f32 y = ReadBEFloat(src + 4);
            f32 z = ReadBEFloat(src + 8);

            memcpy(dst + 0, &x, sizeof(x));
            memcpy(dst + 4, &y, sizeof(y));
            memcpy(dst + 8, &z, sizeof(z));
        }

        outPObj->normalData = data;
        desc->vertex = data;
        return TRUE;

    case GX_VA_CLR0:
        if (desc->comp_type != GX_RGBA8 && desc->comp_type != GX_RGB8) {
            free(data);
            return FALSE;
        }

        outPObj->colorData = data;
        desc->vertex = data;
        return TRUE;

    case GX_VA_TEX0:
    case GX_VA_TEX1:
        if (desc->comp_type != GX_F32 ||
            (desc->comp_cnt != GX_TEX_S && desc->comp_cnt != GX_TEX_ST)) {
            free(data);
            return FALSE;
        }

        for (i = 0; i < elementCount; ++i) {
            u8* dst = data + ((size_t)i * desc->stride);
            const u8* src = archive->storage + sourceOffset + ((size_t)i * desc->stride);
            f32 s = ReadBEFloat(src + 0);

            memcpy(dst + 0, &s, sizeof(s));
            if (desc->comp_cnt == GX_TEX_ST) {
                f32 t = ReadBEFloat(src + 4);

                memcpy(dst + 4, &t, sizeof(t));
            }
        }

        if (desc->attr == GX_VA_TEX0) {
            outPObj->texcoordData = data;
        } else {
            outPObj->texcoord1Data = data;
        }
        desc->vertex = data;
        return TRUE;

        default:
            free(data);
            return FALSE;
    }
}

static void LoadIdentityMtx(f32 mtx[3][4]) {
    memset(mtx, 0, sizeof(f32) * 12u);
    mtx[0][0] = 1.0f;
    mtx[1][1] = 1.0f;
    mtx[2][2] = 1.0f;
}

static void MultiplyAffineMtx(const f32 a[3][4],
                              const f32 b[3][4],
                              f32 out[3][4]) {
    f32 result[3][4];
    u32 row;
    u32 col;

    for (row = 0; row < 3u; ++row) {
        for (col = 0; col < 3u; ++col) {
            result[row][col] = (a[row][0] * b[0][col]) +
                               (a[row][1] * b[1][col]) +
                               (a[row][2] * b[2][col]);
        }

        result[row][3] = (a[row][0] * b[0][3]) +
                         (a[row][1] * b[1][3]) +
                         (a[row][2] * b[2][3]) +
                         a[row][3];
    }

    memcpy(out, result, sizeof(result));
}

static void BuildJointLocalMtx(const PCPortHSDArchive* archive,
                               u32 jointOffset,
                               f32 out[3][4]) {
    f32 rx = ReadBEFloat(archive->storage + jointOffset + 0x14);
    f32 ry = ReadBEFloat(archive->storage + jointOffset + 0x18);
    f32 rz = ReadBEFloat(archive->storage + jointOffset + 0x1C);
    f32 sx = ReadBEFloat(archive->storage + jointOffset + 0x20);
    f32 sy = ReadBEFloat(archive->storage + jointOffset + 0x24);
    f32 sz = ReadBEFloat(archive->storage + jointOffset + 0x28);
    f32 tx = ReadBEFloat(archive->storage + jointOffset + 0x2C);
    f32 ty = ReadBEFloat(archive->storage + jointOffset + 0x30);
    f32 tz = ReadBEFloat(archive->storage + jointOffset + 0x34);
    f32 cx = cosf(rx);
    f32 sxRot = sinf(rx);
    f32 cy = cosf(ry);
    f32 syRot = sinf(ry);
    f32 cz = cosf(rz);
    f32 szRot = sinf(rz);

    out[0][0] = (cz * cy) * sx;
    out[0][1] = ((cz * syRot * sxRot) - (szRot * cx)) * sy;
    out[0][2] = ((cz * syRot * cx) + (szRot * sxRot)) * sz;
    out[0][3] = tx;

    out[1][0] = (szRot * cy) * sx;
    out[1][1] = ((szRot * syRot * sxRot) + (cz * cx)) * sy;
    out[1][2] = ((szRot * syRot * cx) - (cz * sxRot)) * sz;
    out[1][3] = ty;

    out[2][0] = (-syRot) * sx;
    out[2][1] = (cy * sxRot) * sy;
    out[2][2] = (cy * cx) * sz;
    out[2][3] = tz;

    if (getenv("PCPORT_MIRROR_DBG") != NULL && (sx < 0.0f || sy < 0.0f || sz < 0.0f)) {
        u32 flags = ReadBE32(archive->storage + jointOffset + 0x04);
        fprintf(stderr, "[mirror] joint@0x%X NEGATIVE-SCALE=(%.2f,%.2f,%.2f) flags=0x%X trans=(%.2f,%.2f,%.2f)\n",
                jointOffset, sx, sy, sz, flags, tx, ty, tz);
    }
}

static f32 Vec3Dot(const f32 a[3], const f32 b[3]) {
    return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static void Vec3Cross(const f32 a[3], const f32 b[3], f32 out[3]) {
    out[0] = (a[1] * b[2]) - (a[2] * b[1]);
    out[1] = (a[2] * b[0]) - (a[0] * b[2]);
    out[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

static BOOL Vec3Normalize(f32 v[3]) {
    f32 length = sqrtf(Vec3Dot(v, v));

    if (!(length > 0.00001f)) {
        return FALSE;
    }

    v[0] /= length;
    v[1] /= length;
    v[2] /= length;
    return TRUE;
}

static BOOL LoadSerializedWObjPosition(const PCPortHSDArchive* archive,
                                       u32 wobjOffset,
                                       f32 outPos[3]) {
    if (outPos == NULL ||
        !IsArchiveRangeValid(archive, wobjOffset, PCPORT_SERIALIZED_WOBJ_SIZE)) {
        return FALSE;
    }

    outPos[0] = ReadBEFloat(archive->storage + wobjOffset + 0x04);
    outPos[1] = ReadBEFloat(archive->storage + wobjOffset + 0x08);
    outPos[2] = ReadBEFloat(archive->storage + wobjOffset + 0x0C);
    return TRUE;
}

static BOOL BuildCameraViewMatrix(const f32 eye[3],
                                  const f32 interest[3],
                                  const f32 upHint[3],
                                  f32 out[3][4]) {
    f32 forward[3] = {
        interest[0] - eye[0],
        interest[1] - eye[1],
        interest[2] - eye[2]
    };
    f32 side[3];
    f32 up[3] = { upHint[0], upHint[1], upHint[2] };

    if (!Vec3Normalize(forward)) {
        return FALSE;
    }

    if (!Vec3Normalize(up)) {
        up[0] = 0.0f;
        up[1] = 1.0f;
        up[2] = 0.0f;
    }

    Vec3Cross(forward, up, side);
    if (!Vec3Normalize(side)) {
        up[0] = 0.0f;
        up[1] = 0.0f;
        up[2] = 1.0f;
        Vec3Cross(forward, up, side);
        if (!Vec3Normalize(side)) {
            return FALSE;
        }
    }

    Vec3Cross(side, forward, up);
    if (!Vec3Normalize(up)) {
        return FALSE;
    }

    out[0][0] = side[0];
    out[0][1] = side[1];
    out[0][2] = side[2];
    out[0][3] = -Vec3Dot(side, eye);

    out[1][0] = up[0];
    out[1][1] = up[1];
    out[1][2] = up[2];
    out[1][3] = -Vec3Dot(up, eye);

    out[2][0] = -forward[0];
    out[2][1] = -forward[1];
    out[2][2] = -forward[2];
    out[2][3] = Vec3Dot(forward, eye);
    return TRUE;
}

static BOOL BuildCameraProjectionMatrix(f32 fovDegrees,
                                        f32 aspect,
                                        f32 nearZ,
                                        f32 farZ,
                                        f32 out[4][4]) {
    f32 radians;
    f32 cotangent;
    u32 row;
    u32 col;

    if (!(nearZ > 0.0f) || !(farZ > nearZ) || !(aspect > 0.0f)) {
        return FALSE;
    }

    radians = fovDegrees * (3.14159265358979323846f / 180.0f);
    cotangent = 1.0f / tanf(radians * 0.5f);

    for (row = 0; row < 4u; ++row) {
        for (col = 0; col < 4u; ++col) {
            out[row][col] = 0.0f;
        }
    }

    out[0][0] = cotangent / aspect;
    out[1][1] = cotangent;
    out[2][2] = (farZ + nearZ) / (nearZ - farZ);
    out[2][3] = (2.0f * farZ * nearZ) / (nearZ - farZ);
    out[3][2] = -1.0f;
    return TRUE;
}

/* Cycle guard for FindJointPath. The sibling (next) recursion does NOT increment
 * depth, so a sibling cycle (A.next->B, B.next->A) recurses forever -- the depth
 * cap can't stop it. Some character rigs (e.g. casey, reached via the field-walk
 * avatar path) have such cycles and HUNG the render. Track every joint visited in
 * the current search and never revisit one. Reset on the top-level (depth 0) call.
 * Joint graphs here are proper trees, so this never blocks a real path. */
#define PCPORT_MAX_JOINT_VISIT 8192u
static u32 g_jointPathVisited[PCPORT_MAX_JOINT_VISIT];
static u32 g_jointPathVisitedCount;

static BOOL FindJointPath(const PCPortHSDArchive* archive,
                          u32 currentJointOffset,
                          u32 targetJointOffset,
                          u32* pathOffsets,
                          u32 depth,
                          u32* outPathLength) {
    u32 childOffset;
    u32 nextOffset;
    u32 vi;

    if (depth == 0u) {
        g_jointPathVisitedCount = 0u;
    }

    if (pathOffsets == NULL || outPathLength == NULL ||
        depth >= PCPORT_MAX_JOINT_PATH ||
        !IsArchiveRangeValid(archive, currentJointOffset, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return FALSE;
    }

    /* already visited this joint in the current search -> cycle, bail */
    for (vi = 0; vi < g_jointPathVisitedCount; ++vi) {
        if (g_jointPathVisited[vi] == currentJointOffset) {
            return FALSE;
        }
    }
    if (g_jointPathVisitedCount < PCPORT_MAX_JOINT_VISIT) {
        g_jointPathVisited[g_jointPathVisitedCount++] = currentJointOffset;
    }

    pathOffsets[depth] = currentJointOffset;
    if (currentJointOffset == targetJointOffset) {
        *outPathLength = depth + 1u;
        return TRUE;
    }

    childOffset = ReadBE32(archive->storage + currentJointOffset + 0x08);
    if (childOffset != 0u &&
        FindJointPath(archive, childOffset, targetJointOffset,
                      pathOffsets, depth + 1u, outPathLength)) {
        return TRUE;
    }

    nextOffset = ReadBE32(archive->storage + currentJointOffset + 0x0C);
    if (nextOffset != 0u &&
        FindJointPath(archive, nextOffset, targetJointOffset,
                      pathOffsets, depth, outPathLength)) {
        return TRUE;
    }

    return FALSE;
}

static u8* LoadFileBytes(const char* path, u32* outSize) {
    FILE* file;
    long fileSize;
    u8* data;

    if (outSize == NULL) {
        return NULL;
    }

    file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    fileSize = ftell(file);
    if (fileSize <= 0 || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    data = (u8*)malloc((size_t)fileSize);
    if (data == NULL) {
        fclose(file);
        return NULL;
    }

    if (fread(data, 1, (size_t)fileSize, file) != (size_t)fileSize) {
        free(data);
        fclose(file);
        return NULL;
    }

    fclose(file);
    *outSize = (u32)fileSize;
    return data;
}

static BOOL DecompressLZSS(const u8* src, u32 srcSize, u8* dst, u32 dstSize) {
    u8 window[PCPORT_LZSS_WINDOW_SIZE];
    u32 srcPos = PCPORT_LZSS_HEADER_SIZE;
    u32 dstPos = 0;
    u32 windowPos = PCPORT_LZSS_WINDOW_START;
    u32 flags = 0;

    if (src == NULL || dst == NULL || srcSize < PCPORT_LZSS_HEADER_SIZE) {
        return FALSE;
    }

    memset(window, 0, sizeof(window));

    while (srcPos < srcSize && dstPos < dstSize) {
        flags >>= 1;
        if ((flags & 0x100) == 0) {
            if (srcPos >= srcSize) {
                break;
            }
            flags = (u32)src[srcPos++] | 0xFF00u;
        }

        if ((flags & 1) != 0) {
            u8 literal;

            if (srcPos >= srcSize) {
                break;
            }

            literal = src[srcPos++];
            dst[dstPos++] = literal;
            window[windowPos] = literal;
            windowPos = (windowPos + 1) & 0x0FFFu;
        } else {
            u8 byte1;
            u8 byte2;
            u32 offset;
            u32 length;
            u32 j;

            if (srcPos + 1 >= srcSize) {
                break;
            }

            byte1 = src[srcPos++];
            byte2 = src[srcPos++];
            offset = (u32)byte1 | (((u32)byte2 & 0xF0u) << 4);
            length = ((u32)byte2 & 0x0Fu) + 2u;

            for (j = 0; j <= length && dstPos < dstSize; ++j) {
                u8 copyByte = window[(offset + j) & 0x0FFFu];
                dst[dstPos++] = copyByte;
                window[windowPos] = copyByte;
                windowPos = (windowPos + 1) & 0x0FFFu;
            }
        }
    }

    return dstPos == dstSize;
}

/* Enumerate every member of an .fsys (name + decompressed size + compression
 * flag), for investigating which archives hold what (e.g. character models in
 * people_archive/chara_*). Diagnostic; prints to stdout. */
void PCPort_FsysListMembers(const char* fsysPath) {
    u8* fsysData;
    u32 fsysSize = 0;
    u32 entryCount, stringTableOffset, entryTableOffset, i;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        printf("[fsys-ls] cannot open %s\n", fsysPath);
        return;
    }
    if (fsysSize < 0x20 || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        printf("[fsys-ls] %s is not an FSYS archive\n", fsysPath);
        free(fsysData);
        return;
    }
    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4u > fsysSize) { free(fsysData); return; }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    printf("[fsys-ls] %s: %u members\n", fsysPath, entryCount);
    for (i = 0; i < entryCount; ++i) {
        u32 entryOffset = ReadBE32(fsysData + entryTableOffset + (i * 4u));
        u32 nameOffset, dataOffset, decompSize, isLZSS = 0;
        if (entryOffset + 0x28u > fsysSize) { continue; }
        nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
        dataOffset = ReadBE32(fsysData + entryOffset + 0x04);
        decompSize = ReadBE32(fsysData + entryOffset + 0x08);
        if (dataOffset + 4u <= fsysSize) {
            isLZSS = (ReadBE32(fsysData + dataOffset) == PCPORT_LZSS_MAGIC);
        }
        if (nameOffset < fsysSize) {
            printf("[fsys-ls]   [%3u] %-24s size=0x%-8X %s\n", i,
                   (const char*)(fsysData + nameOffset), decompSize,
                   isLZSS ? "(lzss)" : "");
        }
    }
    free(fsysData);
}

static BOOL FindFsysEntry(const u8* fsysData, u32 fsysSize,
                          const char* memberName, u32* outEntryOffset) {
    u32 entryCount;
    u32 stringTableOffset;
    u32 entryTableOffset;
    u32 i;

    if (fsysData == NULL || memberName == NULL || outEntryOffset == NULL ||
        fsysSize < 0x20) {
        return FALSE;
    }

    if (ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        return FALSE;
    }

    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4 > fsysSize) {
        return FALSE;
    }

    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        return FALSE;
    }

    for (i = 0; i < entryCount; ++i) {
        u32 entryOffset = ReadBE32(fsysData + entryTableOffset + (i * 4));
        u32 nameOffset;

        if (entryOffset + 0x28 > fsysSize) {
            continue;
        }

        nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
        if (nameOffset >= fsysSize) {
            continue;
        }

        if (strcmp((const char*)(fsysData + nameOffset), memberName) == 0) {
            *outEntryOffset = entryOffset;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL PCPort_LoadFsysMember(const char* fsysPath, const char* memberName,
                           u8** outData, u32* outSize) {
    u8* fsysData;
    u32 fsysSize = 0;
    u32 entryOffset;
    u32 dataOffset;
    u32 compressedSize;
    u8* output;

    if (outData == NULL || outSize == NULL) {
        return FALSE;
    }

    *outData = NULL;
    *outSize = 0;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        return FALSE;
    }

    if (!FindFsysEntry(fsysData, fsysSize, memberName, &entryOffset)) {
        free(fsysData);
        return FALSE;
    }

    dataOffset = ReadBE32(fsysData + entryOffset + 0x04);
    /* entry+0x08 is the DECOMPRESSED size for LZSS members (and equals the
     * on-disk size for stored members). The true on-disk byte count of a
     * compressed member lives in its LZSS header (dataOffset+0x08), so the
     * up-front check only requires the header word to be in range; each branch
     * below validates the real on-disk length. Bounds-checking against
     * compressedSize here falsely rejected strongly-compressed members
     * (e.g. title.fsys:logo_demo, decompressed 0x15BC6B). */
    compressedSize = ReadBE32(fsysData + entryOffset + 0x08);

    if (dataOffset >= fsysSize ||
        dataOffset + PCPORT_LZSS_HEADER_SIZE > fsysSize) {
        free(fsysData);
        return FALSE;
    }

    if (ReadBE32(fsysData + dataOffset) == PCPORT_LZSS_MAGIC) {
        u32 lzssOutputSize;
        u32 lzssInputSize;

        lzssOutputSize = ReadBE32(fsysData + dataOffset + 0x04);
        lzssInputSize = ReadBE32(fsysData + dataOffset + 0x08);
        if (lzssOutputSize == 0 ||
            lzssInputSize < PCPORT_LZSS_HEADER_SIZE ||
            dataOffset + lzssInputSize > fsysSize) {
            free(fsysData);
            return FALSE;
        }

        output = (u8*)malloc((size_t)lzssOutputSize);
        if (output == NULL) {
            free(fsysData);
            return FALSE;
        }

        if (!DecompressLZSS(fsysData + dataOffset, lzssInputSize,
                            output, lzssOutputSize)) {
            free(output);
            free(fsysData);
            return FALSE;
        }

        *outData = output;
        *outSize = lzssOutputSize;
    } else {
        u32 copySize = compressedSize;

        if (dataOffset + copySize > fsysSize) {
            free(fsysData);
            return FALSE;
        }

        output = (u8*)malloc((size_t)copySize);
        if (output == NULL) {
            free(fsysData);
            return FALSE;
        }

        memcpy(output, fsysData + dataOffset, copySize);
        *outData = output;
        *outSize = copySize;
    }

    free(fsysData);
    return TRUE;
}

/* Decompress the FSYS member at `entryOffset` into a fresh malloc'd buffer.
 * Returns the buffer (caller frees) + size, or NULL. Same per-member logic as
 * PCPort_LoadFsysMember, used by the scene-member scan below. */
static u8* DecompressMemberAt(const u8* fsysData, u32 fsysSize,
                              u32 entryOffset, u32* outSize) {
    u32 dataOffset = ReadBE32(fsysData + entryOffset + 0x04);
    u32 storedSize = ReadBE32(fsysData + entryOffset + 0x08);
    u8* output;

    if (dataOffset >= fsysSize ||
        dataOffset + PCPORT_LZSS_HEADER_SIZE > fsysSize) {
        return NULL;
    }
    if (ReadBE32(fsysData + dataOffset) == PCPORT_LZSS_MAGIC) {
        u32 outN = ReadBE32(fsysData + dataOffset + 0x04);
        u32 inN  = ReadBE32(fsysData + dataOffset + 0x08);
        if (outN == 0 || inN < PCPORT_LZSS_HEADER_SIZE ||
            dataOffset + inN > fsysSize) {
            return NULL;
        }
        output = (u8*)malloc((size_t)outN);
        if (output == NULL) {
            return NULL;
        }
        if (!DecompressLZSS(fsysData + dataOffset, inN, output, outN)) {
            free(output);
            return NULL;
        }
        *outSize = outN;
        return output;
    }
    if (dataOffset + storedSize > fsysSize || storedSize == 0) {
        return NULL;
    }
    output = (u8*)malloc((size_t)storedSize);
    if (output == NULL) {
        return NULL;
    }
    memcpy(output, fsysData + dataOffset, storedSize);
    *outSize = storedSize;
    return output;
}

/* True if `data` is an HSD archive (fileSize word == size) exposing public `sym`. */
static BOOL HSDArchiveHasPublic(const u8* data, u32 size, const char* sym) {
    u32 dataSize, nreloc, npub, next, pubOff, extOff, strOff, k;
    size_t symLen;
    if (size < 0x20 || ReadBE32(data) != size) {
        return FALSE;
    }
    symLen = strlen(sym);
    dataSize = ReadBE32(data + 0x04);
    nreloc   = ReadBE32(data + 0x08);
    npub     = ReadBE32(data + 0x0C);
    next     = ReadBE32(data + 0x10);
    pubOff = 0x20 + dataSize + nreloc * 4;
    extOff = pubOff + npub * 8;
    strOff = extOff + next * 8;
    if (strOff > size) {
        return FALSE;
    }
    for (k = 0; k < npub; ++k) {
        u32 entryOff = pubOff + k * 8;
        u32 key, nameOff;
        if (entryOff + 8 > size) {
            break;
        }
        key = ReadBE32(data + entryOff + 4);
        nameOff = strOff + key;
        if (nameOff >= size) {
            continue;
        }
        if (nameOff + symLen < size &&
            memcmp(data + nameOff, sym, symLen) == 0 &&
            data[nameOff + symLen] == '\0') {
            return TRUE;
        }
    }
    return FALSE;
}

/* Field-map .fsys archives contain MULTIPLE members of the same name; only one is
 * the renderable HSD scene archive. Return the LARGEST member that is an HSD archive
 * exposing a "scene_data" public symbol (the map geometry; smaller scene_data members
 * are shared object sets like ippan_m_b1). Lets the field loader reuse the same
 * scene_data -> RenderJointTree path as the title without knowing member indices. */
BOOL PCPort_LoadFsysSceneMember(const char* fsysPath, u8** outData, u32* outSize) {
    u8* fsysData;
    u32 fsysSize = 0;
    u32 entryCount, stringTableOffset, entryTableOffset, i;
    u8* best = NULL;
    u32 bestSize = 0;
    u32 bestIndex = 0;
    char bestName[96];
    int debug = getenv("PCPORT_FIELD_DEBUG") != NULL;

    if (outData == NULL || outSize == NULL) {
        return FALSE;
    }
    *outData = NULL;
    *outSize = 0;
    bestName[0] = '\0';

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        return FALSE;
    }
    if (fsysSize < 0x20 || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        free(fsysData);
        return FALSE;
    }

    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4 > fsysSize) {
        free(fsysData);
        return FALSE;
    }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        free(fsysData);
        return FALSE;
    }

    for (i = 0; i < entryCount; ++i) {
        u32 entryOffset, memSize = 0;
        u32 nameOffset;
        const char* name = "<bad-name>";
        u8* mem;
        if (entryTableOffset + i * 4 + 4 > fsysSize) {
            break;
        }
        entryOffset = ReadBE32(fsysData + entryTableOffset + (i * 4));
        if (entryOffset + 0x28 > fsysSize) {
            continue;
        }
        nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
        if (nameOffset < fsysSize) {
            name = (const char*)(fsysData + nameOffset);
        }
        mem = DecompressMemberAt(fsysData, fsysSize, entryOffset, &memSize);
        if (mem == NULL) {
            continue;
        }
        if (memSize > bestSize && HSDArchiveHasPublic(mem, memSize, "scene_data")) {
            if (debug) {
                printf("[fsys-scene] candidate %s[%u] size=0x%X\n",
                       name, i, memSize);
            }
            free(best);
            best = mem;
            bestSize = memSize;
            bestIndex = i;
            snprintf(bestName, sizeof(bestName), "%s", name);
        } else {
            free(mem);
        }
    }

    free(fsysData);
    if (best == NULL) {
        return FALSE;
    }
    if (debug) {
        printf("[fsys-scene] selected %s[%u] size=0x%X from %s\n",
               bestName[0] ? bestName : "<unknown>", bestIndex, bestSize,
               fsysPath);
        fflush(stdout);
    }
    *outData = best;
    *outSize = bestSize;
    return TRUE;
}

/* True if `data` looks like a WZX collision mesh (see include/game/gs_colsys.h
 * + pcport/field_collision.c). Top-level header is {u32 vertDataOff@0,
 * u32 groupCount@4}; groupCount group records of 0x40 bytes start at vertDataOff,
 * each with 6 self-relative submesh slots at +0x24..+0x38. A submesh is
 * {u32 vtxOff, u32 triCount, ...}; its triangles are 0x34 bytes (3 Vec3f + normal).
 * We confirm the header shape and that at least one submesh resolves to
 * room-scale finite vertex coordinates -- enough to disambiguate the WZX from
 * the other raw members (which all share the map's member name). */
static BOOL WZXLooksValid(const u8* d, u32 n) {
    u32 vertOff, groupCount, g, validTris = 0;
    if (n < 0x48) {
        return FALSE;
    }
    /* Not an HSD archive (those have fileSize word == n). */
    if (ReadBE32(d) == n) {
        return FALSE;
    }
    vertOff = ReadBE32(d + 0x00);
    groupCount = ReadBE32(d + 0x04);
    if (vertOff < 8u || vertOff >= n || groupCount == 0u || groupCount > 256u) {
        return FALSE;
    }
    if (vertOff + groupCount * 0x40u > n) {
        return FALSE;
    }
    for (g = 0; g < groupCount; ++g) {
        u32 grpBase = vertOff + g * 0x40u;
        u32 slot;
        for (slot = 0; slot < 6u; ++slot) {
            u32 so = ReadBE32(d + grpBase + 0x24u + slot * 4u);
            u32 vtx, cnt, k;
            if (so == 0u || so + 0x10u > n) {
                continue;
            }
            vtx = ReadBE32(d + so + 0x00u);
            cnt = ReadBE32(d + so + 0x04u);
            if (vtx == 0u || cnt == 0u || cnt > 100000u) {
                continue;
            }
            if (vtx + cnt * 0x34u > n) {
                continue;
            }
            /* Sanity-check the first vertex of the first triangle. */
            for (k = 0; k < 3u; ++k) {
                union { u32 u; f32 f; } v;
                f32 a;
                v.u = ReadBE32(d + vtx + k * 4u);
                a = v.f;
                if (!(a == a) || a > 1.0e5f || a < -1.0e5f) {
                    goto next_slot;
                }
            }
            validTris += cnt;
        next_slot:;
        }
    }
    return validTris >= 8u;
}

/* Field-map .fsys archives bundle the WZX collision mesh as a raw member that
 * (like the scene) shares the map's member name -- so it can't be fetched by
 * name. Scan all members and return the one matching the WZX signature.
 * Caller frees *outData via PCPort_FreeBuffer. */
BOOL PCPort_LoadFsysWZXMember(const char* fsysPath, u8** outData, u32* outSize) {
    u8* fsysData;
    u32 fsysSize = 0;
    u32 entryCount, stringTableOffset, entryTableOffset, i;

    if (outData == NULL || outSize == NULL) {
        return FALSE;
    }
    *outData = NULL;
    *outSize = 0;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        return FALSE;
    }
    if (fsysSize < 0x20 || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        free(fsysData);
        return FALSE;
    }

    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4 > fsysSize) {
        free(fsysData);
        return FALSE;
    }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        free(fsysData);
        return FALSE;
    }

    for (i = 0; i < entryCount; ++i) {
        u32 entryOffset, memSize = 0;
        u8* mem;
        if (entryTableOffset + i * 4 + 4 > fsysSize) {
            break;
        }
        entryOffset = ReadBE32(fsysData + entryTableOffset + (i * 4));
        if (entryOffset + 0x28 > fsysSize) {
            continue;
        }
        mem = DecompressMemberAt(fsysData, fsysSize, entryOffset, &memSize);
        if (mem == NULL) {
            continue;
        }
        if (WZXLooksValid(mem, memSize)) {
            free(fsysData);
            *outData = mem;
            *outSize = memSize;
            return TRUE;
        }
        free(mem);
    }

    free(fsysData);
    return FALSE;
}

/* Attempt to locate the per-room exit/door trigger records in a field-map's
 * scene_data. See the header doc + tools/pcport_probe/probe_exits.py: the RE'd
 * 0x2C exit record lives in a RUNTIME SDA r13-relative array populated by the
 * asm "people" subsystem during floor load, NOT a discrete static block; the
 * probe found no coherent 0x2C-stride table in D1_garage_1F.fsys (only scattered
 * geometry/material float coincidences). So this is intentionally a stub that
 * reports "not found" -- the host warp path drives from a hand-specified exit
 * list instead. Wired now so the future RE (parse the people-subsystem populate
 * path / the baked DOL floor table) is a drop-in replacement here. */
BOOL PCPort_LoadFsysExitData(const char* fsysPath, u8** outData, u32* outCount) {
    (void)fsysPath;
    if (outData != NULL) {
        *outData = NULL;
    }
    if (outCount != NULL) {
        *outCount = 0;
    }
    return FALSE;
}

void PCPort_FreeBuffer(void* buffer) {
    free(buffer);
}

BOOL PCPort_HSDArchiveParseBE(PCPortHSDArchive* archive,
                              const void* data, u32 size) {
    const u8* src = (const u8*)data;
    u32 fileSize;
    u32 dataSize;
    u32 relocCount;
    u32 publicCount;
    u32 externCount;
    u32 relocOffset;
    u32 publicOffset;
    u32 externOffset;
    u32 stringOffset;

    if (archive == NULL || src == NULL || size < 0x20u) {
        return FALSE;
    }

    memset(archive, 0, sizeof(*archive));

    fileSize = ReadBE32(src + 0x00);
    dataSize = ReadBE32(src + 0x04);
    relocCount = ReadBE32(src + 0x08);
    publicCount = ReadBE32(src + 0x0C);
    externCount = ReadBE32(src + 0x10);

    if (fileSize != size) {
        return FALSE;
    }

    relocOffset = 0x20u + dataSize;
    publicOffset = relocOffset + (relocCount * 4u);
    externOffset = publicOffset + (publicCount * 8u);
    stringOffset = externOffset + (externCount * 8u);

    if (relocOffset > size || publicOffset > size || externOffset > size ||
        stringOffset > size) {
        return FALSE;
    }

    archive->storage = (u8*)malloc((size_t)size);
    if (archive->storage == NULL) {
        return FALSE;
    }

    memcpy(archive->storage, src, size);
    archive->storageSize = size;
    archive->dataSize = dataSize;
    archive->relocCount = relocCount;
    archive->publicCount = publicCount;
    archive->externCount = externCount;
    archive->dataOffset = 0x20u;
    archive->relocOffset = relocOffset;
    archive->publicOffset = publicOffset;
    archive->externOffset = externOffset;
    archive->stringOffset = stringOffset;

    {
        u32 i;

        for (i = 0; i < relocCount; ++i) {
            u32 relocEntryOffset = relocOffset + (i * 4u);
            u32 fieldOffset = ReadBE32(archive->storage + relocEntryOffset);
            u32 absoluteFieldOffset = archive->dataOffset + fieldOffset;
            u32 value;

            if (absoluteFieldOffset + 4u > archive->storageSize) {
                PCPort_HSDArchiveDestroy(archive);
                return FALSE;
            }

            value = ReadBE32(archive->storage + absoluteFieldOffset);
            WriteBE32(archive->storage + absoluteFieldOffset,
                      value + archive->dataOffset);
        }
    }

    return TRUE;
}

/* =========================================================================
 * BE->LE swizzle for running the GAME'S OWN HSD pipeline.
 *
 * The archive scalars are GameCube big-endian and PCPort_HSDArchiveParseBE
 * leaves relocated pointer fields as BE (storage-relative+dataOffset). To feed
 * the data to the game's real HSD_*LoadDesc (which read native LE structs +
 * native host pointers) we, ONCE, walk the descriptor graph and:
 *   (1) byte-swap every non-pointer multibyte scalar (u16/u32/f32) in place;
 *   (2) convert every relocated pointer field to a native host pointer.
 * Pointer fields are identified structurally (per HSD_*Desc layout); during the
 * scalar walk they still hold BE offsets, so we follow them by offset. After
 * the walk, ApplyHostRelocations rewrites ALL relocated fields (from the reloc
 * table) to native pointers. Visited-set keyed on storage offset handles shared
 * + cyclic descriptors. NOTE: vertex-array payloads + display lists are handled
 * separately (the GX shim already reads BE display-list indices; vertex data
 * swap is a follow-up step). This is the game's data driving the game's code --
 * no re-created rendering. */

static void Swap16InPlace(u8* p) { u8 t = p[0]; p[0] = p[1]; p[1] = t; }
static void Swap32InPlace(u8* p) {
    u8 t; t = p[0]; p[0] = p[3]; p[3] = t; t = p[1]; p[1] = p[2]; p[2] = t;
}

#define PCPORT_SWIZ_MAX_NODES 16384u
typedef struct {
    u8* base;            /* archive->storage */
    u32 size;
    u32 dataOffset;
    u32 visited[PCPORT_SWIZ_MAX_NODES];
    u32 visitedCount;
} PCPortSwizCtx;

static BOOL SwizMarkVisited(PCPortSwizCtx* c, u32 off) {
    u32 i;
    if (off == 0u || off >= c->size) {
        return TRUE; /* NULL / OOB -> treat as already-handled (skip) */
    }
    for (i = 0; i < c->visitedCount; ++i) {
        if (c->visited[i] == off) {
            return TRUE;
        }
    }
    if (c->visitedCount < PCPORT_SWIZ_MAX_NODES) {
        c->visited[c->visitedCount++] = off;
    }
    return FALSE;
}

/* Read a (still-BE) relocated pointer field as a storage offset (0 == NULL). */
static u32 SwizChildOff(PCPortSwizCtx* c, u32 fieldAbsOff) {
    if (fieldAbsOff + 4u > c->size) {
        return 0u;
    }
    return ReadBE32(c->base + fieldAbsOff); /* already dataOffset+offset, abs */
}

/* Forward decls (mutually recursive graph walk). */
static void SwizJoint(PCPortSwizCtx* c, u32 off);
static void SwizDObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizMObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizTObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizPObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizAObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizFObjDesc(PCPortSwizCtx* c, u32 off);
static void SwizAnimJoint(PCPortSwizCtx* c, u32 off);
static void SwizMatAnimJoint(PCPortSwizCtx* c, u32 off);
static void SwizMatAnim(PCPortSwizCtx* c, u32 off);
static void SwizTexAnim(PCPortSwizCtx* c, u32 off);

static void SwizImageDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap16InPlace(p + 0x4);   /* width  */
    Swap16InPlace(p + 0x6);   /* height */
    Swap32InPlace(p + 0x8);   /* format */
    Swap32InPlace(p + 0xC);   /* mipmap */
    Swap32InPlace(p + 0x10);  /* minLOD */
    Swap32InPlace(p + 0x14);  /* maxLOD */
    /* +0x0 image_ptr = pointer (handled by ApplyHostRelocations) */
}

static void SwizTObjDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    u32 i;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x8);   /* id  */
    Swap32InPlace(p + 0xC);   /* src */
    for (i = 0; i < 9u; ++i) Swap32InPlace(p + 0x10 + i * 4u); /* rotate/scale/translate */
    Swap32InPlace(p + 0x34);  /* wrap_s */
    Swap32InPlace(p + 0x38);  /* wrap_t */
    /* +0x3C repeat_s, +0x3D repeat_t : u8, no swap */
    Swap32InPlace(p + 0x40);  /* blend_flags */
    Swap32InPlace(p + 0x44);  /* blending */
    Swap32InPlace(p + 0x48);  /* magFilt */
    SwizImageDesc(c, SwizChildOff(c, off + 0x4C)); /* imagedesc */
    /* next(0x4), tlutdesc(0x50), lod(0x54), tev(0x58): pointers */
    SwizTObjDesc(c, SwizChildOff(c, off + 0x4));    /* next */
}

static void SwizMObjDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    u32 matOff;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x4);   /* rendermode */
    SwizTObjDesc(c, SwizChildOff(c, off + 0x8)); /* texdesc */
    matOff = SwizChildOff(c, off + 0xC);         /* mat (HSD_Material) */
    if (!SwizMarkVisited(c, matOff)) {
        u8* m = c->base + matOff;
        Swap32InPlace(m + 0x0);  /* ambient  */
        Swap32InPlace(m + 0x4);  /* diffuse  */
        Swap32InPlace(m + 0x8);  /* specular */
        Swap32InPlace(m + 0xC);  /* alpha    */
        Swap32InPlace(m + 0x10); /* shininess */
    }
    /* renderdesc(0x10), pedesc(0x14, all u8) : no scalar swap */
}

static void SwizFObjDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x4);   /* length */
    Swap32InPlace(p + 0x8);   /* startframe */
    /* +0xC..0xF: u8 type/frac_value/frac_slope/dummy0 : no swap; +0x10 ad ptr */
    SwizFObjDesc(c, SwizChildOff(c, off + 0x0)); /* next */
}

static void SwizAObjDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x0);   /* flags */
    Swap32InPlace(p + 0x4);   /* end_frame */
    Swap32InPlace(p + 0xC);   /* obj_id */
    SwizFObjDesc(c, SwizChildOff(c, off + 0x8)); /* fobjdesc */
}

static void SwizPObjDesc(PCPortSwizCtx* c, u32 off) {
    u8* p;
    u32 vlist;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap16InPlace(p + 0xC);   /* flags */
    Swap16InPlace(p + 0xE);   /* n_display */
    /* verts(0x8) -> VtxDescList array; display(0x10) bytes (indices read BE by
     * the shim); u.joint(0x14) pointer. */
    vlist = SwizChildOff(c, off + 0x8);
    if (!SwizMarkVisited(c, vlist) && vlist != 0u) {
        u32 e = vlist;
        /* iterate entries until attr (BE u32) == GX_VA_NULL (0xFF) */
        while (e + 0x18u <= c->size) {
            u32 attr = ReadBE32(c->base + e);
            if (attr == 0xFFu) { Swap32InPlace(c->base + e); break; }
            Swap32InPlace(c->base + e + 0x0);  /* attr */
            Swap32InPlace(c->base + e + 0x4);  /* attr_type */
            Swap32InPlace(c->base + e + 0x8);  /* comp_cnt */
            Swap32InPlace(c->base + e + 0xC);  /* comp_type */
            /* +0x10 frac u8; +0x12 stride u16; +0x14 vertex ptr */
            Swap16InPlace(c->base + e + 0x12); /* stride */
            e += 0x18u;
        }
    }
    SwizPObjDesc(c, SwizChildOff(c, off + 0x4)); /* next */
}

static void SwizDObjDesc(PCPortSwizCtx* c, u32 off) {
    if (SwizMarkVisited(c, off)) return;
    /* class_name(0), next(4), mobjdesc(8), pobjdesc(C): all pointers */
    SwizMObjDesc(c, SwizChildOff(c, off + 0x8));
    SwizPObjDesc(c, SwizChildOff(c, off + 0xC));
    SwizDObjDesc(c, SwizChildOff(c, off + 0x4)); /* next */
}

static void SwizJoint(PCPortSwizCtx* c, u32 off) {
    u8* p;
    u32 i;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x4);   /* flags */
    for (i = 0; i < 9u; ++i) Swap32InPlace(p + 0x14 + i * 4u); /* rotation/scale/position */
    /* class_name(0), child(8), next(C), u.dobjdesc(10), mtx(38), robjdesc(3C) ptrs */
    SwizDObjDesc(c, SwizChildOff(c, off + 0x10)); /* u.dobjdesc */
    SwizJoint(c, SwizChildOff(c, off + 0x8));     /* child */
    SwizJoint(c, SwizChildOff(c, off + 0xC));     /* next */
    /* robjdesc(0x3C): flags swap handled lazily if present (skip union for now) */
}

/* --- animation descriptor trees (animjoint + matanimjoint) --- */
static void SwizTexAnim(PCPortSwizCtx* c, u32 off) {
    u8* p;
    if (SwizMarkVisited(c, off)) return;
    p = c->base + off;
    Swap32InPlace(p + 0x4);    /* id */
    Swap16InPlace(p + 0x14);   /* n_imagetbl */
    Swap16InPlace(p + 0x16);   /* n_tluttbl */
    /* next(0), aobjdesc(8), imagetbl(C), tluttbl(10): pointers */
    SwizAObjDesc(c, SwizChildOff(c, off + 0x8));
    SwizTexAnim(c, SwizChildOff(c, off + 0x0));  /* next */
}

static void SwizMatAnim(PCPortSwizCtx* c, u32 off) {
    if (SwizMarkVisited(c, off)) return;
    /* next(0), aobjdesc(4), texanim(8), renderanim(C): pointers */
    SwizAObjDesc(c, SwizChildOff(c, off + 0x4));
    SwizTexAnim(c, SwizChildOff(c, off + 0x8));
    SwizMatAnim(c, SwizChildOff(c, off + 0x0)); /* next */
}

static void SwizMatAnimJoint(PCPortSwizCtx* c, u32 off) {
    if (SwizMarkVisited(c, off)) return;
    /* child(0), next(4), matanim(8): pointers */
    SwizMatAnim(c, SwizChildOff(c, off + 0x8));
    SwizMatAnimJoint(c, SwizChildOff(c, off + 0x0)); /* child */
    SwizMatAnimJoint(c, SwizChildOff(c, off + 0x4)); /* next */
}

static void SwizAnimJoint(PCPortSwizCtx* c, u32 off) {
    if (SwizMarkVisited(c, off)) return;
    /* In Colosseum the scene AnimJoint header is 0x10 bytes and all-pointer:
     * {child@0, next@4, aobjdesc@8, robj_anim@C} -- NO trailing flags scalar (the
     * matanimjoint sits at +0x10, so swapping a "flags@0x10" corrupts it). Just
     * recurse the pointers; ApplyHostRelocations fixes them. */
    SwizAObjDesc(c, SwizChildOff(c, off + 0x8));
    SwizAnimJoint(c, SwizChildOff(c, off + 0x0)); /* child */
    SwizAnimJoint(c, SwizChildOff(c, off + 0x4)); /* next */
}

/* Convert every relocated pointer field (from the reloc table) from its BE
 * storage-relative value to a native host pointer (storage + value). Must run
 * AFTER the scalar walk (which reads pointers as BE offsets to traverse). */
static void PCPort_HSDApplyHostRelocations(PCPortHSDArchive* a) {
    u32 i;
    for (i = 0; i < a->relocCount; ++i) {
        u32 relocEntryOffset = a->relocOffset + (i * 4u);
        u32 fieldOffset, fieldAbs, value;
        if (relocEntryOffset + 4u > a->storageSize) break;
        fieldOffset = ReadBE32(a->storage + relocEntryOffset);
        fieldAbs = a->dataOffset + fieldOffset;
        if (fieldAbs + 4u > a->storageSize) continue;
        value = ReadBE32(a->storage + fieldAbs);     /* dataOffset+offset (abs) */
        if (value == 0u || value >= a->storageSize) continue;
        /* native host pointer (32-bit build: pointer fits in u32) */
        *(u32*)(a->storage + fieldAbs) = (u32)(uintptr_t)(a->storage + value);
    }
}

/* True iff the data field at absolute storage offset `absOff` is a relocated
 * pointer (i.e. appears in the HSD archive reloc table). The reloc table stores
 * DATA-relative offsets (fieldAbs = dataOffset + entry), so we compare against
 * (absOff - dataOffset). This is the ground truth for struct layout RE: a
 * reloc'd field IS a pointer; a non-reloc'd field is a scalar (flags/float/int).
 * Linear scan -- fine for a one-shot probe. */
static BOOL PCPortHSDIsRelocField(const PCPortHSDArchive* a, u32 absOff) {
    u32 i, rel;
    if (absOff < a->dataOffset) {
        return FALSE;
    }
    rel = absOff - a->dataOffset;
    for (i = 0; i < a->relocCount; ++i) {
        u32 e = a->relocOffset + (i * 4u);
        if (e + 4u > a->storageSize) {
            break;
        }
        if (ReadBE32(a->storage + e) == rel) {
            return TRUE;
        }
    }
    return FALSE;
}

/* Dump `nwords` 32-bit words at absolute storage offset `off`, annotating each
 * relocated pointer field with [ptr] and showing the int/float reading of every
 * scalar. Cross-referencing the reloc table reveals the true (Colosseum/XD-
 * specific) HSD AnimJoint / AObjDesc / FObjDesc layouts, which diverge from the
 * stock Melee structs (the stock-layout assumption crashes HSD_AObjLoadDesc). */
static void PCPortHSDDumpStruct(const PCPortHSDArchive* a, u32 off,
                                u32 nwords, const char* label) {
    u32 w;
    if (off == 0u || off >= a->storageSize) {
        printf("[hsd-swiz] %s @0x%X: (out of range)\n", label, off);
        return;
    }
    printf("[hsd-swiz] %s @0x%X:\n", label, off);
    for (w = 0; w < nwords; ++w) {
        u32 foff = off + (w * 4u);
        u32 v;
        union { u32 u; f32 f; } cvt;
        if (foff + 4u > a->storageSize) {
            break;
        }
        v = ReadBE32(a->storage + foff);
        cvt.u = v;
        if (PCPortHSDIsRelocField(a, foff)) {
            printf("[hsd-swiz]   +0x%-2X = 0x%08X  [ptr]\n", w * 4u, v);
        } else {
            printf("[hsd-swiz]   +0x%-2X = 0x%08X  (int=%d float=%.5g)\n",
                   w * 4u, v, (int) v, (double) cvt.f);
        }
    }
}

/* Raw byte hexdump at absolute storage offset `off` (for decoding the FObj
 * keyframe stream `ad` against the known packed format). */
static void PCPortHSDHexDump(const PCPortHSDArchive* a, u32 off,
                             u32 nbytes, const char* label) {
    u32 i;
    if (off == 0u || off >= a->storageSize) {
        printf("[hsd-swiz] %s @0x%X: (out of range)\n", label, off);
        return;
    }
    printf("[hsd-swiz] %s @0x%X (%u bytes):\n", label, off, nbytes);
    for (i = 0; i < nbytes; i += 16u) {
        u32 j;
        printf("[hsd-swiz]   +0x%-3X:", i);
        for (j = 0; j < 16u && (i + j) < nbytes; ++j) {
            if (off + i + j >= a->storageSize) break;
            printf(" %02X", a->storage[off + i + j]);
        }
        printf("\n");
    }
}

/* Public: prepare a parsed archive's scene-data joint graph for the game's HSD
 * pipeline. `rootJointOffset` is the storage offset of the scene root HSD_Joint
 * (scene_data -> branch -> jointList). Returns the root joint as a native ptr. */
/* Smoke test: load a scene member, resolve the root joint (BE), swizzle, then
 * read back NATIVE-LE fields to confirm the swap produced sane values. Prints a
 * report. Verifies the swizzle math independently of the load/render. */
void PCPort_HSDSwizzleSmoke(const char* fsysPath, const char* memberName) {
    u8* data = NULL;
    u32 size = 0;
    PCPortHSDArchive archive;
    const u8* sceneData;
    u32 sceneOffset = 0, branchOff, jointListOff, rootOff;
    void* rootPtr;

    setvbuf(stdout, NULL, _IONBF, 0); /* unbuffered so a crash doesn't eat output */
    if (!PCPort_LoadFsysMember(fsysPath, memberName, &data, &size)) {
        printf("[hsd-swiz] load %s:%s FAILED\n", fsysPath, memberName);
        return;
    }
    if (!PCPort_HSDArchiveParseBE(&archive, data, size)) {
        printf("[hsd-swiz] parse FAILED\n");
        free(data);
        return;
    }
    /* Enumerate ALL public symbols (find any separate animjoint/matanim_joint). */
    {
        u32 k;
        printf("[hsd-swiz] %u public symbols:\n", archive.publicCount);
        for (k = 0; k < archive.publicCount; ++k) {
            u32 entryOff = archive.publicOffset + k * 8u;
            u32 dataOff, key, nameOff;
            if (entryOff + 8u > archive.storageSize) break;
            dataOff = ReadBE32(archive.storage + entryOff + 0);
            key = ReadBE32(archive.storage + entryOff + 4);
            nameOff = archive.stringOffset + key;
            if (nameOff < archive.storageSize) {
                printf("[hsd-swiz]   pub[%u] '%s' @0x%X\n", k,
                       (const char*)(archive.storage + nameOff), dataOff);
            }
        }
    }

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive, "scene_data", &sceneOffset);
    if (sceneData == NULL) { printf("[hsd-swiz] no scene_data\n"); goto done; }
    branchOff = ReadBE32(sceneData + 0x00);
    jointListOff = ReadBE32(archive.storage + branchOff + 0x00);
    rootOff = ReadBE32(archive.storage + jointListOff + 0x00);
    /* Dump the scene branch + jointList structs (find animjoint/matanimjoint:
     * a field whose value is a valid data-section offset [0x20, dataSize)). */
    {
        u32 w;
        printf("[hsd-swiz] branch@0x%X:", branchOff);
        for (w = 0; w < 0x20u; w += 4u) {
            u32 v = ReadBE32(archive.storage + branchOff + w);
            int isPtr = (v >= 0x20u && v < 0x20u + archive.dataSize);
            printf(" +%X=0x%X%s", w, v, isPtr ? "*" : "");
        }
        printf("\n[hsd-swiz] jointList@0x%X:", jointListOff);
        for (w = 0; w < 0x18u; w += 4u) {
            u32 v = ReadBE32(archive.storage + jointListOff + w);
            int isPtr = (v >= 0x20u && v < 0x20u + archive.dataSize);
            printf(" +%X=0x%X%s", w, v, isPtr ? "*" : "");
        }
        printf("\n");
        /* Dump each branch model-set slot (stride 8) as a jointList {root,anim,
         * matanim}: for a character the extra slots are candidate motions. */
        {
            u32 si;
            for (si = 0; si < 6u; ++si) {
                u32 slotOff = ReadBE32(archive.storage + branchOff + si * 8u);
                u32 r, aj2, mj2;
                if (slotOff < 0x20u || slotOff >= 0x20u + archive.dataSize) break;
                r   = ReadBE32(archive.storage + slotOff + 0x0);
                aj2 = ReadBE32(archive.storage + slotOff + 0x4);
                mj2 = ReadBE32(archive.storage + slotOff + 0x8);
                printf("[hsd-swiz] branchSlot[%u]@0x%X: root=0x%X anim=0x%X matanim=0x%X", si, slotOff, r, aj2, mj2);
                /* If anim looks like an animjoint, peek its aobjdesc end_frame. */
                if (aj2 >= 0x20u && aj2 < 0x20u + archive.dataSize) {
                    u32 aod = ReadBE32(archive.storage + aj2 + 0x8);
                    if (aod >= 0x20u && aod < 0x20u + archive.dataSize) {
                        union { u32 u; f32 f; } ef; ef.u = ReadBE32(archive.storage + aod + 4);
                        printf("  [anim aobjdesc end_frame=%.2f fobjdesc=0x%X]", ef.f, ReadBE32(archive.storage+aod+8));
                    }
                }
                printf("\n");
            }
        }
        /* Candidate animjoint(+4) + matanimjoint(+8): dump their first words. */
        {
            u32 aj = ReadBE32(archive.storage + jointListOff + 0x4);
            u32 mj = ReadBE32(archive.storage + jointListOff + 0x8);
            if (aj >= 0x20u && aj < 0x20u + archive.dataSize) {
                printf("[hsd-swiz] +4@0x%X (animjoint?): child=0x%X next=0x%X aobjdesc=0x%X robjanim=0x%X flags=0x%X\n",
                       aj, ReadBE32(archive.storage+aj+0), ReadBE32(archive.storage+aj+4),
                       ReadBE32(archive.storage+aj+8), ReadBE32(archive.storage+aj+0xC),
                       ReadBE32(archive.storage+aj+0x10));
            }
            if (mj >= 0x20u && mj < 0x20u + archive.dataSize) {
                printf("[hsd-swiz] +8@0x%X (matanimjoint?): child=0x%X next=0x%X matanim=0x%X\n",
                       mj, ReadBE32(archive.storage+mj+0), ReadBE32(archive.storage+mj+4),
                       ReadBE32(archive.storage+mj+8));
            }
            /* Raw aobjdesc (BE, pre-swizzle): animjoint(+8) should be an AObjDesc
             * {flags@0, end_frame@4 (float), fobjdesc@8, obj_id@C}. */
            if (aj >= 0x20u && aj < 0x20u + archive.dataSize) {
                u32 aod = ReadBE32(archive.storage + aj + 0x8);
                if (aod >= 0x20u && aod < 0x20u + archive.dataSize) {
                    union { u32 u; f32 f; } ef; ef.u = ReadBE32(archive.storage+aod+4);
                    printf("[hsd-swiz] aobjdesc@0x%X: flags=0x%X end_frame=%.3f fobjdesc=0x%X obj_id=0x%X\n",
                           aod, ReadBE32(archive.storage+aod+0), ef.f,
                           ReadBE32(archive.storage+aod+8), ReadBE32(archive.storage+aod+0xC));
                } else {
                    printf("[hsd-swiz] animjoint+8=0x%X NOT a valid data offset -> +8 isn't aobjdesc\n", aod);
                }
            }
        }
    }
    /* Reloc-aware layout dump: the reloc table marks exactly which fields are
     * pointers, so these dumps reveal the true Colosseum HSD anim-struct layout
     * (vs the stock Melee structs that crash HSD_AObjLoadDesc). Each [ptr] line
     * is a relocated pointer field; scalars show int/float. Walk:
     *   jointList+4 (animjoint-tree root) -> its aobjdesc -> its fobjdesc
     *   jointList+8 (matanimjoint-tree root) -> its matanim. */
    {
        u32 aj = ReadBE32(archive.storage + jointListOff + 0x4);
        u32 mj = ReadBE32(archive.storage + jointListOff + 0x8);
        PCPortHSDDumpStruct(&archive, jointListOff, 8, "jointList");
        if (aj >= archive.dataOffset) {
            u32 ajAobj, ajFobj;
            PCPortHSDDumpStruct(&archive, aj, 8, "animjoint(+4)");
            /* Try the stock AnimJoint aobjdesc slot (+8) and dump what it points
             * to + the slot a possible 0x10-byte-AnimJoint variant would use. */
            ajAobj = ReadBE32(archive.storage + aj + 0x8);
            if (ajAobj >= archive.dataOffset) {
                PCPortHSDDumpStruct(&archive, ajAobj, 6, "  animjoint+8->(aobjdesc?)");
                ajFobj = ReadBE32(archive.storage + ajAobj + 0x8);
                if (ajFobj >= archive.dataOffset) {
                    u32 adPtr;
                    PCPortHSDDumpStruct(&archive, ajFobj, 8, "    aobjdesc+8->(fobjdesc?)");
                    /* the fobjdesc's only [ptr] is the keyframe stream `ad`:
                     * hexdump it + decode the header byte against the FObj format
                     * (low nibble = op 1..6, high bits = run-length varint). */
                    adPtr = ReadBE32(archive.storage + ajFobj + 0x8);
                    if (adPtr >= archive.dataOffset) {
                        PCPortHSDHexDump(&archive, adPtr, 48, "      fobjdesc+8->(keyframe ad?)");
                    }
                }
            }
        }
        if (mj >= archive.dataOffset) {
            u32 mjMat;
            PCPortHSDDumpStruct(&archive, mj, 6, "matanimjoint(+8)");
            mjMat = ReadBE32(archive.storage + mj + 0x8);
            if (mjMat >= archive.dataOffset) {
                PCPortHSDDumpStruct(&archive, mjMat, 8, "  matanimjoint+8->(matanim?)");
            }
        }
    }
    printf("[hsd-swiz] scene=0x%X branch=0x%X jointList=0x%X root=0x%X relocs=%u\n",
           sceneOffset, branchOff, jointListOff, rootOff, archive.relocCount);

    rootPtr = PCPort_SwizzleSceneForHSD(&archive, jointListOff);  /* jointList host ptr */
    if (rootPtr == NULL) { printf("[hsd-swiz] swizzle FAILED\n"); goto done; }

    {
        /* Model entry: +0 rootJoint, +4 animjoint, +8 matanimjoint (host ptrs). */
        u8* jl = (u8*)rootPtr;
        u8* j = (u8*)(uintptr_t)(*(u32*)(jl + 0x0));   /* root HSD_Joint */
        u32 flags = *(u32*)(j + 0x4);
        f32 rx = *(f32*)(j + 0x14), ry = *(f32*)(j + 0x18), rz = *(f32*)(j + 0x1C);
        f32 sx = *(f32*)(j + 0x20), sy = *(f32*)(j + 0x24), sz = *(f32*)(j + 0x28);
        f32 px = *(f32*)(j + 0x2C), py = *(f32*)(j + 0x30), pz = *(f32*)(j + 0x34);
        u32 dobj = *(u32*)(j + 0x10);   /* now native host pointer */
        u8* stack[256]; int sp = 0; int scanned = 0;
        printf("[hsd-swiz] ROOT flags=0x%08X rot=(%.3f,%.3f,%.3f) scale=(%.3f,%.3f,%.3f) pos=(%.1f,%.1f,%.1f) dobj=%p\n",
               flags, rx, ry, rz, sx, sy, sz, px, py, pz, (void*)(uintptr_t)dobj);
        /* Walk the joint tree (host pointers now) to the first dobj-bearing joint. */
        stack[sp++] = j;
        while (sp > 0 && dobj == 0u && scanned < 4096) {
            u8* cur = stack[--sp]; u32 ch, nx;
            ++scanned;
            dobj = *(u32*)(cur + 0x10);
            if (dobj != 0u) { j = cur; break; }
            ch = *(u32*)(cur + 0x8); nx = *(u32*)(cur + 0xC);
            if (nx != 0u && sp < 256) stack[sp++] = (u8*)(uintptr_t)nx;
            if (ch != 0u && sp < 256) stack[sp++] = (u8*)(uintptr_t)ch;
        }
        printf("[hsd-swiz] first dobj-joint after %d nodes: dobj=%p\n", scanned, (void*)(uintptr_t)dobj);
        if (dobj != 0u) {
            u8* d = (u8*)(uintptr_t)dobj;
            u32 mobj = *(u32*)(d + 0x8);
            if (mobj != 0u) {
                u8* m = (u8*)(uintptr_t)mobj;
                u32 rendermode = *(u32*)(m + 0x4);
                u32 mat = *(u32*)(m + 0xC);
                u32 tobj = *(u32*)(m + 0x8);
                printf("[hsd-swiz] MObj rendermode=0x%X mat=%p tobj=%p\n",
                       rendermode, (void*)(uintptr_t)mat, (void*)(uintptr_t)tobj);
                if (mat != 0u) {
                    u8* mm = (u8*)(uintptr_t)mat;
                    printf("[hsd-swiz]   Material ambient=0x%08X diffuse=0x%08X alpha=%.3f shininess=%.2f\n",
                           *(u32*)(mm + 0x0), *(u32*)(mm + 0x4), *(f32*)(mm + 0xC), *(f32*)(mm + 0x10));
                }
                if (tobj != 0u) {
                    u8* tt = (u8*)(uintptr_t)tobj;
                    printf("[hsd-swiz]   TObj scale=(%.3f,%.3f,%.3f) wrap_s=%u img=%p\n",
                           *(f32*)(tt + 0x1C), *(f32*)(tt + 0x20), *(f32*)(tt + 0x24),
                           *(u32*)(tt + 0x34), (void*)(uintptr_t)*(u32*)(tt + 0x4C));
                }
            }
        }
    }
    /* Build the REAL HSD_JObj tree (game's HSD_JObjLoadJoint + HSD_*LoadDesc),
     * attach the SRT + material/texture animation via the game's real
     * HSD_JObjAddAnimAll, and count joints/dobjs + attached aobjs (proves the
     * sand-scroll animation is wired through the game's own code). */
    {
        u8* jl = (u8*)rootPtr;
        HSD_Joint* rootJoint = (HSD_Joint*)(uintptr_t)(*(u32*)(jl + 0x0));
        void* animjoint = (void*)(uintptr_t)(*(u32*)(jl + 0x4));
        void* matanimjoint = (void*)(uintptr_t)(*(u32*)(jl + 0x8));
        HSD_JObj* root = HSD_JObjLoadJoint(rootJoint);
        HSD_JObj* stk[512]; int sp = 0, nj = 0, nd = 0, na = 0, guard = 0;
        /* Anim attach: jointList+4 = root AnimJoint (SRT anim), jointList+8 = root
         * MatAnimJoint (material/texture UV-scroll). The Colosseum AnimJoint/AObjDesc/
         * FObjDesc layouts are confirmed and the swizzle (above) now runs
         * unconditionally, so attach the animation through the game's real
         * HSD_JObjAddAnimAll (loads AObj/FObj from the descriptors). */
        {
            /* Isolation: load the root animjoint's aobjdesc alone first (tests the
             * AObjDesc/FObjDesc swizzle + HSD_AObjLoadDesc/FObj load path). */
            u32 ajAobjDesc = animjoint ? *(u32*)((u8*)animjoint + 0x8) : 0u;
            printf("[hsd-swiz] anim: animjoint=%p aobjdesc=0x%X\n", animjoint, ajAobjDesc);
            if (ajAobjDesc != 0u) {
                void* a = HSD_AObjLoadDesc((HSD_AObjDesc*)(uintptr_t)ajAobjDesc);
                printf("[hsd-swiz] HSD_AObjLoadDesc(root aobjdesc) -> %p OK\n", a);
            }
            printf("[hsd-swiz] calling HSD_JObjAddAnimAll...\n");
            HSD_JObjAddAnimAll(root, (HSD_AnimJoint*)animjoint,
                               (HSD_MatAnimJoint*)matanimjoint, NULL);
            printf("[hsd-swiz] HSD_JObjAddAnimAll done\n");
            /* Kick the FObj state machines so the interpreter actually produces
             * values (HSD_FObjReqAnimAll alone leaves them idle on the adapted
             * src path). HSD_JObjReqAnimAll requests playback; then walk the tree
             * and start each AObj's FObj chain. */
            HSD_JObjReqAnimAll(root, 0.0f);
            PCPort_HSDStartAnimAll(root);
        }
        if (root != NULL) {
            stk[sp++] = root;
            while (sp > 0 && guard < 100000) {
                HSD_JObj* cur = stk[--sp];
                ++guard; ++nj;
                if (union_type_dobj(cur) && cur->u.dobj != NULL) ++nd;
                if (cur->aobj != NULL) ++na;
                if (cur->next != NULL && sp < 512) stk[sp++] = cur->next;
                if (cur->child != NULL && sp < 512) stk[sp++] = cur->child;
            }
        }
        printf("[hsd-swiz] LoadJoint+AddAnimAll -> root=%p joints=%d dobjs=%d jobj-aobjs=%d (animjoint=%p matanim=%p)\n",
               (void*)root, nj, nd, na, animjoint, matanimjoint);
    }
done:
    PCPort_HSDArchiveDestroy(&archive);
    free(data);
}

/* ========================================================================= */
/*  Title HSD animation drive (persistent)                                    */
/* ========================================================================= */

/*
 * Build a live, animated HSD_JObj tree from a scene member and arm its animation,
 * then advance it once per frame. This drives the title material/UV (and SRT)
 * animation through the game's REAL pipeline:
 *   swizzle (BE->LE) -> HSD_JObjLoadJoint -> HSD_JObjAddAnimAll
 *   -> HSD_JObjReqAnimAll + PCPort_HSDStartAnimAll  (arm FObj state machines)
 *   -> [per frame] HSD_JObjAnimAll  -> HSD_JObjAnim / HSD_TObjAnim (host overrides)
 *      -> HSD_AObjInterpretAnim -> FObj interpreter (hsd_fobj_host.c)
 *      -> PCPort_JObjUpdateFunc / PCPort_TObjUpdateFunc -> live HSD_JObj/HSD_TObj
 *         SRT fields (+ TEX_MTX_DIRTY).
 *
 * NOTE (remaining wiring): the title 3D render (pcport_main.c RenderJointTree)
 * reads RAW big-endian archive bytes, NOT this live HSD tree, so the animated
 * HSD_TObj/HSD_JObj fields updated here are not yet consumed by the rasteriser.
 * Making the animation visible requires routing the title render through this
 * live tree (HSD_JObjDispAll / a tree-driven RenderJointTree) and building the
 * texture matrix from the live HSD_TObj SRT into the GX texgen path. This module
 * provides the verified, correct animation engine drive; the render-readback is
 * the next step. PCPORT_TITLE_ANIM_DEBUG prints the root SRT each tick so the
 * drive can be verified to actually move values without the render path.
 */

static PCPortHSDArchive g_titleAnimArchive;
static HSD_JObj*        g_titleAnimRoot = NULL;
static int             g_titleAnimReady = 0;
static u8*             g_titleAnimData = NULL;

int PCPort_TitleAnimSetup(const char* fsysPath, const char* memberName) {
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOffset = 0, branchOff, jointListOff;
    void* rootPtr;
    HSD_Joint* rootJoint;
    void* animjoint;
    void* matanimjoint;

    g_titleAnimReady = 0;
    g_titleAnimRoot = NULL;
    memset(&g_titleAnimArchive, 0, sizeof(g_titleAnimArchive));

    if (!PCPort_LoadFsysMember(fsysPath, memberName, &data, &size)) {
        printf("[title-anim] load %s:%s FAILED\n", fsysPath, memberName);
        return 0;
    }
    if (!PCPort_HSDArchiveParseBE(&g_titleAnimArchive, data, size)) {
        printf("[title-anim] parse FAILED\n");
        free(data);
        return 0;
    }
    g_titleAnimData = data; /* keep alive: the live HSD tree points into storage */

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_titleAnimArchive,
                                                             "scene_data", &sceneOffset);
    if (sceneData == NULL) {
        printf("[title-anim] no scene_data\n");
        PCPort_HSDArchiveDestroy(&g_titleAnimArchive);
        free(data); g_titleAnimData = NULL;
        return 0;
    }
    branchOff = ReadBE32(sceneData + 0x00);
    /* PCPORT_ANIM_SLOT picks which branch model-set slot to load (stride 8:
     * slot0=+0, slot1=+8, ...). For characters the extra slots are candidate
     * alternate motions (idle/walk/run); default 0 keeps existing behavior. */
    {
        const char* sl = getenv("PCPORT_ANIM_SLOT");
        u32 slot = (sl != NULL) ? (u32)atoi(sl) : 0u;
        jointListOff = ReadBE32(g_titleAnimArchive.storage + branchOff + slot * 8u);
    }

    /* Swizzle BE->LE (joint tree + anim + matanim) and relocate to host ptrs. */
    rootPtr = PCPort_SwizzleSceneForHSD(&g_titleAnimArchive, jointListOff);
    if (rootPtr == NULL) {
        printf("[title-anim] swizzle FAILED\n");
        PCPort_HSDArchiveDestroy(&g_titleAnimArchive);
        free(data); g_titleAnimData = NULL;
        return 0;
    }

    {
        u8* jl = (u8*)rootPtr;
        rootJoint    = (HSD_Joint*)(uintptr_t)(*(u32*)(jl + 0x0));
        animjoint    = (void*)(uintptr_t)(*(u32*)(jl + 0x4));
        matanimjoint = (void*)(uintptr_t)(*(u32*)(jl + 0x8));
    }

    g_titleAnimRoot = HSD_JObjLoadJoint(rootJoint);
    if (g_titleAnimRoot == NULL) {
        printf("[title-anim] HSD_JObjLoadJoint FAILED\n");
        PCPort_HSDArchiveDestroy(&g_titleAnimArchive);
        free(data); g_titleAnimData = NULL;
        return 0;
    }

    /* Attach SRT + material/texture animation, then arm the FObj state machines. */
    HSD_JObjAddAnimAll(g_titleAnimRoot, (HSD_AnimJoint*)animjoint,
                       (HSD_MatAnimJoint*)matanimjoint, NULL);
    HSD_JObjReqAnimAll(g_titleAnimRoot, 0.0f);
    PCPort_HSDStartAnimAll(g_titleAnimRoot);

    g_titleAnimReady = 1;
    printf("[title-anim] setup OK (root=%p animjoint=%p matanim=%p)\n",
           (void*)g_titleAnimRoot, animjoint, matanimjoint);
    return 1;
}

void PCPort_TitleAnimTick(void) {
    if (!g_titleAnimReady || g_titleAnimRoot == NULL) {
        return;
    }
    HSD_JObjAnimAll(g_titleAnimRoot);
    if (getenv("PCPORT_TITLE_ANIM_DEBUG") != NULL) {
        HSD_JObj* r = g_titleAnimRoot;
        printf("[title-anim] tick root SRT t=(%.3f,%.3f,%.3f) r=(%.3f,%.3f,%.3f) s=(%.3f,%.3f,%.3f)\n",
               r->translate_x, r->translate_y, r->translate_z,
               r->rotate_x, r->rotate_y, r->rotate_z,
               r->scale_x, r->scale_y, r->scale_z);
    }
}

/* ------------------------------------------------------------------------- */
/*  PCPort_FieldAnimSetup / PCPort_FieldAnimTick                             */
/*                                                                           */
/*  Scene-ambient animation for field maps (e.g. signpost swing in S1_out).  */
/*  Works exactly like PCPort_TitleAnimSetup but uses separate globals so    */
/*  the title and field paths are independent.  A fresh copy of the archive  */
/*  member is loaded and swizzled (BE->LE) without touching the render-side  */
/*  g_engTitleArchive which RenderJointTree reads as raw-BE.  The live       */
/*  HSD_JObj tree is stepped each frame via PCPort_FieldAnimTick.            */
/*                                                                           */
/*  The live JObj SRT updates land in the HSD runtime structs; after each    */
/*  tick they are pushed back into the render-side raw-BE archive storage    */
/*  via PCPort_CharAnimLockstepWrite (see PCPort_FieldAnimSetRenderTarget),  */
/*  so RenderJointTree sees the animation.                                   */
/* ------------------------------------------------------------------------- */

static PCPortHSDArchive g_fieldAnimArchive;
static HSD_JObj*        g_fieldAnimRoot      = NULL;
static int              g_fieldAnimReady     = 0;
static u8*              g_fieldAnimData      = NULL;
/* Looping: track accumulated frame time and loop length so FObj frames
 * wrap around instead of freezing at end_frame. */
static f32              g_fieldAnimTime      = 0.0f;
static f32              g_fieldAnimLoopLen   = -1.0f;
/* Render-side write-back target: the BE archive + root joint offset that
 * RenderJointTree reads from.  Set by PCPort_FieldAnimSetRenderTarget after
 * PCPort_EngineFieldSetup completes so Tick can push updated SRT into the
 * render storage each frame. */
static PCPortHSDArchive* g_fieldAnimRenderArchive = NULL;
static u32               g_fieldAnimRenderRootOff = 0u;

/* Find the largest end_frame in an HSD_JObj tree (loop length). */
static f32 PCPort_FieldAnimMaxEndFrame(HSD_JObj* root) {
    f32 maxEnd = 0.0f;
    HSD_JObj* st[512]; int sp = 0;
    st[sp++] = root;
    while (sp > 0) {
        HSD_JObj* lv = st[--sp];
        if (!lv) continue;
        if (lv->aobj != NULL) {
            HSD_AObj* ao = lv->aobj;
            if (ao->end_frame > maxEnd) maxEnd = ao->end_frame;
        }
        if (lv->child != NULL && sp < 512) st[sp++] = lv->child;
        if (lv->next  != NULL && sp < 512) st[sp++] = lv->next;
    }
    return maxEnd;
}

/* Setup: load a fresh copy of the field map's scene member, swizzle it, and
 * arm the animjoint tree.  fsysPath is the .fsys file; memberName may be NULL
 * (uses the largest scene_data member, same as PCPort_EngineFieldSetup).
 * Returns 1 on success, 0 if no animjoint data present (static map). */
int PCPort_FieldAnimSetup(const char* fsysPath, const char* memberName,
                          u32* outAnimRootOff) {
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOffset = 0;
    u32 branchOff, jointListOff;
    u32 animRootOff = 0u;
    void* rawPtr;
    HSD_Joint* rootJoint;
    void* animjoint;
    void* matanimjoint;
    u8* jl;

    g_fieldAnimReady = 0;
    g_fieldAnimRoot  = NULL;
    g_fieldAnimTime  = 0.0f;
    g_fieldAnimLoopLen = -1.0f;
    memset(&g_fieldAnimArchive, 0, sizeof(g_fieldAnimArchive));

    /* Load a fresh copy (separate from the render-side archive). */
    {
        BOOL loaded = (memberName != NULL && memberName[0] != '\0')
            ? PCPort_LoadFsysMember(fsysPath, memberName, &data, &size)
            : PCPort_LoadFsysSceneMember(fsysPath, &data, &size);
        if (!loaded) {
            printf("[field-anim] load %s failed\n", fsysPath);
            return 0;
        }
    }
    if (!PCPort_HSDArchiveParseBE(&g_fieldAnimArchive, data, size)) {
        printf("[field-anim] archive parse failed\n");
        free(data);
        return 0;
    }
    g_fieldAnimData = data; /* keep alive: live HSD tree points into storage */

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_fieldAnimArchive,
                                                             "scene_data", &sceneOffset);
    if (sceneData == NULL) {
        printf("[field-anim] no scene_data\n");
        PCPort_HSDArchiveDestroy(&g_fieldAnimArchive);
        free(data); g_fieldAnimData = NULL;
        return 0;
    }
    branchOff = ReadBE32(sceneData + 0x00);

    /* Find the first jointList slot in the scene branch that has a non-null
     * animjoint.  Field-map scene branches use stride-4 slots; each slot
     * pointer refers to a {rootJoint, animjoint, matanimjoint} triple. */
    {
        u32 slot;
        jointListOff = 0u;
        for (slot = 0x0u; slot <= 0x20u; slot += 0x4u) {
            u32 jl = ReadBE32(g_fieldAnimArchive.storage + branchOff + slot);
            u32 rj, aj;
            if (!IsArchiveRangeValid(&g_fieldAnimArchive, jl, 0x0Cu)) break;
            rj = ReadBE32(g_fieldAnimArchive.storage + jl + 0x0);
            if (!IsArchiveRangeValid(&g_fieldAnimArchive, rj, 4u)) break;
            aj = ReadBE32(g_fieldAnimArchive.storage + jl + 0x4);
            if (IsArchiveRangeValid(&g_fieldAnimArchive, aj, 0x10u)) {
                jointListOff  = jl;
                animRootOff   = rj; /* archive offset of this slot's rootJoint */
                break;
            }
        }
        if (jointListOff == 0u) {
            PCPort_HSDArchiveDestroy(&g_fieldAnimArchive);
            free(data); g_fieldAnimData = NULL;
            return 0; /* static map: no animjoint in any slot */
        }
    }
    if (outAnimRootOff != NULL) *outAnimRootOff = animRootOff;

    /* Swizzle this SEPARATE copy BE->LE + relocate pointers. */
    rawPtr = PCPort_SwizzleSceneForHSD(&g_fieldAnimArchive, jointListOff);
    if (rawPtr == NULL) {
        printf("[field-anim] swizzle failed\n");
        PCPort_HSDArchiveDestroy(&g_fieldAnimArchive);
        free(data); g_fieldAnimData = NULL;
        return 0;
    }

    jl = (u8*)rawPtr;
    rootJoint    = (HSD_Joint*)(uintptr_t)(*(u32*)(jl + 0x0));
    animjoint    = (void*)(uintptr_t)(*(u32*)(jl + 0x4));
    matanimjoint = (void*)(uintptr_t)(*(u32*)(jl + 0x8));

    g_fieldAnimRoot = HSD_JObjLoadJoint(rootJoint);
    if (g_fieldAnimRoot == NULL) {
        printf("[field-anim] HSD_JObjLoadJoint failed\n");
        PCPort_HSDArchiveDestroy(&g_fieldAnimArchive);
        free(data); g_fieldAnimData = NULL;
        return 0;
    }

    /* The scene animjoint tree's root node is a virtual container that has no
     * corresponding JObj.  Its child subtree maps 1:1 to the rootJoint's JObj
     * tree and carries the actual SRT keyframes.  Skip the root so
     * HSD_JObjAddAnimAll pairs at the correct depth and reaches the keyed nodes
     * (S1_out: 4 animated joints at tree indices 31, 32, 83, 85; end_frame=179). */
    {
        HSD_AnimJoint* aj = (HSD_AnimJoint*)animjoint;
        if (aj != NULL && aj->child != NULL)
            animjoint = aj->child;
    }

    HSD_JObjAddAnimAll(g_fieldAnimRoot,
                       (HSD_AnimJoint*)animjoint,
                       (HSD_MatAnimJoint*)matanimjoint,
                       NULL);
    HSD_JObjReqAnimAll(g_fieldAnimRoot, 0.0f);
    PCPort_HSDStartAnimAll(g_fieldAnimRoot);

    g_fieldAnimReady = 1;
    return 1;
}

/* Forward declaration: defined later in this file (char-anim section). */
static void PCPort_CharAnimLockstepWrite(PCPortHSDArchive* be, u32 beJointOff,
                                         HSD_JObj* live, int isRoot,
                                         int applyRootTranslate);

/* Advance the field scene animation by frameStep game frames (1.0 = one
 * 60 Hz tick).  Loops the animation by re-arming the FObj state machines
 * when the accumulated time exceeds the loop length. */
void PCPort_FieldAnimTick(f32 frameStep) {
    if (!g_fieldAnimReady || g_fieldAnimRoot == NULL) {
        return;
    }
    if (g_fieldAnimLoopLen < 0.0f) {
        g_fieldAnimLoopLen = PCPort_FieldAnimMaxEndFrame(g_fieldAnimRoot);
        if (g_fieldAnimLoopLen < 1.0f) g_fieldAnimLoopLen = 0.0f;
    }
    if (g_fieldAnimLoopLen > 0.0f) {
        g_fieldAnimTime += frameStep;
        if (g_fieldAnimTime >= g_fieldAnimLoopLen) {
            g_fieldAnimTime = 0.0f;
            HSD_JObjReqAnimAll(g_fieldAnimRoot, 0.0f);
            PCPort_HSDStartAnimAll(g_fieldAnimRoot);
        }
    }
    /* Field maps currently need joint/RObj animation for ambient motion. The
     * host DObj/MObj animation path is still unsafe for some swizzled field
     * descriptors, so material/texture animation is harvested from already-live
     * state until the exact material path is recovered. */
    PCPort_HSDJObjAnimJointOnlyAll(g_fieldAnimRoot);

    /* Push updated SRT into the render-side BE archive so RenderJointTree
     * sees the animation.  Uses the same lockstep-write pattern as
     * PCPort_CharAnimLockstepWrite (applyRootTranslate=1: write all fields
     * including root translation so the signpost pivot moves correctly). */
    if (g_fieldAnimRenderArchive != NULL && g_fieldAnimRenderRootOff != 0u) {
        PCPort_CharAnimLockstepWrite(g_fieldAnimRenderArchive,
                                     g_fieldAnimRenderRootOff,
                                     g_fieldAnimRoot,
                                     1, /* isRoot */
                                     1  /* applyRootTranslate */);
    }
}

/* -------------------------------------------------------------------------
 *  PCPort_FieldAnimHarvestTexUV
 *
 *  After each PCPort_FieldAnimTick, walk the live g_fieldAnimRoot tree and
 *  the render-side BE archive simultaneously (lockstep).  For every TObj
 *  whose TexAnim drove a non-zero UV translation, store
 *  (beTObjOffset, translate_x, translate_y) in g_texUVTable.
 *
 *  RenderJointTree looks up the TObj archive offset it finds in the BE
 *  archive against this table.  On a hit it builds a 3x3 UV-translation
 *  matrix and calls GXHostSetTexMatrix(coordSlot, m) before drawing so
 *  the GLSL shader offsets the UV coordinates, producing the cloud drift.
 *
 *  The render-side BE TObj archive offset is the same across all frames
 *  (the archive bytes don't move) so the key is stable.  The table is
 *  rebuilt every tick so stale values age out automatically.
 * ----------------------------------------------------------------------- */

#define PCPORT_TEX_UV_TABLE_MAX 64
typedef struct {
    u32 tobjOff;     /* render-side BE archive offset of this TObj */
    f32 translateU;  /* HSD_TObj::translate_x after animation tick  */
    f32 translateV;  /* HSD_TObj::translate_y after animation tick  */
} PCPortTexUVEntry;

static PCPortTexUVEntry g_texUVTable[PCPORT_TEX_UV_TABLE_MAX];
static int              g_texUVCount = 0;

/* Recursive lockstep walk: compare live HSD_JObj tree with BE archive
 * joint-by-joint, harvest TObj UV offsets into g_texUVTable. */
static void FieldAnimHarvestJoint(PCPortHSDArchive* be, u32 beJointOff,
                                  HSD_JObj* live) {
    u32 beDobjOff, beMobjOff, beTobjOff, beChildOff, beNextOff;
    HSD_DObj* liveDobj;
    HSD_MObj* liveMobj;
    HSD_TObj* liveTobj;

    if (live == NULL || beJointOff == 0u ||
        !IsArchiveRangeValid(be, beJointOff, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return;
    }

    /* Walk this joint's DObj chain in lockstep. */
    beDobjOff = ReadBE32(be->storage + beJointOff + 0x10);
    liveDobj  = union_type_dobj(live) ? live->u.dobj : NULL;
    while (liveDobj != NULL && beDobjOff != 0u &&
           IsArchiveRangeValid(be, beDobjOff, 0x10u)) {
        /* MObj: BE dobj+0x08, live dobj->mobj. */
        beMobjOff = ReadBE32(be->storage + beDobjOff + 0x08);
        liveMobj  = liveDobj->mobj;
        if (liveMobj != NULL && beMobjOff != 0u &&
            IsArchiveRangeValid(be, beMobjOff, 0x0Cu)) {
            /* TObj chain: BE mobj+0x08, live mobj->tobj. */
            beTobjOff = ReadBE32(be->storage + beMobjOff + 0x08);
            liveTobj  = liveMobj->tobj;
            while (liveTobj != NULL && beTobjOff != 0u &&
                   IsArchiveRangeValid(be, beTobjOff, 0x10u) &&
                   g_texUVCount < PCPORT_TEX_UV_TABLE_MAX) {
                /* Only record entries where the animation drove a non-zero
                 * UV offset so the lookup hot-path stays tight. */
                if (liveTobj->translate_x != 0.0f ||
                    liveTobj->translate_y != 0.0f) {
                    g_texUVTable[g_texUVCount].tobjOff    = beTobjOff;
                    g_texUVTable[g_texUVCount].translateU = liveTobj->translate_x;
                    g_texUVTable[g_texUVCount].translateV = liveTobj->translate_y;
                    g_texUVCount++;
                }
                /* Advance TObj chain: BE tobj+0x04 (next ptr), live tobj->next. */
                beTobjOff = ReadBE32(be->storage + beTobjOff + 0x04);
                liveTobj  = liveTobj->next;
            }
        }
        /* Advance DObj chain: BE dobj+0x04 (next ptr), live dobj->next. */
        beDobjOff = ReadBE32(be->storage + beDobjOff + 0x04);
        liveDobj  = liveDobj->next;
    }

    /* Recurse into child/next in lockstep. */
    beChildOff = ReadBE32(be->storage + beJointOff + 0x08);
    beNextOff  = ReadBE32(be->storage + beJointOff + 0x0C);
    FieldAnimHarvestJoint(be, beChildOff, live->child);
    FieldAnimHarvestJoint(be, beNextOff,  live->next);
}

void PCPort_FieldAnimHarvestTexUV(PCPortHSDArchive* renderArchive,
                                   u32 beRootJointOff) {
    g_texUVCount = 0;
    if (!g_fieldAnimReady || g_fieldAnimRoot == NULL ||
        renderArchive == NULL || beRootJointOff == 0u) {
        return;
    }
    FieldAnimHarvestJoint(renderArchive, beRootJointOff, g_fieldAnimRoot);
}

/* Tear down the field scene anim (call before loading a new map). */
void PCPort_FieldAnimRelease(void) {
    g_fieldAnimReady          = 0;
    g_fieldAnimRoot           = NULL;
    g_fieldAnimTime           = 0.0f;
    g_fieldAnimLoopLen        = -1.0f;
    g_fieldAnimRenderArchive  = NULL;
    g_fieldAnimRenderRootOff  = 0u;
    PCPort_HSDArchiveDestroy(&g_fieldAnimArchive);
    if (g_fieldAnimData != NULL) {
        free(g_fieldAnimData);
        g_fieldAnimData = NULL;
    }
}

/* Register the render-side BE archive + root joint offset so PCPort_FieldAnimTick
 * can write updated SRT back into the storage that RenderJointTree reads.
 * Call this from PCPort_EngineFieldSetup after g_engTitleArchive / g_engTitleRootJoint
 * are populated, passing the matching slot's rootJoint offset (the 148-joint model
 * set in S1_out, not the 3-joint outer container). */
void PCPort_FieldAnimSetRenderTarget(PCPortHSDArchive* renderArchive,
                                     u32 renderAnimRootOff) {
    g_fieldAnimRenderArchive = renderArchive;
    g_fieldAnimRenderRootOff = renderAnimRootOff;
}

/* Look up a TObj archive offset in the UV harvest table built by the most
 * recent PCPort_FieldAnimHarvestTexUV call.  Returns 1 and fills *outU / *outV
 * if found; returns 0 if no entry exists for that TObj (identity UV). */
int PCPort_FieldAnimGetTexUV(u32 tobjArchiveOffset, f32* outU, f32* outV) {
    int i;
    for (i = 0; i < g_texUVCount; ++i) {
        if (g_texUVTable[i].tobjOff == tobjArchiveOffset) {
            *outU = g_texUVTable[i].translateU;
            *outV = g_texUVTable[i].translateV;
            return 1;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
/*  PCPort_CharAnimProbe — does a character archive carry REAL joint motion?  */
/*                                                                           */
/*  Decisive diagnostic for wiring overworld animation: builds the live      */
/*  animated tree for <member> (e.g. field_common.fsys :: ken_b1), snapshots */
/*  every joint's SRT, steps the FObj-driven HSD_JObjAnimAll <frames> times,  */
/*  then reports how many joints moved + the largest deltas. A non-zero       */
/*  "moved" count proves the embedded animation is real motion (idle/walk),   */
/*  so the live tree can drive the field skinning. Zero moved => motion lives */
/*  in a separate motion archive and must be located.                         */
/*  Run: PCPORT_CHARANIM_PROBE=<frames> PCPORT_SWIZ_ARCHIVE=.. PCPORT_SWIZ_MEMBER=.. */
/* ------------------------------------------------------------------------- */
#define PCPORT_PROBE_MAX_JOINTS 512
static HSD_JObj* g_probeJoints[PCPORT_PROBE_MAX_JOINTS];
static f32       g_probeRest[PCPORT_PROBE_MAX_JOINTS][9];
static f32       g_probeLoopStart[PCPORT_PROBE_MAX_JOINTS][9];
static f32       g_probeLoopEnd[PCPORT_PROBE_MAX_JOINTS][9];
static int       g_probeCount;
static int       g_probeAnimCount; /* joints with an attached aobj */

static void PCPort_ProbeCollect(HSD_JObj* j) {
    for (; j != NULL; j = j->next) {
        if (g_probeCount < PCPORT_PROBE_MAX_JOINTS) {
            int i = g_probeCount++;
            g_probeJoints[i] = j;
            g_probeRest[i][0] = j->translate_x; g_probeRest[i][1] = j->translate_y; g_probeRest[i][2] = j->translate_z;
            g_probeRest[i][3] = j->rotate_x;    g_probeRest[i][4] = j->rotate_y;    g_probeRest[i][5] = j->rotate_z;
            g_probeRest[i][6] = j->scale_x;     g_probeRest[i][7] = j->scale_y;     g_probeRest[i][8] = j->scale_z;
            if (j->aobj != NULL) g_probeAnimCount++;
        }
        if (j->child != NULL) PCPort_ProbeCollect(j->child);
    }
}

void PCPort_CharAnimProbe(const char* fsysPath, const char* memberName, int frames) {
    int f, i, moved = 0;
    f32 maxRot = 0.0f, maxTrans = 0.0f, maxScale = 0.0f;

    if (frames <= 0) frames = 30;
    if (!PCPort_TitleAnimSetup(fsysPath, memberName)) {
        printf("[charanim] setup FAILED for %s :: %s\n", fsysPath, memberName);
        return;
    }
    g_probeCount = 0; g_probeAnimCount = 0;
    PCPort_ProbeCollect(g_titleAnimRoot);
    printf("[charanim] %s :: %s -> %d joints, %d with aobj. Stepping %d frames...\n",
           fsysPath, memberName, g_probeCount, g_probeAnimCount, frames);

    /* Inspect the attached aobjs BEFORE stepping: real curves vs empty/static. */
    {
        int shown = 0;
        for (i = 0; i < g_probeCount && shown < 12; ++i) {
            HSD_AObj* a = g_probeJoints[i]->aobj;
            if (a == NULL) continue;
            ++shown;
            printf("[charanim]   aobj joint[%d]: end_frame=%.2f framerate=%.3f curr_frame=%.2f fobj=%p",
                   i, a->end_frame, a->framerate, a->curr_frame, (void*)a->fobj);
            if (a->fobj != NULL) {
                HSD_FObj* fo = a->fobj;
                printf("  fobj{op=%u obj_type=%u length=%u ad=%p flags=0x%02X}",
                       fo->op, fo->obj_type, fo->length, (void*)fo->ad_head, fo->flags);
            }
            printf("\n");
        }
    }

    for (f = 0; f < frames; ++f) {
        HSD_JObjAnimAll(g_titleAnimRoot);
    }

    /* Inspect aobjs AFTER stepping: did curr_frame advance? */
    {
        int shown = 0;
        for (i = 0; i < g_probeCount && shown < 4; ++i) {
            HSD_AObj* a = g_probeJoints[i]->aobj;
            if (a == NULL) continue;
            ++shown;
            printf("[charanim]   (after %d steps) aobj joint[%d]: curr_frame=%.2f\n",
                   frames, i, a->curr_frame);
        }
    }

    for (i = 0; i < g_probeCount; ++i) {
        HSD_JObj* j = g_probeJoints[i];
        f32 dt = fabsf(j->translate_x - g_probeRest[i][0]) + fabsf(j->translate_y - g_probeRest[i][1]) + fabsf(j->translate_z - g_probeRest[i][2]);
        f32 dr = fabsf(j->rotate_x - g_probeRest[i][3]) + fabsf(j->rotate_y - g_probeRest[i][4]) + fabsf(j->rotate_z - g_probeRest[i][5]);
        f32 ds = fabsf(j->scale_x - g_probeRest[i][6]) + fabsf(j->scale_y - g_probeRest[i][7]) + fabsf(j->scale_z - g_probeRest[i][8]);
        if (dt > 1.0e-4f || dr > 1.0e-4f || ds > 1.0e-4f) {
            ++moved;
            if (dr > maxRot)   maxRot = dr;
            if (dt > maxTrans) maxTrans = dt;
            if (ds > maxScale) maxScale = ds;
            if (moved <= 8) {
                printf("[charanim]   joint[%d] moved: dTrans=%.4f dRot=%.4f dScale=%.4f  (rest r=%.3f,%.3f,%.3f -> now %.3f,%.3f,%.3f)\n",
                       i, dt, dr, ds,
                       g_probeRest[i][3], g_probeRest[i][4], g_probeRest[i][5],
                       j->rotate_x, j->rotate_y, j->rotate_z);
            }
        }
    }
    printf("[charanim] RESULT: %d / %d joints MOVED over %d frames (maxDRot=%.4f maxDTrans=%.4f maxDScale=%.4f)\n",
           moved, g_probeCount, frames, maxRot, maxTrans, maxScale);
    if (moved == 0) {
        printf("[charanim] => NO embedded motion (static pose). Walk/idle motion is in a separate archive.\n");
    } else {
        printf("[charanim] => REAL embedded animation. The live tree can drive field skinning.\n");
    }
}

/* ========================================================================= */
/*  Field-character animation: drive the BE skinning from a live HSD tree.    */
/*                                                                           */
/*  The field character (Wes/ken_b1) renders through the proven BIG-ENDIAN    */
/*  archive skinning path (pcport_main.c BuildSkinPalette reads joint SRT     */
/*  straight from the archive bytes). To animate it WITHOUT touching that     */
/*  hot path, we keep a SECOND, swizzled copy of the same archive, build the  */
/*  game's real live HSD_JObj tree from it (HSD_JObjLoadJoint + AddAnimAll),  */
/*  advance the animation each frame (HSD_JObjAnimAll -> FObj interpreter),    */
/*  then WRITE the animated per-joint SRT back into the renderer's BE archive  */
/*  storage by walking both trees in lockstep (identical ken_b1 topology).    */
/*  The existing skinning then computes animated world matrices for free.     */
/*                                                                           */
/*  Rigid bones (the bulk of ken_b1: limbs/boots, single-influence weight 1)  */
/*  animate correctly (palette = animated jointWorld). Envelope/blend bones   */
/*  recompute invBind from the now-animated world so they hold the rest pose  */
/*  (no distortion); exact envelope animation = a later refinement.           */
/* ========================================================================= */
static PCPortHSDArchive g_charAnimArchive;
static HSD_JObj*        g_charAnimRoot = NULL;
static int              g_charAnimReady = 0;
static u8*              g_charAnimData = NULL;
static char             g_charAnimFsys[300];
static char             g_charAnimMember[80];
static int              g_charAnimMotionIdx = -1;  /* -1 = use env/default */
static f32              g_charAnimTime = 0.0f;
static f32              g_charAnimLoopLen = -1.0f;
static int              g_charAnimQuietSetup = 0;

int PCPort_CharAnimSetup(const char* fsysPath, const char* memberName) {
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOffset = 0, branchOff, jointListOff;
    void* rootPtr;
    HSD_Joint* rootJoint;
    void* animjoint;
    void* matanimjoint;

    /* Free any prior live tree/archive (motion switches re-run this). */
    if (g_charAnimData != NULL) { free(g_charAnimData); g_charAnimData = NULL; }
    if (g_charAnimArchive.storage != NULL) PCPort_HSDArchiveDestroy(&g_charAnimArchive);
    g_charAnimReady = 0;
    g_charAnimRoot = NULL;
    memset(&g_charAnimArchive, 0, sizeof(g_charAnimArchive));

    /* Remember source so PCPort_CharAnimSetMotion can rebuild with a new motion. */
    if (fsysPath != g_charAnimFsys)
        snprintf(g_charAnimFsys, sizeof(g_charAnimFsys), "%s", fsysPath);
    if (memberName != g_charAnimMember)
        snprintf(g_charAnimMember, sizeof(g_charAnimMember), "%s", memberName);

    if (!PCPort_LoadFsysMember(fsysPath, memberName, &data, &size)) {
        if (!g_charAnimQuietSetup) {
            printf("[char-anim] load %s:%s FAILED\n", fsysPath, memberName);
        }
        return 0;
    }
    if (!PCPort_HSDArchiveParseBE(&g_charAnimArchive, data, size)) {
        if (!g_charAnimQuietSetup) printf("[char-anim] parse FAILED\n");
        free(data);
        return 0;
    }
    g_charAnimData = data; /* keep alive: the live tree points into storage */

    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&g_charAnimArchive,
                                                             "scene_data", &sceneOffset);
    if (sceneData == NULL) {
        if (!g_charAnimQuietSetup) printf("[char-anim] no scene_data\n");
        PCPort_HSDArchiveDestroy(&g_charAnimArchive);
        free(data); g_charAnimData = NULL;
        return 0;
    }
    branchOff = ReadBE32(sceneData + 0x00);
    jointListOff = ReadBE32(g_charAnimArchive.storage + branchOff + 0x00);
    (void)rootPtr;

    /* Resource layout (reverse-engineered from the model loader fn_800E51A4 /
     * attach fn_800ECCA8): Resource = jointList here:
     *   +0x0 = JObjDesc* (the skeleton root)
     *   +0x4 = pointer to a NULL-terminated ARRAY of HSD_AnimJoint* -- the MOTION
     *          BANK, indexed by motion id (idle/walk/run/...). model+0x84 = count.
     *   +0x8 = second anim array (secondary motions), +0xC = material-anim array.
     * The earlier code wrongly treated +0x4 as a single AnimJoint (it's the array
     * pointer) -> attached garbage = a frozen pose. Select motion[PCPORT_MOTION_IDX]. */
    {
        PCPortSwizCtx ctx;
        u32 rootOff    = ReadBE32(g_charAnimArchive.storage + jointListOff + 0x0);
        u32 animArrOff = ReadBE32(g_charAnimArchive.storage + jointListOff + 0x4);
        u32 matanimOff = ReadBE32(g_charAnimArchive.storage + jointListOff + 0x8);
        /* Motion index: runtime-set (g_charAnimMotionIdx, via SetMotion) wins;
         * else PCPORT_MOTION_IDX env; else 0. */
        const char* mi = getenv("PCPORT_MOTION_IDX");
        u32 motionIdx = (g_charAnimMotionIdx >= 0) ? (u32)g_charAnimMotionIdx
                        : ((mi != NULL) ? (u32)atoi(mi) : 0u);
        u32 dataLo = g_charAnimArchive.dataOffset;
        u32 dataHi = g_charAnimArchive.dataOffset + g_charAnimArchive.dataSize;
        u32 realAnimOff = 0u, motionCount = 0u, k;

        /* Count motions (array entries that are valid data pointers). */
        if (animArrOff >= dataLo && animArrOff < dataHi) {
            for (k = 0; k < 64u; ++k) {
                u32 e = ReadBE32(g_charAnimArchive.storage + animArrOff + k * 4u);
                if (e < dataLo || e >= dataHi) break;
                ++motionCount;
            }
            if (motionIdx >= motionCount && motionCount > 0u) motionIdx = motionCount - 1u;
            realAnimOff = ReadBE32(g_charAnimArchive.storage + animArrOff + motionIdx * 4u);
        }

        memset(&ctx, 0, sizeof(ctx));
        ctx.base = g_charAnimArchive.storage;
        ctx.size = g_charAnimArchive.storageSize;
        ctx.dataOffset = g_charAnimArchive.dataOffset;
        SwizJoint(&ctx, rootOff);
        if (realAnimOff >= dataLo && realAnimOff < dataHi) SwizAnimJoint(&ctx, realAnimOff);
        if (matanimOff >= dataLo && matanimOff < dataHi) SwizMatAnimJoint(&ctx, matanimOff);
        PCPort_HSDApplyHostRelocations(&g_charAnimArchive);

        rootJoint    = (HSD_Joint*)(g_charAnimArchive.storage + rootOff);
        animjoint    = (realAnimOff >= dataLo && realAnimOff < dataHi)
                           ? (void*)(g_charAnimArchive.storage + realAnimOff) : NULL;
        matanimjoint = NULL;
        if (!g_charAnimQuietSetup) {
            printf("[char-anim] motion bank: %u motions; using idx %u (animOff=0x%X)\n",
                   motionCount, motionIdx, realAnimOff);
        }
    }

    g_charAnimRoot = HSD_JObjLoadJoint(rootJoint);
    if (g_charAnimRoot == NULL) {
        if (!g_charAnimQuietSetup) printf("[char-anim] HSD_JObjLoadJoint FAILED\n");
        PCPort_HSDArchiveDestroy(&g_charAnimArchive);
        free(data); g_charAnimData = NULL;
        return 0;
    }

    HSD_JObjAddAnimAll(g_charAnimRoot, (HSD_AnimJoint*)animjoint,
                       (HSD_MatAnimJoint*)matanimjoint, NULL);
    HSD_JObjReqAnimAll(g_charAnimRoot, 0.0f);
    PCPort_HSDStartAnimAll(g_charAnimRoot);

    g_charAnimTime = 0.0f;
    g_charAnimLoopLen = -1.0f;
    g_charAnimReady = 1;
    if (!g_charAnimQuietSetup) {
        printf("[char-anim] setup OK (%s :: %s, root=%p animjoint=%p)\n",
               fsysPath, memberName, (void*)g_charAnimRoot, animjoint);
    }
    return 1;
}

/* Switch the playing motion (e.g. idle <-> walk). Rebuilds the live tree with
 * motion[idx] from the remembered archive. Cheap (re-parses ~100KB ken_b1) and
 * only called on a movement-state transition, not per frame. No-op if already
 * on that motion. Returns 1 on success. */
int PCPort_CharAnimSetMotion(int motionIdx) {
    if (g_charAnimReady && motionIdx == g_charAnimMotionIdx) return 1;
    if (g_charAnimFsys[0] == '\0') return 0;   /* never set up */
    g_charAnimMotionIdx = motionIdx;
    return PCPort_CharAnimSetup(g_charAnimFsys, g_charAnimMember);
}

/* Lockstep walk: BE joint chain (offsets: child@+0x08, next@+0x0C) <-> live
 * JObj chain (child/next ptrs). Write each live joint's animated SRT into the
 * BE archive bytes (rot@+0x14, scale@+0x20, translate@+0x2C as BE floats).
 *
 * Field locomotion already moves the avatar's world transform. Do not also copy
 * root-motion translate from the clip by default, or walk/run clips double-apply
 * locomotion and look like sliding/body offsets instead of in-place stride. */
static void PCPort_CharAnimLockstepWrite(PCPortHSDArchive* be, u32 beJointOff,
                                         HSD_JObj* live, int isRoot,
                                         int applyRootTranslate) {
    u32 childOff, nextOff;
    if (live == NULL || beJointOff == 0u ||
        !IsArchiveRangeValid(be, beJointOff, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return;
    }
    WriteBEFloat(be->storage + beJointOff + 0x14, live->rotate_x);
    WriteBEFloat(be->storage + beJointOff + 0x18, live->rotate_y);
    WriteBEFloat(be->storage + beJointOff + 0x1C, live->rotate_z);
    WriteBEFloat(be->storage + beJointOff + 0x20, live->scale_x);
    WriteBEFloat(be->storage + beJointOff + 0x24, live->scale_y);
    WriteBEFloat(be->storage + beJointOff + 0x28, live->scale_z);
    if (!isRoot || applyRootTranslate) {
        WriteBEFloat(be->storage + beJointOff + 0x2C, live->translate_x);
        WriteBEFloat(be->storage + beJointOff + 0x30, live->translate_y);
        WriteBEFloat(be->storage + beJointOff + 0x34, live->translate_z);
    }

    childOff = ReadBE32(be->storage + beJointOff + 0x08);
    nextOff  = ReadBE32(be->storage + beJointOff + 0x0C);
    PCPort_CharAnimLockstepWrite(be, childOff, live->child, 0,
                                 applyRootTranslate);
    PCPort_CharAnimLockstepWrite(be, nextOff, live->next, 0,
                                 applyRootTranslate);
}

/* Find the largest end_frame across the live tree's aobjs (the loop length). */
static f32 PCPort_CharAnimMaxEndFrame(HSD_JObj* root) {
    f32 maxEnd = 0.0f;
    HSD_JObj* st[512]; int sp = 0; st[sp++] = root;
    while (sp > 0) { HSD_JObj* lv = st[--sp]; if (!lv) continue;
        if (lv->aobj != NULL && lv->aobj->end_frame > maxEnd) maxEnd = lv->aobj->end_frame;
        if (sp < 510) { if (lv->child) st[sp++] = lv->child; if (lv->next) st[sp++] = lv->next; } }
    return maxEnd;
}

static u32 PCPort_CharAnimCountMotionBank(const char* fsysPath,
                                          const char* memberName) {
    PCPortHSDArchive archive;
    u8* data = NULL;
    u32 size = 0;
    const u8* sceneData;
    u32 sceneOffset = 0, branchOff, jointListOff, animArrOff;
    u32 dataLo, dataHi, count = 0u, k;

    memset(&archive, 0, sizeof(archive));
    if (!g_charAnimQuietSetup) {
        printf("[charbank] count: load %s :: %s\n", fsysPath, memberName);
        fflush(stdout);
    }
    if (!PCPort_LoadFsysMember(fsysPath, memberName, &data, &size)) {
        if (!g_charAnimQuietSetup) {
            printf("[charbank] count: load FAILED\n");
            fflush(stdout);
        }
        return 0u;
    }
    if (!g_charAnimQuietSetup) {
        printf("[charbank] count: loaded %u bytes, parse\n", size);
        fflush(stdout);
    }
    if (!PCPort_HSDArchiveParseBE(&archive, data, size)) {
        if (!g_charAnimQuietSetup) {
            printf("[charbank] count: parse FAILED\n");
            fflush(stdout);
        }
        if (data != NULL) free(data);
        return 0u;
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL) {
        PCPort_HSDArchiveDestroy(&archive);
        free(data);
        return 0u;
    }

    branchOff = ReadBE32(sceneData + 0x00);
    jointListOff = ReadBE32(archive.storage + branchOff + 0x00);
    animArrOff = ReadBE32(archive.storage + jointListOff + 0x4);
    dataLo = archive.dataOffset;
    dataHi = archive.dataOffset + archive.dataSize;

    if (animArrOff >= dataLo && animArrOff < dataHi) {
        for (k = 0; k < 64u; ++k) {
            u32 e = ReadBE32(archive.storage + animArrOff + k * 4u);
            if (e < dataLo || e >= dataHi) break;
            ++count;
        }
    }

    PCPort_HSDArchiveDestroy(&archive);
    free(data);
    return count;
}

static f32 PCPort_ProbeSRTChecksum(void) {
    f32 sum = 0.0f;
    int i;
    for (i = 0; i < g_probeCount; ++i) {
        HSD_JObj* j = g_probeJoints[i];
        if (j == NULL) continue;
        sum += fabsf(j->translate_x) + fabsf(j->translate_y) + fabsf(j->translate_z);
        sum += fabsf(j->rotate_x) + fabsf(j->rotate_y) + fabsf(j->rotate_z);
        sum += fabsf(j->scale_x) + fabsf(j->scale_y) + fabsf(j->scale_z);
    }
    return sum;
}

static void PCPort_ProbeSnapshot(f32 outSrt[PCPORT_PROBE_MAX_JOINTS][9]) {
    int i;
    for (i = 0; i < g_probeCount; ++i) {
        HSD_JObj* j = g_probeJoints[i];
        if (j == NULL) {
            memset(outSrt[i], 0, sizeof(outSrt[i]));
            continue;
        }
        outSrt[i][0] = j->translate_x; outSrt[i][1] = j->translate_y; outSrt[i][2] = j->translate_z;
        outSrt[i][3] = j->rotate_x;    outSrt[i][4] = j->rotate_y;    outSrt[i][5] = j->rotate_z;
        outSrt[i][6] = j->scale_x;     outSrt[i][7] = j->scale_y;     outSrt[i][8] = j->scale_z;
    }
}

static f32 PCPort_ProbeSnapshotDelta(const f32 a[PCPORT_PROBE_MAX_JOINTS][9],
                                     const f32 b[PCPORT_PROBE_MAX_JOINTS][9],
                                     int ignoreRootTranslate,
                                     int* outMoved,
                                     f32* outMaxRot,
                                     f32* outMaxTrans,
                                     f32* outRootTrans) {
    int i;
    f32 sum = 0.0f;
    int moved = 0;
    f32 maxRot = 0.0f;
    f32 maxTrans = 0.0f;
    f32 rootTrans = 0.0f;

    for (i = 0; i < g_probeCount; ++i) {
        f32 dt = fabsf(b[i][0] - a[i][0]) +
                 fabsf(b[i][1] - a[i][1]) +
                 fabsf(b[i][2] - a[i][2]);
        f32 dr = fabsf(b[i][3] - a[i][3]) +
                 fabsf(b[i][4] - a[i][4]) +
                 fabsf(b[i][5] - a[i][5]);
        f32 ds = fabsf(b[i][6] - a[i][6]) +
                 fabsf(b[i][7] - a[i][7]) +
                 fabsf(b[i][8] - a[i][8]);
        if (i == 0) {
            rootTrans = dt;
            if (ignoreRootTranslate) {
                dt = 0.0f;
            }
        }
        if (dt > 1.0e-4f || dr > 1.0e-4f || ds > 1.0e-4f) {
            ++moved;
        }
        if (dt > maxTrans) maxTrans = dt;
        if (dr > maxRot) maxRot = dr;
        sum += dt + dr + ds;
    }

    if (outMoved != NULL) *outMoved = moved;
    if (outMaxRot != NULL) *outMaxRot = maxRot;
    if (outMaxTrans != NULL) *outMaxTrans = maxTrans;
    if (outRootTrans != NULL) *outRootTrans = rootTrans;
    return sum;
}

typedef struct PCPortMotionProbeStats {
    u32 motionIdx;
    int valid;
    int jointCount;
    int aobjCount;
    int probeFrames;
    int movedFromRest;
    int varyingFrames;
    int loopMoved;
    f32 endFrame;
    f32 checksumFirst;
    f32 checksumLast;
    f32 checksumRange;
    f32 frameDeltaSum;
    f32 frameDeltaMax;
    f32 maxRestRot;
    f32 maxRestTrans;
    f32 loopDelta;
    f32 loopMaxRot;
    f32 loopMaxTrans;
    f32 rootLoopTrans;
    const char* kind;
} PCPortMotionProbeStats;

typedef PCPortHostMotionBank PCPortHeadlessMotionBank;

static BOOL PCPort_IsDataOffset(const PCPortHSDArchive* archive, u32 off,
                                u32 size) {
    u32 dataLo;
    u32 dataHi;
    if (archive == NULL) {
        return FALSE;
    }
    dataLo = archive->dataOffset;
    dataHi = archive->dataOffset + archive->dataSize;
    return off >= dataLo && off < dataHi && size <= archive->storageSize - off;
}

static BOOL PCPort_ResourceHasMotionBank(const PCPortHSDArchive* archive,
                                         u32 resourceOff) {
    u32 rootOff;
    u32 animArrOff;
    if (!PCPort_IsDataOffset(archive, resourceOff, 8u)) {
        return FALSE;
    }
    rootOff = ReadBE32(archive->storage + resourceOff + 0x0);
    animArrOff = ReadBE32(archive->storage + resourceOff + 0x4);
    return PCPort_IsDataOffset(archive, rootOff, PCPORT_SERIALIZED_JOINT_SIZE) &&
           PCPort_IsDataOffset(archive, animArrOff, 4u);
}

static const char* PCPort_PathBaseName(const char* path);

static u32 PCPort_CountMotionBankEntries(const PCPortHSDArchive* archive,
                                         u32 animArrOff,
                                         u32 maxCount) {
    u32 k;
    u32 count = 0u;

    if (archive == NULL || maxCount == 0u) {
        return 0u;
    }
    for (k = 0u; k < maxCount; ++k) {
        u32 e;
        if (!PCPort_IsDataOffset(archive, animArrOff + k * 4u, 4u)) {
            break;
        }
        e = ReadBE32(archive->storage + animArrOff + k * 4u);
        if (e == 0u || !PCPort_IsDataOffset(archive, e, 4u)) {
            break;
        }
        ++count;
    }
    return count;
}

static u32 PCPort_ResourceMotionBankCount(const PCPortHSDArchive* archive,
                                          u32 resourceOff) {
    if (!PCPort_ResourceHasMotionBank(archive, resourceOff)) {
        return 0u;
    }
    return PCPort_CountMotionBankEntries(
        archive, ReadBE32(archive->storage + resourceOff + 0x4), 64u);
}

static void PCPort_ConsiderSceneMotionResource(
    const PCPortHSDArchive* archive,
    u32 resourceOff,
    u32* bestResourceOff,
    u32* bestCount) {
    u32 count = PCPort_ResourceMotionBankCount(archive, resourceOff);
    if (count > *bestCount) {
        *bestResourceOff = resourceOff;
        *bestCount = count;
    }
}

static BOOL PCPort_FindHSDArchivePayload(const u8* data, u32 size,
                                         const u8** outData,
                                         u32* outSize,
                                         u32* outWrapperOffset) {
    u32 off;
    const u8* fallbackData = NULL;
    u32 fallbackSize = 0u;
    u32 fallbackWrapperOffset = 0u;

    if (data == NULL || outData == NULL || outSize == NULL ||
        outWrapperOffset == NULL || size < 0x20u) {
        return FALSE;
    }

    for (off = 0u; off < size && off <= 0x400u; off += 4u) {
        u32 fileSize;
        u32 dataSize;
        u32 relocCount;
        u32 publicCount;
        u32 externCount;
        u32 relocOffset;
        u32 publicOffset;
        u32 externOffset;
        u32 stringOffset;

        if (off + 0x20u > size) {
            break;
        }

        fileSize = ReadBE32(data + off + 0x00);
        dataSize = ReadBE32(data + off + 0x04);
        relocCount = ReadBE32(data + off + 0x08);
        publicCount = ReadBE32(data + off + 0x0C);
        externCount = ReadBE32(data + off + 0x10);
        if (fileSize < 0x20u || fileSize > size - off ||
            dataSize >= fileSize || relocCount > 20000u ||
            publicCount > 1024u || externCount > 1024u) {
            continue;
        }

        relocOffset = 0x20u + dataSize;
        publicOffset = relocOffset + relocCount * 4u;
        externOffset = publicOffset + publicCount * 8u;
        stringOffset = externOffset + externCount * 8u;
        if (relocOffset <= fileSize &&
            publicOffset <= fileSize &&
            externOffset <= fileSize &&
            stringOffset <= fileSize) {
            if (HSDArchiveHasPublic(data + off, fileSize, "scene_data")) {
                *outData = data + off;
                *outSize = fileSize;
                *outWrapperOffset = off;
                return TRUE;
            }
            if (fallbackData == NULL) {
                fallbackData = data + off;
                fallbackSize = fileSize;
                fallbackWrapperOffset = off;
            }
        }
    }

    if (fallbackData != NULL) {
        *outData = fallbackData;
        *outSize = fallbackSize;
        *outWrapperOffset = fallbackWrapperOffset;
        return TRUE;
    }
    return FALSE;
}

static BOOL PCPort_FindSceneMotionResource(const PCPortHSDArchive* archive,
                                           u32 sceneOffset,
                                           u32* outResourceOff) {
    const u8* sceneData;
    u32 bestResourceOff = 0u;
    u32 bestCount = 0u;
    u32 rel;

    if (archive == NULL || outResourceOff == NULL ||
        !PCPort_IsDataOffset(archive, sceneOffset, 4u)) {
        return FALSE;
    }

    sceneData = archive->storage + sceneOffset;
    PCPort_ConsiderSceneMotionResource(archive, sceneOffset, &bestResourceOff,
                                       &bestCount);

    /* Field character archives expose the motion Resource directly through
     * scene_data. Battle pkx archives keep a short scene_data Resource list;
     * one list entry points to a wrapper whose first word is the real model
     * Resource. Scan only that compact scene_data tail so material/shape banks
     * elsewhere in the archive cannot win by accident. */
    for (rel = 0u; rel < 0x40u; rel += 4u) {
        u32 candidate;
        u32 nested;
        if (!PCPort_IsDataOffset(archive, sceneOffset + rel, 4u)) {
            break;
        }
        candidate = ReadBE32(sceneData + rel);
        if (!PCPort_IsDataOffset(archive, candidate, 4u)) {
            continue;
        }
        PCPort_ConsiderSceneMotionResource(archive, candidate,
                                           &bestResourceOff, &bestCount);
        nested = ReadBE32(archive->storage + candidate);
        PCPort_ConsiderSceneMotionResource(archive, nested,
                                           &bestResourceOff, &bestCount);
    }

    if (bestCount == 0u) {
        return FALSE;
    }
    *outResourceOff = bestResourceOff;
    return TRUE;
}

static BOOL PCPort_IsPkxArchivePath(const char* fsysPath) {
    return strncmp(PCPort_PathBaseName(fsysPath), "pkx_", 4) == 0;
}

static BOOL PCPort_IsKnownNonAnimatedCharacterArchive(const char* fsysPath) {
    const char* base = PCPort_PathBaseName(fsysPath);
    return strcmp(base, "chara_big.fsys") == 0 ||
           strcmp(base, "chara_small.fsys") == 0;
}

static BOOL PCPort_IsPlausibleFsysEntry(const u8* fsysData, u32 fsysSize,
                                        u32 entryOffset) {
    u32 dataOffset;
    u32 nameOffset;

    if (fsysData == NULL || entryOffset == 0u ||
        entryOffset + 0x28u > fsysSize) {
        return FALSE;
    }
    dataOffset = ReadBE32(fsysData + entryOffset + 0x04);
    nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
    if (dataOffset >= fsysSize || nameOffset >= fsysSize) {
        return FALSE;
    }
    return TRUE;
}

static u32 PCPort_HeadlessMotionPreflightMemberData(const u8* memberData,
                                                    u32 memberSize,
                                                    u32* outResourceOff) {
    const u8* hsdData;
    u32 hsdSize;
    u32 wrapperOffset;
    PCPortHSDArchive archive;
    const u8* sceneData;
    u32 sceneOffset = 0u;
    u32 resourceOff = 0u;
    u32 count = 0u;

    if (outResourceOff != NULL) {
        *outResourceOff = 0u;
    }
    if (!PCPort_FindHSDArchivePayload(memberData, memberSize, &hsdData,
                                      &hsdSize, &wrapperOffset)) {
        return 0u;
    }
    if (!PCPort_HSDArchiveParseBE(&archive, hsdData, hsdSize)) {
        return 0u;
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(
        &archive, "scene_data", &sceneOffset);
    if (sceneData != NULL &&
        PCPort_FindSceneMotionResource(&archive, sceneOffset, &resourceOff)) {
        count = PCPort_ResourceMotionBankCount(&archive, resourceOff);
        if (outResourceOff != NULL) {
            *outResourceOff = resourceOff;
        }
    }
    PCPort_HSDArchiveDestroy(&archive);
    return count;
}

static void PCPort_HeadlessMotionBankRelease(PCPortHeadlessMotionBank* bank) {
    if (bank == NULL) {
        return;
    }
    if (bank->archive.storage != NULL) {
        PCPort_HSDArchiveDestroy(&bank->archive);
    }
    if (bank->memberData != NULL) {
        free(bank->memberData);
    }
    memset(bank, 0, sizeof(*bank));
}

static BOOL PCPort_HeadlessMotionBankLoadData(const char* fsysPath,
                                              const char* memberName,
                                              u8* memberData,
                                              u32 size,
                                              PCPortHeadlessMotionBank* bank,
                                              int verbose);

static BOOL PCPort_HeadlessMotionBankLoad(const char* fsysPath,
                                          const char* memberName,
                                          PCPortHeadlessMotionBank* bank,
                                          int verbose) {
    u32 size = 0u;

    if (bank == NULL) {
        return FALSE;
    }
    memset(bank, 0, sizeof(*bank));

    if (verbose) {
        printf("[headless-motion] loading member: %s :: %s\n",
               fsysPath, memberName);
        fflush(stdout);
    }
    if (!PCPort_LoadFsysMember(fsysPath, memberName, &bank->memberData, &size)) {
        if (verbose) {
            printf("[headless-motion] load FAILED: %s :: %s\n",
                   fsysPath, memberName);
            fflush(stdout);
        }
        return FALSE;
    }
    return PCPort_HeadlessMotionBankLoadData(fsysPath, memberName,
                                             bank->memberData, size, bank,
                                             verbose);
}

static BOOL PCPort_HeadlessMotionBankLoadData(const char* fsysPath,
                                              const char* memberName,
                                              u8* memberData,
                                              u32 size,
                                              PCPortHeadlessMotionBank* bank,
                                              int verbose) {
    const u8* sceneData;
    const u8* hsdData;
    u32 sceneOffset = 0u;
    u32 resourceOff = 0u;
    u32 animArrOff;
    u32 rootOff;
    u32 hsdSize;
    u32 wrapperOffset;
    u32 k;
    PCPortSwizCtx ctx;

    if (bank == NULL || memberData == NULL) {
        return FALSE;
    }
    memset(bank, 0, sizeof(*bank));
    bank->memberData = memberData;

    if (verbose) {
        printf("[headless-motion] member loaded size=0x%X; parsing HSD\n",
               size);
        fflush(stdout);
    }
    if (!PCPort_FindHSDArchivePayload(bank->memberData, size, &hsdData,
                                      &hsdSize, &wrapperOffset)) {
        if (verbose) {
            printf("[headless-motion] no HSD payload: %s :: %s size=0x%X\n",
                   fsysPath, memberName, size);
            fflush(stdout);
        }
        PCPort_HeadlessMotionBankRelease(bank);
        return FALSE;
    }
    if (verbose && wrapperOffset != 0u) {
        printf("[headless-motion] HSD payload wrapper=0x%X size=0x%X\n",
               wrapperOffset, hsdSize);
        fflush(stdout);
    }
    if (!PCPort_HSDArchiveParseBE(&bank->archive, hsdData, hsdSize)) {
        if (verbose) {
            printf("[headless-motion] HSD parse FAILED: %s :: %s size=0x%X\n",
                   fsysPath, memberName, hsdSize);
            fflush(stdout);
        }
        PCPort_HeadlessMotionBankRelease(bank);
        return FALSE;
    }

    if (verbose) {
        printf("[headless-motion] HSD parsed; resolving scene_data\n");
        fflush(stdout);
    }
    sceneData = (const u8*)PCPort_HSDArchiveGetPublicAddress(&bank->archive,
                                                             "scene_data",
                                                             &sceneOffset);
    if (sceneData == NULL ||
        !PCPort_FindSceneMotionResource(&bank->archive, sceneOffset,
                                        &resourceOff)) {
        if (verbose) {
            printf("[headless-motion] no scene_data Resource+0x4 bank: "
                   "%s :: %s\n", fsysPath, memberName);
            fflush(stdout);
        }
        PCPort_HeadlessMotionBankRelease(bank);
        return FALSE;
    }

    rootOff = ReadBE32(bank->archive.storage + resourceOff + 0x0);
    animArrOff = ReadBE32(bank->archive.storage + resourceOff + 0x4);
    bank->rootOff = rootOff;
    bank->animArrOff = animArrOff;

    for (k = 0u; k < 64u; ++k) {
        u32 e;
        if (!PCPort_IsDataOffset(&bank->archive, animArrOff + k * 4u, 4u)) {
            break;
        }
        e = ReadBE32(bank->archive.storage + animArrOff + k * 4u);
        if (!PCPort_IsDataOffset(&bank->archive, e, 4u)) {
            break;
        }
        bank->motionOffs[k] = e;
        ++bank->motionCount;
    }

    if (bank->motionCount == 0u) {
        if (verbose) {
            printf("[headless-motion] Resource+0x4 bank was empty: "
                   "%s :: %s resource=0x%X animArr=0x%X\n",
                   fsysPath, memberName, resourceOff, animArrOff);
            fflush(stdout);
        }
        PCPort_HeadlessMotionBankRelease(bank);
        return FALSE;
    }

    if (verbose) {
        printf("[headless-motion] bank counted; swizzling skeleton+%u motions\n",
               bank->motionCount);
        fflush(stdout);
    }
    memset(&ctx, 0, sizeof(ctx));
    ctx.base = bank->archive.storage;
    ctx.size = bank->archive.storageSize;
    ctx.dataOffset = bank->archive.dataOffset;
    SwizJoint(&ctx, bank->rootOff);
    for (k = 0u; k < bank->motionCount; ++k) {
        SwizAnimJoint(&ctx, bank->motionOffs[k]);
    }
    PCPort_HSDApplyHostRelocations(&bank->archive);

    if (verbose) {
        printf("[headless-motion] loaded %s :: %s resource=0x%X "
               "root=0x%X bank=0x%X motions=%u\n",
               fsysPath, memberName, resourceOff, bank->rootOff,
               bank->animArrOff, bank->motionCount);
        fflush(stdout);
    }
    return TRUE;
}

static HSD_JObj* PCPort_HeadlessLoadJointSkeleton(HSD_Joint* joint) {
    HSD_JObj* jobj;
    HSD_JObj* child;

    if (joint == NULL) {
        return NULL;
    }

    jobj = HSD_JObjAlloc();
    if (jobj == NULL) {
        return NULL;
    }

    jobj->flags = joint->flags;
    jobj->rotate_x = joint->rotation_x;
    jobj->rotate_y = joint->rotation_y;
    jobj->rotate_z = joint->rotation_z;
    jobj->scale_x = joint->scale_x;
    jobj->scale_y = joint->scale_y;
    jobj->scale_z = joint->scale_z;
    jobj->translate_x = joint->position_x;
    jobj->translate_y = joint->position_y;
    jobj->translate_z = joint->position_z;

    jobj->child = PCPort_HeadlessLoadJointSkeleton(joint->child);
    for (child = jobj->child; child != NULL; child = child->next) {
        child->parent = jobj;
    }
    jobj->next = PCPort_HeadlessLoadJointSkeleton(joint->next);

    return jobj;
}

BOOL PCPort_HostMotionBankLoad(const char* fsysPath,
                               const char* memberName,
                               PCPortHostMotionBank* bank,
                               int verbose) {
    return PCPort_HeadlessMotionBankLoad(fsysPath, memberName, bank, verbose);
}

void PCPort_HostMotionBankRelease(PCPortHostMotionBank* bank) {
    PCPort_HeadlessMotionBankRelease(bank);
}

HSD_JObj* PCPort_HostMotionCreateRoot(PCPortHostMotionBank* bank,
                                      u32 motionIdx,
                                      f32* outEndFrame) {
    HSD_Joint* rootJoint;
    HSD_AnimJoint* animjoint;
    HSD_JObj* root;

    if (outEndFrame != NULL) {
        *outEndFrame = 0.0f;
    }
    if (bank == NULL || bank->archive.storage == NULL ||
        motionIdx >= bank->motionCount ||
        !PCPort_IsDataOffset(&bank->archive, bank->rootOff, 4u) ||
        !PCPort_IsDataOffset(&bank->archive, bank->motionOffs[motionIdx], 4u)) {
        return NULL;
    }

    rootJoint = (HSD_Joint*)(bank->archive.storage + bank->rootOff);
    animjoint = (HSD_AnimJoint*)(bank->archive.storage +
                                 bank->motionOffs[motionIdx]);
    root = PCPort_HeadlessLoadJointSkeleton(rootJoint);
    if (root == NULL) {
        return NULL;
    }

    HSD_JObjAddAnimAll(root, animjoint, NULL, NULL);
    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);
    if (outEndFrame != NULL) {
        *outEndFrame = PCPort_CharAnimMaxEndFrame(root);
    }
    return root;
}

void PCPort_HostMotionRestart(HSD_JObj* root) {
    if (root == NULL) {
        return;
    }
    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);
}

static f32 PCPort_HostMotionSRTChecksumRec(HSD_JObj* root) {
    f32 sum = 0.0f;
    HSD_JObj* j;
    for (j = root; j != NULL; j = j->next) {
        sum += fabsf(j->translate_x) + fabsf(j->translate_y) +
               fabsf(j->translate_z);
        sum += fabsf(j->rotate_x) + fabsf(j->rotate_y) +
               fabsf(j->rotate_z);
        sum += fabsf(j->scale_x) + fabsf(j->scale_y) +
               fabsf(j->scale_z);
        if (j->child != NULL) {
            sum += PCPort_HostMotionSRTChecksumRec(j->child);
        }
    }
    return sum;
}

f32 PCPort_HostMotionSRTChecksum(HSD_JObj* root) {
    return PCPort_HostMotionSRTChecksumRec(root);
}

void PCPort_HostMotionStepAndApply(HSD_JObj* root,
                                   PCPortHSDArchive* beArchive,
                                   u32 beRootJoint,
                                   f32* timeInOut,
                                   f32 loopLen,
                                   BOOL applyRootTranslate) {
    if (root == NULL || beArchive == NULL) {
        return;
    }
    if (loopLen > 0.0f && timeInOut != NULL) {
        *timeInOut += 1.0f;
        if (*timeInOut >= loopLen) {
            *timeInOut = 0.0f;
            PCPort_HostMotionRestart(root);
        }
    }
    HSD_JObjAnimAll(root);
    PCPort_CharAnimLockstepWrite(beArchive, beRootJoint, root, 1,
                                 applyRootTranslate);
}

static void PCPort_HeadlessMotionProbeCollectStats(
    const PCPortHeadlessMotionBank* bank,
    u32 motionIdx,
    int frames,
    PCPortMotionProbeStats* outStats) {
    HSD_Joint* rootJoint;
    HSD_AnimJoint* animjoint;
    HSD_JObj* root;
    int f;
    int i;
    f32 prevChecksum;
    f32 minChecksum;
    f32 maxChecksum;

    memset(outStats, 0, sizeof(*outStats));
    outStats->motionIdx = motionIdx;
    outStats->kind = "setup-failed";
    if (frames <= 1) frames = 2;

    if (bank == NULL || motionIdx >= bank->motionCount ||
        bank->archive.storage == NULL) {
        return;
    }

    rootJoint = (HSD_Joint*)(bank->archive.storage + bank->rootOff);
    animjoint = (HSD_AnimJoint*)(bank->archive.storage +
                                 bank->motionOffs[motionIdx]);
    root = PCPort_HeadlessLoadJointSkeleton(rootJoint);
    if (root == NULL) {
        return;
    }

    HSD_JObjAddAnimAll(root, animjoint, NULL, NULL);
    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);

    g_probeCount = 0;
    g_probeAnimCount = 0;
    PCPort_ProbeCollect(root);
    outStats->valid = 1;
    outStats->jointCount = g_probeCount;
    outStats->aobjCount = g_probeAnimCount;
    outStats->endFrame = PCPort_CharAnimMaxEndFrame(root);

    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);
    HSD_JObjAnimAll(root);
    prevChecksum = PCPort_ProbeSRTChecksum();
    minChecksum = prevChecksum;
    maxChecksum = prevChecksum;
    outStats->checksumFirst = prevChecksum;
    outStats->checksumLast = prevChecksum;
    outStats->probeFrames = 1;

    for (f = 1; f < frames; ++f) {
        f32 now;
        f32 delta;
        HSD_JObjAnimAll(root);
        now = PCPort_ProbeSRTChecksum();
        delta = fabsf(now - prevChecksum);
        if (delta > 1.0e-4f) {
            ++outStats->varyingFrames;
        }
        outStats->frameDeltaSum += delta;
        if (delta > outStats->frameDeltaMax) {
            outStats->frameDeltaMax = delta;
        }
        if (now < minChecksum) minChecksum = now;
        if (now > maxChecksum) maxChecksum = now;
        prevChecksum = now;
        outStats->checksumLast = now;
        ++outStats->probeFrames;
    }
    outStats->checksumRange = maxChecksum - minChecksum;

    for (i = 0; i < g_probeCount; ++i) {
        HSD_JObj* j = g_probeJoints[i];
        f32 dt;
        f32 dr;
        f32 ds;
        if (j == NULL) continue;
        dt = fabsf(j->translate_x - g_probeRest[i][0]) +
             fabsf(j->translate_y - g_probeRest[i][1]) +
             fabsf(j->translate_z - g_probeRest[i][2]);
        dr = fabsf(j->rotate_x - g_probeRest[i][3]) +
             fabsf(j->rotate_y - g_probeRest[i][4]) +
             fabsf(j->rotate_z - g_probeRest[i][5]);
        ds = fabsf(j->scale_x - g_probeRest[i][6]) +
             fabsf(j->scale_y - g_probeRest[i][7]) +
             fabsf(j->scale_z - g_probeRest[i][8]);
        if (dt > 1.0e-4f || dr > 1.0e-4f || ds > 1.0e-4f) {
            ++outStats->movedFromRest;
            if (dr > outStats->maxRestRot) outStats->maxRestRot = dr;
            if (dt > outStats->maxRestTrans) outStats->maxRestTrans = dt;
        }
    }

    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);
    HSD_JObjAnimAll(root);
    PCPort_ProbeSnapshot(g_probeLoopStart);
    if (outStats->endFrame >= 1.0f) {
        int loopFrames = (int)(outStats->endFrame + 0.5f);
        if (loopFrames < 1) loopFrames = 1;
        for (f = 0; f < loopFrames; ++f) {
            HSD_JObjAnimAll(root);
        }
    }
    PCPort_ProbeSnapshot(g_probeLoopEnd);
    outStats->loopDelta = PCPort_ProbeSnapshotDelta(
        g_probeLoopStart, g_probeLoopEnd, 1, &outStats->loopMoved,
        &outStats->loopMaxRot, &outStats->loopMaxTrans,
        &outStats->rootLoopTrans);

    if (outStats->varyingFrames == 0 &&
        outStats->movedFromRest <= 1 &&
        outStats->maxRestRot < 0.01f &&
        outStats->maxRestTrans < 0.01f) {
        outStats->kind = "static/bind";
    } else if (outStats->varyingFrames > 0 &&
               outStats->loopDelta < 5.0f) {
        outStats->kind = "cyclic";
    } else {
        outStats->kind = "one-shot";
    }

    HSD_JObjRemoveAll(root);
}

static BOOL PCPort_HeadlessMotionCollectMemberStats(
    const char* fsysPath,
    const char* memberName,
    int frames,
    PCPortMotionProbeStats stats[64],
    u32* outMotionCount,
    int verbose) {
    PCPortHeadlessMotionBank bank;
    u32 motionIdx;

    if (stats == NULL || outMotionCount == NULL) {
        return FALSE;
    }
    memset(stats, 0, sizeof(PCPortMotionProbeStats) * 64u);
    *outMotionCount = 0u;
    if (frames <= 1) frames = 40;

    if (!PCPort_HeadlessMotionBankLoad(fsysPath, memberName, &bank, verbose)) {
        return FALSE;
    }
    *outMotionCount = bank.motionCount;
    for (motionIdx = 0u; motionIdx < bank.motionCount; ++motionIdx) {
        PCPort_HeadlessMotionProbeCollectStats(&bank, motionIdx, frames,
                                               &stats[motionIdx]);
    }
    PCPort_HeadlessMotionBankRelease(&bank);
    return TRUE;
}

static BOOL PCPort_HeadlessMotionCollectMemberDataStats(
    const char* fsysPath,
    const char* memberName,
    u8* memberData,
    u32 memberSize,
    int frames,
    PCPortMotionProbeStats stats[64],
    u32* outMotionCount,
    int verbose) {
    PCPortHeadlessMotionBank bank;
    u32 motionIdx;

    if (stats == NULL || outMotionCount == NULL) {
        if (memberData != NULL) {
            free(memberData);
        }
        return FALSE;
    }
    memset(stats, 0, sizeof(PCPortMotionProbeStats) * 64u);
    *outMotionCount = 0u;
    if (frames <= 1) frames = 40;

    if (!PCPort_HeadlessMotionBankLoadData(fsysPath, memberName, memberData,
                                           memberSize, &bank, verbose)) {
        return FALSE;
    }
    *outMotionCount = bank.motionCount;
    for (motionIdx = 0u; motionIdx < bank.motionCount; ++motionIdx) {
        PCPort_HeadlessMotionProbeCollectStats(&bank, motionIdx, frames,
                                               &stats[motionIdx]);
    }
    PCPort_HeadlessMotionBankRelease(&bank);
    return TRUE;
}

static void PCPort_MotionProbeCollectStats(const char* fsysPath,
                                           const char* memberName,
                                           u32 motionIdx,
                                           int frames,
                                           PCPortMotionProbeStats* outStats) {
    int f, i;
    f32 prevChecksum, minChecksum, maxChecksum;

    memset(outStats, 0, sizeof(*outStats));
    outStats->motionIdx = motionIdx;
    outStats->kind = "setup-failed";
    if (frames <= 1) frames = 2;

    g_charAnimMotionIdx = (int)motionIdx;
    if (!PCPort_CharAnimSetup(fsysPath, memberName)) {
        return;
    }

    g_probeCount = 0;
    g_probeAnimCount = 0;
    PCPort_ProbeCollect(g_charAnimRoot);
    outStats->valid = 1;
    outStats->jointCount = g_probeCount;
    outStats->aobjCount = g_probeAnimCount;
    outStats->endFrame = PCPort_CharAnimMaxEndFrame(g_charAnimRoot);

    HSD_JObjReqAnimAll(g_charAnimRoot, 0.0f);
    PCPort_HSDStartAnimAll(g_charAnimRoot);
    PCPort_HSDJObjAnimJointOnlyAll(g_charAnimRoot);
    prevChecksum = PCPort_ProbeSRTChecksum();
    minChecksum = prevChecksum;
    maxChecksum = prevChecksum;
    outStats->checksumFirst = prevChecksum;
    outStats->checksumLast = prevChecksum;
    outStats->probeFrames = 1;

    for (f = 1; f < frames; ++f) {
        f32 now, delta;
        PCPort_HSDJObjAnimJointOnlyAll(g_charAnimRoot);
        now = PCPort_ProbeSRTChecksum();
        delta = fabsf(now - prevChecksum);
        if (delta > 1.0e-4f) {
            ++outStats->varyingFrames;
        }
        outStats->frameDeltaSum += delta;
        if (delta > outStats->frameDeltaMax) {
            outStats->frameDeltaMax = delta;
        }
        if (now < minChecksum) minChecksum = now;
        if (now > maxChecksum) maxChecksum = now;
        prevChecksum = now;
        outStats->checksumLast = now;
        ++outStats->probeFrames;
    }
    outStats->checksumRange = maxChecksum - minChecksum;

    for (i = 0; i < g_probeCount; ++i) {
        HSD_JObj* j = g_probeJoints[i];
        f32 dt, dr, ds;
        if (j == NULL) continue;
        dt = fabsf(j->translate_x - g_probeRest[i][0]) +
             fabsf(j->translate_y - g_probeRest[i][1]) +
             fabsf(j->translate_z - g_probeRest[i][2]);
        dr = fabsf(j->rotate_x - g_probeRest[i][3]) +
             fabsf(j->rotate_y - g_probeRest[i][4]) +
             fabsf(j->rotate_z - g_probeRest[i][5]);
        ds = fabsf(j->scale_x - g_probeRest[i][6]) +
             fabsf(j->scale_y - g_probeRest[i][7]) +
             fabsf(j->scale_z - g_probeRest[i][8]);
        if (dt > 1.0e-4f || dr > 1.0e-4f || ds > 1.0e-4f) {
            ++outStats->movedFromRest;
            if (dr > outStats->maxRestRot) outStats->maxRestRot = dr;
            if (dt > outStats->maxRestTrans) outStats->maxRestTrans = dt;
        }
    }

    HSD_JObjReqAnimAll(g_charAnimRoot, 0.0f);
    PCPort_HSDStartAnimAll(g_charAnimRoot);
    PCPort_HSDJObjAnimJointOnlyAll(g_charAnimRoot);
    PCPort_ProbeSnapshot(g_probeLoopStart);
    if (outStats->endFrame >= 1.0f) {
        int loopFrames = (int)(outStats->endFrame + 0.5f);
        if (loopFrames < 1) loopFrames = 1;
        for (f = 1; f < loopFrames; ++f) {
            PCPort_HSDJObjAnimJointOnlyAll(g_charAnimRoot);
        }
    }
    PCPort_ProbeSnapshot(g_probeLoopEnd);
    outStats->loopDelta = PCPort_ProbeSnapshotDelta(
        g_probeLoopStart, g_probeLoopEnd, 1, &outStats->loopMoved,
        &outStats->loopMaxRot, &outStats->loopMaxTrans,
        &outStats->rootLoopTrans);

    if (outStats->varyingFrames == 0 &&
        outStats->movedFromRest <= 1 &&
        outStats->maxRestRot < 0.01f &&
        outStats->maxRestTrans < 0.01f) {
        outStats->kind = "static/bind";
    } else if (outStats->loopDelta < 0.75f) {
        outStats->kind = "cyclic";
    } else {
        outStats->kind = "one-shot";
    }
}

static void PCPort_CharAnimProbeRelease(void) {
    if (g_charAnimData != NULL) {
        free(g_charAnimData);
        g_charAnimData = NULL;
    }
    if (g_charAnimArchive.storage != NULL) {
        PCPort_HSDArchiveDestroy(&g_charAnimArchive);
    }
    g_charAnimRoot = NULL;
    g_charAnimReady = 0;
    g_charAnimMotionIdx = -1;
    g_charAnimTime = 0.0f;
    g_charAnimLoopLen = -1.0f;
}

typedef struct PCPortMotionCandidate {
    int motionIdx;
    f32 energy;
} PCPortMotionCandidate;

static void PCPort_SortMotionCandidates(PCPortMotionCandidate* c, int count) {
    int i, j;
    for (i = 1; i < count; ++i) {
        PCPortMotionCandidate v = c[i];
        j = i - 1;
        while (j >= 0 && c[j].energy > v.energy) {
            c[j + 1] = c[j];
            --j;
        }
        c[j + 1] = v;
    }
}

static f32 PCPort_MotionProbeEnergy(const PCPortMotionProbeStats* s) {
    if (s == NULL || !s->valid) {
        return 0.0f;
    }
    return s->frameDeltaSum + s->checksumRange;
}

static int PCPort_MotionProbeConfirmedCyclic(const PCPortMotionProbeStats* s) {
    return s != NULL &&
           s->valid &&
           strcmp(s->kind, "cyclic") == 0 &&
           s->varyingFrames > 0;
}

static u32 PCPort_MotionProbeVaryingMotionCount(
    const PCPortMotionProbeStats* stats,
    u32 motionCount) {
    u32 i;
    u32 count = 0u;

    if (stats == NULL) {
        return 0u;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (stats[i].valid && stats[i].varyingFrames > 0) {
            ++count;
        }
    }
    return count;
}

static u32 PCPort_MotionProbeCyclicCount(
    const PCPortMotionProbeStats* stats,
    u32 motionCount) {
    u32 i;
    u32 count = 0u;

    if (stats == NULL) {
        return 0u;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (stats[i].valid && strcmp(stats[i].kind, "cyclic") == 0) {
            ++count;
        }
    }
    return count;
}

static u32 PCPort_MotionProbeConfirmedCyclicCount(
    const PCPortMotionProbeStats* stats,
    u32 motionCount) {
    u32 i;
    u32 count = 0u;

    if (stats == NULL) {
        return 0u;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (PCPort_MotionProbeConfirmedCyclic(&stats[i])) {
            ++count;
        }
    }
    return count;
}

static const PCPortMotionProbeStats* PCPort_FindMotionProbeStats(
    const PCPortMotionProbeStats* stats,
    u32 motionCount,
    int motionIdx) {
    u32 i;
    if (stats == NULL || motionIdx < 0) {
        return NULL;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (stats[i].valid && (int)stats[i].motionIdx == motionIdx) {
            return &stats[i];
        }
    }
    return NULL;
}

static int PCPort_SelectLocomotionSuggestionFromStats(
    const PCPortMotionProbeStats* stats,
    u32 motionCount,
    PCPortLocomotionSuggestion* out) {
    PCPortMotionCandidate cyclic[64];
    PCPortMotionCandidate varying[64];
    int count = 0;
    u32 i;
    f32 maxCyclicEndFrame = 0.0f;
    static const f32 thresholds[] = { 1.0f, 0.6f, 0.4f };
    int thresholdIdx;

    if (out == NULL) {
        return 0;
    }
    memset(out, 0, sizeof(*out));
    out->idle = -1;
    out->walk = -1;
    out->run = -1;
    out->motionCount = motionCount;

    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (!stats[i].valid) continue;
        if (strcmp(stats[i].kind, "cyclic") == 0) {
            ++out->cyclicCount;
        }
        if (!PCPort_MotionProbeConfirmedCyclic(&stats[i])) continue;
        if (stats[i].endFrame > maxCyclicEndFrame) {
            maxCyclicEndFrame = stats[i].endFrame;
        }
    }

    for (thresholdIdx = 0;
         thresholdIdx < (int)(sizeof(thresholds) / sizeof(thresholds[0]));
         ++thresholdIdx) {
        f32 minLocomotionEndFrame = maxCyclicEndFrame * thresholds[thresholdIdx];
        count = 0;
        out->varyingCyclicCount = 0u;
        for (i = 0u; i < motionCount && i < 64u; ++i) {
            if (!PCPort_MotionProbeConfirmedCyclic(&stats[i])) continue;
            if (maxCyclicEndFrame > 1.0f &&
                stats[i].endFrame + 0.5f < minLocomotionEndFrame) {
                continue;
            }
            ++out->varyingCyclicCount;
            cyclic[count].motionIdx = (int)stats[i].motionIdx;
            cyclic[count].energy = PCPort_MotionProbeEnergy(&stats[i]);
            ++count;
        }
        if (count >= 3) {
            break;
        }
    }
    if (count < 3) {
        int varyingCount = 0;
        out->varyingCyclicCount = (u32)count;
        for (i = 0u; i < motionCount && i < 64u; ++i) {
            if (!stats[i].valid || stats[i].varyingFrames <= 0) {
                continue;
            }
            varying[varyingCount].motionIdx = (int)stats[i].motionIdx;
            varying[varyingCount].energy = PCPort_MotionProbeEnergy(&stats[i]);
            ++varyingCount;
        }
        if (varyingCount < 3) {
            return 0;
        }
        PCPort_SortMotionCandidates(varying, varyingCount);
        out->idle = varying[0].motionIdx;
        out->run = varying[varyingCount - 1].motionIdx;
        out->walk = varying[(varyingCount > 2) ? 1 : (varyingCount - 1)].motionIdx;
        {
            const PCPortMotionProbeStats* idle =
                PCPort_FindMotionProbeStats(stats, motionCount, out->idle);
            const PCPortMotionProbeStats* walk =
                PCPort_FindMotionProbeStats(stats, motionCount, out->walk);
            const PCPortMotionProbeStats* run =
                PCPort_FindMotionProbeStats(stats, motionCount, out->run);
            out->idleConfirmed = idle != NULL && idle->valid &&
                                 idle->varyingFrames > 0;
            out->walkConfirmed = walk != NULL && walk->valid &&
                                 walk->varyingFrames > 0;
            out->runConfirmed = run != NULL && run->valid &&
                                run->varyingFrames > 0;
            out->allConfirmed = out->idleConfirmed &&
                                out->walkConfirmed &&
                                out->runConfirmed &&
                                out->idle != out->walk &&
                                out->idle != out->run &&
                                out->walk != out->run;
            out->idleEnergy = PCPort_MotionProbeEnergy(idle);
            out->walkEnergy = PCPort_MotionProbeEnergy(walk);
            out->runEnergy = PCPort_MotionProbeEnergy(run);
        }
        out->valid = 1;
        return 1;
    }

    PCPort_SortMotionCandidates(cyclic, count);
    out->idle = cyclic[0].motionIdx;
    out->run = cyclic[count - 1].motionIdx;
    out->walk = cyclic[(count > 2) ? 1 : (count - 1)].motionIdx;
    {
        const PCPortMotionProbeStats* idle =
            PCPort_FindMotionProbeStats(stats, motionCount, out->idle);
        const PCPortMotionProbeStats* walk =
            PCPort_FindMotionProbeStats(stats, motionCount, out->walk);
        const PCPortMotionProbeStats* run =
            PCPort_FindMotionProbeStats(stats, motionCount, out->run);
        out->idleConfirmed = PCPort_MotionProbeConfirmedCyclic(idle);
        out->walkConfirmed = PCPort_MotionProbeConfirmedCyclic(walk);
        out->runConfirmed = PCPort_MotionProbeConfirmedCyclic(run);
        out->allConfirmed = out->idleConfirmed &&
                            out->walkConfirmed &&
                            out->runConfirmed &&
                            out->idle != out->walk &&
                            out->idle != out->run &&
                            out->walk != out->run;
        out->idleEnergy = PCPort_MotionProbeEnergy(idle);
        out->walkEnergy = PCPort_MotionProbeEnergy(walk);
        out->runEnergy = PCPort_MotionProbeEnergy(run);
    }
    out->valid = 1;
    return 1;
}

static int PCPort_SelectLocomotionMapFromStats(
    const PCPortMotionProbeStats* stats,
    u32 motionCount,
    int* outIdle,
    int* outWalk,
    int* outRun) {
    PCPortLocomotionSuggestion s;

    if (outIdle == NULL || outWalk == NULL || outRun == NULL) {
        return 0;
    }
    if (!PCPort_SelectLocomotionSuggestionFromStats(stats, motionCount, &s)) {
        return 0;
    }
    *outIdle = s.idle;
    *outWalk = s.walk;
    *outRun = s.run;
    return 1;
}

int PCPort_CharAnimSuggestLocomotionMapEx(const char* fsysPath,
                                          const char* memberName,
                                          int frames,
                                          PCPortLocomotionSuggestion* out) {
    PCPortMotionProbeStats stats[64];
    u32 motionCount, motionIdx;
    int oldQuiet = g_charAnimQuietSetup;

    if (out == NULL) {
        return 0;
    }
    memset(out, 0, sizeof(*out));
    out->idle = -1;
    out->walk = -1;
    out->run = -1;
    memset(stats, 0, sizeof(stats));
    if (frames <= 1) frames = 40;

    g_charAnimQuietSetup = 1;
    motionCount = PCPort_CharAnimCountMotionBank(fsysPath, memberName);
    if (motionCount > 64u) motionCount = 64u;
    if (motionCount == 0u) {
        g_charAnimQuietSetup = oldQuiet;
        return 0;
    }

    for (motionIdx = 0u; motionIdx < motionCount; ++motionIdx) {
        PCPort_MotionProbeCollectStats(fsysPath, memberName, motionIdx, frames,
                                       &stats[motionIdx]);
    }
    g_charAnimQuietSetup = oldQuiet;
    PCPort_CharAnimProbeRelease();

    return PCPort_SelectLocomotionSuggestionFromStats(stats, motionCount, out);
}

int PCPort_CharAnimSuggestLocomotionMap(const char* fsysPath,
                                        const char* memberName,
                                        int* outIdle,
                                        int* outWalk,
                                        int* outRun) {
    PCPortLocomotionSuggestion s;

    if (outIdle == NULL || outWalk == NULL || outRun == NULL) {
        return 0;
    }
    *outIdle = -1;
    *outWalk = -1;
    *outRun = -1;
    if (!PCPort_CharAnimSuggestLocomotionMapEx(fsysPath, memberName, 40, &s)) {
        return 0;
    }
    *outIdle = s.idle;
    *outWalk = s.walk;
    *outRun = s.run;
    return 1;
}

void PCPort_MotionProbe(const char* fsysPath, const char* memberName,
                        int frames) {
    u32 motionCount, motionIdx;
    const char* maxEnv = getenv("PCPORT_CHARANIM_BANK_MAX");
    const char* verboseEnv = getenv("PCPORT_MOTION_PROBE_VERBOSE");
    PCPortMotionProbeStats stats[64];
    u32 maxMotions = (maxEnv != NULL && maxEnv[0]) ? (u32)atoi(maxEnv) : 0u;
    int verbose = verboseEnv != NULL && verboseEnv[0] != '\0';

    memset(stats, 0, sizeof(stats));
    if (frames <= 0) frames = 30;
    motionCount = PCPort_CharAnimCountMotionBank(fsysPath, memberName);
    if (motionCount > 64u) {
        motionCount = 64u;
    }
    if (maxMotions > 0u && motionCount > maxMotions) {
        motionCount = maxMotions;
    }
    printf("[motion-probe] %s :: %s -> %u motion(s), stepping %d frame(s) each\n",
           fsysPath, memberName, motionCount, frames);
    fflush(stdout);
    if (motionCount == 0u) {
        printf("[motion-probe] no Resource+0x4 motion bank found\n");
        fflush(stdout);
        return;
    }

    printf("[motion-probe] id joints aobjs endFrame frames cyclic kind "
           "varyFrames frameDeltaSum frameDeltaMax checksumRange "
           "loopDelta moved maxDRot maxDTrans rootLoopTrans\n");
    fflush(stdout);
    for (motionIdx = 0u; motionIdx < motionCount; ++motionIdx) {
        PCPortMotionProbeStats* s = &stats[motionIdx];
        PCPort_MotionProbeCollectStats(fsysPath, memberName, motionIdx, frames,
                                       s);
        if (!s->valid) {
            printf("[motion-probe] %2u setup FAILED\n", motionIdx);
            fflush(stdout);
            continue;
        }

        printf("[motion-probe] %2u %6d %5d %8.2f %6d %6s %-11s "
               "%10d %13.4f %13.4f %13.4f %9.4f %5d %7.4f %9.4f %13.4f\n",
               motionIdx, s->jointCount, s->aobjCount, s->endFrame,
               s->probeFrames,
               strcmp(s->kind, "cyclic") == 0 ? "yes" : "no",
               s->kind, s->varyingFrames, s->frameDeltaSum,
               s->frameDeltaMax, s->checksumRange, s->loopDelta,
               s->movedFromRest, s->maxRestRot, s->maxRestTrans,
               s->rootLoopTrans);
        if (verbose) {
            printf("[motion-probe]    checksum %.4f -> %.4f, loopMoved=%d "
                   "loopMaxRot=%.4f loopMaxTrans=%.4f\n",
                   s->checksumFirst, s->checksumLast, s->loopMoved,
                   s->loopMaxRot, s->loopMaxTrans);
        }
        fflush(stdout);
    }
    {
        int idle = -1, walk = -1, run = -1;
        if (PCPort_SelectLocomotionMapFromStats(stats, motionCount,
                                               &idle, &walk, &run)) {
            printf("[motion-probe] data-derived locomotion map: "
                   "idle=%d walk=%d run=%d "
                   "(PCPORT_FIELD_MOTION_MAP=%d,%d,%d)\n",
                   idle, walk, run, idle, walk, run);
        } else {
            printf("[motion-probe] data-derived locomotion map: unavailable\n");
        }
        fflush(stdout);
    }

    PCPort_CharAnimProbeRelease();
}

void PCPort_CharAnimBankProbe(const char* fsysPath, const char* memberName,
                              int frames) {
    PCPort_MotionProbe(fsysPath, memberName, frames);
}

void PCPort_HeadlessMotionProbe(const char* fsysPath, const char* memberName,
                                int frames) {
    u32 motionCount;
    u32 motionIdx;
    const char* maxEnv = getenv("PCPORT_CHARANIM_BANK_MAX");
    const char* verboseEnv = getenv("PCPORT_MOTION_PROBE_VERBOSE");
    PCPortMotionProbeStats stats[64];
    u32 maxMotions = (maxEnv != NULL && maxEnv[0]) ? (u32)atoi(maxEnv) : 0u;
    int verbose = verboseEnv != NULL && verboseEnv[0] != '\0';

    if (frames <= 0) frames = 30;
    if (!PCPort_HeadlessMotionCollectMemberStats(fsysPath, memberName, frames,
                                                 stats, &motionCount, 1)) {
        printf("[headless-motion] %s :: %s -> 0 motion(s)\n",
               fsysPath, memberName);
        printf("[headless-motion] no matching scene_data Resource+0x4 bank\n");
        fflush(stdout);
        return;
    }
    if (motionCount > 64u) {
        motionCount = 64u;
    }
    if (maxMotions > 0u && motionCount > maxMotions) {
        motionCount = maxMotions;
    }

    printf("[headless-motion] %s :: %s -> %u motion(s), stepping %d "
           "frame(s) each\n", fsysPath, memberName, motionCount, frames);
    printf("[headless-motion] id joints aobjs endFrame frames cyclic kind "
           "varyFrames frameDeltaSum frameDeltaMax checksumRange "
           "loopDelta moved maxDRot maxDTrans rootLoopTrans\n");
    fflush(stdout);

    for (motionIdx = 0u; motionIdx < motionCount; ++motionIdx) {
        PCPortMotionProbeStats* s = &stats[motionIdx];
        if (!s->valid) {
            printf("[headless-motion] %2u setup FAILED\n", motionIdx);
            fflush(stdout);
            continue;
        }

        printf("[headless-motion] %2u %6d %5d %8.2f %6d %6s %-11s "
               "%10d %13.4f %13.4f %13.4f %9.4f %5d %7.4f %9.4f %13.4f\n",
               motionIdx, s->jointCount, s->aobjCount, s->endFrame,
               s->probeFrames,
               strcmp(s->kind, "cyclic") == 0 ? "yes" : "no",
               s->kind, s->varyingFrames, s->frameDeltaSum,
               s->frameDeltaMax, s->checksumRange, s->loopDelta,
               s->movedFromRest, s->maxRestRot, s->maxRestTrans,
               s->rootLoopTrans);
        if (verbose) {
            printf("[headless-motion]    checksum %.4f -> %.4f, "
                   "loopMoved=%d loopMaxRot=%.4f loopMaxTrans=%.4f\n",
                   s->checksumFirst, s->checksumLast, s->loopMoved,
                   s->loopMaxRot, s->loopMaxTrans);
        }
        fflush(stdout);
    }

    {
        PCPortLocomotionSuggestion suggestion;
        if (PCPort_SelectLocomotionSuggestionFromStats(stats, motionCount,
                                                       &suggestion)) {
            printf("[headless-motion] data-derived locomotion map: "
                   "idle=%d walk=%d run=%d confirmed=%s cyclic=%u/%u "
                   "energy=%.4f/%.4f/%.4f "
                   "(PCPORT_FIELD_MOTION_MAP=%d,%d,%d)\n",
                   suggestion.idle, suggestion.walk, suggestion.run,
                   suggestion.allConfirmed ? "yes" : "no",
                   suggestion.varyingCyclicCount, suggestion.cyclicCount,
                   suggestion.idleEnergy, suggestion.walkEnergy,
                   suggestion.runEnergy,
                   suggestion.idle, suggestion.walk, suggestion.run);
        } else {
            u32 varying = PCPort_MotionProbeVaryingMotionCount(
                stats, motionCount);
            if (varying > 0u) {
                printf("[headless-motion] data-derived locomotion map: "
                       "unavailable; animates=yes varying=%u "
                       "cyclic=%u varyCyclic=%u\n",
                       varying,
                       PCPort_MotionProbeCyclicCount(stats, motionCount),
                       PCPort_MotionProbeConfirmedCyclicCount(stats,
                                                              motionCount));
            } else {
                printf("[headless-motion] data-derived locomotion map: "
                       "unavailable; animates=no\n");
            }
        }
        fflush(stdout);
    }
}

typedef struct PCPortAnimDumpJointRef {
    HSD_JObj* joint;
    int parent;
} PCPortAnimDumpJointRef;

typedef struct PCPortAnimDumpTransform {
    f32 basis[3][3];
    f32 pos[3];
} PCPortAnimDumpTransform;

static void PCPort_AnimDumpCollectJoints(HSD_JObj* joint,
                                         int parentIndex,
                                         PCPortAnimDumpJointRef* refs,
                                         int* count,
                                         int maxRefs) {
    for (; joint != NULL; joint = joint->next) {
        int currentIndex = -1;
        if (*count < maxRefs) {
            currentIndex = *count;
            refs[currentIndex].joint = joint;
            refs[currentIndex].parent = parentIndex;
            ++(*count);
        }
        if (joint->child != NULL && currentIndex >= 0) {
            PCPort_AnimDumpCollectJoints(joint->child, currentIndex,
                                         refs, count, maxRefs);
        }
    }
}

static void PCPort_AnimDumpMat3Identity(f32 m[3][3]) {
    memset(m, 0, sizeof(f32) * 9u);
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
}

static void PCPort_AnimDumpMat3Mul(const f32 a[3][3], const f32 b[3][3],
                                   f32 out[3][3]) {
    int r, c, k;
    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 3; ++c) {
            f32 sum = 0.0f;
            for (k = 0; k < 3; ++k) {
                sum += a[r][k] * b[k][c];
            }
            out[r][c] = sum;
        }
    }
}

static void PCPort_AnimDumpMat3VecMul(const f32 m[3][3], const f32 v[3],
                                      f32 out[3]) {
    out[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
    out[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
    out[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
}

static void PCPort_AnimDumpBuildLocalTransform(const HSD_JObj* joint,
                                               PCPortAnimDumpTransform* out) {
    f32 rx[3][3];
    f32 ry[3][3];
    f32 rz[3][3];
    f32 tmp[3][3];
    f32 rot[3][3];
    f32 scale[3][3];
    f32 scaled[3][3];
    f32 qx, qy, qz, qw;

    if (out == NULL) {
        return;
    }
    PCPort_AnimDumpMat3Identity(rx);
    PCPort_AnimDumpMat3Identity(ry);
    PCPort_AnimDumpMat3Identity(rz);
    PCPort_AnimDumpMat3Identity(scale);
    scale[0][0] = joint != NULL ? joint->scale_x : 1.0f;
    scale[1][1] = joint != NULL ? joint->scale_y : 1.0f;
    scale[2][2] = joint != NULL ? joint->scale_z : 1.0f;

    if (joint != NULL && (joint->flags & JOBJ_USE_QUATERNION) != 0u) {
        qx = joint->rotate_x;
        qy = joint->rotate_y;
        qz = joint->rotate_z;
        qw = joint->rotate_w;
        rot[0][0] = 1.0f - 2.0f * (qy * qy + qz * qz);
        rot[0][1] = 2.0f * (qx * qy - qz * qw);
        rot[0][2] = 2.0f * (qx * qz + qy * qw);
        rot[1][0] = 2.0f * (qx * qy + qz * qw);
        rot[1][1] = 1.0f - 2.0f * (qx * qx + qz * qz);
        rot[1][2] = 2.0f * (qy * qz - qx * qw);
        rot[2][0] = 2.0f * (qx * qz - qy * qw);
        rot[2][1] = 2.0f * (qy * qz + qx * qw);
        rot[2][2] = 1.0f - 2.0f * (qx * qx + qy * qy);
    } else {
        f32 cx = cosf(joint != NULL ? joint->rotate_x : 0.0f);
        f32 sx = sinf(joint != NULL ? joint->rotate_x : 0.0f);
        f32 cy = cosf(joint != NULL ? joint->rotate_y : 0.0f);
        f32 sy = sinf(joint != NULL ? joint->rotate_y : 0.0f);
        f32 cz = cosf(joint != NULL ? joint->rotate_z : 0.0f);
        f32 sz = sinf(joint != NULL ? joint->rotate_z : 0.0f);

        rx[0][0] = 1.0f; rx[0][1] = 0.0f; rx[0][2] = 0.0f;
        rx[1][0] = 0.0f; rx[1][1] = cx;   rx[1][2] = -sx;
        rx[2][0] = 0.0f; rx[2][1] = sx;   rx[2][2] = cx;

        ry[0][0] = cy;   ry[0][1] = 0.0f; ry[0][2] = sy;
        ry[1][0] = 0.0f; ry[1][1] = 1.0f; ry[1][2] = 0.0f;
        ry[2][0] = -sy;  ry[2][1] = 0.0f; ry[2][2] = cy;

        rz[0][0] = cz;   rz[0][1] = -sz;  rz[0][2] = 0.0f;
        rz[1][0] = sz;   rz[1][1] = cz;   rz[1][2] = 0.0f;
        rz[2][0] = 0.0f; rz[2][1] = 0.0f;  rz[2][2] = 1.0f;

        PCPort_AnimDumpMat3Mul(rz, ry, tmp);
        PCPort_AnimDumpMat3Mul(tmp, rx, rot);
    }

    PCPort_AnimDumpMat3Mul(rot, scale, scaled);
    memcpy(out->basis, scaled, sizeof(out->basis));
    out->pos[0] = joint != NULL ? joint->translate_x : 0.0f;
    out->pos[1] = joint != NULL ? joint->translate_y : 0.0f;
    out->pos[2] = joint != NULL ? joint->translate_z : 0.0f;
}

static void PCPort_AnimDumpCombineTransform(const PCPortAnimDumpTransform* parent,
                                            const PCPortAnimDumpTransform* local,
                                            PCPortAnimDumpTransform* out) {
    f32 rotated[3];

    if (out == NULL || local == NULL) {
        return;
    }
    if (parent == NULL) {
        memcpy(out->basis, local->basis, sizeof(out->basis));
        memcpy(out->pos, local->pos, sizeof(out->pos));
        return;
    }
    PCPort_AnimDumpMat3Mul(parent->basis, local->basis, out->basis);
    PCPort_AnimDumpMat3VecMul(parent->basis, local->pos, rotated);
    out->pos[0] = parent->pos[0] + rotated[0];
    out->pos[1] = parent->pos[1] + rotated[1];
    out->pos[2] = parent->pos[2] + rotated[2];
}

static void PCPort_AnimDumpFillFramePositions(
    HSD_JObj* joint,
    const PCPortAnimDumpTransform* parent,
    f32 positions[PCPORT_PROBE_MAX_JOINTS][3],
    int* cursor,
    int maxRefs) {
    PCPortAnimDumpTransform local;
    PCPortAnimDumpTransform world;

    for (; joint != NULL; joint = joint->next) {
        PCPort_AnimDumpBuildLocalTransform(joint, &local);
        PCPort_AnimDumpCombineTransform(parent, &local, &world);
        if (*cursor < maxRefs) {
            positions[*cursor][0] = world.pos[0];
            positions[*cursor][1] = world.pos[1];
            positions[*cursor][2] = world.pos[2];
            ++(*cursor);
        }
        if (joint->child != NULL) {
            PCPort_AnimDumpFillFramePositions(joint->child, &world,
                                              positions, cursor, maxRefs);
        }
    }
}

static const char* PCPort_AnimDumpModelAlias(const char* memberName) {
    if (memberName == NULL) {
        return "model";
    }
    if (strcmp(memberName, "ken_b1") == 0) {
        return "wes";
    }
    return memberName;
}

static const char* PCPort_AnimDumpMotionLabel(u32 motionIdx) {
    switch (motionIdx) {
    case 1u:
        return "idle";
    case 5u:
        return "walk";
    case 8u:
        return "run";
    default:
        break;
    }
    return NULL;
}

static void PCPort_AnimDumpWriteFrameJson(FILE* out,
                                          int frameIdx,
                                          const PCPortAnimDumpJointRef* refs,
                                          const f32 positions[PCPORT_PROBE_MAX_JOINTS][3],
                                          int jointCount) {
    int i;
    fprintf(out, "    {\"frame\":%d,\"joints\":[", frameIdx);
    for (i = 0; i < jointCount; ++i) {
        if (i > 0) {
            fputs(",", out);
        }
        if (refs[i].joint == NULL) {
            fprintf(out, "{\"x\":0.0,\"y\":0.0,\"z\":0.0,\"parent\":%d}",
                    refs[i].parent);
            continue;
        }
        fprintf(out, "{\"x\":%.6f,\"y\":%.6f,\"z\":%.6f,\"parent\":%d}",
                positions[i][0], positions[i][1], positions[i][2],
                refs[i].parent);
    }
    fputs("]}", out);
}

void PCPort_AnimDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames) {
    PCPortHeadlessMotionBank bank;
    HSD_JObj* root = NULL;
    HSD_AnimJoint* animjoint;
    PCPortAnimDumpJointRef refs[PCPORT_PROBE_MAX_JOINTS];
    f32 positions[PCPORT_PROBE_MAX_JOINTS][3];
    int jointCount = 0;
    int frameIdx;
    FILE* jsonOut = NULL;
    char jsonPath[384];
    const char* alias;
    const char* motionLabel;
    char motionLabelAuto[32];

    if (frames <= 0) {
        frames = 24;
    }
    if (!PCPort_HeadlessMotionBankLoad(fsysPath, memberName, &bank, 1)) {
        printf("[anim-dump] load failed %s :: %s\n", fsysPath, memberName);
        fflush(stdout);
        return;
    }
    if (bank.motionCount == 0u) {
        printf("[anim-dump] no motions %s :: %s\n", fsysPath, memberName);
        fflush(stdout);
        PCPort_HeadlessMotionBankRelease(&bank);
        return;
    }
    if (motionIdx < 0) {
        motionIdx = 0;
    }
    if ((u32)motionIdx >= bank.motionCount) {
        motionIdx = (int)bank.motionCount - 1;
    }

    root = HSD_JObjLoadJoint((HSD_Joint*)(bank.archive.storage + bank.rootOff));
    if (root == NULL) {
        printf("[anim-dump] HSD_JObjLoadJoint failed %s :: %s\n",
               fsysPath, memberName);
        fflush(stdout);
        PCPort_HeadlessMotionBankRelease(&bank);
        return;
    }
    animjoint = (HSD_AnimJoint*)(bank.archive.storage +
                                 bank.motionOffs[(u32)motionIdx]);
    HSD_JObjAddAnimAll(root, animjoint, NULL, NULL);
    HSD_JObjReqAnimAll(root, 0.0f);
    PCPort_HSDStartAnimAll(root);

    memset(refs, 0, sizeof(refs));
    memset(positions, 0, sizeof(positions));
    PCPort_AnimDumpCollectJoints(root, -1, refs, &jointCount,
                                 PCPORT_PROBE_MAX_JOINTS);
    if (jointCount <= 0) {
        printf("[anim-dump] no joints %s :: %s\n", fsysPath, memberName);
        fflush(stdout);
        HSD_JObjRemoveAll(root);
        PCPort_HeadlessMotionBankRelease(&bank);
        return;
    }

    alias = PCPort_AnimDumpModelAlias(memberName);
    motionLabel = PCPort_AnimDumpMotionLabel((u32)motionIdx);
    if (motionLabel == NULL) {
        snprintf(motionLabelAuto, sizeof(motionLabelAuto), "motion%d",
                 motionIdx);
        motionLabel = motionLabelAuto;
    }
    snprintf(jsonPath, sizeof(jsonPath), "build_pc/anim_%s_%s.json",
             alias, motionLabel);
    jsonOut = fopen(jsonPath, "wb");
    if (jsonOut == NULL) {
        printf("[anim-dump] open failed %s\n", jsonPath);
        fflush(stdout);
        HSD_JObjRemoveAll(root);
        PCPort_HeadlessMotionBankRelease(&bank);
        return;
    }

    fprintf(jsonOut,
            "{\n"
            "  \"model\":\"%s\",\n"
            "  \"member\":\"%s\",\n"
            "  \"motionId\":%d,\n"
            "  \"motionLabel\":\"%s\",\n"
            "  \"jointCount\":%d,\n"
            "  \"frames\":[\n",
            alias, memberName, motionIdx, motionLabel, jointCount);
    {
        int loopFrames = (int)(PCPort_CharAnimMaxEndFrame(root) + 0.5f);
        if (loopFrames < 1) {
            loopFrames = 0;
        }
        for (frameIdx = 0; frameIdx < frames; ++frameIdx) {
            int frameJointCount = 0;
            PCPortAnimDumpTransform identity;

            if (frameIdx > 0) {
                fputs(",\n", jsonOut);
            }
            HSD_JObjAnimAll(root);
            memset(positions, 0, sizeof(positions));
            PCPort_AnimDumpMat3Identity(identity.basis);
            identity.pos[0] = 0.0f;
            identity.pos[1] = 0.0f;
            identity.pos[2] = 0.0f;
            PCPort_AnimDumpFillFramePositions(root, &identity, positions,
                                              &frameJointCount,
                                              PCPORT_PROBE_MAX_JOINTS);
            PCPort_AnimDumpWriteFrameJson(jsonOut, frameIdx, refs, positions,
                                          jointCount);
            if (loopFrames > 0 && frameIdx + 1 < frames &&
                ((frameIdx + 1) % loopFrames) == 0) {
                HSD_JObjReqAnimAll(root, 0.0f);
                PCPort_HSDStartAnimAll(root);
            }
        }
    }
    fputs("\n  ]\n}\n", jsonOut);
    fclose(jsonOut);

    printf("[anim-dump] wrote %s frames=%d joints=%d\n",
           jsonPath, frames, jointCount);
    fflush(stdout);

    HSD_JObjRemoveAll(root);
    PCPort_HeadlessMotionBankRelease(&bank);
}

static void PCPort_AnimDumpTransformPoint(const PCPortAnimDumpTransform* t,
                                          const f32 in[3],
                                          f32 out[3]) {
    if (t == NULL || in == NULL || out == NULL) {
        return;
    }
    out[0] = (t->basis[0][0] * in[0]) + (t->basis[0][1] * in[1]) +
             (t->basis[0][2] * in[2]) + t->pos[0];
    out[1] = (t->basis[1][0] * in[0]) + (t->basis[1][1] * in[1]) +
             (t->basis[1][2] * in[2]) + t->pos[1];
    out[2] = (t->basis[2][0] * in[0]) + (t->basis[2][1] * in[1]) +
             (t->basis[2][2] * in[2]) + t->pos[2];
}

static const char* PCPort_MeshDumpPrimitiveName(u8 opcode) {
    switch (opcode & 0xF8u) {
    case 0x80u: return "quads";
    case 0x90u: return "triangles";
    case 0x98u: return "triangle_strip";
    case 0xA0u: return "triangle_fan";
    case 0xA8u: return "lines";
    case 0xB0u: return "line_strip";
    case 0xB8u: return "points";
    default:    return "triangles";
    }
}

static HSD_JObj* PCPort_MeshDumpLoadJoint(HSD_Joint* joint) {
    HSD_JObj* jobj;
    HSD_JObj* child;

    if (joint == NULL) {
        return NULL;
    }

    jobj = HSD_JObjAlloc();
    if (jobj == NULL) {
        return NULL;
    }

    jobj->flags = joint->flags;
    jobj->rotate_x = joint->rotation_x;
    jobj->rotate_y = joint->rotation_y;
    jobj->rotate_z = joint->rotation_z;
    jobj->scale_x = joint->scale_x;
    jobj->scale_y = joint->scale_y;
    jobj->scale_z = joint->scale_z;
    jobj->translate_x = joint->position_x;
    jobj->translate_y = joint->position_y;
    jobj->translate_z = joint->position_z;

    if (joint->u.dobjdesc != NULL) {
        jobj->u.dobj = HSD_DObjLoadDesc(joint->u.dobjdesc);
    }
    jobj->child = PCPort_MeshDumpLoadJoint(joint->child);
    for (child = jobj->child; child != NULL; child = child->next) {
        child->parent = jobj;
    }
    jobj->next = PCPort_MeshDumpLoadJoint(joint->next);
    return jobj;
}

static const HSD_VtxDescList* PCPort_MeshDumpFindVtxDesc(
    const HSD_PObj* pobj,
    u32 attr) {
    const HSD_VtxDescList* v;

    if (pobj == NULL || pobj->verts == NULL) {
        return NULL;
    }
    for (v = pobj->verts; v->attr != GX_VA_NULL; ++v) {
        if (v->attr == attr) {
            return v;
        }
    }
    return NULL;
}

static void PCPort_MeshDumpCountObjects(HSD_JObj* joint,
                                        int* outDObjCount,
                                        int* outPObjCount) {
    HSD_DObj* dobj;

    if (joint == NULL) {
        return;
    }
    if (joint->u.dobj != NULL && outDObjCount != NULL) {
        ++(*outDObjCount);
        for (dobj = joint->u.dobj; dobj != NULL; dobj = dobj->next) {
            HSD_PObj* pobj;
            for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
                if (outPObjCount != NULL) {
                    ++(*outPObjCount);
                }
            }
        }
    }
    if (joint->child != NULL) {
        PCPort_MeshDumpCountObjects(joint->child, outDObjCount, outPObjCount);
    }
    if (joint->next != NULL) {
        PCPort_MeshDumpCountObjects(joint->next, outDObjCount, outPObjCount);
    }
}

static BOOL PCPort_MeshDumpReadPosition(const HSD_VtxDescList* posDesc,
                                        u32 index,
                                        f32 out[3]) {
    const f32* src;

    if (posDesc == NULL || posDesc->vertex == NULL || out == NULL) {
        return FALSE;
    }
    src = (const f32*)((const u8*)posDesc->vertex + ((size_t)index * posDesc->stride));
    out[0] = src[0];
    out[1] = src[1];
    out[2] = (posDesc->comp_cnt == GX_POS_XYZ) ? src[2] : 0.0f;
    return TRUE;
}

static BOOL PCPort_MeshDumpWritePObjJson(
    FILE* out,
    const HSD_PObj* pobj,
    const PCPortAnimDumpTransform* world,
    u32 jointId,
    int* meshCount) {
    const HSD_VtxDescList* posDesc;
    const HSD_VtxDescList* v;
    const u8* cursor;
    const u8* end;
    int primitiveCount;

    if (out == NULL || pobj == NULL || meshCount == NULL) {
        return FALSE;
    }
    posDesc = PCPort_MeshDumpFindVtxDesc(pobj, GX_VA_POS);
    if (posDesc == NULL || pobj->display == NULL || pobj->n_display == 0u) {
        if (getenv("PCPORT_MESH_DEBUG") != NULL) {
            fprintf(stdout,
                    "[mesh-dump-debug] joint=%u posDesc=%p display=%p n_display=%u pobjFlags=0x%X\n",
                    jointId, (const void*)posDesc, (const void*)pobj->display,
                    (u32)pobj->n_display, (u32)pobj->flags);
            fflush(stdout);
        }
        return FALSE;
    }

    cursor = pobj->display;
    end = cursor + pobj->n_display;
    if (*meshCount > 0) {
        fputs(",\n", out);
    }
    fprintf(out,
            "      {\"jointId\":%u,\"pobjFlags\":%u,\"primitives\":[",
            jointId, (u32)pobj->flags);

    primitiveCount = 0;
    while (cursor < end) {
        u8 opcode;
        u16 vertexCount;
        u32 i;
        if ((u32)(end - cursor) < 3u) {
            break;
        }
        if (cursor[0] == 0u && cursor[1] == 0u && cursor[2] == 0u) {
            break;
        }
        if (cursor[0] == 0u) {
            cursor += 1u;
            continue;
        }

        opcode = cursor[0];
        ++cursor;
        vertexCount = (u16)(((u16)cursor[0] << 8) | cursor[1]);
        cursor += 2;
        if (vertexCount == 0u) {
            return FALSE;
        }

        if (primitiveCount > 0) {
            fputs(",", out);
        }
        fprintf(out, "{\"opcode\":%u,\"kind\":\"%s\",\"vertices\":[",
                (u32)opcode, PCPort_MeshDumpPrimitiveName(opcode));

        for (i = 0u; i < vertexCount; ++i) {
            u32 posIndex = 0u;
            f32 localPos[3];
            f32 worldPos[3];

            for (v = pobj->verts; v->attr != GX_VA_NULL; ++v) {
                int indexSize;
                u32 index;

                if (v->attr <= GX_VA_TEX7MTXIDX) {
                    if ((u32)(end - cursor) < 1u) {
                        return FALSE;
                    }
                    cursor += 1u;
                    continue;
                }

                indexSize = GetIndexByteCount(v->attr_type);
                if (indexSize == 0 || (u32)(end - cursor) < (u32)indexSize) {
                    return FALSE;
                }
                if (indexSize == 1) {
                    index = cursor[0];
                } else {
                    index = (u32)(((u16)cursor[0] << 8) | cursor[1]);
                }
                cursor += (u32)indexSize;

                if (v->attr == GX_VA_POS) {
                    posIndex = index;
                }
            }

            if (!PCPort_MeshDumpReadPosition(posDesc, posIndex, localPos)) {
                return FALSE;
            }
            PCPort_AnimDumpTransformPoint(world, localPos, worldPos);
            if (i > 0u) {
                fputs(",", out);
            }
            fprintf(out, "{\"x\":%.6f,\"y\":%.6f,\"z\":%.6f}",
                    worldPos[0], worldPos[1], worldPos[2]);
        }

        fputs("]}", out);
        ++primitiveCount;
    }

    fputs("]}", out);
    if (getenv("PCPORT_MESH_DEBUG") != NULL) {
        fprintf(stdout,
                "[mesh-dump-debug] joint=%u primitives=%d displayBytes=%u\n",
                jointId, primitiveCount, (u32)pobj->n_display);
        fflush(stdout);
    }
    ++(*meshCount);
    return TRUE;
}

static void PCPort_MeshDumpFillFrameMeshes(
    HSD_JObj* joint,
    const PCPortAnimDumpTransform* parent,
    FILE* out,
    int* meshCount) {
    PCPortAnimDumpTransform local;
    PCPortAnimDumpTransform world;
    HSD_DObj* dobj;

    for (; joint != NULL; joint = joint->next) {
        PCPort_AnimDumpBuildLocalTransform(joint, &local);
        PCPort_AnimDumpCombineTransform(parent, &local, &world);
        if (joint->u.dobj != NULL) {
            for (dobj = joint->u.dobj; dobj != NULL; dobj = dobj->next) {
                HSD_PObj* pobj;
                for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
                    if (!PCPort_MeshDumpWritePObjJson(out, pobj, &world,
                                                      joint->id, meshCount)) {
                        continue;
                    }
                }
            }
        }
        if (joint->child != NULL) {
            PCPort_MeshDumpFillFrameMeshes(joint->child, &world, out,
                                           meshCount);
        }
    }
}

void PCPort_MeshDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames) {
    HSD_JObj* root = NULL;
    PCPortAnimDumpJointRef refs[PCPORT_PROBE_MAX_JOINTS];
    f32 positions[PCPORT_PROBE_MAX_JOINTS][3];
    int jointCount = 0;
    int frameIdx;
    FILE* jsonOut = NULL;
    char jsonPath[384];
    const char* alias;
    const char* motionLabel;
    char motionLabelAuto[32];

    (void)positions;

    if (frames <= 0) {
        frames = 24;
    }
    g_charAnimMotionIdx = motionIdx;
    if (!PCPort_CharAnimSetup(fsysPath, memberName)) {
        return;
    }
    root = g_charAnimRoot;
    if (root == NULL) {
        PCPort_HSDArchiveDestroy(&g_charAnimArchive);
        free(g_charAnimData);
        g_charAnimData = NULL;
        g_charAnimRoot = NULL;
        g_charAnimReady = 0;
        return;
    }
    if (getenv("PCPORT_MESH_DEBUG") != NULL) {
        int dobjCount = 0;
        int pobjCount = 0;
        PCPort_MeshDumpCountObjects(root, &dobjCount, &pobjCount);
        printf("[mesh-dump-debug] root=%p dobjs=%d pobjs=%d\n",
               (void*)root, dobjCount, pobjCount);
        fflush(stdout);
    }

    PCPort_AnimDumpCollectJoints(root, -1, refs, &jointCount,
                                 PCPORT_PROBE_MAX_JOINTS);
    (void)jointCount;

    alias = PCPort_AnimDumpModelAlias(memberName);
    motionLabel = PCPort_AnimDumpMotionLabel((u32)motionIdx);
    if (motionLabel == NULL) {
        snprintf(motionLabelAuto, sizeof(motionLabelAuto), "motion%d",
                 motionIdx);
        motionLabel = motionLabelAuto;
    }
    snprintf(jsonPath, sizeof(jsonPath), "build_pc/mesh_%s_%s.json",
             alias, motionLabel);
    jsonOut = fopen(jsonPath, "wb");
    if (jsonOut == NULL) {
        printf("[mesh-dump] open failed %s\n", jsonPath);
        fflush(stdout);
        HSD_JObjRemoveAll(root);
        PCPort_HSDArchiveDestroy(&g_charAnimArchive);
        free(g_charAnimData);
        g_charAnimData = NULL;
        g_charAnimRoot = NULL;
        g_charAnimReady = 0;
        return;
    }

    fprintf(jsonOut,
            "{\n"
            "  \"model\":\"%s\",\n"
            "  \"member\":\"%s\",\n"
            "  \"motionId\":%d,\n"
            "  \"motionLabel\":\"%s\",\n"
            "  \"frames\":[\n",
            alias, memberName, motionIdx, motionLabel);

    {
        int loopFrames = (int)(PCPort_CharAnimMaxEndFrame(root) + 0.5f);
        if (loopFrames < 1) {
            loopFrames = 0;
        }
        for (frameIdx = 0; frameIdx < frames; ++frameIdx) {
            PCPortAnimDumpTransform identity;
            int meshCount = 0;

            if (frameIdx > 0) {
                fputs(",\n", jsonOut);
            }
            HSD_JObjAnimAll(root);
            PCPort_AnimDumpMat3Identity(identity.basis);
            identity.pos[0] = 0.0f;
            identity.pos[1] = 0.0f;
            identity.pos[2] = 0.0f;
            fprintf(jsonOut, "    {\"frame\":%d,\"meshes\":[", frameIdx);
            PCPort_MeshDumpFillFrameMeshes(root, &identity, jsonOut, &meshCount);
            fputs("]}", jsonOut);
            if (loopFrames > 0 && frameIdx + 1 < frames &&
                ((frameIdx + 1) % loopFrames) == 0) {
                HSD_JObjReqAnimAll(root, 0.0f);
                PCPort_HSDStartAnimAll(root);
            }
        }
    }
    fputs("\n  ]\n}\n", jsonOut);
    fclose(jsonOut);

    printf("[mesh-dump] wrote %s frames=%d\n", jsonPath, frames);
    fflush(stdout);

    HSD_JObjRemoveAll(root);
    PCPort_HSDArchiveDestroy(&g_charAnimArchive);
    free(g_charAnimData);
    g_charAnimData = NULL;
    g_charAnimRoot = NULL;
    g_charAnimReady = 0;
}

static const char* PCPort_PathBaseName(const char* path) {
    const char* slash;
    const char* backslash;
    if (path == NULL) {
        return "";
    }
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    if (backslash != NULL && (slash == NULL || backslash > slash)) {
        slash = backslash;
    }
    return slash != NULL ? slash + 1 : path;
}

static void PCPort_MotionBatchProbeArchive(const char* fsysPath, int frames) {
    u8* fsysData;
    u32 fsysSize = 0;
    u32 entryCount, stringTableOffset, entryTableOffset, i;
    u32 probed = 0u, confirmed = 0u;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        printf("[motion-batch] cannot open %s\n", fsysPath);
        fflush(stdout);
        return;
    }
    if (fsysSize < 0x20u || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        printf("[motion-batch] %s is not an FSYS archive\n", fsysPath);
        fflush(stdout);
        free(fsysData);
        return;
    }
    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4u > fsysSize) {
        free(fsysData);
        return;
    }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        free(fsysData);
        return;
    }

    printf("[motion-batch] archive=%s members=%u frames=%d\n",
           fsysPath, entryCount, frames);
    fflush(stdout);
    for (i = 0u; i < entryCount; ++i) {
        u32 entryOffset = ReadBE32(fsysData + entryTableOffset + i * 4u);
        u32 nameOffset;
        u32 motionCount;
        const char* memberName;
        PCPortLocomotionSuggestion s;

        if (entryOffset + 0x28u > fsysSize) {
            continue;
        }
        nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
        if (nameOffset >= fsysSize) {
            continue;
        }
        memberName = (const char*)(fsysData + nameOffset);
        memset(&s, 0, sizeof(s));
        {
            int oldQuiet = g_charAnimQuietSetup;
            g_charAnimQuietSetup = 1;
            motionCount = PCPort_CharAnimCountMotionBank(fsysPath, memberName);
            g_charAnimQuietSetup = oldQuiet;
        }
        if (motionCount == 0u) {
            continue;
        }
        if (!PCPort_CharAnimSuggestLocomotionMapEx(fsysPath, memberName,
                                                   frames, &s)) {
            ++probed;
            printf("[motion-batch] %-18s %-24s motions=%2u cyclic=%2u "
                   "varyCyclic=%2u idle=%2d walk=%2d run=%2d confirmed=%s "
                   "energy=%.4f/%.4f/%.4f\n",
                   PCPort_PathBaseName(fsysPath), memberName,
                   motionCount, 0u, 0u, -1, -1, -1, "no",
                   0.0f, 0.0f, 0.0f);
            fflush(stdout);
            continue;
        }

        ++probed;
        if (s.allConfirmed) {
            ++confirmed;
        }
        printf("[motion-batch] %-18s %-24s motions=%2u cyclic=%2u "
               "varyCyclic=%2u idle=%2d walk=%2d run=%2d confirmed=%s "
               "energy=%.4f/%.4f/%.4f\n",
               PCPort_PathBaseName(fsysPath), memberName,
               s.motionCount, s.cyclicCount, s.varyingCyclicCount,
               s.idle, s.walk, s.run, s.allConfirmed ? "yes" : "no",
               s.idleEnergy, s.walkEnergy, s.runEnergy);
        fflush(stdout);
    }
    printf("[motion-batch] archive=%s probed=%u confirmed=%u\n",
           fsysPath, probed, confirmed);
    fflush(stdout);
    free(fsysData);
}

void PCPort_MotionBatchProbe(int frames) {
    static const char* const archives[] = {
        "orig/GC6E01/disc/files/chara_big.fsys",
        "orig/GC6E01/disc/files/chara_small.fsys",
        "orig/GC6E01/disc/files/field_common.fsys",
        "orig/GC6E01/disc/files/people_archive.fsys"
    };
    int i;
    if (frames <= 1) {
        frames = 40;
    }
    printf("[motion-batch] archive member motions cyclic varyCyclic "
           "idle walk run confirmed energy(idle/walk/run)\n");
    fflush(stdout);
    for (i = 0; i < (int)(sizeof(archives) / sizeof(archives[0])); ++i) {
        PCPort_MotionBatchProbeArchive(archives[i], frames);
    }
}

static void PCPort_HeadlessMotionBatchProbeArchive(const char* fsysPath,
                                                   int frames) {
    u8* fsysData;
    u32 fsysSize = 0u;
    u32 entryCount;
    u32 stringTableOffset;
    u32 entryTableOffset;
    u32 i;
    u32 probed = 0u;
    u32 confirmed = 0u;
    BOOL debugFieldCommon = strcmp(PCPort_PathBaseName(fsysPath),
                                   "field_common.fsys") == 0;
    BOOL debugPeopleArchive = strcmp(PCPort_PathBaseName(fsysPath),
                                     "people_archive.fsys") == 0;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        printf("[headless-motion-batch] cannot open %s\n", fsysPath);
        fflush(stdout);
        return;
    }
    if (fsysSize < 0x20u || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        printf("[headless-motion-batch] %s is not an FSYS archive\n",
               fsysPath);
        fflush(stdout);
        free(fsysData);
        return;
    }
    printf("[headless-motion-batch] begin archive=%s\n", fsysPath);
    fflush(stdout);
    if (PCPort_IsKnownNonAnimatedCharacterArchive(fsysPath)) {
        printf("[headless-motion-batch] archive=%s probed=0 confirmed=0 "
               "note=non-animated-assets\n", fsysPath);
        fflush(stdout);
        free(fsysData);
        return;
    }

    entryCount = ReadBE32(fsysData + 0x08);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (stringTableOffset + 4u > fsysSize) {
        free(fsysData);
        return;
    }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        free(fsysData);
        return;
    }
    if (debugFieldCommon) {
        printf("[headless-motion-batch-debug] field_common count08=%u "
               "count0c=%u entryTable=0x%X frames=%d\n",
               ReadBE32(fsysData + 0x08), ReadBE32(fsysData + 0x0C),
               entryTableOffset, frames);
        fflush(stdout);
    }
    for (i = 0u; i < entryCount; ++i) {
        u32 entryOffset;
        u32 nameOffset;
        const char* memberName;
        u8* memberData;
        u32 memberSize = 0u;
        u32 preflightResourceOff = 0u;
        u32 preflightMotionCount = 0u;
        PCPortMotionProbeStats stats[64];
        PCPortLocomotionSuggestion s;
        u32 motionCount = 0u;

        if (entryTableOffset + i * 4u + 4u > fsysSize) {
            break;
        }
        entryOffset = ReadBE32(fsysData + entryTableOffset + i * 4u);
        if (debugFieldCommon ||
            (debugPeopleArchive && i >= 112u && i <= 122u)) {
            printf("[headless-motion-batch-debug] %s entry[%u] ptr=0x%X\n",
                   PCPort_PathBaseName(fsysPath), i, entryOffset);
            fflush(stdout);
        }
        if (entryOffset == 0u) {
            if (debugFieldCommon) {
                printf("[headless-motion-batch-debug] field_common entry[%u] "
                       "zero stop\n", i);
                fflush(stdout);
            }
            break;
        }
        if (!PCPort_IsPlausibleFsysEntry(fsysData, fsysSize, entryOffset)) {
            if (debugFieldCommon) {
                printf("[headless-motion-batch-debug] field_common entry[%u] "
                       "not-plausible stop\n", i);
                fflush(stdout);
                break;
            }
            continue;
        }
        nameOffset = ReadBE32(fsysData + entryOffset + 0x24);
        if (nameOffset >= fsysSize) {
            if (debugFieldCommon) {
                printf("[headless-motion-batch-debug] field_common entry[%u] "
                       "bad-name stop\n", i);
                fflush(stdout);
                break;
            }
            continue;
        }
        memberName = (const char*)(fsysData + nameOffset);
        if (debugFieldCommon ||
            (debugPeopleArchive && i >= 112u && i <= 122u)) {
            printf("[headless-motion-batch-debug] %s entry[%u] "
                   "name=%s data=0x%X decomp=0x%X\n",
                   PCPort_PathBaseName(fsysPath), i, memberName,
                   ReadBE32(fsysData + entryOffset + 0x04),
                   ReadBE32(fsysData + entryOffset + 0x08));
            fflush(stdout);
        }

        memberData = DecompressMemberAt(fsysData, fsysSize, entryOffset,
                                        &memberSize);
        if (memberData == NULL) {
            if (debugFieldCommon) {
                printf("[headless-motion-batch-debug] field_common entry[%u] "
                       "decompress-failed stop\n", i);
                fflush(stdout);
                break;
            }
            continue;
        }
        preflightMotionCount = PCPort_HeadlessMotionPreflightMemberData(
            memberData, memberSize, &preflightResourceOff);
        if (debugFieldCommon ||
            (debugPeopleArchive && i >= 112u && i <= 122u)) {
            printf("[headless-motion-batch-debug] %s entry[%u] "
                   "memberSize=0x%X preflightResource=0x%X "
                   "preflightMotions=%u\n",
                   PCPort_PathBaseName(fsysPath), i, memberSize,
                   preflightResourceOff, preflightMotionCount);
            fflush(stdout);
        }
        if (preflightMotionCount == 0u ||
            ((debugFieldCommon || debugPeopleArchive) &&
             preflightMotionCount < 5u)) {
            free(memberData);
            if (debugFieldCommon ||
                (debugPeopleArchive && i >= 112u && i <= 122u)) {
                printf("[headless-motion-batch-debug] %s entry[%u] %s skip\n",
                       PCPort_PathBaseName(fsysPath), i,
                       preflightMotionCount == 0u ? "non-motion" :
                                                    "not-locomotion-candidate");
                fflush(stdout);
            }
            continue;
        }
        if (debugFieldCommon ||
            (debugPeopleArchive && i >= 112u && i <= 122u)) {
            printf("[headless-motion-batch-debug] %s entry[%u] collect-start\n",
                   PCPort_PathBaseName(fsysPath), i);
            fflush(stdout);
        }

        if (!PCPort_HeadlessMotionCollectMemberDataStats(
                fsysPath, memberName, memberData, memberSize, frames, stats,
                &motionCount, 0)) {
            if (debugFieldCommon ||
                (debugPeopleArchive && i >= 112u && i <= 122u)) {
                printf("[headless-motion-batch-debug] %s entry[%u] "
                       "collect-failed continue\n",
                       PCPort_PathBaseName(fsysPath), i);
                fflush(stdout);
            }
            continue;
        }
        if (debugFieldCommon ||
            (debugPeopleArchive && i >= 112u && i <= 122u)) {
            printf("[headless-motion-batch-debug] %s entry[%u] "
                   "collect-ok motions=%u\n",
                   PCPort_PathBaseName(fsysPath), i, motionCount);
            fflush(stdout);
        }

        memset(&s, 0, sizeof(s));
        ++probed;
        if (PCPort_SelectLocomotionSuggestionFromStats(stats, motionCount, &s)) {
            u32 varyingCount =
                PCPort_MotionProbeVaryingMotionCount(stats, motionCount);
            BOOL animationConfirmed = varyingCount > 0u;
            if (animationConfirmed) {
                ++confirmed;
            }
            printf("[headless-motion-batch] %-18s %-24s motions=%2u "
                   "cyclic=%2u varyCyclic=%2u idle=%2d walk=%2d run=%2d "
                   "confirmed=%s energy=%.4f/%.4f/%.4f\n",
                   PCPort_PathBaseName(fsysPath), memberName,
                   s.motionCount, s.cyclicCount, s.varyingCyclicCount,
                   s.idle, s.walk, s.run,
                   animationConfirmed ? "yes" : "no",
                   s.idleEnergy, s.walkEnergy, s.runEnergy);
        } else {
            u32 cyclicCount = PCPort_MotionProbeCyclicCount(stats, motionCount);
            u32 varyingCyclicCount =
                PCPort_MotionProbeConfirmedCyclicCount(stats, motionCount);
            u32 varyingCount =
                PCPort_MotionProbeVaryingMotionCount(stats, motionCount);
            BOOL animationConfirmed = varyingCount > 0u;
            if (animationConfirmed) {
                ++confirmed;
            }
            printf("[headless-motion-batch] %-18s %-24s motions=%2u "
                   "cyclic=%2u varyCyclic=%2u idle=%2d walk=%2d run=%2d "
                   "confirmed=%s energy=0.0000/0.0000/0.0000 note=%s\n",
                   PCPort_PathBaseName(fsysPath), memberName,
                   motionCount, cyclicCount, varyingCyclicCount, -1, -1, -1,
                   animationConfirmed ? "yes" : "no",
                   varyingCount > 0u ? "animates-no-locomotion" :
                                        "no-varying-motion");
        }
        fflush(stdout);
    }

    printf("[headless-motion-batch] archive=%s probed=%u confirmed=%u\n",
           fsysPath, probed, confirmed);
    fflush(stdout);
    free(fsysData);
}

static void PCPort_HeadlessMotionBatchProbePkxArchive(const char* fsysPath,
                                                      int frames) {
    u8* fsysData;
    u32 fsysSize = 0u;
    u32 entryCount;
    u32 stringTableOffset;
    u32 entryTableOffset;
    u32 i;
    u32 bestEntryOffset = 0u;
    u32 bestSize = 0u;
    const char* memberName;
    u8* memberData;
    u32 memberSize = 0u;
    u32 preflightResourceOff = 0u;
    u32 preflightMotionCount = 0u;
    PCPortMotionProbeStats stats[64];
    u32 motionCount = 0u;
    u32 varyingCount;
    u32 cyclicCount;
    u32 varyingCyclicCount;

    fsysData = LoadFileBytes(fsysPath, &fsysSize);
    if (fsysData == NULL) {
        printf("[headless-motion-pkx] %-18s %-24s load=open-failed\n",
               PCPort_PathBaseName(fsysPath), "");
        fflush(stdout);
        return;
    }
    if (fsysSize < 0x20u || ReadBE32(fsysData) != PCPORT_FSYS_MAGIC) {
        printf("[headless-motion-pkx] %-18s %-24s load=not-fsys\n",
               PCPort_PathBaseName(fsysPath), "");
        fflush(stdout);
        free(fsysData);
        return;
    }

    entryCount = ReadBE32(fsysData + 0x0C);
    stringTableOffset = ReadBE32(fsysData + 0x18);
    if (entryCount == 0u || stringTableOffset + 4u > fsysSize) {
        printf("[headless-motion-pkx] %-18s %-24s load=no-entry-table\n",
               PCPort_PathBaseName(fsysPath), "");
        fflush(stdout);
        free(fsysData);
        return;
    }
    entryTableOffset = ReadBE32(fsysData + stringTableOffset);
    if (entryTableOffset >= fsysSize) {
        printf("[headless-motion-pkx] %-18s %-24s load=no-entry-table\n",
               PCPort_PathBaseName(fsysPath), "");
        fflush(stdout);
        free(fsysData);
        return;
    }

    for (i = 0u; i < entryCount; ++i) {
        u32 entryOffset;
        u32 size;
        if (entryTableOffset + i * 4u + 4u > fsysSize) {
            break;
        }
        entryOffset = ReadBE32(fsysData + entryTableOffset + i * 4u);
        if (!PCPort_IsPlausibleFsysEntry(fsysData, fsysSize, entryOffset)) {
            continue;
        }
        size = ReadBE32(fsysData + entryOffset + 0x08);
        if (bestEntryOffset == 0u || size > bestSize) {
            bestEntryOffset = entryOffset;
            bestSize = size;
        }
    }
    if (bestEntryOffset == 0u) {
        printf("[headless-motion-pkx] %-18s %-24s load=no-model-entry\n",
               PCPort_PathBaseName(fsysPath), "");
        fflush(stdout);
        free(fsysData);
        return;
    }

    memberName = (const char*)(fsysData + ReadBE32(fsysData + bestEntryOffset + 0x24));
    memberData = DecompressMemberAt(fsysData, fsysSize, bestEntryOffset,
                                    &memberSize);
    if (memberData == NULL) {
        printf("[headless-motion-pkx] %-18s %-24s load=decompress-failed\n",
               PCPort_PathBaseName(fsysPath), memberName);
        fflush(stdout);
        free(fsysData);
        return;
    }

    printf("[headless-motion-pkx-debug] %-18s %-24s memberSize=0x%X "
           "preflight-start\n",
           PCPort_PathBaseName(fsysPath), memberName, memberSize);
    fflush(stdout);
    preflightMotionCount = PCPort_HeadlessMotionPreflightMemberData(
        memberData, memberSize, &preflightResourceOff);
    printf("[headless-motion-pkx-debug] %-18s %-24s preflightResource=0x%X "
           "preflightMotions=%u\n",
           PCPort_PathBaseName(fsysPath), memberName, preflightResourceOff,
           preflightMotionCount);
    fflush(stdout);
    if (preflightMotionCount < 2u) {
        BOOL acceptedPlaceholder =
            preflightMotionCount == 1u &&
            strcmp(PCPort_PathBaseName(fsysPath), "pkx_egg.fsys") == 0 &&
            strcmp(memberName, "egg") == 0;
        printf("[headless-motion-pkx] %-18s %-24s motions=%2u varying=%2u "
               "cyclic=%2u varyCyclic=%2u animates=%s confirmed=%s "
               "note=%s%s\n",
               PCPort_PathBaseName(fsysPath), memberName,
               preflightMotionCount, 0u, 0u, 0u,
               acceptedPlaceholder ? "yes" : "no",
               acceptedPlaceholder ? "yes" : "no",
               preflightMotionCount == 0u ? "preflight-no-motion-bank" :
                                             "low-motion-placeholder",
               acceptedPlaceholder ? " policy=accepted-static-placeholder" :
                                     "");
        fflush(stdout);
        free(memberData);
        free(fsysData);
        return;
    }

    if (!PCPort_HeadlessMotionCollectMemberDataStats(
            fsysPath, memberName, memberData, memberSize, frames, stats,
            &motionCount, 0)) {
        printf("[headless-motion-pkx] %-18s %-24s load=motion-bank-failed "
               "memberSize=0x%X\n",
               PCPort_PathBaseName(fsysPath), memberName, memberSize);
        fflush(stdout);
        free(fsysData);
        return;
    }

    varyingCount = PCPort_MotionProbeVaryingMotionCount(stats, motionCount);
    cyclicCount = PCPort_MotionProbeCyclicCount(stats, motionCount);
    varyingCyclicCount =
        PCPort_MotionProbeConfirmedCyclicCount(stats, motionCount);
    printf("[headless-motion-pkx] %-18s %-24s motions=%2u varying=%2u "
           "cyclic=%2u varyCyclic=%2u animates=%s confirmed=%s\n",
           PCPort_PathBaseName(fsysPath), memberName, motionCount,
           varyingCount, cyclicCount, varyingCyclicCount,
           varyingCount > 0u ? "yes" : "no",
           varyingCount > 0u ? "yes" : "no");
    fflush(stdout);
    free(fsysData);
}

static void PCPort_HeadlessMotionBatchProbePkxArchives(int frames) {
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    HANDLE find;
    const char* dir = "orig/GC6E01/disc/files";
    char pattern[320];
    char path[384];

    snprintf(pattern, sizeof(pattern), "%s/pkx_*.fsys", dir);
    find = FindFirstFileA(pattern, &fd);
    if (find == INVALID_HANDLE_VALUE) {
        printf("[headless-motion-batch] no pkx_*.fsys archives found\n");
        fflush(stdout);
        return;
    }
    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0u) {
            continue;
        }
        snprintf(path, sizeof(path), "%s/%s", dir, fd.cFileName);
        PCPort_HeadlessMotionBatchProbePkxArchive(path, frames);
    } while (FindNextFileA(find, &fd));
    FindClose(find);
#else
    (void)frames;
    printf("[headless-motion-batch] pkx_*.fsys glob skipped "
           "(non-Windows host)\n");
    fflush(stdout);
#endif
}

typedef struct PCPortBattleProbeActor {
    const char* label;
    const char* member;
    const char* displayName;
    u16 trainerId;
    u8 teamSlot;
    u16 speciesId;
    u8 level;
    u16 tableMoves[4];
    int tableTeamResolved;
    int allowTableActorDerive;
    char fsysPath[320];
    f32 x;
    f32 y;
    f32 z;
    PCPortMotionProbeStats stats[64];
    u32 motionCount;
    u32 varyingCount;
    int loaded;
    int stanceMotion;
    int attackMotion;
    int damageMotion;
} PCPortBattleProbeActor;

typedef struct PCPortBattleProbeActorDefault {
    const char* label;
    const char* envName;
    const char* member;
    const char* displayName;
    u16 trainerId;
    u8 teamSlot;
    u16 speciesId;
    u8 level;
    f32 x;
    f32 y;
    f32 z;
} PCPortBattleProbeActorDefault;

typedef struct PCPortBattleProbeMoveScript {
    const char* moveEnv;
    const char* moveIdEnv;
    const char* textIdEnv;
    const char* damageEnv;
    const char* defaultMove;
    u16 moveId;
    u16 textId;
    int attacker;
    int target;
    int defaultDamage;
} PCPortBattleProbeMoveScript;

typedef struct PCPortBattleProbeSpeciesMember {
    u16 speciesId;
    const char* member;
    const char* displayName;
} PCPortBattleProbeSpeciesMember;

#define PCPORT_COMMON_REL_POKEMON_STATS_OFFSET   0x12336Cu
#define PCPORT_COMMON_REL_MOVE_DATA_OFFSET       0x11E048u
#define PCPORT_COMMON_REL_TRAINER_DATA_OFFSET    0x092ED0u
#define PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET 0x09FE28u
#define PCPORT_COMMON_REL_POKEMON_STATS_SIZE     0x11Cu
#define PCPORT_COMMON_REL_MOVE_DATA_SIZE         0x38u
#define PCPORT_COMMON_REL_TRAINER_DATA_SIZE      0x34u
#define PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE   0x50u
#define PCPORT_COMMON_REL_MAX_TRAINERS           819u
#define PCPORT_COMMON_REL_MAX_TRAINER_POKEMON    5510u

static const PCPortBattleProbeActorDefault
    kBattleProbeDefaultActors[4] = {
        { "player-left",  "PCPORT_BATTLE_P0", "eifie",   "Eifie",
          0x0001u, 0u, 196u, 25u,
          -1.35f, 0.0f,  1.10f },
        { "player-right", "PCPORT_BATTLE_P1", "blacky",  "Blacky",
          0x0001u, 1u, 197u, 26u,
           1.35f, 0.0f,  1.10f },
        { "enemy-left",   "PCPORT_BATTLE_E0", "absol",   "Absol",
          0x0200u, 0u, 359u, 50u,
          -1.35f, 0.0f, -1.35f },
        { "enemy-right",  "PCPORT_BATTLE_E1", "pikachu", "Pikachu",
          0x0200u, 1u, 25u, 50u,
           1.35f, 0.0f, -1.35f }
    };

static const PCPortBattleProbeMoveScript kBattleProbeDefaultMoves[2] = {
    { "PCPORT_BATTLE_PLAYER_MOVE", "PCPORT_BATTLE_PLAYER_MOVE_ID",
      "PCPORT_BATTLE_PLAYER_TEXT_ID", "PCPORT_BATTLE_PLAYER_DAMAGE",
      "Swift", 129u, 0x8001u, 0, 2, 32 },
    { "PCPORT_BATTLE_ENEMY_MOVE", "PCPORT_BATTLE_ENEMY_MOVE_ID",
      "PCPORT_BATTLE_ENEMY_TEXT_ID", "PCPORT_BATTLE_ENEMY_DAMAGE",
      "Bite", 44u, 0x8002u, 2, 0, 21 }
};

static const PCPortBattleProbeSpeciesMember
    kBattleProbeSpeciesMembers[] = {
        { 25u,  "pikachu",   "Pikachu" },
        { 196u, "eifie",     "Eifie" },
        { 197u, "blacky",    "Blacky" },
        { 292u, "nukenin",   "Nukenin" },
        { 316u, "gokulin",   "Gokulin" },
        { 317u, "marunoom",  "Marunoom" },
        { 335u, "zangoose",  "Zangoose" },
        { 336u, "habunake",  "Habunake" },
        { 359u, "absol",     "Absol" }
    };

static f32 PCPort_BattleProbeMotionScore(const PCPortMotionProbeStats* s) {
    if (s == NULL || !s->valid) {
        return -1.0f;
    }
    return PCPort_MotionProbeEnergy(s);
}

static int PCPort_BattleProbeFirstValidMotion(
    const PCPortMotionProbeStats* stats,
    u32 motionCount) {
    u32 i;
    if (stats == NULL) {
        return -1;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        if (stats[i].valid) {
            return (int)i;
        }
    }
    return -1;
}

static int PCPort_BattleProbeBestVaryingMotion(
    const PCPortMotionProbeStats* stats,
    u32 motionCount,
    int excludeMotion,
    int highEnergy) {
    u32 i;
    int best = -1;
    f32 bestScore = highEnergy ? -1.0f : 1.0e30f;

    if (stats == NULL) {
        return -1;
    }
    for (i = 0u; i < motionCount && i < 64u; ++i) {
        f32 score;
        if (!stats[i].valid || stats[i].varyingFrames <= 0 ||
            (int)i == excludeMotion) {
            continue;
        }
        score = PCPort_BattleProbeMotionScore(&stats[i]);
        if ((highEnergy && score > bestScore) ||
            (!highEnergy && score < bestScore)) {
            best = (int)i;
            bestScore = score;
        }
    }
    return best;
}

static const PCPortMotionProbeStats* PCPort_BattleProbeStatsForMotion(
    const PCPortBattleProbeActor* actor,
    int motion) {
    if (actor == NULL || motion < 0 || motion >= (int)actor->motionCount ||
        motion >= 64) {
        return NULL;
    }
    if (!actor->stats[motion].valid) {
        return NULL;
    }
    return &actor->stats[motion];
}

static void PCPort_BattleProbeDescribeMotion(
    const PCPortBattleProbeActor* actor,
    const char* role,
    int motion) {
    const PCPortMotionProbeStats* s =
        PCPort_BattleProbeStatsForMotion(actor, motion);
    if (s == NULL) {
        printf("%s=-1", role);
        return;
    }
    printf("%s=%d(kind=%s,end=%.1f,energy=%.4f,delta=%.4f)",
           role, motion, s->kind != NULL ? s->kind : "?",
           s->endFrame, PCPort_MotionProbeEnergy(s),
           s->frameDeltaSum);
}

static void PCPort_BattleProbeInitActor(PCPortBattleProbeActor* actor,
                                        const PCPortBattleProbeActorDefault* d) {
    const char* envValue;

    if (actor == NULL || d == NULL) {
        return;
    }
    envValue = getenv(d->envName);

    memset(actor, 0, sizeof(*actor));
    actor->label = d->label;
    actor->member = (envValue != NULL && envValue[0] != '\0') ?
                    envValue : d->member;
    actor->displayName = (envValue != NULL && envValue[0] != '\0') ?
                         envValue : d->displayName;
    actor->trainerId = d->trainerId;
    actor->teamSlot = d->teamSlot;
    actor->speciesId = d->speciesId;
    actor->level = d->level;
    actor->allowTableActorDerive = (envValue == NULL || envValue[0] == '\0');
    actor->x = d->x;
    actor->y = d->y;
    actor->z = d->z;
    actor->stanceMotion = -1;
    actor->attackMotion = -1;
    actor->damageMotion = -1;
    snprintf(actor->fsysPath, sizeof(actor->fsysPath),
             "orig/GC6E01/disc/files/pkx_%s.fsys", actor->member);
}

static void PCPort_BattleProbeLoadActor(PCPortBattleProbeActor* actor,
                                        int frames) {
    int fallback;

    if (actor == NULL) {
        return;
    }
    actor->loaded = PCPort_HeadlessMotionCollectMemberStats(
        actor->fsysPath, actor->member, frames, actor->stats,
        &actor->motionCount, 0);
    actor->varyingCount =
        PCPort_MotionProbeVaryingMotionCount(actor->stats, actor->motionCount);

    if (!actor->loaded) {
        printf("[battle-probe] actor=%s member=%s load=failed path=%s\n",
               actor->label, actor->member, actor->fsysPath);
        fflush(stdout);
        return;
    }

    actor->stanceMotion =
        PCPort_BattleProbeFirstValidMotion(actor->stats, actor->motionCount);
    actor->attackMotion =
        PCPort_BattleProbeBestVaryingMotion(actor->stats, actor->motionCount,
                                            -1, 1);
    actor->damageMotion =
        PCPort_BattleProbeBestVaryingMotion(actor->stats, actor->motionCount,
                                            actor->attackMotion, 0);
    fallback = actor->stanceMotion;
    if (actor->attackMotion < 0) actor->attackMotion = fallback;
    if (actor->damageMotion < 0) actor->damageMotion = fallback;

    printf("[battle-probe] actor=%s member=%-8s trainer=0x%04X team=%u "
           "species=%u level=%u motions=%2u varying=%2u "
           "pos=(%.2f,%.2f,%.2f) ",
           actor->label, actor->member, actor->trainerId, actor->teamSlot,
           actor->speciesId, actor->level, actor->motionCount,
           actor->varyingCount, actor->x, actor->y, actor->z);
    PCPort_BattleProbeDescribeMotion(actor, "stance", actor->stanceMotion);
    printf(" ");
    PCPort_BattleProbeDescribeMotion(actor, "attack", actor->attackMotion);
    printf(" ");
    PCPort_BattleProbeDescribeMotion(actor, "damage", actor->damageMotion);
    printf("\n");
    fflush(stdout);
}

static void PCPort_BattleProbePrintAction(const char* state,
                                          const PCPortBattleProbeActor* actor,
                                          const char* text,
                                          int motion) {
    const PCPortMotionProbeStats* s =
        PCPort_BattleProbeStatsForMotion(actor, motion);
    printf("[battle-probe-turn] state=%s actor=%s motion=%d text=\"%s\"",
           state, actor != NULL ? actor->label : "-", motion,
           text != NULL ? text : "");
    if (s != NULL) {
        printf(" kind=%s first=%.4f last=%.4f range=%.4f energy=%.4f",
               s->kind != NULL ? s->kind : "?",
               s->checksumFirst, s->checksumLast, s->checksumRange,
               PCPort_MotionProbeEnergy(s));
    }
    printf("\n");
    fflush(stdout);
}

static const char* PCPort_BattleProbeEnvText(const char* envName,
                                             const char* fallback) {
    const char* v = getenv(envName);
    return (v != NULL && v[0] != '\0') ? v : fallback;
}

static int PCPort_BattleProbeEnvInt(const char* envName, int fallback) {
    const char* v = getenv(envName);
    char* end = NULL;
    long parsed;
    if (v == NULL || v[0] == '\0') {
        return fallback;
    }
    parsed = strtol(v, &end, 0);
    return (end != v) ? (int)parsed : fallback;
}

static int PCPort_BattleProbeRangeValid(u32 base, u32 stride, u32 index,
                                        u32 need, u32 size) {
    u32 off;
    if (stride != 0u && index > (0xFFFFFFFFu - base) / stride) {
        return 0;
    }
    off = base + index * stride;
    return off <= size && need <= size - off;
}

static void PCPort_BattleProbePrintCommonRelMove(const u8* commonRel,
                                                 u32 commonRelSize,
                                                 u16 moveId,
                                                 const char* label) {
    u32 moveIndex = moveId > 0u ? (u32)moveId - 1u : 0u;
    u32 off;

    if (!PCPort_BattleProbeRangeValid(PCPORT_COMMON_REL_MOVE_DATA_OFFSET,
                                      PCPORT_COMMON_REL_MOVE_DATA_SIZE,
                                      moveIndex,
                                      PCPORT_COMMON_REL_MOVE_DATA_SIZE,
                                      commonRelSize)) {
        printf("[battle-probe-table] move label=%s moveId=%u load=oob\n",
               label, moveId);
        return;
    }
    off = PCPORT_COMMON_REL_MOVE_DATA_OFFSET +
          moveIndex * PCPORT_COMMON_REL_MOVE_DATA_SIZE;
    printf("[battle-probe-table] move label=%s moveId=%u tableIndex=%u "
           "priority=%u pp=%u type=%u accuracy=%u power=%u effect=%u "
           "nameText=0x%04X descText=0x%04X anim=%u\n",
           label, moveId, moveIndex,
           commonRel[off + 0x00], commonRel[off + 0x01],
           commonRel[off + 0x02], commonRel[off + 0x04],
           commonRel[off + 0x17], commonRel[off + 0x1B],
           ReadBE16(commonRel + off + 0x22),
           ReadBE16(commonRel + off + 0x2E),
           ReadBE16(commonRel + off + 0x32));
}

static void PCPort_BattleProbePrintCommonRelPokemon(const u8* commonRel,
                                                    u32 commonRelSize,
                                                    const PCPortBattleProbeActor* actor) {
    u32 speciesIndex;
    u32 off;

    if (actor == NULL) {
        return;
    }
    speciesIndex = actor->speciesId > 0u ? (u32)actor->speciesId - 1u : 0u;
    if (!PCPort_BattleProbeRangeValid(PCPORT_COMMON_REL_POKEMON_STATS_OFFSET,
                                      PCPORT_COMMON_REL_POKEMON_STATS_SIZE,
                                      speciesIndex,
                                      PCPORT_COMMON_REL_POKEMON_STATS_SIZE,
                                      commonRelSize)) {
        printf("[battle-probe-table] pokemon actor=%s species=%u load=oob\n",
               actor->label, actor->speciesId);
        return;
    }
    off = PCPORT_COMMON_REL_POKEMON_STATS_OFFSET +
          speciesIndex * PCPORT_COMMON_REL_POKEMON_STATS_SIZE;
    printf("[battle-probe-table] pokemon actor=%s species=%u tableIndex=%u "
           "nameText=0x%04X model=%u type=%u/%u base=%u/%u/%u/%u/%u/%u\n",
           actor->label, actor->speciesId, speciesIndex,
           ReadBE16(commonRel + off + 0x1A),
           ReadBE16(commonRel + off + 0x2E),
           commonRel[off + 0x30], commonRel[off + 0x31],
           commonRel[off + 0x85], commonRel[off + 0x87],
           commonRel[off + 0x89], commonRel[off + 0x8B],
           commonRel[off + 0x8D], commonRel[off + 0x8F]);
}

static void PCPort_BattleProbePrintCommonRelTrainer(const u8* commonRel,
                                                    u32 commonRelSize,
                                                    const PCPortBattleProbeActor* actor) {
    u32 off;
    u16 firstPokemon;
    u32 pokeOff;

    if (actor == NULL) {
        return;
    }
    if (!PCPort_BattleProbeRangeValid(PCPORT_COMMON_REL_TRAINER_DATA_OFFSET,
                                      PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                      actor->trainerId,
                                      PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                      commonRelSize)) {
        printf("[battle-probe-table] trainer actor=%s trainer=0x%04X load=oob\n",
               actor->label, actor->trainerId);
        return;
    }
    off = PCPORT_COMMON_REL_TRAINER_DATA_OFFSET +
          (u32)actor->trainerId * PCPORT_COMMON_REL_TRAINER_DATA_SIZE;
    firstPokemon = ReadBE16(commonRel + off + 0x04);
    printf("[battle-probe-table] trainer actor=%s trainer=0x%04X class=%u "
           "firstPokemon=%u nameText=0x%08X pre=0x%08X win=0x%08X "
           "lose=0x%08X\n",
           actor->label, actor->trainerId, commonRel[off + 0x03],
           firstPokemon, ReadBE32(commonRel + off + 0x08),
           ReadBE32(commonRel + off + 0x24),
           ReadBE32(commonRel + off + 0x28),
           ReadBE32(commonRel + off + 0x2C));
    if (!PCPort_BattleProbeRangeValid(
            PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET,
            PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
            (u32)firstPokemon + (u32)actor->teamSlot,
            PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
            commonRelSize)) {
        return;
    }
    pokeOff = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              ((u32)firstPokemon + (u32)actor->teamSlot) *
                  PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    printf("[battle-probe-table] trainer-pokemon actor=%s tableIndex=%u "
           "species=%u level=%u shadow=%u moves=%u/%u/%u/%u\n",
           actor->label, (u32)firstPokemon + (u32)actor->teamSlot,
           ReadBE16(commonRel + pokeOff + 0x0A),
           commonRel[pokeOff + 0x04], commonRel[pokeOff + 0x03],
           ReadBE16(commonRel + pokeOff + 0x34),
           ReadBE16(commonRel + pokeOff + 0x36),
           ReadBE16(commonRel + pokeOff + 0x38),
           ReadBE16(commonRel + pokeOff + 0x3A));
}

static int PCPort_BattleProbeTrainerPokemonValid(const u8* commonRel,
                                                 u32 commonRelSize,
                                                 u32 index) {
    return index < PCPORT_COMMON_REL_MAX_TRAINER_POKEMON &&
           PCPort_BattleProbeRangeValid(
               PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET,
               PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
               index,
               PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE,
               commonRelSize);
}

static u16 PCPort_BattleProbeTrainerPokemonSpecies(const u8* commonRel,
                                                   u32 index) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    return ReadBE16(commonRel + off + 0x0A);
}

static u8 PCPort_BattleProbeTrainerPokemonLevel(const u8* commonRel,
                                                u32 index) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    return commonRel[off + 0x04];
}

static u16 PCPort_BattleProbeTrainerPokemonMove(const u8* commonRel,
                                                u32 index,
                                                u32 moveSlot) {
    u32 off = PCPORT_COMMON_REL_TRAINER_POKEMON_OFFSET +
              index * PCPORT_COMMON_REL_TRAINER_POKEMON_SIZE;
    if (moveSlot >= 4u) {
        return 0u;
    }
    return ReadBE16(commonRel + off + 0x34 + moveSlot * 2u);
}

static u16 PCPort_BattleProbeFirstUsableTableMove(
    const PCPortBattleProbeActor* actor) {
    u32 i;
    if (actor == NULL) {
        return 0u;
    }
    for (i = 0u; i < 4u; ++i) {
        u16 move = actor->tableMoves[i];
        if (move != 0u && move != 0xFFFFu && move < 512u) {
            return move;
        }
    }
    return 0u;
}

static const PCPortBattleProbeSpeciesMember*
PCPort_BattleProbeFindSpeciesMember(u16 speciesId) {
    u32 i;
    for (i = 0u; i < sizeof(kBattleProbeSpeciesMembers) /
                    sizeof(kBattleProbeSpeciesMembers[0]); ++i) {
        if (kBattleProbeSpeciesMembers[i].speciesId == speciesId) {
            return &kBattleProbeSpeciesMembers[i];
        }
    }
    return NULL;
}

static int PCPort_BattleProbeTrainerFirstPokemon(
    const u8* commonRel,
    u32 commonRelSize,
    u16 trainerId,
    u16* outFirstPokemon) {
    u32 off;
    if (outFirstPokemon == NULL) {
        return 0;
    }
    if (!PCPort_BattleProbeRangeValid(PCPORT_COMMON_REL_TRAINER_DATA_OFFSET,
                                      PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                      trainerId,
                                      PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                      commonRelSize)) {
        return 0;
    }
    off = PCPORT_COMMON_REL_TRAINER_DATA_OFFSET +
          (u32)trainerId * PCPORT_COMMON_REL_TRAINER_DATA_SIZE;
    *outFirstPokemon = ReadBE16(commonRel + off + 0x04);
    return 1;
}

static int PCPort_BattleProbeFindTrainerForFirstPokemon(
    const u8* commonRel,
    u32 commonRelSize,
    u32 firstPokemon,
    u16* outTrainerId) {
    u32 trainer;
    if (outTrainerId == NULL) {
        return 0;
    }
    for (trainer = 0u; trainer < PCPORT_COMMON_REL_MAX_TRAINERS; ++trainer) {
        u32 off;
        if (!PCPort_BattleProbeRangeValid(PCPORT_COMMON_REL_TRAINER_DATA_OFFSET,
                                          PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                          trainer,
                                          PCPORT_COMMON_REL_TRAINER_DATA_SIZE,
                                          commonRelSize)) {
            break;
        }
        off = PCPORT_COMMON_REL_TRAINER_DATA_OFFSET +
              trainer * PCPORT_COMMON_REL_TRAINER_DATA_SIZE;
        if ((u32)ReadBE16(commonRel + off + 0x04) == firstPokemon) {
            *outTrainerId = (u16)trainer;
            return 1;
        }
    }
    return 0;
}

static int PCPort_BattleProbeFindTeamPair(
    const u8* commonRel,
    u32 commonRelSize,
    u16 species0,
    u16 species1,
    u32* outFirstPokemon,
    u16* outTrainerId) {
    u32 i;
    if (outFirstPokemon == NULL || outTrainerId == NULL) {
        return 0;
    }
    for (i = 0u; i + 1u < PCPORT_COMMON_REL_MAX_TRAINER_POKEMON; ++i) {
        if (!PCPort_BattleProbeTrainerPokemonValid(commonRel, commonRelSize, i) ||
            !PCPort_BattleProbeTrainerPokemonValid(commonRel, commonRelSize, i + 1u)) {
            break;
        }
        if (PCPort_BattleProbeTrainerPokemonSpecies(commonRel, i) == species0 &&
            PCPort_BattleProbeTrainerPokemonSpecies(commonRel, i + 1u) == species1) {
            u16 trainerId = 0u;
            if (PCPort_BattleProbeFindTrainerForFirstPokemon(
                    commonRel, commonRelSize, i, &trainerId)) {
                *outFirstPokemon = i;
                *outTrainerId = trainerId;
                return 1;
            }
        }
    }
    return 0;
}

static int PCPort_BattleProbeApplyTrainerIdTeam(
    const u8* commonRel,
    u32 commonRelSize,
    PCPortBattleProbeActor* actors,
    u32 actorBase,
    const char* sideLabel,
    u16* outMoveId,
    int allowMoveDerive) {
    u16 trainerId;
    u16 firstPokemon;
    u32 slot;
    const PCPortBattleProbeSpeciesMember* members[2];
    u16 species[2];

    if (actors == NULL || actorBase + 1u >= 4u) {
        return 0;
    }
    trainerId = actors[actorBase].trainerId;
    if (!actors[actorBase].allowTableActorDerive ||
        !actors[actorBase + 1u].allowTableActorDerive) {
        printf("[battle-probe-table-derive] side=%s status=actor-env-override "
               "trainer=0x%04X\n", sideLabel, trainerId);
        return 0;
    }
    if (!PCPort_BattleProbeTrainerFirstPokemon(commonRel, commonRelSize,
                                               trainerId, &firstPokemon)) {
        printf("[battle-probe-table-derive] side=%s status=trainer-oob "
               "trainer=0x%04X\n", sideLabel, trainerId);
        return 0;
    }
    for (slot = 0u; slot < 2u; ++slot) {
        u32 tableIndex = (u32)firstPokemon + slot;
        if (!PCPort_BattleProbeTrainerPokemonValid(commonRel, commonRelSize,
                                                   tableIndex)) {
            printf("[battle-probe-table-derive] side=%s status=team-oob "
                   "trainer=0x%04X firstPokemon=%u slot=%u\n",
                   sideLabel, trainerId, firstPokemon, slot);
            return 0;
        }
        species[slot] =
            PCPort_BattleProbeTrainerPokemonSpecies(commonRel, tableIndex);
        members[slot] = PCPort_BattleProbeFindSpeciesMember(species[slot]);
        if (members[slot] == NULL) {
            printf("[battle-probe-table-derive] side=%s status=unmapped-species "
                   "trainer=0x%04X firstPokemon=%u slot=%u species=%u\n",
                   sideLabel, trainerId, firstPokemon, slot, species[slot]);
            return 0;
        }
    }

    for (slot = 0u; slot < 2u; ++slot) {
        PCPortBattleProbeActor* actor = &actors[actorBase + slot];
        u32 tableIndex = (u32)firstPokemon + slot;
        u32 moveSlot;
        actor->trainerId = trainerId;
        actor->teamSlot = (u8)slot;
        actor->speciesId = species[slot];
        actor->member = members[slot]->member;
        actor->displayName = members[slot]->displayName;
        actor->level = PCPort_BattleProbeTrainerPokemonLevel(commonRel,
                                                             tableIndex);
        actor->tableTeamResolved = 1;
        snprintf(actor->fsysPath, sizeof(actor->fsysPath),
                 "orig/GC6E01/disc/files/pkx_%s.fsys", actor->member);
        for (moveSlot = 0u; moveSlot < 4u; ++moveSlot) {
            actor->tableMoves[moveSlot] =
                PCPort_BattleProbeTrainerPokemonMove(commonRel, tableIndex,
                                                     moveSlot);
        }
    }
    if (allowMoveDerive && outMoveId != NULL) {
        u16 derivedMove =
            PCPort_BattleProbeFirstUsableTableMove(&actors[actorBase]);
        if (derivedMove != 0u) {
            *outMoveId = derivedMove;
        }
    }
    printf("[battle-probe-table-derive] side=%s status=trainer-applied "
           "trainer=0x%04X firstPokemon=%u species=%u/%u levels=%u/%u "
           "members=%s/%s moves0=%u/%u/%u/%u\n",
           sideLabel, trainerId, firstPokemon, species[0], species[1],
           actors[actorBase].level, actors[actorBase + 1u].level,
           actors[actorBase].member, actors[actorBase + 1u].member,
           actors[actorBase].tableMoves[0], actors[actorBase].tableMoves[1],
           actors[actorBase].tableMoves[2], actors[actorBase].tableMoves[3]);
    return 1;
}

static void PCPort_BattleProbeApplyDerivedTeam(
    const u8* commonRel,
    u32 commonRelSize,
    PCPortBattleProbeActor* actors,
    u32 actorBase,
    const char* sideLabel,
    u16* outMoveId,
    int allowMoveDerive) {
    u32 firstPokemon = 0u;
    u16 trainerId = 0u;
    u32 slot;

    if (actors == NULL || actorBase + 1u >= 4u) {
        return;
    }
    if (PCPort_BattleProbeApplyTrainerIdTeam(commonRel, commonRelSize,
                                             actors, actorBase, sideLabel,
                                             outMoveId, allowMoveDerive)) {
        return;
    }
    if (!PCPort_BattleProbeFindTeamPair(commonRel, commonRelSize,
                                        actors[actorBase].speciesId,
                                        actors[actorBase + 1u].speciesId,
                                        &firstPokemon, &trainerId)) {
        printf("[battle-probe-table-derive] side=%s status=fallback "
               "species=%u/%u\n", sideLabel,
               actors[actorBase].speciesId,
               actors[actorBase + 1u].speciesId);
        return;
    }

    for (slot = 0u; slot < 2u; ++slot) {
        PCPortBattleProbeActor* actor = &actors[actorBase + slot];
        u32 tableIndex = firstPokemon + slot;
        u32 moveSlot;
        actor->trainerId = trainerId;
        actor->teamSlot = (u8)slot;
        actor->level = PCPort_BattleProbeTrainerPokemonLevel(commonRel,
                                                             tableIndex);
        actor->tableTeamResolved = 1;
        for (moveSlot = 0u; moveSlot < 4u; ++moveSlot) {
            actor->tableMoves[moveSlot] =
                PCPort_BattleProbeTrainerPokemonMove(commonRel, tableIndex,
                                                     moveSlot);
        }
    }
    if (allowMoveDerive && outMoveId != NULL) {
        u16 derivedMove = PCPort_BattleProbeFirstUsableTableMove(
            &actors[actorBase]);
        if (derivedMove != 0u) {
            *outMoveId = derivedMove;
        }
    }
    printf("[battle-probe-table-derive] side=%s status=applied "
           "trainer=0x%04X firstPokemon=%u levels=%u/%u moves0=%u/%u/%u/%u\n",
           sideLabel, trainerId, firstPokemon,
           actors[actorBase].level, actors[actorBase + 1u].level,
           actors[actorBase].tableMoves[0], actors[actorBase].tableMoves[1],
           actors[actorBase].tableMoves[2], actors[actorBase].tableMoves[3]);
}

static void PCPort_BattleProbePrintCommonRelTables(
    PCPortBattleProbeActor actors[4],
    u16* playerMoveId,
    u16* enemyMoveId,
    int allowPlayerMoveDerive,
    int allowEnemyMoveDerive) {
    u8* commonRel = NULL;
    u32 commonRelSize = 0u;
    u32 i;

    if (!PCPort_LoadFsysMember("orig/GC6E01/disc/files/common.fsys",
                               "pcommon_rel", &commonRel, &commonRelSize)) {
        if (!PCPort_LoadFsysMember("orig/GC6E01/disc/files/common.fsys",
                                   "common_rel", &commonRel, &commonRelSize)) {
            printf("[battle-probe-table] common_rel load=failed\n");
            fflush(stdout);
            return;
        }
    }

    printf("[battle-probe-table] common_rel load=ok size=0x%X\n",
           commonRelSize);
    PCPort_BattleProbeApplyDerivedTeam(commonRel, commonRelSize, actors, 0u,
                                       "player", playerMoveId,
                                       allowPlayerMoveDerive);
    PCPort_BattleProbeApplyDerivedTeam(commonRel, commonRelSize, actors, 2u,
                                       "enemy", enemyMoveId,
                                       allowEnemyMoveDerive);
    PCPort_BattleProbePrintCommonRelMove(commonRel, commonRelSize,
                                         playerMoveId != NULL ? *playerMoveId : 0u,
                                         "player");
    PCPort_BattleProbePrintCommonRelMove(commonRel, commonRelSize,
                                         enemyMoveId != NULL ? *enemyMoveId : 0u,
                                         "enemy");
    for (i = 0u; i < 4u; ++i) {
        PCPort_BattleProbePrintCommonRelPokemon(commonRel, commonRelSize,
                                                &actors[i]);
    }
    PCPort_BattleProbePrintCommonRelTrainer(commonRel, commonRelSize,
                                            &actors[0]);
    PCPort_BattleProbePrintCommonRelTrainer(commonRel, commonRelSize,
                                            &actors[2]);
    fflush(stdout);
    PCPort_FreeBuffer(commonRel);
}

static void PCPort_BattleProbeFormatMoveText(char* out,
                                             u32 outSize,
                                             const PCPortBattleProbeActor* actor,
                                             const char* moveName) {
    if (out == NULL || outSize == 0u) {
        return;
    }
    snprintf(out, outSize, "%s used %s!",
             actor != NULL ? actor->displayName : "Pokemon",
             moveName != NULL ? moveName : "Move");
}

static void PCPort_BattleProbeFormatDamageText(
    char* out,
    u32 outSize,
    const PCPortBattleProbeActor* actor) {
    if (out == NULL || outSize == 0u) {
        return;
    }
    if (actor != NULL && strncmp(actor->label, "enemy-", 6) == 0) {
        snprintf(out, outSize, "The opposing %s took damage.",
                 actor->displayName);
    } else {
        snprintf(out, outSize, "%s took damage.",
                 actor != NULL ? actor->displayName : "Pokemon");
    }
}

void PCPort_BattleProbe(int frames) {
    PCPortBattleProbeActor actors[4];
    const char* playerMoveName;
    const char* enemyMoveName;
    u16 playerMoveId;
    u16 enemyMoveId;
    u16 playerTextId;
    u16 enemyTextId;
    int playerDamage;
    int enemyDamage;
    char playerMoveNameAuto[32];
    char enemyMoveNameAuto[32];
    char playerMoveText[96];
    char playerDamageText[96];
    char enemyMoveText[96];
    char enemyDamageText[96];
    u32 loadedCount = 0u;
    u32 i;
    int hpEnemyLeft = 100;
    int hpPlayerLeft = 100;

    if (frames <= 1) {
        frames = 24;
    }

    for (i = 0u; i < 4u; ++i) {
        PCPort_BattleProbeInitActor(&actors[i],
                                    &kBattleProbeDefaultActors[i]);
    }
    actors[0].trainerId = actors[1].trainerId =
        (u16)PCPort_BattleProbeEnvInt("PCPORT_BATTLE_PLAYER_TRAINER",
                                      actors[0].trainerId);
    actors[2].trainerId = actors[3].trainerId =
        (u16)PCPort_BattleProbeEnvInt("PCPORT_BATTLE_ENEMY_TRAINER",
                                      actors[2].trainerId);

    playerMoveName = PCPort_BattleProbeEnvText(
        kBattleProbeDefaultMoves[0].moveEnv,
        kBattleProbeDefaultMoves[0].defaultMove);
    enemyMoveName = PCPort_BattleProbeEnvText(
        kBattleProbeDefaultMoves[1].moveEnv,
        kBattleProbeDefaultMoves[1].defaultMove);
    playerMoveId = (u16)PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[0].moveIdEnv,
        kBattleProbeDefaultMoves[0].moveId);
    enemyMoveId = (u16)PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[1].moveIdEnv,
        kBattleProbeDefaultMoves[1].moveId);
    playerTextId = (u16)PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[0].textIdEnv,
        kBattleProbeDefaultMoves[0].textId);
    enemyTextId = (u16)PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[1].textIdEnv,
        kBattleProbeDefaultMoves[1].textId);
    playerDamage = PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[0].damageEnv,
        kBattleProbeDefaultMoves[0].defaultDamage);
    enemyDamage = PCPort_BattleProbeEnvInt(
        kBattleProbeDefaultMoves[1].damageEnv,
        kBattleProbeDefaultMoves[1].defaultDamage);

    printf("[battle-probe] mode=headless actors=4 frames=%d\n", frames);
    PCPort_BattleProbePrintCommonRelTables(
        actors, &playerMoveId, &enemyMoveId,
        getenv(kBattleProbeDefaultMoves[0].moveIdEnv) == NULL,
        getenv(kBattleProbeDefaultMoves[1].moveIdEnv) == NULL);
    if (getenv(kBattleProbeDefaultMoves[0].moveEnv) == NULL &&
        playerMoveId != kBattleProbeDefaultMoves[0].moveId) {
        snprintf(playerMoveNameAuto, sizeof(playerMoveNameAuto),
                 "Table Move %u", playerMoveId);
        playerMoveName = playerMoveNameAuto;
    }
    if (getenv(kBattleProbeDefaultMoves[1].moveEnv) == NULL &&
        enemyMoveId != kBattleProbeDefaultMoves[1].moveId) {
        snprintf(enemyMoveNameAuto, sizeof(enemyMoveNameAuto),
                 "Table Move %u", enemyMoveId);
        enemyMoveName = enemyMoveNameAuto;
    }
    PCPort_BattleProbeFormatMoveText(playerMoveText, sizeof(playerMoveText),
                                     &actors[kBattleProbeDefaultMoves[0].attacker],
                                     playerMoveName);
    PCPort_BattleProbeFormatDamageText(
        playerDamageText, sizeof(playerDamageText),
        &actors[kBattleProbeDefaultMoves[0].target]);
    PCPort_BattleProbeFormatMoveText(enemyMoveText, sizeof(enemyMoveText),
                                     &actors[kBattleProbeDefaultMoves[1].attacker],
                                     enemyMoveName);
    PCPort_BattleProbeFormatDamageText(
        enemyDamageText, sizeof(enemyDamageText),
        &actors[kBattleProbeDefaultMoves[1].target]);
    printf("[battle-probe-data] playerTrainer=0x%04X enemyTrainer=0x%04X "
           "playerMove=%u enemyMove=%u playerText=0x%04X enemyText=0x%04X\n",
           actors[0].trainerId, actors[2].trainerId,
           playerMoveId, enemyMoveId, playerTextId, enemyTextId);
    printf("[battle-probe-text] \"FIGHT  POKEMON  BAG  RUN\"\n");
    printf("[battle-probe-text] \"%s\"\n", playerMoveText);
    printf("[battle-probe-text] \"%s\"\n", playerDamageText);
    fflush(stdout);

    for (i = 0u; i < 4u; ++i) {
        PCPort_BattleProbeLoadActor(&actors[i], frames);
        if (actors[i].loaded) {
            ++loadedCount;
        }
    }

    printf("[battle-probe] placement player-left=(-1.35,0.00,1.10) "
           "player-right=(1.35,0.00,1.10) enemy-left=(-1.35,0.00,-1.35) "
           "enemy-right=(1.35,0.00,-1.35)\n");
    printf("[battle-probe-turn] state=opening playerHP=%d enemyHP=%d\n",
           hpPlayerLeft, hpEnemyLeft);
    printf("[battle-probe-turn] state=command-menu selected=FIGHT "
           "text=\"FIGHT  POKEMON  BAG  RUN\"\n");
    printf("[battle-probe-turn] state=move-menu selected=\"%s\" moveId=%u "
           "textId=0x%04X target=%s\n", playerMoveName, playerMoveId,
           playerTextId, actors[kBattleProbeDefaultMoves[0].target].label);
    fflush(stdout);

    PCPort_BattleProbePrintAction(
        "player-attack",
        &actors[kBattleProbeDefaultMoves[0].attacker],
        playerMoveText,
        actors[kBattleProbeDefaultMoves[0].attacker].attackMotion);
    printf("[battle-probe-turn-data] state=player-attack trainer=0x%04X "
           "species=%u moveId=%u textId=0x%04X damage=%d\n",
           actors[kBattleProbeDefaultMoves[0].attacker].trainerId,
           actors[kBattleProbeDefaultMoves[0].attacker].speciesId,
           playerMoveId, playerTextId, playerDamage);
    hpEnemyLeft -= playerDamage;
    PCPort_BattleProbePrintAction(
        "enemy-damage",
        &actors[kBattleProbeDefaultMoves[0].target],
        playerDamageText,
        actors[kBattleProbeDefaultMoves[0].target].damageMotion);
    PCPort_BattleProbePrintAction(
        "enemy-attack",
        &actors[kBattleProbeDefaultMoves[1].attacker],
        enemyMoveText,
        actors[kBattleProbeDefaultMoves[1].attacker].attackMotion);
    printf("[battle-probe-turn-data] state=enemy-attack trainer=0x%04X "
           "species=%u moveId=%u textId=0x%04X damage=%d\n",
           actors[kBattleProbeDefaultMoves[1].attacker].trainerId,
           actors[kBattleProbeDefaultMoves[1].attacker].speciesId,
           enemyMoveId, enemyTextId, enemyDamage);
    hpPlayerLeft -= enemyDamage;
    PCPort_BattleProbePrintAction(
        "player-damage",
        &actors[kBattleProbeDefaultMoves[1].target],
        enemyDamageText,
        actors[kBattleProbeDefaultMoves[1].target].damageMotion);
    printf("[battle-probe-turn] state=end-turn playerHP=%d enemyHP=%d "
           "next=command-menu\n", hpPlayerLeft, hpEnemyLeft);
    printf("[battle-probe] summary loaded=%u/4 confirmed=%s\n",
           loadedCount, loadedCount == 4u ? "yes" : "no");
    fflush(stdout);
}

void PCPort_HeadlessMotionBatchProbe(int frames) {
    static const char* const archives[] = {
        "orig/GC6E01/disc/files/chara_big.fsys",
        "orig/GC6E01/disc/files/chara_small.fsys",
        "orig/GC6E01/disc/files/field_common.fsys",
        "orig/GC6E01/disc/files/people_archive.fsys"
    };
    int i;
    const char* skipPkx = getenv("PCPORT_MOTION_BATCH_SKIP_PKX");
    if (frames <= 1) {
        frames = 40;
    }
    printf("[headless-motion-batch] archive member motions cyclic varyCyclic "
           "idle walk run confirmed energy(idle/walk/run)\n");
    fflush(stdout);
    for (i = 0; i < (int)(sizeof(archives) / sizeof(archives[0])); ++i) {
        PCPort_HeadlessMotionBatchProbeArchive(archives[i], frames);
    }
    if (skipPkx == NULL || skipPkx[0] == '\0' || strcmp(skipPkx, "0") == 0) {
        PCPort_HeadlessMotionBatchProbePkxArchives(frames);
    }
}

void PCPort_CharAnimStepAndApply(PCPortHSDArchive* beArchive, u32 beRootJoint,
                                 f32 frameStep) {
    if (!g_charAnimReady || g_charAnimRoot == NULL || beArchive == NULL) {
        return;
    }
    /* Clamp frameStep: never negative, cap at 4 frames to survive a hitch
     * without a large jump (matches the real game's behaviour on slow frames). */
    if (frameStep < 0.0f) frameStep = 0.0f;
    if (frameStep > 4.0f) frameStep = 4.0f;
    if (g_charAnimLoopLen < 0.0f) {
        g_charAnimLoopLen = PCPort_CharAnimMaxEndFrame(g_charAnimRoot);
        if (g_charAnimLoopLen < 1.0f) g_charAnimLoopLen = 0.0f; /* no real loop */
    }
    /* Loop: the host HSD_AObjInterpretAnim never rewinds, so once curr_frame
     * passes end_frame the FObj settles to a constant (frozen) pose. Re-arm the
     * whole tree at frame 0 each cycle to make the idle/walk loop.
     * frameStep (real elapsed seconds * 60) keeps the animation locked to
     * game-speed (1 GC frame = 1/60 s) regardless of render frame rate. */
    if (g_charAnimLoopLen > 0.0f) {
        g_charAnimTime += frameStep;
        if (g_charAnimTime >= g_charAnimLoopLen) {
            g_charAnimTime = 0.0f;
            HSD_JObjReqAnimAll(g_charAnimRoot, 0.0f);
            PCPort_HSDStartAnimAll(g_charAnimRoot);
        }
    }
    PCPort_HSDJObjAnimJointOnlyAll(g_charAnimRoot);
    PCPort_CharAnimLockstepWrite(beArchive, beRootJoint, g_charAnimRoot, 1,
                                 getenv("PCPORT_CHAR_ROOT_TRANSLATE") != NULL);
    if (getenv("PCPORT_ANIM_DEBUG") != NULL) {
        /* Find the most-rotated live joint, and read back the BE value the
         * renderer will use for the SAME joint offset (lockstep position). */
        HSD_JObj* stack[512]; u32 boff[512]; int sp = 0;
        f32 bestRot = -1.0f; HSD_JObj* bestLive = NULL; u32 bestBE = 0;
        stack[sp] = g_charAnimRoot; boff[sp] = beRootJoint; sp++;
        while (sp > 0) {
            HSD_JObj* lv; u32 bo; f32 ar;
            --sp; lv = stack[sp]; bo = boff[sp];
            if (lv == NULL) continue;
            ar = fabsf(lv->rotate_x) + fabsf(lv->rotate_y) + fabsf(lv->rotate_z);
            if (ar > bestRot) { bestRot = ar; bestLive = lv; bestBE = bo; }
            if (sp < 510) {
                if (lv->child) { stack[sp] = lv->child; boff[sp] = ReadBE32(beArchive->storage + bo + 0x08); sp++; }
                if (lv->next)  { stack[sp] = lv->next;  boff[sp] = ReadBE32(beArchive->storage + bo + 0x0C); sp++; }
            }
        }
        if (bestLive != NULL) {
            f32 beRz = ReadBEFloat(beArchive->storage + bestBE + 0x1C);
            f32 cf = (bestLive->aobj != NULL) ? bestLive->aobj->curr_frame : -999.0f;
            /* also scan the whole tree for the max curr_frame (is anything ticking?) */
            f32 maxCf = -1.0f; int aobjs = 0, withFobj = 0; f32 sumAnim = 0.0f;
            HSD_JObj* st2[512]; int s2 = 0; st2[s2++] = g_charAnimRoot;
            while (s2 > 0) { HSD_JObj* lv = st2[--s2]; if (!lv) continue;
                if (lv->aobj) { aobjs++; if (lv->aobj->fobj) withFobj++;
                    if (lv->aobj->curr_frame > maxCf) maxCf = lv->aobj->curr_frame;
                    sumAnim += fabsf(lv->rotate_x)+fabsf(lv->rotate_y)+fabsf(lv->rotate_z)
                             + fabsf(lv->translate_x)+fabsf(lv->translate_y)+fabsf(lv->translate_z); }
                if (s2 < 510) { if (lv->child) st2[s2++] = lv->child; if (lv->next) st2[s2++] = lv->next; } }
            (void)beRz;
            printf("[anim-dbg] maxCurrFrame=%.2f aobjs=%d withFobj=%d animSRTchecksum=%.4f\n",
                   maxCf, aobjs, withFobj, sumAnim);
        }
    }
}

int PCPort_CharAnimReady(void) { return g_charAnimReady; }

/* Swizzle a scene model-set entry (jointList) BE->LE for the game's HSD pipeline.
 * The model entry is {rootJoint@0, animjoint@4, matanimjoint@8, ...}. Walks the
 * joint tree + the animjoint (SRT anim) + the matanimjoint (material/texture anim
 * = the title sand UV scroll), then relocates ALL pointers to native (once).
 * Returns the model-entry host pointer (read +0/+4/+8 as native HSD_Joint,
 * HSD_AnimJoint, HSD_MatAnimJoint pointers). */
void* PCPort_SwizzleSceneForHSD(PCPortHSDArchive* archive, u32 jointListOffset) {
    PCPortSwizCtx ctx;
    u32 rootOff, animOff, matanimOff;
    if (archive == NULL || archive->storage == NULL ||
        jointListOffset == 0u || jointListOffset + 0xCu > archive->storageSize) {
        return NULL;
    }
    rootOff    = ReadBE32(archive->storage + jointListOffset + 0x0);
    animOff    = ReadBE32(archive->storage + jointListOffset + 0x4);
    matanimOff = ReadBE32(archive->storage + jointListOffset + 0x8);

    memset(&ctx, 0, sizeof(ctx));
    ctx.base = archive->storage;
    ctx.size = archive->storageSize;
    ctx.dataOffset = archive->dataOffset;
    SwizJoint(&ctx, rootOff);                    /* (1a) joint tree (verified) */
    /* Anim-tree swizzle: the Colosseum AnimJoint (0x10B, all-pointer, no flags),
     * AObjDesc (0x10B {flags,end_frame,fobjdesc,obj_id}) and FObjDesc (0x14B)
     * layouts are confirmed (see project memory 2026-06-04/05) and SwizAnimJoint /
     * SwizMatAnimJoint handle them, so the anim tree is swizzled unconditionally.
     * This lets the host build a live HSD_JObj tree with attached AObj/FObj animation
     * (HSD_JObjAddAnimAll). (1b) = JObj SRT animation; (1c) = MObj/TObj material +
     * texture (UV-scroll) animation = the title "sand" drive. */
    SwizAnimJoint(&ctx, animOff);                /* (1b) SRT animation */
    SwizMatAnimJoint(&ctx, matanimOff);          /* (1c) material/texture anim (sand) */
    PCPort_HSDApplyHostRelocations(archive);     /* (2) pointers -> host */
    return (void*)(archive->storage + jointListOffset);
}

void PCPort_HSDArchiveDestroy(PCPortHSDArchive* archive) {
    if (archive == NULL) {
        return;
    }

    free(archive->storage);
    memset(archive, 0, sizeof(*archive));
}

const void* PCPort_HSDArchiveGetPublicAddress(const PCPortHSDArchive* archive,
                                              const char* name,
                                              u32* outArchiveOffset) {
    u32 i;

    if (archive == NULL || archive->storage == NULL || name == NULL) {
        return NULL;
    }

    for (i = 0; i < archive->publicCount; ++i) {
        const u8* pairPtr = archive->storage + archive->publicOffset + (i * 8u);
        PCPortArchivePair pair;
        const char* keyName;

        pair.resultOffset = ReadBE32(pairPtr + 0);
        pair.keyOffset = ReadBE32(pairPtr + 4);
        if (archive->stringOffset + pair.keyOffset >= archive->storageSize) {
            continue;
        }

        keyName = (const char*)(archive->storage + archive->stringOffset +
                                pair.keyOffset);
        if (strcmp(keyName, name) == 0) {
            u32 absoluteOffset = archive->dataOffset + pair.resultOffset;

            if (absoluteOffset >= archive->storageSize) {
                return NULL;
            }

            if (outArchiveOffset != NULL) {
                *outArchiveOffset = absoluteOffset;
            }

            return archive->storage + absoluteOffset;
        }
    }

    return NULL;
}

u32 PCPort_ReadBigEndianU32(const void* data) {
    return ReadBE32((const u8*)data);
}

void PCPort_DestroyTranslatedPObj(PCPortTranslatedPObj* pobj) {
    if (pobj == NULL) {
        return;
    }

    free(pobj->verts);
    free(pobj->displayList);
    free(pobj->positionData);
    free(pobj->colorData);
    free(pobj->texcoordData);
    free(pobj->texcoord1Data);
    free(pobj->normalData);
    memset(pobj, 0, sizeof(*pobj));
}

BOOL PCPort_TranslatePObjFromArchiveBE(const PCPortHSDArchive* archive,
                                       u32 pobjArchiveOffset,
                                       PCPortTranslatedPObj* outPObj) {
    HSD_VtxDescList parsedVerts[PCPORT_MAX_VTXDESC_ENTRIES];
    u32 sourceVertexOffsets[PCPORT_MAX_VTXDESC_ENTRIES];
    PCPortDisplayListStats stats;
    u32 nextOffset;
    u32 vertsOffset;
    u32 displayOffset;
    u32 displayCapacity;
    u32 flagsAndDisplayCount;
    u16 flags;
    u16 serializedDisplayCount;
    u32 translatedDisplaySize = 0;
    u32 entryCount = 0;
    u32 i;

    if (archive == NULL || outPObj == NULL ||
        !IsArchiveRangeValid(archive, pobjArchiveOffset, PCPORT_SERIALIZED_POBJ_SIZE)) {
        return FALSE;
    }

    nextOffset = ReadBE32(archive->storage + pobjArchiveOffset + 0x04);
    vertsOffset = ReadBE32(archive->storage + pobjArchiveOffset + 0x08);
    flagsAndDisplayCount = ReadBE32(archive->storage + pobjArchiveOffset + 0x0C);
    displayOffset = ReadBE32(archive->storage + pobjArchiveOffset + 0x10);
    flags = (u16)(flagsAndDisplayCount >> 16);
    serializedDisplayCount = (u16)(flagsAndDisplayCount & 0xFFFFu);

    /* nextOffset (pobj+0x04) being non-zero just means this PObj is one link of
     * a chain (e.g. a character DObj's body meshes); each link has its own
     * complete vtxdesc + display list, so translate it on its own. The caller
     * (RenderJointTree) walks the chain. (Previously next!=0 was rejected, which
     * dropped most of a skinned character's meshes.) */
    if (serializedDisplayCount == 0u ||
        !IsArchiveRangeValid(archive, vertsOffset, PCPORT_SERIALIZED_VTXDESC_SIZE) ||
        displayOffset >= pobjArchiveOffset) {
        if (getenv("PCPORT_SKIN_DEBUG") != NULL) {
            fprintf(stderr,
                    "[skin] pobj@0x%X flags=0x%04X type=%u early-reject next=0x%X dispCount=%u\n",
                    pobjArchiveOffset, flags, (flags >> 12) & 3u,
                    nextOffset, serializedDisplayCount);
        }
        return FALSE;
    }

    /* The display-list length is the PObj's n_display (serializedDisplayCount) in
     * 32-byte GX-FIFO chunks -- the authoritative size. The old heuristic used the
     * gap to the next struct (pobjArchiveOffset - displayOffset), which for some
     * meshes is LARGER than the real list (e.g. mania pobj@0x5E6C: gap=1804 but
     * real len=55*32=1760). The scan then over-ran 44 bytes into the following
     * data, desynced, failed, and the WHOLE mesh was skipped -- the missing
     * coat/torso. Use n_display*32, capped at the gap so it never overlaps the
     * PObj struct. */
    displayCapacity = (u32)serializedDisplayCount * 32u;
    {
        u32 gapToStruct = pobjArchiveOffset - displayOffset;
        if (displayCapacity == 0u || displayCapacity > gapToStruct) {
            displayCapacity = gapToStruct;
        }
    }
    if (!IsArchiveRangeValid(archive, displayOffset, displayCapacity)) {
        return FALSE;
    }

    memset(outPObj, 0, sizeof(*outPObj));
    memset(parsedVerts, 0, sizeof(parsedVerts));
    memset(sourceVertexOffsets, 0, sizeof(sourceVertexOffsets));

    for (entryCount = 0; entryCount + 1u < PCPORT_MAX_VTXDESC_ENTRIES; ++entryCount) {
        if (!ParseSerializedVtxDesc(archive,
                                    vertsOffset + (entryCount * PCPORT_SERIALIZED_VTXDESC_SIZE),
                                    &parsedVerts[entryCount],
                                    &sourceVertexOffsets[entryCount])) {
            PCPort_DestroyTranslatedPObj(outPObj);
            return FALSE;
        }

        if (parsedVerts[entryCount].attr == GX_VA_NULL) {
            break;
        }

        /* Skinned (envelope) meshes carry GX_VA_PNMTXIDX: a 1-byte DIRECT
         * matrix-palette index inline per vertex, with NO source vertex array.
         * Accept it (so the geometry translates instead of being rejected); the
         * per-vertex matrix + envelope-palette transform are applied at render
         * time. Rigid field meshes (type 0) never carry this attribute, so the
         * proven rigid path is unaffected. */
        if (parsedVerts[entryCount].attr <= GX_VA_TEX7MTXIDX) {
            continue;
        }

        if ((parsedVerts[entryCount].attr != GX_VA_POS &&
             parsedVerts[entryCount].attr != GX_VA_NRM &&
             parsedVerts[entryCount].attr != GX_VA_CLR0 &&
             parsedVerts[entryCount].attr != GX_VA_TEX0 &&
             parsedVerts[entryCount].attr != GX_VA_TEX1) ||
            GetIndexByteCount(parsedVerts[entryCount].attr_type) == 0 ||
            parsedVerts[entryCount].stride == 0) {
            if (getenv("PCPORT_SKIN_DEBUG") != NULL) {
                fprintf(stderr,
                        "[skin] pobj@0x%X flags=0x%04X type=%u reject attr=%u attr_type=%u stride=%u\n",
                        pobjArchiveOffset, flags, (flags >> 12) & 3u,
                        parsedVerts[entryCount].attr,
                        parsedVerts[entryCount].attr_type,
                        parsedVerts[entryCount].stride);
            }
            PCPort_DestroyTranslatedPObj(outPObj);
            return FALSE;
        }
    }

    if (entryCount == 0u || parsedVerts[entryCount].attr != GX_VA_NULL) {
        if (getenv("PCPORT_SKIN_DEBUG") != NULL)
            fprintf(stderr, "[skin] pobj@0x%X FAIL: vtxdesc not terminated (entryCount=%u)\n",
                    pobjArchiveOffset, entryCount);
        PCPort_DestroyTranslatedPObj(outPObj);
        return FALSE;
    }

    outPObj->verts = (HSD_VtxDescList*)calloc(entryCount + 1u, sizeof(HSD_VtxDescList));
    if (outPObj->verts == NULL) {
        PCPort_DestroyTranslatedPObj(outPObj);
        return FALSE;
    }

    memcpy(outPObj->verts, parsedVerts, (entryCount + 1u) * sizeof(HSD_VtxDescList));

    if (!ScanDisplayListIndices(archive->storage + displayOffset,
                                displayCapacity,
                                outPObj->verts,
                                &stats,
                                &translatedDisplaySize)) {
        if (getenv("PCPORT_SKIN_DEBUG") != NULL)
            fprintf(stderr, "[skin] pobj@0x%X FAIL: ScanDisplayListIndices (dispOff=0x%X cap=%u dispCount=%u)\n",
                    pobjArchiveOffset, displayOffset, displayCapacity, serializedDisplayCount);
        PCPort_DestroyTranslatedPObj(outPObj);
        return FALSE;
    }

    if (translatedDisplaySize == 0u) {
        if (getenv("PCPORT_SKIN_DEBUG") != NULL)
            fprintf(stderr, "[skin] pobj@0x%X FAIL: translatedDisplaySize=0\n", pobjArchiveOffset);
        PCPort_DestroyTranslatedPObj(outPObj);
        return FALSE;
    }

    for (i = 0; i < entryCount; ++i) {
        u32 usedCount;

        /* The matrix-index attributes (PNMTXIDX + TEXnMTXIDX) have no source
         * vertex array (inline DIRECT bytes); skip the array load for them. */
        if (outPObj->verts[i].attr <= GX_VA_TEX7MTXIDX) {
            continue;
        }

        switch (outPObj->verts[i].attr) {
        case GX_VA_POS:
            usedCount = stats.maxPosIndex + 1u;
            break;
        case GX_VA_NRM:
            usedCount = stats.maxNormalIndex + 1u;
            break;
        case GX_VA_CLR0:
            usedCount = stats.maxColorIndex + 1u;
            break;
        case GX_VA_TEX0:
            usedCount = stats.maxTexcoordIndex + 1u;
            break;
        case GX_VA_TEX1:
            usedCount = stats.maxTexcoord1Index + 1u;
            break;
        default:
            PCPort_DestroyTranslatedPObj(outPObj);
            return FALSE;
        }

        if (!TranslateVertexArray(archive,
                                  &outPObj->verts[i],
                                  sourceVertexOffsets[i],
                                  usedCount,
                                  outPObj)) {
            if (getenv("PCPORT_SKIN_DEBUG") != NULL)
                fprintf(stderr, "[skin] pobj@0x%X FAIL: TranslateVertexArray attr=%u srcOff=0x%X usedCount=%u stride=%u\n",
                        pobjArchiveOffset, outPObj->verts[i].attr, sourceVertexOffsets[i],
                        usedCount, outPObj->verts[i].stride);
            PCPort_DestroyTranslatedPObj(outPObj);
            return FALSE;
        }
    }

    outPObj->displayList = (u8*)malloc(translatedDisplaySize);
    if (outPObj->displayList == NULL) {
        PCPort_DestroyTranslatedPObj(outPObj);
        return FALSE;
    }

    memcpy(outPObj->displayList, archive->storage + displayOffset, translatedDisplaySize);
    outPObj->pobj.verts = outPObj->verts;
    outPObj->pobj.flags = flags;
    outPObj->pobj.n_display = (u16)translatedDisplaySize;
    outPObj->pobj.display = outPObj->displayList;
    outPObj->sourceArchiveOffset = pobjArchiveOffset;
    outPObj->totalSubmittedVertices = stats.totalSubmittedVertices;
    outPObj->totalPrimitiveCommands = stats.totalPrimitiveCommands;
    return TRUE;
}

BOOL PCPort_TranslateJointChainToMatrixBE(const PCPortHSDArchive* archive,
                                          u32 rootJointArchiveOffset,
                                          u32 targetJointArchiveOffset,
                                          PCPortTranslatedJointTransform* outTransform) {
    u32 pathOffsets[PCPORT_MAX_JOINT_PATH];
    u32 pathLength = 0;
    u32 i;

    if (outTransform == NULL || archive == NULL || archive->storage == NULL ||
        !IsArchiveRangeValid(archive, rootJointArchiveOffset, PCPORT_SERIALIZED_JOINT_SIZE) ||
        !IsArchiveRangeValid(archive, targetJointArchiveOffset, PCPORT_SERIALIZED_JOINT_SIZE)) {
        return FALSE;
    }

    memset(outTransform, 0, sizeof(*outTransform));
    if (!FindJointPath(archive,
                       rootJointArchiveOffset,
                       targetJointArchiveOffset,
                       pathOffsets,
                       0u,
                       &pathLength) ||
        pathLength == 0u) {
        return FALSE;
    }

    LoadIdentityMtx(outTransform->modelMatrix);
    for (i = 0; i < pathLength; ++i) {
        f32 localMtx[3][4];

        BuildJointLocalMtx(archive, pathOffsets[i], localMtx);
        MultiplyAffineMtx(outTransform->modelMatrix,
                          localMtx,
                          outTransform->modelMatrix);
    }

    outTransform->rootArchiveOffset = rootJointArchiveOffset;
    outTransform->jointArchiveOffset = targetJointArchiveOffset;
    return TRUE;
}

BOOL PCPort_TranslatePerspectiveCameraFromArchiveBE(const PCPortHSDArchive* archive,
                                                    u32 cameraArchiveOffset,
                                                    PCPortTranslatedCamera* outCamera) {
    u16 projectionType;
    u32 eyeOffset;
    u32 interestOffset;
    u32 upVectorOffset;
    f32 upVector[3] = { 0.0f, 1.0f, 0.0f };

    if (outCamera == NULL ||
        !IsArchiveRangeValid(archive,
                             cameraArchiveOffset,
                             PCPORT_SERIALIZED_COBJ_PERSPECTIVE_SIZE)) {
        return FALSE;
    }

    memset(outCamera, 0, sizeof(*outCamera));
    projectionType = ReadBE16(archive->storage + cameraArchiveOffset + 0x06);
    if (projectionType != 1u) {
        return FALSE;
    }

    outCamera->viewportLeft = ReadBE16(archive->storage + cameraArchiveOffset + 0x08);
    outCamera->viewportRight = ReadBE16(archive->storage + cameraArchiveOffset + 0x0A);
    outCamera->viewportTop = ReadBE16(archive->storage + cameraArchiveOffset + 0x0C);
    outCamera->viewportBottom = ReadBE16(archive->storage + cameraArchiveOffset + 0x0E);
    outCamera->scissorLeft = ReadBE16(archive->storage + cameraArchiveOffset + 0x10);
    outCamera->scissorRight = ReadBE16(archive->storage + cameraArchiveOffset + 0x12);
    outCamera->scissorTop = ReadBE16(archive->storage + cameraArchiveOffset + 0x14);
    outCamera->scissorBottom = ReadBE16(archive->storage + cameraArchiveOffset + 0x16);
    eyeOffset = ReadBE32(archive->storage + cameraArchiveOffset + 0x18);
    interestOffset = ReadBE32(archive->storage + cameraArchiveOffset + 0x1C);
    upVectorOffset = ReadBE32(archive->storage + cameraArchiveOffset + 0x24);
    outCamera->nearZ = ReadBEFloat(archive->storage + cameraArchiveOffset + 0x28);
    outCamera->farZ = ReadBEFloat(archive->storage + cameraArchiveOffset + 0x2C);
    outCamera->fov = ReadBEFloat(archive->storage + cameraArchiveOffset + 0x30);
    outCamera->aspect = ReadBEFloat(archive->storage + cameraArchiveOffset + 0x34);
    if (!(outCamera->aspect > 0.0f) &&
        outCamera->viewportBottom > outCamera->viewportTop) {
        outCamera->aspect =
            (f32)(outCamera->viewportRight - outCamera->viewportLeft) /
            (f32)(outCamera->viewportBottom - outCamera->viewportTop);
    }

    if (!LoadSerializedWObjPosition(archive, eyeOffset, outCamera->eye) ||
        !LoadSerializedWObjPosition(archive, interestOffset, outCamera->interest)) {
        return FALSE;
    }

    if (upVectorOffset != 0u &&
        IsArchiveRangeValid(archive, upVectorOffset, 0x0Cu)) {
        upVector[0] = ReadBEFloat(archive->storage + upVectorOffset + 0x00);
        upVector[1] = ReadBEFloat(archive->storage + upVectorOffset + 0x04);
        upVector[2] = ReadBEFloat(archive->storage + upVectorOffset + 0x08);
    }
    memcpy(outCamera->up, upVector, sizeof(upVector));

    if (!BuildCameraViewMatrix(outCamera->eye,
                               outCamera->interest,
                               outCamera->up,
                               outCamera->viewMatrix) ||
        !BuildCameraProjectionMatrix(outCamera->fov,
                                     outCamera->aspect,
                                     outCamera->nearZ,
                                     outCamera->farZ,
                                     outCamera->projectionMatrix)) {
        return FALSE;
    }

    outCamera->cameraArchiveOffset = cameraArchiveOffset;
    outCamera->eyeArchiveOffset = eyeOffset;
    outCamera->interestArchiveOffset = interestOffset;
    return TRUE;
}

BOOL PCPort_TranslateMaterialFromArchiveBE(const PCPortHSDArchive* archive,
                                           u32 mobjArchiveOffset,
                                           PCPortTranslatedMaterial* outMaterial) {
    u32 materialOffset;
    u32 peOffset;

    if (outMaterial == NULL ||
        !IsArchiveRangeValid(archive, mobjArchiveOffset, PCPORT_SERIALIZED_MOBJ_SIZE)) {
        return FALSE;
    }

    memset(outMaterial, 0, sizeof(*outMaterial));
    outMaterial->alpha = 1.0f;
    outMaterial->mobjArchiveOffset = mobjArchiveOffset;
    outMaterial->rendermode = ReadBE32(archive->storage + mobjArchiveOffset + 0x04);
    materialOffset = ReadBE32(archive->storage + mobjArchiveOffset + 0x0C);
    peOffset = ReadBE32(archive->storage + mobjArchiveOffset + 0x14);

    if (materialOffset != 0u) {
        if (!IsArchiveRangeValid(archive,
                                 materialOffset,
                                 PCPORT_SERIALIZED_MATERIAL_SIZE)) {
            return FALSE;
        }

        outMaterial->materialArchiveOffset = materialOffset;
        outMaterial->ambient = ReadBE32(archive->storage + materialOffset + 0x00);
        outMaterial->diffuse = ReadBE32(archive->storage + materialOffset + 0x04);
        outMaterial->specular = ReadBE32(archive->storage + materialOffset + 0x08);
        outMaterial->alpha = ReadBEFloat(archive->storage + materialOffset + 0x0C);
        outMaterial->shininess = ReadBEFloat(archive->storage + materialOffset + 0x10);
    }

    if (peOffset != 0u) {
        if (!IsArchiveRangeValid(archive, peOffset, PCPORT_SERIALIZED_PEDESC_SIZE)) {
            return FALSE;
        }

        outMaterial->pedescArchiveOffset = peOffset;
        outMaterial->hasPEDesc = TRUE;
        outMaterial->peFlags = archive->storage[peOffset + 0x00];
        outMaterial->peRef0 = archive->storage[peOffset + 0x01];
        outMaterial->peRef1 = archive->storage[peOffset + 0x02];
        outMaterial->peDstAlpha = archive->storage[peOffset + 0x03];
        outMaterial->peType = archive->storage[peOffset + 0x04];
        outMaterial->peSrcFactor = archive->storage[peOffset + 0x05];
        outMaterial->peDstFactor = archive->storage[peOffset + 0x06];
        outMaterial->peLogicOp = archive->storage[peOffset + 0x07];
        outMaterial->peZComp = archive->storage[peOffset + 0x08];
        outMaterial->peAlphaComp0 = archive->storage[peOffset + 0x09];
        outMaterial->peAlphaOp = archive->storage[peOffset + 0x0A];
        outMaterial->peAlphaComp1 = archive->storage[peOffset + 0x0B];
    }

    return TRUE;
}

static BOOL TranslateTextureFromArchiveCommon(const PCPortHSDArchive* archive,
                                              u32 tobjArchiveOffset,
                                              BOOL allowNext,
                                              PCPortTranslatedTexture* outTexture) {
    u32 imageOffset;
    u32 imageDataOffset;
    u32 tevOffset;
    u32 tlutOffset;

    if (outTexture == NULL ||
        !IsArchiveRangeValid(archive, tobjArchiveOffset, PCPORT_SERIALIZED_TOBJ_SIZE)) {
        return FALSE;
    }

    memset(outTexture, 0, sizeof(*outTexture));
    outTexture->tevMode = PCPORT_GX_TEV_MODULATE;

    if (!allowNext && ReadBE32(archive->storage + tobjArchiveOffset + 0x04) != 0u) {
        return FALSE;
    }

    imageOffset = ReadBE32(archive->storage + tobjArchiveOffset + 0x4C);
    if (!IsArchiveRangeValid(archive, imageOffset, PCPORT_SERIALIZED_IMAGEDESC_SIZE)) {
        return FALSE;
    }

    imageDataOffset = ReadBE32(archive->storage + imageOffset + 0x00);
    outTexture->width = ReadBE16(archive->storage + imageOffset + 0x04);
    outTexture->height = ReadBE16(archive->storage + imageOffset + 0x06);
    outTexture->format = ReadBE32(archive->storage + imageOffset + 0x08);
    outTexture->mipmap = (u8)(ReadBE32(archive->storage + imageOffset + 0x0C) != 0u);
    outTexture->blending = ReadBEFloat(archive->storage + tobjArchiveOffset + 0x44);
    outTexture->texCoordSrc = ReadBE32(archive->storage + tobjArchiveOffset + 0x0C);
    outTexture->hasCoordId = (u8)ResolveTextureCoordIdFromRawSrc(outTexture->texCoordSrc,
                                                                 &outTexture->coordId);
    outTexture->wrapS = ReadBE32(archive->storage + tobjArchiveOffset + 0x34);
    outTexture->wrapT = ReadBE32(archive->storage + tobjArchiveOffset + 0x38);
    outTexture->repeatS = archive->storage[tobjArchiveOffset + 0x3C];
    outTexture->repeatT = archive->storage[tobjArchiveOffset + 0x3D];
    outTexture->flags = ReadBE32(archive->storage + tobjArchiveOffset + 0x40);
    outTexture->magFilter = ReadBE32(archive->storage + tobjArchiveOffset + 0x48);
    outTexture->tobjArchiveOffset = tobjArchiveOffset;
    outTexture->imageArchiveOffset = imageOffset;
    outTexture->imageDataArchiveOffset = imageDataOffset;
    tevOffset = ReadBE32(archive->storage + tobjArchiveOffset + 0x58);
    tlutOffset = ReadBE32(archive->storage + tobjArchiveOffset + 0x50);
    outTexture->tevArchiveOffset = tevOffset;

    /* tobj+0x50 points at an HSD TlutDesc (NOT the palette data directly):
     *   { u32 lutDataOffset@0x00; u32 fmt@0x04 (GXTlutFmt);
     *     u32 name@0x08; u16 n_entries@0x0C }
     * Capture it so palettized (CI4/CI8) textures can be decoded. */
    if (tlutOffset != 0u && IsArchiveRangeValid(archive, tlutOffset, 0x10u)) {
        outTexture->tlutArchiveOffset =
            ReadBE32(archive->storage + tlutOffset + 0x00);
        outTexture->tlutFmt = ReadBE32(archive->storage + tlutOffset + 0x04);
        outTexture->tlutEntries = ReadBE16(archive->storage + tlutOffset + 0x0C);
    }

    if (outTexture->width == 0u || outTexture->height == 0u ||
        (tevOffset != 0u && !IsArchiveRangeValid(archive, tevOffset, 1u)) ||
        !IsArchiveRangeValid(archive, imageDataOffset, 1u)) {
        return FALSE;
    }

    switch (outTexture->flags & PCPORT_TEX_COLORMAP_MASK) {
    case PCPORT_TEX_COLORMAP_REPLACE:
        outTexture->tevMode = PCPORT_GX_TEV_REPLACE;
        break;
    case PCPORT_TEX_COLORMAP_PASS:
        outTexture->tevMode = PCPORT_GX_TEV_PASSCLR;
        break;
    case PCPORT_TEX_COLORMAP_MODULATE:
    default:
        outTexture->tevMode = PCPORT_GX_TEV_MODULATE;
        break;
    }

    TranslateTextureTevPayload(archive, tevOffset, outTexture->format,
                               &outTexture->tev);
    return TRUE;
}

BOOL PCPort_TranslateTextureFromArchiveBE(const PCPortHSDArchive* archive,
                                          u32 tobjArchiveOffset,
                                          PCPortTranslatedTexture* outTexture) {
    return TranslateTextureFromArchiveCommon(archive,
                                             tobjArchiveOffset,
                                             FALSE,
                                             outTexture);
}

BOOL PCPort_TranslateTextureNodeFromArchiveBE(const PCPortHSDArchive* archive,
                                              u32 tobjArchiveOffset,
                                              PCPortTranslatedTexture* outTexture) {
    return TranslateTextureFromArchiveCommon(archive,
                                             tobjArchiveOffset,
                                             TRUE,
                                             outTexture);
}

BOOL PCPort_TranslateTextureExpFromArchiveBE(const PCPortHSDArchive* archive,
                                             u32 tobjArchiveOffset,
                                             PCPortTranslatedTextureExp* outExp) {
    PCPortParsedTextureNodeChain parsedChain;
    u32 stageIndex;

    if (outExp == NULL) {
        return FALSE;
    }

    memset(outExp, 0, sizeof(*outExp));
    if (!PCPort_ParseTextureNodeChainFromArchiveBE(archive,
                                                   tobjArchiveOffset,
                                                   PCPORT_TEXP_STAGE_MAX,
                                                   &parsedChain)) {
        return FALSE;
    }

    outExp->headArchiveOffset = parsedChain.headArchiveOffset;
    outExp->kind = parsedChain.kind;
    outExp->stageCount = parsedChain.nodeCount;
    for (stageIndex = 0u; stageIndex < parsedChain.nodeCount; ++stageIndex) {
        PCPortTranslatedTextureExpStage* stage = &outExp->stages[stageIndex];

        stage->coordId = parsedChain.coordIds[stageIndex];
        stage->texture = parsedChain.nodes[stageIndex];
        stage->kind = parsedChain.stageKinds[stageIndex];
        if (stage->kind == PCPORT_TEXP_STAGE_NONE) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL PCPort_TranslateTextureChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               PCPortTranslatedTextureChain* outChain) {
    PCPortParsedTextureNodeChain parsedChain;
    u8 expKind;
    u32 nodeIndex;

    if (outChain == NULL) {
        return FALSE;
    }

    memset(outChain, 0, sizeof(*outChain));
    if (!PCPort_ParseTextureNodeChainFromArchiveBE(archive,
                                                   tobjArchiveOffset,
                                                   PCPORT_TEXTURE_CHAIN_MAX_NODES,
                                                   &parsedChain)) {
        return FALSE;
    }

    outChain->headArchiveOffset = parsedChain.headArchiveOffset;
    outChain->nodeCount = parsedChain.nodeCount;
    for (nodeIndex = 0u; nodeIndex < parsedChain.nodeCount; ++nodeIndex) {
        outChain->coordIds[nodeIndex] = parsedChain.coordIds[nodeIndex];
        outChain->nodes[nodeIndex] = parsedChain.nodes[nodeIndex];
    }

    expKind = parsedChain.kind;
    if (expKind == PCPORT_TEXTURE_EXP_KIND_I8_RAMP) {
        outChain->kind = PCPORT_TEXTURE_CHAIN_I8_RAMP;
        return TRUE;
    }

    if (expKind == PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK) {
        outChain->kind = PCPORT_TEXTURE_CHAIN_I8_RAMP_MASK;
        return TRUE;
    }

    return FALSE;
}

static BOOL DecodeI8TextureToLinear(const PCPortHSDArchive* archive,
                                    const PCPortTranslatedTexture* texture,
                                    u8** outPixels,
                                    u32* outSize) {
    u32 tilesX;
    u32 tilesY;
    u32 tileY;
    u32 tileX;
    u8* pixels;

    if (outPixels == NULL || outSize == NULL) {
        return FALSE;
    }

    *outPixels = NULL;
    *outSize = 0u;

    if (archive == NULL || texture == NULL ||
        texture->format != GX_TF_I8 ||
        texture->width == 0u || texture->height == 0u ||
        !IsArchiveRangeValid(archive, texture->imageDataArchiveOffset, 1u)) {
        return FALSE;
    }

    *outSize = (u32)texture->width * (u32)texture->height;
    pixels = (u8*)malloc((size_t)*outSize);
    if (pixels == NULL) {
        *outSize = 0u;
        return FALSE;
    }

    tilesX = ((u32)texture->width + 7u) / 8u;
    tilesY = ((u32)texture->height + 3u) / 4u;
    for (tileY = 0; tileY < tilesY; ++tileY) {
        for (tileX = 0; tileX < tilesX; ++tileX) {
            const u8* tileSrc =
                archive->storage + texture->imageDataArchiveOffset +
                (((tileY * tilesX) + tileX) * 32u);
            u32 row;

            for (row = 0; row < 4u; ++row) {
                u32 dstY = (tileY * 4u) + row;
                u32 col;

                if (dstY >= texture->height) {
                    continue;
                }

                for (col = 0; col < 8u; ++col) {
                    u32 dstX = (tileX * 8u) + col;

                    if (dstX >= texture->width) {
                        continue;
                    }

                    pixels[(dstY * (u32)texture->width) + dstX] =
                        tileSrc[(row * 8u) + col];
                }
            }
        }
    }

    *outPixels = pixels;
    return TRUE;
}

static BOOL DecodeTextureToRGBA(const PCPortHSDArchive* archive,
                                const PCPortTranslatedTexture* texture,
                                u8** outPixels,
                                u32* outSize) {
    GXDecodedTexture decoded;
    const void* tlutData = NULL;
    GXTlutFmt tlutFmt = GX_TL_IA8;
    u16 tlutEntries = 0u;

    if (outPixels == NULL || outSize == NULL) {
        return FALSE;
    }

    *outPixels = NULL;
    *outSize = 0u;

    if (archive == NULL || texture == NULL ||
        texture->width == 0u || texture->height == 0u ||
        !IsArchiveRangeValid(archive, texture->imageDataArchiveOffset, 1u)) {
        return FALSE;
    }

    /* Palettized (CI4/CI8) textures carry a TLUT (palette). Point the decoder
     * at the captured palette data so the index->RGBA lookup resolves; for
     * non-palettized formats the TLUT args stay NULL/0. */
    if (texture->tlutArchiveOffset != 0u &&
        IsArchiveRangeValid(archive, texture->tlutArchiveOffset, 1u)) {
        tlutData = archive->storage + texture->tlutArchiveOffset;
        tlutFmt = (GXTlutFmt)texture->tlutFmt;
        tlutEntries = texture->tlutEntries;
    }

    memset(&decoded, 0, sizeof(decoded));
    if (gx_texture_decode(archive->storage + texture->imageDataArchiveOffset,
                          texture->width,
                          texture->height,
                          (GXTexFmt)texture->format,
                          tlutData,
                          tlutFmt,
                          tlutEntries,
                          &decoded) != 0 ||
        decoded.data == NULL ||
        decoded.isCompressed != 0u) {
        gx_texture_free(&decoded);
        return FALSE;
    }

    *outPixels = decoded.data;
    *outSize = decoded.dataSize;
    return TRUE;
}

static u32 WrapNormalizedTexelIndex(f32 coord,
                                    u32 size,
                                    u32 wrapMode) {
    f32 wrapped = coord;
    u32 index;

    if (size == 0u) {
        return 0u;
    }

    if (wrapMode == GX_REPEAT) {
        while (wrapped < 0.0f) {
            wrapped += 1.0f;
        }
        while (wrapped >= 1.0f) {
            wrapped -= 1.0f;
        }
    } else if (wrapMode == GX_MIRROR) {
        while (wrapped < 0.0f) {
            wrapped += 2.0f;
        }
        while (wrapped >= 2.0f) {
            wrapped -= 2.0f;
        }
        if (wrapped > 1.0f) {
            wrapped = 2.0f - wrapped;
        }
        if (wrapped >= 1.0f) {
            wrapped = 0.999999f;
        }
    } else {
        if (wrapped < 0.0f) {
            wrapped = 0.0f;
        } else if (wrapped >= 1.0f) {
            wrapped = 0.999999f;
        }
    }

    index = (u32)(wrapped * (f32)size);
    if (index >= size) {
        index = size - 1u;
    }
    return index;
}

BOOL PCPort_BakeTextureRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                         const PCPortTranslatedTexture* texture,
                                         u8** outPixels,
                                         u32* outSize) {
    u32 totalSize;
    u8* pixels;

    if (outPixels == NULL || outSize == NULL) {
        return FALSE;
    }

    *outPixels = NULL;
    *outSize = 0;

    if (archive == NULL || texture == NULL ||
        texture->width == 0u || texture->height == 0u ||
        !IsArchiveRangeValid(archive, texture->imageDataArchiveOffset, 1u)) {
        return FALSE;
    }

    /* I8 colour-ramp TEV: bake the ramp into RGBA. */
    if (texture->tev.kind == PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP &&
        texture->format == GX_TF_I8) {
        totalSize = (u32)texture->width * (u32)texture->height * 4u;
        if (totalSize == 0u) {
            return FALSE;
        }

        pixels = (u8*)malloc((size_t)totalSize);
        if (pixels == NULL) {
            return FALSE;
        }

        DecodeI8RampTexture(archive->storage + texture->imageDataArchiveOffset,
                            texture->width,
                            texture->height,
                            texture->tev.rampLight,
                            texture->tev.rampDark,
                            pixels);
        *outPixels = pixels;
        *outSize = totalSize;
        return TRUE;
    }

    /* Any other texture (no TEV, or a generic non-ramp TEV stage) is a plain
     * sampled texture: decode its native GX format directly. The pipeline's
     * tevMode (REPLACE/PASS/MODULATE) applies the colour blend, so a non-NULL
     * TEV node here is NOT a reason to fall back to a flat material -- that is
     * what left the RGBA8/palettized ground/ruins nodes rendering gray. */
    return DecodeTextureToRGBA(archive, texture, outPixels, outSize);
}

BOOL PCPort_BakeTextureExpRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                            const PCPortTranslatedTextureExp* exp,
                                            u8** outPixels,
                                            u32* outSize) {
    u8* basePixels = NULL;
    u8* modulatePixels = NULL;
    u32 baseSize = 0u;
    u32 modulateSize = 0u;
    u32 stageIndex;

    if (outPixels == NULL || outSize == NULL) {
        return FALSE;
    }

    *outPixels = NULL;
    *outSize = 0u;

    if (archive == NULL || exp == NULL || exp->stageCount == 0u ||
        !PCPort_BakeTextureRGBAFromArchiveBE(archive,
                                             &exp->stages[0].texture,
                                             &basePixels,
                                             &baseSize)) {
        return FALSE;
    }

    for (stageIndex = 1u; stageIndex < exp->stageCount; ++stageIndex) {
        const PCPortTranslatedTextureExpStage* stage = &exp->stages[stageIndex];
        u32 x;
        u32 y;

        if (stage->kind != PCPORT_TEXP_STAGE_I8_MASK_MODULATE ||
            !DecodeI8TextureToLinear(archive,
                                     &stage->texture,
                                     &modulatePixels,
                                     &modulateSize)) {
            PCPort_FreeBuffer(modulatePixels);
            modulatePixels = NULL;
            break;
        }

        for (y = 0; y < (u32)exp->stages[0].texture.height; ++y) {
            for (x = 0; x < (u32)exp->stages[0].texture.width; ++x) {
                f32 u = ((f32)x + 0.5f) / (f32)exp->stages[0].texture.width;
                f32 v = ((f32)y + 0.5f) / (f32)exp->stages[0].texture.height;
                u32 maskX =
                    WrapNormalizedTexelIndex(u,
                                             (u32)stage->texture.width,
                                             stage->texture.wrapS);
                u32 maskY =
                    WrapNormalizedTexelIndex(v,
                                             (u32)stage->texture.height,
                                             stage->texture.wrapT);
                u8 mask =
                    modulatePixels[(maskY * (u32)stage->texture.width) + maskX];
                u8* dst =
                    basePixels + ((((y * (u32)exp->stages[0].texture.width) + x) * 4u));

                dst[0] = (u8)(((u32)dst[0] * (u32)mask + 127u) / 255u);
                dst[1] = (u8)(((u32)dst[1] * (u32)mask + 127u) / 255u);
                dst[2] = (u8)(((u32)dst[2] * (u32)mask + 127u) / 255u);
                dst[3] = (u8)(((u32)dst[3] * (u32)mask + 127u) / 255u);
            }
        }

        PCPort_FreeBuffer(modulatePixels);
        modulatePixels = NULL;
        (void)modulateSize;
    }

    *outPixels = basePixels;
    *outSize = baseSize;
    return TRUE;
}

BOOL PCPort_BakeTextureChainRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                              const PCPortTranslatedTextureChain* chain,
                                              u8** outPixels,
                                              u32* outSize) {
    u8* basePixels = NULL;
    u8* modulatePixels = NULL;
    u32 baseSize = 0u;
    u32 modulateSize = 0u;
    u32 x;
    u32 y;

    if (outPixels == NULL || outSize == NULL) {
        return FALSE;
    }

    *outPixels = NULL;
    *outSize = 0u;

    if (archive == NULL || chain == NULL) {
        return FALSE;
    }

    if (chain->kind == PCPORT_TEXTURE_CHAIN_I8_RAMP &&
        chain->nodeCount == 1u) {
        return PCPort_BakeTextureRGBAFromArchiveBE(archive,
                                                   &chain->nodes[0],
                                                   outPixels,
                                                   outSize);
    }

    if (chain->kind != PCPORT_TEXTURE_CHAIN_I8_RAMP_MASK ||
        chain->nodeCount != 2u ||
        !PCPort_BakeTextureRGBAFromArchiveBE(archive,
                                             &chain->nodes[0],
                                             &basePixels,
                                             &baseSize) ||
        !DecodeI8TextureToLinear(archive,
                                 &chain->nodes[1],
                                 &modulatePixels,
                                 &modulateSize)) {
        PCPort_FreeBuffer(modulatePixels);
        PCPort_FreeBuffer(basePixels);
        return FALSE;
    }

    for (y = 0; y < (u32)chain->nodes[0].height; ++y) {
        for (x = 0; x < (u32)chain->nodes[0].width; ++x) {
            f32 u = ((f32)x + 0.5f) / (f32)chain->nodes[0].width;
            f32 v = ((f32)y + 0.5f) / (f32)chain->nodes[0].height;
            u32 maskX =
                WrapNormalizedTexelIndex(u,
                                         (u32)chain->nodes[1].width,
                                         chain->nodes[1].wrapS);
            u32 maskY =
                WrapNormalizedTexelIndex(v,
                                         (u32)chain->nodes[1].height,
                                         chain->nodes[1].wrapT);
            u8 mask =
                modulatePixels[(maskY * (u32)chain->nodes[1].width) + maskX];
            u8* dst =
                basePixels + ((((y * (u32)chain->nodes[0].width) + x) * 4u));

            dst[0] = (u8)(((u32)dst[0] * (u32)mask + 127u) / 255u);
            dst[1] = (u8)(((u32)dst[1] * (u32)mask + 127u) / 255u);
            dst[2] = (u8)(((u32)dst[2] * (u32)mask + 127u) / 255u);
            dst[3] = (u8)(((u32)dst[3] * (u32)mask + 127u) / 255u);
        }
    }

    PCPort_FreeBuffer(modulatePixels);
    *outPixels = basePixels;
    *outSize = baseSize;
    (void)modulateSize;
    return TRUE;
}
