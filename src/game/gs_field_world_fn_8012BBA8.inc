    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    li r31, 0x0
    stw r30, 0x18(r1)
    li r30, 0x0
    stw r29, 0x14(r1)
    li r29, 0x0
    stw r28, 0x10(r1)
@L_8012BBD0:
    cmpwi r29, 0x0
    blt @L_8012BBE0
    cmpwi r29, 0x2
    blt @L_8012BBE8
@L_8012BBE0:
    li r0, 0x0
    b @L_8012BC0C
@L_8012BBE8:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r30
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012BC08
    li r0, 0x0
    b @L_8012BC0C
@L_8012BC08:
    li r0, 0x1
@L_8012BC0C:
    clrlwi. r0, r0, 24
    beq @L_8012BC70
    lwz r3, -26224(r2)
    cmpwi r29, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012BC3C
    cmpwi r29, 0x2
    bge @L_8012BC3C
    addi r3, r1, 0x8
    lwzx r28, r3, r31
@L_8012BC3C:
    mr r4, r28
    li r3, 0x0
    li r5, 0x1
    bl fn_8018CA20
    mr r4, r28
    li r3, 0x0
    li r5, 0x700
    bl fn_8018C7C8
    lis r3, 0x8000
    mr r4, r28
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C69C
@L_8012BC70:
    addi r29, r29, 0x1
    addi r30, r30, 0x20
    cmpwi r29, 0x2
    addi r31, r31, 0x4
    blt @L_8012BBD0
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
