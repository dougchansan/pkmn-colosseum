# Phase 3: Decompilation Plan — Pokémon Colosseum (GPXE01)

**Goal:** Replace all ~1,734 asm wrappers with matching decompiled C code, enabling a portable PC build.

**Status:** IN PROGRESS — 854/1,734 decompiled (49.3%), 880 remaining
**Updated:** 2026-04-01

> Audit note (2026-04-02): The matching techniques below are still useful, but
> the progress counts in this document were already stale by the time of the
> audit, and this repo targets `GC6E01` rather than `GPXE01`. See
> [status_audit.md](status_audit.md) for the current repo-backed status
> summary.

---

## Overall Progress

| File | Decompiled | Total | % | Remaining |
|------|-----------|-------|---|-----------|
| src/game/battle/battle_logic.c | 130 | 130 | **100%** | 0 |
| src/game/gs_material.c | 130 | 141 | **92%** | 11 |
| src/hsd/hsd_dobj.c | 18 | 20 | **90%** | 2 |
| src/game/gs_task.c | 39 | 44 | **89%** | 5 |
| src/game/gs_pcbox.c | 53 | 62 | **86%** | 9 |
| src/game/gs_render.c | 206 | 258 | **80%** | 55 |
| src/game/movie.c | 13 | 17 | **77%** | 4 |
| src/game/scene_init.c | 180 | 258 | **70%** | 78 |
| src/game/gs_worldmap.c | 42 | 93 | 45% | 51 |
| src/game/gs_field_world.c | 85 | 246 | 35% | 161 |
| src/game/effect/effect_util.c | 37 | 102 | 36% | 65 |
| src/hsd/hsd_lobj.c | 11 | 31 | 35% | 20 |
| src/hsd/hsd_mobj.c | 6 | 17 | 35% | 11 |
| src/hsd/hsd_jobj.c | 8 | 28 | 29% | 20 |
| src/game/gs_title.c | 16 | 65 | 25% | 49 |
| src/game/people/people_data.c | 28 | 61 | 46% | 28 |
| src/game/people/people.c | 7 | 53 | 13% | 46 |
| src/game/gs_thread.c | 12 | 107 | 11% | 105 |
| src/game/ui/ui_core.c | 14 | 101 | 14% | 87 |
| Other files (30+) | ~various | ~various | varies | ~170 |

---

## Compiler Configuration (Discovered)

### Default flags (GC/1.3 — game code)
```
-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8
```

### Key flag discoveries
| Flag | Effect | When Needed |
|------|--------|-------------|
| `-use_lmw_stmw on` | Inline stmw/lmw for 4+ callee-saved regs instead of _savegpr/_restgpr calls | Always (matches target binary) |
| `-sdata 8 -sdata2 8` | Restore small data thresholds stripped by -nodefaults | Always (enables SDA-relative addressing) |
| `#pragma peephole off` | Disable peephole optimizer for specific functions | When prologue instruction order differs (li before stw) |

### Per-file compiler overrides
- Dolphin SDK + HSD files: some use GC/1.2.5n (see `config/GC6E01/compile_config.json`)

---

## Hard Function Categories & Matching Techniques

### Category 1: Register Allocation Mismatch (Most Common)
**Symptom:** Correct instructions but wrong registers (e.g., r30 vs r29)
**Root Cause:** CW assigns r31→r30→r29... based on variable declaration order and live-range analysis
**Techniques:**
- Declare variables in the exact order they map to r31, r30, r29...
- First function parameter → r31, second → r30 (when saved to callee-saved regs)
- Use `{ }` inner blocks to limit variable scope and prevent unnecessary callee-saved allocation
- Temp variables used only in inner scopes may use volatile regs — don't declare at function scope
- Extra unused declarations shift ALL register assignments — remove any unnecessary vars

### Category 2: Branch Inversion
**Symptom:** `beq+b` (2 instructions) where target has `bne` (1 instruction), or vice versa
**Root Cause:** CW sometimes generates inverted branch + unconditional jump instead of direct conditional
**Techniques:**
- Try negating the condition: `if (x)` ↔ `if (!x)` with swapped branches
- Try `if (x == 0)` vs `if (!x)` — generates different branch patterns
- `mr. rX, rY` (record form) vs `mr rX, rY; cmplwi rX, 0` — use `if ((ptr = fn()) != NULL)` to get `mr.`

### Category 3: Float Register Scheduling
**Symptom:** Correct FP operations but in different FP registers (f29 vs f30)
**Root Cause:** CW's FP register allocator uses a different algorithm than GPR allocator
**Techniques:**
- FP callee-saved regs (f14-f31) assigned by first-use order, not declaration order
- Try reordering float operations to match target's FP register assignment
- `(f32)(s32)x` and `(f32)x` generate different conversion sequences
- `frsp` (round to single) appears after double→float conversion

### Category 4: Integer-to-Float Conversion (xoris/lfd/fsub trick)
**Symptom:** Target uses `xoris rX, rY, 0x8000; stw; lfd; fsubs` for int→float
**Root Cause:** CW generates this trick for unsigned int → double conversion at -O4
**Techniques:**
- Cast pattern: `(f64)(u32)value` triggers the xoris trick
- The magic constant 0x43300000_80000000 must be in sdata2
- Try `(f32)((f64)(u32)x - const)` to match the full sequence

### Category 5: CTR Loop (bdnz)
**Symptom:** Target uses `mtctr N; bdnz @loop` but compiler generates `cmplw; blt`
**Root Cause:** CW generates CTR loops for simple counted loops at -O4
**Techniques:**
- `for (i = 0; i < count; i++)` usually generates CTR loop if body doesn't modify `i`
- `do { ... } while (--count)` also generates CTR loop
- Search/find loops may not generate CTR (compiler can't prove iteration count)

### Category 6: Load-With-Update (lwzu/stbu/lbzu)
**Symptom:** Target uses `lwzu rD, offset(rA)` which combines pointer advance + load
**Root Cause:** CW generates lwzu when it sees pointer increment + dereference pattern
**Techniques:**
- Try `p += stride; val = *p;` pattern to trigger lwzu
- Array-with-stride: `*(type*)((u8*)base + off)` where `off += stride` each iteration
- Sometimes `*p++` in a loop triggers it, but not always

### Category 7: SDA Addressing (la lbl(r2) vs lis/addi)
**Symptom:** Target uses 1-instruction `la lbl(r2)` but compiler generates 2-instruction `lis/addi`
**Root Cause:** Compiler doesn't know the symbol is in sdata2 section
**Techniques:**
- Declare extern as scalar: `extern u32 lbl_XXX` (not `extern u8 lbl_XXX[]`)
- Scalar types ≤8 bytes get SDA access: u8, u16, u32, f32, f64
- Arrays NEVER get SDA access regardless of size
- For string pointers in sdata2: `extern char lbl_XXX;` with `&lbl_XXX`

### Category 8: Peephole Optimizer Issues
**Symptom:** Prologue instruction reordering (li before stw vs after)
**Root Cause:** CW's peephole pass reorders independent instructions
**Techniques:**
- Add `#pragma peephole off` before the function
- Add `#pragma peephole on` after if other functions need it
- This was common in movie.c and gs_pcbox.c

---

## Decompilation Methodology

### Per-function workflow
1. Read `src/PATH/FILENAME_fn_8XXXXXXX.inc` — the reference assembly
2. Count callee-saved registers from stmw/lmw or stw chain in prologue
3. Map registers: which reg holds which variable (r31=first, r30=second...)
4. Identify control flow: forward branches=if/else, backward=loops, bdnz=CTR loop
5. Write C89 implementation in the `#else` block
6. Compile: `python tools/compile_check.py src/PATH/FILE.c`
7. Test: `python tools/match_test.py fn_8XXXXXXX`
8. If close (>90%), use objdiff to identify exact mismatches
9. Iterate until 100% match, then flip `#if 1` → `#if 0`

### Diff tool usage
```bash
# Get instruction-level diff
./tools/objdiff-cli.exe diff -1 build/GC6E01/obj/auto_01_800055E0_text.o \
  -2 build/GC6E01/base/game/FILENAME.o \
  -o diff_out.json --format json fn_8XXXXXXX
python -c "
import json
with open('diff_out.json') as f:
    d = json.load(f)
for sec in d.get('left',{}).get('sections',[]):
    for func in sec.get('functions',[]):
        for i, ins in enumerate(func.get('instructions',[])):
            dk = ins.get('diff_kind','')
            if dk and dk != 'NONE':
                mn = ins.get('mnemonic','')
                args = ' '.join(a.get('text','') for a in ins.get('args',[]))
                print(f'{i:3d} {dk:25s} | {mn:10s} {args}')
"
```

---

## PC Port Notes

After all asm wrappers are replaced, these files still need platform abstraction:
- `src/dolphin/vi/` — video init (replace with SDL2/OpenGL framebuffer setup)
- `src/dolphin/exi/` — GCN serial/memory card (stub or replace)
- `src/dolphin/dvd/` — GCN disc I/O (replace with file I/O)
- `src/dolphin/os/` — GCN OS primitives (replace with POSIX/SDL2)
- `src/hsd/` — sysdolphin GX calls (replace per pc_port_design.md)

See `docs/pc_port_design.md` for the GX→OpenGL translation layer design.
