    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    mr. r30, r3
    stw r29, 0x14(r1)
    li r29, 0x0
    bne @L_8011F290
    li r3, 0x0
    b @L_8011F440
@L_8011F290:
    bne @L_8011F29C
    li r0, 0x0
    b @L_8011F2E4
@L_8011F29C:
    bne @L_8011F2A8
    li r0, 0x0
    b @L_8011F2AC
@L_8011F2A8:
    lhz r0, 0xd8(r30)
@L_8011F2AC:
    clrlwi. r0, r0, 16
    bne @L_8011F2BC
    li r0, 0x0
    b @L_8011F2E4
@L_8011F2BC:
    cmplwi r30, 0x0
    bne @L_8011F2CC
    li r0, 0x0
    b @L_8011F2D0
@L_8011F2CC:
    lwz r0, 0xdc(r30)
@L_8011F2D0:
    cmpwi r0, 0x0
    bge @L_8011F2E0
    li r0, 0x0
    b @L_8011F2E4
@L_8011F2E0:
    li r0, 0x1
@L_8011F2E4:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011F31C
    cmplwi r30, 0x0
    bne @L_8011F300
    li r0, 0x0
    b @L_8011F304
@L_8011F300:
    lhz r0, 0xd6(r30)
@L_8011F304:
    clrlwi. r0, r0, 16
    bne @L_8011F31C
    clrlwi r0, r5, 24
    cmplwi r0, 0x1
    bne @L_8011F31C
    li r29, 0x1
@L_8011F31C:
    cmplwi r30, 0x0
    bne @L_8011F32C
    li r0, 0x0
    b @L_8011F374
@L_8011F32C:
    bne @L_8011F338
    li r0, 0x0
    b @L_8011F33C
@L_8011F338:
    lhz r0, 0xd8(r30)
@L_8011F33C:
    clrlwi. r0, r0, 16
    bne @L_8011F34C
    li r0, 0x0
    b @L_8011F374
@L_8011F34C:
    cmplwi r30, 0x0
    bne @L_8011F35C
    li r0, 0x0
    b @L_8011F360
@L_8011F35C:
    lwz r0, 0xdc(r30)
@L_8011F360:
    cmpwi r0, 0x0
    bge @L_8011F370
    li r0, 0x0
    b @L_8011F374
@L_8011F370:
    li r0, 0x1
@L_8011F374:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011F398
    clrlwi. r0, r31, 16
    bne @L_8011F398
    clrlwi r0, r5, 24
    cmplwi r0, 0x1
    bne @L_8011F398
    li r29, 0x1
@L_8011F398:
    mr r3, r30
    bl fn_8011F77C
    clrlwi r0, r29, 24
    cmplwi r0, 0x1
    bne @L_8011F410
    clrlwi. r0, r31, 16
    bne @L_8011F3BC
    la r3, lbl_80478B58(r13)
    b @L_8011F440
@L_8011F3BC:
    cmplwi r0, 0x1
    bne @L_8011F3D8
    clrlwi r0, r3, 24
    cmplwi r0, 0x2
    bge @L_8011F410
    la r3, lbl_80478B5C(r13)
    b @L_8011F440
@L_8011F3D8:
    cmplwi r0, 0x2
    bne @L_8011F3F4
    clrlwi r0, r3, 24
    cmplwi r0, 0x4
    bge @L_8011F410
    la r3, lbl_80478B5C(r13)
    b @L_8011F440
@L_8011F3F4:
    cmplwi r0, 0x3
    bne @L_8011F410
    clrlwi r0, r3, 24
    cmplwi r0, 0x5
    bge @L_8011F410
    la r3, lbl_80478B5C(r13)
    b @L_8011F440
@L_8011F410:
    clrlwi r0, r31, 16
    cmplwi r0, 0x4
    blt @L_8011F434
    subi r31, r31, 0x4
    clrlwi r0, r31, 16
    cmplwi r0, 0x4
    blt @L_8011F434
    li r3, 0x0
    b @L_8011F440
@L_8011F434:
    clrlslwi r3, r31, 16, 2
    addi r3, r3, 0x78
    add r3, r30, r3
@L_8011F440:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
