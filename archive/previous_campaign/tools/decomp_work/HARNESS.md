# The Band Harness — authoritative spec & AI-free runbook

This is the **design spec and manual operating procedure** for the matching-decomp
band harness. It is deliberately AI-agnostic: every step here is a concrete command a
human (or a plain shell script) can run. `AGENT_ONBOARDING.md` is the agent-facing
companion (lever cheat-sheet + quickstart); this file is the ground truth for *how the
machinery works and why*, so the harness can keep improving without re-derivation.

Audience: anyone — human or agent — who wants to convert a near-miss function to a
byte-exact match and integrate it safely, or to extend the harness itself.

---

## 1. What the harness is for

Goal: replace original PowerPC machine code (`*_fn_<addr>.inc` asm includes) with **real
C** that the CodeWarrior 1.3 compiler turns back into the *byte-identical* instructions.
A function is "done" when its compiled bytes match the target object exactly (objdiff
reports 100.00%).

The harness exists to make that safe **in parallel**: many workers can each attack a big
file at once without corrupting the shared build or each other, and no win enters the
canonical source until it has been independently re-measured.

### The 3-axis metric (know which number you are moving)
- **byte-exact-C** — compiled C that matches the target byte-for-byte. The real goal.
- **C-converted** — function has real C (not an asm include), matching or not. Counts
  toward the PC-port goal even if not byte-exact.
- **ROM fn-match** — objdiff's fuzzy %, the public decomp.dev headline number.

Authoritative per-fn measurement is **always** an objdiff against the canonical base
build — never a self-asserted percentage. `band.py` and `match_scan_file.py` both do this.

---

## 2. Components & data flow

```
config/GC6E01/compile_config.json   per-file compiler version + cflags (-O4,p vs -O4,s, etc.)
        │  (compile_check.py reads this)
        ▼
src/<dir>/<stem>.c  ──init──▶  scratch/band_<tag>.c   (private working copy; edit THIS)
        │                              │ compile (mwcceppc, file-correct flags)
        │                              ▼
        │                       scratch/band_<tag>.o
        │                              │ objdiff -1 <target_o> -2 <scratch_o>
        ▼                              ▼
build/.../base/<stem>.o          per-fn match%   ──save(100% only)──▶ build/band_wins/<tag>.json
(canonical base, untouched                                                    │
 until integration)                                                           │ band_integrate
                                                                              ▼
                                                  splice into a fresh copy of canon, RE-COMPILE,
                                                  RE-MEASURE every spliced fn → keep only the ones
                                                  still 100% → (--apply) overwrite src/<stem>.c
```

Key invariant: **the canonical `src/<stem>.c` is never edited by a worker.** Workers only
touch their private `scratch/band_<tag>.c`. Canon changes exactly once, at integration,
and only for functions that re-verify at 100%.

### Files
| Path | Role |
|---|---|
| `tools/decomp_work/band.py` | the harness CLI (init/check/json/diff/save/sections) |
| `tools/decomp_work/band_integrate.py` | the parent trust-gate: merge → recompile → re-measure → keep/drop |
| `tools/decomp_work/cs_splice.py` | locates a function's definition span and splices a new body |
| `tools/decomp_work/decomp.sh` | thin wrappers (`measure`/`next`/`asm`/`where`/`walls`/`band`) |
| `tools/decomp_work/scratch/band_<tag>.c` | a worker's private editable copy |
| `tools/decomp_work/scratch/band_<tag>.src` | JSON sidecar: {src, target_o, cflags, compiler} for the tag |
| `build/band_wins/<tag>.json` | verified-100% definition bodies + `_src` key, per tag |
| `config/GC6E01/compile_config.json` | per-file flags/version overrides (the build truth) |
| `WALLS.md`, `tools/decomp_work/equivalent.txt` | the wall/Equivalent ledgers (see §6) |

---

## 3. The deterministic win lifecycle (manual, no AI)

Run everything from the repo root. `<tag>` is any short private label (e.g. your initials
+ a number). `<file>` is `src/<dir>/<stem>.c`.

```bash
# 0. (lead, once per file) split the file's REAL-C near-misses into N bands.
#    Active asm-wrappers (#if 1 + .inc) are auto-skipped — their sub-100% scores are
#    pure reloc-name artifacts, NOT real targets. The header reports the skipped count.
python tools/decomp_work/band.py sections <file> 4

# 1. init a private scratch (remembers the file's flags/version/target under <tag>).
python tools/decomp_work/band.py init <tag> <file>

# 2. edit ONLY tools/decomp_work/scratch/band_<tag>.c — repair the real C of ONE fn.
#    (See AGENT_ONBOARDING.md §4 for the lever cheat-sheet.)

# 3. re-measure (compiles the scratch, objdiffs each fn vs the immutable target).
python tools/decomp_work/band.py check <tag> <fn>     # table, one or more fns
python tools/decomp_work/band.py json  <tag>          # machine-readable, all fns
python tools/decomp_work/band.py diff  <tag> <fn>     # aligned target-vs-ours asm

# 4. when (and only when) a fresh check shows <fn> at 100.00%, persist it.
python tools/decomp_work/band.py save <tag> <fn>      # trusted gate: REFUSES anything <100%

# 5. (lead) integrate every tag's saved wins. Dry-run by default; --apply writes canon.
python tools/decomp_work/band_integrate.py            # all tags
python tools/decomp_work/band_integrate.py <tag> ...  # only these tags
python tools/decomp_work/band_integrate.py --apply    # overwrite src/<stem>.c with verified wins
```

### What `save` and `band_integrate` actually guarantee
- `save` recompiles the scratch and **rejects any fn below 100%** — you cannot save a lie.
- `band_integrate` is the **independent re-verification gate**. It splices the saved bodies
  into a *fresh* copy of canon, recompiles with the file's real flags, and re-measures each
  fn. A fn that does not still read 100% post-splice is **dropped** and reported; canon is
  left untouched for it. This catches: stale scratch, splice errors, and out-of-body
  dependencies (a win that only held because of a pragma the splice didn't carry).

### Final human gate before commit (always)
```bash
git diff --name-only                                   # ONLY src/*.c (+ ledgers) — never *.inc
git diff <file> | grep -E '^\+.*#if 1|^\+.*\.inc"'     # MUST be empty (no asm re-activation)
python tools/decomp_work/band.py init verify <file>    # fresh re-measure of the REAL file
python tools/decomp_work/band.py json verify           # confirm new fns 100% + no regressions
```
Commit only `.c` (and ledger) changes, with the before→after byte-exact count in the message.

---

## 4. Parallelism model (why it is safe)

- Each worker has its **own** `band_<tag>.c` and `band_<tag>.o` — no shared mutable state.
- Bands from `sections` are **disjoint** function sets, so two workers never target the same fn.
- The canonical base object is read-only to workers; it changes only at `--apply`.
- `band_wins/<tag>.json` files are independent; `band_integrate` merges them (last-writer-wins,
  but bands are disjoint so there is no real contention).
- Concurrency cap is whatever the launcher imposes; the harness itself has no global lock and
  needs none.

---

## 5. Two known classes that LOOK like targets but are not (don't waste effort)

1. **Active asm-wrappers** (`#if 1` + `#include "..._fn_*.inc"`). Byte-exact in ROM; their
   sub-100% objdiff scores are pure reloc-name artifacts (`jumptable@ha`, `lbl@sda21` vs raw
   addr). `sections` auto-skips them via `active_asm_fns()`. Never flip them to real targets.
2. **Reloc-name walls on real C** (99%+, only a symbol *name* differs — anonymous jumptable
   `@nnn`, sda21 label vs raw offset). The bytes link identical; objdiff just can't see it.
   Log as Equivalent (§6) and move on; no C change reaches 100%.

---

## 6. Wall / Equivalent protocol (so effort is never repeated)

When a function has correct C but cannot reach 100% after honest attempts, record it so no
one re-grinds it:

- `WALLS.md` — prose entry: the fn, the wall class (W1 reg-alloc / W2 scheduling / W6
  reloc-name / W-peephole), the residual, and what was tried. Wall classes are defined at the
  top of WALLS.md.
- `tools/decomp_work/equivalent.txt` — one machine-readable line `fn_XXXX  # class — note; pct`.
  Read by `progress2.py` for the C-converted axis. **Only list a fn here once it is logged in
  WALLS.md and confirmed correct-but-unmatchable.** Never list a 100% fn here (it's matched,
  not "equivalent").

The STOP rule: ~3–4 honest lever attempts with no progress ⇒ it's a wall. Log it, move on.
Breadth over depth — a new lever discovered later can re-open the whole wall ledger.

---

## 7. Configuration & per-file flags

`config/GC6E01/compile_config.json` is the build truth: it maps each source file to its
CodeWarrior version and cflags. The single most important per-file knob is the optimization
flag:

- `-O4,p` (default) — most TUs.
- `-O4,s` (optimize for size) — some TUs (e.g. `trainer.c`, `pokemon.c`). `-O4,s` makes
  `stmw`/`lmw` appear at 2–3 saved regs; `-O4,p` suppresses them below ~5. A whole file can
  be *unattackable* until its flag is correct. `band.py` reads this automatically; never
  hard-code flags.

When integrating a temporary file whose stem is **not** in `compile_config.json` (e.g.
`build/band_<stem>_integrated.c`), resolve flags from the real source with:
```bash
python tools/decomp_work/band.py init <tag> <tempfile> --config-from <real/src.c>
```
`band_integrate` does this internally so `-O4,s` TUs measure correctly.

---

## 8. Extending the harness

- **Add a file to the build** → add its entry to `config/GC6E01/compile_config.json`
  (version + cflags). Then `sections`/`init` work on it immediately.
- **Add a lever** → document it in `AGENT_ONBOARDING.md` §4 with the asm before/after and the
  exact C transform. If it cracks a previously-logged wall, remove that fn from `equivalent.txt`.
- **Improve target selection** → `cmd_sections` in `band.py` is where ranking/banding lives;
  `active_asm_fns()` is the artifact filter. Add new "not-a-real-target" detectors there so the
  whole fleet stops chasing them.
- **Splicing edge cases** → `cs_splice.find_def_span` locates a fn's definition. It is
  `#if 0`-aware (`_active_lines`) so it prefers the live branch when a fn is still wrapped
  (`#if 0 asm stub #else real C #endif`). Extend there if a new wrapper shape appears.

### Self-test after any harness change
```bash
python tools/decomp_work/band.py sections src/hsd/hsd_cobj.c 2   # asm-wrapper skip count sane?
python tools/decomp_work/band.py init t src/game/pokemon.c       # -O4,s resolved?  (check cflags line)
python tools/decomp_work/band_integrate.py                       # dry-run: held/dropped tallies sane?
```

---

## 9. Invariants (never violate — these keep the headline number honest)

1. Workers edit only `scratch/band_<tag>.c`. Canon changes only via `band_integrate --apply`.
2. Never `#if 0 → #if 1`, never add a `#include "..._fn_*.inc"`. That re-embeds original asm
   (byte-matches trivially, 0% decompiled, useless for the PC port). It is fraud; reject it.
3. Never edit `*_fn_*.inc` (the target truth) or `symbols.build.txt`.
4. Every claimed win is re-measured by the parent (`band_integrate`) before commit. Trust no
   self-asserted percentage.
5. Commit only `.c` + ledger files; the final `git diff` for asm-reactivation must be empty.
