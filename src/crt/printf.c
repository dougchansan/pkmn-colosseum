#include "dolphin/types.h"

typedef struct __FILE __FILE;

/* va_list for PPC */
typedef struct __va_list_struct {
    u8  gpr;                /* 0x00: next GPR index (3-10) */
    u8  fpr;                /* 0x01: next FPR index (1-8) */
    u16 padding;            /* 0x02 */
    u32* overflow_arg_area; /* 0x04: pointer to stack args */
    u32* reg_save_area;     /* 0x08: pointer to saved regs */
} __va_list_struct;

typedef __va_list_struct va_list[1];

typedef s32 (*WriteFunc)(void* data, s32 count, __FILE* file);

extern u8 __files[];
extern void __begin_critical_region(s32 region);
extern void __end_critical_region(s32 region);
extern s32 fwide(__FILE* file, s32 mode);
typedef void* (*WriteProcT)(void*, const char*, u32);
int __pformatter_800C88BC(WriteProcT WriteProc, void* WriteProcArg,
                          const char* format_str, va_list arg);
void* __FileWrite(void* pFile, const char* pBuffer, u32 char_num);

s32 vsprintf(char* buf, const char* fmt, va_list args);

#ifndef PRINTF_RESIDUAL_ONLY
/*
 * sprintf - 0x800C8520 | size: 0xE0
 *
 * This MSL revision implements sprintf and vsprintf as separate entry points
 * with the same output-controller body; the target contains no call between
 * them.
 */
s32 sprintf(char* buf, const char* fmt, ...) {
    extern s32 __StringWrite(void* data, s32 count, __FILE* file);
    va_list args;
    struct { char* p; s32 a; s32 b; } sf;
    s32 n;

    __builtin_va_info(&args);
    sf.p = buf;
    sf.a = -1;
    sf.b = 0;
    n = __pformatter_800C88BC((WriteProcT)__StringWrite, &sf, fmt, args);
    if (buf != NULL) {
        s32 idx = -2;
        if ((u32)n < 0xFFFFFFFFU)
            idx = n;
        buf[idx] = 0;
    }
    return n;
}

/* vsprintf - 0x800C8600 | size: 0x78 */
s32 vsprintf(char* buf, const char* fmt, va_list args) {
    extern s32 __StringWrite(void* data, s32 count, __FILE* file);
    struct { char* p; s32 a; s32 b; } sf;
    s32 n;

    sf.p = buf;
    sf.a = -1;
    sf.b = 0;
    n = __pformatter_800C88BC((WriteProcT)__StringWrite, &sf, fmt, args);
    if (buf != NULL) {
        s32 idx = -2;
        if ((u32)n < 0xFFFFFFFFU)
            idx = n;
        buf[idx] = 0;
    }
    return n;
}

/* vprintf - 0x800C8678 | size: 0x98 */
s32 vprintf(const char* fmt, va_list args) {
    __FILE* stdout_file = (__FILE*)(__files + 0x50);
    s32 result;

    if (fwide(stdout_file, -1) >= 0) {
        return -1;
    }

    __begin_critical_region(2);
    result = __pformatter_800C88BC((WriteProcT)__FileWrite, stdout_file, fmt, args);
    __end_critical_region(2);

    return result;
}

/* printf - 0x800C8710 | size: 0xE8 */
s32 printf(const char* fmt, ...) {
    __FILE* stdout_file = (__FILE*)(__files + 0x50);
    va_list args;
    s32 result;

    if (fwide(stdout_file, -1) >= 0) {
        return -1;
    }

    __begin_critical_region(2);
    __builtin_va_info(&args);
    result = __pformatter_800C88BC((WriteProcT)__FileWrite, stdout_file, fmt, args);
    __end_critical_region(2);

    return result;
}
#endif
