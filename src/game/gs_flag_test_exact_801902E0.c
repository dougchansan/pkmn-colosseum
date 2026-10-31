/**
 * @file gs_flag_test_exact_801902E0.c
 * @brief GS flag test (XD: GSflagTest), 0x801902E0 - 0x801903B0.
 *
 * Function-boundary carve of the GS flag module (see gs_range_8018FE30.c
 * for its neighbours), made so the title-floor flag test links while
 * dbgFlagApplyPreset and the setters stay candidates. No local data: the
 * definition and state tables (.sdata), the bit-mask table (XD: _flagAND)
 * and the error string are extern.
 *
 * fn_801902E0 is XD's GSflagTest (FUN_801a02f0: GSflagGet(id) != 0) with
 * GSflagGet and its local _flagGet inlined, as in
 * gs_flag_get_exact_801906A0.c. References: TeamOrre/xd-decomp
 * config/GXXE01/symbols.txt @ 4989794e, trevor403/xd-asm @ b1087f18
 * (func_FUN_801a02f0.s, func_FUN_801a03e8.s).
 */
#include "dolphin/types.h"

typedef struct FlagStateEntry {
    u32 unused;
    u32* buffer;
} FlagStateEntry;

typedef struct FlagDefinition {
    u8 typeAndWidth;
    u8 initialValue;
    u8 itemSwap;
    u8 event;
    u16 bitPosition;
    s16 next;
} FlagDefinition;

extern void GSlogWrite(const char* fmt, ...);

/* XD's local _flagGet (trevor403/xd-asm @ b1087f18 FUN_801a03e8), as in
 * gs_flag_get_exact_801906A0.c. fn_801902E0 is XD's GSflagTest (FUN_801a02f0,
 * GSflagGet(id) != 0) with GSflagGet and _flagGet expanded. */
extern u32 lbl_8036C568[];
extern const char lbl_80274284[];
static inline u32 _flagGet(u32* buffer, FlagDefinition* definitions, s32 flagId)
{
    u32 bitOffset;
    u32 wordIndex;
    u32 bitPosition;
    u32 bitWidth;

    if (buffer == NULL) {
        GSlogWrite(lbl_80274284);
        return 0;
    }
    bitOffset = definitions[flagId].bitPosition;
    bitWidth = definitions[flagId].typeAndWidth & 0x3F;
    wordIndex = bitOffset >> 5;
    bitPosition = bitOffset & 0x1F;
    if (bitWidth > 1) {
        u32 low = buffer[wordIndex];
        u32 high = buffer[wordIndex + 1];

        low >>= bitPosition;
        high <<= 32 - bitPosition;
        return (high | low) & lbl_8036C568[bitWidth];
    }
    return (buffer[wordIndex] >> bitPosition) & 1;
}

u8 std081_flagCheckTrue(s32 flagId)
{
    extern FlagDefinition* lbl_80478F9C;
    extern FlagStateEntry* lbl_80478EEC;
    u32 value;
    u8 result;
    value = _flagGet(lbl_80478EEC[(lbl_80478F9C[flagId].typeAndWidth & 0xC0) >> 6].buffer,
                     lbl_80478F9C, flagId);
    /* RULE-EXCEPTION(title-path): redundant statement for codegen only - see
     * docs/RULE_EXCEPTIONS.md. Retail keeps the 0/1 result as a
     * branch diamond (bne; li 0; b; li 1). Every if/else, ?: and != form is
     * turned into neg/or/srwi by the frontend's if-to-?: conversion; the
     * two-statement else arm is not converted and folds to li 1 later. */
    if (value == 0) {
        result = 0;
    } else {
        result = 0;
        result++;
    }
    return result;
}
