/**
 * @file TRKComm_exact_800C39BC.c
 * @brief Exact pure-C TRK communication helpers, 0x800C39BC - 0x800C3BB8.
 */
#include "dolphin/types.h"

u32 __close_console(u32 arg)
{
    extern s32 fn_800C04F4(void);
    extern u32 TRKCloseFile(u32 cmd, u32 param);
    s32 initOk;
    u32 result;

    initOk = fn_800C04F4();
    if (initOk == 0) {
        return 1;
    }

    result = TRKCloseFile(0xD3, arg) & 0xFF;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

u32 fn_800C3A40(u32 unused, u32 length, u32* addrPtr)
{
    extern u8 fn_800C39A0(void);
    extern s32 fn_800C04F4(void);
    extern u32 TRKAccessFile(u32 cmd, u32 dir, u32* addrBuf, u32 len);
    u32 addrBuf;
    u32 result;

    if ((fn_800C39A0() & 0xFF) == 0) {
        return 1;
    }

    if (fn_800C04F4() == 0) {
        return 1;
    }

    addrBuf = *addrPtr;
    result = TRKAccessFile(0xD0, 1, &addrBuf, length);
    *addrPtr = addrBuf;

    switch (result & 0xFF) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}

u32 fn_800C3AFC(u32 unused, u32 length, u32* addrPtr)
{
    extern u8 fn_800C39A0(void);
    extern s32 fn_800C04F4(void);
    extern u32 TRKAccessFile(u32 cmd, u32 dir, u32* addrBuf, u32 len);
    u32 addrBuf;
    u32 result;

    if ((fn_800C39A0() & 0xFF) == 0) {
        return 1;
    }

    if (fn_800C04F4() == 0) {
        return 1;
    }

    addrBuf = *addrPtr;
    result = TRKAccessFile(0xD1, 0, &addrBuf, length);
    *addrPtr = addrBuf;

    switch (result & 0xFF) {
    case 0:
        return 0;
    case 2:
        return 2;
    default:
        return 1;
    }
}
