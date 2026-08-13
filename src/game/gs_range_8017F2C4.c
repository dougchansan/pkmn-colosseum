/**
 * @file gs_range_8017F2C4.c
 * @brief gs-engine code, 0x8017F2C4 - 0x80180C78 (21 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

extern u8 lbl_80452FC8[0x1000];
extern u32 lbl_80453FDC[];

#pragma push
#pragma optimization_level 0
void fn_8017F2C4(u8* destination, const u8* source)
{
    u32 sourceIndex;
    u32 destinationIndex;
    u32 dictionaryIndex;
    u32 flags;
    u32 sourceLimit;
    u32 offset;
    u32 length;
    u32 index;
    u8 value;
    u8 first;
    u8 second;

    sourceIndex = 0;
    destinationIndex = 0;
    dictionaryIndex = 0xFEE;
    flags = 0;
    source += 0x10;
    sourceLimit = lbl_80453FDC[2] - 0x10;

    for (;;) {
        flags >>= 1;
        if ((flags & 0x100) == 0) {
            value = source[sourceIndex++];
            if (sourceIndex > sourceLimit) {
                return;
            }
            flags = value | 0xFF00;
        }

        if (flags & 1) {
            value = source[sourceIndex++];
            if (sourceIndex > sourceLimit) {
                return;
            }
            destination[destinationIndex++] = value;
            lbl_80452FC8[dictionaryIndex] = value;
            dictionaryIndex = (dictionaryIndex + 1) & 0xFFF;
        } else {
            first = source[sourceIndex++];
            if (sourceIndex > sourceLimit) {
                return;
            }
            second = source[sourceIndex++];
            if (sourceIndex > sourceLimit) {
                return;
            }
            offset = first | ((second << 4) & 0xF00);
            length = (second & 0xF) + 2;
            index = 0;
            do {
                value = lbl_80452FC8[(offset + index) & 0xFFF];
                destination[destinationIndex++] = value;
                lbl_80452FC8[dictionaryIndex] = value;
                dictionaryIndex = (dictionaryIndex + 1) & 0xFFF;
                index++;
            } while (index <= length);
        }
    }
}
#pragma pop

typedef struct GsRangeRequest {
    u8 _pad_0[0x20];
    s32 field_20;
    s32 field_24;
    void* field_28;
    s32 field_2C;
    u32 field_30;
    void* field_34;
    void (*callback)(void* arg0, void* arg1);
    void* field_3C;
} GsRangeRequest;

extern void* GSresGetResource(u32 group, u32 handle);
extern void* GSresAllocResourceAlign(u32 size, u32 alignment, u32 loadParam,
                                      u32 loadParam2, void* callback);

void* fn_8017F3F8(u32 group, u32 handle, u32 size)
{
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    void* volatile old = GSresGetResource(group, handle);
    void* buf = GSresAllocResourceAlign(alignedSize, 0x20, group, handle, NULL);
    if (buf) {
        return buf;
    }
    return NULL;
}

typedef struct GsRangeResource {
    u8 _pad_0[0x38];
    void (*release)(struct GsRangeResource* self);
} GsRangeResource;

typedef struct GsRangeReloadResource {
    u8 _pad_0[0x1c];
    u32 version;
    u32 bssSize;
    u8 _pad_24[0x10];
    void (*prolog)(void);
    u8 _pad_38[0x10];
    u32 fixSize;
} GsRangeReloadResource;

void* fn_8017F484(u32 group, u32 handle, u32 size)
{
    extern u16 fn_800E2B00(u32 size, u32 align);
    extern void* fn_800E27B0(u16 handle);
    extern void fn_800F9210(u32 group, u32 handle);
    extern BOOL fn_8009ED4C(void* module, void* bss);
    extern u16 fn_800E202C(void* ptr);
    extern s32 fn_800E24B0(u16 handle);
    extern s32 fn_800E209C(u16 handle);
    extern void DCFlushRange(void* addr, u32 nBytes);
    extern s32 fn_8017F6B4(void* unused, u32 group, u32 handle);

    GsRangeReloadResource* res = (GsRangeReloadResource*)GSresGetResource(group, handle);
    GsRangeReloadResource* newRes;
    void* buf;
    u16 bufHandle;
    u32 alignedSize;
    u32 alignedExtra;
    u16 h;

    if (res->version >= 3) {
        if (res->bssSize != 0) {
            alignedSize = (size + 0x1f) & ~0x1f;
            alignedExtra = (res->bssSize + 0x1f) & ~0x1f;
            bufHandle = fn_800E2B00(alignedSize, 0x20);
            if ((bufHandle & 0xffff) != 0) {
                buf = fn_800E27B0(bufHandle);
            } else {
                buf = NULL;
            }
            memcpy(buf, res, size);
            fn_800F9210(group, handle);
            newRes = (GsRangeReloadResource*)GSresAllocResourceAlign(
                alignedSize + alignedExtra, 0x20, group, handle, fn_8017F6B4);
            memcpy(newRes, buf, alignedSize);
            fn_8009ED4C(newRes, (u8*)newRes + alignedSize);
        } else {
            alignedSize = (size + 0x1f) & ~0x1f;
            bufHandle = fn_800E2B00(alignedSize, 0x20);
            if ((bufHandle & 0xffff) != 0) {
                buf = fn_800E27B0(bufHandle);
            } else {
                buf = NULL;
            }
            memcpy(buf, res, size);
            fn_800F9210(group, handle);
            newRes = (GsRangeReloadResource*)GSresAllocResourceAlign(
                alignedSize, 0x20, group, handle, fn_8017F6B4);
            DCFlushRange(newRes, alignedSize);
            memcpy(newRes, buf, alignedSize);
            fn_8009ED4C(newRes, (u8*)newRes + ((alignedSize + newRes->fixSize + 0x1f) & ~0x1f));
        }
    } else {
        fn_8009ED4C(res, NULL);
        newRes = res;
        buf = NULL;
    }

    if (newRes->prolog != NULL) {
        newRes->prolog();
    }
    if (buf != NULL) {
        h = fn_800E202C(buf);
        if ((h & 0xffff) != 0) {
            fn_800E24B0(h);
            fn_800E209C(h);
        }
    }
    return newRes;
}

extern void fn_8009EFE4(void* res);

s32 fn_8017F6B4(void* unused, u32 group, u32 handle)
{
    GsRangeResource* res = (GsRangeResource*)GSresGetResource(group, handle);
    if (res->release != NULL) {
        res->release(res);
    }
    fn_8009EFE4(res);
    return 1;
}

typedef struct GsRangeNode {
    s32 field_0;
    u8 _pad_4[4];
    struct GsRangeNode* next;
    s32 value;
    u32 key0;
    u32 key1;
    u32 key2;
} GsRangeNode;

extern GsRangeNode* lbl_80454038[];

s32 fn_8017F728(u32 a, u32 b, u32 c)
{
    GsRangeNode* node = lbl_80454038[0];

    while (node != NULL) {
        if (node->key0 == a && node->key1 == b && node->key2 == c) {
            return node->value;
        }
        node = node->next;
    }
    return 0;
}

s32 fn_8017F794(u32 a, u32 b, u32 c)
{
    GsRangeNode* node = lbl_80454038[0];

    while (node != NULL) {
        if (node->key0 == a && node->key1 == b && node->key2 == c) {
            return node->field_0;
        }
        node = node->next;
    }
    return 0;
}

typedef struct GsRangeCacheNode {
    void* task;
    struct GsRangeCacheNode* link4;
    struct GsRangeCacheNode* link8;
    s32 value;
    u32 fileHandle;
    u32 key1;
    u32 key2;
    u32 active;
} GsRangeCacheNode;

void fn_8017F800(u32 fileHandle)
{
    extern void fn_8017FB08(void*);
    GsRangeCacheNode** list = (GsRangeCacheNode**)lbl_80454038;
    GsRangeCacheNode* node = list[0];
    GsRangeCacheNode* link4;
    GsRangeCacheNode* link8;

    while (node != NULL) {
        if (node->active != 0 && node->fileHandle == fileHandle) {
            link8 = node->link8;
            link4 = node->link4;
            if (link8 != NULL) {
                link8->link4 = node->link4;
            }
            if (link4 != NULL) {
                link4->link8 = node->link8;
            }
            ((u32*)lbl_80454038)[3]--;
            node->value = 0;
            node->fileHandle = 0;
            node->key1 = 0;
            node->key2 = 0;
            if (node->task != NULL) {
                fn_8017FB08(node->task);
                node->task = NULL;
            }
            node->active = 0;
            if (list[1] == node) {
                node->link8 = NULL;
                list[1] = node->link4;
            }
        }
        node = node->link8;
    }
}

extern void* fn_8017FDB0(u32 size);

s32 fn_8017F928(s32 size, u32 fileHandle, u32 key1, u32 key2)
{
    GsRangeCacheNode** list = (GsRangeCacheNode**)lbl_80454038;
    GsRangeCacheNode* pool = list[0];
    s32 count = ((s32*)lbl_80454038)[2];
    GsRangeCacheNode* node;
    GsRangeCacheNode* tail;
    void* buf;
    s32 i;

    for (i = 1; i < count; i++) {
        node = &pool[i];
        if ((s32)node->active == 0) {
            node->link8 = NULL;
            node->link4 = list[1];
            buf = fn_8017FDB0((size + 0x1f) & ~0x1f);
            node->task = buf;
            if (buf != NULL) {
                node->value = size;
                node->fileHandle = fileHandle;
                node->key1 = key1;
                node->key2 = key2;
                tail = list[1];
                if (tail != NULL) {
                    tail->link8 = node;
                }
                list[1] = node;
                ((s32*)lbl_80454038)[3]++;
                node->active = 1;
            }
            return (s32)node->task;
        }
    }
    return 0;
}

typedef struct GsRangeMemNode {
    struct GsRangeMemNode* next;
    u32 size;
    void* data;
    struct GsRangeMemNode* previous;
} GsRangeMemNode;

typedef struct GsRangeStats {
    u32 cursorIndex;
    u8* cursor[8];
    s32 totalBase;
} GsRangeStats;

extern GsRangeMemNode* lbl_8047B1D0;
extern GsRangeStats lbl_80455048;

s32 fn_8017FA5C(void)
{
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* node;
    s32 sum = 0;
    volatile s32 count;

    if (head == NULL) {
        return lbl_80455048.totalBase;
    }

    count = 0;
    node = head->next;
    for (;;) {
        count++;
        if ((u32)node <= 0x80000000u) {
            return sum;
        }
        if (node != NULL) {
            sum += node->size;
        }
        if (node == lbl_8047B1D0) {
            break;
        }
        node = node->next;
    }
    return sum + lbl_80455048.totalBase;
}

typedef struct GsRangeDVDQueueEntry {
    u8 _pad00[0x20];
    u32 state;
    s32 mode;
    void* srcPtr;
    void* dstPtr;
    u32 size;
    u32 flag34;
    void (*callback)(void* entry);
    u32 callbackArg;
    u32 index;
} GsRangeDVDQueueEntry;

extern u32 lbl_8047B1D4;
extern u32 lbl_8047B1D8;
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void ARQPostRequest(void* request, u32 owner, u32 direction,
                           u32 priority, u32 source, u32 destination,
                           u32 size, void (*callback)(void*));
extern void fn_800AE630(void* request, void* owner, u32 direction, u32 offset,
                        void* callback, void* callbackArg, void* src,
                        void* dst, u32 size);
extern void DCFlushRange(void* addr, u32 nBytes);
extern u16 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u16 handle);
extern void fn_8017FB08(void*);
void fn_801808E4(volatile GsRangeRequest* req);

typedef struct GsRangeBufferEntry {
    u32 field00;
    u32 field04;
    struct GsRangeBufferEntry* field08;
    u32 field0C;
    u8 pad10[0xC];
    u32 field1C;
} GsRangeBufferEntry;

typedef struct GsRangeBufferPool {
    u32 field00;
    u32 field04;
    u32 field08;
    u32 field0C;
    u32 field10;
    u32 field14;
    u32 field18;
    u32 field1C;
    GsRangeBufferEntry* entries;
    GsRangeBufferEntry* tail;
    u32 count;
    u32 field2C;
} GsRangeBufferPool;

typedef GsRangeMemNode GsRangeDecompEntry;

extern GsRangeBufferPool lbl_80454018;
extern GsRangeDecompEntry lbl_80455070[];
extern GsRangeMemNode lbl_80465070;
extern void fn_8017D624(void);

static GsRangeMemNode* rangeFindFreeDescriptor(void* data)
{
    s32 i;

    for (i = 0; i < 0x1000; i++) {
        if (lbl_80455070[i].data == NULL) {
            lbl_80455070[i].data = data;
            return &lbl_80455070[i];
        }
    }
    return NULL;
}

#pragma push
#pragma optimization_level 0
#pragma peephole off
void fn_8017FB08(void* allocation)
{
    GsRangeMemNode* block;
    GsRangeMemNode* previous;
    GsRangeMemNode* next;
    GsRangeMemNode* scan;
    GsRangeMemNode* scanPrevious;
    s32 i;

    if (allocation == NULL) {
        return;
    }

    block = NULL;
    for (i = 0; i < 0x1000; i++) {
        if (lbl_80455070[i].data == allocation) {
            block = &lbl_80455070[i];
            break;
        }
    }
    if (block == NULL || lbl_8047B1D0 == NULL) {
        return;
    }

    previous = lbl_8047B1D0;
    for (;;) {
        next = previous->next;
        if (block > previous && block < next) {
            break;
        }
        if (previous >= next &&
            (block > previous || block < next)) {
            break;
        }
        previous = next;
    }

    if (block->previous != NULL &&
        (u8*)block->previous->data + block->previous->size == block->data) {
        scan = block->previous;
        scan->size += block->size;
        block->data = NULL;
        block->next = NULL;
        block->previous = NULL;
        block = scan;

        for (;;) {
            scanPrevious = lbl_8047B1D0;
            scan = scanPrevious->next;
            while (scan != lbl_8047B1D0 &&
                   (u8*)block->data + block->size != scan->data) {
                scanPrevious = scan;
                scan = scan->next;
            }
            if ((u8*)block->data + block->size != scan->data) {
                break;
            }
            block->size += scan->size;
            scanPrevious->next = scan->next;
            if (scan == lbl_8047B1D0) {
                lbl_8047B1D0 = scan->next;
            }
            scan->data = NULL;
            scan->previous = NULL;
            scan->next = NULL;
        }
        return;
    }

    next = previous->next;
    if ((u8*)block->data + block->size == next->data) {
        block->size += next->size;
        block->next = next->next;
        next->data = NULL;
    } else {
        block->next = next;
    }

    if ((u8*)previous->data + previous->size == block->data) {
        previous->size += block->size;
        previous->next = block->next;
        block->data = NULL;
    } else {
        previous->next = block;
    }
    lbl_8047B1D0 = previous;
}

#pragma pop
#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_8017FDB0(u32 size)
{
    GsRangeMemNode* block;
    GsRangeMemNode* previous;
    GsRangeMemNode* descriptor;
    void* allocation;
    u32 arenaSize;
    u32 alignedSize;
    u32 cursorIndex;

    alignedSize = (size + 0x1F) & ~0x1F;
    if (lbl_8047B1D0 == NULL) {
        lbl_80465070.next = &lbl_80465070;
        lbl_80465070.size = 0;
        lbl_8047B1D0 = &lbl_80465070;
    }

    for (;;) {
        previous = lbl_8047B1D0;
        block = previous->next;
        for (;;) {
            if ((u32)block <= 0x80000000) {
                return NULL;
            }
            if (block->size >= alignedSize) {
                if (block->size == alignedSize) {
                    previous->next = block->next;
                    descriptor = block;
                } else {
                    block->size -= alignedSize;
                    allocation =
                        (u8*)block->data + (block->size & ~0xF);
                    descriptor = rangeFindFreeDescriptor(allocation);
                    if (descriptor == NULL) {
                        fn_8017D624();
                        descriptor = rangeFindFreeDescriptor(allocation);
                    }
                    if (descriptor == NULL) {
                        return NULL;
                    }
                    descriptor->previous = block;
                    descriptor->size = alignedSize;
                }
                lbl_8047B1D0 = previous;
                return descriptor->data;
            }
            if (block == lbl_8047B1D0) {
                break;
            }
            previous = block;
            block = block->next;
        }

        arenaSize = alignedSize;
        if (arenaSize < 0x20) {
            arenaSize = 0x20;
        }
        cursorIndex = lbl_80455048.cursorIndex;
        if (cursorIndex >= 7 ||
            (u32)lbl_80455048.totalBase < arenaSize) {
            return NULL;
        }
        allocation = lbl_80455048.cursor[cursorIndex];
        lbl_80455048.cursorIndex = cursorIndex + 1;
        lbl_80455048.cursor[cursorIndex + 1] =
            (u8*)allocation + arenaSize;
        lbl_80455048.totalBase -= arenaSize;

        descriptor = rangeFindFreeDescriptor(allocation);
        if (descriptor == NULL) {
            fn_8017D624();
            descriptor = rangeFindFreeDescriptor(allocation);
        }
        if (descriptor == NULL) {
            return NULL;
        }
        descriptor->size = arenaSize;
        fn_8017FB08(descriptor->data);
    }
}
#pragma pop

#pragma optimize_for_size on
void fn_801800F8(u32 queueCount, u32 field04, u32 initialSize)
{
    GsRangeBufferEntry* buffer;
    GsRangeDVDQueueEntry* queue;
    u16 handle;
    u32 size;
    s32 i;
    void* allocation;

    lbl_80454018.entries = 0;
    handle = fn_800E2C04(0x8000, 0x20);
    if (handle != 0) {
        allocation = fn_800E27B0(handle);
    } else {
        allocation = 0;
    }

    lbl_80454018.field0C = 0;
    lbl_80454018.tail = 0;
    lbl_80454018.field18 = 0;
    lbl_80454018.field00 = 0;
    lbl_80454018.entries = allocation;
    lbl_80454018.field04 = 0;
    lbl_80454018.field08 = 0;
    lbl_80454018.field10 = 0;
    lbl_80454018.field14 = 0;
    lbl_80454018.count = 0x400;
    lbl_80454018.field2C = 0;

    buffer = lbl_80454018.entries;
    i = 0;
    while (i < lbl_80454018.count) {
        buffer->field0C = 0;
        buffer->field08 = 0;
        buffer->field04 = 0;
        buffer->field00 = 0;
        buffer->field1C = 0;
        buffer++;
        i++;
    }

    buffer = lbl_80454018.entries;
    buffer->field0C = 0;
    buffer->field08 = buffer + 1;
    lbl_80454018.tail = buffer;

    i = 0;
    while (i < lbl_80454018.count * 4) {
        lbl_80455070[i].data = 0;
        i++;
    }

    lbl_8047B1D8 = queueCount;
    size = (queueCount * sizeof(GsRangeDVDQueueEntry) + 0x1F) & ~0x1F;
    handle = fn_800E2C04(size, 0x20);
    if (handle != 0) {
        queue = fn_800E27B0(handle);
    } else {
        queue = 0;
    }
    lbl_8047B1D4 = (u32)queue;

    i = 0;
    while (i < lbl_8047B1D8) {
        queue->state = 0;
        queue->mode = 0;
        queue->callback = 0;
        queue->callbackArg = 0;
        queue->index = i;
        queue++;
        i++;
    }

    lbl_8047B1D0 = 0;
    lbl_80455048.cursorIndex = 0;
    lbl_80455048.cursor[0] = (u8*)field04;
    lbl_80455048.totalBase = initialSize;
    allocation = fn_8017FDB0(initialSize);
    fn_8017FB08(allocation);
}
#pragma optimize_for_size reset

#pragma push
#pragma optimization_level 3
#pragma peephole off
void fn_80180320(void* dst, void* src, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;

    if (size == 0) {
        return;
    }

    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    result = NULL;
    for (i = 0; i < lbl_8047B1D8; i++, entry++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    alignedSize = (size + 0x1F) & ~0x1F;
    entry->flag34 = 1;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = dst;
    entry->dstPtr = src;
    entry->size = alignedSize;
    DCFlushRange(dst, size);
    ARQPostRequest(entry, (u32)entry, 1, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);

    while (entry->state != 0) {
        if (entry->mode != 1) {
            entry->state = 0;
        }
    }
}
#pragma pop

void* fn_80180450(void* src, void* dst, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if ((s32)entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, alignedSize);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);

    result = entry;
    while ((s32)result->state != 0) {
        if (result->mode != 1) {
            result->state = 0;
        }
    }
    return result;
}

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_80180584(void* src, void* dst, u32 size, u32 cbA, u32 cbB)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 1;
    entry->mode = 1;
    entry->callback = (void (*)(void*))cbA;
    entry->callbackArg = cbB;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, size);
    ARQPostRequest(entry, (u32)entry, 1, 0, (u32)dst, (u32)src,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_80180694(void* src, void* dst, u32 size, u32 cbA, u32 cbB)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if (entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = (void (*)(void*))cbA;
    entry->callbackArg = cbB;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, size);
    ARQPostRequest(entry, (u32)entry, 0, 0, (u32)src, (u32)dst,
                   alignedSize, (void (*)(void*))fn_801808E4);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

#pragma push
#pragma optimization_level 3
#pragma peephole off
void* fn_801807A8(void* src, void* dst, u32 size)
{
    GsRangeDVDQueueEntry* entry;
    GsRangeDVDQueueEntry* result;
    u32 i;
    u32 alignedSize;
    u32 savedIntr;
    u32 count;

    if (size == 0) {
        return NULL;
    }

    alignedSize = (size + 0x1F) & ~0x1F;
    entry = (GsRangeDVDQueueEntry*)lbl_8047B1D4;
    count = lbl_8047B1D8;
    result = NULL;
    for (i = 0; i < count; i++) {
        if ((s32)entry->state == 0) {
            entry->state = 1;
            result = entry;
            break;
        }
        entry++;
    }

    entry = result;
    savedIntr = OSDisableInterrupts();
    entry->flag34 = 0;
    entry->mode = 1;
    entry->callback = NULL;
    entry->callbackArg = 0;
    entry->srcPtr = src;
    entry->dstPtr = dst;
    entry->size = alignedSize;
    DCFlushRange(src, alignedSize);
    fn_800AE630(entry, entry, 0, 0, fn_801808E4, entry, src, dst,
                alignedSize);
    OSRestoreInterrupts(savedIntr);
    return entry;
}
#pragma pop

void fn_801808E4(volatile GsRangeRequest* req)
{
    void (*cb)(void*, void*);

    req->field_24 = 0;
    if (req->callback != NULL) {
        cb = req->callback;
        cb((void*)req->field_34, (void*)req->field_3C);
    }
    req->field_20 = 0;
    DCFlushRange((void*)req->field_28, req->field_30);
}

typedef struct GsRangeSlotInfo {
    u8 pad00[0xF8];
    void* taskParam;
} GsRangeSlotInfo;

typedef struct GsRangePoolElem {
    s32 active;
    s32 field_4;
    void (*callback)(void*, void*);
    s32 state;
    s32 field_10;
    s32 type;
    void* app;
    struct GsRangePoolElem* nextJob;
    GsRangeSlotInfo* slot;
    u32 index;
    void* subEntry;
    u8 _pad_2C[0x14];
} GsRangePoolElem;

typedef struct GsRangePoolInfo {
    s32 count;
    GsRangePoolElem* base;
} GsRangePoolInfo;

extern GsRangePoolInfo lbl_8047B1E8;
extern void* lbl_8047B1E0;
extern GsRangePoolElem* lbl_8047B1E4;

extern u16 fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E202C(void*);
extern void fn_800E24B0(u16);
extern void fn_800E209C(u16);
extern void fn_8017C1D8(void*, void*, u32, void*);
extern void fn_8017C074(void*, void*, u32, void*);
extern u32 fn_8017AC30(void);
extern void* GSgappCreate(s32, u8, void*, void*);
extern void fn_8018114C(void);
extern void fn_80181224(void);
extern void* fn_80167F28(const char*);
extern u32 fn_80167E5C(void*);
extern void fn_80167E64(void*);
extern const char lbl_80273F80[];

void fn_8018094C(void)
{
    GsRangePoolElem* entry = lbl_8047B1E8.base;
    GsRangePoolElem* job;
    void* file;
    void* allocation;
    u16 handle;
    u32 size;
    s32 i;

    for (i = 0; i < lbl_8047B1E8.count; i++, entry++) {
        if (entry->active != 1) {
            continue;
        }

        if (entry->callback != 0) {
            entry->callback(entry->slot, entry->subEntry);
            return;
        }
        if (entry->app == 0) {
            continue;
        }
        if (entry->state == 1) {
            return;
        }
        if (entry->state == 2) {
            if (entry->type == 0) {
                fn_8017C1D8(entry->slot, entry->subEntry, entry->index, entry);
            }
            entry->state = 0;
            return;
        }

        if (lbl_8047B1E0 != 0) {
            handle = fn_800E202C(lbl_8047B1E0);
            if (handle != 0) {
                fn_800E24B0(handle);
                fn_800E209C(handle);
            }
            lbl_8047B1E0 = 0;
        }

        entry->active = 0;
        entry->app = 0;
        entry->state = 0;
        if (entry->nextJob == 0) {
            lbl_8047B1E4 = 0;
            return;
        }

        job = entry->nextJob;
        if (job->type == 0) {
            lbl_8047B1E4 = job;
            fn_8017C074(job->slot, job->subEntry, job->index, job);
            job->app = GSgappCreate(fn_8017AC30(), 0xC8,
                                     job->slot->taskParam, fn_8018114C);
            if (job->app != 0) {
                job->active = 1;
                job->state = 1;
                lbl_8047B1E4 = job;
            }
            return;
        }

        job->app = GSgappCreate(2, 0x1E, 0, fn_80181224);
        if (job->app != 0) {
            job->active = 1;
            job->state = 1;
            file = fn_80167F28(lbl_80273F80);
            size = fn_80167E5C(file);
            fn_80167E64(file);
            handle = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
            if (handle != 0) {
                allocation = fn_800E27B0(handle);
            } else {
                allocation = 0;
            }
            lbl_8047B1E0 = allocation;
            lbl_8047B1E4 = job;
        }
        return;
    }
}

#pragma optimize_for_size on
void fn_80180B94(s32 count)
{
    s32 size = count * 0x40;
    u32 alignedSize = (size + 0x1F) & ~0x1F;
    u16 handle;
    GsRangePoolElem* elem;
    s32 i;

    lbl_8047B1E8.count = count;
    handle = fn_800E2C04(alignedSize, 0x20);
    if (handle != 0) {
        lbl_8047B1E8.base = fn_800E27B0(handle);
    } else {
        lbl_8047B1E8.base = NULL;
    }
    lbl_8047B1E0 = 0;
    lbl_8047B1E4 = 0;

    elem = lbl_8047B1E8.base;
    for (i = 0; i < count; i++) {
        elem->active = 0;
        elem->field_4 = 0;
        elem->callback = 0;
        elem->state = 0;
        elem->field_10 = 0;
        elem->app = 0;
        elem->nextJob = 0;
        elem->slot = 0;
        elem->subEntry = 0;
        elem++;
    }
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
s32 fn_801808B4(volatile GsRangeRequest* req)
{
    volatile GsRangeRequest* ptr = req;
    s32 out;

    if (ptr->field_24 != 1) {
        ptr->field_20 = 0;
    }
    out = ptr->field_20;
    return out;
}
#pragma optimize_for_size reset
