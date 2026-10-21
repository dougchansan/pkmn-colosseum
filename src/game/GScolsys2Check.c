/**
 * @file GScolsys2Check.c
 * @brief GScolsys2Check -- single dispatch entry point.
 *
 * Fourth of six translation units recovered from the former
 * game/gs_field_colquery.c CodeCandidate bucket (0x8010F6A0-0x801140DC).
 * Single anchor-bearing function; forwards to GScolsys2Thru's
 * GScolsys2ThruTest (see GScolsys2Thru_r56_80111864_o4s.c) once a minimum
 * distance threshold is exceeded.
 *
 * Address range: 0x80111B9C - 0x80111C24
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* 0x80111B9C | 0x88 */
s32 GScolsys2CheckGetEventID(void* arg0, void* arg1, void* arg2) {
    extern f32 PSVECDistance(void* a, void* b);
    extern f32 lbl_8047CF60;

    if (fn_8010CBC0() == 0) {
        return 0;
    }
    if (PSVECDistance(arg1, arg0) <= lbl_8047CF60) {
        return 0;
    }
    return GScolsys2ThruTest(arg0, arg1, arg2);
}
