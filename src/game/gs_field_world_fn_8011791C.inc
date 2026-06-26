    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stw r31, 0x2c(r1)
    stw r30, 0x28(r1)
    stw r29, 0x24(r1)
    lis r3, lbl_804083D0@ha
    addi r3, r3, lbl_804083D0@l
    lwz r29, 0x0(r3)
    bl fn_800FF56C
    bl fn_80115C48
    lis r4, lbl_804083D0@ha
    mr r30, r3
    addi r31, r4, lbl_804083D0@l
    lwz r3, 0x10(r31)
    bl fn_80115C48
    cmplwi r30, 0x0
    beq @L_8011796C
    cmplw r30, r3
    beq @L_80117A20
@L_8011796C:
    lwz r3, 0x10(r31)
    lis r4, lbl_804083D0@ha
    addi r4, r4, lbl_804083D0@l
    li r0, 0x0
    cmplwi r3, 0x0
    stb r0, 0x1c(r4)
    beq @L_801179A8
    bl fn_80115C48
    cmplwi r3, 0x0
    beq @L_801179A8
    lis r4, lbl_804083D0@ha
    addi r4, r4, lbl_804083D0@l
    lwz r0, 0x14(r4)
    stw r0, 0x1c(r3)
    bl fn_80117164
@L_801179A8:
    lis r3, lbl_804083D0@ha
    addi r3, r3, lbl_804083D0@l
    lwz r3, 0x18(r3)
    cmplwi r3, 0x0
    beq @L_801179C0
    bl fn_800FE714
@L_801179C0:
    lis r3, lbl_804083D0@ha
    addi r3, r3, lbl_804083D0@l
    lhz r30, 0xe(r3)
    cmplwi r30, 0x0
    beq @L_801179E4
    mr r3, r30
    bl fn_800E24B0
    mr r3, r30
    bl fn_800E209C
@L_801179E4:
    lis r3, lbl_804083D0@ha
    addi r3, r3, lbl_804083D0@l
    lhz r30, 0xc(r3)
    cmplwi r30, 0x0
    beq @L_80117A08
    mr r3, r30
    bl fn_800E24B0
    mr r3, r30
    bl fn_800E209C
@L_80117A08:
    lis r3, lbl_804083D0@ha
    li r4, 0x0
    addi r3, r3, lbl_804083D0@l
    li r5, 0x20
    bl memset
    b @L_80117AB8
@L_80117A20:
    lis r3, lbl_804083D0@ha
    addi r3, r3, lbl_804083D0@l
    lbz r0, 0x1c(r3)
    cmplwi r0, 0x0
    bne @L_80117AB8
    cmplwi r29, 0x0
    beq @L_80117AB8
    lis r3, lbl_802727B8@ha
    addi r5, r3, lbl_802727B8@l
    lwz r4, 0x0(r5)
    lwz r3, 0x4(r5)
    lwz r0, 0x8(r5)
    stw r4, 0x8(r1)
    stw r3, 0xc(r1)
    stw r0, 0x10(r1)
    bl GSscene_GetMode
    clrlwi r0, r3, 24
    cmplwi r0, 0x5
    bne @L_80117AB8
    bl GScameraGetActiveCamera
    mr r4, r3
    cmplwi r4, 0x0
    beq @L_80117AB8
    addi r3, r1, 0x8
    addi r4, r4, 0x100
    bl fn_800E01D0
    lfs f0, 0x8(r1)
    stfs f0, 0x0(r29)
    lfs f0, 0xc(r1)
    stfs f0, 0x4(r29)
    lfs f0, 0x10(r1)
    stfs f0, 0x8(r29)
    bl fn_80176690
    stfs f1, 0xc(r29)
    bl fn_8017669C
    stfs f1, 0x10(r29)
    bl fn_80176684
    stfs f1, 0x14(r29)
@L_80117AB8:
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    lwz r29, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
