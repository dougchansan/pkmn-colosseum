    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_801248E8
    li r3, 0x0
    b @L_80124960
@L_801248E8:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    li r3, 0x0
    mr r4, r31
    li r5, 0x17
    li r6, 0x1
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_80124934
    mr r4, r31
    li r3, 0x0
    li r5, 0x17
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 16
    b @L_80124960
@L_80124934:
    mr r3, r30
    li r4, 0x0
    li r5, 0xb7
    li r6, 0x0
    bl fn_8012640C
    clrlwi r6, r3, 24
    mr r4, r31
    li r3, 0x0
    li r5, 0x17
    bl fn_8012640C
    clrlwi r3, r3, 16
@L_80124960:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
