    nofralloc
    stwu r1, -0x70(r1)
    mflr r0
    lis r3, lbl_80426BD0@ha
    stw r0, 0x74(r1)
    addi r4, r3, lbl_80426BD0@l
    stmw r21, 0x44(r1)
    li r26, 0x0
    li r25, 0x0
    li r30, 0x0
    lwz r3, 0x180(r4)
    addi r0, r3, 0x1
    cmpwi r0, 0x4
    stw r0, 0x180(r4)
    blt @L_8012B170
    li r0, 0x0
    addi r27, r1, 0x28
    stw r0, 0x180(r4)
    mr r23, r27
    li r24, 0x0
@L_8012AD9C:
    clrlwi r5, r24, 16
    li r3, 0x0
    li r4, 0x3
    bl fn_8012A5B0
    mr. r22, r3
    beq @L_8012AE44
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    beq @L_8012AE44
    mr r3, r22
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r21, r3, 16
    beq @L_8012AE44
    mr r3, r22
    li r4, 0x3
    bl fn_80121ADC
    clrlwi. r0, r3, 24
    bne @L_8012AE04
    mr r3, r22
    li r4, 0x4
    bl fn_80121ADC
    clrlwi. r0, r3, 24
    beq @L_8012AE38
@L_8012AE04:
    subi r21, r21, 0x1
    mr r3, r22
    clrlwi r7, r21, 16
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_801254B4
    clrlwi. r0, r21, 16
    li r30, 0x1
    bne @L_8012AE38
    stw r24, 0x0(r23)
    addi r23, r23, 0x4
    addi r25, r25, 0x1
@L_8012AE38:
    clrlwi. r0, r21, 16
    beq @L_8012AE44
    addi r26, r26, 0x1
@L_8012AE44:
    addi r24, r24, 0x1
    cmpwi r24, 0x6
    blt @L_8012AD9C
    clrlwi. r0, r30, 24
    beq @L_8012AE5C
    bl fn_801C409C
@L_8012AE5C:
    cmpwi r25, 0x0
    ble @L_8012B170
    li r30, 0x0
    li r24, 0x0
    mr r23, r30
@L_8012AE70:
    cmpwi r30, 0x0
    blt @L_8012AE80
    cmpwi r30, 0x2
    blt @L_8012AE88
@L_8012AE80:
    li r0, 0x0
    b @L_8012AEAC
@L_8012AE88:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r23
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012AEA8
    li r0, 0x0
    b @L_8012AEAC
@L_8012AEA8:
    li r0, 0x1
@L_8012AEAC:
    clrlwi. r0, r0, 24
    beq @L_8012AF68
    lwz r3, -26224(r2)
    cmpwi r30, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x8(r1)
    stw r0, 0xc(r1)
    stw r3, 0x20(r1)
    stw r0, 0x24(r1)
    blt @L_8012AEE4
    cmpwi r30, 0x2
    bge @L_8012AEE4
    addi r3, r1, 0x20
    lwzx r31, r3, r24
@L_8012AEE4:
    lwz r3, 0x8(r1)
    cmpwi r30, 0x0
    lwz r0, 0xc(r1)
    stw r3, 0x10(r1)
    stw r0, 0x14(r1)
    blt @L_8012AF0C
    cmpwi r30, 0x2
    bge @L_8012AF0C
    addi r3, r1, 0x10
    lwzx r28, r3, r24
@L_8012AF0C:
    mr r4, r28
    li r3, 0x0
    bl fn_800F9318
    lfs f1, -26216(r2)
    mr r4, r30
    bl fn_8012C660
    lis r3, 0x8000
    mr r4, r31
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C7C8
    mr r4, r31
    li r3, 0x0
    li r5, 0x100
    bl fn_8018C69C
    mr r4, r31
    li r3, 0x0
    li r5, 0x400
    bl fn_8018C69C
    mr r4, r31
    li r3, 0x0
    li r5, 0x0
    bl fn_8018CA20
@L_8012AF68:
    addi r30, r30, 0x1
    addi r23, r23, 0x20
    cmpwi r30, 0x2
    addi r24, r24, 0x4
    blt @L_8012AE70
    li r23, 0x0
    b @L_8012B000
@L_8012AF84:
    lwz r0, 0x0(r27)
    li r3, 0x0
    li r4, 0x3
    clrlwi r5, r0, 16
    bl fn_8012A5B0
    mr r24, r3
    bl fn_8011F4F0
    mr r0, r3
    li r3, 0x32
    mr r4, r0
    bl fn_80132A38
    li r3, 0x444e
    li r4, 0x1
    li r5, 0x0
    bl fn_801067E8
    li r3, 0x1
    bl fn_801065B8
    mr r3, r24
    bl fn_8011F1A0
    bl fn_801440A0
    cmplwi r3, 0x0
    bne @L_8012AFE4
    li r4, 0x0
    b @L_8012AFEC
@L_8012AFE4:
    bl fn_80143F6C
    mr r4, r3
@L_8012AFEC:
    mr r3, r24
    li r5, 0x7
    bl fn_80122370
    addi r27, r27, 0x4
    addi r23, r23, 0x1
@L_8012B000:
    cmpw r23, r25
    blt @L_8012AF84
    cmpwi r26, 0x0
    bgt @L_8012B0B0
    li r3, 0x444f
    li r4, 0x1
    li r5, 0x0
    bl fn_801067E8
    li r3, 0x1
    bl fn_801065B8
    li r3, 0x0
    li r4, 0xc
    li r5, 0x0
    bl fn_8012A5B0
    mr r4, r3
    li r3, 0x0
    srwi r0, r4, 31
    add r0, r0, r4
    srawi r4, r0, 1
    bl fn_80129474
    li r3, 0x1
    bl fn_801D0AFC
    li r23, 0x0
@L_8012B05C:
    clrlwi r5, r23, 16
    li r3, 0x0
    li r4, 0x3
    bl fn_8012A5B0
    mr. r24, r3
    beq @L_8012B0A0
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    beq @L_8012B0A0
    mr r3, r24
    li r4, 0x3e
    bl fn_80121ADC
    clrlwi. r0, r3, 24
    beq @L_8012B0A0
    mr r3, r24
    li r4, 0x3e
    bl fn_80121B4C
@L_8012B0A0:
    addi r23, r23, 0x1
    cmpwi r23, 0x6
    blt @L_8012B05C
    bl fn_80113FE8
@L_8012B0B0:
    li r23, 0x0
    li r25, 0x0
    li r26, 0x0
@L_8012B0BC:
    cmpwi r23, 0x0
    blt @L_8012B0CC
    cmpwi r23, 0x2
    blt @L_8012B0D4
@L_8012B0CC:
    li r0, 0x0
    b @L_8012B0F8
@L_8012B0D4:
    lis r3, lbl_80426BD0@ha
    addi r0, r3, lbl_80426BD0@l
    add r3, r0, r26
    lhz r0, 0x4(r3)
    clrlwi. r0, r0, 31
    bne @L_8012B0F4
    li r0, 0x0
    b @L_8012B0F8
@L_8012B0F4:
    li r0, 0x1
@L_8012B0F8:
    clrlwi. r0, r0, 24
    beq @L_8012B15C
    lwz r3, -26224(r2)
    cmpwi r23, 0x0
    lwz r0, -26220(r2)
    stw r3, 0x18(r1)
    stw r0, 0x1c(r1)
    blt @L_8012B128
    cmpwi r23, 0x2
    bge @L_8012B128
    addi r3, r1, 0x18
    lwzx r29, r3, r25
@L_8012B128:
    mr r4, r29
    li r3, 0x0
    li r5, 0x1
    bl fn_8018CA20
    mr r4, r29
    li r3, 0x0
    li r5, 0x700
    bl fn_8018C7C8
    lis r3, 0x8000
    mr r4, r29
    addi r5, r3, 0x8
    li r3, 0x0
    bl fn_8018C69C
@L_8012B15C:
    addi r23, r23, 0x1
    addi r26, r26, 0x20
    cmpwi r23, 0x2
    addi r25, r25, 0x4
    blt @L_8012B0BC
@L_8012B170:
    lmw r21, 0x44(r1)
    lwz r0, 0x74(r1)
    mtlr r0
    addi r1, r1, 0x70
    blr
