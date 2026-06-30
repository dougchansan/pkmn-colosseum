#!/usr/bin/env python3
"""train_grpo.py — GRPO fine-tune qwen2.5-coder-7b for CW byte-match.

Approach: each training step samples a function from the dataset, generates N
candidate C bodies, computes match% via reward_fn, and applies GRPO update.

Assumes the 3090 box has the repo synced to /storage/finetune/pkmn-colosseum
(or accessible path), and the dataset is at /storage/finetune/rl/dataset.jsonl.

Hardware: 24GB VRAM fits qwen-7b + LoRA + rollout buffer at 4-bit base.
Training ~1000 steps @ ~30-60s/step = 8-16 hours.

Usage (on 3090):
    cd /storage/finetune/rl
    source venv/bin/activate
    python3 train_grpo.py --repo_path /storage/finetune/pkmn-colosseum \\
                          --dataset /storage/finetune/rl/dataset.jsonl \\
                          --output /storage/finetune/rl/checkpoints

Install (one-time):
    pip install trl peft bitsandbytes transformers datasets accelerate
"""
from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

# Import local reward_fn (must be alongside this script)
sys.path.insert(0, str(Path(__file__).resolve().parent))


def build_reward_fn(repo_path: Path):
    """Returns a TRL-compatible reward function closure."""
    # Path setup: reward_fn.py uses REPO_ROOT = ..parent.parent.parent.parent from its own location
    os.environ["PKMN_REPO_ROOT"] = str(repo_path)
    # We import the module fresh; its REPO_ROOT resolution uses its file path
    # For flexibility, we monkeypatch below.
    import reward_fn
    reward_fn.REPO_ROOT = repo_path  # type: ignore

    def reward_fn_trl(completions: list[str], **kwargs) -> list[float]:
        # kwargs contain "stem" and "fn" from the dataset columns
        stems = kwargs.get("stem", [])
        fns = kwargs.get("fn", [])
        rewards = []
        for c, stem, fn in zip(completions, stems, fns):
            r, pct, ok = reward_fn.compute_reward(stem, fn, c)
            # Lower reward floor: 0 if <60% (likely unusable), otherwise match%
            if not ok or pct < 60.0:
                rewards.append(0.0)
            else:
                # Bonus for >=95%, heavy weight on matching
                rewards.append(r + (0.2 if pct >= 95.0 else 0.0))
        return rewards
    return reward_fn_trl


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--repo_path", type=Path, required=True)
    ap.add_argument("--dataset", type=Path, required=True)
    ap.add_argument("--output", type=Path, required=True)
    ap.add_argument("--base_model", default="Qwen/Qwen2.5-Coder-7B-Instruct")
    ap.add_argument("--num_generations", type=int, default=4)
    ap.add_argument("--max_steps", type=int, default=1000)
    ap.add_argument("--batch_size", type=int, default=1)
    args = ap.parse_args()

    # Lazy imports so the script can be inspected without torch installed
    from datasets import Dataset
    from trl import GRPOConfig, GRPOTrainer
    from peft import LoraConfig

    # Load dataset
    records = [json.loads(l) for l in args.dataset.read_text().splitlines() if l.strip()]
    print(f"Loaded {len(records)} training pairs", flush=True)

    ds = Dataset.from_list([
        {
            "prompt": r["prompt"],
            "stem": r["stem"],
            "fn": r["fn"],
        }
        for r in records
    ])

    reward_fn_trl = build_reward_fn(args.repo_path)

    config = GRPOConfig(
        output_dir=str(args.output),
        num_generations=args.num_generations,
        per_device_train_batch_size=args.batch_size,
        max_steps=args.max_steps,
        learning_rate=1e-5,
        bf16=True,
        gradient_checkpointing=True,
        save_steps=50,
        logging_steps=1,
        max_prompt_length=8192,
        max_completion_length=1024,
        temperature=0.7,
    )

    peft_config = LoraConfig(
        r=16, lora_alpha=32, lora_dropout=0.05,
        target_modules=["q_proj", "k_proj", "v_proj", "o_proj"],
        task_type="CAUSAL_LM",
    )

    trainer = GRPOTrainer(
        model=args.base_model,
        reward_funcs=[reward_fn_trl],
        args=config,
        train_dataset=ds,
        peft_config=peft_config,
    )
    trainer.train()


if __name__ == "__main__":
    main()
