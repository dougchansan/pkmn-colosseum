    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    lis r4, lbl_8027296C@ha
    stw r0, 0x44(r1)
    stw r31, 0x3c(r1)
    mr. r31, r3
    stw r30, 0x38(r1)
    addi r30, r4, lbl_8027296C@l
    lwz r12, 0x0(r30)
    lwz r11, 0x4(r30)
    lwz r10, 0x8(r30)
    lwz r9, 0xc(r30)
    lwz r8, 0x10(r30)
    lwz r7, 0x14(r30)
    lwz r6, 0x18(r30)
    lwz r5, 0x1c(r30)
    lwz r4, 0x20(r30)
    lwz r0, 0x24(r30)
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
    bne @L_801206F0
    li r0, 0x0
    b @L_80120768
@L_801206F0:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 16
    li r3, 0x0
    mr r4, r30
    li r5, 0x17
    li r6, 0x1
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_8012073C
    mr r4, r30
    li r3, 0x0
    li r5, 0x17
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 16
    b @L_80120768
@L_8012073C:
    mr r3, r31
    li r4, 0x0
    li r5, 0xb7
    li r6, 0x0
    bl fn_8012640C
    clrlwi r6, r3, 24
    mr r4, r30
    li r3, 0x0
    li r5, 0x17
    bl fn_8012640C
    clrlwi r0, r3, 16
@L_80120768:
    clrlwi r0, r0, 16
    cmplwi r0, 0x35
    bne @L_80120850
    mr r3, r31
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r0, r3, 16
    bne @L_80120850
    bl fn_800E0C54
    lis r4, 0x6666
    clrlwi r5, r3, 16
    addi r0, r4, 0x6667
    mulhw r0, r0, r5
    srawi r0, r0, 2
    srwi r3, r0, 31
    add r0, r0, r3
    mulli r0, r0, 0xa
    subf. r0, r0, r5
    bne @L_80120850
    bl fn_800E0C54
    lis r4, 0x51ec
    clrlwi r5, r3, 16
    subi r0, r4, 0x7ae1
    addi r3, r1, 0x8
    mulhw r0, r0, r5
    li r6, 0x0
    li r7, 0x0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r0, r0, r4
    mulli r0, r0, 0x64
    subf r5, r0, r5
    b @L_80120818
@L_801207F4:
    slwi r4, r0, 1
    addi r0, r4, 0x2
    lhzx r0, r3, r0
    cmplw r0, r5
    ble @L_80120814
    clrlslwi r0, r7, 16, 1
    lhzx r6, r3, r0
    b @L_80120824
@L_80120814:
    addi r7, r7, 0x2
@L_80120818:
    clrlwi r0, r7, 16
    cmpwi r0, 0x14
    blt @L_801207F4
@L_80120824:
    clrlwi. r7, r6, 16
    beq @L_80120850
    cmplwi r31, 0x0
    beq @L_80120848
    mr r3, r31
    li r4, 0x0
    li r5, 0x82
    li r6, 0x0
    bl fn_801254B4
@L_80120848:
    li r3, 0x1
    b @L_80120854
@L_80120850:
    li r3, 0x0
@L_80120854:
    lwz r0, 0x44(r1)
    lwz r31, 0x3c(r1)
    lwz r30, 0x38(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr
