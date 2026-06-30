#!/usr/bin/env python3
"""extract_src_fns.py - pull VERBATIM function bodies out of a src/ .c file by
brace-matching, producing a .inc the oracle harness compiles as the B-side.

Trustworthiness: it copies the exact source bytes (no rewriting). It only
strips the `extern` forward-declarations that name OTHER fns we also extract
(so the .inc has real definitions, not externs) and rewrites cross-fn extern
return types that disagree (the src uses a local `extern u32 fn(...)` shim in
some callers that conflicts with the real `u8*` definition - C would error).

Usage:
  extract_src_fns.py <src.c> <out.inc> <fn1> [fn2 ...]

Each fn is matched as a top-level definition `<ret> <name>(...) {  ...  }`.
"""
import re, sys

def strip_if0_blocks(text):
    """Replace `#if 0 ... [#else ...] #endif` regions with their #else body
    (or nothing), so we extract the ACTIVE C, never the inactive `asm` stub.
    Handles nesting by scanning line by line and tracking #if depth, but only
    treats a top-level `#if 0` as dead. Other #if/#ifdef are left intact (their
    bodies are kept verbatim; the C we target is unconditional or in #else)."""
    out = []
    lines = text.split('\n')
    i = 0
    while i < len(lines):
        ln = lines[i]
        s = ln.strip()
        if s.replace(' ', '') == '#if0':
            # find matching #else / #endif at same depth
            depth = 1
            j = i + 1
            else_idx = None
            while j < len(lines) and depth > 0:
                t = lines[j].strip()
                if t.startswith('#if'):
                    depth += 1
                elif t.startswith('#endif'):
                    depth -= 1
                    if depth == 0:
                        break
                elif t.startswith('#else') and depth == 1:
                    else_idx = j
                j += 1
            endif_idx = j
            if else_idx is not None:
                # keep the #else body (between else_idx and endif_idx)
                out.extend(lines[else_idx+1:endif_idx])
            # else: drop the whole dead block
            i = endif_idx + 1
        else:
            out.append(ln)
            i += 1
    return '\n'.join(out)

def find_fn_span(text, name):
    # match a definition line: something  name(  ... )  {   at column 0
    pat = re.compile(r'(?m)^[A-Za-z_][\w\*\s]*\b' + re.escape(name) + r'\s*\([^;{]*\)\s*\{')
    m = pat.search(text)
    if not m:
        return None
    start = m.start()
    # brace match from the opening { of the signature
    i = text.index('{', m.end()-1)
    depth = 0
    j = i
    while j < len(text):
        c = text[j]
        if c == '{': depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                return (start, j+1)
        j += 1
    return None

def main():
    if len(sys.argv) < 4:
        print(__doc__); sys.exit(2)
    src, out = sys.argv[1], sys.argv[2]
    fns = sys.argv[3:]
    text = open(src, encoding='utf-8', errors='replace').read()
    text = strip_if0_blocks(text)   # keep ACTIVE C (#else), drop dead `asm` stubs

    extracted_names = set(fns)
    bodies = []
    for fn in fns:
        span = find_fn_span(text, fn)
        if not span:
            print(f"ERROR: could not locate definition of {fn} in {src}", file=sys.stderr)
            sys.exit(1)
        body = text[span[0]:span[1]]
        bodies.append((fn, body))

    # Strip `extern <ret> fn_xxx(...);` lines that refer to fns we provide as
    # real definitions in this .inc, so we don't get conflicting/duplicate decls.
    # Also strip externs for ANY fn_XXXXXXXX callee: in the auto-harness those
    # become thunk definitions (wide prototype) and the src's unprototyped
    # `extern u16 fn_x();` would otherwise conflict. (For the hand-harness flow
    # these callees are either extracted or host-shimmed, so stripping is safe.)
    extracted_alt = '|'.join(re.escape(n) for n in extracted_names)
    extern_re = re.compile(r'(?m)^\s*extern\s+[^\n;]*\b((?:' + extracted_alt +
                           r')|fn_[0-9A-Fa-f]{8})\s*\([^;]*\);\s*\n')

    with open(out, 'w', encoding='utf-8') as f:
        f.write("/* AUTO-EXTRACTED verbatim from %s by extract_src_fns.py.\n"
                " * DO NOT EDIT. Functions: %s\n */\n\n" % (src, ", ".join(fns)))
        # forward-declare all extracted fns so order doesn't matter
        # (use the real signatures from each body's first line)
        for fn, body in bodies:
            sig = body.split('{',1)[0].strip()
            f.write(sig + ";\n")
        f.write("\n")
        for fn, body in bodies:
            body = extern_re.sub('', body)
            f.write(body.rstrip() + "\n\n")
    print(f"wrote {out}: {len(bodies)} functions, {sum(len(b) for _,b in bodies)} src bytes")

if __name__ == "__main__":
    main()
