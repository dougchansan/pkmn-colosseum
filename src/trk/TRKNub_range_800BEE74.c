#include "dolphin/types.h"

extern void TRK_memset(void* dst, s32 val, u32 len);

typedef struct TRKBuffer {
    s32 mutex;
    s32 inUse;
    u32 position;
    u32 length;
    u8 data[0x880];
} TRKBuffer;

/* TRKResetBuffer - 0x800BEE74 | size 0x40 | scope none (TRKResetBuffer) */
void TRKResetBuffer(u8* buf, s32 keepData) {
    *(u32*)(buf + 0x8) = 0;
    *(u32*)(buf + 0xC) = 0;
    if (keepData == 0) {
        TRK_memset(buf + 0x10, 0, 0x880);
    }
}

/* TRKReleaseBuffer - 0x800BEEB4 | size 0x64 | scope global */
void TRKReleaseBuffer(s32 bufferID) {
    extern TRKBuffer lbl_803FCE08[];
    extern void TRKReleaseMutex_800C0CC0(void* mutex);
    extern void TRKAcquireMutex_800C0CC8(void* mutex);
    TRKBuffer* buffer;

    if (bufferID != -1 && bufferID >= 0 && bufferID < 3) {
        buffer = &lbl_803FCE08[bufferID];
        TRKAcquireMutex_800C0CC8(&buffer->mutex);
        buffer->inUse = 0;
        TRKReleaseMutex_800C0CC0(&buffer->mutex);
    }
}
