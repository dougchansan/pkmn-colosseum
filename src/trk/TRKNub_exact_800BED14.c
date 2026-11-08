/**
 * @file TRKNub_exact_800BED14.c
 * @brief Exact pure-C MetroTRK raw buffer helpers.
 */
#include "dolphin/types.h"

typedef struct TRKMessageBuffer {
    s32 mutex;
    s32 inUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKMessageBuffer;

extern void TRK_memcpy(void* dst, const void* src, u32 size);

s32 TRKReadBuffer(TRKMessageBuffer* buffer, u8* destination, u32 count)
{
    s32 error = 0;
    if (count == 0) {
        return 0;
    }
    {
        u32 position = buffer->position;
        u32 length = buffer->length;
        u32 available = length - position;
        if (count > available) {
            error = 0x302;
            count = available;
        }
        TRK_memcpy(destination, &buffer->data[position], count);
        buffer->position += count;
    }
    return error;
}

s32 TRKAppendBuffer(TRKMessageBuffer* buffer, u8* source, u32 count)
{
    s32 error = 0;
    if (count == 0) {
        return 0;
    }
    {
        u32 position = buffer->position;
        u32 available = sizeof(buffer->data) - position;
        if (available < count) {
            error = 0x301;
            count = available;
        }
        if (count == 1) {
            buffer->data[position] = source[0];
        } else {
            TRK_memcpy(&buffer->data[position], source, count);
        }
        buffer->position += count;
        buffer->length = buffer->position;
    }
    return error;
}
