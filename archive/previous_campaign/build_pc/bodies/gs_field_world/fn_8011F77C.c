/*
 * fn_8011F77C @ 0x8011F77C | gs_field_world.c | size 0x194
 * HP-level classifier for a field object.
 *
 * Given a field-object pointer, returns a category index 0-7 that encodes
 * the object's current HP as a fraction of its maximum HP:
 *
 *   7  — ptr is NULL, or the object is not "active" (0xC2 flag == 0),
 *         or fn_8012640C(ptr,0,0xC2,0) returns 0.
 *   0  — HP ratio == lbl_8047CFF4  (1.0f, full HP)
 *   1  — HP ratio == lbl_8047CFF8
 *   2  — HP ratio == lbl_8047CFFC
 *   3  — HP ratio == lbl_8047D000
 *   4  — HP ratio == lbl_8047D004
 *   5  — HP ratio  > lbl_8047CFF0  (0.0f, i.e. > 0)
 *   6  — HP ratio <= lbl_8047CFF0  (0.0f, i.e. == 0 or negative)
 *
 * The HP ratio is computed as:
 *   raw_hp  = fn_8012640C(ptr, 0, 0xC5, 0)  (signed 16-bit, XOR 0x8000 bias)
 *   max_hp  = fn_8012640C(ptr, 0, 0xC4, 0)  (u16; if 0 → clamp to 1)
 *   ratio   = (float)(raw_hp ^ 0x8000 - 0x80000000_bias) / lbl_8047CFF4
 *   scaled  = ratio * lbl_8047CFF4 / (float)max_hp
 *
 * The 0x43300000 / xoris trick is the CW int-to-float idiom for signed 16-bit
 * values; replaced here by a direct cast.
 *
 * lbl_8047CFF0 = 0.0f, lbl_8047CFF4 = 1.0f (or the HP scale factor),
 * lbl_8047CFF8/CFFC/D000/D004 = the four intermediate HP thresholds,
 * lbl_8047D008 / lbl_8047D010 = the two double-precision bias constants used
 * by the CW int-to-float idiom (not needed in the C port).
 */
u32 fn_8011F77C(u8* ptr)
{
    extern u32 fn_8012640C(u8* ptr, u32 a, u32 b, u32 c);

    extern f32 lbl_8047CFF0;  /* 0.0f */
    extern f32 lbl_8047CFF4;  /* 1.0f (HP scale) */
    extern f32 lbl_8047CFF8;  /* threshold 1 */
    extern f32 lbl_8047CFFC;  /* threshold 2 */
    extern f32 lbl_8047D000;  /* threshold 3 */
    extern f32 lbl_8047D004;  /* threshold 4 */

    u32 max_hp_raw;
    u32 raw_hp;
    f32 ratio;
    f32 max_hp_f;

    if (ptr == NULL) {
        return 7;
    }

    /* Check "active" flag at selector 0xC2 */
    if ((u8)fn_8012640C(ptr, 0, 0xC2, 0) == 0) {
        return 7;
    }

    /* Read max HP (selector 0xC4, u16); if zero treat as 1 to avoid divide-by-zero */
    max_hp_raw = (u16)fn_8012640C(ptr, 0, 0xC4, 0);
    if (max_hp_raw == 0) {
        max_hp_raw = 1;
    }

    /* Read current HP (selector 0xC5, signed 16-bit via XOR 0x8000 bias in asm).
     * The CW idiom: xoris r3,r3,0x8000 then 0x43300000 double-bias subtract.
     * Equivalent: treat the raw u16 as a signed value relative to 0x8000 midpoint,
     * i.e. signed_hp = (s32)(u16)raw - 0x8000.  Then ratio = signed_hp / lbl_8047CFF4.
     * But since lbl_8047CFF4 == 0x8000 (the scale factor), ratio = signed_hp / 0x8000.
     * Simplified: ratio = (f32)(s32)((u16)raw_hp ^ 0x8000) / lbl_8047CFF4 - 1.0f
     * which equals (f32)(s16)raw_hp / lbl_8047CFF4 when lbl_8047CFF4 == 0x8000.
     * Use the direct cast form that matches the functional intent. */
    raw_hp = fn_8012640C(ptr, 0, 0xC5, 0);
    ratio  = (f32)(s32)((u16)raw_hp ^ 0x8000) / lbl_8047CFF4;

    /* Clamp ratio to [0.0f, ...] */
    if (ratio < lbl_8047CFF0) {
        ratio = lbl_8047CFF0;
    }

    /* Scale by max HP: scaled = ratio * lbl_8047CFF4 / (float)max_hp */
    max_hp_f = (f32)(s32)max_hp_raw;
    ratio    = (ratio * lbl_8047CFF4) / max_hp_f;

    /* Classify into 0-6 */
    if (ratio == lbl_8047CFF4) { return 0; }
    if (ratio == lbl_8047CFF8) { return 1; }
    if (ratio == lbl_8047CFFC) { return 2; }
    if (ratio == lbl_8047D000) { return 3; }
    if (ratio == lbl_8047D004) { return 4; }
    if (ratio >  lbl_8047CFF0) { return 5; }
    return 6;
}
