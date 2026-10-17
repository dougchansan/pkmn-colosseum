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
extern s32 cameraGetResAnimFrame(void*, u32);
extern void cameraPlayOffsetAnime(void*, u32, s32, u32);
extern void cameraSetOffsetPosition(f32*);
extern void cameraSetOffsetRotation(f32*);
extern void cameraSetOffsetScale(f32*);
extern s32 wazaSequenceOwnerGetSizeClass(void*);
extern FloorData* floorDataBiosGetCurrentPtr(void);
extern void* std093_getCurrentGroupID(void);
extern void set__5GSvecFfff(f32*, f32, f32, f32);
extern void* GSresGetResource(void*, u32);
extern ModelList* HSD_ArchiveGetPublicAddress(void*, const char*);
extern u32 floorReadMakeModelResID(u32);
extern void GSmodelSetScale(void*, f32*);
extern void fn_800D1070(u32);
extern void cameraUpdate(void);

typedef struct BattleCameraVec {
    f32 x;
    f32 y;
    f32 z;
} BattleCameraVec;

static inline s32 battleCameraMaxField(void)
{
    u16 i;
    u8** pokemon;
    BattleGridGroupEntry* group;
    u16 j;
    s32 maxField;

    group = lbl_80466DE8;
    maxField = -2;
    if (*(u16*)((u8*)group + 0x40) == 0) {
        return 0;
    }
    for (i = 0; i < 4; i++, group++) {
        if (group->slot != NULL) {
            pokemon = group->pokemon;
            for (j = 0; j < 2; j++, pokemon++) {
                if (*pokemon != NULL) {
                    s32 field = wazaSequenceOwnerGetSizeClass(*pokemon);
                    if (field > maxField) {
                        maxField = field;
                    }
                }
            }
        }
    }
    return maxField;
}

void battleCameraStartOffsetAnime(void* arg0, u32 arg1)
{
    ModelList* modelList;
    f32 scaleValue;
    s32 anim;
    s32 maxField;
    void* resource;
    FloorData* floor;
    void* resourceBase;
    u32 k;
    u32 modelId;
    BattleCameraVec offset;
    f32 scale[3];

    offset = *(BattleCameraVec*)lbl_802757F0;
    cameraMoveStop();
    anim = cameraGetResAnimFrame(arg0, arg1);
    cameraPlayOffsetAnime(arg0, arg1, anim, 1);
    cameraSetOffsetPosition(&offset.x);
    cameraSetOffsetRotation(&offset.x);

    maxField = battleCameraMaxField();

    floor = floorDataBiosGetCurrentPtr();
    resourceBase = std093_getCurrentGroupID();
    k = 0;
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

    resource = GSresGetResource(std093_getCurrentGroupID(), floor->resourceId);
    if (resource != NULL) {
        modelList = HSD_ArchiveGetPublicAddress(resource, lbl_802757FC);
        if (modelList != NULL && modelList->models != NULL) {
            modelId = floorReadMakeModelResID(floor->resourceId);
            for (; modelList->models[k] != NULL; k++) {
                void* model = GSresGetResource(resourceBase, modelId | k);
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
