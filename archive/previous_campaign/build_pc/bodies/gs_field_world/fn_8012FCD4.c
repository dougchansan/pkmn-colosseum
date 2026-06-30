/*
 * fn_8012FCD4 - field/battle entry setup for the two active player slots (0x64, 0x65).
 *
 * Decides between two render/object setup paths based on fn_800FF548() (a "demo/no-link"
 * style flag returned in the low byte). When the flag is clear it runs a map/region lookup:
 * it copies a 0x50-byte region table (lbl_802729C0) onto the stack, queries fn_801906A0 with
 * a packed descriptor, then scans candidate region IDs (fn_8011393C / fn_8006AE18) to pick a
 * theme value r5; a no-match / out-of-range result falls back to 0x00F70400. That r5 theme is
 * pushed to objects 0x64 and 0x65 via fn_8018E050 (or fn_8018D998 on the flag-set path).
 *
 * It then spawns/binds two objects (slots {lbl_8047D030, lbl_8047D034}) via fn_800F9318 and
 * registers them (fn_800EB528), configures them (fn_8018CB5C), and - only when the flag is
 * clear - applies extra setup (fn_8018C8F4) plus binds the two incoming pointers arg0/arg1 as
 * the objects' payload (fn_8018C0A8 / fn_8018BF24). Finally it nudges both turn-zone paths
 * (fn_8012C660 with lbl_8047D038) and clears six counters in the global field block lbl_80426BD0.
 *
 * NOTE: r3/r4 saved into r29/r30 at entry are the two incoming args, only consumed at the very
 * end as the 3rd argument to fn_8018C0A8 / fn_8018BF24, so they are opaque pointers here.
 * ENDIAN-QA: lbl_8047D030/34/38 are read as f32 by sibling fns; here D030/D034 are used as the
 * two object-slot selectors (their stored bit-patterns are the IDs passed to fn_800F9318), and
 * D038 is the f32 turn amount. The transcription's two `if (r25 == 0x2)` guards before the
 * fn_8018C8F4 and fn_8018C0A8 blocks are transcription artifacts: the asm guards both blocks on
 * `clrlwi./bne` of fn_800FF548()'s low byte, i.e. `if (fn_800FF548() == 0)`.
 */
void fn_8012FCD4(void* arg0, void* arg1) {
    extern u8 fn_800FF548(void);
    extern u32 fn_801906A0(const u32* desc);
    extern u32 fn_8011393C(void);
    extern u32 fn_8006AE18(void);
    extern void* fn_800F9318(u32 group, u32 slot);
    extern void fn_800EB528(void* obj);
    extern void fn_8018E050(u32 group, u32 obj, u32 theme);
    extern void fn_8018D998(u32 group, u32 obj);
    extern void fn_8018CB5C(u32 group, u32 obj);
    extern void fn_8018C8F4(u32 group, u32 obj, u32 v);
    extern void fn_80189328(u32 group, u32 obj, u32 v);
    extern void fn_8018C0A8(u32 group, u32 obj, void* payload);
    extern void fn_8018BF24(u32 group, u32 obj, void* payload);
    extern void fn_8012C660(void* obj, s32 playerIdx, f32 turnAmount);
    extern void fn_8012F610(void);

    extern u8 lbl_802729C0[];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_80272A10[];  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047D030;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047D034;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047D038;     /* turn amount passed to fn_8012C660 */
    extern u8  lbl_80426BD0[];   /* global field-state block */

    u32 region_tbl[20];          /* sp+0x3c..: 10-region table copied from lbl_802729C0 */
    u32 query[10];               /* sp+0x18..0x3c: query descriptor for fn_801906A0 */
    u32 match_slots[5];          /* sp+0x18..0x38 viewed as 5 candidate IDs (region match) */
    u32 slot_sel[2];             /* sp+0x8 / sp+0xC: the two object-slot selectors */
    void* objs[2];               /* sp+0x10 / sp+0x14: the two spawned object handles */
    u32 theme;
    s32 i;

    if (fn_800FF548() == 0) {
        u32 region_id;
        s32 found;

        /* Copy the 0x50-byte (10 dword-pairs) region template onto the stack. */
        for (i = 0; i < 10; i++) {
            region_tbl[i * 2 + 0] = lbl_802729C0[i * 2 + 0];
            region_tbl[i * 2 + 1] = lbl_802729C0[i * 2 + 1];
        }

        /* Load the 10-dword query block and ask fn_801906A0 to resolve the current region. */
        for (i = 0; i < 10; i++) {
            query[i] = lbl_80272A10[i];
        }
        region_id = fn_801906A0(query);

        /* cntlzw(region_id) != 0  <=>  region_id == 0 (high bit clear): treat 0 as "valid",
         * any nonzero high bit as failure -> fall back to default theme.
         * The asm tests the top bit via cntlzw/extrwi; a zero result means region_id != 0. */
        if (region_id != 0) {
            theme = 0x00F70400; /* FUNCTIONAL-TODO: default/fallback region theme */
        } else {
            u32 cur = fn_8011393C();   /* current map/region key */
            s32 idx = 0;
            found = 0;

            /* Linear scan of the 20-entry region table for a match; idx counts entries walked. */
            for (i = 0; i < 2 && !found; i++) {
                s32 j;
                for (j = 0; j < 10; j++) {
                    if (cur == region_tbl[i * 10 + j]) { found = 1; break; }
                    idx++;
                }
            }

            if (idx >= 0x14) {
                theme = 0x00F70400; /* not found -> default theme */
            } else {
                u32 zone = fn_8006AE18();  /* resolve a sub-zone index 0..5 from the 5 match slots */
                s32 z;
                /* match_slots reads the same stack region as query (sp+0x18 == query[0]). */
                for (z = 0; z < 2; z++) match_slots[z] = query[z * 2]; /* sp+0x18,0x20 */
                match_slots[2] = query[4];  /* sp+0x28 */
                match_slots[3] = query[6];  /* sp+0x30 */
                match_slots[4] = query[8];  /* sp+0x38 */

                if      (zone == match_slots[0]) z = 0;
                else if (zone == match_slots[1]) z = 1;
                else if (zone == match_slots[2]) z = 2;
                else if (zone == match_slots[3]) z = 3;
                else if (zone == match_slots[4]) z = 4;
                else                              z = 5;

                /* Theme word lives at sp+0x1c + z*8 (odd dword of each pair in query). */
                theme = query[(z * 8 + 4) / 4 + 1 - 1]; /* = *(u32*)((u8*)query + 4 + z*8) */
            }
        }

        fn_8018E050(0, 0x64, theme);
        fn_8018E050(0, 0x65, 0x00F30400);
    } else {
        fn_8018D998(0, 0x64);
        fn_8018D998(0, 0x65);
    }

    /* Spawn and register the two player objects for slots {D030, D034}. */
    slot_sel[0] = lbl_8047D030;
    slot_sel[1] = lbl_8047D034;
    for (i = 0; i < 2; i++) {
        objs[i] = fn_800F9318(0, slot_sel[i]);
        fn_800EB528(objs[i]);
    }

    fn_8018CB5C(0, 0x64);
    fn_8018CB5C(0, 0x65);

    if (fn_800FF548() == 0) {
        fn_8018C8F4(0, 0x64, 0x4000F00);
        fn_8018C8F4(0, 0x65, 0x701);
    }

    fn_80189328(0, 0x65, 1);

    if (fn_800FF548() == 0) {
        fn_8018C0A8(0, 0x64, arg0);
        fn_8018BF24(0, 0x64, arg1);
    }

    for (i = 0; i < 2; i++) {
        fn_8012C660(objs[i], i, lbl_8047D038);
    }

    fn_8012F610();

    *(u32*)(lbl_80426BD0 + 0x44)  = 0;
    *(u32*)(lbl_80426BD0 + 0x48)  = 0;
    *(u32*)(lbl_80426BD0 + 0x414) = 0;
    *(u32*)(lbl_80426BD0 + 0x418) = 0;
    *(u32*)(lbl_80426BD0 + 0x410) = 0;
    *(u32*)(lbl_80426BD0 + 0x188) = 0;
}
