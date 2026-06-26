    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    li r31, 0x0
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    mr r28, r3
    mr r30, r28
@L_801193E4:
    lwz r29, 0x8(r30)
    cmplwi r29, 0x0
    beq @L_80119574
    lwz r4, 0x10(r29)
    lhz r3, 0x18(r4)
    lbz r4, 0x15(r4)
    bl psKillFamily
    lwz r0, 0x44(r29)
    cmpwi r0, 0x0
    beq @L_80119524
    beq @L_80119524
    lwz r3, 0x48(r29)
    li r4, 0x0
    bl fn_800EC160
    li r0, 0x0
    stw r0, 0x48(r29)
    stw r0, 0x4c(r29)
    stb r0, 0x6(r29)
    stb r0, 0x5(r29)
    lwz r3, 0x10(r29)
    bl fn_80169484
    li r0, 0x0
    stw r0, 0x44(r29)
    lwz r0, 0x44(r29)
    cmpwi r0, 0x0
    bne @L_80119480
    addi r3, r29, 0x14
    addi r4, r29, 0x50
    bl fn_800E01D0
    lfs f0, 0x50(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x20(r3)
    lfs f0, 0x54(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x24(r3)
    lfs f0, 0x58(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x28(r3)
    b @L_8011948C
@L_80119480:
    addi r3, r29, 0x50
    mr r4, r3
    bl fn_800E01D0
@L_8011948C:
    lwz r0, 0x44(r29)
    cmpwi r0, 0x0
    bne @L_801194CC
    addi r3, r29, 0x20
    addi r4, r29, 0x5c
    bl fn_800E01D0
    lfs f0, 0x5c(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x8c(r3)
    lfs f0, 0x60(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x90(r3)
    lfs f0, 0x64(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x94(r3)
    b @L_801194D8
@L_801194CC:
    addi r3, r29, 0x5c
    mr r4, r3
    bl fn_800E01D0
@L_801194D8:
    lwz r0, 0x44(r29)
    cmpwi r0, 0x0
    bne @L_80119518
    addi r3, r29, 0x2c
    addi r4, r29, 0x68
    bl fn_800E01D0
    lfs f0, 0x68(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x98(r3)
    lfs f0, 0x6c(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0x9c(r3)
    lfs f0, 0x70(r29)
    lwz r3, 0x10(r29)
    stfs f0, 0xa0(r3)
    b @L_80119524
@L_80119518:
    addi r3, r29, 0x68
    mr r4, r3
    bl fn_800E01D0
@L_80119524:
    lwz r3, 0x10(r29)
    bl psKillGenerator
    lwz r5, 0xc(r29)
    li r0, 0x40
    li r3, 0x0
    mr r4, r5
    mtctr r0
@L_80119540:
    lwz r0, 0x8(r4)
    cmplw r0, r29
    bne @L_80119560
    slwi r3, r3, 2
    li r4, 0x0
    addi r0, r3, 0x8
    stwx r4, r5, r0
    b @L_8011956C
@L_80119560:
    addi r4, r4, 0x4
    addi r3, r3, 0x1
    bdnz @L_80119540
@L_8011956C:
    li r0, 0x0
    stb r0, 0x0(r29)
@L_80119574:
    addi r31, r31, 0x1
    addi r30, r30, 0x4
    cmplwi r31, 0x40
    blt @L_801193E4
    li r0, 0x0
    stb r0, 0x0(r28)
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
