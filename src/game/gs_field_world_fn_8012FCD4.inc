    nofralloc
    stwu r1, -0xb0(r1)
    mflr r0
    stw r0, 0xb4(r1)
    stmw r25, 0x94(r1)
    mr r29, r3
    mr r30, r4
    bl fn_800FF548
    clrlwi. r0, r3, 24
    bne @L_8012FEF4
    lis r3, lbl_802729C0@ha
    li r0, 0xa
    addi r3, r3, lbl_802729C0@l
    addi r5, r1, 0x3c
    subi r4, r3, 0x4
    mtctr r0
@L_8012FD10:
    lwz r3, 0x4(r4)
    lwzu r0, 0x8(r4)
    stw r3, 0x4(r5)
    stwu r0, 0x8(r5)
    bdnz @L_8012FD10
    lis r4, lbl_80272A10@ha
    li r3, 0x8ae
    addi r28, r4, lbl_80272A10@l
    lwz r12, 0x0(r28)
    lwz r11, 0x4(r28)
    lwz r10, 0x8(r28)
    lwz r9, 0xc(r28)
    lwz r8, 0x10(r28)
    lwz r7, 0x14(r28)
    lwz r6, 0x18(r28)
    lwz r5, 0x1c(r28)
    lwz r4, 0x20(r28)
    lwz r0, 0x24(r28)
    stw r12, 0x18(r1)
    stw r11, 0x1c(r1)
    stw r10, 0x20(r1)
    stw r9, 0x24(r1)
    stw r8, 0x28(r1)
    stw r7, 0x2c(r1)
    stw r6, 0x30(r1)
    stw r5, 0x34(r1)
    stw r4, 0x38(r1)
    stw r0, 0x3c(r1)
    bl fn_801906A0
    cntlzw r0, r3
    extrwi. r0, r0, 8, 19
    beq @L_8012FD9C
    lis r3, 0xf7
    addi r5, r3, 0x400
    b @L_8012FED0
@L_8012FD9C:
    bl fn_8011393C
    li r0, 0x2
    addi r5, r1, 0x40
    li r4, 0x0
    mtctr r0
@L_8012FDB0:
    lwz r0, 0x0(r5)
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x4(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x8(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0xc(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x10(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x14(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x18(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x1c(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x20(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    lwz r0, 0x24(r5)
    addi r4, r4, 0x1
    cmplw r3, r0
    beq @L_8012FE58
    addi r5, r5, 0x28
    addi r4, r4, 0x1
    bdnz @L_8012FDB0
@L_8012FE58:
    cmpwi r4, 0x14
    blt @L_8012FE6C
    lis r3, 0xf7
    addi r5, r3, 0x400
    b @L_8012FED0
@L_8012FE6C:
    bl fn_8006AE18
    lwz r0, 0x18(r1)
    li r4, 0x0
    cmpw r3, r0
    beq @L_8012FEC4
    lwz r0, 0x20(r1)
    li r4, 0x1
    cmpw r3, r0
    beq @L_8012FEC4
    lwz r0, 0x28(r1)
    li r4, 0x2
    cmpw r3, r0
    beq @L_8012FEC4
    lwz r0, 0x30(r1)
    li r4, 0x3
    cmpw r3, r0
    beq @L_8012FEC4
    lwz r0, 0x38(r1)
    li r4, 0x4
    cmpw r3, r0
    beq @L_8012FEC4
    li r4, 0x5
@L_8012FEC4:
    slwi r0, r4, 3
    addi r3, r1, 0x1c
    lwzx r5, r3, r0
@L_8012FED0:
    li r3, 0x0
    li r4, 0x64
    bl fn_8018E050
    lis r4, 0xf3
    li r3, 0x0
    addi r5, r4, 0x400
    li r4, 0x65
    bl fn_8018E050
    b @L_8012FF0C
@L_8012FEF4:
    li r3, 0x0
    li r4, 0x64
    bl fn_8018D998
    li r3, 0x0
    li r4, 0x65
    bl fn_8018D998
@L_8012FF0C:
    addi r26, r1, 0x10
    addi r28, r1, 0x8
    mr r27, r26
    li r25, 0x0
@L_8012FF1C:
    lwz r3, -26224(r2)
    cmpwi r25, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012FF40
    cmpwi r25, 0x2
    bge @L_8012FF40
    lwz r31, 0x0(r28)
@L_8012FF40:
    mr r4, r31
    li r3, 0x0
    bl fn_800F9318
    stw r3, 0x0(r27)
    lwz r3, 0x0(r27)
    bl fn_800EB528
    addi r25, r25, 0x1
    addi r27, r27, 0x4
    cmpwi r25, 0x2
    addi r28, r28, 0x4
    blt @L_8012FF1C
    li r3, 0x0
    li r4, 0x64
    bl fn_8018CB5C
    li r3, 0x0
    li r4, 0x65
    bl fn_8018CB5C
    bl fn_800FF548
    clrlwi. r0, r3, 24
    bne @L_8012FFB4
    lis r4, 0x4000
    li r3, 0x0
    addi r5, r4, 0xf00
    li r4, 0x64
    bl fn_8018C8F4
    li r3, 0x0
    li r4, 0x65
    li r5, 0x701
    bl fn_8018C8F4
@L_8012FFB4:
    li r3, 0x0
    li r4, 0x65
    li r5, 0x1
    bl fn_80189328
    bl fn_800FF548
    clrlwi. r0, r3, 24
    bne @L_8012FFF0
    mr r5, r29
    li r3, 0x0
    li r4, 0x64
    bl fn_8018C0A8
    mr r5, r30
    li r3, 0x0
    li r4, 0x64
    bl fn_8018BF24
@L_8012FFF0:
    li r27, 0x0
@L_8012FFF4:
    lwz r3, 0x0(r26)
    mr r4, r27
    lfs f1, -26216(r2)
    bl fn_8012C660
    addi r27, r27, 0x1
    addi r26, r26, 0x4
    cmpwi r27, 0x2
    blt @L_8012FFF4
    bl fn_8012F610
    lis r3, lbl_80426BD0@ha
    li r0, 0x0
    addi r4, r3, lbl_80426BD0@l
    li r3, 0x0
    stw r0, 0x44(r4)
    stw r0, 0x48(r4)
    stw r0, 0x414(r4)
    stw r0, 0x418(r4)
    stw r0, 0x410(r4)
    stw r0, 0x188(r4)
    lmw r25, 0x94(r1)
    lwz r0, 0xb4(r1)
    mtlr r0
    addi r1, r1, 0xb0
    blr
