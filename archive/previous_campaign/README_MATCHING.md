# Matching Kit -- Pokemon Colosseum Decompilation (GC6E01)

This archive contains everything needed to start matching decompiled functions
against the original Pokemon Colosseum binary on a Windows machine.

## What is in this kit

```
colosseum_matching_kit/
  src/                          Source files (decompiled C code)
  include/                      Header files
  config/GC6E01/                Symbol table, compile config, splits
  build/GC6E01/obj/             Original object files (matching targets)
  tools/
    compile_check.py            Compile a source file and verify
    match_test.py               Test a function's match percentage
    quick_diff.py               Show per-instruction diffs
    assign_work.py              Generate/view work packets
    merge_results.py            Merge completed work back into the tree
    objdiff-cli.exe             Binary diff tool for PPC objects
    mwcc_compiler/
      GC/1.2.5n/mwcceppc.exe   Primary compiler (most game/SDK code)
      GC/1.3/mwcceppc.exe      CRT compiler (libc modules)
  docs/
    matching_guide.md           Detailed compiler tips and patterns
    matching_status.md          Current match status
    function_map.md             Address map of the binary
    setup.md                    Full setup guide (for reference)
  objdiff.json                  objdiff configuration
  README_MATCHING.md            This file
```

## Prerequisites

- **Windows 10/11** (the mwcceppc compiler is a Windows executable)
- **Python 3.10+** (for the build/match scripts)
- **Git** (for submitting results)

No additional installs are needed. The compiler and diff tools are included.

## Quick Start

### 1. Unzip the archive

Unzip `colosseum_matching_kit.zip` to a directory of your choice.
All paths below are relative to the unzipped root.

### 2. Verify the setup works

Open a terminal in the kit directory and run:

```bash
python tools/compile_check.py --all
```

This compiles all 79+ source files. Every file should report `OK`.

### 3. Test a known-matching function

```bash
python tools/match_test.py SetPauseFlag
```

This should report 100% match. If it does, your environment is working.

### 4. View available work

```bash
python tools/assign_work.py --status
python tools/assign_work.py --list
```

The `--status` flag shows an overall breakdown of matched vs unmatched
functions. The `--list` flag shows available work packets.

## How to Match a Function

### Step 1: Pick a function

Choose an unmatched function from a work packet, or browse the symbol table:

```bash
python tools/match_test.py --list --functions
python tools/match_test.py --list-sources
```

### Step 2: Find or create the source file

Check if a source file already exists for the address range. Use:

```bash
python tools/match_test.py --scan src/game/main.c
```

If no source file exists yet, create one under `src/` mirroring the original
module layout. Add the function address in a comment so the tools can find it:

```c
/* Address: 0x80012345 */
void MyFunction(void) {
    /* ... your decompiled code ... */
}
```

### Step 3: Compile and check

```bash
python tools/compile_check.py src/game/my_module.c
```

Fix any compilation errors. Remember: the compiler enforces C89 (no
declarations after statements). See `docs/matching_guide.md` for common
pitfalls.

### Step 4: Test the match

```bash
python tools/match_test.py MyFunction
```

This reports the match percentage. For a detailed per-instruction diff:

```bash
python tools/compile_check.py src/game/my_module.c --diff --symbol MyFunction
```

Or use the quick diff tool directly:

```bash
python tools/quick_diff.py build/GC6E01/obj/auto_01_800055E0_text.o build/GC6E01/base/game/my_module.o MyFunction
```

### Step 5: Iterate

Adjust your C code until the match reaches 100%. Common adjustments:

- Reorder variable declarations to match register allocation
- Use `do { } while()` instead of `while() { }` for loop patterns
- Add `volatile` for hardware register accesses
- Use exact struct layouts (field offsets matter)
- Check signedness: `(u32)` comparison generates `cmplw`, `(s32)` gives `cmpw`

### Step 6: Scan the whole file

Once individual functions match, verify the entire file:

```bash
python tools/match_test.py --scan src/game/my_module.c
```

## How to Pick Up a Work Packet

### 1. Generate or receive work packets

```bash
python tools/assign_work.py --output-dir work_packets
```

Each `WP-XXXX.json` file lists functions in a contiguous address range.

### 2. Claim a packet

Open `work_packets/manifest.json`, find an `"available"` packet, and update
its status to `"in_progress"` with your name.

### 3. Work through the functions

For each function in the packet:
1. Write the C code in the appropriate source file under `src/`
2. Compile and test until it matches 100%
3. Mark it done in your notes

### 4. Prepare your results

Create a results directory:

```
my_results/
  WP-0001/
    src/
      game/my_module.c          (new or updated source files)
      dolphin/os/SomeOS.c
    packet.json                 (copy of the original work packet)
    notes.txt                   (what you matched, issues, tips)
```

### 5. Submit results

Option A -- Git (preferred):
```bash
git checkout -b matching/WP-0001
# Copy your source files into src/
git add src/
git commit -m "Match functions from WP-0001 (address range 0x800XXXXX-0x800YYYYY)"
git push origin matching/WP-0001
# Open a pull request
```

Option B -- Send the results directory to the maintainer for merging:
```bash
python tools/merge_results.py my_results/ --dry-run    # Preview
python tools/merge_results.py my_results/              # Merge
```

## Compiler Reference

### Default flags (game code, HSD, Dolphin SDK)

```
Compiler: mwcceppc.exe (GC/1.2.5n)
Flags:    -O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off
```

### CRT / libc modules

```
Compiler: mwcceppc.exe (GC/1.3)
Flags:    -O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off
```

Per-file compiler version overrides are in `config/GC6E01/compile_config.json`.
The tools automatically use the correct compiler version for each file.

### Key rules

1. **C89 only** -- All variable declarations must precede statements in a block.
2. **No GCC extensions** -- Use `asm { }` not `__asm__()`.
3. **Section pragmas** -- Use `#pragma section` for `.init` and `.dtors` code.
4. **Forward declarations** -- Assembly functions referenced by branch must
   be forward-declared.
5. **SPR names** -- Some SPR names are not recognized; use numeric SPR IDs
   (see `docs/matching_guide.md` for the full table).

## Matching Tips

### Easy wins

- Functions named `fn_XXXXXXXX` with small sizes (< 0x40 bytes) are often
  simple getters/setters or wrappers. These are the fastest to match.
- Game code (Genius Sonority engine, "GS" prefix) matches reliably. The
  developers used consistent patterns.
- HSD modules (sysdolphin) follow Melee patterns -- reference existing Melee
  decomp projects for struct layouts.

### Known hard areas

- **CRT/libc** (printf, stdio, mem): Hand-optimized by MetroWerks. Large
  functions may need to remain as assembly stubs.
- **Dolphin SDK**: SDA base register addressing (r2/r13) is the main
  difficulty. Variables must be placed in the correct section (`.sdata`,
  `.sbss`, `.sdata2`, `.sbss2`).
- **Inline assembly**: PPC SPR encoding differences can cause 50% matches
  even when the code is structurally correct.

### Register allocation patterns

MWCC assigns callee-saved registers from r31 downward. Function arguments
arrive in r3-r10 (GPR) and f1-f8 (FPR). Reordering variable declarations
in your C code changes register allocation.

### Loop patterns

The compiler uses `bdnz` for counted loops and tends to place the condition
test at the bottom of the loop body. `do { } while()` compiles more
directly than `while() { }`.

## File Layout

Source files under `src/` mirror the original module structure:

```
src/
  init/         CRT / startup code
  crt/          MetroWerks C runtime
  dolphin/      Nintendo Dolphin SDK
    db/         Debug
    dvd/        DVD filesystem
    exi/        External interface (memory cards, etc.)
    os/         Operating system
    si/         Serial interface
    vi/         Video interface
  game/         Genius Sonority game code
  hsd/          HAL sysdolphin (rendering engine)
  trk/          MetroTRK debugger
  runtime/      C++ runtime support
  string/       String utilities
  libc/         Additional libc
  pcport/       PC port stubs (not in original)
  ppc/          PPC-specific code
```

## Useful Commands Reference

```bash
# Compile a single file
python tools/compile_check.py src/game/main.c

# Compile all files
python tools/compile_check.py --all

# Test one function by name
python tools/match_test.py GameInit

# Test one function by address
python tools/match_test.py 0x800057B0

# Test all functions in a file
python tools/match_test.py --scan src/game/main.c

# Interactive diff (TUI)
python tools/compile_check.py src/game/main.c --diff --symbol main

# List all function symbols
python tools/match_test.py --list --functions

# Show source file coverage
python tools/match_test.py --list-sources

# View work packet status
python tools/assign_work.py --status

# Generate work packets
python tools/assign_work.py

# Merge results
python tools/merge_results.py results_dir/ --dry-run
python tools/merge_results.py results_dir/
```

## Current Status

As of the last update, 772+ functions (9.0% of 8,603) are verified
byte-identical, with 23 modules at 100% match. See `docs/matching_status.md`
for the full breakdown.

The game code (Genius Sonority engine) matches very reliably. The main
remaining work is in Dolphin SDK modules, CRT, and uncovered game subsystems.
