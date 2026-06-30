# symbolmap - evidence-driven naming support for GC6E01

`tools/symbolmap` is a naming-evidence pipeline for Pokemon Colosseum
(`GC6E01`). It is not the symbol map itself. The scripts in this directory mine
the DOL disassembly, compare it with Pokemon XD where available, and write
reviewable naming artifacts under:

```text
config/GC6E01/symbolmap/
```

Use this folder when you want better names for `fn_XXXXXXXX` functions, want to
confirm which translation unit a range came from, or want a safer way to decide
whether a proposed name is strong enough to adopt.

The important rule: this pipeline is advisory by default. It can generate
`// Proposed:` comments and rename maps, but the normal `run.py` pipeline does
not edit `symbols.txt`, source files, asm, or `.inc` truth files.

## Why This Helps Naming Work

The game contains debug/assert strings even though it has no normal source-level
symbol table. Those strings give strong naming evidence:

- CodeWarrior `__FILE__` literals such as `menuCardE_Matrix.c` prove the source
  file for any function that references them.
- Assert/log strings often contain the real function name, for example
  `_menuPop():stack under.` or `GSmaterialCreate: ...`.
- Pokemon XD (`GXXE01`) shares much of the same engine. If Colosseum and XD
  functions share rare string literals or identical instruction structure, XD's
  decomp name can be a high-value naming lead.

This is especially useful during decomp work because names become evidence
records rather than guesses. For example, Card-E work can combine local offset
evidence with `symbols.txt` `// Proposed:` comments without renaming the whole
project.

## Inputs

The pipeline uses these inputs:

| input | purpose |
|---|---|
| `build/GC6E01/asm/auto_*.s` | dtk symbol-resolved Colosseum asm. Build with `python configure.py && ninja` if absent. |
| `config/GC6E01/symbols.txt` | Current project symbol names and existing `// Proposed:` comments. |
| `config/GC6E01/splits_refined.txt` | Current address-range to source-file map for TU attribution checks. |
| XD asm, optional | Enables stronger name ports from Pokemon XD. |
| XD `symbols.txt`, optional | Provides real XD names to port from. |

## Outputs

Most useful generated/review files live in `config/GC6E01/symbolmap/`.

| output | what it means | how to use it |
|---|---|---|
| `strings.json` | All mined string objects from dtk asm. | Raw input for other passes. |
| `fn_strings.json` | Function to referenced-string map. | Raw evidence for a single function's string refs. |
| `tu_attribution.md` | Human-readable source-file attribution from `*.c` strings. | Check whether a range belongs to the file we think it does. |
| `tu_evidence.json` | Machine-readable TU attribution evidence. | Feed follow-up scripts or custom analysis. |
| `name_proposals.md` | Names mined from self-identifying log/assert strings. | Review proposed names and their exact string evidence. |
| `name_proposals_high.txt` | High-confidence self-name proposals. | Good source for `// Proposed:` comments. |
| `xd_port.md` / `xd_port.json` | XD-to-Colosseum ports by shared string evidence. | Stronger than local self-name when unambiguous. |
| `xd_port_apply.txt` | XD-port rename candidates in simple map form. | Review before applying. |
| `xd_vocabulary.json` | Names seen in XD. | Search when trying to match naming style. |
| `proposed_symbols.txt` | Merged high-confidence rename proposal map. | Main review file: `fn_OLD -> NewName // provenance`. |
| `symbols.with_proposals.txt` | Copy of `symbols.txt` with proposed renames applied. | Diff against `symbols.txt` before adopting. |
| `applied_symbols.txt` | Names judged safe for mechanical application. | Input to `apply_names.py`. |
| `leads_needs_wiring.md` | Confirmed names blocked by typed-prototype/signature work. | Per-function decomp follow-up list. |
| `structural_ports.json` | XD ports by instruction-mnemonic fingerprint. | Naming leads; verify before adopting. |
| `structural_applied.txt` | Structural-port candidates that were applied/recorded. | Existing structural naming record. |

## Evidence Strength

Treat evidence tiers roughly like this:

| tier | signal | confidence |
|---|---|---|
| Strong | XD shared-string port, unambiguous, high score. | Often safe as a proposed name; still verify context. |
| Strong | Unique self-identifying assert/log string. | Good for `// Proposed:` and often for real rename after compile checks. |
| Medium | Structural XD port with identical mnemonic fingerprint only. | Useful lead, but verify behavior and surrounding names. |
| Medium | TU attribution from `__FILE__`. | Strong file evidence, but not usually a function name by itself. |
| Weak | Shared format string, generic prefix, or broad string fanout. | Treat as a clue only. |

## Common Naming Workflow

For normal decomp naming work:

1. Search the current project symbols first.

   ```sh
   rg -n "menuCardE_CompareEntryPtrs|Proposed:" config/GC6E01/symbols.txt
   ```

2. Search the symbolmap evidence.

   ```sh
   rg -n "menuCardE_CompareEntryPtrs|menuCardE|CardE|Card-E" config/GC6E01/symbolmap
   ```

3. Inspect the generated review files, not only the final map.

   ```sh
   code config/GC6E01/symbolmap/name_proposals.md
   code config/GC6E01/symbolmap/xd_port.md
   code config/GC6E01/symbolmap/tu_attribution.md
   ```

4. If the evidence is good but the function still needs typing, record it as a
   `// Proposed:` comment in `config/GC6E01/symbols.txt` rather than bulk
   renaming the source.

5. Only do a real symbol rename when the call signatures, header prototypes,
   source references, and compile/match checks are understood.

6. After any real rename or source edit, run focused verification.

   ```sh
   python tools/compile_check.py src/game/menu/menu_carde_matrix.c
   python tools/match_test.py menuCardE_CompareEntryPtrs --verbose
   ```

## Running The Pipeline

From the repository root:

```sh
# Build or refresh Colosseum asm first if needed.
python configure.py
ninja

# Run all default Colosseum-only stages.
python tools/symbolmap/run.py
```

`run.py` runs these stages:

1. `mine_xrefs.py` - mine strings and function string references.
2. `attribute_tus.py` - map functions/ranges to source files via `__FILE__`.
3. `propose_names.py` - mine self-identifying log/assert names.
4. `port_xd.py` - port XD names if XD data is present.
5. `build_symbol_map.py` - merge high-confidence evidence into review maps.

Use explicit paths if you are working from a different build/output tree:

```sh
python tools/symbolmap/run.py \
  --asm-dir build/GC6E01/asm \
  --out-dir config/GC6E01/symbolmap \
  --symbols config/GC6E01/symbols.txt \
  --splits config/GC6E01/splits_refined.txt
```

## Enabling The XD Port

The XD port needs XD's `main.dol` and an XD asm split. From a disc image:

```sh
# 1. Convert rvz to iso if needed.
dolphin-2603a-x64/Dolphin-x64/DolphinTool.exe convert -i XD.rvz -o xd.iso -f iso

# 2. Extract main.dol. Run dtk from the iso directory because drive-letter
#    colons collide with dtk's container separator.
cd <dir-with-xd.iso>
tools/dtk.exe vfs cp "xd.iso:sys/main.dol" xd_main.dol

# 3. Split with the mining config.
cd tools/decomp_work/refs/xd-decomp
cp <repo>/tools/symbolmap/xd_config.mine.yml config/GXXE01/config.mine.yml
cp <path-to>/xd_main.dol orig/GXXE01/sys/main.dol
../../../dtk.exe dol split config/GXXE01/config.mine.yml _xdsplit

# 4. Run with XD asm enabled.
cd <repo>
python tools/symbolmap/run.py --xd-asm tools/decomp_work/refs/xd-decomp/_xdsplit/asm
```

Expected XD `main.dol` SHA1:

```text
ff9e752ead9914af0b363ae6c831a34ccce189d2
```

## Tool Index

| script | role |
|---|---|
| `run.py` | One-command driver for the standard string/XD pipeline. |
| `mine_xrefs.py` | Extracts `strings.json` and `fn_strings.json` from dtk asm. |
| `attribute_tus.py` | Generates TU/source-file attribution evidence. |
| `apply_tu_attribution.py` | Manually curated updater for `splits_refined.txt`; review before use. |
| `propose_names.py` | Mines names from self-identifying strings. |
| `port_xd.py` | Ports names from XD using shared string evidence. |
| `structural_port.py` | Ports names from XD using identical mnemonic fingerprints. |
| `build_symbol_map.py` | Merges high-confidence proposals into `proposed_symbols.txt`. |
| `partition_apply.py` | Splits proposals into mechanical renames vs. wiring-needed leads. |
| `apply_names.py` | Applies an approved rename map to symbols/source. Use carefully. |
| `wire_leads.py` | Handles a narrow class of return-type-only lead wiring. |
| `finalize_leads.py` | Refreshes applied/lead records after wiring. |
| `regen_named_incs.py` | Regenerates `.inc` files for already-renamed functions. |
| `compile_sweep.ps1` | Compile regression sweep for rename batches. |

## Applying Names Safely

Mechanical application is intentionally separate from evidence generation.

```sh
python tools/symbolmap/partition_apply.py
python tools/symbolmap/apply_names.py \
  --map config/GC6E01/symbolmap/applied_symbols.txt \
  --symbols --source
```

Before applying:

- Read `proposed_symbols.txt`.
- Diff `symbols.with_proposals.txt` against `symbols.txt`.
- Check for typed prototypes or source declarations that would conflict.
- Prefer a `// Proposed:` comment when evidence is good but typing is not done.
- Run compile checks and focused `match_test.py` after edits.

Why this is byte-safe when done correctly: the byte-match build is based on
dtk-extracted asm objects, and the DOL has no symbol table. A rename by itself is
byte-neutral. Source-level typing changes are still real decomp work and must be
verified normally.

## Regenerating `.inc` After A Rename

The stock wrapper regeneration scripts mostly recognize `fn_` wrappers. For
renamed functions, use the name-aware script:

```sh
ninja build/GC6E01/ok
python tools/symbolmap/regen_named_incs.py --all
```

## Practical Guidance For Current Work

For targeted areas like Card-E:

- Do not rename the whole project to chase one function.
- Use `symbols.txt` `// Proposed:` comments for strong local findings.
- Keep low-confidence fields as `unk_XX` in partial structs.
- Use `tu_attribution.md` to confirm file/range ownership.
- Use `name_proposals.md`, `xd_port.md`, and `structural_ports.json` as leads,
  then verify against code, offsets, call signatures, and objdiff/match output.

That makes this folder a research aid and audit trail for naming, not a license
to bulk-rename unknown code.
