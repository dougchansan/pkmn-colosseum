# Model benchmarking — 2026-04-25 (post-expert-prompt)

## TL;DR

Switching from the **compact** prompt (~2KB) to the **expert** prompt (~24KB) flipped
results from **0/8 PASS across all cloud models** to **7/8 PASS for both Kimi K2.6 and
DeepSeek V4-pro** on the hard `gs_field_world.c` benchmark suite. The expert prompt
inlines `CW_QUIRKS.md`, `few_shot_examples.md`, `REFERENCE_PROJECTS.md`, peer functions
from the same TU, and all existing externs (~24KB / ~6k tokens per call).

**Recommended primary for complex decomp: `deepseek/deepseek-v4-pro` or `moonshot/kimi-k2.6`.**
Both 7/8 PASS, ~69% mean match. Tie-breaker: cost — Kimi K2.6 is faster (68s vs 78s) and
on Moonshot's tier the user already has access; V4-pro is reasoning-mode (better on the
hardest function) at higher per-call cost.

**Recommended cheap/fast baseline: `deepseek/deepseek-v4-flash`** with `thinking: disabled`.
6/8 PASS at 36s total — half the wall time, 1/3 the cost. Use this for first-shot drafts
that Opus/Kimi can repair.

## Expert-prompt single-shot leaderboard (gs_field_world, 8 functions)

| Model | PASS | mean% | total time | notes |
|---|---|---|---|---|
| `moonshot/kimi-k2.6` | **7/8** | 68.7% | 68s | Best on fn_801233F4 (89.5%) |
| `deepseek/deepseek-v4-pro` | **7/8** | 68.9% | 78s | Best on fn_801231A4 (95.9%); reasoning model |
| `deepseek/deepseek-v4-flash` | 6/8 | 59.8% | 36s | Fastest + cheapest; misses fn_801231A4 |
| `openrouter/qwen/qwen3.6-plus` | 2/8 | 18.8% | 357s | Rate-limited after 2 calls; not viable in production |

## Per-function match%

| Function | V4-flash | V4-pro | Kimi K2.6 | Qwen 3.6+ | best |
|---|---|---|---|---|---|
| `fn_8011BA0C` | 91.3% | 85.9% | 91.3% | 91.3% | 91.3% (3-way tie) |
| `fn_80120B00` | 43.7% | 43.7% | 40.4% | 58.7% | Qwen 58.7% |
| `fn_80120C6C` | 0% | 0% | 0% | 0% | 0% (stuck — none compile) |
| `fn_80122BC0` | 77.1% | 72.1% | 55.0% | 0% | V4-flash 77.1% |
| `fn_801231A4` | 0% | 95.9% | 95.1% | 0% | V4-pro 95.9% |
| `fn_801233F4` | 86.7% | 80.1% | 89.5% | 0% | Kimi 89.5% |
| `fn_80125390` | 88.6% | 82.3% | 87.2% | 0% | V4-flash 88.6% |
| `fn_80129F20` | 91.2% | 91.2% | 91.2% | 0% | 91.2% (3-way tie) |

## Key insights

1. **Reasoning models matter for complex multi-state functions.** `fn_801231A4`
   (return-with-state-flag pattern) hits 95% on V4-pro and Kimi K2.6 (both reasoning) but
   0% on V4-flash with thinking disabled. V4-flash with thinking enabled would likely
   recover this — pre-condition: bump `max_tokens` to ≥4096.
2. **fn_80120C6C is unanimously stuck at 0%.** Probably an idiom none of the cloud
   models understand from the available context. Candidate for human-Opus or
   best-of-N + repair.
3. **Qwen 3.6 Plus on OpenRouter is rate-limited.** After the second function it
   stalled out (0.0s "no-code" responses). Not usable for production benchmarking.
4. **Cost is dominated by prompt-cache hit ratio.** DeepSeek API auto-caches matching
   prefixes; we observed 8576 / 8668 = 99% prompt cache hit on V4-flash. At V4 pricing
   (~$0.27/M input, ~$1.10/M output), the entire 8-function sweep cost <$0.01.

## Infrastructure changes that unlocked the result

1. **`reward_fn.py` revert fix** — replaced `git checkout` with file-snapshot revert.
   The 3090 working tree at `/storage/finetune/pkmn-colosseum` has no `.git/`, so
   reverts were silently failing, leaving each .c file mutated after the first call.
   This is the single biggest reason all prior cloud benchmarks were 0/8.
2. **`bench_opencode.py`** — added `thinking: {type: "disabled"}` for DeepSeek V4
   models (otherwise a 1024 max_tokens budget is fully consumed by `reasoning_content`
   with empty `content`). Also added `reasoning_content` fallback for any thinking
   model whose final content slot is empty.
3. **Expert prompt suite** — `make_expert_suite.py` produces ~24KB-per-test prompts
   that include CW_QUIRKS, few_shot_examples, REFERENCE_PROJECTS, peer functions, and
   externs. The compact prompt (~2KB) gave 0/8.

## Routing matrix (all confirmed working 2026-04-25)

| Surface | Provider config | Status |
|---|---|---|
| DeepSeek V4 Flash (direct) | `deepseek/deepseek-v4-flash` | ✅ — uses `openrouterkey.txt:deepseek` |
| DeepSeek V4 Pro (direct) | `deepseek/deepseek-v4-pro` | ✅ |
| DeepSeek V4 Flash (via OpenRouter) | `openrouter/deepseek/deepseek-v4-flash` | ✅ — uses `openrouterkey.txt:openrouter` |
| Kimi K2.6 (Moonshot direct) | `moonshot/kimi-k2.6` | ✅ |
| Qwen 3.6 Plus (via OpenRouter) | `openrouter/qwen/qwen3.6-plus` | ⚠️ rate-limited |
| Local DeepSeek V4 (3090) | n/a | ❌ V4 weights not available locally; deepseek-coder-v2:16b is the largest fit and was previously verified non-functional |

## Historical: archived compact-prompt baseline (for reference)

The same 8-function suite under the **compact** prompt (~2KB) — pre-expert-prompt — gave
**0/8 PASS** for `moonshot/kimi-k2.6` and `llamacpp-proxmox/qwen3.6-35b-a3b`. Output was
plausible but never compiled — register-name variables, hallucinated jumptables, missing
externs. The richer prompt closes that gap dramatically.
