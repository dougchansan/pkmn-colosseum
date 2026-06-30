/*
 * fn_8012BCA4 — Field-world turn-zone activation sweep (2 slots).
 *
 * Iterates over 2 entries in lbl_80426BD0 (stride 0x20 each).  For each
 * entry whose u16 at offset +4 has bit 0 set, looks up an object via
 * fn_800F9318, calls the turn-zone path selector, then fires three
 * sound/state dispatcher calls with the slot's packed ID.
 *
 * The two packed IDs come from lbl_8047D030 / lbl_8047D034 (raw-bit
 * reinterpret as u32); at iteration i the id is the i-th 32-bit word
 * of a two-word table initialised to those two globals.
 */
void fn_8012BCA4(void)
{
    extern u8   lbl_80426BD0[];
    extern f32  lbl_8047D030;
    extern f32  lbl_8047D034;
    extern f32  lbl_8047D038;
    extern void* fn_800F9318(u32 a, u32 b);
    extern void  fn_8012C660(void* obj, s32 playerIdx, f32 turnAmount);
    extern void  fn_8018C7C8(u32 a, u32 b, u32 c);
    extern void  fn_8018C69C(u32 a, u32 b, u32 c);
    extern void  fn_8018CA20(u32 a, u32 b, u32 c);

    /* Two-entry packed-ID table, mirrored as in the asm (table_a == table_b).
     * Entry i selects lbl_8047D030 (i=0) or lbl_8047D034 (i=1) by raw bits.
     * *  ENDIAN-QA: u32 reinterpret of f32 globals used as opaque object IDs */
     u32 id_table[2];
    void* obj;
    s32   i;
    u32   id;
    u16   flag;

    id_table[0] = *(u32*)&lbl_8047D030;
    id_table[1] = *(u32*)&lbl_8047D034;

    for (i = 0; i < 2; i++) {
        /* Check bit 0 of u16 at byte offset +4 within the i-th 0x20-byte slot */
        flag = *(u16*)(&lbl_80426BD0[i * 0x20] + 4);
        if (!(flag & 1)) {
            continue;
        }

        id  = id_table[i];
        obj = fn_800F9318(0, id);
        fn_8012C660(obj, i, lbl_8047D038);
        fn_8018C7C8(0, id, 0x80000008);
        fn_8018C69C(0, id, 0x100);
        fn_8018C69C(0, id, 0x400);
        fn_8018CA20(0, id, 0);
    }
}
