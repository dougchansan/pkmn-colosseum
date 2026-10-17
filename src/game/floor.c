/**
 * @file floor.c
 * @brief floor -- fade/transition control, object list, character init,
 * model load/animation, and floor resource accessors.
 *
 * Sixth of six translation units recovered from the former
 * game/gs_field_colquery.c CodeCandidate bucket (0x8010F6A0-0x801140DC).
 * Unusually large vs. XD's floor.cpp TU size (our span ~0x1D5C bytes vs.
 * XD's ~0x3244 bytes) -- interpreted as Colosseum's floor.cpp/field-
 * control file carrying substantially more Colosseum-specific logic
 * (battle-arena field transitions etc.) than XD's slimmed-down version
 * of the same file. The 8 anchors inside it (floorCheckFightKind ..
 * EvlogSet, the last of which lands just past our end boundary) appear
 * in exactly the same relative order as in XD, which is strong evidence
 * this is still one TU rather than several.
 *
 * fn_801123D4 and fn_801129CC previously carried invented
 * "GSfield_ResourceInit" / "GSfield_UpdateObjects" names/signatures
 * from an earlier bad campaign pass (same class of issue documented in
 * include/game/gs_colsys.h); reverted to the standard fn_<addr>
 * placeholder since no confirmed symbols.txt name exists yet for
 * either.
 *
 * Address range: 0x80112380 - 0x801140DC (.text), .rodata
 * 0x80272088 - 0x802721FA, .sdata2 0x8047CF70 - 0x8047CF9C. Linked.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"
#include "game/gs_model_anim.h"

/*
 * Floor record (0x4C bytes per entry in the lbl_80478FBC floor table; see
 * floorDataBiosGetPtr). The floorDataBios* accessors in floor_data.c read
 * these fields.
 */
typedef struct FloorData {
    /* 0x00 */ u8 kind : 3;
    /* 0x00 */ u8 flags : 5;
    /* 0x01 */ u8 area;
    /* 0x02 */ u8 pad02[2];
    /* 0x04 */ u32 groupId;
    /* 0x08 */ u32 mapResId;
    /* 0x0C */ u32 floorId;
    /* 0x10 */ u8 unk10[0x3C];
} FloorData;

/* Start position record in a floor's position list (0x10 bytes). */
typedef struct FloorPos {
    /* 0x00 */ s16 direction;
    /* 0x02 */ u8 pad02[2];
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
} FloorPos;

typedef struct FloorPosHeader {
    /* 0x00 */ u32 count;
} FloorPosHeader;

typedef struct FloorPosList {
    /* 0x00 */ FloorPosHeader* header;
    /* 0x04 */ FloorPos* entries;
} FloorPosList;

/* Public "scene data" block of a floor's map archive. */
typedef struct FloorMapHeader {
    /* 0x00 */ void** models;
    /* 0x04 */ u32 pad04;
    /* 0x08 */ void** lights;
} FloorMapHeader;

typedef struct FloorViewVec {
    f32 x, y, z;
} FloorViewVec;

/* Default floorRead handler triple registered for resource types 1 and 2. */
typedef struct FloorReadFuncs {
    void* func[3];
} FloorReadFuncs;

typedef struct FloorColor {
    f32 r, g, b, a;
} FloorColor;

extern void _floorRestoreStateData(void);
extern void _floorMakeStateData(void);
extern void _floorGetStateSize(void);

/*
 * This TU's .rodata, in retail order (0x80272088-0x802720BB), followed by
 * its log strings (non-pooled, readonly: -str reuse,readonly). The unit
 * owns .rodata 0x80272088-0x802721FA and its literal pool, .sdata2
 * 0x8047CF70-0x8047CF9C (see splits.txt).
 */
static const FloorViewVec sFloorCameraView = {0.0f, 14.0f, 0.0f};
static const FloorReadFuncs sFloorDefaultReadFuncs = {
    _floorRestoreStateData, _floorMakeStateData, _floorGetStateSize,
};
static const FloorColor sFloorFadeColor = {0.0f, 0.0f, 0.0f, 0.0f};
/* Name of a map archive's public scene descriptor; GScolsys2 reads it too. */
const char lbl_802720B0[] = "scene_data";

extern FloorData* floorDataBiosGetPtr(u32 floorId);
extern FloorData* floorDataBiosGetCurrentPtr(void);
extern u32 floorDataBiosGetGroupID(FloorData* floor);
extern u8 floorDataBiosGetFloorKind(FloorData* floor);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void floorTerminateTasks(void* callback);

/* Group ID of the current floor; expanded three times in fn_801129CC. */
static inline u32 floorGetCurrentGroupID(void) {
    return floorDataBiosGetGroupID(floorDataBiosGetCurrentPtr());
}

/* 0x80112380 | 0x54 */
s32 floorCheckFightKind(u32 id) {
    s32 ready = 0;

    if (id != 0xFFFFFFFF) {
        switch (floorDataBiosGetFloorKind(floorDataBiosGetPtr(id))) {
        case 2:
            ready = 1;
            break;
        }
    }
    return ready;
}

/*
 * Resident-archive helpers used by fn_801123D4. The sync expansion occurs
 * six times there (ids 0x9A/0x0C/0x0B for kind 6, 0x0C for kind 1, 0x0B
 * for kind 2) and the release wait three times, each with identical call
 * order and constants: repeated-expansion evidence for static inlines.
 */
extern s32 fn_8017B2CC(s32 id);
extern void fn_800F915C(s32 id);
extern void GSresReleaseById(s32 id);
extern void _threadSwitch(void);

/* Wait out a pending load (status 1), then commit a loaded archive. */
static inline void floorSyncResident(s32 id) {
    switch (fn_8017B2CC(id)) {
    case 1:
        while (fn_8017B2CC(id) == 1) {
            _threadSwitch();
        }
    case 0:
        fn_800F915C(id);
        GSresReleaseById(id);
        break;
    case -1:
        break;
    }
}

/* Poll until a requested resident change completes, logging errors. */
static inline void floorWaitResident(s32 id) {
    s32 status;

    while (TRUE) {
        status = fn_8017B2CC(id);
        if (status < 0) {
            GSlogWrite("読み出しエラー\n");
        } else if (status == 0) {
            break;
        }
        _threadSwitch();
    }
}

/* 0x801123D4 | 0x32C */
void fn_801123D4(FloorData* floor) {
    extern void GSscene_SetMode(s32);
    extern void fn_800D36B4(FloorColor color);
    extern void fsysRequestEntry(s32, u32);
    extern void fn_8017B3E4(s32);
    extern u32 heroMoveGetKenObjID(void);
    FloorColor color = sFloorFadeColor;
    u32 objectId;

    GSscene_SetMode(3);
    fn_800D36B4(color);

    switch (floorDataBiosGetFloorKind(floor)) {
    case 6:
        floorSyncResident(0x9A);
        floorSyncResident(0x0C);
        floorSyncResident(0x0B);
        break;
    case 1:
        floorSyncResident(0x0C);
        objectId = heroMoveGetKenObjID();
        if (objectId != 0x00F70400 && fn_8017B2CC(0x9A) != 0) {
            fsysRequestEntry(0x9A, objectId);
            floorWaitResident(0x9A);
        }
        if (fn_8017B2CC(0x0B) != 0) {
            fn_8017B3E4(0x0B);
            floorWaitResident(0x0B);
        }
        break;
    case 2:
        floorSyncResident(0x0B);
        if (fn_8017B2CC(0x0C) != 0) {
            fn_8017B3E4(0x0C);
            floorWaitResident(0x0C);
        }
        break;
    }
}

/* 0x80112700 | 0x4C */
void floorStartSoundScript(void) {
    extern u32 floorDataBiosGetSoundSctID(FloorData*);
    extern void _vmThreadCreate(s32, u32, s32, s32, s32, ...);
    u32 id;

    if ((id = floorDataBiosGetSoundSctID(floorDataBiosGetCurrentPtr())) != 0) {
        _vmThreadCreate(0xF, id, 0x1000, 1, 0, 0);
    }
}

/* 0x8011274C | 0x34 */
void floorCheckFade(void) {
    extern void fadeCheck(s32);
    extern void fn_800D3074(s32);
    extern u8 lbl_80478DD0;

    fadeCheck(1);
    lbl_80478DD0 = 0;
    fn_800D3074(1);
}

/* 0x80112780 | 0x3C */
void fn_80112780(void) {
    extern void cmsRunSync(void* callback, s32 arg, ...);
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;
    void* callback;

    callback = (void*)state->transitionBeginCallback;
    if (callback != NULL) {
        cmsRunSync(callback, 0);
    }
}

/* 0x801127BC | 0x88 */
void fn_801127BC(void) {
    extern u8 lbl_80478DD0;
    extern void fn_800D3074(s32);
    extern u32 floorGetCurrentContext(void);
    extern void fn_800FF2A0(u32, u32, void*);
    extern void fn_80112844(void);
    s32 kind;

    lbl_80478DD0 = 1;
    kind = floorDataBiosGetFloorKind(floorDataBiosGetCurrentPtr());
    switch (kind) {
    case 5:
    case 6:
        fn_800D3074(1);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    default:
        fn_800D3074(2);
        break;
    }
    if (*(u32*)(lbl_80408378 + 0x28) != 0) {
        fn_800FF2A0(0, floorGetCurrentContext(), (void*)fn_80112844);
    }
}

/* 0x80112844 | 0x48 */
void fn_80112844(void) {
    extern void cmsRunSync(void* callback, s32 arg, ...);
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;
    void* callback;

    callback = (void*)state->transitionPollCallback;
    if (callback != NULL) {
        cmsRunSync(callback, 0);
        floorTerminateTasks(fn_80112844);
    }
}

/* 0x8011288C | 0x14 */
void floorSetFadeScript(u32 a, u32 b) {
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;

    state->transitionPollCallback = b;
    state->transitionBeginCallback = a;
}

/* 0x801128A0 | 0x10C */
void fn_801128A0(void) {
    extern u8 lbl_80408378[];
    extern void fn_800FF3C0(s32, s32, void*);
    extern void fn_800FF178(s32, s32, void*);
    extern void fn_800FF4D4(void*, u32);
    extern s32 GSgetVersionMinor(void);
    extern void GSfloorOpen(u32);
    extern void fn_800FF784(u32);
    extern void fn_80118020(void);
    extern void floorResolveEntryPosition(void);
    extern void fn_801129AC(void);
    FloorReadFuncs funcs = sFloorDefaultReadFuncs;
    u32 r;

    fn_800FF3C0(0, 0x5000, (void*)floorResolveEntryPosition);
    fn_800FF178(0xFF, 0x5000, (void*)fn_801129AC);
    fn_800FF4D4(&funcs, 1);
    fn_800FF4D4(&funcs, 2);
    switch (GSgetVersionMinor()) {
    case 5:
        r = 0x3E4;
        break;
    case 4:
        r = 0x320;
        break;
    case 1:
        r = 0x3E6;
        break;
    case 2:
        r = 0x3E7;
        break;
    case 3:
    default:
        r = 0x399;
        break;
    }
    *(u32*)(lbl_80408378 + 0x0) = 0;
    *(u32*)(lbl_80408378 + 0x4) = r;
    *(u8*)(lbl_80408378 + 0x8) = 1;
    *(u32*)(lbl_80408378 + 0xC) = 0;
    GSfloorOpen(r);
    fn_800FF784(r);
    fn_80118020();
}

/* 0x801129AC | 0x20 */
void fn_801129AC(void) {
    extern void mailMainReceiveTerminate(void);
    mailMainReceiveTerminate();
}

/*
 * fn_801129CC's first phase (floorEnterReset below) was an inline function
 * in the original source. Retail zero-initialises its two area locals with
 * `li r28,0; mr r31,r28`: the zero is routed through one register and
 * copied to the other's home register. Controlled compiler tests on the
 * same code written in place -- initialised in the declarations, by
 * statements or at block scope -- give two separate `li` under GC/1.1,
 * 1.2.5n, 1.3, 1.3.2, 2.0, 2.5, 2.6, 2.7 and 3.0a3 at -O2, -O3, -O4,p,
 * -O4,s, with -opt nopeephole, -opt nocse, -opt noprop, -inline deferred,
 * -inline auto,deferred and -lang=c++ (0 of 99 configurations each). A
 * chained `a = b = 0` does the same everywhere except at -O2, which is
 * excluded for this TU (at -O2 the rest of floor.c drops to 84-96%). The
 * copy appears when the same code is the body of an inlined function (70
 * of 99, including this TU's GC/1.3 -O4,p -opt nopeephole).
 *
 * The map-visibility and sun phase that follows (floorInitScene below) does
 * not match exactly when written in place
 * (99.8%): the loop index and its strength-reduced table offset take
 * r27/r28 the other way round from retail. The inline helper fixes that.
 * The helper's evidence is Pokemon XD's named function of the same body,
 * which the sister-title clause admits (see floorInitScene).
 *
 * Why no in-place form can fix it (GC/2.6 regalloc replay, which is exact
 * for this function): MWCC numbers a function's own locals below every
 * frontend temporary (parameter r32, block locals r33-r37, top-level locals
 * r38-r41, temporaries from r42 up in reverse creation order), and colours
 * everything that is not a spill candidate in descending virtual-register
 * order. The loop's strength-reduced offset is frontend temporary @261
 * (r50), so it is always coloured before any declared local, and `index`
 * (r34) then takes the next free register (r27). For retail's order `index`
 * must itself be a temporary created before @261, i.e. a local of an inlined
 * body, or be a spill candidate (about 28+ neighbours; it has 22, and its
 * range is fixed by retail's `li` right after the map-ID test and its last
 * use in the loop). Tried (objdiff-cli, 99.78% now): index at function
 * scope 99.74%, initialised in its declaration 99.78% (same order), set in
 * the for header 99.27% (moves the `li`), and folding the loop into
 * floorEnterReset 99.71%.
 */
extern u8 fn_800FF548(void);
extern u8 fn_800FF554(void);
extern u8 fn_800FF52C(void);
extern void GSflagClear(u32);
extern u8 floorDataBiosGetArea(FloorData*);
extern u32 floorDataBiosGetMapResID(FloorData*);
extern FloorPosList* floorDataBiosGetPosListPtr(FloorData*);
extern u32 floorDataBiosGetSunResID(FloorData*);
extern void* GSresGetResource(u32, u32);
extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
extern u32 floorReadMakeModelResID(u32);
extern void GSmodelSetVisibility(void*, u32);
extern void* GSmodelGetPart(void*, u32);
extern void GSpartGetTransform(void*, void*, void*, void*);
extern void GSpartFree(void*);
extern void flareSetEnable(u32);
extern void flareSetSunPos(void*);
extern void msgctrlInitValue(void);
extern void fn_801CBA84(void);

/*
 * Reset per-floor state on entering a floor: clear the floor flags (and the
 * area flags when the area changes), reset the message control values, and
 * take the hero start position from the link request, if any.
 */
static inline void floorEnterReset(void) {
    FloorPos* pos;
    u8 area = 0;
    u8 nextArea = 0;
    FloorData* data;
    FloorPosList* list;

    /*
     * Clear the per-floor flags on a floor change, and the per-area flags
     * when the next floor lies in another area (fights keep them).
     */
    if (fn_800FF548() != 1 && fn_800FF554() != 1 && fn_800FF52C() == 0) {
        GSflagClear(1);
        data = floorDataBiosGetPtr(*(u32*)(lbl_80408378 + 0x0));
        if (data != NULL) {
            area = floorDataBiosGetArea(data);
        }
        /* Retail reads the kind even when the lookup above failed. */
        if (data->kind != 2) {
            data = floorDataBiosGetPtr(*(u32*)(lbl_80408378 + 0x4));
            if (data != NULL) {
                nextArea = floorDataBiosGetArea(data);
            }
            if (area != nextArea) {
                GSflagClear(2);
            }
        }
    }

    msgctrlInitValue();
    fn_801CBA84();

    /*
     * Take the hero start position/direction from the current floor's
     * position list when a link requested one (floorLink sets the flag and
     * index).
     */
    pos = NULL;
    if (*(u8*)(lbl_80408378 + 0x8) != 0) {
        list = floorDataBiosGetPosListPtr(floorDataBiosGetCurrentPtr());
        if (list != NULL && *(u32*)(lbl_80408378 + 0xC) < list->header->count) {
            pos = &list->entries[*(u32*)(lbl_80408378 + 0xC)];
        }
        if (pos != NULL) {
            set__5GSvecFfff(lbl_80408378 + 0x10, pos->x, pos->y, pos->z);
            set__5GSvecFfff(lbl_80408378 + 0x1C, 0.0f,
                            0.017453292f * pos->direction, 0.0f);
        } else {
            set__5GSvecFfff(lbl_80408378 + 0x10, 0.0f, 0.0f, 0.0f);
            set__5GSvecFfff(lbl_80408378 + 0x1C, 0.0f, 0.0f, 0.0f);
        }
        *(u8*)(lbl_80408378 + 0x8) = 0;
    }
}


/*
 * Helper admitted under the same-engine sister-title clause of docs/CAMPAIGN_OPERATIONS.md
 * (user decision, 2026-09-28; XD names from TeamOrre/xd-decomp symbols.txt at 4989794e,
 * XD code from trevor403/xd-asm at b1087f18; see docs/recon/title_walls_evidence.md): show the current
 * floor's map models and aim the sun. Pokemon XD has this body as the static
 * _floorInitScene__FP11GSfloor_dd_ (XD 0x8011F368, called from
 * _floorInitializeCommon): the discarded floorDataBiosGetGroupID(floor), the
 * map-ID test, `li r28,0`, the scene_data model loop re-reading the current
 * group ID, then the sun/lens-flare part. Colosseum has no second expansion.
 * Written as this inline, fn_801129CC is exact.
 */
static inline void floorInitScene(FloorData* floor) {
    FloorData* data;
    FloorMapHeader* map;
    u32 baseId;
    u32 index;
    void* model;
    u32 resId;
    void* part;
    f32 sunTransform[3];

    floorDataBiosGetGroupID(floor); /* result unused in retail */
    if (floorDataBiosGetMapResID(floor) != 0) {
        index = 0;
        data = floorDataBiosGetCurrentPtr();
        map = HSD_ArchiveGetPublicAddress(
            GSresGetResource(floorGetCurrentGroupID(), data->mapResId),
            lbl_802720B0);
        if (map != NULL && map->models != NULL) {
            baseId = floorReadMakeModelResID(data->mapResId);
            for (; map->models[index] != NULL; index++) {
                model = GSresGetResource(floorGetCurrentGroupID(), baseId | index);
                if (model != NULL) {
                    GSmodelSetVisibility(model, 1);
                }
            }
        }
    }

    flareSetEnable(0);
    resId = floorDataBiosGetSunResID(floor);
    if (resId != 0) {
        part = GSmodelGetPart(GSresGetResource(floorGetCurrentGroupID(), resId), 0);
        if (part != NULL) {
            GSpartGetTransform(part, sunTransform, NULL, NULL);
            flareSetSunPos(sunTransform);
            flareSetEnable(1);
            GSpartFree(part);
        }
    }
}

/* 0x801129CC | 0x5C0 */
void floorResolveEntryPosition(FloorData* floor) {
    extern void tvInitFloor(FloorData*);
    extern void GSmodelSetShadowBoundExpansion(u32, u32);
    extern void GSmaterialSetDistanceThreshold(f32);
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern void* floorDataBiosGetModifySctID(FloorData*);
    extern void cmsRunSync(void* callback, s32 arg, ...);
    extern u32 floorGetCurrentContext(void);
    extern u32 floorDataBiosGetMainSctID(FloorData*);
    extern u32 floorDataBiosGetPreSctID(FloorData*);
    extern u32 floorDataBiosGetPostSctID(FloorData*);
    extern void floorDataBiosSetMainSctID(FloorData*, u32);
    extern void fn_800FF3C0(u8, u32, void*);
    extern void fn_800FF2A0(u8, u32, void*);
    extern void fn_800FF178(u8, u32, void*);
    extern void fn_800FEF8C(u8, u32, u32);
    extern void fn_800FEE68(u8, u32, u32);
    extern void fn_800FED3C(u8, u32, u32);
    extern void* floorDataBiosGetPreFunc(FloorData*);
    extern void* floorDataBiosGetPostFunc(FloorData*);
    extern void* floorDataBiosGetMainFunc(FloorData*);
    extern void _floorInitialize__FUi14FloorEnterMode(void);
    extern void _floorUpdate__FUi14FloorEnterMode(void);
    extern void _fightInitialize__FUi14FloorEnterMode(void);
    extern void _fightFinalize__FUi14FloorEnterMode(void);
    extern void fn_801EF488(void);
    extern void _wazaViewerInitialize(void);
    extern void _wazaViewerUpdate(void);
    extern void _wazaViewerFinalize(void);
    extern void fn_801139BC(void);
    extern void floorStartMainThread(FloorData*);
    void* script;
    u32 resId;
    u32 task;

    floorEnterReset();
    floorInitScene(floor);
    tvInitFloor(floor);

    switch (floorDataBiosGetFloorKind(floor)) {
    case 1:
        GSmodelSetShadowBoundExpansion(0, 0);
        switch ((s32)floor->floorId) {
        case 0x72:
        case 0x73:
        case 0x74:
            GSmaterialSetDistanceThreshold(0.0f);
            GSmodelSetShadowTextureSize(0x280, 0x1E0);
            break;
        default:
            GSmaterialSetDistanceThreshold(70.0f);
            GSmodelSetShadowTextureSize(0x180, 0x180);
            break;
        }
        break;
    case 2:
    case 4:
        GSmodelSetShadowTextureSize(0x280, 0x1E0);
        GSmaterialSetDistanceThreshold(80.0f);
        GSmodelSetShadowBoundExpansion(0x0B, 7);
        break;
    case 3:
    case 5:
    case 6:
        break;
    }

    if ((script = floorDataBiosGetModifySctID(floor)) != NULL) {
        cmsRunSync(script, 0);
    }
    task = floorGetCurrentContext();
    if (fn_800FF548() == 0) {
        switch (floorDataBiosGetFloorKind(floor)) {
        case 1:
            if (floorDataBiosGetMainSctID(floor) == 0) {
                floorDataBiosSetMainSctID(floor, 0x05960003);
            }
            fn_800FF3C0(10, task, _floorInitialize__FUi14FloorEnterMode);
            fn_800FF2A0(20, task, _floorUpdate__FUi14FloorEnterMode);
            fn_800FF178(10, task, fn_801139BC);
            break;
        case 2:
            fn_800FF3C0(15, task, _fightInitialize__FUi14FloorEnterMode);
            fn_800FF2A0(20, task, fn_801EF488);
            fn_800FF178(30, task, _fightFinalize__FUi14FloorEnterMode);
            break;
        case 4:
            fn_800FF3C0(15, task, _wazaViewerInitialize);
            fn_800FF2A0(20, task, _wazaViewerUpdate);
            fn_800FF178(25, task, _wazaViewerFinalize);
            break;
        case 5:
        case 6:
            fn_800FF2A0(20, task, _floorUpdate__FUi14FloorEnterMode);
            break;
        case 3:
            break;
        }
        if ((resId = floorDataBiosGetPreSctID(floor)) != 0) {
            fn_800FEF8C(15, task, resId);
        }
        if ((resId = floorDataBiosGetMainSctID(floor)) != 0) {
            fn_800FEE68(15, task, resId);
        }
        if ((resId = floorDataBiosGetPostSctID(floor)) != 0) {
            fn_800FED3C(15, task, resId);
        }
        if ((script = floorDataBiosGetPreFunc(floor)) != NULL) {
            fn_800FF3C0(15, task, script);
        }
        if ((script = floorDataBiosGetPostFunc(floor)) != NULL) {
            fn_800FF178(15, task, script);
        }
        if (floorDataBiosGetMainFunc(floor) != NULL) {
            fn_800FF2A0(15, task, floorStartMainThread);
        }
    }
}

/* 0x80112F8C | 0x60 */
void floorStartMainThread(FloorData* floor) {
    extern void* floorDataBiosGetMainFunc(FloorData*);
    extern u32 floorGetCurrentContext(void);
    extern void GSthreadCreate(s32 a, u32 b, u32 c, u32 d, u32 e, void* f);
    void* obj;

    obj = floorDataBiosGetMainFunc(floor);
    if (obj != NULL) {
        GSthreadCreate(1, floorGetCurrentContext(), 0x4000, 1, 1, obj);
    }
    floorTerminateTasks(floorStartMainThread);
}

/* 0x80112FEC | 0x25C */
void floorInitPeople(FloorData* floor)
{
    typedef struct FloorObjectEntry {
        /* 0x00 */ u8 kind : 3;
        /* 0x00 */ u8 flags : 5;
        /* 0x01 */ u8 pad_01;
        /* 0x02 */ s16 rotation;
        /* 0x04 */ u16 floorId;
        /* 0x06 */ u16 eventFlag;
        /* 0x08 */ u16 hideFlag;
        /* 0x0A */ u8 pad_0A[6];
        /* 0x10 */ f32 x;
        /* 0x14 */ f32 y;
        /* 0x18 */ f32 z;
    } FloorObjectEntry;
    extern u8 fn_800FF548(void);
    extern void* peopleOpen(u32, u32, u32);
    extern void* peopleIsInGroup(u32, u32);
    extern void set__5GSvecFfff(void*, f32, f32, f32);
    extern void peopleSetPositionByIndex(u32, u32, void*);
    extern void peopleSetRotation(u32, u32, void*);
    extern void peopleRegisterHumanCollision(u32, u32);
    extern void peopleSetFlagsByIndex(u32, u32, s32);
    extern void peopleSetVisible(u32, u32, s32);
    extern void* GSresGetResource(u32, u32);
    extern void GSmodelClearShadowFlags(void*, u32);
    extern u8 std081_flagCheckTrue(u16);
    extern void floorEventCtrlTresure(u32, u32, u32);
    extern void peopleStartScript(u32, u32, s32, u32, s32);
    extern u32* lbl_80478EB8;
    extern FloorObjectEntry* lbl_80478EBC;
    FloorObjectEntry* entry;
    u32 i;
    u32 group;
    u32 resourceId;
    u32 peopleInfo;
    u32 objectIndex = 0;
    void* object;
    f32 position[3];
    f32 rotation[3];

    group = floorDataBiosGetGroupID(floor);
    for (i = 0; i < *lbl_80478EB8; i++) {
        entry = &lbl_80478EBC[i];
        if (entry->floorId != floor->floorId) {
            continue;
        }
        /*
         * No default: for any other kind retail goes on with peopleInfo
         * unset (the switch falls straight through to the object setup).
         */
        switch (entry->kind) {
        case 1:
            peopleInfo = 0x03770400;
            break;
        case 2:
            peopleInfo = 0x03780400;
            break;
        case 3:
            peopleInfo = 0x03790400;
            break;
        }

        resourceId = 0x7FFF0000 | objectIndex++;
        if (fn_800FF548() == 0) {
            object = peopleOpen(group, resourceId, peopleInfo);
        } else {
            object = peopleIsInGroup(group, resourceId);
        }
        if (object == NULL || fn_800FF548() == 1) {
            continue;
        }

        set__5GSvecFfff(position, entry->x, entry->y, entry->z);
        peopleSetPositionByIndex(group, resourceId, position);
        set__5GSvecFfff(rotation, 0.0f, 0.017453292f * entry->rotation, 0.0f);
        peopleSetRotation(group, resourceId, rotation);
        peopleRegisterHumanCollision(group, resourceId);
        peopleSetFlagsByIndex(group, resourceId, 4);
        peopleSetVisible(group, resourceId, 1);

        if (entry->kind == 2) {
            object = GSresGetResource(group, resourceId);
            if (object != NULL) {
                GSmodelClearShadowFlags(object, 1);
            }
        }
        if (entry->eventFlag != 0) {
            if (std081_flagCheckTrue(entry->eventFlag)) {
                floorEventCtrlTresure(group, resourceId, 0);
            } else {
                floorEventCtrlTresure(group, resourceId, 1);
                if (entry->hideFlag != 0) {
                    if (!std081_flagCheckTrue(entry->hideFlag)) {
                        peopleSetVisible(group, resourceId, 0);
                    }
                    peopleStartScript(group, resourceId, 0x0596000A,
                                entry->eventFlag, entry->hideFlag);
                }
            }
        }
    }
}

/* 0x80113248 | 0x29C */
void _floorInitCharacters__FP11GSfloor_dd_(FloorData* floor) {
    extern u32 floorDataBiosGetCharNum(FloorData* floor);
    extern u8* floorCharacterBiosGetByIndex(u32 i);
    extern void* floorCharacterBiosGetPeopleInfoPtr(u8* obj);
    extern u32 fn_8018F6B4(void* info);
    extern u8 fn_800FF548(void);
    extern u32 peopleOpen(u32 model, u32 i, s32 x);
    extern u32 peopleIsInGroup(u32 model, u32 i);
    extern s32 fn_80183958(u32 model, u32 i);
    extern void peopleStartScript(u32 model, u32 i, s32 a, u32 b, s32 c);
    extern void peopleSetFlagsByIndex(u32 model, u32 i, s32 flag);
    extern void floorCharacterBiosGetPos(u8* obj, void* out);
    extern void peopleSetPositionByIndex(u32 model, u32 i, void* p);
    extern void floorCharacterBiosGetRot(u8* obj, void* out);
    extern void peopleSetRotation(u32 model, u32 i, void* p);
    extern void peopleRegisterHumanCollision(u32 model, u32 i);
    extern s32 floorCharacterBiosGetVisibility(u8* obj);
    extern void peopleSetVisible(u32 model, u32 i, s32 x);
    extern s32 floorCharacterBiosGetLoadInit(u8* obj);
    extern void std151_characterFaceDirection(u32 model, u32 i, s32 x);
    extern u8 floorCharacterBiosGetMoveType(u8* obj);
    extern void peopleStartWander(u32 model, u32 i, f32 x);
    extern void fn_801839A0(u32 model, u32 i, f32 x, f32 y);
    extern u8 floorCharacterBiosGetTalkStartType(u8* obj);
    extern u8 floorCharacterBiosGetTalkEndType(u8* obj);
    extern void peopleClearFlagsByIndex(u32 model, u32 i, s32 flag);
    extern void std065_characterLookAtPlayer(u32 model, u32 i);
    extern const char lbl_8035B888[];
    u32 model;
    u32 count;
    u32 i;
    u32 result;
    u8* obj;
    f32 v14[3];
    f32 v8[3];

    model = floorDataBiosGetGroupID(floor);
    count = floorDataBiosGetCharNum(floor);
    for (i = 0; i < count; i++) {
        obj = floorCharacterBiosGetByIndex(i);
        result = fn_8018F6B4(floorCharacterBiosGetPeopleInfoPtr(obj));
        if (fn_800FF548() == 0) {
            result = peopleOpen(model, i, result);
        } else {
            result = peopleIsInGroup(model, i);
        }
        if (result == 0) {
            GSlogWrite("Error! [%s] 人の初期化に失敗\n", lbl_8035B888);
        } else if (fn_800FF548() != 1) {
            {
                s32 emitId;

                emitId = fn_80183958(model, i);
                peopleStartScript(model, i, emitId, result, 0);
            }
            peopleSetFlagsByIndex(model, i, 4);
            peopleSetFlagsByIndex(model, i, 8);
            floorCharacterBiosGetPos(obj, v14);
            peopleSetPositionByIndex(model, i, v14);
            floorCharacterBiosGetRot(obj, v8);
            peopleSetRotation(model, i, v8);
            peopleRegisterHumanCollision(model, i);
            peopleSetVisible(model, i, floorCharacterBiosGetVisibility(obj));
            std151_characterFaceDirection(model, i, floorCharacterBiosGetLoadInit(obj));
            switch (floorCharacterBiosGetMoveType(obj)) {
            case 0:
            case 1:
                break;
            case 2:
                peopleStartWander(model, i, 15.0f);
                break;
            case 3:
                fn_801839A0(model, i, 5.0f, 3.0f);
                break;
            }
            switch (floorCharacterBiosGetTalkStartType(obj)) {
            case 0:
                break;
            case 1:
                peopleSetFlagsByIndex(model, i, 0x10);
                break;
            case 2:
                peopleSetFlagsByIndex(model, i, 0x20);
                break;
            }
            switch (floorCharacterBiosGetTalkEndType(obj)) {
            case 0:
                peopleClearFlagsByIndex(model, i, 0x40);
                break;
            case 1:
                peopleSetFlagsByIndex(model, i, 0x40);
                break;
            }
            if (((u32)((*obj >> 5) & 1)) != 0U) {
                std065_characterLookAtPlayer(model, i);
            }
        }
    }
}

/* 0x801134E4 | 0x294 */
void floorInitMap(u32 group, u32 floorId)
{
    extern void* GSresGetResource(u32, u32);
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern u32 floorReadMakeModelResID(u32);
    extern u32 floorReadMakeLightResID(u32);
    extern u32 floorReadMakeCameraResID(u32);
    extern u32 floorReadMakeFogResID(u32);
    extern void fn_800F9210(u32, u32);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern u32 _floorUnloadModel__FPvUlUl(u32);
    extern void GSmodelSetVisibility(void*, u32);
    extern s32 GSgetVersionMajor(void);
    extern u32 std092_getCurrentRoomID(void);
    extern u8 GSmodelCanAnimate(void*);
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSlightSetActive(void*, u32);
    extern u8 GSlightCanAnimate(void*);
    extern void GSlightSetAnimIndex(void*, u32);
    extern void GSlightSetAnimRate(void*, f32);
    extern void GSlightStartAnimation(void*);
    extern void cameraSetGScamera(void*);
    extern void gfxSetFog(void*);
    extern const char lbl_8035B878[];
    u32 resourceId;
    void* object;
    void* lightObj;
    u32 modelBase;
    u32 lightBase;
    FloorMapHeader* map;
    u32 model = 0;
    u32 light = 0;
    u32 cameraId;
    void* camera;
    u32 fogId;

    if (floorId == 0) {
        return;
    }
    map = HSD_ArchiveGetPublicAddress(GSresGetResource(group, floorId),
                                      lbl_802720B0);
    if (map == NULL) {
        return;
    }

    if (map->models != NULL) {
        modelBase = floorReadMakeModelResID(floorId);
        for (; map->models[model] != NULL; model++) {
            resourceId = modelBase | model;
            object = floorOpenObject(resourceId);
            if (object == NULL) {
                GSlogWrite("Error! %s: couldn't load model %d\n", lbl_8035B878, model);
                GSlogWrite("マップモデルの初期化に失敗\n");
                continue;
            }
            fn_800F9210(group, resourceId);
            GSresRegisterResource(object, group, resourceId,
                                  _floorUnloadModel__FPvUlUl);
            GSmodelSetVisibility(object, 0);

            if (GSgetVersionMajor() == 1) {
                switch (std092_getCurrentRoomID()) {
                case 3:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0x15:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x72:
                case 0x75:
                case 0x7B:
                case 0xCA:
                case 0xD0:
                case 0xDE:
                    if (GSmodelCanAnimate(object)) {
                        GSmodelLinkTexAnimToAnim(object, 0);
                        GSmodelSetTexAnimRate(object, 0.35f);
                    }
                    break;
                }
            }
        }
    }

    if (map->lights != NULL) {
        lightBase = floorReadMakeLightResID(floorId);
        for (; map->lights[light] != NULL; light++) {
            lightObj = GSresGetResource(group, lightBase | light);
            if (lightObj != NULL) {
                GSlightSetActive(lightObj, 1);
                if (GSlightCanAnimate(lightObj)) {
                    GSlightSetAnimIndex(lightObj, 0);
                    GSlightSetAnimRate(lightObj, 0.5f);
                    GSlightStartAnimation(lightObj);
                }
            }
        }
    }

    cameraId = floorReadMakeCameraResID(floorId);
    camera = GSresGetResource(group, cameraId);
    if (camera != NULL) {
        cameraSetGScamera(camera);
    }
    fogId = floorReadMakeFogResID(floorId);
    gfxSetFog(GSresGetResource(group, fogId));
}

/* 0x80113778 | 0xB0 */
void floorChangePos(u32 floorId, s16 direction, f32 x, f32 y, f32 z) {
    extern u32 std092_getCurrentRoomID(void);
    extern void floorRequestChange(u32);
    if (floorId == 0) {
        return;
    }
    set__5GSvecFfff((void*)(lbl_80408378 + 0x10), x, y, z);
    set__5GSvecFfff((void*)(lbl_80408378 + 0x1C), 0.0f,
                    0.017453292f * direction, 0.0f);
    *(u32*)(lbl_80408378 + 0x0) = std092_getCurrentRoomID();
    *(u32*)(lbl_80408378 + 0x4) = floorId;
    *(u8*)(lbl_80408378 + 0x8) = 0;
    floorRequestChange(floorId);
}

/* 0x80113828 | 0x64 */
void floorLink(u32 arg0, s32 arg1) {
    extern u32 std092_getCurrentRoomID(void);
    extern void floorRequestChange(s32);

    if (arg0 != 0) {
        *(u32*)(lbl_80408378 + 0x0) = std092_getCurrentRoomID();
        *(s32*)(lbl_80408378 + 0x4) = arg0;
        *(u8*)(lbl_80408378 + 0x8) = 1;
        *(s32*)(lbl_80408378 + 0xC) = arg1;
        floorRequestChange(arg0);
    }
}

/* 0x8011388C | 0xA0 */
void floorLinkWithSE(void* a, void* b, s32 c) {
    extern u8 lbl_80408378[];
    extern u32 std092_getCurrentRoomID(void);
    extern void soundStartSE(u32);
    extern void floorRequestChange(void*);
    u32 sndId;

    if (a == NULL) {
        return;
    }
    *(u32*)(lbl_80408378 + 0x0) = std092_getCurrentRoomID();
    *(void**)(lbl_80408378 + 0x4) = a;
    *(u8*)(lbl_80408378 + 0x8) = 1;
    *(void**)(lbl_80408378 + 0xC) = b;
    switch (c) {
    case 0:
        sndId = 0;
        break;
    case 1:
    default:
        sndId = 0x3F9;
        break;
    }
    if (sndId != 0) {
        soundStartSE(sndId);
    }
    floorRequestChange(a);
}

/* 0x8011392C | 0x10 */
u32 floorGetNextPosIndex(void) {
    return *(u32*)(lbl_80408378 + 0xC);
}

/* 0x8011393C | 0x10 */
u32 floorGetNextFloorID(void) {
    return *(u32*)(lbl_80408378 + 0x4);
}

/* 0x8011394C | 0x10 */
u32 floorGetPrevFloorID(void) {
    return *(u32*)(lbl_80408378 + 0x0);
}

/* 0x8011395C | 0x10 */
void floorSetPrevFloorID(u32 value) {
    *(u32*)(lbl_80408378 + 0x0) = value;
}

/* 0x8011396C | 0x50 */
s32 floorGetPlaceName(s32 param) {
    extern s32 floorDataBiosGetPlaceName(FloorData*);
    FloorData* data;

    switch (param) {
    case 0xFD:
    case 0xFE:
    case 0xFF:
        return 0;
    }
    data = floorDataBiosGetPtr(param);
    if (data == NULL) {
        return 0;
    }
    return floorDataBiosGetPlaceName(data);
}

/* 0x801139BC | 0x50 */
void fn_801139BC(void) {
    extern void std051_characterSetAnimation(s32, s32, s32, s32, s32);
    extern void fn_80117154(void);

    std051_characterSetAnimation(0, 0x64, 1, 0, 1);
    std051_characterSetAnimation(0, 0x65, 1, 0, 1);
    fn_80117154();
}

/* 0x80113A0C | 0x178 */
void _floorUpdate__FUi14FloorEnterMode(u32 floorId, s32 enterMode) {
    typedef struct FloorEventLog {
        /* 0x00 */ s8 type;
        /* 0x01 */ u8 pad01[3];
        /* 0x04 */ u32 value;
    } FloorEventLog;
    extern u8* GSresGetResource(u32, u32);
    extern void fn_801171C8(void);
    extern void tvRender(void);
    extern void cameraUpdate(void);
    extern void fn_8010D8D4(s32);
    extern void fn_800D9ED8(s32);
    extern void GSmsgPrintf(s32, s32, u32, const char*, ...);
    extern s8 lbl_8047AD60;
    extern FloorEventLog lbl_8035B818[10];
    u8* debug;
    s32 y;
    s32 i;
    FloorEventLog* entry;

    fn_801171C8();
    tvRender();
    cameraUpdate();
    /* Debug overlay: resource (0, 2) holds the on/off flag. */
    debug = GSresGetResource(0, 2);
    if (debug != NULL && *debug != 0) {
        fn_8010D8D4(0);
        debug = GSresGetResource(0, 2);
        if (debug != NULL && *debug != 0) {
            fn_800D9ED8(1);
            y = 0x1E;
            for (i = 0; i < 10; i++) {
                entry = &lbl_8035B818[(lbl_8047AD60 + i) % 10];
                switch (entry->type) {
                case 1:
                    GSmsgPrintf(0x1E, y, 0x8080FFFF, "PASS EVENT = %d", entry->value);
                    break;
                case 2:
                    GSmsgPrintf(0x1E, y, 0x8080FFFF, "TOUCH EVENT = %d", entry->value);
                    break;
                case 3:
                    GSmsgPrintf(0x1E, y, 0x8080FFFF, "CHECK EVENT = %d", entry->value);
                    break;
                }
                y += 12;
            }
            fn_800D9ED8(0);
        }
    }
}

/* Door record (0x18 bytes) in the lbl_80478ECC table. */
typedef struct FloorDoorEntry {
    /* 0x00 */ u8 pad00[7];
    /* 0x07 */ u8 type;
    /* 0x08 */ u8 pad08[4];
    /* 0x0C */ u16 floorId;
    /* 0x0E */ u16 openFlag;
    /* 0x10 */ u8 pad10[8];
} FloorDoorEntry;

extern u8 std081_flagCheckTrue(u16);
extern void floorEventCtrlDoor(u32, u32, u32);
extern FloorDoorEntry* lbl_80478ECC;
extern u32* lbl_80478EC8;

/*
 * Set every door of a floor open/closed from its event flag. An inline in
 * the original source: retail starts the loop with `li r31,0; mr r30,r31`,
 * the door-table offset copied from the freshly zeroed index. Controlled
 * compiler tests on the same loop written in place (for, while, block
 * scope) give two separate `li` under GC/1.1, 1.2.5n, 1.3, 1.3.2, 2.0,
 * 2.5, 2.6, 2.7 and 3.0a3 at -O2, -O3, -O4,p, -O4,s, with -opt nopeephole,
 * -opt nocse, -opt noprop, -inline deferred, -inline auto,deferred and
 * -lang=c++ (0 of 99 configurations); the copy appears only when the loop
 * is the body of an inlined function (58 of 99, including this TU's
 * GC/1.3 -O4,p -opt nopeephole).
 */
static inline void floorInitDoors(FloorData* floor) {
    u32 i;
    FloorDoorEntry* door;

    for (i = 0; i < *lbl_80478EC8; i++) {
        door = &lbl_80478ECC[i];
        if (door->floorId != floor->floorId) {
            continue;
        }
        switch (door->type) {
        case 1:
        case 3:
            if (std081_flagCheckTrue(door->openFlag)) {
                floorEventCtrlDoor(floor->floorId, i, 1);
            } else {
                floorEventCtrlDoor(floor->floorId, i, 3);
            }
            break;
        case 2:
            floorEventCtrlDoor(floor->floorId, i, 3);
            break;
        }
    }
}

/* 0x80113B84 | 0x18C */
void _floorInitialize__FUi14FloorEnterMode(FloorData* floor, s32 enterMode) {
    extern void heroMoveInit(void*, void*);
    extern u8 fn_800FF548(void);
    extern void _floorInitCharacters__FP11GSfloor_dd_(FloorData*);
    extern void floorInitPeople(FloorData*);
    extern void fieldCameraLoadList(FloorData*);
    extern void GSscene_SetMode(s32);
    extern void cameraSetTarget(s32, s32);
    extern void GSscene_SetCameraViewVector(FloorViewVec*);
    extern void cameraUpdate(void);
    extern void GSinputResetLeftSmoothing(s32, s32, s32);
    extern void GSinputResetRightSmoothing(s32, s32, s32);
    heroMoveInit(lbl_80408378 + 0x10, lbl_80408378 + 0x1C);
    if (fn_800FF548() == 0) {
        _floorInitCharacters__FP11GSfloor_dd_(floor);
        floorInitDoors(floor);
        floorInitPeople(floor);
    }
    fieldCameraLoadList(floor);
    if (fn_800FF548() == 0) {
        FloorViewVec view = sFloorCameraView;

        GSscene_SetMode(0);
        cameraSetTarget(0, 0x64);
        GSscene_SetCameraViewVector(&view);
    }
    cameraUpdate();
    GSinputResetLeftSmoothing(1, 0, 0);
    GSinputResetRightSmoothing(1, 0, 0);
}

/* 0x80113D10 | 0x24 */
u32 _floorUnloadModel__FPvUlUl(u32 group) {
    extern void GSmodelFree();

    GSmodelFree(group);
    return 1;
}

/* 0x80113D34 | 0x24 */
void floorOpenModel(u32 unused, u32 modelIndex) {
    floorOpenObject(modelIndex);
}

/* 0x80113D58 | 0x1F0 */
void* floorOpenObject(u32 modelIndex) {
    extern const char lbl_8035B868[];
    extern void* GSresFindById(u32);
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern void* GSmodelLoad(void*);
    extern void GSmodelSetVisibility(void*, u32);
    extern void GSmodelSetAnimIndex(void*, u32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelStartAnimation(void*);
    extern void GSmodelSetTexAnimIndex(void*, u32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSmodelStartTexAnimation(void*);
    u8 special = 0;
    void* archive;
    void* pub;
    void* model;

    switch (modelIndex) {
    case 3:
    case 100:
        modelIndex = 0x00F71000;
        break;
    case 4:
    case 101:
        modelIndex = 0x00F31000;
        break;
    default:
        if (((modelIndex >> 9) & 0x3F) == 2) {
            special = 1;
        }
        break;
    }

    if (modelIndex == 0) {
        GSlogWrite("Error!! %s: objID is zero\n", lbl_8035B868);
        return NULL;
    }

    archive = GSresFindById(modelIndex);
    if (special != 0) {
        if (archive == NULL) {
            GSlogWrite("%s: got NULL pointer archive\n", lbl_8035B868);
            return NULL;
        }
        pub = HSD_ArchiveGetPublicAddress(archive, lbl_802720B0);
        if (pub == NULL) {
            GSlogWrite("%s: got NULL pointer scene descriptor\n", lbl_8035B868);
            return NULL;
        }
        archive = *(void**)pub;
        if (*(void**)archive == NULL) {
            GSlogWrite("%s: couldn't load model\n", lbl_8035B868);
            return NULL;
        }
        archive = *(void**)archive;
    }

    model = GSmodelLoad(archive);
    if (model == NULL) {
        GSlogWrite("Error! %s: couldn't load model %d\n", lbl_8035B868);
        GSlogWrite("モデルの初期化に失敗\n");
    } else {
        GSmodelSetVisibility(model, 1);
        if ((u8)GSmodelCanAnimate(model) != 0) {
            GSmodelSetAnimIndex(model, 0);
            GSmodelSetAnimRate(model, 0.5f);
            GSmodelStartAnimation(model);
        }
        if ((u8)GSmodelCanTexAnimate(model) != 0) {
            GSmodelSetTexAnimIndex(model, 0);
            GSmodelSetTexAnimRate(model, 0.5f);
            GSmodelStartTexAnimation(model);
        }
    }

    return model;
}

/* 0x80113F48 | 0x24 */
u32 std093_getCurrentGroupID(void) {
    return floorDataBiosGetGroupID(floorDataBiosGetCurrentPtr());
}

/* 0x80113F6C | 0x48 */
void* floorGetResource(u32 key, u32 arg) {
    extern void* GSresGetResource(u32, u32);
    FloorData* resource;

    resource = floorDataBiosGetPtr(key);
    if (resource == NULL) {
        return NULL;
    }
    return GSresGetResource(floorDataBiosGetGroupID(resource), arg);
}

/* 0x80113FB4 | 0x34 */
u32 fn_80113FB4(u32 key) {
    FloorData* resource;

    resource = floorDataBiosGetPtr(key);
    if (resource == NULL) {
        return 0;
    }
    return floorDataBiosGetGroupID(resource);
}

/* 0x80113FE8 | 0xE0 */
void floorReturnToSavedRoom(void) {
    extern u8 lbl_80408378[];
    extern u32 gamedatasaveGetStatus(s32, s32);
    extern u32 std092_getCurrentRoomID(void);
    extern void floorRequestChange(u32);
    u8* state = lbl_80408378;
    u32 a;
    u32 b;
    u8 c;

    if (*(u8*)(state + 0x51) != 0) {
        a = *(u32*)(state + 0x48);
        b = *(u32*)(state + 0x4C);
        c = *(u8*)(state + 0x50);
    } else {
        a = gamedatasaveGetStatus(0, 5);
        b = gamedatasaveGetStatus(0, 7);
        c = (u8)gamedatasaveGetStatus(0, 8);
    }
    if (a != 0) {
        *(u32*)(lbl_80408378 + 0x0) = std092_getCurrentRoomID();
        *(u32*)(lbl_80408378 + 0x4) = a;
        *(u8*)(lbl_80408378 + 0x8) = 1;
        *(u32*)(lbl_80408378 + 0xC) = c;
        floorRequestChange(a);
    }
    *(u8*)(state + 0x51) = 0;
    *(u32*)(lbl_80408378 + 0x0) = b;
}

/* 0x801140C8 | 0x14 */
void std162_fieldClearPending(void) {
    *(u8*)(lbl_80408378 + 0x51) = 0;
}
