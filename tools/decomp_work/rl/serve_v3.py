#!/usr/bin/env python3
"""serve_v3.py — persistent inference server for the SeedCoder-8B-v3 CW-decomp
adapter. Loads base+LoRA ONCE and serves POST /gen {asm, n, temp} -> candidates,
so the fleet's seed lane can request drafts over HTTP without a 38s model reload
per function. Run on the 3090 (GPU idle post-training):

  source /storage/finetune/llm4decompile/venv/bin/activate
  python serve_v3.py --adapter /storage/finetune/sft/seedcoder8b-cw-v3/final \
                     --base ByteDance-Seed/Seed-Coder-8B-Instruct --port 8780
"""
import argparse, json, re
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

SYS = ("You are a GameCube PowerPC decompiler. Convert the target CodeWarrior 1.3 "
       "PPC assembly to byte-matching C89. Use extern labels for SDA globals, never "
       "float literals for sdata2 returns. Output only the C function with any needed externs.")

TOK = MODEL = None


def load(base, adapter):
    global TOK, MODEL
    import torch
    from transformers import AutoModelForCausalLM, AutoTokenizer, BitsAndBytesConfig
    from peft import PeftModel
    TOK = AutoTokenizer.from_pretrained(base)
    bnb = BitsAndBytesConfig(load_in_4bit=True, bnb_4bit_compute_dtype=torch.bfloat16,
                             bnb_4bit_quant_type="nf4", bnb_4bit_use_double_quant=True)
    m = AutoModelForCausalLM.from_pretrained(base, quantization_config=bnb,
                                             device_map="auto", dtype=torch.bfloat16)
    MODEL = PeftModel.from_pretrained(m, adapter)
    MODEL.eval()
    print("[serve_v3] model loaded, ready")


def generate(asm, n=4, temp=0.6, max_new=1200):
    import torch
    msgs = [{"role": "system", "content": SYS},
            {"role": "user", "content": f"Decompile this function to byte-matching C:\n\n```\n{asm.strip()}\n```"}]
    prompt = TOK.apply_chat_template(msgs, tokenize=False, add_generation_prompt=True)
    ids = TOK(prompt, return_tensors="pt").to(MODEL.device)
    cands = []
    for _ in range(n):
        with torch.no_grad():
            out = MODEL.generate(**ids, max_new_tokens=max_new, do_sample=(temp > 0),
                                 temperature=max(temp, 1e-4), top_p=0.95,
                                 pad_token_id=TOK.eos_token_id)
        txt = TOK.decode(out[0][ids["input_ids"].shape[1]:], skip_special_tokens=True)
        m = re.search(r"```c?\s*\n(.*?)```", txt, re.S)
        cands.append((m.group(1) if m else txt).strip())
    return cands


class H(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def do_GET(self):
        self._send(200, {"ok": True, "model": "seedcoder8b-cw-v3"})

    def do_POST(self):
        try:
            n = int(self.headers.get("Content-Length", 0))
            req = json.loads(self.rfile.read(n) or b"{}")
            cands = generate(req["asm"], int(req.get("n", 4)),
                             float(req.get("temp", 0.6)), int(req.get("max_new", 1200)))
            self._send(200, {"candidates": cands})
        except Exception as e:
            self._send(500, {"error": str(e)})

    def _send(self, code, obj):
        body = json.dumps(obj).encode()
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--adapter", required=True)
    ap.add_argument("--base", default="ByteDance-Seed/Seed-Coder-8B-Instruct")
    ap.add_argument("--host", default="0.0.0.0")
    ap.add_argument("--port", type=int, default=8780)
    a = ap.parse_args()
    load(a.base, a.adapter)
    print(f"[serve_v3] http://{a.host}:{a.port}/  POST /gen {{asm,n,temp}}")
    ThreadingHTTPServer((a.host, a.port), H).serve_forever()


if __name__ == "__main__":
    main()
