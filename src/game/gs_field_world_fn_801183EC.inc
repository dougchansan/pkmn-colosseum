    nofralloc
    stwu r1, -0x70(r1)
    mflr r0
    stw r0, 0x74(r1)
    stmw r25, 0x54(r1)
    mr r26, r3
    li r29, 0x0
    li r30, 0x0
    b @L_801187F4
@L_8011840C:
    lwz r0, lbl_8047AD9C(r13)
    add r3, r0, r30
    lbz r0, 0x0(r3)
    cmplwi r0, 0x1
    bne @L_801187EC
    li r28, 0x0
    mr r31, r3
@L_80118428:
    lwz r27, 0x8(r31)
    cmplwi r27, 0x0
    beq @L_801187DC
    lwz r0, 0x44(r27)
    cmpwi r0, 0x0
    beq @L_801187DC
    lbz r0, 0x6(r27)
    cmplwi r0, 0x0
    beq @L_801187DC
    lwz r3, 0x48(r27)
    lwz r4, 0x4c(r27)
    bl fn_800EE150
    addi r4, r1, 0x3c
    mr r25, r3
    addi r5, r1, 0x30
    addi r6, r1, 0x24
    bl fn_800EE3BC
    mr r3, r25
    bl fn_800EE828
    addi r3, r1, 0x14
    addi r4, r1, 0x30
    bl fn_800E06EC
    addi r3, r1, 0x8
    addi r4, r1, 0x14
    addi r5, r27, 0x50
    bl fn_800DFEEC
    addi r3, r1, 0x3c
    addi r5, r1, 0x8
    mr r4, r3
    bl fn_800E019C
    addi r3, r1, 0x30
    addi r5, r27, 0x5c
    mr r4, r3
    bl fn_800E019C
    addi r3, r1, 0x24
    addi r5, r27, 0x68
    mr r4, r3
    bl fn_800E0108
    addi r3, r27, 0x14
    addi r4, r27, 0x50
    bl fn_800E01D0
    lfs f0, 0x50(r27)
    addi r3, r27, 0x20
    lwz r5, 0x10(r27)
    addi r4, r27, 0x5c
    stfs f0, 0x20(r5)
    lfs f0, 0x54(r27)
    lwz r5, 0x10(r27)
    stfs f0, 0x24(r5)
    lfs f0, 0x58(r27)
    lwz r5, 0x10(r27)
    stfs f0, 0x28(r5)
    bl fn_800E01D0
    lfs f0, 0x5c(r27)
    addi r3, r27, 0x2c
    lwz r5, 0x10(r27)
    addi r4, r27, 0x68
    stfs f0, 0x8c(r5)
    lfs f0, 0x60(r27)
    lwz r5, 0x10(r27)
    stfs f0, 0x90(r5)
    lfs f0, 0x64(r27)
    lwz r5, 0x10(r27)
    stfs f0, 0x94(r5)
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
    lwz r0, 0x44(r27)
    cmplwi r0, 0x7
    bgt @L_801187CC
    lis r3, jumptable_8035BB88@ha
    slwi r0, r0, 2
    addi r3, r3, jumptable_8035BB88@l
    lwzx r0, r3, r0
    mtctr r0
    bctr
    addi r3, r27, 0x14
    addi r4, r1, 0x3c
    bl fn_800E01D0
    lfs f0, 0x3c(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x20(r3)
    lfs f0, 0x40(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x24(r3)
    lfs f0, 0x44(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x28(r3)
    b @L_801187CC
    addi r3, r27, 0x20
    addi r4, r1, 0x30
    bl fn_800E01D0
    lfs f0, 0x30(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x8c(r3)
    lfs f0, 0x34(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x90(r3)
    lfs f0, 0x38(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x94(r3)
    b @L_801187CC
    addi r3, r27, 0x2c
    addi r4, r1, 0x24
    bl fn_800E01D0
    lfs f0, 0x24(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x98(r3)
    lfs f0, 0x28(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x9c(r3)
    lfs f0, 0x2c(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0xa0(r3)
    b @L_801187CC
    addi r3, r27, 0x14
    addi r4, r1, 0x3c
    bl fn_800E01D0
    lfs f0, 0x3c(r1)
    addi r3, r27, 0x20
    lwz r5, 0x10(r27)
    addi r4, r1, 0x30
    stfs f0, 0x20(r5)
    lfs f0, 0x40(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x24(r5)
    lfs f0, 0x44(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x28(r5)
    bl fn_800E01D0
    lfs f0, 0x30(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x8c(r3)
    lfs f0, 0x34(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x90(r3)
    lfs f0, 0x38(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x94(r3)
    b @L_801187CC
    addi r3, r27, 0x20
    addi r4, r1, 0x30
    bl fn_800E01D0
    lfs f0, 0x30(r1)
    addi r3, r27, 0x2c
    lwz r5, 0x10(r27)
    addi r4, r1, 0x24
    stfs f0, 0x8c(r5)
    lfs f0, 0x34(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x90(r5)
    lfs f0, 0x38(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x94(r5)
    bl fn_800E01D0
    lfs f0, 0x24(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x98(r3)
    lfs f0, 0x28(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x9c(r3)
    lfs f0, 0x2c(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0xa0(r3)
    b @L_801187CC
    addi r3, r27, 0x14
    addi r4, r1, 0x3c
    bl fn_800E01D0
    lfs f0, 0x3c(r1)
    addi r3, r27, 0x2c
    lwz r5, 0x10(r27)
    addi r4, r1, 0x24
    stfs f0, 0x20(r5)
    lfs f0, 0x40(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x24(r5)
    lfs f0, 0x44(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x28(r5)
    bl fn_800E01D0
    lfs f0, 0x24(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x98(r3)
    lfs f0, 0x28(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x9c(r3)
    lfs f0, 0x2c(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0xa0(r3)
    b @L_801187CC
    addi r3, r27, 0x14
    addi r4, r1, 0x3c
    bl fn_800E01D0
    lfs f0, 0x3c(r1)
    addi r3, r27, 0x20
    lwz r5, 0x10(r27)
    addi r4, r1, 0x30
    stfs f0, 0x20(r5)
    lfs f0, 0x40(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x24(r5)
    lfs f0, 0x44(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x28(r5)
    bl fn_800E01D0
    lfs f0, 0x30(r1)
    addi r3, r27, 0x2c
    lwz r5, 0x10(r27)
    addi r4, r1, 0x24
    stfs f0, 0x8c(r5)
    lfs f0, 0x34(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x90(r5)
    lfs f0, 0x38(r1)
    lwz r5, 0x10(r27)
    stfs f0, 0x94(r5)
    bl fn_800E01D0
    lfs f0, 0x24(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x98(r3)
    lfs f0, 0x28(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0x9c(r3)
    lfs f0, 0x2c(r1)
    lwz r3, 0x10(r27)
    stfs f0, 0xa0(r3)
@L_801187CC:
    lbz r0, 0x5(r27)
    cntlzw r0, r0
    extrwi r0, r0, 8, 19
    stb r0, 0x6(r27)
@L_801187DC:
    addi r28, r28, 0x1
    addi r31, r31, 0x4
    cmplwi r28, 0x40
    blt @L_80118428
@L_801187EC:
    addi r30, r30, 0x108
    addi r29, r29, 0x1
@L_801187F4:
    lwz r0, lbl_8047ADA0(r13)
    cmplw r29, r0
    blt @L_8011840C
    li r27, 0x0
    b @L_8011881C
@L_80118808:
    li r3, 0x0
    bl psInterpretParticles
    li r3, 0x0
    bl psExecGenerator
    addi r27, r27, 0x1
@L_8011881C:
    cmplw r27, r26
    blt @L_80118808
    bl fn_800057A0
    cmpwi r3, 0x2
    bne @L_80118860
    lbz r3, lbl_8047ADB0(r13)
    addi r3, r3, 0x1
    clrlwi r0, r3, 24
    stb r3, lbl_8047ADB0(r13)
    cmplwi r0, 0x5
    blt @L_80118860
    li r3, 0x0
    bl psInterpretParticles
    li r3, 0x0
    bl psExecGenerator
    li r0, 0x0
    stb r0, lbl_8047ADB0(r13)
@L_80118860:
    lmw r25, 0x54(r1)
    lwz r0, 0x74(r1)
    mtlr r0
    addi r1, r1, 0x70
    blr
