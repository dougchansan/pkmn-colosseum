    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    lis r6, lbl_802729A4@ha
    lis r7, lbl_80272998@ha
    stw r0, 0x44(r1)
    addi r10, r6, lbl_802729A4@l
    stw r31, 0x3c(r1)
    mr r31, r5
    stw r30, 0x38(r1)
    mr r30, r4
    mr r4, r31
    stw r29, 0x34(r1)
    mr r29, r3
    addi r3, r7, lbl_80272998@l
    stw r28, 0x30(r1)
    lwz r9, 0x0(r10)
    lwz r28, 0x0(r3)
    lwz r12, 0x4(r3)
    lhz r11, 0x8(r3)
    li r3, 0x0
    lwz r8, 0x4(r10)
    lwz r7, 0x8(r10)
    lwz r6, 0xc(r10)
    lwz r5, 0x10(r10)
    lwz r0, 0x14(r10)
    stw r28, 0x8(r1)
    stw r12, 0xc(r1)
    sth r11, 0x10(r1)
    stw r9, 0x14(r1)
    stw r8, 0x18(r1)
    stw r7, 0x1c(r1)
    stw r6, 0x20(r1)
    stw r5, 0x24(r1)
    stw r0, 0x28(r1)
    bl fn_80120C6C
    clrlwi. r0, r3, 24
    bne @L_8012839C
    li r3, 0x0
    b @L_80128504
@L_8012839C:
    mr r3, r30
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_801283B4
    li r3, 0x0
    b @L_801283B8
@L_801283B4:
    bl fn_8011E778
@L_801283B8:
    cmplwi r3, 0x0
    bne @L_801283C8
    li r3, 0x0
    b @L_80128504
@L_801283C8:
    mr r3, r29
    mr r4, r30
    bl fn_8011F5FC
    mr r3, r29
    mr r4, r31
    bl fn_8011DFF0
    mr r3, r29
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_801283F8
    li r3, 0x0
    b @L_801283FC
@L_801283F8:
    bl fn_8011E778
@L_801283FC:
    cmplwi r3, 0x0
    bne @L_8012840C
    li r3, 0x0
    b @L_80128504
@L_8012840C:
    bl fn_8011E760
    bl fn_800FA280
    mr r0, r3
    mr r3, r29
    mr r4, r0
    bl fn_8011DEE4
    mr r3, r29
    li r4, 0x0
    bl fn_8011DCB4
    mr r3, r29
    li r4, 0x0
    li r5, 0xbb
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r29
    li r4, 0x0
    li r5, 0xbe
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    li r30, 0x0
    addi r28, r1, 0x8
@L_80128468:
    lhz r5, 0x0(r28)
    mr r3, r29
    li r4, 0x0
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    addi r30, r30, 0x1
    addi r28, r28, 0x2
    cmpwi r30, 0x5
    blt @L_80128468
    li r30, 0x0
    addi r28, r1, 0x14
@L_80128498:
    lhz r5, 0x0(r28)
    mr r3, r29
    li r4, 0x0
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    addi r30, r30, 0x1
    addi r28, r28, 0x2
    cmpwi r30, 0xc
    blt @L_80128498
    mr r3, r29
    li r4, 0x0
    li r5, 0xaf
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r29
    bl fn_801252E0
    mr r3, r29
    li r4, 0x0
    li r5, 0xbc
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r29
    bl fn_8012546C
    li r3, 0x1
@L_80128504:
    lwz r0, 0x44(r1)
    lwz r31, 0x3c(r1)
    lwz r30, 0x38(r1)
    lwz r29, 0x34(r1)
    lwz r28, 0x30(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr
