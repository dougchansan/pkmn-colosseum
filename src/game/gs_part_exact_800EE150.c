/**
 * @file gs_part_exact_800EE150.c
 * @brief GSpart lookup and rotation-callback registration,
 *        0x800EE150 - 0x800EE3BC.
 *
 * The head of the GSpart range: GSmodelGetPart and its traversal callback
 * fn_800EE20C, GSpartGetJObjIndex, fn_800EE288 and GSpartRegisterRotation.
 * It is linked on its own while GSpartGetTransform (gs_part.c, the only
 * function between this head and gs_part_exact_800EE6B4.c) is still being
 * matched.
 *
 * GSpartFindRotationSlot is a reconstructed inline: GSpartRegisterRotation
 * carries its fingerprint, the helper's "return NULL" materialised in r3
 * (li r3, 0x0) and then tested again by the caller (cmplwi r3, 0x0; beq)
 * after both failure paths.
 */
#include "dolphin/types.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_jobj.h"

typedef struct GSpart {
    u8 inUse;
    u8 _pad;
    u16 index;
    void *model;
    void *jobj;
} GSpart;

extern void *lbl_8047ABA8;
extern u32 lbl_8047ABAC;
extern u32 lbl_8047ABB0;
extern u8 lbl_8047ABC4;
extern u32 lbl_8047ABC8;
extern s32 lbl_8047ABCC;
extern void* lbl_8047ABD0;

extern void* modelGetRenderJObj(void* model);
extern void HSD_JObjWalkTree(void* root, void (*callback)(void*), u32 flags);
extern void GSlogWrite(const char* format, ...);
extern char lbl_80270F10[];
extern char lbl_80270F44[];

void fn_800EE20C(void* jobj);
GSpart* GSpartCreate(void);
void _partFindIndexCB__FP9_HSD_JObjPPvi(void* jobj);

GSpart* GSmodelGetPart(void* model, s32 index)
{
    s32 traversalIndex = index;
    void* jobj;
    GSpart* part;

    if (*(u32*)model & 0x20000) {
        traversalIndex++;
    }

    lbl_8047ABAC = traversalIndex;
    lbl_8047ABB0 = 0;
    lbl_8047ABA8 = NULL;
    jobj = modelGetRenderJObj(model);
    if (traversalIndex != 0) {
        HSD_JObjWalkTree(jobj, fn_800EE20C, 0);
        if (lbl_8047ABA8 == NULL) {
            return NULL;
        }
    } else {
        lbl_8047ABA8 = jobj;
    }

    part = GSpartCreate();
    if (part == NULL) {
        return NULL;
    }
    part->model = model;
    part->jobj = lbl_8047ABA8;
    part->index = index;
    return part;
}

void fn_800EE20C(void *jobj)
{
    if (lbl_8047ABB0++ == lbl_8047ABAC) {
        lbl_8047ABA8 = jobj;
    }
}

u32 GSpartGetJObjIndex(void* jobj, void* root)
{
    if (jobj == root) {
        return 0;
    }

    lbl_8047ABC4 = 0;
    lbl_8047ABD0 = root;
    lbl_8047ABC8 = 0;
    lbl_8047ABCC = -1;
    HSD_JObjWalkTree(jobj, _partFindIndexCB__FP9_HSD_JObjPPvi, 0);
    return lbl_8047ABCC;
}

void fn_800EE288(GSpart* part)
{
    u8* model = part->model;
    s32 count = 4;

    while (count-- != 0) {
        if (*(u32*)(model + 0xE8) == part->index) {
            *(u32*)(model + 0xE4) = 0;
            *(u32*)(model + 0xE8) = -1;
            return;
        }
    }
}

typedef struct GSpartRotationSlot {
    s32 callback;
    u32 partIndex;
    void* userData;
} GSpartRotationSlot;

static inline GSpartRotationSlot* GSpartFindRotationSlot(GSpart* part)
{
    u8* model = part->model;
    GSpartRotationSlot* slot;
    u32 i;

    slot = (GSpartRotationSlot*)(model + 0xE4);
    for (i = 0; i < 4; i++, slot++) {
        if (slot->partIndex == part->index) {
            GSlogWrite(lbl_80270F10);
            return NULL;
        }
    }

    slot = (GSpartRotationSlot*)(model + 0xE4);
    for (i = 0; i < 4; i++, slot++) {
        if (slot->callback == 0) {
            return slot;
        }
    }
    GSlogWrite(lbl_80270F44);
    return NULL;
}

void GSpartRegisterRotation(GSpart* part, void* userData, void* callback)
{
    GSpartRotationSlot* slot;

    if (callback == NULL) {
        return;
    }

    slot = GSpartFindRotationSlot(part);
    if (slot != NULL) {
        slot->callback = (s32)callback;
        slot->partIndex = part->index;
        slot->userData = userData;
    }
}
