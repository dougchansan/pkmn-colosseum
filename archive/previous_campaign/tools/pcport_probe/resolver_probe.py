#!/usr/bin/env python3
"""Bounds-safe reference for the HSD archive resolver — mirrors
pcport/real_content_host.c (FSYS member load + LZSS + HSD archive parse +
relocation + public-symbol resolution). Used to verify the P1 pipeline on real
game data and to debug the native probe.

Usage: python tools/pcport_probe/resolver_probe.py orig/GC6E01/disc/files/topmenu.fsys [member]
"""
import sys, struct

FSYS_MAGIC = 0x46535953
LZSS_MAGIC = 0x4C5A5353
LZSS_HEADER_SIZE = 0x10
LZSS_WINDOW_START = 0x0FEE


def be32(b, o): return struct.unpack_from(">I", b, o)[0]


def lzss(src, in_size, out_size):
    win = bytearray(0x1000)
    sp, dp, wp, flags = LZSS_HEADER_SIZE, 0, LZSS_WINDOW_START, 0
    out = bytearray(out_size)
    while sp < in_size and dp < out_size:
        flags >>= 1
        if (flags & 0x100) == 0:
            if sp >= in_size: break
            flags = src[sp] | 0xFF00; sp += 1
        if flags & 1:
            if sp >= in_size: break
            c = src[sp]; sp += 1
            out[dp] = c; dp += 1; win[wp] = c; wp = (wp + 1) & 0xFFF
        else:
            if sp + 1 >= in_size: break
            b1, b2 = src[sp], src[sp + 1]; sp += 2
            off = b1 | ((b2 & 0xF0) << 4); length = (b2 & 0x0F) + 2
            for j in range(length + 1):
                if dp >= out_size: break
                c = win[(off + j) & 0xFFF]
                out[dp] = c; dp += 1; win[wp] = c; wp = (wp + 1) & 0xFFF
    return bytes(out)


def load_member(fsys, name=None):
    """Yield (name, decompressed_bytes) for matching members."""
    n = len(fsys)
    if n < 0x20 or be32(fsys, 0) != FSYS_MAGIC:
        print("not an FSYS archive"); return
    entry_count = be32(fsys, 0x08)
    str_tab = be32(fsys, 0x18)
    print(f"FSYS: members={entry_count} stringTableOffset=0x{str_tab:X} fileSize=0x{n:X}")
    if str_tab + 4 > n:
        print("  stringTableOffset out of bounds"); return
    entry_tab = be32(fsys, str_tab)
    print(f"  entryTableOffset=0x{entry_tab:X}")
    if entry_tab >= n:
        print("  entryTableOffset out of bounds"); return
    for i in range(entry_count):
        if entry_tab + i * 4 + 4 > n: break
        eo = be32(fsys, entry_tab + i * 4)
        if eo + 0x28 > n: continue
        no = be32(fsys, eo + 0x24)
        if no >= n: continue
        end = fsys.find(b"\0", no); nm = fsys[no:end].decode("ascii", "replace")
        if name is not None and nm != name: continue
        data_off = be32(fsys, eo + 0x04); comp = be32(fsys, eo + 0x08)
        if data_off >= n or data_off + comp > n:
            yield (nm, None); continue
        if comp >= LZSS_HEADER_SIZE and be32(fsys, data_off) == LZSS_MAGIC:
            out_n = be32(fsys, data_off + 0x04); in_n = be32(fsys, data_off + 0x08)
            yield (nm, lzss(fsys[data_off:data_off + in_n], in_n, out_n))
        else:
            yield (nm, bytes(fsys[data_off:data_off + comp]))


def hsd_parse(data):
    """Return (ok, info) — parse header + apply relocation in place (a copy)."""
    size = len(data)
    if size < 0x20: return (False, "too small")
    file_size, data_size = be32(data, 0), be32(data, 4)
    nreloc, npub, next = be32(data, 8), be32(data, 0x0C), be32(data, 0x10)
    if file_size != size:
        return (False, f"fileSize 0x{file_size:X} != size 0x{size:X}")
    data_off = 0x20
    reloc_off = 0x20 + data_size
    pub_off = reloc_off + nreloc * 4
    ext_off = pub_off + npub * 8
    str_off = ext_off + next * 8
    if max(reloc_off, pub_off, ext_off, str_off) > size:
        return (False, "table offsets out of bounds")
    buf = bytearray(data)
    for i in range(nreloc):
        field_off = be32(buf, reloc_off + i * 4)
        ab = data_off + field_off
        if ab + 4 > size: return (False, f"reloc {i} OOB")
        struct.pack_into(">I", buf, ab, be32(buf, ab) + data_off)
    pubs = []
    for i in range(npub):
        res = be32(buf, pub_off + i * 8); key = be32(buf, pub_off + i * 8 + 4)
        nm_off = str_off + key
        end = buf.find(b"\0", nm_off) if nm_off < size else -1
        nm = buf[nm_off:end].decode("ascii", "replace") if end >= 0 else "?"
        pubs.append((nm, data_off + res))
    return (True, dict(data_size=data_size, nreloc=nreloc, npub=npub, next=next, pubs=pubs))


def main():
    if len(sys.argv) < 2:
        print("usage: resolver_probe.py <file.fsys> [member]"); return
    fsys = open(sys.argv[1], "rb").read()
    want = sys.argv[2] if len(sys.argv) > 2 else None
    shown = 0
    for nm, data in load_member(fsys, want):
        if data is None:
            print(f"  {nm:<22} <decompress failed>"); continue
        is_hsd = len(data) >= 0x20 and be32(data, 0) == len(data)
        print(f"  {nm:<22} size=0x{len(data):<7X} {'HSD-ARCHIVE' if is_hsd else ''}")
        if is_hsd and (want or shown < 4):
            ok, info = hsd_parse(data)
            if ok:
                print(f"      data=0x{info['data_size']:X} reloc={info['nreloc']} "
                      f"public={info['npub']} extern={info['next']}  (relocated OK)")
                for k, (sym, addr) in enumerate(info["pubs"][:8]):
                    print(f"      public[{k}] = '{sym}' @ storage+0x{addr:X}")
            else:
                print(f"      parse: {info}")
            shown += 1


if __name__ == "__main__":
    main()
