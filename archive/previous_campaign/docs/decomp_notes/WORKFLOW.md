# Decomp attack workflow (token-efficient pipeline)

The mandatory order of operations for any file. Each stage removes work
from the next so LLM tokens are only spent where nothing cheaper works.

## Stage 0 — recon (zero token)

```
python tools/near_miss_report.py            # which files have the most near-misses
```

## Stage 1 — classify (zero token, seconds)

```
python tools/diffclass.py src/path/file.c --band 85 99.99 --json cls.json
```

Partitions every near-miss into:

- **AUTO**    — a deterministic rewrite/pragma will fix it
- **HINT**    — LLM needed, but with a one-line hypothesis
- **BLOCKED** — compiler wall (stmw-emission, anonymous-sda21);
  skip entirely, record in this file's note, never dispatch an agent

## Stage 2 — automatch sweep (zero token, ~1 min/fn)

```
python tools/automatch.py src/path/file.c --band 90 99.99 --apply --report r.md
```

Tries the curated pragma catalog on every near-miss. Lands the
pragma-solvable ones (peephole/scheduling/opt-level/fp_contract) with no
tokens. Cross-reference its "blocked" list against diffclass: anything
diffclass calls AUTO that automatch couldn't pragma-fix is a
**rewrite-class** win (signed-compare, redundant-extend) — those go to
Stage 3, not straight to an LLM.

## Stage 3 — targeted LLM agents (tokens, but hypothesis-driven)

Only for diffclass HINT + AUTO-but-not-pragma. The agent prompt MUST
include the diffclass category and detail for each function, e.g.:

> fn_8022E6F0 — `signed-compare`: flip `*(u32*)`→`*(s32*)` (or add
> `(s32)`/`(u32)`) at the compare site. Do NOT explore other avenues
> first; test this hypothesis.

Agents are hypothesis-testers, not explorers. They read
`docs/decomp_notes/<file>.md` first and append findings after.

## Project-wide triage snapshot (2026-05-14, 5 files, band 85-99.99)

| File | near | AUTO | HINT | BLOCKED |
|---|---|---|---|---|
| gs_field_world.c | 78 | 32 | 43 | 3 |
| gs_render.c | 56 | 33 | 22 | 1 |
| scene_init.c | 61 | 34 | 24 | 3 |
| colosseum_event.c | 43 | 16 | 20 | 7 |
| gs_material.c | 31 | 10 | 20 | 1 |
| **total** | **269** | **125** | **129** | **15** |

~125 deterministically-attackable functions in 5 files alone. The
dominant AUTO class is `signed-compare`.

## 2026-05-17 — CRITICAL: manual-LLM agents need whole-file non-regression check

w2/scene_init incident: a HINT agent made its 10 target functions 100%
(each independently `verify_commit --measure`-verified) but the commit
NET REGRESSED the file 107→101 — it broke >=16 OTHER functions in
scene_init while fixing 10. Reverted (commits bd3140a, 7861c12).

Root cause: automatch/autorewrite have a whole-file matched-count guard
with auto-revert. **Manual LLM-agent edits do NOT** — `verify_commit
--measure` only checks the *claimed* functions, never the rest of the
file. A localized edit (pragma scope, decl reorder) can shift codegen
for neighbours.

MANDATORY going forward:
- Agent prompts MUST require: after edits, measure the WHOLE file's
  matched-count and confirm it is >= the pre-edit baseline; revert if not.
- Harden verify_commit.py: for each changed .c, recompile and compare
  whole-file matched-count vs the parent commit; REJECT on any net drop.
  (This would have auto-caught w2.) — highest-priority gate improvement.
- Parent MUST rebuild cherry-picked files before quoting any aggregate
  (progress.py reads stale .o; only per-file rebuild gives truth).

## 2026-05-17 — verify_commit hardened + cp1252 bug fixed (VALIDATED)

verify_commit.py now has check_file_regression: isolated-recompiles each
changed src/.c parent-vs-head and REJECTS on whole-file matched-count
drop. Found+fixed a latent cp1252 decode crash (text=True on source
bytes like 0x81) that had made every file un-checkable.

Validated: `verify_commit --range ae2db37..7861c12` → `[REGRESSED]
scene_init.c 107->101` → REJECTED.

MANDATORY cherry-pick protocol for manual-LLM (HINT) agents:
  python tools/verify_commit.py --range <master-before-agent>..<agentHEAD>
Run over the FULL agent range, NOT per-commit (cumulative regressions
span an agent's commit series, as w2 did). Only cherry-pick if it
prints "clean". Deterministic loop (automatch/autorewrite) already has
its own whole-file guard and is exempt.
