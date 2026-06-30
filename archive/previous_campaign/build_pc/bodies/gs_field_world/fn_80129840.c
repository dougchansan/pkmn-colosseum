/* fn_80129840 — iterate party slots 0..5, release each valid Pokemon object.
   For each slot index i, fetch the Pokemon object pointer via fn_8012A5B0(party, 3, i).
   If the pointer passes the validity check (fn_80123FBC), release/free it (fn_80120674).
   r30 = loop counter (u16 compare < 6), r29 = party ptr, r31 = slot ptr. */
void fn_80129840(u8* party) {
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);
    extern u32  fn_80123FBC(u8* ptr);
    extern s32  fn_80120674(void* obj);
    u32 i;
    for (i = 0; (u16)i < 6; i++) {
        u8* slot = (u8*)fn_8012A5B0(party, 3, i);
        if ((u8)fn_80123FBC(slot) != 0) {
            fn_80120674(slot);
        }
    }
}
