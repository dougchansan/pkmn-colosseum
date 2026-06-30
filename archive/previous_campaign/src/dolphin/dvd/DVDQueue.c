#include "dolphin/dvd/dvd.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * DVDQueue.c - DVD command priority queue implementation.
 *
 * Manages a 4-priority-level doubly-linked waiting queue for DVD commands.
 * Priority 0 is highest, 3 is lowest.
 *
 * Matches: 0x800A7DE8 - 0x800A7F80
 */

/* Queue link node - embedded in DVDCommandBlock at offset 0x00/0x04 */
typedef struct DVDQueueNode {
    struct DVDQueueNode* next;
    struct DVDQueueNode* prev;
} DVDQueueNode;

/* 4 priority levels, each is a doubly-linked list sentinel */
/* Located at 0x803FC3F8 */
extern DVDQueueNode WaitingQueue_803FC3F8[4];

/*
 * __DVDClearWaitingQueue_803FC3F8 - Initialize all 4 priority queues as empty
 * 0x800A7DE8 | size: 0x38
 *
 * Each queue is a circular doubly-linked list with a sentinel node.
 * Empty state: sentinel.next == sentinel.prev == &sentinel
 */
void __DVDClearWaitingQueue_803FC3F8(void) {
    DVDQueueNode* q0 = &WaitingQueue_803FC3F8[0];
    DVDQueueNode* q1 = &WaitingQueue_803FC3F8[1];
    DVDQueueNode* q2 = &WaitingQueue_803FC3F8[2];
    DVDQueueNode* q3 = &WaitingQueue_803FC3F8[3];

    q0->next = q0;
    q0->prev = q0;
    q1->next = q1;
    q1->prev = q1;
    q2->next = q2;
    q2->prev = q2;
    q3->next = q3;
    q3->prev = q3;
}

/*
 * __DVDPushWaitingQueue_803FC3F8 - Insert a command block at the tail of a priority queue
 * 0x800A7E20 | size: 0x68
 *
 * Parameters:
 *   prio  - priority level (0-3)
 *   block - command block to enqueue (links at offset 0x00/0x04)
 */
BOOL __DVDPushWaitingQueue_803FC3F8(s32 prio, DVDCommandBlock* block) {
    BOOL enabled;
    DVDQueueNode* sentinel;
    DVDQueueNode* tail;

    enabled = OSDisableInterrupts();

    sentinel = &WaitingQueue_803FC3F8[prio];
    tail = sentinel->prev;

    /* Insert before sentinel (at end of queue) */
    if (tail == sentinel) {
        sentinel->next = (DVDQueueNode*)block;
    } else {
        ((DVDCommandBlock*)tail)->next = block;
    }
    block->prev = (DVDCommandBlock*)tail;
    block->next = (DVDCommandBlock*)sentinel;
    sentinel->prev = (DVDQueueNode*)block;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * __DVDPopWaitingQueue_803FC3F8 - Remove and return the highest-priority waiting command
 * 0x800A7E88 | size: 0xA0
 *
 * Scans priorities 0-3, returns the first non-empty queue's head entry.
 * Returns NULL if all queues are empty.
 */
DVDCommandBlock* __DVDPopWaitingQueue_803FC3F8(void) {
    BOOL enabled;
    DVDQueueNode* sentinel;
    DVDCommandBlock* block;
    DVDCommandBlock* next;
    s32 i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; i++) {
        sentinel = &WaitingQueue_803FC3F8[i];

        if (sentinel->next == sentinel) {
            /* This priority level is empty */
            continue;
        }

        /* Found a non-empty queue */
        OSRestoreInterrupts(enabled);
        enabled = OSDisableInterrupts();

        sentinel = &WaitingQueue_803FC3F8[i];
        block = (DVDCommandBlock*)sentinel->next;
        next = block->next;

        /* Remove from queue */
        if ((DVDQueueNode*)next == sentinel) {
            sentinel->next = sentinel;
            sentinel->prev = sentinel;
        } else {
            sentinel->next = (DVDQueueNode*)next;
            next->prev = (DVDCommandBlock*)sentinel;
        }

        OSRestoreInterrupts(enabled);

        /* Clear links */
        block->next = NULL;
        block->prev = NULL;

        return block;
    }

    OSRestoreInterrupts(enabled);
    return NULL;
}

/*
 * __DVDCheckWaitingQueue_803FC3F8 - Check if any commands are waiting
 * 0x800A7F28 | size: 0x58
 *
 * Returns TRUE if at least one priority queue is non-empty.
 */
BOOL __DVDCheckWaitingQueue_803FC3F8(void) {
    BOOL enabled;
    DVDQueueNode* sentinel;
    s32 i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; i++) {
        sentinel = &WaitingQueue_803FC3F8[i];

        if (sentinel->next != sentinel) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}
