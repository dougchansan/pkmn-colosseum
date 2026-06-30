# Ghidra Headless Batch Decompilation Pipeline

Automated pipeline for generating C stubs from the Pokemon Colosseum DOL
using Ghidra's headless mode and decompiler.

## Overview

The pipeline runs in four stages:

```
start.dol ──► Ghidra headless ──► raw C ──► cleanup ──► staged files ──► src/
              (analyze + decompile)  │        (types,     (grouped by     (import
                                     │        params,     link_order.txt)  + compile
                                     │        registers)                   check)
                                     ▼
                            raw_decompilation.c
```

## Prerequisites

- **Ghidra 12.0.4** installed at `C:\Users\douglaswhittingham\tools\ghidra_12.0.4_PUBLIC`
  (or set `GHIDRA_HOME` environment variable)
- **Java 21+** on PATH (required by Ghidra)
- **GameCube Loader extension** installed in Ghidra (the `.zip` is at
  `Extensions/Ghidra/GameCubeLoader-1.3.0.zip` -- install via Ghidra GUI
  or extract into `Ghidra/Extensions/`)
- **Python 3.10+** for the processing and import scripts
- **mwcceppc.exe** (MetroWerks CodeWarrior) for compilation checks

## Quick Start (One Command)

```bash
# Full pipeline: decompile + clean + import (dry-run)
python tools/ghidra_batch_decompile.py && \
python tools/process_ghidra_output.py && \
python tools/ghidra_import.py

# To actually write stubs into src/:
python tools/ghidra_import.py --apply --compile-check
```

Windows `.bat` alternative (no Python wrapper needed for Ghidra step):

```batch
tools\ghidra_batch_decompile.bat
python tools\process_ghidra_output.py
python tools\ghidra_import.py --apply --compile-check
```

## Stage 1: Ghidra Headless Analysis

**Script:** `tools/ghidra_batch_decompile.py`
**Wrapper:** `tools/ghidra_batch_decompile.bat`

Runs Ghidra's `analyzeHeadless` to:
1. Import `orig/GC6E01/start.dol` with the `PowerPC:BE:32:Gekko_Broadway` processor
2. Run full auto-analysis (function detection, data references, call graphs)
3. Execute `ExportDecompilation.java` as a post-script to decompile all functions

```bash
# First run (imports DOL + full analysis, ~10-30 minutes):
python tools/ghidra_batch_decompile.py

# Subsequent runs (reuse existing project, ~5 minutes):
python tools/ghidra_batch_decompile.py

# Force re-import (delete project and start fresh):
python tools/ghidra_batch_decompile.py --reimport

# Preview the command without running it:
python tools/ghidra_batch_decompile.py --dry-run
```

**Output:** `build/ghidra_output/raw_decompilation.c`

### GhidraScript Details

**Script:** `tools/ghidra_scripts/ExportDecompilation.java`

The Java GhidraScript iterates every function in the program and calls
Ghidra's built-in decompiler on each one. Output format:

```c
/* ===== FUN_800055E0  addr=0x800055E0  size=0x1D0 ===== */
void FUN_800055e0(void) {
    // Ghidra's decompiled C ...
}
```

Each function header includes the address and body size for downstream
processing. Functions that fail to decompile get a `DECOMPILE_FAILED`
marker instead.

## Stage 2: Post-Processing

**Script:** `tools/process_ghidra_output.py`

Transforms the raw Ghidra output into project-compatible C:

### Type Replacements

| Ghidra Type  | Project Type |
|-------------|-------------|
| `undefined4` | `u32`       |
| `undefined2` | `u16`       |
| `undefined1` / `undefined` | `u8` |
| `undefined8` | `u64`       |
| `uint`       | `u32`       |
| `ushort`     | `u16`       |
| `byte`       | `u8`        |
| `ulonglong`  | `u64`       |
| `longlong`   | `s64`       |
| `ulong`      | `u32`       |
| `bool`       | `BOOL`      |

### Parameter Renaming (PowerPC Calling Convention)

| Ghidra Name | Register |
|------------|----------|
| `param_1`  | `r3`     |
| `param_2`  | `r4`     |
| `param_3`  | `r5`     |
| `param_4`  | `r6`     |
| `param_5`  | `r7`     |
| `param_6`  | `r8`     |
| `param_7`  | `r9`     |
| `param_8`  | `r10`    |
| `fParam_1` | `f1`     |
| `fParam_2` | `f2`     |
| ... | ... |

### Additional Cleanup

- `in_rN` register references simplified to `rN`
- Redundant double casts `(u32)(u32)x` collapsed
- Functions grouped by address into source files per `config/GC6E01/link_order.txt`
- Already-decompiled functions (found in `src/`) are skipped by default

```bash
python tools/process_ghidra_output.py

# Include all functions (even those already in src/):
python tools/process_ghidra_output.py --no-skip-existing
```

**Output:** `build/ghidra_output/src/game/main.c`, `build/ghidra_output/src/game/gs_task.c`, etc.

## Stage 3: Import into Project

**Script:** `tools/ghidra_import.py`

Merges cleaned Ghidra stubs into the existing source tree:

```bash
# Dry run (see what would be imported):
python tools/ghidra_import.py

# Apply changes to src/:
python tools/ghidra_import.py --apply

# Apply + compile each modified file:
python tools/ghidra_import.py --apply --compile-check

# Process a single file only:
python tools/ghidra_import.py --apply --file src/game/main.c
```

The import script:
1. Parses staged files from `build/ghidra_output/`
2. Checks each function against the existing `src/` file
3. Appends new function stubs with a clear marker comment
4. Optionally compiles with `mwcceppc` to verify buildability

Imported functions are marked with:
```c
/* ================================================================
 * AUTO-IMPORTED FROM GHIDRA DECOMPILATION
 * These stubs need manual review and cleanup.
 * ================================================================ */
```

## File Layout

```
tools/
  ghidra_batch_decompile.py     # Stage 1: Python wrapper for analyzeHeadless
  ghidra_batch_decompile.bat    # Stage 1: Windows .bat wrapper
  ghidra_scripts/
    ExportDecompilation.java    # GhidraScript: decompile all functions
  process_ghidra_output.py      # Stage 2: cleanup + grouping
  ghidra_import.py              # Stage 3: import into src/

build/
  ghidra_project/               # Ghidra project database (auto-created)
    PokemonColosseum.gpr
    PokemonColosseum.rep/
  ghidra_output/
    raw_decompilation.c         # Stage 1 output (raw)
    src/                        # Stage 2 output (cleaned, grouped)
      game/
        main.c
        gs_task.c
        ...
      dolphin/
        ...
```

## Troubleshooting

### Ghidra fails to start

Make sure Java 21+ is installed and `java` is on your PATH:
```bash
java -version
```

### GameCube Loader not found

The DOL may still load with Ghidra's generic binary importer, but the
GameCube Loader provides better results. Install it:
1. Open Ghidra GUI
2. File > Install Extensions
3. Select `GameCubeLoader-1.3.0.zip` from `Extensions/Ghidra/`
4. Restart Ghidra

Or for headless use, extract the extension zip into:
```
<GHIDRA_HOME>/Ghidra/Extensions/GameCubeLoader/
```

### analyzeHeadless hangs or runs out of memory

Edit `support/analyzeHeadless.bat` and increase the Java heap:
```
-Xmx4g
```

Or set the `JAVA_OPTS` environment variable:
```batch
set JAVA_OPTS=-Xmx4g
```

### Functions not being detected

If Ghidra misses functions, you can improve detection by:
1. Running with `--reimport` to get a fresh analysis
2. Adding a Ghidra symbol map (`.sym` file) before running
3. Manually marking functions in the Ghidra GUI and re-running the export

### Compilation failures after import

Ghidra output often needs manual adjustment:
- Missing struct/enum definitions (add to `include/`)
- Incorrect pointer types (Ghidra may use `int *` where `void *` is needed)
- Missing forward declarations
- Register variable annotations for matching

## Tips for Matching

1. **Start with small, leaf functions** -- these typically decompile cleanly
2. **Cross-reference with the disassembly** (`tools/dump_asm.py`) to verify
   control flow matches
3. **Use `tools/compile_check.py --diff`** after cleanup to compare against
   the original object code
4. **Ghidra's output is a starting point** -- expect to rewrite 30-70% of
   each function for a match
