s32 fn_80017990(u8* src, u8* dst) {
    s32 idx = (s32)(s8)src[0x95];
    if ((s32)*(s16*)(dst + 6) == *(s32*)((u8*)lbl_80266918 + idx * 0x4C + 0x10)) {
        dst[0x64] = lbl_80266918[idx * 0x4C];
        dst[0x65] = lbl_80266918[(s32)(s8)*(volatile u8*)(src + 0x95) * 0x4C + 1];
        dst[0x66] = lbl_80266918[(s32)(s8)*(volatile u8*)(src + 0x95) * 0x4C + 2];
        dst[0x67] = 0xFF;
    } else {
        dst[0x67] = 0;
    }
    return 0;
}
