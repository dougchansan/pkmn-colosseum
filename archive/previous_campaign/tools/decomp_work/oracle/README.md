# Behavioral-equivalence oracle (DolRecomp differential)

Machine-verifies that decompiled C is *behaviorally identical* to the original
ROM code. Two uses: upgrade `equivalent.txt` (judged → differential-tested), and
act as a **correctness net** on UNVERIFIED C where a surviving mismatch is a real
caught bug.

## Correctness-net deployment (unverified C)

Run on 14 flat-struct getters in `battle_logic.c` / `colosseum_event.c` that are
NOT in equivalent.txt (a divergence would be real): **13 PASS, 0 surviving
MISMATCH, 1 FAILED (callee-tree crash).** No decomp bug found — but the run
caught and fixed **two harness bugs** that had produced FALSE mismatches:

1. **Field-map window-bleed** — `fn_body` used a fixed 0x2000 slop window that
   bled past a 0x18-byte getter into dozens of neighbors, mis-seeding a u8-at-0
   read as `{0,0,2}` (wrong width) → byte-endianness false mismatch. Fixed by
   bounding the window EXACTLY to the function size from `symbols.txt`
   (`ORACLE_SYMBOLS` env or auto-discovered). A `u8 fn(u8*ptr){return ptr[0];}`
   now maps to exactly `{0,0,1}`.
2. **Return-normalization mis-fire** — a `u32` value getter whose field value
   coincidentally fell in the host-mirror address window had its return
   "normalized" host→guest, corrupting it by the constant mirror-base delta
   (the tell: `A-B` identical across different inputs). Fixed: host→guest
   normalization is applied only when it RECONCILES B to A (genuine pointer
   return); a data value is never mangled. The gate (fightSideGetStatus, which
   has real pointer-returning cases) still passes, confirming the pointer path
   is intact. KNOWN LIMITATION: this reconcile could in principle mask a
   pointer-arithmetic bug whose wrong pointer normalizes exactly to A — a
   negligible coincidence, and it cannot mask a data-value bug.

Both were caught by the MISMATCH discipline (refuse to call a divergence a bug
until artifacts are ruled out) — exactly its purpose.

### Correctness-net reach (honest)

The net's reach on real game functions is NARROW. A first batch of 14
clean-looking `battle_logic.c` functions came back almost entirely
LIMITED/FAILED: they take an INT index (not a pointer), derive their base from a
CALLEE's return (`fn_801EF1E4(0)`), or are `void`-return — none auto-seedable.
The net verifies cleanly only **flat-struct pointer-arg getters** (single `u8*`
arg, direct `*(T*)(ptr+off)` reads). That class is real and now banked, but it is
a minority of game code; coordinators, callee-base getters, and void setters need
the hand-harness or the (not-yet-built) global-write snapshot.

## Idea

`tools/decomp_work/equivalent.txt` lists functions whose decompiled C is correct
and complete but does **not** byte-match (compiler-codegen walls: reg-alloc,
scheduling, jumptable naming, FPR coloring, etc.). The oracle runs, on identical
inputs:

- **A-side** — the ORIGINAL function's machine code, statically recompiled to C
  by [DolRecomp](https://github.com/ExpansionPak/DolRecomp) from
  `orig/GC6E01/start.dol`, executed on a flat big-endian guest RAM
  (`CPUState`, base `0x80000000`, 24 MB).
- **B-side** — OUR decompiled C, extracted **verbatim** from `src/`.

If return value + memory side-effects match across a structured sweep + fuzz,
the function is **machine-verified equivalent**.

## Proven results

| target | file | shape | result |
|--------|------|-------|--------|
| `fightSideGetStatus` (0x801F76B8) | `src/game/pokemon.c` | jumptable dispatcher, 8 callees, SDA table | **2050 cases, 0 mismatches** |
| `fn_80194510` (0x80194510) | `src/hsd/hsd_cobj.c` | `f32(u8*)`, SDA2 float consts, switch | **5 in-scope cases, 0 mismatches** (case 1 = Tier-2, see below) |

Both confirm the corresponding `equivalent.txt` walls are purely cosmetic
(jumptable symbol name / FPR register permutation) — semantically identical.

## Files

- `oracle_diff.sh` — driver: extracts B-side, discovers + compiles the
  DolRecomp chunk(s), links the core runtime + DOL loader, runs the harness.
- `extract_src_fns.py` — pulls function bodies VERBATIM from a `src/` `.c` by
  brace-matching. Skips `#if 0 … #else … #endif` dead `asm` stubs and keeps the
  active C. No rewriting (trustworthiness): only strips inner `extern` decls of
  other extracted fns.
- `bside_shim.h` — minimal decls so verbatim src compiles standalone (types come
  from DolRecomp `core/cpu.h`; this only adds `NULL`).
- `harness_<fn>.c` — per-function memory model, input sweep, comparison
  (hand-harness; the most trustworthy, used for the proven results above).

### Full-program image + auto-harness (scaling)

- `oracle_runtime.h` — shared A-side runtime over the **full-program image**
  (`libcolo.a`: all 154 DolRecomp chunks + core + DOL loader, ~88 MB, built on
  the box). A 154-entry dispatch table runs deep call trees to their REAL
  callees (Tier-2 reach). Sets SDA bases r2/r13 and **HID2.PSE|LSQE** (paired-
  single enable — without it, float library code raises a 0x700 exception).
- `oracle_auto.sh <fn> <addr> <src.c> [callee...]` — AUTO oracle: no hand-written
  harness. Resolves the ABI from the src signature, extracts verbatim src,
  auto-generates callee thunks (route to recomp) + SDA `lbl_` storage, compiles
  the generic `auto_harness.c`, fuzzes 1200 inputs, compares return + RAM deltas.
- `resolve_abi.py` / `gen_thunks.py` / `gen_lbls.py` — ABI resolution, callee
  thunk generation (int + float-return variants), and SDA/global storage.
- `auto_harness.c` — the generic harness.
- `batch_run.sh <list>` — runs `oracle_auto.sh` across many functions, one
  status line each.

### Struct-aware seeding (`gen_fieldmap.py`) — the endianness unlock

Pointer-data getters used to be `AUTOHARNESS-LIMITED`: the little-endian B mirror
of big-endian guest RAM could not seed arbitrary struct reads across widths, so a
"mismatch" was a harness artifact. Struct-aware seeding removes that at the root.

`gen_fieldmap.py <chunks> <addr> <nargs> <ptr_arg_csv> <out.inc>` walks the
recomp-annotated C of the target **and the callees it passes a pointer to**,
recovering every `(lbz|lhz|lwz|lha|lfs|lfd) N(rB)` access whose base register
traces back to a pointer arg (following `or rD,rS` copies, `addi` offset
adjust, and `add rD,rS,rIdx` index arithmetic with the `cmplwi` bound). It emits
a `(arg,offset,width)` field map. Per fuzz case the harness seeds **each field
as ONE logical value — big-endian into A's guest RAM, little-endian into B's
mirror** (`-DORC_USE_FIELDMAP`), so both sides read the same logical value at
every field regardless of endianness/width. Overlapping fields are deduped
(widest wins) so a u16+u32 at the same offset can't clobber each other. The
callee thunks pass **pointer-width** args so a host mirror-pointer survives and
is converted to its guest address before entering the recomp.

Bounds: a field map with **>40 fields** signals a deep call tree that almost
certainly dereferences POINTER-typed (nested/child) fields — flat seeding would
put random bytes where a valid pointer must be (wild read / crash), so those are
refused as `AUTOHARNESS-LIMITED` (hand-harness needed). Crashes in a callee tree
(signal ≥128) are reported `AUTOHARNESS-FAILED (crash)`, never a PASS.

### GROUND-TRUTH GATE (mandatory before trusting any sweep)

`fightSideGetStatus` (0x801F76B8) is the gate: the hand-harness proved it
equivalent (2050 cases, 0 mismatches). The struct-aware auto-harness must
reproduce that — **real-coverage PASS, 0 mismatches** — or the seeding is wrong.

| | before struct-aware seeding | after |
|---|---|---|
| `fightSideGetStatus` | 1072/1200 mismatch (all artifacts) | **1200 run, 0 mismatch, 28 nonzero-ret → PASS** |

The gate is satisfied; the seeding reproduces ground truth. A number that cannot
reproduce the gate is not banked.

### BANKED machine-verified equivalent.txt functions

First real equivalent.txt machine-verifications via struct-aware seeding
(deterministic, real coverage, reproduce ground truth):

| fn | file | result |
|----|------|--------|
| `fn_80129280` | gs_field_world.c | **PASS** — 1200 run, 0 mismatch, 626 nonzero-ret |
| `fightSideGetStatus` | pokemon.c | **PASS** (gate) — 1200 run, 0 mismatch, 28 nonzero-ret |

14-ptr-deref sweep result: **2 banked PASS, 0 MISMATCH, 1 PASS-TRIVIAL, 7
LIMITED (deep nested trees / large field maps), 4 FAILED (struct-typed args
needing headers).** No divergence survived the now-correct seeding — no decomp
bug found.

### Verdicts the auto-harness emits

| verdict | meaning |
|---------|---------|
| `PASS` (nonzero-ret > 0) | machine-verified equivalent over the fuzz corpus, real coverage |
| `PASS-TRIVIAL` (nonzero-ret = 0) | sides agreed but every output was 0 — the fuzzer never drove a non-trivial path (coordinator function needing realistic state, not a leaf the fuzzer can exercise). **Not** a meaningful equivalence result |
| `MISMATCH` | the two sides diverged — **investigate**: real decomp bug or harness flaw |
| `UNREACHABLE(Tier-2)` | ran off budget / exception in the callee tree |
| `AUTOHARNESS-LIMITED` | dereferences fuzzed POINTER data AND struct-aware seeding could not recover a usable flat field map — i.e. a deep call tree / nested or child-pointer fields (field map empty, unparseable, or >40 fields). Flat seeding can't satisfy these; use the hand-harness. (Functions whose field map DOES resolve are now seeded and verified, not LIMITED.) |
| `AUTOHARNESS-FAILED` | could not resolve the ABI (`void` param list = implicit register/global args; varargs), the B-side wouldn't compile (struct-typed args needing headers), or the callee tree crashed (signal ≥128) — never a PASS |

### Trust boundary of the auto-harness (important)

The auto-harness is **sound** (it never claims a PASS it can't stand behind):

- **Fully trustworthy** for *scalar-arg* functions and *pointer-arithmetic-only*
  getters (`return base + idx*N + K`) — no pointer-data endianness involved.
  Proven: `fn_801F78D4`, `fn_801F7908` (pokemon.c) — real PASS, ~560/1200 nonzero.
- **Refused (LIMITED)** when the function dereferences fuzzed pointer data: there
  is no single byte layout that matches big-endian guest reads across widths and
  offsets, so a black-box data fuzz can produce a FALSE mismatch. Confirmed
  empirically: `fn_801F7824` (a 6-instruction, instruction-byte-identical getter)
  produced 4/1200 spurious mismatches until reclassified LIMITED. Such functions
  need a hand-harness (logical-field seeding), as done for `fn_80194510`.
- **Callees** are routed to their recomp originals via thunks, so the
  differential isolates the TARGET function's own codegen.

## Batch result (12 functions)

| fn | file | status |
|----|------|--------|
| fn_801F78D4 | pokemon.c | **PASS** (561 nonzero) — pure pointer math |
| fn_801F7908 | pokemon.c | **PASS** (585 nonzero) — pure pointer math |
| fn_801F7824 | pokemon.c | AUTOHARNESS-LIMITED (derefs fuzzed ptr data) |
| fn_8023CE60 | colosseum_script.c | PASS-TRIVIAL (cov=0; coordinator, needs real state) |
| fn_8023CFDC | colosseum_script.c | PASS-TRIVIAL (cov=0; coordinator) |
| fn_802389D4 | colosseum_script.c | PASS-TRIVIAL (cov=0; coordinator) |
| fn_800BD91C | GXInit.c | AUTOHARNESS-LIMITED (derefs ptr data) |
| fn_8000DEC4 | gs_npc_interact.c | AUTOHARNESS-LIMITED (setter, derefs ptr data) |
| fn_801A6DDC | hsd_mobj.c | AUTOHARNESS-LIMITED (`mobj->mat->alpha`) |
| fn_80194510 | hsd_cobj.c | AUTOHARNESS-LIMITED (byte selector + word data) |
| fn_800A4C94 | DVDLow.c | AUTOHARNESS-FAILED (`void` params = implicit args) |
| fn_8000C824 | gs_party_access.c | AUTOHARNESS-FAILED (`void` params) |

No `MISMATCH` survived investigation: the only divergence found (`fn_801F7824`)
was a harness endianness artifact on a provably-identical getter, now correctly
refused as LIMITED rather than reported as a decomp bug.

**Takeaway:** the auto-harness scales cleanly to the *pointer-arithmetic /
scalar* slice of `equivalent.txt`; the larger share (coordinators needing real
game state, pointer-data getters, and `void`-param implicit-arg functions) need
the hand-harness or realistic-state seeding. The honest split is the deliverable.

## Usage

Prereqs on the build host (the 3090 box `/tmp` layout is the default):

```sh
# 1. build DolRecomp
git clone https://github.com/ExpansionPak/DolRecomp /tmp/DolRecomp
cmake -S /tmp/DolRecomp -B /tmp/DolRecomp/build && cmake --build /tmp/DolRecomp/build -j4
# 2. generate C from the ORIGINAL dol
cp orig/GC6E01/start.dol /tmp/start.dol
/tmp/DolRecomp/build/dolrecomp --gamecube /tmp/start.dol /tmp/colo_out
# 3. run a differential
export DOLRECOMP_SRC=/tmp/DolRecomp/src GEN_DIR=/tmp/colo_out/generated ORACLE_DOL=/tmp/start.dol
./oracle_diff.sh harness_fightSideGetStatus.c path/to/src/game/pokemon.c \
    fightSideGetStatus fn_801F7858 fn_801F78BC fn_801F793C fn_801F77F0 fn_801F7824 fn_801F78D4 fn_801F7908
```

A new target = write one `harness_<fn>.c` (copy an existing one; set the entry
address, args, the fields its inputs read, and the return kind) + list the
target and any same-file callees to extract.

## How A-side execution works

DolRecomp emits each function as `func_<chunkbase>(CPUState*)` with a
`switch(ctx->pc)` over every instruction; a `bl` sets `lr`/`pc` and returns to
the host driver, which re-dispatches. The harness:

1. `cpu_init`; preload all DOL text+data sections into `ram` (so jumptables /
   `.rodata` / `.sdata2` resolve).
2. set `gpr[2]=0x804836A0` (`_SDA2_BASE_`), `gpr[13]=0x80480820` (`_SDA_BASE_`)
   — recovered from `__init_registers` @ 0x800032B0.
3. set args (`gpr[3..]`), plant `lr = 0x0BADF00D` (a sentinel owned by no
   function), `pc = entry`.
4. run the dispatch loop until `pc` returns to the sentinel; read `gpr[3]`
   (int return) / `fpr[1]` (float return) + RAM deltas.

## Trustworthiness & honest limits

- **B-side is the real src.** `extract_src_fns.py` copies bytes; it does not
  paraphrase. `#if 0` asm stubs are skipped so the *active* C is tested.
- **Endianness handled.** A-side RAM is big-endian (guest native via DolRecomp
  `mem_*`); the B-side runs on a little-endian host mirror seeded with the same
  logical values. Pointer results are normalised host→guest before comparison,
  keyed strictly on the return *kind* (never a value-range heuristic).
- **One host-ABI accommodation, documented.** `fn_801F7870` uses the decomp
  idiom `(u8*)(u32_value)` which is lossy on LP64; the harness provides a
  host-correct version preserving the exact count-gate + 0x14 stride. Isolated
  and labeled in `harness_fightSideGetStatus.c`.
- **Tier boundaries.**
  - *Tier 1 (this harness):* leaf / same-file-callee functions, fully verified.
  - *Tier 2:* functions calling into deep cross-chunk trees (e.g. `fn_80194510`
    case 1 → `fn_800CE220` tan-family). The minimal 2-chunk harness cannot
    evaluate those; such cases are reported separately and **NOT** counted as a
    pass. A full version compiles all 154 chunks (142 MB of C) so the
    whole-program dispatch reaches every callee.
- **Coverage caveat.** The oracle proves equivalence only over the inputs fed.
  For the confirmed wall classes (semantics-preserving codegen artifacts),
  structured-sweep + fuzz gives high confidence cheaply; it is not a proof over
  the entire input domain.
- **SMC.** DolRecomp flagged 106 self-modifying-code sites; a target overlapping
  one needs manual review before trusting its A-side.

## In-repo vs on-box

Only the harnesses + scripts live in the repo (`tools/decomp_work/oracle/`).
The DolRecomp build (`/tmp/DolRecomp`) and 142 MB of generated C
(`/tmp/colo_out`) stay on the build box — regenerate with the steps above.
