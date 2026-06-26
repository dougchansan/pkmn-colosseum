    nofralloc
    stwu r1, -0x80(r1)
    mflr r0
    stw r0, 0x84(r1)
    stfd f31, 0x70(r1)
    psq_st f31, 0x78(r1), 0, 0
    stfd f30, 0x60(r1)
    psq_st f30, 0x68(r1), 0, 0
    stmw r23, 0x3c(r1)
    mr r23, r3
    mr r24, r4
    mr r26, r5
    mr r25, r6
    bl fn_800D3088
    lis r0, 0x4330
    stw r3, 0x34(r1)
    lfd f1, -26168(r2)
    mr r3, r24
    stw r0, 0x30(r1)
    addi r4, r1, 0x24
    lfd f0, 0x30(r1)
    fsubs f30, f0, f1
    bl fn_800E3D98
    addi r28, r1, 0x8
    lfs f31, -26216(r2)
    mr r31, r28
    mr r30, r23
    mr r29, r26
    li r26, 0x0
@L_8012BF24:
    stfs f31, 0x10(r30)
    stfs f31, 0x14(r30)
    stfs f31, 0x18(r30)
    lwz r4, 0x0(r29)
    stfs f31, 0x0(r31)
    cmpwi r4, 0x0
    blt @L_8012BF90
    mr r3, r24
    bl fn_800EE150
    mr. r27, r3
    beq @L_8012BF90
    addi r4, r1, 0x18
    li r5, 0x0
    li r6, 0x0
    bl fn_800EE3BC
    mr r3, r27
    bl fn_800EE828
    lfs f1, 0x1c(r1)
    lfs f0, 0x28(r1)
    lwz r3, 0x18(r1)
    lwz r0, 0x1c(r1)
    fsubs f0, f1, f0
    stw r3, 0x10(r30)
    stw r0, 0x14(r30)
    lwz r0, 0x20(r1)
    stfs f0, 0x0(r31)
    stw r0, 0x18(r30)
@L_8012BF90:
    addi r26, r26, 0x1
    addi r30, r30, 0xc
    cmpwi r26, 0x4
    addi r29, r29, 0x4
    addi r31, r31, 0x4
    blt @L_8012BF24
    lfs f2, -26208(r2)
    lfs f3, 0x0(r25)
    lfs f0, -26216(r2)
    fcmpo cr0, f3, f2
    cror eq, gt, eq
    bne @L_8012BFDC
    lfs f1, 0x8(r1)
    fcmpo cr0, f1, f2
    bge @L_8012BFDC
    fsubs f1, f3, f1
    fdivs f1, f1, f30
    stfs f1, 0x0(r23)
    b @L_8012BFE0
@L_8012BFDC:
    stfs f0, 0x0(r23)
@L_8012BFE0:
    lfs f3, 0x4(r25)
    fcmpo cr0, f3, f2
    cror eq, gt, eq
    bne @L_8012C00C
    lfs f1, 0xc(r1)
    fcmpo cr0, f1, f2
    bge @L_8012C00C
    fsubs f1, f3, f1
    fdivs f1, f1, f30
    stfs f1, 0x4(r23)
    b @L_8012C010
@L_8012C00C:
    stfs f0, 0x4(r23)
@L_8012C010:
    lfs f3, 0x8(r25)
    fcmpo cr0, f3, f2
    cror eq, gt, eq
    bne @L_8012C03C
    lfs f1, 0x8(r28)
    fcmpo cr0, f1, f2
    bge @L_8012C03C
    fsubs f1, f3, f1
    fdivs f1, f1, f30
    stfs f1, 0x8(r23)
    b @L_8012C040
@L_8012C03C:
    stfs f0, 0x8(r23)
@L_8012C040:
    lfs f3, 0xc(r25)
    fcmpo cr0, f3, f2
    cror eq, gt, eq
    bne @L_8012C06C
    lfs f1, 0xc(r28)
    fcmpo cr0, f1, f2
    bge @L_8012C06C
    fsubs f1, f3, f1
    fdivs f1, f1, f30
    stfs f1, 0xc(r23)
    b @L_8012C070
@L_8012C06C:
    stfs f0, 0xc(r23)
@L_8012C070:
    lfs f1, 0x8(r1)
    lfs f0, 0xc(r1)
    stfs f1, 0x0(r25)
    lfs f1, 0x10(r1)
    stfs f0, 0x4(r25)
    lfs f0, 0x14(r1)
    stfs f1, 0x8(r25)
    stfs f0, 0xc(r25)
    psq_l f31, 0x78(r1), 0, 0
    lfd f31, 0x70(r1)
    psq_l f30, 0x68(r1), 0, 0
    lfd f30, 0x60(r1)
    lmw r23, 0x3c(r1)
    lwz r0, 0x84(r1)
    mtlr r0
    addi r1, r1, 0x80
    blr
