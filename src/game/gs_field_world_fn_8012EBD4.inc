    nofralloc
    stwu r1, -0xb0(r1)
    mflr r0
    stw r0, 0xb4(r1)
    stfd f31, 0xa0(r1)
    psq_st f31, 0xa8(r1), 0, 0
    stfd f30, 0x90(r1)
    psq_st f30, 0x98(r1), 0, 0
    stmw r26, 0x78(r1)
    bl fn_801337B0
    clrlwi. r0, r3, 24
    beq @L_8012EC08
    li r3, 0x0
    b @L_8012EF94
@L_8012EC08:
    li r3, 0xca
    bl fn_80102620
    clrlwi. r0, r3, 24
    beq @L_8012EC20
    li r3, 0x0
    b @L_8012EF94
@L_8012EC20:
    bl GSscene_GetMode
    clrlwi r0, r3, 24
    cmplwi r0, 0x6
    bne @L_8012EC38
    li r3, 0x0
    b @L_8012EF94
@L_8012EC38:
    lis r3, lbl_80426BD0@ha
    addi r4, r3, lbl_80426BD0@l
    lwz r3, 0x188(r4)
    cmpwi r3, 0x0
    ble @L_8012EC5C
    subi r0, r3, 0x1
    li r3, 0x0
    stw r0, 0x188(r4)
    b @L_8012EF94
@L_8012EC5C:
    lwz r4, 0x0(r4)
    lwz r3, -26224(r2)
    lwz r0, -26220(r2)
    cmpwi r4, 0x0
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012EC8C
    cmpwi r4, 0x2
    bge @L_8012EC8C
    slwi r0, r4, 2
    addi r3, r1, 0x10
    lwzx r26, r3, r0
@L_8012EC8C:
    mr r4, r26
    li r3, 0x0
    lis r5, 0x8000
    bl fn_8018C424
    clrlwi. r0, r3, 24
    beq @L_8012ECAC
    li r3, 0x0
    b @L_8012EF94
@L_8012ECAC:
    lis r3, lbl_80426BD0@ha
    addi r29, r3, lbl_80426BD0@l
    lwz r3, 0x18c(r29)
    cmplwi r3, 0x0
    beq @L_8012ECEC
    li r0, 0x0
    lwz r5, 0x190(r29)
    stw r0, 0x18c(r29)
    li r4, 0x4
    lwz r6, 0x194(r29)
    lwz r7, 0x198(r29)
    lwz r8, 0x19c(r29)
    crxor 6, 6, 6
    bl fn_800F7434
    li r3, 0x0
    b @L_8012EF94
@L_8012ECEC:
    mr r28, r29
    li r26, 0x0
    b @L_8012ED0C
@L_8012ECF8:
    lhz r4, 0x1d0(r28)
    li r3, 0x3
    bl fn_80116D30
    addi r28, r28, 0x34
    addi r26, r26, 0x1
@L_8012ED0C:
    lwz r0, 0x410(r29)
    cmpw r26, r0
    blt @L_8012ECF8
    lis r3, lbl_80426BD0@ha
    mr r31, r29
    addi r28, r3, lbl_80426BD0@l
    li r26, 0x0
    b @L_8012ED40
@L_8012ED2C:
    lhz r4, 0x370(r31)
    li r3, 0x2
    bl fn_80116D30
    addi r31, r31, 0x34
    addi r26, r26, 0x1
@L_8012ED40:
    lwz r0, 0x418(r28)
    cmpw r26, r0
    blt @L_8012ED2C
    lis r3, lbl_80426BD0@ha
    mr r27, r29
    addi r31, r3, lbl_80426BD0@l
    li r26, 0x0
    b @L_8012ED74
@L_8012ED60:
    lhz r4, 0x2a0(r27)
    li r3, 0x1
    bl fn_80116D30
    addi r27, r27, 0x34
    addi r26, r26, 0x1
@L_8012ED74:
    lwz r0, 0x414(r31)
    cmpw r26, r0
    blt @L_8012ED60
    lwz r0, 0x18c(r29)
    li r3, 0x0
    stw r3, 0x414(r31)
    cmplwi r0, 0x0
    stw r3, 0x418(r28)
    stw r3, 0x410(r29)
    beq @L_8012EDA4
    li r3, 0x0
    b @L_8012EF94
@L_8012EDA4:
    li r3, 0x1
    bl fn_800F7AF0
    mr r31, r3
    li r3, 0x1
    bl fn_800F7BC4
    and r0, r3, r31
    rlwinm. r0, r0, 0, 19, 21
    beq @L_8012EDD4
    li r3, 0x0
    bl fn_8000D710
    li r3, 0x0
    b @L_8012EF94
@L_8012EDD4:
    li r28, 0x0
    mr r27, r29
@L_8012EDDC:
    cmpwi r28, 0x0
    blt @L_8012EDEC
    cmpwi r28, 0x2
    blt @L_8012EDF4
@L_8012EDEC:
    li r0, 0x0
    b @L_8012EE0C
@L_8012EDF4:
    lhz r0, 0x4(r27)
    clrlwi. r0, r0, 31
    bne @L_8012EE08
    li r0, 0x0
    b @L_8012EE0C
@L_8012EE08:
    li r0, 0x1
@L_8012EE0C:
    clrlwi. r0, r0, 24
    beq @L_8012EE2C
    lis r3, lbl_80426BD0@ha
    lwz r0, lbl_80426BD0@l(r3)
    cmpw r28, r0
    beq @L_8012EE2C
    mr r3, r28
    bl fn_8012DE94
@L_8012EE2C:
    addi r28, r28, 0x1
    addi r27, r27, 0x20
    cmpwi r28, 0x2
    blt @L_8012EDDC
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    lwz r3, 0x0(r3)
    bl fn_8012E7B8
    lis r3, lbl_80272A38@ha
    lis r4, lbl_80426BD0@ha
    addi r10, r3, lbl_80272A38@l
    fmr f30, f1
    lwz r9, 0x0(r10)
    addi r11, r4, lbl_80426BD0@l
    lwz r8, 0x4(r10)
    lwz r7, 0x8(r10)
    lwz r6, 0xc(r10)
    lwz r5, 0x10(r10)
    lwz r4, 0x14(r10)
    lwz r3, 0x18(r10)
    lwz r0, 0x1c(r10)
    stw r9, 0x18(r1)
    lwz r31, 0x0(r11)
    stw r8, 0x1c(r1)
    stw r7, 0x20(r1)
    stw r6, 0x24(r1)
    stw r5, 0x28(r1)
    stw r4, 0x2c(r1)
    stw r3, 0x30(r1)
    stw r0, 0x34(r1)
    bl fn_800D3088
    lwz r3, -26224(r2)
    cmpwi r31, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012EED4
    cmpwi r31, 0x2
    bge @L_8012EED4
    slwi r0, r31, 2
    addi r3, r1, 0x8
    lwzx r30, r3, r0
@L_8012EED4:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    cmplwi r3, 0x0
    beq @L_8012EF14
    lis r4, lbl_80426BD0@ha
    slwi r6, r31, 5
    addi r0, r4, lbl_80426BD0@l
    slwi r7, r31, 4
    addi r5, r1, 0x18
    mr r4, r3
    add r6, r0, r6
    addi r3, r1, 0x38
    add r5, r5, r7
    addi r6, r6, 0x14
    bl fn_8012BEB4
@L_8012EF14:
    li r3, 0x8ae
    bl fn_801906A0
    cntlzw r0, r3
    extrwi. r0, r0, 8, 19
    beq @L_8012EF90
    lis r3, lbl_80426BD0@ha
    lfs f31, -26172(r2)
    addi r30, r3, lbl_80426BD0@l
    lfs f0, 0x13c(r30)
    fadds f0, f0, f30
    stfs f0, 0x13c(r30)
    b @L_8012EF80
@L_8012EF44:
    li r28, 0x0
    mr r27, r29
@L_8012EF4C:
    lwz r12, 0x140(r27)
    cmplwi r12, 0x0
    beq @L_8012EF64
    lwz r3, 0x144(r27)
    mtctr r12
    bctrl
@L_8012EF64:
    addi r28, r28, 0x1
    addi r27, r27, 0x8
    cmpwi r28, 0x8
    blt @L_8012EF4C
    lfs f0, 0x13c(r30)
    fsubs f0, f0, f31
    stfs f0, 0x13c(r30)
@L_8012EF80:
    lfs f0, 0x13c(r30)
    fcmpo cr0, f0, f31
    cror eq, gt, eq
    beq @L_8012EF44
@L_8012EF90:
    li r3, 0x0
@L_8012EF94:
    psq_l f31, 0xa8(r1), 0, 0
    lfd f31, 0xa0(r1)
    psq_l f30, 0x98(r1), 0, 0
    lfd f30, 0x90(r1)
    lmw r26, 0x78(r1)
    lwz r0, 0xb4(r1)
    mtlr r0
    addi r1, r1, 0xb0
    blr
