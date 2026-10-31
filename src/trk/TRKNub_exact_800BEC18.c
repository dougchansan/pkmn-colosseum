/**
 * @file TRKNub_exact_800BEC18.c
 * @brief Exact pure-C MetroTRK 64-bit append helper.
 */
#include "dolphin/types.h"

extern void TRK_memcpy(void* dst, const void* src, u32 size);
extern u8 gTRKBigEndian[];

s32 TRKAppendBuffer1_ui64(u8* buffer, u64 value)
{
    u8 swapped[8];
    u8* source;
    u32 position;
    s32 error;
    u32 count;

    if (*(s32*)gTRKBigEndian != 0) {
        source = (u8*)&value;
    } else {
        swapped[0] = ((u8*)&value)[7];
        swapped[1] = ((u8*)&value)[6];
        swapped[2] = ((u8*)&value)[5];
        swapped[3] = ((u8*)&value)[4];
        swapped[4] = ((u8*)&value)[3];
        swapped[5] = ((u8*)&value)[2];
        swapped[6] = ((u8*)&value)[1];
        swapped[7] = ((u8*)&value)[0];
        source = swapped;
    }

    position = *(u32*)(buffer + 0xC);
    count = 8;
    error = 0;
    if (0x880 - position < 8) {
        error = 0x301;
        count = 0x880 - position;
    }
    if (count == 1) {
        buffer[position + 0x10] = *source;
    } else {
        TRK_memcpy(buffer + position + 0x10, source, count);
    }
    *(u32*)(buffer + 0xC) = *(u32*)(buffer + 0xC) + count;
    *(u32*)(buffer + 0x8) = *(u32*)(buffer + 0xC);
    return error;
}
