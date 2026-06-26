    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r24, 0x10(r1)
    mr r24, r4
    mr r27, r3
    mr r28, r5
    mr r29, r6
    mr r30, r7
    mr r3, r24
    li r31, 0x0
    li r26, 0x0
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    bne @L_80128568
    li r3, -0x1
    b @L_801286B4
@L_80128568:
    mr r4, r28
    li r3, 0x0
    bl fn_80120C6C
    clrlwi. r0, r3, 24
    bne @L_80128584
    li r3, -0x1
    b @L_801286B4
@L_80128584:
    mr r3, r24
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_8012859C
    li r25, 0x0
    b @L_801285A4
@L_8012859C:
    bl fn_8011E778
    mr r25, r3
@L_801285A4:
    cmplwi r25, 0x0
    bne @L_801285B4
    li r3, -0x1
    b @L_801286B4
@L_801285B4:
    mr r3, r27
    mr r4, r24
    bl fn_8011F5FC
    mr r3, r27
    bl fn_8011F4F0
    mr r0, r3
    mr r3, r25
    mr r25, r0
    bl fn_8011E760
    bl fn_800FA280
    mr r0, r3
    mr r3, r25
    mr r4, r0
    bl GScharCmp
    cmpwi r3, 0x0
    bne @L_80128614
    mr r3, r28
    bl fn_8011E778
    bl fn_8011E760
    bl fn_800FA280
    mr r0, r3
    mr r3, r27
    mr r4, r0
    bl fn_8011DEE4
@L_80128614:
    mr r3, r27
    mr r4, r28
    bl fn_8011DFF0
    mr r3, r27
    bl fn_8012546C
    lbz r0, 0x0(r29)
    cmplwi r0, 0x6
    bne @L_80128644
    mr r3, r27
    li r4, 0x0
    li r5, 0x0
    bl fn_80123110
@L_80128644:
    mr r3, r27
    bl fn_8011F4A8
    li r0, 0x0
    mr r28, r3
    stb r0, 0x8(r1)
    b @L_80128694
@L_8012865C:
    mr r3, r27
    mr r4, r25
    bl fn_80123B5C
    extsb r0, r3
    cmpwi r0, -0x1
    bne @L_80128688
    addi r31, r31, 0x1
    sthx r25, r30, r26
    cmpwi r31, 0x14
    addi r26, r26, 0x2
    bge @L_801286B0
@L_80128688:
    lbz r3, 0x8(r1)
    addi r0, r3, 0x1
    stb r0, 0x8(r1)
@L_80128694:
    mr r3, r27
    mr r4, r28
    addi r5, r1, 0x8
    bl fn_801236F8
    clrlwi. r0, r3, 16
    mr r25, r3
    bne @L_8012865C
@L_801286B0:
    mr r3, r31
@L_801286B4:
    lmw r24, 0x10(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
