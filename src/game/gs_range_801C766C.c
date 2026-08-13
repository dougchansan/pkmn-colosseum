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
#pragma peephole off

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

extern u32 fn_801906A0(u32 flag);
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
extern void fn_80189990(s32, s32, s32);
extern s8 fn_8001E184(void);
extern void GBAInit(void);
extern void gbaCommandSetKeyState(s32, s32);
extern void fn_8007B090(s32);
extern s32 fn_8007AB10(s32, s32*);
extern s32 fn_8007AAA8(void);
extern void fn_80189490(s32, s32);
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
extern u32 fn_800D3088(void);
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

u8 fn_801C766C(void)
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

void fn_801C852C(s32 mode)
{
    void* status;
    void* pokemon;
    u16 i;

    if (fn_801906A0(0x8AE) == 0) {
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

void fn_801C8628(void)
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

u8 fn_801C8688(u16 arg0, u16 arg1, u16 arg2)
{
    u32 v0;
    u32 v1;
    u32 v2;

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
    default: v0 = 0; break;
    }

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
    default: v1 = 0; break;
    }

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
    default: v2 = 0; break;
    }

    return fn_800884BC(v0, v1, v2) == 0;
}

void fn_801C8804(void)
{
    u32 ready = (u8)fn_80075638();

    if (ready != 0) {
        fn_8007565C();
    }
}

extern u32 fn_800756C8(s32 pokemonId);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);

s32 fn_801C8834(u16 arg0)
{
    u16 v31;
    u16 v30;

    switch (arg0) {
    case 0x200: case 0x201: v31 = 0x99; break;
    case 0x202: case 0x203: v31 = 0x9c; break;
    case 0x204: case 0x205: v31 = 0x9f; break;
    case 0x206: case 0x207: v31 = 0xb9; break;
    case 0x208: case 0x209: v31 = 0xc8; break;
    case 0x20a: case 0x20b: v31 = 0x11f; break;
    case 0x20c: case 0x20d: v31 = 0xf3; break;
    case 0x20e: case 0x20f: v31 = 0xf4; break;
    case 0x210: case 0x211: v31 = 0xf5; break;
    default: v31 = 0; break;
    }

    if (v31 != 0) {
        fn_800756C8(v31);

        switch (arg0) {
        case 0x200: v30 = 0x201; break;
        case 0x202: v30 = 0x203; break;
        case 0x204: v30 = 0x205; break;
        case 0x206: v30 = 0x207; break;
        case 0x208: v30 = 0x209; break;
        case 0x20a: v30 = 0x20b; break;
        case 0x20c: v30 = 0x20d; break;
        case 0x20e: v30 = 0x20f; break;
        case 0x210: v30 = 0x211; break;
        default: v30 = 0; break;
        }

        if (v30 != 0) {
            heroItemDecItemDataId((u8*)0, arg0, 1, -1);
            heroItemAddItemDataId((u8*)0, v30, 1, -1);
        }
    }

    return v31;
}

s32 fn_801C89F8(s32 arg0, s32 arg1)
{
    s32 linkState;
    s32 linkWork;
    s32 result;
    u8 repeat;
    s32 answer;
    f32 elapsed;

    fn_80189990(arg0, arg1, 0x3B61);
    if (fn_8001E184() == 0) {
        GBAInit();
        gbaCommandSetKeyState(2, 0);
        fn_8007B090(1);
        fn_80189990(arg0, arg1, 0x3B62);

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
                fn_80189990(arg0, arg1, 0x3B63);
                if (fn_8001E184() != 0) {
                    linkState = 29;
                } else {
                    linkState = 4;
                }
                break;
            case 4:
                fn_80189990(arg0, arg1, 0x3B64);
                if (fn_8001E184() != 0) {
                    linkState = 9;
                } else {
                    linkState = 5;
                }
                break;
            case 5:
                fn_80189990(arg0, arg1, 0x3B66);
                if (fn_8001E184() != 0) {
                    linkState = 29;
                } else {
                    linkState = 6;
                }
                break;
            case 7:
                fn_80189990(arg0, arg1, 0x3B82);
                if (fn_8001E184() != 0) {
                    linkState = 29;
                } else {
                    linkState = 8;
                }
                fn_8007AB10(7, &linkWork);
                break;
            case 9:
                fn_80189990(arg0, arg1, 0x3B6C);
                if (fn_8001E184() != 0) {
                    linkState = 29;
                } else {
                    linkState = 10;
                }
                fn_8007AB10(9, &linkWork);
                break;
            case 11:
                fn_80189990(arg0, arg1, 0x3B82);
                answer = (s8)fn_8001E184();
                fn_8007AB10(9, &linkWork);
                if (answer != 0) {
                    linkState = 29;
                } else {
                    linkState = 12;
                }
                fn_8007AB10(11, &linkWork);
                break;
            case 14:
                fn_80189990(arg0, arg1, 0x3B6F);
                repeat = 1;
                break;
            case 15:
                fn_80189990(arg0, arg1, 0x3B80);
                repeat = 1;
                break;
            case 16:
                if (linkWork != 0) {
                    fn_8007AAA8();
                }
                fn_80189990(arg0, arg1, 0x3B81);
                fn_80189990(arg0, arg1, 0x3B6A);
                fn_80189490(arg0, arg1);
                return 1;
            case 17:
                fn_80189990(arg0, arg1, 0x3B68);
                elapsed = lbl_8047E114;
                while (elapsed < lbl_8047E100) {
                    _threadSwitch();
                    elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
                }
                linkState = 16;
                break;
            case 19:
                fn_80189990(arg0, arg1, 0x3B82);
                if (fn_8001E184() != 0) {
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

    fn_80189990(arg0, arg1, 0x3B83);
    fn_80189490(arg0, arg1);
    return 0;
}

void fn_801C8DD0(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    menuSubOpenSelect(1, a, b, c, d, e);
}

extern s32 fn_801C8E14(s32 floorDataId, u32 actorIndex, u16 mode,
                       u8 direction);
extern u8 fn_801C9910(void);
extern void floorEventCtrlElevator(s32 arg0, s32 arg1, u16 arg2,
                                   u8 arg3, s32 speed);
extern u8 lbl_8047B3C4;

u8 fn_801C9910(void)
{
    extern u8 fn_8012B19C(s32 arg0, s32 arg1, Vec3* pos);
    extern void fn_8018BDF4(s32 arg0, s32 arg1, Vec3* out);
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

    Vec3 vA;
    Vec3 vB;
    Vec3 diff;
    Vec3 scaled;
    Vec3 sum;
    Vec3 dir;
    Vec3 offset;
    Vec3 candidate;
    s32 i;
    f32 speed;

    if (heroMoveChkHinderClear(1)) {
        lbl_8047B3C0 = 0;
        return 1;
    }

    fn_8018BDF4(0, 0x65, &vA);
    fn_8018BDF4(0, 0x64, &vB);

    if (fn_8012B19C(1, 0, &vB)) {
        lbl_80467090[0] = vB;
        lbl_8047B3C0 = 1;
        return 1;
    }

    PSVECDistance(&vA, &vB);
    PSVECSubtract(&vB, &vA, &diff);
    PSVECScale(lbl_8047E108, &diff, &scaled);
    PSVECAdd(&vA, &scaled, &sum);

    dir.y = lbl_8047E114;
    if (vB.x > vA.x) {
        dir.x = lbl_8047E13C * -diff.z;
        dir.z = lbl_8047E13C * diff.x;
    } else {
        dir.x = lbl_8047E13C * diff.z;
        dir.z = lbl_8047E13C * -diff.x;
    }
    PSVECNormalize(&dir, &dir);

    speed = lbl_8047E100;
    for (i = 0; i < 10; i++, speed += lbl_8047E100) {
        PSVECScale(speed, &dir, &offset);
        PSVECAdd(&sum, &offset, &candidate);
        if (fn_8012B19C(1, 0, &candidate)) {
            lbl_80467090[0] = candidate;
            if (fn_8012B19C(1, 0, &vB)) {
                lbl_80467090[1] = vB;
                lbl_8047B3C0 = 2;
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

u8 fn_801C9CDC(s32 lightId, s32 wait)
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

void fn_801C9DC4(s32 lightId, s32 animIndex, s32 frame, s32 loop)
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

void fn_801C9E7C(s32 lightId, s32 active)
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

void fn_801C9F00(s32 active)
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

extern u8 fn_801CA358(void);

u8 fn_801C9F5C(void)
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
    } else if (fn_801902E0(0xADA)) {
        fn_80190528(0xA0F);
        fn_80117AE4(0xC);

        elapsed = lbl_8047E114;
        while (elapsed < lbl_8047E11C) {
            _threadSwitch();
            elapsed += (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();
        }

        winMsgOpenFieldWithSE(0x439F, 1, 0, 5);
        winMsgCloseField(1);
        fn_80117AE4(0xF);
        winMsgOpenFieldWithSE(0x43A0, 1, 0, 5);
        winMsgCloseField(1);
        fn_801CA358();
        heroMoveTermEvent();
        return 1;
    } else if (fn_801902E0(0xADB)) {
        fn_80190528(0x483);
        fn_80117AE4(0xC);

        elapsed = lbl_8047E114;
        while (elapsed < lbl_8047E11C) {
            _threadSwitch();
            elapsed += (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();
        }

        winMsgOpenFieldWithSE(0x439D, 1, 0, 5);
        winMsgCloseField(1);
        fn_80117AE4(0xE);
        winMsgOpenFieldWithSE(0x439E, 1, 0, 5);
        winMsgCloseField(1);
        fn_801CA358();
        heroMoveTermEvent();
        return 1;
    } else if (fn_801902E0(0x42C)) {
        fn_80190528(0x42D);
        mode = 4;
    } else if (fn_801902E0(0xAD3)) {
        fn_80190528(0xA08);
        mode = 3;
    } else if (fn_801902E0(0xAD0)) {
        fn_80190528(0xA07);
        mode = 2;
    } else if (fn_801902E0(0xACF)) {
        fn_80190528(0xA06);
        mode = 1;
    } else {
        fn_80190528(0xA05);
        mode = 0;
    }

    fn_801CA358();

    elapsed = lbl_8047E114;
    while (elapsed < lbl_8047E11C) {
        _threadSwitch();
        elapsed += (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();
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

u8 fn_801CA358(void)
{
    u32 cameraMode;

    if (fn_801902E0(0xADA) != 0) {
        if (fn_801902E0(0xA0F) == 0) {
            cameraMode = 11;
        } else {
            cameraMode = 12;
        }
    } else if (fn_801902E0(0xADB) != 0) {
        if (fn_801902E0(0x483) == 0) {
            cameraMode = 11;
        } else {
            cameraMode = 12;
        }
    } else if (fn_801902E0(0x42C) != 0) {
        if (fn_801902E0(0x42D) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (fn_801902E0(0xAD3) != 0) {
        if (fn_801902E0(0xA08) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (fn_801902E0(0xAD0) != 0) {
        if (fn_801902E0(0xA07) == 0) {
            cameraMode = 3;
        } else {
            cameraMode = 4;
        }
    } else if (fn_801902E0(0xACF) != 0) {
        if (fn_801902E0(0xA06) == 0) {
            cameraMode = 1;
        } else {
            cameraMode = 2;
        }
    } else {
        fn_80190528(0xA05);
        cameraMode = 0;
    }

    fn_80117AE4(cameraMode);
    return 1;
}

u32 fadeEffectGetRandom(u32 limit)
{
    return _fadeEffectGetRandom__FUl(limit);
}

void fn_801CA4F8(f32 frames)
{
    f32 elapsed = lbl_8047E114;

    while (elapsed < frames) {
        _threadSwitch();
        elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}

u16 fn_801CA5C4(u32 arg0, u8 arg1, s32 arg2)
{
    extern void fn_80112700(void);
    extern void fn_801903B0(u32 flagId);
    extern void* fightEncountDataBiosGetPtr(u32 index);
    extern u16 fn_8006A65C(void);
    extern void fn_801CBA90(s32 value);
    extern void floorSetFadeScript(s32 a, s32 b);
    extern u32 fn_8020DAD0(u32 arg0);
    extern u8 fightEncountDataBiosGetZenmetuFlag(void* ptr);
    extern u32 fn_801EF634(void);
    extern u8 fightFloorIsGcHeroWin(u32 obj, u32 side);
    extern int fadeSet();
    extern const f32 lbl_8047E108;

    u8 done = 0;
    u32 result;
    u32 old;
    u16 input;

    fn_801903B0(0xE05);
    result = (u32)fightEncountDataBiosGetPtr(arg0);

    if (fn_801906A0(0x8AE)) {
        fn_801903B0(0xE05);
        result = fn_8006A65C();

        if (fn_801906A0(0x8AE)) {
            fn_80112700();
        } else {
            fn_801CBA90(0);
            floorSetFadeScript(0, 0x5960008);
        }
    } else {
        if (arg1 != 0 || arg2 != 0) {
            fn_80190528(0xE05);
        }

        old = result;
        result = fn_8020DAD0(arg0);

        if (fightEncountDataBiosGetZenmetuFlag((void*)old)) {
            input = fn_801EF634();
            if (!fightFloorIsGcHeroWin(0, input)) {
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

    fn_801903B0(0xE05);
    return (u16)result;
}

void fn_801CA708(void)
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

s32 fn_801CA7CC(void)
{
    return fn_8006ADEC();
}


/* Eight-direction collision search used to place a battle actor. */
s32 fn_801C7730(s32 side, s32 slot)
{
    extern const Vec3 lbl_802758A0;
    extern const f32 lbl_8047E100, lbl_8047E104, lbl_8047E10C;
    extern const f32 lbl_8047E110, lbl_8047E118, lbl_8047E11C;
    extern void fn_8018BDF4(s32 arg0, s32 arg1, Vec3* out);
    extern u32 fn_8018D998();
    extern void* peopleSearchID();
    extern void* peopleInfoBiosGetPtr();
    extern f32 fn_8018F5E4();
    extern f32 GSvecDistance();
    extern void GSvecAdd();
    extern void fn_800E013C();
    extern u8 fn_8018D680();
    extern s32 fn_8010F320();
    extern void qsort();
    extern void fn_80183350();
    extern void fn_8018AACC();
    extern u8 peopleMoveCheck();
    extern void fn_8018BA04();
    extern void fn_80187D48();
    extern void fn_80183018();
    extern f64 cos(f64);
    extern f64 sin(f64);

    s32 floorId, resId, floorId2, resId2;
    Vec3 origin = lbl_802758A0;
    Vec3 partner, requested, center, offset, midpoint, saved;
    DistanceSortEntry entries[8];
    f32 radius;
    f32 timeout;
    s32 i, k, hit, wanted;
    u8 done;

#define SEGMENT_HIT(out, a, b, c) do {                                    \
    if (GSvecDistance((a), (b)) < lbl_8047E104) {                         \
        (out) = fn_8018D680((a), (b), (c), lbl_8047E118);                 \
    } else {                                                               \
        GSvecAdd(&midpoint, (a), (b));                                    \
        fn_800E013C(&midpoint, &midpoint, lbl_8047E108);                  \
        (out) = fn_8018D680((a), &midpoint, (c), lbl_8047E118);           \
        if ((out) == 0)                                                    \
            (out) = fn_8018D680(&midpoint, (b), (c), lbl_8047E118);       \
    }                                                                      \
} while (0)

#define WAIT_MOVE(out) do {                                                \
    (out) = 0;                                                             \
    timeout = lbl_8047E11C;                                                \
    while (peopleMoveCheck(0, 0x65, 0) != 0) {                            \
        _threadSwitch();                                                   \
        timeout -= (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();          \
        if (timeout <= lbl_8047E114) { (out) = 1; break; }                 \
    }                                                                      \
} while (0)

    if (heroMoveGetResID(&floorId, &resId, 1) == 0) return 0;
    fn_8018BDF4(resId, floorId, &origin);
    if (heroMoveGetResID(&floorId2, &resId2, 0) == 0) return 0;
    fn_8018BDF4(resId2, floorId2, &partner);
    fn_8018BDF4(side, slot, &requested);

    {
        void* person = peopleSearchID(fn_8018D998(side, slot));
        void* info;
        if (person == NULL) return 0;
        info = peopleInfoBiosGetPtr(*(u32*)((u8*)person + 0x30));
        if (info == NULL) return 0;
        radius = fn_8018F5E4(info) + lbl_8047E100;
    }

    center = origin;
    if (GSvecDistance(&partner, &requested) < lbl_8047E104) {
        hit = fn_8018D680(&partner, &requested, &center,
                          radius + lbl_8047E100);
    } else {
        GSvecAdd(&midpoint, &partner, &requested);
        fn_800E013C(&midpoint, &midpoint, lbl_8047E108);
        hit = fn_8018D680(&partner, &midpoint, &center,
                          radius + lbl_8047E100);
        if (hit == 0)
            hit = fn_8018D680(&midpoint, &requested, &center,
                              radius + lbl_8047E100);
    }
    if (hit == 0) return 0;

    for (i = 0; i < 8; i++) {
        f32 angle = (f32)i * lbl_8047E10C;
        entries[i].index = i;
        offset.x = lbl_8047E110 * (f32)cos(angle);
        offset.y = lbl_8047E114;
        offset.z = lbl_8047E110 * (f32)sin(angle);
        GSvecAdd(&entries[i].pos, &offset, &partner);
        if (fn_8010F320(&partner, &entries[i].pos, 0) != 0) {
            entries[i].blocked = 1;
        } else {
            entries[i].blocked = 0;
        }
        entries[i].distance = GSvecDistance(&entries[i].pos, &origin);
    }
    qsort(entries, 8, sizeof(DistanceSortEntry), _fnDistanceSortFunc__FPCvPCv);

    for (i = 0; i < 8; i++) {
        if (entries[i].blocked == 1) continue;
        center = entries[i].pos;
        SEGMENT_HIT(hit, &partner, &requested, &center);
        if (hit != 0) continue;
        center = partner;
        SEGMENT_HIT(hit, &origin, &entries[i].pos, &center);
        if (hit != 0) continue;

        fn_80183350(resId, floorId);
        fn_8018AACC(resId, floorId, 1, &entries[i].pos);
        WAIT_MOVE(done);
        if (done != 0) break;

        wanted = -2;
        if (entries[i].index == 1 || entries[i].index == 3) wanted = -1;
        if (entries[i].index == 5) wanted = 4;
        if (entries[i].index == 7) wanted = 0;
        if (wanted != -2) {
            for (k = 0; k < 8; k++) {
                if (entries[k].index == wanted && entries[k].blocked == 0) {
                    center = entries[k].pos;
                    SEGMENT_HIT(hit, &partner, &requested, &center);
                    if (hit == 0) {
                        fn_8018AACC(resId, floorId, 1, &entries[k].pos);
                        WAIT_MOVE(done);
                    }
                    break;
                }
            }
        }
        if (done == 0) {
            fn_8018BA04(resId, floorId, &saved);
            fn_80187D48(resId, floorId, saved.x, saved.y, saved.z,
                        lbl_8047E100);
            WAIT_MOVE(done);
            fn_80183018(resId, floorId);
            return 0;
        }
    }

    for (i = 0; i < 8; i++) {
        f32 angle = (f32)i * lbl_8047E10C;
        entries[i].index = i;
        offset.x = lbl_8047E110 * (f32)cos(angle);
        offset.y = lbl_8047E114;
        offset.z = lbl_8047E110 * (f32)sin(angle);
        GSvecAdd(&entries[i].pos, &offset, &origin);
        if (fn_8010F320(&origin, &entries[i].pos, 0) != 0) {
            entries[i].blocked = 1;
        } else {
            entries[i].blocked = 0;
        }
        entries[i].distance = GSvecDistance(&entries[i].pos, &origin);
    }
    qsort(entries, 8, sizeof(DistanceSortEntry), _fnDistanceSortFunc__FPCvPCv);

    for (i = 0; i < 8; i++) {
        if (entries[i].blocked == 1) continue;
        center = partner;
        SEGMENT_HIT(hit, &origin, &entries[i].pos, &center);
        if (hit != 0) continue;
        center = entries[i].pos;
        SEGMENT_HIT(hit, &partner, &requested, &center);
        if (hit != 0) continue;
        fn_80183350(resId, floorId);
        fn_8018AACC(resId, floorId, 1, &entries[i].pos);
        WAIT_MOVE(done);
        if (done == 0) {
            fn_8018BA04(resId, floorId, &saved);
            fn_80187D48(resId, floorId, saved.x, saved.y, saved.z,
                        lbl_8047E100);
            WAIT_MOVE(done);
            fn_80183018(resId, floorId);
            return 0;
        }
    }

#undef WAIT_MOVE
#undef SEGMENT_HIT
    return 0;
}

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
    typedef struct PartTransform {
        Vec3 position;
        f32 rest[4];
    } PartTransform;

    extern u32* lbl_80478EC8;
    extern FieldAnimActor* lbl_80478ECC;
    extern s32 lbl_8047B3C0;
    extern u8 lbl_8047B3C4;
    extern Vec3 lbl_80467090[];
    extern const f32 lbl_8047E100;
    extern const f32 lbl_8047E11C;
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
    extern void fn_801845E4();
    extern void fn_801860F8();
    extern void fn_80188AF4();
    extern void fn_80184470();
    extern void fn_80188F78();
    extern void fn_8018805C();
    extern void fn_8018AACC();
    extern void fn_8018C0A8();
    extern u8 peopleMoveCheck();
    extern void fn_80166A28();
    extern void fn_801669BC();
    extern void GSmodelSetAnimIndex();
    extern void GSmodelSetAnimFrame();
    extern void GSmodelSetAnimRate();
    extern void GSmodelSetAnimType();
    extern void GSmodelStartAnimation();
    extern u8 GSmodelHasAnimationEnded();
    extern u8 GSmodelCanAnimate();
    extern void* GSmodelGetPart();
    extern void GSpartGetTransform();
    extern void GSpartFree();
    extern void GSmodelGetFrameCount();

    FieldAnimActor* actor;
    void* model;
    void* part;
    PartTransform transform;
    Vec3 firstPos;
    Vec3 secondPos;
    f32 frame;
    f32 timer;
    s32 groupId;
    s16 animIndex;
    u8 timedOut;
    s32 i;

#define START_MODEL_ANIM(index, startFrame)                                \
    do {                                                                    \
        if (model != 0 && (s16)(index) >= 0) {                             \
            GSmodelSetAnimIndex(model, (s16)(index));                      \
            GSmodelSetAnimFrame(model, (startFrame));                      \
            GSmodelSetAnimRate(model, lbl_8047E108);                       \
            GSmodelSetAnimType(model, 0);                                  \
            GSmodelStartAnimation(model);                                  \
        }                                                                   \
    } while (0)

#define WAIT_MODEL()                                                        \
    do {                                                                    \
        if (model != 0) {                                                   \
            while (GSmodelHasAnimationEnded(model) == 0) {                 \
                _threadSwitch();                                            \
            }                                                               \
        }                                                                   \
    } while (0)

#define WAIT_SECONDS(amount)                                                \
    do {                                                                    \
        timer = lbl_8047E114;                                               \
        while (timer < (amount)) {                                         \
            _threadSwitch();                                                \
            timer += (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();         \
        }                                                                   \
    } while (0)

#define WAIT_ACTOR65(out)                                                   \
    do {                                                                    \
        (out) = 0;                                                          \
        timer = lbl_8047E11C;                                               \
        while (timer > lbl_8047E114) {                                     \
            if (peopleMoveCheck(0, 0x65, 0) == 0) {                        \
                break;                                                      \
            }                                                               \
            _threadSwitch();                                                \
            timer -= (f32)(u32)fn_800D3088() / (f32)fn_800D37CC();         \
        }                                                                   \
        if (timer <= lbl_8047E114) {                                       \
            (out) = 1;                                                      \
        }                                                                   \
    } while (0)

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
    model = floorGetResource(actor->resource);
    if (model == 0) {
        return -1;
    }
    groupId = floorDataBiosGetGroupID(floorDataBiosGetPtr(floorDataId));

    if (direction >= 2 && direction <= 5) {
        fn_801845E4(0, 0x64, groupId, actor->resource, actor->partIndex);
        fn_801845E4(0, 0x65, groupId, actor->resource, actor->partIndex);
        switch (direction) {
        case 2:
            fn_801860F8(0, 0x64, lbl_8047E114, lbl_8047E114,
                        lbl_8047E130);
            fn_801860F8(0, 0x65, lbl_8047E114, lbl_8047E114,
                        lbl_8047E134);
            break;
        case 3:
            fn_801860F8(0, 0x64, lbl_8047E114, lbl_8047E114,
                        lbl_8047E134);
            fn_801860F8(0, 0x65, lbl_8047E114, lbl_8047E114,
                        lbl_8047E130);
            break;
        case 4:
            fn_801860F8(0, 0x64, lbl_8047E130, lbl_8047E114,
                        lbl_8047E114);
            fn_801860F8(0, 0x65, lbl_8047E134, lbl_8047E114,
                        lbl_8047E114);
            break;
        case 5:
            fn_801860F8(0, 0x64, lbl_8047E134, lbl_8047E114,
                        lbl_8047E114);
            fn_801860F8(0, 0x65, lbl_8047E130, lbl_8047E114,
                        lbl_8047E114);
            break;
        }
        fn_80188AF4(0, 0x65);
        START_MODEL_ANIM(mode, lbl_8047E114);
        return 0;
    }

    mode = (u16)mode;
    switch (mode) {
    case 1:
    case 2:
    case 0x81:
    case 0x82:
        START_MODEL_ANIM(actor->enterAnim, frame);
        if ((mode & 0x80) != 0) {
            fn_80166A28(0x44);
        }
        WAIT_MODEL();

        part = GSmodelGetPart(model, actor->partIndex);
        GSpartGetTransform(part, &transform, 0, 0);
        GSpartFree(part);

        if ((mode & 1) != 0) {
            fn_80188AF4(0, 0x65);
            transform.position.z -= lbl_8047E130;
            fn_8018AACC(0, 0x64, 1, &transform.position);
            firstPos.x = transform.position.x;
            firstPos.y = transform.position.y;
            firstPos.z = transform.position.z + lbl_8047E138;
            WAIT_SECONDS(lbl_8047E13C);

            if (lbl_8047B3C0 != 0) {
                for (i = 0; i < lbl_8047B3C0; i++) {
                    fn_8018AACC(0, 0x65, 1, &lbl_80467090[i]);
                    WAIT_ACTOR65(timedOut);
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
                WAIT_ACTOR65(timedOut);
            }
            fn_8018805C(0, 0x64, lbl_8047E114, lbl_8047E108);
            if (timedOut == 0) {
                fn_8018805C(0, 0x65, lbl_8047E114, lbl_8047E108);
            }
            peopleMoveCheck(0, 0x64, 1);
            if (timedOut == 0) {
                WAIT_ACTOR65(timedOut);
            }
        } else {
            fn_80184470(0, 0x64);
            fn_80184470(0, 0x65);
            fn_80188F78(0, 0x65);

            part = GSmodelGetPart(model, actor->partIndex);
            GSpartGetTransform(part, &transform, 0, 0);
            GSpartFree(part);
            firstPos = transform.position;
            secondPos = transform.position;
            firstPos.z += lbl_8047E130;
            secondPos.z -= lbl_8047E130;
            fn_8018C0A8(0, 0x64, &firstPos);
            fn_8018C0A8(0, 0x65, &secondPos);
            firstPos.z += lbl_8047E140;
            secondPos.z += lbl_8047E140;
            fn_8018AACC(0, 0x64, 1, &firstPos);
            WAIT_SECONDS(lbl_8047E13C);
            fn_8018AACC(0, 0x65, 1, &secondPos);
            peopleMoveCheck(0, 0x64, 1);
            peopleMoveCheck(0, 0x65, 1);
            lbl_8047B3C4 = 0;
        }

        START_MODEL_ANIM(actor->exitAnim, frame);
        if ((mode & 0x80) != 0) {
            fn_80166A28(0x44);
        }
        WAIT_MODEL();
        break;
    case 0xC0:
        WAIT_MODEL();
        if ((mode & 0x80) != 0) {
            fn_801669BC(0x45);
            fn_80166A28(0x46);
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
        if (animIndex < 0 || GSmodelCanAnimate(model) == 0) {
            return -1;
        }

        if ((mode & 0x20) != 0) {
            fn_801845E4(0, 0x64, groupId, actor->resource,
                        actor->partIndex);
            fn_801845E4(0, 0x65, groupId, actor->resource,
                        actor->partIndex);
            if (direction != 0) {
                fn_801860F8(0, 0x64, lbl_8047E114, lbl_8047E114,
                            lbl_8047E130);
                fn_801860F8(0, 0x65, lbl_8047E114, lbl_8047E114,
                            lbl_8047E134);
            } else {
                fn_801860F8(0, 0x64, lbl_8047E114, lbl_8047E114,
                            lbl_8047E134);
                fn_801860F8(0, 0x65, lbl_8047E114, lbl_8047E114,
                            lbl_8047E130);
            }
            fn_80188AF4(0, 0x65);
        }
        if ((mode & 0x10) != 0) {
            GSmodelSetAnimIndex(model, animIndex);
            GSmodelGetFrameCount(model, &frame, 0);
            frame -= lbl_8047E100;
        }
        START_MODEL_ANIM(animIndex, frame);
        if ((mode & 0x40) != 0) {
            fn_801C8E14(floorDataId, actorIndex, 0xC0, direction);
        } else if ((mode & 0x80) != 0) {
            fn_80166A28(0x45);
        }
        break;
    }

    GSmodelGetFrameCount(model, &frame, 0);
    return (s32)(lbl_8047E144 *
                 ((lbl_8047E148 * frame) / (f32)fn_800D37CC()));

#undef WAIT_ACTOR65
#undef WAIT_SECONDS
#undef WAIT_MODEL
#undef START_MODEL_ANIM
}
#pragma peephole reset
