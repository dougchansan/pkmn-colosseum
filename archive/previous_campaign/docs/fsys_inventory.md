# FSYS Archive Inventory — Pokémon Colosseum (GC6E01)

Total archives: **1,852**

## Categories

### pkx_* — Pokémon Models & Data (527 archives)
Each Pokémon has one or more `pkx_*.fsys` archives containing 3D models, textures, and animations.
Uses Japanese romanized names (e.g., `pkx_achamo` = Torchic, `pkx_absol` = Absol).

### wzx_* — Move Animations/Effects (1,108 archives)
Each move has multiple archives: `_attack`, `_damage`, and sometimes Pokémon-specific variants.
Uses Japanese move names (e.g., `wzx_10manvolt` = Thunderbolt, `wzx_abareru` = Thrash).
This is the largest category — over half of all archives on disc.

### D*_ — Dungeon/Building Areas (68 archives)
Interior areas with floor designations:
- **D1** (13) — Lab and Garage (Orre Colosseum area, opening sequence)
- **D2** (29) — Various buildings
- **D4** (26) — Later dungeons
Suffixes: `_1F` = 1st floor, `_B1` = basement 1, `_bf` = battle field, `_ev_` = event variant

### M*_ — Map/Town Areas (85 archives)
Town exteriors and interiors:
- **M1** (17) — Phenac City (gym, houses, shops, Pokémon Center)
- **M2** (26) — Pyrite Town
- **M3** (15) — The Under / Agate Village
- **M4** (27) — Later areas
Suffixes: `_out` = exterior, `_1F`/`_2F` = floor, `_bf` = battle field

### S*_ — Story/Special Areas (13 archives)
Key story locations:
- **S1** (3) — Early story area with shop
- **S2** (10) — Boss rooms, hallways, multi-floor buildings

### System & UI Archives (50 archives)
| Archive | Contents |
|---------|----------|
| `common.fsys` | Shared game assets |
| `field_common.fsys` | Shared overworld assets |
| `fight_common.fsys` | Shared battle assets |
| `fight_demo.fsys` | Battle demo/intro |
| `menu_common.fsys` | Shared menu UI |
| `title.fsys` | Title screen |
| `topmenu.fsys` | Main menu |
| `pda_menu.fsys` | PDA/P★DA interface |
| `pocket_menu.fsys` | Bag/items menu |
| `waza_menu.fsys` | Move summary screen |
| `save_menu.fsys` | Save interface |
| `pcbox_*.fsys` (4) | PC Box system (box, pocket, name entry, menu) |
| `pokemonchange_menu.fsys` | Party swap screen |
| `pokecoupon_menu.fsys` | Poké Coupon shop |
| `carde_menu.fsys` | Card e-Reader menu |
| `name_entry_menu.fsys` | Name input screen |
| `toolbattle_menu.fsys` | Colosseum battle mode menu |
| `colosseumbattle*.fsys` (2) | Battle mode UI |

### Character Assets
| Archive | Contents |
|---------|----------|
| `chara_big.fsys` (5.8 MB) | Large character models (overworld) |
| `chara_small.fsys` (136 KB) | Small character sprites/models |
| `people_archive.fsys` | NPC models and data |
| `poke_body.fsys` | Pokémon body models |
| `poke_face.fsys` | Pokémon face textures |

### Special Archives
| Archive | Contents |
|---------|----------|
| `auto_demo.fsys` | Auto-play demo sequence |
| `opening_demo.fsys` | Opening cutscene |
| `staffroll.fsys` | Credits sequence |
| `genius_logo.fsys` | Genius Sonority logo |
| `nintendo_logo.fsys` | Nintendo logo |
| `pokemon_logo.fsys` | Pokémon logo |
| `evolution.fsys` | Evolution animations |
| `bgm_archive.fsys` | Background music index |
| `camera_shake_data.fsys` | Camera effects |
| `world_map.fsys` | World/region map |
| `debug_menu.fsys` | Debug menu (!) |
| `DNA_key.fsys` | DNA key item (purification?) |
| `keydisc.fsys` | Key disc data |
| `ex_*.fsys` (4) | Extra content (coupon shop, shrine, title, top) |
| `prog_memcard.fsys` | Memory card program |
| `waza_sequence_global.fsys` | Global move sequence data |
| `waza_viewer.fsys` | Move viewer |
| `mail*.fsys` (3) | Mail system |

## Notable Findings

1. **debug_menu.fsys exists** — the game shipped with debug menu assets still on disc
2. **No REL modules** — all executable code is in the DOL; FSYS archives contain only assets
3. **Japanese naming throughout** — Pokémon and move names use Japanese romanization
4. **1,108 move effect archives** make up 60% of all files on disc
5. **T1_ancient_colo.fsys** — likely the final dungeon (Realgam Tower ancient colosseum)
