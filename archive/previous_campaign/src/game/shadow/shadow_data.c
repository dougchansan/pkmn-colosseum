/**
 * @file shadow_data.c
 * @brief Shadow Pokemon data tables.
 *
 * This file contains the static data tables for all 48 Shadow Pokemon
 * in Pokemon Colosseum. These tables define:
 *   - Which Pokemon species are shadow Pokemon
 *   - Which trainer owns each shadow Pokemon
 *   - Initial heart gauge values
 *   - Encounter locations
 *   - Associated GSflags for encounter/snag/purify tracking
 *
 * In the original game, this data is loaded from common_rel.fdat at
 * runtime (CommonIndexes slot 80 for data, slot 81 for count). The
 * data table is stored within the common_rel file, not in the DOL.
 *
 * The entries below are reconstructed from community research,
 * hacking tools (GoD Tool / Mausoleum Tool), and the game's behavior.
 * Species IDs use the National Pokedex numbering. Trainer indices
 * reference the trainer data table in common_rel (offset 0x92ED0).
 *
 * Shadow Pokemon List (in approximate encounter order):
 *   1. Makuhita     - Trudly (Pyrite Town)
 *   2. Bayleef      - Verde (The Under)
 *   3. Quilava      - Rosso (The Under)
 *   4. Croconaw     - Bluno (The Under)
 *   5. Noctowl      - Green (Pyrite Bldg)
 *   6. Flaaffy      - Bitt (Pyrite Town)
 *   7. Skiploom     - Rider Nover (Pyrite Town)
 *   8. Quagsire     - Bandana Guy Divel (Pyrite Cave)
 *   9. Misdreavus   - Rider Vant (Pyrite Town)
 *  10. Slugma       - Roller Boy Lon (Pyrite Town)
 *  11. Furret       - Rogue Cail (Pyrite Bldg)
 *  12. Yanma        - Cipher Peon Nore (Pyrite Bldg)
 *  13. Remoraid     - Miror B. Peon Reath (Pyrite Cave)
 *  14. Mantine      - Miror B. Peon Ferma (Pyrite Cave)
 *  15. Qwilfish     - Hunter Doken (Pyrite Cave)
 *  16. Meditite     - Rider Twan (Pyrite Town)
 *  17. Dunsparce    - Rider Sosh (Pyrite Town)
 *  18. Swablu       - Hunter Zalo (Pyrite Cave)
 *  19. Sudowoodo    - Cipher Peon Tanie (Shadow Lab)
 *  20. Hitmontop    - Cipher Peon Skrub (Shadow Lab)
 *  21. Entei        - Cipher Admin Dakim (Mt. Battle)
 *  22. Ledian       - Cipher Peon Kloak (The Under)
 *  23. Suicune      - Cipher Admin Venus (The Under)
 *  24. Gligar       - Hunter Frena (The Under)
 *  25. Stantler     - Chaser Liaks (The Under)
 *  26. Piloswine    - Bodybuilder Lonia (The Under)
 *  27. Sneasel      - Rider Neban (The Under)
 *  28. Aipom        - Cipher Peon Cole (Shadow Lab)
 *  29. Murkrow      - Cipher Peon Lare (Shadow Lab)
 *  30. Forretress   - Cipher Peon Vana (Shadow Lab)
 *  31. Ariados      - Cipher Peon Lesar (Shadow Lab)
 *  32. Granbull     - Cipher Peon Tanie (Shadow Lab)
 *  33. Vibrava      - Cipher Peon Remil (Shadow Lab)
 *  34. Raikou       - Cipher Admin Ein (Shadow Lab)
 *  35. Sunflora     - Cipher Peon Snattle (Realgam Tower)
 *  36. Delibird     - Cipher Peon Arton (Realgam Tower)
 *  37. Heracross    - Cipher Peon Dioge (Realgam Tower)
 *  38. Skarmory     - Snagem Head Gonzap (Snagem Hideout)
 *  39. Miltank      - Bodybuilder Jomas (Realgam Tower)
 *  40. Absol        - Rider Delan (Realgam Tower)
 *  41. Houndoom     - Cipher Peon Nella (Realgam Tower)
 *  42. Tropius      - Cipher Peon Ston (Realgam Tower)
 *  43. Metagross    - Cipher Nascour (Realgam Tower)
 *  44. Tyranitar    - Cipher Head Evice (Realgam Tower)
 *  45. Smeargle     - Team Snagem Biden (Snagem Hideout)
 *  46. Ursaring     - Team Snagem Agrev (Snagem Hideout)
 *  47. Shuckle      - Deep King Agnol (Deep Colosseum)
 *  48. Togetic      - Cipher Peon Fein (Outskirt Stand)
 */

#include "game/shadow/shadow.h"

/* =========================================================================
 * Species index constants (National Pokedex numbers)
 * ========================================================================= */

#define SPECIES_MAKUHITA    296
#define SPECIES_BAYLEEF     153
#define SPECIES_QUILAVA     156
#define SPECIES_CROCONAW    159
#define SPECIES_NOCTOWL     164
#define SPECIES_FLAAFFY     180
#define SPECIES_SKIPLOOM    188
#define SPECIES_QUAGSIRE    195
#define SPECIES_MISDREAVUS  200
#define SPECIES_SLUGMA      218
#define SPECIES_FURRET      162
#define SPECIES_YANMA       193
#define SPECIES_REMORAID    223
#define SPECIES_MANTINE     226
#define SPECIES_QWILFISH    211
#define SPECIES_MEDITITE    307
#define SPECIES_DUNSPARCE   206
#define SPECIES_SWABLU      333
#define SPECIES_SUDOWOODO   185
#define SPECIES_HITMONTOP   237
#define SPECIES_ENTEI       244
#define SPECIES_LEDIAN      166
#define SPECIES_SUICUNE     245
#define SPECIES_GLIGAR      207
#define SPECIES_STANTLER    234
#define SPECIES_PILOSWINE   221
#define SPECIES_SNEASEL     215
#define SPECIES_AIPOM       190
#define SPECIES_MURKROW     198
#define SPECIES_FORRETRESS  205
#define SPECIES_ARIADOS     168
#define SPECIES_GRANBULL    210
#define SPECIES_VIBRAVA     329
#define SPECIES_RAIKOU      243
#define SPECIES_SUNFLORA    192
#define SPECIES_DELIBIRD    225
#define SPECIES_HERACROSS   214
#define SPECIES_SKARMORY    227
#define SPECIES_MILTANK     241
#define SPECIES_ABSOL       359
#define SPECIES_HOUNDOOM    229
#define SPECIES_TROPIUS     357
#define SPECIES_METAGROSS   376
#define SPECIES_TYRANITAR   248
#define SPECIES_SMEARGLE    235
#define SPECIES_URSARING    217
#define SPECIES_SHUCKLE     213
#define SPECIES_TOGETIC     176

/* Shadow Rush move ID (Colosseum: move index 0x0164 = 356) */
#define MOVE_SHADOW_RUSH    356

/* =========================================================================
 * Shadow Pokemon data table
 * ========================================================================= */

/**
 * The complete table of all 48 Shadow Pokemon.
 *
 * Note: The actual data in common_rel.fdat uses the game's internal
 * species indices which differ slightly from National Dex numbers.
 * Trainer indices reference the trainer table at common_rel offset
 * 0x92ED0. Heart gauge values are from community research.
 *
 * The GSflag IDs are placeholders derived from the known flag system
 * layout. Exact values require further analysis of the flag
 * definition table loaded during game initialization.
 *
 * Heart gauge values scale with game progression:
 *   - Early shadows (Pyrite): 3000-4000
 *   - Mid-game shadows (The Under, Shadow Lab): 5000-6000
 *   - Late-game shadows (Realgam Tower): 6000-8000
 *   - Legendary shadows (Entei, Suicune, Raikou): 10000+
 *   - Post-game shadows (Togetic, etc.): 5000-6000
 */
const ShadowPokemonData gShadowDataTable[SHADOW_POKEMON_COUNT] = {
    /*  1: Makuhita     */ { SPECIES_MAKUHITA,    MOVE_SHADOW_RUSH,  50,  0, 3000, 180, 30,  5, 100, 101, 102, 0 },
    /*  2: Bayleef      */ { SPECIES_BAYLEEF,     MOVE_SHADOW_RUSH,  55,  0, 3000,  45, 30,  8, 103, 104, 105, 0 },
    /*  3: Quilava      */ { SPECIES_QUILAVA,     MOVE_SHADOW_RUSH,  56,  0, 3000,  45, 30,  8, 106, 107, 108, 0 },
    /*  4: Croconaw     */ { SPECIES_CROCONAW,    MOVE_SHADOW_RUSH,  57,  0, 3000,  45, 30,  8, 109, 110, 111, 0 },
    /*  5: Noctowl      */ { SPECIES_NOCTOWL,     MOVE_SHADOW_RUSH,  60,  0, 3000,  90, 30,  5, 112, 113, 114, 0 },
    /*  6: Flaaffy      */ { SPECIES_FLAAFFY,     MOVE_SHADOW_RUSH,  62,  0, 3000, 120, 30,  5, 115, 116, 117, 0 },
    /*  7: Skiploom     */ { SPECIES_SKIPLOOM,    MOVE_SHADOW_RUSH,  63,  0, 3000, 120, 30,  5, 118, 119, 120, 0 },
    /*  8: Quagsire     */ { SPECIES_QUAGSIRE,    MOVE_SHADOW_RUSH,  65,  0, 3500,  90, 30,  6, 121, 122, 123, 0 },
    /*  9: Misdreavus   */ { SPECIES_MISDREAVUS,  MOVE_SHADOW_RUSH,  67,  0, 3500,  45, 30,  5, 124, 125, 126, 0 },
    /* 10: Slugma       */ { SPECIES_SLUGMA,      MOVE_SHADOW_RUSH,  68,  0, 3500, 190, 30,  5, 127, 128, 129, 0 },
    /* 11: Furret       */ { SPECIES_FURRET,      MOVE_SHADOW_RUSH,  70,  0, 3500,  90, 33,  5, 130, 131, 132, 0 },
    /* 12: Yanma        */ { SPECIES_YANMA,       MOVE_SHADOW_RUSH,  72,  0, 4000,  75, 33,  5, 133, 134, 135, 0 },
    /* 13: Remoraid     */ { SPECIES_REMORAID,    MOVE_SHADOW_RUSH,  75,  0, 4000, 190, 20,  6, 136, 137, 138, 0 },
    /* 14: Mantine      */ { SPECIES_MANTINE,     MOVE_SHADOW_RUSH,  76,  0, 4000,  25, 20,  6, 139, 140, 141, 0 },
    /* 15: Qwilfish     */ { SPECIES_QWILFISH,    MOVE_SHADOW_RUSH,  78,  0, 4000,  45, 20,  6, 142, 143, 144, 0 },
    /* 16: Meditite     */ { SPECIES_MEDITITE,    MOVE_SHADOW_RUSH,  80,  0, 4000,  90, 33,  5, 145, 146, 147, 0 },
    /* 17: Dunsparce    */ { SPECIES_DUNSPARCE,   MOVE_SHADOW_RUSH,  82,  0, 4000, 190, 33,  5, 148, 149, 150, 0 },
    /* 18: Swablu       */ { SPECIES_SWABLU,      MOVE_SHADOW_RUSH,  83,  0, 4000, 255, 33,  6, 151, 152, 153, 0 },
    /* 19: Sudowoodo    */ { SPECIES_SUDOWOODO,   MOVE_SHADOW_RUSH, 100,  0, 5000,  65, 35, 10, 154, 155, 156, 0 },
    /* 20: Hitmontop    */ { SPECIES_HITMONTOP,   MOVE_SHADOW_RUSH, 101,  0, 5000,  45, 38, 10, 157, 158, 159, 0 },
    /* 21: Entei        */ { SPECIES_ENTEI,       MOVE_SHADOW_RUSH, 110,  0, 13000,  3, 40, 12, 160, 161, 162, 0 },
    /* 22: Ledian       */ { SPECIES_LEDIAN,      MOVE_SHADOW_RUSH, 115,  0, 5000,  90, 40,  8, 163, 164, 165, 0 },
    /* 23: Suicune      */ { SPECIES_SUICUNE,     MOVE_SHADOW_RUSH, 120,  0, 13000,  3, 40, 12, 166, 167, 168, 0 },
    /* 24: Gligar       */ { SPECIES_GLIGAR,      MOVE_SHADOW_RUSH, 122,  0, 5500,  60, 43,  8, 169, 170, 171, 0 },
    /* 25: Stantler     */ { SPECIES_STANTLER,    MOVE_SHADOW_RUSH, 124,  0, 5500,  45, 43,  8, 172, 173, 174, 0 },
    /* 26: Piloswine    */ { SPECIES_PILOSWINE,   MOVE_SHADOW_RUSH, 126,  0, 5500,  75, 43,  8, 175, 176, 177, 0 },
    /* 27: Sneasel      */ { SPECIES_SNEASEL,     MOVE_SHADOW_RUSH, 128,  0, 5500,  60, 43,  8, 178, 179, 180, 0 },
    /* 28: Aipom        */ { SPECIES_AIPOM,       MOVE_SHADOW_RUSH, 130,  0, 5000,  45, 43, 10, 181, 182, 183, 0 },
    /* 29: Murkrow      */ { SPECIES_MURKROW,     MOVE_SHADOW_RUSH, 132,  0, 5000,  30, 43, 10, 184, 185, 186, 0 },
    /* 30: Forretress   */ { SPECIES_FORRETRESS,  MOVE_SHADOW_RUSH, 134,  0, 5500,  75, 43, 10, 187, 188, 189, 0 },
    /* 31: Ariados      */ { SPECIES_ARIADOS,     MOVE_SHADOW_RUSH, 136,  0, 5500,  90, 43, 10, 190, 191, 192, 0 },
    /* 32: Granbull     */ { SPECIES_GRANBULL,    MOVE_SHADOW_RUSH, 138,  0, 5500,  75, 43, 10, 193, 194, 195, 0 },
    /* 33: Vibrava      */ { SPECIES_VIBRAVA,     MOVE_SHADOW_RUSH, 140,  0, 6000,  45, 43, 10, 196, 197, 198, 0 },
    /* 34: Raikou       */ { SPECIES_RAIKOU,      MOVE_SHADOW_RUSH, 145,  0, 13000,  3, 40, 12, 199, 200, 201, 0 },
    /* 35: Sunflora     */ { SPECIES_SUNFLORA,    MOVE_SHADOW_RUSH, 200,  0, 6000, 120, 45, 15, 202, 203, 204, 0 },
    /* 36: Delibird     */ { SPECIES_DELIBIRD,    MOVE_SHADOW_RUSH, 202,  0, 6000,  45, 45, 15, 205, 206, 207, 0 },
    /* 37: Heracross    */ { SPECIES_HERACROSS,   MOVE_SHADOW_RUSH, 204,  0, 6500,  45, 45, 15, 208, 209, 210, 0 },
    /* 38: Skarmory     */ { SPECIES_SKARMORY,    MOVE_SHADOW_RUSH, 210,  0, 7000,  25, 47, 18, 211, 212, 213, 0 },
    /* 39: Miltank      */ { SPECIES_MILTANK,     MOVE_SHADOW_RUSH, 215,  0, 6500,  45, 48, 15, 214, 215, 216, 0 },
    /* 40: Absol        */ { SPECIES_ABSOL,       MOVE_SHADOW_RUSH, 218,  0, 7000,  30, 48, 15, 217, 218, 219, 0 },
    /* 41: Houndoom     */ { SPECIES_HOUNDOOM,    MOVE_SHADOW_RUSH, 220,  0, 7000,  45, 48, 15, 220, 221, 222, 0 },
    /* 42: Tropius      */ { SPECIES_TROPIUS,     MOVE_SHADOW_RUSH, 222,  0, 7000, 200, 49, 15, 223, 224, 225, 0 },
    /* 43: Metagross    */ { SPECIES_METAGROSS,   MOVE_SHADOW_RUSH, 250,  0, 8000,   3, 50, 16, 226, 227, 228, 0 },
    /* 44: Tyranitar    */ { SPECIES_TYRANITAR,   MOVE_SHADOW_RUSH, 255,  0, 8000,  45, 55, 16, 229, 230, 231, 0 },
    /* 45: Smeargle     */ { SPECIES_SMEARGLE,    MOVE_SHADOW_RUSH, 260,  0, 5000,  45, 45, 18, 232, 233, 234, 0 },
    /* 46: Ursaring     */ { SPECIES_URSARING,    MOVE_SHADOW_RUSH, 262,  0, 6000,  60, 45, 18, 235, 236, 237, 0 },
    /* 47: Shuckle      */ { SPECIES_SHUCKLE,     MOVE_SHADOW_RUSH, 270,  0, 5000, 190, 45, 20, 238, 239, 240, 0 },
    /* 48: Togetic      */ { SPECIES_TOGETIC,     MOVE_SHADOW_RUSH, 280,  0, 6000,  75, 20, 22, 241, 242, 243, 0 },
};

/* =========================================================================
 * Shadow Pokemon encounter location strings (for debug/reference)
 * ========================================================================= */

/**
 * Location names for debug display.
 * These correspond to the floorID field in ShadowPokemonData.
 * In the actual game, locations are resolved via the floor system.
 */
static const char* const sShadowLocationNames[] = {
    /* Floor  5 */ "Pyrite Town",
    /* Floor  6 */ "Pyrite Cave",
    /* Floor  8 */ "The Under",
    /* Floor 10 */ "Shadow Pokemon Lab",
    /* Floor 12 */ "Mt. Battle",
    /* Floor 15 */ "Realgam Tower",
    /* Floor 16 */ "Realgam Colosseum",
    /* Floor 18 */ "Snagem Hideout",
    /* Floor 20 */ "Deep Colosseum",
    /* Floor 22 */ "Outskirt Stand",
};

/**
 * Number of location names.
 */
static const u32 sShadowLocationCount =
    sizeof(sShadowLocationNames) / sizeof(sShadowLocationNames[0]);

/* =========================================================================
 * Purification replacement move table
 * ========================================================================= */

/**
 * When a Shadow Pokemon is purified, Shadow Rush is replaced with
 * a species-specific normal move. This table maps species to the
 * replacement move. The move chosen is typically one the Pokemon
 * would learn near its current level.
 *
 * Note: Actual move IDs would need to be derived from the common_rel
 * move data table. The values below are representative placeholders
 * based on known game behavior.
 */
typedef struct ShadowReplacementMove {
    u16  species;
    u16  replacementMoveID;
} ShadowReplacementMove;

/**
 * Replacement moves are resolved at purification time by looking up
 * the Pokemon's level-up move list from the species stats table in
 * common_rel. The game finds the highest-level move the Pokemon would
 * have learned at or below its current level that it doesn't already
 * know. This is handled by shadow_GetReplacementMove() in shadow.c
 * via the species data access functions.
 *
 * No static table is needed here -- the replacement is dynamic.
 */
