#!/usr/bin/env python3
"""Lane 1: Ollama bulk generation (FREE — RTX 3090 + local CPU)"""
import json, os, re, sys, time, urllib.request
from pathlib import Path

ROOT = Path(__file__).parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))

OLLAMA_GPU_HOST = os.environ.get("DECOMP_GPU_HOST", "192.168.50.101")
OLLAMA_GPU_PORT = os.environ.get("DECOMP_OLLAMA_PORT", "11434")
OLLAMA_GPU = f"http://{OLLAMA_GPU_HOST}:{OLLAMA_GPU_PORT}/api/generate"
OLLAMA_LOCAL = "http://localhost:11434/api/generate"
GPU_MODEL = "codestral:22b"
LOCAL_MODEL = "qwen3:32b"
KIMI_URL = "https://api.moonshot.ai/v1/chat/completions"
KIMI_KEY = os.environ.get("KIMI_API_KEY", "")

QUEUE = ROOT / "tools" / "decomp_work" / "work_queue.json"
CANDIDATES = ROOT / "tools" / "decomp_work" / "candidates"
LOCKS = ROOT / "tools" / "decomp_work" / "locks"

def call_ollama(prompt, host=OLLAMA_GPU, model=GPU_MODEL):
    payload = json.dumps({"model": model, "prompt": prompt, "stream": False,
                          "options": {"temperature": 0.1, "num_predict": 2048}}).encode()
    req = urllib.request.Request(host, data=payload, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=300) as r:
        return json.loads(r.read()).get("response", "")

def call_kimi(prompt):
    payload = json.dumps({"model": "kimi-k2-turbo-preview",
                          "messages": [{"role": "user", "content": prompt}],
                          "temperature": 0.1, "max_tokens": 2048}).encode()
    req = urllib.request.Request(KIMI_URL, data=payload, headers={
        "Content-Type": "application/json", "Authorization": f"Bearer {KIMI_KEY}"})
    with urllib.request.urlopen(req, timeout=120) as r:
        return json.loads(r.read())["choices"][0]["message"]["content"]

def normalize(code):
    for old, new in [("uint32_t","u32"),("uint16_t","u16"),("uint8_t","u8"),
                     ("int32_t","s32"),("int16_t","s16"),("int8_t","s8"),
                     ("uintptr_t","u32"),("size_t","u32")]:
        code = code.replace(old, new)
    code = "\n".join(l for l in code.split("\n") if not l.strip().startswith("#include"))
    code = re.sub(r'__attribute__\s*\([^)]*\)', '', code)
    # Strip thinking tags
    code = re.sub(r'<think>.*?</think>', '', code, flags=re.DOTALL)
    return code.strip()

def extract_c(response):
    if not response: return None
    m = re.search(r'```c?\s*\n(.*?)```', response, re.DOTALL)
    if m: return m.group(1).strip()
    if "void " in response or "return " in response or "extern " in response:
        lines = response.split("\n")
        for i, l in enumerate(lines):
            if l.strip().startswith(("extern ","void ","u32 ","s32 ","f32 ","u8 ","#pragma ")):
                return "\n".join(lines[i:]).strip()
    return None

def build_prompt(wrapper):
    from decomp_scheduler import generate_prompt
    return generate_prompt(wrapper)

def main():
    CANDIDATES.mkdir(parents=True, exist_ok=True)
    LOCKS.mkdir(parents=True, exist_ok=True)

    queue = json.loads(QUEUE.read_text())
    # Filter to simple + medium, sort by size
    targets = [w for w in queue if w["tier"] in ("simple", "medium")]
    targets.sort(key=lambda w: w["asm_lines"])

    # Skip locked or already done
    done_file = ROOT / "tools" / "decomp_work" / "progress.json"
    done = set()
    if done_file.exists():
        done = set(json.loads(done_file.read_text()).get("completed", []))

    print(f"{'='*60}")
    print(f"LANE 1: OLLAMA BULK GENERATION")
    print(f"Targets: {len(targets)} simple+medium functions")
    print(f"GPU model: {GPU_MODEL} @ {OLLAMA_GPU_HOST}")
    print(f"Kimi fallback: kimi-k2-turbo-preview")
    print(f"{'='*60}\n")

    generated = 0
    for w in targets:
        fn = w["function"]
        if fn in done:
            continue
        lock = LOCKS / f"{fn}.lock"
        if lock.exists():
            continue

        # Acquire lock
        lock.write_text(json.dumps({"lane": "ollama", "time": time.time()}))

        print(f"[{generated+1}] {fn} ({w['asm_lines']} lines, {w['tier']})...", end=" ", flush=True)

        try:
            prompt = build_prompt(w)
            if not prompt:
                print("SKIP (no prompt)")
                lock.unlink()
                continue

            # Try GPU codestral first
            start = time.time()
            response = call_ollama(prompt, OLLAMA_GPU, GPU_MODEL)
            elapsed = time.time() - start
            code = extract_c(response)

            if not code or fn not in code:
                # Fallback to Kimi
                print(f"GPU miss ({elapsed:.0f}s), trying Kimi...", end=" ", flush=True)
                start = time.time()
                response = call_kimi(prompt)
                elapsed = time.time() - start
                code = extract_c(response)

            if code:
                code = normalize(code)
                out = CANDIDATES / f"{fn}.json"
                out.write_text(json.dumps({
                    "function": fn,
                    "file": w["file"],
                    "tier": w["tier"],
                    "code": code,
                    "backend": "ollama+kimi",
                    "time": elapsed,
                    "status": "candidate"
                }, indent=2), encoding="utf-8")
                print(f"OK ({elapsed:.1f}s, {len(code)} chars)")
                generated += 1
            else:
                print(f"FAIL (no extractable C)")

        except Exception as e:
            print(f"ERROR: {e}")
        finally:
            lock.unlink(missing_ok=True)

        time.sleep(0.5)  # courtesy pause

    print(f"\n{'='*60}")
    print(f"Generated {generated} candidates in {CANDIDATES}")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()
