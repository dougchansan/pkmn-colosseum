# Decomp Knowledge Graph (`kg/`)

A single-file SQLite "knowledge graph" that collapses the project's scattered decomp
knowledge into one queryable store, plus a best-of-N representation harness on top of it.

**The thesis (the "sudoku" mental model):** these games were written by ~10-20 people,
so wins propagate. A lever that cracks one function very likely cracks its siblings; a
struct discovered in one TU constrains every function that touches it. The graph turns
"I vaguely remember peephole-off worked in gs_title" into a query that *finds the next
function it will work on.*

## Files

| File | What it is |
|---|---|
| `kg.py` | The graph: `init` / `backfill` / `q` (named queries) / `sql` / `record-*`. |
| `kg_feed_measure.py` | Heavy feeder — populates the full ~8k-fn universe from `progress2.py`'s objdiff measure. |
| `kg_externals.py` | Harvests technique commits from other decomp projects (tww/xd/dolsdk) into `externals`. |
| `kg_to_obsidian.py` | Projects the graph into the Obsidian vault as a regenerable encyclopedia page. |
| `kg_callgraph.py` | Mines caller/callee edges, calltags, and name-evidence rows for rename reasoning. |
| `bestof.py` | KG-driven best-of-N harness: pick targets (`--json`), assemble diverse work packets, record wins. |
| `kg_diff_repair.py` | Diff-aware minimal-edit loop: current best C + exact objdiff -> model proposal, accept only trusted improvements. |
| `kg.db` | The database (gitignored-candidate; regenerable from sources). |
| `bundles/` | Generated per-function work packets (markdown). |

Companion files outside this dir:
- `tools/ghidra/scripts/ExportPcode.java` + `tools/ghidra/run_pcode.sh` — Ghidra headless P-Code export.
- `tools/decomp_work/wf_kg_bestof.js` — the fleet workflow that drives the whole sudoku loop.

## Quick start

```bash
python tools/decomp_work/kg/kg.py init           # create schema + seed levers/walls
python tools/decomp_work/kg/kg.py backfill        # ingest scratches/equivalent/WALLS.md/git
python tools/decomp_work/kg/kg_feed_measure.py    # populate full fn universe (needs base build)

python tools/decomp_work/kg/kg.py q list          # see the named "sudoku" queries
python tools/decomp_work/kg/kg_callgraph.py build # mine calls + relationship tags
python tools/decomp_work/kg/kg_callgraph.py context fn_80017990 --markdown
python tools/decomp_work/kg/bestof.py targets 25 --real-c-only  # active-C adapt targets
python tools/decomp_work/kg/bestof.py bundle fn_80022834   # work packet for one fn
```

Re-run `backfill` + `kg_feed_measure.py` after a decomp session to refresh. Both are idempotent.

## What's in the graph

- **functions** — addr, tu, byte_pct, status, is_equivalent, wall_class (full ~8k universe)
- **levers** — 28 reusable CW transforms seeded from `CW_QUIRKS.md` + the `feedback_*` memory ledger
- **walls** — the W1..W6 / W-* taxonomy from `WALLS.md` (c_controllable flag)
- **cracked_by** — the money edge: which lever cracked which fn (git-mined + agent-recorded)
- **commits** — the technique timeline (822 technique commits scraped from git)
- **externals** — *(empty; see roadmap)* new methods harvested from other decomp projects

Additional relationship tables:

- **function_calls** - caller/callee edges from active source and Ghidra p-code bundles
- **function_tags** - low/medium/high calltags such as `HSD`, `GStexture`, or TU clusters
- **name_evidence** - candidate names and provenance, including symbolmap and call-neighborhood evidence

## Relationship-guided naming

Run `kg_callgraph.py build` after a naming/decomp pass. It refreshes deterministic
relationship rows without changing source:

- `src-c` edges are high-confidence direct C call tokens.
- `ghidra-pcode` edges are medium-confidence direct CALL records from generated bundles.
- `function_tags` accumulate relationship context without forcing a rename.
- `name_evidence` records candidates from current symbols, symbolmap proposals, and calltags.

The useful loop is:

1. Recover or rename one function conservatively.
2. Run `kg_callgraph.py build`.
3. Inspect `kg.py q call-neighborhood <fn>` or `kg_callgraph.py context <fn> --markdown`.
4. Feed that relationship context to an AI/model or use it manually to rank names.
5. Only promote names when independent evidence moves them to high confidence.

## Named queries (`kg.py q <name>`)

`nearmiss-by-tu`, `siblings <fn>`, `top-levers`, `wall-load`, `lever-targets <slug>`,
`tu-progress`, `recent-techniques`, `call-neighborhood <fn>`, `calltags`,
`name-evidence <fn>`. `kg.py sql "<SELECT...>"` for anything else (read-only).

## Best-of-N (the benchmark recipe)

The "best-of-four" finding: giving a model several *independent views* of the same
function nearly doubles full-match rate vs any single view. `bestof.py bundle` assembles
the views we can self-host:

| rep | source | status |
|---|---|---|
| `asm` | `show_target_asm.py` (target `.inc`) | ✅ wired |
| `ghidra_c` | `ghidra_out/<fn>_ghidra.c` (`ExportDecomp.java`) | ✅ wired (74 exported so far) |
| `ghidra_pc` | `pcode_out/<fn>_pcode.txt` (`ExportPcode.java`) | ✅ wired — 252 near-miss fns exported; regen with `tools/ghidra/run_pcode.sh` |
| `bn_hlil` | Binary Ninja HLIL | ❌ deferred — free BN can't script HLIL export (needs Commercial). Clean seam left in `REPS`. |

To export more p-code: `python kg.py q nearmiss-by-tu` to pick a TU, regenerate the address
list, then `bash tools/ghidra/run_pcode.sh <list.txt>` (uses the existing `text1.bin` Ghidra project).

Each bundle also injects the **TU-proven levers** (sudoku propagation) and **near-miss
siblings** so the worker starts with the file's known idioms, not a blank slate.

The bundle is the *input* to the existing consumers — fleet models / decomp.me /
`permute_match.py`. Record outcomes back so the graph learns:

```bash
python tools/decomp_work/kg/bestof.py record fn_80022834 100.0 --rep ghidra_pc --lever peephole-off
```

For HTTP models that regress when asked to rewrite a full function, use the minimal-edit
loop instead. It persists the best accepted partial, promotes cross-model improvements to
`repair_pool/<fn>/shared_best.c`, and rejects non-improving proposals:

```bash
python tools/decomp_work/kg/kg_diff_repair.py fn_80017990 --model deepseek/deepseek-v4-pro --rounds 3
python tools/decomp_work/kg/kg_diff_repair.py fn_80022834 --models deepseek/deepseek-v4-pro,kimi/kimi-k2.7-code --rounds 2
python tools/decomp_work/kg/kg_diff_repair.py fn_80017990 --start source --rounds 1  # branch away from shared best
```

## Fleet worker protocol (the shared blackboard)

The KG + `bestof.py` are the coordination surface for parallel workers. The loop, driven
end-to-end by `tools/decomp_work/wf_kg_bestof.js`:

1. **Scout** — `python bestof.py targets <N> --json --real-c-only` → highest-ROI
   active-C near-miss targets, grouped per-TU (siblings tackled together; earliest wins
   inform later ones). Use plain `targets` only when intentionally doing asm-wrapper
   conversion work; adapt-bundle HTTP models should not start from active wrappers.
2. **Bundle** — `python bestof.py bundle <fn>` → a `bundles/<fn>.md` work packet with the
   diverse views (asm / ghidra_c / ghidra_pc), the TU-proven levers, and sibling near-misses.
3. **Crack** — worker attempts in a private scratch (existing band/cs_band/event_scratch harness).
   Try the bundle's TU-proven levers first; if one view's structure stalls, re-derive from another.
4. **Record** — `python bestof.py record <fn> <pct> --rep <view> [--lever <slug>]` writes the
   outcome back so the graph learns which view/lever wins where.

Run the whole thing: `Workflow({name:'kg-bestof-crack-fleet', args:{count:12, perTU:3}})`
(or your fleet's workflow runner). The workflow is authored but **not yet executed end-to-end** —
its first real run should be watched to tune the per-worker variant budget and prompts.

## Refresh cadence

After a decomp session: `kg.py backfill && kg_feed_measure.py` (re-measure), then
`kg_externals.py --fetch` (other projects) and `kg_to_obsidian.py` (vault page). All idempotent.

## Roadmap (remaining)

1. **First fleet run** — execute `wf_kg_bestof.js` on a small `count`, watch, tune prompts/budget.
2. **`uses_struct` edges** — parse C for struct refs so "fns sharing struct X" becomes a query
   (deeper sudoku propagation than per-TU grouping).
3. **Lever-mapping recall** — the externals keyword→lever map is conservative (9/146 mapped);
   improve it as cross-project patterns are confirmed.
4. **BN HLIL** — drop a 4th `REPS` entry if a Commercial Binary Ninja license appears.
