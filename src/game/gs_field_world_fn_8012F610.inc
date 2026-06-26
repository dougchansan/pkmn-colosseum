    nofralloc
    stwu r1, -0x110(r1)
    mflr r0
    stw r0, 0x114(r1)
    stfd f31, 0x100(r1)
    psq_st f31, 0x108(r1), 0, 0
    stfd f30, 0xf0(r1)
    psq_st f30, 0xf8(r1), 0, 0
    stfd f29, 0xe0(r1)
    psq_st f29, 0xe8(r1), 0, 0
    stmw r24, 0xc0(r1)
    bl fn_800FF548
    clrlwi. r0, r3, 24
    bne @L_8012F8A8
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    addi r4, r4, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r4, 0x0(r4)
    stw r3, 0x18(r1)
    cmpwi r4, 0x0
    stw r0, 0x1c(r1)
    blt @L_8012F67C
    cmpwi r4, 0x2
    bge @L_8012F67C
    slwi r0, r4, 2
    addi r3, r1, 0x18
    lwzx r24, r3, r0
@L_8012F67C:
    mr r4, r24
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x44
    bl fn_800E3D98
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    addi r4, r4, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r4, 0x0(r4)
    stw r3, 0x10(r1)
    cmpwi r4, 0x0
    stw r0, 0x14(r1)
    blt @L_8012F6C8
    cmpwi r4, 0x2
    bge @L_8012F6C8
    slwi r0, r4, 2
    addi r3, r1, 0x10
    lwzx r28, r3, r0
@L_8012F6C8:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x50
    bl fn_800E3D6C
    lfs f1, 0x54(r1)
    bl sin
    frsp f31, f1
    lfs f1, 0x54(r1)
    bl cos
    frsp f1, f1
    lfs f0, 0x48(r1)
    lis r3, lbl_80426BD0@ha
    fneg f31, f31
    stfs f0, 0x3c(r1)
    addi r28, r3, lbl_80426BD0@l
    fneg f30, f1
    lfs f29, -26100(r2)
    addi r26, r1, 0x30
    addi r25, r1, 0x8
    li r24, 0x0
@L_8012F71C:
    cmpwi r24, 0x0
    blt @L_8012F72C
    cmpwi r24, 0x2
    blt @L_8012F734
@L_8012F72C:
    li r0, 0x0
    b @L_8012F74C
@L_8012F734:
    lhz r0, 0x4(r28)
    clrlwi. r0, r0, 31
    bne @L_8012F748
    li r0, 0x0
    b @L_8012F74C
@L_8012F748:
    li r0, 0x1
@L_8012F74C:
    clrlwi. r0, r0, 24
    beq @L_8012F890
    lis r3, lbl_80426BD0@ha
    lwz r0, lbl_80426BD0@l(r3)
    cmpw r24, r0
    beq @L_8012F890
    lfs f1, 0x44(r1)
    addi r3, r1, 0x38
    lfs f0, 0x4c(r1)
    addi r4, r1, 0x5c
    fmadds f2, f31, f29, f1
    lfs f1, 0x48(r1)
    fmadds f0, f30, f29, f0
    stfs f1, 0x3c(r1)
    stfs f2, 0x38(r1)
    stfs f0, 0x40(r1)
    bl fn_8010E138
    cmpwi r3, 0x0
    ble @L_8012F81C
    cmpwi r3, 0x2
    blt @L_8012F814
    lfs f5, -26056(r2)
    addi r4, r1, 0x5c
    lfs f2, 0x3c(r1)
    li r0, 0x0
    fmr f4, f5
    lfs f0, -26148(r2)
    mtctr r3
    cmpwi r3, 0x0
    ble @L_8012F7FC
@L_8012F7C4:
    lfs f3, 0x0(r4)
    fcmpo cr0, f4, f3
    bge @L_8012F7D4
    fmr f4, f3
@L_8012F7D4:
    fsubs f1, f3, f2
    fcmpo cr0, f1, f0
    cror eq, gt, eq
    beq @L_8012F7F4
    fcmpo cr0, f5, f3
    bge @L_8012F7F4
    fmr f5, f3
    li r0, 0x1
@L_8012F7F4:
    addi r4, r4, 0xc
    bdnz @L_8012F7C4
@L_8012F7FC:
    cmpwi r0, 0x0
    beq @L_8012F80C
    stfs f5, 0x3c(r1)
    b @L_8012F81C
@L_8012F80C:
    stfs f4, 0x3c(r1)
    b @L_8012F81C
@L_8012F814:
    lfs f0, 0x5c(r1)
    stfs f0, 0x3c(r1)
@L_8012F81C:
    lfs f0, -26100(r2)
    cmpwi r24, 0x0
    lwz r3, -26224(r2)
    lwz r0, -26220(r2)
    fadds f29, f29, f0
    stw r3, 0x30(r1)
    stw r0, 0x34(r1)
    blt @L_8012F848
    cmpwi r24, 0x2
    bge @L_8012F848
    lwz r31, 0x0(r26)
@L_8012F848:
    mr r4, r31
    addi r5, r1, 0x38
    li r3, 0x0
    bl fn_8018C0A8
    lwz r3, -26224(r2)
    cmpwi r24, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012F87C
    cmpwi r24, 0x2
    bge @L_8012F87C
    lwz r27, 0x0(r25)
@L_8012F87C:
    mr r4, r27
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x50
    bl fn_800E4170
@L_8012F890:
    addi r24, r24, 0x1
    addi r26, r26, 0x4
    cmpwi r24, 0x2
    addi r25, r25, 0x4
    addi r28, r28, 0x20
    blt @L_8012F71C
@L_8012F8A8:
    lis r3, lbl_80426BD0@ha
    addi r25, r1, 0x28
    addi r26, r3, lbl_80426BD0@l
    li r27, 0x0
    mr r24, r26
@L_8012F8BC:
    cmpwi r27, 0x0
    blt @L_8012F8CC
    cmpwi r27, 0x2
    blt @L_8012F8D4
@L_8012F8CC:
    li r5, 0x0
    b @L_8012F8EC
@L_8012F8D4:
    lhz r0, 0x4(r24)
    clrlwi. r0, r0, 31
    bne @L_8012F8E8
    li r5, 0x0
    b @L_8012F8EC
@L_8012F8E8:
    li r5, 0x1
@L_8012F8EC:
    lwz r3, -26224(r2)
    cmpwi r27, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x28(r1)
    stw r0, 0x2c(r1)
    blt @L_8012F910
    cmpwi r27, 0x2
    bge @L_8012F910
    lwz r30, 0x0(r25)
@L_8012F910:
    mr r4, r30
    li r3, 0x0
    bl fn_8018C1E8
    addi r27, r27, 0x1
    addi r25, r25, 0x4
    cmpwi r27, 0x2
    addi r24, r24, 0x20
    blt @L_8012F8BC
    lfs f0, -26216(r2)
    addi r25, r1, 0x20
    li r27, 0x0
    stfs f0, 0x14(r26)
    stfs f0, 0x18(r26)
    stfs f0, 0x1c(r26)
    stfs f0, 0x20(r26)
    stfs f0, 0x34(r26)
    stfs f0, 0x38(r26)
    stfs f0, 0x3c(r26)
    stfs f0, 0x40(r26)
@L_8012F95C:
    cmpwi r27, 0x0
    blt @L_8012F96C
    cmpwi r27, 0x2
    blt @L_8012F974
@L_8012F96C:
    li r0, 0x0
    b @L_8012F98C
@L_8012F974:
    lhz r0, 0x4(r26)
    clrlwi. r0, r0, 31
    bne @L_8012F988
    li r0, 0x0
    b @L_8012F98C
@L_8012F988:
    li r0, 0x1
@L_8012F98C:
    clrlwi. r0, r0, 24
    beq @L_8012FA7C
    cmpwi r27, 0x0
    blt @L_8012F9A4
    cmpwi r27, 0x2
    blt @L_8012F9AC
@L_8012F9A4:
    li r0, 0x0
    b @L_8012F9C4
@L_8012F9AC:
    lhz r0, 0x4(r26)
    clrlwi. r0, r0, 31
    bne @L_8012F9C0
    li r0, 0x0
    b @L_8012F9C4
@L_8012F9C0:
    li r0, 0x1
@L_8012F9C4:
    clrlwi. r0, r0, 24
    beq @L_8012F9D4
    lwz r24, 0xc(r26)
    b @L_8012F9D8
@L_8012F9D4:
    li r24, 0x2
@L_8012F9D8:
    cmpwi r24, 0x0
    blt @L_8012FA7C
    cmpwi r24, 0x2
    bge @L_8012FA7C
    cmpwi r27, 0x0
    blt @L_8012F9F8
    cmpwi r27, 0x2
    blt @L_8012FA00
@L_8012F9F8:
    li r0, 0x0
    b @L_8012FA18
@L_8012FA00:
    lhz r0, 0x4(r26)
    clrlwi. r0, r0, 31
    bne @L_8012FA14
    li r0, 0x0
    b @L_8012FA18
@L_8012FA14:
    li r0, 0x1
@L_8012FA18:
    clrlwi. r0, r0, 24
    beq @L_8012FA7C
    lwz r3, -26224(r2)
    cmpwi r27, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x20(r1)
    stw r0, 0x24(r1)
    blt @L_8012FA44
    cmpwi r27, 0x2
    bge @L_8012FA44
    lwz r29, 0x0(r25)
@L_8012FA44:
    lwz r0, 0xc(r26)
    cmpwi r0, 0x1
    beq @L_8012FA54
    b @L_8012FA60
@L_8012FA54:
    mr r4, r29
    li r3, 0x0
    bl fn_80188AF4
@L_8012FA60:
    cmpwi r24, 0x1
    beq @L_8012FA6C
    b @L_8012FA78
@L_8012FA6C:
    mr r4, r29
    li r3, 0x0
    bl fn_80188F78
@L_8012FA78:
    stw r24, 0xc(r26)
@L_8012FA7C:
    addi r27, r27, 0x1
    addi r25, r25, 0x4
    cmpwi r27, 0x2
    addi r26, r26, 0x20
    blt @L_8012F95C
    lis r3, lbl_80426BD0@ha
    lfs f0, -26216(r2)
    addi r3, r3, lbl_80426BD0@l
    li r0, 0x12c
    stw r0, 0x10(r3)
    stw r0, 0x30(r3)
    stfs f0, 0x13c(r3)
    psq_l f31, 0x108(r1), 0, 0
    lfd f31, 0x100(r1)
    psq_l f30, 0xf8(r1), 0, 0
    lfd f30, 0xf0(r1)
    psq_l f29, 0xe8(r1), 0, 0
    lfd f29, 0xe0(r1)
    lmw r24, 0xc0(r1)
    lwz r0, 0x114(r1)
    mtlr r0
    addi r1, r1, 0x110
    blr
