import json, subprocess, sys, os

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
# Usage: extract_target_asm.py <symbol> [target_obj]
sym = sys.argv[1]
target = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "build/GC6E01/obj/auto_01_800055E0_text.o")
exe = os.path.join(ROOT, ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli"))
base = os.path.join(ROOT, "build/GC6E01/base/crt/exit.o")
out = subprocess.run([exe, "diff", "-1", target, "-2", base,
                      "-o", "-", "--format", "json",
                      "-c", "ppc.calculatePoolRelocations=false", sym],
                     capture_output=True, text=True, cwd=ROOT)
data = json.loads(out.stdout)
for side in ("left", "right"):
    syms = data[side]["symbols"]
    for s in syms:
        if s.get("name") == sym and "instructions" in s:
            print(f"==== {side} ({sym}) size={s.get('size')} ====")
            for idx, ins in enumerate(s["instructions"]):
                i = ins.get("instruction", {})
                kind = ins.get("diff_kind", "")
                mark = " " if kind in ("", "DIFF_NONE") else "*"
                # show relocation target if present
                reloc = ""
                for p in i.get("parts", []):
                    r = p.get("arg", {})
                    if isinstance(r, dict) and "reloc" in str(r).lower():
                        pass
                t = i.get("formatted", "")
                # capture reloc symbol name
                rel = i.get("relocation") or {}
                relname = ""
                if rel:
                    relname = "  ; -> " + (rel.get("target",{}).get("name","") or "")
                print(f"  {idx:>3}{mark} {t}{relname}")
            break
