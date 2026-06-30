# TU split — tool built, but data says DON'T split yet (2026-05-17)

`tools/tu_split.py` is implemented and correct: it partitions a mega-file
by `config/GC6E01/func_tu_map.json`, replicates the preamble, regenerates
config, and **auto-reverts unless whole-project matched-count is
preserved/improved**. It refuses to run on a dirty file.

## Empirical finding (dry-run)

Running it on the mega-files shows **0 functions to extract**:

    src/game/battle/battle_main.c   : 21/21 stay (0 to extract)
    src/game/gs_field_world.c       : 735/735 stay (0 to extract)

Reason: `func_tu_map.json` (built from `splits_refined.txt`) attributes
each mega-file's entire address range to **itself**, not to finer
sub-TUs. Per the current ground-truth attribution these files *are*
single translation units.

## Consequence

A TU split is only beneficial when a file is genuinely several original
TUs merged into one. The current attribution does not identify such
boundaries inside the mega-files, so:

- There is **nothing to beneficially split today**.
- Forcing an arbitrary split (e.g. by size) without real TU boundaries
  would scramble `.data`/`.bss`/symbol order and **reduce** match% — the
  tool's preservation guard would correctly auto-revert it.
- The earlier "stmw-emission needs TU split" note (battle_main) was
  speculative: battle_main is already its own TU, so the stmw blocker
  there is a *different* problem, not a merge artifact.

## Real prerequisite (separate project)

Finer-grained TU boundary attribution: sub-divide the large/GAP ranges in
`splits_refined.txt` using `.file`/`.comment` STAB records or pad-region
heuristics, then regenerate `func_tu_map.json`. Only then does
`tu_split.py --apply` have correct boundaries to act on. Until that
analysis exists, the tool stays dry-run-only by intent.

The tool + guard are committed so the capability is ready the moment the
boundary data improves; no further code is needed, only better splits
attribution.
