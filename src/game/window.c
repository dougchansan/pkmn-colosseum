#include "dolphin/types.h"

extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);
extern void* fn_800E24B0(u16 handle);
extern void fn_800E209C(u16 handle);

/* 0x80103FFC | 0xA4 */
#pragma push
#pragma peephole off
void* windowAllocMemory(void* p, s32 size)
{
    s32 r31 = size;
    void* r30 = p;

    if (r30 == (void*)0) {
        return (void*)0;
    }
    if (*(u16*)((u8*)r30 + 0xAC) != 0) {
        fn_800E24B0(*(u16*)((u8*)r30 + 0xAC));
        fn_800E209C(*(u16*)((u8*)r30 + 0xAC));
        *(u32*)((u8*)r30 + 0xB0) = 0;
    }
    if (r31 <= 0) {
        return (void*)0;
    }
    {
        u16 h = _toolentryAlloc__FUl((u32)r31);

        *(u16*)((u8*)r30 + 0xAC) = h;
        if (*(u16*)((u8*)r30 + 0xAC) != 0) {
            void* ptr = fn_800E27B0(*(u16*)((u8*)r30 + 0xAC));

            *(void**)((u8*)r30 + 0xB0) = ptr;
        } else {
            return (void*)0;
        }
    }
    return *(void**)((u8*)r30 + 0xB0);
}
#pragma pop
