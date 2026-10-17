#include "dolphin/types.h"

/*
 * MetroWerks CodeWarrior CRT string functions for GameCube (PowerPC).
 */

/* MSL word-at-a-time null-byte detection magic (MSL_Common/Src/string.c). */
#define K1 0x80808080
#define K2 0xFEFEFEFF

/* strcmp @ 0x800CA7FC (size 0x128). MSL_C/MSL_Common/Src/string.c (zeldaret/tww); CW 1.3. Verified 100%. */
int strcmp(const char* str1, const char* str2) {
    register u8* left = (u8*)str1;
    register u8* right = (u8*)str2;
    u32 align, r1, l1, x;

    l1 = *left;
    r1 = *right;
    x = l1 - r1;
    if (x) {
        return x;
    }

    if ((align = ((int)left & 3)) != ((int)right & 3)) {
        goto bytecopy;
    }

    if (align) {
        if (l1 == 0) {
            return 0;
        }
        for (align = 3 - align; align; align--) {
            l1 = *(++left);
            r1 = *(++right);
            x = l1 - r1;
            if (x) {
                return x;
            }
            if (l1 == 0) {
                return 0;
            }
        }
        left++;
        right++;
    }

    l1 = *(int*)left;
    r1 = *(int*)right;
    x = l1 + K2;
    if (x & K1) {
        goto adjust;
    }

    while (l1 == r1) {
        l1 = *(++((int*)(left)));
        r1 = *(++((int*)(right)));
        x = l1 + K2;
        if (x & K1) {
            goto adjust;
        }
    }

    x = -1;
    if (l1 > r1) {
        x = 1;
    }
    return x;

adjust:
    l1 = *left;
    r1 = *right;
    x = l1 - r1;
    if (x) {
        return x;
    }

bytecopy:
    if (l1 == 0) {
        return 0;
    }

    do {
        {
            u32 tmp = *(++left);
            l1 = tmp;
        }
        r1 = *(++right);
        x = l1 - r1;
        if (x) {
            return x;
        }
    } while (l1 != 0);

    return 0;
}


/*
 * strncpy - Copy at most n characters from src to dst.
 *
 * If the source string is shorter than n, the destination is padded
 * with null bytes up to length n.
 *
 * 0x800CA924 | size: 0x44
 */
char* strncpy(char* dst, const char* src, u32 n) {
    const u8* s = (const u8*)src - 1;
    u8* d = (u8*)dst - 1;
    u32 c;

    n++;
    while (--n != 0) {
        c = *++s;
        *++d = c;
        if (c == 0) {
            while (--n != 0) {
                *++d = 0;
            }
            return dst;
        }
    }

    return dst;
}

/*
 * strcpy - Copy a null-terminated string from src to dst.
 *
 * Uses word-at-a-time copying when both pointers are word-aligned,
 * with a null-byte detection trick for early termination. Falls
 * back to byte-by-byte copy when unaligned.
 *
 * 0x800CA968 | size: 0xB8
 */
char* strcpy(char* dst, const char* src) {
    char* ret = dst;
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    u32 align;
    u32 dstAlign;
    u32 c;
    u32 word;
    u32 mask;

    dstAlign = (u32)d & 3;
    if (dstAlign != (align = (u32)s & 3)) {
        goto adjust;
    }

    if (align != 0) {
        c = *s;
        *d = c;
        if (c == 0) {
            return ret;
        }
        for (align = 3 - align; align != 0; align--) {
            c = *++s;
            *++d = c;
            if (c == 0) {
                return ret;
            }
        }
        d++;
        s++;
    }

    word = *(const u32*)s;
    mask = 0x80808080;
    if (((word - 0x01010101) & mask) != 0) {
        goto adjust;
    }

    d -= 4;
    do {
        *(u32*)(d += 4) = word;
        word = *(const u32*)(s += 4);
    } while (((word - 0x01010101) & mask) == 0);
    d += 4;

adjust:
    c = *s;
    *d = c;
    if (c == 0) {
        return ret;
    }

    do {
        c = *++s;
        *++d = c;
    } while (c != 0);

    return ret;
}

/*
 * strlen - Compute the length of a null-terminated string.
 *
 * @param str  Pointer to the null-terminated string
 * @return     Number of characters before the terminating null byte
 *
 * Assembly pattern uses lbzu pre-increment loop:
 *   subi r4, r3, 1    ; p = str - 1
 *   li r3, -1         ; len = -1
 *   lbzu r0, 1(r4)    ; load *++p
 *   addi r3, r3, 1    ; len++
 *   cmplwi r0, 0      ; check null
 *   bne loop
 */
u32 strlen(const char* str) {
    const u8* p = (const u8*)str - 1;
    u32 len = (u32)-1;

    do {
        len++;
    } while (*++p != 0);

    return len;
}

/* wcscpy - 0x800CAA3C | size: 0x1C
 * wcscpy - Copy a wide character string from src to dst.
 * MSL_C/MSL_Common/Src/wchar_io.c (CW pattern); CW 1.3.
 */
u16* wcscpy(u16* dst, const u16* src) {
    const u16* s = src - 1;
    u16* d = dst - 1;
    u16 c;

    do {
        c = *++s;
        *++d = c;
    } while (c != 0);

    return dst;
}
