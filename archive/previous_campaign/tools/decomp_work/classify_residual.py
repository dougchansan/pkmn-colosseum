#!/usr/bin/env python3
"""classify_residual.py <TAG> <fn> - mechanical WALL gate.

Runs `band.py diff <TAG> <fn>`, looks at ONLY the real mismatch lines, and decides
whether the residual is winnable (so an agent may NOT file it as WALL) or a genuine
artifact wall. Built because lanes kept filing pure register-coloring residuals
(same instructions, different register numbers) as ~98% WALLs instead of applying
the declaration-order lever - e.g. fn_80200E00, which stalled as a raw-m2c draft
(registers declared as locals, which PINS the coloring).

Verdicts:
  REG-COLORING  -> WINNABLE. Same instrs, only register NUMBERS differ (or an extra
                   mr/li round-trip). Rewrite with NAMED locals + declaration-order
                   lever. DO NOT file WALL.
  SCHEDULING    -> likely wall. Same instruction SET, reordered. Try scheduling
                   pragma / operand-order once, else WALL.
  RELOC         -> wall. @nnn-vs-@named / SDA-numeric / conversion-literal artifact.
  SHAPE         -> REWORK. Mnemonics differ / many lines -> m2c_draft reshape, not a
                   reg-alloc lever.

Exit code: 0 if WINNABLE (caller must keep grinding), 1 otherwise.
"""
import re, subprocess, sys, os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
REG = re.compile(r"\b[rf]\d+\b")
ADDR = re.compile(r"\b0x[0-9a-fA-F]+\b")


def norm_regs(s):
    return REG.sub("rN", s)


def norm_all(s):
    return ADDR.sub("A", REG.sub("rN", s))


def mnemonic(s):
    s = s.strip()
    return s.split()[0] if s else ""


def main():
    if len(sys.argv) < 3:
        print("usage: classify_residual.py <TAG> <fn>")
        return 2
    tag, fn = sys.argv[1], sys.argv[2]
    out = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "decomp_work", "band.py"), "diff", tag, fn],
        capture_output=True, text=True, cwd=ROOT,
    ).stdout

    mismatches = []          # (left, right)
    total = 0
    for ln in out.splitlines():
        if "|" not in ln:
            continue
        left, right = ln.split("|", 1)
        left = left.replace(">>>", "").strip()
        right = right.strip()
        if not left and not right:
            continue
        total += 1
        is_diff = ln.lstrip().startswith(">>>")
        if is_diff:
            mismatches.append((left, right))

    if not mismatches:
        print(f"{fn}: NO-RESIDUAL (already byte-exact or diff unavailable) - band.py check / save.")
        return 0

    reg_only = sched = reloc = shape = 0
    for left, right in mismatches:
        if left == "---" or right == "---":
            # an inserted/deleted instr. A spare mr/li round-trip is a coloring artifact.
            other = right if left == "---" else left
            if mnemonic(other) in ("mr", "li", "mr."):
                reg_only += 1
            else:
                shape += 1
            continue
        if mnemonic(left) != mnemonic(right):
            shape += 1
        elif norm_regs(left) == norm_regs(right):
            reg_only += 1               # identical except register numbers
        elif norm_all(left) == norm_all(right):
            reloc += 1                  # identical except an address/label/immediate
        else:
            # same mnemonic, differs beyond regs+addr -> operand reorder = scheduling
            sched += 1

    n = len(mismatches)
    frac = n / max(total, 1)
    print(f"{fn}: {n} mismatch line(s) of {total} ({frac*100:.0f}%) - "
          f"reg-coloring:{reg_only} scheduling:{sched} reloc:{reloc} shape:{shape}")

    if shape >= 1 and (shape >= n / 2 or frac > 0.15):
        print("  VERDICT: SHAPE - wrong control-flow/types. m2c_draft.py reshape, "
              "then re-diff. NOT a reg-alloc lever.")
        return 1
    if reloc and reloc >= reg_only and not reg_only:
        print("  VERDICT: RELOC - @nnn/SDA/conversion-literal artifact. FILE WALL.")
        return 1
    if reg_only >= 1 and reg_only >= sched:
        print("  VERDICT: REG-COLORING - WINNABLE. The instructions are correct; only the\n"
              "  register MAP differs. DO NOT file WALL. Fixes, in order:\n"
              "    1. If your draft declares registers as locals (u32 r27, r28...), STOP -\n"
              "       raw-m2c register locals PIN the coloring. Rewrite with NAMED locals\n"
              "       (msgId, basePtr, entry, result) so CW's allocator is free.\n"
              "    2. Reorder the local DECLARATIONS so co-surviving locals claim the\n"
              "       target's registers (highest-reg-first: the local that must land in\n"
              "       r30 declared before the r29 one, etc.). See docs/CRACK_LEVERS.md.\n"
              "    3. Kill any mr/li round-trip: assign a call's result straight into its\n"
              "       destination local (no intermediate temp -> no spare mr through r0).")
        return 0
    if sched:
        print("  VERDICT: SCHEDULING - same instr set, reordered. Try #pragma scheduling\n"
              "  off / operand-eval-order swap ONCE; if it holds, save; else FILE WALL.")
        return 1
    print("  VERDICT: UNCLEAR - inspect band.py diff manually.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
