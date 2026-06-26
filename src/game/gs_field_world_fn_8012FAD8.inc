    nofralloc
    stwu r1, -0x90(r1)
    mflr r0
    lis r3, lbl_802729C0@ha
    stw r0, 0x94(r1)
    addi r3, r3, lbl_802729C0@l
    li r0, 0xa
    addi r5, r1, 0x2c
    stw r31, 0x8c(r1)
    subi r4, r3, 0x4
    mtctr r0
@L_8012FB00:
    lwz r3, 0x4(r4)
    lwzu r0, 0x8(r4)
    stw r3, 0x4(r5)
    stwu r0, 0x8(r5)
    bdnz @L_8012FB00
    lis r4, lbl_80272A10@ha
    li r3, 0x8ae
    addi r31, r4, lbl_80272A10@l
    lwz r12, 0x0(r31)
    lwz r11, 0x4(r31)
    lwz r10, 0x8(r31)
    lwz r9, 0xc(r31)
    lwz r8, 0x10(r31)
    lwz r7, 0x14(r31)
    lwz r6, 0x18(r31)
    lwz r5, 0x1c(r31)
    lwz r4, 0x20(r31)
    lwz r0, 0x24(r31)
    stw r12, 0x8(r1)
    stw r11, 0xc(r1)
    stw r10, 0x10(r1)
    stw r9, 0x14(r1)
    stw r8, 0x18(r1)
    stw r7, 0x1c(r1)
    stw r6, 0x20(r1)
    stw r5, 0x24(r1)
    stw r4, 0x28(r1)
    stw r0, 0x2c(r1)
    bl fn_801906A0
    cntlzw r0, r3
    extrwi. r0, r0, 8, 19
    beq @L_8012FB8C
    lis r3, 0xf7
    addi r3, r3, 0x400
    b @L_8012FCC0
@L_8012FB8C:
    bl fn_8011393C
    li r0, 0x2
    addi r4, r1, 0x30
    li r5, 0x0
    mtctr r0
@L_8012FBA0:
    lwz r0, 0x0(r4)
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x4(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x8(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0xc(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x10(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x14(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x18(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x1c(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x20(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    lwz r0, 0x24(r4)
    addi r5, r5, 0x1
    cmplw r3, r0
    beq @L_8012FC48
    addi r4, r4, 0x28
    addi r5, r5, 0x1
    bdnz @L_8012FBA0
@L_8012FC48:
    cmpwi r5, 0x14
    blt @L_8012FC5C
    lis r3, 0xf7
    addi r3, r3, 0x400
    b @L_8012FCC0
@L_8012FC5C:
    bl fn_8006AE18
    lwz r0, 0x8(r1)
    li r4, 0x0
    cmpw r3, r0
    beq @L_8012FCB4
    lwz r0, 0x10(r1)
    li r4, 0x1
    cmpw r3, r0
    beq @L_8012FCB4
    lwz r0, 0x18(r1)
    li r4, 0x2
    cmpw r3, r0
    beq @L_8012FCB4
    lwz r0, 0x20(r1)
    li r4, 0x3
    cmpw r3, r0
    beq @L_8012FCB4
    lwz r0, 0x28(r1)
    li r4, 0x4
    cmpw r3, r0
    beq @L_8012FCB4
    li r4, 0x5
@L_8012FCB4:
    slwi r0, r4, 3
    addi r3, r1, 0xc
    lwzx r3, r3, r0
@L_8012FCC0:
    lwz r0, 0x94(r1)
    lwz r31, 0x8c(r1)
    mtlr r0
    addi r1, r1, 0x90
    blr
