#include "crt/string.h"

extern void GXDrawDone(void);
extern void fn_800B856C(void);
extern void GStextureFree(void* texture);

u32 _auraDestroy(void* arg)
{
    void* ptr;

    if (arg != 0) {
        ptr = arg;
        if (*(void**)((u8*)ptr + 4) != 0) {
            GXDrawDone();
            fn_800B856C();
            GStextureFree(*(void**)((u8*)ptr + 4));
        }
        memset(ptr, 0, 0x34);
    }
    return 1;
}
