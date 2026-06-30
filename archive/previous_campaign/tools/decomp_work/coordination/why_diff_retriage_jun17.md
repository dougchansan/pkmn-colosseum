# why_diff re-triage of the worker near-miss queue — 2026-06-17

The blind near-miss queue the worker (%5) was grinding was routed by why_diff.py.
**0 of 5 were correctly-routed worker reg-alloc cracks.** This is the case the
ORCHESTRATOR_BRIEFING predicted: workers burn hours on functions that are walls,
stubs, or permuter cases. Route by why_diff BEFORE assigning a worker.

| fn | TU | objdiff% | why_diff verdict | correct route |
|---|---|---|---|---|
| fn_80034DC0 | gs_npc_event.c | 95.27 | SCHEDULE — `li r6,15560` (0x3CC8 arg) hoisted; 5 same-insn blocks | **permuter** anneal_pragma (running; peephole_off reached score 10) |
| fn_801839A0 | people/people.c | "98.86" | **STUB** — ours = 1 insn / frame 0x0 / 0 calls vs target 105 insn / frame 0x40 / 8 calls. The % was an asm-wrapper artifact. | **full Phase-1 decomp** (m2c_draft + Ghidra), NOT a crack |
| fn_80209960 | colosseum_event.c | 98.95 | why_diff MATCH + objdiff<100 ⇒ reloc artifact; already filed **W3 stmw** wall | leave walled |
| fn_80211A78 | colosseum_event.c | 98.45 | DIFFERENT-INSN = `r13 @sda21` vs absolute `@ha` (SDA reloc); already filed **W2/W3** wall | leave walled |
| fn_8020E7AC | colosseum_event.c | 96.44 | DIFFERENT-INSN — `clrlwi. r0,r0,24;bne` (target) vs `cmplwi r0,0;beq` (ours): byte-test + branch-sense. Filed as **W1 reg-alloc tie-break** — why_diff CONTRADICTS that cause. | worker re-attempt on the **bool-test expression shape** (NOT reg-alloc) |

## Lesson encoded
- **why_diff MATCH but objdiff < 100% ⇒ a reloc/representation WALL** (@sda21 vs
  absolute, stmw vs individual stw). The instruction stream matches; the residual
  is non-C-fixable section/reloc placement. Do not grind these.
- **ours = ~1 insn ⇒ STUB, not a near-miss.** The headline % came from the inactive
  asm wrapper, not the active C. Route to full decomp.
- **why_diff can re-open a misfiled wall.** fn_8020E7AC was filed as a saved-band
  reg-alloc tie-break on 2026-06-10; why_diff shows the FIRST divergence is an
  expression (byte-test + branch-sense), which a worker can attack — the filed
  cause is wrong.

## fn_8020E7AC worker packet (the one genuine worker-actionable item)
- Target insn 36-38: `clrlwi. r0,r0,24` then `bne` (positive sense, branch when
  `(u8)X != 0`). Ours: `cmplwi r0,0` then `beq` (branch when `X == 0`).
- Tried-and-FAILED lever: typing `bVar1` as `u8` → 95.97% (regressed from 96.44).
  So the `clrlwi.` value is NOT `bVar1`.
- Next levers to try: invert the `if (bVar1 == 0) goto LAB` (line ~8468, the
  matchVal<0 branch) and the `if ((bVar1) && ...)` (line ~8483) to the positive
  truthy sense so CW emits `bne`; check whether the tested value is a `(u8)` cast
  of a callee return (`fn_801FBF04`) or the `(uVar5 & 0xff) < 4` loop guard.
- 4 reg-renames + 4 structural blocks remain past the first divergence; the
  expression fix may not reach 100% alone, but it is the correct first move and
  it re-opens the wall.

## Top-30 queue routed (routed_worklist.md, commit after this)
Honest buckets after SDA-wall detection: **7 permuter, 3 worker-ex, 2 worker-fr,
7 stubs, 11 reloc-wall.** 60% (walls+stubs) are NOT worker-actionable.
- permuter (autonomous): 5-fn gs_field_world REG-RENAME cluster (fn_8011A280/570/
  9EC/AB50/AFCC, identical `mr r27` @ insn 17) + fn_800330B8 (gs_npc_event SCHEDULE)
  + fn_80068738 (ui_core REG-RENAME). [anneal_pragma on fn_8011A280 running]
- worker-ex (genuine expression): fn_8004B598 (scene_init), fn_800218BC (gs_title),
  fn_80057DE8 (scene_init).
- worker-fr (reduce our frame): fn_80128E38 (gs_field_world 0x40->0x30, +2 extra
  calls too), fn_80008868 (gs_task 0x60->0x30).
- stubs (NOT near-misses — empty C, % was asm-wrapper artifact; full decomp):
  fn_80040308, fn_8004B7EC (scene_init), fn_800DE128, fn_800E0790 (gs_render),
  fn_800109A0, fn_8000ED34, fn_8000F964 (gs_npc_interact).

## ⚠️ CORRECTION (verified against object relocations) — the SDA story below is WRONG
Ground-truth `objdump -dr` on fn_80024DBC: the TARGET uses `R_PPC_ADDR16` (absolute
symbol + explicit r13 base in the .inc), OURS uses `R_PPC_EMB_SDA21`. Both are encodings
of the SAME `lwz r0,-0x64B0(r13)` and **resolve to identical LINKED bytes** — an
object-level relocation-REPRESENTATION artifact, not a real byte diff. why_diff's
"SDA(r13) vs absolute" flag (and my route_queue SDA heuristic) trips on this representation
gap. The REAL residual on the gs_title twins is the human comment's note: **`@27` anonymous
f64 int->double bias constants vs target's NAMED `lbl_8047B8B8@sda21`** = the conversion-
literal `@nnn` artifact ([[feedback_fpr_web_conv_literal_artifact]]), a proven-uncrackable wall.
NET: the reloc-wall BUCKET is still right (leave walled), but a BSS/sdata campaign would
NOT crack them, and the per-fn "worker-ex" count is an UPPER bound — some worker-ex entries
are really @nnn conversion-literal walls the heuristic didn't catch. **Do not pursue the
BSS campaign below as a quick unlock.**

## (RETRACTED) earlier hypothesis — kept for the record, see correction above
10 of the 11 reloc-walls (whole gs_title cluster fn_80024DBC/F2C/509C/520C/537C/
25490, gs_party_access fn_8000C824/C92C, colosseum fn_80211A78/fn_8021C308) are the
SAME artifact: target reads a small global via `lwz rX, lbl@sda21(r13)`; ours emits
absolute `lis/lwz @ha/@l`. Root cause (verified): these `lbl_XXXX` globals are plain
`extern u32` resolved as ABSOLUTE linker symbols, never defined as real `.sdata`
section data — so CW cannot emit @sda21 (it's not section-relative), despite `-sdata 8`
making a u32 SDA-eligible. **A BSS/sdata reconstruction campaign (define these small
globals in real .sdata) could crack the ENTIRE @sda21 wall class at once** — far higher
leverage than per-fn grinding. Large + risky (touches symbol layout across all TUs);
needs its own validated campaign. Until then these stay filed as W-SDA-RELOC walls.
