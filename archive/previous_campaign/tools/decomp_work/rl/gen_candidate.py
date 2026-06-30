#!/usr/bin/env python3
"""gen_candidate.py — box-side: load Qwen-7B + the flywheel LoRA adapter and
generate N candidate C decompilations for a given target asm. The dev box drives
this over ssh (provides the asm, consumes the candidates), then seeds permute_match.

usage (on box):
  python gen_candidate.py --adapter /storage/finetune/sft/qwen7b-cw-wins/final \
                          --asm-file /tmp/target.asm --n 4 --temp 0.7
Prints candidates delimited by  =====CANDIDATE k=====  markers.
"""
import argparse, re, sys

SYS = ("You are a GameCube PowerPC decompiler. Convert the target CodeWarrior 1.3 "
       "PPC assembly to byte-matching C89. Use extern labels for SDA globals, never "
       "float literals for sdata2 returns. Output only the C function with any needed externs.")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--adapter", required=True)
    ap.add_argument("--base", default="Qwen/Qwen2.5-Coder-7B-Instruct")
    ap.add_argument("--asm-file", required=True)
    ap.add_argument("--n", type=int, default=4)
    ap.add_argument("--temp", type=float, default=0.7)
    ap.add_argument("--max-new", type=int, default=1200)
    a = ap.parse_args()

    import torch
    from transformers import AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig
    from peft import PeftModel

    asm = open(a.asm_file, encoding="utf-8", errors="replace").read()
    tok = AutoTokenizer.from_pretrained(a.base)
    bnb = BitsAndBytesConfig(load_in_4bit=True, bnb_4bit_compute_dtype=torch.bfloat16,
                             bnb_4bit_quant_type="nf4", bnb_4bit_use_double_quant=True)
    model = AutoModelForCausalLM.from_pretrained(a.base, quantization_config=bnb,
                                                 device_map="auto", dtype=torch.bfloat16)
    model = PeftModel.from_pretrained(model, a.adapter)
    model.eval()

    msgs = [{"role": "system", "content": SYS},
            {"role": "user", "content": f"Decompile this function to byte-matching C:\n\n```\n{asm.strip()}\n```"}]
    prompt = tok.apply_chat_template(msgs, tokenize=False, add_generation_prompt=True)
    ids = tok(prompt, return_tensors="pt").to(model.device)

    for i in range(a.n):
        with torch.no_grad():
            out = model.generate(**ids, max_new_tokens=a.max_new, do_sample=(a.temp > 0),
                                 temperature=max(a.temp, 1e-4), top_p=0.95,
                                 pad_token_id=tok.eos_token_id)
        txt = tok.decode(out[0][ids["input_ids"].shape[1]:], skip_special_tokens=True)
        m = re.search(r"```c?\s*\n(.*?)```", txt, re.S)
        c = (m.group(1) if m else txt).strip()
        print(f"=====CANDIDATE {i}=====")
        print(c)
        print()


if __name__ == "__main__":
    main()
