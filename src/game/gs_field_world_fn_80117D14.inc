    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    lwz r0, lbl_8047AD88(r13)
    cmplwi r0, 0x0
    beq @L_80117E44
    lwz r3, lbl_8047AD90(r13)
    cmplwi r3, 0x0
    bne @L_80117D40
    b @L_80117E44
@L_80117D40:
    bl fn_800EC134
    bl GScameraGetActiveCamera
    mr r0, r3
    li r3, 0x2
    mr r31, r0
    bl fn_800D4604
    li r3, 0x1
    bl fn_800D377C
    lwz r3, lbl_8047AD8C(r13)
    li r4, 0x0
    bl fn_800D3410
    addi r3, r1, 0x16
    addi r4, r1, 0x14
    addi r5, r1, 0x12
    addi r6, r1, 0x10
    bl fn_800D9B24
    addi r3, r1, 0xe
    addi r4, r1, 0xc
    addi r5, r1, 0xa
    addi r6, r1, 0x8
    bl fn_800D9AF0
    lwz r3, lbl_8047AD94(r13)
    bl fn_800D258C
    lwz r5, lbl_8047AD88(r13)
    li r3, 0x0
    li r4, 0x0
    lhz r6, 0x0(r5)
    lhz r5, 0x2(r5)
    subi r6, r6, 0x1
    subi r0, r5, 0x1
    clrlwi r5, r6, 16
    clrlwi r6, r0, 16
    bl fn_800D9D68
    lwz r5, lbl_8047AD88(r13)
    li r3, 0x0
    li r4, 0x0
    lhz r6, 0x0(r5)
    lhz r5, 0x2(r5)
    subi r6, r6, 0x1
    subi r0, r5, 0x1
    clrlwi r5, r6, 16
    clrlwi r6, r0, 16
    bl fn_800D9C24
    bl fn_800D2248
    lwz r3, lbl_8047AD90(r13)
    li r4, 0x3010
    bl fn_800E3760
    bl fn_800D3190
    li r3, 0x1
    bl fn_800D377C
    mr r3, r31
    bl fn_800D258C
    lhz r3, 0x16(r1)
    lhz r4, 0x14(r1)
    lhz r5, 0x12(r1)
    lhz r6, 0x10(r1)
    bl fn_800D9D68
    lhz r3, 0xe(r1)
    lhz r4, 0xc(r1)
    lhz r5, 0xa(r1)
    lhz r6, 0x8(r1)
    bl fn_800D9C24
    bl fn_800D2248
    li r3, 0x1
    bl fn_800D4604
@L_80117E44:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
