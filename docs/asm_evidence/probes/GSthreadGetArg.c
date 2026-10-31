/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

u32 GSthreadGetArg(u32 arg) {
    return arg;
}
