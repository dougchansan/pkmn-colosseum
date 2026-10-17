#include "crt/string.h"

extern void GXDrawDone(void);
extern void GXInvalidateVtxCache(void);
extern void GStextureFree(void* texture);

u32 fn_8013E54C(void* arg)
{
    void* ptr;

    if (arg != 0) {
        ptr = arg;
        if (*(void**)((u8*)ptr + 4) != 0) {
            GXDrawDone();
            GXInvalidateVtxCache();
            GStextureFree(*(void**)((u8*)ptr + 4));
        }
        memset(ptr, 0, 0x34);
    }
    return 1;
}
