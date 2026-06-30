# decomp notes: people/people_field.c

## Solved

### 2026-06-10: `_GetInputValue` symbol-name lever — 11-function family to 100%

The `peopleFieldMotionResolveInput23C`..`peopleFieldMotionResolveInput3C8`
family (11 siblings, stride 0x48) was stuck at
**99.72%**. The C bodies were already byte-identical to the target — the ONLY
diff was the relocation symbol name on the tail call:

- target `.o`: `bl _GetInputValue`
- our base `.o`: `bl fn_80161134`

Address 0x80161134 is the same on both sides; the target object was built from
`config/GC6E01/symbols.build.txt` which names it `_GetInputValue`, while our
build resolves the C identifier verbatim. Renaming the call (and its `extern`
decl) `fn_80161134` -> `_GetInputValue` in the TU makes CW emit a reloc to the
matching symbol name. **Byte-neutral, authoritative** (the target's own reloc
literally reads `_GetInputValue`, not a structural-port guess).

Result: all 11 family members -> **100.0%** with no code change.

| fn | before | after |
|----|--------|-------|
| peopleFieldMotionResolveInput23C | 99.72 | 100.0 |
| peopleFieldMotionResolveInput260 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput284 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput2A8 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput2CC | 99.72 | 100.0 |
| peopleFieldMotionResolveInput2F0 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput338 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput35C | 99.72 | 100.0 |
| peopleFieldMotionResolveInput380 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput3A4 | 99.72 | 100.0 |
| peopleFieldMotionResolveInput3C8 | 99.72 | 100.0 |

LESSON: a 99.x C-active near-miss whose only objdiff DIFF is a `bl <name>`
reloc is a symbol-naming artifact, not reg-alloc. Check the target's reloc name
(`.inc` `bl _Name` or `symbols.build.txt`) and rename the C identifier to match.

### 2026-06-10: sound-library symbol-name renames — 4 more to 100%

Same lever applied to the audio/reverb cluster. Each name verified at its exact
address in `symbols.build.txt`:
- fn_80164520 -> ReverbHICreate   (sndAuxCallbackPrepareReverbHI caller -> 100.0)
- fn_801652DC -> ReverbHICallback (sndAuxCallbackReverbHI caller -> 100.0)
- fn_80164A2C -> ReverbHIModify   (sndAuxCallbackUpdateSettingsReverbHI caller -> 100.0)
- fn_80162118 -> sndBSearch       (fn_801523B8 caller -> 100.0)
- fn_8015A870 -> salExitDspCtrl   (hwExit -> 100.0)
- fn_80163DE8 -> aramFreeStreamBuffer (fn_80162F68 -> 100.0)
- fn_8015A950 -> salActivateStudio    (fn_80162D6C -> 100.0)
- fn_8016221C/fn_80163FFC/fn_8015A484 (fn_80162370 partial), fn_8015D44C
  (fn_801628C8 partial)

GOTCHA: a stale block of K&R structural-name externs (`extern void Name();`)
already existed near the top of the file (lines ~505-512) AND duplicated some of
the in-TU definitions/typed decls I was renaming TO. Renaming created
return-type redeclaration conflicts (`void sndBSearch()` vs `void* sndBSearch(...)`).
Fix: delete the stale K&R `extern void ReverbHICreate();` and
`extern void sndBSearch();` — the proper typed decl/def now supplies the symbol.

## Improved but not 100% (structural residual after name fixed)

- fn_80162370 98.70 -> 99.13 — name reloc fixed; residual is a `clrlwi`
  register/scheduling order (r3/r4/r5 vs r5/r3/r4). W2.
- fn_801628C8 97.67 -> 98.00 — name reloc fixed; residual is a PURE-REG
  commutative tie-break (mulli r6 vs r5, `add r0,r6` vs `add r5,r0`). W1.

## Blocked (no name component)

- fn_801631AC 97.6% — load/store ORDER (0x0 vs 0x4 then sda21 stores swapped). W2.
- fn_8015211C 96.6% — `li rN, lbl@sda21` vs `addi rN, r13, 0x0`. W6 sda21 naming.
- fn_801619E8 87.8% — `_GetInputValue` reloc already correct; residual elsewhere
  (mask/branch materialization). Not name-only.
