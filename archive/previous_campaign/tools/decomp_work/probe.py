import json, sys
sym = sys.argv[1] if len(sys.argv) > 1 else "fn_801F7258"
with open(r"C:\Users\douglaswhittingham\pkmn-colosseum\_dj.json", "r", encoding="utf-8-sig") as f:
    d = json.load(f)

def rows(side):
    for e in d.get(side, {}).get("symbols", []):
        if e.get("name") == sym:
            out = []
            for ins in e.get("instructions", []):
                f = ins.get("instruction", {}).get("formatted", "")
                dk = ins.get("diff_kind", "")
                out.append((dk, f))
            return out, e.get("match_percent")
    return [], None

L, lp = rows("left")    # TARGET (match this)
R, rp = rows("right")   # MINE
print(f"left(TARGET) match%={lp}  right(MINE) match%={rp}")
n = max(len(L), len(R))
print(f"{'#':>3} {'TARGET (left)':40} {'kind(L)':12} | {'MINE (right)':40} {'kind(R)':12}")
for i in range(n):
    lt = L[i][1] if i < len(L) else ""
    lk = L[i][0] if i < len(L) else ""
    rt = R[i][1] if i < len(R) else ""
    rk = R[i][0] if i < len(R) else ""
    mark = "" if lt == rt else "  <<<"
    print(f"{i:>3} {lt:40} {lk:12} | {rt:40} {rk:12}{mark}")
