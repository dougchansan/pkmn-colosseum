    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    mr r31, r4
    lwz r30, lbl_8047A368@sda21(r0)
    lwz r3, 0x4(r3)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r29, 0x0
@L_80024A64:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80024A88
    cmpw r30, r29
    bne @L_80024A84
    mr r29, r3
    b @L_80024AA8
@L_80024A84:
    addi r29, r29, 0x1
@L_80024A88:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80024AA4
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_80024A64
@L_80024AA4:
    li r29, 0x0
@L_80024AA8:
    li r30, 0x0
    mr r28, r30
    b @L_80024AD8
@L_80024AB4:
    lwz r3, lbl_80478DDC@sda21(r0)
    addi r0, r28, 0x8
    lwzx r3, r3, r0
    bl fn_8005D934
    cmplw r29, r3
    bne @L_80024AD0
    b @L_80024AEC
@L_80024AD0:
    addi r28, r28, 0x10
    addi r30, r30, 0x1
@L_80024AD8:
    lwz r3, lbl_80478DD8@sda21(r0)
    lwz r0, 0x0(r3)
    cmplw r30, r0
    blt @L_80024AB4
    li r30, 0x0
@L_80024AEC:
    lwz r0, lbl_8047A370@sda21(r0)
    cmpwi r0, 0x1
    beq @L_80024AFC
    b @L_80024B24
@L_80024AFC:
    lfs f1, lbl_80478898@sda21(r0)
    lfs f0, lbl_8047B8A8@sda21(r0)
    lfs f2, lbl_8047B8DC@sda21(r0)
    fdivs f0, f1, f0
    fmuls f0, f2, f0
    fctiwz f0, f0
    stfd f0, 0x8(r1)
    lwz r0, 0xc(r1)
    stb r0, 0x67(r31)
    b @L_80024B2C
@L_80024B24:
    li r0, 0xff
    stb r0, 0x67(r31)
@L_80024B2C:
    lwz r3, lbl_80478DD8@sda21(r0)
    lwz r0, 0x0(r3)
    cmplw r30, r0
    bge @L_80024B84
    lwz r0, lbl_80478DDC@sda21(r0)
    slwi r28, r30, 4
    add r3, r0, r28
    lwz r0, 0x4(r3)
    cmplwi r0, 0x66
    bne @L_80024B74
    li r3, 0x45d
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80024B74
    lis r3, 0xc5f
    addi r0, r3, 0x1200
    b @L_80024B80
@L_80024B74:
    lwz r0, lbl_80478DDC@sda21(r0)
    add r3, r0, r28
    lwz r0, 0xc(r3)
@L_80024B80:
    stw r0, 0x58(r31)
@L_80024B84:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

