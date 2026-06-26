    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r23, 0xc(r1)
    mr r26, r3
    bl fn_80115C48
    lis r4, lbl_804083D0@ha
    li r0, 0x0
    addi r28, r4, lbl_804083D0@l
    mr r27, r3
    lwz r4, 0x10(r28)
    stb r0, 0x1c(r28)
    cmplwi r4, 0x0
    beq @L_80117724
    mr r3, r4
    bl fn_80115C48
    cmplwi r3, 0x0
    beq @L_80117724
    lis r4, lbl_804083D0@ha
    addi r4, r4, lbl_804083D0@l
    lwz r0, 0x14(r4)
    stw r0, 0x1c(r3)
    bl fn_80117164
@L_80117724:
    lis r3, lbl_804083D0@ha
    addi r29, r3, lbl_804083D0@l
    lwz r3, 0x18(r29)
    cmplwi r3, 0x0
    beq @L_8011773C
    bl fn_800FE714
@L_8011773C:
    lis r3, lbl_804083D0@ha
    addi r30, r3, lbl_804083D0@l
    lhz r25, 0xe(r30)
    cmplwi r25, 0x0
    beq @L_80117760
    mr r3, r25
    bl fn_800E24B0
    mr r3, r25
    bl fn_800E209C
@L_80117760:
    lis r3, lbl_804083D0@ha
    addi r31, r3, lbl_804083D0@l
    lhz r24, 0xc(r31)
    cmplwi r24, 0x0
    beq @L_80117784
    mr r3, r24
    bl fn_800E24B0
    mr r3, r24
    bl fn_800E209C
@L_80117784:
    lis r3, lbl_804083D0@ha
    li r4, 0x0
    addi r3, r3, lbl_804083D0@l
    li r5, 0x20
    bl memset
    cmplwi r27, 0x0
    beq @L_80117878
    lis r4, fn_8011791C@ha
    li r3, 0x1
    addi r6, r4, fn_8011791C@l
    li r5, 0x0
    li r4, 0x7f
    bl fn_800FE834
    cmplwi r3, 0x0
    stw r3, 0x18(r29)
    beq @L_80117878
    li r3, 0x8
    bl fn_800E3534
    mr r25, r3
    clrlwi r0, r25, 16
    cmplwi r0, 0x0
    beq @L_80117878
    bl fn_800E27B0
    lis r4, lbl_804083D0@ha
    sth r25, 0xc(r31)
    addi r25, r4, lbl_804083D0@l
    stw r3, 0x4(r25)
    mr r3, r27
    bl fn_801155CC
    stw r26, 0x10(r28)
    lis r4, lbl_804083D0@ha
    addi r4, r4, lbl_804083D0@l
    mr r26, r3
    lwz r0, 0x1c(r27)
    cmplwi r26, 0x0
    lwz r3, 0x4(r25)
    addi r27, r4, 0x8
    stw r0, 0x14(r4)
    stw r27, 0x0(r3)
    beq @L_80117908
    lwz r3, 0x0(r26)
    lwz r23, 0x0(r3)
    cmplwi r23, 0x0
    beq @L_80117908
    mulli r24, r23, 0x18
    mr r3, r24
    bl fn_800E3534
    clrlwi r0, r3, 16
    cmplwi r0, 0x0
    beq @L_80117878
    sth r3, 0xe(r30)
    stw r23, 0x0(r27)
    bl fn_800E27B0
    lwz r4, 0x4(r25)
    mr r5, r24
    stw r3, 0x4(r4)
    lwz r3, 0x4(r25)
    lwz r4, 0x4(r26)
    lwz r3, 0x4(r3)
    bl memcpy
    b @L_80117908
@L_80117878:
    lwz r3, 0x10(r28)
    li r0, 0x0
    stb r0, 0x1c(r28)
    cmplwi r3, 0x0
    beq @L_801178AC
    bl fn_80115C48
    cmplwi r3, 0x0
    beq @L_801178AC
    lis r4, lbl_804083D0@ha
    addi r4, r4, lbl_804083D0@l
    lwz r0, 0x14(r4)
    stw r0, 0x1c(r3)
    bl fn_80117164
@L_801178AC:
    lwz r3, 0x18(r29)
    cmplwi r3, 0x0
    beq @L_801178BC
    bl fn_800FE714
@L_801178BC:
    lhz r25, 0xe(r30)
    cmplwi r25, 0x0
    beq @L_801178D8
    mr r3, r25
    bl fn_800E24B0
    mr r3, r25
    bl fn_800E209C
@L_801178D8:
    lhz r24, 0xc(r31)
    cmplwi r24, 0x0
    beq @L_801178F4
    mr r3, r24
    bl fn_800E24B0
    mr r3, r24
    bl fn_800E209C
@L_801178F4:
    lis r3, lbl_804083D0@ha
    li r4, 0x0
    addi r3, r3, lbl_804083D0@l
    li r5, 0x20
    bl memset
@L_80117908:
    lmw r23, 0xc(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
