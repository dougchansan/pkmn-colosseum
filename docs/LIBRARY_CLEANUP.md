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

The remaining consolidation targets are the MusyX sequencer and DSP controller,
then HAL DObj and WObj. Use the appropriate older Dolphin branch of
[AxioDL MusyX](https://github.com/AxioDL/musyx/tree/89850639d731ec080c12cb0bc8d7ab2ea3880c3a)
and the compatible HAL implementations in
[Melee](https://github.com/doldecomp/melee/tree/598987cf347c125c7416517af18bf5c1dc3f15a1/src/sysdolphin/baselib)
as pinned references. Preserve the existing attribution and license notices.

Ownership must be established by symbols and their consumers, not filenames
or neighboring addresses. For example, the old WObj source also contains two
archive functions, and the BSS next to DObj belongs to FObj. A mixed data
owner may transfer only the proven subset, retaining every neighboring byte.

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
