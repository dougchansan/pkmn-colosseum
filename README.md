# Pokémon Colosseum decompilation (GC6E01 / GPXE01)

[![Progress](https://decomp.dev/dougchansan/pkmn-colosseum.svg)](https://decomp.dev/dougchansan/pkmn-colosseum)

A work-in-progress matching decompilation of the GameCube game **Pokémon
Colosseum** (NTSC-U: `GC6E01`). The goal is byte-identical reproduction of the
original `main.dol` from C source, using the same Metrowerks CodeWarrior
compiler that Genius Sonority used in 2003.

## Status

| Metric | Value |
|---|---|
| Function match | ~63.4% (4,308 / 6,795 functions) |
| Code match | ~39.6% (542,148 / 1,368,604 matched code bytes) |
| Data match | ~0.0% (0 / 1,276,195 matched data bytes) |

Last updated 2026-06-28 from trust-gated per-function scans on top of the
2026-06-25 `python tools/gen_decomp_report.py -o report.json` baseline,
against the local ROM-extracted target/base objects, after a full `compile_check
--all` rebuild and removing four duplicate measurement units that had double-counted
real TUs (gs_field_world / gs_title were counted twice; gs_render_w2 / pokemon_boss2
were 94-95% copies of gs_render / pokemon). De-duplication lowered the headline
~2.4pts (functions) / ~5pts (code) but the number is now honest and locally
reproducible. The numbers are regenerated locally — public CI
cannot hold the ROM-derived target objects, so `.github/workflows/progress.yml`
only gates compilation until the private build container is published. (Note:
the denominators grew over time as more translation units were unblocked, so
percentages are not comparable to figures from before 2026-06-04.)

## You will need

The repository does **not** ship the game or the compiler. You must legally
obtain:

1. **An original Pokémon Colosseum disc image** (`GC6E01.iso` or `.gcm`).
   Place under `orig/GC6E01/`.
2. **Metrowerks CodeWarrior for GameCube** — `configure.py` downloads the
   publicly mirrored compiler bundle from `files.decomp.dev` to `build/compilers`
   automatically (the binaries are copyrighted and not committed). `decomp-toolkit`
   is vendored at `tools/dtk.exe`.

Without both, only `compile_check.py` works — the match% step needs the
extracted target objects.

## Quick start (Docker)

```bash
docker build -t pkmn-colosseum-build .
docker run --rm -v "$PWD:/work" pkmn-colosseum-build configure.py
docker run --rm -v "$PWD:/work" pkmn-colosseum-build python3 tools/progress.py
```

See [`docker/README.md`](docker/README.md) for details.

## Quick start (Windows, native)

```powershell
# 1. Extract your orig disc to orig/GC6E01/ (dtk; not included)
python configure.py            # auto-downloads dtk + the Metrowerks compilers to build/ (gitignored)
ninja                          # split -> build -> link -> verify SHA-1 (reproduces main.dol byte-exact)
python configure.py progress   # match% report
```

The build follows the canonical [dtk-template](https://github.com/encounter/dtk-template)
pipeline: decomp-toolkit splits your DOL into objects, matching C objects are
substituted where declared, and the result is checked against `config/GC6E01/build.sha1`.

## Repository layout

```
src/            C source — the game's own decompiled C (match targets)
pcport/         Ship-of-Harkinian-style native PC port (separate build; not the DOL)
include/        Headers
config/GC6E01/  config.yml, symbols.txt, splits, build.sha1
build/          Output incl. regenerated asm + downloaded compilers (gitignored)
asm/            Per-function disassembly — local only, gitignored (regenerate from your ROM)
tools/          Build, diff, and match utilities
  progress.py            Per-file match% report
  decompctx.py           Flatten includes → ctx.c for decomp.me
  weak_order_diff.py     Detect symbol-order regressions
  gen_struct_catalog.py  Refresh docs/struct_sizes.md
  compile_check.py       Compile one .c
docs/
  matching_guide.md      Compiler tips and patterns
  struct_sizes.md        Typedef catalog
  tu_split.md            Future direction
```

## Contributing

See **[CONTRIBUTING.md](CONTRIBUTING.md)** for full setup, the per-function
workflow, the objdiff GUI, and the rules that keep the match honest. In short,
to push a function from `X%` to `100%`:

1. Pick a file from `python tools/progress.py` (worst-match files printed first).
2. Run `python tools/decompctx.py src/path/to/file.c` to get a `ctx.c` you
   can paste into [decomp.me](https://decomp.me).
3. Iterate locally with `python tools/compile_check.py src/path/to/file.c`.
4. Verify with `./tools/objdiff-cli diff -1 <target.o> -2 <yours.o>`.
5. Open a PR.

Strict rules:

- **Never edit `*_fn_*.inc` files** — those are the ROM-truth bytes the
  diff measures against.
- **Never flip `#if 0` → `#if 1`** on asm-wrapper scaffolding to forge a
  match; the wrappers exist as placeholders, not as the goal state.
- Only `.c` / `.h` / config changes are accepted in match-improvement PRs.

## Inspiration

Tooling and methodology lifted from established GCN/Wii decomps:

- [`zeldaret/tp`](https://github.com/zeldaret/tp) — Twilight Princess
- [`zeldaret/tww`](https://github.com/zeldaret/tww) — Wind Waker
- [`projectPiki/pikmin2`](https://github.com/projectPiki/pikmin2) — Pikmin 2
- [`PrimeDecomp/prime`](https://github.com/PrimeDecomp/prime) — Metroid Prime
- [`doldecomp/melee`](https://github.com/doldecomp/melee) — Super Smash Bros. Melee

## License

The decompiled C source in this repository is released under the [MIT
license](LICENSE) (scope details in [NOTICE](NOTICE)). The Pokémon Colosseum
game, its assets, and the CodeWarrior toolchain are property of their
respective copyright holders — this project does not redistribute any of that
material.

## Disclaimer

This is a clean-room decompilation, produced by disassembling the user's
own legally-obtained copy of the game and rewriting the disassembly back
into C until the compiler reproduces the original bytes. No copyrighted
game data, assets, or compiler binaries are committed to this repository.
