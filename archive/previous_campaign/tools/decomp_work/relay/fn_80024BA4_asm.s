    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    mr r31, r4
    lwz r30, lbl_8047A36C@sda21(r0)
    lwz r3, 0x4(r3)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r29, 0x0
@L_80024BDC:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80024C00
    cmpw r30, r29
    bne @L_80024BFC
    mr r29, r3
    b @L_80024C20
@L_80024BFC:
    addi r29, r29, 0x1
@L_80024C00:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80024C1C
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_80024BDC
@L_80024C1C:
    li r29, 0x0
@L_80024C20:
    li r30, 0x0
    mr r28, r30
    b @L_80024C50
@L_80024C2C:
    lwz r3, lbl_80478DDC@sda21(r0)
    addi r0, r28, 0x8
    lwzx r3, r3, r0
    bl fn_8005D934
    cmplw r29, r3
    bne @L_80024C48
    b @L_80024C64
@L_80024C48:
    addi r28, r28, 0x10
    addi r30, r30, 0x1
@L_80024C50:
    lwz r3, lbl_80478DD8@sda21(r0)
    lwz r0, 0x0(r3)
    cmplw r30, r0
    blt @L_80024C2C
    li r30, 0x0
@L_80024C64:
    lwz r3, lbl_80478DD8@sda21(r0)
    lwz r0, 0x0(r3)
    cmplw r30, r0
    bge @L_80024CBC
    lwz r0, lbl_80478DDC@sda21(r0)
    slwi r28, r30, 4
    add r3, r0, r28
    lwz r0, 0x4(r3)
    cmplwi r0, 0x66
    bne @L_80024CAC
    li r3, 0x45d
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80024CAC
    lis r3, 0xc5f
    addi r0, r3, 0x1200
    b @L_80024CB8
@L_80024CAC:
    lwz r0, lbl_80478DDC@sda21(r0)
    add r3, r0, r28
    lwz r0, 0xc(r3)
@L_80024CB8:
    stw r0, 0x58(r31)
@L_80024CBC:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

