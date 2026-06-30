# fn_80024308 — Final Review

Target: `src/game/gs_title.c` (replaces the `#else` branch at lines 4522-4570). Size: 0x130. Frame: 0x20, only r31 saved.

Closest matched sibling: `fn_80024CDC` (lines 823-866) — same float math with `fn_800D3088()` int→float magic, matched with `#pragma optimization_level 4` + `#pragma scheduling off`.

## Final C

Replace the file-level extern declarations at lines 4511-4512 (currently `u32`) with `f32`:

```c
/* fn_80024308 - 0x80024308 | size: 0x130 */
extern u32 lbl_8047A370;
extern f64 lbl_8047B8D0;
extern f32 lbl_8047A378;
extern f32 lbl_8047A374;
extern f32 lbl_8047B8C0;   /* was u32 */
extern f32 lbl_8047B8C4;   /* was u32 */
extern f32 lbl_8047B8C8;
extern f32 lbl_80478898;
extern f32 lbl_8047B8AC;
extern u32 lbl_8047A36C;
extern u32 lbl_8047A368;
```

Then the `#else` body:

```c
#pragma optimization_level 4
#pragma scheduling off
s32 fn_80024308(u8* arg0) {
    extern u8* fn_80105624(void);
    u8* ctx;
    f32 f0;
    f32 f1;
    f32 f3;

    ctx = fn_80105624();
    if (arg0 != NULL) {
        if (lbl_8047A370 != 1) {
            if ((*(u16*)(ctx + 4) & 0x10) != 0) {
                arg0[0x98] = 1;
            }
        }
        if ((*(u16*)(ctx + 4) & 0x20) != 0) {
            arg0[0x98] = 1;
            arg0[0x99] = 1;
        }
    }

    f3 = lbl_8047A378;
    f0 = lbl_8047A374 + f3 * (f32)(u32)fn_800D3088();
    lbl_8047A374 = f0;
    if (f0 < lbl_8047B8C0) {
        lbl_8047A374 = lbl_8047B8C0;
        lbl_8047A378 = -f3;
    } else if (f0 > lbl_8047B8C4) {
        lbl_8047A374 = lbl_8047B8C4;
        lbl_8047A378 = -f3;
    }

    if (lbl_8047A370 == 1) {
        f1 = lbl_80478898 - lbl_8047B8C8 * (f32)(u32)fn_800D3088();
        lbl_80478898 = f1;
        if (f1 < lbl_8047B8AC) {
            lbl_80478898 = lbl_8047B8AC;
            lbl_8047A370 = 0;
            lbl_8047A368 = lbl_8047A36C;
        }
    }
    return 0;
}
```

## Rationale

- **`#pragma scheduling off`**: the asm reloads `*(u16*)(ctx+4)` twice (`lhz r0, 0x4(r3)` appears at both 0x80024344 and 0x8002434C). With scheduling on, CW would CSE the two loads into one. `fn_80024CDC` — same file, same float idiom, matched — uses `scheduling off`. The `scheduling on` recipe from `fn_80022EE4` does not apply here; that one had no shared-memory reload pattern to preserve.
- **Local `extern u8* fn_80105624(void);`**: file-level prototype at line 2808 is `void fn_80105624(void)`, but the asm consumes r3 as a pointer. A local `u8*`-return extern shadows the broken prototype cleanly. Removes the `((u8* (*)(void))...)` cast in the previous attempt — those casts spill through a function-pointer temp and can perturb register allocation.
- **File-level `lbl_8047B8C0`/`lbl_8047B8C4` changed from `u32` to `f32`**: the asm loads them with `lfs`. The previous attempt wrote `*(f32*)&lbl_8047B8C0`, which invites CW to emit a `lwz; stw; lfs` round-trip through stack instead of a direct `lfs@sda21`. Typed as `f32`, the labels compile straight to `lfs`. These labels are referenced nowhere else in the file (grep confirmed), so retyping is local and safe.
- **`f0 = lbl_8047A374 + f3 * (f32)(u32)fn_800D3088();`**: the add-of-product ordering in C maps to CW's `fmadds f0, f3, f2, f0` (f3 and f2 are the multiplicands, f0 is the addend and destination). The inline cast `(f32)(u32)fn_800D3088()` triggers the `lis 0x4330; stw; stw; lfd; fsubs` magic-double int-to-float idiom that the asm shows at 0x80024370-0x8002438C. `fn_800D3088` already has the correct `u32` return at line 824; no local override needed.
- **`f1 = lbl_80478898 - lbl_8047B8C8 * (f32)(u32)fn_800D3088();`**: subtract-of-product → `fnmsubs f1, f2, f3, f1` (computes `-(f2*f3 - f1)` = `f1 - f2*f3`). CW 1.3 -O4 reliably picks `fnmsubs` for this specific form.
- **`lbl_8047A368 = lbl_8047A36C;`**: asm loads `lbl_8047A36C` into r0 *before* it zeros `lbl_8047A370` and then stores r0 to `lbl_8047A368`. C assignment produces this load-first ordering naturally under `-O4`. Keep the three stores in the order: `lbl_80478898`, `lbl_8047A370`, `lbl_8047A368` — that matches the asm store sequence at 0x800243E8-0x80024414.
- **No changes to the top bit-flag block structure**: the nesting `if (arg0 != NULL) { if (lbl_8047A370 != 1) { if (...0x10) ... } if (...0x20) ... }` is exactly the asm's branch graph. The 0x20 check is a sibling of the 0x10 check, not nested under it — the 0x20 branch also uses the early-exit label `@L_80024368`, not `@L_8002434C`.
- **`NULL` vs `0` for arg0 compare**: either compiles to `cmplwi r31, 0x0`. Using `NULL` reads more clearly; behavior-equivalent.

## Expected match risk (high/medium/low)

**Low.**

- The sibling `fn_80024CDC` in the same file matches with the identical pragma recipe and the same `(f32)(u32)fn_800D3088()` idiom.
- Every quirk in the asm maps to one specific C-level lever above (scheduling off for the double lhz, typed f32 externs for lfs@sda21, fn_80105624 shadowed extern for r3-as-pointer, fmadds/fnmsubs via natural operator ordering).
- No stack-buffer ambiguity (no arrays), no scheduling-sensitive compare chain, no struct layout guesses.

Residual risks, ranked:
1. **Order of the two consecutive stores** `stb 0x98; stb 0x99` at 0x80024360 (low). C writes them in source order; keep as above.
2. **`cmpwi` vs `cmplwi`** on `lbl_8047A370` comparisons (low). The asm uses signed `cmpwi r0, 0x1`; treating `lbl_8047A370` as `u32` and comparing `!= 1` / `== 1` produces `cmpwi` under CW -O4 for small positive constants. If a mismatch shows, change the extern to `s32`.
3. **`fn_800D3088` return treated as signed somewhere else in the file** (very low). File-level `u32` return matches the magic-double idiom in the asm; no need to touch it.

If this mismatches, the diff will localize to one of those three. Do not rewrite structure.
