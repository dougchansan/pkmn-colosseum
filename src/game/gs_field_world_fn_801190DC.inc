    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr r28, r3
    mr r29, r4
    mr r30, r5
    lbz r0, 0x0(r3)
    cmplwi r0, 0x0
    bne @L_8011910C
    li r3, 0x0
    b @L_801193A8
@L_8011910C:
    lwz r0, lbl_8047ADAC(r13)
    lwz r31, lbl_8047ADA8(r13)
    mtctr r0
    cmplwi r0, 0x0
    ble @L_80119138
@L_80119120:
    lbz r0, 0x0(r31)
    cmplwi r0, 0x0
    bne @L_80119130
    b @L_8011913C
@L_80119130:
    addi r31, r31, 0x74
    bdnz @L_80119120
@L_80119138:
    li r31, 0x0
@L_8011913C:
    cmplwi r31, 0x0
    bne @L_8011914C
    li r3, 0x0
    b @L_801193A8
@L_8011914C:
    li r0, 0x8
    mr r4, r28
    li r3, 0x0
    mtctr r0
@L_8011915C:
    lwz r0, 0x8(r4)
    cmplwi r0, 0x0
    bne @L_8011916C
    b @L_80119208
@L_8011916C:
    lwz r0, 0xc(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_80119180
    b @L_80119208
@L_80119180:
    lwz r0, 0x10(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_80119194
    b @L_80119208
@L_80119194:
    lwz r0, 0x14(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801191A8
    b @L_80119208
@L_801191A8:
    lwz r0, 0x18(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801191BC
    b @L_80119208
@L_801191BC:
    lwz r0, 0x1c(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801191D0
    b @L_80119208
@L_801191D0:
    lwz r0, 0x20(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801191E4
    b @L_80119208
@L_801191E4:
    lwz r0, 0x24(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801191F8
    b @L_80119208
@L_801191F8:
    addi r4, r4, 0x20
    addi r3, r3, 0x1
    bdnz @L_8011915C
    li r3, -0x1
@L_80119208:
    cmpwi r3, -0x1
    mr r27, r3
    bne @L_801192EC
    mr r3, r28
    bl fn_80119930
    li r0, 0x8
    mr r4, r28
    li r3, 0x0
    mtctr r0
@L_8011922C:
    lwz r0, 0x8(r4)
    cmplwi r0, 0x0
    bne @L_8011923C
    b @L_801192D8
@L_8011923C:
    lwz r0, 0xc(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_80119250
    b @L_801192D8
@L_80119250:
    lwz r0, 0x10(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_80119264
    b @L_801192D8
@L_80119264:
    lwz r0, 0x14(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_80119278
    b @L_801192D8
@L_80119278:
    lwz r0, 0x18(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_8011928C
    b @L_801192D8
@L_8011928C:
    lwz r0, 0x1c(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801192A0
    b @L_801192D8
@L_801192A0:
    lwz r0, 0x20(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801192B4
    b @L_801192D8
@L_801192B4:
    lwz r0, 0x24(r4)
    addi r3, r3, 0x1
    cmplwi r0, 0x0
    bne @L_801192C8
    b @L_801192D8
@L_801192C8:
    addi r4, r4, 0x20
    addi r3, r3, 0x1
    bdnz @L_8011922C
    li r3, -0x1
@L_801192D8:
    cmpwi r3, -0x1
    mr r27, r3
    bne @L_801192EC
    li r3, 0x0
    b @L_801193A8
@L_801192EC:
    clrlwi r0, r30, 24
    li r3, 0x0
    cmplwi r0, 0x1
    bne @L_80119300
    li r3, 0x1
@L_80119300:
    lbz r4, 0x1(r28)
    mr r5, r29
    bl fn_80173718
    stw r3, 0x10(r31)
    lwz r3, 0x10(r31)
    cmplwi r3, 0x0
    bne @L_80119324
    li r3, 0x0
    b @L_801193A8
@L_80119324:
    li r4, 0x1
    li r0, 0x0
    stb r4, 0x17(r3)
    addi r3, r31, 0x38
    stb r4, 0x0(r31)
    stw r28, 0xc(r31)
    sth r29, 0x2(r31)
    stw r0, 0x44(r31)
    stw r0, 0x48(r31)
    stw r0, 0x4c(r31)
    stb r0, 0x5(r31)
    stb r0, 0x6(r31)
    lwz r4, 0x10(r31)
    lfs f1, 0x20(r4)
    lfs f2, 0x24(r4)
    lfs f3, 0x28(r4)
    bl fn_800E01F4
    stb r30, 0x4(r31)
    bl fn_800D3094
    slwi r0, r27, 2
    stw r3, 0x8(r31)
    add r3, r28, r0
    stw r31, 0x8(r3)
    lwz r3, 0x10(r31)
    lhz r0, 0x10(r3)
    cmplwi r0, 0x0
    bne @L_8011939C
    li r0, 0x1
    stb r0, 0x1(r31)
    b @L_801193A4
@L_8011939C:
    li r0, 0x0
    stb r0, 0x1(r31)
@L_801193A4:
    mr r3, r31
@L_801193A8:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
