# Per-file decomp notes

Every time an agent works a `src/**/*.c` file, it **must read and update** the
corresponding note at `docs/decomp_notes/<filename>.md`. The point: don't
re-explore dead ends future agents already proved are dead.

## Format

```markdown
# Decomp notes: <relative path>

## Status snapshot
N/M @ 100% (YYYY-MM-DD)

## Blocked near-misses
Functions that have been worked but cannot reach 100% with currently known
techniques. Each entry names the **identified compiler-level blocker**, not
just "tried X". A new technique unlocks retry.

- **fn_XXXXXXXX** @ 99.X% — <blocker name>
  - Symptom: <what the diff shows>
  - Tried: <techniques used>
  - Next leads: <what hasn't been tried, if any>
  - Last attempt: YYYY-MM-DD by <agent>

## Recently landed
Functions matched at 100% in the last session. Move to "matched" after stable.

- **fn_XXXXXXXX** — <one-line technique summary> (YYYY-MM-DD)

## Matched (100%)
Just a list. Bulk of the file.

## Untouched near-misses
Functions never explored. Pull from `python tools/near_miss_report.py`.
```

## Convention for agents

Before starting:

1. `cat docs/decomp_notes/<file>.md` — read existing notes.
2. **Skip** any "Blocked near-misses" unless you have a *new technique* not
   yet tried for that blocker.
3. Prefer "Untouched near-misses" or "Recently landed" near-misses for
   easier follow-up wins.

After finishing:

1. Append your session block to the file (date, agent ID, outcome).
2. Move newly-100% fns to "Matched".
3. Move newly-blocked fns to "Blocked near-misses" with **identified blocker
   + everything tried**.
4. Commit the note in the same commit as the source changes.

## Known blocker vocabulary

Use these tags so future agents can scan for matching cases:

- `anonymous-sda21` — CW emits `@N@sda21` vs target's `lbl_X@sda21`
- `peephole-bne-b` — `beq @far` vs `bne @near; b @far`
- `reg-alloc-permutation` — same callee-save count but rN/rN+1 swapped
- `stmw-emission` — CW won't emit `stmw/lmw` for N regs in this TU
- `instr-scheduling-order` — high-latency loads in different order
- `frame-size-quirk` — stack frame off by 0x10/0x20 with no source fix
- `cvt-fp2unsigned` — CW emits runtime call CW that target inlines
- `__cvt_fp2unsigned-inverse` — and vice versa
- `tracking-tu-boundary` — TU split needed; current file is two-merged-into-one
