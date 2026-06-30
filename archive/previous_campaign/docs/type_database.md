# Type / Symbol / Signature Database (`symdb`)

**Single source of truth** for symbol addresses, function signatures, type
names, and struct field offsets in the Pokemon Colosseum (GC6E01) matching
decompilation.

- Tool: `tools/symdb.py`
- Database file: `config/GC6E01/symdb.json` (seeded from `config/GC6E01/symbols.txt`)
- Conflict report: `logs/extern_conflicts.txt`

## Why this exists

Functions used to be decompiled in a vacuum. Each agent invented its own
`extern` declarations, struct field offsets, and signatures *locally inside its
own .c file*. That caused two recurring, expensive failures:

1. **Conflicting externs break CodeWarrior compilation.** Two `.c` files declare
   the same symbol `extern` with different return types or parameter types. When
   their translation units are linked, the declarations disagree and the build
   fails (or, worse, silently miscompiles a call).

2. **Unmatchable large functions.** A single wrong struct field offset cascades
   into many register-allocation / scheduling diffs. Those get misdiagnosed as
   "needs a compiler pragma" and waste hours, when the real fix was one correct
   offset.

A scan of the current tree found **1755 symbols** declared `extern` with
conflicting signatures (out of 8352 extern symbols) — see
[Live conflict baseline](#live-conflict-baseline). This is not hypothetical; it
is the dominant correctness hazard in the tree today.

The fix: every agent **reads and writes type/signature/offset facts through
`symdb`**, never by inventing them locally.

## The rules every agent MUST follow

### Before you write an `extern` declaration

1. **Look it up first.**

   ```
   python tools/symdb.py lookup fn_800096B4
   python tools/symdb.py lookup BattlePokemon
   ```

   If a canonical `signature` / `type` exists, **use it verbatim**. Do not
   re-derive the prototype from the disassembly and "improve" the parameter
   types — that is exactly how conflicts get created.

2. **If no signature is recorded yet**, derive the *minimal correct* prototype,
   then **record it** so the next agent reuses it instead of re-inventing:

   ```
   python tools/symdb.py set-sig fn_800096B4 \
       "s32 fn_800096B4(void* ptr, s32 a, u8* b, u8* c, u8* d, u8* e)" \
       --note "from gs_task.c call sites"
   ```

3. **Match the recorded empty-arg form.** `void f()` and `void f(void)` are
   different C declarations and *both* appear in this tree. Pick the form in the
   DB. If the function genuinely takes no args, prefer `(void)`.

### When you discover or rely on a struct field offset

Record it — never leave it implicit in a cast:

```
python tools/symdb.py set-struct-size BattlePokemon 0x58
python tools/symdb.py set-field   BattlePokemon 0x00 species u16
python tools/symdb.py set-field   BattlePokemon 0x04 hp      u16 --note "verified vs target asm"
```

A wrong offset in your `.c` is a silent landmine for the next person matching a
neighboring function. If you confirm an offset against the target asm, add the
`--note`.

### Before you hand a file off / claim a match

**Run the conflict gate.** If your new externs introduced a conflict, fix it
(adopt the canonical signature) before handing off:

```
python tools/symdb.py check-conflicts
```

This exits non-zero when any conflict exists, so it can gate a commit hook or an
agent's "done" check.

## Command reference

| Command | Purpose |
|---------|---------|
| `lookup <name>` | Show a symbol's addr/kind/size/signature, or a struct's fields. Falls back to `symbols.txt` (incl. `Proposed:` names) if not yet in the DB. |
| `set-sig <name> "<sig>" [--note]` | Record a function's canonical signature. |
| `set-type <name> <type> [--kind] [--note]` | Record a symbol's type. |
| `set-field <struct> <offset> <field> <type> [--note]` | Record a struct field offset/type. |
| `set-struct-size <struct> <size>` | Record a struct's total size. |
| `seed` | (Re)seed addresses/kinds/sizes from `config/GC6E01/symbols.txt`. Never clobbers curated `signature`/`type`/`note`. |
| `check-conflicts [--report PATH]` | Scan `src/**.c` for conflicting `extern` decls. Exits non-zero if any exist. |
| `validate` | Sanity-check the DB and confirm the src tree is conflict-free. |

## How conflict detection works

`check-conflicts` strips comments (so commented-out decls don't count), finds
every `extern ...;` (including multi-line ones), extracts the declared symbol,
and **normalizes** the declaration:

- whitespace is collapsed,
- **parameter names are dropped** (only types matter),
- the return type and ordered parameter *types* form the signature.

So these two are treated as **identical** (no conflict — only the param name
differs):

```c
extern void DCInvalidateRange(void* addr, u32 size);
extern void DCInvalidateRange(void* addr, u32 nBytes);
```

…while these are a **conflict** (return type and arg types differ):

```c
extern s32  fn_800096B4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);   // gs_party_access.c
extern s32  fn_800096B4(void* ptr, s32 a, u8* b, u8* c, u8* d, u8* e); // gs_task.c
```

The report lists, per conflicting symbol, each distinct normalized signature and
every `file:line` that declares it — so you can go fix them.

## Live conflict baseline

Captured against the current `src/` tree (see `logs/extern_conflicts.txt`):

- **extern symbols scanned:** 8352
- **conflicting symbols:** **1755**
  - 852 with a **differing return type**
  - 472 with **differing arity / argument types**
  - 431 differing only in the `()` vs `(void)` empty-arg form

The 1324 return-type/argtype conflicts are unambiguous CW-breaking hazards.
Drive this number toward zero by adopting canonical signatures from the DB.

## Maintenance

- Re-run `seed` after `config/GC6E01/symbols.txt` changes (address/size refresh
  is safe; curated fields are preserved).
- `symdb.json` is sorted/normalized JSON, so diffs stay readable and merge
  cleanly when multiple agents add facts.
