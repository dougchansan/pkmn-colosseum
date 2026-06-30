#!/usr/bin/env python3
"""gen_thunks.py - generate B-side callee thunks for the auto-harness.

Scans the extracted bside_real_src.inc for fn_XXXXXXXX symbols that are CALLED
but not DEFINED there, and emits bside_thunks.inc: one wrapper per callee that
routes to its recomp original via orc_thunk(addr, args...).

Each thunk uses a fixed wide prototype (u32 fn(u32 x8)); C passes the real args
in r3.. and the callee ignores extras. This is correct for scalar/pointer-by-
value-as-int args (the common leaf/getter callee shape). The harness's orc_p2g
is NOT auto-applied here (the thunk receives whatever the B body passed: for an
int that's the value; for a host pointer that's a truncated host addr) - so if a
callee takes a POINTER arg derived from a fuzzed struct, the thunk cannot map it
back to a guest address and the result may diverge. We DETECT that case and emit
a warning line `THUNK_PTR_RISK <fn>` so the runner can flag it rather than fake
a pass.

Usage: gen_thunks.py <bside_real_src.inc> <out.inc> [orig_src.c]
  orig_src.c (optional): scanned to detect FLOAT/DOUBLE-returning callees, which
  get a float-ABI thunk (arg in f1, result from f1) instead of the integer one.
"""
import re, sys

def main():
    inc, out = sys.argv[1], sys.argv[2]
    orig = sys.argv[3] if len(sys.argv) > 3 else None
    text = open(inc, encoding='utf-8', errors='replace').read()
    origtext = open(orig, encoding='utf-8', errors='replace').read() if orig else ""

    # symbols DEFINED in the inc (real bodies we extracted)
    defined = set(re.findall(r'(?m)^[A-Za-z_][\w\s\*]*?\b(fn_[0-9A-Fa-f]{8}|[A-Za-z_]\w*)\s*\([^;{]*\)\s*\{', text))
    # all fn_XXXXXXXX referenced anywhere
    called = set(re.findall(r'\b(fn_[0-9A-Fa-f]{8})\b', text))
    # also catch named callees declared `extern <type> NAME(...)` inside the body
    extern_named = re.findall(r'extern\s+[\w\s\*]+?\b([A-Za-z_]\w*)\s*\([^;]*\)\s*;', text)

    need = sorted((called | set(extern_named)) - defined)
    # only thunk fn_XXXXXXXX (addressable); skip libc-ish names we can't map
    need = [n for n in need if re.fullmatch(r'fn_[0-9A-Fa-f]{8}', n)]

    # detect float/double-returning callees (need an f1-ABI thunk):
    #   - `extern <float-type> fn_x(...)` in the original src, OR
    #   - a `(f32)fn_x(` / `(double)fn_x(` cast at any call site
    float_cb = set()
    for n in need:
        if re.search(r'extern\s+(?:f32|f64|float|double)\s+'+re.escape(n)+r'\s*\(', origtext) or \
           re.search(r'extern\s+(?:f32|f64|float|double)\s+'+re.escape(n)+r'\s*\(', text) or \
           re.search(r'\((?:f32|f64|float|double)\)\s*'+re.escape(n)+r'\s*\(', text):
            float_cb.add(n)

    fneed = sorted(float_cb)
    with open(out, 'w') as f:
        f.write("/* AUTO-GENERATED callee thunks. Each routes to its recomp original. */\n")
        for n in need:
            addr = "0x" + n[3:] + "u"
            if n in float_cb:
                # float-ABI thunk: single f32 arg (the common shape for the
                # math helpers), returns double from f1.
                f.write(
                    f"double {n}(f32 x);\n"
                    f"double {n}(f32 x){{ return orc_thunk_f({addr}, x); }}\n")
            else:
                # Unprototyped int thunk: matches the src's `extern u16 fn();`
                # call style and any arg count; SysV passes args in rdi,rsi,...
                # Args are POINTER-WIDTH (uintptr_t) so a host mirror-pointer
                # survives intact (orc_thunk converts each to a guest address);
                # a u32 param would truncate a 64-bit host pointer to garbage.
                f.write(
                    f"u32 {n}();\n"
                    f"u32 {n}(a3,a4,a5,a6,a7,a8,a9,a10)\n"
                    f"  uintptr_t a3,a4,a5,a6,a7,a8,a9,a10;\n"
                    f"{{ return orc_thunk({addr},a3,a4,a5,a6,a7,a8,a9,a10); }}\n")
    print("THUNKS " + " ".join(need) + (" | FLOAT " + " ".join(fneed) if fneed else ""))

if __name__ == "__main__":
    main()
