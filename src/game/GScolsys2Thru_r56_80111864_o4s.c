/**
 * @file GScolsys2Thru_r56_80111864_o4s.c
 * @brief fn_80111864, event-triangle query.
 *
 * By .sdata2 ownership fn_80111864 belongs to the GScolsys2Check TU: it
 * and GScolsys2CheckGetEventID share the pool 0x8047CF60-0x8047CF68,
 * separate from GScolsys2Thru's 0x8047CF48-0x8047CF60 (which has its
 * own 0.0f/1.0f).
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

extern f32 PSVECDotProduct(void* a, void* b);
extern s32 GScolsy2UtilChkInTri(void* point, void* verts, void* normal);
extern f32 lbl_8047CF60; /* 0.0f */
extern f32 lbl_8047CF64; /* 1.0f */

/* Append one triangle to the result list (GScolsys2Thru's addList). */
static inline void addEventList(GSfieldQueryTriangle* out, GScolsys2Vec3* verts,
                                GScolsys2Vec3* normal, u16 id)
{
    out->verts[0] = verts[0];
    out->verts[1] = verts[1];
    out->verts[2] = verts[2];
    out->normal = *normal;
    out->id = id;
}

/* RULE-EXCEPTION(user-approved): inline boundaries and scalar carriers shape
 * allocation; shared pool stand-ins remain. See docs/RULE_EXCEPTIONS.md.
 * The transformed-model pass of one region against the shared direction. */
static inline s32 getEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                   GScolsys2Vec3* dirVec, GSFieldWzxTriangleList* list,
                                   f32* mtxInv, f32* mtxFwd, GSfieldQueryTriangle* out,
                                   GScolsys2Vec3** vertexBase,
                                   GScolsys2Vec3** vertexSrc, s32* vertexIndex)
{
    GScolsys2Vec3* vertexDst;
    GScolsys2Vec3 hitPoint;
    GScolsys2Vec3 normal;
    f32 t;
    s32 hit;
    GScolsys2Vec3 verts[3];
    GSfieldQueryTriangle* top;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* scan;
    GSfieldQueryTriangle* tri;
    u32 i;
    s32 j;
    s32 count;

    count = 0;
    top = out;
    tri = (GSfieldQueryTriangle*)list->triangles;
    i = 0;
    write = top;
    for (; i < list->triangleCount && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= lbl_8047CF60) {
            continue;
        }
        *vertexBase = verts;
        vertexDst = *vertexBase;
        *vertexSrc = tri->verts;
        for (*vertexIndex = 0; *vertexIndex < 3;
             (*vertexIndex)++, vertexDst++, (*vertexSrc)++) {
            PSMTXMultVec(mtxInv, *vertexSrc, vertexDst);
        }
        if (GScolsys2UtilGetCpPlaneLine((Vec3f*)&hitPoint, &t,
                                      (Vec3f*)&normal, (Vec3f*)verts,
                                      (Vec3f*)start, (Vec3f*)end) == 0) {
            hit = 0;
            goto finishedSegment;
        }
        if (t < lbl_8047CF60 || t > lbl_8047CF64) {
            hit = 0;
            goto finishedSegment;
        }
        if (GScolsy2UtilChkInTri(&hitPoint, verts, &normal) == 0) {
            hit = 0;
            goto finishedSegment;
        }
        hit = 1;
finishedSegment:
        if (hit) {
            addEventList(write, *vertexBase, &normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

static inline GSFieldWzxData* identityWzx(GSFieldWzxData* value)
{
    return value;
}

static inline void getFloorTriangles(GSFieldWzxRegion* region,
                                     GSFieldWzxTriangleList** list)
{
    *list = region->floorTriangles;
}

/* 0x80111864 | 0x338 */
s32 fn_80111864(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out)
{
    /* Keep the base separate from the inlined pass's advancing cursor. */
    struct {
        GScolsys2Vec3* value;
    } vertexBase;
    GSfieldQueryTriangle temp[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    GScolsys2Vec3 dirVec;
    register struct {
        GSFieldWzxData* value;
    } wzxCarrier;
    s32 vertexIndex;
    GSFieldWzxRegion* region;
    GSFieldWzxTriangleList* list;
    u32 i;
    GScolsys2Vec3* vertexSrc;
    struct {
        GSfieldQueryTriangle* value;
    } temporaryCarrier;
    GSfieldQueryTriangle* scan;
    s32 j;
    s32 k;
    s32 enabled;
    s32 tempCount;
    s32 outCount;

    outCount = 0;
    wzxCarrier.value = identityWzx((GSFieldWzxData*)fn_8010CBC0());
    PSVECSubtract(end, start, &dirVec);
    region = wzxCarrier.value->regions;
    for (i = 0; i < wzxCarrier.value->regionCount && outCount < 4; i++, region++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        getFloorTriangles(region, &list);
        if (list == NULL) {
            continue;
        }
        colsysMakeSRTMatrix(mtxInv, i);
        colsysMakeRotMatrix(mtxFwd, i);
        temporaryCarrier.value = temp;
        tempCount = getEventListPass(start, end, &dirVec, list,
                                     mtxInv, mtxFwd, temporaryCarrier.value,
                                     &vertexBase.value, &vertexSrc, &vertexIndex);
        for (j = 0; j < tempCount && outCount < 4; temporaryCarrier.value++, j++) {
            for (scan = out, k = 0; k < outCount; scan++, k++) {
                if (scan->id == temporaryCarrier.value->id) {
                    break;
                }
            }
            if (k >= outCount) {
                out[outCount++] = *temporaryCarrier.value;
            }
        }
    }
    return outCount;
}
