# MusyX and HSD source cleanup

The baseline is the fully matching tree at
[`0275f31498f43ad807d4c91fa2da95ede5f5291b`](https://github.com/dougchansan/pkmn-colosseum/commit/0275f31498f43ad807d4c91fa2da95ede5f5291b):
8,677 functions, 2,504,940 code bytes, 3,565,996 data bytes, and 2,272 complete
source-backed scoring units. Source cleanup preserves the complete function
and physical-byte coverage while recovering coherent library translation units.
Reuniting owners may change the scoring-unit count; it must not remove bytes
or hide a function regression.

## Retired candidate files

Seven obsolete MusyX files and eleven obsolete HSD files have moved unchanged
to [`archive/previous_campaign/library_candidates/`](../archive/previous_campaign/library_candidates/).
They are absent from the active source-owner report and from the transitive
quoted-include dependencies of every source owner. A repository-wide reference
review found historical worklists, attribution proposals and explanatory
comments, but no production build registration or live source include.

The old `musyx_candidate_80158BB4.c` wrapper receives no matching credit.
Its active replacement is [`runtime/synth_ac.c`](../src/musyx/runtime/synth_ac.c),
with two exact functions, 288 code bytes, 1,024 bytes of pitch tables and
24 bytes of `.sdata2` constants. The historical wrapper and aggregate are
preserved in the archive rather than presented beside the active code.

## Library restoration scope

The consolidation covers the MusyX sequencer and DSP controller, then HAL
DObj and WObj, with the archive functions separated from the former WObj
source. The appropriate older Dolphin branch of
[AxioDL MusyX](https://github.com/AxioDL/musyx/tree/89850639d731ec080c12cb0bc8d7ab2ea3880c3a)
and the compatible HAL implementations in
[Melee](https://github.com/doldecomp/melee/tree/598987cf347c125c7416517af18bf5c1dc3f15a1/src/sysdolphin/baselib)
are pinned references. Existing attribution and license notices are retained.

| Owner | Retained functions | Code bytes | Owned data bytes |
|---|---:|---:|---:|
| `src/musyx/runtime/seq.c` | 16 | 16,096 | 55,504 |
| `src/musyx/runtime/hw_dspctrl.c` | 18 | 13,132 | 1,936 |
| `src/hsd/dobj.c` | 13 | 1,936 | 184 |
| `src/hsd/wobj.c` | 14 | 2,520 | 208 |
| `src/hsd/archive.c` | 2 | 536 | 72 |

The proposed mapping replaces 21 old owners with these five, reducing the
scoring-unit inventory from 2,272 to 2,256. An independent split-map audit
confirms the identical physical interval union in every section, with no
overlaps or missing bytes. Native matching is required before accepting this
mapping or refreshing the frozen report topology.

The sequencer owns its notes, instances, MIDI priorities and list state. The
DSP controller owns its mixer/SRC tables, studio state and command-list
storage. DObj and WObj own their class information, small globals and literal
pools. Scalar and table symbol sizes describe the actual typed objects;
existing section-end padding remains part of the physical owner ranges.

Ownership must be established by symbols and their consumers, not filenames
or neighboring addresses. For example, the old WObj source also contains two
archive functions, and the BSS next to DObj belongs to FObj. A mixed data
owner may transfer only the proven subset, retaining every neighboring byte.

## Compiler comparisons

The isolated experiments at
[`b721f05d6db0c38424604bf1bef5a46896ed2332`](https://github.com/dougchansan/pkmn-colosseum/commit/b721f05d6db0c38424604bf1bef5a46896ed2332)
compiled unchanged controls and alternative source copies with the generated
compiler commands. They preserved source basenames and exported only scores,
hashes, symbol layouts and call metadata in
[the native CI diagnostics](https://github.com/dougchansan/pkmn-colosseum/actions/runs/41007472661).
The temporary probe runner and exact recipes remain in that commit's history.

The DSP's original automatic-inline setting matches all 18 functions and
their helper-call sites, so the temporary scoped `SortVoices` pragma and
unit-specific no-auto-inline override have been removed. DObj's natural
load/helper form still scores 96.9469% for `DObjLoad`; its existing parameter
copies remain documented in [RULE_EXCEPTIONS.md](RULE_EXCEPTIONS.md). Its
separately owned literal stand-ins have been removed.

Explicitly marking the public `seqGetPrivateId` definition inline omitted its
required out-of-line entry, so the ordinary public definition is retained.
The sequencer's private helper order and ordinary `static GetPublicId`
annotation follow the pinned upstream source, allowing normal first-reference
allocation of its BSS without extra pragmas or dummy state.

## Acceptance

Each restored owner must compile and match as a complete object: all sibling
functions, data, literal-pool ordering, sizes, alignment and relocations.
Require the unchanged 26 canonical DOL/REL hashes and direct byte comparisons,
the module/address/size regression audit, exact physical coverage, and the
normal quality and assembly-evidence checks. Refresh the topology freeze and
README only from a fresh successful source measurement.

The interrupt-vector gate checks a coherent, fully source-complete report
inventory while retaining the exact global function/code/data requirements.
It no longer duplicates a fixed historical count of unrelated source owners;
the strict object-map freeze remains responsible for their reviewed topology.
Missing or duplicated report units, incomplete owners, changed denominators,
or new vector function/code/data credit still fail.
