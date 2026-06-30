# Matching Plan: 5,600 Remaining Functions

**Date:** 2026-03-22
**Target:** 100% matching (8,603 functions byte-identical)
**Current:** ~3,000 verified (~35%)
**Remaining:** ~5,600 unverified

> Audit note (2026-04-02): Treat this as a historical planning snapshot. The
> March 2026 counts above are not authoritative for the current tree. See
> [status_audit.md](status_audit.md) for the current repo-backed status summary.

## Priority Order

### Phase 1: Quick Wins (+1,400 functions, ~2 weeks)
1. **Accessor batch** (~800): Run gen_accessors.py, verify each
2. **SDK ports** (~600): Port from Melee/TP decomp projects

### Phase 2: Ghidra-Assisted (~2,000 functions, 3-4 weeks)
1. Run Ghidra headless pipeline
2. Selective import for unverified functions only
3. Manual idiomatization (92%+ match rate proven)

### Phase 3: Irreducible Control Flow (~1,500 functions, 4-6 weeks)
1. Switch statement recovery from jump tables
2. Manual rewrite with Ghidra reference
3. Accept gotos where truly irreducible

### Phase 4: HSD + Difficult Game Code (~600 functions, 2-3 weeks)
1. Port HSD from Melee decomp
2. Standard idiomatic rewrite for game code

### Phase 5: CRT Stragglers (~200 functions, 1-2 weeks)
1. Idiomatic attempts with CW 1.3
2. Assembly stubs for hand-optimized (printf, stdio)

## Per-Function Workflow
1. IDENTIFY (address, file, compiler version)
2. ASSESS (register-level? gotos? size?)
3. REFERENCE (assembly, Ghidra, Melee decomp)
4. WRITE idiomatic C89
5. COMPILE (compile_check.py)
6. TEST (match_test.py per-function, NOT --scan)
7. ITERATE until 100%
8. VERIFY no regressions in same file

## Critical Rules
- NEVER run auto tools on verified files (commit 2b4742f is checkpoint)
- NEVER use --scan on large files (OOMs)
- Always test per-function: python tools/match_test.py fn_XXXXXXXX
- CW 1.3 for: gs_render, gs_field_world, gs_model, gs_thread, battle_waza, ui_core, colosseum_battle + all CRT
- SDA21: extern u32 for .sbss, extern u8[] for absolute addressing
- do/while for bottom-tested loops (matches bdnz)
- Declare callee-saved vars r31-downward

## Effort Estimate
- 5,600 functions × ~503 person-days total
- 1 machine: ~20 months
- 5 machines: ~5-6 months
- Key: distribute work via tools/distribute.py + assign_work.py
