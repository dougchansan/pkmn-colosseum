/* fn_80053778 @ 0x80053778 | script_callback.c
 * PC-port bridge for the script callback main path. The full retail handler
 * manages more callback state, but this preserves the important text bridge:
 * script callbacks resolve the active menu_msgbox context via fn_80057270. */
u8 lbl_803A95E8[0x138];
u8 lbl_803A9720[0x48];

extern void* fn_80057270(void);

void fn_80053778(void) {
    void* msgCtx = fn_80057270();
    u32 count = *(u32*)(lbl_803A95E8 + 8) + 1u;

    *(void**)lbl_803A95E8 = msgCtx;
    *(u32*)(lbl_803A95E8 + 8) = count;
    *(void**)lbl_803A9720 = msgCtx;
    *(u32*)(lbl_803A9720 + 8) = count;
}
