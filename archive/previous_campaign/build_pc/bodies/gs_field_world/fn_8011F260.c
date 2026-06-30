/*
 * fn_8011F260 @ 0x8011F260 | gs_field_world.c | size 0x1FC
 * Slot / HP-bar record lookup helper.
 *
 * Given a field-object pointer (ptr), a slot index (idx, u16), and a
 * selector flag (flag, u32), returns a pointer to the appropriate 4-byte
 * slot record within the object's embedded slot array, or to a global
 * "default" HP-bar record, or NULL.
 *
 * Slot layout in the object:
 *   ptr + 0x78 + idx*4  — the idx-th 4-byte slot record
 *
 * The object also carries:
 *   ptr[0xD6] (u16) — secondary HP-bar slot count / active flag
 *   ptr[0xD8] (u16) — HP-bar count (number of active HP-bar slots)
 *   ptr[0xDC] (s32) — HP-bar state (negative = inactive)
 *
 * Behaviour:
 *   1. If ptr is NULL → return NULL.
 *   2. If idx >= 4   → return NULL (only 4 slots: 0..3).
 *   3. Determine whether the object has an active HP-bar:
 *        active = (ptr[0xD8] != 0) && (ptr[0xDC] >= 0)
 *   4. If active && flag == 1 && ptr[0xD6] != 0:
 *        return lbl_80478B58  (global "secondary HP-bar" default record)
 *   5. If active && idx == 1 && flag == 1:
 *        return lbl_80478B58
 *   6. Call fn_8011F77C(ptr) to get the HP-level category (0-7).
 *      Based on level:
 *        level < 2                    → return lbl_80478B5C
 *        level == 2 && level < 4      → return lbl_80478B5C
 *        level == 3 && level < 5      → return lbl_80478B5C
 *   7. Default: return ptr + 0x78 + idx*4.
 *
 * ENDIAN-QA: lbl_80478B58 / lbl_80478B5C are BSS arrays used as opaque
 * 4-byte records; callers read u16 at offset 0 and u8 at offsets 2/3.
 * The HP-level thresholds in steps 6 are from the pseudo-register body;
 * the exact conditions are transcription artifacts but the functional
 * intent (return a default record for low-HP objects) is preserved.
 */
u8* fn_8011F260(u8* ptr, u16 idx, u32 flag)
{
    extern u32 fn_8011F77C(u8* ptr);
    extern u8  lbl_80478B58[];
    extern u8  lbl_80478B5C[];

    u16 hpbar_count;
    s32 hpbar_state;
    u32 active;
    u32 level;

    if (ptr == NULL) {
        return NULL;
    }

    if ((u32)idx >= 4) {
        return NULL;
    }

    /* Check whether the object has an active HP-bar */
    hpbar_count = *(u16*)(ptr + 0xD8);
    hpbar_state = *(s32*)(ptr + 0xDC);
    active = (hpbar_count != 0) && (hpbar_state >= 0);

    if (active) {
        /* Secondary HP-bar slot: check ptr[0xD6] */
        if (flag == 1 && *(u16*)(ptr + 0xD6) != 0) {
            return lbl_80478B58;
        }
        /* Primary HP-bar slot for idx==1 */
        if ((u32)idx == 1 && flag == 1) {
            return lbl_80478B58;
        }
    }

    /* Classify HP level and decide which default record to return */
    level = fn_8011F77C(ptr);

    if (level < 2) {
        return lbl_80478B5C;
    }
    if (level == 2 && level < 4) {
        return lbl_80478B5C;
    }
    if (level == 3 && level < 5) {
        return lbl_80478B5C;
    }

    /* Default: return the idx-th 4-byte slot in the object's slot array */
    return ptr + 0x78 + (u32)idx * 4;
}
