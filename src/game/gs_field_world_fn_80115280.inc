    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    cmplwi r3, 0x0
    li r4, 0x0
    bne @L_801152B8
    lis r3, lbl_80272608@ha
    lis r4, lbl_8035BB50@ha
    addi r3, r3, lbl_80272608@l
    addi r4, r4, lbl_8035BB50@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_8011537C
@L_801152B8:
    lwz r3, 0x10(r3)
    cmplwi r3, 0x0
    bne @L_801152CC
    li r3, 0x0
    b @L_8011537C
@L_801152CC:
    lwz r3, 0x0(r3)
    cmplwi r3, 0x0
    bne @L_801152F8
    lis r3, lbl_8027262C@ha
    lis r4, lbl_8035BB50@ha
    addi r3, r3, lbl_8027262C@l
    addi r4, r4, lbl_8035BB50@l
    crxor 6, 6, 6
    bl fn_800DD970
    li r3, 0x0
    b @L_8011537C
@L_801152F8:
    lwz r0, 0x8(r3)
    cmplwi r0, 0x0
    beq @L_80115308
    li r4, 0x1
@L_80115308:
    lwz r0, 0xc(r3)
    cmplwi r0, 0x0
    beq @L_80115318
    addi r4, r4, 0x1
@L_80115318:
    lwz r0, 0x10(r3)
    cmplwi r0, 0x0
    beq @L_80115328
    addi r4, r4, 0x1
@L_80115328:
    lwz r0, 0x14(r3)
    cmplwi r0, 0x0
    beq @L_80115338
    addi r4, r4, 0x1
@L_80115338:
    lwz r0, 0x18(r3)
    cmplwi r0, 0x0
    beq @L_80115348
    addi r4, r4, 0x1
@L_80115348:
    lwz r0, 0x1c(r3)
    cmplwi r0, 0x0
    beq @L_80115358
    addi r4, r4, 0x1
@L_80115358:
    lwz r0, 0x20(r3)
    cmplwi r0, 0x0
    beq @L_80115368
    addi r4, r4, 0x1
@L_80115368:
    lwz r0, 0x24(r3)
    cmplwi r0, 0x0
    beq @L_80115378
    addi r4, r4, 0x1
@L_80115378:
    mr r3, r4
@L_8011537C:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
