# Overnight Decompilation Queue — Workflow

## Roles
- **GLM-5.1 (tmux pane %7)** — Planner: read target asm, write a C plan
- **Codex GPT-5.4 (tmux pane %8)** — Implementer: read GLM's plan, write C, compile, match
- **Claude (me)** — Reviewer/merger: pick up completed items, verify, commit to master

## Queue structure
`tools/decomp_work/queue/QUEUE.md` — master list of pending functions with status
`tools/decomp_work/queue/fn_XXXXXXXX.md` — per-function brief with all 3 sections
`tools/decomp_work/queue/in_progress/` — function briefs currently being worked on
`tools/decomp_work/queue/completed/` — function briefs after claude-merged

## Per-function brief format
Each `fn_XXXXXXXX.md` has 4 sections in order:

```
# fn_XXXXXXXX (<file.c>, <size>, <role>)

## Status
- [ ] GLM plan
- [ ] Codex implementation
- [ ] Claude review+merge

## Target asm
<include relative path to .inc>

## Baseline
- Current match: <%>
- Already-matched functions in same file: <list, for regression-check>

## GLM Plan
<GLM writes here — pseudocode, externs needed, expected patterns>

## Codex Input (iteration 1)
<Codex writes the C, reports compile+match, revises if needed>

## Claude Review
<Claude checks, notes final match, merges>
```

## Session learnings (MUST apply)
1. **`#pragma push / peephole off / pragma pop`** around each function — defeats CW 1.3 branchless lowering
2. Switch cases in numeric order in source
3. Init accumulator/flag variables BEFORE dispatch
4. Single-return style preferred over early-return-with-blr
5. **Type externs properly**: `extern s32 fn_X(s32)` not `extern void fn_X(void)` — grep for existing decls first; if void is declared, update to match new signature
6. `(s32)(s8)ctx[N]` for signed byte access
7. **For-loop pointer-walk**: `for (idx=0; idx<N; idx++) { ... walk += 0xc; }` forces CW to emit `addi walk, walk, 0xc` instead of indexed loads
8. **Bitfield structs** (`u8 bit:1`) match CW's extlwi/rlwimi bit-toggle idiom better than `^= MASK`
9. `#pragma optimization_level 4` (not 0) for gs_scene-style existing stubs
10. Use comma-operator `!=` chain for Quirk 7 table-index searches

## Common mistakes to AVOID
- Duplicating extern decls → "identifier redeclared" compile error
- Taking too much working code out when converting (only change what's needed)
- Regressing already-matching functions in the file
- Writing pseudo-code scraps (e.g. `u8 sp[0x20]; u32 r3 = 0; ...`) — these are NOT compilable
- Using `sp[0x20]` variables or ctr_fn type placeholders — those never match

## Commit discipline
- Each function gets its own commit: `<file>: fn_XXXXXXXX 0%->Y.Y% via <approach>`
- Check regression before commit: run match_scan_file on nearby functions
- If regressed, revert and try different approach

## Regression check (CRITICAL)
Before committing, run `python3 tools/match_scan_file.py <stem> fn_A fn_B fn_C`
where fn_A..C are 3-5 previously-matched functions in the same file.
If any dropped, revert the change.

## Naming conventions from disk code
Extracted .dol strings (rodata labels, function name hints) are in:
- `src/game/*.c` header comments — many functions have inferred names
- `tools/ghidra/scripts/function_list.txt` — Ghidra's inferred symbol table
- `build/GC6E01/base/game/*.s` — disassembly with symbol names
Use these to pick descriptive local variable names.

## Handoff signal
- GLM marks "GLM plan" checkbox when done, moves file to in_progress/
- Codex marks "Codex implementation" checkbox + adds match %
- Claude marks "Claude review+merge" + moves to completed/
