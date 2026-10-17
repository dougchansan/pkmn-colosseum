/**
 * GSlog: in-memory text log, its snprintf wrapper and both formatters
 * (.text 0x800DD270-0x800DEFC8), the whole retail translation unit, with
 * its data:
 *   .rodata 0x802704A0-0x80270528  "0123456789ABCDEF" (lbl_80478AE8's
 *                                  target, the digit table),
 *                                  the timestamp format, GSlogInit messages
 *   .data   0x80315388-0x80315490  the two formatters' switch tables
 *   .bss    0x80400F30-0x804011B8  the buffers (function-local statics)
 *   .sdata  0x80478AE8-0x80478AF0  lbl_80478AE8, the digit table pointer
 *   .sbss   0x8047AAF8-0x8047AB18  the log state
 *   .sdata2 0x8047CAA0-0x8047CAC8  "(float)", "(null)", the %f constants
 *
 * Source attribution: the XD path GSAPI/GSlogM/GSlog.cpp and the XD helper
 * names (logFloat2Str, logHex2Str, logInt2Str, logStr2Int, logStrRev) are
 * inherited from commit e0e2b44b, which read TeamOrre/xd-decomp's
 * splits.txt. Verified by lane B23 against TeamOrre/xd-decomp
 * config/GXXE01/symbols.txt (commit 4989794e): logVsnprintf_float
 * 0x802A66F0, logFloat2Str__FfPcPc 0x802A6A78, logHex2Str__FUlPcb
 * 0x802A6CC4, logInt2Str__FiPc 0x802A6D34, logStr2Int__FPc 0x802A6DD0,
 * logStrRev__FPc 0x802A6E18 (XD keeps them out of line; the mangled names
 * confirm a C++ TU). XD's logFloat2Str (trevor403/xd-asm b1087f18,
 * func_FUN_802a6a78.s) has this file's statement order: sign into buf,
 * whole = (s32)value kept across logInt2Str(whole, buf), value -= whole,
 * the 0.1f/10.0f zero count, a cursor from fraction filled with '0' and
 * passed to the second logInt2Str, strlen(fraction), then the pad cursor
 * fraction + length.
 *
 * TU evidence (C++, statics in declaration order): logVsnprintf_float
 * addresses its three buffers from the TU's .bss base, lbl_80400F30 +
 * 0x258/0x268/0x278 = fraction, field, spec in declaration order. Built as
 * one C++ file, every static a function-local static and no extra flags,
 * MWCC reproduces the whole retail .bss block (GSlogWritef's and
 * GSlogWrite's timestamp/message buffers, then fraction/field/spec for each
 * formatter, including GSlogFormat's fraction buffer at 0x80401158, which
 * it declares but never uses), the .rodata pool, .sdata, .sbss, .sdata2 and
 * both switch tables at their retail offsets. The C front end instead
 * orders statics by first reference and the .sbss state in reverse. The
 * .bss symbols in symbols.txt carry MWCC's local-static names so the report
 * pairs them.
 *
 * Log layout: a text buffer of lbl_8047AB04 bytes and a table of u16 line
 * lengths (one entry per 128 buffer bytes). Lines are stored back to back
 * with their trailing NUL; when a new line does not fit, the oldest lines are
 * dropped from the front.
 */
#include "dolphin/types.h"
#include "game/gs_log_format.h"

typedef struct GSCalendarTime {
    s32 sec;
    s32 min;
    s32 hour;
    s32 mday;
    s32 mon;
    s32 year;
    s32 wday;
    s32 yday;
    s32 msec;
    s32 usec;
} GSCalendarTime;

extern "C" {
u64 OSGetTime(void);
void OSTicksToCalendarTime(u64 ticks, GSCalendarTime* td);
void* memcpy(void* dst, const void* src, u32 n);

void GSmemFree(u16 handle);  /* GSmem: free handle */
void fn_800E24B0(u16 handle);  /* GSmem: unlock handle */
void* GSmemLock(u16 handle); /* GSmem: lock handle, return pointer */

u32 GSlogGetLine(u32 count);
u32 GSlogGetLineCount(void);
void GSlogWritef(const char* fmt, ...);
void GSlogWrite(const char* fmt, ...);
u32 GSlogInit(u32 size, u8 timestamps);
void GSlogVPrintf(char* dst, u32 size, const char* format, ...);
void GSlogFormat(char* output, u32 capacity, const char* format, va_list args);
void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args);
char* strchr(const char* s, int c);
}

u16 _toolentryAlloc(u32 size); /* GSmem: allocate a handle */

char* lbl_80478AE8 = "0123456789ABCDEF";

static u16 lbl_8047AAF8;   /* text buffer handle */
static u16 lbl_8047AAFA;   /* line length table handle */
static u8* lbl_8047AAFC;   /* text buffer (locked) */
static u16* lbl_8047AB00;  /* line length table (locked) */
static u32 lbl_8047AB04;   /* text buffer size */
static u32 lbl_8047AB08;   /* line count */
static u32 lbl_8047AB0C;   /* line capacity */
static u8 lbl_8047AB10;    /* cleared by GSlogInit, never read */
static u8 lbl_8047AB11;    /* prefix lines with a timestamp */

u32 GSlogGetLine(u32 count) {
    u16* entries;
    u32 sum;
    u32 i;
    u32 ret;

    if (lbl_8047AAF8 == 0) {
        return 0;
    }
    if (count > lbl_8047AB08) {
        return 0;
    }

    entries = (u16*)GSmemLock(lbl_8047AAFA);
    sum = 0;
    for (i = 0; i < count; i++) {
        sum += entries[i];
    }
    ret = (u32)lbl_8047AAFC + sum;
    fn_800E24B0(lbl_8047AAFA);
    return ret;
}

u32 GSlogGetLineCount(void) { return lbl_8047AB08; }

/* Forward copy that moves whole words when source and destination are a word
 * or more apart and bytes otherwise. The distance test is transcribed from
 * the retail code, which recomputes the difference and selects it when it is
 * non-zero (an ABS-style macro expansion); both loggers expand it the same
 * way, and so does the GSmem compactor at 0x800E1544. */
static inline void GSlogCopyForward(void* dst, const void* src, u32 size) {
    if ((u32)(((s32)src - (s32)dst) != 0 ? ((s32)src - (s32)dst)
                                         : -((s32)src - (s32)dst)) >= 4) {
        u32* wordDst = (u32*)dst;
        const u32* wordSrc = (const u32*)src;
        u32 words = size >> 2;
        u32 rest = size & 3;
        u8* byteDst;
        const u8* byteSrc;

        while (words-- != 0) {
            *wordDst++ = *wordSrc++;
        }
        byteDst = (u8*)wordDst;
        byteSrc = (const u8*)wordSrc;
        while (rest-- != 0) {
            *byteDst++ = *byteSrc++;
        }
    } else {
        u8* byteDst = (u8*)dst;
        const u8* byteSrc = (const u8*)src;

        while (size-- != 0) {
            *byteDst++ = *byteSrc++;
        }
    }
}

/* Bytes currently held by the stored lines. */
static inline u32 GSlogUsedBytes(void) {
    u16* entries = lbl_8047AB00;
    u32 used = 0;
    u32 i;

    for (i = 0; i < lbl_8047AB08; i++) {
        used += entries[i];
    }
    return used;
}

/* Remove the oldest line: slide the text and the length table down. */
static inline void GSlogDropOldestLine(void) {
    u16* entries = lbl_8047AB00;
    u32 i;
    u16* dst;
    u16* src;

    GSlogCopyForward(lbl_8047AAFC, lbl_8047AAFC + entries[0], lbl_8047AB04 - entries[0]);
    dst = lbl_8047AB00;
    src = dst + 1;
    for (i = lbl_8047AB08 - 1; i != 0; i--) {
        *dst++ = *src++;
    }
    lbl_8047AB08--;
}

static inline u8 GSlogHasRoom(u16 lineLength) {
    u8 fits = TRUE;

    if (lbl_8047AB08 >= lbl_8047AB0C) {
        fits = FALSE;
    }
    if (lineLength + GSlogUsedBytes() >= lbl_8047AB04) {
        fits = FALSE;
    }
    return fits;
}

void GSlogWritef(const char* fmt, ...) {
    static char lbl_80400F30[0x14];  /* timestamp */
    static char lbl_80400F44[0x100]; /* message */
    va_list args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    va_start(args, fmt);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        GSlogVPrintf(lbl_80400F30, 0x14, "[%d/%02d %d:%02d:%02d] ", time.mon + 1,
                    time.mday, time.hour, time.min, time.sec);
    }

    logVsnprintf_float(lbl_80400F44, 0xFF, fmt, args);
    lbl_80400F44[0xFF] = 0;

    if (lbl_8047AAFC != NULL) {
        if (lbl_8047AB11 != 0) {
            lineLength = strlen(lbl_80400F30) + strlen(lbl_80400F44) + 1;
        } else {
            lineLength = strlen(lbl_80400F44) + 1;
        }

        do {
            fits = GSlogHasRoom(lineLength);
            if (!fits) {
                GSlogDropOldestLine();
            }
        } while (!fits);

        buffer = lbl_8047AAFC + GSlogUsedBytes();
        if (lbl_8047AB11 != 0) {
            memcpy(buffer, lbl_80400F30, strlen(lbl_80400F30));
            buffer += strlen(lbl_80400F30);
        }
        memcpy(buffer, lbl_80400F44, strlen(lbl_80400F44) + 1);
        lbl_8047AB00[lbl_8047AB08++] = lineLength;
    }

    /* Retail evaluates both lengths again and discards them (the remains of
     * a compiled-out echo, most likely). */
    if (lbl_8047AB11 != 0) {
        strlen(lbl_80400F30);
    }
    strlen(lbl_80400F44);
}

void GSlogWrite(const char* fmt, ...) {
    static char lbl_80401044[0x14];  /* timestamp */
    static char lbl_80401058[0x100]; /* message */
    va_list args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    va_start(args, fmt);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        GSlogVPrintf(lbl_80401044, 0x14, "[%d/%02d %d:%02d:%02d] ", time.mon + 1,
                    time.mday, time.hour, time.min, time.sec);
    }

    GSlogFormat(lbl_80401058, 0xFF, fmt, args);
    lbl_80401058[0xFF] = 0;

    if (lbl_8047AAFC != NULL) {
        if (lbl_8047AB11 != 0) {
            lineLength = strlen(lbl_80401044) + strlen(lbl_80401058) + 1;
        } else {
            lineLength = strlen(lbl_80401058) + 1;
        }

        do {
            fits = GSlogHasRoom(lineLength);
            if (!fits) {
                GSlogDropOldestLine();
            }
        } while (!fits);

        buffer = lbl_8047AAFC + GSlogUsedBytes();
        if (lbl_8047AB11 != 0) {
            memcpy(buffer, lbl_80401044, strlen(lbl_80401044));
            buffer += strlen(lbl_80401044);
        }
        memcpy(buffer, lbl_80401058, strlen(lbl_80401058) + 1);
        lbl_8047AB00[lbl_8047AB08++] = lineLength;
    }

    if (lbl_8047AB11 != 0) {
        strlen(lbl_80401044);
    }
    strlen(lbl_80401058);
}

/* Allocates a text buffer of `size` bytes plus one u16 length slot per 128
 * bytes. Returns 1 on success (or when logging is disabled with size 0). */
u32 GSlogInit(u32 size, u8 timestamps) {
    lbl_8047AB10 = 0;
    lbl_8047AB04 = size;
    lbl_8047AB11 = timestamps;
    if (size == 0) {
        GSlogWrite("GSlog: Init OK, no buffer\n");
        return 1;
    }

    lbl_8047AAF8 = _toolentryAlloc(size);
    if (lbl_8047AAF8 == 0) {
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AB0C = size >> 7;
    lbl_8047AAFA = _toolentryAlloc((size >> 6) & ~1);
    if (lbl_8047AAFA == 0) {
        GSmemFree(lbl_8047AAF8);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AAFC = (u8*)GSmemLock(lbl_8047AAF8);
    if (lbl_8047AAFC == NULL) {
        GSmemFree(lbl_8047AAF8);
        GSmemFree(lbl_8047AAFA);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AB00 = (u16*)GSmemLock(lbl_8047AAFA);
    if (lbl_8047AB00 == NULL) {
        fn_800E24B0(lbl_8047AAF8);
        GSmemFree(lbl_8047AAF8);
        GSmemFree(lbl_8047AAFA);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    GSlogWrite("GSlog: Init OK, log buffer size %d bytes\n", lbl_8047AB04);
    return 1;
}

void GSlogVPrintf(char* dst, u32 size, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    GSlogFormat(dst, size, format, args);
}

void GSlogFormat(char* output, u32 capacity, const char* format, va_list args)
{
    /* The same three buffers as logVsnprintf_float; this formatter has no
     * %f conversion, so it never uses its fraction buffer. */
    static char lbl_80401158[16];    /* fraction (unused) */
    static char lbl_80401168[16];    /* converted field */
    static char lbl_80401178[16];    /* conversion specifier */
    const char* src = format;
    char* dst = output;
    char* token;
    u8 done = 0;
    u8 parsing = 0;
    u8 leftJustify = 0;
    u8 zeroPad = 0;
    u8 ready = 0;
    char* text;
    s32 width;
    u32 hex;
    s32 number;

    while (!done) {
        if (!parsing) {
            if (*src == '%') {
                if (src[1] == '%') {
                    *dst++ = '%';
                    src++;
                } else {
                    token = lbl_80401178;
                    parsing = 1;
                }
            } else {
                *dst++ = *src;
            }
        } else {
            switch (*src) {
            case 'c':
                text = lbl_80401168;
                lbl_80401168[0] = va_arg(args, int);
                lbl_80401168[1] = 0;
                ready = 1;
                break;
            case 'd':
                number = va_arg(args, int);
                logInt2Str(number, lbl_80401168);
                text = lbl_80401168;
                ready = 1;
                break;
            case 'f':
                va_arg(args, double);
                text = "(float)";
                ready = 1;
                break;
            case 's':
                text = va_arg(args, char*);
                if (text == NULL) {
                    text = "(null)";
                }
                ready = 1;
                break;
            case 'X':
            case 'x':
                hex = va_arg(args, u32);
                if (*src == 'X') {
                    logHex2Str(hex, lbl_80401168, TRUE);
                } else {
                    logHex2Str(hex, lbl_80401168, FALSE);
                }
                text = lbl_80401168;
                ready = 1;
                break;
            default:
                *token++ = *src;
                break;
            }

            if (ready == 1) {
                s32 length;

                *token = 0;
                token = lbl_80401178;
                if (*token == '-') {
                    leftJustify = 1;
                    token++;
                }
                if (*token == '0') {
                    zeroPad = 1;
                    token++;
                }
                width = logStr2Int(token);
                length = strlen(text);
                if (width > length) {
                    width -= length;
                    if (!leftJustify) {
                        while (width-- != 0 && (u32)(dst - output) < capacity) {
                            if (zeroPad) {
                                *dst++ = '0';
                            } else {
                                *dst++ = ' ';
                            }
                        }
                    }
                }
                while (*text != 0 && (u32)(dst - output) < capacity) {
                    *dst++ = *text++;
                }
                if (leftJustify == 1) {
                    while (width-- != 0 && (u32)(dst - output) < capacity) {
                        *dst++ = ' ';
                    }
                }
                leftJustify = 0;
                zeroPad = 0;
                parsing = 0;
                ready = 0;
            }
        }

        if (*src == 0 || (u32)(dst - output) >= capacity) {
            done = 1;
        }
        src++;
    }
    *dst = 0;
}

/*
 * Writes the sign and integer part to buf, and to fraction the fractional
 * part as up to ten leading zeros followed by (s32)(fraction * 1e9), padded
 * with '0' to at least ten digits.
 *
 * The fraction half reuses the integer half's variables: buf becomes the
 * cursor into fraction, and whole takes the scaled fraction digits, so both
 * halves read `whole = value; logInt2Str(whole, buf);`. That is what gives
 * retail's allocation (lane B23b, from MWCC's own allocator dumps via
 * tools/local_campaign_colouring.py): the reused buf is split into a new
 * web, numbered below the second logInt2Str's locals, so it absorbs
 * logStrRev's start and is simplified before `whole`, taking r18; and the
 * reused whole is one more coalesced copy, which keeps output, capacity and
 * args last in the simplify order (r29-r31). With a separate cursor `p` the
 * cursor was coloured r27 and the second expansion took the first one's
 * volatile registers; with only one of the two reuses the float code
 * matches but the three parameters move to r19-r21.
 */
static inline void logFloat2Str(f32 value, char* buf, char* fraction)
{
    s32 whole;
    s32 zeros;
    f32 scale;
    char* p;
    s32 length;

    if (value < 0.0f) {
        *buf++ = '-';
        value = -value;
    }
    whole = value;
    logInt2Str(whole, buf);
    value -= whole;
    scale = 0.1f;
    for (zeros = 0; zeros < 10; zeros++) {
        if (scale < value) {
            break;
        }
        scale /= 10.0f;
    }
    buf = fraction;
    while (zeros-- != 0) {
        *buf++ = '0';
    }
    value *= 1000000000.0f;
    whole = value;
    logInt2Str(whole, buf);
    length = strlen(fraction);
    p = fraction + length;
    while (length < 10) {
        *p++ = '0';
        length++;
    }
    *p = 0;
}

void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args)
{
    static char fraction[16]; /* 0x80401188 */
    static char field[16];    /* 0x80401198 */
    static char spec[16];     /* 0x804011A8 */
    const char* src = format;
    char* dst = output;
    char* token;
    u8 done = 0;
    u8 parsing = 0;
    u8 ready = 0;
    u8 leftJustify = 0;
    u8 zeroPad = 0;
    char* text;
    s32 width;
    u32 hex;
    s32 number;
    f32 value;

    while (!done) {
        if (!parsing) {
            if (*src == '%') {
                if (src[1] == '%') {
                    *dst++ = '%';
                    src++;
                } else {
                    token = spec;
                    parsing = 1;
                }
            } else {
                *dst++ = *src;
            }
        } else {
            switch (*src) {
            case 'c':
                text = field;
                field[0] = va_arg(args, int);
                field[1] = 0;
                ready = 1;
                break;
            case 'd':
                number = va_arg(args, int);
                logInt2Str(number, field);
                text = field;
                ready = 1;
                break;
            case 'f':
                value = va_arg(args, double);
                logFloat2Str(value, field, fraction);
                text = field;
                ready = 1;
                break;
            case 's':
                text = va_arg(args, char*);
                ready = 1;
                break;
            case 'X':
            case 'x':
                hex = va_arg(args, u32);
                if (*src == 'X') {
                    logHex2Str(hex, field, TRUE);
                } else {
                    logHex2Str(hex, field, FALSE);
                }
                text = field;
                ready = 1;
                break;
            default:
                *token++ = *src;
                break;
            }

            if (ready == 1) {
                s32 length;

                *token = 0;
                token = spec;
                if (*token == '-') {
                    leftJustify = 1;
                    token++;
                }
                if (*token == '0') {
                    zeroPad = 1;
                    token++;
                }
                width = logStr2Int(token);
                length = strlen(text);
                if (width > length) {
                    width -= length;
                    if (!leftJustify) {
                        while (width-- != 0 && (u32)(dst - output) < capacity) {
                            if (zeroPad) {
                                *dst++ = '0';
                            } else {
                                *dst++ = ' ';
                            }
                        }
                    }
                }
                while (*text != 0 && (u32)(dst - output) < capacity) {
                    *dst++ = *text++;
                }
                if (*src == 'f') {
                    const char* dot;

                    text = fraction;
                    length = strlen(fraction);
                    dot = strchr(token, '.');
                    if (dot != NULL) {
                        width = logStr2Int(dot + 1);
                        if (width > 0 && length > width) {
                            text[width] = 0;
                        }
                    }
                    *dst++ = '.';
                    while (*text != 0 && (u32)(dst - output) < capacity) {
                        *dst++ = *text++;
                    }
                }
                if (leftJustify == 1) {
                    while (width-- != 0 && (u32)(dst - output) < capacity) {
                        *dst++ = ' ';
                    }
                }
                leftJustify = 0;
                zeroPad = 0;
                parsing = 0;
                ready = 0;
            }
        }

        if (*src == 0 || (u32)(dst - output) >= capacity) {
            done = 1;
        }
        src++;
    }
    *dst = 0;
}
