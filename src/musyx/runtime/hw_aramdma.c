/**
 * @file hw_aramdma.c
 * @brief MusyX ARAM transfer queue and ARAM allocator, 0x80163214 - 0x80163EE0.
 *
 * Follows the reference MusyX runtime's hw_aramdma.c (AxioDL/musyx), in its
 * pre-2.0.1 form (one global aramWrite, no ARAMInfo). The queues, the
 * stream-buffer pool and the allocator state are the reference's statics;
 * they stay extern here (.bss/.sbss owned by the data units) under their
 * symbol-map names.
 *
 * Retail inlines aramUploadData into aramInit and twice into aramStoreData,
 * which only happens with its body in the same TU ahead of them, so the
 * text carves that used to split this file could not reproduce those two
 * functions. aramQueueInit is the reference's static helper, inlined into
 * aramInit; its unused out-of-line copy is dead-stripped at link.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"

typedef struct ARQRequest {
    struct ARQRequest* next;
    u32 owner;
    u32 type;
    u32 priority;
    u32 source;
    u32 dest;
    u32 length;
    void (*callback)(u32);
} ARQRequest;

typedef struct ARAMTransferJob {
    ARQRequest arq;
    void (*callback)(u32);
    u32 user;
} ARAMTransferJob;

typedef struct ARAMTransferQueue {
    ARAMTransferJob queue[16];
    volatile u8 write;
    volatile u8 valid;
} ARAMTransferQueue;

typedef struct STREAM_BUFFER {
    struct STREAM_BUFFER* next;
    u32 aram;
    u32 length;
    u32 allocLength;
} STREAM_BUFFER;

extern ARAMTransferQueue lbl_8044FB90;              /* aramQueueLo */
extern ARAMTransferQueue lbl_8044FE14;              /* aramQueueHi */
extern STREAM_BUFFER lbl_80450098[64];              /* aramStreamBuffers */
extern STREAM_BUFFER* lbl_8047B060;                 /* aramIdleStreamBuffers */
extern STREAM_BUFFER* lbl_8047B064;                 /* aramFreeStreamBuffers */
extern STREAM_BUFFER* lbl_8047B068;                 /* aramUsedStreamBuffers */
extern u32 lbl_8047B06C;                            /* aramUploadChunkSize */
extern void* (*lbl_8047B070)(u32, u32);             /* aramUploadCallback */
extern u32 lbl_8047B074;                            /* aramStream */
extern u32 lbl_8047B078;                            /* aramWrite */
extern u32 lbl_8047B07C;                            /* aramTop */

extern u32 ARGetBaseAddress(void);
extern u32 ARGetSize(void);
extern u32 ARQGetChunkSize(void);
extern void ARQPostRequest(ARQRequest* request, u32 owner, u32 type, u32 priority,
                           u32 source, u32 dest, u32 length, void (*callback)(u32));
extern void DCFlushRange(void* addr, u32 size);
extern void* salMalloc(u32 size);                 /* salMalloc */
extern void salFree(void* ptr);                 /* salFree */

void InitStreamBuffers(void);

static void aramQueueInit(void)
{
    lbl_8044FB90.write = lbl_8044FB90.valid = 0;
    lbl_8044FE14.write = lbl_8044FE14.valid = 0;
}

void aramQueueCallback(u32 ptr)
{
    u32 i;
    ARQRequest* arq;
    ARAMTransferQueue* aramQueue;

    arq = (ARQRequest*)ptr;
    if (arq->priority == 1) {
        aramQueue = &lbl_8044FE14;
    } else {
        aramQueue = &lbl_8044FB90;
    }

    for (i = 0; i < 16; ++i) {
        if (arq == &aramQueue->queue[i].arq && aramQueue->queue[i].callback) {
            aramQueue->queue[i].callback(aramQueue->queue[i].user);
        }
    }

    --aramQueue->valid;
}

void aramUploadData(void* mram, u32 aram, u32 len, u32 highPrio, void (*callback)(u32),
                    u32 user)
{
    ARAMTransferQueue* aramQueue;
    int old;

    aramQueue = highPrio != 0 ? &lbl_8044FE14 : &lbl_8044FB90;

    for (;;) {
        old = OSDisableInterrupts();
        if (aramQueue->valid < 16) {
            aramQueue->queue[aramQueue->write].arq.owner = 42;
            aramQueue->queue[aramQueue->write].arq.type = 0;
            aramQueue->queue[aramQueue->write].arq.priority = highPrio != 0 ? 1 : 0;
            aramQueue->queue[aramQueue->write].arq.source = (u32)mram;
            aramQueue->queue[aramQueue->write].arq.dest = aram;
            aramQueue->queue[aramQueue->write].arq.length = len;
            aramQueue->queue[aramQueue->write].arq.callback = aramQueueCallback;
            aramQueue->queue[aramQueue->write].callback = callback;
            aramQueue->queue[aramQueue->write].user = user;
            ARQPostRequest(&aramQueue->queue[aramQueue->write].arq,
                           aramQueue->queue[aramQueue->write].arq.owner,
                           aramQueue->queue[aramQueue->write].arq.type,
                           aramQueue->queue[aramQueue->write].arq.priority,
                           aramQueue->queue[aramQueue->write].arq.source,
                           aramQueue->queue[aramQueue->write].arq.dest,
                           aramQueue->queue[aramQueue->write].arq.length,
                           aramQueue->queue[aramQueue->write].arq.callback);
            ++aramQueue->valid;
            aramQueue->write = (aramQueue->write + 1) % 16;
            OSRestoreInterrupts(old);
            return;
        }
        OSRestoreInterrupts(old);
    }
}

void aramSyncTransferQueue(void) /* aramSyncTransferQueue */
{
    while (lbl_8044FB90.valid != 0) {
    }
}

void aramInit(u32 length) /* aramInit */
{
    s16* tmpMem;
    u32 i;
    u32 aramBase;

    aramBase = ARGetBaseAddress();
    tmpMem = (s16*)salMalloc(sizeof(s16) * 640);

    for (i = 0; i < 640; ++i) {
        tmpMem[i] = 0;
    }

    DCFlushRange(tmpMem, sizeof(s16) * 640);
    aramQueueInit();
    aramUploadData(tmpMem, aramBase, sizeof(s16) * 640, 0, NULL, 0);
    aramSyncTransferQueue();
    salFree(tmpMem);
    lbl_8047B07C = aramBase + length;
    if (lbl_8047B07C > ARGetSize()) {
        lbl_8047B07C = ARGetSize();
    }

    lbl_8047B078 = aramBase + sizeof(s16) * 640;
    lbl_8047B070 = NULL;
    InitStreamBuffers();
}

void aramExit(void) /* aramExit */
{
}

u32 aramGetZeroBuffer(void) /* aramGetZeroBuffer */
{
    return ARGetBaseAddress();
}

void aramSetUploadCallback(void* (*callback)(u32, u32), u32 chunckSize)
{
    u32 acs;

    if (callback != NULL) {
        chunckSize = (chunckSize + 31) & ~31;
        acs = ARQGetChunkSize();
        lbl_8047B06C = chunckSize < acs ? acs : chunckSize;
    }

    lbl_8047B070 = callback;
}

void* aramStoreData(void* src, u32 len) /* aramStoreData */
{
    u32 addr;
    void* buffer;
    u32 blkSize;

    len = (len + 31) & ~31;
    addr = lbl_8047B078;
    if (lbl_8047B070 == NULL) {
        DCFlushRange(src, len);
        aramUploadData(src, lbl_8047B078, len, 0, NULL, 0);
        lbl_8047B078 += len;
        return (void*)addr;
    }

    while (len != 0) {
        blkSize = len >= lbl_8047B06C ? lbl_8047B06C : len;
        buffer = lbl_8047B070((u32)src, blkSize);
        DCFlushRange(buffer, blkSize);
        aramUploadData(buffer, lbl_8047B078, blkSize, 0, NULL, 0);
        len -= blkSize;
        lbl_8047B078 += blkSize;
        src = (void*)((u32)src + blkSize);
    }

    return (void*)addr;
}

void aramRemoveData(void* aram, u32 len) /* aramRemoveData */
{
    len = (len + 31) & ~31;
    lbl_8047B078 -= len;
}

void InitStreamBuffers(void)
{
    u32 i;

    lbl_8047B068 = NULL;
    lbl_8047B064 = NULL;
    lbl_8047B060 = lbl_80450098;
    for (i = 1; i < 64; ++i) {
        lbl_80450098[i - 1].next = &lbl_80450098[i];
    }
    lbl_80450098[i - 1].next = NULL;
    lbl_8047B074 = lbl_8047B07C;
}

u8 aramAllocateStreamBuffer(u32 len) /* aramAllocateStreamBuffer */
{
    STREAM_BUFFER* sb;
    STREAM_BUFFER* oSb;
    STREAM_BUFFER* lastSb;
    u32 minLen;

    len = (len + 31) & ~31;
    lastSb = oSb = NULL;
    minLen = -1;

    for (sb = lbl_8047B064; sb != NULL; sb = sb->next) {
        if (sb->allocLength == len) {
            oSb = sb;
            break;
        }

        if (sb->allocLength > len && minLen > sb->allocLength) {
            oSb = sb;
            minLen = sb->allocLength;
        }
        lastSb = sb;
    }

    if (oSb == NULL) {
        if (lbl_8047B060 != NULL && lbl_8047B074 - len >= lbl_8047B078) {
            oSb = lbl_8047B060;
            lbl_8047B060 = oSb->next;
            oSb->allocLength = len;
            oSb->length = len;
            lbl_8047B074 -= len;
            oSb->aram = lbl_8047B074;
            oSb->next = lbl_8047B068;
            lbl_8047B068 = oSb;
        }
    } else {
        if (lastSb != NULL) {
            lastSb->next = oSb->next;
        } else {
            lbl_8047B064 = oSb->next;
        }

        oSb->length = len;
        oSb->next = lbl_8047B068;
        lbl_8047B068 = oSb;
    }

    if (oSb == NULL) {
        return 0xFF;
    }

    return oSb - lbl_80450098;
}

u32 aramGetStreamBufferAddress(u8 id, u32* len)
{
    if (len != NULL) {
        *len = lbl_80450098[id].length;
    }

    return lbl_80450098[id].aram;
}

void aramFreeStreamBuffer(u8 id)
{
    STREAM_BUFFER* fSb;
    STREAM_BUFFER* sb;
    STREAM_BUFFER* lastSb;
    STREAM_BUFFER* nextSb;
    u32 minAddr;

    fSb = &lbl_80450098[id];
    lastSb = NULL;
    sb = lbl_8047B068;

    while (sb != NULL) {
        if (sb == fSb) {
            if (lastSb != NULL) {
                lastSb->next = fSb->next;
            } else {
                lbl_8047B068 = fSb->next;
            }
            break;
        } else {
            lastSb = sb;
            sb = sb->next;
        }
    }

    if (fSb->aram == lbl_8047B074) {
        fSb->next = lbl_8047B060;
        lbl_8047B060 = fSb;
        minAddr = -1;
        sb = lbl_8047B068;
        while (sb != NULL) {
            if (sb->aram <= minAddr) {
                minAddr = sb->aram;
            }
            sb = sb->next;
        }

        sb = lbl_8047B064;
        while (sb != NULL) {
            nextSb = sb->next;
            if (sb->aram < minAddr) {
                lbl_8047B064 = sb->next;
                sb->next = lbl_8047B060;
                lbl_8047B060 = sb;
            }
            sb = nextSb;
        }

        lbl_8047B074 = minAddr != -1 ? minAddr : lbl_8047B07C;
        return;
    }

    fSb->next = lbl_8047B064;
    lbl_8047B064 = fSb;
}
