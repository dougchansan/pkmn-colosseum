# Handoff to Codex (gpt-5.4): Continue RL training via GRPO

Copy everything below the `---` as your prompt to Codex.

---

You are continuing a CW 1.3 byte-match decompilation project for Pokémon Colosseum (GameCube). The previous session (Claude Opus 4.7) built the full training scaffolding and completed an SFT (supervised fine-tune) phase. **Your job is to run the next phase: GRPO reinforcement learning with compile-match reward, producing a LoRA adapter that outputs byte-matching C for CW-compiled PPC functions.**

## Context (what Opus already built)

Repo: `C:/Users/douglaswhittingham/pkmn-colosseum` on Windows workstation.
GPU box: `ssh douglaswhittingham@10.0.0.3` (RTX 3090 24GB). SSH key auth set up. No password needed.

Key files:
- `tools/decomp_work/CW_QUIRKS.md` — CW 1.3 byte-match cheatsheet.
- `tools/decomp_work/rl/SFT_TRAINING_RESULTS.md` — SFT phase findings (READ THIS FIRST).
- `tools/decomp_work/rl/train_grpo.py` — GRPO scaffolding (already committed).
- `tools/decomp_work/rl/reward_fn.py` — compile+match reward function.
- `tools/decomp_work/rl/prepare_dataset.py` — builds training pairs from repo's matched functions.

3090 state:
- `/storage/finetune/sft/qwen7b-cw/final/` — SFT adapter (308MB LoRA, trained 9h on 70,585 pairs from 10 GC/Wii decomps). This is your RL warm-start.
- `/storage/finetune/rl/combined.jsonl` — 70,585 matched CW function bodies (SFT dataset, not used for RL).
- `/storage/finetune/llm4decompile/venv/` — Python env with torch/transformers/bitsandbytes/peft/trl (usable; add anything missing).
- `/storage/finetune/llm4decompile/hf_token` — your HF token for model downloads.
- `/storage/finetune/decomps/` — 10 cloned decomp repos (225MB).

## Your job, in order

### Step 1: Build a proper (asm, C) RL dataset

Current `prepare_dataset.py` emits training pairs from THIS repo (pkmn-colosseum). The function bodies live in `src/game/*.c` under `#if 0 / #else / <C body>` blocks. The paired asm is in `src/game/<stem>_<fn>.inc`.

Run it:

```bash
# On Windows workstation
cd C:/Users/douglaswhittingham/pkmn-colosseum
python3 tools/decomp_work/rl/prepare_dataset.py > /tmp/rl_dataset.jsonl
# Should emit ~160 records, one per 100%-matched function
wc -l /tmp/rl_dataset.jsonl
```

If the extractor errors (Opus encountered a threading/encoding bug during dry-run), debug it. Most likely: add UTF-8 stdout wrapper like other scripts in that dir have, or fix latin-1 decoding on subprocess output.

Transfer to 3090:
```bash
scp /tmp/rl_dataset.jsonl douglaswhittingham@10.0.0.3:/storage/finetune/rl/pkmn_colosseum.jsonl
```

### Step 2: Make the reward function work on the 3090

`reward_fn.py` assumes the repo is available locally with compile/match toolchain installed. The 3090 (Linux) currently has the matched source cloned but probably NOT the CW compiler binaries (they're Windows .exe in our case — `tools/mwcc_compiler/GC/1.3/mwcceppc.exe`).

Options:
1. **Simplest**: run RL on Windows workstation (requires GPU access from there — probably not an option).
2. **Port toolchain**: install `wine` on 3090, run `mwcceppc.exe` under wine, verify it produces identical `.o` output. Opus verified the toolchain works on Windows but hasn't tested wine compatibility.
3. **Skip the reward compile**: use SFT-only and evaluate manually after. Skips the RL win entirely — not recommended.

Recommended path: **option 2**. Install wine, copy the repo to 3090 via rsync, run `python3 tools/compile_check.py src/game/gs_pokemon_summary.c` as sanity check. Expect to debug PATH/shell issues with wine calling PPC compiler. Budget 2-4 hours for this.

If wine proves too fragile, fall back to a simpler reward: use `objdiff-cli` to compare the candidate C against the existing matched C via text similarity — not byte-matching, but a usable proxy. Opus didn't try this path, so you'd be pioneering.

### Step 3: Adjust `train_grpo.py` to use the SFT adapter as base policy

Current `train_grpo.py` uses `Qwen/Qwen2.5-Coder-7B-Instruct` as base. Change it to load the SFT adapter on top:

```python
from peft import PeftModel
base = AutoModelForCausalLM.from_pretrained('Qwen/Qwen2.5-Coder-7B-Instruct', quantization_config=bnb, ...)
policy = PeftModel.from_pretrained(base, '/storage/finetune/sft/qwen7b-cw/final')
# Then add a NEW LoRA on top for RL:
policy.add_adapter('rl', peft_config)
policy.set_adapter('rl')
```

This preserves the SFT knowledge and lets GRPO update a second LoRA. Alternative: merge SFT adapter into base first, then add fresh LoRA for RL. Either works; adapter-stacking is more flexible.

### Step 4: Launch GRPO training

```bash
ssh douglaswhittingham@10.0.0.3
cd /storage/finetune/rl
source /storage/finetune/llm4decompile/venv/bin/activate
export HF_TOKEN=$(cat /storage/finetune/llm4decompile/hf_token)
export HF_HOME=/storage/finetune/llm4decompile/hf_cache

nohup python3 train_grpo.py \
    --repo_path /storage/finetune/pkmn-colosseum \
    --dataset /storage/finetune/rl/pkmn_colosseum.jsonl \
    --output /storage/finetune/grpo/qwen7b-cw-rl \
    --base_model Qwen/Qwen2.5-Coder-7B-Instruct \
    --sft_adapter /storage/finetune/sft/qwen7b-cw/final \
    --num_generations 4 \
    --max_steps 500 \
    --batch_size 1 \
    > /storage/finetune/grpo/train.log 2>&1 &
```

Expected runtime: 2-3 days. Each step = 4 rollouts × (generate ~600 tokens + compile + match) ≈ 60-120s per step. 500 steps = 8-16 hours, possibly longer if compile is slow.

Monitor: `tail -f /storage/finetune/grpo/train.log`. Watch for reward trend (should increase over first 100 steps). If reward stays at 0, the reward function is broken — debug before letting it run longer.

### Step 5: Evaluate the trained model

After training (or at a mid-training checkpoint), test on unmatched Colosseum functions:

```bash
# On Windows
cd C:/Users/douglaswhittingham/pkmn-colosseum
python3 tools/decomp_work/build_prompt.py gs_field_world fn_8011BAC0 > /tmp/prompt.md

# On 3090
scp /tmp/prompt.md douglaswhittingham@10.0.0.3:/tmp/prompt.md
ssh douglaswhittingham@10.0.0.3 "source /storage/finetune/llm4decompile/venv/bin/activate && python3 /storage/finetune/rl/eval_fn.py /tmp/prompt.md --adapter /storage/finetune/grpo/qwen7b-cw-rl/checkpoint-500"

# Eval template is at /storage/finetune/rl/eval_fn.py (Opus wrote it)
# Copy output into src/game/gs_field_world.c #else block, compile, measure
```

If trained model produces byte-matching C on 2+ unmatched Colosseum functions, the RL phase is validated. Commit wins to master.

## Rules for you (Codex)

1. **Read `tools/decomp_work/rl/SFT_TRAINING_RESULTS.md` FIRST** — it explains the SFT gap and what RL needs to fix.
2. **Never commit below 90% match** — asm wrappers are a free 100%, anything worse is a regression.
3. **Always `rm -f build/GC6E01/base/game/<stem>.o` before re-measuring** — stale builds give fake scores (Opus was burned by Sonnet hallucinating 100% due to stale .o).
4. **Run match_scan_file.py TWICE** back-to-back — identical output means trustworthy measure.
5. **Paste raw match_scan output in commit messages** — not summaries.
6. **When encountering Edit/tool errors** about "file modified since read" — just re-read and retry. Never stop the autonomous loop on this.
7. **Work on worktree `wt-scene-init-b`** (already at master) to avoid conflicting with any parallel work.

## Expected outcome

If everything works, you'll produce an RL-trained LoRA adapter that, when applied to qwen-7b, generates byte-matching C for unseen Colosseum functions at 80-95% success rate (up from the SFT model's current "produces real C but wrong semantics" baseline).

If it doesn't work, document the failure mode — whether it's reward signal, model capacity, dataset size, or something else — so next iteration has better information.

Start with step 1. Report progress each phase.
