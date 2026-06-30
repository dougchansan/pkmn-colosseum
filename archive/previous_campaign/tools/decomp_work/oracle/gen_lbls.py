#!/usr/bin/env python3
"""gen_lbls.py - generate storage + RAM-loaders for SDA/global lbl_XXXXXXXX
symbols a B-side function references.

For each lbl_<addr> referenced in the extracted body, we:
  - find its declared type from the ORIGINAL src (an `extern <type> lbl_x;` or
    `extern <type> lbl_x[...]` near a use), defaulting to u32 if unknown;
  - emit a definition in bside_lbls.inc;
  - emit a loader line in bside_lbls_load.inc that reads the symbol's bytes from
    the DOL-populated guest RAM at <addr> into the storage (so B sees the same
    constant/global the A-side does).

Scalars (u8/u16/u32/f32) are loaded by width. Anything else (arrays/structs)
is loaded as a raw byte copy of `sizeof`. Address-of-symbol idioms where the
code uses `lbl_x` as a base address (e.g. `(u8*)lbl_x`) are NOT auto-handled and
are flagged: LBL_ADDR_RISK <name>.

Usage: gen_lbls.py <bside_real_src.inc> <orig_src.c> <out_decls.inc> <out_load.inc>
"""
import re, sys

TYPE_RE = re.compile(r'extern\s+([A-Za-z_][\w\s\*]*?)\s+(lbl_[0-9A-Fa-f]{8})\s*(\[[^\]]*\])?\s*;')

def main():
    inc, orig, out_d, out_l = sys.argv[1:5]
    body = open(inc, encoding='utf-8', errors='replace').read()
    src  = open(orig, encoding='utf-8', errors='replace').read()

    refs = sorted(set(re.findall(r'\b(lbl_[0-9A-Fa-f]{8})\b', body)))
    # collect declared types from the original src
    decl = {}
    for m in TYPE_RE.finditer(src):
        t, name, arr = m.group(1).strip(), m.group(2), m.group(3)
        decl[name] = (t, arr)

    risks = []
    with open(out_d,'w') as fd, open(out_l,'w') as fl:
        fd.write("/* AUTO lbl_ storage. */\n")
        fl.write("/* AUTO lbl_ loaders (read from DOL-populated guest RAM). */\n")
        fl.write("static void orc_load_lbls(void){\n")
        fl.write("    u32 _b;\n")
        for n in refs:
            addr = "0x" + n[4:] + "u"
            t, arr = decl.get(n, ('u32', None))
            # detect address-of-symbol use: `(u8*)lbl_x` or `(... *)lbl_x` or
            # `*(u32*)lbl_x` (value-as-address idiom) -> can't model as storage.
            if re.search(r'\(\s*[A-Za-z_][\w\s]*\*\s*\)\s*'+re.escape(n), body) or \
               re.search(r'\*\s*\(\s*[A-Za-z_][\w\s]*\*\s*\)\s*'+re.escape(n), body):
                risks.append(n)
            base = t.replace('*','').strip()
            if arr:
                # array/aggregate: copy raw bytes (size unknown -> 64B window)
                fd.write(f"{t} {n}{arr};\n")
                fl.write(f"    for(unsigned _i=0;_i<sizeof({n});_i++) "
                         f"((u8*)&{n})[_i]=mem_read8(&g_cpu,{addr}+_i);\n")
            elif base in ('f32','float'):
                fd.write(f"f32 {n};\n")
                fl.write(f"    _b=mem_read32(&g_cpu,{addr}); memcpy(&{n},&_b,4);\n")
            elif base in ('f64','double'):
                fd.write(f"f64 {n};\n")
                fl.write(f"    {{ u64 _q=mem_read64(&g_cpu,{addr}); memcpy(&{n},&_q,8); }}\n")
            elif base in ('u16','s16','short'):
                fd.write(f"{t} {n};\n")
                fl.write(f"    {n}=({t})mem_read16(&g_cpu,{addr});\n")
            elif base in ('u8','s8','char'):
                fd.write(f"{t} {n};\n")
                fl.write(f"    {n}=({t})mem_read8(&g_cpu,{addr});\n")
            else:
                fd.write(f"u32 {n};\n")
                fl.write(f"    {n}=mem_read32(&g_cpu,{addr});\n")
        fl.write("}\n")

    print("LBLS " + " ".join(refs) + (" | ADDR_RISK " + " ".join(risks) if risks else ""))

if __name__ == "__main__":
    main()
