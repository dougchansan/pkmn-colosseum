#!/usr/bin/env python3
"""Write GLM Plans for remaining queue briefs."""

import os

QUEUE_DIR = r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\decomp_work\queue"

# Map of fn_name -> GLM Plan text (everything between the GLM Plan header and Codex Input header)
plans = {}

plans[
    "fn_800114A4"
] = """**Signature**: `void fn_800114A4(void* r3, void* r4, void* r5, void* r6, void* r7, void* r8)` -- 6 args. Render setup with model creation and dialog dispatch. Gets render context, then dispatches based on u8 arg (clrlwi r0, r28, 24). Similar to fn_80010294 pattern.

**Externs needed**: fn_8020E204, fn_8020E1A4, fn_80205B8C, fn_8012640C, fn_800FA444, fn_800FB680, fn_80132A38, fn_802040E8, fn_801F4C14, fn_8011BEB4, fn_800FA280

**Quirks**: stmw r23 saves 9 regs, crxor for variadic calls, clrlwi for u8 extraction, multiple conditional render paths."""

plans[
    "fn_8000ED34"
] = """**Signature**: `void fn_8000ED34(void* r3, void* r4)` -- 2 args. Complex dialog callback with 4-iteration party member validation, shadow Pokemon checks, and conditional direction setting. 428 lines, 0x40 stack, stmw r23-r31.

**Externs needed**: fn_80105624, fn_801040D0, fn_80109220, fn_80123FBC, fn_80075FEC, fn_8011E8DC, fn_8001DA60

**Quirks**: 4-iteration loop checking party slots, conditional shadow Pokemon validation, two dispatch paths."""

plans[
    "fn_8000F400"
] = """**Signature**: `void fn_8000F400(void* r3, void* r4)` -- 2 args. Party validation with 5-iteration loop, shadow Pokemon checks, and battle model rendering. 228 lines, 0x50 stack.

**Externs needed**: fn_801040D0, fn_80109220, fn_80123FBC, fn_80075FEC, fn_8011E8DC, fn_8012640C, fn_800FBB34, fn_800FB680, fn_800FA280, fn_80132A38

**Quirks**: 5-iteration loop, model dim calculation (srwi+extsh), fn_800FBB34 with 8 args."""

plans[
    "fn_8000F964"
] = """**Signature**: `void fn_8000F964(void* r3, void* r4)` -- 2 args. Large NPC interaction with party validation loop, shadow Pokemon rendering, and dialog dispatch. 325 lines, 0xC0 stack.

**Externs needed**: fn_801040D0, fn_80103FE4, fn_80109220, fn_80123FBC, fn_8011FC74, fn_8011FC14, fn_8012640C, fn_800FBB34, fn_800FB680, fn_800FA280, fn_800FA444, fn_80132A38, fn_8011E778, fn_8011E760, fn_80166AB8, fn_801293FC, fn_80176E0C

**Quirks**: Very complex, multiple render paths, stmw saves many registers, fn_80166AB8 for dialog setup."""

plans[
    "fn_8000D710"
] = """**Signature**: `void fn_8000D710(void* r3, void* r4)` -- 2 args. NPC interaction with jump table dispatch, team validation, and conditional battle rendering. 247 lines.

**Externs needed**: fn_801040D0, fn_801040A0, fn_80109220, fn_801040F0, fn_8012A5B0, fn_8011E850, fn_8011E8DC, fn_8011F4F0, fn_80075FEC, fn_80123FBC, fn_800FA280, fn_80132A38, fn_800FBB34, fn_800FB680, fn_800FA444

**Quirks**: Jump table for NPC IDs, shadow Pokemon validation via fn_8012A5B0, multiple render paths."""

plans[
    "fn_80010C98"
] = """**Signature**: `void fn_80010C98(void* r3_ctx, void* r4_npc)` -- 2 args. Large NPC interaction (372 lines) with party validation, model dimension calculations, and conditional rendering paths. Similar to fn_80010294 with more cases.

**Externs needed**: fn_80103FE4, fn_801040A0, fn_80205B8C, fn_80105624, fn_8012640C, fn_80132A38, fn_800FA444, fn_800FB680, fn_800FBB34, fn_80123FBC, fn_80075FEC, fn_8011E8DC, fn_8011E850, fn_80109220

**Quirks**: Multiple battle result dispatch paths, fn_800FBB34 with 8 args, model positioning calculations."""

plans[
    "fn_80011EA4"
] = """**Signature**: `void fn_80011EA4(void* r3_ctx, void* r4_npc)` -- 2 args. Very large (680 lines) NPC interaction with FP math, party member rendering loop (similar to fn_80032ED8), position calculation, and model setup. 0x70 stack, psq_st for f27-f31.

**Externs needed**: fn_80103FE4, fn_8020E0F8, fn_8020DFA0, fn_8020DF90, fn_801FCCC4, fn_801FCA2C, fn_801FCB94, fn_801FCC54, fn_801FCB84, fn_801FCAFC, fn_800896E0, fn_800896D8, fn_800896D0, fn_801FCB40, fn_801FCC3C, fn_800330B8, fn_801FC794, fn_801CA5C4, fn_80113F48, fn_80176E0C, fn_801653C4, fn_80165A20

**Quirks**: Paired-single save/restore, FP math for positions, bdnz loop for 4 Pokemon entries, species table at lbl_803A3334."""

plans[
    "fn_80012FB0"
] = """**Signature**: `void fn_80012FB0(s32 r3_idx)` -- 1 arg. Story battle callback that uses indexed struct table (stride 0x4C) at lbl_80266918 to look up participant data. 206 lines, 0x30 stack.

**Externs needed**: fn_801040A0, fn_801040D0, fn_80103FE4, fn_80205B8C, fn_8012640C, fn_80132A38, fn_800FA444, fn_800FB680, fn_8011E778, fn_8011E760, fn_800FA280, lbl_80266918, lbl_80266BD8, lbl_8047A2F8

**Quirks**: `mulli r29, r3, 0x4C` for stride lookup, stmw saves r25-r31."""

plans[
    "fn_8001329C"
] = """**Signature**: `void fn_8001329C(void* r3, void* r4)` -- 2 args. Story battle function with jump table on NPC ID (range 0x92-0x98). 252 lines, 0x20 stack.

**Externs needed**: fn_801040D0, fn_80103FE4, fn_80205B8C, fn_8012640C, fn_80132A38, fn_800FA444, fn_800FB680, fn_80109220, jumptable_802E5E4C

**Quirks**: Jump table `subi r0, r4, 0x92; cmplwi r0, 0x6`, stw saves r28-r31."""

plans[
    "fn_80013A18"
] = """**Signature**: `void fn_80013A18(s32 r3_type, void* r4_npc, void* r5_ctx)` -- 3+ args. Large event execution handler with indexed struct table (stride 0x4C) and party data lookup. Same table as fn_80012FB0. 276 lines, 0x70 stack.

**Externs needed**: fn_80205B8C, fn_8020DFA0, fn_8020DF90, fn_8020E0F8, fn_801FCCC4, fn_801FCA2C, fn_801FCB94, fn_801FCC54, fn_801FCB84, fn_801FCAFC, fn_800896E0, fn_800896D8, fn_800896D0, fn_801FCB40, fn_801FCC3C, fn_800330B8, fn_801FC794, fn_801CA5C4, fn_80113F48, fn_80176E0C, fn_801653C4, fn_80165A20, lbl_80266918, lbl_80266BD8, lbl_8047A2F8

**Quirks**: `mulli r29, r3, 0x4C` stride, indexed table with party member rendering loop, `clrlwi r0, r6, 28` bit extraction."""

plans[
    "fn_80014574"
] = """**Signature**: `void fn_80014574(void* r3_ctx)` -- 1 arg. Story battle callback with bit-flag checks and conditional rendering. Searches table at lbl_80266B58. 331 lines.

**Externs needed**: fn_80105624, fn_80205B8C, fn_8012640C, fn_80132A38, fn_800FA444, fn_800FA280, fn_800FB680, fn_80109220, lbl_80266B58, lbl_8047A2F8

**Quirks**: `clrlwi r0, r6, 28` extracts 4-bit field, table search pattern, `li r5, 0` default."""

plans[
    "fn_800EB904"
] = """**Signature**: `void fn_800EB904(void* r3_obj, f32 f1_scale)` -- r3 + float arg. Material setup with two code paths (r4 NULL check). Uses fn_800D88DC, fn_800D6A00, fn_800D85D4, fn_800D67BC. 406 lines, 0x90 stack.

**Externs needed**: fn_800D88DC, fn_800D888C, fn_800D6A00, fn_800D7820, fn_800D85D4, fn_800D67BC, fn_800D61E4, fn_800D5CB8, fn_800D59B8, fn_80032ED8

**Quirks**: psq_st/psq_l for f30/f31, `mr. r29, r4` tests r4 for NULL, two completely different code paths."""

plans[
    "fn_800E93B8"
] = """**Signature**: `s32 fn_800E93B8(void* r3, void* r4, void* r5_arg, s32 r6)` -- 4+ args. Material pool initialization that checks if pool base (lbl_80401490) exists. 433 lines, 0x50 stack.

**Externs needed**: fn_800D88DC, fn_800D888C, fn_800D6A00, fn_800D7820, fn_800D85D4, fn_800D67BC, fn_800D61E4, fn_800D5CB8, fn_800D59B8, lbl_80401490

**Quirks**: `lwz r0, 0x0(r31); cmplwi r0, 0x0` pool base check, two paths for new vs existing entries."""

plans[
    "fn_800E9E90"
] = """**Signature**: `s32 fn_800E9E90(void* r3, void* r4, void* r5, s32 r6, s32 r7, s32 r8)` -- 6+ args. Very large (533 lines) material loading function with GQR setup, paired-single ops, and conditional paths. 0x1B0 stack.

**Externs needed**: fn_800D7820, fn_800D85D4, fn_800D88DC, fn_800D888C, fn_800D6A00, fn_800D67BC, fn_80032ED8, fn_801EF214, fn_80176838, fn_80176E08, fn_800EB904

**Quirks**: `mr. r23, r3` checks first arg NULL, GQR register setup, `rlwinm. r0, r4, 0, 19, 19` bit test, stmw r22-r31."""

plans[
    "fn_80020C9C"
] = """**Signature**: `s32 fn_80020C9C(void* r3_ctx)` -- 1 arg. Title screen function that loops checking if resource 0xAA (170) is available, then renders a title menu. Uses fn_800F0308 (vsync). 140 lines.

**Externs needed**: fn_800F0308, fn_80102620, fn_801026A4, fn_80102568

**Quirks**: Loop with `fn_800F0308` for vsync, `li r3, 0xAA` resource constant, `li r31, 0` / `li r30, 0x1` initial values."""

plans[
    "fn_80023760"
] = """**Signature**: `s32 fn_80023760(void* r3_ctx, void* r4_npc)` -- 2 args. Title screen party display with 6-iteration loop checking Pokemon availability. Uses fn_800141BC, fn_80014118, fn_8005D934. 140 lines, 0x1C0 stack.

**Externs needed**: fn_800141BC, fn_80014118, fn_8005D934, fn_8012A5B0, fn_80123FBC, fn_80082EA4

**Quirks**: stmw r26 saves 6 regs, `addi r29, r1, 0x1C` for local struct, clrlwi for u16 extraction."""

plans[
    "fn_80023068"
] = """**Signature**: `s32 fn_80023068(void* r3_ctx, void* r4_npc)` -- 2 args. Title screen function that iterates party slots (fn_800141BC with arg=1), checks if Pokemon can battle. 143 lines, 0x1C0 stack.

**Externs needed**: fn_800141BC, fn_80123FBC, fn_8011FC74, fn_8011FC14, fn_8012A5B0, fn_8005D934

**Quirks**: `mr r3, r30; li r4, 0x1` party slot iteration, `cmpwi r29, 0x0` checks return value."""

plans[
    "fn_80023B9C"
] = """**Signature**: `s32 fn_80023B9C(void* r3_ctx, void* r4_npc)` -- 2 args. Title screen function that validates a single party slot (fn_800141BC with arg=1), checks HP ratio and shadow status. 143 lines, 0x1B0 stack.

**Externs needed**: fn_800141BC, fn_80123FBC, fn_8011FC74, fn_8011FC14, fn_8011E8DC, fn_8012A5B0

**Quirks**: `cmpwi r29, 0x0` after fn_800141BC, f31 comparison for HP threshold, stw saves r28-r31."""

plans[
    "fn_80023968"
] = """**Signature**: `s32 fn_80023968(void* r3_ctx, void* r4_npc)` -- 2 args. Title screen function that iterates party slots, validates each Pokemon, renders display data. Very similar to fn_80023068. 155 lines, 0x1C0 stack.

**Externs needed**: fn_800141BC, fn_80123FBC, fn_8011FC74, fn_8011FC14, fn_8012A5B0, fn_8005D934

**Quirks**: `li r26, 0x0` counter, `li r4, 0x1` party slot iteration, stmw r26-r31."""

plans[
    "fn_80025730"
] = """**Signature**: `s32 fn_80025730(void* r3_ctx)` -- 1 arg. Title screen initialization that creates a party display panel, renders Pokemon team data. Uses fn_8005D934, fn_8005DA18, fn_801040A0. 179 lines.

**Externs needed**: fn_8005D934, fn_8005DA18, fn_801040A0, fn_80103F74, fn_801081F8, fn_80113F48, lbl_80478DD8, lbl_80478DDC, lbl_80478DD4, lbl_80478DD0

**Quirks**: Multiple `li r0, 0x0` SDA global initializations, `lwz` SDA access pattern, bdnz loop for party iteration."""

count = 0
for fn, plan_text in plans.items():
    md_path = os.path.join(QUEUE_DIR, f"{fn}.md")
    with open(md_path, "r") as f:
        content = f.read()
    if "(fill in)" in content:
        # Replace the first (fill in) with our plan, then add Codex Input section back
        new_content = content.replace(
            "(fill in)\n\n## Codex Input", plan_text + "\n\n## Codex Input", 1
        )
        # If that didn't work, try the other pattern
        if "(fill in)" in new_content:
            new_content = new_content.replace(
                "(fill in)", plan_text + "\n\n## Codex Input", 1
            )
        with open(md_path, "w") as f:
            f.write(new_content)
        count += 1
        print(f"Updated {fn}")
    else:
        print(f"SKIPPED {fn} (already planned)")

print(f"\nTotal updated: {count}")
