#!/usr/bin/env python3
"""train_sft_wins.py — QLoRA SFT of Qwen2.5-Coder-7B on the byte-exact win pairs
(ChatML, completion-masked: loss only on the assistant C, not the asm prompt).
The flywheel's proposer. Small high-signal set (~476 on-distribution matches).
"""
import argparse, json
from pathlib import Path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dataset", type=Path, required=True)
    ap.add_argument("--output", type=Path, required=True)
    ap.add_argument("--base", default="Qwen/Qwen2.5-Coder-7B-Instruct")
    ap.add_argument("--epochs", type=float, default=3)
    ap.add_argument("--lr", type=float, default=2e-4)
    ap.add_argument("--batch_size", type=int, default=1)
    ap.add_argument("--grad_accum", type=int, default=16)
    ap.add_argument("--max_length", type=int, default=4096)
    ap.add_argument("--lora_r", type=int, default=32)
    args = ap.parse_args()

    import torch
    from datasets import Dataset
    from transformers import (AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig,
                              TrainingArguments, Trainer, DataCollatorForSeq2Seq)
    from peft import LoraConfig, get_peft_model, prepare_model_for_kbit_training

    recs = [json.loads(l) for l in args.dataset.read_text(encoding="utf-8").splitlines() if l.strip()]
    print(f"{len(recs)} examples", flush=True)

    tok = AutoTokenizer.from_pretrained(args.base)
    if tok.pad_token is None:
        tok.pad_token = tok.eos_token
    if tok.chat_template is None:  # base models lack one -> use ChatML so apply_chat_template works
        tok.chat_template = (
            "{% for m in messages %}{{'<|im_start|>' + m['role'] + '\n' + m['content'] + '<|im_end|>\n'}}{% endfor %}"
            "{% if add_generation_prompt %}{{'<|im_start|>assistant\n'}}{% endif %}"
        )
    ML = args.max_length

    def build(rec):
        msgs = rec["messages"]
        full = tok.apply_chat_template(msgs, tokenize=False)
        prompt = tok.apply_chat_template(msgs[:-1], tokenize=False, add_generation_prompt=True)
        full_ids = tok(full, truncation=True, max_length=ML, add_special_tokens=False)["input_ids"]
        prompt_ids = tok(prompt, truncation=True, max_length=ML, add_special_tokens=False)["input_ids"]
        plen = min(len(prompt_ids), len(full_ids))
        labels = [-100] * plen + full_ids[plen:]
        return {"input_ids": full_ids, "attention_mask": [1] * len(full_ids), "labels": labels}

    ds = Dataset.from_list([build(r) for r in recs])
    trained = sum(sum(1 for x in e if x != -100) for e in ds["labels"]) // max(1, len(ds))
    print(f"avg completion (trained) tokens/example: {trained}", flush=True)

    bnb = BitsAndBytesConfig(load_in_4bit=True, bnb_4bit_compute_dtype=torch.bfloat16,
                             bnb_4bit_quant_type="nf4", bnb_4bit_use_double_quant=True)
    model = AutoModelForCausalLM.from_pretrained(args.base, quantization_config=bnb,
                                                 device_map="auto", dtype=torch.bfloat16)
    model = prepare_model_for_kbit_training(model)
    model = get_peft_model(model, LoraConfig(
        r=args.lora_r, lora_alpha=args.lora_r * 2, lora_dropout=0.05,
        target_modules=["q_proj", "k_proj", "v_proj", "o_proj", "gate_proj", "up_proj", "down_proj"],
        task_type="CAUSAL_LM"))
    model.print_trainable_parameters()
    model.config.use_cache = False

    targs = TrainingArguments(
        output_dir=str(args.output), num_train_epochs=args.epochs,
        per_device_train_batch_size=args.batch_size, gradient_accumulation_steps=args.grad_accum,
        learning_rate=args.lr, lr_scheduler_type="cosine", warmup_ratio=0.05,
        logging_steps=5, save_strategy="epoch", bf16=True, gradient_checkpointing=True,
        gradient_checkpointing_kwargs={"use_reentrant": False}, report_to="none",
        optim="paged_adamw_8bit", dataloader_num_workers=2)
    trainer = Trainer(model=model, args=targs, train_dataset=ds,
                      data_collator=DataCollatorForSeq2Seq(tok, label_pad_token_id=-100, padding=True))
    trainer.train()
    out = args.output / "final"
    model.save_pretrained(str(out))
    tok.save_pretrained(str(out))
    print(f"SAVED adapter -> {out}", flush=True)


if __name__ == "__main__":
    main()
