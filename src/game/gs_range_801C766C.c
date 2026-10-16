/**
 * @file gs_range_801C766C.c
 * @brief gs-engine, 0x801C766C - 0x801CA7EC.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"
#include "game/gs_flag.h"
#include "game/world/gs_field.h"
/* peephole off is the unit-level -opt nopeephole flag (configure.py) */

typedef struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

typedef struct DistanceSortEntry {
    s32 index;
    s32 blocked;
    Vec3 pos;
    f32 distance;
} DistanceSortEntry;

typedef struct CursorPos {
    u8 row;
    s8 col;
} CursorPos;


extern u8 heroMoveIsMember(s32 member);
extern void heroSetStatus(s32 hero, s32 status, u8 value);
extern void* savedataGetStatus(s32 save, s32 status);
extern void* gamedatasaveBiosGetPtr(void);
extern void fn_8011418C(s32* floorId, s32* prevFloorId, u8* floorPosIndex);
extern void gamedatasaveBiosSetFloorid(void* save, s32 floorId);
extern void gamedatasaveBiosSetPrevfloorid(void* save, s32 floorId);
extern void gamedatasaveBiosSetFloorposindex(void* save, u8 floorPosIndex);
extern s32 fn_801D0748(s32, s32, s32);

extern u32 std082_flagGetValue(u32 flag);
extern void fn_801D0AFC(s32);
extern void* heroBiosGetPokemonPtr(void* status, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 fn_80121ADC(void* pokemon, s32 ribbon);
extern void fn_80121B4C(void* pokemon, s32 ribbon);
extern s32 menuGetCursorFromItemID(s32 menu, s32 itemId);
extern void cursorBiosSetPos(s32 cursor, const CursorPos* pos);
extern void fn_8000D710(s32);
extern s32 fn_80075638(void);
extern void fn_8007565C(void);
extern void std054_characterTalk(s32, s32, s32);
extern s8 std011_dialogDisplayYesNoMenu(void);
extern void GBAInit(void);
extern void gbaCommandSetKeyState(s32, s32);
extern void fn_8007B090(s32);
extern s32 fn_8007AB10(s32, s32*);
extern s32 fn_8007AAA8(void);
extern void std055_characterEndSpeech(s32, s32);
extern void menuSubOpenSelect(s32, s32, s32, s32, s32, s32);
extern u8 heroMoveGetResID(s32* floorId, s32* resId, s32 member);
extern u32 heroMoveChkHinderClear(s32 member);
extern void* fn_800F92D4(s32 lightId);
extern void GSlogWrite(const char* fmt, ...);
extern u8 GSlightHasAnimationEnded(void* light);
extern void GSlightStopAnimation(void* light);
extern void GSlightSetAnimIndex(void* light, s32 index);
extern void GSlightSetAnimRate(void* light, f32 rate);
extern void GSlightSetAnimFrame(void* light, f32 frame);
extern void GSlightSetAnimType(void* light, s32 type);
extern void GSlightSetActive(void* light, u8 active);
extern void _threadSwitch(void);
extern void memoDataSetMemoFlag(s32 flag);
extern void fn_8012F1FC(s32 member);
extern void heroMoveDismissMember(s32 member);
extern u32 _fadeEffectGetRandom__FUl(u32 limit);
extern s32 fn_800D37CC(void);
extern u32 GSgetFrameStep(void);
extern void fn_8025D164(void);
extern s32 fn_8006ADEC(void);
extern f32 fn_8025D0A8(void* status);
extern void fn_8006ADB4(s32 value);

extern const char lbl_802758AC[];
extern const f32 lbl_8047E100;
extern const f32 lbl_8047E108;
extern const f32 lbl_8047E114;
extern const f64 lbl_8047E120;
extern const f64 lbl_8047E128;

/* Forward declarations: functions are laid out in retail address order. */
u8 std235_menuShowSaveMenu(void);
s32 _fnDistanceSortFunc__FPCvPCv(const void* lhs, const void* rhs);
void std216_partyStatusUpdate_8ae(s32 mode);
void std208_promptDNASampleSelection(void);
u8 std204_promptDNAKeyCode(u16 arg0, u16 arg1, u16 arg2);
void std207_gbaUnk_80075638(void);
s32 std205_getDNASampleSepcies(u16 arg0);
s32 std200_gbaConnectEvent(s32 arg0, s32 arg1);
void std199_menuDisplayCustomMenu(s32 a, s32 b, s32 c, s32 d, s32 e);
u8 fn_801C9910(void);
void fn_801C9B6C(s32 arg0, s32 arg1, u16 arg2, u8 arg3);
void fn_801C9C9C(void);
u8 std193_lightWaitForAnimation(s32 lightId, s32 wait);
void scriptLightStopMotion(s32 lightId);
void std191_mapTransformElement(s32 lightId, s32 animIndex, s32 frame, s32 loop);
void std190_lightSetActive(s32 lightId, s32 active);
void scriptSetMemoFlag(void);
void std035_setPartnerFollowing(s32 active);
u8 std182_tvNewsEvent(void);
u8 std211_tvSetChannelByStory(void);
u32 fadeEffectGetRandom(u32 limit);
void std157_sleep(f32 frames);
u16 fieldStartEncount(u32 arg0, u8 arg1, s32 arg2);
void std239_playerGetCouponsEarnedInLastArea(void);
s32 fn_801CA728(s32 count);
void fn_801CA7AC(s32 value);
s32 std236_playerGetCouponsEarnedOnCurrentRun(void);
s32 std232_characterApproachPlayer(s32 side, s32 slot);
s32 fn_801C8E14(s32 floorDataId, u32 actorIndex, u16 mode, u8 direction);

extern u8 peopleMoveCheck();
extern void GSmodelSetAnimIndex();
extern void GSmodelSetAnimFrame();
extern void GSmodelSetAnimRate();
extern void GSmodelSetAnimType();
extern void GSmodelStartAnimation();
extern u8 GSmodelHasAnimationEnded();

static inline void fieldModelStartAnim(void* model, s16 index, f32 frame)
{
    if (model != 0 && index >= 0) {
        GSmodelSetAnimIndex(model, index);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, lbl_8047E108);
        GSmodelSetAnimType(model, 0);
        GSmodelStartAnimation(model);
    }
}

static inline void fieldModelWaitAnim(void* model)
{
    if (model == 0) {
        return;
    }
    while (GSmodelHasAnimationEnded(model) == 0) {
        _threadSwitch();
    }
}

static inline void fieldWaitSeconds(f32 seconds)
{
    f32 timer = lbl_8047E114;

    while (timer < seconds) {
        _threadSwitch();
        timer += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}

static inline u8 fieldWaitPartnerMove(void)
{
    extern const f32 lbl_8047E11C;
    f32 timer = lbl_8047E11C;

    while (timer > lbl_8047E114) {
        if (peopleMoveCheck(0, 0x65, 0) == 0) {
            return 0;
        }
        _threadSwitch();
        timer -= (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
    return 1;
}

extern const f32 lbl_8047E104;
extern const f32 lbl_8047E10C;
extern const f32 lbl_8047E110;
extern const f32 lbl_8047E118;
extern f32 GSvecDistance();
extern void GSvecAdd();
extern void vecScale();
extern void GSvecSub(Vec3*, const Vec3*, const Vec3*);
extern void fn_800E0060(Vec3*, const Vec3*);
extern void fn_800DFFCC(Vec3*, const Vec3*, const Vec3*);
extern s32 GScolsys2UtilGetCpPlaneLine(
    Vec3*, f32*, const Vec3*, const Vec3*, const Vec3*, const Vec3*);
extern u8 fn_8018D680();
extern s32 colsysWallSweep();
extern void qsort();
extern f64 cos(f64);
extern f64 sin(f64);

static inline u8 fieldSegmentBlocked(const Vec3* from, const Vec3* to,
                                     const Vec3* centerPtr, f32 radius)
{
    Vec3 midpoint;
    Vec3 center = *centerPtr;

    if (GSvecDistance(from, to) < lbl_8047E104) {
        return fn_8018D680(from, to, &center, radius);
    }
    GSvecAdd(&midpoint, from, to);
    vecScale(&midpoint, &midpoint, lbl_8047E108);
    if (fn_8018D680(from, &midpoint, &center, radius)) {
        return 1;
    }
    return fn_8018D680(&midpoint, to, &center, radius);
}

static inline u8 fieldPlaneCrossed(const Vec3* fromPtr, const Vec3* toPtr,
                                   const Vec3* lineStart, const Vec3* lineEnd)
{
    Vec3 from = *fromPtr;
    Vec3 to = *toPtr;
    Vec3 flatTo = to;
    Vec3 fullDirection;
    Vec3 flatDirection;
    Vec3 planeHit;
    Vec3 planeNormal;
    f32 planeT;

    flatTo.y = lbl_8047E100;
    GSvecSub(&fullDirection, &from, &to);
    fn_800E0060(&fullDirection, &fullDirection);
    GSvecSub(&flatDirection, &from, &flatTo);
    fn_800E0060(&flatDirection, &flatDirection);
    fn_800DFFCC(&planeNormal, &fullDirection, &flatDirection);
    if (GScolsys2UtilGetCpPlaneLine(&planeHit, &planeT, &planeNormal, &from,
                                    lineStart, lineEnd) == 0) {
        return 0;
    }
    if (planeT < lbl_8047E114 || planeT > lbl_8047E100) {
        return 0;
    }
    return 1;
}

static inline void fieldBuildRing(DistanceSortEntry* entries,
                                  const Vec3* center, const Vec3* origin)
{
    Vec3 offset;
    f32 angle;
    s32 i;

    for (i = 0; i < 8; i++) {
        angle = (f32)i * lbl_8047E10C;
        entries[i].index = i;
        offset.x = lbl_8047E110 * (f32)cos(angle);
        offset.y = lbl_8047E114;
        offset.z = lbl_8047E110 * (f32)sin(angle);
        GSvecAdd(&entries[i].pos, &offset, center);
        if (colsysWallSweep(center, &entries[i].pos, 0, lbl_8047E118) != 0) {
            entries[i].blocked = 1;
        } else {
            entries[i].blocked = 0;
        }
        entries[i].distance = GSvecDistance(&entries[i].pos, origin);
    }
    qsort(entries, 8, sizeof(DistanceSortEntry), _fnDistanceSortFunc__FPCvPCv);
}

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C766C_ONLY)
u8 std235_menuShowSaveMenu(void)
{
    void* save;
    s32 floorId;
    s32 prevFloorId;
    u8 floorPosIndex;
    s32 isMember;

    if (heroMoveIsMember(1) != 0) {
        isMember = 1;
    } else {
        isMember = 0;
    }

    heroSetStatus(0, 0x18, isMember);
    savedataGetStatus(0, 1);
    save = gamedatasaveBiosGetPtr();
    fn_8011418C(&floorId, &prevFloorId, &floorPosIndex);
    gamedatasaveBiosSetFloorid(save, floorId);
    gamedatasaveBiosSetPrevfloorid(save, prevFloorId);
    gamedatasaveBiosSetFloorposindex(save, floorPosIndex);

    if (fn_801D0748(4, 2, 0) == 4) {
        return 1;
    }

    return 0;
}

#endif /* GS_801C766C_ONLY */

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C7730_ONLY)
/* Eight-direction collision search used to place a battle actor. */
s32 std232_characterApproachPlayer(s32 side, s32 slot)
{
    extern const Vec3 lbl_802758A0;
    extern const f32 lbl_8047E100;
    extern void std172_characterGetPosition(s32 arg0, s32 arg1, Vec3* out);
    extern u32 peopleIsInGroup();
    extern void* peopleSearchID();
    extern void* peopleInfoBiosGetPtr();
    extern f32 fn_8018F5E4();
    extern void std076_characterEventFlagsClear();
    extern void fn_8018AACC();
    extern void fn_8018BA04();
    extern void peopleTurnToPoint();
    extern void std077_characterEventFlagsSet();

    DistanceSortEntry entries[8];
    Vec3 saved;
    Vec3 origin = lbl_802758A0;
    Vec3 partner, requested;
    s32 floorId2, resId2, floorId, resId;
    DistanceSortEntry* entry;
    void* person;
    void* info;
    f32 radius;
    s32 i, k, wanted;
    u8 done;

    if (heroMoveGetResID(&floorId, &resId, 1) == 0) {
        return 0;
    }
    std172_characterGetPosition(floorId, resId, &origin);
    if (heroMoveGetResID(&floorId2, &resId2, 0) == 0) {
        return 0;
    }
    std172_characterGetPosition(floorId2, resId2, &partner);
    std172_characterGetPosition(side, slot, &requested);

    person = peopleSearchID(peopleIsInGroup(side, slot));
    if (person == NULL) {
        return 0;
    }
    info = peopleInfoBiosGetPtr(*(u32*)((u8*)person + 0x30));
    if (info == NULL) {
        return 0;
    }
    radius = fn_8018F5E4(info);
    radius += lbl_8047E100;

    if (fieldSegmentBlocked(&partner, &requested, &origin,
                            1.0f + radius) == 0) {
        return 0;
    }

    fieldBuildRing(entries, &partner, &origin);
    for (i = 0; i < 8; i++) {
        if (entries[i].blocked == 1) {
            continue;
        }
        if (fieldSegmentBlocked(&partner, &requested, &entries[i].pos,
                                lbl_8047E118)) {
            continue;
        }
        if (fieldSegmentBlocked(&origin, &entries[i].pos, &partner,
                                lbl_8047E118)) {
            continue;
        }
        if (fieldPlaneCrossed(&partner, &requested, &origin,
                              &entries[i].pos) == 0) {
            continue;
        }

        std076_characterEventFlagsClear(floorId, resId);
        fn_8018AACC(floorId, resId, 1, &entries[i].pos);
        done = fieldWaitPartnerMove();
        if (done == 0 && (entries[i].index & 1) != 0) {
            wanted = -1;
            if (entries[i].index == 3 || entries[i].index == 5) {
                wanted = 4;
            }
            if (entries[i].index == 1 || entries[i].index == 7) {
                wanted = 0;
            }
            entry = entries;
            for (k = 0; k < 8; k++) {
                if (entry->index == wanted) {
                    break;
                }
                entry++;
            }
            if (entry->blocked == 0 &&
                fieldSegmentBlocked(&partner, &requested, &entry->pos,
                                    lbl_8047E118) == 0) {
                fn_8018AACC(floorId, resId, 1, &entry->pos);
                done = fieldWaitPartnerMove();
            }
        }
        if (done == 0) {
            fn_8018BA04(floorId, resId, &saved);
            peopleTurnToPoint(floorId, resId, saved.x, saved.y, saved.z,
                        lbl_8047E100);
            fieldWaitPartnerMove();
        }
        std077_characterEventFlagsSet(floorId, resId);
        return 0;
    }

    fieldBuildRing(entries, &origin, &origin);
    for (i = 0; i < 8; i++) {
        if (entries[i].blocked == 1) {
            continue;
        }
        if (fieldSegmentBlocked(&origin, &entries[i].pos, &partner,
                                lbl_8047E118)) {
            continue;
        }
        if (fieldPlaneCrossed(&partner, &requested, &origin,
                              &entries[i].pos) == 0) {
            continue;
        }
        if (fieldSegmentBlocked(&partner, &requested, &entries[i].pos,
                                lbl_8047E118)) {
            continue;
        }

        std076_characterEventFlagsClear(floorId, resId);
        fn_8018AACC(floorId, resId, 1, &entries[i].pos);
        if (fieldWaitPartnerMove() == 0) {
            fn_8018BA04(floorId, resId, &saved);
            peopleTurnToPoint(floorId, resId, saved.x, saved.y, saved.z,
                        lbl_8047E100);
            fieldWaitPartnerMove();
        }
        std077_characterEventFlagsSet(floorId, resId);
        return 0;
    }

    return 0;
}

#endif /* GS_801C7730_ONLY */

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C84FC_ONLY)
s32 _fnDistanceSortFunc__FPCvPCv(const void* lhs, const void* rhs)
{
    const DistanceSortEntry* left = (const DistanceSortEntry*)lhs;
    const DistanceSortEntry* right = (const DistanceSortEntry*)rhs;

    if (left->distance == right->distance) {
        return 0;
    }

    if (left->distance > right->distance) {
        return 1;
    }

    return -1;
}

void std216_partyStatusUpdate_8ae(s32 mode)
{
    void* status;
    void* pokemon;
    u16 i;

    if (std082_flagGetValue(0x8AE) == 0) {
        switch (mode) {
        case 0:
            fn_801D0AFC(0);
            break;
        case 1:
            status = savedataGetStatus(0, 2);
            i = 0;
            while (i < 6) {
                pokemon = heroBiosGetPokemonPtr(status, i);
                if (pokemonCheckValid(pokemon) != 0) {
                    if (fn_80121ADC(pokemon, 3) != 0) {
                        fn_80121B4C(pokemon, 3);
                    }
                    if (fn_80121ADC(pokemon, 4) != 0) {
                        fn_80121B4C(pokemon, 4);
                    }
                }
                i++;
            }
            break;
        }
    }
}

void std208_promptDNASampleSelection(void)
{
    CursorPos initial;
    CursorPos pos;
    s32 cursor;

    cursor = menuGetCursorFromItemID(0x59, 0x20C);
    if (cursor < 0) {
        cursor = 0;
    }

    initial.row = 0;
    initial.col = cursor;
    pos = initial;
    cursorBiosSetPos(2, &pos);
    fn_8000D710(1);
}

extern u32 fn_800884BC(u32 arg0, u32 arg1, u32 arg2);

u8 std204_promptDNAKeyCode(u16 arg0, u16 arg1, u16 arg2)
{
    u32 v0;
    u32 v1;
    u32 v2;

    v0 = 0;
    switch (arg0) {
    case 0x200: case 0x201: v0 = 0x99; break;
    case 0x202: case 0x203: v0 = 0x9c; break;
    case 0x204: case 0x205: v0 = 0x9f; break;
    case 0x206: case 0x207: v0 = 0xb9; break;
    case 0x208: case 0x209: v0 = 0xc8; break;
    case 0x20a: case 0x20b: v0 = 0x11f; break;
    case 0x20c: case 0x20d: v0 = 0xf3; break;
    case 0x20e: case 0x20f: v0 = 0xf4; break;
    case 0x210: case 0x211: v0 = 0xf5; break;
    }

    v1 = 0;
    switch (arg1) {
    case 0x200: case 0x201: v1 = 0x99; break;
    case 0x202: case 0x203: v1 = 0x9c; break;
    case 0x204: case 0x205: v1 = 0x9f; break;
    case 0x206: case 0x207: v1 = 0xb9; break;
    case 0x208: case 0x209: v1 = 0xc8; break;
    case 0x20a: case 0x20b: v1 = 0x11f; break;
    case 0x20c: case 0x20d: v1 = 0xf3; break;
    case 0x20e: case 0x20f: v1 = 0xf4; break;
    case 0x210: case 0x211: v1 = 0xf5; break;
    }

    v2 = 0;
    switch (arg2) {
    case 0x200: case 0x201: v2 = 0x99; break;
    case 0x202: case 0x203: v2 = 0x9c; break;
    case 0x204: case 0x205: v2 = 0x9f; break;
    case 0x206: case 0x207: v2 = 0xb9; break;
    case 0x208: case 0x209: v2 = 0xc8; break;
    case 0x20a: case 0x20b: v2 = 0x11f; break;
    case 0x20c: case 0x20d: v2 = 0xf3; break;
    case 0x20e: case 0x20f: v2 = 0xf4; break;
    case 0x210: case 0x211: v2 = 0xf5; break;
    }

    return fn_800884BC(v0, v1, v2) == 0;
}

void std207_gbaUnk_80075638(void)
{
    u32 ready = (u8)fn_80075638();

    if (ready != 0) {
        fn_8007565C();
    }
}

#endif /* GS_801C84FC_ONLY */

extern u32 fn_800756C8(s32 pokemonId);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C8834_ONLY)
/* Pokemon id unlocked by one of the 0x200-0x211 items (0 for any other item). */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_801C8834) — see docs/RULE_EXCEPTIONS.md */
static inline s32 itemToPokemonId(u16 item)
{
    s32 id = 0;
    switch (item) {
    case 0x200: case 0x201: id = 0x99; break;
    case 0x202: case 0x203: id = 0x9c; break;
    case 0x204: case 0x205: id = 0x9f; break;
    case 0x206: case 0x207: id = 0xb9; break;
    case 0x208: case 0x209: id = 0xc8; break;
    case 0x20a: case 0x20b: id = 0x11f; break;
    case 0x20c: case 0x20d: id = 0xf3; break;
    case 0x20e: case 0x20f: id = 0xf4; break;
    case 0x210: case 0x211: id = 0xf5; break;
    }
    return id;
}

/* Add (count > 0) or remove (count < 0) |count| of itemId: the shape of scriptAddItem (0x801CA7EC range). */
/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_801C8834) — see docs/RULE_EXCEPTIONS.md */
static inline void gsAddItem(u32 itemId, s32 count)
{
    if (count > 0) {
        heroItemAddItemDataId((u8*)0, (u16)itemId, count & 0xFFFF, -1);
    } else if (count < 0) {
        count = -count;
        heroItemDecItemDataId((u8*)0, (u16)itemId, count & 0xFFFF, -1);
    }
}

s32 std205_getDNASampleSepcies(u16 arg0)
{
    s32 pokemonId;
    u16 usedItem;

    pokemonId = itemToPokemonId(arg0);

    if ((u16)pokemonId == 0) {
        return 0;
    }
    fn_800756C8(pokemonId);

    switch (arg0) {
    case 0x200: usedItem = 0x201; break;
    case 0x202: usedItem = 0x203; break;
    case 0x204: usedItem = 0x205; break;
    case 0x206: usedItem = 0x207; break;
    case 0x208: usedItem = 0x209; break;
    case 0x20a: usedItem = 0x20b; break;
    case 0x20c: usedItem = 0x20d; break;
    case 0x20e: usedItem = 0x20f; break;
    case 0x210: usedItem = 0x211; break;
    default: usedItem = 0; break;
    }

    if (usedItem != 0) {
        gsAddItem(arg0, -1);
        gsAddItem(usedItem, 1);
    }

    return pokemonId;
}

#endif /* GS_801C8834_ONLY */

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C89F8_ONLY)
s32 std200_gbaConnectEvent(s32 arg0, s32 arg1)
{
    s32 linkState;
    s32 linkWork;
    s32 result;
    u8 repeat;
    s32 answer;
    f32 elapsed;

    std054_characterTalk(arg0, arg1, 0x3B61);
    if (std011_dialogDisplayYesNoMenu() == 0) {
        GBAInit();
        gbaCommandSetKeyState(2, 0);
        fn_8007B090(1);
        std054_characterTalk(arg0, arg1, 0x3B62);

        linkWork = 0;
        linkState = 1;
        repeat = 0;
        for (;;) {
            result = fn_8007AB10(linkState, &linkWork);
            if (repeat != 0 && result == 0) {
                continue;
            }

            repeat = 0;
            if (result != 0) {
                linkState = result;
            }

            switch (linkState) {
            case 2:
                std054_characterTalk(arg0, arg1, 0x3B63);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 29;
                } else {
                    linkState = 4;
                }
                break;
            case 4:
                std054_characterTalk(arg0, arg1, 0x3B64);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 9;
                } else {
                    linkState = 5;
                }
                break;
            case 5:
                std054_characterTalk(arg0, arg1, 0x3B66);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 29;
                } else {
                    linkState = 6;
                }
                break;
            case 7:
                std054_characterTalk(arg0, arg1, 0x3B82);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 29;
                } else {
                    linkState = 8;
                }
                fn_8007AB10(7, &linkWork);
                break;
            case 9:
                std054_characterTalk(arg0, arg1, 0x3B6C);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 29;
                } else {
                    linkState = 10;
                }
                fn_8007AB10(9, &linkWork);
                break;
            case 11:
                std054_characterTalk(arg0, arg1, 0x3B82);
                answer = (s8)std011_dialogDisplayYesNoMenu();
                fn_8007AB10(9, &linkWork);
                if (answer != 0) {
                    linkState = 29;
                } else {
                    linkState = 12;
                }
                fn_8007AB10(11, &linkWork);
                break;
            case 14:
                std054_characterTalk(arg0, arg1, 0x3B6F);
                repeat = 1;
                break;
            case 15:
                std054_characterTalk(arg0, arg1, 0x3B80);
                repeat = 1;
                break;
            case 16:
                if (linkWork != 0) {
                    fn_8007AAA8();
                }
                std054_characterTalk(arg0, arg1, 0x3B81);
                std054_characterTalk(arg0, arg1, 0x3B6A);
                std055_characterEndSpeech(arg0, arg1);
                return 1;
            case 17:
                std054_characterTalk(arg0, arg1, 0x3B68);
                elapsed = lbl_8047E114;
                while (elapsed < lbl_8047E100) {
                    _threadSwitch();
                    elapsed += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
                }
                linkState = 16;
                break;
            case 19:
                std054_characterTalk(arg0, arg1, 0x3B82);
                if (std011_dialogDisplayYesNoMenu() != 0) {
                    linkState = 29;
                } else {
                    linkState = 1;
                }
                fn_8007AB10(19, &linkWork);
                break;
            }

            if (linkState == 29) {
                break;
            }
        }

        if (linkWork != 0) {
            fn_8007AAA8();
        }
    }

    std054_characterTalk(arg0, arg1, 0x3B83);
    std055_characterEndSpeech(arg0, arg1);
    return 0;
}

#endif /* GS_801C89F8_ONLY */

#if !defined(GS_801C766C_SPLIT) || defined(GS_801C8DD0_ONLY)
void std199_menuDisplayCustomMenu(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    menuSubOpenSelect(1, a, b, c, d, e);
}

extern s32 fn_801C8E14(s32 floorDataId, u32 actorIndex, u16 mode,
                       u8 direction);
extern u8 fn_801C9910(void);
extern void floorEventCtrlElevator(s32 arg0, s32 arg1, u16 arg2,
                                   u8 arg3, s32 speed);
extern u8 lbl_8047B3C4;

/* Drive one scripted field actor animation and its paired movement actors. */
s32 fn_801C8E14(s32 floorDataId, u32 actorIndex, u16 mode, u8 direction)
{
    typedef struct FieldAnimActor {
        s8 enterAnim;
        s8 exitAnim;
        u8 pad02[3];
        s8 actionAnimA;
        s8 actionAnimB;
        u8 pad07;
        u8 partIndex;
        u8 pad09[11];
        void* resource;
    } FieldAnimActor;

    extern u32* lbl_80478EC8;
    extern FieldAnimActor* lbl_80478ECC;
    extern s32 lbl_8047B3C0;
    extern u8 lbl_8047B3C4;
    extern Vec3 lbl_80467090[];
    extern const f32 lbl_8047E100;
    extern const f32 lbl_8047E130;
    extern const f32 lbl_8047E134;
    extern const f32 lbl_8047E138;
    extern const f32 lbl_8047E13C;
    extern const f32 lbl_8047E140;
    extern const f32 lbl_8047E144;
    extern const f32 lbl_8047E148;
    extern void* floorGetResource();
    extern void* floorDataBiosGetPtr();
    extern s32 floorDataBiosGetGroupID();
    extern void std070_characterAttachToCharacter();
    extern void std059_characterSetPosition();
    extern void std067_characterStopLookAt();
    extern void std168_characterDetachFromCharacter();
    extern void std065_characterLookAtPlayer();
    extern void peopleStartTurn();
    extern void fn_8018AACC();
    extern void peopleSetPositionByIndex();
    extern void soundStartSE();
    extern void soundStopSE();
    extern u8 GSmodelCanAnimate();
    extern void* GSmodelGetPart();
    extern void GSpartGetTransform();
    extern void GSpartFree();
    extern void GSmodelGetFrameCount();

    FieldAnimActor* actor;
    void* model;
    void* part;
    Vec3 transform;
    Vec3 secondPos;
    Vec3 firstPos;
    f32 frame;
    s32 groupId;
    s16 animIndex;
    u8 timedOut;
    s32 i;

    frame = lbl_8047E114;
    animIndex = -1;
    timedOut = 0;

    if (actorIndex >= lbl_80478EC8[0]) {
        return -1;
    }
    actor = &lbl_80478ECC[actorIndex];
    if (actor->resource == 0) {
        return -1;
    }
    model = floorGetResource(floorDataId, actor->resource);
    if (model == 0) {
        return -1;
    }
    groupId = floorDataBiosGetGroupID(floorDataBiosGetPtr(floorDataId));

    if (direction >= 2 && direction <= 5) {
        std070_characterAttachToCharacter(0, 0x64, groupId, actor->resource, actor->partIndex);
        std070_characterAttachToCharacter(0, 0x65, groupId, actor->resource, actor->partIndex);
        switch (direction) {
        case 2:
            std059_characterSetPosition(0, 0x64, lbl_8047E114, lbl_8047E114,
                        lbl_8047E130);
            std059_characterSetPosition(0, 0x65, lbl_8047E114, lbl_8047E114,
                        lbl_8047E134);
            break;
        case 3:
            std059_characterSetPosition(0, 0x64, lbl_8047E114, lbl_8047E114,
                        lbl_8047E134);
            std059_characterSetPosition(0, 0x65, lbl_8047E114, lbl_8047E114,
                        lbl_8047E130);
            break;
        case 4:
            std059_characterSetPosition(0, 0x64, lbl_8047E130, lbl_8047E114,
                        lbl_8047E114);
            std059_characterSetPosition(0, 0x65, lbl_8047E134, lbl_8047E114,
                        lbl_8047E114);
            break;
        case 5:
            std059_characterSetPosition(0, 0x64, lbl_8047E134, lbl_8047E114,
                        lbl_8047E114);
            std059_characterSetPosition(0, 0x65, lbl_8047E130, lbl_8047E114,
                        lbl_8047E114);
            break;
        }
        std067_characterStopLookAt(0, 0x65);
        fieldModelStartAnim(model, mode, lbl_8047E114);
        return 0;
    }

    switch (mode) {
    case 1:
    case 2:
    case 0x81:
    case 0x82:
        fieldModelStartAnim(model, actor->enterAnim, frame);
        if ((mode & 0x80) != 0) {
            soundStartSE(0x44);
        }
        fieldModelWaitAnim(model);

        part = GSmodelGetPart(model, actor->partIndex);
        GSpartGetTransform(part, &transform, 0, 0);
        GSpartFree(part);

        if ((mode & 1) != 0) {
            std067_characterStopLookAt(0, 0x65);
            transform.z -= lbl_8047E130;
            fn_8018AACC(0, 0x64, 1, &transform);
            firstPos.x = transform.x;
            firstPos.y = transform.y;
            firstPos.z = lbl_8047E138 + transform.z;
            fieldWaitSeconds(lbl_8047E13C);

            if (lbl_8047B3C0 != 0) {
                for (i = 0; i < lbl_8047B3C0; i++) {
                    fn_8018AACC(0, 0x65, 1, &lbl_80467090[i]);
                    timedOut = fieldWaitPartnerMove();
                    if (timedOut != 0) {
                        break;
                    }
                }
            }
            if (timedOut == 0) {
                fn_8018AACC(0, 0x65, 1, &firstPos);
            }
            peopleMoveCheck(0, 0x64, 1);
            if (timedOut == 0) {
                timedOut = fieldWaitPartnerMove();
            }
            peopleStartTurn(0, 0x64, lbl_8047E114, lbl_8047E108);
            if (timedOut == 0) {
                peopleStartTurn(0, 0x65, lbl_8047E114, lbl_8047E108);
            }
            peopleMoveCheck(0, 0x64, 1);
            if (timedOut == 0) {
                fieldWaitPartnerMove();
            }
        } else {
            std168_characterDetachFromCharacter(0, 0x64);
            std168_characterDetachFromCharacter(0, 0x65);
            std065_characterLookAtPlayer(0, 0x65);

            part = GSmodelGetPart(model, actor->partIndex);
            GSpartGetTransform(part, &transform, 0, 0);
            GSpartFree(part);
            secondPos.x = transform.x;
            secondPos.y = transform.y;
            secondPos.z = lbl_8047E130 + transform.z;
            peopleSetPositionByIndex(0, 0x64, &secondPos);
            firstPos.x = transform.x;
            firstPos.y = transform.y;
            firstPos.z = transform.z - lbl_8047E130;
            peopleSetPositionByIndex(0, 0x65, &firstPos);
            secondPos.z += lbl_8047E140;
            firstPos.z += lbl_8047E140;
            fn_8018AACC(0, 0x64, 1, &secondPos);
            fieldWaitSeconds(lbl_8047E13C);
            fn_8018AACC(0, 0x65, 1, &firstPos);
            peopleMoveCheck(0, 0x64, 1);
            peopleMoveCheck(0, 0x65, 1);
            lbl_8047B3C4 = 0;
        }

        fieldModelStartAnim(model, actor->exitAnim, frame);
        if ((mode & 0x80) != 0) {
            soundStartSE(0x44);
        }
        fieldModelWaitAnim(model);
        break;
    case 0xC0:
        fieldModelWaitAnim(model);
        if ((mode & 0x80) != 0) {
            soundStopSE(0x45);
            soundStartSE(0x46);
        }
        break;
    case 0x100:
        break;
    default:
        if ((mode & 4) != 0) {
            animIndex = actor->actionAnimA;
        } else if ((mode & 8) != 0) {
            animIndex = actor->actionAnimB;
        }
        if (animIndex < 0) {
            return -1;
        }
        if (GSmodelCanAnimate(model) == 0) {
            return -1;
        }

        if ((mode & 0x20) != 0) {
            std070_characterAttachToCharacter(0, 0x64, groupId, actor->resource,
                        actor->partIndex);
            std070_characterAttachToCharacter(0, 0x65, groupId, actor->resource,
                        actor->partIndex);
            if (direction != 0) {
                std059_characterSetPosition(0, 0x64, lbl_8047E114, lbl_8047E114,
                            lbl_8047E130);
                std059_characterSetPosition(0, 0x65, lbl_8047E114, lbl_8047E114,
                            lbl_8047E134);
            } else {
                std059_characterSetPosition(0, 0x64, lbl_8047E114, lbl_8047E114,
                            lbl_8047E134);
                std059_characterSetPosition(0, 0x65, lbl_8047E114, lbl_8047E114,
                            lbl_8047E130);
            }
            std067_characterStopLookAt(0, 0x65);
        }
        if ((mode & 0x10) != 0) {
            GSmodelSetAnimIndex(model, animIndex);
            GSmodelGetFrameCount(model, &frame, 0);
            frame -= lbl_8047E100;
        }
        fieldModelStartAnim(model, animIndex, frame);
        if ((mode & 0x40) != 0) {
            fn_801C8E14(floorDataId, actorIndex, 0xC0, direction);
        } else if ((mode & 0x80) != 0) {
            soundStartSE(0x45);
        }
        break;
    }

    GSmodelGetFrameCount(model, &frame, 0);
    return (s32)(lbl_8047E144 *
                 ((lbl_8047E148 * frame) / (f32)fn_800D37CC()));
}


u8 fn_801C9910(void)
{
    extern u8 fn_8012B19C(s32 member, Vec3* start, Vec3* target,
                          f32 extraRadius);
    extern void std172_characterGetPosition(s32 arg0, s32 arg1, Vec3* out);
    extern f32 PSVECDistance(const Vec3* a, const Vec3* b);
    extern void PSVECSubtract(const Vec3* a, const Vec3* b, Vec3* out);
    extern void PSVECScale(f32 scale, const Vec3* src, Vec3* dst);
    extern void PSVECAdd(const Vec3* a, const Vec3* b, Vec3* out);
    extern void PSVECNormalize(const Vec3* src, Vec3* dst);
    extern s32 lbl_8047B3C0;
    extern Vec3 lbl_80467090[];
    extern const f32 lbl_8047E100;
    extern const f32 lbl_8047E108;
    extern const f32 lbl_8047E114;
    extern const f32 lbl_8047E13C;
    extern const f32 lbl_8047E148;

    Vec3 vA;
    Vec3 vB;
    Vec3 candidate;
    Vec3 diff;
    Vec3 sum;
    Vec3 scaled;
    Vec3 dir;
    Vec3 offset;
    s32 i;
    f32 speed;

    if ((u8)heroMoveChkHinderClear(1)) {
        lbl_8047B3C0 = 0;
        return 1;
    }

    std172_characterGetPosition(0, 0x65, &vA);
    std172_characterGetPosition(0, 0x64, &vB);
    if (fn_8012B19C(1, 0, &vB, lbl_8047E148)) {
        lbl_8047B3C0 = 1;
        lbl_80467090[0].x = vB.x;
        lbl_80467090[0].y = vB.y;
        lbl_80467090[0].z = vB.z;
        return 1;
    }

    PSVECDistance(&vA, &vB);
    PSVECSubtract(&vB, &vA, &diff);
    PSVECScale(lbl_8047E108, &diff, &scaled);
    PSVECAdd(&vA, &scaled, &sum);

    dir.y = lbl_8047E114;
    if (vB.x > vA.x) {
        dir.z = lbl_8047E13C * diff.x;
        dir.x = lbl_8047E13C * -diff.z;
    } else {
        dir.z = lbl_8047E13C * -diff.x;
        dir.x = lbl_8047E13C * diff.z;
    }
    PSVECNormalize(&dir, &dir);

    speed = lbl_8047E100;
    for (i = 0; i < 10; i++, speed += lbl_8047E100) {
        PSVECScale(speed, &dir, &offset);
        PSVECAdd(&sum, &offset, &candidate);
        if (fn_8012B19C(1, 0, &candidate, lbl_8047E148)) {
            lbl_80467090[0].x = candidate.x;
            lbl_80467090[0].y = candidate.y;
            lbl_80467090[0].z = candidate.z;
            if (fn_8012B19C(1, &candidate, &vB, lbl_8047E148)) {
                lbl_8047B3C0 = 2;
                lbl_80467090[1].x = vB.x;
                lbl_80467090[1].y = vB.y;
                lbl_80467090[1].z = vB.z;
                return 1;
            }
        }
    }

    return 0;
}

void fn_801C9B6C(s32 arg0, s32 arg1, u16 arg2, u8 arg3)
{
    s32 floorId;
    s32 resId;
    u8 found;

    if (arg3 >= 2 && arg3 <= 5) {
        fn_801C8E14(arg0, arg1, arg2, arg3);
        return;
    }

    if (arg2 & 1) {
        found = heroMoveGetResID(&floorId, &resId, 1);
        if (!found || (arg3 == 0 && !fn_801C9910())) {
            lbl_8047B3C4 = 0;
        } else {
            lbl_8047B3C4 = 1;
        }
    }

    if (arg3 == 1 && heroMoveGetResID(&floorId, &resId, 1)) {
        lbl_8047B3C4 = 1;
    }

    if (lbl_8047B3C4) {
        fn_801C8E14(arg0, arg1, arg2, arg3);
    } else {
        floorEventCtrlElevator(arg0, arg1, arg2, 0, 100);
    }
}

void fn_801C9C9C(void)
{
    s32 floorId;
    s32 resId;
    u32 found;

    found = (u8)heroMoveGetResID(&floorId, &resId, 1);
    if (found != 0) {
        heroMoveChkHinderClear(1);
    }
}

u8 std193_lightWaitForAnimation(s32 lightId, s32 wait)
{
    void* light = fn_800F92D4(lightId);

    if (light == NULL) {
        GSlogWrite(lbl_802758AC, lightId);
        return 0;
    }

    for (;;) {
        if (GSlightHasAnimationEnded(light) != 0) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 0;
        }
    }

    return 0;
}

void scriptLightStopMotion(s32 lightId)
{
    void* light = fn_800F92D4(lightId);

    if (light == NULL) {
        GSlogWrite(lbl_802758AC, lightId);
    } else {
        GSlightStopAnimation(light);
    }
}

void std191_mapTransformElement(s32 lightId, s32 animIndex, s32 frame, s32 loop)
{
    void* light = fn_800F92D4(lightId);

    if (light == NULL) {
        GSlogWrite(lbl_802758AC, lightId);
    } else {
        GSlightSetAnimIndex(light, animIndex);
        GSlightSetAnimRate(light, lbl_8047E108);
        GSlightSetAnimFrame(light, (f32)frame);
        if (loop != 0) {
            GSlightSetAnimType(light, 1);
        } else {
            GSlightSetAnimType(light, 0);
        }
    }
}

void std190_lightSetActive(s32 lightId, s32 active)
{
    void* light = fn_800F92D4(lightId);

    if (light == NULL) {
        GSlogWrite(lbl_802758AC, lightId);
    } else {
        GSlightSetActive(light, (u8)active);
    }
}

void scriptSetMemoFlag(void)
{
    memoDataSetMemoFlag(0);
}

void std035_setPartnerFollowing(s32 active)
{
    u32 enabled = (u8)active;

    if (enabled != 0) {
        heroSetStatus(0, 0x18, 1);
        fn_8012F1FC(1);
    } else {
        heroSetStatus(0, 0x18, 0);
        heroMoveDismissMember(1);
    }
}

extern u8 std211_tvSetChannelByStory(void);

u8 std182_tvNewsEvent(void)
{
    extern void heroMoveInitEvent(void);
    extern void heroMoveTermEvent(void);
    extern s32 winMsgOpenFieldWithSE(u32 message, u32 wait, u8 pause, u8 sound);
    extern void winMsgCloseField(u32 wait);
    extern u8 fn_801EEAD0(u16 id);
    extern const f32 lbl_8047E11C;

    f32 elapsed;
    u32 mode;

    heroMoveInitEvent();

    if (fn_801EEAD0(0x43)) {
        mode = 4;
    } else if (std081_flagCheckTrue(0xADA)) {
        std078_flagSetFalse(0xA0F);
        std171_mapSetTextureChange(0xC);

        elapsed = lbl_8047E114;
        while (elapsed < lbl_8047E11C) {
            _threadSwitch();
            elapsed += (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
        }

        winMsgOpenFieldWithSE(0x439F, 1, 0, 5);
        winMsgCloseField(1);
        std171_mapSetTextureChange(0xF);
        winMsgOpenFieldWithSE(0x43A0, 1, 0, 5);
        winMsgCloseField(1);
        std211_tvSetChannelByStory();
        heroMoveTermEvent();
        return 1;
    } else if (std081_flagCheckTrue(0xADB)) {
        std078_flagSetFalse(0x483);
        std171_mapSetTextureChange(0xC);

        elapsed = lbl_8047E114;
        while (elapsed < lbl_8047E11C) {
            _threadSwitch();
            elapsed += (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
        }

        winMsgOpenFieldWithSE(0x439D, 1, 0, 5);
        winMsgCloseField(1);
        std171_mapSetTextureChange(0xE);
        winMsgOpenFieldWithSE(0x439E, 1, 0, 5);
        winMsgCloseField(1);
        std211_tvSetChannelByStory();
        heroMoveTermEvent();
        return 1;
    } else if (std081_flagCheckTrue(0x42C)) {
        std078_flagSetFalse(0x42D);
        mode = 4;
    } else if (std081_flagCheckTrue(0xAD3)) {
        std078_flagSetFalse(0xA08);
        mode = 3;
    } else if (std081_flagCheckTrue(0xAD0)) {
        std078_flagSetFalse(0xA07);
        mode = 2;
    } else if (std081_flagCheckTrue(0xACF)) {
        std078_flagSetFalse(0xA06);
        mode = 1;
    } else {
        std078_flagSetFalse(0xA05);
        mode = 0;
    }

    std211_tvSetChannelByStory();

    elapsed = lbl_8047E114;
    while (elapsed < lbl_8047E11C) {
        _threadSwitch();
        elapsed += (f32)(u32)GSgetFrameStep() / (f32)fn_800D37CC();
    }

    switch (mode) {
    case 0:
        winMsgOpenFieldWithSE(0x426E, 1, 0, 5);
        winMsgCloseField(1);
        break;
    case 1:
        winMsgOpenFieldWithSE(0x426F, 1, 0, 5);
        winMsgCloseField(1);
        break;
    case 2:
        winMsgOpenFieldWithSE(0x4270, 1, 0, 5);
        winMsgCloseField(1);
        break;
    case 3:
        winMsgOpenFieldWithSE(0x4271, 1, 0, 5);
        winMsgCloseField(1);
        break;
    case 4:
        winMsgOpenFieldWithSE(0x4272, 1, 0, 5);
        winMsgCloseField(1);
        break;
    default:
        break;
    }

    heroMoveTermEvent();
    return 1;
}

u8 std211_tvSetChannelByStory(void)
{
    u32 cameraMode;

    if (std081_flagCheckTrue(0xADA) != 0) {
        if (std081_flagCheckTrue(0xA0F) == 0) {
            cameraMode = 11;
        } else {
            cameraMode = 12;
        }
    } else if (std081_flagCheckTrue(0xADB) != 0) {
        if (std081_flagCheckTrue(0x483) == 0) {
            cameraMode = 11;
        } else {
            cameraMode = 12;
        }
    } else if (std081_flagCheckTrue(0x42C) != 0) {
        if (std081_flagCheckTrue(0x42D) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (std081_flagCheckTrue(0xAD3) != 0) {
        if (std081_flagCheckTrue(0xA08) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (std081_flagCheckTrue(0xAD0) != 0) {
        if (std081_flagCheckTrue(0xA07) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (std081_flagCheckTrue(0xACF) != 0) {
        if (std081_flagCheckTrue(0xA06) == 0) {
            cameraMode = 1;
        } else {
            cameraMode = 2;
        }
    } else {
        std078_flagSetFalse(0xA05);
        cameraMode = 0;
    }

    std171_mapSetTextureChange(cameraMode);
    return 1;
}

u32 fadeEffectGetRandom(u32 limit)
{
    return _fadeEffectGetRandom__FUl(limit);
}

void std157_sleep(f32 frames)
{
    f32 elapsed = lbl_8047E114;

    while (elapsed < frames) {
        _threadSwitch();
        elapsed += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}

u16 fieldStartEncount(u32 arg0, u8 arg1, s32 arg2)
{
    extern void fn_80112700(void);
    extern void std079_flagSetTrue(u32 flagId);
    extern void* fightEncountDataBiosGetPtr(u32 index);
    extern u16 fn_8006A65C(void);
    extern void std229_mapLoad395(s32 value);
    extern void floorSetFadeScript(s32 a, s32 b);
    extern u32 fightStartEncount(u32 arg0);
    extern u8 fightEncountDataBiosGetZenmetuFlag(void* ptr);
    extern u32 std103_battleGetResult(void);
    extern u8 fightFloorIsGcHeroWin(u32 obj, u32 side);
    extern int fadeSet();
    extern const f32 lbl_8047E108;

    u32 result;
    void* ptr;
    u8 done = 0;

    std079_flagSetTrue(0xE05);
    ptr = fightEncountDataBiosGetPtr(arg0);

    if (std082_flagGetValue(0x8AE)) {
        std079_flagSetTrue(0xE05);
        result = fn_8006A65C();

        if (std082_flagGetValue(0x8AE)) {
            fn_80112700();
        } else {
            std229_mapLoad395(0);
            floorSetFadeScript(0, 0x5960008);
        }
    } else {
        if (arg1 != 0 || arg2 != 0) {
            std078_flagSetFalse(0xE05);
        }

        result = fightStartEncount(arg0);

        if (fightEncountDataBiosGetZenmetuFlag(ptr)) {
            if (!fightFloorIsGcHeroWin(0, std103_battleGetResult())) {
                _threadSwitch();
                _threadSwitch();
                _threadSwitch();
                done = 1;
            }
        }

        if (arg2 == 0 && !done && arg1 != 0) {
            fadeSet(lbl_8047E108, 2);
        }
    }

    std079_flagSetTrue(0xE05);
    return (u16)result;
}

void std239_playerGetCouponsEarnedInLastArea(void)
{
    fn_8025D164();
}

s32 fn_801CA728(s32 count)
{
    s32 base;
    s32 value;
    f32 multiplier;

    base = fn_8006ADEC();
    multiplier = fn_8025D0A8(savedataGetStatus(0, 2));
    value = base + (s32)((f32)count * multiplier);
    fn_8006ADB4(value);
    return value;
}

void fn_801CA7AC(s32 value)
{
    fn_8006ADB4(value);
}

s32 std236_playerGetCouponsEarnedOnCurrentRun(void)
{
    return fn_8006ADEC();
}


#endif /* GS_801C8DD0_ONLY */
