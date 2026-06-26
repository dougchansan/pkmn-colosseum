    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80121434
    li r3, 0x0
    b @L_8012146C
@L_80121434:
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 16
    subf r0, r31, r0
    cntlzw r0, r0
    extrwi r3, r0, 8, 19
@L_8012146C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
