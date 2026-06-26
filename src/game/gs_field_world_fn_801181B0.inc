    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    li r29, 0x0
    li r30, 0x0
    b @L_801183CC
@L_801181CC:
    lwz r0, lbl_8047AD9C(r13)
    add r3, r0, r30
    lbz r0, 0x0(r3)
    cmplwi r0, 0x1
    bne @L_801183C4
    li r28, 0x0
    mr r31, r3
@L_801181E8:
    lwz r27, 0x8(r31)
    cmplwi r27, 0x0
    beq @L_801183B4
    lbz r0, 0x1(r27)
    cmplwi r0, 0x0
    bne @L_801183B4
    cmplwi r0, 0x1
    bne @L_80118210
    li r3, -0x1
    b @L_80118218
@L_80118210:
    lwz r3, 0x10(r27)
    bl fn_801694E0
@L_80118218:
    cmplwi r3, 0x0
    bne @L_801183B4
    lwz r3, 0x10(r27)
    bl fn_801694A8
    cmplwi r3, 0x0
    bne @L_801183B4
    lwz r4, 0x10(r27)
    lhz r3, 0x18(r4)
    lbz r4, 0x15(r4)
    bl psKillFamily
    lwz r0, 0x44(r27)
    cmpwi r0, 0x0
    beq @L_80118364
    beq @L_80118364
    lwz r3, 0x48(r27)
    li r4, 0x0
    bl fn_800EC160
    li r0, 0x0
    stw r0, 0x48(r27)
    stw r0, 0x4c(r27)
    stb r0, 0x6(r27)
    stb r0, 0x5(r27)
    lwz r3, 0x10(r27)
    bl fn_80169484
    li r0, 0x0
    stw r0, 0x44(r27)
    lwz r0, 0x44(r27)
    cmpwi r0, 0x0
    bne @L_801182C0
    addi r3, r27, 0x14
    addi r4, r27, 0x50
    bl fn_800E01D0
    lfs f0, 0x50(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x20(r3)
    lfs f0, 0x54(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x24(r3)
    lfs f0, 0x58(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x28(r3)
    b @L_801182CC
@L_801182C0:
    addi r3, r27, 0x50
    mr r4, r3
    bl fn_800E01D0
@L_801182CC:
    lwz r0, 0x44(r27)
    cmpwi r0, 0x0
    bne @L_8011830C
    addi r3, r27, 0x20
    addi r4, r27, 0x5c
    bl fn_800E01D0
    lfs f0, 0x5c(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x8c(r3)
    lfs f0, 0x60(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x90(r3)
    lfs f0, 0x64(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x94(r3)
    b @L_80118318
@L_8011830C:
    addi r3, r27, 0x5c
    mr r4, r3
    bl fn_800E01D0
@L_80118318:
    lwz r0, 0x44(r27)
    cmpwi r0, 0x0
    bne @L_80118358
    addi r3, r27, 0x2c
    addi r4, r27, 0x68
    bl fn_800E01D0
    lfs f0, 0x68(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x98(r3)
    lfs f0, 0x6c(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0x9c(r3)
    lfs f0, 0x70(r27)
    lwz r3, 0x10(r27)
    stfs f0, 0xa0(r3)
    b @L_80118364
@L_80118358:
    addi r3, r27, 0x68
    mr r4, r3
    bl fn_800E01D0
@L_80118364:
    lwz r3, 0x10(r27)
    bl psKillGenerator
    lwz r5, 0xc(r27)
    li r0, 0x40
    li r3, 0x0
    mr r4, r5
    mtctr r0
@L_80118380:
    lwz r0, 0x8(r4)
    cmplw r0, r27
    bne @L_801183A0
    slwi r3, r3, 2
    li r4, 0x0
    addi r0, r3, 0x8
    stwx r4, r5, r0
    b @L_801183AC
@L_801183A0:
    addi r4, r4, 0x4
    addi r3, r3, 0x1
    bdnz @L_80118380
@L_801183AC:
    li r0, 0x0
    stb r0, 0x0(r27)
@L_801183B4:
    addi r28, r28, 0x1
    addi r31, r31, 0x4
    cmplwi r28, 0x40
    blt @L_801181E8
@L_801183C4:
    addi r30, r30, 0x108
    addi r29, r29, 0x1
@L_801183CC:
    lwz r0, lbl_8047ADA0(r13)
    cmplw r29, r0
    blt @L_801181CC
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
