    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    lwz r31, lbl_8047AD88(r13)
    cmplwi r31, 0x0
    beq @L_80117CF4
    bl fn_80113F48
    lwz r4, 0x8(r31)
    bl fn_800F9318
    bl fn_800E5550
    lwz r3, lbl_8047AD8C(r13)
    cmplwi r3, 0x0
    beq @L_80117CC8
    bl fn_800EF5A4
    li r0, 0x0
    stw r0, lbl_8047AD8C(r13)
@L_80117CC8:
    lwz r3, lbl_8047AD90(r13)
    cmplwi r3, 0x0
    beq @L_80117CE0
    bl fn_800E4BF4
    li r0, 0x0
    stw r0, lbl_8047AD90(r13)
@L_80117CE0:
    li r3, 0x0
    li r0, -0x1
    stw r3, lbl_8047AD94(r13)
    stw r3, lbl_8047AD88(r13)
    stw r0, lbl_80478B40(r13)
@L_80117CF4:
    li r0, 0x0
    stw r0, lbl_8047AD80(r13)
    stw r0, lbl_8047AD84(r13)
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
