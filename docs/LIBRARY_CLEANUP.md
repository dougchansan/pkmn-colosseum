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
Those first 18 files were absent from the active source-owner report and from
the transitive quoted-include dependencies of every source owner. A repository-wide reference
review found historical worklists, attribution proposals and explanatory
comments, but no production build registration or live source include.
Another 21 former owners are archived unchanged after their implementations
and data were reunited below, for 39 preserved historical C files in total.

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

The verified mapping replaces 21 old owners with these five, reducing the
scoring-unit inventory from 2,272 to 2,256. Independent split-map and native
report audits confirm the identical physical interval union in every section,
with no overlaps, added bytes or missing bytes. The object-map freeze and
README measurements are refreshed from the verified native source report.

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

## Verified source result

The native source revision
[`df0ecc3dd0f0dc34e090bc45d51f5dffe81c717a`](https://github.com/dougchansan/pkmn-colosseum/commit/df0ecc3dd0f0dc34e090bc45d51f5dffe81c717a)
passes all 26 unchanged canonical DOL/REL hashes and direct byte comparisons.
Its [report and diagnostic artifacts](https://github.com/dougchansan/pkmn-colosseum/actions/runs/41008273719)
show all 8,677 functions, 2,504,940 code bytes and 3,565,996 data bytes exact
and source-linked, with all 2,256 scoring units complete. The subsequent
acceptance commit updates only the README, this record and the topology freeze.
The complete final checks are recorded in [PR #668](https://github.com/dougchansan/pkmn-colosseum/pull/668).

The extended zero-data gate verifies 38 owners, 40 zero sections and 1,597
symbols covering 655,416 physical bytes. Every previously checked byte remains
covered, with another 336 bytes now checked. All 31 real state objects belonging
to the four restored stateful units have exact final linked addresses and
sizes. DSP's 60-byte native small-data section and WObj's 4-byte section each
receive their proven four-byte terminal alignment at link time; no artificial
state or padding variables were introduced. The gate records the actual native
sizes and eight-byte alignment, requires the independently mapped terminal
gap, and preserves every real symbol's strict offset, size and binding checks.

All 334 checker tests and 13 jump-table tests pass. The source-wide wrapper
scan, registered assembly branch-target checks and complete vector-template
validation pass. The code/data/function denominators are unchanged. The
independent report audit retains every function identity and bound and every
physical section interval across all 26 modules, with zero overlaps or
added/removed physical bytes.

Report SHA-256:
`2779ecf834a656a461eb1febdd95a617d9b603c73e0ee1f2a6c006c8ff9bc7a0`.
Function-identity SHA-256:
`29d80aeb75534ca8bfc9fbbdd80290e93bafea9d57354bdabc02b319446ea6ad`.
Physical-interval-union SHA-256:
`3ab2e1a11519104c854210097602a65dc0576bcafc03baa146d9b5aa0780b1f7`.
