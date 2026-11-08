#include "game/gs_thread.h"

extern u32 lbl_8047AC00;
extern GSThread* lbl_8047AC28;
extern u32 lbl_8047AC30;
extern u8 lbl_8047AC0C;

u8 GSvmGetGroup(GSThread* thread)
{
    return thread->affinity;
}

u32 GSvmGetOwner(u32 context)
{
    GSThread* thread;

    thread = (GSThread*)context;
    return thread->priority;
}

u32 GSthreadGetCurrentThread(void)
{
    return lbl_8047AC00;
}

void GSthreadUnblockGroup(u32 priority)
{
    u32 index;
    GSThread* thread;

    if (priority == 0) {
        return;
    }

    for (index = 0; index < lbl_8047AC30; index++) {
        thread = &lbl_8047AC28[index];
        if (thread->priority == priority) {
            thread->sleeping = 0;
        }
    }

    lbl_8047AC0C = 1;
}

void GSthreadBlockGroup(u32 priority)
{
    u32 index;
    GSThread* thread;

    if (priority == 0) {
        return;
    }

    for (index = 0; index < lbl_8047AC30; index++) {
        thread = &lbl_8047AC28[index];
        if (thread->priority == priority) {
            thread->sleeping = 1;
        }
    }

    lbl_8047AC0C = 1;
}
