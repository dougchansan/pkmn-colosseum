# Decomp pipeline — orchestrator resume (2026-06-18)

You are the **orchestrator Opus** (pane 8). Read this, then drive the staged
decomp pipeline. You are the **trust-gate**: never trust an agent's claimed %,
re-measure everything before it touches canon.

## Fleet (8 workers + proxy, codex-heavy — we have ~8% Codex vs ~60% Claude weekly)
Built by `tools/llm-proxy/launch-decomp.ps1`; ids in `tmux_control/panes.env`.

| Pane | Role | Drive it with |
|---|---|---|
| 0 | proxy (GLM router; only GLM is proxied) | — |
| 1-4 | Codex ×4 (gpt-5.5) | `control.sh send-codex[2/3/4]-safe "…"` |
| 5 | GLM (glm-5.2[1m], proxied) | `control.sh send-glm-safe "…"` |
| 6 | Sonnet (direct) | `control.sh send-sonnet-safe "…"` |
| 7 | Opus worker (direct) | `control.sh send-worker-safe "…"` |
| 8 | Opus orchestrator (you) | — |

`ctl="bash tools/decomp_work/tmux_control/control.sh"`. Capture: `*-capture`. Idle: `*-idle`.

## The pipeline (stage-aware, FILE-LEVEL ownership — kills lock contention)
`pipeline.py` state = `coordination/pipeline.json`. **One file per lane; no two
lanes share a file.** Stages: **A** asm→C draft (Opus/Codex) · **B** near-miss
finish via levers (Sonnet/Codex) · **C** permuter (walled fns).

Per lane each round:
```
python tools/decomp_work/pipeline.py status
python tools/decomp_work/pipeline.py packet <LANE> > coordination/pl_<LANE>.md   # tight, no-explore packet
$ctl send-<lane>-safe "Read coordination/pl_<LANE>.md and run it. Report REALC/SAVED/WALL per fn."
# when a lane finishes, harvest (below), then:
python tools/decomp_work/pipeline.py release <LANE> [pct.json]   # advances the file's stage by new pct
```
Already seeded + claimed (5 files). **CODEX-3 and GLM are idle** — seed 2 more files:
`decomp.sh band sections <file> 1` to get fns, add to `coordination/seed.json`, re-seed, claim.
Give **GLM a small/light file** and prompt it gently — it must not rate-limit out and die.

## Trust-gate (MANDATORY before canon)
- Byte-exact: `python band_integrate.py <tag> --apply` — re-splices + re-measures; only ≥100% lands.
  The gate REJECTS inline-asm/`#include .inc` fraud and DROPS sub-100 (it caught GLM's 4 fake
  100%s and a 99.46% in-context drop this session).
- **Salvage (NEW):** `band_integrate.py <tag> --equivalent --min-pct 90 --apply` — high-% faithful
  real C that isn't byte-exact gets registered in `equivalent.txt` + `WALLS.md` (advances the
  C-converted axis; asm stays active for byte-match; DOL build unaffected). "Both tracked separately."
- Structural drafts often fail to compile in canon because the fn references data symbols
  (`lbl_*`) declared only in the agent's scratch — lift those `extern`/typedef decls into canon
  first (the gate re-verifies, so a wrong type won't hold).

## Usage / cost (read agent_limits.json + the dashboards)
- **Claude Max traffic CANNOT be proxied** (it ignores ANTHROPIC_BASE_URL → goes to claude.ai).
  So Opus/Sonnet run DIRECT; their 5h/weekly usage is the **manual** `agent_limits.json` (update
  reset times when you observe lockout). Only **GLM** is live via the proxy.
- Proxy writes `tools/llm-proxy/context_usage.json` (TRUE per-session context — the in-TUI meter
  is wrong under [1m]) and `usage_limits.json` (GLM rate-limit headers).
- **Policy: lean on Codex** (huge headroom) — prefer Codex lanes, keep the single Opus for the
  hardest structural fns, Sonnet for mechanical near-miss finishing. Throttle GLM.

## Monitors (keep both up)
- Renaming dashboard: `:8792` (PID may change) — reads `agent_limits.json` for the lockout panel.
- Pipeline monitor: `python tools/decomp_work/pipeline_monitor.py --port 8794` — pipeline + true context.

## This session's real progress
- Committed `c0d861aa`: gs_npc_interact `fn_8000FE38` + `fn_80010588` → 100% real C (2 verified wins).
- The near-miss byte-exact queue is saturated on reg-alloc walls; structural asm→C (Stage A) +
  Equivalent salvage is where the honest decomp % moves now. That's what this pipeline is for.
