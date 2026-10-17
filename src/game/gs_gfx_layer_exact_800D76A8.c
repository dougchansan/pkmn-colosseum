/* GSgfx: draw a vertex list with its vertex format, 0x800D76A8 - 0x800D7820. */
#include "dolphin/types.h"

extern u32 lbl_8047AA80; /* GSgfx state pointer */
extern u8 lbl_804001F0[];
extern u8 lbl_80314370[];
extern u8 lbl_803143A8[];
extern u8 lbl_803143B4[];
extern u8 lbl_803143D8[];

extern void gfxRecordCommand(u32, ...);
extern void fn_800D7940(u8*, u16);
extern void gfxSetupChannelsTev(u32);
extern void GXSetVtxAttrFmt(u32, u32, u32, u32, u8);
extern void GXClearVtxDesc(void);
extern void GXSetVtxDesc(u32, u32);
extern void GXSetArray(u32, u32, u8);

/*
 * Body of fn_800D7A70 (out-of-line at 0x800D7A70; source in
 * gs_gfx_layer_state.c). Retail also expands it in place in gfxSetVtxDescRaw, so
 * the vertex-descriptor load is expanded here as the same body.
 */
static inline void GSgfxLoadVtxDesc(u32 obj) {
    u32 i;
    u8* entry;
    u32 attr;

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0 && (s32)i != 0) {
            attr = ((u32*)lbl_80314370)[i];
            GXSetVtxAttrFmt(*(u32*)(obj + 0x4), attr,
                        ((u32*)lbl_803143B4)[*(u32*)(entry + 0x10)],
                        ((u32*)lbl_803143D8)[*(u32*)(entry + 0x14)],
                        entry[0x18]);
        }
    }

    GXClearVtxDesc();

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0) {
            GXSetVtxDesc(((u32*)lbl_80314370)[i], ((u32*)lbl_803143A8)[*(u32*)(entry + 0xc)]);
            if (*(u32*)(entry + 0x1c) != 0) {
                GXSetArray(((u32*)lbl_80314370)[i], *(u32*)(entry + 0x1c), entry[0x20]);
            }
        }
    }

    *(u32*)(lbl_804001F0 + 0x14) += 1;
}

void gfxSetVtxDescRaw(u32 obj, u16 vertCount) {
    u32 oldObj;

    if (*(u8*)(lbl_8047AA80 + 0x47e) == 1) {
        oldObj = *(u32*)(lbl_8047AA80 + 0x24);
        *(u32*)(lbl_8047AA80 + 0x24) = obj;
        fn_800D7940((u8*)obj, vertCount);
        *(u32*)(lbl_8047AA80 + 0x24) = oldObj;
        return;
    }

    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x47, 2, obj, (u32)vertCount);
        return;
    }

    GSgfxLoadVtxDesc(obj);
    gfxSetupChannelsTev(obj);
    fn_800D7940((u8*)obj, vertCount);
}
