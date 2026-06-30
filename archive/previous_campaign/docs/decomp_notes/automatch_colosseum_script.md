# automatch report: colosseum_script.c

baseline 11/462 @ 100%, swept 19 near-misses in 1288s

## Solved (deterministic, zero-token)

- **fn_80222110** 94.42% -> 100% via `#pragma peephole off`

## Improved but not 100%

- fn_8021AB9C 91.56% -> 94.84% via `#pragma optimization_level 2`
- fn_80222EF0 91.52% -> 91.78% via `#pragma peephole off`

## Blocked — escalate to LLM agent

- fn_8022E1C4 stuck at 98.85% (no pragma variant helped)
- fn_80217018 stuck at 95.56% (no pragma variant helped)
- fn_802173D4 stuck at 93.12% (no pragma variant helped)
- fn_8021DDD8 stuck at 92.80% (no pragma variant helped)
- fn_8022E6F0 stuck at 91.54% (no pragma variant helped)
- fn_80238B0C stuck at 91.16% (no pragma variant helped)
- fn_8021D010 stuck at 91.09% (no pragma variant helped)
- fn_8021ECF8 stuck at 91.00% (no pragma variant helped)
- fn_80214CB0 stuck at 90.79% (no pragma variant helped)
- fn_8021E288 stuck at 90.40% (no pragma variant helped)
- fn_80222ADC stuck at 90.35% (no pragma variant helped)
- fn_8021E754 stuck at 90.33% (no pragma variant helped)
- fn_802266EC stuck at 90.29% (no pragma variant helped)
- fn_8021F92C stuck at 90.19% (no pragma variant helped)
- fn_8023793C stuck at 90.10% (no pragma variant helped)
- fn_80214AB4 stuck at 90.00% (no pragma variant helped)
