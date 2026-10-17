/**
 * @file menu_exact_8007B350.c
 * @brief Prepare the GBA upload context and launch its worker.
 */
#include "dolphin/types.h"

typedef struct GbaBootContext {
    u8 alarm[0x28];
    u8 thread_and_work[0x318];
    u8 channel;
    u8 pad_341;
    u8 cancel;
    u8 reject_upload;
    u8 status_only;
    u8 pad_345;
    u8 state;
    u8 pad_347;
    u16 boot_type;
    u8 pad_34A[2];
    u32 device_code;
    u32 game_code;
    u32 crc;
    u32 challenge;
    u8 pad_35C[0x24];
    u8* upload_start;
    u8* upload_end;
    u8 pad_388[0x2000];
    u32 reply[0x410];
} GbaBootContext;

extern u32 lbl_803FAEF8[256];
extern u8* lbl_8047A648;
extern u32 lbl_8047A650;
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, s32 value, u32 size);
extern void OSInitMessageQueue(void* queue, void* messages, u32 count);
extern void _gbaBootThread(u8* context);
extern void OSCreateThread(void* thread, void* entry, void* arg, void* stack,
                           u32 stack_size, u32 priority, u32 attributes);
extern void OSResumeThread(void* thread);

void gbaBootStartThread(GbaBootContext* context, u32 channel, const u8* device_code,
                 u32 region, const u8* name, u32 variant, u32 flags)
{
    u8* upload_start;
    u32 upload_size;
    u8* upload_end;
    u32 payload_size;
    u32 crc;
    u32 crc_limit;
    u32 i;

    memset(context, 0, (u8*)context->reply - (u8*)context);
    context->channel = (u8)channel;
    context->device_code = ((u32)device_code[0] << 24) |
                           ((u32)device_code[1] << 16) |
                           ((u32)device_code[2] << 8) | device_code[3];
    context->status_only = flags & 1;
    context->reject_upload = flags & 2;
    context->crc = *(const u32*)device_code;
    if (region == 0x4A) {
        context->game_code = 0x5053414A;
    } else {
        context->game_code = 0x50534145;
    }

    upload_start = lbl_8047A648;
    upload_end = upload_start + lbl_8047A650;
    context->upload_start = upload_start;
    context->upload_end = upload_end;
    upload_size = upload_end - upload_start;
    payload_size = upload_size - 0x3C;
    upload_start[0x2C] = payload_size;
    upload_start[0x2D] = payload_size >> 8;
    upload_start[0x2E] = payload_size >> 16;
    upload_start[0x2F] = payload_size >> 24;

    for (i = 0; i < 0x20; i++) {
        if (name[i] == 0) {
            break;
        }
        upload_start[i + 4] = name[i];
    }
    for (; i < 0x20; i++) {
        upload_start[i + 4] = 0;
    }

    upload_start[0x28] = 2;
    upload_start[0x29] = (u8)variant;
    upload_start[0x30] = 8;
    upload_start[0x31] = 0;
    upload_start[0x32] = 0;
    upload_start[0x33] = 0;
    memcpy(upload_end - 8, device_code, 4);
    memcpy(upload_end - 4, &context->crc, 4);

    crc = 0xAA478422;
    crc_limit = upload_size - 4;
    i = 0;
    do {
        crc = (crc >> 8) ^
              lbl_803FAEF8[(crc ^ upload_start[i++ + 4]) & 0xFF];
    } while (i != crc_limit);
    upload_start[0] = crc;
    upload_start[1] = crc >> 8;
    upload_start[2] = crc >> 16;
    upload_start[3] = crc >> 24;

    OSInitMessageQueue(context->pad_35C, context->pad_35C + 0x20, 1);
    OSCreateThread(context->thread_and_work, (void*)_gbaBootThread, context,
                   context->reply, 0x2000, 8, 0);
    OSResumeThread(context->thread_and_work);
}
