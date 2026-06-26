    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stmw r27, 0x2c(r1)
    li r30, 0x0
    li r28, 0x0
    li r27, 0x0
@L_8012BCC0:
    cmpwi r30, 0x0
    blt @L_8012BCD0
    cmpwi r30, 0x2
    blt @L_8012BCD8
@L_8012BCD0:
    li r0, 0x0
    b @L_8012BCFC
@L_8012BCD8:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r27
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012BCF8
    li r0, 0x0
    b @L_8012BCFC
@L_8012BCF8:
    li r0, 0x1
@L_8012BCFC:
    clrlwi. r0, r0, 24
    beq @L_8012BDB8
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    stw r3, 0x18(r1)
    stw r0, 0x1c(r1)
    blt @L_8012BD34
    cmpwi r30, 0x2
    bge @L_8012BD34
    addi r3, r1, 0x18
    lwzx r29, r3, r28
@L_8012BD34:
    lwz r3, 0x8(r1)
    cmpwi r30, 0x0
    lwz r0, 0xc(r1)
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012BD5C
    cmpwi r30, 0x2
    bge @L_8012BD5C
    addi r3, r1, 0x10
    lwzx r31, r3, r28
@L_8012BD5C:
    mr r4, r31
    li r3, 0x0
    bl fn_800F9318
    lfs f1, -26216(r2)
    mr r4, r30
    bl fn_8012C660
    lis r3, 0x8000
    mr r4, r29
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C7C8
    mr r4, r29
    li r3, 0x0
    li r5, 0x100
    bl fn_8018C69C
    mr r4, r29
    li r3, 0x0
    li r5, 0x400
    bl fn_8018C69C
    mr r4, r29
    li r3, 0x0
    li r5, 0x0
    bl fn_8018CA20
@L_8012BDB8:
    addi r30, r30, 0x1
    addi r27, r27, 0x20
    cmpwi r30, 0x2
    addi r28, r28, 0x4
    blt @L_8012BCC0
    lmw r27, 0x2c(r1)
    lwz r0, 0x44(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr
