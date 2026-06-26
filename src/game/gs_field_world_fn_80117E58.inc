    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    lwz r4, 0x48(r3)
    cmplwi r4, 0x0
    beq @L_8011800C
    lwz r4, 0x0(r4)
    lwz r0, 0x4(r4)
    cmplwi r0, 0x0
    stw r0, lbl_8047AD80(r13)
    beq @L_8011800C
    lwz r3, 0x48(r3)
    lwz r3, 0x0(r3)
    lwz r3, 0x0(r3)
    lwz r0, 0x0(r3)
    cmplwi r0, 0x0
    stw r0, lbl_8047AD84(r13)
    beq @L_8011800C
    lwz r0, lbl_80478B40(r13)
    cmpwi r0, 0x0
    beq @L_8011800C
    lwz r31, lbl_8047AD88(r13)
    cmplwi r31, 0x0
    beq @L_80117F0C
    bl fn_80113F48
    lwz r4, 0x8(r31)
    bl fn_800F9318
    bl fn_800E5550
    lwz r3, lbl_8047AD8C(r13)
    cmplwi r3, 0x0
    beq @L_80117EE4
    bl fn_800EF5A4
    li r0, 0x0
    stw r0, lbl_8047AD8C(r13)
@L_80117EE4:
    lwz r3, lbl_8047AD90(r13)
    cmplwi r3, 0x0
    beq @L_80117EFC
    bl fn_800E4BF4
    li r0, 0x0
    stw r0, lbl_8047AD90(r13)
@L_80117EFC:
    li r3, 0x0
    li r0, -0x1
    stw r3, lbl_8047AD94(r13)
    stw r0, lbl_80478B40(r13)
@L_80117F0C:
    lwz r0, lbl_8047AD80(r13)
    li r4, 0x0
    lwz r3, lbl_8047AD84(r13)
    stw r0, lbl_8047AD88(r13)
    mtctr r3
    cmplwi r3, 0x0
    beq @L_80117F4C
@L_80117F28:
    lwz r3, lbl_8047AD88(r13)
    lwz r0, 0x4(r3)
    cmplwi r0, 0x0
    bne @L_80117F40
    li r4, 0x1
    b @L_80117F4C
@L_80117F40:
    addi r0, r3, 0x18
    stw r0, lbl_8047AD88(r13)
    bdnz @L_80117F28
@L_80117F4C:
    clrlwi r0, r4, 24
    cmplwi r0, 0x0
    bne @L_80117F64
    li r0, 0x0
    stw r0, lbl_8047AD88(r13)
    b @L_8011800C
@L_80117F64:
    lwz r4, lbl_8047AD88(r13)
    li r5, 0x44
    li r6, 0x0
    li r7, 0x0
    lhz r3, 0x0(r4)
    lhz r4, 0x2(r4)
    bl GStextureCreate
    cmplwi r3, 0x0
    stw r3, lbl_8047AD8C(r13)
    bne @L_80117F98
    li r0, 0x0
    stw r0, lbl_8047AD88(r13)
    b @L_8011800C
@L_80117F98:
    bl fn_80113F48
    lwz r4, lbl_8047AD88(r13)
    lwz r4, 0xc(r4)
    bl fn_80113D34
    stw r3, lbl_8047AD90(r13)
    li r4, 0x0
    bl fn_800E4014
    lwz r3, lbl_8047AD90(r13)
    li r4, 0x1
    bl fn_800EC188
    lwz r4, lbl_8047AD88(r13)
    lwz r3, lbl_8047AD90(r13)
    lwz r4, 0x10(r4)
    bl fn_800ECCA8
    lwz r3, lbl_8047AD90(r13)
    bl fn_800EC990
    bl fn_80113F48
    lwz r4, lbl_8047AD88(r13)
    lwz r4, 0x14(r4)
    bl fn_800F9318
    stw r3, lbl_8047AD94(r13)
    bl fn_80113F48
    lwz r4, lbl_8047AD88(r13)
    lwz r4, 0x8(r4)
    bl fn_800F9318
    lwz r4, lbl_8047AD8C(r13)
    bl fn_800E563C
    li r0, 0x0
    stw r0, lbl_80478B40(r13)
@L_8011800C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
