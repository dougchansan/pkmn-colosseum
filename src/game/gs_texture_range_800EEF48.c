/**
 * @file gs_texture_range_800EEF48.c
 * @brief GStexture (head function stranded by splitter; rest is existing unit game/gs_texture.c)
 *
 * Split from gs_range_800E202C.c (0x800EEF48-0x800EF098) — one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#include "dolphin/types.h"

typedef struct GStextureRange {
    u16 width;
    u16 height;
    u8 pad_04[4];
    s32 format;
    u8 pad_0C[0x1C];
    u16* data;
    u8 pad_2C[0x20];
    u32 dataSize;
    u16 convertCount;
} GStextureRange;

extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void GSmemUnlock(u16 handle);
extern void GSmemFree(u16 handle);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void DCFlushRange(void* address, u32 size);
extern void GXInvalidateTexAll(void);

void GStextureConvertToHW(GStextureRange* texture)
{
    u32 byteCount;
    u16* source;
    u16* converted;
    u16 width;
    u16 pixelCount;
    u16 handle;
    u16 tile;
    u16 row;
    u16 column;
    u16 block;
    u16 sourceIndex;
    u16 i;
    u16 blocksPerRow;

    if (texture->format != 0x44) {
        return;
    }
    texture->convertCount++;
    source = texture->data;
    if (source == NULL) {
        return;
    }

    width = texture->width;
    pixelCount = width * texture->height;
    byteCount = pixelCount * sizeof(u16);
    handle = _toolentryAlloc__FUl(byteCount);
    if (handle == 0) {
        return;
    }

    converted = GSmemLock(handle);
    blocksPerRow = width >> 2;
    for (i = 0; i < pixelCount; i++) {
        block = i >> 4;
        tile = block / blocksPerRow;
        block %= blocksPerRow; /* block column within its tile row */
        row = (i % 16) / 4;
        column = i % 4;
        sourceIndex = ((tile * width + block) << 2) + row * width + column;
        converted[i] = source[sourceIndex];
    }

    memcpy(source, converted, byteCount);
    DCFlushRange(texture->data, texture->dataSize);
    GXInvalidateTexAll();
    texture->convertCount--;
    GSmemUnlock(handle);
    GSmemFree(handle);
}
