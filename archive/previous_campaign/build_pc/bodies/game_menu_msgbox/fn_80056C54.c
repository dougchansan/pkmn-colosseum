/* fn_80056C54 @ 0x80056C54 | menu_msgbox.c
 * PC-port bridge for the message-box context load path. The recovered caller
 * shape passes a source text context and selects one of the two 0x138-byte
 * contexts inside lbl_803A9768. */
extern u8 lbl_803A9768[];

void fn_80056C54(u8* out, u8* src, u32 slot) {
    u8* ctx;
    u32 i;

    slot &= 1u;
    *(u32*)(lbl_803A9768 + 0x278) = slot;
    ctx = lbl_803A9768 + 8u + slot * 0x138u;

    if (src != 0) {
        for (i = 0; i < 0x138u; ++i) {
            ctx[i] = src[i];
        }
    }

    if (out != 0) {
        for (i = 0; i < 0x138u; ++i) {
            out[i] = ctx[i];
        }
    }
}
