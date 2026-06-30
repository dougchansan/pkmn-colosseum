# Pokémon Colosseum Decompilation (GC6E01)

[![Progress](https://decomp.dev/dougchansan/pkmn-colosseum.svg)](https://decomp.dev/dougchansan/pkmn-colosseum)

A matching decompilation project for the GameCube game **Pokémon Colosseum**
(`GC6E01`, NTSC-U). The active project has been reset to the standard
[dtk-template](https://github.com/encounter/dtk-template) workflow.

## Status

| Metric | Value |
|---|---|
| Function match | ~0.1% (9 / 8,603 functions) |
| Code match | ~0.0% (616 / 2,495,108 matched code bytes) |
| Data match | ~0.6% (12,364 / 2,196,256 matched data bytes) |

These numbers come from the canonical dtk/objdiff report generated at
`build/GC6E01/report.json`. Old campaign metrics and helper reports are archived
under `archive/previous_campaign/` and are not used for the published progress.

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
