#include "dolphin/types.h"

#pragma section ".data"

extern u8 lbl_8036CD88[];
extern void* jumptable_8036CD94[];
extern void* lbl_8036CFA8[];

extern u8 fn_801AEFF0[];
extern u8 _HSD_StateInvalidateRenderMode[];
extern u8 _HSD_StateInvalidateVtxAttr[];
extern u8 _HSD_StateInvalidatePrimitive[];
extern u8 _HSD_StateInvalidateTexCoordGen[];
extern u8 _HSD_StateInvalidateTevRegister[];
extern u8 _HSD_StateInvalidateTevStage[];
extern u8 _HSD_StateInvalidateColorChannel[];

/* .data unit 0x8036CFA8..0x8036CFE8: the state invalidate table (state.c's
 * channel set-ups before it, 0x8036CE88, are owned by hsd/state.c). Originally
 * auto-carved 0x8036CD88..0x8036CFE8. Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* lbl_8036CFA8[16] = {
    (void*)0x00000001,
    (void*)((u8*)_HSD_StateInvalidatePrimitive),
    (void*)0x00000002,
    (void*)((u8*)_HSD_StateInvalidateVtxAttr),
    (void*)0x00000004,
    (void*)((u8*)_HSD_StateInvalidateColorChannel),
    (void*)0x00000008,
    (void*)((u8*)_HSD_StateInvalidateTevStage),
    (void*)0x00000010,
    (void*)((u8*)_HSD_StateInvalidateTevRegister),
    (void*)0x00000020,
    (void*)((u8*)_HSD_StateInvalidateTexCoordGen),
    (void*)0x00000040,
    (void*)((u8*)_HSD_StateInvalidateRenderMode),
    (void*)0x00000000,
    (void*)0x00000000,
};
