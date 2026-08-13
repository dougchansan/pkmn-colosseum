#include "dolphin/types.h"
#include "game/battle/battle_grid_types.h"

typedef struct BattleGridGroupEntry {
    u8* slot;
    u8* pokemon[2];
    u16 memberCount;
    u8 arg1;
    u8 arg2;
} BattleGridGroupEntry;

typedef struct FloorData {
    u8 pad_00[8];
    u32 resourceId;
} FloorData;

typedef struct ModelList {
    void** models;
} ModelList;

extern BattleGridGroupEntry lbl_80466DE8[];
extern s32 lbl_80478CA8;
extern u8 lbl_8047B398;
extern const f32 lbl_8047DF5C;
extern const f32 lbl_8047DF60;
extern const f32 lbl_8047DF64;
extern const f32 lbl_8047DF68;
extern char lbl_802757F0[];
extern char lbl_802757FC[];

extern void cameraMoveStop(void);
extern s32 fn_80176C04(void*, u32);
extern void cameraPlayOffsetAnime(void*, u32, s32, u32);
extern void cameraSetOffsetPosition(f32*);
extern void cameraSetOffsetRotation(f32*);
extern void cameraSetOffsetScale(f32*);
extern s32 fn_801DAC24(void*);
extern FloorData* floorDataBiosGetCurrentPtr(void);
extern void* fn_80113F48(void);
extern void set__5GSvecFfff(f32*, f32, f32, f32);
extern void* GSresGetResource(void*, u32);
extern ModelList* HSD_ArchiveGetPublicAddress(void*, const char*);
extern u32 floorReadMakeModelResID(u32);
extern void GSmodelSetScale(void*, f32*);
extern void fn_800D1070(u32);
extern void cameraUpdate(void);

void fn_801C2F00(void* arg0, u32 arg1)
{
    BattleGridGroupEntry* group;
    FloorData* floor;
    ModelList* modelList;
    void* resourceBase;
    void* resource;
    s32 maxField;
    u32 modelId;
    f32 scaleValue;
    u16 i;
    u16 j;
    f32 scale[3];

    group = lbl_80466DE8;
    maxField = -2;

    {
        u32* initial = (u32*)lbl_802757F0;
        scale[0] = *(f32*)&initial[0];
        scale[1] = *(f32*)&initial[1];
        scale[2] = *(f32*)&initial[2];
    }

    cameraMoveStop();
    cameraPlayOffsetAnime(arg0, arg1, fn_80176C04(arg0, arg1), 1);
    cameraSetOffsetPosition(scale);
    cameraSetOffsetRotation(scale);

    if (group[4].memberCount == 0) {
        maxField = 0;
    } else {
        for (i = 0; i < 4; i++, group++) {
            if (group->slot != NULL) {
                for (j = 0; j < 2; j++) {
                    if (group->pokemon[j] != NULL) {
                        s32 field = fn_801DAC24(group->pokemon[j]);
                        if (field > maxField) {
                            maxField = field;
                        }
                    }
                }
            }
        }
    }

    floor = floorDataBiosGetCurrentPtr();
    resourceBase = fn_80113F48();
    switch (maxField) {
    case 1:
        scaleValue = lbl_8047DF5C;
        break;
    case 2:
        scaleValue = lbl_8047DF60;
        break;
    case 3:
        scaleValue = lbl_8047DF64;
        break;
    default:
        scaleValue = lbl_8047DF68;
        break;
    }
    set__5GSvecFfff(scale, scaleValue, scaleValue, scaleValue);

    resource = GSresGetResource(fn_80113F48(), floor->resourceId);
    if (resource != NULL) {
        modelList = HSD_ArchiveGetPublicAddress(resource, lbl_802757FC);
        if (modelList != NULL && modelList->models != NULL) {
            modelId = floorReadMakeModelResID(floor->resourceId);
            for (i = 0; modelList->models[i] != NULL; i++) {
                void* model = GSresGetResource(resourceBase, modelId | i);
                if (model != NULL) {
                    GSmodelSetScale(model, scale);
                }
            }
        }
    }

    cameraSetOffsetScale(scale);
    fn_800D1070(0);
    cameraUpdate();
    lbl_80478CA8 = 200;
    lbl_8047B398 = 0;
}
