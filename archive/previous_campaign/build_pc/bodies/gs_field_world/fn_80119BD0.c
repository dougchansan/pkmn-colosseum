/*
 * fn_80119BD0 - Particle/texture node setup and visibility callback.
 * Walks a linked list to the tail node, resolves a resource and texture,
 * optionally remaps the node via a secondary chain, finds a matching entry,
 * initialises per-node transform/color fields if not yet assigned, then
 * advances the particle entry and conditionally hides its child particle set.
 *
 * arg1, arg2 - unused in the function body (r3/r4 are not read before
 *              being clobbered); kept for ABI compatibility.
 * arg5       - selector/id forwarded to fn_801190DC (r5 in the original call).
 * arg6       - head pointer of the node linked list (r6 in the original call).
 */
void fn_80119BD0(u32 arg1, u32 arg2, u32 arg5, u8* arg6)
{
    /* Resource/texture accessors */
    extern u8*  fn_800E3B6C(u8* node);          /* get resource from node */
    extern u8*  fn_800E3BF8(u8* resource);       /* get texture from resource */
    extern u32  fn_800E6DC0(u8* resource);       /* get resource flags */
    extern u32  fn_800EE22C(u8* node, u8* orig); /* find entry index */
    extern u8*  fn_800EE150(u8* resource, u32 idx); /* get entry by index */
    extern u32  fn_800E3CBC(u8* resource);       /* get resource sub-id/type */
    extern u8*  fn_801190DC(u8* texture, u32 sel, u32 subid); /* resolve node from texture */
    extern u32  fn_800E3BF0(u8* resource);       /* get resource handle/id */
    extern u32  fn_800E3D08(u8* resource);       /* get resource visibility flags */
    extern void fn_800E01F4(u8* vec3, f32 x, f32 y, f32 z); /* write xyz floats */
    extern void fn_800EE828(u8* entry);          /* advance/consume particle entry */
    extern void psSetParticleVisibility(void* psObj, s32 visible);

    u8*  node     = arg6;   /* r30 / r28: current node (also kept as original) */
    u8*  orig_node = arg6;  /* original head, used as 2nd arg to fn_800EE22C */
    u8*  resource;          /* r29 */
    u8*  texture;           /* r31 */
    u8*  entry;             /* r28 after fn_800EE150 */
    u32  index;
    s32  handle;            /* r31 after fn_800E3BF0 */

    /* Walk the +0x0C child chain to the tail node. */
    while ((node != NULL ? *(u8**)(node + 0x0C) : NULL) != NULL) {
        if (node == NULL) {
            node = NULL;
        } else {
            node = *(u8**)(node + 0x0C);
        }
    }

    resource = fn_800E3B6C(node);
    if (resource == NULL) {
        return;
    }

    texture = fn_800E3BF8(resource);
    if (texture == NULL) {
        return;
    }

    /* If the resource has a non-zero flag byte, redirect node via +0x10 sibling. */
    if ((fn_800E6DC0(resource) & 0xFF) != 0) {
        node = *(u8**)(node + 0x10);
    }

    index = fn_800EE22C(node, orig_node);
    if (index == 0xFFFFFFFF) {
        return;
    }

    entry = fn_800EE150(resource, index);
    if (entry == NULL) {
        return;
    }

    /* Resolve the target node from the texture bank using arg5 as selector. */
    node = fn_801190DC(texture, arg5, fn_800E3CBC(resource));
    if (node != NULL) {
        handle = (s32)fn_800E3BF0(resource);
        /* Only initialise if node has not been assigned a handle yet. */
        if (*(s32*)(node + 0x44) == 0 && handle != 0) {
            *(u32*)(node + 0x48) = *(u32*)(entry + 0x04); /* copy entry word */
            *(u32*)(node + 0x4C) = *(u16*)(entry + 0x02); /* copy entry half-word */ /* ENDIAN-QA */
            /* Initialise six 3-float fields: two groups of (zero,zero,zero)
             * and two groups of (one,one,one) interleaved at fixed offsets.
             * lbl_8047CFE8 = 0.0f, lbl_8047CFEC = 1.0f (typical sdata2 constants). */
            fn_800E01F4(node + 0x50, lbl_8047CFE8, lbl_8047CFE8, lbl_8047CFE8);
            fn_800E01F4(node + 0x5C, lbl_8047CFE8, lbl_8047CFE8, lbl_8047CFE8);
            fn_800E01F4(node + 0x68, lbl_8047CFEC, lbl_8047CFEC, lbl_8047CFEC);
            fn_800E01F4(node + 0x14, lbl_8047CFE8, lbl_8047CFE8, lbl_8047CFE8);
            fn_800E01F4(node + 0x20, lbl_8047CFE8, lbl_8047CFE8, lbl_8047CFE8);
            fn_800E01F4(node + 0x2C, lbl_8047CFEC, lbl_8047CFEC, lbl_8047CFEC);
            *(u32*)(node + 0x44) = (u32)handle; /* stamp handle to prevent re-init */
            *(u8*)(node + 0x05) = 0;
            *(u8*)(node + 0x06) = 1;
        }
    }

    fn_800EE828(entry);

    /* If the resource visibility flag byte is zero, hide the particle set.
     * FUNCTIONAL-TODO: if fn_801190DC returned NULL above, node is NULL here
     * and lwz 0x10(node) will fault — this mirrors the original asm behaviour
     * (the PPC also dereferences r30 unconditionally at L_80119D7C branch);
     * leave the null-deref to match original semantics. */
    if ((fn_800E3D08(resource) & 0xFF) == 0) {
        psSetParticleVisibility(*(void**)(node + 0x10), 0);
    }
}
