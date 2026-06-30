    nofralloc
    stwu r1, -0x1c0(r1)
    mflr r0
    stw r0, 0x1c4(r1)
    stmw r26, 0x1a8(r1)
    mr r30, r3
    mr r31, r4
    li r27, 0x0
    li r4, 0x0
    bl fn_800141BC
    addi r29, r1, 0x1c
    li r28, 0x0
@L_8002378C:
    mr r3, r28
    addi r4, r1, 0xc
    addi r5, r1, 0x8
    bl fn_80014118
    lwz r3, 0xc(r1)
    bl fn_80123FBC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_800238F0
    lwz r3, 0xc(r1)
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    cmpwi r3, 0x0
    bgt @L_800238F0
    lwz r3, 0xc(r1)
    li r4, 0x3e
    bl fn_80121ADC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_800238F0
    lwz r4, 0xc(r1)
    addi r3, r1, 0xa0
    lwz r5, 0x8(r1)
    clrlwi r6, r30, 16
    li r7, 0x0
    bl fn_80144574
    mr r26, r3
    extsh r0, r26
    cmpwi r0, 0x0
    ble @L_800238F0
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
    beq @L_80023880
    li r5, 0x1
    lhz r0, 0x12(r1)
    cmplw r30, r0
    beq @L_80023880
    li r5, 0x2
    lhz r0, 0x14(r1)
    cmplw r30, r0
    beq @L_80023880
    li r5, 0x3
    lhz r0, 0x16(r1)
    cmplw r30, r0
    beq @L_80023880
    li r5, 0x4
    clrlwi r0, r3, 16
    cmplw r30, r0
    beq @L_80023880
    li r5, 0x5
@L_80023880:
    cmpwi r5, 0x5
    bge @L_80023890
    li r3, 0x466
    b @L_80023894
@L_80023890:
    li r3, 0x465
@L_80023894:
    li r4, 0x0
    li r5, 0xff
    li r6, 0x0
    bl fn_80166A50
    bl fn_8001D378
    lwz r7, 0xc(r1)
    mr r6, r26
    addi r3, r1, 0x1c
    addi r5, r1, 0xa0
    li r4, 0x40
    bl fn_800216E8
    mr r4, r29
    li r3, 0x4d
    bl fn_80132A38
    li r3, 0x2
    li r4, 0xe0
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    add r0, r27, r26
    clrlwi r27, r0, 16
@L_800238F0:
    addi r28, r28, 0x1
    cmpwi r28, 0x6
    blt @L_8002378C
    clrlwi r0, r27, 16
    cmplwi r0, 0x0
    bne @L_8002392C
    li r3, 0x2
    li r4, 0x4261
    li r5, 0x1
    li r6, 0x0
    bl fn_80106D3C
    li r3, 0x1
    bl fn_801069FC
    li r26, -0x1
    b @L_80023930
@L_8002392C:
    li r26, 0x1
@L_80023930:
    li r3, -0x1
    bl fn_80014198
    cmpwi r26, 0x0
    bge @L_80023948
    li r3, 0x1
    b @L_80023954
@L_80023948:
    li r0, 0x1
    li r3, 0x0
    stw r0, 0x0(r31)
@L_80023954:
    lmw r26, 0x1a8(r1)
    lwz r0, 0x1c4(r1)
    mtlr r0
    addi r1, r1, 0x1c0
    blr

