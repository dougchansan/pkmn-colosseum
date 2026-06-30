# Pokemon Colosseum Function Map

Analysis of the code layout based on disassembly of GC6E01 (NTSC-U).

## Address Range Overview

The .text section spans `0x800055E0` to `0x8026635C` (size 0x260D7C, ~2.4 MB).

### Major Subsystem Regions

| Start | End (approx) | Size (approx) | Subsystem | Evidence |
|-------|-------------|----------------|-----------|----------|
| `0x800055E0` | `0x80006500` | ~3.9 KB | **Game main / init** | `main()`, error handlers, game loop |
| `0x80006500` | `0x80009700` | ~12.8 KB | **Game logic / flags** | Flag system (`[%4d]FLAG_%4d` string), scene management |
| `0x80009700` | `0x8000BA00` | ~9.0 KB | **Battle core** | Large function fn_800096B4 (0x23E0 = 9.2 KB) |
| `0x8000BA00` | `0x8000D300` | ~6.1 KB | **Battle helpers / data** | Clusters of 0x6C-sized accessor functions |
| `0x8000D300` | `0x80010000` | ~11.5 KB | **World / scene / overworld** | Scene state management |
| `0x80010000` | `0x80020000` | ~64 KB | **Scene scripting** | Event/script execution functions |
| `0x80080000` | `0x80090000` | ~64 KB | **PAD / controller** | fn_8008ABA0 (PAD motor check), fn_8008AC34 (PAD recalibrate) |
| `0x80098000` | `0x800A3000` | ~44 KB | **Dolphin SDK: EXI / OS** | EXIImm, EXIInit, OSInit, OSExceptionInit, etc. |
| `0x800A3000` | `0x800AB000` | ~32 KB | **Dolphin SDK: DVD / Reset / SRAM** | DVDInit, DVDFSInit, OSResetSystem, SRAM functions |
| `0x800AB000` | `0x800B0000` | ~20 KB | **Dolphin SDK: VI / AI / AR** | VIInit, AISetDSPSampleRate, ARInit |
| `0x800C4000` | `0x800C9000` | ~20 KB | **C runtime / libc** | `__va_arg`, `vsprintf`, `sprintf` |
| `0x800D2000` | `0x800E0000` | ~56 KB | **GSgfx (Genius Sonority GFX)** | GSgfx init, render state, display lists, materials |
| `0x800E0000` | `0x800F0000` | ~64 KB | **GSmem / GX wrapper** | Memory management, GX FIFO, VI setup |
| `0x800F0000` | `0x80100000` | ~64 KB | **GSthread / PAD / Floor** | Thread system, pad polling, floor/scene loading |
| `0x80100000` | `0x80110000` | ~64 KB | **Rendering pipeline** | GX render state, particle system, effect system |
| `0x80110000` | `0x80120000` | ~64 KB | **Colosseum mode / 3D models** | fn_801128A0 (Colosseum init), material system |
| `0x80120000` | `0x80136000` | ~88 KB | **Sound / RNG / model loader** | Sound mixer, RNG, texture cache, 3D model loading |
| `0x80136000` | `0x80168000` | ~200 KB | **UI / Menu system** | Menu source files in strings: menuCardE.c, menuCB_Battle.c, menuCB_Common.c, menuToolBattle.c, menuCB_Rule.c, menuExDiscShrine.c, menuExDiscCoupon.c, menuPokeCoupon.c, menuCardE_Matrix.c |
| `0x80168000` | `0x80180000` | ~96 KB | **Floor / scene loader** | GSfloor functions, scene data registration |
| `0x80180000` | `0x80192000` | ~72 KB | **World / map / Pokemon data** | World system, Pokemon model/data init |
| `0x801C4000` | `0x801E0000` | ~112 KB | **Script engine** | Script/event system, flag management |
| `0x801E0000` | `0x801F0000` | ~64 KB | **Save / Card / GBA link** | Card/save system, GBA communication |
| `0x80250000` | `0x80260000` | ~64 KB | **REL module loader** | fn_8025DBD4 (load REL by ID) |

### Data Sections

| Start | End | Size | Section | Contents |
|-------|-----|------|---------|----------|
| `0x802663A0` | `0x8027A4F0` | 0x14150 (~82 KB) | `.rodata` | String literals, lookup tables, float constants |
| `0x8027A500` | `0x8039A6DB` | 0x1201DB (~1.1 MB) | `.data` | Scene data, floor tables, Pokemon data, textures |
| `0x8039A700` | `0x803A1700+` | varies | `.bss` | ARAM base, crash report buffers, thread stacks |
| `0x80470000` | `0x8047C000` | varies | `.sdata/.sbss` | Small data: global flags, pointers, counters |

## Identified Functions

### Game Main (0x800055E0 - 0x80006500)

| Address | Size | Proposed Name | Description |
|---------|------|---------------|-------------|
| `0x800055E0` | 0xE4 | `main` | Entry point; heap/DVD/GFX init, calls GameInit |
| `0x800056C4` | 0x10 | `SetPauseFlag` | Set init-complete/pause flag, return old value |
| `0x800056D4` | 0x10 | `ClearAndGetPauseFlag` | Read and clear the pause flag |
| `0x800056E4` | 0x08 | `SetRumbleEnabled` | Set controller rumble enable byte |
| `0x800056EC` | 0x5C | `SetMasterVolume` | Set sound mixer master volume |
| `0x80005748` | 0x58 | `GetMasterVolume` | Get sound mixer master volume |
| `0x800057A0` | 0x08 | `GetVersionMajor` | Returns 1 |
| `0x800057A8` | 0x08 | `GetVersionMinor` | Returns 6 (version 1.6) |
| `0x800057B0` | 0x2FC | `GameInit` | Master init; sets up all subsystems, enters main loop |
| `0x80005AAC` | 0x190 | `GameMainLoop` | Main game thread; per-frame updates, RNG, subsystems |
| `0x80005C3C` | 0xA8 | `TaskResetHandler` | Reset button handler (latch on press, reset on release) |
| `0x80005CE4` | 0x9C | `TaskResetPoll` | Clear exception vectors, 100-frame countdown |
| `0x80005D80` | 0x80 | `TaskVBlank` | Primary VBlank: render, scene tick, save update |
| `0x80005E00` | 0x1A8 | `TaskRetraceMain` | Per-retrace: debug menu, card state, particles, sound |
| `0x80005FA8` | 0x54 | `TaskRetraceAudio` | Audio retrace; locale-aware reconnect |
| `0x80005FFC` | 0x50 | `TaskPadRead` | Poll pads 1-4, recalibrate disconnected pads |
| `0x8000604C` | 0x50 | `TaskPadRumble` | Update rumble for pads 1-4 |
| `0x8000609C` | 0x54 | `InitBackgroundColor` | Set clear color from rodata table |
| `0x800060F0` | 0x160 | `OSReportCrash` | Variadic crash report formatter (calls vsprintf/sprintf) |
| `0x80006250` | 0x128 | `InstallErrorHandlers` | Set memory protection, install OS error handlers |
| `0x80006378` | 0x128 | `ErrorHandler` | Custom exception handler; format & display crash info |
| `0x800064A0` | 0x24 | `ExceptionRecoveryHandler` | Exception 8 handler; calls OSLoadContext |
| `0x800064C4` | 0x16C | `ErrorDisplayThread` | Thread that renders crash report on screen |

## Key String References

### Source File Names (from .rodata assertions)
These reveal the original source file organization:

- `menuCardE.c` - e-Reader card menu
- `menuCB_Battle.c` - Colosseum battle menu callbacks
- `menuCB_Common.c` - Common menu callbacks (has stack push/pop assertions)
- `menuToolBattle.c` - Battle tool/utility menu
- `menuCB_Rule.c` - Rule selection menu callbacks
- `menuExDiscShrine.c` - Mt. Battle exchange menu
- `menuExDiscCoupon.c` - Coupon exchange menu
- `menuPokeCoupon.c` - Pokemon coupon management
- `menuCardE_Matrix.c` - e-Reader card matrix display
- `cardesavedata.c` - e-Reader card save data
- `pokeconv.c` - Pokemon data conversion (GBA<->GCN format)
- `gbaCommunication.c` - GBA-GameCube communication
- `shadow.h` / `shadow.c` - Shadow rendering
- `object.h` - Game object base class
- `pslist.c` / `psinterpret.c` - Particle system
- `generator.c` - Particle generator
- `displayfunc.c` - Display/rendering functions
- `initialize.c` - Initialization module
- `memory.c` - Memory management

### Engine Subsystem Strings (GS = Genius Sonority)
- `GSgfx` - Graphics engine (init, matrix stacks, render state)
- `GSmaterial` - Material/shader system
- `GSmem` - Memory allocator (block-based, with fragmentation tracking)
- `GSpart` - Skeletal animation / part system (joint hierarchies)
- `GStexture` - Texture management (format validation, size adjustment)
- `GSthread` - Threading system (cooperative multitasking on GCN)
- `GSfloor` - Floor/scene management (room loading, transitions)
- `GScolsys2` - Collision system (display list allocation for debug draw)
- `GSeffect` - Particle/visual effect system
- `GSsnd` - Sound system (SE and BGM management)

### Other Notable Strings
- `"OS avail memory: %d\n"` - Printed during init (fn_800057B0)
- `"movie/openingdemo.thp"` - Opening movie THP video
- `"movie/staffroll.thp"` - Staff credits video
- `"movie/autodemo01.thp"` - Auto-play demo video
- `"movie/gs_logo.thp"` - Genius Sonority logo video
- `"movie/tpc.thp"` - The Pokemon Company logo video
- `"bg0thumbcode.bin"` / `"bg1thumbcode.bin"` / `"bg2thumbcode.bin"` - GBA ARM thumb code for backgrounds
- `"scene_data"` - Scene data archive identifier
- `"MetroTRK for GAMECUBE v2.6"` - Debug kernel version
- `"Stack overflow.\n"` - GSthread stack overflow detection

## Identified Global Variables (SDA region)

| Address | Type | Proposed Name | Usage |
|---------|------|---------------|-------|
| `0x80478DC0` | struct | `gDVDInfo` | DVD drive info structure |
| `0x80478DC8` | u8 | `gRumbleEnabled` | Controller rumble on/off flag |
| `0x80478DC9` | u8 | `gInitComplete` | Set to 1 after GameInit finishes |
| `0x80478DCA` | u8 | `gResetButtonLatched` | Reset button press latch |
| `0x80478DCC` | s32 | `gResetCountdown` | 100-frame countdown after exception clear |
| `0x80478DD0` | u8 | `gDrawMode` | Current rendering draw mode |
| `0x80478820` | u8 | `gCardSystemFlag` | Controls card/save update behavior |
| `0x80478990` | s32 | `gMaxErrorHandler` | Maximum OS error handler index installed |
| `0x80478FB8` | u32* | `gFrameCounterPtr` | Pointer to current frame counter |
| `0x80478FBC` | void* | `gSceneStatePtr` | Pointer to active scene state |
| `0x8047A260` | u32 | `gCrashErrorNumber` | Error number saved during crash |
| `0x8047A264` | u32 | `gCrashSRR0` | SRR0 (PC) saved during crash |
| `0x8047A268` | u32 | `gCrashSRR1` | SRR1 (MSR) saved during crash |

## Boot Sequence

1. `__start` (CRT) initializes registers, hardware, data sections, calls `main()`
2. `main()`:
   - Clears low memory scratch (0x80001800)
   - `InstallErrorHandlers()` - OS exception handling + memory protection
   - Sets up GSmem heap from OS arena (~14.5 MB)
   - Creates OS heap for system allocations
   - Initializes DVD filesystem
   - Initializes ARAM (audio memory)
   - Initializes XFB and VI (video)
   - Parses command-line arguments
   - Calls `GameInit()` (never returns)
3. `GameInit()`:
   - GSgfx graphics engine init
   - Warm boot detection and video reconfiguration
   - GX FIFO, sound, object pool, lighting, textures, materials
   - Texture cache, camera, controller pads (4 controllers)
   - GSthread cooperative threading system
   - PAD system, floor/scene system, script/event system
   - Save/card system, GBA communication, battle system
   - Particle and effect systems
   - Registers 7 per-frame callback tasks
   - Creates main game loop thread
   - Enters infinite scheduler yield loop
4. `GameMainLoop()` (runs as thread):
   - Scene tick, frame counter update
   - RNG seeding from OS clock
   - REL module loading (modules 1, 4, 7)
   - RTC calendar sync
   - Subsystem initialization (world, models, Pokemon, GBA, UI, Colosseum)

## Architecture Notes

- **Engine**: "GS" prefix throughout = Genius Sonority's proprietary engine
- **Threading**: Cooperative multitasking via GSthread (not preemptive OS threads for game logic)
- **Scene system**: Floor-based with REL (relocatable) module loading for different areas
- **Memory**: GSmem block allocator on top of OS arena, separate OS heap for system use
- **Version**: 1.6 (GetVersionMajor=1, GetVersionMinor=6)
- **Sound**: Separate mixer with master volume control, supports both SE and BGM
- **GBA link**: Full bidirectional communication system for Pokemon Ruby/Sapphire transfer
- **Save**: Card-based save system with e-Reader support
