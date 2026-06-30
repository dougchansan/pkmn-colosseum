# Verification Target

## Chosen target

`fn_80143A94` in `src/game/people/people_data.c`

## Why this target was chosen

- It comes from the newest commit in the repository, `2685605 people_data: decompile 20 functions (100% match each)`, so it verifies the most recent claimed decompilation progress.
- It is a single 0x28-byte function, which is the smallest realistic symbol-level verification target in the current workflow.
- Its owning translation unit already has a stable source-range mapping (`0x80140588 - 0x80144574`), so the existing match harness can find and compile it without adding new infrastructure.
- Larger targets were less realistic:
  - The PC-port CMake path is not yet executable. `CMakeLists.txt` still has the main executable commented out and refers to a missing `src/pcport/pcport_main.c`.
  - Whole-file or whole-game validation would be broader than needed for proving recent progress.

## Required dependencies or assumptions

- `orig/GC6E01/start.dol` must exist.
- `build/GC6E01/obj/auto_01_800055E0_text.o` must exist as the comparison target.
- `tools/mwcc_compiler/GC/1.3/mwcceppc.exe` must exist.
- `tools/objdiff-cli.exe` must exist.
- `config/GC6E01/symbols.txt` and `objdiff.json` must be present.
- This verifies matching progress through the decompilation toolchain, not by running the game or emulator.

## Exact steps attempted

1. Reviewed the last 20 commits with:

   ```powershell
   git log -20 --oneline --decorate
   git log -20 --stat --oneline --decorate
   ```

2. Inspected the available execution paths:

   ```powershell
   Get-Content CMakeLists.txt
   Get-Content docs\setup.md
   Get-Content README_MATCHING.md
   python tools\compile_check.py --help
   python tools\match_test.py --help
   ```

3. Confirmed the decompilation toolchain inputs already exist locally:

   ```powershell
   Get-ChildItem orig\GC6E01
   Get-ChildItem build\GC6E01
   ```

4. Compiled the owning translation unit:

   ```powershell
   python tools\compile_check.py src\game\people\people_data.c --verbose
   ```

5. Ran a symbol-level match test for the selected function:

   ```powershell
   python tools\match_test.py fn_80143A94 --verbose
   ```

## Results

- `src/game/people/people_data.c` compiled successfully with CodeWarrior GC/1.3.
- Output object produced:
  - `build/GC6E01/base/game/people/people_data.o`
- `fn_80143A94` matched the original binary at `100.0%`.
- Objdiff reported:
  - size `0x28` (40 bytes, about 10 instructions)
  - `10/10` instructions matched

## Blockers

- No blocker prevented verification of the chosen target.
- A broader runtime bootstrap remains blocked by missing PC-port executable wiring:
  - `CMakeLists.txt` has `add_executable(pokemon_colosseum ...)` commented out.
  - `src/pcport/pcport_main.c` does not exist.
  - The PC-port path still depends on GLAD/GLFW/SDL2 pieces that are not part of this verified slice.

## Recommended next prerequisite if verification fails

Not applicable for this target because verification succeeded.

If the goal is to move one step larger than this symbol-level proof, the next smallest follow-up is:

- run `python tools\match_test.py --scan src\game\people\people_data.c` to verify the broader recently edited translation unit, or
- verify another symbol from commit `2685605`, such as `fn_80142984` or `fn_801429E8`

