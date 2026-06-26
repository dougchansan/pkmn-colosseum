    nofralloc
    stwu r1, -0xd0(r1)
    mflr r0
    stw r0, 0xd4(r1)
    stfd f31, 0xc0(r1)
    psq_st f31, 0xc8(r1), 0, 0
    stfd f30, 0xb0(r1)
    psq_st f30, 0xb8(r1), 0, 0
    stmw r27, 0x9c(r1)
    mr r27, r3
    lis r3, lbl_80426BD0@ha
    slwi r6, r27, 5
    mr r28, r4
    addi r0, r3, lbl_80426BD0@l
    mr r29, r5
    add r3, r0, r6
    lfs f31, 0x8(r3)
    bl fn_800D3088
    lis r0, 0x4330
    stw r3, 0x94(r1)
    lis r4, lbl_80426BD0@ha
    lwz r3, -26224(r2)
    stw r0, 0x90(r1)
    lwz r4, lbl_80426BD0@l(r4)
    lwz r0, -26220(r2)
    lfd f1, -26168(r2)
    cmpwi r4, 0x0
    lfd f0, 0x90(r1)
    stw r3, 0x1c(r1)
    fsubs f30, f0, f1
    stw r0, 0x20(r1)
    blt @L_8012D880
    cmpwi r4, 0x2
    bge @L_8012D880
    slwi r0, r4, 2
    addi r3, r1, 0x1c
    lwzx r31, r3, r0
@L_8012D880:
    mr r4, r31
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x84
    bl fn_800E3D98
    lis r3, lbl_80426BD0@ha
    addi r3, r3, lbl_80426BD0@l
    lwz r0, 0x48(r3)
    cmpwi r0, 0x0
    bgt @L_8012D8B0
    li r31, 0x0
    b @L_8012D8EC
@L_8012D8B0:
    lwz r3, 0x44(r3)
    subic. r3, r3, 0x1
    bge @L_8012D8C0
    addi r3, r3, 0x14
@L_8012D8C0:
    mulli r0, r3, 0xc
    lis r3, lbl_80426BD0@ha
    li r31, 0x1
    addi r3, r3, lbl_80426BD0@l
    add r5, r3, r0
    lwz r4, 0x4c(r5)
    lwz r3, 0x50(r5)
    lwz r0, 0x54(r5)
    stw r4, 0x54(r1)
    stw r3, 0x58(r1)
    stw r0, 0x5c(r1)
@L_8012D8EC:
    lwz r3, -26224(r2)
    cmpwi r27, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x14(r1)
    stw r0, 0x18(r1)
    blt @L_8012D918
    cmpwi r27, 0x2
    bge @L_8012D918
    slwi r0, r27, 2
    addi r3, r1, 0x14
    lwzx r30, r3, r0
@L_8012D918:
    mr r4, r30
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x78
    bl fn_800E3D98
    lfs f2, 0x84(r1)
    lfs f0, 0x78(r1)
    lfs f1, 0x8c(r1)
    fsubs f4, f2, f0
    lfs f0, 0x80(r1)
    lfs f3, -26216(r2)
    fsubs f2, f1, f0
    fmuls f1, f4, f4
    stfs f4, 0x6c(r1)
    fmuls f0, f2, f2
    stfs f3, 0x70(r1)
    stfs f2, 0x74(r1)
    fadds f0, f1, f0
    fcmpo cr0, f0, f3
    ble @L_8012D9B0
    frsqrte f2, f0
    lfd f4, -26200(r2)
    lfd f3, -26192(r2)
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f0, f1, f3
    fmul f2, f2, f1
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f0, f1, f3
    fmul f2, f2, f1
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f0, f1, f3
    fmul f1, f2, f1
    fmul f0, f0, f1
    frsp f0, f0
    b @L_8012DA34
@L_8012D9B0:
    lfd f1, -26184(r2)
    fcmpo cr0, f0, f1
    bge @L_8012D9C8
    lis r3, lbl_80478AC0@ha
    lfs f0, lbl_80478AC0@l(r3)
    b @L_8012DA34
@L_8012D9C8:
    stfs f0, 0x10(r1)
    lis r0, 0x7f80
    lwz r4, 0x10(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012D9F0
    bge @L_8012DA20
    cmpwi r3, 0x0
    beq @L_8012DA08
    b @L_8012DA20
@L_8012D9F0:
    clrlwi. r0, r4, 9
    beq @L_8012DA00
    li r0, 0x1
    b @L_8012DA24
@L_8012DA00:
    li r0, 0x2
    b @L_8012DA24
@L_8012DA08:
    clrlwi. r0, r4, 9
    beq @L_8012DA18
    li r0, 0x5
    b @L_8012DA24
@L_8012DA18:
    li r0, 0x3
    b @L_8012DA24
@L_8012DA20:
    li r0, 0x4
@L_8012DA24:
    cmpwi r0, 0x1
    bne @L_8012DA34
    lis r3, lbl_80478AC0@ha
    lfs f0, lbl_80478AC0@l(r3)
@L_8012DA34:
    clrlwi. r0, r31, 24
    beq @L_8012DB40
    lfs f4, 0x78(r1)
    lfs f1, 0x54(r1)
    lfs f3, 0x80(r1)
    lfs f2, 0x5c(r1)
    fsubs f4, f4, f1
    lfs f1, -26216(r2)
    fsubs f2, f3, f2
    fmuls f3, f4, f4
    fmuls f2, f2, f2
    fadds f5, f3, f2
    fcmpo cr0, f5, f1
    ble @L_8012DAB4
    frsqrte f2, f5
    lfd f4, -26200(r2)
    lfd f3, -26192(r2)
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f5, f1, f3
    fmul f2, f2, f1
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f5, f1, f3
    fmul f2, f2, f1
    fmul f1, f2, f2
    fmul f2, f4, f2
    fnmsub f1, f5, f1, f3
    fmul f1, f2, f1
    fmul f5, f5, f1
    frsp f5, f5
    b @L_8012DB38
@L_8012DAB4:
    lfd f1, -26184(r2)
    fcmpo cr0, f5, f1
    bge @L_8012DACC
    lis r3, lbl_80478AC0@ha
    lfs f5, lbl_80478AC0@l(r3)
    b @L_8012DB38
@L_8012DACC:
    stfs f5, 0xc(r1)
    lis r0, 0x7f80
    lwz r4, 0xc(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012DAF4
    bge @L_8012DB24
    cmpwi r3, 0x0
    beq @L_8012DB0C
    b @L_8012DB24
@L_8012DAF4:
    clrlwi. r0, r4, 9
    beq @L_8012DB04
    li r0, 0x1
    b @L_8012DB28
@L_8012DB04:
    li r0, 0x2
    b @L_8012DB28
@L_8012DB0C:
    clrlwi. r0, r4, 9
    beq @L_8012DB1C
    li r0, 0x5
    b @L_8012DB28
@L_8012DB1C:
    li r0, 0x3
    b @L_8012DB28
@L_8012DB24:
    li r0, 0x4
@L_8012DB28:
    cmpwi r0, 0x1
    bne @L_8012DB38
    lis r3, lbl_80478AC0@ha
    lfs f5, lbl_80478AC0@l(r3)
@L_8012DB38:
    fmr f1, f5
    b @L_8012DB44
@L_8012DB40:
    lfs f1, -26216(r2)
@L_8012DB44:
    clrlwi. r0, r31, 24
    beq @L_8012DB6C
    fcmpo cr0, f0, f1
    cror eq, lt, eq
    beq @L_8012DB6C
    lfs f1, -26176(r2)
    fadds f1, f1, f31
    fcmpo cr0, f0, f1
    cror eq, lt, eq
    bne @L_8012DBEC
@L_8012DB6C:
    lfs f1, -26176(r2)
    fadds f1, f1, f31
    fcmpo cr0, f0, f1
    ble @L_8012DB94
    fsubs f1, f0, f31
    fcmpo cr0, f1, f30
    ble @L_8012DB8C
    fmr f1, f30
@L_8012DB8C:
    fdivs f1, f1, f0
    b @L_8012DBC4
@L_8012DB94:
    lfs f1, -26100(r2)
    fcmpo cr0, f0, f1
    bge @L_8012DBC0
    fsubs f0, f1, f0
    fcmpo cr0, f0, f30
    ble @L_8012DBB0
    fmr f0, f30
@L_8012DBB0:
    fneg f1, f0
    lfs f0, -26100(r2)
    fdivs f1, f1, f0
    b @L_8012DBC4
@L_8012DBC0:
    lfs f1, -26216(r2)
@L_8012DBC4:
    mr r4, r28
    addi r3, r1, 0x6c
    bl fn_800A3AC0
    lwz r3, 0x6c(r1)
    lwz r0, 0x70(r1)
    stw r3, 0x0(r29)
    stw r0, 0x4(r29)
    lwz r0, 0x74(r1)
    stw r0, 0x8(r29)
    b @L_8012DE70
@L_8012DBEC:
    lfs f2, 0x54(r1)
    lfs f0, 0x78(r1)
    lfs f1, 0x5c(r1)
    fsubs f4, f2, f0
    lfs f0, 0x80(r1)
    lfs f3, -26216(r2)
    fsubs f2, f1, f0
    fmuls f1, f4, f4
    stfs f4, 0x6c(r1)
    fmuls f0, f2, f2
    stfs f3, 0x70(r1)
    stfs f2, 0x74(r1)
    fadds f4, f1, f0
    fcmpo cr0, f4, f3
    ble @L_8012DC70
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
    b @L_8012DCF4
@L_8012DC70:
    lfd f0, -26184(r2)
    fcmpo cr0, f4, f0
    bge @L_8012DC88
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
    b @L_8012DCF4
@L_8012DC88:
    stfs f4, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012DCB0
    bge @L_8012DCE0
    cmpwi r3, 0x0
    beq @L_8012DCC8
    b @L_8012DCE0
@L_8012DCB0:
    clrlwi. r0, r4, 9
    beq @L_8012DCC0
    li r0, 0x1
    b @L_8012DCE4
@L_8012DCC0:
    li r0, 0x2
    b @L_8012DCE4
@L_8012DCC8:
    clrlwi. r0, r4, 9
    beq @L_8012DCD8
    li r0, 0x5
    b @L_8012DCE4
@L_8012DCD8:
    li r0, 0x3
    b @L_8012DCE4
@L_8012DCE0:
    li r0, 0x4
@L_8012DCE4:
    cmpwi r0, 0x1
    bne @L_8012DCF4
    lis r3, lbl_80478AC0@ha
    lfs f4, lbl_80478AC0@l(r3)
@L_8012DCF4:
    fcmpo cr0, f4, f30
    ble @L_8012DD10
    fdivs f1, f30, f4
    addi r3, r1, 0x6c
    addi r4, r1, 0x60
    bl fn_800A3AC0
    b @L_8012DD28
@L_8012DD10:
    lwz r4, 0x6c(r1)
    lwz r3, 0x70(r1)
    lwz r0, 0x74(r1)
    stw r4, 0x60(r1)
    stw r3, 0x64(r1)
    stw r0, 0x68(r1)
@L_8012DD28:
    addi r3, r1, 0x78
    addi r4, r1, 0x60
    addi r5, r1, 0x3c
    bl fn_800A3A78
    fmr f1, f31
    addi r3, r1, 0x78
    mr r6, r3
    addi r4, r1, 0x3c
    addi r5, r1, 0x84
    addi r7, r1, 0x48
    bl fn_8012D39C
    cmpwi r3, 0x0
    ble @L_8012DE40
    lfs f0, 0x7c(r1)
    addi r3, r1, 0x3c
    addi r4, r1, 0x78
    addi r5, r1, 0x24
    stfs f0, 0x4c(r1)
    bl fn_800A3A9C
    lfs f6, 0x24(r1)
    lfs f5, 0x28(r1)
    fmuls f1, f6, f6
    lfs f7, 0x2c(r1)
    fmuls f0, f5, f5
    lfs f30, -26216(r2)
    fmuls f2, f7, f7
    fadds f0, f1, f0
    fadds f4, f2, f0
    fcmpu cr0, f30, f4
    bne @L_8012DDBC
    lfs f2, 0x78(r1)
    lfs f1, 0x7c(r1)
    lfs f0, 0x80(r1)
    stfs f2, 0x30(r1)
    stfs f1, 0x34(r1)
    stfs f0, 0x38(r1)
    b @L_8012DE10
@L_8012DDBC:
    lfs f1, 0x4c(r1)
    addi r3, r1, 0x24
    lfs f0, 0x7c(r1)
    mr r4, r3
    lfs f2, 0x48(r1)
    fsubs f0, f1, f0
    lfs f1, 0x78(r1)
    lfs f3, 0x50(r1)
    fsubs f1, f2, f1
    lfs f2, 0x80(r1)
    fmuls f0, f5, f0
    fsubs f2, f3, f2
    fmadds f0, f6, f1, f0
    fmadds f0, f7, f2, f0
    fdivs f30, f0, f4
    fmr f1, f30
    bl fn_800A3AC0
    addi r3, r1, 0x24
    addi r4, r1, 0x78
    addi r5, r1, 0x30
    bl fn_800A3A78
@L_8012DE10:
    lfs f0, -26216(r2)
    fcmpo cr0, f30, f0
    cror eq, gt, eq
    bne @L_8012DE40
    lfs f0, -26144(r2)
    fcmpo cr0, f30, f0
    cror eq, lt, eq
    bne @L_8012DE40
    addi r3, r1, 0x48
    addi r4, r1, 0x78
    addi r5, r1, 0x60
    bl fn_800A3A9C
@L_8012DE40:
    lwz r3, 0x60(r1)
    lwz r0, 0x64(r1)
    stw r3, 0x0(r28)
    stw r0, 0x4(r28)
    lwz r0, 0x68(r1)
    stw r0, 0x8(r28)
    lwz r3, 0x60(r1)
    lwz r0, 0x64(r1)
    stw r3, 0x0(r29)
    stw r0, 0x4(r29)
    lwz r0, 0x68(r1)
    stw r0, 0x8(r29)
@L_8012DE70:
    psq_l f31, 0xc8(r1), 0, 0
    lfd f31, 0xc0(r1)
    psq_l f30, 0xb8(r1), 0, 0
    lfd f30, 0xb0(r1)
    lmw r27, 0x9c(r1)
    lwz r0, 0xd4(r1)
    mtlr r0
    addi r1, r1, 0xd0
    blr
