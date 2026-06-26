    nofralloc
    stwu r1, -0x110(r1)
    mflr r0
    stw r0, 0x114(r1)
    stfd f31, 0x100(r1)
    psq_st f31, 0x108(r1), 0, 0
    stmw r16, 0xc0(r1)
    lis r6, lbl_80426BD0@ha
    lwz r5, -26224(r2)
    addi r6, r6, lbl_80426BD0@l
    lwz r0, -26220(r2)
    lwz r6, 0x0(r6)
    mr r30, r3
    stw r5, 0x44(r1)
    cmpwi r6, 0x0
    stw r0, 0x48(r1)
    blt @L_8012DEE8
    cmpwi r6, 0x2
    bge @L_8012DEE8
    slwi r0, r6, 2
    addi r3, r1, 0x44
    lwzx r4, r3, r0
@L_8012DEE8:
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0xa8
    bl fn_800E3D98
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x3c(r1)
    stw r0, 0x40(r1)
    blt @L_8012DF24
    cmpwi r30, 0x2
    bge @L_8012DF24
    slwi r0, r30, 2
    addi r3, r1, 0x3c
    lwzx r16, r3, r0
@L_8012DF24:
    mr r4, r16
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x9c
    bl fn_800E3D98
    lfs f1, 0xb0(r1)
    lfs f0, 0xa4(r1)
    lfs f2, 0xa8(r1)
    fsubs f3, f1, f0
    lfs f1, 0x9c(r1)
    lfs f0, -26216(r2)
    fsubs f2, f2, f1
    fmuls f1, f3, f3
    fmadds f1, f2, f2, f1
    fcmpo cr0, f1, f0
    ble @L_8012DF68
    b @L_8012DF70
@L_8012DF68:
    lfd f0, -26184(r2)
    fcmpo cr0, f1, f0
@L_8012DF70:
    mr r3, r30
    addi r4, r1, 0x84
    addi r5, r1, 0x60
    bl fn_8012D7F0
    mr r3, r30
    addi r4, r1, 0x84
    addi r5, r1, 0x60
    bl fn_8012CA84
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x34(r1)
    stw r0, 0x38(r1)
    blt @L_8012DFBC
    cmpwi r30, 0x2
    bge @L_8012DFBC
    slwi r0, r30, 2
    addi r3, r1, 0x34
    lwzx r18, r3, r0
@L_8012DFBC:
    mr r4, r18
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x90
    bl fn_800E3D98
    lfs f3, 0xa8(r1)
    lfs f0, 0x90(r1)
    lfs f2, 0xb0(r1)
    lfs f1, 0x98(r1)
    fsubs f3, f3, f0
    lfs f0, -26216(r2)
    fsubs f1, f2, f1
    fmuls f2, f3, f3
    fmuls f1, f1, f1
    fadds f31, f2, f1
    fcmpo cr0, f31, f0
    ble @L_8012E048
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
    b @L_8012E0CC
@L_8012E048:
    lfd f0, -26184(r2)
    fcmpo cr0, f31, f0
    bge @L_8012E060
    lis r3, lbl_80478AC0@ha
    lfs f31, lbl_80478AC0@l(r3)
    b @L_8012E0CC
@L_8012E060:
    stfs f31, 0x8(r1)
    lis r0, 0x7f80
    lwz r4, 0x8(r1)
    rlwinm r3, r4, 0, 1, 8
    cmpw r3, r0
    beq @L_8012E088
    bge @L_8012E0B8
    cmpwi r3, 0x0
    beq @L_8012E0A0
    b @L_8012E0B8
@L_8012E088:
    clrlwi. r0, r4, 9
    beq @L_8012E098
    li r0, 0x1
    b @L_8012E0BC
@L_8012E098:
    li r0, 0x2
    b @L_8012E0BC
@L_8012E0A0:
    clrlwi. r0, r4, 9
    beq @L_8012E0B0
    li r0, 0x5
    b @L_8012E0BC
@L_8012E0B0:
    li r0, 0x3
    b @L_8012E0BC
@L_8012E0B8:
    li r0, 0x4
@L_8012E0BC:
    cmpwi r0, 0x1
    bne @L_8012E0CC
    lis r3, lbl_80478AC0@ha
    lfs f31, lbl_80478AC0@l(r3)
@L_8012E0CC:
    addi r3, r1, 0x90
    addi r4, r1, 0x9c
    addi r5, r1, 0x78
    bl fn_800A3A9C
    lfs f0, -26096(r2)
    fcmpo cr0, f31, f0
    bge @L_8012E104
    lis r3, lbl_80426BD0@ha
    slwi r0, r30, 5
    addi r3, r3, lbl_80426BD0@l
    li r4, 0x12c
    add r3, r3, r0
    stw r4, 0x10(r3)
    b @L_8012E36C
@L_8012E104:
    bl fn_800D3088
    lis r4, lbl_80426BD0@ha
    slwi r0, r30, 5
    addi r4, r4, lbl_80426BD0@l
    add r31, r4, r0
    lwz r0, 0x10(r31)
    subf. r0, r3, r0
    stwu r0, 0x10(r31)
    bgt @L_8012E36C
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x2c(r1)
    stw r0, 0x30(r1)
    blt @L_8012E154
    cmpwi r30, 0x2
    bge @L_8012E154
    slwi r0, r30, 2
    addi r3, r1, 0x2c
    lwzx r19, r3, r0
@L_8012E154:
    mr r4, r19
    li r3, 0x0
    bl fn_800F9318
    cmplwi r3, 0x0
    bne @L_8012E170
    li r3, 0x0
    b @L_8012E174
@L_8012E170:
    bl fn_800E3C64
@L_8012E174:
    clrlwi. r0, r3, 24
    bne @L_8012E36C
    lis r3, lbl_80426BD0@ha
    slwi r20, r30, 2
    addi r24, r1, 0x24
    addi r23, r1, 0xc
    addi r22, r1, 0x1c
    addi r21, r1, 0x14
    addi r29, r3, lbl_80426BD0@l
    li r18, 0x0
    li r19, 0x0
    b @L_8012E2B0
@L_8012E1A4:
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x24(r1)
    stw r0, 0x28(r1)
    blt @L_8012E1C8
    cmpwi r30, 0x2
    bge @L_8012E1C8
    lwzx r28, r24, r20
@L_8012E1C8:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    mr. r16, r3
    bne @L_8012E1E4
    li r16, 0x0
    b @L_8012E2A0
@L_8012E1E4:
    addic. r0, r1, 0x6c
    beq @L_8012E258
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0xc(r1)
    stw r0, 0x10(r1)
    blt @L_8012E210
    cmpwi r30, 0x2
    bge @L_8012E210
    lwzx r25, r23, r20
@L_8012E210:
    mr r4, r25
    li r3, 0x0
    bl fn_800F9318
    addi r4, r1, 0x54
    bl fn_800E3D98
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x1c(r1)
    stw r0, 0x20(r1)
    blt @L_8012E248
    cmpwi r30, 0x2
    bge @L_8012E248
    lwzx r27, r22, r20
@L_8012E248:
    mr r4, r27
    addi r5, r1, 0x6c
    li r3, 0x0
    bl fn_8018C0A8
@L_8012E258:
    mr r3, r16
    bl fn_800E3C64
    addic. r0, r1, 0x6c
    mr r16, r3
    beq @L_8012E2A0
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x14(r1)
    stw r0, 0x18(r1)
    blt @L_8012E290
    cmpwi r30, 0x2
    bge @L_8012E290
    lwzx r26, r21, r20
@L_8012E290:
    mr r4, r26
    addi r5, r1, 0x54
    li r3, 0x0
    bl fn_8018C0A8
@L_8012E2A0:
    clrlwi. r0, r16, 24
    bne @L_8012E2AC
    li r18, 0x1
@L_8012E2AC:
    addi r19, r19, 0x1
@L_8012E2B0:
    lwz r0, 0x48(r29)
    cmpw r19, r0
    blt @L_8012E2C4
    li r6, 0x0
    b @L_8012E314
@L_8012E2C4:
    cmpwi r19, 0x14
    blt @L_8012E2D4
    li r6, 0x0
    b @L_8012E314
@L_8012E2D4:
    lwz r0, 0x44(r29)
    subf r3, r19, r0
    subic. r3, r3, 0x1
    bge @L_8012E2E8
    addi r3, r3, 0x14
@L_8012E2E8:
    mulli r4, r3, 0xc
    lis r3, lbl_80426BD0@ha
    li r6, 0x1
    addi r0, r3, lbl_80426BD0@l
    add r5, r0, r4
    lwz r4, 0x4c(r5)
    lwz r3, 0x50(r5)
    lwz r0, 0x54(r5)
    stw r4, 0x6c(r1)
    stw r3, 0x70(r1)
    stw r0, 0x74(r1)
@L_8012E314:
    clrlwi. r0, r6, 24
    beq @L_8012E324
    clrlwi. r0, r18, 24
    beq @L_8012E1A4
@L_8012E324:
    clrlwi. r0, r18, 24
    beq @L_8012E36C
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x4c(r1)
    stw r0, 0x50(r1)
    blt @L_8012E354
    cmpwi r30, 0x2
    bge @L_8012E354
    addi r3, r1, 0x4c
    lwzx r17, r3, r20
@L_8012E354:
    mr r4, r17
    addi r5, r1, 0x6c
    li r3, 0x0
    bl fn_8018C0A8
    li r0, 0x12c
    stw r0, 0x0(r31)
@L_8012E36C:
    psq_l f31, 0x108(r1), 0, 0
    lfd f31, 0x100(r1)
    lmw r16, 0xc0(r1)
    lwz r0, 0x114(r1)
    mtlr r0
    addi r1, r1, 0x110
    blr
