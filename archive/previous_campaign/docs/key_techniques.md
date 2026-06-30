# Key Techniques — Pokémon Colosseum Decompilation

Everything we've learned about matching Metrowerks CodeWarrior GC output
byte-for-byte against the original Pokémon Colosseum DOL. This document
is a living reference — update it as new patterns are discovered.

**Last updated:** 2026-04-11

---

## Table of Contents

1. [Toolchain Setup](#1-toolchain-setup)
2. [Compiler Versions](#2-compiler-versions)
3. [Pragma Cookbook](#3-pragma-cookbook)
4. [C89 Rules](#4-c89-rules)
5. [Register Allocation](#5-register-allocation)
6. [SDA / Small Data Area](#6-sda--small-data-area)
7. [Float Matching](#7-float-matching)
8. [Branch Patterns](#8-branch-patterns)
9. [Signed vs Unsigned](#9-signed-vs-unsigned)
10. [Struct and Pointer Access](#10-struct-and-pointer-access)
11. [Common Mismatches](#11-common-mismatches)
12. [Workflow](#12-workflow)
13. [Tools Reference](#13-tools-reference)
14. [New Patterns (Apr 10)](#14-new-patterns-discovered-2026-04-11)
15. [SDA Array Access — Scalar + Address-Of](#15-sda-array-access--scalar--address-of-pattern-session-5)
16. [Return Type Affects Register Masking](#16-return-type-affects-register-masking-session-5)
17. [Function Pointer Casts](#17-function-pointer-casts-for-callback-registration-session-5)
18. [Batch Decompilation Strategies](#18-batch-decompilation-strategies-session-5)

---

## 1. Toolchain Setup

### Default compiler flags

```
-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int
-warn off -use_lmw_stmw on -sdata 8 -sdata2 8
```

| Flag | Purpose |
|------|---------|
| `-O4,p` | Full optimization with peephole pass |
| `-use_lmw_stmw on` | Inline stmw/lmw for 4+ callee-saved registers |
| `-sdata 8 -sdata2 8` | Restore small-data thresholds stripped by `-nodefaults` |
| `-proc gekko` | GameCube CPU (PowerPC 750CXe) |
| `-fp hard` | Hardware floating point |
| `-enum int` | Enums are always 4 bytes |

### Compilers

Two compiler versions are used in this project:

| Compiler | Location | Used For |
|----------|----------|----------|
| CW GC/1.3 | `tools/mwcc_compiler/GC/1.3/` | Game code (`src/game/`), default |
| CW GC/1.2.5n | `tools/mwcc_compiler/GC/1.2.5n/` | Dolphin SDK (`src/dolphin/`), HSD (`src/hsd/`) |

---

## 2. Compiler Versions

**Always check the compiler version before decompiling a function.** Wrong
version creates an insurmountable match ceiling (typically 50-60%).

### How they differ

| Behavior | GC/1.3 | GC/1.2.5n |
|----------|--------|-----------|
| Prologue order | `stwu` first, then `mflr` | `mflr` first, then `stwu` |
| Register move | `mr rD, rA` (`or` encoding) | `addi rD, rA, 0` |
| Stack frame | Smaller, tighter | Larger (extra padding) |

### How to check

```bash
python tools/match_test.py fn_XXXXXXXX --verbose 2>&1 | grep "Compiler:"
```

### How to override

Edit `config/GC6E01/compile_config.json`:
```json
{
  "overrides": {
    "src/dolphin/dvd/DVDFsExtras.c": {
      "compiler": "GC/1.2.5n",
      "comment": "Dolphin SDK compiled with CW GC/1.2.5n"
    }
  }
}
```

### Known overrides (discovered so far)

- `src/dolphin/dvd/DVD.c` — GC/1.2.5n
- `src/dolphin/dvd/DVDLow.c` — GC/1.2.5n
- `src/dolphin/dvd/DVDQueue.c` — GC/1.2.5n
- `src/dolphin/dvd/DVDFsExtras.c` — GC/1.2.5n
- `src/hsd/hsd_class.c` — GC/1.2.5n

**Rule of thumb:** If a Dolphin SDK or HSD file is stuck at 50-60% match
with correct logic, try adding a GC/1.2.5n override.

---

## 3. Pragma Cookbook

### Optimization level

```c
#pragma push
#pragma optimization_level 0   // 0, 1, 2, 3, or 4
#pragma optimizewithasm off
void fn_XXXXXXXX(void) { ... }
#pragma pop
```

**When to use each level:**

| Level | Stack frame | Zero reuse | Scheduling | Use when |
|-------|-------------|------------|------------|----------|
| O0 | Large | No CSE | None | Original has stack-spilled locals |
| O1 | Moderate | Partial | Basic | Some scheduling but no full CSE |
| O2 | Moderate | Partial | Moderate | Middle ground |
| O4 | Minimal | Full CSE | Aggressive | Most game code (default) |

**Important:** The pragma on the asm wrapper (`#if 1` block) is NOT necessarily
the right pragma for the C replacement. The asm wrapper pragma was set to make
the assembler happy, not to match C codegen. Common pattern: wrapper has O0,
but the matching C needs no pragma (default O4).

### Peephole optimizer

```c
#pragma peephole off
void fn_XXXXXXXX(void) { ... }
#pragma peephole on
```

Use when prologue instruction order differs (li before stw) or when branch
pair optimization causes mismatches. Keep peephole ON when `srwi.` (record
form) is needed.

---

## 4. C89 Rules

CW for GameCube uses **strict C89**. All declarations must come before
statements in each block.

### The problem

```c
// ERROR in CW:
void fn(void) {
    u32 x = 1;
    do_something(x);
    u32 y = 2;      // Declaration after statement — compile error
}
```

### The fix: block scoping

```c
// OK in CW:
void fn(void) {
    u32 x = 1;
    do_something(x);
    {
        u32 y = 2;  // New block allows declaration
        use(y);
    }
}
```

### Block scoping also prevents CSE

When the original asm loads a global (e.g., `lwz r0, sym(r13)`) twice, it
means the compiler couldn't CSE the loads. Use block scopes to force reload:

```c
// Original asm: loads lbl_XXXX twice
{
    u8* p = (u8*)lbl_XXXX + offset;
    *(u32*)(p + 0x94) = val1;
}
{
    u8* p = (u8*)lbl_XXXX + offset;
    *(u32*)(p + 0x98) = val2;
}
```

---

## 5. Register Allocation

CW GC/1.3 at -O4 assigns callee-saved registers (r31 down to r14) by
**variable weight** = (number of uses) * (loop nesting depth factor).

### Rules

1. Highest-weight variable -> r31, second -> r30, etc.
2. Function parameters saved early get high weight (usually r3 -> r31)
3. Variables inside loops get much higher weight
4. Declaration order matters when weights are tied
5. Variables in inner `{ }` blocks may use volatile regs (r0, r3-r12)

### Controlling allocation

| Problem | Fix |
|---------|-----|
| Wrong register (r30 vs r29) | Reorder variable declarations |
| Extra callee-save | Remove unused declarations (they shift ALL numbering) |
| Short-lived var in callee-save | Use inner `{ }` block to keep it in volatile reg |
| Param used only once | Don't save to local — CW keeps it in r3/r4 |

### Two layers — REG-ALLOC vs SCHEDULING (2026-05-28 research)

A near-miss in a loop function is usually NOT one problem but TWO independent
layers. Separate them before grinding:

**Layer 1 — register assignment (which var → r31/r30/r29…). SOMETIMES controllable.**
For **genuinely tied-weight** loop variables, the saved register is assigned by
order of first definition — reordering declarations can shift the map. Worked on
fn_800E5790 (single-loop pointer walk): declaring `ptr, i, n` produced the target's
map (ptr=r31, counter=r29, bound=r30).

⚠️ **BUT the lever is UNRELIABLE — it is often completely inert.** On fn_800E3604
(pure register permutation, 87/87 instr, 10 reg-only diffs) declaration reordering
AND a live-range split BOTH produced byte-identical output — zero effect. There the
allocation is **weight-dominated**: CW gave r31 to the loop pointer (5 uses), but
the TARGET gave r31 to a loop-INVARIANT compare value (1 use) that inherits r31 from
a dead pre-loop variable's range. That coloring choice is NOT reachable from C. So:
try declaration reorder, but if the diff is byte-identical after, the allocator is
weight/interference-pinned and the lever won't move it — stop.

**Layer 2 — instruction scheduling (ORDER of independent ops: the prologue loads,
the loop's `addi` increments). NOT reliably controllable from C.**
Even with Layer-1 correct, CW's scheduler may order independent ops differently
from the target — e.g. it increments the loop COUNTER before the POINTER (to
shorten the path to the exit `cmpw`), whereas the target increments the POINTER
first (pipelining the next-iter load). `#pragma scheduling off` makes it globally
WORSE, not better. No source lever reliably flips it.

**The killer tension:** declaration order drives BOTH the register assignment AND
the prologue load order — and the target frequently wants them OPPOSITE. Declaring
`ptr` first gives `ptr=r31` (good) but then CW schedules the `ptr` load LAST in the
prologue, while the target loads it FIRST. You can satisfy reg-alloc OR load-order
via declaration order, not both. This is why the index→pointer loop families
(gs_material fn_800E5550 twins ×4, fn_800E5790; gs_field_world idx/base clusters)
get stuck at ~85-99% — the registers can be made right, but the residual
scheduler ordering can't. **Triage rule:** if after fixing Layer 1 the only
remaining diffs are reordered-but-otherwise-identical independent instructions,
it's a Layer-2 scheduler wall — stop, it won't reach 100% from C.

---

## 6. SDA / Small Data Area

The GameCube uses two small data areas for fast access to globals:

| Register | Section | Base Address | Purpose |
|----------|---------|-------------|---------|
| r13 | .sdata / .sbss | 0x80480820 (`_SDA_BASE_`) | Writable globals |
| r2 | .sdata2 / .sbss2 | 0x804836A0 (`_SDA2_BASE_`) | Read-only constants |

### Computing addresses from asm offsets

```python
# r13-relative (sdata/sbss)
SDA_BASE = 0x80480820
raw_offset = 0xA878  # from instruction encoding
signed = raw_offset - 0x10000 if raw_offset >= 0x8000 else raw_offset
address = SDA_BASE + signed

# r2-relative (sdata2/sbss2)
SDA2_BASE = 0x804836A0
# Same formula
```

Then look up the symbol: `grep "ADDRESS" config/GC6E01/symbols.txt`

### Scalar vs array declarations

```c
extern u32 lbl_XXXX;      // -> SDA reloc (1 instruction)
extern u8  lbl_XXXX[];    // -> lis/addi pair (2 instructions, NEVER SDA)
```

**Critical:** Objects <= 8 bytes declared as scalars get SDA addressing.
Declare large structs or arrays with `[]` to force absolute addressing.

### String pointers in SDA2

```c
extern char lbl_XXXX;   // Declare as char scalar
&lbl_XXXX               // Address-of gets SDA2 reloc
```

---

## 7. Float Matching

### sdata2 float returns — MUST use extern labels

```c
// WRONG (50% match — relocation mismatch):
f32 fn_XXXXXXXX(void) {
    return 0.0f;
}

// RIGHT (100% match):
extern f32 lbl_8047DD60;  // Look up in symbols.txt
f32 fn_XXXXXXXX(void) {
    return lbl_8047DD60;
}
```

**Why:** Float literals create TU-local sdata2 entries with different
relocations than the original binary's global sdata2 symbols. The match
tool compares relocations, so the literal version always mismatches.

### Finding the float value from the DOL

```python
import struct
with open('orig/GC6E01/sys/main.dol', 'rb') as f:
    offsets = struct.unpack('>18I', f.read(72))
    addresses = struct.unpack('>18I', f.read(72))
    sizes = struct.unpack('>18I', f.read(72))
    TARGET = 0x8047DD60  # address from symbols.txt
    for i in range(18):
        if sizes[i] > 0 and addresses[i] <= TARGET < addresses[i] + sizes[i]:
            f.seek(offsets[i] + (TARGET - addresses[i]))
            print(struct.unpack('>f', f.read(4))[0])
```

### Type declarations

| Asm instruction | C declaration |
|----------------|---------------|
| `lfs` / `stfs` | `extern f32 lbl_XXX;` |
| `lfd` / `stfd` | `extern f64 lbl_XXX;` |
| `lwz` (integer load) | `extern u32 lbl_XXX;` (NOT f32!) |

### Integer-to-float conversion

```c
(f64)(u32)value   // -> xoris/stw/lfd/fsubs (unsigned conversion trick)
(f64)(s32)value   // -> different conversion path
(f32)(s32)value   // -> different from (f32)value
```

### Float register allocation

FP callee-saved regs (f14-f31) are assigned by **first-use order**, not
declaration order. Reorder float operations to match.

---

## 8. Branch Patterns

### `beq+b` vs `bne` — UNSOLVABLE from C

CW sometimes generates `beq @skip; b @target` where the target binary has
`bne @target`. This is a code generator artifact. Functions at 95-98% with
only this mismatch are effectively matched. **Accept and move on.**

### `mr.` vs `mr` + `cmplwi`

```c
// Generates mr. (record form) — 1 instruction:
if ((ptr = fn()) != NULL)

// Generates mr + cmplwi — 2 instructions:
ptr = fn();
if (ptr != NULL)
```

### `bnelr` — conditional return

A `cmplw; bnelr` pattern in the asm means the original had an early-return
comparison. CW generates this for leaf functions at O4:

```c
void fn(Type* info) {
    info->field1 = 0;
    info->field2 = 0;
    if (info == &global_instance) {
        // extra cleanup
    }
}
```

---

## 9. Signed vs Unsigned

The C variable type controls the compare instruction:

| C type | PPC compare | Extension |
|--------|------------|-----------|
| `u32` | `cmplwi` (unsigned) | none |
| `s32` | `cmpwi` (signed) | none |
| `u16` | `cmplwi` | `clrlwi` (zero-extend) |
| `s16` | `cmpwi` | `extsh` (sign-extend) |

### Special: `(u16)-1` equality

```c
if (val == (u16)-1)  // -> subf/cntlzw/srwi (branchless)
if (val == 0xFFFF)   // -> cmplwi r3, 0xFFFF (different!)
```

---

## 10. Struct and Pointer Access

### Global pointer dereference

When the asm does `lwz rN, sym(r13); lwz rM, offset(rN)`, the C is:

```c
extern u32 sym;  // holds a pointer value
u32 result = *(u32*)((u8*)sym + offset);
```

### Setting through a global pointer

```c
extern u32 sym;
*(u32*)((u8*)sym + offset) = value;
```

### Output parameters

When the asm stores to `0(r3)` and `0(r4)`:

```c
void fn(u32* out1, u32* out2) {
    *out1 = some_global;
    *out2 = another_global;
}
```

---

## 11. Common Mismatches

| Mismatch | Likely Fix |
|----------|-----------|
| Wrong register (r30 vs r29) | Reorder variable declarations |
| `cmplwi` vs `cmpwi` | Change variable to `u32` vs `s32` |
| `clrlwi` before `stb` | Use `u8` variable or cast `(u8)` |
| Extra `extsb` / `extsh` | Remove unnecessary `(s8)` / `(s16)` cast |
| `lis/addi` vs `la lbl(r2)` | Change `extern u8[]` to `extern u32` scalar |
| `mr` + `cmplwi` vs `mr.` | Use `if ((p = fn()) != NULL)` combined pattern |
| Missing `frsp` | Add explicit `(f32)` cast |
| `li r0, X` reordered | Try `#pragma peephole off` |
| `beq+b` vs `bne` | UNSOLVABLE — accept 2-instruction mismatch |
| Stack frame too small | Try different `#pragma optimization_level` |
| `mflr` order wrong | Wrong compiler version (try GC/1.2.5n) |
| `addi rD,rA,0` vs `mr` | Wrong compiler version |
| Float literal mismatch | Use `extern f32 lbl_XXX` instead of `0.0f` |
| `li` not reused across stores | Remove `#pragma optimization_level 0` |
| Two `li r0,0` vs one | Use block scoping to prevent CSE |
| Division result differs | See Section 14: division magic |
| Thunk instruction reordered | `#pragma scheduling off` around thunk |
| Branch direction inverted after goto | Remove goto — CW O4,p inverts conditions |
| `(-r)\|r` operand order wrong | See Section 14: nonzero-test idiom |
| Pointer addressing (SDA vs lis/addi) | `extern u16*` (SDA) vs `extern u16[]` (lis/addi) |
| `li @sda21` vs `lwz @sda21` for array base | Use `extern u16 lbl;` + `(&lbl)[i]` (see Section 15) |
| `clrlwi.` vs `mr.` after function call | Return type is u16, change to u32 (see Section 16) |
| Function pointer type mismatch | Add explicit cast: `(GSEffectStartFunc)fn_name` |
| Forward decl `void()` conflicts with C params | Update forward extern to match C signature |

---

## 12. Workflow

### Per-function decompilation

1. **Read the .inc file** — understand the PPC assembly
2. **Check compiler version** — `--verbose` flag on match_test
3. **Check surrounding pragmas** — read context in the .c file
4. **Write C89 code** — declarations first, no mixed declarations
5. **Compile** — `python tools/compile_check.py <file.c>`
6. **Match test** — `python tools/match_test.py fn_XXXXXXXX`
7. **If < 100%: get diff** — use objdiff to compare instructions
8. **Iterate** — fix one mismatch at a time, retest

### Prioritization strategy

1. Start with **smallest functions** (2-5 line .inc files)
2. Batch **accessor/setter patterns** — they're mechanical
3. Do **medium functions** (10-30 lines) for steady progress
4. Leave **large functions** (100+ lines) for focused sessions
5. Park functions at 80-95% and revisit later with fresh eyes

### "Undefined label" cascade

Missing `extern` for a `bl fn_xxx` target in an asm wrapper kills ALL
subsequent asm blocks in the file. If you get a cascade of syntax errors,
check for a missing forward declaration.

---

## 13. Tools Reference

| Command | Purpose |
|---------|---------|
| `python tools/compile_check.py <file.c>` | Compile a single file |
| `python tools/compile_check.py --all` | Compile all 218 files |
| `python tools/match_test.py fn_XXXXXXXX` | Test match percentage |
| `python tools/match_test.py fn_XXXXXXXX --verbose` | Show compiler info |
| `python tools/assign_work.py --status` | Overall coverage stats |

### objdiff instruction comparison

```bash
tools/objdiff-cli.exe diff \
  -1 build/GC6E01/obj/auto_01_800055E0_text.o \
  -2 build/GC6E01/base/<path>.o \
  -o diff.json --format json fn_XXXXXXXX
```

Then parse with Python to see left vs right instruction differences.

### SDA address lookup

```python
SDA_BASE  = 0x80480820   # r13
SDA2_BASE = 0x804836A0   # r2

# Convert instruction offset to address:
raw = 0xA878  # from instruction hex
signed = raw - 0x10000 if raw >= 0x8000 else raw
addr = SDA_BASE + signed  # or SDA2_BASE for r2
# Then: grep addr in config/GC6E01/symbols.txt
```

---

## 14. New Patterns (discovered 2026-04-11)

### Division magic: `/7*4` vs `/28`

CW generates **different division algorithms** for these mathematically equivalent expressions:

```c
(x / 7) * 4   // CW: multiply-high magic number for /7, then shift for *4
x / 28        // CW: different multiply-high magic number for /28
```

If the asm shows a specific magic multiplier, match the exact expression form — do not
simplify or factor the arithmetic. Inspect the magic constant to determine which form
the original used.

### `#pragma scheduling off` for thunk instruction ordering

When a thunk (`bl target` wrapper) has its instructions reordered relative to the
surrounding code, add:

```c
#pragma scheduling off
void fn_XXXXXXXX(void) { target(); }
#pragma scheduling on
```

This prevents the peephole/scheduler from moving the `bl` instruction relative to
prologue/epilogue stores. Usually needed when the thunk is the only instruction
and the scheduler tries to hoist/sink it.

### Null-check branch direction: `if(ptr){return x;} return 0;`

CW O4,p prefers **null-check branches that fall through to the null case**:

```c
// Generates: beq @null_case (fall through), then return x path
if (ptr) {
    return x;
}
return 0;

// WRONG — generates inverted branch:
if (!ptr) {
    return 0;
}
return x;
```

The branch direction in the asm tells you which form the original used. `beq @skip`
over the return-value block means the `if(ptr){return x;}` form.

### `(-r)|r` nonzero-test idiom — operand order matters

CW generates `neg rD, rA; or rD, rD, rA` for a nonzero test that produces 0 or -1.
The operand order in the `or` is always `(negated, original)` — do not swap them:

```c
// Correct: produces CW's (-r)|r
s32 nonzero = (-(s32)val) | (s32)val;

// Wrong operand order — generates or rD, rA, rD instead:
s32 nonzero = (s32)val | (-(s32)val);
```

### `extern u16*` vs `extern u16[]` — pointer vs array addressing

These two declarations generate **completely different addressing code**:

```c
extern u16 *lbl_XXXX;   // Pointer stored in sdata2 — single lwz from r2
extern u16  lbl_XXXX[]; // Array — lis/addi absolute address pair
```

When the asm loads a 16-bit pointer via `lwz rN, offset(r2)` (one instruction),
use `extern u16 *lbl_XXXX`. When the asm uses `lis/addi` or `lis/la`, use the
array form. Mixing these is a common source of relocation mismatches.

### `beq+blr` vs `bne+skip` — known CW O4 gap

When the target has `bne @skip; blr` and our code generates `beq @done; b @skip; @done: blr`,
this is a known CW O4 peephole gap that **cannot be closed from C**. Accept a
2-instruction mismatch for functions that are otherwise 100% correct.

### `goto` makes CW codegen WORSE at O4,p

Using `goto` in C source causes CW -O4,p to:
- Invert branch conditions (`bne` becomes `beq`)
- Reorder epilogue stores
- Generate redundant moves

**Never use `goto` to match branch patterns.** Rewrite as structured control flow
(`if/else`, `while`, `do/while`) even if it looks less obvious. The optimizer
handles structured flow better than explicit jumps.

---

## 15. SDA Array Access — Scalar + Address-Of Pattern (Session 5)

When the asm accesses an SDA-resident array using `li rN, label@sda21` (compute
address) followed by `lhzx`/`lwzx` (indexed load), the extern must be a **scalar**
with address-of, not a pointer or array:

```c
// WRONG — generates lwz (loads pointer value, not address):
extern u16 *lbl_80478BF8;
result = lbl_80478BF8[index];

// WRONG — generates lis+addi (absolute addressing):
extern u16 lbl_80478BF8[];
result = lbl_80478BF8[index];

// RIGHT — generates li rN, label@sda21 (SDA address computation):
extern u16 lbl_80478BF8;
result = (&lbl_80478BF8)[index];
```

**Key distinction:**
| Declaration | Asm generated | When to use |
|------------|--------------|-------------|
| `extern u16 lbl;` + `(&lbl)[i]` | `li rN, lbl@sda21` + `lhzx` | Array data IN the SDA |
| `extern u16* lbl;` + `lbl[i]` | `lwz rN, lbl@sda21` + `lhzx` | Pointer TO array stored in SDA |
| `extern u16 lbl[];` + `lbl[i]` | `lis rN, lbl@ha` + `addi` | Array NOT in SDA |

---

## 16. Return Type Affects Register Masking (Session 5)

If an extern function returns `u16` but the asm uses `mr.` (no zero-extension),
CW is inserting a `clrlwi` instruction to mask the upper 16 bits. Fix by
declaring the return type as `u32`:

```c
// WRONG — generates clrlwi. (zero-extend u16 to u32):
extern u16 fn_80131428(void* cb, u16 size);
u32 id = fn_80131428(cb, 0x18);  // clrlwi. r31, r3, 16

// RIGHT — no mask, generates mr. directly:
extern u32 fn_80131428(void* cb, u16 size);
u32 id = fn_80131428(cb, 0x18);  // mr. r31, r3
```

The `clrlwi` vs `mr.` also cascades into register allocation — fixing the
return type often fixes seemingly unrelated register numbering mismatches
downstream.

---

## 17. Function Pointer Casts for Callback Registration (Session 5)

When passing function pointers to a registration function with typed parameters,
explicit casts are required even with `-warn off`:

```c
// Compile error without casts:
fn_80131200(id, fn_start, fn_stop, fn_trigger, ...);

// Correct — cast each function pointer:
fn_80131200(id,
    (GSEffectStartFunc)fn_start,
    (GSEffectStopFunc)fn_stop,
    (GSEffectStartFunc)fn_trigger,
    (GSEffectStopFunc)fn_stop2,
    0,
    (GSEffectUpdateFunc)fn_update,
    (GSEffectRenderFunc)fn_render);
```

Function pointer casts are compile-time only — they do NOT change the generated
`lis`/`addi` instruction sequence. Safe to use freely.

---

## 18. Batch Decompilation Strategies (Session 5)

### Template-based batching

When multiple functions follow the same pattern (e.g., all 12 GSEffect Start
functions call alloc → register callbacks → reset), extract the parameters
from each .inc file and generate C code from a template. This session matched
**11 functions in one batch** using this approach.

### Flip-and-test pipeline

For functions with existing C attempts in `#else` blocks:
1. Flip `#if 1` to `#if 0`
2. Run `match_test.py`
3. If 100%, keep. If not, revert.
4. Automate with a Python script for bulk testing.

This approach found **17 matches in gs_npc_event.c** — all from existing C code
that was already correct but never activated.

### Extern redeclaration for local scope

When a function needs a different extern signature than what's declared globally
(e.g., adding a parameter the global extern omits), use a block-local extern:

```c
void fn_80140138(u8* ptr) {
    extern void fn_800DF608(void* handle);  // Override global void() decl
    if (*(u32*)(ptr) != 0) {
        fn_800DF608(*(void**)(ptr));
        *(u32*)(ptr) = 0;
    }
}
```

### Forward declaration conflicts

When flipping asm → C, the C function signature may differ from the forward
`extern void fn_XXX(void);` declared earlier (which was only for the asm wrapper).
Update the forward declaration to match the C signature:

```c
// Before: extern void fn_801629A4(void);        // for asm wrapper
// After:  extern void fn_801629A4(u32 idx, u8 val);  // match C params
```
