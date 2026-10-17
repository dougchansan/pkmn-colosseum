#include "game/battle/battle_waza_types.h"

extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

WazaSequence* wazaSequenceAllocNode(void)
{
    u16 handle;
    WazaSequence* sequence;

    handle = _toolentryAlloc__FUl(sizeof(WazaSequence));
    if (handle != 0) {
        sequence = fn_800E27B0(handle);
        memset(sequence, 0, sizeof(WazaSequence));
        sequence->handle = handle;
        return sequence;
    }
    return NULL;
}
