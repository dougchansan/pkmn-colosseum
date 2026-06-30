# AGENT ONBOARDING — Pokémon Colosseum matching decompilation

You are a coding agent joining an in-progress **matching decompilation** of Pokémon
Colosseum (GameCube `GC6E01`, compiler **CodeWarrior 1.3**, `-O4,p`). The tooling and
all the hard-won CodeWarrior matching tricks already exist. **Do not reimplement them.**
Pick a target, write C, measure, keep it if it byte-matches. This page gets you decompiling
in five minutes.

> Companions: **`AGENTS.md`** (full guardrails + current campaign), **`tools/decomp_work/HARNESS.md`**
> (the authoritative AI-free harness spec — architecture, the win lifecycle, the integration
> gate, how to extend it), **`WALLS.md`** (the wall ledger — what NOT to grind),
> **`tools/decomp_work/equivalent.txt`** (logged Equivalents = hard-skip),
> **`tools/decomp_work/CLAUDE.md`** (the 7 CW-1.3 quirks + Ghidra→CW pipeline). The distilled
> opus/Fable levers live in the table below — you usually don't need to leave this file.

Windows Python (use this exact path in every command):
`C:\Users\douglaswhittingham\AppData\Local\Programs\Python\Python312\python.exe`

---

## 0. The one tool you must know

```bash
# THE authoritative per-function match command. Compiles the file, objdiffs the symbol.
python tools/match_scan_file.py <src/file.c> <fn_XXXXXXXX> [more fns...]
```

Everything else is convenience. There is also a wrapper that bundles the common ops:

```bash
bash tools/decomp_work/decomp.sh measure <file_or_stem> <fn...>   # -> match_scan_file (authoritative %)
bash tools/decomp_work/decomp.sh next    [filter]                 # next non-100% targets, walls removed
bash tools/decomp_work/decomp.sh asm     <fn>                     # print the target asm (.inc)
bash tools/decomp_work/decomp.sh where   <fn>                     # which src/*.c file owns this fn
bash tools/decomp_work/decomp.sh walls   <fn>                     # is this fn a logged wall? (skip if yes)
```

---

## 1. QUICKSTART loop (copy/paste)

```bash
# (0) Make sure the function isn't already a logged wall — if it is, SKIP it.
bash tools/decomp_work/decomp.sh walls fn_800DF21C

# (1) Pick a near-miss target (closest-to-100 first; logged walls already removed).
bash tools/decomp_work/decomp.sh next

# (1b) Find which source file owns it.
bash tools/decomp_work/decomp.sh where fn_800DF21C

# (2) Read the TARGET assembly you must reproduce (the included .inc).
bash tools/decomp_work/decomp.sh asm fn_800DF21C
#   ...or open it directly:  src/<dir>/<stem>_fn_800DF21C.inc

# (3) Find + read the C. The function is in src/<dir>/<stem>.c. Two wrapper forms exist
#     (mind which branch is ACTIVE — `#if 0` is false, so its `#else` is what compiles):
#       Form A — real C ACTIVE (this is your target; repair the C in the #else):
#         #if 0
#           asm void fn_XXXX(...) { #include "..._fn_XXXX.inc" }   // parked, inactive
#         #else
#           <real C>                                              // ACTIVE — edit THIS
#         #endif
#       Form B — asm ACTIVE (#if 1 + .inc): already byte-exact, a reloc-name artifact at
#         most. `band sections` auto-skips these. Do NOT make it a target / never flip it.

# (4) MEASURE — the single source of truth. Never assert a % you didn't measure.
python tools/match_scan_file.py src/game/gs_render.c fn_800DF21C
#   100.0%  -> keep it. Done.
#   < 100   -> VIEW the residual, apply a lever, re-measure:
#     bash tools/decomp_work/decomp.sh band init s1 src/game/gs_render.c   # one-time per file
#     bash tools/decomp_work/decomp.sh band diff s1 fn_800DF21C            # TARGET vs OURS, >>> = differs
#              After ~3 honest tries with no progress, treat it as a wall (section 5) and move on.
```

> MEASUREMENT AUTHORITY: `match_scan_file.py` (= `decomp.sh measure`) and `band check/json`
> are authoritative (live objdiff). `decomp.sh next` / `equiv_pool.txt` are a CACHE and can be
> STALE — a fn it lists at 99% may already be 100% (skip it) or measure lower. Always confirm
> the live % before attacking and before claiming a win. Regenerate the pool (lead only, slow):
> `python tools/decomp_work/progress2.py --measure`.

`bash tools/decomp_work/decomp.sh asm fn_X` prints nothing/“no .inc” when `fn_X` is
**already fully decompiled** (real C, no asm form) — that's expected, not an error.

---

## 2. HARD RULES (read these once, never break them)

1. **Never edit `*_fn_*.inc`.** They are the ground-truth target machine code. Editing one
   to forge a match has happened before — it is fraud and is reverted on sight.
2. **Never flip `#if 0` → `#if 1` to re-activate an asm wrapper for a function that has real
   C.** An asm wrapper byte-matches *by definition* but is **0% decompiled** and useless to
   the PC port (x86 can't run PPC). Re-wrapping a near-miss to make the objdiff number tick
   up is a **regression of honest decompilation**. The only legitimate asm wrapper is for a
   **confirmed, logged wall** (real C written, proven unmatchable) — and the real C stays
   parked under `#else`.
3. **Real C beats asm-wrappers everywhere.** The PC port needs C. If your C can't reach 100%
   but is correct, leave it **active** and log it as an Equivalent (section 5) — do not
   re-wrap it.
4. **ALWAYS re-measure with `match_scan_file.py`.** Do not self-assert a percentage. Sub-agents
   (and humans) have hallucinated "100%". If you didn't run the command this turn, you don't
   know the number.
5. **Don't touch `config/GC6E01/symbols.build.txt`, jump-table labels, struct layout, field
   signedness, or function signatures** unless the change *is* the matching fix and you've
   validated it. Keep diffs minimal.
6. **Coordination — take a lock before you edit.** Before editing a file or attacking a
   function, claim it atomically: `python tools/decomp_work/coordination/locks.py
   acquire-file <agent> src/game/<file>.c` (whole-file) or `… acquire <agent> fn_XXXX
   --file src/game/<file>.c` (one fn). Exit 1 = DENIED → someone else owns it, pick another
   target. `renew <agent>` periodically (locks auto-expire after 30 min); `release` when
   done. See `tools/decomp_work/coordination/LOCKS.md`. When you finish, the parent
   re-measures before trusting/committing.
7. **NEVER push to master. Work on a branch and open a PR.** Direct pushes to `master`
   are **hard-blocked** by a `pre-push` git hook (`.githooks/pre-push`, enabled via
   `core.hooksPath`). Your workflow is: `git switch -c decomp/<agent>-<topic>`, commit your
   verified wins there, then `bash tools/decomp_work/handoff.sh` (pushes the branch + opens
   a GitHub PR for review) — or click **Ship → Prepare handoff** in the dashboard. Every
   change is reviewed as a PR, not written blindly to origin/master. (Human override for an
   intentional master push only: `ALLOW_MASTER_PUSH=1 git push ...`.)

---

## 3. THE METRIC — three honest axes (know which one you're moving)

Run `python tools/decomp_work/progress2.py --measure` for the live numbers. Three axes:

| # | Axis | ~Value | What it is | How to grow it |
|---|---|---|---|---|
| 1 | **scan C-converted** | **~83%** | how much C is *written* (real C, match or not) | convert ASM/STUB → real C |
| 2 | **objdiff ROM fn-match** | **~58–63%** | **the decomp.dev / README "Function match" number.** Counts asm-wrappers as matched, so it *over*-states real decomp. | byte-exact C **or** symbols.build.txt reloc renames |
| 3 | **byte-exact C** (strict) | **~42%** | real C that *actually* byte-matches | win a near-miss to 100% |

- **Your job moves axis 3** (and therefore axis 2 honestly): take a real-C near-miss to a
  byte-exact 100%. Renames are byte-neutral (don't move axes 2/3 except the reloc-name case).
- The **decomp.dev / README headline** is axis 2. The live web dashboard is at
  **http://127.0.0.1:8792** (when running locally).
- `match_scan_file.py` runs objdiff **without** `ppc.calculatePoolRelocations=false`, so a
  function whose only residual is an anonymous `@nnn` int→float pool constant can read a few
  points lower here than its "true" % (e.g. a logged 99.39% may show ~91%). That gap is the
  W6/`@nnn` artifact (section 5), not your C being wrong — verify the residual before chasing it.

---

## 4. LEVER CHEAT-SHEET — apply on a near-miss

Read the objdiff residual, find the matching **symptom** row, apply the **fix**, re-measure.
This consolidates Fable's levers + the 7 CW-1.3 quirks (`tools/decomp_work/CLAUDE.md`) +
the WALLS.md taxonomy. Levers marked **OPT-GATED** only bite below `optimization_level 4`
(at opt4 CW copy-propagates them away).

> CW 1.3 IS A C90 COMPILER: every local **declaration must sit at the top of its block**,
> before any statement. `u16 chk = fn(...);` mid-block is an `expression syntax error`. When a
> lever says "add a local", declare it at the block top, then assign below.

> PRAGMAS MUST BE IN-BODY for band integration: put `#pragma peephole off` (etc.) on the line
> *after* the function's opening `{`, NOT as an external `#pragma push/pop` wrapper around the
> def. The band splice carries only the function definition, so a wrapping pragma is dropped at
> integrate time and the win silently regresses. In-body pragma → it travels with the def.

| # | Symptom in the objdiff residual | Fix (lever) |
|---|---|---|
| 1 | Only diff is a `bl <name>` / `lis name@ha` **reloc name** (same address, bytes identical) — e.g. `bl fn_80053110` vs `bl dbgMenuFightWazaEditSub` | **Reloc-name rename** (biggest yield, byte-neutral): rename your C identifier to the name in `config/GC6E01/symbols.build.txt` for that address (extern + callsites + same-TU def). Delete any stale K&R `extern void Name();` that collides. |
| 2 | `mr rN,r3` / extra `crset cr1eq` (vs `crclr`) right before a varargs `bl` | **Dropped vararg arg.** `crset cr1eq` = dropped a **float** arg; an extra `mr rN,r3` = dropped a **leading int** arg. Add a file-scope `extern T fn(fixed, ...);` and pass the missing argument. |
| 3 | Target `*p` accessed via displacement store `stw 0xNN(rX)`; yours collapses to `stwx`/`lwzx` | **add+displacement form:** write `((u32*)(p + 0x6C))[i] = v` (keeps `add`+`stw 0x6c`), not the flattened `*(u32*)(p + i*4 + 0x6C)` (collapses to `stwx`). |
| 4 | `*(u32*)p |= 0xNNN` emits `li/and` or `lis/or`; target has a single `rlwinm` / `oris` | **Typed-field flag op:** `((T*)p)->flags |= 0xNNNu` / `&= ~0xNNNu`. The **`u` suffix is load-bearing** for high-bit (≥0x10000) ORs → `oris`. |
| 5 | Target reads the same byte/word **twice**; CW caches it once (`lbz r4`…reuse) | **volatile cast** to defeat CSE: `((u32)*(volatile u8*)p >> 7) & 1`. Forces the re-load. |
| 6 | Target loads a label via `li rX, lbl@sda21`; yours emits `lis@ha; addi@l` (absolute) | **sda21 scalar extern:** declare `extern u8 lbl;` (scalar, not `[]`) and pass `&lbl`; or use `lbl` directly as a scalar value. |
| 7 | Prologue/epilogue: target `stmw r2N` / `lmw`; yours has individual `stw`/`lwz` (2–4 saved regs) | **`-O4,s` stmw threshold (W3, dissolved):** set this TU to `-O4,s` in `config/GC6E01/compile_config.json`, or `#pragma push; #pragma optimize_for_size on; …; #pragma pop`. **Gate on whole-TU 0 regressions** — some TUs are genuinely `-O4,p`. |
| 8 | Ghidra-imported fn has `void fn(void)` + `u32 r3; r3=0;` pseudo-register locals; CW deletes the param | **Real signature:** give the actual `(u32 r3, u32 r4, …)` params + a forward decl; drop the uninit pseudo-reg locals. |
| 9 | A value materializes in the wrong scratch reg / first-use coloring is off | **block-scope decl demotion** (**OPT-GATED**): move a decl into an inner block / reverse decl order within a block to push it toward the band bottom. Only works below opt4. |
| 10 | A run of contiguous word/byte stores in the **wrong order** | **named-struct block copy:** `*(struct Tag*)dst = *(struct Tag*)src;` (a *named* tag — anon casts fail) → CW's `lwzu/stwu/bdnz` interleave. |
| 11 | Target has a one-case `switch` shape: `beq L; b end; L:` with a dead `cmpwi` | **one-case switch:** `switch(x){ case 0: …; break; }` (add `case 0:`+`default:` sharing a body for the dead-`cmpwi` prologue, quirk 1). |
| 12 | Spurious `extsh`/`extsb` before a truncating `sth`/`stb` | **direct narrow cast:** store `(s16)expr` (not `(s16)(s32)expr`); declare the local `s32` and put the `(s16)`/`(s8)` cast at the *use* site, not the assignment (quirks 2, and `s16` direct-cast). |
| 13 | An unsigned compare uses `extsb/cmpwi` but target uses `clrlwi/cmplwi` (or vice-versa) | **`u8` vs `s8` retype** the local to match the compare signedness; or **narrow-at-def**: declare a `u32` local whose source extern returns `u8`/`u16` so CW masks at assignment (`clrlwi rN,r3,24`), not at the compare. |
| 14 | Call goes through `mtctr/bctrl` (indirect) but target has a direct `bl` | **uncast direct call:** call the named function without a cast-to-fn-ptr; add a block-local `extern <ret> fn(<args>);` if the global signature doesn't fit. |
| 15 | `val = fn(...) + k;` puts the call result in a fresh reg; target accumulates in place | **accumulate split:** `val = fn(...); val += k;` (flips to call-result-first `add`). Winnable only when the saved var is genuinely live after (else it's W5, a wall). |
| 16 | `f32`/scalar frame is one slot too small/large vs target | **`f32 buf[N]` array** to force the larger frame; pick the *minimum* array size the code reads (quirk 3). |
| 17 | Null-check getter: target `beq null` first; yours `bne load` | **goto-ret0 inversion:** `if (ptr == NULL) goto _ret0;` to flip the branch sense. |
| 18 | List-walk spills the node to a non-volatile reg | **nested call keeps r3:** make the producing call the last statement / inline it into the consumer so CW keeps the pointer in r3 (quirk 5). |

Two pragmas show up repeatedly and are *whole-file* switches (not per-spot): `#pragma scheduling on/off`, `#pragma peephole off`. The `fp_contract on` pragma is needed for `fmadds` fusion. Use sparingly and gate on the whole TU.

---

## 5. WALL CLASSES + the STOP rule

A **wall** is a residual not controllable from C (no decl order, cast, pragma, version, or
expression reachable from CW 1.3 closes it). Mature decomp projects don't crack these — they
ship them as **Equivalent C** and move on. The taxonomy (full detail in `WALLS.md`):

| Class | Signature in the residual | Verdict |
|---|---|---|
| **W1** data-flow-locked reg-alloc | pure register *permutation* among param-fed / loop-carried values | wall (try lever 9 once if below opt4, else log) |
| **W2** instruction scheduling | residual is a *permutation of identical opcodes/operands* (no reg/imm differs) | wall |
| **W3** stmw/lmw threshold | residual concentrated in `stmw`/`lmw` vs individual `stw`/`lwz` | **try lever 7 first** (often dissolves); else wall |
| **W4** carry-vs-sign bool | fixed `subic/subfe` ↔ `neg/or/srwi` swap around a compare | wall |
| **W5** commutative fresh-reg add | which reg holds a commutative add operand, saved var dead after | wall (cousin of lever 15) |
| **W6** reloc/jumptable/call **name** | bytes identical; only a jumptable/reloc/`bl`-target **name** differs | lever 1 if it's a call/reloc name; **anonymous jumptable name = wall** (caps ~99.85–99.99) |
| **`@nnn` int→float artifact** | only diff is an anonymous `@140`/`@nnn` 2^52 magic-double pool constant vs a named `lbl_…(r2)` | wall (caps ~99.85); measurement artifact under our objdiff flags |

**STOP rule:** after ~3 honest attempts with materially different C shapes and no progress,
**stop grinding.** If the residual matches a class above (or you tested levers and it didn't
move), it's a wall.

**How to log a wall (so nobody re-grinds it):**
1. Add a row to **`WALLS.md`** (function, file, class, best %, evidence).
2. If your C is **real and correct** (just not byte-exact), add the `fn_XXXXXXXX` to
   **`tools/decomp_work/equivalent.txt`** (one per line, `# comment`). This counts it toward
   the C-converted axis and marks it HARD-SKIP. Leave the C **active**; do not re-wrap as asm.

---

## 6. WHERE THINGS ARE

| Thing | Path |
|---|---|
| Full guardrails + current campaign, model tiering, walls-to-skip | `AGENTS.md` |
| Wall ledger (taxonomy W1–W6, per-fn rows, two/three-axis model) | `WALLS.md` |
| Logged Equivalents (hard-skip list) | `tools/decomp_work/equivalent.txt` |
| Near-miss pool (every non-100% real-C fn + current %) | `tools/decomp_work/equiv_pool.txt` |
| CW-1.3 quirks (7 numbered) + Ghidra→CW pipeline | `tools/decomp_work/CLAUDE.md` |
| Authoritative per-fn measure | `tools/match_scan_file.py` |
| Whole-tree 3-axis progress | `tools/decomp_work/progress2.py [--measure]` |
| Parent re-measure gate (used before commit) | `tools/decomp_work/overnight/verify_match.py <stem> <fn>` |
| Convenience wrapper for all of the above | `tools/decomp_work/decomp.sh` |
| Target asm for `fn_X` | `src/<dir>/<stem>_fn_<ADDR>.inc` (next to its source) |
| Authoritative call-target / reloc names | `config/GC6E01/symbols.build.txt` (read-only) |
| Per-TU compiler flags (e.g. `-O4,s`) | `config/GC6E01/compile_config.json` |
| Distilled levers / session history (point-in-time) | `~/.claude/projects/.../memory/feedback_*.md`, `project_*.md` |

---

## 7. The two-minute mental model

- A `.inc` next to a source file = a function that still has an **asm form** (wrapper or
  parked under `#else`). No `.inc` = it's already **real C**.
- Files mix three states per function: **ASM-wrapper** (`#include "…inc"`, 0% decompiled),
  **STUB** (empty/`/*TODO*/`), **REAL_C** (decompiled). Your wins convert ASM/STUB → REAL_C
  that byte-matches.
- The objdiff headline counts wrappers as "matched" — so converting a wrapper to byte-exact
  C doesn't move axis 2 much, but it's the *honest* win and it's what the PC port needs.
- When in doubt: `decomp.sh walls <fn>` (is it a wall?), `decomp.sh asm <fn>` (what must I
  reproduce?), write C, `match_scan_file.py` (did it work?). Repeat.

---

## 8. Big-file band harness (parallel agents)

When several agents attack ONE big source file at once, they must **not** all edit the same
`src/<file>.c` (they'd clobber each other's edits and corrupt the shared base object). The
**band harness** gives each agent a PRIVATE scratch copy of the file and a disjoint set of
functions ("a band/section") to own. It is `tools/decomp_work/band.py`, exposed as
`decomp.sh band <args...>`. It is file-parameterized: compiler version, flags, and the
immutable target object are resolved per file from `config/GC6E01/compile_config.json` (same
as the real matching build), so it works on ANY `src/**.c`, not just colosseum_script.c.

> The harness is **read-only on canon**: it only reads `src/<file>.c`, writes private scratch
> to `tools/decomp_work/scratch/band_<tag>.*`, and saves verified wins to
> `build/band_wins/<tag>.json`. It never edits canonical sources or `*_fn_*.inc`.

### How sections are assigned (the parent/lead does this once)

```bash
# Split the file's active-asm + near-miss (<100%) fns into 4 disjoint bands.
bash tools/decomp_work/decomp.sh band sections src/game/<file>.c 4
```

This compiles the canon, objdiffs it vs the target, ranks the non-100% fns closest-to-100%
first, tiers them (A>=90, B>=75, C>=50, D<50), and round-robins them into N disjoint bands
`b0 b1 … b{N-1}`. Hand each agent ONE band's function list and a unique `<tag>`.

> **Lock the file first.** Each band agent should `acquire-file <tag> src/game/<file>.c`
> (see `coordination/LOCKS.md`) so two sessions can never pick the same TU — the SQLite
> lock is atomic where the old `claims.json` was not. Renew it during long grinds; release
> when the band is integrated.

`sections` auto-**SKIPS active asm-wrapper fns** (`#if 1` + `#include "..._fn_*.inc"`):
they are byte-exact in ROM and their sub-100% objdiff scores are pure reloc-name artifacts
(`jumptable@ha`, `lbl@sda21` vs raw addr) — NOT real-C targets. Only real-C near-misses are
banded; the header prints the skipped wrapper count. Never spend attempts on a wrapper.

### The loop each agent runs (copy/paste — replace `<tag>`, `<file>`, `<fn>`)

```bash
# (1) INIT a private scratch from the canonical file (remembers flags/target for <tag>).
bash tools/decomp_work/decomp.sh band init <tag> src/game/<file>.c

# (2) EDIT YOUR SCRATCH ONLY: tools/decomp_work/scratch/band_<tag>.c
#     Decompile the fns in YOUR band (apply the LEVER CHEAT-SHEET in section 4).
#     Do NOT touch src/<file>.c or any other agent's scratch.

# (3) CHECK your band's match% (compiles your scratch, objdiffs each fn).
bash tools/decomp_work/decomp.sh band check <tag> <fn1> <fn2>
#   (use `band json <tag>` for machine-readable all-fn %; `band diff <tag> <fn>`
#    for the aligned target-vs-ours asm when a fn is below 100%.)

# (4) SAVE only the byte-exact wins (the harness REJECTS anything below 100%).
bash tools/decomp_work/decomp.sh band save <tag> <fn1> <fn2>
#   -> persists verbatim defs to build/band_wins/<tag>.json (trusted-gate: 100% only)
```

### Parent integration (after agents finish — the parent trust gate)

```bash
# Merge every tag's saved wins back into its canonical src, RE-COMPILE the integrated
# file, and RE-MEASURE each fn. Only fns that still hit 100% in the integrated file are
# kept; any that regressed are reported and dropped. Dry-run by default:
python tools/decomp_work/band_integrate.py            # writes build/band_<stem>_integrated.c
python tools/decomp_work/band_integrate.py --apply    # also overwrites src/<file>.c in place
```

`band_integrate.py` trusts no agent's claim — it recompiles and re-objdiffs in the parent
before keeping anything, exactly like `cs_integrate.py` did for colosseum_script.c.
