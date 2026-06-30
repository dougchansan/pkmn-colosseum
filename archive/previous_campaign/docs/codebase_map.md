# Pokemon Colosseum (GC6E01) Codebase Map

## Overview

- **Engine**: Genius Sonority custom engine (GS* prefix)
- **Graphics Middleware**: HAL SysDolphin (HSD_* prefix) - same lineage as Super Smash Bros. Melee
- **SDK**: Nintendo GameCube SDK (OS*, GX*, DVD*, PAD*, VI*, SI*, EXI*)
- **Runtime**: Metrowerks CodeWarrior for GCN
- **Text section**: `0x800055E0` - `0x80260000` (~2MB, 8589 functions)
- **Named functions**: 274 (SDK/library symbols)
- **Unnamed functions**: 8315 (game code + engine, all `fn_*`)
- **Functions identified this pass**: 122 proposed names

---

## .text Section Layout

### 0x800055E0 - 0x80005C3C: Game Entry / Main Loop
| Address | Proposed Name | Size | Source |
|---------|---------------|------|--------|
| 0x800055E0 | `main` | 0xE4 | (named) |
| 0x800056C4 | `GS_MainLoopInit` | 0x10 | - |
| 0x800056D4 | `GS_MainLoopUpdate` | 0x10 | - |
| 0x800056E4 | `GS_MainLoopReturnZero` | 0x8 | - |
| 0x800056EC | `GS_SystemInit` | 0x5C | - |
| 0x80005748 | `GS_SystemShutdown` | 0x58 | - |
| 0x800057B0 | `GS_InitSubsystems` | 0x2FC | - |

### 0x80005C3C - 0x80034280: Early Game Systems (~323 functions before menu)
Includes flag system setup, early initialization, various game utilities.

### 0x80033278 - 0x80034280: Card-E Menu System
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80033278 | `menuCardE_Main` | 0x1008 | menuCardE.c |

String reference: `"_CARDE.card_type == CARDE_CARDTYPE_TRAINER"` (assert)

### 0x80034280 - 0x80059BDC: Game Utilities (~323 functions)
Unidentified region. Likely contains game state management, save/load helpers, and general utilities.

### 0x80059BDC - 0x8006A000: UI Core (~103 functions)
Contains `fn_80059BDC` (size 0x30F4 - very large, likely a UI state machine).

### 0x80069A60 - 0x80069C0C: Battle Menu Callbacks
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80069A60 | `menuCB_Battle_Assert` | 0x1AC | menuCB_Battle.c |

String: `"FIGHT_ENCOUNTER_DATA_null != null"`, `"0 <= eRuleType && _LENGTH(_CB.m_aRule) > eRuleType"`

### 0x80069C0C - 0x8007109C: Menu System Middle (~100 functions)
Additional menu-related code between Battle and Common callbacks.

### 0x8007109C - 0x8007162C: Common Menu Callbacks
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8007109C | `menuCB_Common_Assert1` | 0x68 | menuCB_Common.c |
| 0x80071104 | `menuCB_Common_Assert2` | 0x5C | menuCB_Common.c |
| 0x80071398 | `menuCB_Common_MenuPush` | 0x130 | menuCB_Common.c |
| 0x800714C8 | `menuCB_Common_MenuPop` | 0xF4 | menuCB_Common.c |
| 0x800715BC | `menuCB_Common_Assert3` | 0x70 | menuCB_Common.c |

Key strings: `"_menuPop():stack under."`, `"_menuPush(int eMenuID):stack over."`

### 0x8007581C - 0x80075A34: Battle Tool Menu
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8007581C | `menuToolBattle_Assert` | 0x218 | menuToolBattle.c |

String: `"BATTLEMODE_BATTLEYAMA100 == _CB.m_eBattleMode"`

### 0x800767B8 - 0x80077A5C: Rule Menu Callbacks
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x800767B8 | `menuCB_Rule_Handler1` | 0x2D4 | menuCB_Rule.c |
| 0x800772AC | `menuCB_Rule_Handler2` | 0x228 | menuCB_Rule.c |
| 0x800774D4 | `menuCB_Rule_Handler3` | 0x210 | menuCB_Rule.c |
| 0x800776E4 | `menuCB_Rule_Handler4` | 0x378 | menuCB_Rule.c |

### 0x80077ED4 - 0x80078390: Extra Disc Shrine Menu
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80077ED4 | `menuExDiscShrine_Main` | 0x4BC | menuExDiscShrine.c |

### 0x800792D8 - 0x800798E8: Extra Disc Coupon Menu
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x800792D8 | `menuExDiscCoupon_Main` | 0x610 | menuExDiscCoupon.c |

References GBA thumb code binaries: `bg0thumbcode.bin`, `bg1thumbcode.bin`, `bg2thumbcode.bin`

### 0x8007C2C0 - 0x8007C300: Poke Coupon Menu
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8007C2C0 | `menuPokeCoupon_Assert` | 0x40 | menuPokeCoupon.c |

String: `"POKECOUPONREFER_INVALID != _menuPokeCouponWork.m_eRefer"`

### 0x8007C300 - 0x8007FD64: Card-E Matrix Menu
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8007C300 | `menuCardE_Matrix_Init` | 0x114 | menuCardE_Matrix.c |
| 0x8007C450 | `menuCardE_Matrix_Assert1` | 0x1E4 | menuCardE_Matrix.c |
| 0x8007C7EC | `menuCardE_Matrix_Assert2` | 0x2C4 | menuCardE_Matrix.c |
| 0x8007D978 | `menuCardE_Matrix_Main` | 0x23EC | menuCardE_Matrix.c |

Strings: `"i < cem->m_seriesN"`, `"!cem->m_isAnimating"`, `"s[ANIM_cur]"`

### 0x80082650 - 0x80083AF4: Card-E Save Data (11 functions)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80082650 | `cardesavedata_ValidateLevel` | 0xE8 | cardesavedata.c |
| 0x80082738 | `cardesavedata_CheckSeries` | 0x228 | cardesavedata.c |
| 0x80082960-0x80083AF4 | `cardesavedata_Func3..11` | varies | cardesavedata.c |

### 0x80089048 - 0x800895A4: Pokemon Conversion (2 functions)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80089048 | `pokeconv_ConvertPokemon` | 0x338 | pokeconv.c |
| 0x80089380 | `pokeconv_ValidateItems` | 0x224 | pokeconv.c |

String: `"cp->pc_items_num == 50 || cp->pc_items_num == 30"` (GBA PC item count validation)

### 0x80092C90 - 0x800937F4: GBA Communication (9 functions)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80092C90 | `gbaCommunication_Transfer1` | 0x1A8 | gbaCommunication.c |
| 0x80092E38-0x800937F4 | `gbaCommunication_Transfer2..6` | varies | gbaCommunication.c |

String: `"Ribbon Index: %d"` (GBA ribbon data transfer)

---

## SDK / Library Code (0x80097FFC - 0x800D0F68)

### 0x80097FFC - 0x800A2B9C: PPC / OS (226 functions)
Named SDK functions: `PPCMfmsr`, `EXIImm`, `OSInit`, `OSReport`, etc.

### 0x800A2B9C - 0x800A2C30: CW Runtime Init (3 functions)
`__init_user`, `__init_cpp`, `_ExitProcess`

### 0x800A2C30 - 0x800AA430: DVD / Filesystem (153 functions)
Named: `DVDInit`, `DVDLowReset`, `__DVDFSInit`, `stateReady`, `stateBusy`, etc.

### 0x800AA430 - 0x800B5E8C: Video / PAD (163 functions)
Named: `VIGetTvFormat` + unnamed VI/PAD functions.

### 0x800B5E8C - 0x800BE348: GX Graphics API (168 functions)
Named: `GXInit`, `__GXInitGX`, `GXInitFifoBase`, `GXSetCPUFifo`, `GXSetGPFifo`, etc.

### 0x800BE348 - 0x800C44F8: MetroTRK Debugger (157 functions)
Named: `TRKNubMainLoop`, `TRK_main`, `InitMetroTRK`, `TRKTargetContinue`, etc.
String: `"MetroTRK for GAMECUBE v2.6"`

### 0x800C44F8 - 0x800CE7C4: CW Standard Library (122 functions)
Named: `__va_arg`, `__save_fpr`, `__restore_gpr`, `exit`, `fwrite`, `strlen`, `memchr`, etc.

### 0x800CE7C4 - 0x800D0F68: SI / Peripherals (21 functions)
Named: `SIInterruptHandler`, `SIInit`, `SITransfer`, `SIGetType`, etc.

---

## Genius Sonority Engine (0x800D39E0 - 0x80190E34)

### 0x800D39E0 - 0x800D3E4C: GSgfx - Graphics Init
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800D39E0 | `GSgfx_Init` | 0x46C |

Strings: `"GSgfx: unable to allocate gsgfx state!"`, `"GSgfx: Init OK, state located at %08Xh (size=%d)"`

### 0x800D3E4C - 0x800E202C: GSrender -- Extended rendering pipeline (278 functions)
**Source: gs_render.c / gs_render.h** (NEW -- Gap 3 decompilation)

Sub-modules within this range:
- **GSgfx callbacks** (0x800D3E4C-0x800D4610): VBlank, retrace, draw-done
- **GSlog** (0x800D461C-0x800D5504): Debug printf with hex formatting
- **GSgfx matrix** (0x800D5504-0x800D7900): ~80 matrix stack operations
- **GSgfx viewport** (0x800D7900-0x800D9F40): Viewport, projection, scissor
- **GSgfx blend/TEV** (0x800D9F40-0x800DA2BC): Blend, TEV, fog config
- **GSgfx lighting** (0x800DB890-0x800DC224): Light setup and commands
- **GSmaterial** (0x800DC224-0x800DE680): HSD MObj-based material system
- **GSgfx draw** (0x800DE680-0x800E202C): Draw command dispatch

| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x800D3E4C | `GSgfx_VBlankCallback` | 0x78 | State save/restore (0x5A0 bytes) |
| 0x800D3EC4 | `GSgfx_PreRetraceCallback` | 0x8C | 6 float params for viewport |
| 0x800D3F50 | `GSgfx_DrawDoneCallback` | 0xC | Sets flag at lbl_8047AA91 |
| 0x800D3FA4 | `GSgfx_BeginFrame` | 0x654 | Full frame setup |
| 0x800D461C | `GSlog_PrintFormatted` | 0x97C | Varargs printf |
| 0x800D4F98 | `GSlog_QueueCommand` | 0x56C | Batch rendering commands |
| 0x800D892C | `GSgfx_ConfigurePipeline` | 0x910 | Full GX pipeline config |
| 0x800DE680 | `GSmaterial_Create` | 0x948 | Material setup + envmap |
| 0x800E1544 | `GSgfx_DrawDispatch` | 0xAE8 | Main draw dispatch |

Strings: `"GSgfx: invalid matrix index"`, `"GSgfx: matrix stack underflow!"`, `"GSgfx: matrix stack overflow!"`, `"0123456789ABCDEF"`, `"GSmaterialSetPEdescr: Warning: already using a custom description!"`, `"GSmaterialCreate: Run out of materials..."`, `"GSmaterial MObj"`, `"GSmaterial: Unsupported texture format for environment map!"`, `"GSmaterial: Error creating environment map: no texture defined!"`

### 0x800E2DB0 - 0x800E3604: GSmem - Memory Management
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800E2DB0 | `GSmem_AllocateBlock` | 0x784 |
| 0x800E3568 | `GSmem_Init` | 0x9C |

Strings: `"GSmem: Warning -- memory loss of %d bytes..."`, `"GSmem: Init OK, using area %08Xh -> %08Xh"`

### 0x800E3604 - 0x800EE2C8: GSmaterial System (~180+ functions)
Strings: `"GSmaterialSetPEdescr: Warning: already using a custom description!"`,
`"GSmaterialCreate: Run out of materials. Increase materialcount at initialisation"`

### 0x800EE2C8 - 0x800EF5FC: GSpart - Model Part System
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800EE2C8 | `GSpart_RegisterRotation` | 0xF4 |

Strings: `"GSpart: part is already registered for rotation"`, `"GSpart: model has too many registered rotations"`, `"GSpart: child node too deep"`

### 0x800EF5FC - 0x800F07A8: GStexture - Texture System
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800EF5FC | `GStexture_Create` | 0x718 |

Strings: `"GStexture: invalid texture format"`, `"GStexture: warning -- texture size adjusted from [%d,%d] to [%d,%d]"`

### 0x800F07A8 - 0x800F09D8: GSthread - Thread System
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800F07A8 | `GSthread_Init` | 0x230 |

Strings: `"GSthreadCreate. Warning: 'usesFPU==FALE' OK?"`, `"GSthread: Init OK, maximum of %d threads"`, `"Stack overflow."`

### 0x800FF788 - 0x80101910: GSfloor - Floor/World System (~199 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x800FF788 | `GSfloor_Open` | 0x94 |
| 0x80101244 | `GSfloor_LoadParticle` | 0xA4 |
| 0x801012E8 | `GSfloor_FindAndOpen` | 0xB8 |
| 0x801013A0 | `GSfloor_LoadData` | 0xDC |
| 0x8010147C | `GSfloor_LoadMain` | 0x494 |

Strings: `"GSfloorOpen: cannot find floor %d"`, `"loadParticle(): loading..."`

### 0x80101910 - 0x8010C220: GSmodel -- 3D Model Management (145 functions)
**Source: gs_model.c / gs_model.h** (NEW -- Gap 2 decompilation)

| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x80101910 | `GSmodel_FindLoadedResource` | 0xE8 | Unrolled search (bdnz, 3 slots/iter) |
| 0x801019F8 | `GSmodel_ClearResourceTable` | 0x30 | memset(table, 0, 0x2400) |
| 0x801028B0 | `GSmodel_AttachAnimation` | 0x624 | Joint hierarchy + playback init |
| 0x80102F38 | `GSmodel_UpdateAnimation` | 0x54C | Per-frame anim advance |
| 0x801074D4 | `GSmodel_RenderModel` | 0x9A4 | Full GX render dispatch |
| 0x80108580 | `GSmodel_BuildDisplayList` | 0x694 | GX display list cache |
| 0x80108C14 | `GSmodel_ProcessSkinning` | 0x5E0 | Skeletal vertex skinning |

BSS: Model resource table at lbl_80402518 (0x2400 bytes, 0x48 per slot, ~0x60 slots).
No debug strings in this range (silent module, errors via GSgfx/GSmem paths).

### 0x8010C220 - 0x8010E138: GScolsys2 -- Collision System
**Source: gs_colsys.c** (existing)

| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x8010D3C8 | `GScolsys2_Draw` | 0x50C |

String: `"GScolsys2Draw : can't alloc display list memory."`

### 0x8010E138 - 0x80130CE0: GSfield -- Field/World System (824 functions, 142KB)
**Source: gs_field_colquery.c, gs_field_resource.c, gs_field_world.c / gs_field.h** (NEW -- Gap 1 decompilation, PRIORITY)

Three sub-modules:

**gs_field_colquery.c** (0x8010E138-0x80114300): Collision queries
| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x8010E138 | `GSfield_RayCast` | 0x404 | Ray vs collision mesh |
| 0x8010E53C | `GSfield_SphereSweep` | 0x5EC | Swept sphere test |
| 0x8010EFE4 | `GSfield_LinePlaneTest` | 0x1A4 | Segment-plane test |
| 0x8010F71C | `GSfield_FindGroundHeight` | 0x338 | Vertical ray cast |
| 0x801101B4 | `GSfield_BuildCollisionGrid` | 0x4E8 | Spatial acceleration |
| 0x8011069C | `GSfield_GridLookup` | 0x7C8 | Grid point query |
| 0x801123D4 | `GSfield_ResourceInit` | 0x32C | Resource slot setup |
| 0x801129CC | `GSfield_UpdateObjects` | 0x5C0 | Per-frame object update |

**gs_field_resource.c** (0x8011432C-0x80114CA8): Floor resource pre-funcs
| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x8011432C | `floorReadGFLPreFunc` | 0x74 | Geometry alloc (32-byte align) |
| 0x801143A8 | `floorReadSoundPreFunc_CheckBuffer` | 0x44 | Buffer overflow check |
| 0x801144D0 | `floorReadSoundPreFunc` | 0xF0 | Sound buffer alloc |
| 0x80114760 | `floorReadParticlePreFunc` | 0x74 | Particle alloc |

Strings: `"floorReadGFLPreFunc(): can't alloc..."`, `"ERROR: Over Sound Buffer!"`, `"floorReadParticlePreFunc(): can't alloc..."`, `"floorReadWZXPreFunc()"`, `"floorReadPKXPreFunc()"`, `"floorReadTexPreFunc()"`, `"floorReadCameraPreFunc"`, `"floorReadMapPreFunc"`, `"floorReadScriptPreFunc()"`, `"floorReadFontPreFunc()"`, `"floorReadMsgPreFunc()"`, `"floorReadNormalPreFunc()"`

**gs_field_world.c** (0x80114CA8-0x80130CE0): Field camera, world logic
| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x80117514 | `floorUpdateFieldCamera` | 0x1B4 | Camera interpolation |
| 0x801254B4 | `GSfield_RenderPass` | 0xF58 | Full field render |
| 0x8012640C | `GSfield_TransitionStateMachine` | 0x1550 | Floor transition (LARGEST) |
| 0x80124A60 | `GSfield_ObjectBatchUpdate` | 0x7D8 | Batch object update |
| 0x8012CA84 | `GSfield_ProcessTriggers` | 0x838 | Event/warp triggers |

~600 functions total. Many small accessors (0x18-0x34 bytes) read struct fields at fixed offsets.
String: `"floorUpdateFieldCamera: error updating field camera - divide by zero!"`

### 0x80130000 - 0x8014A000: GSeffect / VFX System (~446 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x8013111C | `GSeffect_TriggerEffect` | 0xE4 |

Strings: `"GSeffect: Cannot trigger effect - instance uninitialised: effect ID %d."`, `"tracefxStartEffect: Could not start trail effect!"`, `"leaffxStartEffect: Could not start leaf effect"`, `"electronStartEffect"`, `"filterStart"`, `"surfEffectStart"`, `"seaEffectStart"`, `"envMapEffectInit"`, `"envMapEffectStart"`, `"blurEffectStart"`, `"auraEffectStart"`, `"distortionEffectStart"`, `"billboardEffectStart"`, `"Failed to create Patchiru texture"` (Jirachi!)

VFX sub-modules:
- **tracefx**: Trail/trace effects
- **leaffx**: Leaf particle effects
- **electron**: Electrical effects
- **filter**: Screen filter effects
- **surfEffect**: Water surface effects
- **seaEffect**: Ocean effects
- **envMap**: Environment mapping
- **blur**: Motion blur
- **aura**: Aura glow effects (Shadow Pokemon?)
- **distortion**: Screen distortion
- **billboard**: Billboard sprite effects

### 0x8014A000 - 0x801655D4: Pokemon Model / Animation / DVD (~341 functions)
Strings: `"%s/test%04d.thp"` (THP movie paths), `"GSmem state OK"`, `"GSmem state broken!"`, `"[GSDVD_ERROR_STATE_COVEROPEN_WAIT] status = %d"`

### 0x801655D4 - 0x80166000: Sound System (21 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x801655D4 | `sndWaveOpen` | 0x94 |
| 0x801657F8 | `sndStop` | 0x104 |
| 0x80165A44 | `sndCheckFileInfo` | 0x22C |
| 0x80165DEC | `sndWaveOpenByName` | 0xF4 |

Source: sound.c
Strings: `"ERROR: can't open WAVE ID = %d"`, `"ERROR: can't open WAVE File = %s"`, `"soundStop: Warning! BGM cannot be stopped.(snd_id=%d)"`, `"ERROR(sound.c): invalid file_info number snd_id=%d"`, `"ERROR: Can't Read Group(%d)"`, `"GSsndGetStatus:Forced termination SE=%d"`, `"GSsndGetStatus:Forced termination BGM=%d"`, `"_sndCheckSndWorkALL:Start"`, `"_sndCheckSndWorkALL:End"`

### 0x80167040 - 0x80168C64: GSDVD -- DVD/Disc I/O System (56 functions)
**Source: gs_dvd.c / gs_dvd.h** (NEW -- Gap 4 decompilation)

| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x80167040 | `GSDVD_CheckAndClose` | 0x30 | Check active + close |
| 0x80167070 | `GSDVD_CloseHandle` | 0xA8 | Free handle resources |
| 0x80167118 | `GSDVD_Open` | 0x180 | Open file (8 params) |
| 0x80167720 | `GSDVD_CheckActive` | 0x48 | Poll handle activity |
| 0x80167FA4 | `GSDVD_EmptyFunc` | 0x4 | Stub (blr) |
| 0x80167FA8 | `GSDVD_ErrorStateMachine` | 0x118 | Error recovery FSM |
| 0x80168284 | `_sndCheckSndWorkALL` | 0x184 | Validate sound buffers |
| 0x80168638 | `GSDVD_ErrorCoverOpenMain` | 0x2FC | Cover-open handler |
| 0x80168934 | `GSDVD_Init` | 0x330 | DVD subsystem init |

Handle table: lbl_80478FAC (0x0C bytes per handle, sda21).
Extended state: lbl_80478FB4 (0x18 bytes per entry, sda21).

Strings: `"_sndCheckSndWorkALL:Start"`, `"_sndCheckSndWorkALL:End"`, `"[GSDVD_ERROR_STATE_COVEROPEN_WAIT]  status = %d"`

### 0x80168C64 - 0x80168DAC: Script List System (2 functions)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x80168C64 | `pslist_LinkAssert` | 0x6C | pslist.c |
| 0x80168CD0 | `pslist_UnlinkAssert` | 0xDC | pslist.c |

### 0x8016F430 - 0x80172630: Script Interpreter (2 functions, one HUGE)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8016F430 | `psinterpret_ObjRefAssert` | 0xD0 | psinterpret.c |
| 0x8016F500 | `psinterpret_Main` | 0x3130 | psinterpret.c |

`psinterpret_Main` at 12,592 bytes is one of the largest functions in the binary -- this is the main script bytecode interpreter loop.

### 0x8017424C - 0x8017572C: Generator System (1 function)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x8017424C | `generator_Main` | 0x14E0 | generator.c |

String: `"psCamera"` -- generates camera scripts.

### 0x8017572C - 0x8017A5FC: GSscene -- Scene Object System (78 functions)
**Source: gs_scene.c / gs_scene.h** (NEW -- Gap 5 decompilation)

| Address | Proposed Name | Size | Notes |
|---------|---------------|------|-------|
| 0x8017572C | `GSscene_ProcessFreeList` | 0x1AC | Active/free list management |
| 0x80175B94 | `GSscene_SpawnObject` | 0x25C | Allocate + init object |
| 0x80176228 | `GSscene_DetachFromParent` | 0x3CC | Hierarchy management |
| 0x801766A8 | `GSscene_SetPosition` | 0xB0 | Set XYZ position |
| 0x80176C78 | `GSscene_Render` | 0x194 | Scene render dispatch |
| 0x8017707C | `GSscene_MainUpdate` | 0x378 | Per-frame scene update |
| 0x80177A64 | `GSscene_XFBCapture` | 0xBF8 | Framebuffer capture (LARGEST) |
| 0x80178AA8 | `GSscene_CameraUpdate` | 0x578 | Scene camera update |
| 0x80179020 | `GSscene_CameraInterpolate` | 0x3E4 | Camera interpolation |
| 0x80179FA4 | `GSscene_Init` | 0x658 | Scene system init |

Object lists (sda21): Active=lbl_8047B188, Free=lbl_8047B18C, Count=lbl_8047B118.
Camera state: lbl_80478C40. Scene render table: lbl_80478FB8.

String: `"gs%04d.xfb"` (numbered framebuffer capture for debug screenshots)

### 0x8017AC40 - 0x8017AF6C: FSYS Archive System
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x8017AC40 | `FSYS_LoadTOC` | 0x32C |

Strings: `"gsfsys.toc"`, `"s1_out.fsys"` (FSYS table of contents, first archive to load)

### 0x80181300 - 0x8018F200: People / NPC System (~227+ functions)
Massive NPC/People management system. The warning string `"Warining: people[%d,%d] group is different!!"` appears in 100+ locations across this range, indicating this is a large system handling NPC groups, spawning, and world population management.

### 0x8018FE30 - 0x80190E34: GSflag - Game Flag System (9 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x8018FE30 | `GSflagSet` | 0x4B0 |
| 0x801902E0 | `GSflagGet` | 0xD0 |
| 0x801903B0 | `GSflagSet16` | 0x178 |
| 0x80190528 | `GSflagSet32` | 0x178 |
| 0x801906A0 | `GSflagGet16` | 0xBC |
| 0x8019075C | `GSflagGet32` | 0x178 |
| 0x80190C90 | `GSflagSetBitValue` | 0x1A4 |

Strings: `"ERROR[GSflagSet]:Initialization has not finished."`, `"ERROR[GSflagSet]:Overflow BitValue FlagID=%d Value=%d (%d BitLength=%d)>(Limit=%dBit)."`, `"ERROR[GSflagGet]:Initialization has not finished."`

---

## HAL SysDolphin (HSD) Library (0x80190E34 - 0x801C0000)

This is a customized version of HAL Laboratory's SysDolphin library, also used in Super Smash Bros. Melee. The library string `"sysdolphin_base_library"` appears multiple times.

### HSD Modules
| Address | Proposed Name | Module | Source File |
|---------|---------------|--------|-------------|
| 0x801914F4 | `HSD_WObjInit` | had_wobj | - |
| 0x801938FC | `HSD_ClassInit` | hsd_class | - |
| 0x80193C24 | `HSD_CObjInit` | hsd_cobj | - |
| 0x80197A64 | `HSD_DObjDisplayFunc1` | displayfunc.c | displayfunc.c |
| 0x80197B6C | `HSD_DObjDisplayFunc2` | displayfunc.c | displayfunc.c |
| 0x80198F7C | `HSD_DObjInit` | hsd_dobj | - |
| 0x8019B7C0 | `HSD_FogAdjInit` | hsd_fogadj | - |
| 0x8019C690 | `HSD_InitAssert1` | initialize.c | initialize.c |
| 0x8019CE50 | `HSD_JObjInit` | hsd_jobj | - |
| 0x801A4000 | `HSD_LObjInit` | hsd_lobj | - |
| 0x801A69C0 | `HSD_MemoryAssert` | memory.c | memory.c |
| 0x801A6A34 | `HSD_MObjInit` | hsd_mobj | - |
| 0x801AA608 | `HSD_PObjInit` | hsd_pobj | - |
| 0x801BBAC8 | `HSD_TObjInit` | hsd_tobj | - |

HSD module list:
- **WObj**: World Object (animation targets)
- **CObj**: Camera Object
- **DObj**: Display Object (visible geometry)
- **JObj**: Joint Object (skeletal hierarchy)
- **LObj**: Light Object
- **MObj**: Material Object
- **PObj**: Primitive Object (mesh data)
- **TObj**: Texture Object
- **FogAdj**: Fog Adjustment
- **FObj**: Float Object (animation keyframes)
- **RObj**: Reference Object (constraints)

Additional HSD source files identified:
- `hsd_class` - Object-oriented class system
- `initialize.c` - HSD initialization with pixel format handling
- `displayfunc.c` - Billboard and render pass dispatch
- `memory.c` - HSD memory management (`"size == sizeof(__mem_cb)"`)
- `object.h` - Base object reference counting (`"HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF"`)

Strings: `"unkown type of billboard."`, `"unkown type of render pass."`, `"assertion \"%s\" failed"`, `"HSD_ArchiveParse: byte-order mismatch! Please check data format"`

### 0x801B019C - 0x801B1730: Shadow System (14 functions)
| Address | Proposed Name | Size | Source File |
|---------|---------------|------|-------------|
| 0x801B019C | `HSD_ShadowFunc1` | 0x204 | shadow.c |
| 0x801B0EB8 | `HSD_ShadowMain` | 0x66C | shadow.c |

References `shadow.h` header. Includes JObj quaternion checks: `"!(jobj->flags & JOBJ_USE_QUATERNION)"`.

---

## Battle System (0x801C3108 - 0x801F000C)

### 0x801C3108 - 0x801C53BC: Battle Grid (36 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x801C3430 | `battleGridSetup` | 0x634 |
| 0x801C3A64 | `battleGridLoadModels` | 0x11C |
| 0x801C3B80 | `battleGridUpdatePositions` | 0x118 |
| 0x801C3D64 | `battleGridReplacePokemon` | 0xD8 |
| 0x801C3F10 | `battleGridReplaceTrainer` | 0xAC |

Strings: `"battleGridReplacePokemon: NULL argument"`, `"battleGridReplaceTrainer: NULL argument!"`

The "battle grid" manages the 3D battle scene layout: Pokemon positions, trainer positions, model loading and swapping.

### 0x801C53BC - 0x801D7230: Battle Logic (~205 functions)
Unidentified battle subsystem. Likely contains:
- Damage calculation
- Type effectiveness
- Status effects
- Turn order determination
- AI decision making

### 0x801D7230 - 0x801E03D4: Waza (Move Animation) System (~108 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x801D7464 | `wazaSequenceLoad` | 0x730 |
| 0x801D7B94 | `wazaSequenceUpdate` | 0x2C4 |
| 0x801D7E58 | `wazaSequenceEntryStop` | 0x374 |
| 0x801D81CC | `wazaSequenceEntryUpdate` | 0x328 |
| 0x801D84F4 | `wazaSequenceEntryStart` | 0x2BC |
| 0x801D87B0 | `wazaSequenceStartEntry` | 0x388 |
| 0x801D8B38 | `_wazaSequenceParticleEntryStart` | 0x6B4 |
| 0x801D91EC | `_wazaSequenceModelEntryStart` | 0x604 |
| 0x801D9950 | `wazaSequencePokemonMotionStart` | 0x2CC |

"Waza" (Japanese for "technique/move") sequences control battle move animations. Each move has a sequence of visual entries (particles, model animations, screen effects).

Strings: `"wazaSequenceEntryStop: Invalid Data Type!"`, `"wazaSequenceEntryUpdate: Invalid Data Type!"`, `"wazaSequenceEntryStart: Invalid Data Type!"`, `"wazaSequenceEntryStart: Failed to start sequence entry!"`, `"_wazaSequenceParticleEntryStart: Some error occured here"`, `"_wazaSequenceModelEntryStart: Some error occured here"`, `"wazaSequencePokemonMotionStart: Failed to start pokemon motion!"`, `"wazaSequenceUpdate: Could not start sequence entry!"`, `"Error Loading Waza file!"`, `"sequenceLoad: Error Loading Sequence file!"`, `"Waza Viewer: Cannot attach to a model that hasn't started yet"` (debug tool!)

The "Waza Viewer" string suggests a debug tool for previewing move animations existed.
References `wazaViewer.wvd` file format.

### 0x801E03D4 - 0x801EF02C: Battle State Machine (~150 functions)
Contains very large functions (0x1A88+ bytes) that are likely battle state machine switch tables. Three functions at 0x801E6684, 0x801E810C, and 0x801E9B98 are each ~6.8KB, suggesting parallel state machines (possibly one per battle side, or different battle phases).

### 0x801EF02C - 0x801F000C: Battle Core / Fight Flow (26 functions)
| Address | Proposed Name | Size |
|---------|---------------|------|
| 0x801EF374 | `battle_FightEnd` | 0x114 |
| 0x801EF4B0 | `battle_FightStart` | 0x110 |
| 0x801EFA08 | `battle_MainLoop` | 0x5BC |

Strings: `"---------- fight end !! ----------"`, `"---------- fight start !! ----------"`

---

## Source File Cross-Reference

| Source File | Rodata Label | Code Range | Functions |
|-------------|-------------|------------|-----------|
| menuCardE.c | lbl_80266FAC | 0x80033278-0x80034280 | 1 |
| menuCB_Battle.c | lbl_80267C94 | 0x80069A60-0x80069C0C | 1 |
| menuCB_Common.c | lbl_80268708 | 0x8007109C-0x8007162C | 5 |
| menuToolBattle.c | lbl_802688F8 | 0x8007581C-0x80075A34 | 1 |
| menuCB_Rule.c | lbl_80268A48 | 0x800767B8-0x80077A5C | 4 |
| menuExDiscShrine.c | lbl_80268AB8 | 0x80077ED4-0x80078390 | 1 |
| menuExDiscCoupon.c | lbl_80268AE0 | 0x800792D8-0x800798E8 | 1 |
| menuPokeCoupon.c | lbl_80268B38 | 0x8007C2C0-0x8007C300 | 1 |
| menuCardE_Matrix.c | lbl_80268D78 | 0x8007C300-0x8007FD64 | 4 |
| cardesavedata.c | lbl_8026F1C8 | 0x80082650-0x80083AF4 | 11 |
| pokeconv.c | lbl_8026F568 | 0x80089048-0x800895A4 | 2 |
| gbaCommunication.c | lbl_8026F5A8 | 0x80092C90-0x800937F4 | 6 |
| GCN_Mem_Alloc.c | lbl_8026FE70 | ~0x800C5000 (CW runtime area) | - |
| pslist.c | lbl_802737B8 | 0x80168C64-0x80168DAC | 2 |
| psinterpret.c | lbl_802739A0 | 0x8016F430-0x8016F500 | 1 |
| generator.c | lbl_802739E4 | 0x8017424C-0x8017572C | 1 |
| displayfunc.c | lbl_802746DC | 0x80197A64-0x80197C70 | 2 |
| initialize.c | lbl_802749E4 | 0x8019C690-0x8019C978 | 4 |
| memory.c | lbl_80274E10 | 0x801A69C0-0x801A6A34 | 1 |
| shadow.c | lbl_802752C0 | 0x801B019C-0x801B1730 | 10 |
| gs_render.c | (gap 3) | 0x800D3E4C-0x800E202C | 278 |
| gs_model.c | (gap 2) | 0x80101910-0x8010C220 | 145 |
| gs_field_colquery.c | (gap 1a) | 0x8010E138-0x80114300 | ~200 |
| gs_field_resource.c | (gap 1b) | 0x8011432C-0x80114CA8 | ~18 |
| gs_field_world.c | (gap 1c) | 0x80114CA8-0x80130CE0 | ~600 |
| gs_dvd.c | (gap 4) | 0x80167040-0x80168C64 | 56 |
| gs_scene.c | (gap 5) | 0x8017572C-0x8017A5FC | 78 |

---

## Key GS Engine Strings (Rodata Address Map)

| Rodata Address | String | Engine Module |
|---------------|--------|---------------|
| 0x80266420 | `"OS avail memory: %d\n"` | System init |
| 0x80266688 | `"[%4d]FLAG_%4d"` | Flag debug |
| 0x80270360 | `"GSgfx: unable to allocate..."` | GSgfx |
| 0x80270D78 | `"GSmem: Warning -- memory loss..."` | GSmem |
| 0x80270F10 | `"GSpart: part is already registered..."` | GSpart |
| 0x80270F98 | `"GStexture: invalid texture format"` | GStexture |
| 0x80271008 | `"GSthreadCreate..."` | GSthread |
| 0x802717F0 | `"GSfloorOpen: cannot find floor %d"` | GSfloor |
| 0x80272050 | `"GScolsys2Draw..."` | GScolsys |
| 0x80272A58 | `"GSeffect: Cannot trigger effect..."` | GSeffect |
| 0x80273548 | `"ERROR: can't open WAVE ID = %d"` | Sound |
| 0x802735C4 | `"ERROR(sound.c)..."` | Sound |
| 0x80273748 | `"_sndCheckSndWorkALL:Start"` | GSDVD |
| 0x80273780 | `"[GSDVD_ERROR_STATE_COVEROPEN_WAIT]..."` | GSDVD |
| 0x802737B8 | `"pslist.c"` | Script |
| 0x802739A0 | `"psinterpret.c"` | Script |
| 0x80273A00 | `"gs%04d.xfb"` | GSscene |
| 0x80273F70 | `"gsfsys.toc"` | FSYS |
| 0x802741F8 | `"ERROR[GSflagSet]..."` | GSflag |
| 0x80275808 | `"battleGridReplacePokemon..."` | Battle |
| 0x80279588 | `"wazaSequenceEntryStop..."` | Waza |
| 0x80279BD8 | `"---------- fight end !! ----------"` | Battle |

---

## Movie Files Referenced

| Rodata Label | Path | Usage |
|-------------|------|-------|
| lbl_80266FE8 | `movie/openingdemo.thp` | Opening cinematic |
| lbl_80266FF8 | `movie/staffroll.thp` | Credits roll |
| lbl_80267008 | `movie/autodemo01.thp` | Auto-demo / attract mode |
| lbl_80267018 | `movie/gs_logo.thp` | Genius Sonority logo |
| lbl_80267028 | `movie/tpc.thp` | The Pokemon Company logo |

---

## Statistics

- **Total .text functions**: 8,589
- **SDK/Library named**: 274
- **Game code (fn_*)**: 8,315
- **Proposed names this pass**: 252 (122 original + 130 from gap analysis)
- **Subsystems identified**: 37 (30 original + 5 from gap analysis + 2 sub-modules)
- **Source files**: 27 (20 confirmed + 7 new gap decompilations)
- **Functions covered by gap analysis**: 1,381 (824+145+278+56+78)
- **Remaining unnamed**: ~6,812 (~82% of game code)

## New subsystems from gap analysis (2026-03-19)

| Gap | Range | Size | Subsystem | Functions | Source Files |
|-----|-------|------|-----------|-----------|--------------|
| 1 (PRIORITY) | 0x8010E138-0x80130CE0 | 142KB | GSfield (field/world) | 824 | gs_field_colquery.c, gs_field_resource.c, gs_field_world.c |
| 2 | 0x80101910-0x8010C220 | 44KB | GSmodel (3D models) | 145 | gs_model.c |
| 3 | 0x800D3E4C-0x800E202C | 56KB | GSrender (GX pipeline) | 278 | gs_render.c |
| 4 | 0x80167040-0x80168C64 | 7KB | GSDVD (disc I/O) | 56 | gs_dvd.c |
| 5 | 0x8017572C-0x8017A5FC | 22KB | GSscene (scene objects) | 78 | gs_scene.c |

## Priority areas for further analysis
1. **GSfield matching** (0x8010E138-0x80130CE0): 824 functions now identified, begin matching
2. **Battle logic** (0x801C53BC-0x801D7230): 205 functions, core game mechanics
3. **VFX system** (0x80130000-0x8014A000): 446 functions, visual effects
4. **People/NPC** (0x80181300-0x8018F200): 227+ functions, NPC management
5. **UI Core** (0x80059BDC-0x8006A000): 103 functions, menu state machine
6. **Pokemon model/animation** (0x8014A000-0x801655D4): 341 functions
7. **Battle state machine** (0x801E03D4-0x801EF02C): 150 functions with huge state tables
