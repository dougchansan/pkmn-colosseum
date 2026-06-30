// Ghidra headless script: export decompiled C for a list of function addresses.
// Invocation via analyzeHeadless:
//   -postScript ExportDecomp.java <path/to/function_list.txt> <output_dir>
// @category Pokemon

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.util.task.ConsoleTaskMonitor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.List;

public class ExportDecomp extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            println("usage: ExportDecomp <function_list.txt> <output_dir>");
            return;
        }
        String listPath = args[0];
        String outDir = args[1];

        File outDirFile = new File(outDir);
        if (!outDirFile.exists()) outDirFile.mkdirs();

        List<Long> addrs = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(listPath))) {
            String line;
            while ((line = br.readLine()) != null) {
                String s = line.trim();
                if (s.isEmpty() || s.startsWith("#")) continue;
                int hash = s.indexOf('#');
                if (hash >= 0) s = s.substring(0, hash).trim();
                if (s.startsWith("0x") || s.startsWith("0X")) s = s.substring(2);
                try {
                    addrs.add(Long.parseLong(s, 16));
                } catch (NumberFormatException e) {
                    println("skipping unparseable: " + line);
                }
            }
        }
        println("exporting " + addrs.size() + " functions");

        DecompInterface decomp = new DecompInterface();
        decomp.setOptions(new DecompileOptions());
        decomp.openProgram(currentProgram);

        ConsoleTaskMonitor monitor = new ConsoleTaskMonitor();
        FunctionManager fnMgr = currentProgram.getFunctionManager();
        int ok = 0;
        int fail = 0;

        for (Long a : addrs) {
            Address addr = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(a);
            Function fn = fnMgr.getFunctionAt(addr);
            if (fn == null) fn = fnMgr.getFunctionContaining(addr);
            if (fn == null) {
                println(String.format("no function at 0x%08x", a));
                fail++;
                continue;
            }
            DecompileResults result = decomp.decompileFunction(fn, 120, monitor);
            if (result == null || !result.decompileCompleted()) {
                String msg = (result != null) ? result.getErrorMessage() : "null";
                println(String.format("decompile failed for fn_%08X: %s", a, msg));
                fail++;
                continue;
            }
            String code = result.getDecompiledFunction().getC();
            String outPath = new File(outDirFile, String.format("fn_%08X_ghidra.c", a)).getPath();
            try (FileWriter w = new FileWriter(outPath)) {
                w.write(code);
            }
            ok++;
        }
        println("exported " + ok + ", failed " + fail);
        decomp.dispose();
    }
}
