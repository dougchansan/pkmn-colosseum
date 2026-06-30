// Ghidra headless script: seed known fn_XXXXXXXX symbols from a C source file.
// Invocation via analyzeHeadless:
//   -preScript SeedSymbols.java <path/to/gs_title.c>
// @category Pokemon

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.SourceType;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.HashSet;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SeedSymbols extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 1) {
            println("usage: SeedSymbols <path/to/gs_title.c>");
            return;
        }
        String cPath = args[0];

        FunctionManager fnMgr = currentProgram.getFunctionManager();
        Pattern pat = Pattern.compile("\\bfn_([0-9A-Fa-f]{8})\\b");
        Set<String> seen = new HashSet<>();
        int created = 0;
        int renamed = 0;

        try (BufferedReader br = new BufferedReader(new FileReader(cPath))) {
            String line;
            StringBuilder sb = new StringBuilder();
            while ((line = br.readLine()) != null) sb.append(line).append('\n');
            Matcher m = pat.matcher(sb);
            while (m.find()) {
                String hex = m.group(1).toLowerCase();
                if (!seen.add(hex)) continue;
                long addrVal = Long.parseLong(hex, 16);
                Address addr = currentProgram.getAddressFactory().getDefaultAddressSpace()
                        .getAddress(addrVal);
                if (currentProgram.getMemory().getBlock(addr) == null) continue;
                String name = "fn_" + hex.toUpperCase();
                Function fn = fnMgr.getFunctionAt(addr);
                if (fn == null) {
                    try {
                        fnMgr.createFunction(name, addr, null, SourceType.IMPORTED);
                        created++;
                    } catch (Exception e) {
                        println("create failed " + name + " @ " + addr + ": " + e);
                    }
                } else {
                    try {
                        fn.setName(name, SourceType.IMPORTED);
                        renamed++;
                    } catch (Exception e) {
                        println("rename failed " + name + " @ " + addr + ": " + e);
                    }
                }
            }
        }
        println("Seeded " + seen.size() + " addresses (created=" + created + ", renamed=" + renamed + ")");
    }
}
