# Pokémon Colosseum Decompilation (GC6E01)

[![Progress](https://decomp.dev/dougchansan/pkmn-colosseum.svg)](https://decomp.dev/dougchansan/pkmn-colosseum)

A matching decompilation project for the GameCube game **Pokémon Colosseum**
(`GC6E01`, NTSC-U). The active project has been reset to the standard
[dtk-template](https://github.com/encounter/dtk-template) workflow.

## Status

![Progress over time](https://raw.githubusercontent.com/dougchansan/pkmn-colosseum/progress-chart/progress.png)

Every point on the chart is a real `objdiff` measurement. Points from
2026-07-01 onward come from the report built in CI (or, for that first stretch,
from this table's own git history); earlier points were recovered by checking
out each historical commit and running the build against it. The campaign
starts on 2026-06-27, when `config/GC6E01/splits.txt` grew past its stub and
units first became matchable — before that date this metric did not exist, and
the work of that period was measured a different way.

The build workflow republishes the chart on every push to master, to the
[`progress-chart`](https://github.com/dougchansan/pkmn-colosseum/tree/progress-chart)
branch — master is protection-gated, so CI cannot commit there, and keeping the
regenerated image off master stops its history growing a copy per push. Render
one locally with:

```bash
uv run --with matplotlib tools/plot_progress.py -o /tmp/progress.png
```

| Metric | Value |
|---|---|
| Fuzzy match | 99.70% |
| Function match | 98.56% (8,552 / 8,677 functions) |
| Code match | 92.29% (2,311,820 / 2,504,940 matched code bytes) |
| Data match | 83.30% (2,970,480 / 3,565,996 matched data bytes) |
| Source-linked | 1,982 / 2,203 scoring units (91.57% of code) |

These measurements cover the **DOL and all 25 configured REL modules**, from the
full `build/GC6E01/report.json` for the #642 integration, matching batches 2
and 3, the corrected [#645 data reconstruction](docs/REL_MODULES.md#rel-125-source),
the #646 name port, and the [#647 Count-Sheep delta](https://github.com/dougchansan/pkmn-colosseum/pull/647)
(2026-10-18).
All 26 rebuilt binaries passed their unchanged canonical hashes and direct byte
comparisons against the originals in the private build image. This is a
matching reconstruction with original fallback objects where required, not a
claim of 100% decompilation or gameplay testing. See
[`docs/COUNT_SHEEP_INTEGRATION.md`](docs/COUNT_SHEEP_INTEGRATION.md) for validation
scope and provenance.

The DOL alone is 92.12% code-matched and 98.56% data-matched. The combined data
percentage includes the 561,032-byte `common_rel` resource region supplied by
original-object fallback. Those resource bytes remain in the denominator and
do not count as reconstructed source data. On the chart, points from the PR
#638 merge onward use this DOL + REL scope; the data line reflects that scope
change and is not comparable to an older DOL-only data percentage.

**Scoring units are not translation units.** An original source region may be
split into address-owned units with different compiler flags, compiled through
one-line `#include` shims. The full report contains 8,677 distinct module/address
function identities, with no duplicate claims. Its 2,504,940-byte code denominator
contains 2,495,112 DOL bytes and 9,828 REL bytes; source-linked coverage is measured
across those same outputs. The freeze tracks their exact section topology.

After rebuilding the report, refresh the table with:

```bash
python3 tools/update_readme_progress.py
```

Use `python3 tools/update_readme_progress.py --check` to verify that the README
matches the current report.

The published denominator is frozen in
`config/GC6E01/object_map.freeze.json`. After changing `splits.txt`,
`symbols.txt`, or object declarations, rebuild the report and run:

```bash
python3 tools/check_object_map_freeze.py
```

The check locks total code bytes, data bytes, function count, and unit section
topology. Progress counters may rise without updating the freeze. Intentional
split/topology changes must update the freeze in the same change with
`python3 tools/check_object_map_freeze.py --update` and a clear reason. The
current topology still includes auto-generated units; those are visible
placeholders to retire deliberately, not hidden progress. Named extracted units
without a source path are object-map progress only; they are not counted as
decompiled source matches.

## Requirements

You must provide your own legally obtained game files. Place a supported disc
image or extracted disc at `orig/GC6E01/`; see
[`docs/getting_started.md`](docs/getting_started.md).

The build downloads pinned dtk-template tools into `build/` when local paths are
not supplied.

## Build

```bash
python configure.py
ninja
python configure.py progress
```

The build splits `sys/main.dol` and the 25 configured original REL modules,
substitutes only objects declared in `configure.py`, and verifies all 26 rebuilt
outputs against `config/GC6E01/build.sha1`. Original RELs belong under
`orig/GC6E01/files/`; complete loose modules or their FSYS archives are supported.

## Worker Worktrees

Use the helper below for parallel decompilation workers. It creates or repairs a
worktree and links `orig/GC6E01/sys` to the local original game files so
`ninja all_source build/GC6E01/report.json` can validate from the worker tree.

```bash
python3 tools/setup_worker_worktree.py /tmp/pkmn-parallel-example \
  --branch campaign/parallel-example \
  --base origin/master
```

Add `--validate` to run the standard worker checks after setup:

```bash
python3 configure.py --no-progress
ninja all_source build/GC6E01/report.json
python3 tools/update_readme_progress.py --check
git diff --check
```

## Layout

```text
config/GC6E01/          dtk config, symbols, splits, SHA-1
include/                project headers
src/                    active source/data candidates declared in configure.py
tools/                  dtk-template helper scripts
docs/                   dtk-template setup documentation
archive/previous_campaign/
                        old campaign tools, notes, PC-port experiments, and
                        unconfigured source kept for research only
```

## Contributing

Use the dtk-template flow. Add or refine splits in `config/GC6E01/splits.txt`,
declare source objects in `configure.py`, build with `ninja`, and inspect the
generated `objdiff.json` / `build/GC6E01/report.json`.

Do not commit generated `.inc` files, extracted game assets, target objects,
compiler binaries, or asm-wrapper bodies as decompilation progress.

## License

The decompiled source in this repository is released under the
[MIT license](LICENSE). The Pokémon Colosseum game, assets, and toolchain are
owned by their respective rights holders and are not distributed here.
