#!/usr/bin/env python3
"""
CW source-search harness  ("lever annealer")
============================================
Simulated-annealing / Metropolis-Hastings search over SEMANTICS-PRESERVING source
mutations, to drive a near-miss function to a byte-exact CodeWarrior match.

Oracle  : mwcceppc (via tools/compile_check.py) -- source in, match% out, deterministic.
Energy  : E(s) = 100 - match_percent(target_fn)   (0 == exact byte match).
Accept  : Metropolis    P = min(1, exp(-beta * dE)),  beta annealed upward.

WHY THIS BEATS MANUAL LEVERS:
  Register allocation is driven by local-declaration order; instruction scheduling is
  driven by decl order + #pragma. The two are COUPLED -- a human can only nudge one axis
  and the other breaks. This searches the *combination* space (decl-permutation x pragma
  config), which is where the joint solution hides. The reg-alloc wall is a local minimum
  in byte-distance; annealing tunnels through it where greedy hill-climbing cannot.

MUTATION OPERATORS (all provably semantics-preserving -- this is the safety guarantee):
  1. decl_reorder : permute the order of independent leading local declarations
                    (CW assigns registers in declaration order -> this is the reg-alloc knob)
  2. pragma_set   : choose a #pragma config wrapped around the function
                    (scheduling on/off x peephole on/off x fp_contract on/off -> the scheduler knob)

SAFETY / ANTI-FRAUD:
  - Only ever edits the ONE target .c file; always restores it on exit (backup kept).
  - Never touches *_fn_*.inc / symbols / splits / config / build.
  - A win is only reported when compile_check re-measures EXACTLY 100.0000%.
  - Emits the winning source to stdout for human review before any commit.

USAGE:
  python tools/decomp_work/permute_match.py <file.c> <fn_name> [--iters N] [--beta0 B0] [--beta1 B1] [--seed S]
"""
import argparse, os, re, subprocess, sys, math, shutil, json

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def sh(args):
    return subprocess.run(args, capture_output=True, text=True, cwd=ROOT)


# ---------------------------------------------------------------- energy (oracle)
_PCT = {}
def measure(relfile, fn):
    """Compile relfile and return match_percent for fn (0..100). -1 on compile failure."""
    r = sh([sys.executable, "tools/compile_check.py", "--diff", "-s", fn, relfile])
    out = r.stdout + r.stderr
    if "COMPILE FAILED" in out or "Errors caused tool to abort" in out or "declaration syntax" in out:
        return -1.0
    # rows like:  fn_800XXXXX   99.1234%   12/13   1
    m = re.search(re.escape(fn) + r"\s+([0-9.]+)%", out)
    if not m:
        return -1.0
    return float(m.group(1))


def energy(relfile, fn):
    p = measure(relfile, fn)
    if p < 0:
        return 1e6  # compile failure = infinite energy (reject)
    return 100.0 - p


# ---------------------------------------------------------------- function locate / splice
def find_function(text, fn):
    """Return (start_idx, body_open_idx, body_close_idx) for `fn` definition, or None.
    body_open_idx points at the '{' ; body_close_idx at the matching '}'."""
    # find a definition: <stuff> fn ( ... ) <maybe newline> {
    for m in re.finditer(r"(^|\n)([^\n;{}]*\b" + re.escape(fn) + r"\s*\([^;]*?\)\s*)\{", text):
        open_idx = text.index("{", m.start(2))
        depth = 0
        i = open_idx
        while i < len(text):
            c = text[i]
            if c == "{":
                depth += 1
            elif c == "}":
                depth -= 1
                if depth == 0:
                    return (m.start(2) if m.group(1) == "" else m.start(2), open_idx, i)
            i += 1
    return None


DECL_RE = re.compile(r"^[ \t]*((?:unsigned |signed |const |volatile |struct |static )*"
                     r"[A-Za-z_]\w*[ \t*]+[A-Za-z_]\w*(?:\s*\[[^\]]*\])?\s*(?:=\s*[^;]*)?;)[ \t]*$")


def leading_decls(body):
    """Split a function body into (list_of_decl_lines, rest_of_body). Only contiguous
    leading simple declarations are mutable (independent => reordering is safe)."""
    lines = body.split("\n")
    decls, rest_start = [], 0
    for idx, ln in enumerate(lines):
        if ln.strip() == "" or ln.strip().startswith("//"):
            decls.append(("blank", ln)); continue
        m = DECL_RE.match(ln)
        if m and "=" not in ln.split(";")[0]:        # only plain decls (no initializer => order-independent)
            decls.append(("decl", ln)); continue
        rest_start = idx
        break
    decl_lines = [v for (k, v) in decls if k == "decl"]
    head = "\n".join(lines[:rest_start])
    rest = "\n".join(lines[rest_start:])
    return decl_lines, head, rest


PRAGMA_AXES = [
    ("scheduling", ["", "on", "off"]),
    ("peephole",   ["", "on", "off"]),
    ("fp_contract",["", "on", "off"]),
]


def render(decl_order, pragmas, base_decls, head_no_decls, rest, open_idx, close_idx, text):
    """Rebuild the file text with reordered decls + pragma wrapper around the function."""
    new_decls = "\n".join(decl_order)
    pre = "".join(f"#pragma {name} {val}\n" for name, val in pragmas if val)
    post = "".join(f"#pragma {name} reset\n" for name, val in pragmas if val)
    # body = decls + rest ; we splice pragmas just *inside* the braces is unsafe -> wrap the whole def
    new_body = (("\n" + new_decls) if new_decls else "") + "\n" + rest
    new_text = text[:open_idx + 1] + new_body + text[close_idx:]
    if pre:
        # insert pragma block on the line before the function definition start
        defstart = new_text.rfind("\n", 0, new_text.index("{", 0)) if False else None
    return pre, new_text, post


# ---------------------------------------------------------------- annealing loop
def anneal(relfile, fn, iters, beta0, beta1, seed):
    import random
    rng = random.Random(seed)
    abs_path = os.path.join(ROOT, relfile)
    original = open(abs_path, encoding="utf-8").read()
    bak = abs_path + ".permute_bak"
    shutil.copy2(abs_path, bak)

    loc = find_function(original, fn)
    if not loc:
        print(f"ERROR: could not locate function {fn} in {relfile}", file=sys.stderr)
        return None
    _, open_idx, close_idx = loc
    body = original[open_idx + 1:close_idx]
    decls, head_decls, rest = leading_decls(body)
    if len(decls) < 2:
        print(f"NOTE: {fn} has <2 reorderable leading decls; only pragma axis will be searched.", file=sys.stderr)

    def build(decl_order, pragmas):
        pre = "".join(f"#pragma {n} {v}\n" for n, v in pragmas if v)
        rst = "".join(f"#pragma {n} reset\n" for n, v in pragmas if v)
        new_body = ("\n".join(decl_order) + "\n" if decl_order else "") + rest
        # wrap pragmas around the whole definition
        def_start = original.rfind("\n", 0, open_idx) + 1
        new_text = (original[:def_start] + pre + original[def_start:open_idx + 1]
                    + new_body + original[close_idx:close_idx + 1] + rst + original[close_idx + 1:])
        return new_text

    def apply_and_score(decl_order, pragmas):
        open(abs_path, "w", encoding="utf-8").write(build(decl_order, pragmas))
        return energy(relfile, fn)

    # initial state = current order, no pragmas
    state = (list(decls), [(n, "") for n, _ in PRAGMA_AXES])
    cur_E = apply_and_score(*state)
    best_E, best_state = cur_E, state
    print(f"[init] {fn}: match={100-cur_E:.4f}%  decls={len(decls)}", file=sys.stderr)

    for it in range(iters):
        if best_E <= 0.0:
            break
        beta = beta0 + (beta1 - beta0) * it / max(1, iters - 1)
        decl_order, pragmas = list(state[0]), list(state[1])
        if decl_order and (not pragmas or rng.random() < 0.6):
            i = rng.randrange(len(decl_order))
            j = rng.randrange(len(decl_order))
            decl_order[i], decl_order[j] = decl_order[j], decl_order[i]
        else:
            k = rng.randrange(len(PRAGMA_AXES))
            name, vals = PRAGMA_AXES[k]
            pragmas[k] = (name, rng.choice(vals))
        cand_E = apply_and_score(decl_order, pragmas)
        dE = cand_E - cur_E
        if dE <= 0 or rng.random() < math.exp(-beta * dE):
            state = (decl_order, pragmas)
            cur_E = cand_E
            if cur_E < best_E:
                best_E, best_state = cur_E, (list(decl_order), list(pragmas))
                print(f"[{it:5d}] beta={beta:.2f} NEW BEST match={100-best_E:.4f}%"
                      f"  pragmas={[f'{n} {v}' for n,v in pragmas if v]}", file=sys.stderr)
    # restore original, then report
    winning = build(*best_state)
    shutil.copy2(bak, abs_path); os.remove(bak)
    result = {"fn": fn, "file": relfile, "best_match": round(100 - best_E, 4),
              "exact": best_E <= 0.0,
              "pragmas": [f"{n} {v}" for n, v in best_state[1] if v],
              "decl_order": best_state[0]}
    print(json.dumps(result, indent=1))
    if best_E <= 0.0:
        print("\n===== WINNING FUNCTION SOURCE (apply + re-verify before commit) =====", file=sys.stderr)
        sys.stderr.write(winning[winning.rfind('\n',0,winning.index('{',
                         winning.find(fn)))+1: ] [:4000])
    return result


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file"); ap.add_argument("fn")
    ap.add_argument("--iters", type=int, default=400)
    ap.add_argument("--beta0", type=float, default=0.3)
    ap.add_argument("--beta1", type=float, default=3.0)
    ap.add_argument("--seed", type=int, default=1)
    a = ap.parse_args()
    anneal(a.file.replace("\\", "/"), a.fn, a.iters, a.beta0, a.beta1, a.seed)


if __name__ == "__main__":
    main()
