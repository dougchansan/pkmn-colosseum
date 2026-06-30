# Opus Orchestrator — decomp cockpit (proxy + codex + glm + 2×opus)

Paste this whole file into the **orchestrator Opus** pane (or tell it: "read
`tools/decomp_work/OPUS_COCKPIT_ORCHESTRATION.md` and run it"). You are the
**orchestrator and the trust-gate**, not a worker. You assign disjoint decomp
targets to three workers — **codex**, **glm**, and a second **worker** Opus —
then independently re-measure every claimed win before it touches canon. Never
self-assert a match %, and never edit canonical source yourself except through
`band_integrate.py --apply`.

You drive the workers through the control lib (`tools/decomp_work/tmux_control/
control.sh`). The launcher already wrote `panes.env`, so the semantic names
resolve correctly — you do **not** need to detect panes by hand.

| Worker | Send a prompt | Check idle | Capture output |
|---|---|---|---|
| codex  | `control.sh send-codex-safe  "…"` | `control.sh codex-idle`  | `control.sh codex-capture` |
| glm    | `control.sh send-glm-safe    "…"` | `control.sh glm-idle`    | `control.sh glm-capture` |
| worker | `control.sh send-worker-safe "…"` | `control.sh worker-idle` | `control.sh worker-capture` |

Role split: **glm** and **codex** grind the cheap ranked queue in bulk; the
**worker** Opus is your specialist — hand it the gnarly near-misses (low %,
wrong control-flow shape, or a specific wall you want a fresh lever tried on).

---

## 0. Orient (run once, before dispatching anything)

```bash
cd /c/Users/douglaswhittingham/pkmn-colosseum
ctl="bash tools/decomp_work/tmux_control/control.sh"
echo "MY PANE = $TMUX_PANE"                 # = CLAUDE_PANE; never send keystrokes here
$ctl check                                  # session + registry self-test
cat tools/decomp_work/tmux_control/panes.env
$ctl codex-idle ; $ctl glm-idle ; $ctl worker-idle   # all three resolve + report
```

If any name resolves to the wrong pane (e.g. you relaunched the cockpit and the
ids drifted), re-run the launcher — it rewrites `panes.env`. Do **not** run
`control.sh refresh`; its index map (0=claude,1=codex,…) does not match this
cockpit. The registry the launcher writes is the source of truth.

---

## 1. Pick work (cheapest-first, cluster by file)

Authoritative queue: `tools/decomp_work/_winnable_queue.md` (92 fns, ranked).
**Cross-check the SHARED MIND first** — the KG knows which levers crack which
function shapes, so it picks higher-yield clusters than the raw queue:

```bash
kg="python tools/decomp_work/kg/kg.py"
$kg q lever-targets     # near-miss fns ranked by an already-applicable lever
$kg q top-levers        # which levers yield the most — bias clusters toward these
$kg q siblings <fn>     # a lever that cracked one sibling likely cracks the rest
```

Prefer a **cluster in one file** so the band harness amortizes — e.g. the
`gs_field_world.c` sudoku siblings `fn_8011A280 / fn_8011A570 / fn_8011A9EC /
fn_8011AB50 / fn_8011AFCC`. Confirm a target is live and not a logged wall:

```bash
tools/decomp_work/decomp.sh where  fn_8011A280     # which file owns it
tools/decomp_work/decomp.sh walls  fn_8011A280     # already a logged wall? skip if so
tools/decomp_work/decomp.sh next   src/game/gs_field_world.c   # live non-100% targets
```

Band the chosen file into disjoint function sets (active asm-wrappers are
auto-skipped — never target those):

```bash
tools/decomp_work/decomp.sh band sections src/game/gs_field_world.c 4
```

---

## 2. Dispatch — one function-cluster per worker, disjoint tags

Give each worker **non-overlapping** functions and a **distinct private tag**
(`glm1`, `cdx1`, `wrk1`). Because each worker edits only its own
`scratch/band_<tag>.c`, parallel work is safe (HARNESS.md §4). Don't paste
multi-line prompts into a TUI — write a **packet file** and tell the worker to
read it. Hand the hardest function in the band to the **worker** Opus (`wrk1`).

Write a packet (repeat per worker, adjusting tag/fns):

```bash
cat > tools/decomp_work/coordination/packet_glm1.md <<'PKT'
# Work packet — tag glm1
File:  src/game/gs_field_world.c
Fns:   fn_8011A280 fn_8011A570 fn_8011A9EC     # YOURS ONLY — do not touch others
Tag:   glm1

Protocol (band harness — read tools/decomp_work/HARNESS.md §3 if unsure):
  tools/decomp_work/decomp.sh band init  glm1 src/game/gs_field_world.c
  # edit ONLY tools/decomp_work/scratch/band_glm1.c — repair the real C of ONE fn at a time
  tools/decomp_work/decomp.sh band check glm1 <fn>     # re-measure (objdiff vs target)
  tools/decomp_work/decomp.sh band diff  glm1 <fn>     # aligned target-vs-ours asm
  tools/decomp_work/decomp.sh band save  glm1 <fn>     # ONLY when a fresh check shows 100.00

Levers: tools/decomp_work/CW_QUIRKS.md and tools/decomp_work/CLAUDE.md
        (switch-with-dead-cmpwi, s32+late-(s16), volatile-cast CSE break,
         nested-call-keeps-r3, decl-order reg-alloc, -O4,s vs -O4,p).

KNOWLEDGE GRAPH (the fleet's SHARED MIND — read BEFORE, append AFTER):
  kg="python tools/decomp_work/kg/kg.py"
  # BEFORE grinding each fn, ask what other agents already learned:
  $kg q siblings <fn>        # functions like this one + the levers that cracked them
  $kg q top-levers           # highest-yield levers across the project — try these first
  $kg q lever-targets        # is THIS fn already tagged with a likely-applicable lever?
  $kg q recent-techniques    # newest levers other agents just discovered
  # AFTER a SAVED 100% win, append so the next agent benefits (this is mandatory):
  $kg record-crack <fn> <lever-slug> --commit <sha> --delta "-> 100%"
  # If you found a NEW lever not in `q top-levers` / CW_QUIRKS.md, register it:
  $kg record-lever <new-slug> --title "..." --desc "what it does + when it bites/is inert" \
       [--opt-gated] --source "agent:<your-tag>"
  A lever that cracks one fn very likely cracks its siblings — always check siblings first.

Hard rules:
  - Edit ONLY scratch/band_glm1.c. NEVER edit canon src/*.c, *_fn_*.inc, or symbols.
  - NEVER flip #if 0 -> #if 1 and NEVER add #include "..._fn_*.inc" (that is fraud).
  - `save` refuses anything < 100% — do not try to force it.
  - STOP after ~3-4 honest lever attempts on a fn with no progress; mark it a wall.
  - ALWAYS consult the KG before grinding and append every win/new lever after.

Deliverable (report back in your pane, one line per fn):
  SAVED <fn> 100.00 (kg: recorded <lever-slug>)  |  WALL <fn> <best%> <residual>
PKT
```

**Context hygiene — check BEFORE every dispatch.** Each batch is an independent
task, so a worker carrying a full prior round degrades the next one. Capture the
pane; if the status line shows a high context fraction (e.g. `100% context used`)
or it's about to auto-compact, send `/clear` first (independent task → `/clear`
beats `/compact`; a 100%-context agent will otherwise auto-compact a hard task
into a summarized blur). Re-derivable knowledge (file structure, asm) is cheap to
reload on a clean slate. Slash sends need `MSYS_NO_PATHCONV=1`:

```bash
$ctl worker-capture 6 | grep -iE 'context|compact'         # check fraction
MSYS_NO_PATHCONV=1 tmux send-keys -t "$WORKER_PANE" -l "/clear"; tmux send-keys -t "$WORKER_PANE" Enter
```

Write one packet per worker (`packet_glm1.md`, `packet_cdx1.md`, `packet_wrk1.md`
— same shape, different tag/fns), then send each a one-line "read & execute"
prompt via the control lib. `-safe` captures the pane first so you can confirm
the worker was idle:

```bash
$ctl send-glm-safe    "Read tools/decomp_work/coordination/packet_glm1.md and execute it. Report SAVED/WALL per fn."
$ctl send-codex-safe  "Read tools/decomp_work/coordination/packet_cdx1.md and execute it. Report SAVED/WALL per fn."
$ctl send-worker-safe "Read tools/decomp_work/coordination/packet_wrk1.md and execute it. Report SAVED/WALL per fn."
```

One dispatch per worker; verify it landed (`*-capture`); never rapid-fire prompts.

---

## 3. Poll for completion (don't babysit)

```bash
$ctl codex-idle  ;  $ctl glm-idle  ;  $ctl worker-idle      # exit 0 = idle/at-prompt
$ctl glm-capture 30 | tail -15                              # look for SAVED/WALL lines
```

Check periodically; do other orchestration in between. When a worker goes idle,
gate its tag (§4) and hand it the next cluster.

---

## 4. Trust-gate every claim (you re-measure — workers can lie)

A worker saying "100%" means nothing until `band_integrate.py` re-splices its
saved body into a fresh copy of canon, recompiles with the file's real flags,
and re-measures. Anything that doesn't still read 100% is dropped.

```bash
WINPY="C:/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe"
"$WINPY" tools/decomp_work/band_integrate.py glm1 cdx1 wrk1          # DRY-RUN: held/dropped tally
"$WINPY" tools/decomp_work/band_integrate.py glm1 cdx1 wrk1 --apply  # write verified wins into canon
```

Final human gate before commit (HARNESS.md §3 — always):

```bash
git diff --name-only                                    # MUST be only src/*.c (+ ledgers), never *.inc
git diff src/game/gs_field_world.c | grep -E '^\+.*#if 1|^\+.*\.inc"'   # MUST be empty
tools/decomp_work/decomp.sh band init verify src/game/gs_field_world.c
tools/decomp_work/decomp.sh band json verify           # confirm new 100%s + zero regressions
```

Commit only `.c` + ledger changes, message = before→after byte-exact count.
For each WALL the workers reported, append to `WALLS.md` (class + residual) and
one line to `tools/decomp_work/equivalent.txt` so no one re-grinds it.

**Feed the SHARED MIND** (do this for every verified win — you hold the
authoritative commit SHA, so record it even if the agent already did; the
`cracked_by` UNIQUE constraint dedups):

```bash
kg="python tools/decomp_work/kg/kg.py"
$kg record-crack <fn> <lever-slug> --commit <sha> --delta "-> 100%"
# if the agent surfaced a NEW lever not yet in `q top-levers`:
$kg record-lever <slug> --title "..." --desc "what it does + when it bites/is inert" \
     [--opt-gated] --source "agent:<tag>"
```

(Walls still go to `WALLS.md` + `equivalent.txt` as above — the KG tracks
levers/cracks, those ledgers track walls.)

---

## 5. Loop

Re-band the next cluster, dispatch the now-idle worker, gate, commit. Keep all
three workers (glm, codex, worker) busy on disjoint tags. Stop when
`_winnable_queue.md` is exhausted for the file, then pick the next file.

## Invariants (never violate)
1. Workers edit only `scratch/band_<tag>.c`; canon changes only via `band_integrate --apply`.
2. Never `#if 0 → #if 1`, never add `#include "..._fn_*.inc"`, never edit `*_fn_*.inc` or `symbols.build.txt`.
3. Re-measure every claimed win yourself; trust no self-reported %.
4. Never send keystrokes to `$TMUX_PANE` (your own pane).
5. Commit only `.c` + ledger files; the asm-reactivation `git diff` grep must be empty.
6. **A byte-match win must be DECOMPILED C, never assembly.** Never write the answer as
   an `asm`-storage-class function (`asm void fn(){ stwu...; lwz...; }`) or an inline
   `asm{}` / `__asm` block, and never keep the `#include ".inc"` wrapper. Hand-transcribing
   the disassembly into inline asm (even with relocations fixed) games the byte-match metric
   while producing zero C — it is FRAUD and is auto-rejected by band_integrate.py's
   inline-asm guard (REJECT-ASM). On 2026-06-17, 30 of 39 "wins" were this fraud and had to
   be reverted. If you cannot express a function in real C, mark it WALL — do NOT inline-asm it.
