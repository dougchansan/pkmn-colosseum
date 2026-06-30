# Pokemon Colosseum Reference Research

Archived: 2026-06-04

Primary source: The Cutting Room Floor (TCRF) Pokemon Colosseum page and its subpages. The direct page returned an agent-targeted trap page in this environment, so the real article text was retrieved through TCRF's MediaWiki API and paraphrased here.

This document is a working reference for play, mechanics, debug access, unused content, and restoration/decomp leads. It is not a replacement for the source pages; use the source links for screenshots, raw dialogue, and exact tables.

Validation addendum: see [dolphin_ar_validation.md](dolphin_ar_validation.md) for Dolphin 2603a harness notes, raw AR forms, and current runtime-validation status.

## Source Index

- TCRF main page: https://tcrf.net/Pok%C3%A9mon_Colosseum
- TCRF unfinished postgame: https://tcrf.net/Pok%C3%A9mon_Colosseum/Unfinished_Postgame
- TCRF leftover text: https://tcrf.net/Pok%C3%A9mon_Colosseum/Leftover_Text
- TCRF bonus discs: https://tcrf.net/Pok%C3%A9mon_Colosseum/Bonus_Discs
- TCRF e-Reader leftovers: https://tcrf.net/Pok%C3%A9mon_Colosseum/e-Reader_Leftovers
- Bulbapedia gameplay overview: https://bulbapedia.bulbagarden.net/wiki/Pok%C3%A9mon_Colosseum
- Bulbapedia Shadow Pokemon: https://bulbapedia.bulbagarden.net/wiki/Shadow_Pok%C3%A9mon
- Bulbapedia snagging: https://bulbapedia.bulbagarden.net/wiki/Snagging
- StrategyWiki guide hub: https://strategywiki.org/wiki/Pok%C3%A9mon_Colosseum

## How To Play

Pokemon Colosseum has two main surfaces:

- Story Mode: a 3D RPG in the Orre region. Wes starts with Espeon and Umbreon, meets Rui in Phenac City, and uses the Snag Machine to rescue Shadow Pokemon from Cipher-linked trainers.
- Battle Mode: Stadium-style battles using Story Mode Pokemon, Game Boy Advance Pokemon, or preset rental teams depending on mode.

Core loop:

1. Progress through towns/dungeons by winning mostly double battles.
2. Let Rui identify Shadow Pokemon in opponent parties.
3. Use Poke Balls converted by the Snag Machine to catch Shadow Pokemon from trainers.
4. Reduce each Shadow Pokemon's Heart Gauge.
5. Use the Relic Stone in Agate Village to purify the Pokemon once the final lock is ready.
6. After the credits, revisit postgame targets to snag missed Shadow Pokemon and unlock late rewards.

Important play notes:

- The first two party members are Espeon level 25 and Umbreon level 26.
- Early in Phenac City, the player chooses one Johto middle evolution to snag: Bayleef, Quilava, or Croconaw. The other two become available later.
- Shadow Pokemon are the main source of obtainable Pokemon because the game has no normal wild encounters in Story Mode.
- A successful snag immediately gives the Pokemon to the player. If the party is full, it goes to PC storage. If the player whites out after a snag, the snag still counts.
- Battle Mode includes Colosseum Battle and Battle Now. Colosseum Battle includes player-vs-player, venue challenges, Mt. Battle, and Poke Coupon rewards. Battle Now provides immediate CPU battles with preset teams.

## Mechanics Primer

### Snagging

The Snag Machine turns ordinary Poke Balls into Snag Balls. Snag Balls ignore the fact that the target already belongs to a trainer and attempt a normal capture calculation against that Pokemon. In Colosseum, this is used by Wes to rescue Shadow Pokemon rather than to steal normal Pokemon.

Implementation lead:

- Snagging is normal capture behavior with a trainer-owned override.
- Party/PC placement happens immediately on success.
- Rematches must account for already-snagged Shadow Pokemon.

### Shadow Pokemon

Shadow Pokemon are created by Cipher by closing a Pokemon's heart. In Colosseum, they:

- Initially know Shadow Rush.
- Cannot level normally while still Shadow.
- Hold earned experience and friendship until purification.
- Do not gain effort values until purification.
- Cannot be traded to mainline Generation III games until purified.
- Can enter Hyper Mode, especially as they approach purification.
- Can be calmed by calling them in battle or through other purification methods.

Implementation lead:

- Treat Shadow state as a stateful wrapper over normal Pokemon data.
- Track Heart Gauge, Shadow move replacement, deferred experience/friendship, and trade eligibility.
- Battle command handling needs Hyper Mode interruption behavior.

### Purification

The Heart Gauge is reduced by:

- Walking with the Shadow Pokemon in the party.
- Sending it into battle.
- Calling it during Hyper Mode.
- Using scents bought in Agate Village.
- Leaving it at the Agate Village Day Care.

When the gauge reaches the final state, the player must use the Relic Stone in Agate Village. Purification grants stored experience/friendship, removes Shadow Rush, and awards a purification ribbon.

### Economy And Rewards

Poke Coupons are earned mainly through Mt. Battle and Colosseum Battle. They are spent on battle-relevant TMs, held items, and rare berries. TCRF also documents unused bonus-disc reward rooms that check Poke Coupon ranks for extra prizes.

## TCRF Main Page Audit

### Debug Mode

TCRF documents a small runtime debug overlay/function that can be enabled with Action Replay codes.

Controls once enabled:

- D-Pad Up: show a Japanese instruction/debug bar.
- D-Pad Down: hide the bar.
- L: cycle pause, slow, and normal playback.
- R: pause directly.

Known behavior:

- If debug mode is disabled while the game is frozen, characters can animate in place while simulation progression remains stopped.
- A room named `waza_viewer` ("move viewer") exists in the files. It is a Phenac Stadium repeat where Debug Mode loads by default and the camera is locked.
- TCRF credits Ralf at gc-forever for the codes.

USA debug Action Replay:

```text
0W0T-3G3X-BEGU4
761R-MU87-Y5HP6
VV06-B5P3-PQQJK
43X6-ZGAX-7CNFZ
NBDE-ZNJ7-MDHZ1
```

Europe debug Action Replay:

```text
ZVG4-2Q1X-1TXE3
CXJH-RJR6-M51AT
ZKFE-CMMY-9GKHG
9393-TB37-2N5AG
XTAM-9FPW-JC1VC
```

Decomp/restoration leads:

- Locate the mode switch that the codes patch.
- Identify the debug bar renderer and instruction text.
- Trace the pause/slow/play branch to isolate frame-step and simulation-time controls.
- Investigate `waza_viewer` as a safe test room for animation and move playback tooling.

### Unused Graphics

TCRF notes:

- `face412` is a desaturated placeholder Bulbasaur icon, shifted one pixel left and lacking a shiny variant.
- Many Generation I and II Pokemon models/textures are reused from Pokemon Stadium / Stadium 2. Some expression textures and mouths remain unused.
- Shiny Celebi exists for consistency but was not legitimately obtainable in Generation III Colosseum distribution paths.
- Several cut postgame trainers had unique recolors/designs left in room textures:
  - Myth: male Chaser variant, dark skin, white suit, blue scarf.
  - Malpes: Rich Boy variant, yellow jacket, red scarf.
  - Pian: Lady variant, pink/red recolor.
- Battle Mode has unused VS portraits for Story Mode-only characters, Cipher members, Team Snagem, side characters, Somek/Battlus variants, Eagun, and Rui.

Implementation leads:

- Build an asset inventory for `face412`, cut trainer recolors, and unused VS portraits.
- Rui has enough battle-facing data to be worth tracing as a potential unused partner/trainer implementation.

### Unused Room Data

Unused battlefields:

- Cipher Lab exterior: intended for postgame or altered Card Key lab fights. Met location would be `FRONT OF LAB`.
- Mt. Battle lobby: no final battle uses the lobby. Met location would be `MT. BATTLE`.
- Realgam Tower 2F: intended for Gonzap/postgame Milky-style encounters but visibly unfinished, with actors placed above ground.

Unused met locations include:

- `PRE GYM`
- `PHENAC STADIUM` (used only by e-Reader Shadow captures)
- `PYRITE COLOSSEUM`
- `RELIC CAVE`
- `MT.BTL COLOSSEUM`
- `UNDER COLOSSEUM`
- `FRONT OF LAB`
- `REALGAMTWR LOBBY`
- `REALGAM TOWER 2F`
- `ORRE COLOSSEUM`

Possible removed dungeon:

- Dungeon prefixes skip from `D2_` Mt. Battle to `D4_` Realgam Tower.
- Empty map indices between Mt. Battle and Realgam Tower suggest a cut `D3_` dungeon with roughly five maps.
- Loading those empty maps crashes because room data is absent.
- Pokemon XD later reuses the `D3_` prefix for S.S. Libra.

Implementation leads:

- Map prefix/index gaps are useful for reconstructing scene tables.
- Do not treat empty `D3_` indices as valid scenes without asset presence checks.

### Extra Shadow Pokemon

TCRF compares debug strings, final Shadow lists, and save flags. The game has unused snag and purification flags:

- Snag flags: `1262-1357`
- Purification flags: `2126-2221`

Scrapped/replaced Shadow Pokemon in debug/final comparisons:

- Spoink -> replaced by Yanma.
- Psyduck -> replaced by Qwilfish.
- Baltoy -> replaced by Dunsparce.
- Mightyena -> replaced by Hitmontop.
- Castform -> replaced by Aipom.
- Porygon2 -> replaced by Vibrava.
- Breloom -> replaced by Sunflora.
- Lunatone -> replaced by Absol.
- Solrock -> replaced by Houndoom.

Scrapped/placeholder Shadow Pokemon or postgame candidates:

- Qwilfish, Dunsparce, Houndoom, Roselia, Yanma, Aipom, Chimecho, Minun, Sunflora, Vibrava, Electrode, Snorunt, Teddiursa, Hitmontop, Smeargle, Corphish, Spheal, Sandslash, Magikarp, Kecleon, Absol, Whiscash, Xatu, Wobbuffet, Wailmer.

e-Reader Shadow slots:

- Shadow Extra 1: Togepi.
- Shadow Extra 2: Mareep.
- Shadow Extra 3: Scizor.
- Shadow Extra 4-29: unused.

Implementation leads:

- Separate final shipped Story Mode Shadow list from debug/postgame/e-Reader candidates.
- Save flags already imply room for a larger Shadow roster.
- Use TCRF's exact order before assigning IDs in any restored content.

### Unfinished Companion Inventory

TCRF documents an incomplete sixth inventory pocket apparently associated with Rui. The intended behavior appears to be a companion overflow/holding pocket when the player inventory is full. There is text for Rui taking/using/giving items and for duplicate item handling.

Implementation leads:

- Inventory UI already has hidden pocket capacity or labels.
- Companion name placeholders appear in `pocket_menu`.
- This may be restorable, but it needs item storage layout research first.

### Unused Animations

TCRF notes three animation families:

- In-air knockout animations inherited from Stadium for Flying-type Pokemon using Fly.
- Similar in-air knockout animations exist for some Hoenn Flying types, suggesting the mechanic was considered again.
- Trainer animation slots include unused recall-like poses, hit reactions, being attacked by Shadow Pokemon, and intro animations.

Trainer animation slot notes from TCRF:

- `0`: T-pose.
- `1`: overworld idle.
- `2`: ball throw.
- `3`: arm extended, possibly recall/hold.
- `4`: battle idle.
- `5`: fear/hit reaction.
- `6`: defeat.
- `7`: attacked by a Hyper Mode Shadow Pokemon.
- `8`: guard after being attacked.
- `9`: intro animation.

Special cases:

- Wes has extra Snag Ball throw/idle animations.
- Rui has only the first seven described slots and uses one slot for Shadow detection.
- GBA protagonists have trainer-hit animations that cannot normally play because they are Battle Mode-only.
- Hypno has a fainting animation present but apparently not assigned correctly; Pokemon XD later uses it.

Implementation leads:

- Animation tables likely contain more behavior hooks than final gameplay exercises.
- Trainer hit/reaction animations can guide Shadow Hyper Mode restoration.

### Regional Differences

TCRF notes:

- Japanese intro has a longer title reveal with the stone colosseum rising from the desert.
- International title screen uses a different Rui render/expression.
- Memory card/banner and logo art changed internationally.
- Rui's international outfit has a longer shirt/skirt and a white belt.
- Somek's horseshoe moustache was removed internationally, likely to avoid resemblance issues.
- Story Mode main-menu art changed from an older Wes model in Japan to a newer one internationally.
- International credits music adds roughly seven seconds of intro buildup.

Implementation leads:

- Regional asset differences matter for matching hashes and expected screenshots.
- Rui/Somek/model differences should be region-gated if assets are ever surfaced in tooling.

## TCRF Subpage Audit

### Unfinished Postgame

TCRF reconstructs a much larger postgame from unused e-mails, battle dialogue, trainer textures, met locations, debug strings, and Snag List descriptions. The final game keeps a trimmed postgame, but the leftovers imply a chain of Shadow Pokemon sidequests.

Sidequest/restoration leads:

| Lead | Evidence summary | Likely gameplay shape |
| --- | --- | --- |
| Aipom Brothers | Bitt/Secc/Cail e-mails and dialogue mention Eins, "Eisha", six Aipom siblings, Shadow Aipom, and Shadow Qwilfish. | Repeating blocker encounters at locations such as Under Colosseum and Shrine of Time, likely with identical-looking Rider brothers. |
| Dunsparce | Secc e-mail and Myth trainer data; Myth can appear in several rooms. | Roaming trainer puzzle across Pyrite Cave, Relic Cave, Under Subway, Realgam Tower, Snagem Hideout. |
| Houndoom | Nett e-mail and Retay dialogue. | Under Tournament entry with rematches until Shadow Houndoom is snagged. |
| Roselia | Eagun e-mail, TV news, Barb gatekeeper context, Ritta trainer. | News-driven Agate/Mt. Battle encounter against a Cipher Peon. |
| Yanma | Malpes photographer intro, photo clue, Speeding Lady Dizzy. | Chase sequence in Phenac City before battle/rematch. |
| Chimecho | Pyrite police e-mail, Shadow Grass Pokemon King setup, Yuragim in Agate. | Photo clue to Agate big tree, rematch until snagged. |
| Minun | Megg e-mail, Pyrite bridge NPC, Cail/Dakem/Fortune Teller guidance. | Multi-step hunt from Pyrite to desert/suspicious-building location, using Plusle/Minun clueing. |
| Sunflora | Balargas/Shadow Grass Pokemon King e-mail and Outskirt Stand evidence. | Comedic battle after Rui corrects the "Grass-type Chimecho" mistake. |
| Vibrava | Duking e-mail and Forten/Bandana Guy battle dialogue. | Under/Pyrite Colosseum tournament route with rematches. |
| Snagem Hideout | Debug strings and Bitt e-mails. | Shadow Electrode and Shadow Snorunt were likely there before final substitutions/trimming. |
| Teddiursa | Megg e-mails, Pian Lady recolor, Duel Square context. | Duel Square rematch chain; apparently replaced by Shadow Ursaring late. |
| Hitmontop | Megg/Malpes follow-up, Pian's mother Cako. | Anger/revenge follow-up in Venus' studio/The Under after Teddiursa. |
| Smeargle | Marcia e-mail and Atri graffiti context. | Pyrite Building moving-rematch encounter. |
| Absol | Malpes photo and Guria Bandana Guy data. | Under Subway/stairs encounter with a trainer trying to control Hyper Mode. |
| Whiscash | Loget e-mail accidentally invites player. | Phenac entrance encounter and rematch. |
| Xatu | Malpes photo chase and Sanari hunter data. | Pyrite/Pyrite Cave rescue chain. |
| Wobbuffet | Johnson TV e-mail, Windmill operator clue, Mirakle B. | TV-led Pyrite Cave encounter against Mirakle B. as mandatory fight. |
| Wailmer | Malpes finally appears as a battle trainer. | Realgam Tower fight; Rich Boy recolor. |
| Kecleon | Longo e-mail and password clue. | Password-gated encounter outside Cipher Lab; likely uses unused Cipher Lab exterior battlefield. |
| Togetic | Alternate Nett e-mail exists. | Still appears to be final "last Shadow Pokemon" structure, with variant unused messaging. |

Other subpage details:

- Snag List text appears more detailed than the final game usually needs and helps reconstruct sidequest locations.
- Some loss dialogue remains unassigned in `fight.fdat`.
- `pda_menu.fdat` preserves localized sender names for unused characters.

Implementation leads:

- The best restoration path is e-mail/Snag List first, then trainer placement, then battle data.
- Avoid inventing exact quest ordering until the debug Shadow order, e-mail triggers, and room texture evidence are aligned.

### Leftover Text

Categories found by TCRF:

- Story text from Phenac City, Pyrite Town, Agate Village, Mt. Battle, Shadow Pokemon Lab, and other rooms.
- Berry Juice item text, effectively unused in Generation III because its planned source was not released.
- SD/debug SD Card text.
- Japanese start-menu text remaining in international releases.
- Internal room/weather-type names relevant to Secret Power and Weather Ball.
- Japanese-only name-entry strings in `name_entry_menu.fdat`.
- Name-only cut characters in `common_rel.fdat`.
- Cut trainers with battle dialogue in `tool_fight.fdat`.
- Ruby/Sapphire leftovers.
- Debug strings in `common_rel.fdat` in all non-Japanese versions.

Named cut-character leads:

- Name-only examples: Binet, Heatol, Yui, Rika, Drum, Carter, Kobei, Jegg, Guinta, Surfura, Lara, Rakushi, Gardon.
- Battle-dialogue examples: Tanes, Brame, Alvis, Gogor, Roaty and localized equivalents.
- Extra Cipher Admin intro dialogue exists for Deep Colosseum, possibly differentiating first encounter vs Shadow-snag state.

Debug strings worth tracing:

- `MAP CHANGE`
- `Memory Information on/off`
- `Color Bar Display`
- `Camera Follow`
- `Camera Rotate`
- `Camera Move`
- `Process Bar ON/OFF`
- `Camera Info ON/OFF`
- `Party Change`
- `Message Test`
- `Menu Test`
- `SYSTEM DEBUG`
- `FUNCTIONS TEST`
- Map groups for diner, Phenac, Pyrite, Agate, The Under, Laboratory, Mt. Battle, Tower, etc.

Implementation leads:

- `common_rel.fdat` is a high-value target for debug menu string references.
- `tool_fight.fdat`, `fight.fdat`, `pda_menu.fdat`, `name_entry_menu.fdat`, and `pocket_menu` should be indexed together for cut systems.

### Bonus Discs

TCRF documents three bonus disc releases:

- US Bonus Disc: Colosseum preview, Jirachi movie trailer, WISHMKR Jirachi transfer to Ruby/Sapphire.
- Japanese Pokemon Colosseum Special Disc: Celebi and Pikachu content, plus Pokemon Channel demo.
- Pokemon Colosseum Nintendo Special Disc: Celebi/Pikachu content plus previews for multiple Nintendo GameCube and GBA games.

US Bonus Disc technical notes:

- The GBA multiboot file contains working FireRed/LeafGreen and Japanese-game support, but the check only permits non-Japanese Ruby/Sapphire by default.
- TCRF notes patch points `020134CC` and `020134D4` in the decompressed multiboot code for allowing Japanese games and FireRed/LeafGreen.
- Emerald support appears absent, likely because Emerald released later.
- The US disc contains a never-released `METEOR` Jirachi variant related to the Japanese Wishing Star Jirachi.
- `/tgc/pokedownload.tgc` contains Kirby Air Ride leftovers.
- TCRF TODOs mention `main.dol` crash debugger-like text and `debug_level_0.ini`.

Bonus-disc content left in the standard game:

- Japanese bonus-disc content remains in the full game.
- US standard game has mostly complete text in `system_tool.fdat`, while graphics remain Japanese.
- European versions appear corrupted/incomplete for this content.
- Four `ex_` rooms exist: `ex_title`, `ex_top`, `ex_coupon`, `ex_shrine`.

Bonus-disc Action Replay entry codes:

USA:

```text
5M54-WC2B-QHBG6
WNR8-RY3Q-29P6T
GMNK-UQNE-QR78H
WHZ0-EWPJ-12BTD
NRTU-8KHF-1PZZ8
DEED-TK1H-U6N5P
BHDF-N90M-BKMPV
```

Europe:

```text
3V7P-NZA1-ZBNVQ
WNR8-RY3Q-29P6T
GMNK-UQNE-QR78H
WHZ0-EWPJ-12BTD
NRTU-8KHF-1PZZ8
F57Q-1RNH-ZBAKF
CNU4-TJFW-TR1W0
```

Japan:

```text
RDNQ-FMT8-Q5K58
WNR8-RY3Q-29P6T
GMNK-UQNE-QR78H
WHZ0-EWPJ-12BTD
NRTU-8KHF-1PZZ8
72FZ-KBY7-AEERM
9P78-U2P1-Y2ZHN
```

Bonus content behavior:

- `ex_top`: menu hub for Relic Stone/Celebi, Mt. Battle coupon clerk, or title.
- `ex_coupon`: checks Poke Coupon rank. TCRF records Bronze 2500 -> PP Max, Silver 5000 -> event Pikachu with Light Ball text bug, Gold 30000 -> Master Ball.
- `ex_shrine`: after all normally available Story Mode Shadow Pokemon are snagged and purified, Celebi can be received. A connected GBA save that has reached the Hall of Fame and has not already received Celebi can receive additional Celebi, up to the documented limit.

Local validation notes:

- The local `GC6E01` main-game ISO contains `ex_title.fsys`, `ex_top.fsys`, `ex_coupon.fsys`, `ex_shrine.fsys`, `coupon.bin`, `bg0thumbcode.bin`, `bg1thumbcode.bin`, `bg2thumbcode.bin`, `pkx_jirachi.fsys`, `pkx_rare_jirachi.fsys`, `wzx_relive_celebi_appear.fsys`, and `wzx_relive_celebi_trail.fsys`.
- No local US Bonus Disc image, `.tgc`, or `/tgc/pokedownload.tgc` payload was found in the workspace search, so the authentic WISHMKR/METEOR Jirachi distribution path is not locally validated.
- Live Dolphin floor-state warps validated `ex_title`/entry `0x0320`, hub `0x0321`, coupon menu `0x0322`, and Celebi shrine `0x0323`. Screenshots and exact RAM notes are archived in [dolphin_ar_validation.md](dolphin_ar_validation.md).
- The native `ex_shrine` gift builder uses Celebi species `0x00FB` at runtime address `0x801306B4` (`388000FB`).
- The native `ex_coupon` gift builder uses Pikachu species `0x0019` at runtime address `0x801307C4` (`38800019`).
- For testing both Celebi and Jirachi from the dormant bonus-disc flow, keep the shrine instruction as Celebi and patch the coupon gift instruction to Jirachi species `0x0181` (`38800181`). This produces an experimental Jirachi reward path, not a faithful US Bonus Disc WISHMKR transfer.

Implementation leads:

- Treat `ex_` rooms as dormant feature modules.
- The reward logic is useful for understanding coupon checks, GBA connection checks, and event Pokemon transfer code.
- Preserve the distinction between the Japanese bonus-disc leftovers in the standard game and the separate US Bonus Disc multiboot Jirachi distribution when implementing or documenting restoration work.

### e-Reader Leftovers

Card e Room:

- In Japan, the Card e Room unlocks after Story Mode completion and is accessed through the left door in Phenac Stadium.
- International versions route both doors to the battle area, leaving Card e Room inaccessible normally.
- The room remains visible in the title-screen demo.
- North American builds still allow Japanese e-Reader cards/hardware to function when the room is accessed with cheats.
- European builds retain files, but English text is empty and connection support appears nonfunctional without further modification.
- The menu song `virtual` remains in the international versions.
- Phenac Stadium is internally designated as underwater; because e-Reader Shadow Pokemon appear there, this is the only path that exercises Dive Ball's underwater catch bonus in Colosseum.

Card e Room Action Replay warp codes:

USA:

```text
ED1R-7K1B-6MH0T
6V4G-BR63-1VBAG
GMNK-UQNE-QR78H
ZDXJ-VZFY-NTNQX
NRTU-8KHF-1PZZ8
DEED-TK1H-U6N5P
BHDF-N90M-BKMPV
```

Europe:

```text
YY7R-UN4A-YYT8M
6V4G-BR63-1VBAG
GMNK-UQNE-QR78H
ZDXJ-VZFY-NTNQX
NRTU-8KHF-1PZZ8
F57Q-1RNH-ZBAKF
CNU4-TJFW-TR1W0
```

Modes:

- Panel Battle: scan Trainer cards; defeated trainers fill panels and cannot be re-challenged in that mode. Difficulties are Easy, Normal, Difficult. Completing rows/panels can spawn bonus VR Trainers. Defeating all 20 VR Trainers at one difficulty unlocks a Shadow Pokemon trainer.
- Endless Battle: unlocks after top Panel Battle trainers are defeated. It is always Difficult difficulty, does not heal between fights, scans cards before battles, and inserts bonus stages every five battles. Poke Coupon rewards stop after 30 consecutive wins until the streak resets, though interval messages continue.

Dummy card/trainer data:

- `carde_fsys` has developer dummy card data named like `cardXX.bin`.
- Dummy cards appear under a `Genius Sonority` set once decoded/adjusted for e-Reader use.
- Dummy trainer names/classes do not consistently match across difficulties.
- Some dummy Pokemon have random EV/IV data, items, or impossible moves.
- Shadow trainer data is stored across `Cards01-20.bin` and uses Shadow IDs corresponding to Togepi, Mareep, and Scizor.

Implementation leads:

- The e-Reader system is a clean target for restoring optional battle content.
- Card parsing, trainer data, and save slots should be documented separately before attempting behavior restoration.
- PAL needs text-file and connection-path investigation before being treated as equivalent to US.

## Practical Research Backlog

High-value next steps for this repo:

1. Index `common_rel.fdat` references for debug strings and Shadow list names.
2. Map the Action Replay debug patch locations back to code addresses in the decomp.
3. Find and label `waza_viewer`, `ex_*`, `carde_*`, and `pocket_menu` assets/symbols in the extracted data.
4. Build a Shadow Pokemon table with final, debug, e-Reader, and unused postgame slots side by side.
5. Verify save flag ranges `1262-1357` and `2126-2221` against local save structures.
6. Trace trainer animation tables and identify slots `3`, `5`, `7`, `8`, and `9`.
7. Preserve regional asset gates for Rui, Somek, title, intro, banner, logo, and credits music.
8. Keep raw dialogue out of repo unless needed for a legally safe patching workflow; store pointers and offsets instead.

## Safety Notes

- Use these codes only against legally obtained copies/dumps and backed-up saves.
- Action Replay paths can crash or corrupt state, especially PAL bonus/e-Reader paths described as incomplete.
- Treat TCRF restoration inferences as evidence-backed leads, not finished implementation specs, until confirmed against local data and executable behavior.
