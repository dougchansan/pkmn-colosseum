# Routed near-miss worklist (why_diff) — 2026-06-17

Classified top 92 of `_winnable_queue.md` via why_diff first-divergence.


## permuter  (17)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 99.18 | fn_80228DAC | colosseum_script.c | REG-RENAME | first reg divergence @ aligned insn 42: target `mr      r27,r3`  vs  o |
| 98.7 | fn_80037180 | scene_init.c | REG-RENAME | first reg divergence @ aligned insn 6: target `lha     r0,6(r30)`  vs  |
| 98.6 | fn_8011A280 | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 17: target `mr      r27,r3`  vs  o |
| 98.6 | fn_8011A570 | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 17: target `mr      r27,r3`  vs  o |
| 98.6 | fn_8011A9EC | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 17: target `mr      r27,r3`  vs  o |
| 98.6 | fn_8011AB50 | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 17: target `mr      r27,r3`  vs  o |
| 98.6 | fn_8011AFCC | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 17: target `mr      r27,r3`  vs  o |
| 98.21 | fn_800330B8 | gs_npc_event.c | SCHEDULE-SWAP | instruction order differs (same instructions reordered) @ target 6..6  |
| 97.14 | fn_80121C18 | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 19: target `clrlwi. r28,r3,16`  vs |
| 97.06 | fn_8002BE08 | gs_worldmap.c | REG-RENAME | first reg divergence @ aligned insn 6: target `lwz     r31,96(r30)`  v |
| 96.64 | fn_80129E20 | gs_field_world.c | REG-RENAME | first reg divergence @ aligned insn 39: target `clrlwi  r0,r31,24`  vs |
| 96.25 | fn_800F8138 | input.c | REG-RENAME | first reg divergence @ aligned insn 6: target `li      r3,0`  vs  ours |
| 95.58 | fn_800EE3BC | gs_particle.c | REG-RENAME | first reg divergence @ aligned insn 5: target `mr      r28,r4`  vs  ou |
| 95.24 | fn_8004DC18 | scene_init.c | SCHEDULE-SWAP | instruction order differs (same instructions reordered) @ target 3..3  |
| 95.19 | fn_800F7434 | gs_thread.c | REG-RENAME | first reg divergence @ aligned insn 17: target `lis     r31,lbl_802710 |
| 95.03 | fn_8020DAD0 | colosseum_event.c | REG-RENAME | first reg divergence @ aligned insn 12: target `mr      r29,r0`  vs  o |
| 95.0 | fn_80068738 | ui_core.c | REG-RENAME | first reg divergence @ aligned insn 8: target `addi    r31,r3,lbl_803A |

## worker-ex  (30)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 98.66 | fn_8004B598 | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 0..0 / ours 0..1 |
| 98.33 | fn_800218BC | gs_title.c | DIFFERENT-INSN | different instruction(s) @ target 35..35 / ours 35..36 |
| 98.27 | fn_80043728 | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 2..2 / ours 2..3 |
| 98.14 | fn_8004DFCC | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 4..4 / ours 4..5 |
| 97.69 | fn_8019BB78 | hsd_fog.c | DIFFERENT-INSN | different instruction(s) @ target 2..3 / ours 2..3 |
| 97.07 | fn_8000FE38 | gs_npc_interact.c | DIFFERENT-INSN | different instruction(s) @ target 8..10 / ours 8..9 |
| 97.01 | fn_80029FAC | gs_worldmap.c | DIFFERENT-INSN | different instruction(s) @ target 16..16 / ours 16..28 |
| 97.01 | fn_8002A0B8 | gs_worldmap.c | DIFFERENT-INSN | different instruction(s) @ target 16..16 / ours 16..28 |
| 96.97 | fn_8002A1C4 | gs_worldmap.c | DIFFERENT-INSN | different instruction(s) @ target 13..13 / ours 13..25 |
| 96.97 | fn_8002A2CC | gs_worldmap.c | DIFFERENT-INSN | different instruction(s) @ target 13..13 / ours 13..25 |
| 96.76 | fn_80010588 | gs_npc_interact.c | DIFFERENT-INSN | different instruction(s) @ target 6..6 / ours 6..7 |
| 96.44 | fn_8020E7AC | colosseum_event.c | DIFFERENT-INSN | different instruction(s) @ target 36..38 / ours 36..38 |
| 96.36 | fn_8004D7D0 | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 13..15 / ours 13..14 |
| 96.29 | fn_80123B5C | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 18..19 / ours 18..18 |
| 96.28 | fn_80229C90 | colosseum_script.c | DIFFERENT-INSN | different instruction(s) @ target 14..15 / ours 14..15 |
| 96.07 | fn_8011DEE4 | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 9..11 / ours 9..10 |
| 95.78 | fn_8004CF78 | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 12..15 / ours 12..14 |
| 95.77 | fn_8013356C | effect_util.c | DIFFERENT-INSN | different instruction(s) @ target 6..8 / ours 6..7 |
| 95.71 | fn_800492CC | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 4..4 / ours 4..5 |
| 95.61 | fn_8011A6D4 | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 15..16 / ours 15..16 |
| 95.61 | fn_8011A860 | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 15..16 / ours 15..16 |
| 95.61 | fn_8011ACB4 | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 15..16 / ours 15..16 |
| 95.61 | fn_8011AE40 | gs_field_world.c | DIFFERENT-INSN | different instruction(s) @ target 15..16 / ours 15..16 |
| 95.56 | fn_800263B0 | gs_worldmap.c | DIFFERENT-INSN | different instruction(s) @ target 12..14 / ours 12..14 |
| 95.56 | fn_80036240 | movie.c | DIFFERENT-INSN | different instruction(s) @ target 21..22 / ours 21..21 |
| 95.27 | fn_80034DC0 | gs_npc_event.c | DIFFERENT-INSN | different instruction(s) @ target 2..2 / ours 2..3 |
| 95.23 | fn_80057DE8 | scene_init.c | DIFFERENT-INSN | different instruction(s) @ target 6..8 / ours 6..7 |
| 95.06 | fn_801FEC10 | trainer.c | DIFFERENT-INSN | different instruction(s) @ target 19..20 / ours 19..19 |
| 95.04 | fn_8000BFA0 | gs_party_access.c | DIFFERENT-INSN | different instruction(s) @ target 6..8 / ours 6..7 |
| 95.0 | fn_8013151C | effect_util.c | DIFFERENT-INSN | different instruction(s) @ target 2..4 / ours 2..4 |

## worker-fr  (9)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 99.6 | fn_80128E38 | gs_field_world.c | FRAME-SIZE | frame mismatch: target 0x30 vs ours 0x40 (ours larger by 0x10) |
| 98.54 | fn_80008868 | gs_task.c | FRAME-SIZE | frame mismatch: target 0x30 vs ours 0x60 (ours larger by 0x30) |
| 96.24 | fn_80007B30 | gs_task.c | SAVE-SET-DIFF | callee-saved set differs: ours saves EXTRA GPR [18] |
| 95.95 | fn_800281F4 | gs_worldmap.c | SAVE-SET-DIFF | callee-saved set differs: ours is MISSING GPR [24, 25, 26] that target |
| 95.56 | fn_80117330 | gs_field_world.c | FRAME-SIZE | frame mismatch: target 0x90 vs ours 0xA0 (ours larger by 0x10) |
| 95.53 | fn_80206C94 | colosseum_event.c | FRAME-SIZE | frame mismatch: target 0x30 vs ours 0x20 (target larger by 0x10) |
| 95.15 | fn_8020BFA0 | colosseum_event.c | SAVE-SET-DIFF | callee-saved set differs: ours saves EXTRA GPR [30] |
| 52.42 | fn_80025730 | gs_title.c | FRAME-SIZE | frame mismatch: target 0x20 vs ours 0x40 (ours larger by 0x20) |
| 35.34 | fn_80022478 | gs_title.c | FRAME-SIZE | frame mismatch: target 0x1C0 vs ours 0x270 (ours larger by 0xB0) |

## stub  (11)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 99.9 | fn_80040308 | scene_init.c | FRAME-SIZE | frame mismatch: target 0xE0 vs ours 0x0 (target larger by 0xE0) |
| 99.68 | fn_8004B7EC | scene_init.c | FRAME-SIZE | frame mismatch: target 0x30 vs ours 0x0 (target larger by 0x30) |
| 99.65 | fn_800DE128 | gs_render.c | FRAME-SIZE | frame mismatch: target 0x40 vs ours 0x0 (target larger by 0x40) |
| 98.8 | fn_800109A0 | gs_npc_interact.c | FRAME-SIZE | frame mismatch: target 0x20 vs ours 0x0 (target larger by 0x20) |
| 98.24 | fn_8000ED34 | gs_npc_interact.c | FRAME-SIZE | frame mismatch: target 0x40 vs ours 0x0 (target larger by 0x40) |
| 98.11 | fn_8000F964 | gs_npc_interact.c | FRAME-SIZE | frame mismatch: target 0x40 vs ours 0x0 (target larger by 0x40) |
| 98.11 | fn_8011F260 | gs_field_world.c | FRAME-SIZE | frame mismatch: target 0x20 vs ours 0x0 (target larger by 0x20) |
| 96.57 | fn_8015211C | people_field.c | FRAME-SIZE | frame mismatch: target 0x10 vs ours 0x0 (target larger by 0x10) |
| 95.26 | fn_8013EA44 | effect_visual.c | FRAME-SIZE | frame mismatch: target 0xC0 vs ours 0x0 (target larger by 0xC0) |
| 95.24 | fn_800E0790 | gs_render.c | FRAME-SIZE | frame mismatch: target 0x10 vs ours 0x0 (target larger by 0x10) |
| 0.3 | fn_80021B14 | gs_title.c | FRAME-SIZE | frame mismatch: target 0x3A0 vs ours 0x0 (target larger by 0x3A0) |

## reloc-wall  (22)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 98.75 | fn_80024DBC | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 5..6 / ours 5. |
| 98.75 | fn_80024F2C | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 5..6 / ours 5. |
| 98.75 | fn_8002509C | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 5..6 / ours 5. |
| 98.75 | fn_8002520C | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 5..6 / ours 5. |
| 98.53 | fn_800DAF60 | gs_render.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 8..9 / ours 8. |
| 98.46 | fn_80117D14 | gs_field_world.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 4..5 / ours 4. |
| 98.45 | fn_80211A78 | colosseum_event.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 41..42 / ours  |
| 98.41 | fn_8002537C | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 8..9 / ours 8. |
| 98.41 | fn_80025490 | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 8..9 / ours 8. |
| 98.33 | fn_8000C824 | gs_party_access.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 7..8 / ours 7. |
| 98.33 | fn_8000C92C | gs_party_access.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 7..8 / ours 7. |
| 98.32 | fn_8021C308 | colosseum_script.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 6..7 / ours 6. |
| 97.56 | fn_800DCD98 | gs_render.c | MATCH | no divergence found -- traces are identical |
| 97.53 | fn_800255A4 | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 14..15 / ours  |
| 97.5 | fn_800F670C | gs_thread.c | MATCH | no divergence found -- traces are identical |
| 96.51 | fn_80024160 | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 31..32 / ours  |
| 96.42 | fn_800A5108 | DVDFs.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 13..14 / ours  |
| 95.75 | fn_80216410 | colosseum_script.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 62..63 / ours  |
| 95.62 | fn_800D27FC | gs_render_util.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 4..6 / ours 4. |
| 95.25 | fn_800F7068 | gs_thread.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 7..9 / ours 7. |
| 95.16 | fn_80024A2C | gs_title.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 8..9 / ours 8. |
| 95.0 | fn_80222ADC | colosseum_script.c | DIFFERENT-INSN | SDA(r13) vs absolute: different instruction(s) @ target 23..24 / ours  |

## blocked  (3)

| match | fn | file | cat | note |
|---|---|---|---|---|
| 98.32 | fn_8003686C | scene_init.c |  | why_diff: failed to produce asm pair (see messages above) |
| 95.92 | fn_801F8E34 | trainer.c |  | why_diff: failed to produce asm pair (see messages above) |
| 95.62 | fn_801B00E0 | hsd_pobj_ext.c |  | why_diff: failed to produce asm pair (see messages above) |