# Battle and purification small-data reconstruction

These source owners reconstruct 152 bytes of the previously original-backed
`.sdata` interval `0x80478D08..0x80478DB4`:

| Owner | Interval | Bytes |
| --- | --- | ---: |
| `src/game/data/sdata_80478D08.c` | `0x80478D08..0x80478D78` | 112 |
| `src/game/data/sdata_80478D80.c` | `0x80478D80..0x80478D8C` | 12 |
| `src/game/data/sdata_80478D98.c` | `0x80478D98..0x80478DB4` | 28 |

The eight-byte flag block at `0x80478D78..0x80478D80` and the 12 bytes at
`0x80478D8C..0x80478D98` retain their original-object fallback. The flag block
requires one real eight-byte array plus interior labels; the damage operand's
complete extent remains unresolved.
No initializer is inferred from an unrelated short byte match.

CI9 at source revision `dcdfc947cd77221ff38159679f07e704be77d9e4` verifies
all three source owners as exact and source-linked: 112 + 12 + 28 = 152 bytes.
All 26 canonical output hashes and direct comparisons pass. The 20 fallback
bytes retain their original storage and receive no source credit. The Build
job, report artifact and remaining final PR checks are recorded in
[`DATA_MATCHING.md`](../DATA_MATCHING.md).

## Reconstruction evidence and native verification

The reconstruction uses existing C consumers, typed table layouts and the
metadata-only target hashes from CI5 (`2d9ab305`). No original payload bytes
were read or copied. Every one of the 21 reconstructed original symbols matches
its complete preexisting SHA-256, including any zero padding included in its
old extent. The flag bytes and gaps also have all-zero hashes, but remain
original-backed until a sound alias model is available. The version pointer
has one `R_PPC_ADDR32` relocation to
`lbl_803751F0`, with addend zero; its encoded word is zero before relocation.

The source models were checked against the private metadata before compiling.
CI9 confirms equal native section bytes, equal original symbol offsets and
sizes, and the version pointer's relocation. Address-named source padding
corresponds to the target's synthetic gap symbols and preserves every byte at
its original address. The active-object extent refinements are detailed below.
The complete source intervals have these
pre-relocation hashes:

| Interval | Pre-relocation SHA-256 |
| --- | --- |
| `0x80478D08..0x80478D78` | `e30374a2fe8e13ba3b87fd0f82aa18b964809ce1db36944ddfacc5c7f54bfcc4` |
| `0x80478D80..0x80478D8C` | `a885f1e132b95f490c3c1ad177d3da305fb4a8d7f2b24fd8f95579b38c57db82` |
| `0x80478D98..0x80478DB4` | `fccf313e5a55cb7dd8c680590df9c563a5a8b1fe1ef01846ef25fa6a59e18a9b` |

## Typed values

The address suffix identifies each existing `lbl_80478...` symbol.

| Suffix | Source type and initializer | Existing evidence |
| --- | --- | --- |
| `D08` | `char* = lbl_803751F0` | `THPInit` passes the pointer to `OSRegisterVersion`; target relocation identifies the same string. |
| `D10` | `u16 = 21 * 2` | `fightPreloadResources` advances two halfwords per archive/result pair; `lbl_803752A0` has 21 live pairs. |
| `D18` | `u8 = 5` | `fightActionFlowKaisiPreSubLoad` uses this random-index bound for the five nonzero IDs in `lbl_80375970`. |
| `D20` | `u32 = 11` | `fightEncountWipeDataBiosGetPtr` indexes eleven 12-byte wipe records. |
| `D28` | `u32 = 8` | The condition accessors index eight 24-byte `ColosseumEventPairRow` records. |
| `D30` | One zero `ColosseumEventRow6` | `joukenDataGet0`, `joukenDataGet2` and `joukenDataGet4` read its byte/halfword fields with six-byte row stride. |
| `D38` | `u32 = 1` | Bounds check for that one event row. |
| `D40` | `u32 = 30` | `fightTargetDataBiosGetPtr` indexes thirty eight-byte records in `lbl_80375AC8`. |
| `D48` | `u32 = 20` | `fightActionKindDataBiosGetPtr` indexes the twenty typed action records in `lbl_80375BB8`. |
| `D50` | `u32 = 2` | `fightKoukaDataBiosGetPtr` indexes two six-byte effect records in `lbl_80375CB8`. |
| `D58` | `u8[5] = {16, 8, 4, 3, 2}` | `fightWazaCriticalDataBiosGetBunbo` reads a byte denominator; the caller clamps the stage to the table bound. The complete eight-byte mapped extent verifies the five-stage denominator hypothesis and its padding. |
| `D60` | `u32 = 5` | Bounds and clamping for those five denominator entries. |
| `D68` | `u32 = 13` | `fightAbicntDoKakeWaru` indexes the thirteen nonpadding numerator/denominator pairs in `lbl_80375D10`. |
| `D70` | `u32 = 13` | `fightWazaHitKakurituDataBiosGetPtr` indexes the thirteen nonpadding pairs in `lbl_80375DD0`. |
| `D80` | `u16[3] = {0x45, 0x43, 0x40}` | Script `lbl_80377B0C` passes the first halfword to `WS_STATUS_SET16`; other scripts use `D80+2`. Opcode `0xFC` and status kind 7 route these operands to `fightFloorSetStatus` field `0x3B`. The same three values occur in direct C setters of that field; the six-byte hash selects their order. |
| `D86`, `D88` | `u16 = 0x41`, `u16 = 0x42` | CI8's five-site reference diagnostic proves `WS_STATUS_SET16`, fight-floor field `0x3C`, which routes through Pokemon field `0xDF` to move-status removal. `fightWazaIsJoutaiSousai` explicitly checks the same paired status IDs. Complete symbol hashes verify both values and the two zero padding bytes after `D88`. |
| `D98` | `s32 = 4 * 6` | `toolentryDebugPokemonCreate` allocates/counts this many Pokemon and initializes four parties of six. Complete extent verification confirms the capacity is 24. |
| `DA0` | `u16[2] = {100, 3}` | `std244_spawnEffectAtPosition` passes one pair to `effectCreate` and `effectPlayWaza`. |
| `DA8` | `u16[4] = {77, 4, 122, 4}` | `reliveMain` passes two pairs to `loadSequence` and `effectPlayWaza`. |
| `DB0` | `u16[2] = {122, 4}` | The purification ceremony and move-forgetting callback pass one pair to the same consumers. |

The sequence IDs were recovered as typed `(group, type)` pairs. Both consumers
advance a `u16*` by two elements per pair. `fn_801DDEE4` explicitly accepts
types 1 through 4, with nonzero group IDs below the existing source-backed
counts 356 for types 1 through 3 and 219 for type 4. Comparing complete symbol
hashes across that finite valid domain selects the listed pairs uniquely.
This does not reuse opaque bytes from an unrelated compiled source object.

The source evidence is in:

- `src/dolphin/thp/THP_range_801E1B54.c`
- `src/game/battle/battle_main.c`
- `src/game/fight_action.c` and `fight_action_exact_8020D784.c`
- `src/game/fight_encount_exact_8020DD44.c`
- `src/game/fight_kouka_exact_8020D968.c`
- `src/game/fight_out_pokemon_exact_802096E8.c`
- `src/game/fight_target.c`, `fight_trainer.c` and `fight_seq.c`
- `src/game/fight_range_80211A00.c`, `status.c` and `fight_floor.c`
- `src/game/toolentry.c`, `pokemon_relive.c` and `sequence.c`
- `src/game/data/data_80375230.c`, `data_80375938.c`, `rodata_80279B78.c` and
  `sdata_80478CA8.c`
- `src/game/battle_scripts/scripts_80377B0C.battle.json` and
  `scripts_80379714.battle.json` (formerly `data_80377B0C.c` and
  `data_80379714.c`)

## Scalar extents and padding

Seventeen old map entries include padding beyond the active type demonstrated
by their consumers. Their symbol extents are refined to the active object size;
the same bytes remain owned by the same section, and address-named source
padding retains the independently verified zero storage. Neither section
coverage nor the total-data denominator changes.

| Symbols | Old size | Active source size |
| --- | ---: | ---: |
| `D08`, `D20`, `D28`, `D38`, `D40`, `D48`, `D50`, `D60`, `D68`, `D70`, `D98`, `DA0` | 8 | 4 |
| `D10` | 8 | 2 |
| `D18` | 8 | 1 |
| `D30` | 8 | 6 |
| `D58` | 8 | 5 |
| `D88` | 4 | 2 |

Explicit padding arrays use the compiler-verified one-byte alignment attribute
to avoid adding a second alignment gap. Every definition is explicitly placed
in `.sdata`, including zero-initialized records, and retained through
the scoped main export list. No original symbols are renamed.

CI8 exposed one compiler storage distinction: scalar zero-byte definitions in
the flag block moved into `.sbss` despite the explicit `.sdata` declaration.
Initialized arrays avoid that placement change, but three separate one-byte
objects would not describe the caller's actual eight-byte array accesses.
The entire flag block therefore retains its original fallback. A future source
owner must preserve one eight-byte C array and supply verified interior aliases
at offsets 3, 4 and 5 (`D7B`, `D7C` and `D7D`). No consumer functions are
changed for this checkpoint. CI9 passes all 8,677 function-identity and code
matching checks with this fallback retained.

## Status operand references and remaining fallback

The bounded metadata diagnostic in `tools/small_data_reference_diagnostics.py`
validates exactly five preexisting relocation sites using the existing handler
schemas. CI8 verifies `D86` and `D88` as the 16-bit status-removal operands
described above. The sites are `lbl_80377531` at offsets `0xEB` and `0xFF`,
and `lbl_80378115` at offsets `0xB3` and `0xC7`.

The fifth site, `lbl_80377531` at `0x113`, reads `D8C` through
`WS_STATUS_SET32`, fight-floor field `0x3E`. The existing setter chain reaches
`fightWazaBiosSetDamage`, which writes a 32-bit damage value at offset `0x9C`
of the move state. This establishes the leading word's type and purpose, but
does not independently establish the entire 12-byte mapped extent. Those 12
bytes remain an explicit nonmatching owner with four-byte split alignment.
Neither that owner nor the eight-byte flag fallback receives source completion
credit.
