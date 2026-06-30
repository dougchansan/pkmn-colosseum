# Pokemon Colosseum Struct Size Catalog

Auto-generated from `include/**/*.h` typedef struct definitions.
Run `python3 tools/gen_struct_catalog.py` to refresh after header changes.

For structs with `/* 0xNN */` offset comments, **Last offset** is the highest documented field offset (a floor on struct size — trailing padding/undocumented fields can push real size higher).

Inspired by [zeldaret/tp `docs/re_notes.md`](https://github.com/zeldaret/tp/blob/main/docs/re_notes.md).

## Index

| Struct | Header | Last offset | Fields |
|--------|--------|-------------|--------|
| `DVDDiskID` | `include/dolphin/dvd/dvd.h` | — | 7 |
| `DVDDriveInfo` | `include/dolphin/dvd/dvd.h` | — | 4 |
| `DVDFileInfo` | `include/dolphin/dvd/dvd.h` | — | 4 |
| `GXFifoObj` | `include/dolphin/gx/GX.h` | — | 10 |
| `GXRenderModeObj` | `include/dolphin/gx/GX.h` | — | 13 |
| `GXTexRegion` | `include/dolphin/gx/GX.h` | — | 1 |
| `GXTlutRegion` | `include/dolphin/gx/GX.h` | — | 1 |
| `OSBootInfo` | `include/dolphin/os/OS.h` | 0x3C | 9 |
| `OSContext` | `include/dolphin/os/OSContext.h` | 0x1C8 | 14 |
| `OSResetFunctionInfo` | `include/dolphin/os/OSReset.h` | — | 4 |
| `PADStatus` | `include/dolphin/pad/Pad.h` | — | 11 |
| `BattleCameraState` | `include/game/battle/battle.h` | 0x1C | 10 |
| `BattleGridSlot` | `include/game/battle/battle.h` | 0x7 | 5 |
| `BattlePokemon` | `include/game/battle/battle.h` | 0x58 | 34 |
| `MoveData` | `include/game/battle/battle.h` | — | 11 |
| `StatStages` | `include/game/battle/battle.h` | — | 8 |
| `TurnAction` | `include/game/battle/battle.h` | — | 6 |
| `TypeMatchup` | `include/game/battle/battle.h` | — | 3 |
| `CommonMoveData` | `include/game/data/common_rel.h` | 0x34 | 29 |
| `CommonNatureData` | `include/game/data/common_rel.h` | 0x18 | 13 |
| `CommonRelData` | `include/game/data/common_rel.h` | — | 2 |
| `CommonTrainerData` | `include/game/data/common_rel.h` | 0x30 | 15 |
| `CommonTrainerPokemon` | `include/game/data/common_rel.h` | 0x4E | 39 |
| `PokemonStats` | `include/game/data/common_rel.h` | 0x10A | 49 |
| `GSEffectGlobals` | `include/game/effect/gs_effect.h` | 0x14 | 7 |
| `GSEffectInstance` | `include/game/effect/gs_effect.h` | 0x30 | 15 |
| `TraceFXWork` | `include/game/effect/gs_effect.h` | 0xA8 | 25 |
| `DVDPoolEntry` | `include/game/fsys/fsys.h` | 0x1C | 8 |
| `DecompPoolEntry` | `include/game/fsys/fsys.h` | 0xC | 4 |
| `FSYSDecompContext` | `include/game/fsys/fsys.h` | 0xC | 4 |
| `FSYSFileEntry` | `include/game/fsys/fsys.h` | 0x28 | 11 |
| `FSYSFileHandle` | `include/game/fsys/fsys.h` | 0x4 | 2 |
| `FSYSManager` | `include/game/fsys/fsys.h` | 0x28 | 11 |
| `FSYSSlot` | `include/game/fsys/fsys.h` | 0x13C | 33 |
| `FSYSSubEntry` | `include/game/fsys/fsys.h` | 0x8 | 3 |
| `ConvertedPokemon` | `include/game/gba/gba.h` | 0x10 | 5 |
| `GBAPokemonData` | `include/game/gba/gba.h` | 0x14 | 5 |
| `GSColMeshHeader` | `include/game/gs_colsys.h` | 0x4 | 2 |
| `GSColSubMesh` | `include/game/gs_colsys.h` | 0xC | 4 |
| `GSColSurfaceType` | `include/game/gs_colsys.h` | 0x8 | 5 |
| `GSColSysState` | `include/game/gs_colsys.h` | 0x370C | 5 |
| `GSColTriState` | `include/game/gs_colsys.h` | 0x26 | 3 |
| `GSColTriangle` | `include/game/gs_colsys.h` | 0x3E | 9 |
| `Mtx33f` | `include/game/gs_colsys.h` | — | 1 |
| `Vec3f` | `include/game/gs_colsys.h` | — | 1 |
| `GSDVDEntry` | `include/game/gs_dvd.h` | 0x14 | 7 |
| `GSDVDHandle` | `include/game/gs_dvd.h` | 0x8 | 4 |
| `GSflagConfig` | `include/game/gs_flag.h` | 0x4 | 2 |
| `GSflagDef` | `include/game/gs_flag.h` | 0x6 | 6 |
| `GSflagSceneEntry` | `include/game/gs_flag.h` | 0x14 | 9 |
| `GSflagState` | `include/game/gs_flag.h` | 0x1C | 8 |
| `GSFloorContext` | `include/game/gs_floor.h` | 0x10 | 7 |
| `GSFloorDataEntry` | `include/game/gs_floor.h` | 0x44 | 3 |
| `GSFloorResHandler` | `include/game/gs_floor.h` | 0xC | 5 |
| `GSFloorResource` | `include/game/gs_floor.h` | 0x22 | 13 |
| `GSgfxState` | `include/game/gs_gfx.h` | 0x4A0 | 31 |
| `GSmaterialEntry` | `include/game/gs_material.h` | 0x16C | 24 |
| `GSmemBlock` | `include/game/gs_mem.h` | 0x8 | 3 |
| `GSmemEntry` | `include/game/gs_mem.h` | 0xE | 6 |
| `GSModelSlot` | `include/game/gs_model.h` | 0x44 | 19 |
| `GSSceneObject` | `include/game/gs_scene.h` | 0x50 | 12 |
| `GSSceneRenderEntry` | `include/game/gs_scene.h` | 0x8 | 3 |
| `GStextureHandle` | `include/game/gs_texture.h` | 0x74 | 23 |
| `GSTask` | `include/game/gs_thread.h` | 0x14 | 8 |
| `GSThread` | `include/game/gs_thread.h` | 0x22 | 16 |
| `PADInputState` | `include/game/input/input.h` | 0x19 | 19 |
| `MenuBattleWork` | `include/game/menu/menu.h` | 0x34 | 12 |
| `MenuCallback` | `include/game/menu/menu.h` | 0xC | 4 |
| `MenuCardEMatrixWork` | `include/game/menu/menu.h` | 0x24 | 10 |
| `MenuCardEWork` | `include/game/menu/menu.h` | 0x18 | 7 |
| `MenuCommonWork` | `include/game/menu/menu.h` | 0xAC | 13 |
| `MenuPokeCouponWork` | `include/game/menu/menu.h` | 0x1C | 8 |
| `MenuStackEntry` | `include/game/menu/menu.h` | 0x4 | 2 |
| `PeopleEntry` | `include/game/people/people.h` | 0xBC | 48 |
| `PeopleOpenWork` | `include/game/people/people.h` | 0x2C | 12 |
| `PeopleFieldEntry` | `include/game/people/people_field.h` | 0x24 | 16 |
| `PeopleFieldWork` | `include/game/people/people_field.h` | 0x211 | 51 |
| `CardEMatrixEntry` | `include/game/save/save.h` | 0xD | 4 |
| `CardESeries` | `include/game/save/save.h` | 0x26 | 12 |
| `SaveHeader` | `include/game/save/save.h` | 0x24 | 6 |
| `SaveItem` | `include/game/save/save.h` | 0x2 | 2 |
| `SavePokemon` | `include/game/save/save.h` | 0xFC | 41 |
| `PSParticle` | `include/game/script/script.h` | 0x90 | 73 |
| `PurifyRates` | `include/game/shadow/shadow.h` | 0x4 | 5 |
| `ShadowPokemonData` | `include/game/shadow/shadow.h` | 0x16 | 12 |
| `ShadowRegistration` | `include/game/shadow/shadow.h` | 0xE | 8 |
| `ShadowSystem` | `include/game/shadow/shadow.h` | — | 6 |
| `SndListener` | `include/game/sound/sound.h` | 0x28 | 10 |
| `SndResData` | `include/game/sound/sound.h` | 0x8 | 7 |
| `SndVec` | `include/game/sound/sound.h` | — | 1 |
| `SndWork` | `include/game/sound/sound.h` | 0x8 | 7 |
| `GSFieldCamera` | `include/game/world/gs_field.h` | 0x4C | 12 |
| `GSFieldHitResult` | `include/game/world/gs_field.h` | 0xA | 6 |
| `HSD_Class` | `include/hsd/hsd_class.h` | — | 1 |
| `HSD_ClassInfo` | `include/hsd/hsd_class.h` | — | 6 |
| `HSD_ClassInfoHead` | `include/hsd/hsd_class.h` | — | 12 |
| `HSD_FreeList` | `include/hsd/hsd_class.h` | — | 1 |
| `HSD_MemoryEntry` | `include/hsd/hsd_class.h` | — | 5 |
| `HSD_RectF32` | `include/hsd/hsd_cobj.h` | — | 4 |
| `HSD_RectS16` | `include/hsd/hsd_cobj.h` | — | 4 |
| `Scissor` | `include/hsd/hsd_cobj.h` | — | 4 |
| `HSD_FObj` | `include/hsd/hsd_fobj.h` | — | 18 |
| `HSD_FObjDesc` | `include/hsd/hsd_fobj.h` | — | 8 |
| `HSD_SList` | `include/hsd/hsd_forward.h` | — | 2 |
| `HSD_JObjInfo` | `include/hsd/hsd_jobj.h` | — | 6 |
| `HSD_MObjDesc` | `include/hsd/hsd_mobj.h` | — | 6 |
| `HSD_MatAnim` | `include/hsd/hsd_mobj.h` | — | 4 |
| `HSD_ObjInfo` | `include/hsd/hsd_object.h` | — | 1 |
| `HSD_TObjDesc` | `include/hsd/hsd_tobj.h` | — | 24 |
| `HSD_TObjInfo` | `include/hsd/hsd_tobj.h` | — | 4 |
| `HSD_TexAnim` | `include/hsd/hsd_tobj.h` | — | 7 |
| `DBCommTable` | `include/trk/trk.h` | 0x24 | 10 |
| `TRKBuffer` | `include/trk/trk.h` | 0x10 | 5 |
| `TRKEvent` | `include/trk/trk.h` | 0x8 | 3 |

## How to use

1. When recovering a typed signature for a function, grep this index for the struct hosting the field offset you see in asm.
2. The canonical header always wins; this file is just an index.
3. When you decompile or expand a struct, document field offsets with `/* 0xNN */` comments before each member in the header. They feed both this catalog and reader comprehension.
