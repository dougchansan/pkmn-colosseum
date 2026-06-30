# Decomp Agent Coordination Protocol

All agents (Claude Code, OpenCode, Ollama) MUST follow these rules
to prevent regressions and coordinate work.

## Golden Rules

1. **NEVER modify a file without first running `python tools/compile_check.py <file>`**
2. **NEVER commit a function that doesn't pass `python tools/match_test.py fn_XXXXXXXX`**
3. **ALWAYS check the lock before starting work on a function**
4. **ALWAYS revert on failure — backup before apply, restore on mismatch**
5. **ALWAYS update progress.json after completing or failing a function**

## Workflow

```
1. Check lock:     tools/decomp_work/locks/<fn_name>.lock must NOT exist
2. Acquire lock:   Create the .lock file
3. Read .inc file: Get the PPC assembly
4. Generate C:     Write matching C89 code
5. Backup:         Save original .c file state
6. Apply:          Replace the #if 1 block with C code
7. Compile:        python tools/compile_check.py <file.c>
8. Match test:     python tools/match_test.py fn_XXXXXXXX
9. If 100%:        Update progress.json, release lock, commit
10. If < 100%:     REVERT to backup, release lock, log failure
```

## File Ownership

Each .c file should only be modified by ONE agent at a time.
Check `tools/decomp_work/locks/` — if ANY function in a file is locked,
do NOT modify that file.

## Progress Tracking

After every 5 successful matches, update:
- `python tools/sync_progress_metadata.py --sync` — sync README + `tools/decomp_work/progress.json` from `report.json`
- Run `python tools/decomp_scheduler.py --status` to verify counts

## Commit Protocol

Commits should follow this format:
```
Phase 3: decompile N functions in <file.c> (all 100% match)

- fn_XXXXXXXX: <brief description>
- fn_YYYYYYYY: <brief description>

Co-Authored-By: <agent name>
```

## Complexity Routing

| Tier | ASM Lines | Route To |
|------|-----------|----------|
| simple | 2-10 | Ollama (qwen2.5-coder:7b), Codex (o4-mini), or OpenCode free models |
| medium | 11-30 | Codex (o4-mini), Ollama, or Claude |
| complex | 31-80 | Codex or Claude |
| hard | 80+ | Claude only |

## Key Techniques Reference

See `docs/key_techniques.md` for:
- C89 rules, pragma cookbook, register allocation
- SDA address computation, float matching
- Common mismatch fixes
