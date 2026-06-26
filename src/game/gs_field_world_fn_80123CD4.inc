    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr. r30, r3
    bne @L_80123CFC
    li r3, 0x0
    b @L_80123D40
@L_80123CFC:
    mr r6, r31
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_80123D1C
    li r3, 0x0
    b @L_80123D40
@L_80123D1C:
    mr r3, r30
    mr r6, r31
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    subfic r4, r3, 0x163
    subi r0, r3, 0x163
    or r0, r4, r0
    srwi r3, r0, 31
@L_80123D40:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
