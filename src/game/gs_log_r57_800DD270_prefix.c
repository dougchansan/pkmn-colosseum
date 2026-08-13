#include "dolphin/types.h"

extern void GSlogWrite(const char*, ...);
extern void fn_800DE09C(char*, u32, const char*, ...);
extern void fn_800DE128(char*, u32, const char*, void*);
extern void logVsnprintf_float(char*, u32, const char*, void*);
extern void fn_800E209C(u16);
extern void fn_800E24B0(u16);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);
extern u64 OSGetTime(void);
extern void OSTicksToCalendarTime(u64 ticks, void* td);
extern u32 strlen(const char*);

extern u32 lbl_8047AB10;
extern u32 lbl_8047AB11;
extern u32 lbl_8047AAF8;
extern u32 lbl_8047AAFA;
extern u32 lbl_8047AAFC;
extern u32 lbl_8047AB00;
extern u32 lbl_8047AB04;
extern u32 lbl_8047AB08;
extern u32 lbl_8047AB0C;

extern u8 lbl_802704A0[];
extern u8 lbl_802704B4[];
extern u8 lbl_80400F30[];
extern u8 lbl_80400F44[];
extern u8 lbl_80401044[];
extern u8 lbl_80401058[];

typedef struct GSLogVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} GSLogVaList;

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

u32 GSlogGetLine(u32 count) {
    u16* entries;
    u32 sum;
    u32 i;
    u32 ret;

    if (*(u16*)&lbl_8047AAF8 == 0) {
        return 0;
    }
    if (count > lbl_8047AB08) {
        return 0;
    }

    entries = fn_800E27B0(*(u16*)&lbl_8047AAFA);
    sum = 0;
    for (i = 0; i < count; i++) {
        sum += entries[i];
    }
    ret = lbl_8047AAFC + sum;
    fn_800E24B0(*(u16*)&lbl_8047AAFA);
    return ret;
}

u32 GSlogGetLineCount(void) { return lbl_8047AB08; }

void GSlogWritef(const char* fmt, ...) {
    GSLogVaList args;
    GSCalendarTime time;
    u16* entries;
    u8* buffer;
    char* message;
    u32 used;
    u32 i;
    u16 lineLength;

    if (*(u8*)&lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C((char*)lbl_80400F30, 0x14, (const char*)lbl_802704B4,
                    time.mon + 1, time.mday, time.hour, time.min, time.sec);
    }

    __builtin_va_info(&args);
    logVsnprintf_float((char*)lbl_80400F44, 0xFF, fmt, &args);
    lbl_80400F44[0xFF] = 0;

    if (lbl_8047AAFC == 0) {
        goto done;
    }

    if (*(u8*)&lbl_8047AB11 != 0) {
        lineLength =
            (u16)(strlen((const char*)lbl_80400F30) +
                  strlen((const char*)lbl_80400F44) + 1);
    } else {
        lineLength = (u16)(strlen((const char*)lbl_80400F44) + 1);
    }

    for (;;) {
        used = 0;
        entries = (u16*)lbl_8047AB00;
        for (i = 0; i < lbl_8047AB08; i++) {
            used += entries[i];
        }
        if (lbl_8047AB08 < lbl_8047AB0C && lineLength + used < lbl_8047AB04) {
            break;
        }
        if (lbl_8047AB08 == 0) {
            break;
        }

        {
            u32 removeLen = entries[0];
            u8* src = (u8*)lbl_8047AAFC + removeLen;
            u8* dst = (u8*)lbl_8047AAFC;
            u32 remaining = lbl_8047AB04 - removeLen;

            for (i = 0; i < remaining; i++) {
                dst[i] = src[i];
            }

            for (i = 1; i < lbl_8047AB08; i++) {
                entries[i - 1] = entries[i];
            }
            lbl_8047AB08--;
        }
    }

    used = 0;
    entries = (u16*)lbl_8047AB00;
    for (i = 0; i < lbl_8047AB08; i++) {
        used += entries[i];
    }

    buffer = (u8*)lbl_8047AAFC + used;
    if (*(u8*)&lbl_8047AB11 != 0) {
        u32 prefixLen = strlen((const char*)lbl_80400F30);
        for (i = 0; i < prefixLen; i++) {
            buffer[i] = lbl_80400F30[i];
        }
        buffer += prefixLen;
    }

    message = (char*)lbl_80400F44;
    for (i = 0;; i++) {
        buffer[i] = (u8)message[i];
        if (message[i] == '\0') {
            break;
        }
    }

    entries[lbl_8047AB08] = lineLength;
    lbl_8047AB08++;

done:
    if (*(u8*)&lbl_8047AB11 != 0) {
        strlen((const char*)lbl_80400F30);
    }
    strlen((const char*)lbl_80400F44);
}

void GSlogWrite(const char* fmt, ...) {
    GSLogVaList args;
    GSCalendarTime time;
    u16* entries;
    u8* buffer;
    char* message;
    u32 used;
    u32 i;
    u16 lineLength;

    if (*(u8*)&lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C((char*)lbl_80401044, 0x14, (const char*)lbl_802704B4,
                    time.mon + 1, time.mday, time.hour, time.min, time.sec);
    }

    __builtin_va_info(&args);
    fn_800DE128((char*)lbl_80401058, 0xFF, fmt, &args);
    lbl_80401058[0xFF] = 0;

    if (lbl_8047AAFC == 0) {
        goto done;
    }

    if (*(u8*)&lbl_8047AB11 != 0) {
        lineLength =
            (u16)(strlen((const char*)lbl_80401044) +
                  strlen((const char*)lbl_80401058) + 1);
    } else {
        lineLength = (u16)(strlen((const char*)lbl_80401058) + 1);
    }

    for (;;) {
        used = 0;
        entries = (u16*)lbl_8047AB00;
        for (i = 0; i < lbl_8047AB08; i++) {
            used += entries[i];
        }
        if (lbl_8047AB08 < lbl_8047AB0C && lineLength + used < lbl_8047AB04) {
            break;
        }
        if (lbl_8047AB08 == 0) {
            break;
        }

        {
            u32 removeLen = entries[0];
            u8* src = (u8*)lbl_8047AAFC + removeLen;
            u8* dst = (u8*)lbl_8047AAFC;
            u32 remaining = lbl_8047AB04 - removeLen;

            for (i = 0; i < remaining; i++) {
                dst[i] = src[i];
            }

            for (i = 1; i < lbl_8047AB08; i++) {
                entries[i - 1] = entries[i];
            }
            lbl_8047AB08--;
        }
    }

    used = 0;
    entries = (u16*)lbl_8047AB00;
    for (i = 0; i < lbl_8047AB08; i++) {
        used += entries[i];
    }

    buffer = (u8*)lbl_8047AAFC + used;
    if (*(u8*)&lbl_8047AB11 != 0) {
        u32 prefixLen = strlen((const char*)lbl_80401044);
        for (i = 0; i < prefixLen; i++) {
            buffer[i] = lbl_80401044[i];
        }
        buffer += prefixLen;
    }

    message = (char*)lbl_80401058;
    for (i = 0;; i++) {
        buffer[i] = (u8)message[i];
        if (message[i] == '\0') {
            break;
        }
    }

    entries[lbl_8047AB08] = lineLength;
    lbl_8047AB08++;

done:
    if (*(u8*)&lbl_8047AB11 != 0) {
        strlen((const char*)lbl_80401044);
    }
    strlen((const char*)lbl_80401058);
}

u32 GSlogInit(u32 size, u8 flag) {
    const char* strings;
    u32 n;
    u16 h0;
    u16 h1;
    u32 alloc_size;

    strings = (const char*)lbl_802704A0;
    n = size;
    *(u8*)&lbl_8047AB10 = 0;
    lbl_8047AB04 = n;
    *(u8*)&lbl_8047AB11 = flag;
    if (n == 0) {
        GSlogWrite(strings + 0x2c);
        return 1;
    }

    h0 = _toolentryAlloc__FUl(n);
    *(u16*)&lbl_8047AAF8 = h0;
    if (h0 == 0) {
        GSlogWrite(strings + 0x48);
        return 0;
    }

    lbl_8047AB0C = n >> 7;
    alloc_size = (n >> 6) & ~1;
    h1 = _toolentryAlloc__FUl(alloc_size);
    *(u16*)&lbl_8047AAFA = h1;
    if (h1 == 0) {
        fn_800E209C(*(u16*)&lbl_8047AAF8);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    lbl_8047AAFC = (u32)fn_800E27B0(*(u16*)&lbl_8047AAF8);
    if (lbl_8047AAFC == 0) {
        fn_800E209C(*(u16*)&lbl_8047AAF8);
        fn_800E209C(*(u16*)&lbl_8047AAFA);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    lbl_8047AB00 = (u32)fn_800E27B0(*(u16*)&lbl_8047AAFA);
    if (lbl_8047AB00 == 0) {
        fn_800E24B0(*(u16*)&lbl_8047AAF8);
        fn_800E209C(*(u16*)&lbl_8047AAF8);
        fn_800E209C(*(u16*)&lbl_8047AAFA);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    GSlogWrite(strings + 0x5c, lbl_8047AB04);
    return 1;
}
