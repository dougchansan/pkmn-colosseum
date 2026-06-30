## Goal

Advance from the current exact two-node chained-`TObj` object path to the
smallest shared `TObj` / `TExp` translation boundary that can replace
branch-specific texture composition logic, without jumping to engine boot.

## Ranked Paths

1. Add one narrow shared `TObj` chain boundary that covers the currently
   reachable I8 families:
   - one-node I8 ramp paths
   - the existing two-node I8 ramp + mask path
   Why first:
   this directly replaces the current branch-specific composition logic and is
   smaller than finding and validating another one-off chain family.

2. Add another object-specific chained-`TObj` family.
   Why second:
   it would increase coverage, but it would not reduce the structural debt at
   the current `TObj` / `TExp` boundary.

3. Jump to engine boot.
   Why last:
   still larger and less direct than crossing the shared texture-object
   boundary already exposed by the working rendered branches.

## Chosen Path

I chose option 1: a shared translated `TObj` chain boundary.

The reason was straightforward: the current rendered branches already expose a
common narrow family:

- `pda2_bg` object 1: one-node I8-ramp chain
- `pda2_bg` object 0: two-node I8-ramp + mask chain

That meant a fixed-size shared chain translator and chain baker was smaller
than inventing another object-specific composition path.

## Shared Work Implemented

### 1. Shared translated `TObj` chain metadata

- [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L125):
  added `PCPortTranslatedTextureChainKind`
- [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L132):
  added `PCPortTranslatedTextureChain`

This is intentionally narrow:

- max 2 nodes
- only the currently verified I8 chain families

### 2. Shared chain translation

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1482):
  added `PCPort_TranslateTextureChainFromArchiveBE`
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L194):
  added internal raw `src` -> coord-id resolution for this shared boundary

The shared translator now classifies:

- `PCPORT_TEXTURE_CHAIN_I8_RAMP`
- `PCPORT_TEXTURE_CHAIN_I8_RAMP_MASK`

For this step, that is the smallest honest shared boundary above:

- the current one-node textured paths
- the current exact two-node chained path

### 3. Shared chain baking

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1697):
  added `PCPort_BakeTextureChainRGBAFromArchiveBE`

Behavior:

- one-node ramp chains reuse the existing I8 ramp bake path
- two-node ramp + mask chains use the same narrow composition logic that object
  0 previously used, but now through the shared chain boundary

### 4. Replace branch-specific usage in the existing rendered smokes

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3295):
  `RunRealTevSceneSlice3Smoke` now uses the shared one-node chain boundary
- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3620):
  `RunRealSceneSlice4Smoke` now uses the shared two-node chain boundary

That is the key structural change in this task: the object-0 composition logic
is no longer a branch-specific helper in `pcport_main.c`.

## Exact Steps Attempted

1. Rebuilt:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Verified the one-node shared-boundary path:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
```

3. Verified the two-node shared-boundary path:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
```

4. Re-ran regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### What was directly verified

The shared chain boundary now backs at least one existing rendered branch, and
in practice backs both the one-node and two-node verified `pda2_bg` paths.

The one-node proof was:

```text
[pcport_bootstrap] Real TEV scene slice 3 smoke passed (... obj1TObj=0x1A7C chainKind=1 chainNodes=1 obj1Tev=0x1A5C ...)
```

The two-node proof was:

```text
[pcport_bootstrap] Real scene slice 4 smoke passed (... obj0TObj0=0x19BC obj0TObj1=0x1918 chainKind0=2 chainKind1=1 obj0Tev0=0x199C obj0Tev1=0x18F8 rawSrc0=4 coord0=0 rawSrc1=5 coord1=1 ...)
```

That directly proves:

- an existing rendered one-node branch now uses the shared boundary
- the existing rendered two-node branch now uses the shared boundary
- the game-owned draw bridge is unchanged

### Whether the result is materially richer or structurally cleaner

Structurally cleaner.

I did not claim a richer visible scene here. The verified visible output stayed
working, but the important change is architectural at the narrow host bridge:

- one-node and two-node textured branches now use one shared translation/bake
  boundary
- the old object-specific composition helper in `pcport_main.c` is gone

### Regression status

These all still passed:

- `--real-tev-scene-slice-3-smoke`
- `--real-scene-slice-4-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--real-scene-slice-3-smoke`
- `--real-textured-scene-slice-smoke`
- `--gsgfx-visible-smoke`

## What Is Verified Versus Inferred

### Directly verified

- the shared chain helper classifies the reachable one-node path as kind `1`
- the shared chain helper classifies the reachable two-node path as kind `2`
- both paths still render through the existing game-owned bridge
- the regression set stayed green

### Narrow inference still present

The shared chain baker is still not a general `TExp` interpreter.

It is a narrow shared boundary for the currently reachable real families:

- one-node I8 ramp
- two-node I8 ramp + mask

That is still repository-grounded, but it is not general `TObj` / `TExp`
support yet.

## Exact Blockers Still Remaining

1. There is still no general chained `TObj` list support beyond the current
   fixed-size shared boundary.
2. There is still no real shared `TObj` / `TExp` parser.
3. `texCoordSrc` support is still narrow to the exercised meanings.
4. TEV interpretation is still family-specific.
5. TLUT / CI paths, broader material/lighting behavior, and engine-owned
   title/menu boot are still outside the current bridge.

## Conclusion

The shared `TObj` chain boundary was the smaller next step, and it succeeded.

That means the current critical path is still:

- not engine boot
- not another one-off object-specific texture composition path
- the next broader shared `TObj` / `TExp` boundary above the current fixed-size
  I8 chain families

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is either:

- one more shared chain family at the same boundary, if a nearby reachable
  branch exposes one cheaply
- or the smallest real `TObj` / `TExp` translation layer above the current
  fixed-size chain classifier
