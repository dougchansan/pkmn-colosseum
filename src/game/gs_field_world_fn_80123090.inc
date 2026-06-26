    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r4, 0x0
    li r5, 0x82
    stw r0, 0x14(r1)
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r4, r3, 16
    bne @L_801230BC
    li r3, 0x0
    b @L_801230D0
@L_801230BC:
    li r3, 0x0
    li r5, 0x7
    li r6, 0x0
    bl itemGetStatus
    clrlwi r3, r3, 16
@L_801230D0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
