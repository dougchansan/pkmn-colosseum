/* Standalone carve of _seaStop (_envMapStart), 0x8013C670 - 0x8013C718. */
#include "dolphin/types.h"

extern void GSmodelStopTexAnimation(void* model);
extern void GSmodelSetVisibility(void* model, u32 visible);
extern void GXDrawDone(void);
extern void GXInvalidateVtxCache(void);
extern u32 _seaResetMaterial(void* model, void* work);
extern void* fn_800E24B0(u32 handle);
extern void GSmemFree(u32 handle);

/* RULE-EXCEPTION(user-approved): local optimizer control -- see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma global_optimizer off
u32 _seaStop(void* arg) {
    void* ptr;
    void* inner;

    if (arg != 0) {
        ptr = arg;
        inner = *(void**)ptr;
        GSmodelStopTexAnimation(inner);
        if (inner != 0) {
            GSmodelSetVisibility(inner, 0);
        }
        GXDrawDone();
        GXInvalidateVtxCache();
        _seaResetMaterial(inner, ptr);

        {
            u16 val;
            val = *(u16*)((u8*)ptr + 0x1c);
            if (val != 0) {
                fn_800E24B0(val);
                GSmemFree(val);
            }
        }

        ptr = (void*)(u32)*(u16*)((u8*)ptr + 0x8c);
        if (ptr != 0) {
            fn_800E24B0((u32)ptr);
            GSmemFree((u32)ptr);
        }
    }
    return 1;
}
#pragma pop
