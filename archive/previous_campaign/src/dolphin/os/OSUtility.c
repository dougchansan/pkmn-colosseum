#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"

/*
 * OSUtility.c - OS utility functions for time conversion and delays.
 *
 * Contains calendar time conversion and busy-wait utilities that sit
 * between OSTime.c and __init_cpp_exceptions.c in the link order.
 *
 * Matches: 0x800A27F8 - 0x800A2B9C
 *   fn_800A27FC (0x19C) - OSTicksToCalendarTime
 *   fn_800A2998 (0x204) - OSCalendarTimeToTicks or __OSTimeToSystemTime
 */

typedef struct OSCalendarTime {
    s32 sec;        /* 0-59 */
    s32 min;        /* 0-59 */
    s32 hour;       /* 0-23 */
    s32 mday;       /* 1-31 */
    s32 mon;        /* 0-11 */
    s32 year;       /* years since 2000 */
    s32 wday;       /* 0-6 (Sunday = 0) */
    s32 yday;       /* 0-365 */
    s32 msec;       /* milliseconds */
    s32 usec;       /* microseconds */
} OSCalendarTime;

/* Time base frequency: 40.5 MHz (Gekko bus clock / 4) */
#define OS_TIMER_CLOCK  40500000
#define OS_BUS_CLOCK    162000000

/* Seconds from 1970-01-01 to 2000-01-01 */
#define SECS_1970_TO_2000 946684800LL

/* Days per month tables */
static const s32 DaysInMonth[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const s32 DaysInMonthLeap[12] = {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int IsLeapYear(s32 year) {
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

/*
 * OSTicksToCalendarTime - Convert ticks to calendar time.
 * 0x800A27FC | size: 0x19C
 *
 * Converts an absolute tick count to a calendar time structure.
 */
void OSTicksToCalendarTime(s64 ticks, OSCalendarTime* cal) {
    s64 secs;
    s64 remain;
    s32 days;
    s32 year;
    s32 mon;
    const s32* dayTab;

    /* Convert ticks to seconds and microseconds */
    secs = ticks / OS_TIMER_CLOCK;
    remain = ticks - (secs * OS_TIMER_CLOCK);

    if (remain < 0) {
        secs--;
        remain += OS_TIMER_CLOCK;
    }

    cal->usec = (s32)((remain * 1000000) / OS_TIMER_CLOCK) % 1000;
    cal->msec = (s32)((remain * 1000) / OS_TIMER_CLOCK);
    cal->sec  = (s32)(secs % 60);
    secs /= 60;
    if (cal->sec < 0) {
        cal->sec += 60;
        secs--;
    }

    cal->min  = (s32)(secs % 60);
    secs /= 60;
    if (cal->min < 0) {
        cal->min += 60;
        secs--;
    }

    cal->hour = (s32)(secs % 24);
    days = (s32)(secs / 24);
    if (cal->hour < 0) {
        cal->hour += 24;
        days--;
    }

    /* Day of week: Jan 1, 2000 was a Saturday (6) */
    cal->wday = (days + 6) % 7;
    if (cal->wday < 0) {
        cal->wday += 7;
    }

    /* Calculate year */
    year = 2000;
    if (days >= 0) {
        while (1) {
            s32 ydays = IsLeapYear(year) ? 366 : 365;
            if (days < ydays) break;
            days -= ydays;
            year++;
        }
    } else {
        do {
            year--;
            days += IsLeapYear(year) ? 366 : 365;
        } while (days < 0);
    }

    cal->year = year;
    cal->yday = days;

    /* Calculate month */
    dayTab = IsLeapYear(year) ? DaysInMonthLeap : DaysInMonth;
    for (mon = 0; mon < 11; mon++) {
        if (days < dayTab[mon]) break;
        days -= dayTab[mon];
    }
    cal->mon = mon;
    cal->mday = days + 1;
}

/*
 * OSCalendarTimeToTicks - Convert calendar time to ticks.
 * 0x800A2998 | size: 0x204
 *
 * Converts a calendar time structure to an absolute tick count.
 */
s64 OSCalendarTimeToTicks(OSCalendarTime* cal) {
    s64 secs;
    s32 days;
    s32 year;
    s32 y;
    const s32* dayTab;
    s32 mon;

    /* Count days from year 2000 */
    days = 0;
    year = cal->year;

    if (year > 2000) {
        for (y = 2000; y < year; y++) {
            days += IsLeapYear(y) ? 366 : 365;
        }
    } else if (year < 2000) {
        for (y = year; y < 2000; y++) {
            days -= IsLeapYear(y) ? 366 : 365;
        }
    }

    /* Add months */
    dayTab = IsLeapYear(year) ? DaysInMonthLeap : DaysInMonth;
    for (mon = 0; mon < cal->mon; mon++) {
        days += dayTab[mon];
    }

    /* Add days */
    days += cal->mday - 1;

    /* Convert to seconds */
    secs = (s64)days * 86400;
    secs += (s64)cal->hour * 3600;
    secs += (s64)cal->min * 60;
    secs += (s64)cal->sec;

    /* Convert to ticks */
    return secs * OS_TIMER_CLOCK + ((s64)cal->msec * OS_TIMER_CLOCK / 1000);
}
