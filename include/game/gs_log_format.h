#ifndef GAME_GS_LOG_FORMAT_H
#define GAME_GS_LOG_FORMAT_H

/*
 * Shared pieces of the GSlog formatters (GSlogFormat and
 * logVsnprintf_float in gs_log.cpp): the MWCC va_list, and the
 * helpers logStrRev, logInt2Str, logHex2Str and logStr2Int (names inherited
 * from commit e0e2b44b's reading of TeamOrre/xd-decomp, not verified here;
 * the static inline form is justified by the expansions). Every one of
 * them is expanded several times in the retail formatters (logStrRev in every
 * integer conversion, logHex2Str once per case letter, logInt2Str for %d and
 * twice in %f, logStr2Int for the width and the %f precision); none survives
 * as a symbol.
 */

#include "dolphin/types.h"

typedef struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];
typedef __va_list va_list;

extern "C" {
void* __va_arg(void* ap, int type);
u32 strlen(const char* s);

/* "0123456789ABCDEF", the digit table of the integer conversions. */
extern char* lbl_80478AE8;
}

#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_arg(ap, t) (*((t*)__va_arg(ap, _var_arg_typeof(t))))

static inline void logStrRev(char* start)
{
    char* end = &start[strlen(start) - 1];

    while (start < end) {
        *start ^= *end;
        *end ^= *start;
        *start++ ^= *end--;
    }
}

static inline void logInt2Str(s32 value, char* buf)
{
    char* p = buf;
    u8 negative = 0;

    if (value < 0) {
        negative = 1;
        value = -value;
    }
    do {
        *p++ = lbl_80478AE8[value % 10];
        value /= 10;
    } while (value != 0);
    if (negative == 1) {
        *p++ = '-';
    }
    *p = 0;
    logStrRev(buf);
}

static inline void logHex2Str(u32 value, char* buf, BOOL upper)
{
    char* p = buf;

    if (upper) {
        do {
            *p++ = lbl_80478AE8[value & 0xF];
            value >>= 4;
        } while (value != 0);
    } else {
        do {
            *p = lbl_80478AE8[value & 0xF];
            if (*p >= 'A') {
                *p += 'a' - 'A';
            }
            value >>= 4;
            p++;
        } while (value != 0);
    }
    *p = 0;
    logStrRev(buf);
}

static inline s32 logStr2Int(const char* s)
{
    s32 value = 0;

    while (*s != 0) {
        if (*s < '0' || *s > '9') {
            break;
        }
        value *= 10;
        value += *s++ - '0';
    }
    return value;
}

#endif /* GAME_GS_LOG_FORMAT_H */
