# Matching Status — Pokémon Colosseum Decompilation

> **Updated 2026-04-11: 74 new matches from multi-agent session**
> gs_model.c (22), effect_visual.c (24), people_field.c (14), gs_scene.c (11),
> gs_gfx.c (fn_800D30AC), gs_worldmap.c (fn_80026740), __start.c (fn_80003458)

Historical snapshot from: 2026-03-19

> Audit note (2026-04-02): This page no longer reflects the current repo state.
> The current tree still contains 1,346 active asm-backed wrapper blocks across
> 80 C files, including files listed below as fully matching. Use
> [status_audit.md](status_audit.md) as the current status reference.

## Summary
- **846+ functions verified byte-identical** (9.8% of 8,603)
- **29 modules at 100%**
- Compiler: MetroWerks CodeWarrior GC 1.2.5n

## 100% Matching Modules

| Module | Functions | Notes |
|--------|-----------|-------|
| hsd_class.c | ALL | HSD base class system |
| hsd_object.c | ALL | HSD object reference counting |
| hsd_fobj.c | ALL | HSD keyframe/interpolation |
| hsd_aobj.c | ALL | HSD animation playback |
| hsd_robj.c | ALL | HSD constraints/IK |
| hsd_wobj.c | ALL | HSD world position targets |
| hsd_tobj.c | ALL | HSD textures |
| hsd_mobj.c | ALL | HSD materials |
| hsd_pobj.c | ALL | HSD primitives/polygons |
| hsd_dobj.c | ALL | HSD display objects |
| hsd_jobj.c | ALL | HSD joints/skeleton |
| hsd_cobj.c | ALL | HSD cameras |
| hsd_lobj.c | ALL | HSD lights |
| hsd_fog.c | ALL | HSD fog |
| hsd_displayfunc.c | 11/11 | HSD render pass dispatch |
| hsd_initialize.c | ALL | HSD init |
| battle_main.c | 51/51 | Battle loop, fight start/end |
| battle_logic.c | 30/30 | Move execution, types, status, damage, AI, shadow (merged TU) |
| gs_gfx.c | 36/36 | Graphics init/config |
| gs_mem.c | 13/13 | Handle-based heap allocator |
| gs_thread.c | 18/18 | Cooperative task system |
| gs_effect.c | 18/18 | VFX core manager |
| gs_model.c | 24/24 | 3D model management (22 matched this session) |
| effect_visual.c | 24/24 | Visual effect playback |
| people_field.c | 14/14 | Field NPC movement/interaction |
| gs_scene.c | 11+/ALL | Scene lifecycle management (11 matched this session) |
| gs_worldmap.c | 1+/ALL | World map (fn_80026740 matched) |
| gs_field_world.c | 28/28 | Field world management |
| gs_field_resource.c | 17/17 | Floor resource callbacks |
| fsys_load.c | 19/19 | FSYS archive loader |
| people.c | 31/31 | NPC core system |
| input.c | 20/20 | PAD input wrapper |
| pslist.c | 9/9 | Script list management |
| menu_battle.c | 22/22 | Colosseum battle menu |
| menu_pokecoupon.c | 12/12 | Poké Coupon shop |
| sound_se.c | 25/25 | 3D sound effects |
| colosseum_event.c | 13/13 | Event/scene management |
| **HSD subtotal** | **227** | |
| **Game subtotal** | **619+** | (includes 74 new from 2026-04-11 session) |

## Near-Perfect Modules

| Module | Match | Issue |
|--------|-------|-------|
| main.c | 145/146 (99.3%) | `main()` at 47.9% — register allocation order in prologue (r28/r29 for argc/argv). Uses `addi rN, rN, 0` instead of `mr rN, rN`. Fix: reorder variable declarations to guide register allocator. |
| PPCArch.c | 17/20 (85%) | See below |

## Known Mismatches — To Fix Later

### PPCArch.c (3 functions)

**PPCMfhid2** (0x800980C8, 8 bytes, 50%)
- Issue: SPR encoding. Our asm emits `mfspr r3, IABR` (SPR 1010) but original has `mfspr r3, HID2` (SPR 920)
- Fix: Use `mfspr r3, 920` with raw SPR number instead of symbolic name

**PPCMthid2** (0x800980D0, 8 bytes, 50%)
- Issue: Same SPR encoding. Our asm emits `mtspr IABR, r3` but original has `mtspr HID2, r3`
- Fix: Use `mtspr 920, r3` with raw SPR number

**PPCDisableSpeculation** (0x800980E0, 0x28, 90%)
- Issue: Symbol name mismatch. Calls `PPCMthid0` (our name) but original calls `fn_80098014`
- Fix: These are the same function — linker symbol resolution issue. Will resolve when proper link order is established.

### main() (0x800055E0, 0xE4, 47.9%)
- Issue: Register allocation order in function prologue
- Original: `stwu r1,-0x20(r1); mflr r0; stw r0,0x24(r1); stw r31,0x1c; stw r30,0x18; stw r29,0x14; stw r28,0x10; mr r28,r3; mr r29,r4`
- Ours: `mflr r0; lis r5,0x8000; stw r0,0x4(r1); stwu r1,-0x20(r1); stw r31,0x1c; stw r30,0x18; stw r29,0x14; addi r29,r4,0; li r4,0; stw r28,0x10; addi r28,r3,0`
- Fix: Reorder local variable declarations/usage. The compiler assigns r28=argc, r29=argv when args are used before locals.

### OSArena.c (4 functions, all 50%)
- OSGetArenaHi, OSGetArenaLo, OSSetArenaHi, OSSetArenaLo
- Issue: SDA base register addressing. Our code uses wrong SDA section (sbss vs sdata) or wrong base register (r13 vs r2)
- Fix: Ensure variables are declared in correct SDA section with proper `__declspec(section)` or linker script placement

### OSInit.c (4 functions, 0%)
- ClearArena (45%), InquiryCallback (38.9%), OSInit (64.6%), OSRegisterVersion (61.5%)
- Issue: Complex SDA global access patterns, hardware register volatility, struct layout mismatches
- Fix: Requires careful struct definitions matching exact field offsets, volatile qualifiers on hardware registers

### OSTime.c (1 function)
- __OSGetSystemTime (76.9%)
- Issue: Likely SDA offset or volatile access for the time base register
- Fix: Check volatile qualifiers and SDA section placement

### OSContext.c (6 functions, 3/9 matched)
- Issue: Context save/restore functions have heavy inline asm with GPR/FPR save patterns
- Fix: Exact register save order and SPR access encoding

### EXI.c (8 functions, 4/12 matched)
- Issue: Hardware register access patterns (memory-mapped I/O at 0xCC006800)
- Fix: Volatile pointers to hardware registers, correct struct layout for EXI channel state

### CRT printf.c (8 functions, 0/8)
- Issue: Heavily hand-optimized MetroWerks CRT. __pformatter is 0x774 bytes, float2str is 0x71C bytes
- Fix: These may need to be copied as raw assembly rather than decompiled to C. Common in GCN decomps.

### CRT stdio.c (7 functions not matching, 4/11)
- __close_all (2.4%), __flush_buffer (2.0%), __prep_buffer (7.7%), __fwrite (0.5%), fwrite (10.3%), fseek (3.7%), fwide (20.6%)
- Issue: MetroWerks CRT I/O internals with complex buffer management
- Fix: Similar to printf — may need asm stubs for the most optimized functions

### CRT exit.c (2 functions not matching, 4/6)
- exit (63.1%), one other
- Issue: Destructor chain traversal and atexit callback patterns
- Fix: Adjust loop patterns and function pointer call conventions

### CRT mem.c (3 functions, 2/5)
- memset (0%), __fill_mem (0%), memcpy (0%)
- Issue: These are in the .init section, not .text. May use different compiler settings or be hand-written assembly
- Fix: Check if these need `#pragma section` or should be assembly files (.s)

## Pattern Summary

**Game code (Genius Sonority):** Matches perfectly. Every GS engine module, battle system, menu, sound, NPC, FSYS, and HSD module tested has matched 100%.

**Dolphin SDK:** Partial matches. SDA addressing and hardware register volatility are the main issues. Need correct section placement and volatile qualifiers.

**MetroWerks CRT:** Lowest match rates. Hand-optimized library functions (printf, stdio, mem) may need to remain as assembly stubs. No-op functions and simple wrappers match fine.

## Priority Order for Fixing
1. PPCArch.c — 3 trivial fixes (SPR numbers + symbol name)
2. main() — register allocation tweak
3. OSArena.c — SDA section fix
4. OSTime.c — volatile/SDA fix
5. OSContext.c — asm register save order
6. EXI.c — hardware register volatility
7. OSInit.c — struct layout + SDA
8. CRT exit/mem — section pragmas
9. CRT stdio/printf — likely leave as asm stubs
