    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    beq @L_80125458
    mr r7, r4
    li r4, 0x0
    li r5, 0x79
    li r6, 0x0
    bl fn_801254B4
    mr r3, r31
    bl fn_8012546C
@L_80125458:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
