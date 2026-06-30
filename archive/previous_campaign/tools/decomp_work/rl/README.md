# RL training for CW 1.3 byte-match

**Status**: scaffolding complete. Not yet run.

**Goal**: fine-tune `qwen2.5-coder-7b` via GRPO with reward = actual match% from
`match_scan_file.py`. The model learns CW 1.3's register allocation and
scheduling idiosyncrasies through trial-and-error feedback — patterns no
pretraining dataset contains.

## Files

| File | Purpose |
|---|---|
| `prepare_dataset.py` | Scans repo for ≥95% matched functions, emits JSONL training pairs (prompt + ground-truth C + asm). |
| `reward_fn.py` | Callable from training loop. Swaps candidate C into the source tree, compiles, measures match%, reverts. Returns reward in [0,1]. Serialized via flock. |
| `train_grpo.py` | TRL GRPO trainer with LoRA (16-rank). Samples N candidates per step, computes rewards, updates model. |

## Setup (one-time, on 3090)

```bash
# SSH in
ssh douglaswhittingham@10.0.0.3

# Prep workdir on /storage (not / — disk full)
mkdir -p /storage/finetune/rl
cd /storage/finetune/rl

# Reuse the llm4decompile venv if it has torch + transformers already
source /storage/finetune/llm4decompile/venv/bin/activate
# Or create fresh:
# python3 -m venv venv && source venv/bin/activate

export PIP_CACHE_DIR=/storage/finetune/pip-cache
export TMPDIR=/storage/finetune/tmp
pip install trl peft datasets accelerate

# Clone the repo (first time) or sync
git clone https://github.com/<your-fork>/pkmn-colosseum.git /storage/finetune/pkmn-colosseum
# Or rsync from your workstation if the remote isn't set up
```

## Workflow

### Step 1: build the dataset (on Windows workstation, where the repo lives)

```bash
cd C:/Users/douglaswhittingham/pkmn-colosseum
python3 tools/decomp_work/rl/prepare_dataset.py > /tmp/dataset.jsonl
# ~160 pairs if run on current state of repo
```

Then transfer:
```bash
scp /tmp/dataset.jsonl douglaswhittingham@10.0.0.3:/storage/finetune/rl/dataset.jsonl
```

### Step 2: sync the repo to 3090 (needed for compile+match during training)

```bash
# Push from Windows
git push
# On 3090
cd /storage/finetune/pkmn-colosseum && git pull
# Rebuild base objects so compile_check works
python3 tools/compile_check.py src/game/gs_field_world.c   # sanity check
```

**Note**: The 3090 box needs the full build toolchain (mwcceppc, objdiff-cli).
Verify with: `python3 tools/compile_check.py src/game/gs_pokemon_summary.c`.
If it fails, copy the compiler binaries from the Windows box.

### Step 3: run GRPO training

```bash
cd /storage/finetune/rl
source /storage/finetune/llm4decompile/venv/bin/activate
HF_TOKEN=$(cat /storage/finetune/llm4decompile/hf_token) \
python3 /storage/finetune/pkmn-colosseum/tools/decomp_work/rl/train_grpo.py \
    --repo_path /storage/finetune/pkmn-colosseum \
    --dataset /storage/finetune/rl/dataset.jsonl \
    --output /storage/finetune/rl/checkpoints \
    --max_steps 500
```

Expected: 8-16 hours for 500-1000 steps. Each step does:
1. Sample a function prompt from dataset
2. Generate 4 candidate C bodies (rollouts)
3. For each: compile + match_scan → reward
4. GRPO policy update on the rollouts

### Step 4: evaluate

```bash
# After training, run inference with the LoRA adapter
python3 /storage/finetune/rl/eval.py \
    --adapter /storage/finetune/rl/checkpoints/final \
    --test_functions fn_8011791C fn_800166BC fn_80016F14
```

(eval script not yet written — simple adapter-load + ollama-style inference)

## Cost / risk

- **3090 time**: 8-16h training + some eval. Free (your hardware).
- **Claude Opus tokens**: scaffolding cost (this commit) only. Runtime cost = 0.
- **Risk**: may not converge — 160 examples is small. If after 200 steps reward
  isn't trending up, pause and either (a) augment dataset with near-miss
  rollouts or (b) accept that qwen-32B + Opus verification is the better path.

## Known gotchas

- `reward_fn.py` mutates `src/game/<stem>.c` during rollout — flock serializes
  access. Training will be slower because compile+match is sequential per
  stem. Expect 30-60s per rollout. 4 rollouts/step × 500 steps = 1000 rewards
  total.
- If a rollout corrupts the source tree and the revert fails, subsequent
  rewards will cascade. Run under `git worktree` if paranoid.
- qwen-7b will generate raw text that must be parsed for the `#else` block.
  `train_grpo.py` passes completions as-is to the reward function; the
  reward function expects just the C body. Add a simple regex extractor
  between generation and reward if needed.
