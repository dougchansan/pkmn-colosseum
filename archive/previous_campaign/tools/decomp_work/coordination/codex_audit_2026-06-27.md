# Codex Audit Run - 2026-06-27

Owner: Codex
Started: 2026-06-27 11:24 HST

## Guardrails

- No asm-wrapper wins. A win must be active real C and must pass local/parent verification.
- No `.inc` wins and no committed `.inc` files. `*_fn_*.inc` / `.inc` files are ROM-derived scaffolding and must stay ignored/local only.
- No inline asm, `__asm`, `#include .inc/.s`, or asm-wrapper bodies are accepted as decompilation.
- No register-dump/pointer-arithmetic C as a finish line. Raw pointer arithmetic is temporary debt only when byte-match forces it before struct recovery.
- Do not touch `src/game/gs_field_world.c`; it is reserved for the live codex session.
- Do not globally rename symbols in source without a rename audit and verification plan.
- Treat comments and subsystem notes as semantic hints only. Target asm, declarations, compile, and objdiff remain authoritative.

## Task List

- [ ] Audit function-count sources and explain why reported totals differ.
- [ ] Count active asm wrappers, stubs, real-C functions, `fn_`-named functions, and file-scoped naming coverage.
- [ ] Estimate pointer-arithmetic/register-dump density and identify the worst files.
- [ ] Re-review alleged fleet wins and permuter 20 wins; only integrate active real-C wins that re-verify.
- [ ] Reject any alleged win that adds `.inc`, reactivates an asm wrapper, or leaves only wrapper/pointer-dump C.
- [ ] Check permuter status, timeout settings, batching strategy, and win banking.
- [ ] Retask GLM/Codex fleet toward asm-to-C bucket completion with no-wrapper policy.
- [ ] Review SeedCoder prompt/benchmark/training flow and queue safe improvements.
- [ ] Propose roadmap toward readable, named, byte-matching C.

## Live Notes

- Work started after deploying doc-hint prompts and no-wrapper rejection to local Seed paths and the 3090 v3 server.
- 3090 v3 health at start of this run: `seedcoder8b-cw-v3`, `bf16`.
