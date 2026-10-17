#include "dolphin/types.h"

typedef struct CircleBuffer {
    u8* readPtr;
    u8* writePtr;
    u8* buffer;
    u32 size;
    u32 used;
    u32 free;
} CircleBuffer;

u32 CBGetBytesAvailableForRead(CircleBuffer* circle) {
    return circle->used;
}
