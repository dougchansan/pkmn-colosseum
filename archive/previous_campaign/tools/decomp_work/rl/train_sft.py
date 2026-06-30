#!/usr/bin/env python3
"""train_sft.py — SFT fine-tune qwen2.5-coder-7b on CW-matched C function bodies.

Trains model to produce CW-matching C given project + flags + file context.
The idea: after SFT, the model will have internalized CW 1.3 idioms across
70k+ matched functions from 10 projects. At inference time, prompt it with
CW_QUIRKS + asm + existing externs, expect cleaner draft output.

Usage:
    source /storage/finetune/llm4decompile/venv/bin/activate
    HF_TOKEN=$(cat /storage/finetune/llm4decompile/hf_token) \
    python3 train_sft.py \
        --dataset /storage/finetune/rl/combined.jsonl \
        --output /storage/finetune/sft/qwen7b-cw \
        --base Qwen/Qwen2.5-Coder-7B-Instruct \
        --epochs 1 \
        --lr 1e-5 \
        --batch_size 1 \
        --grad_accum 16

Expected: ~24-48h for 1 epoch on 70k examples (qwen-7b, LoRA r=32) on 3090.
"""
from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

PROMPT_TEMPLATE = (
    "You are decompiling a CodeWarrior-compiled PowerPC binary. "
    "Write a matching C function body that CW 1.3 would compile to "
    "byte-identical assembly.\n\n"
    "Project: {project}\n"
    "File: {file}\n"
    "Library: {lib}\n"
    "Compile flags: {cflags_name}\n\n"
    "Matching C function:\n"
)


def format_example(record: dict) -> str:
    """Build the (prompt + completion) text for SFT."""
    prompt = PROMPT_TEMPLATE.format(
        project=record.get("project", ""),
        file=record.get("file", ""),
        lib=record.get("lib", ""),
        cflags_name=record.get("cflags_name", ""),
    )
    completion = record.get("function_body", "")
    return prompt + completion


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dataset", type=Path, required=True)
    ap.add_argument("--output", type=Path, required=True)
    ap.add_argument("--base", default="Qwen/Qwen2.5-Coder-7B-Instruct")
    ap.add_argument("--epochs", type=int, default=1)
    ap.add_argument("--lr", type=float, default=1e-5)
    ap.add_argument("--batch_size", type=int, default=1)
    ap.add_argument("--grad_accum", type=int, default=16)
    ap.add_argument("--max_length", type=int, default=2048)
    ap.add_argument("--lora_r", type=int, default=32)
    ap.add_argument("--save_steps", type=int, default=500)
    args = ap.parse_args()

    # Lazy imports
    import torch
    from datasets import Dataset
    from transformers import (
        AutoModelForCausalLM, AutoTokenizer,
        BitsAndBytesConfig, TrainingArguments, Trainer,
        DataCollatorForLanguageModeling,
    )
    from peft import LoraConfig, get_peft_model, prepare_model_for_kbit_training

    # Load dataset
    print(f"Loading {args.dataset}...", flush=True)
    records = [json.loads(l) for l in args.dataset.read_text(encoding="utf-8").splitlines() if l.strip()]
    print(f"  {len(records)} records", flush=True)
    texts = [format_example(r) for r in records]
    ds = Dataset.from_dict({"text": texts})

    # Tokenizer
    print(f"Loading tokenizer {args.base}...", flush=True)
    tok = AutoTokenizer.from_pretrained(args.base)
    if tok.pad_token is None:
        tok.pad_token = tok.eos_token

    def tokenize(examples):
        out = tok(examples["text"], truncation=True, max_length=args.max_length, padding=False)
        out["labels"] = out["input_ids"].copy()
        return out

    print("Tokenizing...", flush=True)
    ds_tok = ds.map(tokenize, batched=True, remove_columns=["text"], num_proc=4)

    # Model (4-bit QLoRA to fit 7B on 24GB with room for gradients)
    print(f"Loading model 4-bit...", flush=True)
    bnb = BitsAndBytesConfig(
        load_in_4bit=True,
        bnb_4bit_compute_dtype=torch.bfloat16,
        bnb_4bit_quant_type="nf4",
        bnb_4bit_use_double_quant=True,
    )
    model = AutoModelForCausalLM.from_pretrained(
        args.base, quantization_config=bnb, device_map="auto", dtype=torch.bfloat16,
    )
    model = prepare_model_for_kbit_training(model)

    peft_cfg = LoraConfig(
        r=args.lora_r, lora_alpha=args.lora_r * 2, lora_dropout=0.05,
        target_modules=["q_proj", "k_proj", "v_proj", "o_proj", "gate_proj", "up_proj", "down_proj"],
        task_type="CAUSAL_LM",
    )
    model = get_peft_model(model, peft_cfg)
    model.print_trainable_parameters()

    training_args = TrainingArguments(
        output_dir=str(args.output),
        num_train_epochs=args.epochs,
        per_device_train_batch_size=args.batch_size,
        gradient_accumulation_steps=args.grad_accum,
        learning_rate=args.lr,
        bf16=True,
        gradient_checkpointing=True,
        gradient_checkpointing_kwargs={"use_reentrant": False},
        logging_steps=10,
        save_steps=args.save_steps,
        save_total_limit=3,
        warmup_ratio=0.03,
        lr_scheduler_type="cosine",
        report_to="none",
        remove_unused_columns=False,
    )

    collator = DataCollatorForLanguageModeling(tokenizer=tok, mlm=False)

    trainer = Trainer(
        model=model,
        args=training_args,
        train_dataset=ds_tok,
        data_collator=collator,
    )
    trainer.train()
    trainer.save_model(str(args.output / "final"))
    tok.save_pretrained(str(args.output / "final"))
    print(f"Saved to {args.output}/final", flush=True)


if __name__ == "__main__":
    main()
