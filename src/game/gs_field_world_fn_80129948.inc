    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r7, 0x0
    li r8, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r6
    li r6, 0x0
    stw r30, 0x18(r1)
    mr r30, r5
    addi r5, r1, 0x8
    bl fn_80129BC8
    mr. r4, r3
    beq @L_801299B0
    lhz r3, 0x8(r1)
    rlwinm r0, r30, 0, 16, 31
    cmplw r0, r3
    bge @L_801299B0
    rlwinm r0, r31, 0, 16, 31
    cmplw r0, r3
    bge @L_801299B0
    clrlslwi r3, r30, 16, 2
    clrlslwi r0, r31, 16, 2
    add r3, r4, r3
    add r4, r4, r0
    bl fn_80140A9C
@L_801299B0:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
