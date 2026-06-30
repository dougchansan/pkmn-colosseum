# Decomp worker economic model

Match the worker to the *kind* of remaining work. Most near-miss fixes are **mechanical**
(the classifier names the residual; the fix is a fixed recipe), so they don't need an
always-on premium model — they need a directed cheap agent or a search loop.

| Worker | Cost | Best at | Trigger |
|--------|------|---------|---------|
| **permuter** (decomp-permuter) | ~free (no LLM tokens) | RELOC / SCHEDULING / SHAPE *search*-walls on real-C near-misses — blind annealing over equivalent C | always-on; queue = `.omc/permuter_queue.json` (built by `refill_queue.py`, fed by banked near-misses) |
| **lever-crack-batch** (Sonnet + Haiku) | cheap, on-demand | **REG-COLORING** near-misses — `classify_residual` says "reg-alloc", fix = named locals + declaration-order permutation. Directed, not search | `Workflow({name:'lever-crack-batch', args:{n:12}})` |
| **reshape lanes** (Opus) | premium, reserve | LOW Ghidra drafts needing structural rework (control-flow/types/signature) — genuine reasoning | `build/.reshape_first` fleet lanes |

## Why this split

- **REG-COLORING is mechanical** — the permuter *can* crack it but blind-searches; a cheap
  agent applying the known transform (named locals + decl-order) is faster and far cheaper.
  So route it to `lever-crack-batch`, not the permuter and not an Opus lane.
- **The permuter is the cheapest worker for search-walls** — zero LLM cost, runs 24/7. Start
  it (ideally on the Linux/Mac box) and let the banking pipeline keep its queue full.
- **Opus is the scarce resource** — spend it only where reasoning is required (reshape), not
  on mechanical lever application.

## Running a batch

The workflow lives (tracked) at `tools/decomp_work/workflows/lever-crack-batch.js`.
`.claude/workflows/` is gitignored, so to invoke it by name on a fresh machine (the Mac),
install it once:

```
mkdir -p .claude/workflows && cp tools/decomp_work/workflows/lever-crack-batch.js .claude/workflows/
Workflow({ name: 'lever-crack-batch', args: { n: 12 } })
```

Or run the tracked copy directly without installing:

```
Workflow({ scriptPath: 'tools/decomp_work/workflows/lever-crack-batch.js', args: { n: 12 } })
```

Pipeline: **Scout** (`crack_batch_worklist.py` → closest unlocked near-misses) →
**Crack** (one Sonnet per fn, private band `wf_<fn>`, classify + levers, `save`→band_wins or
`bank`→permuter) → **Verify** (Haiku re-measures every claimed 100% and rejects asm/.inc
fraud — agents *do* fake matches; never trust a self-reported win).

Wins flow to `band_wins/` and are integrated by the normal gate (`band_integrate` no-regress
+ fraud guard) — the workflow never edits `src/` directly, so it's safe to run alongside the
fleet. It is also lock-aware: the scout skips files the fleet holds in `build/locks/`.

## Migration target

As the batch's confirmed hit-rate proves out, **dial the always-on Opus/Sonnet crack lanes
down** and run cheap batches on demand instead. Steady state:
- permuter grinding search-walls continuously (free),
- `lever-crack-batch` sweeps clearing REG-COLORING (cheap, on-demand),
- a small number of Opus reshape lanes on the LOW Ghidra drafts (premium, where it's earned).
