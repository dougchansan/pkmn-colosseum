// ExportDecompilation.java
//
// Ghidra headless script that iterates every function in the loaded program,
// decompiles it with the built-in decompiler, and writes the resulting C to
// an output file.  Designed for batch use via analyzeHeadless.
//
// Output format:
//   /* ===== FUN_XXXXXXXX  addr=0xXXXXXXXX  size=0xNNNN ===== */
//   <decompiled C>
//
// The output path is controlled by the script variable "outputDir".
// If not set, it defaults to <project_root>/build/ghidra_output/raw_decompilation.c
//
// @category PokemonColosseum

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Program;
import ghidra.program.model.address.Address;
import ghidra.util.task.TaskMonitor;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

public class ExportDecompilation extends GhidraScript {

    // Decompiler timeout per function (seconds).
    private static final int DECOMPILE_TIMEOUT = 60;

    @Override
    protected void run() throws Exception {
        Program program = currentProgram;
        if (program == null) {
            printerr("ERROR: No program loaded.");
            return;
        }

        // Determine output file path.
        // Check for a script argument first (passed via -scriptArgs on the
        // analyzeHeadless command line).  Fall back to a default location.
        String outputPath = null;
        String[] args = getScriptArgs();
        if (args != null && args.length > 0 && args[0].length() > 0) {
            outputPath = args[0];
        }
        if (outputPath == null || outputPath.isEmpty()) {
            // Default: write next to the imported file's directory.
            String programDir = program.getExecutablePath();
            if (programDir != null) {
                File pd = new File(programDir).getParentFile();
                if (pd != null) {
                    outputPath = new File(pd, "raw_decompilation.c").getAbsolutePath();
                }
            }
            if (outputPath == null || outputPath.isEmpty()) {
                outputPath = System.getProperty("user.home") + File.separator +
                             "ghidra_raw_decompilation.c";
            }
        }

        File outputFile = new File(outputPath);
        File parentDir = outputFile.getParentFile();
        if (parentDir != null && !parentDir.exists()) {
            parentDir.mkdirs();
        }

        println("ExportDecompilation: Output -> " + outputFile.getAbsolutePath());

        // Set up the decompiler.
        DecompInterface decomp = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        // Simplify output: turn off certain display options for cleaner C.
        options.setEliminateUnreachable(true);
        decomp.setOptions(options);

        if (!decomp.openProgram(program)) {
            printerr("ERROR: Failed to open program in decompiler: " +
                      decomp.getLastMessage());
            return;
        }

        // Iterate all functions.
        FunctionIterator funcIter = program.getFunctionManager()
                                           .getFunctions(true); // forward order

        int totalFunctions = 0;
        int decompiled = 0;
        int failed = 0;

        try (PrintWriter writer = new PrintWriter(
                new BufferedWriter(new FileWriter(outputFile)))) {

            // File header.
            writer.println("/*");
            writer.println(" * Auto-generated Ghidra decompilation output");
            writer.println(" * Program: " + program.getName());
            writer.println(" * Processor: " + program.getLanguage().getLanguageID());
            writer.println(" *");
            writer.println(" * WARNING: This is raw decompiler output. Variable names,");
            writer.println(" * types, and control flow may not match the original source.");
            writer.println(" * Use process_ghidra_output.py to clean up before importing.");
            writer.println(" */");
            writer.println();

            // Common Ghidra type definitions (so the output is more self-contained).
            writer.println("/* Ghidra type aliases */");
            writer.println("typedef unsigned char   undefined;");
            writer.println("typedef unsigned char   undefined1;");
            writer.println("typedef unsigned short  undefined2;");
            writer.println("typedef unsigned int    undefined4;");
            writer.println("typedef unsigned long long undefined8;");
            writer.println("typedef unsigned char   byte;");
            writer.println("typedef unsigned short  ushort;");
            writer.println("typedef unsigned int    uint;");
            writer.println("typedef unsigned long long ulonglong;");
            writer.println("typedef int             bool;");
            writer.println("typedef long long       longlong;");
            writer.println();

            while (funcIter.hasNext()) {
                if (monitor.isCancelled()) {
                    println("Cancelled by user.");
                    break;
                }

                Function func = funcIter.next();
                totalFunctions++;

                Address entryAddr = func.getEntryPoint();
                long bodySize = func.getBody().getNumAddresses();
                String funcName = func.getName();

                // Skip external / thunk functions with no body.
                if (func.isExternal()) {
                    continue;
                }

                // Decompile.
                DecompileResults result = decomp.decompileFunction(
                        func, DECOMPILE_TIMEOUT, monitor);

                if (result == null || !result.decompileCompleted()) {
                    String errMsg = (result != null) ? result.getErrorMessage()
                                                      : "null result";
                    writer.println("/* ===== " + funcName +
                                   "  addr=0x" + entryAddr.toString() +
                                   "  size=0x" + Long.toHexString(bodySize) +
                                   "  DECOMPILE_FAILED: " + errMsg + " ===== */");
                    writer.println();
                    failed++;
                    continue;
                }

                String decompiledC = result.getDecompiledFunction().getC();
                if (decompiledC == null || decompiledC.trim().isEmpty()) {
                    writer.println("/* ===== " + funcName +
                                   "  addr=0x" + entryAddr.toString() +
                                   "  size=0x" + Long.toHexString(bodySize) +
                                   "  EMPTY_OUTPUT ===== */");
                    writer.println();
                    failed++;
                    continue;
                }

                // Write the function with a header comment.
                writer.println("/* ===== " + funcName +
                               "  addr=0x" + entryAddr.toString() +
                               "  size=0x" + Long.toHexString(bodySize) +
                               " ===== */");
                writer.println(decompiledC);
                writer.println();
                decompiled++;

                // Progress reporting every 200 functions.
                if (totalFunctions % 200 == 0) {
                    println("Progress: " + totalFunctions + " functions processed, " +
                            decompiled + " decompiled, " + failed + " failed");
                }
            }
        }

        decomp.dispose();

        println("========================================");
        println("ExportDecompilation complete.");
        println("  Total functions seen:  " + totalFunctions);
        println("  Successfully decompiled: " + decompiled);
        println("  Failed / empty:          " + failed);
        println("  Output: " + outputFile.getAbsolutePath());
        println("========================================");
    }
}
