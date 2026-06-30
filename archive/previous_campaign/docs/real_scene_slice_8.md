## Goal

Advance from the current shared narrow `TObj` / `TExp` stage boundary to the
next smallest broader shared texture-expression milestone by unlocking exactly
one additional real reachable stage family, if one exists cheaper than a more
generic parser step.

## Cost Comparison

### Option 1: admit one more nearby real stage family

The cheapest nearby candidate was not a brand-new scene branch. It was the
already rendered `topmenu.fsys:menu_bg00` front textured object:

- `joint 0x6FE8`
- `dobj 0x3B98`
- `mobj 0x3900`
- `tobj 0x3890`
- `image 0x3878`
- `pobj 0x3B80`

Why it was cheaper:

- the branch was already visible and verified
- the host GX path already handled its raw texture upload
- the missing step was only that it still bypassed the shared `TObj` / `TExp`
  stage translator

### Option 2: broaden the parser generically above the current shared boundary

Why it lost:

- it would require a larger step than needed for the already reachable front
  branch
- it would not give a smaller proof than routing that branch through the
  existing translated stage layer

### Option 3: engine boot

Why it was still rejected:

- still larger and less direct than crossing the already reachable
  texture-expression boundary

## Chosen Path

I chose option 1: unlock one additional nearby real stage family.

The new family is a one-stage direct sample path:

- no serialized TEV payload
- one `TObj`
- current verified instance uses `GX_TF_CMPR`

This was smaller than a generic parser step because the render path already
worked; only the shared stage boundary was missing.

## Shared Work Implemented

### 1. Added a new shared stage-family kind

- [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L129):
  `PCPORT_TEXP_STAGE_DIRECT_SAMPLE = 3`

### 2. Extended the shared stage classifier

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L225):
  `ClassifyTextureExpStageKind(...)`

The new narrow classification is:

- one-node direct sample
- `format == GX_TF_CMPR`
- `tevArchiveOffset == 0`
- no translated TEV payload

That is intentionally smaller than a generic parser or general texture-stage
model.

### 3. Moved the existing front textured smoke onto the shared boundary

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L1507):
  `RunRealTexturedSceneSliceSmoke`

That smoke now:

- calls `PCPort_TranslateTextureExpFromArchiveBE(...)`
- verifies `stageCount == 1`
- verifies `stage0 == PCPORT_TEXP_STAGE_DIRECT_SAMPLE`
- uses the translated stage-0 texture to initialize the host `GXTexObj`

The important result is that this existing rendered branch no longer bypasses
the shared `TObj` / `TExp` stage layer.

## Exact Steps Attempted

1. Rebuilt:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Verified the new direct-sample family on the already rendered front branch:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
```

3. Re-ran current rendered regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### New shared-stage-family proof

The front textured branch now uses the shared stage translator and passed with:

```text
[pcport_bootstrap] Real textured scene slice smoke passed (... texTObj=0x3890 ... stage0=3 stages=1 diffPixels=307200 ... tev=0 size=640x480 format=14 submitted=4 expanded=4 prim=0x98)
```

That directly verifies:

- one existing rendered branch now uses a broader shared `TObj` / `TExp`
  boundary than the previous narrow staged families
- the added family is real and repository-backed
- the game-owned `fn_801AA568 -> fn_800DAD10` draw bridge is unchanged

### Whether the result is visually richer or structurally cleaner

Structurally cleaner.

The visible result stayed working, but the important change is that the
existing textured front branch is now routed through the shared translated
stage boundary instead of a direct `TObj` translation path.

### Regression status

These all still passed:

- `--real-textured-scene-slice-smoke`
- `--real-scene-slice-3-smoke`
- `--real-scene-slice-4-smoke`
- `--real-tev-scene-slice-3-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--gsgfx-visible-smoke`

## Directly Verified Versus Inferred

### Directly verified

- the topmenu front textured branch now reaches `stage0=3`, `stages=1`
- the branch still renders visibly through the existing game-owned path
- the existing rendered TEV and chained-`TObj` regressions stayed green

### Still intentionally narrow

- this new family is still not a general direct-sample parser
- it is currently limited to the reachable no-TEV CMPR family already present
  on the front branch

## Exact Blockers Still Remaining

1. There is still no general shared `TObj` / `TExp` parser.
2. The direct-sample family is still narrow to the currently verified no-TEV
   CMPR case.
3. `texCoordSrc` support is still narrow to the meanings already exercised.
4. TEV interpretation is still family-specific.
5. TLUT / CI paths, broader material-lighting behavior, and engine-owned
   title/menu boot or update flow are still outside the current bridge.

## Conclusion

The smaller next step was one additional nearby real stage family, not the
broader parser layer and not engine boot.

That step succeeded: the already rendered front textured branch now uses the
shared translated `TObj` / `TExp` boundary as a one-stage direct-sample family.

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is either:

- one more nearby real stage family that fits under the same shared translated
  stage boundary
- or the next broader real parser layer above it, if the next reachable branch
  no longer collapses cleanly to one narrow family
