/*
 * GSgfx: bind a texture to a texture slot, 0x800D85D4 - 0x800D87AC.
 *
 * Written from the retail code: the LOD call passes 0.0f as both minLod and
 * lodBias, and each filter case is an if/else with no default. The 0.0f and
 * the signed int-to-float bias are this unit's own literal pool (.sdata2
 * 0x8047CA40-0x8047CA50; no other function uses it). As literals, MWCC
 * schedules the final call's argument setup as retail does; extern
 * variable loads cannot be moved across the stack store. Links as Matching.
 */
#include "dolphin/types.h"

extern u32 lbl_8047AA80; /* GSgfx state pointer */
extern u8 lbl_804001F0[];
extern u8 lbl_803144F0[];
extern u8 lbl_80314510[];
extern u8 lbl_80314530[];

extern void gfxRecordCommand(u32, ...);
extern void GXInitTexObjWrapMode(void* texObj, u32 wrapS, u32 wrapT);     /* GXInitTexObjWrapMode */
extern void GXInitTexObjLOD(void* texObj, u32 minFilt, u32 magFilt, f32 minLod, f32 maxLod,
                        f32 lodBias, u8 biasClamp, u8 edgeLod, u32 maxAniso); /* GXInitTexObjLOD */
extern void GXLoadTlut(void* tlutObj, u32 tlutName);           /* GXLoadTlut */
extern void GXLoadTexObj(void* texObj, u32 id);

void gfxLoadTexture(s32 slot, void* model) {
    u8* obj;
    u32 minFilt;

    if (model == NULL) {
        return;
    }
    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x26, 2, slot, model);
        return;
    }
    if (*(void**)(lbl_8047AA80 + 0x28 + slot * 4) == model) {
        return;
    }

    obj = (u8*)model;
    if (obj[0x7] != 0) {
        GXInitTexObjWrapMode(obj + 0x54, ((u32*)lbl_80314530)[*(u32*)(obj + 0x10)],
                    ((u32*)lbl_80314530)[*(u32*)(obj + 0x14)]);
        switch (*(s32*)(obj + 0x20)) {
        case 0:
            if (*(s32*)(obj + 0x18) == 2) {
                minFilt = 1;
            } else {
                minFilt = 0;
            }
            break;
        case 1:
            if (*(s32*)(obj + 0x18) == 2) {
                minFilt = 3;
            } else {
                minFilt = 2;
            }
            break;
        case 2:
            if (*(s32*)(obj + 0x18) == 2) {
                minFilt = 5;
            } else {
                minFilt = 4;
            }
            break;
        }
        GXInitTexObjLOD(obj + 0x54, minFilt, *(s32*)(obj + 0x1c) == 2, 0.0f,
                    (f32)(obj[0x5] - 1), 0.0f, 0, 0, 0);
        obj[0x7] = 0;
    }

    *(void**)(lbl_8047AA80 + 0x28 + slot * 4) = model;
    if (*(u32*)(obj + 0x48) != 0) {
        GXLoadTlut(obj + 0x74, ((u32*)lbl_80314510)[slot]);
    }
    GXLoadTexObj(obj + 0x54, ((u32*)lbl_803144F0)[slot]);
    *(u32*)(lbl_804001F0 + 0x24) += 1;
}
