/**
 * @file gs_colsys_exact_8010C8D0.c
 * @brief GScolsys2 object matrices and the CCD accessor,
 *        0x8010C8D0 - 0x8010CBD0.
 *
 * A function-boundary carve of the GScolsys2 core TU (see
 * gs_range_8010CBD0.c). The axis table and the rotation order are local
 * aggregates in retail; their initialiser images (lbl_80272020,
 * lbl_80272044) belong to the core TU's .rodata, so the carve copies
 * them from the extern objects. Project default flags (GC/1.3), no
 * pragmas.
 *
 * The object pointer is taken in two steps (floor->objs, then + index):
 * retail adds the objs base (+4) before index * 0x28, exactly as the
 * linked GScolsys2Get/SetObjEnable do; &floor->objs[index] folds the +4
 * last. The locals are declared in the order that reproduces retail's
 * register assignment.
 */
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "game/gs_colsys.h"

extern GSColSysState lbl_80404C68;

typedef struct ColAxes {
    Vec axis[3];
} ColAxes;

typedef struct ColRotOrder {
    u32 index[3];
} ColRotOrder;

extern const ColAxes lbl_80272020;
extern const ColRotOrder lbl_80272044;

typedef struct CCD_FILEHEAD {
    void* records;
    u32 count;
} CCD_FILEHEAD;

/* 0x8010C8D0 | 0x160 */
s32 colsysMakeRotMatrix(Mtx out, u32 index)
{
    Mtx result;
    ColAxes axes;
    Mtx combined;
    Mtx rotation;
    ColRotOrder order;
    CCD_FILEHEAD* head;
    GSColFloor* floor;
    s32 i;
    u32* axis;
    f32* angle;
    GSColFloorObj* obj;

    head = lbl_80404C68.wzxDataPtr;
    if (head == NULL) {
        return 0;
    }
    if (index >= head->count) {
        return 0;
    }

    floor = &lbl_80404C68.floors[lbl_80404C68.activeLayer];
    obj = floor->objs;
    obj += index;
    PSMTXIdentity(out);
    if ((obj->flags & 1) != 0) {
        axes = lbl_80272020;
        order = lbl_80272044;
        angle = &obj->rot.x;
        PSMTXIdentity(combined);
        for (i = 0, axis = order.index; i < 3; i++, axis++) {
            PSMTXRotAxisRad(rotation, &axes.axis[*axis], angle[*axis]);
            PSMTXConcat(rotation, combined, combined);
        }
        PSMTXCopy(combined, result);
        PSMTXConcat(out, result, out);
    }
    return 1;
}

/* 0x8010CA30 | 0x190 */
s32 colsysMakeSRTMatrix(Mtx out, u32 index)
{
    Mtx result;
    ColAxes axes;
    Mtx combined;
    Mtx rotation;
    ColRotOrder order;
    CCD_FILEHEAD* head;
    GSColFloor* floor;
    s32 i;
    u32* axis;
    f32* angle;
    GSColFloorObj* obj;

    head = lbl_80404C68.wzxDataPtr;
    if (head == NULL) {
        return 0;
    }
    if (index >= head->count) {
        return 0;
    }

    floor = &lbl_80404C68.floors[lbl_80404C68.activeLayer];
    obj = floor->objs;
    obj += index;
    PSMTXIdentity(out);
    if ((obj->flags & 1) != 0) {
        PSMTXScaleApply(out, out, obj->scale.x, obj->scale.y, obj->scale.z);
        axes = lbl_80272020;
        order = lbl_80272044;
        angle = &obj->rot.x;
        PSMTXIdentity(combined);
        for (i = 0, axis = order.index; i < 3; i++, axis++) {
            PSMTXRotAxisRad(rotation, &axes.axis[*axis], angle[*axis]);
            PSMTXConcat(rotation, combined, combined);
        }
        PSMTXCopy(combined, result);
        PSMTXConcat(out, result, out);
        PSMTXTransApply(out, out, obj->trans.x, obj->trans.y, obj->trans.z);
    }
    return 1;
}

/* 0x8010CBC0 | 0x10 */
void* fn_8010CBC0(void)
{
    return lbl_80404C68.wzxDataPtr;
}
