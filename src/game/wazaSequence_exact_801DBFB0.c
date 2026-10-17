#include "game/battle/battle_waza_types.h"

extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);

WazaSequence* wazaSequenceAllocNode(void)
{
    u16 handle;
    WazaSequence* sequence;

    handle = _toolentryAlloc__FUl(sizeof(WazaSequence));
    if (handle != 0) {
        sequence = GSmemLock(handle);
        memset(sequence, 0, sizeof(WazaSequence));
        sequence->handle = handle;
        return sequence;
    }
    return NULL;
}
