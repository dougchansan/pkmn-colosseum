# CW GC/1.3 Matching Guide — Pokémon Colosseum

Practical techniques for achieving 100% instruction-level matches against
the original Pokémon Colosseum DOL compiled with Metrowerks CodeWarrior
for GameCube.

## Compiler Configuration

```
-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int
-warn off -use_lmw_stmw on -sdata 8 -sdata2 8
```

### Flag notes
| Flag | Why |
|------|-----|
| `-use_lmw_stmw on` | Inline stmw/lmw for 4+ callee-saved regs (matches target) |
| `-sdata 8 -sdata2 8` | Restore small-data thresholds stripped by -nodefaults |
| `-O4,p` | Full optimization with peephole. Use `#pragma peephole off` per-function when needed |

---

## Register Allocation

CW GC/1.3 at -O4 assigns callee-saved registers (r31 down to r14) by
**variable weight** = (number of uses) * (loop nesting depth factor).

### Rules
1. **Highest-weight variable -> r31**, second -> r30, etc.
2. Function parameters saved early get high weight (usually r3->r31, r4->r30)
3. Variables inside loops get much higher weight than outside
4. Declaration order matters when weights are tied
5. Variables in inner `{ }` blocks may use volatile regs (r0, r3-r12) if their
   lifetime doesn't cross a function call

### Controlling allocation
- Declare variables in weight order, not source order
- Remove unused declarations -- they shift ALL register numbering
- Use inner `{ }` blocks to prevent callee-saved allocation for short-lived vars
- If a param is only used once, don't save it to a local -- CW may keep it in r3/r4

---

## Branch Patterns

### `beq+b` vs `bne` (UNSOLVABLE from C)
CW sometimes generates a 2-instruction `beq @skip; b @target` where the
target binary has a 1-instruction `bne @target`. This is a code generation
pattern, not fixable by negating conditions, `#pragma peephole off`,
restructuring if/else, or using gotos. Functions at 95-98% with only this
mismatch are effectively matched.

### `mr.` vs `mr` + `cmplwi`
```c
// Generates mr. (record form) -- 1 instruction:
if ((ptr = fn()) != NULL)

// Generates mr + cmplwi -- 2 instructions:
ptr = fn();
if (ptr != NULL)
```

---

## Signed vs Unsigned Comparisons

The C variable type controls the compare instruction:
- `u32 x` -> `cmplwi` (unsigned)
- `s32 x` -> `cmpwi` (signed)
- `u16 x` -> `cmplwi` after `clrlwi` (zero-extend)
- `s16 x` -> `cmpwi` after `extsh` (sign-extend)

### Special: `(u16)-1` equality check
```c
if (val == (u16)-1)  // Branchless: subf/cntlzw/srwi pattern
if (val == 0xFFFF)   // Different: cmplwi r3, 0xFFFF
```

---

## Float Operations

### Type declarations
Float globals MUST be declared with correct float type:
- `extern f32 lbl_XXX;` for lfs/stfs access
- `extern f64 lbl_XXX;` for lfd/stfd access
- `extern u32 lbl_XXX;` is WRONG -- generates lwz, not lfs

### Integer-to-float conversion
- `(f64)(u32)value` -> xoris/stw/lfd/fsubs trick (unsigned)
- `(f64)(s32)value` -> different conversion (signed)
- `(f32)(s32)value` -> different from `(f32)value`

### Float register allocation
FP callee-saved regs (f14-f31) are assigned by first-use order, not
declaration order. Reorder float operations to match.

---

## SDA (Small Data Area) Addressing

### Scalar vs array declarations
- `extern u32 lbl_XXXX;` -> `la lbl(r13)` or `la lbl(r2)` -- 1 instruction (SDA)
- `extern u8 lbl_XXXX[];` -> `lis/addi` pair -- 2 instructions, NEVER gets SDA

### String pointers in SDA2
```c
extern char lbl_XXXX;   // Declare as char scalar
&lbl_XXXX                // Address-of gets SDA reloc
```

---

## Peephole Optimizer

### `#pragma peephole off`
Use when prologue instruction order differs (li before stw) or when
branch pair optimization causes mismatches. Add before function, restore after:
```c
#pragma peephole off
void fn_XXXXXXXX(void) { ... }
#pragma peephole on
```

### Keep peephole ON when
- `srwi.` (record form) needed -- requires peephole to combine shift + compare

---

## Common Mismatch -> Fix Mapping

| Mismatch | Fix |
|----------|-----|
| Wrong register (r30 vs r29) | Reorder variable declarations |
| `cmplwi` vs `cmpwi` | Change variable to `u32` vs `s32` |
| `clrlwi` before `stb` | Use `u8` variable or cast `(u8)` |
| Extra `extsb` / `extsh` | Remove unnecessary `(s8)` / `(s16)` cast |
| `lis/addi` vs `la lbl(r2)` | Change `extern u8[]` to `extern u32` scalar |
| `mr` + `cmplwi` vs `mr.` | Use `if ((p = fn()) != NULL)` combined pattern |
| Missing `frsp` | Add explicit `(f32)` cast |
| `li r0, X` reordered | Try `#pragma peephole off` |
| `beq+b` vs `bne` | UNSOLVABLE -- accept 2-instruction mismatch |

---

## Debugging: Get Instruction-Level Diff

```bash
./tools/objdiff-cli.exe diff \
  -1 build/GC6E01/obj/auto_01_800055E0_text.o \
  -2 build/GC6E01/base/game/FILENAME.o \
  -o diff_out.json --format json fn_XXXXXXXX

python -c "
import json
with open('diff_out.json') as f:
    d = json.load(f)
left_ins = right_ins = []
for sym in d['left'].get('symbols',[]):
    if sym.get('name') == 'fn_XXXXXXXX':
        left_ins = [ins.get('instruction',{}).get('formatted','') for ins in sym.get('instructions',[])]
for sym in d['right'].get('symbols',[]):
    if sym.get('name') == 'fn_XXXXXXXX':
        right_ins = [ins.get('instruction',{}).get('formatted','') for ins in sym.get('instructions',[])]
for i in range(max(len(left_ins), len(right_ins))):
    l = left_ins[i] if i < len(left_ins) else ''
    r = right_ins[i] if i < len(right_ins) else ''
    if l != r:
        print(f'{i:3d} L: {l}')
        print(f'    R: {r}')
"
```
