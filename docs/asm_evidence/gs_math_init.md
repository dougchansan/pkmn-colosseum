# GSmath quantisation setup in Pokémon Colosseum (GC6E01)

This is a first-party (game-code) assembly routine, admitted under the tier
described in README.md ("First-party (game) assembly"). It is the whole of
Genius Sonority's GSmath init module (`src/game/gs_math_init.c`, .text
0x800E0790–0x800E07E4; in XD it is the sole content of init.cpp). No other
decompilation of this code exists, so the routine is backed by a compiler
probe.

## GSmathInit

Retail address 0x800E0790.

- Why it cannot be C: the routine programs the Gekko paired-single quantisation registers GQR2–GQR5 with `mtspr GQR2..GQR5, r3` (SPRs 914–917), loading each value as `li r3, n; oris r3, r3, n` (n = 4, 5, 6, 7: u8/u16/s8/s16 load/store scales for the paired-single loads the GS math code uses). MWCC has no C construct or intrinsic that writes a GQR; the Dolphin SDK itself sets GQRs from assembly (`OSInitFastCast`). The rest of the routine is a standard frame around two ordinary calls (`bl GSsrandTime`, `bl GSmathInitCosTable`), which is why the whole body is kept as assembly: inline asm inside a C function is not admitted.
- Compiler probe: docs/asm_evidence/probes/GSmathInit.txt (candidate docs/asm_evidence/probes/GSmathInit.c, the two calls with the GQR values stored through an ordinary variable) ends "verdict: no-match (20 compilers compared)" under the unit's GC/1.3 -O4,p flags.
- Origin: Genius Sonority GS engine, GSmath module initialisation (game code, hand-written GQR setup), called once at boot.
