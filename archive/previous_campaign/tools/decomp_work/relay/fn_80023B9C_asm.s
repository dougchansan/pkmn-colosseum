    nofralloc
    stwu r1, -0x1b0(r1)
    mflr r0
    stw r0, 0x1b4(r1)
    stw r31, 0x1ac(r1)
    stw r30, 0x1a8(r1)
    stw r29, 0x1a4(r1)
    stw r28, 0x1a0(r1)
    mr r30, r3
    mr r31, r4
    li r4, 0x1
    bl fn_800141BC
    mr r29, r3
    cmpwi r29, 0x0
    blt @L_80023D38
    addi r4, r1, 0xc
    addi r5, r1, 0x8
    bl fn_80014118
    lwz r3, 0xc(r1)
    li r4, 0x3e
    bl fn_80121ADC
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80023D00
    lwz r4, 0xc(r1)
    addi r3, r1, 0xa0
    lwz r5, 0x8(r1)
    clrlwi r6, r30, 16
    li r7, 0x0
    bl fn_80144574
    mr r28, r3
    extsh r0, r28
    cmpwi r0, 0x0
    ble @L_80023CBC
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
    beq @L_80023C94
    li r5, 0x1
    lhz r0, 0x12(r1)
    cmplw r30, r0
    beq @L_80023C94
    li r5, 0x2
    lhz r0, 0x14(r1)
    cmplw r30, r0
    beq @L_80023C94
    li r5, 0x3
    lhz r0, 0x16(r1)
    cmplw r30, r0
    beq @L_80023C94
    li r5, 0x4
    clrlwi r0, r3, 16
    cmplw r30, r0
    beq @L_80023C94
    li r5, 0x5
@L_80023C94:
    cmpwi r5, 0x5
    bge @L_80023CA4
    li r3, 0x466
    b @L_80023CA8
@L_80023CA4:
    li r3, 0x465
@L_80023CA8:
    li r4, 0x0
    li r5, 0xff
    li r6, 0x0
    bl fn_80166A50
    bl fn_8001D378
@L_80023CBC:
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
    b @L_80023D38
@L_80023D00:
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
@L_80023D38:
    mr r3, r29
    bl fn_80014198
    cmpwi r29, 0x0
    blt @L_80023D84
    extsh r0, r28
    cmpwi r0, 0x0
    ble @L_80023D84
    cmpwi r30, 0x2c
    bge @L_80023D74
    cmpwi r30, 0x27
    bge @L_80023D68
    b @L_80023D74
@L_80023D68:
    li r0, 0x0
    stw r0, 0x0(r31)
    b @L_80023D7C
@L_80023D74:
    li r0, 0x1
    stw r0, 0x0(r31)
@L_80023D7C:
    li r3, 0x0
    b @L_80023D88
@L_80023D84:
    li r3, 0x1
@L_80023D88:
    lwz r0, 0x1b4(r1)
    lwz r31, 0x1ac(r1)
    lwz r30, 0x1a8(r1)
    lwz r29, 0x1a4(r1)
    lwz r28, 0x1a0(r1)
    mtlr r0
    addi r1, r1, 0x1b0
    blr

