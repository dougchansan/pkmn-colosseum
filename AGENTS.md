# AGENTS.md — Pokémon Colosseum Decompilation Guardrails

This repository is being reset around the standard dtk-template workflow. The
active build/report path is `configure.py` -> `ninja` ->
`build/GC6E01/report.json`.

## Active Project

- Target: Pokémon Colosseum `GC6E01`.
- Source candidates: `src/`.
- Headers: `include/`.
- Canonical config: `config/GC6E01/config.yml`, `symbols.txt`, `splits.txt`,
  `build.sha1`.
- Archived old campaign material: `archive/previous_campaign/`.

## Hard Rules

- Do not add, edit, stage, or commit `.inc` files.
- Do not count asm wrappers, inline asm, or included assembly as decompilation
  progress.
- Do not commit extracted game assets, target objects, compiler binaries, or
  generated build products.
- Do not move archived campaign material back into the active tree unless it is
  reintroduced through the dtk-template pipeline.
- Do not rename symbols or change splits without a concrete build/report reason.
- Keep source changes scoped to objects declared in `configure.py`.

## Validation

Use the smallest check that proves the change:

1. `python configure.py --no-progress`
2. `ninja all_source build/GC6E01/report.json`
3. `ninja`
4. `python configure.py progress`

If a command cannot run in the environment, report the exact command and why.

## Naming

Rename conservatively and preserve address traceability. A nontrivial rename
needs evidence from callsites, strings, known SDK patterns, data tables, or
confirmed assembly/decompiler analysis.
