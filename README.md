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
| Fuzzy match | 72.31% |
| Function match | 76.40% (6,573 / 8,603 functions) |
| Code match | 39.78% (992,560 / 2,495,108 matched code bytes) |
| Data match | 97.29% (2,136,513 / 2,196,100 matched data bytes) |
| Linked into DOL | 1,030 / 2,238 units (26.04% of code) |

These numbers come from the canonical dtk/objdiff report generated at
`build/GC6E01/report.json`. Old campaign metrics and helper reports are archived
under `archive/previous_campaign/` and are not used for the published progress.
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

The build splits `sys/main.dol`, substitutes only objects declared in
`configure.py`, links `build/GC6E01/main.dol`, and verifies it against
`config/GC6E01/build.sha1`.

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
