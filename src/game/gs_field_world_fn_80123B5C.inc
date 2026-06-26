    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    mr. r28, r3
    bne @L_80123B88
    li r3, -0x1
    b @L_80123C34
@L_80123B88:
    clrlwi r31, r4, 16
    li r29, 0x0
    b @L_80123C24
@L_80123B94:
    cmplwi r28, 0x0
    bne @L_80123BA4
    li r0, 0x0
    b @L_80123BF4
@L_80123BA4:
    extsb r30, r29
    mr r3, r28
    mr r6, r30
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_80123BCC
    li r0, 0x0
    b @L_80123BF4
@L_80123BCC:
    mr r3, r28
    mr r6, r30
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x163
    bne @L_80123BF0
    li r0, 0x0
    b @L_80123BF4
@L_80123BF0:
    li r0, 0x1
@L_80123BF4:
    clrlwi. r0, r0, 24
    beq @L_80123C20
    mr r3, r28
    extsb r6, r29
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpw r31, r3
    bne @L_80123C20
    mr r3, r29
    b @L_80123C34
@L_80123C20:
    addi r29, r29, 0x1
@L_80123C24:
    extsb r0, r29
    cmpwi r0, 0x4
    blt @L_80123B94
    li r3, -0x1
@L_80123C34:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
