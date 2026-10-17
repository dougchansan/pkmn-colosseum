/**
 * @file gbaCommunication_exact_80093F2C.c
 * @brief gbaCommunication carve, 0x80093F2C - 0x80094650 (2 functions:
 *        menuPokemonStatusCtrlRibbon and the ribbon-grid builder fn_80093F64).
 *
 * Split from gbaCommunication_candidate_800937F4.c so these exact functions
 * link while _gbaCommThread and fn_80093B4C are still being matched. Standalone
 * carve: GBA_COMMUNICATION_DECLS_ONLY takes only the shared declarations from
 * gbaCommunication.c. The bodies are gbaCommunication.c's; keep them in step.
 */
#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

/* 0x80093F2C | size: 0x38 */
void menuPokemonStatusCtrlRibbon(void) {
    extern void fn_80093F64();
    u8 *r4 = (u8*)&lbl_803FB380;
    u32 r3 = *(u32*)(r4 + 0xC);

    if (r3 != 0) {
        fn_80093F64(r3, r4 + 0x1c);
    }
    return;
}

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;

extern RibbonGroupDescriptor lbl_802EEFD8[10];
extern RibbonGroupDescriptor lbl_802EF000[7];
extern u32 pokemonGetStatus();

/* Build the four-column ribbon grid used by the Pokemon status window. */
void fn_80093F64(u8* pokemon, PokemonRibbonGrid* grid)
{
    s32 ribbon;
    u32 groupIndex;
    s32 outputIndex;
    s32 row;
    s32 column;
    s32 count;

    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            grid->ribbon[row][column] = -1;
        }
    }

    outputIndex = 0;
    for (groupIndex = 0; groupIndex < 10; groupIndex++) {
        /* RULE-EXCEPTION(user-approved): count doubles as the per-group ribbon limit for register allocation — see docs/RULE_EXCEPTIONS.md */
        count = pokemonGetStatus(pokemon, 0, lbl_802EEFD8[groupIndex].selector, 0);
        if (count > lbl_802EEFD8[groupIndex].maximum) {
            count = lbl_802EEFD8[groupIndex].maximum;
        }
        for (ribbon = 0; ribbon < count; ribbon++) {
            /* RULE-EXCEPTION(user-approved): row and column reused as staging scalars for the grid index - see docs/RULE_EXCEPTIONS.md */
            row = outputIndex % 9;
            column = outputIndex / 9;
            grid->ribbon[row][column] = ribbon + lbl_802EEFD8[groupIndex].firstRibbon;
            outputIndex++;
        }
    }

    outputIndex = 0;
    for (groupIndex = 0; groupIndex < 7; groupIndex++) {
        count = pokemonGetStatus(pokemon, 0, lbl_802EF000[groupIndex].selector, 0);
        if (count > lbl_802EF000[groupIndex].maximum) {
            count = lbl_802EF000[groupIndex].maximum;
        }
        for (ribbon = 0; ribbon < count; ribbon++) {
            grid->ribbon[outputIndex][3] = ribbon + lbl_802EF000[groupIndex].firstRibbon;
            outputIndex++;
        }
    }

    count = 0;
    for (column = 0; column < 4; column++) {
        for (row = 0; row < 9; row++) {
            if (grid->ribbon[row][column] >= 0) {
                count++;
            }
        }
    }
    grid->count = count;
}
