    nofralloc
    stwu r1, -0x50(r1)
    mflr r0
    stw r0, 0x54(r1)
    stmw r27, 0x3c(r1)
    mr r30, r4
    lwz r0, lbl_8047A370@sda21(r0)
    cmpwi r0, 0x1
    beq @L_80024DE8
    bge @L_80024E2C
    cmpwi r0, 0x0
    b @L_80024E2C
@L_80024DE8:
    lfs f1, lbl_80478898@sda21(r0)
    bl fn_800E0CA0
    lis r3, lbl_803A2058@ha
    lis r5, lbl_803A204C@ha
    addi r4, r3, lbl_803A2058@l
    addi r3, r1, 0x8
    addi r5, r5, lbl_803A204C@l
    bl fn_800E090C
    lfs f1, 0x8(r1)
    lfs f0, 0xc(r1)
    fctiwz f1, f1
    fctiwz f0, f0
    stfd f1, 0x18(r1)
    stfd f0, 0x20(r1)
    lwz r28, 0x1c(r1)
    lwz r31, 0x24(r1)
    b @L_80024EA4
@L_80024E2C:
    lwz r4, lbl_80478DD8@sda21(r0)
    lwz r27, lbl_8047A368@sda21(r0)
    lwz r0, 0x0(r4)
    cmplw r27, r0
    bge @L_80024EA4
    lwz r3, 0x4(r3)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r29, 0x0
@L_80024E54:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80024E74
    cmpw r27, r29
    bne @L_80024E70
    b @L_80024E94
@L_80024E70:
    addi r29, r29, 0x1
@L_80024E74:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80024E90
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_80024E54
@L_80024E90:
    li r3, 0x0
@L_80024E94:
    cmplwi r3, 0x0
    beq @L_80024EA4
    lha r28, 0x2(r3)
    lha r31, 0x4(r3)
@L_80024EA4:
    extsh r0, r28
    lis r3, 0x4330
    xoris r4, r0, 0x8000
    extsh r0, r31
    stw r4, 0x24(r1)
    xoris r0, r0, 0x8000
    lfd f3, lbl_8047B8B8@sda21(r0)
    stw r3, 0x20(r1)
    lfs f0, lbl_8047A374@sda21(r0)
    lfd f1, 0x20(r1)
    lfs f4, lbl_8047B8E0@sda21(r0)
    fsubs f1, f1, f3
    stw r0, 0x2c(r1)
    stw r3, 0x28(r1)
    fadds f1, f1, f0
    lfd f0, 0x28(r1)
    fadds f2, f4, f1
    fsubs f1, f0, f3
    fctiwz f0, f2
    stfd f0, 0x18(r1)
    lwz r0, 0x1c(r1)
    sth r0, 0x50(r30)
    lfs f0, lbl_8047A374@sda21(r0)
    fadds f0, f1, f0
    fadds f0, f4, f0
    fctiwz f0, f0
    stfd f0, 0x30(r1)
    lwz r0, 0x34(r1)
    sth r0, 0x52(r30)
    lmw r27, 0x3c(r1)
    lwz r0, 0x54(r1)
    mtlr r0
    addi r1, r1, 0x50
    blr

