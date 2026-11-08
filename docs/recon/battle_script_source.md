# Authored battle-sequence source

All packed DOL battle programs are represented by named commands in
`src/game/battle_scripts/*.battle.json`. The 34 source owners retain all 262
original object names and exactly 106,756 physical data bytes: 259 command
programs, the 214-entry move-effect dispatch table `lbl_80379BFF`, and two
zero-filled storage objects. These are programs for the game's battle
interpreter, so they contribute to source-backed **data**. They do not add
decompiled C functions or native assembly functions.

The first 17 owners (195 programs, 13,500 bytes) contributed a **13,500-byte
gain in matched and complete source data**; that integration evidence is
recorded below. The other 17 owners keep their original `game/data/data_*.c`
unit identities. They were already complete byte-array C sources; replacing
those arrays with typed sources (67 objects, 93,256 bytes) changes no progress
metric. It makes every battle address in them relocatable; see
[layout-independent addresses](#layout-independent-addresses).

`tools/battle_script_format.py` defines the supported commands and their typed
operands. Each command records the existing matching C handler and source path
used to establish its meaning, operand widths, and instruction stride. The
dispatch authority is `lbl_8027A00C` in
`src/game/data/rodata_80279B78.c`. The source language has 228 supported commands;
the reconstructed programs contain 3,789 command occurrences.

## Source and compilation

The authored files contain operations such as `calculate_damage`,
`apply_damage_and_update_hp`, `jump_if_condition`, `call_sequence`,
`copy_memory`, and `set_status_word`. They contain no opaque byte-array escape,
native instruction listing, or original-file path. Comparison, stat, battler,
and status-family operands have explicit domains. Status fields are checked
against the actual getter or setter switch for their family; read-only fields
are not silently accepted as setter fields.

For example, a fixed-damage transfer can be expressed as:

```json
{
  "command": "set_status_word",
  "family": "fight_floor",
  "context": null,
  "object_id": null,
  "field": 62,
  "index": null,
  "value": {"symbol": "lbl_80478D90"}
}
```

The existing `WS_STATUS_SET32` handler reads a 32-bit value through its value
operand and passes it to `statusSetStatus`. Family 7 selects
`fightFloorSetStatus`; field `0x3E` is the current damage transfer. The command
in `lbl_803767D1` references `lbl_80478D90`. Its operand is not word aligned in
the packed stream, so the old relocation scan missed this typed reference.
The analogous command in `lbl_80377531` references `lbl_80478D8C`. These uses
establish the two separate four-byte damage operands; no script reference was
found to the trailing alignment word at `0x80478D94`.

Branches and dispatch-table entries name a script and a named entry, rather
than an integer address or offset. Entries are calculated from command widths,
so every reference lands on an instruction boundary of an owned program. The
catalog no longer declares external C entries: the neighboring C byte arrays
that held the remaining 67 battle objects are now typed sources. Their 16
additional opcodes were typed from their handlers: `0x1a`/`0x1b` (status menu
open/close), `0x4b`, `0x56` (cry), `0x6f` (visibility), `0x83` (no operation),
`0x85`/`0x86`/`0x87` (Stockpile, Spit Up, Swallow), `0xaa` (Destiny Bond),
`0xac` (Flail power), `0xad` (Spite), `0xae` (party cure), `0xc8`, `0xce`
(Torment), and `0xeb`. Every five-byte form jumps through `*(u32*)(pc + 1)` or
advances five bytes, so its operand is a branch. `command_c8` and `command_eb`
keep handler-derived names because their gameplay meaning is not yet confirmed.

A source owner may also contain a `table` of script entries (with trailing
`padding` bytes the mapped object includes) or `zero_fill` storage of a declared
size. Neither accepts raw bytes.

Memory operands name data objects and optional nonnegative addends. Their
access widths must fit the declared symbol extents. Where the public map keeps
a historical fragment of a larger native C array, the compiler reads the
array's registered extent from `config/GC6E01/data_aliases/sdata_80478D78.json`
(`lbl_80478D78[8]`; scripts address its bytes six and seven). Byte-copy and
comparison commands check the complete transfer count against both memory
objects. That count does not constrain an unrelated branch destination. Null is
accepted only in fields whose handlers provide a null case.

The compiler runs without original game files:

```sh
python tools/compile_battle_scripts.py \
  --source src/game/battle_scripts/scripts_80375E60.battle.json \
  --symbols config/GC6E01/symbols.txt \
  --native-bounds config/GC6E01/data_aliases/sdata_80478D78.json \
  --assembler build/binutils/powerpc-eabi-as \
  --output build/GC6E01/battle-validation/scripts_80375E60.o \
  --comparison-output build/GC6E01/battle-validation/compare/scripts_80375E60.o
```

It emits generated GNU data assembly beside the output object and invokes the
normal PowerPC assembler. There are no native instructions. Each original
program becomes a global ELF object symbol with its actual byte-array extent.
The compiler requires contiguous ownership inside each owner and checks its
derived command sizes against the public map. It preserves each owner's
original section alignment: 17 owners use four-byte alignment and 17 use
eight-byte alignment. No internal alignment directives or object-byte patches
are needed.

Generated assembly and objects belong under the ignored build directory.
The authored JSON, compiler, source schema, neighboring C definitions, and
public symbol map are the build inputs. Private originals were used only to
reverse engineer and independently verify the authored source.

The generated object follows the existing GNU assembler build path through
`dtk elf fixup`. This source-only compatibility step sets the embedded PowerPC
ELF flag and removes empty sections and their section symbols. Independent
comparison of all 17 battle and 12 message objects verifies unchanged allocated
bytes, object symbols, relocations and section alignment. Their pinned objdiff
report remains 17,135 of 17,135 data bytes exact and 29 of 29 units complete.
No target object or original payload is an input to this fixup.

## Layout-independent addresses

The interpreter reads packed, big-endian wire addresses, and 1,184 of the 1,515
semantic address operands are not word aligned. Source format v1 marked those
operands `absolute` and wrote their retail public-map addresses into the
linked object, because dtk's relocation scan only finds aligned words and the
split target objects carry no relocation there. The neighboring C byte arrays
held the same retail values as literals. Any code or data change that shifted
the battle data therefore left branch targets, dispatch entries and memory
operands pointing at the old layout ([issue #670](https://github.com/dougchansan/pkmn-colosseum/issues/670)):
the interpreter then executed unrelated bytes as commands.

Format v2 has no per-operand encoding. Every address operand, including every
dispatch-table entry, is an ordinary `R_PPC_ADDR32` assembler expression in
the object the linker reads. GNU as emits unaligned relocations when the
operand is unaligned, and mwld applies them. A field cannot request a
relocation for an arbitrary scalar, opcode, or byte span.

dtk 1.8.3 cannot represent an unaligned relocation in a split target object;
`add_relocations` rounds its source down to the containing word. The compiler
therefore also writes a comparison object, which objdiff reads and the linker
never does. It differs from the linked object only at unaligned operands,
which it writes as their public-map addresses. Aligned operands are
relocations in both objects. The comparison object's bytes and relocations
match the split targets exactly for all 34 owners, so the stock report, the
matching percentages and all 26 canonical hashes are unchanged.

There are 331 aligned relocations. All 299 that the first 17 owners already
had keep their offsets, types, symbol identities, addends, and local target
offsets.

Two previous relocations were scanner false positives. In `lbl_80378F8C`, the
`set_status_halfword` command begins at `0x80378FE1` and occupies 20 bytes. Its
four-byte value-address field begins at `0x80378FF1` and references
`lbl_80478D80`. The next `play_animation` command begins at `0x80378FF5`, with
separate one-byte battler and animation operands. The aligned word beginning
at `0x80378FF4` therefore crosses the last byte of the value pointer, the next
command, and those two scalar operands. It is not a reference to
`lbl_80451217`. The same pattern appears in `lbl_803784C0`: the value pointer
of its `set_status_halfword` at `0x803784CD` ends at `0x803784E0`, and the word
there reads `0x80451217`. The C byte array linked that word as a relocation, so
a moved layout would have rewritten the next command's opcode and operands.
Narrowly scoped `block_relocations` entries at those two source sites correct
the object metadata; the programs' physical bytes, symbol names, and behavior
remain unchanged. Neither target hashes nor progress denominators are changed
to accommodate them.

## Preserved interpreter compatibility behavior

The source preserves the existing interpreter and programs, including unusual
wire behavior. It does not repair game logic as a side effect of reconstruction.

| Source command | Existing behavior preserved |
| --- | --- |
| `print_selection_message_legacy` | `wsCmd11` advances three bytes but loads a 32-bit message at offset one. Its two stored operand bytes are followed by two bytes of lookahead into subsequent commands. The source retains this three-byte stride and does not invent a 16-bit handler load. |
| `animate_selected_condition` | `wsCmd66` ignores the byte at offset two and compares the following 32-bit number with seven. The number is not treated as a pointer. |
| `copy_target_stat_stages` | `WS_JIKOANJI` occupies five bytes but does not read its four-byte compatibility target. The encoded address and its relocation remain, while the control-flow audit excludes it as an executable branch edge. |
| `apply_intimidate` | The handler ignores its four-byte compatibility operand. The source retains its zero scalar and full command stride. |
| `play_animation_indirect` in `lbl_80378AF1`, `lbl_80378B00`, `lbl_80378B0F` and `lbl_80378B1E` | `wsCmd46` reads a pointer at offset two, dereferences a byte when non-null, and advances ten bytes. These four sibling commands encode addresses ten through thirteen. The scoped `wsCmd46_pointer_ten` ... `wsCmd46_pointer_thirteen` legacy operands preserve that pointer interpretation; they are not changed into immediate animation IDs or relocated. Each name is accepted only at its one original program and position. |

No new C out-of-bounds array definition is introduced to emulate packed data.
The generated objects are byte-array storage; the matching interpreter's
existing access and control-flow behavior remain the authority. The low-address
legacy operand is an observation of the original program, not evidence that
executing that path is safe or that it is reachable in normal play.

## Validation and limits

The complete surrounding script range, `0x80375E48` through `0x80379BFE`, was
decoded using handler-derived strides. All 262 script symbols decode to their
declared boundaries. The 17 original owners contain 628 executable branch/call
operands targeting 201 distinct instruction entries; all 34 owners contain 717
targeting 229. Every non-null target
falls on an instruction boundary; ignored compatibility addresses are excluded
from that control-flow count. This is a structural audit, not a runtime
reachability or termination proof.

Local native-object validation assembled all 17 authored owners and compared
them with the canonical private DOL and object metadata. A separate independent
review regenerated DTK target objects from the current configuration and symbol
map, assembled every source owner again, and compared the resulting objects
directly. Both checks passed all 13,500 physical bytes, all 195 symbol
names/offsets/sizes/types/bindings, all 299 relocation records, and every section
alignment. The direct comparison with regenerated targets required no manual
exclusion or data normalization beyond the target's ordinary relocation
representation. No non-data storage was emitted. No original payload, target
object, or generated object is committed as evidence.

The focused suite is:

```sh
python .github/scripts/test_battle_script_source.py
```

Its 30 tests cover packed byte/halfword/word encoding, real unaligned GNU
relocations and addends, the link and comparison views, dispatch tables and
zero-filled storage, symbol metadata, legacy lookahead and ignored operands,
the tightly scoped low-address operands, memory bounds, branch labels, status
getter/setter domains, duplicate keys, unsupported fields, ownership
gaps/overlaps, rejection of v1 sources, and rejection of numeric/raw address
escapes. Source-only tests also verify the dispatch identities, status field
switches, the complete 34-owner extent, and dependency coverage.

A layout regression test links all 34 owners with GNU ld twice: at their
original addresses, and with the battle data moved by `0x18` bytes and every
external symbol by `0x1000`. All 1,515 operands must equal their target's
address in both layouts, and every other byte must be unchanged.
Assembler-dependent cases explicitly skip when the PowerPC binutils have not
been downloaded.

The same check was run once against a real modified executable: a 20-byte
`force_active` array appended to `data_80375938.c` moves all 34 owners by
`0x18`. In the rebuilt `main.dol`, all 1,515 operands held their symbols' new
`main.elf` addresses and no opcode or scalar byte changed. The previous sources
built with the same change left 835 operands at retail values; operands into
`.sdata` and `.sbss`, which that change did not move, happened to stay correct.
That probe was not committed, and no runtime gameplay was tested.

The following CI evidence records the 17-owner checkpoint.

[Normal Build 40968205519](https://github.com/dougchansan/pkmn-colosseum/actions/runs/40968205519/job/123777997002)
at [source commit 456efababa4448e5b66c1c8ac336531c1e669536](https://github.com/dougchansan/pkmn-colosseum/commit/456efababa4448e5b66c1c8ac336531c1e669536)
passed all **26 unchanged canonical DOL/REL hashes and direct binary
comparisons**. The normal build ran no diagnostic target-substitution variants.
It also passed the 34 zero-data layout checks, assembly-branch checks,
metric-integrity checks, strict freeze, and README metric table check.

The full source report confirms all 17 battle owners complete, with
`total_data = matched_data = complete_data = 13,500`. This is a verified
13,500-byte source-data gain. The independent audit preserves all **8,677
function identities**, all 2,504,940 native code bytes, and the exact physical
interval union across all 26 modules, with no overlaps, removed bytes or added
bytes. The full data denominator remains 3,565,996 bytes; no native functions
or physical storage were reclassified out of scope to obtain this result.

The [report artifact from that build](https://github.com/dougchansan/pkmn-colosseum/actions/runs/40968205519/artifacts/12939685347)
has report SHA-256
`c4e3995c1d8155a5b9536a4de02dae2580ff2b9a8a265cdcf4426e897680e09b`.
These integrated checks establish source accounting and binary identity.
Runtime gameplay was not tested, and the structural control-flow audit does
not claim that every branch is reachable or terminates.
