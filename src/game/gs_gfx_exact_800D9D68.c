/**
 * @file gs_gfx_exact_800D9D68.c
 * @brief Matching GS graphics state range, 0x800D9D68-0x800DA578.
 */

#include "dolphin/types.h"

extern u32 lbl_8047AA80;
extern void fn_800D4F98(u32, ...);
extern u32 GScameraGetActiveCamera(void);
extern void fn_800BD7A0(u32, u32, u32, u32);
extern void fn_800D2150(u32, u16, u16, u16, u16);
extern void HSD_FogSet(u32);
extern u32 lbl_8047AA8C;
extern void fn_800B953C(u32);
extern void GXSetClipMode(u32);
extern void fn_800B94F0(u32);
extern u8 lbl_8031453C[];
extern void fn_800BCFDC(u32);
extern void fn_800BC618(u32, u8, u32, u32, u8);
extern u8 lbl_8031457C[];
extern u8 lbl_8031456C[];
extern void GXSetZMode(u32, u32, u32);
extern void fn_800BCEBC(u32);
extern u8 lbl_8031454C[];
extern void fn_800BCE30(u32);
extern void fn_800BCE5C(u32);
extern void GXSetDstAlpha(u32);
extern void GXSetBlendMode(u32, u32, u32, u32);
extern u8 lbl_803145D0[];
extern u32 lbl_8031459C[];
extern u32 lbl_803145A8[];

/* Retail marks every function in this physical island active. */
#pragma force_active on

void fn_800D9D68(u16 x1, u16 y1, u16 x2, u16 y2) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x37, 4, (u32)x1, (u32)y1, (u32)x2, (u32)y2);
    } else {
        fn_800BD7A0((u32)x1, (u32)y1, (u32)(x2 - x1) + 1, (u32)(y2 - y1) + 1);
        *(u16*)(lbl_8047AA80 + 0x46e) = x1;
        *(u16*)(lbl_8047AA80 + 0x470) = y1;
        *(u16*)(lbl_8047AA80 + 0x472) = x2;
        *(u16*)(lbl_8047AA80 + 0x474) = y2;
        {
            u32 result = GScameraGetActiveCamera();
            if (result) {
                fn_800D2150(result, x1, y1, x2, y2);
            }
        }
    }
}

void fn_800D9E4C(s32 val) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x36, 1, val);
    } else {
        if (val == 1) {
            *(u8*)(state + 0x5d) = 1;
        } else if (val == 0) {
            *(u8*)(state + 0x5d) = 0;
        }
        if (*(u8*)(lbl_8047AA80 + 0x5d) == 1) {
            HSD_FogSet(lbl_8047AA8C);
        } else {
            HSD_FogSet(0);
        }
    }
}

void fn_800D9ED8(s32 val) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x35, 1, val); }
    else if (val == 1) { *(u8*)(lbl_8047AA80 + 0x1a) = 1; }
    else if (val == 0) { *(u8*)(lbl_8047AA80 + 0x1a) = 0; }
}

void fn_800D9F40(s32 val) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) { fn_800D4F98(0x34, 1, val); }
    else {
        if (val == 1) { *(u8*)(state + 0x42d) = 1; }
        else if (val == 0) { *(u8*)(state + 0x42d) = 0; }
        fn_800B953C(*(u8*)(lbl_8047AA80 + 0x42d));
    }
}

void fn_800D9FB4(s32 val) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) { fn_800D4F98(0x33, 1, val); }
    else {
        if (val == 1) { *(u8*)(state + 0x42c) = 0; }
        else if (val == 0) { *(u8*)(state + 0x42c) = 1; }
        GXSetClipMode(*(u8*)(lbl_8047AA80 + 0x42c));
    }
}

void fn_800DA028(u32 idx) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x32, 1, idx); }
    else {
        *(s32*)(lbl_8047AA80 + 0x428) = ((u32*)lbl_8031453C)[idx];
        fn_800B94F0(*(s32*)(lbl_8047AA80 + 0x428));
    }
}

void fn_800DA08C(s32 val) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) { fn_800D4F98(0x31, 1, val); }
    else {
        if (val == 1) { *(u8*)(state + 0x426) = 1; }
        else if (val == 0) { *(u8*)(state + 0x426) = 0; }
        fn_800BCFDC(*(u8*)(lbl_8047AA80 + 0x426));
    }
}

void fn_800DA100(s32 enable, s32 comp0, u8 ref0, s32 op, s32 comp1, u8 ref1) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x30, 6, enable, comp0, (u32)ref0, op, comp1, (u32)ref1);
    } else {
        if (enable == 1) {
            *(u8*)(state + 0x425) = 1;
        } else if (enable == 0) {
            *(u8*)(state + 0x425) = 0;
        }
        if (*(u8*)(lbl_8047AA80 + 0x425) == 0) {
            fn_800BC618(7, 0, 1, 7, 0);
        } else {
            fn_800BC618(((u32*)lbl_8031457C)[comp0], ref0,
                        ((u32*)lbl_8031456C)[op],
                        ((u32*)lbl_8031457C)[comp1], ref1);
        }
    }
}

void fn_800DA1E8(s32 zEnable, s32 zFunc, s32 zUpdate) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x2f, 3, zEnable, zFunc, zUpdate);
    } else {
        if (zEnable == 1) {
            *(u8*)(state + 0x41c) = 1;
        } else if (zEnable == 0) {
            *(u8*)(state + 0x41c) = 0;
        }
        if (zUpdate == 1) {
            *(u8*)(lbl_8047AA80 + 0x424) = 1;
        } else if (zUpdate == 0) {
            *(u8*)(lbl_8047AA80 + 0x424) = 0;
        }
        *(u32*)(lbl_8047AA80 + 0x420) = *(u32*)(lbl_8031454C + zFunc * 4);
        GXSetZMode(*(u8*)(lbl_8047AA80 + 0x41c), *(u32*)(lbl_8047AA80 + 0x420), *(u8*)(lbl_8047AA80 + 0x41b));
        fn_800BCEBC(*(u8*)(lbl_8047AA80 + 0x424));
    }
}

void fn_800DA2BC(s32 zCompare, s32 zBeforeTexture, s32 zFunc) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x2e, 3, zCompare, zBeforeTexture, zFunc);
    } else {
        if (zCompare == 1) {
            *(u8*)(state + 0x419) = 1;
        } else if (zCompare == 0) {
            *(u8*)(state + 0x419) = 0;
        }
        if (zBeforeTexture == 1) {
            *(u8*)(lbl_8047AA80 + 0x41a) = 1;
        } else if (zBeforeTexture == 0) {
            *(u8*)(lbl_8047AA80 + 0x41a) = 0;
        }
        if (zFunc == 1) {
            *(u8*)(lbl_8047AA80 + 0x41b) = 1;
        } else if (zFunc == 0) {
            *(u8*)(lbl_8047AA80 + 0x41b) = 0;
        }
        fn_800BCE30(*(u8*)(lbl_8047AA80 + 0x419));
        fn_800BCE5C(*(u8*)(lbl_8047AA80 + 0x41a));
        GXSetZMode(*(u8*)(lbl_8047AA80 + 0x41c),
                   *(u32*)(lbl_8047AA80 + 0x420),
                   *(u8*)(lbl_8047AA80 + 0x41b));
    }
}

void fn_800DA3B0(s32 val, u8 b) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) { fn_800D4F98(0x2d, 2, val, (u32)b); }
    else {
        if (val == 1) { *(u8*)(state + 0x418) = 1; }
        else if (val == 0) { *(u8*)(state + 0x418) = 0; }
        GXSetDstAlpha(*(u8*)(lbl_8047AA80 + 0x418));
    }
}

void fn_800DA428(s32 val) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x2c, 1, val);
    } else if (val == 0) {
        GXSetBlendMode(0, 1, 1, 5);
        *(u32*)(lbl_8047AA80 + 0x8) = 0x10;
    } else {
        GXSetBlendMode(2, 1, 1, *(u32*)(lbl_803145D0 + val * 4));
        *(u32*)(lbl_8047AA80 + 0x8) = 0x2000;
    }
}

void fn_800DA4C4(s32 a, s32 b, s32 c) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x2b, 3, a, b, c);
    } else if (a == 0) {
        GXSetBlendMode(0, 1, 1, 5);
        *(u32*)(lbl_8047AA80 + 0x8) = 0x10;
    } else {
        GXSetBlendMode(lbl_8031459C[a], lbl_803145A8[b], lbl_803145A8[c], 5);
        *(u32*)(lbl_8047AA80 + 0x8) = 0x2000;
    }
}
