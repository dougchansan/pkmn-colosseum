/**
 * @file gs_thread_hi_range_800FEC34.c
 * @brief Floor resource request queue, .text 0x800FEC34 - 0x800FF0A0.
 *
 * threadReqUnblock / threadReqBlock look up the active (state 3) request with a
 * given callback and unblock / block its thread. threadReqAlloc2, threadReqAlloc1
 * and threadReqAlloc0 take a free slot from one of the three request pools
 * (lbl_8047ACB0 array of lbl_8047ACB4 + lbl_8047ACB8 + lbl_8047ACBC
 * entries), fill it in and insert it into the priority-ordered request list
 * headed by lbl_8047ACCC. floor.c calls them with priority 15.
 *
 * The three request functions share one body (written out per function);
 * each keeps its own pool and active state. Its .sbss state (lbl_8047ACB0 - lbl_8047ACCC) is extern.
 */
#include "dolphin/types.h"
#include "game/gs_floor.h"

extern GSFloorResource* lbl_8047ACB0;
extern u32 lbl_8047ACB4;
extern u32 lbl_8047ACB8;
extern u32 lbl_8047ACBC;
extern u32 lbl_8047ACC0;
extern GSFloorResource* lbl_8047ACCC;

extern void GSthreadUnblock(void* thread);
extern void GSthreadBlock(void* thread);

static inline GSFloorResource* resourceFindCallback(u32 callback)
{
    GSFloorResource* resource = lbl_8047ACB0;
    u32 i;

    for (i = lbl_8047ACC0; i > 0; i--) {
        if (resource->active == 3 && (u32)resource->callback == callback) {
            return resource;
        }
        resource++;
    }
    return NULL;
}

/* 0x800FEC34 | 0x84 */
void threadReqUnblock(u32 callback)
{
    GSFloorResource* resource = resourceFindCallback(callback);

    if (resource != NULL) {
        resource->pending = 0;
        if (resource->status == 1 && resource->modelHandle != NULL) {
            GSthreadUnblock(resource->modelHandle);
        }
    }
}

/* 0x800FECB8 | 0x84 */
void threadReqBlock(u32 callback)
{
    GSFloorResource* resource = resourceFindCallback(callback);

    if (resource != NULL) {
        resource->pending = 1;
        if (resource->status == 1 && resource->modelHandle != NULL) {
            GSthreadBlock(resource->modelHandle);
        }
    }
}

static inline GSFloorResource* resourceFindFree(GSFloorResource* resource, u32 count)
{
    u32 i;

    for (i = count; i > 0; i--) {
        if (resource->active == 0) {
            return resource;
        }
        resource++;
    }
    return NULL;
}

static inline void resourceSetCallback(GSFloorResource* resource, void* callback)
{
    if (resource->status == 0) {
        resource->callback = callback;
    } else {
        resource->callback = callback;
        resource->textureHandle = 0;
        resource->modelHandle = NULL;
    }
}

static inline void resourceInsert(GSFloorResource* resource)
{
    GSFloorResource* current;

    current = lbl_8047ACCC;
    if (current == NULL) {
        lbl_8047ACCC = resource;
        return;
    }
    while (current->next != NULL && current->priority < resource->priority) {
        current = current->next;
    }
    if (current->next == NULL && current->priority < resource->priority) {
        resource->prev = current;
        resource->next = NULL;
        current->next = resource;
        return;
    }
    if (current->prev != NULL) {
        current->prev->next = resource;
    }
    resource->prev = current->prev;
    resource->next = current;
    current->prev = resource;
    if (lbl_8047ACCC == current) {
        lbl_8047ACCC = resource;
    }
}

/* 0x800FED3C | 0x12C: third pool */
void threadReqAlloc2(u8 priority, u32 floorId, void* callback)
{
    GSFloorResource* resource;

    resource = resourceFindFree(lbl_8047ACB0 + (lbl_8047ACB4 + lbl_8047ACB8), lbl_8047ACBC);
    if (resource == NULL) {
        return;
    }
    resource->prev = NULL;
    resource->next = NULL;
    resource->active = 5;
    resource->status = 1;
    resource->floorId = floorId;
    resource->priority = priority;
    resource->pending = 0;
    resourceSetCallback(resource, callback);
    resourceInsert(resource);
}

/* 0x800FEE68 | 0x124: second pool */
void threadReqAlloc1(u8 priority, u32 floorId, void* callback)
{
    GSFloorResource* resource;

    resource = resourceFindFree(lbl_8047ACB0 + lbl_8047ACB4, lbl_8047ACB8);
    if (resource == NULL) {
        return;
    }
    resource->prev = NULL;
    resource->next = NULL;
    resource->active = 3;
    resource->status = 1;
    resource->floorId = floorId;
    resource->priority = priority;
    resource->pending = 0;
    resourceSetCallback(resource, callback);
    resourceInsert(resource);
}

/* 0x800FEF8C | 0x114: first pool */
void threadReqAlloc0(u8 priority, u32 floorId, void* callback)
{
    GSFloorResource* resource;

    resource = resourceFindFree(lbl_8047ACB0, lbl_8047ACB4);
    if (resource == NULL) {
        return;
    }
    resource->prev = NULL;
    resource->next = NULL;
    resource->active = 1;
    resource->status = 1;
    resource->floorId = floorId;
    resource->priority = priority;
    resource->pending = 0;
    resourceSetCallback(resource, callback);
    resourceInsert(resource);
}
