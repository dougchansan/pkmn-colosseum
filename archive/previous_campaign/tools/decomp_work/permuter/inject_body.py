#!/usr/bin/env python3
"""inject_body.py — replace a build_dir.sh scrubbed stub body with a real C body.

build_dir.sh preprocesses a TU and isolates ONE function into base.c. For a pure
asm stub (asm wrapper live in a `#if 1` branch, no usable `#else` C candidate),
scrub_asm_bodies.py reduces the target to an EMPTY definition:

    void fn_8017C39C(void) { }

There is nothing for the permuter to mutate. This tool swaps that empty body for
a real, compilable C definition (e.g. an m2c draft cleaned by m2c_clean.py, or a
hand-written transcription of the truth asm). The replacement body file must
contain the COMPLETE function definition (signature + braces), e.g.

    s32 fn_8017C39C(u8* r3) { ... return 0; }

The new signature replaces whatever scrubbed signature was in base.c, so the body
file is free to give the target real parameters the `void(void)` scrub dropped.

Usage:
    inject_body.py <base.c> <fn_name> <body.c>

Rewrites <base.c> in place. Exits non-zero (and leaves base.c untouched) if the
scrubbed empty body cannot be located. Only ever writes the file it is given;
never touches src/ or the .inc truth files.
"""
import re
import sys


def inject(base_path: str, fn: str, body_path: str) -> None:
    with open(base_path, "r", encoding="utf-8", errors="replace") as f:
        src = f.read()
    with open(body_path, "r", encoding="utf-8", errors="replace") as f:
        body = f.read().strip() + "\n"

    # Match the scrubbed target definition: a header line
    #   <ret/quals> <fn>(<params>) { <empty-ish> }
    # whose body contains no nested braces (the scrubber emits a single-line
    # empty `{ }`, possibly with a stray comment). Anchored to <fn> so we never
    # touch a sibling. [^{}]* inside the braces keeps us to a flat (no nested
    # block) body, which is exactly what scrub_asm_bodies.py produces.
    pat = re.compile(
        r"^[^\n;{}]*\b" + re.escape(fn) + r"\s*\([^;{}]*\)\s*\{[^{}]*\}[ \t]*$",
        re.M,
    )
    m = pat.search(src)
    if not m:
        sys.exit(
            f"inject_body: could not find a scrubbed empty body for {fn} in "
            f"{base_path} (was build_dir.sh run, and did it scrub the target?)"
        )
    src = src[: m.start()] + body + src[m.end():]
    with open(base_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(src)
    print(f"inject_body: replaced {fn} body in {base_path}")


def main() -> None:
    if len(sys.argv) != 4:
        sys.exit("usage: inject_body.py <base.c> <fn_name> <body.c>")
    inject(sys.argv[1], sys.argv[2], sys.argv[3])


if __name__ == "__main__":
    main()
