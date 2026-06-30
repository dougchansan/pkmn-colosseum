# decomp notes: gs_task.c

## Solved

### 2026-06-10: symbol-name reloc lever — 9-function cluster to 100%

A cluster of 99.6-99.9% C-active functions had byte-identical code; the only
objdiff DIFF was a `bl`/`lis+addi` relocation whose symbol the target names
with the real (symbols.build.txt) name while our build emitted the `fn_<addr>`
name. The source even carried `SYMBOL-NAME WALL` comments flagging this, but the
rename had not been applied. Renaming the C identifiers (extern decls, call
sites, and same-TU definitions) to the symbols.build.txt names makes CW emit the
matching reloc. Byte-neutral; each name verified against `symbols.build.txt` at
the exact address.

Renames applied:
- fn_80053110 -> dbgMenuFightWazaEditSub
- fn_80007364 -> _dbgMenuFightFightPokemonSelectSub (6 caller siblings + def)
- fn_8011BA0C -> wazaGetWazaTypeIdName
- fn_80008244 -> _dbgMenuFightGetFightTrainerAiAddsubValueDataIdSub (def + caller)
- fn_80053064 -> dbgMenuFightTrainerDataStatusInputDigit

Result (objdiff calculatePoolRelocations=false):
| fn | before | after |
|----|--------|-------|
| fn_80007154 | 99.77 | 100.0 |
| fn_800071AC | 99.77 | 100.0 |
| fn_80007204 | 99.77 | 100.0 |
| fn_8000725C | 99.77 | 100.0 |
| fn_800072B4 | 99.77 | 100.0 |
| fn_8000730C | 99.77 | 100.0 |
| fn_800084C0 | 99.77 | 100.0 |
| fn_80007A84 | 99.88 | 100.0 |
| fn_80007848 | 99.63 | 100.0 |

## Blocked

- fn_80007B30 96.11% — `bl fn_8001E074` -> menuSubOpenYesNo name fixed in the
  reloc, but residual is STRUCTURAL: instruction scheduling reorder + stack slot
  0x8 vs 0xc and an extra `mr r0,r3`/`clrlwi` ordering. Not name-only; left as-is.
- fn_80008C40 99.64% — W6 sda21 symbol naming (`lbl_80478838@sda21` vs
  `0x0(r13)`), same class as filed fn_80008868. Not C-addressable.
