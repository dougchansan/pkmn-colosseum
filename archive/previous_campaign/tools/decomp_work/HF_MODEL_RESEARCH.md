# HF decompilation model research (2026-04-18)

User asked about these HF models for our PPC byte-match task:

## LLM4Binary / LLM4Decompile-6.7B-v2

- **Trained for**: decompiling **x86_64 assembly** (Linux ELF) into C source.
- **Two variants**:
  - `End`: takes raw x86 asm from `objdump`
  - `Ref`: takes **Ghidra pseudo-code** as input, refines to human C
- **Quantized**: `LLM4Binary/llm4decompile-6.7b-v2` loaded 8-bit via BitsAndBytesConfig on 3090.
- **GGUF version**: Not available. Uses transformers + bitsandbytes directly.

## LLM4Binary / sk2decompile-struct-6.7b + sk2decompile-ident-6.7b

- **Trained for**: Linux-x64 ELF binaries via IDA pseudocode preprocessing.
- **Architecture**: x64 only, C language only.
- **Not useful** for our PowerPC target directly.

## One-shot test result (2026-04-18, 20:47)

Ran `LLM4Binary/llm4decompile-6.7b-v2` with 8-bit quantization on the 3090.
Environment: torch 2.5.1+cu121, transformers 5.5.4, bitsandbytes 0.49.2.
Install path: `/storage/finetune/llm4decompile/` (venv + 26GB hf_cache).

**Input**: `tools/decomp_work/ghidra_out/fn_80020F54_ghidra.c` (real PPC Ghidra export,
64 lines, includes `unaff_r2`, `unaff_r13`, `short`/`int` locals, multiple fn_ calls).

**Timing**: model load 204s (first time, includes 13GB weight fetch). Generation 82s for 1024 tokens.

**Output**: GARBAGE.
1. Tokenizer artifacts unhandled — literal `Ġ` (space) and `Ċ` (newline) markers in output, `skip_special_tokens=True` didn't clean them.
2. Semantic collapse: the model entered degenerate greedy decoding. The entire "C output" was:
   ```
   void fn_80020F54(unsigned char* pInputData, unsigned short* pOutputData)
   {
       int i;
       unsigned char* pCmd;
       unsigned char* pData;
       unsigned char* pData2;
       unsigned char* pData3;
       ... (continues through pData109+)
   }
   ```
   No actual function body. Just endless unused variable declarations.

## Interpretation

The model is heavily x86-specialized. When fed PPC-patterned pseudo-code (which uses
distinct idioms like `unaff_rN`, `r13`-relative loads, `extsh`-style truncations), the
token sequences don't match its training distribution. The model falls into a
deterministic degenerate loop producing syntactically-plausible but semantically-empty
code.

**Conclusion**: `llm4decompile-6.7b-v2` is NOT viable for our PowerPC / CodeWarrior
byte-match task, neither directly on PPC asm nor via Ghidra pseudo-code intermediate.

## Verdict for the project

**Skip both LLM4Decompile and sk2decompile** for our workflow.

Stick with what works:
- **`qwen2.5-coder:32b` via ollama** on the 3090 — proven 95% on fn_80115C48, general code-coder model, doesn't need x86-specific training.
- **Claude Opus verification** — reads qwen draft, applies CW quirks, commits.
- **Our `build_prompt.py` pipeline** — encodes CW knowledge where llm4decompile has none.

The lesson: architecture-specific decompile models generalize poorly. General code models + domain-specific prompt engineering (CW_QUIRKS.md + match_helper.py) outperform specialized but wrong-arch models.

## Disk usage (for cleanup later)

`/storage/finetune/llm4decompile/` contains:
- `venv/` — ~2.5GB (torch, transformers, bitsandbytes)
- `hf_cache/` — ~26GB (model weights + xet blobs)

To reclaim: `rm -rf /storage/finetune/llm4decompile/hf_cache` frees 26GB.
