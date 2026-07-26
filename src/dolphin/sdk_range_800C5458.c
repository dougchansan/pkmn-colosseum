/**
 * @file sdk_range_800C5458.c
 * @brief dolphin-sdk code, 0x800C5458 - 0x800C754C (7 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef struct _MSL_FILE {
    /* 0x00 */ u32 handle;
    /* 0x04 */ u16 file_mode_open : 2;
    /* 0x04 */ u16 file_mode_io : 3;
    /* 0x04 */ u16 file_mode_buffer : 2;
    /* 0x04 */ u16 file_kind : 3;
    /* 0x04 */ u16 file_orientation : 2;
    /* 0x04 */ u16 binary_io : 1;
    /* 0x06 */ u8 file_mode_unused;
    /* 0x08 */ u32 file_state;
    /* 0x0C */ u8 is_dynamically_allocated;
    /* 0x0D */ char pad0d;
    /* 0x0E */ char pad0e;
    /* 0x0F */ char pad0f;
    /* 0x10 */ char ungetc_buffer[2];
    /* 0x12 */ u16 ungetc_wide_buffer[2];
    /* 0x18 */ u32 position;
    /* 0x1C */ u8* buffer;
    /* 0x20 */ u32 buffer_size;
    /* 0x24 */ u8* buffer_ptr;
    /* 0x28 */ u32 buffer_length;
    /* 0x2C */ u32 buffer_alignment;
    /* 0x30 */ u32 save_buffer_length;
    /* 0x34 */ u32 buffer_position;
    /* 0x38 */ void* position_fn;
    /* 0x3C */ void* read_fn;
    /* 0x40 */ void* write_fn;
    /* 0x44 */ void* close_fn;
    /* 0x48 */ void* idle_fn;
    /* 0x4C */ struct _MSL_FILE* next_file;
} MSL_FILE;

void __close_all(void) {
    extern MSL_FILE __files;
    extern void __begin_critical_region(int lock);
    extern void __end_critical_region(int lock);
    extern int fclose(void* stream);
    extern void free(void* ptr);

    MSL_FILE* fp = &__files;

    __begin_critical_region(2);
    while (fp != 0) {
        MSL_FILE* cur;

        if (fp->file_kind != 0) {
            fclose(fp);
        }

        cur = fp;
        fp = cur->next_file;
        if (cur->is_dynamically_allocated) {
            free(cur);
        } else {
            cur->file_kind = 3;
            if (fp != 0 && fp->is_dynamically_allocated) {
                cur->next_file = 0;
            }
        }
    }
    __end_critical_region(2);
}

/* ------------------------------------------------------------------ */
/* MSL ansi_fp.c: binary double <-> decimal record conversion.         */
/* ------------------------------------------------------------------ */

#define SIGDIGLEN 36
#define DBL_MANT_DIG 53

typedef struct decimal {
    /* 0x00 */ char sign;
    /* 0x01 */ char unk1;
    /* 0x02 */ short exp;
    struct {
        /* 0x04 */ unsigned char length;
        /* 0x05 */ unsigned char text[SIGDIGLEN];
        /* 0x29 */ unsigned char unk41;
    } sig;
} decimal;

typedef struct decform {
    /* 0x00 */ char style;
    /* 0x01 */ char unk1;
    /* 0x02 */ short digits;
} decform;

static const char* const unused =
    "179769313486231580793729011405303420";

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

#define __HI(x) (*(u32*)&x)
#define __LO(x) (*(1 + (u32*)&x))

inline int __fpclassifyd(f64 x) {
    switch (__HI(x) & 0x7FF00000) {
    case 0x7FF00000:
        if ((__HI(x) & 0x000FFFFF) || (__LO(x) & 0xFFFFFFFF)) {
            return FP_NAN;
        } else {
            return FP_INFINITE;
        }
        break;
    case 0:
        if ((__HI(x) & 0x000FFFFF) || (__LO(x) & 0xFFFFFFFF)) {
            return FP_SUBNORMAL;
        } else {
            return FP_ZERO;
        }
        break;
    }
    return FP_NORMAL;
}

#define isnan(x) (__fpclassifyd(x) == FP_NAN)
#define isfinite(x) (__fpclassifyd(x) > FP_INFINITE)

extern f64 frexp(f64 x, int* exp);
extern f64 ldexp(f64 x, int exp);
extern f64 modf(f64 x, f64* ip);

void __two_exp_800DCCB8(decimal* result, long exp);
void __num2dec_internal(decimal* d, f64 x);
static void __timesdec(decimal* result, const decimal* x, const decimal* y);

inline int __count_trailing_zerol(unsigned long x) {
    int result = 0;
    int bits_not_checked = sizeof(unsigned long) * 8;
    int n = bits_not_checked / 2;
    int mask_size = n;
    unsigned long mask = (~0UL) >> (bits_not_checked - n);

    while (bits_not_checked) {
        if (!(x & mask)) {
            result += mask_size;
            x >>= mask_size;
            bits_not_checked -= mask_size;
        } else if (mask == 1) {
            break;
        }
        if (n > 1) {
            n /= 2;
        }
        if (mask > 1) {
            mask >>= n;
            mask_size -= n;
        }
    }
    return result;
}

inline int __count_trailing_zero(f64 x) {
    unsigned long* l = (unsigned long*)&x;

    if (l[1] != 0) {
        return __count_trailing_zerol(l[1]);
    }
    return (int)(sizeof(unsigned long) * 8 + __count_trailing_zerol(l[0] | 0x00100000));
}

inline int __must_round(const decimal* d, int pos) {
    unsigned char const* i = d->sig.text + pos;

    if (*i > 5) {
        return 1;
    }
    if (*i < 5) {
        return -1;
    }
    {
        unsigned char const* e = d->sig.text + d->sig.length;
        for (++i; i < e; ++i) {
            if (*i != 0) {
                return 1;
            }
        }
    }
    if (d->sig.text[pos - 1] & 1) {
        return 1;
    }
    return -1;
}

inline void __dorounddecup(decimal* d, int digits) {
    unsigned char* b = d->sig.text;
    unsigned char* i = b + digits - 1;

    while (1) {
        if (*i < 9) {
            (*i)++;
            break;
        }
        if (i == b) {
            *i = 1;
            d->exp++;
            break;
        }
        *i-- = 0;
    }
}

inline void __rounddec(decimal* d, int digits) {
    if (digits <= 0 || digits >= d->sig.length) {
        return;
    }
    {
        int r = __must_round(d, digits);
        d->sig.length = (unsigned char)digits;
        if (r < 0) {
            return;
        }
    }
    __dorounddecup(d, digits);
}

inline void __ull2dec(decimal* result, unsigned long long integer) {
    result->sign = 0;
    if (integer == 0) {
        result->exp = 0;
        result->sig.length = 1;
        result->sig.text[0] = 0;
        return;
    }

    result->sig.length = 0;
    for (; integer != 0; integer /= 10) {
        result->sig.text[result->sig.length++] = (unsigned char)(integer % 10);
    }
    {
        unsigned char* i = result->sig.text;
        unsigned char* j = result->sig.text + result->sig.length;
        for (; i < --j; ++i) {
            unsigned char t = *i;
            *i = *j;
            *j = t;
        }
    }
    result->exp = (short)(result->sig.length - 1);
}

void __num2dec(const decform* f, f64 x, decimal* d) {
    short digits = f->digits;
    int i;

    __num2dec_internal(d, x);
    if (d->sig.text[0] > 9) {
        return;
    }
    if (digits > SIGDIGLEN) {
        digits = SIGDIGLEN;
    }
    __rounddec(d, digits);
    while (d->sig.length < digits) {
        d->sig.text[d->sig.length++] = 0;
    }
    d->exp -= d->sig.length - 1;
    for (i = 0; i < d->sig.length; ++i) {
        d->sig.text[i] += '0';
    }
}

void __num2dec_internal(decimal* d, f64 x) {
    char sgn = (*(u32*)&x & 0x80000000) != 0;

    if (x == 0.0) {
        d->sign = sgn;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = 0;
        return;
    }
    if (!isfinite(x)) {
        d->sign = sgn;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = (unsigned char)(isnan(x) ? 'N' : 'I');
        return;
    }

    if (sgn) {
        x = -x;
    }
    {
        int exp;
        f64 frac = frexp(x, &exp);
        int num_bits_extract = DBL_MANT_DIG - __count_trailing_zero(frac);
        f64 integer;
        decimal int_d;
        decimal pow2_d;

        __two_exp_800DCCB8(&pow2_d, exp - num_bits_extract);
        frac = modf(ldexp(frac, num_bits_extract), &integer);
        __ull2dec(&int_d, (unsigned long long)integer);
        __timesdec(d, &int_d, &pow2_d);
        d->sign = sgn;
    }
}

inline void __str2dec(decimal* d, const char* s, short exp) {
    int i;

    d->exp = exp;
    d->sign = 0;
    for (i = 0; i < SIGDIGLEN && *s != 0;) {
        d->sig.text[i++] = (unsigned char)(*s++ - '0');
    }
    d->sig.length = (unsigned char)i;
    if (*s != 0) {
        if (*s < 5) {
            return;
        }
        if (*s > 5) {
            goto round;
        }
        {
            const char* p = s + 1;
            for (; *p != 0; ++p) {
                if (*p != '0') {
                    goto round;
                }
            }
            if ((d->sig.text[i - 1] & 1) == 0) {
                return;
            }
        }
    round:
        __dorounddecup(d, d->sig.length);
    }
}

void __two_exp_800DCCB8(decimal* result, long exp) {
    decimal x2;

    switch (exp) {
    case -64:
        __str2dec(result, "542101086242752217003726400434970855712890625", -20);
        return;
    case -53:
        __str2dec(result, "11102230246251565404236316680908203125", -16);
        return;
    case -32:
        __str2dec(result, "23283064365386962890625", -10);
        return;
    case -16:
        __str2dec(result, "152587890625", -5);
        return;
    case -8:
        __str2dec(result, "390625", -3);
        return;
    case -7:
        __str2dec(result, "78125", -3);
        return;
    case -6:
        __str2dec(result, "15625", -2);
        return;
    case -5:
        __str2dec(result, "3125", -2);
        return;
    case -4:
        __str2dec(result, "625", -2);
        return;
    case -3:
        __str2dec(result, "125", -1);
        return;
    case -2:
        __str2dec(result, "25", -1);
        return;
    case -1:
        __str2dec(result, "5", -1);
        return;
    case 0:
        __str2dec(result, "1", 0);
        return;
    case 1:
        __str2dec(result, "2", 0);
        return;
    case 2:
        __str2dec(result, "4", 0);
        return;
    case 3:
        __str2dec(result, "8", 0);
        return;
    case 4:
        __str2dec(result, "16", 1);
        return;
    case 5:
        __str2dec(result, "32", 1);
        return;
    case 6:
        __str2dec(result, "64", 1);
        return;
    case 7:
        __str2dec(result, "128", 2);
        return;
    case 8:
        __str2dec(result, "256", 2);
        return;
    }

    __two_exp_800DCCB8(&x2, exp / 2);
    __timesdec(result, &x2, &x2);
    if (exp & 1) {
        decimal temp = *result;
        if (exp > 0) {
            __str2dec(&x2, "2", 0);
        } else {
            __str2dec(&x2, "5", -1);
        }
        __timesdec(result, &temp, &x2);
    }
}

static void __timesdec(decimal* result, const decimal* x, const decimal* y) {
    unsigned char mantissa[2 * SIGDIGLEN];
    int y_len = y->sig.length;
    int x_len = x->sig.length;
    unsigned long accumulator;
    int i = x_len + y_len - 1;
    unsigned char* ip = mantissa + i + 1;
    unsigned char* ep = ip;

    accumulator = 0;
    result->sign = 0;

    for (; i > 0; --i) {
        int k = y_len - 1;
        int j = i - k - 1;
        int l;
        int t;
        const unsigned char* jp;
        const unsigned char* kp;

        if (j < 0) {
            j = 0;
            k = i - 1;
        }
        jp = x->sig.text + j;
        kp = y->sig.text + k;
        l = k + 1;
        t = x_len - j;
        if (l > t) {
            l = t;
        }
        for (; l > 0; --l, ++jp, --kp) {
            accumulator += (unsigned long)(*jp) * *kp;
        }
        *--ip = (unsigned char)(accumulator % 10);
        accumulator /= 10;
    }

    result->exp = (short)(x->exp + y->exp);
    if (accumulator > 0) {
        *--ip = (unsigned char)accumulator;
        result->exp++;
    }
    for (i = 0; i < SIGDIGLEN && ip < ep; ++i, ++ip) {
        result->sig.text[i] = *ip;
    }
    result->sig.length = (unsigned char)i;
    if (ip < ep && *ip >= 5) {
        if (*ip == 5) {
            unsigned char* jp = ip + 1;
            for (; jp < ep; ++jp) {
                if (*jp != 0) {
                    goto round;
                }
            }
            if ((*(ip - 1) & 1) == 0) {
                return;
            }
        }
    round:
        __dorounddecup(result, result->sig.length);
    }
}

int __flush_buffer(MSL_FILE* stream, u32* count) {
    int result;
    u32 written = stream->buffer_ptr - stream->buffer;

    if (written != 0) {
        stream->buffer_length = written;
        result = ((int (*)(u32, void*, u32*, void*))stream->write_fn)(
            stream->handle, stream->buffer, &stream->buffer_length, stream->idle_fn);
        if (count) {
            *count = stream->buffer_length;
        }
        if (result != 0) {
            return result;
        }
        stream->position += stream->buffer_length;
    }

    result = 0;
    stream->buffer_ptr = stream->buffer;
    stream->buffer_length = stream->buffer_size;
    stream->buffer_length -= stream->position & stream->buffer_alignment;
    stream->buffer_position = stream->position;

    return result;
}

void __prep_buffer(MSL_FILE* stream) {
    stream->buffer_ptr = stream->buffer;
    stream->buffer_length = stream->buffer_size;
    stream->buffer_length -= stream->position & stream->buffer_alignment;
    stream->buffer_position = stream->position;
}
