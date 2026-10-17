/**
 * @file GScolsys2Human_exact_8010FFC4.c
 * @brief GScolsys2Human floor-slot enable and registration,
 *        0x8010FFC4 - 0x801101B4.
 *
 * A function-boundary carve of the GScolsys2Human TU. That TU owns the
 * .sdata2 pool 0x8047CF20-0x8047CF48 (GScolsys2CheckCollision and
 * GScolsys2HumanCollision, which live in GScolsys2Human_range_8010FAF4.c)
 * and manages the 48 human-collision slots of the current floor. Its last
 * function is fn_80110084, which registers a slot and uses the same
 * status codes as GScolsys2HumanEnable (1 no floor, 4 bad slot, 5 full);
 * the GScolsys2Thru triangle queries start at fn_801101B4. Neither
 * function here touches data, and both are exact on the project default
 * flags (GC/1.3) with no local pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern GSColFloor* GScolsys2GetCurFloor(void);

/* Look up an in-use floor event; shared by GScolsys2HumanCollision and
 * GScolsys2HumanEnable (the same status-code sequence is expanded in
 * both). */
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

/* 0x8010FFC4 | 0xC0 */
s32 GScolsys2HumanEnable(s32 index, s32 enable)
{
    GSColFloorEvent* event;
    s32 result;

    result = GScolsys2HumanGetEvent(index, &event);
    if (result != 0) {
        return result;
    }
    if (enable != 0) {
        event->flags &= ~2;
    } else {
        event->flags |= 2;
    }
    return 0;
}

/* 0x80110084 | 0x130 */
s32 fn_80110084(s32* outIndex, GSColFloorEvent* src)
{
    GSColFloor* floor;
    s32 i;

    floor = GScolsys2GetCurFloor();
    if (floor == NULL) {
        return 1;
    }
    for (i = 0; i < 48; i++) {
        if ((floor->events[i].flags & 1) == 0) {
            break;
        }
    }
    if (i >= 48) {
        return 5;
    }

    floor->events[i].key0 = src->key0;
    floor->events[i].key1 = src->key1;
    floor->events[i].radius = src->radius;
    floor->events[i].height = src->height;
    floor->events[i].flags = 0;
    floor->events[i].flags |= 1;
    *outIndex = i;
    return 0;
}
