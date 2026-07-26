/**
 * @file gs_part.c
 * @brief GSpart (model part/joint-subtree accessors)
 *
 * Split from gs_range_800E202C.c (0x800EE150-0x800EE928) — one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#include "dolphin/types.h"
#include "hsd/hsd_jobj.h"

typedef struct GSpart {
    u8 inUse;
    u8 _pad;
    u16 index;
    void *model;
    void *jobj;
} GSpart;

extern GSpart *lbl_8047ABBC;
extern u32 lbl_8047ABC0;
extern void *lbl_8047ABA8;
extern u32 lbl_8047ABAC;
extern u32 lbl_8047ABB0;
extern u8 lbl_8047ABC4;
extern u32 lbl_8047ABC8;
extern s32 lbl_8047ABCC;
extern void* lbl_8047ABD0;

extern void* modelGetRenderJObj(void* model);
extern void fn_801A3918(void* root, void (*callback)(void*), u32 flags);
extern void* GSmaterialCreate(void);
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
        fn_801A3918(jobj, fn_800EE20C, 0);
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
    fn_801A3918(jobj, _partFindIndexCB__FP9_HSD_JObjPPvi, 0);
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
    void* callback;
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
    if (slot->callback == NULL) {
        return slot;
    }
    slot++;
    if (slot->callback == NULL) {
        return slot;
    }
    slot++;
    if (slot->callback == NULL) {
        return slot;
    }
    slot++;
    if (slot->callback == NULL) {
        return slot;
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
        slot->callback = callback;
        slot->partIndex = part->index;
        slot->userData = userData;
    }
}

void* GSpartGetMaterial(GSpart* part, u32 index)
{
    u8* jobj = part->jobj;
    u32 count;
    u8* material;
    void* source;
    void* result;

    if (union_type_dobj((HSD_JObj*)jobj)) {
        material = *(u8**)(jobj + 0x18);
    } else {
        material = NULL;
    }
    if (material == NULL) {
        return NULL;
    }

    count = 0;
    while (material != NULL) {
        if (count++ == index) {
            source = *(void**)(material + 8);
            break;
        }
        material = *(u8**)(material + 4);
    }
    if (material == NULL) {
        source = NULL;
    }
    if (source == NULL) {
        return NULL;
    }

    result = GSmaterialCreate();
    if (result != NULL) {
        *(void**)((u8*)result + 8) = source;
    }
    return result;
}

static inline void GSpartCountMaterials(u8* material, u32* result)
{
    u32 count = 0;

    while (material != NULL) {
        if ((u16)count == 0xFFFF) {
            if (result != NULL) {
                *result = -1;
            }
            return;
        }
        count++;
        material = *(u8**)(material + 4);
    }
    if (result != NULL) {
        *result = count;
    }
}

u32 GSpartGetMaterialCount(GSpart* part)
{
    u8* jobj = part->jobj;
    u8* material;
    u32 result;

    if (union_type_dobj((HSD_JObj*)jobj)) {
        material = *(u8**)(jobj + 0x18);
    } else {
        material = NULL;
    }
    if (material == NULL) {
        return 0;
    }
    GSpartCountMaterials(material, &result);
    return result;
}

u8 fn_800EE7E0(GSpart* part)
{
    u8* jobj = part->jobj;
    u8* material;
    u32 value;

    if (union_type_dobj((HSD_JObj*)jobj)) {
        material = *(u8**)(jobj + 0x18);
        if (material != NULL) {
            value = *(u32*)(material + 0xC);
        } else {
            value = 0;
        }
    } else {
        value = 0;
    }
    return value == 0;
}

void GSpartFree(GSpart *part)
{
    part->inUse = 0;
}

static inline GSpart *GSpartFindFree(void)
{
    GSpart *part;
    u32 i;

    part = lbl_8047ABBC;
    for (i = 0; i < lbl_8047ABC0; i++, part++) {
        if (part->inUse == 0) {
            return part;
        }
    }
    return NULL;
}

GSpart *GSpartCreate(void)
{
    GSpart *part;

    part = GSpartFindFree();
    if (part == NULL) {
        return NULL;
    }
    part->inUse = 1;
    return part;
}

extern u16 lbl_8047ABB8;
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

void GSpartInit(u32 count)
{
    u32 handle;
    u32 i;

    lbl_8047ABC0 = count;
    handle = _toolentryAlloc__FUl(count * sizeof(GSpart));
    lbl_8047ABB8 = handle;
    if ((u16)handle != 0) {
        lbl_8047ABBC = fn_800E27B0((u16)handle);
        for (i = 0; i < lbl_8047ABC0; i++) {
            ((u8*)lbl_8047ABBC)[i * sizeof(GSpart)] = 0;
        }
    }
}

void _partFindIndexCB__FP9_HSD_JObjPPvi(void* jobj)
{
    if (lbl_8047ABC4 == 1) {
        return;
    }
    if (lbl_8047ABD0 == jobj) {
        lbl_8047ABC4 = 1;
        lbl_8047ABCC = lbl_8047ABC8;
    }
    lbl_8047ABC8++;
}
