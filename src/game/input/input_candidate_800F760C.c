/**
 * @file input_candidate_800F760C.c
 * @brief GS script unregister routine at 0x800F760C.
 */
#include "game/input/input.h"

extern void GSlogWritef(const char* fmt, ...);
extern GSVMPool* lbl_80478B00;
extern const char lbl_802712B8[];

/* Unlink `script` from the pool's script list, then mark every context
 * running it for termination (state 3). */
s32 cms_UnregisterScript(GSVMScript* script) {
    GSVMPool* pool = lbl_80478B00;
    GSVMScript* prev;
    GSVMScript* cur;
    u32 offset;
    GSVMContext* ctx;
    s32 i;
    u8 state;

    cur = pool->scripts;
    if (cur == script) {
        pool->scripts = cur->next;
    } else {
        prev = cur;
        while ((cur = prev->next) != NULL) {
            if (cur == script) {
                prev->next = cur->next;
                goto scan;
            }
            prev = cur;
        }
        if (cur == NULL) {
            GSlogWritef(lbl_802712B8, script);
            return -1;
        }
    }

scan:
    i = 0;
    offset = 0;
    state = 3;
    while (i < (s32)lbl_80478B00->count) {
        ctx = (GSVMContext*)((u8*)lbl_80478B00->contexts + offset);
        if (ctx->state != 0) {
            if (script->id == (u16)(ctx->entryId >> 16)) {
                ctx->state = state;
            }
        }
        offset += sizeof(GSVMContext);
        i++;
    }
    return 0;
}
