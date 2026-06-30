# Decompilation Roadmap Tracker

Last updated: 2026-06-16

This document tracks the current decompilation campaign around readable, portable C,
multi-agent routing, diff-aware repair, and knowledge graph assisted naming. It is
intended to be updated as commits land, agents report back, or priorities change.

## North Star

Advance the project toward real, readable, portable C while preserving traceability to
the original DOL. Byte matching matters, but asm wrappers are not decompiled C and must
not be treated as the final win when a real C body exists.

## Score Axes

Track progress on three separate axes. Do not collapse them into one number.

| Axis | Meaning | Primary command | Notes |
|---|---|---|---|
| C-converted | How many source functions have real C bodies instead of active asm wrappers | `python tools/decomp_work/progress2.py` | Honest PC-port progress axis. |
| Objdiff function match | The decomp.dev style byte-match score | `python tools/progress.py` | Counts asm wrappers as matched, so it can overstate decomp progress. |
| Byte-exact real C | Real C functions that byte-match | `python tools/decomp_work/progress2.py --measure` | Strictest honest axis. |

## Operating Rules

- Keep real, readable C active when it honestly advances the C-converted axis.
- Do not rewrap a function solely to recover wrapper-inflated objdiff score.
- Treat raw pointer arithmetic as unfinished unless it is hardware/register access or a
  documented unavoidable compiler-shape lever.
- Prefer typed structs, named fields, and symbolmap evidence over byte offsets.
- Parent remeasure every agent claim before committing.
- Never edit `*_fn_*.inc` files.
- Do not touch `src/game/gs_field_world.c` while another session owns it.
- Commit only explicit pathspecs because unrelated staged `.inc` files are present.

## Active Roadmap

| Status | Workstream | Goal | Current next action | Evidence / command |
|---|---|---|---|---|
| In progress | Wrapper to readable C | Convert tractable asm wrappers to portable C even when residual match is not perfect | Target small or medium helpers in compileable TUs first | `python tools/decomp_work/active_asm_targets.py --band small_under_120` |
| In progress | Diff-aware repair | Improve active C near-matches by feeding exact residual diffs into repair/permutation loops | Start with committed near-matches listed below | `python tools/match_scan_file.py <file> <fn>` |
| In progress | Typed struct recovery | Replace raw offsets with partial structs and field names backed by usage evidence | Promote only high-confidence fields and keep uncertain names as `field_XX` | repo-wide `rg`, callsite review, Ghidra/decompiler evidence |
| In progress | Knowledge graph guidance | Give agents best current candidate, typed context, call relationships, and known residual diff | Extend dashboard/KG views to surface class/file/function relationships and confidence | `tools/decomp_work/kg/*` |
| In progress | Agent orchestration | Route tasks by function size, TU health, and model availability | Use cheap models for leaf wrappers; GLM/Opus for larger or stuck repairs after reset | tmux sessions plus parent verification |
| Blocked / repair needed | TU validation blockers | Make more files safe for parent-side per-function validation | Fix or isolate compile failures in `ui_core.c` before accepting UI candidates | `python tools/match_scan_file.py src/game/ui/ui_core.c <fn>` |
| Ongoing | Naming confidence | Use more named files/functions to raise low/medium calltag confidence | Add rename audits before broad symbol changes | audit table from `AGENTS.md` |

## Current Repair Candidates

| Function | File | Current state | Residual | Next repair idea |
|---|---|---|---|---|
| `fn_801A4A54` | `src/hsd/hsd_lobj.c` | Real C active; portable switch mapping GX light index to bitmask | `jumptable_8036CA64` relocation label plus `r0`/`r3` dispatch register shape | Investigate local jump-table naming/placement or compiler switch-shape levers. Do not rewrap. |
| `fn_80162EB8` | `src/game/people/people_field.c` | Real C active; ARAM alignment/cache-store/upload helper | Nonvolatile register allocation around `r26`/`r27`/`r28` and arithmetic scheduling | Try focused register-pressure/source-order permutations while keeping readable C. |
| `fn_80061B74` | `src/game/ui/ui_core.c` | Candidate semantics identified, not committed | TU currently fails parent validation for unrelated earlier compile issues | Fix `ui_core.c` validation first; then reapply typed message/global-mode candidate. |
| `fn_800A48DC` | `src/dolphin/dvd/DVDLow.c` | Probe rejected and reverted | Hardware-register source shape did not improve enough | Leave for focused SDK/hardware-register pass. |

## Agent Routing

| Lane | Best targets | Avoid |
|---|---|---|
| Cheap fast models | Small leaf wrappers, SDK-style register writers, functions under roughly 80 instructions | Giant real-C functions, files with known compile blockers |
| GLM 5.2 | Medium helpers with typed context, clear call relationships, and adapt bundles | Prompts that force raw byte-offset pointer arithmetic as final source |
| Opus / high-reasoning agents | Large wrappers, stubborn near-matches, compiler-shape diagnosis | Spending time on cheap leaf wrappers when quota is scarce |
| Parent Codex | Verification, commits, final judgement, routing, cleanup of unverified probes | Trusting child-agent 100% claims without local remeasure |

## Validation Gates

For a source change:

1. Run the narrowest per-function measure:

   ```powershell
   python tools/match_scan_file.py src/path/file.c fn_XXXXXXXX --report report.json
   ```

2. Inspect residual when not 100%:

   ```powershell
   python tools/decomp_work/fndiff.py build/GC6E01/base/path/file.o fn_XXXXXXXX --all
   ```

3. Run the structural gate before commit:

   ```powershell
   python tools/verify_gate.py --range HEAD~1..HEAD
   ```

4. Commit only explicit pathspecs:

   ```powershell
   git commit --only src/path/file.c
   ```

## Progress Log

| Date | Commit / artifact | Area | Result | Follow-up |
|---|---|---|---|---|
| 2026-06-16 | `a1ce3a0b` | `src/hsd/hsd_lobj.c` | Kept `fn_801A4A54` as portable real C; measured 85.7% with known jump-table/register residual | Repair jump-table label/dispatch register without rewrapping. |
| 2026-06-16 | `f91f183b` | `src/game/people/people_field.c` | Converted `fn_80162EB8` to readable ARAM upload C; measured 63.9% / 86.9% fndiff | Repair nonvolatile register allocation. |
| 2026-06-16 | Reverted probe | `src/game/ui/ui_core.c` | Identified likely typed semantics for `fn_80061B74`, but rejected because TU cannot be parent-validated | Fix compile blockers before retry. |
| 2026-06-16 | Reverted probe | `src/dolphin/dvd/DVDLow.c` | Tested `fn_800A48DC` C body and pointer-local register shape; rejected weak result | Revisit in SDK/hardware-register batch. |

## Update Checklist

When updating this roadmap:

- [ ] Add new committed wins to the progress log with commit hash and command evidence.
- [ ] Move functions out of repair candidates when they reach byte-perfect C or are logged as a real wall.
- [ ] Add validation blockers as soon as a TU prevents parent-side measurement.
- [ ] Update agent routing if model availability or benchmark evidence changes.
- [ ] Keep score axes separate in any status summary.
