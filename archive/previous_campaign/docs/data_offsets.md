# Data Offset Reference — Pokemon Colosseum (GC6E01, NTSC-U)

Cross-reference of community-documented data offsets with our decomp symbol map.
Primary sources: StarsMmd's Project Pokemon tutorials, PekanMmd's GoD Tool source code,
PokeCommunity XD info dump, gc-forever AR/Gecko codes, and TuxSH's PkmGCTools.

---

## Table of Contents

1. [Memory Layout Overview](#memory-layout-overview)
2. [DOL Data Tables (start.dol)](#dol-data-tables-startdol)
3. [common_rel.fdat Data Tables](#common_relfdat-data-tables)
4. [Pokemon Stats Structure](#pokemon-stats-structure-0x11c-bytes)
5. [Move Data Structure](#move-data-structure-0x38-bytes)
6. [Trainer Data Structure](#trainer-data-structure-0x34-bytes)
7. [Trainer Pokemon Structure](#trainer-pokemon-structure-0x50-bytes)
8. [Item Data Structure (XD reference)](#item-data-structure-xd-reference-0x28-bytes)
9. [Nature Data Structure](#nature-data-structure-0x28-bytes)
10. [Type Data / Effectiveness Table](#type-data--effectiveness-table-0x30-bytes)
11. [Evolution Structure](#evolution-structure-0x06-bytes)
12. [String Tables](#string-tables)
13. [Save File Pokemon Structure](#save-file-pokemon-structure-0x138-bytes)
14. [Gift/Starter Pokemon (DOL)](#giftstarter-pokemon-dol)
15. [Action Replay / Gecko Code Addresses](#action-replay--gecko-code-addresses)
16. [Symbol Map Cross-Reference](#symbol-map-cross-reference)
17. [Sources](#sources)

---

## Memory Layout Overview

Our DOL (main.dol) sections map to RAM as follows:

| Section   | RAM Start      | RAM End (approx) | Size         | Description                              |
|-----------|----------------|-------------------|--------------|------------------------------------------|
| `.init`   | `0x80003100`   | `0x800055CF`      | ~9 KB        | CRT startup, MetroTRK debug kernel       |
| `.text`   | `0x800055E0`   | `0x8026635C`      | ~2.4 MB      | Executable code                          |
| `.ctors`  | `0x80266360`   | `0x80266380`      | 0x20         | C++ static constructors                  |
| `.dtors`  | `0x80266380`   | `0x8026639F`      | 0x20         | C++ static destructors                   |
| `.rodata` | `0x802663A0`   | `0x8027A4F0`      | ~82 KB       | Read-only data (strings, LUTs, floats)   |
| `.data`   | `0x8027A500`   | `0x8039A6DB`      | ~1.13 MB     | Initialized data (scene tables, etc.)    |
| `.bss`    | `0x8039A700`   | ...               | varies       | Uninitialized data (stacks, buffers)     |
| `.sdata`  | `0x80478820`   | `0x80478DC0`      | ~1.4 KB      | Small initialized data (global vars)     |
| `.sbss`   | `0x80478DC0`   | ...               | varies       | Small uninitialized data                 |
| `.sdata2` | `0x8047B6A0`   | ...               | varies       | Small read-only data (float consts)      |
| `.sbss2`  | `0x8047E700`   | ...               | varies       | Small read-only BSS                      |

**Important**: The community-documented "DOL file offsets" (e.g., 0x365018 for TMs) refer to
byte positions within the start.dol *file* on disc, NOT RAM addresses. The DOL header maps
each section's file offset to its RAM load address. Our symbol map uses RAM addresses
(0x80xxxxxx prefix).

**common_rel.fdat** is loaded dynamically from `common.fsys` at runtime into a heap
allocation; its offsets are relative to the start of that file, not fixed RAM addresses.

---

## DOL Data Tables (start.dol)

These are stored in the DOL file and loaded directly into RAM. Community-documented
DOL file offsets are listed; corresponding RAM addresses require DOL header analysis.

### TM/HM Table
- **DOL file offset**: `0x365018` (Colosseum), `0x4023A0` (XD)
- **Entry size**: 8 bytes (6 bytes padding + 2-byte move ID)
- **Count**: 50 TMs + 8 HMs
- **HM flag**: First byte is `0x01` for HMs, `0x00` for TMs
- **Search pattern**: `00 00 00 00 00 00 01 08 00 00 00 00 00 00 01 51`
  (Focus Punch = 0x0108, Dragon Claw = 0x0151)

### DOL String Table
- **DOL file offset**: `0x2CC810` (Colosseum), `0x374FC0` (XD)
- This is a string table embedded in the DOL, separate from common_rel strings.

### Gift/Starter Pokemon Data
See [Gift/Starter Pokemon section](#giftstarter-pokemon-dol) below.

---

## common_rel.fdat Data Tables

The `common_rel.fdat` file is the first file inside `common.fsys`. It contains the bulk
of game data tables accessed via a pointer/index system. The GoD Tool's `CommonIndexes`
enum (from CMRelIndexes.swift) maps 108 index slots to data tables.

### Colosseum common_rel Table Index (CommonIndexes)

All offsets below are relative to the start of common_rel.fdat.

| Index | Name                    | Offset       | Entry Size | Count  | Description                   |
|-------|-------------------------|--------------|------------|--------|-------------------------------|
| 2     | LegendaryPokemon        | —            | —          | idx 3  | Legendary Pokemon entries     |
| 6     | PeopleIDs               | —            | —          | —      | NPC ID table                  |
| 14    | Rooms                   | —            | —          | idx 15 | Room/floor definitions        |
| 24    | TrainerClasses          | —            | 0x0C       | idx 25 | Trainer class definitions     |
| 28    | BattleFields            | —            | —          | idx 29 | Battle arena definitions      |
| 32    | BattleTypes             | —            | —          | idx 33 | Battle type definitions       |
| 42    | BattleStyles            | —            | —          | idx 43 | Battle style definitions      |
| 44    | Trainers                | `0x92ED0`    | 0x34       | 819    | Trainer data table            |
| 46    | TrainerAIData           | —            | —          | idx 47 | AI behavior data              |
| 48    | TrainerPokemonData      | `0x9FE28`    | 0x50       | 5510   | Trainer Pokemon entries        |
| 50    | Battles                 | —            | —          | idx 51 | Battle encounter definitions  |
| 52    | SoundFiles              | —            | —          | idx 53 | Sound file references         |
| 56    | AIWeightEffects         | —            | —          | idx 57 | AI weight effect data         |
| 58    | AIPokemonRoles          | —            | —          | idx 59 | AI Pokemon role assignments   |
| 60    | TreasureBoxData         | —            | —          | idx 61 | Treasure/item box data        |
| 62    | Moves                   | `0x11E048`   | 0x38       | idx 63 | Move data table               |
| 64    | Natures                 | —            | 0x28       | 25     | Nature definitions            |
| 68    | PokemonStats            | `0x12336C`   | 0x11C      | idx 69 | Pokemon base stat table       |
| 70    | NatureMultipliers       | —            | —          | idx 71 | Nature stat multiplier table  |
| 72    | CharacterModels         | —            | —          | idx 73 | Character 3D model refs       |
| 80    | ShadowData              | —            | —          | idx 81 | Shadow Pokemon definitions    |
| 82    | PokemonMetLocations     | —            | —          | idx 83 | Met location strings          |
| 86    | InteractionPoints       | —            | —          | idx 87 | Warp/interaction point defs   |
| 98    | StringTable1            | `0x59890`    | —          | —      | First string table            |
| 99    | StringTable2            | —            | —          | —      | Second string table           |
| 100   | StringTable3            | —            | —          | —      | Third string table            |
| 101   | Script                  | —            | —          | —      | Script data (US region)       |

**Note**: `NumberOfItems = -1` — items are stored in the DOL, not in common_rel (Colosseum).

### XD common_rel Offsets (for comparison)

| Table              | XD Offset    | Entry Size | Count   |
|--------------------|--------------|------------|---------|
| First Item         | `0x1FEE4`   | 0x28       | 0x01BC  |
| First Move         | `0xA2710`   | 0x38       | 0x0177  |
| First Nature       | `0x47728`   | 0x28       | 0x19    |
| First Trainer Class| `0xEA40`    | 0x0C       | 0x33    |
| First Tutor Move   | `0xA7918`   | 0x0C       | 0x0C    |
| First Type         | `0xA7C30`   | 0x30       | 0x12    |
| Ribbons Table      | `0x47BA0`   | 0x08       | —       |
| Battle Bingo Card  | `0x1CAF`    | 0xB6       | 0x0B    |
| PokeSpot Pokemon   | `0x2FAC`    | 0x0C       | 0x0B    |
| String Table       | `0x4E274`   | —          | —       |
| Pokemon Stats      | `0x29ECC`   | 0x124      | —       |

---

## Pokemon Stats Structure (0x11C bytes)

Location in common_rel.fdat: offset `0x12336C` (first entry = Bulbasaur).
Each of the ~413 Pokemon entries is 0x11C (284) bytes.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Level-up Rate       | Growth curve ID                          |
| 0x01   | 1 byte  | Catch Rate          | Base capture rate                        |
| 0x02   | 1 byte  | Gender Ratio        | Male/female distribution                 |
| 0x07   | 1 byte  | Base EXP Reward     | EXP yield on defeat                      |
| 0x09   | 1 byte  | Base Happiness      | Starting friendship value                |
| 0x0A   | 1 byte  | Height (feet)       | —                                        |
| 0x0B   | 1 byte  | Height (inches)     | —                                        |
| 0x0C   | 2 bytes | Weight (lbs x10)    | e.g., 0x64 = 10.0 lbs                   |
| 0x0E   | 2 bytes | Cry ID              | Sound ID for Pokemon cry                 |
| 0x17   | 1 byte  | Egg Cycles          | Steps to hatch / 256                     |
| 0x1A   | 2 bytes | Name String ID      | String table reference for name          |
| 0x1E   | 2 bytes | Species Name ID     | e.g., "Seed Pokemon"                     |
| 0x2E   | 2 bytes | Model ID            | 3D model asset reference                 |
| 0x30   | 1 byte  | Type 1              | Primary type                             |
| 0x31   | 1 byte  | Type 2              | Secondary type (same as Type 1 if mono)  |
| 0x32   | 1 byte  | Ability 1           | Primary ability                          |
| 0x33   | 1 byte  | Ability 2           | Secondary ability (0 if none)            |
| 0x34   | varies  | TM Compatibility     | TM/HM learnset bitfield                 |
| 0x6E   | 1 byte  | Egg Group 1         | Breeding group 1                         |
| 0x6F   | 1 byte  | Egg Group 2         | Breeding group 2                         |
| 0x70   | 1 byte  | Wild Held Item 1    | Common held item                         |
| 0x72   | 2 bytes | Wild Held Item 2    | Rare held item                           |
| 0x85   | 1 byte  | Base HP             | —                                        |
| 0x87   | 1 byte  | Base Attack         | —                                        |
| 0x89   | 1 byte  | Base Defense        | —                                        |
| 0x8B   | 1 byte  | Base Sp. Attack     | —                                        |
| 0x8D   | 1 byte  | Base Sp. Defense    | —                                        |
| 0x8F   | 1 byte  | Base Speed          | —                                        |
| 0x91   | varies  | EV Yield            | EVs awarded on defeat                    |
| 0x9C   | 6 bytes | Evolution 1         | Method(2) + Condition(2) + Species(2)    |
| 0xA2   | 6 bytes | Evolution 2         | (if dual evolution)                      |
| 0xBA   | 4 bytes | Level-up Move 1     | Level(1) + unused(1) + MoveID(2)         |
|        |         | ...                 | Up to 20 level-up moves, 4 bytes each   |

**Note**: Base stats use odd offsets (0x85, 0x87, ...) with gaps — likely padded struct fields.

---

## Move Data Structure (0x38 bytes)

Location in common_rel.fdat: offset `0x11E048` (first entry = Pound).
Each of the ~375 moves is 0x38 (56) bytes. Shadow moves start at index 0x0164.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Priority            | Move priority (255 = -1, etc.)           |
| 0x01   | 1 byte  | Base PP             | Power Points                             |
| 0x02   | 1 byte  | Type                | Type index                               |
| 0x03   | 1 byte  | Targets             | Target selection flags                   |
| 0x04   | 1 byte  | Accuracy            | Hit rate (0-100)                         |
| 0x05   | 1 byte  | Effect Accuracy     | Secondary effect trigger chance           |
| 0x06   | 1 byte  | Makes Contact       | 1 = contact move                         |
| 0x07   | 1 byte  | Blocked by Protect  | 1 = blocked                              |
| 0x08   | 1 byte  | Magic Coat Reflects | 1 = reflectable                          |
| 0x09   | 1 byte  | Snatch Steals       | 1 = snatchable                           |
| 0x0A   | 1 byte  | Mirror Move Copies  | 1 = copyable                             |
| 0x0B   | 1 byte  | King's Rock Flinch  | 1 = can cause flinch w/ King's Rock      |
| 0x10   | 1 byte  | Sound-Based         | 1 = sound move (Soundproof blocks)       |
| 0x12   | 1 byte  | HM / Shadow Flag    | 1 = HM move (or shadow flag in XD)       |
| 0x13   | 1 byte  | Recoil              | Recoil damage flag                       |
| 0x17   | 1 byte  | Base Power          | Damage output                            |
| 0x1B   | 1 byte  | Effect              | Effect index (Gen III effect IDs)        |
| 0x1C   | 2 bytes | Effect ID (alt)     | 16-bit effect ID (Code snippets.swift)   |
| 0x22   | 2 bytes | Move Name ID        | String table reference                   |
| 0x2E   | 2 bytes | Description Text ID | String table reference                   |
| 0x32   | 2 bytes | Animation ID        | Battle animation reference               |

---

## Trainer Data Structure (0x34 bytes)

Location in common_rel.fdat: offset `0x92ED0` (first trainer entry).
819 entries, each 0x34 (52) bytes.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Gender              | 0 = male, 1 = female                    |
| 0x01   | 1 byte  | Padding             | Always 0                                 |
| 0x03   | 2 bytes | Trainer Class       | Index into trainer class table           |
| 0x04   | 2 bytes | First Pokemon Index | Index into trainer Pokemon table          |
| 0x06   | 2 bytes | AI Value            | AI behavior flags                        |
| 0x08   | 4 bytes | Name String ID      | String table reference                   |
| 0x0C   | 4 bytes | Opening Anim Index  | Send-out animation                       |
| 0x10   | 4 bytes | Model Index         | 3D model reference                       |
| 0x14   | 16 bytes| Item Slots 1-8      | 2 bytes each, 8 held items for battle    |
| 0x24   | 4 bytes | Pre-battle String   | String table ID                          |
| 0x28   | 4 bytes | Win String          | String table ID                          |
| 0x2C   | 4 bytes | Lose String         | String table ID                          |
| 0x30   | 4 bytes | Alt Lose String     | String table ID                          |

---

## Trainer Pokemon Structure (0x50 bytes)

Location in common_rel.fdat: offset `0x9FE28` (first trainer Pokemon entry).
5510 entries, each 0x50 (80) bytes.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Ability Slot        | 0 or 1 (which ability)                  |
| 0x01   | 1 byte  | Gender              | 0=male, 1=female, 2=genderless          |
| 0x02   | 1 byte  | Nature              | Nature index (0-24)                      |
| 0x03   | 1 byte  | Shadow ID           | Shadow Pokemon ID (0 = not shadow)       |
| 0x04   | 1 byte  | Level               | Pokemon level                            |
| 0x05   | 1 byte  | Send-out Priority   | Battle send-out order                    |
| 0x0A   | 2 bytes | Species             | Pokemon species index                    |
| 0x0C   | 2 bytes | Poke Ball           | Ball caught in                           |
| 0x10   | 2 bytes | Item Flag           | —                                        |
| 0x12   | 2 bytes | Held Item           | Item index                               |
| 0x14   | 4 bytes | Nickname String ID  | String table reference                   |
| 0x1C   | 6 bytes | IVs                 | 1 byte each: HP/Atk/Def/SpA/SpD/Spe     |
| 0x22   | 12 bytes| EVs                 | 2 bytes each: HP/Atk/Def/SpA/SpD/Spe    |
| 0x30   | 1 byte  | Move 1 PP Bonuses   | PP Up count for move 1                   |
| 0x36   | 2 bytes | Move 1 Index        | Move ID                                  |
| 0x38   | 1 byte  | Move 2 PP Bonuses   | —                                        |
| 0x3E   | 2 bytes | Move 2 Index        | —                                        |
| 0x40   | 1 byte  | Move 3 PP Bonuses   | —                                        |
| 0x46   | 2 bytes | Move 3 Index        | —                                        |
| 0x48   | 1 byte  | Move 4 PP Bonuses   | —                                        |
| 0x4E   | 2 bytes | Move 4 Index        | —                                        |

---

## Item Data Structure (XD reference, 0x28 bytes)

**In Colosseum, items are stored in the DOL, NOT in common_rel** (`NumberOfItems = -1` in
CommonIndexes). The structure below is from XD (offset `0x1FEE4`, count 0x01BC = 444),
but Colosseum likely uses a similar layout.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Pocket              | Bag pocket (key items, berries, etc.)    |
| 0x06   | 2 bytes | Price               | Buy price in Pokedollars                 |
| 0x0B   | 1 byte  | Battle Hold Effect  | Effect when held in battle               |
| 0x12   | 2 bytes | Name String ID      | String table reference                   |
| 0x16   | 2 bytes | Description ID      | String table reference                   |
| 0x24   | 3 bytes | Friendship Effects  | 3 signed Int8 values                     |

Item categories (ItemCategoryIndex): Pokeballs(1), Regular(2), Berries(3), TMs(4),
Key Items(5), Colognes(6), Battle CDs(7).

---

## Nature Data Structure (0x28 bytes)

XD offset: `0x47728`, 25 entries. Colosseum uses index 64 in CommonIndexes.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 1 byte  | Purification: Battle| Rate for battle purification             |
| 0x01   | 1 byte  | Purification: Walk  | Rate for walking purification            |
| 0x02   | 1 byte  | Purification: Call  | Rate for calling purification            |
| 0x03   | 1 byte  | Purification: DayCare| Rate for day care purification          |
| 0x04   | 1 byte  | Purification: Cologne| Rate for cologne purification           |
| 0x05   | 1 byte  | Attack Modifier     | +/- for Attack stat                      |
| 0x06   | 1 byte  | Defense Modifier    | +/- for Defense stat                     |
| 0x07   | 1 byte  | Sp.Atk Modifier     | +/- for Sp. Attack stat                  |
| 0x08   | 1 byte  | Sp.Def Modifier     | +/- for Sp. Defense stat                 |
| 0x09   | 1 byte  | Speed Modifier      | +/- for Speed stat                       |
| 0x16   | 2 bytes | Name String ID      | String table reference                   |

---

## Type Data / Effectiveness Table (0x30 bytes)

XD offset: `0xA7C30`, 18 entries (one per type). Colosseum uses a similar structure.

Each type entry is 0x30 (48) bytes. Starting at offset 0x0D within each entry, there is
a byte for each of the 18 types indicating effectiveness when *this* type attacks *that* type.

| Value | Meaning           |
|-------|--------------------|
| 0x3F  | Neutral (1x)       |
| 0x41  | Super Effective (2x)|
| 0x42  | Not Very Effective (0.5x)|
| 0x43  | Ineffective (0x)   |

**Code snippets.swift reference**: Type matchup base at `0x80CD89AC` (this appears to be
an XD RAM address). Offset calculation: `(type * 0x30) + field_offset`.

---

## Evolution Structure (0x06 bytes)

Within each Pokemon stats entry, starting at offset 0x9C.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 2 bytes | Method              | Evolution trigger type                   |
| 0x02   | 2 bytes | Condition           | Level/item/friendship threshold          |
| 0x04   | 2 bytes | Evolved Species     | Target Pokemon index                     |

Method 0x10 = XD-exclusive Eevee Sun/Moon Shard evolution.

---

## String Tables

### String Table Format

String tables begin with a header:
- Offset 0x04: 2-byte entry count
- Offset 0x06: 2-byte language code (e.g., "US" = 0x5553)

Following the header, each entry is 8 bytes:
- 4-byte string ID
- 4-byte offset (relative to table start) pointing to the string data

All text uses **Unicode (2 bytes per character)**. Control sequences start with `0xFFFF`:
- Most use 3 bytes total
- Some (0x07, 0x08, 0x09, 0x38, 0x52-53, 0x5B-5C) use 4 bytes
- `0xFFFF08` = color change (RGBA format)
- `0xFFFF38` = predefined palette color

### String Table Locations

| Table                 | Colosseum Offset | XD Offset   | Notes                      |
|-----------------------|------------------|-------------|----------------------------|
| common_rel StringTable1 | `0x59890`      | `0x4E274`   | Main game strings          |
| DOL String Table      | `0x2CC810`       | `0x374FC0`  | DOL-embedded strings       |
| Map-specific tables   | varies           | varies      | 3rd file in map .fsys      |

### Key String ID Ranges

| Element                | First String ID |
|------------------------|-----------------|
| First Ability Name     | 0xC1D           |
| First Ability Desc     | 0xCE5           |
| First Trainer Class    | 0x1B59          |
| Max String ID (approx) | 0xEA5F         |

---

## Save File Pokemon Structure (0x138 bytes)

From TuxSH's PkmGCTools (LibPkmGC). This is the *save file* representation,
distinct from the common_rel stats table and trainer Pokemon table.

| Offset | Size    | Field               | Notes                                    |
|--------|---------|---------------------|------------------------------------------|
| 0x00   | 2 bytes | Species Index       | Pokemon species                          |
| 0x02   | 2 bytes | Unknown             | 0 on shadow Pokemon                      |
| 0x04   | 4 bytes | PID                 | Personality ID                           |
| 0x08   | 4 bytes | Version Info        | Game version metadata                    |
| 0x0C   | 2 bytes | Location Caught     | Met location ID                          |
| 0x0E   | 1 byte  | Level Met           | Level when caught                        |
| 0x0F   | 1 byte  | Ball Caught With    | Poke Ball type                           |
| 0x10   | 1 byte  | OT Gender           | Original Trainer gender                  |
| 0x14   | 2 bytes | SID                 | Secret ID                                |
| 0x16   | 2 bytes | TID                 | Trainer ID                               |
| 0x18   | 11 bytes| OT Name             | Original Trainer name (Unicode)          |
| 0x2E   | 11 bytes| Pokemon Name        | Nickname (Unicode)                       |
| 0x5C   | 4 bytes | Experience          | Current EXP                              |
| 0x60   | 1 byte  | Current Level       | Derived from EXP                         |
| 0x78   | 16 bytes| Moves Info          | 4 moves with PP data                     |
| 0x88   | 2 bytes | Held Item           | Item index                               |
| 0x8A   | 2 bytes | Current HP          | Current hit points                       |
| 0x8C   | 12 bytes| Stats               | u16[6]: HP/Atk/Def/SpA/SpD/Spe          |
| 0x98   | 12 bytes| EVs                 | u16[6]                                   |
| 0xA4   | 12 bytes| IVs                 | u16[6]                                   |
| 0xB0   | 2 bytes | Happiness           | Friendship value                         |
| 0xB2   | 5 bytes | Contest Stats       | Cool/Beauty/Cute/Smart/Tough             |
| 0xB7   | 5 bytes | Contest Ribbons     | Contest ribbon data                      |
| 0xBC   | 1 byte  | Contest Luster      | Luster/sheen value                       |
| 0xBD   | 12 bytes| Special Ribbons     | Special ribbon flags                     |
| 0xCA   | 1 byte  | Pokerus Status      | Infection status                         |
| 0xCB   | 3 bytes | Flags               | Egg/ability/validity flags               |
| 0xD8   | 2 bytes | Shadow Pokemon ID   | Shadow ID (0 if not shadow)              |
| 0xDC   | 4 bytes | Purification Counter| Shadow purification progress (signed)    |
| 0xE0   | 4 bytes | EXP Stored          | Experience stored during shadow state    |
| 0xF8   | 1 byte  | Obedient            | Obedience flag                           |
| 0xFB   | 1 byte  | Encounter Type      | How the Pokemon was obtained             |

---

## Gift/Starter Pokemon (DOL)

These are hardcoded in start.dol. Community-documented DOL file offsets (Colosseum):

| Pokemon          | DOL File Offset | Hex Pattern    |
|------------------|-----------------|----------------|
| Espeon           | `0x12DAC8`      | `388000C4`     |
| Umbreon          | `0x12DBF0`      | `388000C5`     |
| Duking's Plusle  | `0x12D9C8`      | `38800161`     |
| Mt. Battle Ho-Oh | `0x12D8E4`      | `388000FA`     |
| Agate Celebi     | `0x12D6B4`      | `388000FB`     |
| Agate Pikachu    | `0x12D7C4`      | `38800019`     |

**Internal structure** (relative to each entry's base):
- +0x02: Species (2 bytes)
- +0x07: Level (1 byte)
- +0x16: Move 1 (2 bytes) — Espeon/Umbreon only
- +0x26: Move 2 (2 bytes) — Espeon/Umbreon only
- +0x36: Move 3 (2 bytes) — Espeon/Umbreon only
- +0x46: Move 4 (2 bytes) — Espeon/Umbreon only

Other gift Pokemon (Plusle, Ho-Oh, Celebi, Pikachu) auto-generate moves from level-up.

---

## Action Replay / Gecko Code Addresses

These RAM addresses from AR/Gecko codes reveal runtime data locations:

### Battle/Gameplay
| Address        | Description                          |
|----------------|--------------------------------------|
| `0x80478F14`   | Trainer Pokemon base (Phenac battle) |
| `0x8047ADB8`   | Party Pokemon Slot 1                 |
| `0x0420DADC`   | Battle encounter modifier (AR)       |
| `0x040FF5AC`   | Room/location loader (AR)            |

### Pokemon Generation
| Address        | Description                          |
|----------------|--------------------------------------|
| `0x04124424`   | Global shiny/gender modifier         |
| `0x04124428`   | Global nature modifier               |
| `0x04130B24`   | Espeon shiny flag                    |
| `0x04130C4C`   | Umbreon shiny flag                   |

### Currency
| Address        | Description                          |
|----------------|--------------------------------------|
| `0x0412A880`   | PokeDollars (stores at `0x804EB2A8`) |
| `0x0412A838`   | PokeCoupons                          |

### Audio
| Address        | Description                          |
|----------------|--------------------------------------|
| `0x06165B1C`   | BGM modifier (song IDs 0x00-0x4E)   |

---

## Symbol Map Cross-Reference

Our symbol table (`config/GC6E01/symbols.txt`) contains 14,601 entries. Most are
auto-generated labels (`lbl_*`, `fn_*`, `jumptable_*`). Below are the large data
objects in `.data` that likely correspond to game data tables.

### Large .data Objects (potential game data)

| Symbol              | Address        | Size      | Possible Identity                     |
|---------------------|----------------|-----------|---------------------------------------|
| `lbl_8027A500`      | `0x8027A500`   | 0x42D60   | **Scene/floor data table** (~273 KB)  |
| `lbl_802BD260`      | `0x802BD260`   | 0x3A50    | Floor/character data (~15 KB)         |
| `lbl_802C0CB0`      | `0x802C0CB0`   | 0xEB60    | Large data table (~60 KB)             |
| `lbl_802CF810`      | `0x802CF810`   | 0x124E0   | Large data table (~75 KB)             |
| `lbl_802E2DB8`      | `0x802E2DB8`   | 0x1DC0    | Data table (~7.6 KB)                  |
| `lbl_802E6428`      | `0x802E6428`   | 0x18C0    | Data table (~6.3 KB)                  |
| `lbl_802E7CE8`      | `0x802E7CE8`   | 0x5C70    | Data table (~23 KB)                   |
| `lbl_802EF0A8`      | `0x802EF0A8`   | 0x20EF8   | **Large data table** (~135 KB)        |
| `lbl_803156E0`      | `0x803156E0`   | 0xFE00    | Data table (~65 KB)                   |
| `lbl_803254E0`      | `0x803254E0`   | 0x10300   | Data table (~66 KB)                   |
| `lbl_803357E0`      | `0x803357E0`   | 0x2A0A    | Data table (~10.8 KB)                 |
| `lbl_803381EA`      | `0x803381EA`   | 0x22E76   | **Very large data** (~143 KB)         |
| `lbl_8035C430`      | `0x8035C430`   | 0x1BF8    | Data table (~7.2 KB)                  |
| `lbl_80363CE8`      | `0x80363CE8`   | 0x3E08    | Data table (~15.9 KB)                 |
| `lbl_8036A520`      | `0x8036A520`   | 0x19E0    | Data table (~6.6 KB)                  |
| `lbl_8036E150`      | `0x8036E150`   | 0x26F0    | Data table (~9.9 KB)                  |
| `lbl_80370BD0`      | `0x80370BD0`   | 0x1390    | Data table (~5 KB)                    |
| `lbl_80373750`      | `0x80373750`   | 0x16F0    | Data table (~5.9 KB)                  |
| `lbl_80379F58`      | `0x80379F58`   | 0x16002   | **Very large data** (~90 KB)          |
| `lbl_8038FFFC`      | `0x8038FFFC`   | 0x9F5C    | Large byte array (~40 KB)             |

### Repeated-Size Data Clusters (potential array tables)

Five consecutive 0x240-byte objects at `0x802E5570`-`0x802E5E70` may form a table
of 5 entries of size 0x240 (576 bytes each), possibly type/battle-related data.

### Identified SDA Variables (from function_map.md)

| Address        | Type   | Proposed Name          | Description                         |
|----------------|--------|------------------------|-------------------------------------|
| `0x80478DC0`   | struct | `gDVDInfo`             | DVD drive info structure            |
| `0x80478DC8`   | u8     | `gRumbleEnabled`       | Controller rumble on/off flag       |
| `0x80478DC9`   | u8     | `gInitComplete`        | Set to 1 after GameInit finishes    |
| `0x80478DCA`   | u8     | `gResetButtonLatched`  | Reset button press latch            |
| `0x80478DCC`   | s32    | `gResetCountdown`      | 100-frame countdown after crash     |
| `0x80478DD0`   | u8     | `gDrawMode`            | Current rendering draw mode         |
| `0x80478820`   | u8     | `gCardSystemFlag`      | Card/save update control            |
| `0x804EB2A8`   | u32    | `gPokeDollars`         | Current money (from AR code)        |

### Key Function Regions (from code strings)

| Address Range           | Module Source File       | Purpose                        |
|-------------------------|--------------------------|--------------------------------|
| `0x80136000-0x80168000` | menuCardE.c, menuCB_*.c  | UI / Menu system               |
| `0x80180000-0x80192000` | (world/map module)       | Pokemon model/data init        |
| `0x801E0000-0x801F0000` | cardesavedata.c          | Save/Card/GBA link             |
| near `0x80200000`       | pokeconv.c               | Pokemon GBA<->GCN conversion   |
| near `0x80200000`       | gbaCommunication.c       | GBA-GCN communication link     |
| `0x80250000-0x80260000` | (REL module)             | REL module loader              |

---

## Build Information

From strings in `.data` section:

| Component     | Version String                                                        |
|---------------|-----------------------------------------------------------------------|
| Dolphin SDK OS| `<< Dolphin SDK - OS  release build: Mar 17 2003 04:20:41 (0x2301) >>`|
| Dolphin SDK DVD| `<< Dolphin SDK - DVD release build: Oct 29 2002 09:56:49 (0x2301) >>`|
| Dolphin SDK PAD| `<< Dolphin SDK - PAD release build: Sep  5 2002 05:34:02 (0x2301) >>`|
| Dolphin SDK AI | `<< Dolphin SDK - AI  release build: Sep  5 2002 05:34:25 (0x2301) >>`|
| Dolphin SDK GX | `<< Dolphin SDK - GX  release build: Feb  7 2003 04:01:13 (0x2301) >>`|
| Dolphin SDK SI | `<< Dolphin SDK - SI  release build: Sep  5 2002 05:33:08 (0x2301) >>`|
| Dolphin SDK CARD| `<< Dolphin SDK - CARD release build: Sep  5 2002 05:35:20 (0x2301) >>`|
| MetroTRK      | `MetroTRK for GAMECUBE v2.6`                                          |
| Game Version  | `1.6` (GetVersionMajor=1, GetVersionMinor=6)                          |

SDK version 0x2301 corresponds to a 2002-2003 era Nintendo GameCube SDK. The most recent
SDK component (GX, OS) was built March 2003, consistent with Colosseum's November 2003
Japan release.

---

## Sources

- [Stars' Pokemon Colosseum and XD Hacking Tutorial (Overview)](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/)
- [Part 1: File Decompression and Recompression](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-1-file-decompression-and-recompression-r5/)
- [Part 2: Text Editing](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-2-text-editing-r6/)
- [Part 3: Editing Starters and Gift Pokemon](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-3-editing-starters-and-gift-pokemon-r7/)
- [Part 4: Editing TMs and HMs](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-4-editing-tms-and-hms-r8/)
- [Part 5: Editing Pokemon Stats](https://projectpokemon.org/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-5-editing-pokemon-stats-r9/)
- [Part 6: Editing Move Data](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-6-editing-move-data-r10/)
- [Part 7: Editing Trainers (Colosseum)](https://projectpokemon.org/home/tutorials/rom/stars-pok%C3%A9mon-colosseum-and-xd-hacking-tutorial/part-7-editing-trainers-colosseum-r11/)
- [PekanMmd/Pokemon-XD-Code (GitHub)](https://github.com/PekanMmd/Pokemon-XD-Code) — CMRelIndexes.swift, Code snippets.swift, GoDGameInit.swift
- [PekanMmd/Mausoleum-Tool (GitHub)](https://github.com/PekanMmd/Mausoleum-Tool)
- [TuxSH/PkmGCTools (GitHub)](https://github.com/TuxSH/PkmGCTools) — LibPkmGC Colosseum Pokemon.h, ItemInfo.h
- [Pokemon XD Hacking Info Dump (PokeCommunity)](https://www.pokecommunity.com/threads/pokemon-xd-hacking-info-dump.351350/)
- [Pokemon Colosseum AR Codes (gc-forever)](http://www.gc-forever.com/forums/viewtopic.php?t=2024)
- [Pokemon Colosseum Gecko Codes (gc-forever)](https://www.gc-forever.com/forums/viewtopic.php?t=2152)
- [pret/colosseum-mb (GitHub)](https://github.com/pret/colosseum-mb) — GBA multiboot decomp
- [List of Items by Index (Bulbapedia)](https://bulbapedia.bulbagarden.net/wiki/List_of_items_by_index_number_(Colosseum))
- [Extracting Pokemon from RAM in Colo/XD](https://legofigure11.github.io/misc/colo-xd-ram/)
- [GC6E01 Disc Filesystem](https://gcwiifst.home.blog/2020/01/29/pokemon-colosseum-gc6e01/)
- [GameHacking.org Pokemon Colosseum](https://gamehacking.org/game/54231)
