#include "dolphin/types.h"

typedef struct TRKBuffer {
    s32 mutex;
    s32 inUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

extern TRKBuffer lbl_803FCE08[3];
extern char lbl_8026F668[];

extern void TRKInitializeMutex_800C0CD0(void* mutex);
extern void TRKAcquireMutex_800C0CC8(void* mutex);
extern void TRKReleaseMutex_800C0CC0(void* mutex);
extern void usr_puts_serial(const char* message);

static TRKBuffer* TRKGetBuffer(s32 index)
{
    TRKBuffer* buffer = NULL;

    if (index >= 0 && index < 3) {
        buffer = &lbl_803FCE08[index];
    }
    return buffer;
}

static void TRKResetBuffer(TRKBuffer* buffer)
{
    buffer->length = 0;
    buffer->position = 0;
}

s32 TRKGetFreeBuffer(s32* bufferID, TRKBuffer** outBuffer)
{
    TRKBuffer* buffer;
    s32 result;
    s32 index;

    *outBuffer = NULL;
    result = 0x300;
    for (index = 0; index < 3; index++) {
        buffer = TRKGetBuffer(index);
        TRKAcquireMutex_800C0CC8(buffer);
        if (buffer->inUse == 0) {
            TRKResetBuffer(buffer);
            buffer->inUse = 1;
            result = 0;
            *outBuffer = buffer;
            *bufferID = index;
            index = 3;
        }
        TRKReleaseMutex_800C0CC0(buffer);
    }

    if (result == 0x300) {
        usr_puts_serial(lbl_8026F668);
    }
    return result;
}

s32 TRKInitializeMessageBuffers(void)
{
    s32 index;

    for (index = 0; index < 3; index++) {
        TRKInitializeMutex_800C0CD0(&lbl_803FCE08[index]);
        TRKAcquireMutex_800C0CC8(&lbl_803FCE08[index]);
        lbl_803FCE08[index].inUse = 0;
        TRKReleaseMutex_800C0CC0(&lbl_803FCE08[index]);
    }
    return 0;
}
