#!/usr/bin/env python3
"""m2c_clean.py — best-effort cleanup of an m2c --no-context draft into a
COMPILABLE CodeWarrior C body for the permuter farm.

m2c (run via `m2c_draft.py <fn> <src> --no-context`) emits C that is structurally
close to the target but does NOT compile, because it leans on m2c-only constructs:

  * `void *p; p->unk40;`            member access through void* (illegal C)
  * `p + *(p + ...)`               pointer arithmetic on void* (illegal/UB sizing)
  * `M2C_ERROR(/* ... */)`         placeholder for un-inferrable reads
  * `? fn(...)` / `? x;`           unknown return/var types
  * `/* Duplicate return ... */`   stray analysis comments

This pass rewrites the dominant, mechanical breakages so the body compiles:

  1. Retype every `void *NAME` declaration to `u8 *NAME` (u8* makes pointer
     arithmetic byte-addressed and lets us turn member access into offset reads).
  2. Rewrite `NAME->unkHH` / `NAME->fieldHH` (hex-suffixed members m2c invents for
     unknown structs) into `(*(u32*)((u8*)NAME + 0xHH))`. `unk0` -> +0x0.
  3. Replace `M2C_ERROR(...)` with `0` (the farm only needs a compilable,
     finite-scoring base; a wrong-but-typed read is fine to anneal from).
  4. Replace a leading `?` return/var type with `int`.
  5. Strip m2c analysis comments.

It is deliberately CONSERVATIVE and best-effort: drafts that depend on a global
symbol being treated as a struct (`&lbl_X.unkNN`, `lbl_X + 0xNN`), float pools,
or other shapes it cannot mechanically fix are left partly-broken — they simply
fail to compile and the farm skips them (compiled=N). That is the intended
contract: cheap wins now, no attempt at full type recovery.

Usage:
    m2c_clean.py <m2c_out/fn_XXXX.c> [-o <out.c>]

With no -o, writes <input>.clean.c next to the input and prints that path.
Never writes into src/.
"""
import re
import sys


def clean(text: str) -> str:
    # Drop the m2c header comment block and any stray analysis comments.
    text = re.sub(r"/\*\s*m2c first draft.*?\*/\s*", "", text, flags=re.S)
    text = re.sub(r"/\*\s*Duplicate return.*?\*/", "", text, flags=re.S)
    text = re.sub(r"/\*\s*extern\s*\*/", "", text)

    # 3. M2C_ERROR(...) -> 0  (balanced-ish: M2C_ERROR wraps a single comment).
    text = re.sub(r"M2C_ERROR\([^()]*\)", "0", text)

    # 1. `void *NAME` decls -> `u8 *NAME` so byte pointer arithmetic + offset
    #    reads work. Match a declaration context (start of statement-ish).
    text = re.sub(r"\bvoid\s*\*\s*([A-Za-z_]\w*)", r"u8 *\1", text)

    # 4. unknown type `?` m2c emits when it cannot infer a type.
    #    - leading `? fn(...)` / `? x;`            -> `int ...`
    #    - bare `?` as a parameter type `(void*, ?)` -> `int`
    text = re.sub(r"(^|\n)\s*\?\s+", r"\1int ", text)
    text = re.sub(r"\?\s+(?=[A-Za-z_]\w*\s*[;,)=])", "int ", text)
    # `?` standing alone as a type in a parameter list: `, ?)` or `(?,` or `(?)`
    text = re.sub(r"(?<=[(,])\s*\?\s*(?=[,)])", " int ", text)

    # 2. NAME->unkHH / NAME->fieldHH (hex offset members m2c fabricates) ->
    #    (*(u32*)((u8*)NAME + 0xHH)). Also bare `->unkHH`. Handle parenthesised
    #    LHS like (expr)->unk4 by capturing a primary expression token.
    #    Primary: an identifier, optionally with a leading cast/paren already
    #    rewritten; we only need the simple identifier form m2c emits.
    member_re = re.compile(r"([A-Za-z_]\w*)\s*->\s*(?:unk|field|unk_)([0-9A-Fa-f]+)")

    def member_sub(m: "re.Match[str]") -> str:
        name, off = m.group(1), m.group(2)
        return f"(*(u32*)((u8*){name} + 0x{off}))"

    # Apply repeatedly so chained `a->unkX->unkY` collapse left-to-right.
    prev = None
    while prev != text:
        prev = text
        text = member_re.sub(member_sub, text)

    # Any remaining `->unkHH` on a parenthesised expr: (EXPR)->unk4
    paren_member_re = re.compile(r"(\([^()]*\))\s*->\s*(?:unk|field)([0-9A-Fa-f]+)")
    prev = None
    while prev != text:
        prev = text
        text = paren_member_re.sub(
            lambda m: f"(*(u32*)((u8*){m.group(1)} + 0x{m.group(2)}))", text
        )

    # 5. base.c (minimal preprocessed TU) often has no <stddef.h>: NULL is
    #    undefined. m2c uses it freely for null pointers -> replace with 0.
    text = re.sub(r"\bNULL\b", "0", text)

    # 6. CodeWarrior (strict, no implicit int<->pointer) rejects assigning a
    #    `*(u32*)(...)` memory read to a pointer local. Cast the RHS of any
    #    assignment whose LHS is a DECLARED pointer local to (void*). We collect
    #    pointer-typed locals/params (`<type> *name`) and wrap `name = <rhs>;`.
    ptr_names = set(re.findall(r"\b[A-Za-z_]\w*\s*\*\s*([A-Za-z_]\w*)\s*[;,)=]", text))
    for name in sorted(ptr_names, key=len, reverse=True):
        # name = EXPR ;   ->   name = (void*)(EXPR);   (skip if already a cast to
        # a pointer, or RHS is a bare 0 / another pointer expr that's fine).
        def _cast(m: "re.Match[str]", nm=name) -> str:
            rhs = m.group(1).strip()
            if rhs == "0" or rhs.startswith("(void*)") or rhs.startswith("&"):
                return m.group(0)
            return f"{nm} = (void*)({rhs});"
        # (?<![*.>\w]) so we never match a deref-store `*name =`, a member
        # `x.name =` / `x->name =`, or a longer identifier ending in name.
        # (?!=) so we skip `==` comparisons.
        text = re.sub(
            r"(?<![*.>\w])" + re.escape(name) + r"\s*=\s*(?!=)([^;]+);",
            _cast, text,
        )

    return text.strip() + "\n"


def main() -> None:
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    out = None
    if "-o" in sys.argv:
        out = sys.argv[sys.argv.index("-o") + 1]
    if not args:
        sys.exit("usage: m2c_clean.py <m2c_out/fn_XXXX.c> [-o out.c]")
    inp = args[0]
    with open(inp, "r", encoding="utf-8", errors="replace") as f:
        cleaned = clean(f.read())
    if out is None:
        out = re.sub(r"\.c$", "", inp) + ".clean.c"
    with open(out, "w", encoding="utf-8", newline="\n") as f:
        f.write(cleaned)
    print(out)


if __name__ == "__main__":
    main()
