    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stw r31, 0x3c(r1)
    stw r30, 0x38(r1)
    stw r29, 0x34(r1)
    mr r31, r3
    lwz r0, lbl_8047A370@sda21(r0)
    cmpwi r0, 0x1
    bne @L_80023E90
    li r3, 0x0
    b @L_80024144
@L_80023E90:
    lwz r3, 0x4(r31)
    bl fn_8005DA18
    mr r0, r3
    mr r3, r31
    mr r29, r0
    bl fn_80104318
    mr r30, r3
    cmplwi r30, 0x0
    bne @L_80023EC4
    li r0, 0x0
    li r3, 0x0
    stb r0, 0x95(r31)
    b @L_80024144
@L_80023EC4:
    bl fn_80105624
    mr r0, r3
    mr r3, r31
    mr r5, r0
    mr r4, r30
    mr r6, r29
    bl fn_80024160
    lbz r3, 0x95(r31)
    lbz r0, 0x94(r31)
    extsb r3, r3
    lwz r29, lbl_8047A368@sda21(r0)
    extsb r0, r0
    add r0, r3, r0
    cmplw r29, r0
    stw r0, lbl_8047A36C@sda21(r0)
    beq @L_80024140
    lfs f0, lbl_8047B8A8@sda21(r0)
    li r0, 0x1
    stw r0, lbl_8047A370@sda21(r0)
    stfs f0, lbl_80478898@sda21(r0)
    lwz r3, 0x4(r31)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r30, 0x0
@L_80023F28:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80023F48
    cmpw r29, r30
    bne @L_80023F44
    b @L_80023F68
@L_80023F44:
    addi r30, r30, 0x1
@L_80023F48:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80023F64
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_80023F28
@L_80023F64:
    li r3, 0x0
@L_80023F68:
    cmplwi r3, 0x0
    beq @L_80023FC0
    lha r5, 0x2(r3)
    lis r0, 0x4330
    lis r4, lbl_803A2058@ha
    stw r0, 0x20(r1)
    xoris r5, r5, 0x8000
    lfd f2, lbl_8047B8B8@sda21(r0)
    stw r5, 0x24(r1)
    addi r4, r4, lbl_803A2058@l
    lfs f0, lbl_8047B8AC@sda21(r0)
    lfd f1, 0x20(r1)
    stw r0, 0x28(r1)
    fsubs f1, f1, f2
    stfs f1, 0x0(r4)
    lha r0, 0x4(r3)
    xoris r0, r0, 0x8000
    stfs f0, 0x8(r4)
    stw r0, 0x2c(r1)
    lfd f0, 0x28(r1)
    fsubs f0, f0, f2
    stfs f0, 0x4(r4)
@L_80023FC0:
    lwz r29, lbl_8047A36C@sda21(r0)
    lwz r3, 0x4(r31)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r30, 0x0
@L_80023FD8:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80023FF8
    cmpw r29, r30
    bne @L_80023FF4
    b @L_80024018
@L_80023FF4:
    addi r30, r30, 0x1
@L_80023FF8:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80024014
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_80023FD8
@L_80024014:
    li r3, 0x0
@L_80024018:
    cmplwi r3, 0x0
    beq @L_80024070
    lha r5, 0x2(r3)
    lis r0, 0x4330
    lis r4, lbl_803A204C@ha
    stw r0, 0x28(r1)
    xoris r5, r5, 0x8000
    lfd f2, lbl_8047B8B8@sda21(r0)
    stw r5, 0x2c(r1)
    addi r4, r4, lbl_803A204C@l
    lfs f0, lbl_8047B8AC@sda21(r0)
    lfd f1, 0x28(r1)
    stw r0, 0x20(r1)
    fsubs f1, f1, f2
    stfs f1, 0x0(r4)
    lha r0, 0x4(r3)
    xoris r0, r0, 0x8000
    stfs f0, 0x8(r4)
    stw r0, 0x24(r1)
    lfd f0, 0x20(r1)
    fsubs f0, f0, f2
    stfs f0, 0x4(r4)
@L_80024070:
    lwz r5, lbl_8047A390@sda21(r0)
    lis r0, 0x4330
    lis r4, lbl_803A204C@ha
    stw r0, 0x28(r1)
    lha r3, 0x2(r5)
    addi r4, r4, lbl_803A204C@l
    lfd f3, lbl_8047B8B8@sda21(r0)
    xoris r3, r3, 0x8000
    lfs f4, 0x0(r4)
    stw r3, 0x2c(r1)
    lfs f2, 0x4(r4)
    lfd f0, 0x28(r1)
    stw r0, 0x20(r1)
    fsubs f1, f0, f3
    lfs f0, lbl_8047B8AC@sda21(r0)
    fsubs f1, f4, f1
    stfs f1, 0x14(r1)
    fcmpu cr0, f0, f1
    lha r0, 0x4(r5)
    xoris r0, r0, 0x8000
    stfs f0, 0x1c(r1)
    stw r0, 0x24(r1)
    lfd f1, 0x20(r1)
    fsubs f1, f1, f3
    fsubs f1, f2, f1
    stfs f1, 0x18(r1)
    bne @L_800240E4
    fcmpu cr0, f0, f1
    beq @L_80024130
@L_800240E4:
    addi r3, r1, 0x8
    addi r4, r1, 0x14
    bl fn_800E0060
    lfs f0, lbl_8047B8AC@sda21(r0)
    addi r3, r1, 0x14
    lfs f1, lbl_8047B8B0@sda21(r0)
    addi r4, r1, 0x8
    stfs f0, 0x18(r1)
    stfs f1, 0x14(r1)
    stfs f0, 0x1c(r1)
    bl fn_800E0000
    lfs f0, lbl_8047B8AC@sda21(r0)
    fcmpo cr0, f1, f0
    bge @L_80024128
    li r0, 0x1
    stb r0, lbl_8047A380@sda21(r0)
    b @L_80024130
@L_80024128:
    li r0, 0x0
    stb r0, lbl_8047A380@sda21(r0)
@L_80024130:
    li r3, 0x464
    li r4, 0x0
    li r5, 0xff
    bl fn_80165A20
@L_80024140:
    li r3, 0x0
@L_80024144:
    lwz r0, 0x44(r1)
    lwz r31, 0x3c(r1)
    lwz r30, 0x38(r1)
    lwz r29, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr

