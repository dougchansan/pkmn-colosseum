    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80122A94
    li r3, 0x0
    b @L_80122AC8
@L_80122A94:
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
    mulli r0, r31, 0x64
    clrlwi r3, r3, 16
    divw r3, r0, r3
@L_80122AC8:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
