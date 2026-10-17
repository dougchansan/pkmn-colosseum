/**
 * @file floor_range_80111DF8.c
 * @brief Floor map resource/state helpers ahead of floor.c.
 *
 * Address range: 0x80111DF8 - 0x80112380 (_floorGetStateSize, _floorMakeStateData,
 * _floorRestoreStateData, fn_80112260).
 *
 * These four functions read floor.c's .rodata (lbl_802720B0, inside
 * floor.c's 0x80272088-0x802721FA) and call the floorDataBios and
 * floorRead helpers, and in XD floor.o directly follows GScolsys2Sun.o
 * (NXXJ01.map lines 6934-6935, StarsMmd/Colo-XD-PBR-symbol-maps @
 * 6b51d3a), so they are the start of the floor TU. GScolsys2Sun, which
 * used to share this candidate range, is linked on its own
 * (GScolsys2Sun.c). Built with floor.c's -opt nopeephole (the mr rX,r3 +
 * cmplwi rX,0 pairs show the peephole off).
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
extern void GSmodelPopState(void*, void*);
extern void GSlightPopState(void*, void*);
extern void GSmodelSetVisibility(void*, u8);
extern void GSlightSetActive(void*, u8);

/*
 * The four functions below are Pokemon XD's floor.cpp map-resource state code
 * with every helper inlined. The helpers are admitted under the same-engine
 * sister-title clause of docs/CAMPAIGN_OPERATIONS.md (user decision,
 * 2026-09-28; XD names from TeamOrre/xd-decomp config/GXXE01/symbols.txt at
 * 4989794e, XD code from trevor403/xd-asm at b1087f18):
 *   floorMapResourceDataIterate  XD 0x8011E224 (global): walk the map's
 *       scene_data models then lights, calling callback(kind 1/2, groupId,
 *       baseId | i, resource, arg) for each resource that loads;
 *   _floorMapGetCountCB  XD 0x8011E1F0, used by _floorMapGetResourceCounts
 *       (XD 0x8011E190) with two stack counters;
 *   _floorMapPushCB      XD 0x8011DE40: store id and kind, push the model or
 *       light state, advance the cursor by 0x74;
 *   _floorMapPopCB       XD 0x8011E110: find the record with this id and pop
 *       the model or light state;
 *   _floorMapSetDispCB   XD 0x8011E3A0 (floorMapSetDisp, XD 0x8011E374).
 * Colosseum's iterator gets the group ID through floorDataBiosGetCurrentPtr +
 * floorDataBiosGetGroupID (XD calls floorGetCurrentGroupID). The callback is a
 * compile-time constant at each use, so MWCC inlines it into the iterator.
 */
typedef void (*FloorMapResourceCB)(u32 kind, u32 groupId, u32 resourceId,
                                   void* resource, void* arg);

typedef struct FloorStateData {
    u8* data;
    u32 count;
} FloorStateData;

static inline void floorMapResourceDataIterate(FloorMapResourceCB callback,
                                               void* arg)
{
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 groupId;
    u32 mapId;
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
                        callback(1, groupId, resourceId, resource, arg);
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    resourceId = baseId | lightIndex;
                    resource = GSresGetResource(groupId, resourceId);
                    if (resource != NULL) {
                        callback(2, groupId, resourceId, resource, arg);
                    }
                }
            }
        }
    }
}

static inline void floorMapGetCountCB(u32 kind, u32 groupId, u32 resourceId,
                                      void* resource, void* arg)
{
    switch (kind) {
    case 1:
        ((u32*)arg)[0]++;
        break;
    case 2:
        ((u32*)arg)[1]++;
        break;
    }
}

static inline void floorMapPushCB(u32 kind, u32 groupId, u32 resourceId,
                                  void* resource, void* arg)
{
    u8* state = *(u8**)arg;

    *(u32*)(state + 0) = resourceId;
    *(u32*)(state + 4) = kind;
    switch (kind) {
    case 1:
        GSmodelPushState(resource, state + 8);
        break;
    case 2:
        GSlightPushState(resource, state + 8);
        break;
    }
    *(u8**)arg = state + 0x74;
}

static inline void floorMapPopCB(u32 kind, u32 groupId, u32 resourceId,
                                 void* resource, void* arg)
{
    u8* record;
    u32 count;

    record = ((FloorStateData*)arg)->data;
    count = ((FloorStateData*)arg)->count;
    for (; count != 0; record += 0x74) {
        count--;
        if (resourceId == *(u32*)record) {
            switch (*(s32*)(record + 4)) {
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

static inline void floorMapSetDispCB(u32 kind, u32 groupId, u32 resourceId,
                                     void* resource, void* arg)
{
    switch (kind) {
    case 1:
        GSmodelSetVisibility(resource, (u8)arg);
        break;
    case 2:
        GSlightSetActive(resource, (u8)arg);
        break;
    }
}

/* 0x80111DF8 | 0x134: state size (XD _floorGetStateSize, 0x8011DF24) */
u32 _floorGetStateSize(void) {
    u32 counts[2];

    counts[0] = 0;
    counts[1] = 0;
    floorMapResourceDataIterate(floorMapGetCountCB, counts);
    return (counts[0] + counts[1]) * 0x74;
}

/* 0x80111F2C | 0x150: make state (XD _floorMakeStateData, 0x8011E07C) */
void _floorMakeStateData(u8* state) {
    u8* cursor = state;

    floorMapResourceDataIterate(floorMapPushCB, &cursor);
}

/* 0x8011207C | 0x1E4: restore state (XD _floorRestoreStateData, 0x8011DF78) */
void _floorRestoreStateData(u8* state, u32 stateSize) {
    FloorStateData data;

    data.data = state;
    data.count = stateSize / 0x74;
    floorMapResourceDataIterate(floorMapPopCB, &data);
}

/* 0x80112260 | 0x120: XD floorMapSetDisp (0x8011E374) */
void floorSetMapVisibility(u8 visible) {
    floorMapResourceDataIterate(floorMapSetDispCB, (void*)visible);
}
