# Co-op input probe (gate G1).
#
# Two trampolines that observe the retail input backend without changing its
# behaviour. Both are entered by an unconditional `b` planted over the first
# instruction of the target, so the link register is never disturbed; each
# executes the displaced original instruction itself and then jumps to the
# target's second instruction via CTR.
#
# Only volatile registers are touched (r0, r3-r8, CTR, CR0). Both hook sites
# are function entry points, where r3 is the sole incoming argument, so
# preserving r3 is sufficient for ABI correctness.
#
# See docs/coop/hook_inventory.md for the address/field evidence.

    .section .text

    .global coop_entry
coop_entry:
    blr


# ---------------------------------------------------------------------------
# Poll hook — entry of fn_800F7F64(padId)
#
# Displaced instruction: stwu r1, -0x20(r1)   (94 21 FF E0)
# Snapshots all four pad-table slots on every poll, so the capture shows the
# logical ID, connection status and input of each slot regardless of which IDs
# story mode actually registered.
# ---------------------------------------------------------------------------
    .global coop_hook_poll
coop_hook_poll:
    stwu    r1, -0x40(r1)
    stw     r3, 0x08(r1)
    stw     r4, 0x0C(r1)
    stw     r5, 0x10(r1)
    stw     r6, 0x14(r1)
    stw     r7, 0x18(r1)

    lis     r4, coop_stats@ha
    addi    r4, r4, coop_stats@l

    lwz     r0, 0x08(r4)            # poll_total++
    addi    r0, r0, 1
    stw     r0, 0x08(r4)
    stw     r3, 0x38(r4)            # last_poll_id

    lis     r5, 0x8040              # pad table base lbl_80401C10
    addi    r5, r5, 0x1C10
    addi    r6, r4, 0x40            # slot snapshot area
    li      r7, 4

coop_poll_loop:
    lwz     r0, 0x00(r5)            # logical pad ID (0 = free slot)
    stw     r0, 0x00(r6)
    lhz     r0, 0x24(r5)            # held buttons
    stw     r0, 0x04(r6)
    lwz     r0, 0x30(r5)            # previous buttons
    stw     r0, 0x08(r6)
    lbz     r0, 0x58(r5)            # main stick X, processed
    stw     r0, 0x0C(r6)
    lbz     r0, 0x59(r5)            # main stick Y, processed
    stw     r0, 0x10(r6)
    lbz     r0, 0x26(r5)            # main stick X, raw
    stw     r0, 0x14(r6)
    lbz     r0, 0x27(r5)            # main stick Y, raw
    stw     r0, 0x18(r6)
    lwz     r0, 0x0C(r5)            # status word
    stw     r0, 0x1C(r6)
    addi    r5, r5, 0x6C
    addi    r6, r6, 0x20
    addic.  r7, r7, -1
    bne     coop_poll_loop

    lis     r0, 0x800F              # continue at fn_800F7F64 + 4
    ori     r0, r0, 0x7F68
    mtctr   r0

    lwz     r3, 0x08(r1)
    lwz     r4, 0x0C(r1)
    lwz     r5, 0x10(r1)
    lwz     r6, 0x14(r1)
    lwz     r7, 0x18(r1)
    addi    r1, r1, 0x40

    stwu    r1, -0x20(r1)           # displaced original instruction
    bctr


# ---------------------------------------------------------------------------
# Registration hook — entry of fn_800F80B0(padId)
#
# Displaced instruction: lis r4, lbl_80401C10@ha   (3C 80 80 40)
# Counts every registration attempt per logical ID. This is what answers
# whether story mode ever claims a slot for pad 2.
# ---------------------------------------------------------------------------
    .global coop_hook_register
coop_hook_register:
    stwu    r1, -0x20(r1)
    stw     r3, 0x08(r1)
    stw     r5, 0x0C(r1)

    lis     r5, coop_stats@ha
    addi    r5, r5, coop_stats@l

    lwz     r0, 0x0C(r5)            # reg_total++
    addi    r0, r0, 1
    stw     r0, 0x0C(r5)
    stw     r3, 0x3C(r5)            # last_reg_id

    cmpwi   r3, 1
    blt     coop_reg_other
    cmpwi   r3, 4
    bgt     coop_reg_other
    addi    r0, r3, -1
    rlwinm  r0, r0, 2, 0, 29
    add     r5, r5, r0
    lwz     r0, 0x20(r5)            # reg_count[id-1]++
    addi    r0, r0, 1
    stw     r0, 0x20(r5)
    b       coop_reg_done

coop_reg_other:
    lwz     r0, 0x34(r5)            # reg_other++
    addi    r0, r0, 1
    stw     r0, 0x34(r5)

coop_reg_done:
    lis     r0, 0x800F              # continue at fn_800F80B0 + 4
    ori     r0, r0, 0x80B4
    mtctr   r0

    lwz     r3, 0x08(r1)
    lwz     r5, 0x0C(r1)
    addi    r1, r1, 0x20

    lis     r4, 0x8040              # displaced original instruction
    bctr


# ---------------------------------------------------------------------------
# Per-frame driver hook — entry of fn_8000604C
#
# Displaced instruction: stwu r1, -0x10(r1)   (94 21 FF F0)
# fn_8000604C is registered via GSgappCreate(1, 0, 0, fn_8000604C) as a
# per-frame task. It walks logical IDs 1..4 and calls fn_800F7F64(id) only
# when the gate fn_8008ABA0(id) returns 0. Counting entries here separates
# "the game never reaches the per-frame task" from "the task runs but the
# gate suppresses the poll".
# ---------------------------------------------------------------------------
    .global coop_hook_frame
coop_hook_frame:
    stwu    r1, -0x20(r1)
    stw     r3, 0x08(r1)

    lis     r3, coop_stats@ha
    addi    r3, r3, coop_stats@l
    lwz     r0, 0x10(r3)            # frame_total++
    addi    r0, r0, 1
    stw     r0, 0x10(r3)

    lis     r0, 0x8000              # continue at fn_8000604C + 4
    ori     r0, r0, 0x6050
    mtctr   r0

    lwz     r3, 0x08(r1)
    addi    r1, r1, 0x20

    stwu    r1, -0x10(r1)           # displaced original instruction
    bctr


# ---------------------------------------------------------------------------
# Gate hook — exit path of fn_8008ABA0
#
# Counts how often the gate allows a poll (r3 == 0) versus blocks it.
# Hooked at the final clrlwi so the return value is already in r4.
# Displaced instruction: clrlwi r3, r4, 24   (54 83 06 3E)
# ---------------------------------------------------------------------------
    .global coop_hook_gate
coop_hook_gate:
    stwu    r1, -0x20(r1)
    stw     r5, 0x08(r1)
    stw     r6, 0x0C(r1)

    lis     r5, coop_stats@ha
    addi    r5, r5, coop_stats@l
    clrlwi  r6, r4, 24
    cmplwi  r6, 0x0
    bne     coop_gate_blocked
    lwz     r0, 0x14(r5)            # gate_allow++
    addi    r0, r0, 1
    stw     r0, 0x14(r5)
    b       coop_gate_done
coop_gate_blocked:
    lwz     r0, 0x18(r5)            # gate_block++
    addi    r0, r0, 1
    stw     r0, 0x18(r5)
coop_gate_done:
    lis     r0, 0x8008              # continue at fn_8008ABA0 + 4
    ori     r0, r0, 0xABE0
    mtctr   r0

    lwz     r5, 0x08(r1)
    lwz     r6, 0x0C(r1)
    addi    r1, r1, 0x20

    clrlwi  r3, r4, 24              # displaced original instruction
    bctr


# ---------------------------------------------------------------------------
# Telemetry block.
#
#   +0x00  magic 'COOP'
#   +0x04  version
#   +0x08  poll_total
#   +0x0C  reg_total
#   +0x10  poll_count[4]        (reserved, not yet written)
#   +0x20  reg_count[4]         indexed by logical ID 1..4
#   +0x30  poll_other
#   +0x34  reg_other            registration attempts outside 1..4
#   +0x38  last_poll_id
#   +0x3C  last_reg_id
#   +0x40  slot[4], 0x20 bytes each:
#            +0x00 logical ID   +0x04 held      +0x08 prev     +0x0C stickX
#            +0x10 stickY       +0x14 rawX      +0x18 rawY     +0x1C status
# ---------------------------------------------------------------------------
    .section .data
    .global coop_stats
    .align 5
coop_stats:
    .long 0x434F4F50
    .long 1
    .long 0
    .long 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
    .long 0, 0, 0, 0, 0, 0, 0, 0
