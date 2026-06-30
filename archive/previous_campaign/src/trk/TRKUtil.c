#include "dolphin/types.h"

/*
 * TRKUtil.c - MetroTRK utility functions.
 *
 * Small utility functions used by the MetroTRK debugger system.
 * These sit between TRKComm.c and udp_cc.c in the link order.
 *
 * Matches: 0x800C3B00 - 0x800C3BB8
 *   fn_800C3B00 (0x44) - TRKMemCpy or utility helper
 *   fn_800C3B44 (0x38) - TRKMemSet or utility helper
 *   fn_800C3B7C (0x20) - TRKString helper
 *   fn_800C3B9C (0x1C) - TRKString helper 2
 */

/*
 * TRKMemCpy - Copy memory for TRK debug protocol.
 * 0x800C3B00 | size: 0x44
 *
 * Simple memcpy implementation used by the TRK debugger
 * for copying data during debug message processing.
 */
void* TRK_memcpy(void* dst, const void* src, u32 n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;

    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dst;
}

/*
 * TRKMemSet - Fill memory for TRK debug protocol.
 * 0x800C3B44 | size: 0x38
 */
void* TRK_memset(void* dst, int val, u32 n) {
    u8* d = (u8*)dst;

    while (n > 0) {
        *d++ = (u8)val;
        n--;
    }

    return dst;
}

/*
 * TRK_strlen - String length for TRK.
 * 0x800C3B7C | size: 0x20
 */
u32 TRK_strlen(const char* s) {
    const char* p = s;

    while (*p != '\0') {
        p++;
    }

    return (u32)(p - s);
}

/*
 * TRK_strcat - String concatenation for TRK.
 * 0x800C3B9C | size: 0x1C
 */
char* TRK_strcat(char* dst, const char* src) {
    char* d = dst;

    /* Find end of dst */
    while (*d != '\0') {
        d++;
    }

    /* Copy src */
    while (*src != '\0') {
        *d++ = *src++;
    }
    *d = '\0';

    return dst;
}
