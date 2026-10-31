#include "dolphin/types.h"

typedef struct CircleBuffer {
    u8* readPtr;
    u8* writePtr;
    u8* buffer;
    u32 size;
    u32 used;
    u32 free;
    u32 state;
} CircleBuffer;

extern void TRKInitializeMutex_800C459C(u32* state);

void CircleBufferInitialize(CircleBuffer* circle, u8* buffer, u32 size)
{
    circle->buffer = buffer;
    circle->size = size;
    circle->readPtr = circle->buffer;
    circle->writePtr = circle->buffer;
    circle->used = 0;
    circle->free = circle->size;
    TRKInitializeMutex_800C459C(&circle->state);
}
