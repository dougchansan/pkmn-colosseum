/**
 * @file GScolsys2Human_range_8010FAF4.c
 * @brief GScolsys2Human (tail) -- human/character collision queries.
 *
 * Second of six translation units recovered from the former
 * game/gs_field_colquery.c CodeCandidate bucket (0x8010F6A0-0x801140DC).
 * Only the tail of the XD GScolsys2Human.cpp TU falls in our address
 * range (the head is elsewhere); named with a _range_ suffix per
 * convention since the exact internal split boundary vs. the next
 * unit (GScolsys2Thru) is lower-confidence.
 *
 * Address range: 0x8010FAF4 - 0x80110084
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* 0x8010FAF4 | 0x304 */
s32 fn_8010FAF4(
    u8* source, f32 unusedStep, s32 excludedIndex,
    GScolsys2Vec3* segmentStart,
    GScolsys2Vec3* segmentEnd, GScolsys2Vec3* result)
{
    extern u8* GScolsys2GetCurFloor(void);
    extern void* fn_8018D998(s32, s32);
    extern void* peopleSearchID(void*);
    extern GScolsys2Vec3* fn_8018FCBC(void*);
    extern f32 sqrtf(f32);
    extern f32 lbl_8047CF20;
    extern f32 lbl_8047CF40;
    extern f32 lbl_8047CF44;
    u8* floor;
    GScolsys2Vec3 current;
    GScolsys2Vec3 adjusted;
    s32 pass;

    (void)segmentStart;
    (void)unusedStep;
    current = *segmentEnd;
    floor = GScolsys2GetCurFloor();
    pass = 0;

    do {
        s32 collided;
        s32 index;
        u8* entry;

        collided = 0;
        entry = floor + 0xA00;
        for (index = 0; index < 0x30; index++, entry += 0x14) {
            GScolsys2Vec3* other;
            void* person;
            f32 dx;
            f32 dz;
            f32 distance;
            f32 combinedRadius;
            f32 scale;

            if (index == excludedIndex ||
                (*(u16*)(entry + 0x10) & 1) == 0 ||
                (*(u16*)(entry + 0x10) & 2) != 0) {
                continue;
            }
            person = peopleSearchID(
                fn_8018D998(*(s32*)(entry + 0), *(s32*)(entry + 4)));
            if (person == NULL) {
                continue;
            }
            other = fn_8018FCBC(person);
            if (other == NULL) {
                continue;
            }
            if (other->y >= current.y + *(f32*)(source + 0xC) ||
                other->y + *(f32*)(entry + 0xC) <= current.y) {
                continue;
            }

            dx = current.x - other->x;
            dz = current.z - other->z;
            distance = sqrtf(dx * dx + dz * dz);
            combinedRadius =
                *(f32*)(entry + 8) + *(f32*)(source + 8);
            if (distance >= combinedRadius) {
                continue;
            }
            if (result == NULL) {
                return 1;
            }
            if (distance <= lbl_8047CF20) {
                distance = lbl_8047CF40;
            }
            scale = (lbl_8047CF44 + combinedRadius) / distance;
            adjusted.x = other->x + dx * scale;
            adjusted.y = current.y;
            adjusted.z = other->z + dz * scale;
            collided = 1;
            break;
        }
        if (!collided) {
            break;
        }
        current = adjusted;
        pass++;
    } while (pass < 10);

    if (pass <= 0) {
        return 0;
    }
    *result = current;
    return 1;
}

/* 0x8010FDF8 | 0x1CC */
s32 GScolsys2HumanCollision(
    s32 index, GScolsys2Vec3* start, GScolsys2Vec3* end, void* result) {
    extern u8* GScolsys2GetCurFloor(void);
    extern f32 PSVECDistance(void*, void*);
    extern void PSVECSubtract(void*, void*, void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);
    extern s32 fn_8010FAF4(u8*, f32, s32, GScolsys2Vec3*,
                           GScolsys2Vec3*, GScolsys2Vec3*);
    extern f32 lbl_8047CF20;
    extern f32 lbl_8047CF40;
    u8* floor;
    u8* entry;
    GScolsys2Vec3 delta;
    GScolsys2Vec3 segmentStart;
    GScolsys2Vec3 segmentEnd;
    f32 distance;
    f32 one;
    f32 step;
    f32 zero;
    f32 t;
    s32 status;

    if (index < 0 || index >= 0x30) {
        status = 4;
    } else {
        floor = GScolsys2GetCurFloor();
        if (floor == NULL) {
            status = 1;
        } else {
            u8* candidate = floor + 0xA00 + index * 0x14;
            if ((*(u16*)(candidate + 0x10) & 1) == 0) {
                status = 4;
            } else {
                entry = candidate;
                status = 0;
            }
        }
    }
    if (status != 0) {
        return status;
    }

    distance = PSVECDistance(start, end);
    if (distance > lbl_8047CF20) {
        step = *(f32*)(entry + 8);
        step /= distance;
        if (step > lbl_8047CF40) {
            step = lbl_8047CF40;
        }
    } else {
        step = lbl_8047CF20;
    }
    PSVECSubtract(end, start, &delta);
    zero = lbl_8047CF20;
    one = lbl_8047CF40;
    t = zero;
    while (t < one) {
        f32 next = t + step;
        if (next > one) {
            next = one;
        }
        PSVECScale(&delta, &segmentStart, t);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&delta, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010FAF4(entry, next, index, &segmentStart,
                        &segmentEnd, result)) {
            return 6;
        }
        if (step <= zero) {
            break;
        }
        t += step;
    }
    return 7;
}
/* 0x8010FFC4 | 0xC0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GScolsys2HumanEnable(s32 index, s32 flag) {
#pragma optimization_level 4
    extern u8* GScolsys2GetCurFloor(void);
    u8* table;
    u8* entry;
    u8* p;
    s32 result;

    if (index < 0 || index >= 0x30) {
        result = 4;
    } else {
        table = GScolsys2GetCurFloor();
        if (table == NULL) {
            result = 1;
        } else {
            p = table + index * 0x14 + 0xA00;
            if ((*(u16*)(p + 0x10) & 1) == 0) {
                result = 4;
            } else {
                entry = p;
                result = 0;
            }
        }
    }
    if (result != 0) {
        return result;
    }
    if (flag != 0) {
        *(u16*)(entry + 0x10) &= ~0x2;
    } else {
        *(u16*)(entry + 0x10) |= 0x2;
    }
    return 0;
}
#pragma pop
