    nofralloc
    stwu r1, -0x1c0(r1)
    mflr r0
    stw r0, 0x1c4(r1)
    stmw r26, 0x1a8(r1)
    mr r30, r3
    mr r31, r4
    li r26, 0x0
@L_80023084:
    mr r3, r30
    li r4, 0x1
    bl fn_800141BC
    mr r29, r3
    cmpwi r29, 0x0
    blt @L_800230E0
    addi r4, r1, 0xc
    addi r5, r1, 0x8
    bl fn_80014118
    lwz r3, 0xc(r1)
    li r4, 0x3e
    bl fn_80121ADC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_800230C8
    li r26, 0x1
    b @L_800230E0
@L_800230C8:
    extsb r3, r29
    bl fn_80019B48
    mr r27, r3
    bl fn_80019B1C
    cmpwi r27, 0x0
    blt @L_80023084
@L_800230E0:
    cmpwi r29, 0x0
    blt @L_80023230
    cmpwi r26, 0x0
    beq @L_8002312C
    lwz r3, 0xc(r1)
    bl fn_8011F4F0
    mr r0, r3
    li r3, 0x32
    mr r4, r0
    bl fn_80132A38
    li r3, 0x2
    li r4, 0x424d
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r28, 0x0
    b @L_80023230
@L_8002312C:
    lwz r4, 0xc(r1)
    addi r3, r1, 0xa0
    lwz r5, 0x8(r1)
    clrlwi r6, r30, 16
    clrlwi r7, r27, 24
    bl fn_80144574
    mr r28, r3
    extsh r0, r28
    cmpwi r0, 0x0
    ble @L_800231F0
    lis r3, lbl_80266DB0@ha
    li r5, 0x0
    addi r3, r3, lbl_80266DB0@l
    lwz r4, 0x0(r3)
    lwz r0, 0x4(r3)
    lhz r3, 0x8(r3)
    stw r4, 0x10(r1)
    stw r0, 0x14(r1)
    sth r3, 0x18(r1)
    lhz r0, 0x10(r1)
    cmplw r30, r0
    beq @L_800231C8
    li r5, 0x1
    lhz r0, 0x12(r1)
    cmplw r30, r0
    beq @L_800231C8
    li r5, 0x2
    lhz r0, 0x14(r1)
    cmplw r30, r0
    beq @L_800231C8
    li r5, 0x3
    lhz r0, 0x16(r1)
    cmplw r30, r0
    beq @L_800231C8
    li r5, 0x4
    clrlwi r0, r3, 16
    cmplw r30, r0
    beq @L_800231C8
    li r5, 0x5
@L_800231C8:
    cmpwi r5, 0x5
    bge @L_800231D8
    li r3, 0x466
    b @L_800231DC
@L_800231D8:
    li r3, 0x465
@L_800231DC:
    li r4, 0x0
    li r5, 0xff
    li r6, 0x0
    bl fn_80166A50
    bl fn_8001D378
@L_800231F0:
    lwz r7, 0xc(r1)
    mr r6, r28
    addi r3, r1, 0x1c
    addi r5, r1, 0xa0
    li r4, 0x40
    bl fn_800216E8
    addi r4, r1, 0x1c
    li r3, 0x4d
    bl fn_80132A38
    li r3, 0x2
    li r4, 0xe0
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
@L_80023230:
    mr r3, r29
    bl fn_80014198
    cmpwi r29, 0x0
    blt @L_8002325C
    extsh r0, r28
    cmpwi r0, 0x0
    ble @L_8002325C
    li r0, 0x1
    li r3, 0x0
    stw r0, 0x0(r31)
    b @L_80023260
@L_8002325C:
    li r3, 0x1
@L_80023260:
    lmw r26, 0x1a8(r1)
    lwz r0, 0x1c4(r1)
    mtlr r0
    addi r1, r1, 0x1c0
    blr

