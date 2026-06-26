    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    lis r5, lbl_802727D8@ha
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r3
    lis r3, 0x4750
    stw r30, 0x8(r1)
    addi r0, r3, 0x5431
    addi r30, r5, lbl_802727D8@l
    lwz r4, 0x0(r31)
    cmpw r4, r0
    beq @L_80119634
    bge @L_80119620
    addi r0, r3, 0x5430
    cmpw r4, r0
    bge @L_801195F4
    b @L_80119620
@L_801195F4:
    addi r3, r30, 0x50
    crxor 6, 6, 6
    bl fn_800DD970
    addi r3, r30, 0xac
    crxor 6, 6, 6
    bl fn_800DD970
    addi r3, r30, 0x50
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_8011980C
@L_80119620:
    addi r3, r30, 0x108
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_8011980C
@L_80119634:
    lwz r0, 0x4(r31)
    add r0, r0, r31
    stw r0, 0x4(r31)
    lwz r0, 0x8(r31)
    add r0, r0, r31
    stw r0, 0x8(r31)
    lwz r0, 0x10(r31)
    add r0, r0, r31
    stw r0, 0x10(r31)
    lwz r3, 0x4(r31)
    lhz r5, 0x0(r3)
    cmpwi r5, 0x43
    beq @L_80119680
    addi r3, r30, 0x128
    li r4, 0x43
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_8011980C
@L_80119680:
    lwz r4, 0xc(r31)
    lwz r3, 0x8(r31)
    addi r0, r4, 0x1f
    clrrwi r4, r0, 5
    bl DCFlushRange
    lwz r6, lbl_8047AD9C(r13)
    lwz r7, lbl_8047ADA0(r13)
    mr r30, r6
    mtctr r7
    cmplwi r7, 0x0
    ble @L_801196C4
@L_801196AC:
    lbz r0, 0x0(r30)
    cmplwi r0, 0x0
    bne @L_801196BC
    b @L_801196C8
@L_801196BC:
    addi r30, r30, 0x108
    bdnz @L_801196AC
@L_801196C4:
    li r30, 0x0
@L_801196C8:
    cmplwi r30, 0x0
    bne @L_801196D8
    li r3, 0x0
    b @L_8011980C
@L_801196D8:
    li r4, 0x0
    b @L_80119724
@L_801196E0:
    mr r5, r6
    clrlwi r0, r4, 24
    mtctr r7
    cmplwi r7, 0x0
    ble @L_80119710
@L_801196F4:
    lbz r3, 0x1(r5)
    cmplw r3, r0
    bne @L_80119708
    li r0, 0x1
    b @L_80119714
@L_80119708:
    addi r5, r5, 0x108
    bdnz @L_801196F4
@L_80119710:
    li r0, 0x0
@L_80119714:
    clrlwi. r0, r0, 24
    bne @L_80119720
    b @L_80119734
@L_80119720:
    addi r4, r4, 0x1
@L_80119724:
    clrlwi r0, r4, 24
    cmplwi r0, 0x40
    blt @L_801196E0
    li r4, 0xff
@L_80119734:
    clrlwi r0, r4, 24
    cmplwi r0, 0xff
    bne @L_80119748
    li r3, 0x0
    b @L_8011980C
@L_80119748:
    li r3, 0x1
    li r0, 0x2
    stb r3, 0x0(r30)
    mr r5, r30
    li r3, 0x0
    stb r4, 0x1(r30)
    stw r31, 0x4(r30)
    mtctr r0
@L_80119768:
    stw r3, 0x8(r5)
    stw r3, 0xc(r5)
    stw r3, 0x10(r5)
    stw r3, 0x14(r5)
    stw r3, 0x18(r5)
    stw r3, 0x1c(r5)
    stw r3, 0x20(r5)
    stw r3, 0x24(r5)
    stw r3, 0x28(r5)
    stw r3, 0x2c(r5)
    stw r3, 0x30(r5)
    stw r3, 0x34(r5)
    stw r3, 0x38(r5)
    stw r3, 0x3c(r5)
    stw r3, 0x40(r5)
    stw r3, 0x44(r5)
    stw r3, 0x48(r5)
    stw r3, 0x4c(r5)
    stw r3, 0x50(r5)
    stw r3, 0x54(r5)
    stw r3, 0x58(r5)
    stw r3, 0x5c(r5)
    stw r3, 0x60(r5)
    stw r3, 0x64(r5)
    stw r3, 0x68(r5)
    stw r3, 0x6c(r5)
    stw r3, 0x70(r5)
    stw r3, 0x74(r5)
    stw r3, 0x78(r5)
    stw r3, 0x7c(r5)
    stw r3, 0x80(r5)
    stw r3, 0x84(r5)
    addi r5, r5, 0x80
    bdnz @L_80119768
    lbz r3, 0x1(r30)
    li r7, 0x0
    lwz r4, 0x4(r31)
    lwz r5, 0x8(r31)
    lwz r6, 0x10(r31)
    bl psInitDataBank
    mr r3, r30
@L_8011980C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
