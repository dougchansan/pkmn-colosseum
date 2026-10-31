# MetroTRK exception-vector templates for GC6E01.
# Hardware entry code and reserved vector slots; not an ABI-callable C function.
# Evidence and public source-family provenance:
# docs/asm_evidence/interrupt_vectors.md
#
# The debugger copies selected 0x100-byte slots from this table. Keep the
# original relative reset branch and the shared performance-monitor slot.
# Handler addresses are ordinary symbolic relocations, never copied words.

.section .init, "ax", @progbits
.balign 4, 0

# A vector saves the interrupted registers before arranging the state that
# TRKInterruptHandler expects: r2 = interrupted PC, r3 = vector, r4 = MSR.
# SRR1 enables instruction/data translation for the rfi into that handler.
.macro trk_redirect vector
    mtsprg 1, r2
    mtsprg 2, r3
    mtsprg 3, r4
    mfsrr0 r2
    mfsrr1 r4
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    lis r3, TRKInterruptHandler@h
    ori r3, r3, TRKInterruptHandler@l
    mtsrr0 r3
    li r3, \vector
    rfi
.endm

# Vector positions are offsets within the table. Unused space is zero-filled;
# slot placement does not depend on the assembled length of earlier entries.
.macro vector_slot offset
    .org gTRKInterruptVectorTable + \offset, 0
.endm

.macro trk_vector vector
    vector_slot \vector
    trk_redirect \vector
.endm

# Legacy 603-family TLB entry switches out of the temporary GPR bank when
# MSR[TGPR] is set. The two SPRG1 saves straddle that switch intentionally.
.macro trk_tlb_redirect vector
    vector_slot \vector
    mtsprg 1, r2
    mfcr r2
    mtsprg 2, r2
    mfmsr r2
    andis. r2, r2, 2
    beq .Lrestore_cr_\@
    mfmsr r2
    xoris r2, r2, 2
    sync
    mtmsr r2
    sync
    mtsprg 1, r2
.Lrestore_cr_\@:
    mfsprg r2, 2
    mtcrf 0xff, r2
    mfsprg r2, 1
    trk_redirect \vector
.endm

.global gTRKInterruptVectorTable
gTRKInterruptVectorTable:
    .asciz "Metrowerks Target Resident Kernel for PowerPC"
    .balign 4, 0

# System reset. This relative transfer is retained exactly as stored; copying
# the slot to a hardware vector address does not relocate the branch again.
    vector_slot 0x100
    b __TRK_reset

# Machine check invalidates the instruction/data cache lines implicated by
# SRR0 and DAR before entering the ordinary debugger exception path.
    vector_slot 0x200
    mtsprg 1, r2
    mfsrr0 r2
    icbi 0, r2
    mfdar r2
    dcbi 0, r2
    mfsprg r2, 1
    trk_redirect 0x200

    trk_vector 0x300             # Data storage interrupt
    trk_vector 0x400             # Instruction storage interrupt
    trk_vector 0x500             # External interrupt
    trk_vector 0x600             # Alignment
    trk_vector 0x700             # Program
    trk_vector 0x800             # Floating-point unavailable
    trk_vector 0x900             # Decrementer

# Preserve the existing public labels inside the reserved 0xA00/0xB00 space.
    vector_slot 0xB44
.global lbl_80003FF0
lbl_80003FF0:
    vector_slot 0xB54
.global lbl_80004000
lbl_80004000:

    trk_vector 0xC00             # System call
    trk_vector 0xD00             # Trace
    trk_vector 0xE00             # Floating-point assist

# Performance monitor and vector-unit unavailable share one hardware slot.
    vector_slot 0xF00
    b .Lperformance_monitor
    vector_slot 0xF20
    trk_redirect 0xF20
.Lperformance_monitor:
    trk_redirect 0xF00

    trk_tlb_redirect 0x1000      # Instruction TLB miss
    trk_tlb_redirect 0x1100      # Data-load TLB miss
    trk_tlb_redirect 0x1200      # Data-store TLB miss

    trk_vector 0x1300            # Instruction address breakpoint
    trk_vector 0x1400            # System management
    trk_vector 0x1600            # Denormal/Java mode
    trk_vector 0x1700            # Thermal management
    trk_vector 0x1C00            # Data breakpoint
    trk_vector 0x1D00            # Instruction breakpoint
    trk_vector 0x1E00            # Peripheral breakpoint
    trk_vector 0x1F00            # Non-maskable development port

.global gTRKInterruptVectorTableEnd
gTRKInterruptVectorTableEnd:
