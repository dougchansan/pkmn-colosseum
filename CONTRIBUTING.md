# Contributing

This branch is intentionally reset around the standard dtk-template workflow.
Keep changes small and make the dtk build/report the source of truth.

## Setup

1. Put your legally obtained game files under `orig/GC6E01/`.
2. Run `python configure.py`.
3. Run `ninja`.
4. Run `python configure.py progress` or inspect `build/GC6E01/report.json`.

## Matching Work

- Add source only through `configure.py` object entries and
  `config/GC6E01/splits.txt`.
- Leave nonmatching source as `CodeCandidate` / `DataCandidate` until the
  generated dtk report proves it should be linked.
- Prefer `python tools/decompctx.py -I include src/path/to/file.c` for
  decomp.me context.
- Use objdiff against the generated target/source objects before claiming a
  match.

## Rules

- Do not commit `.inc` files or ROM-derived snippets.
- Do not use asm wrappers, inline asm, or included assembly as decompilation
  progress.
- Do not edit generated target objects, game assets, compiler binaries, or other
  copyrighted extracted data into the repository.
- Do not edit symbols/splits/linker config just to improve a metric; config
  changes need a build reason.
- Keep old campaign research inside `archive/previous_campaign/`.
