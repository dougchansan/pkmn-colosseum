/**
 * @file sdk_range_800BA7C0.c
 * @brief Dolphin GX texture object helpers, 0x800BA7C0 - 0x800BAE5C.
 */

#include "dolphin/types.h"

typedef enum GXTexFmt_800BA7C0 {
    GX_TF_I4_800BA7C0 = 0,
    GX_TF_I8_800BA7C0 = 1,
    GX_TF_IA4_800BA7C0 = 2,
    GX_TF_IA8_800BA7C0 = 3,
    GX_TF_RGB565_800BA7C0 = 4,
    GX_TF_RGB5A3_800BA7C0 = 5,
    GX_TF_RGBA8_800BA7C0 = 6,
    GX_TF_C4_800BA7C0 = 8,
    GX_TF_C8_800BA7C0 = 9,
    GX_TF_C14X2_800BA7C0 = 10,
    GX_TF_CMPR_800BA7C0 = 14,
    GX_TF_Z8_800BA7C0 = 17,
    GX_TF_Z16_800BA7C0 = 19,
    GX_TF_Z24X8_800BA7C0 = 22,
    GX_CTF_R4_800BA7C0 = 32,
    GX_CTF_RA4_800BA7C0 = 34,
    GX_CTF_RA8_800BA7C0 = 35,
    GX_CTF_A8_800BA7C0 = 39,
    GX_CTF_R8_800BA7C0 = 40,
    GX_CTF_G8_800BA7C0 = 41,
    GX_CTF_B8_800BA7C0 = 42,
    GX_CTF_RG8_800BA7C0 = 43,
    GX_CTF_GB8_800BA7C0 = 44,
    GX_CTF_Z4_800BA7C0 = 48,
    GX_CTF_Z8M_800BA7C0 = 57,
    GX_CTF_Z8L_800BA7C0 = 58,
    GX_CTF_Z16L_800BA7C0 = 60,
} GXTexFmt_800BA7C0;

typedef enum GXTexFilter_800BA7C0 {
    GX_NEAR_800BA7C0,
    GX_LINEAR_800BA7C0,
    GX_NEAR_MIP_NEAR_800BA7C0,
    GX_LIN_MIP_NEAR_800BA7C0,
    GX_NEAR_MIP_LIN_800BA7C0,
    GX_LIN_MIP_LIN_800BA7C0,
} GXTexFilter_800BA7C0;

typedef enum GXAnisotropy_800BA7C0 {
    GX_ANISO_1_800BA7C0,
    GX_ANISO_2_800BA7C0,
    GX_ANISO_4_800BA7C0,
} GXAnisotropy_800BA7C0;

typedef struct GXTexObj_800BA7C0 {
    u32 dummy[8];
} GXTexObj_800BA7C0;

typedef struct GXTexObjInternal_800BA7C0 {
    u32 mode0;
    u32 mode1;
    u32 image0;
    u32 image3;
    void* userData;
    GXTexFmt_800BA7C0 format;
    u32 tlutName;
    u16 loadCount;
    u8 loadFormat;
    u8 flags;
} GXTexObjInternal_800BA7C0;

extern u8 lbl_80478AB0[8];

extern void* memset(void* destination, int value, u32 size);

#define GX_SET_REG_FIELD(reg, size, shift, value)                              \
    do {                                                                        \
        (reg) = ((u32)(reg) & ~(((1U << (size)) - 1) << (shift))) |            \
                ((u32)(value) << (shift));                                      \
    } while (0)

static inline void GXGetTexTileShift_800BA7C0(GXTexFmt_800BA7C0 format,
                                               u32* rowShift,
                                               u32* columnShift) {
    switch (format) {
    case GX_TF_I4_800BA7C0:
    case GX_TF_C4_800BA7C0:
    case GX_TF_CMPR_800BA7C0:
    case GX_CTF_R4_800BA7C0:
    case GX_CTF_Z4_800BA7C0:
        *rowShift = 3;
        *columnShift = 3;
        break;
    case GX_TF_I8_800BA7C0:
    case GX_TF_IA4_800BA7C0:
    case GX_TF_C8_800BA7C0:
    case GX_TF_Z8_800BA7C0:
    case GX_CTF_RA4_800BA7C0:
    case GX_CTF_A8_800BA7C0:
    case GX_CTF_R8_800BA7C0:
    case GX_CTF_G8_800BA7C0:
    case GX_CTF_B8_800BA7C0:
    case GX_CTF_Z8M_800BA7C0:
    case GX_CTF_Z8L_800BA7C0:
        *rowShift = 3;
        *columnShift = 2;
        break;
    case GX_TF_IA8_800BA7C0:
    case GX_TF_RGB565_800BA7C0:
    case GX_TF_RGB5A3_800BA7C0:
    case GX_TF_RGBA8_800BA7C0:
    case GX_TF_C14X2_800BA7C0:
    case GX_TF_Z16_800BA7C0:
    case GX_TF_Z24X8_800BA7C0:
    case GX_CTF_RA8_800BA7C0:
    case GX_CTF_RG8_800BA7C0:
    case GX_CTF_GB8_800BA7C0:
    case GX_CTF_Z16L_800BA7C0:
        *rowShift = 2;
        *columnShift = 2;
        break;
    default:
        *rowShift = *columnShift = 0;
        break;
    }
}

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap,
                       u8 maxLod) {
    u32 tileShiftX;
    u32 tileShiftY;
    u32 tileBytes;
    u32 bufferSize;
    u32 nx;
    u32 ny;
    u32 level;

    GXGetTexTileShift_800BA7C0((GXTexFmt_800BA7C0)format, &tileShiftX,
                               &tileShiftY);
    if (format == GX_TF_RGBA8_800BA7C0 || format == GX_TF_Z24X8_800BA7C0) {
        tileBytes = 64;
    } else {
        tileBytes = 32;
    }

    if (mipmap == 1) {
        bufferSize = 0;
        for (level = 0; level < maxLod; level++) {
            nx = (width + (1 << tileShiftX) - 1) >> tileShiftX;
            ny = (height + (1 << tileShiftY) - 1) >> tileShiftY;
            bufferSize += tileBytes * (nx * ny);
            if (width == 1 && height == 1) {
                break;
            }
            width = width > 1 ? width >> 1 : 1;
            height = height > 1 ? height >> 1 : 1;
        }
    } else {
        nx = (width + (1 << tileShiftX) - 1) >> tileShiftX;
        ny = (height + (1 << tileShiftY) - 1) >> tileShiftY;
        bufferSize = nx * ny * tileBytes;
    }

    return bufferSize;
}

void __GetImageTileCount(GXTexFmt_800BA7C0 format, u16 width, u16 height,
                         u32* rowTiles, u32* columnTiles, u32* planeCount) {
    u32 rowShift;
    u32 columnShift;

    GXGetTexTileShift_800BA7C0(format, &rowShift, &columnShift);
    if (width == 0) {
        width = 1;
    }
    if (height == 0) {
        height = 1;
    }
    *rowTiles = (width + (1 << rowShift) - 1) >> rowShift;
    *columnTiles = (height + (1 << columnShift) - 1) >> columnShift;
    *planeCount =
        (format == GX_TF_RGBA8_800BA7C0 || format == GX_TF_Z24X8_800BA7C0)
            ? 2
            : 1;
}

void GXInitTexObj(GXTexObj_800BA7C0* object, void* image, u16 width,
                 u16 height, GXTexFmt_800BA7C0 format, u32 wrapS, u32 wrapT,
                 u8 mipmap) {
    u32 imageBase;
    u32 maxLod;
    u16 rowShift;
    u16 columnShift;
    u32 rowCount;
    u32 columnCount;
    GXTexObjInternal_800BA7C0* texture =
        (GXTexObjInternal_800BA7C0*)object;

    memset(texture, 0, 0x20);
    GX_SET_REG_FIELD(texture->mode0, 2, 0, wrapS);
    GX_SET_REG_FIELD(texture->mode0, 2, 2, wrapT);
    GX_SET_REG_FIELD(texture->mode0, 1, 4, 1);
    if (mipmap != 0) {
        u8 encodedMaxLod;

        texture->flags |= 1;
        if ((u32)(format - GX_TF_C4_800BA7C0) <= 2U) {
            texture->mode0 = (texture->mode0 & 0xFFFFFF1F) | 0xA0;
        } else {
            texture->mode0 = (texture->mode0 & 0xFFFFFF1F) | 0xC0;
        }
        if (width > height) {
            maxLod = 31 - __cntlzw(width);
        } else {
            maxLod = 31 - __cntlzw(height);
        }
        encodedMaxLod = 16.0f * maxLod;
        GX_SET_REG_FIELD(texture->mode1, 8, 8, encodedMaxLod);
    } else {
        texture->mode0 = (texture->mode0 & 0xFFFFFF1F) | 0x80;
    }

    texture->format = format;
    GX_SET_REG_FIELD(texture->image0, 10, 0, width - 1);
    GX_SET_REG_FIELD(texture->image0, 10, 10, height - 1);
    GX_SET_REG_FIELD(texture->image0, 4, 20, format & 0xF);
    imageBase = ((u32)image >> 5) & 0x01FFFFFF;
    GX_SET_REG_FIELD(texture->image3, 21, 0, imageBase);

    switch (format & 0xF) {
    case GX_TF_I4_800BA7C0:
    case GX_TF_C4_800BA7C0:
        texture->loadFormat = 1;
        rowShift = 3;
        columnShift = 3;
        break;
    case GX_TF_I8_800BA7C0:
    case GX_TF_IA4_800BA7C0:
    case GX_TF_C8_800BA7C0:
        texture->loadFormat = 2;
        rowShift = 3;
        columnShift = 2;
        break;
    case GX_TF_IA8_800BA7C0:
    case GX_TF_RGB565_800BA7C0:
    case GX_TF_RGB5A3_800BA7C0:
    case GX_TF_C14X2_800BA7C0:
        texture->loadFormat = 2;
        rowShift = 2;
        columnShift = 2;
        break;
    case GX_TF_RGBA8_800BA7C0:
        texture->loadFormat = 3;
        rowShift = 2;
        columnShift = 2;
        break;
    case GX_TF_CMPR_800BA7C0:
        texture->loadFormat = 0;
        rowShift = 3;
        columnShift = 3;
        break;
    default:
        texture->loadFormat = 2;
        rowShift = 2;
        columnShift = 2;
        break;
    }

    rowCount = (width + (1 << rowShift) - 1) >> rowShift;
    columnCount = (height + (1 << columnShift) - 1) >> columnShift;
    texture->loadCount = (rowCount * columnCount) & 0x7FFF;
    texture->flags |= 2;
}

void GXInitTexObjCI(GXTexObj_800BA7C0* object, void* image, u16 width,
                    u16 height, GXTexFmt_800BA7C0 format, u32 wrapS,
                    u32 wrapT, u8 mipmap, u32 tlutName) {
    GXTexObjInternal_800BA7C0* texture =
        (GXTexObjInternal_800BA7C0*)object;

    GXInitTexObj(object, image, width, height, format, wrapS, wrapT, mipmap);
    texture->flags &= ~2;
    texture->tlutName = tlutName;
}

void GXInitTexObjLOD(GXTexObj_800BA7C0* object, GXTexFilter_800BA7C0 minFilter,
                 GXTexFilter_800BA7C0 magFilter, f32 minLod, f32 maxLod,
                 f32 lodBias, u8 biasClamp, u8 edgeLod,
                 GXAnisotropy_800BA7C0 maxAnisotropy) {
    u8 encodedBias;
    u8 encodedMinLod;
    u8 encodedMaxLod;
    GXTexObjInternal_800BA7C0* texture =
        (GXTexObjInternal_800BA7C0*)object;

    if (lodBias < -4.0f) {
        lodBias = -4.0f;
    } else if (lodBias >= 4.0f) {
        lodBias = 3.99f;
    }
    encodedBias = 32.0f * lodBias;
    GX_SET_REG_FIELD(texture->mode0, 8, 9, encodedBias);
    GX_SET_REG_FIELD(texture->mode0, 1, 4,
                     magFilter == GX_LINEAR_800BA7C0 ? 1 : 0);
    GX_SET_REG_FIELD(texture->mode0, 3, 5, lbl_80478AB0[minFilter]);
    GX_SET_REG_FIELD(texture->mode0, 1, 8, edgeLod ? 0 : 1);
    texture->mode0 &= 0xFFFDFFFF;
    texture->mode0 &= 0xFFFBFFFF;
    GX_SET_REG_FIELD(texture->mode0, 2, 19, maxAnisotropy);
    GX_SET_REG_FIELD(texture->mode0, 1, 21, biasClamp);

    if (minLod < 0.0f) {
        minLod = 0.0f;
    } else if (minLod > 10.0f) {
        minLod = 10.0f;
    }
    encodedMinLod = 16.0f * minLod;

    if (maxLod < 0.0f) {
        maxLod = 0.0f;
    } else if (maxLod > 10.0f) {
        maxLod = 10.0f;
    }
    encodedMaxLod = 16.0f * maxLod;
    GX_SET_REG_FIELD(texture->mode1, 8, 0, encodedMinLod);
    GX_SET_REG_FIELD(texture->mode1, 8, 8, encodedMaxLod);
}

void GXInitTexObjWrapMode(u32* mode0, u32 wrapS, u32 wrapT) {
    *mode0 = (*mode0 & 0xFFFFFFFC) | wrapS;
    *mode0 = (*mode0 & 0xFFFFFFF3) | (wrapT << 2);
}
