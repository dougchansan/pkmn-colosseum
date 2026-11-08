# GSthread context-switch primitives in Pokémon Colosseum (GC6E01)

These are first-party (game-code) assembly routines, admitted under the tier
described in README.md ("First-party (game) assembly"). They come from
Genius Sonority's GSthread module, which is the cooperative fibre scheduler of
the GS engine. Its source file is GSthread.cpp, at
`game/pxdvs/GSAPI/GSthread/GSthread.cpp` in the Pokémon XD reference splits.
No other decompilation of this code exists, so each routine is backed by a
compiler probe instead. The probe compiles the closest C candidate with every
GameCube MWCC build in `build/compilers/GC`, using the unit's own flags, and
compares the result with the retail instruction words.

Units:

- `src/game/gs_thread_exact_800F0030.c` covers .text 0x800F0030–0x800F036C,
  the head of the retail GSthread translation unit. It holds the register
  save/restore routines and the two context switches.
- `src/game/gs_thread_exact_800F106C.c` covers .text 0x800F106C–0x800F10E8,
  the GS VM native-call trampoline.

The save/restore routines share one convention. They take no arguments. They
read the current context block from `lbl_8047AC1C` (a GSThreadCtx: r0–r31 at
+0x00, LR at +0x80, CTR at +0x84, f0–f31 at +0x88). They borrow r3, parking it
in an 8-byte frame of their own, and return with every other register either
untouched or freshly loaded. The `bl` calls in threadExecute and _threadSwitch
are ordinary calls between these routines; none of them branches out with `b`.

## threadLoadGPRRegisters

Retail address 0x800F0030.

- Why it cannot be C: it loads r0, r2 and r4–r31 directly from the context block (`lwz r0`, `lwz r2`, `lwz r13`, `lwz r14`–`lwz r31`), overwriting the small-data base registers and every callee-saved register, inside a hand-built 8-byte frame that saves and restores only r3. MWCC never assigns r0/r2/r13 as destinations of user loads and never clobbers callee-saved registers without saving them.
- Compiler probe: docs/asm_evidence/probes/threadLoadGPRRegisters.txt (candidate docs/asm_evidence/probes/threadLoadGPRRegisters.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## threadLoadFPRRegisters

Retail address 0x800F00C0.

- Why it cannot be C: it reloads all 32 FPRs, f0–f31, with `lfd` from context+0x88 and keeps nothing in them, including the callee-saved f14–f31, which MWCC would have to save and restore. Its frame is hand-built and holds only r3.
- Compiler probe: docs/asm_evidence/probes/threadLoadFPRRegisters.txt (candidate docs/asm_evidence/probes/threadLoadFPRRegisters.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## threadSaveGPRRegisters

Retail address 0x800F015C.

- Why it cannot be C: it stores the live values of r0, r2 and r3–r31 (`stw r0`, `stw r2`, `stw r13`, …) into the context block. C has no way to name the caller's register contents, and MWCC never emits stores of r2/r13 or of unallocated callee-saved registers.
- Compiler probe: docs/asm_evidence/probes/threadSaveGPRRegisters.txt (candidate docs/asm_evidence/probes/threadSaveGPRRegisters.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## threadSaveFPRRegisters

Retail address 0x800F01F0.

- Why it cannot be C: it stores the live values of f0–f31 with `stfd` into context+0x88. C cannot read the caller's floating-point register file, and the closest C (storing values it computes) yields different code under every compiler.
- Compiler probe: docs/asm_evidence/probes/threadSaveFPRRegisters.txt (candidate docs/asm_evidence/probes/threadSaveFPRRegisters.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## threadExecute

Retail address 0x800F028C.

- Why it cannot be C: it builds a 12-byte frame and parks r3 and LR in it, saves the scheduler context, and then stores r1 into the saved context (`stw r1, 0x4(r3)`). It switches to the thread context and reloads r1 from it (`lwz r1, 0x4(r3)`), plants a -1 back-chain word at the new stack top, and jumps into the thread through CTR with `bctr` and a forged LR. Stack-pointer swaps, a `bctr` that never returns, and r3 surviving across the register-loading calls are outside anything MWCC generates.
- Compiler probe: docs/asm_evidence/probes/threadExecute.txt (candidate docs/asm_evidence/probes/threadExecute.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## GSthreadGetArg

Retail address 0x800F02F4.

- Why it cannot be C: it is the landing pad that threadExecute's frame returns through. It reloads LR and the original r3 from the 12-byte frame that threadExecute built, pops that frame (`addi r1, r1, 0xc`) and returns. It has no frame of its own and pops one it did not allocate, which no C function body compiles to.
- Compiler probe: docs/asm_evidence/probes/GSthreadGetArg.txt (candidate docs/asm_evidence/probes/GSthreadGetArg.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## _threadSwitch

Retail address 0x800F0308.

- Why it cannot be C: it is frameless. It parks r3 and r5 in small-data globals and keeps LR in r5 across a `bl` (`mflr r5` and later `stw r5, 0x80(r3)`). It saves the running context, including r1, then loads the scheduler context, reloads r1 and LR from it, and returns into that context with `blr`. That is a stack and return-address swap that MWCC cannot express. Its frameless call sequence would also need a frame under MWCC.
- Compiler probe: docs/asm_evidence/probes/_threadSwitch.txt (candidate docs/asm_evidence/probes/_threadSwitch.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, GSthread.cpp (cooperative thread scheduler); hand-written assembly in the game's own code.

## cms_CallStdTrampoline

Retail address 0x800F106C (formerly `cms_CallStdTrampoline`; named in PR #637).

- Why it cannot be C: its prologue stores r0 into the new frame before `mflr` (`stw r0, 0x8(r1)` then `mflr r0`) and restores it in the epilogue, a save that MWCC never emits. The body loads eight integer and eight float arguments in descending order and calls the native function through CTR with `crclr 6` (the variadic-call flag). Under every compiler, the closest C call through a function pointer produces a different prologue, epilogue and load order.
- Compiler probe: docs/asm_evidence/probes/fn_800F106C.txt (candidate docs/asm_evidence/probes/fn_800F106C.c) ends "verdict: no-match (20 compilers compared)".
- Origin: Genius Sonority GS engine, the GS VM native-call path that follows GSthread.cpp in the binary; hand-written assembly in the game's own code.
