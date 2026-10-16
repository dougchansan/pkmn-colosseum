# REA trial: fn_80039A84

Date: 2026-10-15. Starting revision: `2413295d7` on `master`.

## Accepted Follow-Up, 2026-10-16

`fn_80039A84` and its entire one-function unit
`main/game/menu/pda_candidate_80039A50` are now **100% and linked**. The native
acceptance checker reports `accepted`, with no issues and two explicitly
documented exceptions: `opt_lifetimes off` and `peephole off`.

The final fix came from MWCC's allocator trace, after REA had supplied the
earlier cursor improvement. With lifetime splitting disabled, reusing
`countTotal` and `target` for the initial item scan gives retail's count/index
registers. A distinct `previousPageTop` prevents the page-up expression from
sharing the page-down local's allocation. Call order, loop bounds, item access,
selection, sound and global updates are unchanged. This is a documented
byte-match-first result, not a strict unshaped source-recovery claim.

Acceptance evidence in ignored `build/decomp-work/rea-audit/acceptance.json`:

- All 1,216 raw text bytes and all 70 normalized relocation records are equal
  to the retail object. The only nonempty allocated section is `.text`, aligned
  to four bytes; no extra function or data is emitted.
- Raw text SHA-256:
  `0f364dc52954e810b78ebf3c7084c9cd7489839e97f1b74dd9649efe6229cd53`.
- Report SHA-256:
  `9f2a360cd0344bc0e2defc4a172f3a5a73fda05178e8a67576c245a3442a8851`.
- Configure, all_source/report, full link and progress passed. All 26 canonical
  DOL/REL hashes passed after the source unit was actually selected for linking.
- The native checker accepted both tagged/listed pragmas. The working-source
  quality scan, all 41 quality tests, the 2,364-file wrapper scan and metric
  integrity check passed. No function regressed against the frozen baseline.
- Gain: one exact function, one complete unit, and 1,216 exact/linked code bytes.
  This does not claim completion of other PDA/menu functions or native binding.

The sections below preserve the earlier non-exact trial and search history.

## Initial Trial Result (Historical)

REA **did help**. Its masked instruction search found a matched local example,
`menuDbgItemCreateCursor`, which led to a measured source improvement:
**99.93421% -> 99.96710%**. The down-cursor branch now updates its globals
directly, without either the cursor or top temporary. Merely changing the
increment spelling, as earlier probes did, was insufficient.

The initial canonical report confirmed 302 of 304 instructions match. The two
remaining differences are the first loop's count register at function offsets
`+0x3c` and `+0x5c`: retail uses `r26`, the candidate uses `r28`.
At that initial-trial checkpoint, `main/game/menu/pda_candidate_80039A50` was
still `CodeCandidate`, incomplete and unlinked. That initial improvement added
no exception, assembly, split, or compiler-option change.

This trial does not establish that REA cannot help further. It establishes a
concrete benefit, an audited set of target-applicable capabilities, and the
remaining compiler-allocation question. An unlimited search or reverse
engineering MWCC itself is not claimed to have been exhausted.

## Inputs And Reproduction

- REA `6.5.0`, Ghidra `12.1.4`, JDK `21.0.10`, temporary Node `24.22.0`.
- Optional Rizin `0.9.1` installed and exercised through REA, not silently skipped.
- Local retail and baseline candidate linked ELFs were compared at identical
  addresses. The candidate differs from the canonical DOL by exactly four
  bytes, corresponding to the original four register-operand differences.
- REA rejects the embedded-PPC ELF flag. Only analysis copies had `EF_PPC_EMB`
  cleared; executable sections, original binaries, config, and assets were not
  altered. The target is PowerPC 32-bit big-endian; generic PowerPC analysis is
  not a claim of complete Gekko paired-single support.
- Both complete 1,216-byte REA memory reads were checked against the immutable
  input ELF bytes. REA's separate load-image verifier explicitly does not
  support ELF; its full observations were exported despite the response-size
  limit. It was not reported as a passing independent verifier.
- Full local evidence, schemas, input hashes, source probes, and reproduction
  drivers are in ignored `build/decomp-work/rea-audit/`. Retail disassembly,
  objects, ELFs, and generated products are not committed.

The tool-by-tool disposition is in
[`rea-fn_80039A84-tool-coverage.csv`](rea-fn_80039A84-tool-coverage.csv).
It covers all 207 advertised tools, distinguishing actual executions, duplicate
views, metadata operations, unavailable routes, and unrelated target formats.
It does **not** claim 207 successful executions.

## Analysis Coverage

| Area | Work performed and outcome |
| --- | --- |
| Function recovery | Full retail and baseline candidate dossiers: pseudocode, all 304 instructions, 74 blocks, callees, references, strings/names, API and value flow. No target-body truncation. |
| P-code / def-use | Followed the count's zero extension through its signed comparison and branch; followed the cursor increment through bounds checks and stores. Block-member operations were retained even when Ghidra marked them dead. No hidden use of the first count was recovered. |
| Cross-function flow | Whole-callback depth-one traversal exposed missing argument recovery and deeper boundaries. Separate depth-four traces for `pcboxGetNbItemSlot` and `pcboxGetItem` completed without truncation. Deeper sound-system calls do not explain this local allocation. |
| Types / ABI | Inspected recovered API, count helper, and cursor global. Ghidra reports the cursor as `undefined4`, not an original C signedness declaration. Generic calling-convention and return-type guesses remain guesses. |
| Instruction semantics | Ghidra instruction inspection and Rizin ESIL/RzIL covered the mismatching count and cursor operations. These describe instructions, not the MWCC allocator. |
| Call graph / closure | Ghidra call graph and call-target resolution identify the nine direct callees. Ordinary xrefs returned no caller. A complete initialized-memory byte search located the callback pointer at `0x802E31D0`; an empty xref list was not treated as absence. |
| Matched references | Exact-register patterns found only the target. Broader register masks found eight cursor idioms and four touch-loop occurrences, including local matched source in `menuDbgItemCreateCursor` and `fn_8003956C`. Inspected these examples before compiler tests. No public Colosseum source was consulted. |
| Differential analysis | `compare_functions` reports unchanged identity, pseudocode, comments, calls, references, strings/names and CFG; assembly alone changed. `find_changed_behavior` retains this as a structural difference, not observed gameplay behavior. |
| Verification / unknowns | A finite specification passes CFG and pseudocode equality but fails assembly equality. The unresolved allocation is recorded explicitly. Reconstruction-ledger support is exercised as acceptance bookkeeping, not as a substitute for objdiff. |
| Alternative engine | Rizin runs successfully and leaves input hashes unchanged. Its automatic function recovery reports only 72 bytes and fails to classify some PPC conditional branches; ESIL is absent for several instructions. Its recovered function/CFG is therefore not accepted as complete. |
| Evidence lifecycle | Exported canonical bundles, imported retained evidence, compared explicit dossiers, kept provider limitations and unknowns, and closed analysis sessions. Large-response failures were retained and investigated rather than mistaken for empty results. |

## Scope Boundaries

- Hopper explicitly does not admit PowerPC; REA's IDA adapter has no verified
  PowerPC lane. Installing those providers would not fix this adapter boundary.
- REA native call/UI recording requires Mach-O. Frida and host GDB do not
  provide a GameCube execution environment. No compatible live console or
  emulator debug endpoint was available in this trial. No native-port code was
  changed and no static result was described as a headed/runtime observation.
- Offline binary-layout/core inspection uses a Linux/x86-64 provider. The
  target's PowerPC ELF, bytes and relocations were instead inspected through
  Ghidra, objdump and Rizin.
- Navigation, bookmarks, comments and renames cannot recover original source
  types or choose MWCC registers. The reviewed annotation surface has no C
  type-reconstruction oracle. No speculative metadata was promoted to fact.
- Android/ADB/JEB, managed code, Objective-C/Swift, web/Electron/JavaScript,
  WASM/EVM, Dart, Go/Rust metadata, firmware extraction, packet/signal decoding,
  SQLite and Apple package/UI resources do not describe this MWCC C callback.
  Their sections were reviewed and explicitly excluded, not called with fake
  inputs to inflate coverage.

## Compiler Experiments

118 serial isolated builds were scored with the project's objdiff CLI:

1. 27 baseline/bounded-count/cursor variants. Three equivalent direct-global
   cursor spellings reached 99.96710%; the postincrement form was integrated.
2. 43 baseline/ignored-result counting-helper/parameter variants. No further gain.
3. 48 item-local helper variants, varying declaration order, return shape and
   bounded-count type. No further gain.

The matched example informed the winning hypothesis; REA did not generate an
exact C replacement. The type/loop tests preserve the existing best source and
are recorded separately from successful integration.

## Initial Trial Validation (Historical)

- `python3 configure.py --no-progress`: passed through the campaign build lock.
- `ninja all_source build/GC6E01/report.json`: passed; report confirms 99.96710%.
- `ninja`: passed; **26 canonical files OK**.
- `python3 configure.py progress`: passed.
- Wrapper scan: 2,364 source files, no live `.inc` wrappers.
- Working-tree quality scan: passed using the repository scanner's
  `added_lines_from_diff` and `scan_source` against the actual uncommitted diff.
  The standalone CLI requires two commits and cannot scan a worktree directly.
- No regressions against the tracked remaining-functions CSV.
- The build retains pre-existing Shift-JIS and linker FP-setting warnings.

At this initial checkpoint, passing canonical hashes did not mean the candidate
was linked: its retail object was still selected by `CodeCandidate`. Only the
later full-source acceptance described above clears this unit's decomp gate.

The initial obligation ledger remained `unknown`: it had a real source owner
and retail evidence, but no passing exact fixture/verifier and an open allocation
question. The later acceptance record supersedes that initial ledger for this
unit; it does not claim completion of other PDA units.

The temporary npm cache is removed after the trial. Reinstall the pinned CLI
with `npm exec --package=node@24 --package=rea-agents@6.5.0 -- rea` to repeat it.
Ghidra was already installed. The optional Rizin installation used Homebrew and
also installed/upgraded its declared dependencies; it did not modify the harness
or launch any campaign worker.

## Pipeline Lessons

Use REA as a context-and-reference layer around m2c: retain full dossiers and
def-use coverage, search already-matched local idioms with register masks, and
test entire dependent expressions rather than one spelling in isolation.
Use objdiff and the canonical source link as the acceptance authorities.

## Allocator Follow-Up, 2026-10-16

The initial follow-up batches preserved the live cursor improvement and the
99.96710% score. No lower-scoring candidate was applied to source. The final
allocator-guided refinement described above then completed and linked the unit.

- 306 serial source probes compiled: 105 loop/lifetime variants, 39 caller-local
  reuse variants, 62 helper-order/prototype variants, 24 count-parameter
  variants, 22 nested-helper variants, 11 optimizer controls, 16 direct-expression
  variants and 27 signedness/cast variants. None beat the incumbent. Evidence:
  `build/decomp-work/rea-audit/lifetimes/`.
- A four-minute local permuter run, with branch-target checking enabled,
  reached iteration 485 with 14 errors and no saved improvement. Its base
  score was 10. Evidence: `build/decomp-work/rea-audit/local-permuter/`.
- Eleven installed compiler versions were tested on the same improved source.
  GC/1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1, 2.5, 2.6 and 2.7 all scored 99.96710%.
  GC/1.1, 1.2.5 and 1.2.5n scored 88.26316%. The canonical compiler is unchanged.
  Evidence: `build/decomp-work/rea-audit/compiler-versions/`.
- Direct objdiff between the GC/1.3 and GC/2.6 candidate functions reports
  100%, all 304 instructions, zero changed rows. This makes the existing
  GC/2.6 MWCC debugger a useful next diagnostic lane. It does not make either
  candidate exact against retail.
- The debugger runner source is pinned to retrowin32 commit
  `11dbea5a68af21121511a6577a2d4a2f917da6dc` in ignored build storage. GDB's
  i386 architecture support was checked. The runner built successfully; complete
  frontend/backend and allocation dumps were captured for the incumbent and
  the count/index-reuse variant in `mwcc-trace/` and `mwcc-trace-reuse/`.
- The incumbent bound is virtual r61, allocated to r28 before the first lookup
  helper's locals; the index is virtual r62, allocated to r30. Of 18 subsequent
  reuse/optimizer cross-checks, `countTotal`/`target` reuse with `opt_lifetimes off`
  fixed that pair and left only three page-up operands different (99.95066%).
  The first separate-page-up-local probe then reached 100%. In total this
  follow-up measured 325 source probes and 11 compiler-version builds, plus the
  separately counted local permuter run and canonical acceptance builds.

Inlining the first loop into the caller can move its bound into retail's r26,
but then its index moves from r30 to r25. Passing that bound to an iteration
helper returns to the incumbent allocation. This is evidence to inspect the
allocator's priority/interference/coalescing decisions, not a reason to combine
instruction operands from different candidates or add assembly.

Key baseline evidence IDs:

- Retail dossier: `ev_048413210a970e14adfc8ee3913a95b868a94a8e3de78df66ab16e377c175a32`
- Candidate dossier: `ev_bebc9aa70219168b75958cfbf17bcf074d4ce24d5e56ea37fa1cc87b30162c7b`
- Function comparison: `ev_de690cf023f2ff622f2aa30ebe22bab4ce8384bf418af7afd0cfffce33ca8eb9`
- Open allocation question: `unk_08be643140597cc042ab934ef44f8c77e922b54f8d8570fab32ebd4d0ee0eb34`
