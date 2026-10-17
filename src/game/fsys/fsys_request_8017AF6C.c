/**
 * @file fsys_request_8017AF6C.c
 * @brief FSYS file requests against an already-known archive
 *        (0x8017AF6C - 0x8017B07C).
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code (see configure.py).
 */
#include "dolphin/types.h"
#include "game/fsys/fsys.h"

extern FSYSSlot* fsysFindSlotByHandle(u32 fileHandle, s32 mode);
extern u8 fsysLoadEntrySync(FSYSSlot* slot);
extern void fsysStartLoadMode3(FSYSSlot* slot, u32 fileHandle, u32 callbackA,
                        u32 callbackB, u32 callbackC);

/* Address: 0x8017AF6C | size: 0x94
 * Retail keeps an unreachable "b" after the "return 1" branch
 * (0x8017AFD8: b 0x8017AFE4, the jump over the outer else; the beq before
 * it was chained straight to the final return). At optimisation level 0
 * that jump only survives when a statement that compiles to nothing
 * follows the inner if, e.g. a debug report stripped from the release
 * build; with the if as the last statement, or an empty statement, empty
 * block or do-while(0) there, it is dropped. Same idiom as the stripped
 * test in fsysStartLoadMode1. */
s32 fn_8017AF6C(u32 fileHandle, u32 requestID)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 3);
    if (slot->fileHandle == fileHandle) {
        slot->requestID = requestID;
        slot->callbackA = 0;
        slot->callbackB = 0;
        slot->callbackC = 0;
        if (fsysLoadEntrySync(slot)) {
            return 1;
        }
        (void)0;
    } else {
        return 0;
    }
    return 0;
}

/* Address: 0x8017B000 | size: 0x7C */
s32 fn_8017B000(u32 fileHandle, u32 requestID, u32 callbackA, u32 callbackB,
                u32 callbackC)
{
    FSYSSlot* slot;

    slot = fsysFindSlotByHandle(fileHandle, 3);
    if (slot) {
        slot->requestID = requestID;
        fsysStartLoadMode3(slot, fileHandle, callbackA, callbackB, callbackC);
        return 1;
    }
    return 0;
}
