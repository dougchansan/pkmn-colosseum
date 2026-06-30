# Cheap model pipeline for CW 1.3 decomp matching

## TL;DR

```bash
# Build a self-contained prompt for any function
python3 tools/decomp_work/build_prompt.py gs_field_world fn_80115C48 > /tmp/prompt.md

# Try any model
ollama run qwen2.5-coder:32b-instruct-q4_K_M   < /tmp/prompt.md > /tmp/reply.c
ollama run deepseek-coder-v2:16b-lite-instruct < /tmp/prompt.md > /tmp/reply.c
ollama run gpt-oss:20b                         < /tmp/prompt.md > /tmp/reply.c

# Paste reply.c into the #else block in src/game/<stem>.c, then:
rm -f build/GC6E01/base/game/<stem>.o
python3 tools/compile_check.py src/game/<stem>.c
python3 tools/match_scan_file.py <stem> fn_80115C48   # RUN TWICE
```

## Three tools, one pipeline

| Tool | Purpose |
|---|---|
| `tools/decomp_work/CW_QUIRKS.md` | Consolidated CW 1.3 byte-match cheatsheet (loop idioms, pragma recipes, skip markers). Fed to every model prompt. |
| `tools/decomp_work/match_helper.py` | Reads a `.inc` file, detects CW-relevant patterns (mtctr/bdnz, cror, fmadds, empty-else, etc.), emits skeleton + specific hints. |
| `tools/decomp_work/build_prompt.py` | Wraps match_helper + existing-extern grep + CW_QUIRKS.md into one prompt file. |

## Model recommendations for a 24GB GPU (RTX 3090)

Ranked by expected quality per VRAM:

| Model | Size | Notes |
|---|---|---|
| `qwen2.5-coder:32b-instruct-q4_K_M` | ~18GB | Strongest code model that fits. Best first try. |
| `qwen2.5-coder:14b-instruct-q6_K` | ~11GB | Faster, still strong. Good for iteration. |
| `deepseek-coder-v2:16b-lite-instruct-q5_K_M` | ~12GB | Code-specialized MoE. Very good at byte-level reasoning. |
| `gpt-oss:20b` | ~12GB | OpenAI's recent open-source release, reasoning-focused. Worth benching. |
| `qwen3-coder:30b-a3b-instruct-q4` | ~17GB | Qwen3 coder variant if available, MoE arch. |
| `gemma-2:9b-instruct-q6_K` | ~7GB | General-purpose, fast. Not code-specialized — weakest on this task. |
| `gemma-3:27b-instruct-q4` | ~15GB | Google's latest. Worth testing but not code-tuned. |

**Already tested (mediocre)**: `gemma3:4b` on proxmox — 7/13 on repo benchmark.

## Recommended benchmark flow

1. Pick 5 known-tractable functions (already matched by Claude Opus):
   - `fn_80115C48` (gs_field_world, 100%) — mtctr/bdnz
   - `fn_8001501C` (gs_pokemon_summary, 100%) — simple call + return
   - `fn_80017868` (gs_pokemon_summary, 100%) — compute + call
   - `fn_8001793C` (gs_pokemon_summary, 100%) — table lookup + 3 stores
   - `fn_800D7894` (gs_render, 99.5%) — free-slot scan + ring counter

2. For each function + each model:
   ```bash
   python3 tools/decomp_work/build_prompt.py <stem> <fn> > /tmp/prompt.md
   ollama run <model> < /tmp/prompt.md > /tmp/reply.c
   # Manually paste reply into test branch, compile, measure match%
   ```

3. Rank models by average match% across the 5 test functions. Pick top-2 for production.

## Automated benchmark script (optional)

To automate the measurement, wrap the test cycle:

```bash
# tools/decomp_work/bench_model.sh
#!/bin/bash
MODEL=$1
TESTS=(
    "gs_field_world fn_80115C48 100.0"
    "gs_pokemon_summary fn_8001501C 100.0"
    "gs_pokemon_summary fn_80017868 100.0"
    "gs_pokemon_summary fn_8001793C 100.0"
    "gs_render fn_800D7894 99.5"
)
for t in "${TESTS[@]}"; do
    set -- $t
    stem=$1; fn=$2; target_pct=$3
    python3 tools/decomp_work/build_prompt.py $stem $fn > /tmp/prompt.md
    ollama run $MODEL < /tmp/prompt.md > /tmp/reply.c
    # ... apply reply, compile, measure, revert ...
done
```

(Deferred — can write this out if the manual flow proves too slow.)

## Pipeline integration with Codex/opencode

Instead of asking Codex to find-and-read everything itself:

```
cat tools/decomp_work/CW_QUIRKS.md > codex_system_prompt.md
# Then for each function:
python3 tools/decomp_work/build_prompt.py <stem> <fn>
# Paste the output as the user prompt.
```

This removes ~90% of the exploration Codex was doing itself. Faster, cheaper, less error.

## Why this should work

Sonnet/Codex weren't failing because they're weak — they were failing because every attempt started from zero. They re-discovered the `mtctr/bdnz` inline-for-init rule each time, or missed it entirely. With `CW_QUIRKS.md` + `match_helper.py` hints in the prompt, ANY reasonably capable code model (14B+ parameters, code-tuned) should hit 85-95% on simple-to-medium functions. The hard ones stay hard — those need Opus-level pattern recognition or manual study regardless.

## What this does NOT solve

- Jump tables, cntlzw, divw, frsqrte — same skip list regardless of model.
- SDA2 literal-pool magic doubles — toolchain-locked at ~93-97%.
- Register-allocation ceiling on large pointer-walk functions.
- Hallucinated match% — still required to verify in parent session before merging.

## Cost comparison (rough)

| Strategy | Per-function cost |
|---|---|
| Opus reads + writes directly | $0.50 - $2 |
| Sonnet subagent (no quirks context) | $0.10 - $0.30, often wrong |
| Codex (gpt-5) with quirks | $0.05 - $0.15 |
| Local Qwen2.5-Coder-32B on 3090 | ~free (electricity) |
| Local gpt-oss:20b on 3090 | ~free |

The local option is ~100x cheaper per attempt. Even at 50% success rate, it's dramatically cheaper than any remote API.
