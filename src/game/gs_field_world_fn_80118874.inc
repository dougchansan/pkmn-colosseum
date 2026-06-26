    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r3
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    li r29, 0x0
    stw r28, 0x10(r1)
    mr r28, r4
@L_8011889C:
    lwz r30, 0x8(r31)
    cmplwi r30, 0x0
    beq @L_80118A38
    clrlwi r0, r28, 24
    cmplwi r0, 0x1
    bne @L_801188C4
    lwz r4, 0x10(r30)
    lhz r3, 0x18(r4)
    lbz r4, 0x15(r4)
    bl psKillFamily
@L_801188C4:
    lwz r0, 0x44(r30)
    cmpwi r0, 0x0
    beq @L_801189E8
    beq @L_801189E8
    lwz r3, 0x48(r30)
    li r4, 0x0
    bl fn_800EC160
    li r0, 0x0
    stw r0, 0x48(r30)
    stw r0, 0x4c(r30)
    stb r0, 0x6(r30)
    stb r0, 0x5(r30)
    lwz r3, 0x10(r30)
    bl fn_80169484
    li r0, 0x0
    stw r0, 0x44(r30)
    lwz r0, 0x44(r30)
    cmpwi r0, 0x0
    bne @L_80118944
    addi r3, r30, 0x14
    addi r4, r30, 0x50
    bl fn_800E01D0
    lfs f0, 0x50(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x20(r3)
    lfs f0, 0x54(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x24(r3)
    lfs f0, 0x58(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x28(r3)
    b @L_80118950
@L_80118944:
    addi r3, r30, 0x50
    mr r4, r3
    bl fn_800E01D0
@L_80118950:
    lwz r0, 0x44(r30)
    cmpwi r0, 0x0
    bne @L_80118990
    addi r3, r30, 0x20
    addi r4, r30, 0x5c
    bl fn_800E01D0
    lfs f0, 0x5c(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x8c(r3)
    lfs f0, 0x60(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x90(r3)
    lfs f0, 0x64(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x94(r3)
    b @L_8011899C
@L_80118990:
    addi r3, r30, 0x5c
    mr r4, r3
    bl fn_800E01D0
@L_8011899C:
    lwz r0, 0x44(r30)
    cmpwi r0, 0x0
    bne @L_801189DC
    addi r3, r30, 0x2c
    addi r4, r30, 0x68
    bl fn_800E01D0
    lfs f0, 0x68(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x98(r3)
    lfs f0, 0x6c(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0x9c(r3)
    lfs f0, 0x70(r30)
    lwz r3, 0x10(r30)
    stfs f0, 0xa0(r3)
    b @L_801189E8
@L_801189DC:
    addi r3, r30, 0x68
    mr r4, r3
    bl fn_800E01D0
@L_801189E8:
    lwz r3, 0x10(r30)
    bl psKillGenerator
    lwz r5, 0xc(r30)
    li r0, 0x40
    li r3, 0x0
    mr r4, r5
    mtctr r0
@L_80118A04:
    lwz r0, 0x8(r4)
    cmplw r0, r30
    bne @L_80118A24
    slwi r3, r3, 2
    li r4, 0x0
    addi r0, r3, 0x8
    stwx r4, r5, r0
    b @L_80118A30
@L_80118A24:
    addi r4, r4, 0x4
    addi r3, r3, 0x1
    bdnz @L_80118A04
@L_80118A30:
    li r0, 0x0
    stb r0, 0x0(r30)
@L_80118A38:
    addi r29, r29, 0x1
    addi r31, r31, 0x4
    cmplwi r29, 0x40
    blt @L_8011889C
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
