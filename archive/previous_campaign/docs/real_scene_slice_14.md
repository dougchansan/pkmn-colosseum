# Goal

Advance exactly one bounded milestone beyond slice 13 toward a broader shared
real `TObj` / `TExp` parser/translation/classification layer, while staying
strictly below engine boot or update-flow work.

## Why this step was chosen

After slice 13, the parsed-node-chain boundary already carried additive
whole-expression `kind` metadata, but one smaller duplicated reasoning surface
still remained inside the shared classification path:

- `PCPort_ParseTextureNodeChainFromArchiveBE(...)` parsed nodes and classified
  the whole expression kind
- `ClassifyTextureExpKindFromParsedChain(...)` still recomputed per-node stage
  kinds on demand while deciding that whole-expression kind
- `PCPort_TranslateTextureExpFromArchiveBE(...)` then recomputed those same
  per-node stage kinds again when filling `outExp->stages[i].kind`

There was no clearly reachable new shared family with a smaller or safer delta
than removing that remaining duplicated stage-kind reasoning.

So the smallest correct slice-14 milestone was to broaden the parsed-chain
boundary one more step: cache the currently exercised per-node stage kinds on
the parsed-chain result itself, so both whole-expression classification and
`TExp` stage filling reuse classified parser output directly.

That is smaller than broadening TEV or hunting a farther family-specific branch,
and it directly sets up the next family by making the parsed-chain boundary own
more of the classification facts future family recognition will need.

## Exact files changed

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`
- `docs/real_scene_slice_14.md`

## Exact functions / boundaries changed

### `src/pcport/real_content_host.h`

Broadened `PCPortParsedTextureNodeChain` again:

- added `stageKinds[PCPORT_TEXP_STAGE_MAX]`

So the parsed-chain boundary now carries, for each parsed node:

- resolved coord id
- translated texture payload
- additive per-node stage kind
- additive whole-expression kind

### `src/pcport/real_content_host.c`

#### `PCPort_ParseTextureNodeChainFromArchiveBE(...)`

Now computes and stores `outChain->stageKinds[nodeIndex]` while parsing each
linked `TObj` node.

That makes the parsed-chain boundary itself responsible for returning the
currently exercised per-node stage-kind facts, instead of leaving those facts to
be recomputed later by each consumer.

#### `ClassifyTextureExpKindFromParsedChain(...)`

Now reuses `chain->stageKinds[]` instead of recomputing stage kinds from node
payloads every time whole-expression classification runs.

#### `PCPort_TranslateTextureExpFromArchiveBE(...)`

Now reuses `parsedChain.stageKinds[]` directly when filling
`outExp->stages[i].kind`.

This removes the last repeated per-node stage-kind derivation between parsed
whole-expression classification and translated `TExp` stage filling.

## What semantics became shared that were not shared before

Before this slice:

- the parsed-chain boundary carried structural node data, coord ids, and whole
  expression kind
- but per-node stage kinds were still recomputed after parsing

After this slice:

- the parsed-chain boundary also carries additive per-node stage-kind metadata
- whole-expression kind classification reuses those cached per-node stage kinds
- translated `TExp` stage filling also reuses those cached per-node stage kinds

So one more piece of currently exercised classification semantics is now shared
at parsed-chain level rather than being recomputed downstream.

## What remained narrow or family-specific

Still narrow or family-specific after this slice:

- the cached per-node stage kinds still only cover the currently exercised
  families:
  - direct sample
  - `I8` ramp sample
  - `I8` mask modulate
- whole-expression `kind` still only covers:
  - direct sample
  - `I8` ramp
  - `I8` ramp + mask
- no new shared family was added in this slice
- `texCoordSrc` coverage remains narrow beyond current verified meanings
- TEV interpretation, TLUT / CI, and broader material-lighting behavior remain
  narrow / unsupported
- engine-owned title/menu/game boot and update-flow work remain entirely out of
  scope

## Exact verification commands run

### Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

### Required smokes

```powershell
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Exact observed results

All required commands passed.

### Build

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed

### Smoke results

- `--real-textured-scene-slice-smoke`
  - passed
  - observed: `kind=3 stage0=3 stages=1 baked=1228800`
- `--real-scene-slice-3-smoke`
  - passed
- `--real-scene-slice-4-smoke`
  - passed
  - observed: `kind0=2 stage00=1 stage01=2 stages0=2 kind1=1 stage10=1 stages1=1 rawSrc0=4 coord0=0 rawSrc1=5 coord1=1`
- `--real-tev-scene-slice-3-smoke`
  - passed
  - observed: `kind=1 stage0=1 stages=1`
- `--real-tev-scene-slice-2-smoke`
  - passed
- `--gsgfx-visible-smoke`
  - passed

## Remaining blockers

1. No new shared whole-expression family has been added beyond the current
   direct sample / `I8` ramp / `I8` ramp+mask set.
2. The shared parser/classifier still only understands the currently exercised
   stage and expression families.
3. `texCoordSrc` support remains narrow beyond current verified meanings.
4. TEV interpretation, TLUT / CI, and broader material-lighting behavior remain
   narrow / unsupported.
5. The next real progress step is still either one newly reachable shared
   family or one parser/classifier extension that directly enables it.
6. Engine-owned title/menu/game boot and update-flow work remain out of scope
   and untouched.

## Why engine boot is still not the next step

Engine boot is still larger and less direct than the remaining shared
parser/classification debt on the current working path.

The next useful steps are still inside the shared real content bridge:

- add the next reachable shared whole-expression family
- or broaden the parser/classifier just enough to enable that family
- widen `texCoordSrc` or TEV only when directly forced by that family

So engine boot is still not the next step because the smaller, verifiable
shared parser/classification path is still the tightest path forward.

## Git / checkpoint note

The workspace is still dirty outside this slice, so this slice should not force
a commit unless the tree is cleaned first. A verified documented stop is safer
than a mixed commit.
