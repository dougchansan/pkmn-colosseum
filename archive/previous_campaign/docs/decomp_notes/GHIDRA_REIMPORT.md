# Ghidra re-import — findings (2026-05-17)

## What was run

`ghidra_batch_decompile.py` (reused analyzed project) → 10.6 MB fresh
decompiler C at `build/ghidra_output/raw_decompilation.c` (~381k lines,
~22k function-like defs). Then `process_ghidra_output.py` →
`ghidra_import.py` **dry-run**.

## Outcome: bulk --apply is NOT safe or valuable (do not run it)

The dry-run reported `Total imported (new): 7753 / Skipped: 650`. That is a
**broken result**, not an opportunity:

- `process_ghidra_output.py` put **7481 functions into a single
  `__unassigned__` bucket** — Ghidra has no function→TU map, so it cannot
  group them by source file. Importing these dumps thousands of stubs into
  one `__unassigned__.c` with no TU context → wrong symbol order → they
  **cannot match** and only add compile noise.
- Only ~few hundred functions map to real files, and they are concentrated
  in **dolphin/crt/trk SDK** TUs (DVD.c +36, PPCArch.c +20, OSCache.c +17,
  printf.c +9 …) that are currently empty. Those symbols are largely
  **outside the match target** (`auto_01_800055E0_text.o` is game text,
  not the SDK), so importing them risks breaking currently-clean files for
  little or no match% gain.
- Skip-detection works per-file (gs_render.c: 143 skipped, 0 imported) but
  the `__unassigned__` bulk defeats the point.

**Conclusion:** `ghidra_import.py --apply` must NOT be run until the
function→TU mapping is repaired. Forcing it in would corrupt the source
tree with ~7.5k unmappable duplicate/garbage stubs.

## What IS valuable: raw_decompilation.c as a reference

`build/ghidra_output/raw_decompilation.c` is a fresh, full Ghidra decomp
of every function. Use it as a **lookup reference**, not a bulk import:

- An agent working `fn_800XXXXX` can `grep -A60 "FUN_800xxxxx" ` (note:
  raw file uses `FUN_<lowercase-addr>`) to get fresh decompiler C for that
  one function instead of spawning a Ghidra run.
- It is gitignored (under `build/`). Regenerate any time with:
  `python tools/ghidra_batch_decompile.py` (~3 min, reuses analyzed
  project).

## RESOLVED 2026-05-17 — pipeline repaired (two structural bugs fixed)

1. **Function→TU map built**: `tools/gen_func_tu_map.py` →
   `config/GC6E01/func_tu_map.json`. Joins symbols.txt (addr/fn) with
   splits_refined.txt (188 addr-ranges → src). 8603 fns, 78.8% attributed
   (KNOWN 5788 + LIKELY 992), 1809 true GAP. `tu_for_address()` wired into
   `process_ghidra_output.py` as the fallback past sparse link_order.txt.

2. **DOL offset bug fixed**: Ghidra had exported DOL *file offsets*, not
   VAs (FUN_00000340 == VA 0x80003340). `tools/dol_addr.py`
   `normalize_addresses()` translates off→VA and renames FUN_<off> →
   fn_<VA>, applied right after parse.

Result: `__unassigned__` collapsed **7481 → ~0** functions; staged output
now correctly grouped into 73 real TU files. Import dry-run is sane:
**326 new / 461 skipped** (was 7753 / 650 garbage).

## Remaining barrier: raw stubs don't compile (inherent, not structural)

Scoped `--apply --compile-check` proof on `src/dolphin/os/PPCArch.c`
(+20 new, 0 skip): wrote, then **COMPILE FAIL** — raw Ghidra C has
unresolved types/externs. This is normal decomp reality: Ghidra output is
a *starting point* needing per-function cleanup (LLM/manual), not
drop-in compilable bulk.

**So:** bulk `ghidra_import.py --apply` still must NOT be run (it would
break every file it touches). But the staged, correctly-mapped,
correctly-addressed output under `build/ghidra_output/src/<tu>/` is now
high-quality **per-function starting material**: an agent assigned
fn_800XXXXX can pull its real Ghidra C from the correct TU file and
clean just that one function. This is the intended use; the map made it
possible.
