/**
 * @file GScolsys2Sun_range_80111C24.c
 * @brief GScolsys2Sun (+ neighbor overflow) -- boundary/region ray tests.
 *
 * Fifth of six translation units recovered from the former
 * game/gs_field_colquery.c CodeCandidate bucket (0x8010F6A0-0x801140DC).
 * Only the first function (GScolsys2Sun) is anchor-confirmed; the
 * remaining four have no confirmed match and may include
 * Colosseum-only additions or GScolsys2Check remainder overflow, so
 * this unit keeps the _range_ fallback name pending call-graph
 * confirmation of the exact XD TU boundary.
 *
 * Address range: 0x80111C24 - 0x80112380
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"
#include "hsd/hsd_archive.h"

typedef struct FloorArchiveResourceLists {
    u32* models;
    u32 unused04;
    u32* lights;
} FloorArchiveResourceLists;

extern const char lbl_802720B0[];
extern void* GSresGetResource(u32 groupId, u32 resourceId);
extern void* floorDataBiosGetCurrentPtr(void);
extern u32 floorDataBiosGetMapResID(void*);
extern u32 floorDataBiosGetGroupID(void*);
extern u32 floorReadMakeModelResID(u32);
extern u32 floorReadMakeLightResID(u32);
extern void GSmodelPushState(void*, void*);
extern void GSlightPushState(void*, void*);
extern void GSmodelSetVisibility(void*, u8);
extern void GSlightSetActive(void*, u8);

/* 0x80111C24 | 0x1D4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
s32 GScolsys2Sun(void* origin, void* dir) {
#pragma optimization_level 4
    extern f32 PSVECDistance(void* a, void* b);
    extern s32 GScolsy2UtilChkInTri(void* a, void* b, void* c);
    extern f32 lbl_8047CF68;
    extern f32 lbl_8047CF6C;
    GSFieldWzxData* wzx;
    u32 regIdx;
    GSFieldWzxTriangleList* triList;
    GSFieldWzxRegion* region;
    s32 k;
    u32 vertIdx;
    GSFieldWzxCompactTriangle* tri;
    GSFieldVec3f* vdst;
    GSFieldVec3f* vsrc;
    s32 visFlag;
    f32 resultT;
    GSFieldVec3f dirVec;
    GSFieldVec3f pt;
    GSFieldVec3f out;
    f32 mtxFwd[12];
    f32 mtxInv[12];
    GSFieldVec3f verts[3];
    s32 found;
    s32 hit;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }
    if (PSVECDistance(dir, origin) <= lbl_8047CF68) {
        return 0;
    }
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    PSVECSubtract(dir, origin, &dirVec);
    region = wzx->regions;
    regIdx = 0;
    while (regIdx < wzx->regionCount) {
        GScolsys2GetObjEnable(regIdx, &visFlag);
        if (visFlag != 0) {
            triList = region->boundaryTriangles;
            if (triList != NULL) {
                fn_8010CA30(mtxInv, regIdx);
                fn_8010C8D0(mtxFwd, regIdx);
                tri = (GSFieldWzxCompactTriangle*)triList->triangles;
                vertIdx = 0;
                while (vertIdx < triList->triangleCount) {
                    PSMTXMultVec(mtxFwd, &tri->normal, &pt);
                    vsrc = tri->vertices;
                    vdst = verts;
                    k = 0;
                    do {
                        PSMTXMultVec(mtxInv, vsrc, vdst);
                        k++;
                        vsrc++;
                        vdst++;
                    } while (k < 3);
                    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)&out, &resultT,
                                                   (const Vec3f*)&pt,
                                                   (const Vec3f*)verts,
                                                   (const Vec3f*)origin,
                                                   (const Vec3f*)dir) == 0) {
                        hit = 0;
                    } else if (resultT < lbl_8047CF68 || resultT > lbl_8047CF6C) {
                        hit = 0;
                    } else if (GScolsy2UtilChkInTri(&out, verts, &pt) == 0) {
                        hit = 0;
                    } else {
                        hit = 1;
                    }
                    if (hit != 0) {
                        found = 1;
                        goto inner_done;
                    }
                    vertIdx++;
                    tri++;
                }
                found = 0;
            inner_done:
                if (found != 0) {
                    return 1;
                }
            }
        }
        regIdx++;
        region++;
    }
    return 0;
}
#pragma peephole on
#pragma pop

/* 0x80111DF8 | 0x134 */
u32 fn_80111DF8(void) {
    FloorArchiveResourceLists* lists;
    void* floor;
    u32 mapId;
    u32 groupId;
    u32 baseId;
    u32 modelCount;
    u32 lightCount;
    u32 modelIndex;
    u32 lightIndex;

    modelCount = 0;
    lightCount = 0;
    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    if (GSresGetResource(groupId,
                                          baseId | modelIndex) != NULL) {
                        modelCount++;
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    if (GSresGetResource(groupId,
                                          baseId | lightIndex) != NULL) {
                        lightCount++;
                    }
                }
            }
        }
    }
    return (modelCount + lightCount) * 0x74;
}

/* 0x80111F2C | 0x150 */
void fn_80111F2C(u8* state) {
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 resourceId;
    u32 baseId;
    u32 modelIndex;
    u32 lightIndex;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    resourceId = baseId | modelIndex;
                    resource = GSresGetResource(groupId, resourceId);
                    if (resource != NULL) {
                        *(u32*)(state + 0) = resourceId;
                        *(u32*)(state + 4) = 1;
                        GSmodelPushState(resource, state + 8);
                        state += 0x74;
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    resourceId = baseId | lightIndex;
                    resource = GSresGetResource(groupId, resourceId);
                    if (resource != NULL) {
                        *(u32*)(state + 0) = resourceId;
                        *(u32*)(state + 4) = 2;
                        GSlightPushState(resource, state + 8);
                        state += 0x74;
                    }
                }
            }
        }
    }
}

/* 0x8011207C | 0x1E4 */
void fn_8011207C(u8* state, u32 stateSize) {
    extern void GSmodelPopState(void*, void*);
    extern void GSlightPopState(void*, void*);
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 resourceId;
    u32 baseId;
    u32 stateCount = stateSize / 0x74;
    u32 modelIndex;
    u32 lightIndex;
    u32 j;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId == 0) {
        return;
    }
    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    lists = HSD_ArchiveGetPublicAddress(
        GSresGetResource(groupId, mapId), lbl_802720B0);
    if (lists == NULL) {
        return;
    }
    if (lists->models != NULL) {
        baseId = floorReadMakeModelResID(mapId);
        for (; lists->models[modelIndex] != 0; modelIndex++) {
            resourceId = baseId | modelIndex;
            resource = GSresGetResource(groupId, resourceId);
            if (resource != NULL) {
                u8* record = state;
                for (j = 0; j < stateCount; j++, record += 0x74) {
                    if (*(u32*)record == resourceId) {
                        switch (*(u32*)(record + 4)) {
                        case 1:
                            GSmodelPopState(resource, record + 8);
                            break;
                        case 2:
                            GSlightPopState(resource, record + 8);
                            break;
                        }
                        break;
                    }
                }
            }
        }
    }
    if (lists->lights != NULL) {
        baseId = floorReadMakeLightResID(mapId);
        for (; lists->lights[lightIndex] != 0; lightIndex++) {
            resourceId = baseId | lightIndex;
            resource = GSresGetResource(groupId, resourceId);
            if (resource != NULL) {
                u8* record = state;
                for (j = 0; j < stateCount; j++, record += 0x74) {
                    if (*(u32*)record == resourceId) {
                        switch (*(u32*)(record + 4)) {
                        case 1:
                            GSmodelPopState(resource, record + 8);
                            break;
                        case 2:
                            GSlightPopState(resource, record + 8);
                            break;
                        }
                        break;
                    }
                }
            }
        }
    }
}

/* 0x80112260 | 0x120 */
void fn_80112260(s32 visible) {
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 baseId;
    u32 modelIndex;
    u32 lightIndex;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    resource = GSresGetResource(groupId, baseId | modelIndex);
                    if (resource != NULL) {
                        GSmodelSetVisibility(resource, (u8)visible);
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    resource = GSresGetResource(groupId, baseId | lightIndex);
                    if (resource != NULL) {
                        GSlightSetActive(resource, (u8)visible);
                    }
                }
            }
        }
    }
}
