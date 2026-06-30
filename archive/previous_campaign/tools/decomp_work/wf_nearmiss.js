export const meta = {
  name: 'decomp-nearmiss-production',
  description: 'Discover + triage + fix near-miss CW functions using validated levers, one agent per file',
  phases: [
    { title: 'Triage', detail: 'per file: find non-100% functions, classify winnable-lever vs compiler wall' },
    { title: 'Fix', detail: 'one agent per winnable file applies levers, self-verifies' },
  ],
}

// args = ["src/game/....c", ...]  (candidate files) — tolerate array OR JSON-string
let FILES = []
if (Array.isArray(args)) FILES = args
else if (typeof args === 'string') { try { const p = JSON.parse(args); if (Array.isArray(p)) FILES = p } catch (e) { FILES = [] } }
log(`triage+fix over ${FILES.length} files`)

const RULES = `
HARD RULES (violating any => the work is rejected by the parent and reverted):
- Edit ONLY the one assigned file: {FILE} (a src/**/*.c file).
- NEVER edit *_fn_*.inc (assembly truth), symbols.txt, splits*.txt, config/**, build scripts, headers, or ANY other file.
- NEVER change '#if 0' to '#if 1' (re-activating an asm wrapper = fraud).
- NEVER add/modify extern decls that could conflict across TUs; if a fix needs a header/extern change, DROP that function.
- A function COUNTS only when re-measured at EXACTLY 100.0000% by: python tools/compile_check.py --diff --require-match {FILE}
- Report the EXACT per-function % from that tool. Never estimate; never trust a prior run.
- If a change does not reach 100.0000%, MANUALLY restore that function's original code text (re-edit it back). NEVER run ANY git command (git checkout/revert/stash/etc.) — many agents edit different files at once and git index locks would corrupt the run. Just edit the file text. Never leave a regression or a broken compile.
`

const LEVERS = `
VALIDATED WINNABLE LEVERS (each fixes a specific near-miss signature; pick by reading the diff):
1. Signed/unsigned compare: cmplw<->cmpw mismatch => flip s32<->u32 on the compared variable.
2. fp_contract: target has fmadds/fmsubs but you emit fmuls+fadds => '#pragma fp_contract on' for a*b+c fusion.
3. Uncast fn-ptr -> direct bl: target 'bl func' but you emit mtctr/bctrl via a cast fn-ptr => call the function directly.
4. Dropped vararg arg: 'crset cr1eq' before a varargs bl = a dropped FLOAT arg (restore a param already in fr1). Extra 'mr rN,r3' before a varargs bl = a dropped LEADING INT arg.
5. Typed-field flag ops -> rlwinm/oris: bitfield set/clear on a typed (u8/u16/u32) field => use the unsigned-suffix literal (e.g. 0x8000u).
6. sda21 scalar extern + &: take the address of an sda21 scalar extern to match the load form.
7. clrrwi mask: 'x & ~0xN' => clrrwi.
8. void-pseudo-register param: a decompiled 'void fn(void)' that actually takes a param => real signature + forward decl, then #pragma scheduling/peephole as needed.
9. goto-ret0 shared-exit: rewrite 'if(ptr==NULL) goto _ret0;' to flip 'bne load_path' into the target's 'beq null_path' shape.
10. widen shared extern: widen a shared extern's type to kill a spurious clrlwi truncation.
11. named-struct-copy: store-ORDER mismatch in a block copy => '*(struct S*)dst = *(struct S*)src' with a NAMED struct tag.
12. mtctr/bdnz inline: inline the loop-count expression into the for-init; no separate count local, no outer if-guard.
13. separate int->float: pull '(f32)(u32)fn()' into its own statement to avoid a __cvt_fp2unsigned lib call.
14. f32 array frame-size: convert scalar f32 locals to 'f32 buf[N]' to force the target's larger stack frame.
15. inner parens assoc: parentheses around (a+b)/(a-b) flip CW's FP add evaluation order.
16. volatile cast reload: '*(volatile T*)&lbl' defeats CSE on repeated sdata2 reads (forces per-use reload).
17. s16 float direct cast: '(s16)f32' NOT '(s16)(s32)f32' (avoids a spurious extsh); pair with peephole off.
18. u16->s32 param demotion: demote a u16 param to s32 so CW emits clrlslwi on the original reg.
19. switch one-case: 'switch(x){case 0: ...; break;}' forces CW's 'beq L; b end; L:' two-branch shape.
20. lwzu via do-while-break: 'lwzu' is reachable via do{...break...}while(0) + a 'p += K;' compound-assign.
21. OR operand order; a lower -O level; or inlining a call into the argument list.
Per-file CW quirks are documented in tools/decomp_work/CLAUDE.md (read it for declaration-order / cast tricks).
PRAGMAS that unlock matches: '#pragma scheduling on', '#pragma fp_contract on', '#pragma peephole off' (or push/peephole off/pop on one function). Combos like 'scheduling on + peephole off' match where singles don't.

WALLS -- do NOT attempt (compiler-deterministic, unfixable from C); classify as wall:
- register allocation / live-range coalescing (extra mr, swapped r31/r30/r29 that declaration order cannot fix)
- instruction scheduling where the target wants the opposite load order
- stmw threshold (CW emits stmw only at >=5 saved regs; target wants 2-3 separate stw)
- commutative operand order that is pure 'saved OP call()' landing in a fresh reg
- boolean-materialize idiom (subic/subfe vs neg/or/srwi)
`

const TRIAGE_SCHEMA = {
  type: 'object',
  properties: {
    file: { type: 'string' },
    winnable: {
      type: 'array',
      description: 'functions below 100% that match a validated lever and are worth a fix attempt',
      items: {
        type: 'object',
        properties: {
          name: { type: 'string' },
          pct: { type: 'number' },
          lever: { type: 'string' },
        },
        required: ['name', 'lever'],
      },
    },
    wall_count: { type: 'number', description: 'how many non-100% functions were classified as walls' },
  },
  required: ['file', 'winnable', 'wall_count'],
}

const FIX_SCHEMA = {
  type: 'object',
  properties: {
    file: { type: 'string' },
    fixed: { type: 'array', items: { type: 'string' }, description: 'function names re-measured at EXACTLY 100.0000% after your edits' },
    compile_ok: { type: 'boolean' },
    notes: { type: 'string' },
  },
  required: ['file', 'fixed', 'compile_ok'],
}

const results = await pipeline(
  FILES,
  // Stage 1: triage — discover + classify (read-only)
  (file) =>
    agent(
      `Triage near-miss CodeWarrior decomp functions in ONE file for byte-exact matching.\n` +
        `File: ${file}\n\n` +
        `Step 1: run  python tools/compile_check.py --diff ${file}  to list every function and its match %.\n` +
        `Step 2: focus ONLY on functions in the 80%-99.99% band (the winnable-dense range; ignore <80% as wrong-shape/structural and ignore 100% and 0% stubs). ` +
        `For each such function view its diff: python tools/compile_check.py --diff --symbols <name> ${file} , and decide if the residual is a WINNABLE lever or a WALL. ` +
        `If a file has NO functions in the 80-99.99% band, return empty winnable and wall_count 0 quickly.\n${LEVERS}\n` +
        `Return ONLY the winnable functions (with the lever that applies) plus a count of walls. ` +
        `Be conservative: if the residual diff is pure register allocation / scheduling / stmw-threshold / commutative-operand / bool-materialize, it is a WALL, not winnable.`,
      { label: `triage:${file.split('/').pop()}`, phase: 'Triage', schema: TRIAGE_SCHEMA }
    ),
  // Stage 2: fix — only if triage found winnable functions
  (triage, file) => {
    const win = (triage && triage.winnable) || []
    if (!win.length) return { file, fixed: [], compile_ok: true, notes: `no winnable (${triage ? triage.wall_count : '?'} walls)` }
    return agent(
      `Fix near-miss CodeWarrior decomp functions to EXACT 100.0000% byte match in ONE file.\n` +
        RULES.replace(/{FILE}/g, file) +
        `\nTarget functions and the lever to try for each:\n${JSON.stringify(win, null, 1)}\n${LEVERS}\n\n` +
        `Per function: view its diff (python tools/compile_check.py --diff --symbols <name> ${file}), apply the indicated lever, recompile/recheck, iterate. ` +
        `If a function will not reach EXACTLY 100.0000%, manually restore its original code text (NO git commands) and move on. Do not touch functions already at 100%. ` +
        `Finally run  python tools/compile_check.py --diff --require-match ${file}  and report ONLY the function names now at exactly 100.0000%.`,
      { label: `fix:${file.split('/').pop()}`, phase: 'Fix', schema: FIX_SCHEMA }
    )
  }
)

const out = results.filter(Boolean)
const fixedFiles = out.filter((r) => r.fixed && r.fixed.length)
const total = fixedFiles.reduce((n, r) => n + r.fixed.length, 0)
log(`fix agents reported ${total} functions at 100% across ${fixedFiles.length} files (PARENT MUST RE-VERIFY EACH)`)
return out
