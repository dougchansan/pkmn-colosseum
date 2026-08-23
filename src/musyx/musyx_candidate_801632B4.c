/**
 * @file musyx_candidate_801632B4.c
 * @brief Standalone owner for aramUploadData at 0x801632B4.
 */

#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"

typedef struct PFArqRequest {
    u32 next;          /* 0x00 */
    u32 owner;         /* 0x04 */
    u32 type;          /* 0x08 */
    u32 priority;      /* 0x0C */
    u32 source;        /* 0x10 */
    u32 dest;          /* 0x14 */
    u32 length;        /* 0x18 */
    void (*callback)(u32); /* 0x1C */
} PFArqRequest;

typedef struct PFAramQueueEntry {
    PFArqRequest arq;  /* 0x00 */
    void (*callback)(u32); /* 0x20 */
    u32 user;          /* 0x24 */
} PFAramQueueEntry;

typedef struct PFAramQueue {
    PFAramQueueEntry entries[16]; /* 0x000 */
    volatile u8 writeIndex;       /* 0x280 */
    volatile u8 count;            /* 0x281 */
} PFAramQueue;

extern u8 lbl_8044FB90[];
extern u8 lbl_8044FE14[];
extern void aramQueueCallback(u32 ptr);
extern void ARQPostRequest(PFArqRequest* request, u32 owner, u32 type,
                           u32 priority, u32 source, u32 dest, u32 length,
                           void (*callback)(u32));

void aramUploadData(void* mram, u32 aram, u32 size, u32 highPriority,
                    void (*callback)(u32), u32 user) {
    PFAramQueue* queue;
    s32 old;

    queue = highPriority != 0 ? (PFAramQueue*)lbl_8044FE14
                              : (PFAramQueue*)lbl_8044FB90;

    for (;;) {
        old = OSDisableInterrupts();
        if (queue->count < 16) {
            queue->entries[queue->writeIndex].arq.owner = 42;
            queue->entries[queue->writeIndex].arq.type = 0;
            queue->entries[queue->writeIndex].arq.priority =
                highPriority != 0 ? 1 : 0;
            queue->entries[queue->writeIndex].arq.source = (u32)mram;
            queue->entries[queue->writeIndex].arq.dest = aram;
            queue->entries[queue->writeIndex].arq.length = size;
            queue->entries[queue->writeIndex].arq.callback = aramQueueCallback;
            queue->entries[queue->writeIndex].callback = callback;
            queue->entries[queue->writeIndex].user = user;
            ARQPostRequest(
                &queue->entries[queue->writeIndex].arq,
                queue->entries[queue->writeIndex].arq.owner,
                queue->entries[queue->writeIndex].arq.type,
                queue->entries[queue->writeIndex].arq.priority,
                queue->entries[queue->writeIndex].arq.source,
                queue->entries[queue->writeIndex].arq.dest,
                queue->entries[queue->writeIndex].arq.length,
                queue->entries[queue->writeIndex].arq.callback);
            ++queue->count;
            queue->writeIndex = (queue->writeIndex + 1) % 16;
            OSRestoreInterrupts(old);
            return;
        }
        OSRestoreInterrupts(old);
    }
}
