/**
 * @file effect_visual_exact_8013CE58.c
 * @brief _seaResetMaterial (_envMapUpdateTexture), 0x8013CE58 - 0x8013D0A8.
 *
 * Function-boundary carve of the effect_visual suffix chunk (see
 * effect_visual.c): the stage checks are a compare chain (no jump table),
 * no pooled constant, no data. GC/1.3 -O4,p like the chunk, no pragmas.
 */
#include "dolphin/types.h"

extern void* fn_8019FF48(void* model);

/*
 * The display object's material (HSD_DObj.mobj at +0xC), or NULL for no
 * display object. Admitted by inline fingerprint: retail tests the display
 * object for NULL, returns, then branches again on the stale CR of that
 * test (beq at 0x8013CEA8) -- this helper's guard, left after CSE folded
 * it into the caller's own NULL check.
 */
static inline u8* dobjGetMObj(u8* dobj)
{
    return dobj != NULL ? *(u8**)(dobj + 0xC) : NULL;
}

/* Copy the env-map texture parameters into the display object's TEV
 * stages once its material is the expected four-stage setup (stages 9, 10,
 * 11 and 13 with their fixed input/scale values); 0 if it is not. */
u32 _seaResetMaterial(void* inner, void* ptr) {
    u8* p = ptr;
    u8* displayObject;
    u8* material;
    u8* stages;
    u8* stage;
    s32 op;
    s32 found9;
    s32 found10;
    s32 found13;
    s32 found11;
    s32 stageIndex;

    displayObject = fn_8019FF48(*(void**)((u8*)inner + 0x8));
    found9 = 0;
    found10 = 0;
    found13 = 0;
    found11 = 0;
    if (p[0x46] == 0) {
        return 1;
    }
    if (displayObject == NULL) {
        return 0;
    }

    material = dobjGetMObj(displayObject);
    if (material == NULL) {
        return 0;
    }
    stages = *(u8**)(material + 0x8);
    stage = stages;
    if (stages == NULL) {
        return 0;
    }

    stageIndex = 1;
    for (; (op = *(s32*)stage) != 0xFF; stage += 0x18, stageIndex++) {
        switch (op) {
        case 9:
            found9 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 12) {
                return 0;
            }
            break;
        case 10:
            found10 = stageIndex;
            if (*(s32*)(stage + 0x8) != 0) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 12) {
                return 0;
            }
            break;
        case 11:
            found11 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 5) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 4) {
                return 0;
            }
            break;
        case 13:
            found13 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 8) {
                return 0;
            }
            break;
        default:
            return 0;
        }
    }

    if (!found9 || !found10 || !found11 || !found13) {
        return 0;
    }

    stageIndex = 0;
    while (*(s32*)stages != 0xFF) {
        *(u32*)(stages + 0x4) = *(u32*)(p + 0x20 + stageIndex * 4);
        *(u32*)(stages + 0x14) = *(u32*)(p + 0x30 + stageIndex * 4);
        stageIndex++;
        stages += 0x18;
    }
    *(u32*)(material + 0x10) = *(u32*)(p + 0x40);
    *(u16*)(material + 0xE) = *(u16*)(p + 0x44);
    return 1;
}
