    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_801253B4
    li r3, 0x0
    b @L_8012540C
@L_801253B4:
    li r4, 0x0
    li r5, 0x75
    li r6, 0x0
    bl fn_8012640C
    mr r31, r3
    mr r3, r30
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    srwi r4, r31, 16
    clrlwi r0, r31, 16
    srwi r5, r3, 16
    clrlwi r6, r3, 16
    xor r0, r4, r0
    li r3, 0x8
    xor r0, r5, r0
    xor r0, r6, r0
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi r3, r0, 31
@L_8012540C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
