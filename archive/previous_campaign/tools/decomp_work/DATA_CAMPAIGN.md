# Data Matching Campaign

Status as of 2026-06-27:

- Public/report data denominator: `1,276,195` loadable non-code bytes.
- Verified matched data: `516` bytes (`4` bytes in `.dtors`, `512` bytes in `.sdata2`).
- First verified source entry: `src/crt_data/__init_cpp_exceptions.c` emits
  `__destroy_global_chain_reference` as a real function pointer relocation.
- Next queue: `.sdata2`, generated at `tools/decomp_work/data_sdata2_worklist.json`.
- Prioritized queue: `tools/decomp_work/data_campaign_queue.json`.
- Daily metrics ledger: `tools/decomp_work/metrics_history.jsonl`.

## Rules

- No `.inc`, `incbin`, raw section dumps, or pasted ROM byte arrays.
- Data only counts after `python3 tools/verify_data_progress.py` passes.
- Prefer typed C declarations: `const f32`, `const f64`, string literals,
  pointer tables, structs, and enums.
- Unknown bytes are not a win. Leave a chunk queued until the data shape is
  understood enough to express as source.
- If a chunk needs alignment padding, emit it through compiler/source layout,
  not copied target bytes.

## Queue Shape

`.sdata2` is currently one target section object:

- target object: `build/GC6E01/obj/auto_09_8047B6A0_sdata2.o`
- section span: `0x8047B6A0..0x8047E700`
- section bytes: `12,384`
- symbol count: `2,190`
- symbol bytes: `11,480`
- padding/unattributed bytes: `904`
- worklist chunks: `49`

Finishing `.sdata2` would move the public data bar from `4` bytes to roughly
`12,388 / 1,276,195` bytes, or about `0.97%`.

## First Tasks

1. Keep `.dtors` green with `python3 tools/verify_data_progress.py`.
2. Recover `.sdata2` chunks from source evidence, starting with numeric CRT math
   chunks that are floats/doubles and have no unknown symbols.
3. Completed first package:
   - `sdata2-014` and `sdata2-015`, `0x8047C4A0..0x8047C6A0`
   - Source: `src/crt_data/sdata2_math.c`
   - Evidence: referenced from `src/crt/extras.c` by `__ieee754_asin`,
     `__ieee754_atan2`, `__ieee754_exp`, `__ieee754_log`, `__ieee754_pow`,
     and nearby fdlibm-style runtime math.
   - Policy followed: typed `f64` constants only; no `.inc`, `incbin`, or raw
     target byte array.
4. Next priority package:
   - `sdata2-016` through `sdata2-018`, `0x8047C6A0..0x8047C99C`
   - Continue the CRT math constant run only after each range is source-owned by
     a split entry and passes `tools/verify_data_progress.py`.
5. Split `.sdata2` into source-replaceable ranges or replace the whole section
   only after enough typed chunks are proven.
6. Add each verified range to `config/GC6E01/data_progress.json`.
7. Regenerate `report.json`, then run `python3 tools/check_report_sanity.py`.

## Useful Commands

```bash
python3 tools/decomp_work/build_data_worklist.py --section .sdata2 --chunk-bytes 256 --output tools/decomp_work/data_sdata2_worklist.json
python3 tools/decomp_work/prioritize_data_worklist.py
python3 tools/verify_data_progress.py
python3 tools/check_report_sanity.py report.json
python3 tools/decomp_work/snapshot_metrics.py
```

## Fleet Lanes

- Codex data lane: edit mode, one source-owned chunk at a time.
- GLM data lane: research-only mode, writes evidence notes for training and later
  Codex application.
- Preferred next edit run: `sdata2-016,sdata2-017,sdata2-018`.

```bash
DATA_PREFER_IDS=sdata2-016,sdata2-017,sdata2-018 tools/decomp_work/data_lane_worker.sh codex
DATA_MODE=research tools/decomp_work/data_lane_worker.sh glm
```
