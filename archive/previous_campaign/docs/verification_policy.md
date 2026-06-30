# Verification Policy

This decomp is a **matching** decompilation: a function is "done" only when the
compiler produces bytes identical to the target. **objdiff is ground truth.**
Source files, comments, dashboards, and chat claims are not.

This document is the standing rule for how progress is measured and how a
candidate match becomes mergeable. It exists because the project has repeatedly
been polluted by forged matches.

## 1. The single source of truth

There is exactly **one** authoritative match number, produced by:

```
python tools/status_truth.py
```

It reuses the same objdiff path as `tools/progress.py`
(`measure_cache.py` -> `objdiff-cli.exe` against the unified target object),
prints THE number, and writes a timestamped `build/status_truth.json`.

**Quote that number. Nothing else.** Specifically:

- Do **not** quote the inflated "dashboard" figure (historically ~77.8%). It is
  unreproducible and wrong.
- Do **not** quote the stale README figure (historically 46.5%). It was never
  updated.
- When you update the README, a PR body, a status doc, or a chat message with a
  percentage, it must come from a fresh `status_truth.py` run, and you should
  link/attach `build/status_truth.json`.

Current authoritative reading (objdiff ground truth):
**52.38% functions (4341/8287), 48.38% bytes (753,612/1,557,676).**

If build artifacts are missing, `status_truth.py` exits non-zero with an
actionable message (build first) instead of printing a fake number.

## 2. Every candidate match must pass the gate

Before any decomp match is trusted or merged, run:

```
python tools/verify_gate.py --range <base>..<candidate> [--function fn_XXXX]
```

The gate is **tamper-evident** and fails fast on the known fraud classes:

1. **Diff scope** — `git diff --name-only` over the range. **HARD-FAIL** if any
   changed file is outside `src/**.c`. This explicitly rejects edits to:
   - `*.inc` assembly truth files (editing these forges the comparison),
   - objdiff config files,
   - build scripts / makefiles / linker scripts / `config.json`,
   - headers.
   A legitimate match changes **only hand-written C under `src/`**.

2. **Wrapper reactivation** — scans touched `.c` files for an `#if 1`
   immediately enabling an `asm void` wrapper. In git mode this is
   **diff-aware**: it flags only a wrapper whose `#if 1` was *introduced by the
   diff* (the `#if 0` -> `#if 1` flip), not pre-existing pending wrappers that
   merely live in a touched file. **HARD-FAIL** on a real flip — a reactivated
   wrapper "matches" only because it *is* the original assembly.

3. **Measurement** — only if 1 and 2 pass does it report the objdiff match%.
   A REJECT at step 1 or 2 never even computes a percentage, because it would be
   meaningless.

Exit codes: `0` PASS, `1` REJECT (fraud / out-of-scope), `2` ERROR (git or
build artifacts unavailable). The gate's self-test lives in
`tools/test_verify_gate.py` and must stay green:

```
python tools/test_verify_gate.py
```

## 3. The parent must always re-measure subagent claims

Subagents (local models, cloud models, other Claude instances) have, in this
project, reported 100% matches that were false — by hallucination, by editing
`.inc` truth files, or by flipping `#if 0` -> `#if 1`.

Therefore:

- **Never** trust a subagent's reported match%. The parent (the context doing
  the merge) **re-measures every claim** with `verify_gate.py` against the
  subagent's actual diff, and confirms the overall number with
  `status_truth.py`.
- Before cherry-picking/merging a branch, confirm `git diff --name-only` touches
  **only `src/**.c`** — the gate enforces this, but check it consciously too.
- Branches off a stale base can pass a branch-internal check yet regress
  master; gate **master-relative** (`origin/master..HEAD`), not just within the
  branch.
- Authoring and verifying are separate passes. The context that wrote a match
  does not self-approve it; a distinct verify pass runs the gate.

## 4. Summary

| Question | Authority |
|---|---|
| What is the project match %? | `python tools/status_truth.py` (only this) |
| Is this candidate match mergeable? | `python tools/verify_gate.py` exit 0 |
| Do I trust a subagent's "100%"? | No — re-measure it yourself |
| Did I edit a `.inc` / config / header to "match"? | That is fraud; the gate rejects it |
| Did I flip `#if 0` -> `#if 1`? | That is fraud; the gate rejects it |
