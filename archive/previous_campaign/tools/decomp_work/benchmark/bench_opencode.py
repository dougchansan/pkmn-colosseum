#!/usr/bin/env python3
"""
bench_opencode.py — Benchmark models against the decomp test suite via direct HTTP API calls.

Usage:
    python bench_opencode.py [--suite path/to/suite.json] [model1] [model2] ...

If no models specified, benchmarks all new models.

Provider routing:
  opencode/*     -> https://opencode.ai/zen/v1  (GitHub Copilot token via auth.json)
  moonshot/*     -> https://api.moonshot.ai/v1  (API key from opencode.json)
  ollama-local/* -> http://127.0.0.1:11434/v1   (no auth)
"""
import json
import os
import socket
import sys
import time
import re
import urllib.request
import urllib.error
from pathlib import Path

DEFAULT_SUITE = Path(__file__).resolve().parent / "test_suite.json"
RESULTS_DIR = Path(__file__).resolve().parent / "results"

OPENCODE_CONFIG = Path.home() / ".config" / "opencode" / "opencode.json"
REPO_ROOT = Path(__file__).resolve().parents[3]
KEY_FILE = REPO_ROOT / "openrouterkey.txt"

# Per-provider timeouts in seconds (ollama local is slow)
PROVIDER_TIMEOUTS = {
    "ollama-local": 300,
    "ollama-proxmox": 180,
    "llamacpp-proxmox": 240,
    "opencode": 120,
    "moonshot": 90,
    "deepseek": 120,
    "mimo": 200,
    "kimi": 200,
    "glm": 240,
    "openrouter": 180,
}

NEW_MODELS = [
    "opencode/nemotron-3-super-free",
    "opencode/minimax-m2.5-free",
    "opencode/gpt-5-nano",
    "moonshot/kimi-latest",
    "ollama-local/devstral-small-2:latest",
]

# Moonshot model name mapping (opencode config name -> actual API model ID)
MOONSHOT_MODEL_MAP = {
    "kimi-latest": "moonshot-v1-128k",
    "kimi-k2-turbo-preview": "moonshot-v1-128k",
    "kimi-k2.6": "kimi-k2.6",
}

MODEL_SPEC_ALIASES = {
    # Historical queue rows and ad-hoc retry commands used these names. Keep
    # accepting them so old artifacts can be replayed against the working APIs.
    "kimi/kimi-k2": "kimi/kimi-k2.6",
    "moonshot/kimi-k2": "moonshot/kimi-k2.6",
    "mimo/mimo-vl-7b": "mimo/mimo-v2.5-pro",
}


def normalize_model_spec(model_spec: str) -> str:
    """Return the currently routable provider/model spec for known stale aliases."""
    return MODEL_SPEC_ALIASES.get(model_spec.strip(), model_spec.strip())


def load_keyfile() -> dict[str, str]:
    """Parse openrouterkey.txt — format `provider: key` per line.
    Returns a dict like {'openrouter': 'sk-or-...', 'deepseek': 'sk-...'}."""
    if not KEY_FILE.exists():
        return {}
    out: dict[str, str] = {}
    for line in KEY_FILE.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or ":" not in line:
            continue
        name, _, value = line.partition(":")
        out[name.strip().lower()] = value.strip()
    return out


_KEYS = load_keyfile()

def get_opencode_zen_token() -> str:
    """
    Get auth token for opencode zen API.
    The zen endpoint (https://opencode.ai/zen/v1) accepts requests with an empty
    bearer token for free-tier models. If OPENCODE_API_KEY env var is set, use it.
    """
    import os
    api_key = os.environ.get("OPENCODE_API_KEY", "")
    if api_key:
        return api_key
    # Free tier: send empty bearer token
    return ""


def get_provider_config(model_spec: str) -> tuple[str, str, str]:
    """
    Parse 'provider/model' spec and return (base_url, api_key, model_id).
    """
    model_spec = normalize_model_spec(model_spec)
    if "/" not in model_spec:
        raise ValueError(f"Model spec must be 'provider/model', got: {model_spec}")

    provider, model_id = model_spec.split("/", 1)

    if provider == "opencode":
        base_url = "https://opencode.ai/zen/v1"
        api_key = get_opencode_zen_token()
        return base_url, api_key, model_id

    config = json.loads(OPENCODE_CONFIG.read_text())

    if provider == "moonshot":
        api_key = config["provider"]["moonshot"]["options"]["apiKey"]
        base_url = config["provider"]["moonshot"]["options"]["baseURL"]
        # Map config model name to actual Moonshot API model ID
        actual_model = MOONSHOT_MODEL_MAP.get(model_id, model_id)
        return base_url, api_key, actual_model

    if provider == "ollama-local":
        base_url = config["provider"]["ollama-local"]["options"]["baseURL"]
        return base_url, "", model_id

    if provider == "ollama-proxmox":
        base_url = config["provider"]["ollama-proxmox"]["options"]["baseURL"]
        return base_url, "", model_id

    if provider == "deepseek":
        api_key = _KEYS.get("deepseek", "")
        if not api_key:
            raise ValueError("DeepSeek API key missing — add `deepseek: sk-...` to openrouterkey.txt")
        return "https://api.deepseek.com/v1", api_key, model_id

    if provider == "mimo":
        api_key = _KEYS.get("mimo", "")
        if not api_key:
            raise ValueError("Mimo API key missing - add `mimo: ...` to openrouterkey.txt")
        return "https://api.xiaomimimo.com/v1", api_key, model_id

    if provider == "kimi":
        api_key = _KEYS.get("kimi", "")
        if not api_key:
            raise ValueError("Kimi API key missing - add `kimi: ...` to openrouterkey.txt")
        return "https://api.moonshot.ai/v1", api_key, model_id

    if provider == "glm":
        api_key = _KEYS.get("glm5.2", "") or _KEYS.get("glm", "")
        if not api_key:
            raise ValueError("GLM API key missing - add `glm5.2: ...` to openrouterkey.txt")
        return "https://api.z.ai/api/paas/v4", api_key, model_id

    if provider == "openrouter":
        api_key = _KEYS.get("openrouter", "")
        if not api_key:
            raise ValueError("OpenRouter API key missing — add `openrouter: sk-or-...` to openrouterkey.txt")
        return "https://openrouter.ai/api/v1", api_key, model_id

    if provider in config.get("provider", {}):
        provider_cfg = config["provider"][provider]
        base_url = provider_cfg["options"]["baseURL"]
        api_key = provider_cfg["options"].get("apiKey", "")
        if isinstance(api_key, str) and api_key.startswith("@file:"):
            # Format: @file:openrouterkey.txt:deepseek
            parts = api_key.split(":")
            if len(parts) >= 3:
                api_key = _KEYS.get(parts[2].lower(), "")
        return base_url, api_key, model_id

    raise ValueError(f"Unknown provider: {provider}")


def call_api(
    base_url: str,
    api_key: str,
    model_id: str,
    prompt: str,
    timeout: int = 120,
    *,
    max_tokens: int = 1024,
    temperature_override: float | None = None,
    extra_body: dict | None = None,
) -> tuple[str, float]:
    """Call an OpenAI-compatible chat completions API. Returns (response_text, elapsed)."""
    temperature = 0.1 if temperature_override is None else temperature_override
    payload_obj = {
        "model": model_id,
        "messages": [{"role": "user", "content": prompt}],
        "max_tokens": max_tokens,
        "temperature": temperature,
    }
    if model_id == "kimi-k2.6":
        payload_obj["temperature"] = 0.6
        payload_obj["thinking"] = {"type": "disabled"}
    lower_model = model_id.lower()
    if model_id != "kimi-k2.6" and (lower_model.startswith("kimi-") or "kimi-k2.7" in lower_model):
        # Kimi code models reject low temperatures and require normal reasoning.
        payload_obj["temperature"] = 1
    if lower_model.startswith("qwen3") or "qwen3.6" in lower_model:
        payload_obj["chat_template_kwargs"] = {"enable_thinking": False}
    if "deepseek-v4" in lower_model or "deepseek/deepseek-v4" in lower_model:
        # DeepSeek V4 is a thinking model by default; disable to match other
        # baselines (1-shot, no chain-of-thought). Raise max_tokens floor for
        # the cases where thinking IS desired.
        payload_obj["thinking"] = {"type": "disabled"}

    # Ollama OpenAI-compat endpoints default to a tiny num_ctx (often 2048).
    # The expert prompt is ~6k tokens; bump explicitly via the `options` field.
    if "11434" in base_url or "18080" in base_url:
        payload_obj.setdefault("options", {})
        payload_obj["options"].setdefault("num_ctx", 16384)
        payload_obj["options"].setdefault("num_predict", max(max_tokens, 1024))
    if extra_body:
        payload_obj.update(extra_body)

    payload = json.dumps(payload_obj).encode()

    headers = {
        "Content-Type": "application/json",
        "Accept": "application/json",
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36",
    }
    if api_key is not None:
        headers["Authorization"] = f"Bearer {api_key}"

    url = base_url.rstrip("/") + "/chat/completions"
    req = urllib.request.Request(url, data=payload, headers=headers)

    start = time.time()
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            data = json.loads(resp.read())
        elapsed = time.time() - start

        # Extract content from OpenAI-compatible response. For reasoning models
        # whose `content` slot is empty (max_tokens consumed by chain-of-thought),
        # fall back to `reasoning_content` so we can still extract code.
        choices = data.get("choices", [])
        if choices:
            msg = choices[0].get("message", {}) or {}
            content = msg.get("content") or ""
            if not content:
                content = msg.get("reasoning_content") or ""
            return content, elapsed
        return "", elapsed

    except urllib.error.HTTPError as e:
        elapsed = time.time() - start
        body = e.read().decode(errors="replace")[:200]
        return f"HTTP_ERROR_{e.code}: {body}", elapsed
    except urllib.error.URLError as e:
        elapsed = time.time() - start
        return f"URL_ERROR: {e.reason}", elapsed
    except (TimeoutError, socket.timeout):
        return "TIMEOUT", time.time() - start
    except Exception as ex:
        return f"ERROR: {ex}", time.time() - start


def run_model(
    model_spec: str,
    prompt: str,
    timeout: int = 0,
    *,
    max_tokens: int = 1024,
    temperature_override: float | None = None,
    extra_body: dict | None = None,
) -> tuple[str, float]:
    """Run a prompt through the appropriate provider. Returns (response, elapsed)."""
    model_spec = normalize_model_spec(model_spec)
    try:
        base_url, api_key, model_id = get_provider_config(model_spec)
    except Exception as ex:
        return f"CONFIG_ERROR: {ex}", 0.0

    provider = model_spec.split("/", 1)[0]
    if timeout <= 0:
        env_key = f"BENCH_{provider.replace('-', '_').upper()}_TIMEOUT"
        env_timeout = os.environ.get(env_key) or os.environ.get("BENCH_PROVIDER_TIMEOUT")
        timeout = int(env_timeout) if env_timeout else PROVIDER_TIMEOUTS.get(provider, 120)

    return call_api(
        base_url,
        api_key,
        model_id,
        prompt,
        timeout,
        max_tokens=max_tokens,
        temperature_override=temperature_override,
        extra_body=extra_body,
    )


def extract_c(response: str) -> str | None:
    """Extract C code from markdown code block or raw text."""
    m = re.search(r'```c?\s*\n(.*?)```', response, re.DOTALL)
    if m:
        return m.group(1).strip()
    # Try to find function definition directly
    m = re.search(
        r'((?:extern\s+.*?;\s*\n)*(?:void|u32|u16|u8|s32|f32|u32\*)\s+fn_\w+\(.*?\)\s*\{.*?\})',
        response, re.DOTALL
    )
    if m:
        return m.group(1).strip()
    return None


def normalize(code: str) -> str:
    """Normalize type names and strip includes."""
    for old, new in [
        ("uint32_t", "u32"), ("uint16_t", "u16"), ("uint8_t", "u8"),
        ("int32_t", "s32"), ("uintptr_t", "u32"), ("size_t", "u32"),
    ]:
        code = code.replace(old, new)
    code = "\n".join(l for l in code.split("\n") if not l.strip().startswith("#include"))
    return code.strip()


def check_structural(generated: str, correct: str) -> bool:
    """Check if generated code structurally matches correct code (normalized whitespace)."""
    if not generated:
        return False
    gen_clean = re.sub(r'\s+', ' ', generated.strip())
    cor_clean = re.sub(r'\s+', ' ', correct.strip())
    return gen_clean == cor_clean


def benchmark_model(model_spec: str, suite: list) -> dict:
    """Benchmark a single model against the test suite."""
    print(f"\n{'='*60}")
    print(f"Benchmarking: {model_spec}")
    print(f"{'='*60}")

    results = []
    for i, test in enumerate(suite):
        fn = test["function"]
        prompt = test["prompt"]
        correct = normalize(test["correct_c"])

        print(f"  [{i+1}/{len(suite)}] {fn}...", end=" ", flush=True)

        response, elapsed = run_model(model_spec, prompt)

        code = extract_c(response)
        if code:
            code = normalize(code)

        status = "FAIL"
        if code and check_structural(code, correct):
            status = "PASS"

        results.append({
            "fn": fn,
            "status": status,
            "time": round(elapsed, 3),
            "code": code or "",
        })
        print(f"{status} ({elapsed:.1f}s)")

    return {
        "model": model_spec,
        "results": results,
        "total_time": round(sum(r["time"] for r in results), 3),
    }


def main():
    args = sys.argv[1:]
    suite_path = DEFAULT_SUITE
    models = []

    i = 0
    while i < len(args):
        arg = args[i]
        if arg == "--suite":
            if i + 1 >= len(args):
                raise SystemExit("--suite requires a path")
            suite_path = Path(args[i + 1]).resolve()
            i += 2
            continue
        models.append(arg)
        i += 1

    if not models:
        models = NEW_MODELS

    with open(suite_path) as f:
        suite = json.load(f)

    print(f"Test suite: {suite_path} ({len(suite)} functions)")
    print(f"Models to benchmark: {len(models)}")

    RESULTS_DIR.mkdir(exist_ok=True)

    for model_spec in models:
        result = benchmark_model(model_spec, suite)
        safe_name = model_spec.replace("/", "_").replace(":", "_")
        if suite_path != DEFAULT_SUITE:
            safe_name = f"{suite_path.stem}__{safe_name}"
        out_path = RESULTS_DIR / f"{safe_name}.json"
        with open(out_path, "w") as f:
            json.dump(result, f, indent=2)

        passes = sum(1 for r in result["results"] if r["status"] == "PASS")
        total = len(result["results"])
        print(f"\n  Result: {passes}/{total} PASS, total={result['total_time']:.1f}s")
        print(f"  Saved: {out_path}")


if __name__ == "__main__":
    main()
