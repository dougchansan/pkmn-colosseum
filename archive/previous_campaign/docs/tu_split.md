# Translation Unit (TU) Splitting — Future Direction

## What

Our `src/game/*.c` files are large catch-alls — `gs_field_world.c` alone is
~26,000 lines and ~700 functions. The original Pokemon Colosseum binary was
built from many smaller TUs (translation units), each producing one `.o`.
Combining them into single mega-files has been an expedient way to get
something compiling, but it makes the codebase harder to:

- Parallelize across agents (any agent edit blocks all others on the same
  giant file)
- Diagnose compile breaks (one bad function in a 26k-line file takes the
  whole file down)
- Match the original binary's symbol order (we get lucky with our giant
  files; matching multi-TU layouts is more disciplined)

The [zeldaret/tp](https://github.com/zeldaret/tp) project keeps every
original TU as a separate `.c` file under `src/`, which is the canonical
approach.

## Why we haven't done it yet

The split is **not a quick refactor**. The linker must be told the new
file boundaries, and:

- The DOL's symbol layout depends on the original TU boundaries
- Splitting mid-file changes the order of `.data` / `.bss` / `.rodata`
  entries
- A wrong split breaks `objdiff-cli` match% because the function
  *addresses* shift
- Identifying original TU boundaries requires careful analysis of:
  - `.file` symbol records (if preserved in the unstripped DOL)
  - `.comment` section CW version stamps (per-TU)
  - Pad-region heuristics between TUs

## What needs to happen

1. **Identify original TU boundaries** in `build/GC6E01/obj/*.o`
   - Inspect `.file` symbol records via `objdiff-cli` JSON dump
   - Cross-reference `.comment` section signatures
   - Catalog "natural" boundaries (data alignment, file pragmas)

2. **Generate split manifest** (`config/GC6E01/tu_split.txt` or similar)
   - One line per TU: function-range, file-name, compiler-version
   - Driven by `configure.py`

3. **Migrate functions** from `gs_field_world.c` → many small files
   - `gs_field_world__init.c`, `gs_field_world__draw.c`, etc.
   - Or, if original names are recoverable, use those

4. **Update `compile_config.json`** and `splits.txt`/`config.yml`
   - Per-TU compiler overrides where needed
   - Linker script TU ordering

5. **Verify byte-match** across all split outputs vs the merged original

## Estimated effort

- gs_field_world.c split: 1-2 days
- All large game/*.c files: 1-2 weeks
- Requires careful diffing and rollback discipline

## Interim mitigations

Until we split:
- Bulk pattern fixes (regex over the giant file) work well — see this
  session's compound_setter and GX-FIFO emitter fixes for examples.
- Worktrees per-agent isolate edits, but cherry-picks can conflict when
  agents touch nearby functions.
- The `progress.py` per-file breakdown still works at the current
  granularity.

## Inspiration

`zeldaret/tp` keeps every original TU as a separate `.cpp` under `src/f_op/`,
`src/m_Do/`, etc. Their `configure.py` reads a manifest that maps source
files to versioned compilers and link order. We could lift their pattern
once we're ready.

---

Adoption #5 of TP-inspired patterns. Tracked but deferred.
