# fn_800DFEEC (gs_render.c, 44 asm lines)

## Status
- [x] GLM plan
- [ ] Codex implementation
- [ ] Claude review+merge

## Target asm
See `src/game/gs_render_fn_800DFEEC.inc`

## File target
`src/game/gs_render.c`

## Baseline
- Current: asm wrapper at 100% (target identical bytes, but wrapper, not real C)
- Target: ≥60% real C match

## GLM Plan (planner)
_GLM-5.1: read the .inc, identify args/return types, describe pseudocode, list externs needed. Note any quirks (switch, table-search, bit-ops, FP math, list walks, etc.)_

**Signature**: `void fn_800DFEEC(f32* r3_result, f32* r4_matA, f32* r5_vecB)` — computes some kind of matrix-vector or vector transformation product, storing 3 floats at r3.

**Pseudocode**:
```c
// This function computes a 3-component vector result from two 3x4 matrix rows and cross products.
// It appears to be: result[i] = sum_j(matA[row_i]*vecB[j]) with some cross-product terms.
// Specifically looks like a Mtx3 transform or cross-product + dot computation.

void fn_800DFEEC(Vec3* result, f32* r4_A, f32* r5_B) {
    f32 B_y = r5_B[1];  // 0x4(r5)
    f32 A_y1 = r4_A[1]; // 0x4(r4)
    f32 B_z = r5_B[2];  // 0x8(r5)
    f32 A_x1 = r4_A[0]; // 0x0(r4)

    // Cross product or transform intermediate:
    f32 f0 = B_y * A_y1;        // fmuls
    f32 B_x = r5_B[0];          // 0x0(r5)
    f32 f2 = B_z * A_y1;        // fmuls
    f32 A_z1 = r4_A[2];         // 0x8(r4)
    f32 f1 = B_y * A_x1;        // fmuls
    f32 A_w = r4_A[3];          // 0xc(r4)
    f0 = B_x * A_x1 + f0;      // fmadds (dot product accumulation)
    f32 f3 = B_x * A_w + f2;    // fmadds
    f32 f2 = B_x * A_z1;        // fmuls
    f32 f7 = B_z * A_z1 + f0;   // fmadds (cross component)
    f1 = B_z * A_w + f1;        // fmadds
    f32 f4 = -(B_y * A_z1) + f3;// fnmsubs (negated cross term)
    f32 f0 = A_x1 * f7;         // fmuls
    f2 = B_y * A_w + f2;        // fmadds
    f32 f6 = -(B_x * A_y1) + f1;// fnmsubs
    f0 = A_w * f4 + f0;         // fmadds
    f32 f5 = -(B_z * A_x1) + f2;// fnmsubs
    f0 = A_y1 * f6 + f0;        // fmadds
    f0 = -(A_z1 * f5) + f0;     // fnmsubs
    result[0] = f0;             // stfs f0, 0x0(r3)

    // Second component (similar pattern with different column access)
    f32 t_A2 = r4_A[2];  // 0x8(r4)
    f32 t_Aw = r4_A[3];  // 0xc(r4)
    f0 = t_A2 * f7;
    f32 t_A0 = r4_A[0]; // 0x0(r4)
    f32 t_A1 = r4_A[1]; // 0x4(r4)
    f0 = t_Aw * f5 + f0;
    f0 = t_A0 * f6 + f0;
    f0 = -(t_A1 * f4) + f0;
    result[1] = f0;             // stfs f0, 0x4(r3)

    // Third component
    f0 = r4_A[2] * f7;          // reuse A_z
    f1 = r4_A[3];                // A_w again
    f0 = f1 * f6 + f0;
    f2 = r4_A[0];
    f3 = r4_A[1];
    f0 = f2 * f5 + f0;
    f0 = f3 * f4 + f0;          // Wait, last is fnmsubs...
    result[2] = -(r4_A[1] * f4) + f0;  // Actually the last instruction
}
```

Actually, looking more carefully, this is a **matrix-vector product** where r4 is a 3x4 or 4x3 matrix stored as 3 rows of 4 floats, and r5 is a 3-element vector. The result is a 3-element vector. The intermediate values f4,f5,f6,f7 represent row-major computations.

**Externs needed**: None (pure computation)

**Quirks**:
- **FP-heavy**: Many `fmadds` and `fnmsubs` instructions. Use `#pragma fp_contract on` to enable FMADD/FNMSUB fusion.
- `fnmsubs f4, f6, f8, f3` = `-(f6 * f8) + f3` — NOT the typical `fma - sub` pattern.
- The function reads from r3 (result), r4 (3×4 matrix), r5 (3-element vector) — but note r4/r5 ordering.
- No stack frame — pure register-based function with leaf behavior.
- **Implementation**: This is likely `Mtx3Transform(Vec3* out, Mtx3* mat, Vec3* vec)` or a `(A × B)` cross-product+translation function. Use `#pragma fp_contract on` for the fused multiply-add patterns. Declare local f32s for the intermediates to match MWCC register allocation.

## Codex Input (implementer, iteration 1)
_Codex: implement the C based on GLM's plan. Wrap with `#pragma push / peephole off / pragma pop`. Run `python3 tools/compile_check.py src/game/gs_render.c` then `python3 tools/match_scan_file.py gs_render fn_800DFEEC`. If <60%, try the pragma variants (scheduling off), reorder locals, or use pointer-walk patterns. Commit when ≥60% OR after 5 attempts. Regression-check 3 already-matched functions in gs_render.c._

(fill in)

## Claude Review (merger)
_Claude: verify compile + match, check no regression, commit to master with message `gs_render: fn_800DFEEC 0%->X.X% via <approach>`, move this file to completed/._

(fill in)
