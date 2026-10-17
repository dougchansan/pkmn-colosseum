/**
 * @file GScolsys2Sun.c
 * @brief GScolsys2Sun: does the segment origin->dir cross a region's
 *        boundary ("sun") triangles.
 *
 * Address range: .text 0x80111C24 - 0x80111DF8, .sdata2 0x8047CF68 -
 * 0x8047CF70 (0.0f, 1.0f).
 *
 * TU boundary: XD's GScolsys2Sun.o (NXXJ01.map lines 6926-6934,
 * StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3a) ends with GScolsys2Sun
 * (0x1D4, the same size as here), and floor.o follows it. In Colosseum
 * GScolsys2Sun follows GScolsys2CheckGetEventID directly and is followed by
 * floor code (_floorGetStateSize onwards reads floor.c's .rodata), so the retail
 * TU here is this one function; XD's live getCpSegPolyArray and
 * GScolsys2SunArray have no Colosseum copies.
 *
 * Inline helpers: getCpSegPoly, chkCrossMdl and chkCrossObj are XD
 * GScolsys2Sun.o functions that XD lists as UNUSED (NXXJ01.map lines
 * 6928-6932: getCpSegPoly__FP5GSvecP5GSvecP5GSvecP5GSvecP5GSvec,
 * chkCrossMdl__FP5GSvecP5GSvecP5GSvecP15CCD_SUNMDL_HEADP5GSmtxP5GSmtx,
 * chkCrossObj__FP5GSvecP5GSvec), i.e. expanded into GScolsys2Sun. XD's
 * GScolsys2Sun (trevor403/xd-asm @ b1087f1, code/func_FUN_8011dbdc.s) has
 * the same three-level expansion: the per-triangle 0/1 result, the
 * triangle loop's found flag and the region loop's return.
 *
 * Flags: -opt nopeephole (with the peephole on, the vertex loop's
 * cmpwi/addi pair is reordered), as floor.c, which follows, also builds.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

extern f32 PSVECDistance(void* a, void* b);

static inline s32 getCpSegPoly(GSFieldVec3f* out, GSFieldVec3f* normal, GSFieldVec3f* verts,
                               GSFieldVec3f* start, GSFieldVec3f* end) {
    f32 t;

    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)out, &t, (const Vec3f*)normal,
                                    (const Vec3f*)verts, (const Vec3f*)start,
                                    (const Vec3f*)end) == 0) {
        return 0;
    }
    if (t < 0.0f || t > 1.0f) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(out, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

static inline s32 chkCrossMdl(GSFieldVec3f* start, GSFieldVec3f* end, GSFieldVec3f* dir,
                              GSFieldWzxTriangleList* mdl, f32* inv, f32* fwd) {
    GSFieldVec3f* vsrc;
    GSFieldVec3f* vdst;
    GSFieldWzxCompactTriangle* tri;
    u32 i;
    s32 k;
    GSFieldVec3f out;
    GSFieldVec3f normal;
    GSFieldVec3f verts[3];

    tri = (GSFieldWzxCompactTriangle*)mdl->triangles;
    for (i = 0; i < mdl->triangleCount; i++, tri++) {
        PSMTXMultVec(fwd, &tri->normal, &normal);
        vsrc = tri->vertices;
        vdst = verts;
        k = 0;
        do {
            PSMTXMultVec(inv, vsrc, vdst);
            k++;
            vsrc++;
            vdst++;
        } while (k < 3);
        if (getCpSegPoly(&out, &normal, verts, start, end)) {
            return 1;
        }
    }
    return 0;
}

static inline s32 chkCrossObj(GSFieldVec3f* start, GSFieldVec3f* end) {
    GSFieldWzxData* wzx;
    GSFieldWzxRegion* region;
    GSFieldWzxTriangleList* mdl;
    u32 i;
    s32 enable;
    GSFieldVec3f dir;
    f32 inv[12];
    f32 fwd[12];

    wzx = (GSFieldWzxData*)fn_8010CBC0();
    PSVECSubtract(end, start, &dir);
    region = wzx->regions;
    for (i = 0; i < wzx->regionCount; i++, region++) {
        GScolsys2GetObjEnable(i, &enable);
        if (enable != 0) {
            mdl = region->boundaryTriangles;
            if (mdl != NULL) {
                colsysMakeSRTMatrix(inv, i);
                colsysMakeRotMatrix(fwd, i);
                if (chkCrossMdl(start, end, &dir, mdl, inv, fwd)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* 0x80111C24 | 0x1D4 */
s32 GScolsys2Sun(void* origin, void* dir) {
    if (fn_8010CBC0() == NULL) {
        return 0;
    }
    if (PSVECDistance(dir, origin) <= 0.0f) {
        return 0;
    }
    return chkCrossObj(origin, dir);
}
