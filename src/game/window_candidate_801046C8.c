/** Exact standalone owner for 0x801046C8 - 0x80104704. */
#include "dolphin/types.h"

void* windowSearchItemID(void* head, s32 key) {
    void* item;
    s32 search_key;

    if (head == NULL) {
        return NULL;
    }
    item = *(void**)((u8*)head + 0x1c);
    search_key = (u16)key;
    while (item != NULL) {
        s16 item_key = *(s16*)((u8*)item + 0x6);
        if ((s32)item_key == search_key) {
            return item;
        }
        item = *(void**)item;
    }
    return NULL;
}
