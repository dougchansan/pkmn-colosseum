    nofralloc
    stwu r1, -0x120(r1)
    mflr r0
    stw r0, 0x124(r1)
    stfd f31, 0x110(r1)
    psq_st f31, 0x118(r1), 0, 0
    stfd f30, 0x100(r1)
    psq_st f30, 0x108(r1), 0, 0
    stfd f29, 0xf0(r1)
    psq_st f29, 0xf8(r1), 0, 0
    stfd f28, 0xe0(r1)
    psq_st f28, 0xe8(r1), 0, 0
    stmw r23, 0xbc(r1)
    mr r23, r3
    mr r24, r4
    mr r25, r5
    bl fn_800D3088
    lis r5, 0x4330
    lwz r4, -26224(r2)
    stw r3, 0xac(r1)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r5, 0xa8(r1)
    lfd f1, -26168(r2)
    lfd f0, 0xa8(r1)
    stw r4, 0x48(r1)
    fsubs f29, f0, f1
    stw r0, 0x4c(r1)
    blt @L_8012CB08
    cmpwi r23, 0x2
    bge @L_8012CB08
    slwi r0, r23, 2
    addi r3, r1, 0x48
    lwzx r26, r3, r0
@L_8012CB08:
    lfs f0, 0x0(r24)
    lfs f1, 0x8(r24)
    fmuls f2, f0, f0
    lfs f0, -26216(r2)
    fmuls f1, f1, f1
    fadds f31, f2, f1
    fcmpo cr0, f31, f0
    ble @L_8012CB70
    frsqrte f1, f31
    lfd f3, -26200(r2)
    lfd f2, -26192(r2)
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f31, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f31, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f31, f0, f2
    fmul f0, f1, f0
    fmul f31, f31, f0
    frsp f31, f31
    b @L_8012CBF4
@L_8012CB70:
    lfd f0, -26184(r2)
    fcmpo cr0, f31, f0
    bge @L_8012CB88
    lis r3, lbl_80478AC0@ha
    lfs f31, lbl_80478AC0@l(r3)
    b @L_8012CBF4
@L_8012CB88:
    stfs f31, 0xc(r1)
    lis r0, 0x7f80
    lwz r4, 0xc(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012CBB0
    bge @L_8012CBE0
    cmpwi r3, 0x0
    beq @L_8012CBC8
    b @L_8012CBE0
@L_8012CBB0:
    clrlwi. r0, r4, 9
    beq @L_8012CBC0
    li r0, 0x1
    b @L_8012CBE4
@L_8012CBC0:
    li r0, 0x2
    b @L_8012CBE4
@L_8012CBC8:
    clrlwi. r0, r4, 9
    beq @L_8012CBD8
    li r0, 0x5
    b @L_8012CBE4
@L_8012CBD8:
    li r0, 0x3
    b @L_8012CBE4
@L_8012CBE0:
    li r0, 0x4
@L_8012CBE4:
    cmpwi r0, 0x1
    bne @L_8012CBF4
    lis r3, lbl_80478AC0@ha
    lfs f31, lbl_80478AC0@l(r3)
@L_8012CBF4:
    lfs f0, -26216(r2)
    fcmpo cr0, f31, f0
    ble @L_8012CE24
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x38(r1)
    stw r0, 0x3c(r1)
    blt @L_8012CC2C
    cmpwi r23, 0x2
    bge @L_8012CC2C
    slwi r0, r23, 2
    addi r3, r1, 0x38
    lwzx r31, r3, r0
@L_8012CC2C:
    mr r4, r31
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x5c
    bl fn_800E3D98
    lfs f0, -26216(r2)
    mr r4, r26
    mr r5, r24
    li r3, 0x0
    stfs f0, 0x60(r1)
    li r6, 0x0
    bl fn_801885C4
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x30(r1)
    stw r0, 0x34(r1)
    blt @L_8012CC88
    cmpwi r23, 0x2
    bge @L_8012CC88
    slwi r0, r23, 2
    addi r3, r1, 0x30
    lwzx r30, r3, r0
@L_8012CC88:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x50
    bl fn_800E3D98
    lfs f0, -26216(r2)
    addi r3, r1, 0x50
    addi r4, r1, 0x5c
    addi r5, r1, 0x68
    stfs f0, 0x54(r1)
    bl fn_800A3A9C
    fdivs f30, f31, f29
    lfs f0, -26144(r2)
    fcmpo cr0, f30, f0
    ble @L_8012CCC8
    fmr f30, f0
@L_8012CCC8:
    fdivs f1, f30, f31
    addi r3, r1, 0x68
    addi r4, r1, 0x74
    bl fn_800A3AC0
    lfs f0, 0x0(r25)
    lfs f1, 0x8(r25)
    fmuls f2, f0, f0
    lfs f0, -26216(r2)
    fmuls f1, f1, f1
    fadds f4, f2, f1
    fcmpo cr0, f4, f0
    ble @L_8012CD40
    frsqrte f1, f4
    lfd f3, -26200(r2)
    lfd f2, -26192(r2)
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f3, f1
    fnmsub f0, f4, f0, f2
    fmul f0, f1, f0
    fmul f4, f4, f0
    frsp f4, f4
    b @L_8012CDC4
@L_8012CD40:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012CD58
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
    b @L_8012CDC4
@L_8012CD58:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012CD80
    bge @L_8012CDB0
    cmpwi r3, 0x0
    beq @L_8012CD98
    b @L_8012CDB0
@L_8012CD80:
    clrlwi. r0, r4, 9
    beq @L_8012CD90
    li r0, 0x1
    b @L_8012CDB4
@L_8012CD90:
    li r0, 0x2
    b @L_8012CDB4
@L_8012CD98:
    clrlwi. r0, r4, 9
    beq @L_8012CDA8
    li r0, 0x5
    b @L_8012CDB4
@L_8012CDA8:
    li r0, 0x3
    b @L_8012CDB4
@L_8012CDB0:
    li r0, 0x4
@L_8012CDB4:
    cmpwi r0, 0x1
    bne @L_8012CDC4
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
@L_8012CDC4:
    lfs f0, -26176(r2)
    fcmpo cr0, f4, f0
    ble @L_8012CDF0
    lfs f1, 0x0(r25)
    lfs f2, 0x8(r25)
    bl fn_800CE2D8
    frsp f1, f1
    mr r4, r26
    fmr f2, f30
    li r3, 0x0
    bl fn_8018805C
@L_8012CDF0:
    mr r4, r26
    addi r5, r1, 0x74
    li r3, 0x0
    bl fn_801887D8
    fmr f28, f1
    mr r4, r25
    addi r3, r1, 0x68
    bl fn_800A3B7C
    lfs f0, -26216(r2)
    fcmpo cr0, f1, f0
    bge @L_8012D244
    fneg f28, f28
    b @L_8012D244
@L_8012CE24:
    cmpwi r23, 0x0
    blt @L_8012CE34
    cmpwi r23, 0x2
    blt @L_8012CE3C
@L_8012CE34:
    li r0, 0x0
    b @L_8012CE64
@L_8012CE3C:
    lis r3, lbl_80426BD0@ha
    slwi r0, r23, 5
    addi r3, r3, lbl_80426BD0@l
    add r3, r3, r0
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012CE60
    li r0, 0x0
    b @L_8012CE64
@L_8012CE60:
    li r0, 0x1
@L_8012CE64:
    clrlwi. r0, r0, 24
    beq @L_8012CE84
    lis r3, lbl_80426BD0@ha
    slwi r0, r23, 5
    addi r3, r3, lbl_80426BD0@l
    add r3, r3, r0
    lwz r0, 0xc(r3)
    b @L_8012CE88
@L_8012CE84:
    li r0, 0x2
@L_8012CE88:
    cmpwi r0, 0x1
    beq @L_8012CEA4
    mr r4, r26
    li r3, 0x0
    bl fn_8018790C
    lfs f28, -26216(r2)
    b @L_8012D244
@L_8012CEA4:
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    addi r4, r4, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r4, 0x0(r4)
    stw r3, 0x28(r1)
    cmpwi r4, 0x0
    stw r0, 0x2c(r1)
    blt @L_8012CEDC
    cmpwi r4, 0x2
    bge @L_8012CEDC
    slwi r0, r4, 2
    addi r3, r1, 0x28
    lwzx r30, r3, r0
@L_8012CEDC:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x8c
    bl fn_800E3D98
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x20(r1)
    stw r0, 0x24(r1)
    blt @L_8012CF1C
    cmpwi r23, 0x2
    bge @L_8012CF1C
    slwi r0, r23, 2
    addi r3, r1, 0x20
    lwzx r29, r3, r0
@L_8012CF1C:
    mr r4, r29
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x80
    bl fn_800E3D98
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x18(r1)
    stw r0, 0x1c(r1)
    blt @L_8012CF5C
    cmpwi r23, 0x2
    bge @L_8012CF5C
    slwi r0, r23, 2
    addi r3, r1, 0x18
    lwzx r28, r3, r0
@L_8012CF5C:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x98
    bl fn_800E3D6C
    lfs f4, 0x9c(r1)
    lfs f0, -26124(r2)
    b @L_8012CF80
@L_8012CF7C:
    fsubs f4, f4, f0
@L_8012CF80:
    fcmpo cr0, f4, f0
    cror eq, gt, eq
    beq @L_8012CF7C
    lfs f1, -26124(r2)
    lfs f0, -26120(r2)
    b @L_8012CF9C
@L_8012CF98:
    fadds f4, f4, f1
@L_8012CF9C:
    fcmpo cr0, f4, f0
    cror eq, lt, eq
    beq @L_8012CF98
    lfs f3, 0x8c(r1)
    lfs f1, 0x80(r1)
    lfs f2, 0x94(r1)
    lfs f0, 0x88(r1)
    fsubs f1, f3, f1
    stfs f4, 0x9c(r1)
    fsubs f2, f2, f0
    bl fn_800CE2D8
    frsp f28, f1
    lfs f1, 0x9c(r1)
    lfs f0, -26116(r2)
    fsubs f31, f28, f1
    fcmpo cr0, f31, f0
    bge @L_8012CFEC
    lfs f0, -26124(r2)
    fadds f31, f31, f0
    b @L_8012D000
@L_8012CFEC:
    lfs f0, -26112(r2)
    fcmpo cr0, f31, f0
    ble @L_8012D000
    lfs f0, -26124(r2)
    fsubs f31, f31, f0
@L_8012D000:
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x40(r1)
    stw r0, 0x44(r1)
    blt @L_8012D020
    cmpwi r23, 0x2
    blt @L_8012D028
@L_8012D020:
    li r0, 0x0
    b @L_8012D038
@L_8012D028:
    slwi r0, r23, 2
    addi r3, r1, 0x40
    lwzx r31, r3, r0
    li r0, 0x1
@L_8012D038:
    clrlwi. r0, r0, 24
    bne @L_8012D048
    li r3, -0x1
    b @L_8012D06C
@L_8012D048:
    mr r4, r31
    li r3, 0x0
    bl fn_8018D998
    bl fn_8018D928
    cmplwi r3, 0x0
    bne @L_8012D068
    li r3, -0x1
    b @L_8012D06C
@L_8012D068:
    lwz r3, 0x30(r3)
@L_8012D06C:
    bl fn_8018F6F4
    mr r24, r3
    bl fn_8018F678
    lfs f0, -26216(r2)
    fcmpo cr0, f1, f0
    ble @L_8012D094
    mr r3, r24
    bl fn_8018F678
    fmr f29, f1
    b @L_8012D0A0
@L_8012D094:
    mr r3, r24
    bl fn_8018F678
    fneg f29, f1
@L_8012D0A0:
    mr r3, r24
    bl fn_8018F658
    lfs f0, -26216(r2)
    fcmpo cr0, f1, f0
    ble @L_8012D0C0
    mr r3, r24
    bl fn_8018F658
    b @L_8012D0CC
@L_8012D0C0:
    mr r3, r24
    bl fn_8018F658
    fneg f1, f1
@L_8012D0CC:
    lfs f0, -26216(r2)
    li r3, 0x0
    fcmpo cr0, f31, f0
    bge @L_8012D110
    ble @L_8012D0E4
    b @L_8012D0E8
@L_8012D0E4:
    fneg f31, f31
@L_8012D0E8:
    fcmpo cr0, f31, f1
    ble @L_8012D150
    fadds f30, f28, f1
    lfs f0, -26124(r2)
    fcmpo cr0, f30, f0
    cror eq, gt, eq
    bne @L_8012D108
    fsubs f30, f30, f0
@L_8012D108:
    li r3, 0x1
    b @L_8012D150
@L_8012D110:
    fcmpo cr0, f31, f29
    ble @L_8012D150
    fsubs f1, f28, f29
    lfs f0, -26116(r2)
    fcmpo cr0, f1, f0
    bge @L_8012D134
    lfs f0, -26124(r2)
    fadds f1, f1, f0
    b @L_8012D148
@L_8012D134:
    lfs f0, -26112(r2)
    fcmpo cr0, f1, f0
    ble @L_8012D148
    lfs f0, -26124(r2)
    fsubs f1, f1, f0
@L_8012D148:
    fmr f30, f1
    li r3, 0x1
@L_8012D150:
    clrlwi. r0, r3, 24
    beq @L_8012D210
    lfs f3, 0x9c(r1)
    lfs f0, -26116(r2)
    fsubs f2, f30, f3
    fmr f1, f2
    fcmpo cr0, f2, f0
    bge @L_8012D17C
    lfs f0, -26124(r2)
    fadds f1, f2, f0
    b @L_8012D190
@L_8012D17C:
    lfs f0, -26112(r2)
    fcmpo cr0, f2, f0
    ble @L_8012D190
    lfs f0, -26124(r2)
    fsubs f1, f2, f0
@L_8012D190:
    lfs f0, -26216(r2)
    fcmpo cr0, f1, f0
    ble @L_8012D1CC
    lfs f0, -26116(r2)
    fcmpo cr0, f2, f0
    bge @L_8012D1B4
    lfs f0, -26124(r2)
    fadds f2, f2, f0
    b @L_8012D1FC
@L_8012D1B4:
    lfs f0, -26112(r2)
    fcmpo cr0, f2, f0
    ble @L_8012D1FC
    lfs f0, -26124(r2)
    fsubs f2, f2, f0
    b @L_8012D1FC
@L_8012D1CC:
    lfs f0, -26116(r2)
    fcmpo cr0, f2, f0
    bge @L_8012D1E4
    lfs f0, -26124(r2)
    fadds f2, f2, f0
    b @L_8012D1F8
@L_8012D1E4:
    lfs f0, -26112(r2)
    fcmpo cr0, f2, f0
    ble @L_8012D1F8
    lfs f0, -26124(r2)
    fsubs f2, f2, f0
@L_8012D1F8:
    fneg f2, f2
@L_8012D1FC:
    lfs f0, -26108(r2)
    fcmpo cr0, f2, f0
    bge @L_8012D210
    fmr f30, f3
    li r3, 0x0
@L_8012D210:
    clrlwi. r0, r3, 24
    beq @L_8012D234
    fmr f1, f30
    lfs f2, -26144(r2)
    mr r4, r26
    li r3, 0x0
    bl fn_8018805C
    lfs f28, -26144(r2)
    b @L_8012D244
@L_8012D234:
    mr r4, r26
    li r3, 0x0
    bl fn_8018790C
    lfs f28, -26216(r2)
@L_8012D244:
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012D270
    cmpwi r23, 0x2
    bge @L_8012D270
    slwi r0, r23, 2
    addi r3, r1, 0x10
    lwzx r27, r3, r0
@L_8012D270:
    mr r4, r27
    li r3, 0x0
    bl fn_800F9318
    fmr f1, f28
    mr r4, r23
    bl fn_8012C660
    psq_l f31, 0x118(r1), 0, 0
    lfd f31, 0x110(r1)
    psq_l f30, 0x108(r1), 0, 0
    lfd f30, 0x100(r1)
    psq_l f29, 0xf8(r1), 0, 0
    lfd f29, 0xf0(r1)
    psq_l f28, 0xe8(r1), 0, 0
    lfd f28, 0xe0(r1)
    lmw r23, 0xbc(r1)
    lwz r0, 0x124(r1)
    mtlr r0
    addi r1, r1, 0x120
    blr
