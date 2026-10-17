/**
 * Linked gs_title unit 0x80022E54 - 0x80023DA8 (fn_80022E54 .. itemFieldUseStatBoost)
 * and .sdata2 0x8047B8A0-0x8047B8A8, compiled from gs_title.c. The 0.5f
 * fade speed (0x8047B8A4) is this range's own literal; the 0.0f before it
 * belongs to the same pool but is read only by itemFieldUseCologne and fn_80022B3C,
 * which are not linked yet.
 */
/* RULE-EXCEPTION(title-path): the pool's leading 0.0f, read through this
 * name by the unlinked itemFieldUseCologne and fn_80022B3C carves, is defined here
 * ahead of the code so MWCC places it first. The unit
 * has to start its .sdata2 at 0x8047B8A0: an object's pool is 8-aligned - see
 * docs/RULE_EXCEPTIONS.md */
const float lbl_8047B8A0 = 0.0f;

#define GS_TITLE_SPLIT
#define GS_TITLE_RANGE_80022E54
#define GS_TITLE_RANGE_80023068
#define GS_TITLE_RANGE_80023274
#define GS_TITLE_RANGE_800232F0
#include "src/game/gs_title.c"

