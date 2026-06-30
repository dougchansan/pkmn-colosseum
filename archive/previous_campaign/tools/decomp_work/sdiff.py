import json, subprocess, sys, os

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
CLI = os.path.join(ROOT, "tools", ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli"))
TARGET = os.path.join(ROOT, "build", "GC6E01", "obj", "auto_01_800055E0_text.o")
BASE = os.path.join(ROOT, "build", "GC6E01", "base", "game", "battle", "battle_scene.o")

sym = sys.argv[1]

cmd = [CLI, "diff", "-1", TARGET, "-2", BASE, "-o", "-", "--format", "json",
       "-c", "ppc.calculatePoolRelocations=false", sym]
r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
data = json.loads(r.stdout)

def fmt_instr(ins):
    i = ins.get("instruction", {})
    return i.get("formatted", "")

def get_sym(side):
    for e in data.get(side, {}).get("symbols", []):
        if e.get("name") == sym:
            return e
    return None

left = get_sym("left")    # target (aim for)
right = get_sym("right")  # ours

li = left.get("instructions", []) if left else []
ri = right.get("instructions", []) if right else []

print(f"=== {sym} ===  TARGET(left) {len(li)} instrs | OURS(right) {len(ri)} instrs")
n = max(len(li), len(ri))
for k in range(n):
    lf = fmt_instr(li[k]) if k < len(li) else ""
    rf = fmt_instr(ri[k]) if k < len(ri) else ""
    dk = ri[k].get("diff_kind", "") if k < len(ri) else ""
    mark = "" if dk in ("", "DIFF_NONE", None) else "  <<<"
    print(f"{k:3} | {lf:38} | {rf:38} {dk}{mark}")
