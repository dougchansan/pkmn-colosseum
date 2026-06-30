/* Resolve or allocate a texture-backed particle/model node.
   The texture owns a 64-entry pointer table at +0x08.  Each active node is
   drawn from the global 0x74-byte node pool and linked into that table. */
u8* fn_801190DC(u8* texture, u32 selector, u32 subid)
{
    extern u32 lbl_8047ADA8;
    extern u32 lbl_8047ADAC;
    extern u32 fn_800D3094(void);
    extern void fn_800E01F4(u8* dst, f32 x, f32 y, f32 z);
    extern void fn_80119930(u8* texture);
    extern u8* fn_80173718(u32 use_alt, u32 texture_type, u32 selector);

    u8* node;
    u8* object;
    u32 slot;
    u32 i;

    if (texture == NULL || texture[0] == 0) {
        return NULL;
    }

    node = NULL;
    for (i = 0; i < lbl_8047ADAC; i++) {
        u8* candidate = (u8*)lbl_8047ADA8 + i * 0x74;
        if (candidate[0] == 0) {
            node = candidate;
            break;
        }
    }
    if (node == NULL) {
        return NULL;
    }

    slot = 0xFFFFFFFFu;
    for (i = 0; i < 64; i++) {
        if (*(u32*)(texture + 0x08 + i * 4) == 0) {
            slot = i;
            break;
        }
    }
    if (slot == 0xFFFFFFFFu) {
        fn_80119930(texture);
        for (i = 0; i < 64; i++) {
            if (*(u32*)(texture + 0x08 + i * 4) == 0) {
                slot = i;
                break;
            }
        }
        if (slot == 0xFFFFFFFFu) {
            return NULL;
        }
    }

    object = fn_80173718(((u8)subid == 1) ? 1u : 0u, texture[1], selector);
    *(u32*)(node + 0x10) = (u32)object;
    if (object == NULL) {
        return NULL;
    }

    object[0x17] = 1;
    node[0] = 1;
    *(u32*)(node + 0x0C) = (u32)texture;
    *(u16*)(node + 0x02) = (u16)selector;
    *(u32*)(node + 0x44) = 0;
    *(u32*)(node + 0x48) = 0;
    *(u32*)(node + 0x4C) = 0;
    node[5] = 0;
    node[6] = 0;

    fn_800E01F4(node + 0x38,
                *(f32*)(object + 0x20),
                *(f32*)(object + 0x24),
                *(f32*)(object + 0x28));
    node[4] = (u8)subid;
    *(u32*)(node + 0x08) = fn_800D3094();
    *(u32*)(texture + 0x08 + slot * 4) = (u32)node;

    node[1] = (*(u16*)(object + 0x10) == 0) ? 1 : 0;
    return node;
}
