/**
 * @file GScolsys2Human_range_8010FAF4.c
 * @brief GScolsys2Human (head): GScolsys2CheckCollision (XD checkCollision) and
 *        GScolsys2HumanCollision, .text 0x8010FAF4-0x8010FFC4 with the
 *        TU's .sdata2 pool 0x8047CF20-0x8047CF48.
 *
 * The TU continues with GScolsys2HumanEnable and fn_80110084, linked as
 * the carve GScolsys2Human_exact_8010FFC4.c. This unit owns the pool
 * (0.0f, 0.5, 3.0, 0.0, 1.0f, 0.0001f) and writes the constants as
 * literals; they are emitted in retail's order. Retail loads the 1.0f in
 * GScolsys2HumanCollision's step clamp in an order that only a literal,
 * not an extern, gives.
 *
 * XD GScolsys2Human.o (NXXJ01.map lines 6894-6906, StarsMmd/Colo-XD-PBR-
 * symbol-maps) has checkCollisionHeadToHead (0x428) and checkCollision
 * (0x108). Colosseum inlines the first into the second. The cylinder
 * test's per-slot 0/1 result is the helper's return value, and its
 * xz-delta is a GSvec local, as XD's vector code has it: as scalar
 * locals the delta takes the wrong float registers.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

extern GSColFloor* GScolsys2GetCurFloor(void);

typedef union GScolsys2HumanFloatShape {
    f32 value;
    u32 bits;
} GScolsys2HumanFloatShape;

extern f32 lbl_80478AC0[];
extern f64 __frsqrte(f64 value);

static inline f32 GScolsys2HumanSqrt(f32 value)
{
    GScolsys2HumanFloatShape shape;
    f64 estimate;
    u32 exponent;
    s32 fpclass;

    if (value > 0.0f) {
        estimate = __frsqrte(value);
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        estimate = 0.5 * estimate *
                   (3.0 - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if ((f64)value < 0.0) {
        return lbl_80478AC0[0];
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 1;
        } else {
            fpclass = 2;
        }
        break;
    case 0:
        if ((shape.bits & 0x007FFFFF) != 0) {
            fpclass = 5;
        } else {
            fpclass = 3;
        }
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

extern void* peopleIsInGroup(s32, s32);
extern void* peopleSearchID(void*);
extern GScolsys2Vec3* peopleGetPositionPtr(void*);

/*
 * XD checkCollisionHeadToHead__FP15GSCOLSYS2_HUMANP15GSCOLSYS2_HUMANP5GSvecP5GSvec
 * (0x80118B24, 0x428): cylinder test of one registered human against pos.
 */
static inline s32 checkCollisionHeadToHead(GSColFloorEvent* self,
                                           GSColFloorEvent* other,
                                           GScolsys2Vec3* pos,
                                           GScolsys2Vec3* out)
{
    void* person;
    GScolsys2Vec3* otherPos;
    GScolsys2Vec3 d;
    f32 distance;
    f32 radius;
    f32 scale;

    person = peopleSearchID(peopleIsInGroup(other->key0, other->key1));
    if (person == NULL) {
        return 0;
    }
    otherPos = peopleGetPositionPtr(person);
    if (otherPos == NULL) {
        return 0;
    }
    if (otherPos->y >= pos->y + self->height ||
        otherPos->y + other->height <= pos->y) {
        return 0;
    }
    d.z = pos->z - otherPos->z;
    d.x = pos->x - otherPos->x;
    distance = GScolsys2HumanSqrt(d.x * d.x + d.z * d.z);
    radius = other->radius + self->radius;
    if (distance >= radius) {
        return 0;
    }
    if (out == NULL) {
        return 1;
    }
    if (distance <= 0.0f) {
        distance = d.x = 1.0f;
    }
    scale = (0.0001f + radius) / distance;
    d.x *= scale;
    d.z *= scale;
    out->x = d.x + otherPos->x;
    out->y = pos->y;
    out->z = d.z + otherPos->z;
    return 1;
}

/* 0x8010FAF4 | 0x304: XD checkCollision__FP15GSCOLSYS2_HUMANiP5GSvecP5GSvecP5GSvec. */
s32 GScolsys2CheckCollision(GSColFloorEvent* self, s32 excludedIndex,
                GScolsys2Vec3* start, GScolsys2Vec3* end,
                GScolsys2Vec3* result)
{
    GScolsys2Vec3* out;
    s32 i;
    GSColFloor* floor;
    s32 pass;
    GSColFloorEvent* other;
    GScolsys2Vec3 adjusted;
    GScolsys2Vec3 pos;

    pos = *end;
    out = NULL;
    if (result != NULL) {
        out = &adjusted;
    }
    floor = GScolsys2GetCurFloor();
    for (pass = 0; pass < 10; pass++) {
        other = floor->events;
        for (i = 0; i < 48; i++, other++) {
            if (i == excludedIndex) {
                continue;
            }
            if (!(other->flags & 1) || (other->flags & 2)) {
                continue;
            }
            if (checkCollisionHeadToHead(self, other, &pos, out)) {
                break;
            }
        }
        if (i >= 48) {
            break;
        }
        if (out == NULL) {
            return 1;
        }
        pos = *out;
    }
    if (pass <= 0) {
        return 0;
    }
    *result = pos;
    return 1;
}

/* Look up an in-use floor human slot. GScolsys2HumanCollision and
 * GScolsys2HumanEnable both expand this same status-code sequence. */
static inline s32 GScolsys2HumanGetEvent(s32 index, GSColFloorEvent** out)
{
    GSColFloor* floor;
    GSColFloorEvent* event;

    if (index < 0 || index >= 48) {
        return 4;
    }
    floor = GScolsys2GetCurFloor();
    if (floor == NULL) {
        return 1;
    }
    event = &floor->events[index];
    if ((event->flags & 1) == 0) {
        return 4;
    }
    *out = event;
    return 0;
}

/* 0x8010FDF8 | 0x1CC */
s32 GScolsys2HumanCollision(s32 index, GScolsys2Vec3* start,
                            GScolsys2Vec3* end, GScolsys2Vec3* result)
{
    extern f32 PSVECDistance(void*, void*);
    extern void PSVECSubtract(void*, void*, void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);
    GSColFloorEvent* event;
    GScolsys2Vec3 delta;
    GScolsys2Vec3 segmentStart;
    GScolsys2Vec3 segmentEnd;
    f32 distance;
    f32 step;
    f32 t;
    f32 next;
    s32 status;

    status = GScolsys2HumanGetEvent(index, &event);
    if (status != 0) {
        return status;
    }

    distance = PSVECDistance(start, end);
    if (distance > 0.0f) {
        step = event->radius / distance;
        if (step > 1.0f) {
            step = 1.0f;
        }
    } else {
        step = 0.0f;
    }
    PSVECSubtract(end, start, &delta);
    t = 0.0f;
    while (t < 1.0f) {
        next = t + step;
        if (next > 1.0f) {
            next = 1.0f;
        }
        PSVECScale(&delta, &segmentStart, t);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&delta, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (GScolsys2CheckCollision(event, index, &segmentStart, &segmentEnd,
                        result)) {
            return 6;
        }
        if (step <= 0.0f) {
            break;
        }
        t += step;
    }
    return 7;
}
