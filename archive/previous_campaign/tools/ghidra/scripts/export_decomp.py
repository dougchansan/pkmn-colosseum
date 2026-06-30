# Ghidra headless script: export decompiled C for a list of function addresses
#
# Invocation (via analyzeHeadless):
#   -postScript export_decomp.py <path/to/function_list.txt> <output_dir>
#
# function_list.txt format: one hex address per line (with or without 0x prefix)
# Lines starting with # are comments.
#
# Output: <output_dir>/fn_<ADDR>_ghidra.c with the Ghidra decompiler's C for
# that function. Addresses that don't resolve to a function are logged.
#@category Pokemon

import os

from ghidra.app.decompiler import DecompInterface, DecompileOptions
from ghidra.util.task import ConsoleTaskMonitor


def parse_list(path):
    addrs = []
    with open(path, "r") as f:
        for raw in f:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            line = line.split("#")[0].strip()
            if line.lower().startswith("0x"):
                line = line[2:]
            try:
                addrs.append(int(line, 16))
            except ValueError:
                print("skipping unparseable line: %r" % raw)
    return addrs


def run(program, list_path, out_dir):
    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)

    decomp = DecompInterface()
    opts = DecompileOptions()
    decomp.setOptions(opts)
    decomp.openProgram(program)

    monitor = ConsoleTaskMonitor()
    fn_mgr = program.getFunctionManager()
    space = program.getAddressFactory().getDefaultAddressSpace()

    addrs = parse_list(list_path)
    print("exporting %d functions" % len(addrs))
    ok = 0
    fail = 0

    for a in addrs:
        addr = space.getAddress(a)
        fn = fn_mgr.getFunctionAt(addr)
        if fn is None:
            fn = fn_mgr.getFunctionContaining(addr)
        if fn is None:
            print("no function at 0x%08x" % a)
            fail += 1
            continue

        result = decomp.decompileFunction(fn, 120, monitor)
        if result is None or not result.decompileCompleted():
            msg = result.getErrorMessage() if result else "no result"
            print("decompile failed for fn_%08X: %s" % (a, msg))
            fail += 1
            continue

        code = result.getDecompiledFunction().getC()
        out_path = os.path.join(out_dir, "fn_%08X_ghidra.c" % a)
        with open(out_path, "w") as f:
            f.write(code)
        ok += 1

    print("exported %d, failed %d" % (ok, fail))


args = getScriptArgs()
if len(args) < 2:
    print("usage: export_decomp.py <function_list.txt> <output_dir>")
else:
    run(currentProgram, args[0], args[1])
