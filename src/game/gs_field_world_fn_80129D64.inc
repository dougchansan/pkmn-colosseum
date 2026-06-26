    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r5, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r29, r3
    stw r28, 0x10(r1)
    mr r28, r4
    li r4, 0x2
    bl fn_8012A5B0
    mr r30, r3
    mr r3, r29
    li r4, 0x1
    li r5, 0x0
    bl fn_8012A5B0
    mr r0, r3
    mr r3, r28
    mr r29, r0
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r28
    li r4, 0x0
    li r5, 0x76
    li r6, 0x0
    bl fn_8012640C
    cmplw r30, r31
    mr r4, r3
    beq @L_80129DF0
    li r3, 0x0
    b @L_80129E00
    @L_80129DF0:
    mr r3, r29
    bl fn_800F9EE4
    cntlzw r0, r3
    srwi r3, r0, 5
    @L_80129E00:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
