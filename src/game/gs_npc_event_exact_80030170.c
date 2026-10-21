#include "dolphin/types.h"

typedef struct GSnpcEventQueue {
    void* items[13];
    u32 index;
} GSnpcEventQueue;

void* gbaTradeNextPartyEntry(GSnpcEventQueue* queue)
{
    u32 index;

    if (queue == NULL) {
        return NULL;
    }

    index = queue->index;
    if (index >= 13) {
        return NULL;
    }

    queue->index = index + 1;
    return queue->items[index];
}

void menuDCDrawItem(void)
{
}

void menuDCCtrl(void)
{
}
