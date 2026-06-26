    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr. r30, r3
    beq @L_801249E0
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x17
    li r6, 0x1
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_801249C8
    li r31, 0x0
@L_801249C8:
    mr r3, r30
    clrlwi r7, r31, 24
    li r4, 0x0
    li r5, 0xb7
    li r6, 0x0
    bl fn_801254B4
@L_801249E0:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
