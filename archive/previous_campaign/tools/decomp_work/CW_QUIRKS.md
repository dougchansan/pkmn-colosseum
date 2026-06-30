# CW 1.3 byte-match cheatsheet (Pokémon Colosseum decomp)

**Use this as a system prompt when asking any model to write matching C from a PPC .inc file.**

Compiler: **CodeWarrior 1.3**, flags `-O4,p -fp hard`. Some files use override `-opt nopeephole` via `config/GC6E01/compile_config.json`.

Goal: C source that, when compiled by CW 1.3, produces **byte-identical** PowerPC output to the target `.inc`.

## Mandatory per-function workflow

1. Read `src/game/<file>_fn_XXXXXXXX.inc` line by line.
2. Grep the `.c` file for existing externs — **never duplicate**. Reuse existing typed decls.
3. Write C wrapped in:
   ```c
   #pragma push
   #pragma peephole off
   <signature> {
       <body>
   }
   #pragma pop
   ```
4. Flip `#if 1` → `#if 0` for the asm wrapper, fill the `#else` TODO slot.
5. `rm -f build/GC6E01/base/game/<file>.o && python3 tools/compile_check.py src/game/<file>.c`
6. `python3 tools/match_scan_file.py <stem> <fn> <peer1> <peer2>` — **RUN TWICE**, require identical output.
7. **Floor**: ≥90%. Below that, `git checkout src/game/<file>.c` and revert. **Never commit below 90%** — asm wrappers give 100% for free.

## Signatures & types (getting these wrong is the #1 cause of sub-90%)

- Grep the `.c` file FIRST. If `extern T fn_XXXX(...)` exists, use that exact signature.
- `(u16)` param → `cmplwi`, `(s32)(s16)` → `cmpwi` + `extsh`. Type choice governs instruction choice.
- Return type `u16` → `clrlwi` on return. `u32` → no mask. `s32` → signed-extended where needed.
- Function pointer return: use `typedef T (*Fp)(A,B); extern Fp fn(void);` — NOT `extern T (*fn(void))(A,B);` (CW rejects).
- If the local extern is K&R-style `extern void fn_X()` or `extern void fn_X(void)` but your call needs args, narrow the local decl: `extern u32 fn_X(u32, s32)`. File-local decl is enough; other TUs don't care.

## SDA (small-data-area) patterns

- Externs declared `u32 lbl_XXXX` but storing a float? Read as `*(f32*)&lbl_XXXX` (produces `lfs`, no int→float conversion).
- Scalar `extern u16 lbl_X` + `(&lbl_X)[idx]` → `li rN, lbl_X@sda21` + offset arithmetic. **Use this shape for SDA arrays.**
- Never: `extern u16 lbl_X[]` (produces absolute `lis+addi`, wrong).
- Never: `extern u16* lbl_X` (produces `lwz` of pointer value — only right if the label truly holds a pointer).
- SDA float constants that appear in asm as `lfd f2, lbl_XXXX@sda21` (magic doubles for int→float): the C `(f32)(s32)val` generates CW's internal magic. If target uses lbl_XXXX explicitly, your auto-generated `@NNNN` literal-pool ref will mismatch. **Toolchain-locked**, accept 90-97% here.

## Loop idioms

### CTR counted loop (`mtctr rN; bdnz LOOP`)
**Inline the count load into the `for` init. Separate local + outer if-guard SUPPRESSES the CTR idiom.**

✅ Correct (100% match):
```c
u32 i;
for (i = *(u32*)lbl_COUNT; i != 0; i--) {
    ...
}
```

❌ Wrong (88%):
```c
u32 n = *(u32*)lbl_COUNT;
u32 i;
if (n != 0) {
    for (i = n; i != 0; i--) { ... }
}
```

- Use `u32` counter (→ `cmplwi` unsigned). `s32` gives `cmpwi` signed — some targets require that instead; pick by reading target.
- `for (; i > 0; i--)` vs `for (; i != 0; i--)` differ in the `ble`/`beq` exit condition — pick to match target's branch.

### Down-count do-while
Target: `mtctr; cmplwi r0, 0; beq END; LOOP: ...; bdnz LOOP`
```c
for (i = EXPR; i != 0; i--) { ... }
```

## Float comparisons (fcmpo / cror)

- `fcmpo cr0, fA, fB; cror eq, gt, eq; bne SKIP` = `if (A >= B) fall-through-to-body`.
  **Write `if (A >= B)` alone — NEVER `if (A >= B || A == B)` (produces two branches, not cror).**
- `fcmpo; ble; b; fneg; ` (empty-else `ble/b` pair) = `if (A > X) { /* empty */ } else { A = -A; }`.
  Use `goto` to force the shape:
  ```c
  if (A > X) goto SKIP;
  A = -A;
  SKIP:;
  ```
- `fmadds` requires `#pragma fp_contract on` — CW 1.3 default splits `a*b+c` into `fmuls+fadds`. The pragma is **sticky** (affects all functions after it in the TU).
- `(f32)fn_RETURNING_F64(x)` generates `bl; frsp f3, f1` — required to match target's single-precision truncation.

## Integer compares & casts

- `(s8)*p` for signed byte read → `lbz; extsb`. `(s32)(s8)ctx[N]` same.
- `(s32)(s16)p[N]` for signed halfword read → `lhz; extsh` OR direct `lha`.
- `(u16)*(u32*)ptr` → `lwz; clrlwi rN, rN, 16`. Use this form (not `*(u16*)(ptr+2)` which emits `lhz`).
- `*(volatile u8*)ptr` forces reload when CW would CSE two reads of the same byte into one.
- **Bit toggle**: `bit ^= MASK` peephole-collapses into branchless. Prefer bitfield struct (`u8 bit:1` at byte offset) — matches CW's `extlwi/rlwimi` idiom.
- Signed byte subtract then `>`/`<` check: `s8 diff = (s8)((u8)a - (u8)b);` then compare — preserves the `subf; extsb; cmpwi 0; ble/bge` shape.

## Pragma recipes (try in this order when default misses)

1. Default (no pragmas)
2. `#pragma peephole off`
3. `#pragma scheduling on`
4. `#pragma scheduling on + peephole off` (COMBO — unlocks load-batching)
5. `#pragma fp_contract on` (target has `fmadds`)
6. All three + `#pragma optimization_level 4` inside the push/pop block

**Scope pragmas** with `#pragma push ... #pragma pop`. `fp_contract on` is sticky — use only when needed, and turn off before later functions if they must split `a*b+c`.

## Switch / dispatch

- 2-case switch (e.g. `case 0` + `case 3`): write cases in **numeric order** even if CW reorders dispatch tree. Case bodies stay in source order.
- Initialize the accumulator/flag variable BEFORE the switch, not inside case 0.
- Jump table (`lwzx + bctr` in asm): **SKIP** unless the jumptable symbol is already declared. Matching requires `jumptable_XXXX` decl + exact case body order.
- Progressive-if chain (6+ sequential `!=` checks with incrementing index — used for small table lookups): use **Quirk 7 comma chain**:
  ```c
  iVar = 0;
  if ((((key != tbl[0]) && (iVar=1, key != tbl[1])) && (iVar=2, ...)) && ...) iVar = N;
  ```

## Control flow shape

- `if (cond != 0) return 0; /* body */` vs `if (cond == 0) { body } return 0` pick different branch inversions in CW. Read target's `beq` vs `bne` and pick the matching form. Generally:
  - Target `beq SKIP; body; SKIP: epilogue` → `if (cond != 0) { body }`
  - Target `bne BODY; li r3, 0; b END; BODY: ...; END: epilogue` → `if (cond != 0) { body; return X; } return 0;`

## Block scoping for SDA reload

If target asm loads the same SDA symbol TWICE in different parts of the function, CW's CSE will collapse them unless each access is in a separate block scope:
```c
{ f32 x = lbl_XXXX; /* use x */ }
// ... later ...
{ f32 x = lbl_XXXX; /* force reload */ }
```

## Compile-time constants

- 3-byte writes from a u32: use `stb` triple, not `memcpy`.
- 14-byte pattern-clear at stride 0x1C: write explicit stores `p[0x08]=0; p[0x24]=0; ...`. Do NOT use a loop — CW emits a loop with different reg pattern.

## Known toolchain-locked (accept 88-99%)

- Commutative `add rA, rB, rC` vs `add rA, rC, rB` — random ordering.
- Register allocation: `f30` vs `f29` for same-scoped temp — governed by first-use order, sometimes can't be forced.
- SDA2 magic-double literal pool: target uses `lbl_XXXX@sda21`, CW's auto int→float uses `@NNNN` internal pool.
- Pointer-walk vs indexed load at -O4: adding explicit `p += stride` doesn't prevent CW from folding to `lwz offset(base)`.
- Frame-size ±0x10 on stmw-saved functions.

## Hard stop markers — skip these

- `bctr` preceded by `lwzx`/`mtctr` of a rodata symbol — jump table
- `mtspr GQR` — paired-single register setup, inline asm only
- `cntlzw` in bit-extract pattern — CW doesn't expose this intrinsic
- 64-bit `subfc/adde` — CW won't emit these from scalar C
- `frsqrte` + multi-pass Newton-Raphson — compiler-specific rsqrt expansion
- `lwzu`, `stbu`, `stfsu` (update-form loads/stores) — hard to reproduce from C
- Full `stmw r24/r25/r26` with 6+ callee-saved GPRs — reg pressure pattern is fragile

## Extern discipline (from past agent failures)

- **NEVER add externs the agent invents** — always check if one exists higher in the file.
- CW errors on redeclaration with different signature. Narrow the EXISTING decl if needed — don't add a new one.
- If unsure whether an extern exists, grep before writing. Cost of duplicate-extern compile error is higher than an extra grep.

## Verification discipline (from Sonnet hallucination incident)

- Always `rm -f build/GC6E01/base/game/<stem>.o` before re-measuring — stale `.o` from earlier attempt gives FAKE 100%.
- Run `match_scan_file.py` **twice** back-to-back. Identical output → trustworthy. Different output → stale build, investigate.
- Paste raw match_scan output in the commit message. Don't summarize.
- Parent session re-verifies every subagent's claim before merging.

## Candidate filtering (from 231-summary catalog)

Skip if summary mentions: jump table / bctr / cntlzw / divw / frsqrte / stmw r2[4-6] / mtspr / varargs with 6+ args / large state machine / scene graph.

Prefer: table-walk by key / bounds-checked array index / linear init loop / dispatch to function pointer at known offset / simple conditional write to struct field / float constant scaled by s16/s32.
