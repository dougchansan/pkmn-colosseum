# MetroTRK interrupt-vector templates (GC6E01)

The `crt/interrupt_vectors` owner is the **7,988-byte** interval
`0x800034AC–0x800053E0` in `.init`. It is a table of executable exception-entry
templates, reserved slots and a debugger identification string, not an ordinary
C function. The four other physical alignment bytes mentioned in earlier
completion reports belong to the already-complete
`main/trk/TRKTarget_residual_800C25FC` owner. They are outside this interval;
do not append four bytes to this object or include them in its size.

## Source family and provenance

**Origin:** The corresponding library source is Metrowerks MetroTRK for Dolphin,
`TRK_MINNOW_DOLPHIN/Processor/ppc/Generic/exception.s`. The pinned public
[Twilight Princess reconstruction](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/exception.s)
provides independently inspectable evidence for the same signature, vector
offsets, entry convention and special cases. That repository publishes its
reconstruction under [CC0 1.0](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/LICENSE.md).
This is corroborating decompilation source, not a claim that a proprietary SDK
release was obtained or that its exact version has been established.

**Other decompilations:** The pinned
[Twilight Princess exception assembly](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/TRK_MINNOW_DOLPHIN/debugger/embedded/MetroTRK/Processor/ppc/Generic/exception.s)
retains the same hardware register protocol, exception IDs and special entries.

The Colosseum source is an independently authored symbolic assembly
representation of these templates. It uses meaningful instruction mnemonics,
named PowerPC registers, shared entry macros and explicit slot boundaries.
It does not include another project's `macros.inc`, a target-object payload,
generated retail disassembly, or an array of encoded instructions. The
identification string, `Metrowerks Target Resident Kernel for PowerPC`, is
retained as the library's original identifying text.

## Hardware need and local corroboration

**Why it cannot be C:** These entries run at exception entry without a C calling
convention or a usable compiler-generated stack frame. They preserve r2–r4 in SPRG1–SPRG3,
retain the interrupted PC and machine state from SRR0/SRR1, set the instruction
and data translation bits in the state used for transfer, and enter the
debugger through `rfi`. C cannot specify that exact register-entry protocol,
special-register access or exception return instruction.

The existing [TRK initialization](../../src/trk/TRKInit.c) copies selected
`0x100`-byte slots from `gTRKInterruptVectorTable` and flushes their cache
ranges. The existing [TRK interrupt handler](../../src/trk/TRKInterrupt.c)
accepts the preserved PC/state in r2/r4, recovers the original r4 from SPRG3,
and then saves the remaining interrupted context. These callers and consumers
corroborate both the table layout and its register contract.

The vendor's [MetroTRK Reference](https://www.nxp.com/docs/en/reference-manual/METROTRKRM.pdf),
PowerPC appendix, pages 167–170, describes copying exception vectors into their
processor-defined memory locations. The
[MPC750 family manual](https://www.nxp.com/docs/en/reference-manual/MPC750UM.pdf),
exception-processing chapter, documents SRR0/SRR1, SPRGs, translation state and
`rfi`. These hardware references explain the instruction semantics; they do
not establish a particular Colosseum library revision.

## Layout and control transfers

The reserved first slot contains the identification string and zero storage.
The reset entry is at table offset `0x100`. The regular entries use a shared
13-instruction redirect sequence and retain their exception IDs. Special
cases are the machine-check cache invalidations, the shared performance-monitor
and vector-unavailable slot, and three legacy TLB entry wrappers. The table
retains the generic PowerPC library's unused slots and processor-family
entries; their presence is not evidence that Gekko implements every exception
represented by the template library.

The existing public symbols remain at their original addresses:

| Symbol | Address | Role |
| --- | --- | --- |
| `gTRKInterruptVectorTable` | `0x800034AC` | Table start |
| `lbl_80003FF0` | `0x80003FF0` | Existing interior label |
| `lbl_80004000` | `0x80004000` | Existing interior label |
| `gTRKInterruptVectorTableEnd` | `0x800053E0` | End-exclusive table boundary |
| `__TRK_reset` | `0x800053E0` | Existing reset helper; `OSResetSystem(0, 0, 0)` |
| `TRKInterruptHandler` | `0x800C0EAC` | Common debugger entry |

**External branch targets:** The reset entry transfers to `__TRK_reset`, and
all 24 SRR-mediated debugger entries transfer to `TRKInterruptHandler`. The
source declares symbolic relocations to those existing functions. Internal
performance/TLB branches remain inside the template; there are no other
external destinations.

The reset instruction at stored address `0x800035AC` is represented as a
relative branch to `__TRK_reset`, a displacement of `0x1E34`. Copying the slot
without rewriting that instruction preserves its displacement, not its
stored-table destination: execution at physical `0x100` would branch to
`0x1F34`. Preserve and validate the original encoding rather than changing it
to an absolute branch or adding a new runtime relocation. Matching this
stored template does not establish that the copied reset path was exercised.

Regular debugger transfers load the existing handler symbol into SRR0 and
use `rfi`; they are not direct `b`/`bl` calls. The ordinary direct-branch checker
alone therefore cannot validate their destinations. The vector-specific check
must validate each high/low address pair, its SRR0 write, the exception ID,
the translation-state setup and the final `rfi`, as well as the reset branch
and all internal branch destinations.

## Acceptance and accounting

Validate the assembled source object's `.init` extent, all labels, reserved
zeros and slot boundaries against the private original. Resolve symbolic
relocations using the existing linked symbols and compare the complete table;
then require the unchanged canonical DOL and all 25 REL hashes and direct
binary comparisons. Checks should report structural facts and results without
printing or publishing the original payload or disassembly. A failed
comparison remains a failure; it must not be repaired by copying target bytes
into a source object.

The historical hidden synthetic padding marker is not a new function. Making
this owner source-backed adds **zero tracked functions and zero tracked code
or data bytes**: the existing denominators and physical coverage stay intact.
Report the reconstructed executable template as assembly separately from C/C++.
Full source-backed unit completion requires the fresh whole-project report and
its normal source-status/topology checks; it does not follow solely from a
matching final DOL or imply runtime gameplay testing.

## Verified source-completion result

The [normal full Build](https://github.com/dougchansan/pkmn-colosseum/actions/runs/40986293627/job/123837998946)
at source revision
[`ce09b514e43171ccb1b3c335dcc898dc22ae6963`](https://github.com/dougchansan/pkmn-colosseum/commit/ce09b514e43171ccb1b3c335dcc898dc22ae6963)
compiles this source through GNU assembly and the normal DTK ELF fixup, passes
the dedicated vector gate, and links all 26 outputs with unchanged canonical
hashes and direct byte equality to the private originals. The
[fresh report](https://github.com/dougchansan/pkmn-colosseum/actions/runs/40986293627/artifacts/12947501389)
has **2,272 / 2,272 complete, source-backed units**, with all 8,677 tracked
functions, 2,504,940 tracked code bytes and 3,565,996 tracked data bytes still
exact and source-linked. Only the vector owner's source status changes.

| Verified property | Result |
| --- | ---: |
| Complete `.init` owner | 7,988 bytes, alignment 4 |
| Executable instructions | 365 / 1,460 bytes |
| Identifying text including terminator | 46 bytes |
| Reserved and alignment zeros | 6,482 bytes |
| Public labels | 4, exact offsets and `NOTYPE` metadata |
| Callable function symbols | 0 |
| Symbolic relocations | 1 `REL24`, 24 `ADDR16_HI`, 24 `ADDR16_LO` |
| Direct branch destinations | 5, all verified |
| SRR-mediated debugger entries | 24, all verified |
| Entire relocated span versus original | Equal |

An independent native audit reproduces the complete-span comparison without
using the production verifier's ELF parser. The checker suite passes all
307 tests, including 17 vector tests for invalid relocations, targets, source
escapes, local function symbols, padding and incomplete reports; the separate
13 jump-table tests also pass. The resolved table SHA-256 is
`0b9e59e77ccc383067155f629cb44c9645323255e44608c6970df89eec58e157`.
The full report SHA-256 is
`b3d16289a1b16caed16c311bd4db447484a937749121308c469147a26a611b32`.

The initial source-measurement build stops only when the strict source-status
freeze still expects 2,271 source-backed units. The follow-up changes that
verified count to 2,272 and refreshes the README; no topology or denominator
changes. Final required checks and integration are recorded in
[PR #666](https://github.com/dougchansan/pkmn-colosseum/pull/666).
