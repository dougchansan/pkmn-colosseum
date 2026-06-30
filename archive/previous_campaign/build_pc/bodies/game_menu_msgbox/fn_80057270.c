/* fn_80057270 @ 0x80057270 | menu_msgbox.c
 * PC-port bridge for the message context accessor used by script callbacks.
 * The real system stores two 0x138-byte text contexts in lbl_803A9768 and
 * selects the active one with the u32 at +0x278. */
u8 lbl_803A9768[0x2A0];

void* fn_80057270(void) {
    u32 slot = *(u32*)(lbl_803A9768 + 0x278);

    if (slot > 1u) {
        slot = 0u;
        *(u32*)(lbl_803A9768 + 0x278) = slot;
    }

    return lbl_803A9768 + 8u + slot * 0x138u;
}
