    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r24, 0x10(r1)
    mr r25, r3
    mr r26, r6
    lhz r3, 0x0(r5)
    li r29, 0x0
    clrlwi r0, r3, 31
    cmpwi r0, 0x0
    beq @L_80024194
    ori r0, r29, 0x1
    clrlwi r29, r0, 24
@L_80024194:
    rlwinm r0, r3, 0, 30, 30
    cmpwi r0, 0x0
    beq @L_800241A8
    ori r0, r29, 0x4
    clrlwi r29, r0, 24
@L_800241A8:
    rlwinm r0, r3, 0, 29, 29
    cmpwi r0, 0x0
    beq @L_800241BC
    ori r0, r29, 0x8
    clrlwi r29, r0, 24
@L_800241BC:
    rlwinm r0, r3, 0, 28, 28
    cmpwi r0, 0x0
    beq @L_800241D0
    ori r0, r29, 0x2
    clrlwi r29, r0, 24
@L_800241D0:
    clrlwi r0, r29, 24
    cmplwi r0, 0x0
    beq @L_800242F4
    lwz r28, lbl_8047A368@sda21(r0)
    lwz r3, 0x4(r25)
    bl fn_8005DA18
    lha r3, 0x4(r3)
    bl fn_8005D934
    li r27, 0x0
@L_800241F4:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_80024218
    cmpw r28, r27
    bne @L_80024214
    mr r31, r3
    b @L_80024238
@L_80024214:
    addi r27, r27, 0x1
@L_80024218:
    lbz r0, 0x0(r3)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_80024234
    lha r3, 0x18(r3)
    bl fn_8005D934
    b @L_800241F4
@L_80024234:
    li r31, 0x0
@L_80024238:
    clrlwi r29, r29, 24
    li r27, 0x0
    li r30, 0x0
    b @L_800242E4
@L_80024248:
    lha r3, 0x4(r26)
    li r28, 0x0
    bl fn_8005D934
    mr r24, r3
@L_80024258:
    cmplwi r24, 0x0
    beq @L_800242DC
    lbz r0, 0x0(r24)
    extrwi r0, r0, 1, 24
    cmplwi r0, 0x0
    beq @L_800242BC
    lwz r0, lbl_80478DF4@sda21(r0)
    add r3, r30, r0
    lbz r0, 0x0(r3)
    and r0, r29, r0
    cmpw r29, r0
    bne @L_800242B8
    lwz r3, 0x4(r3)
    bl fn_8005D934
    cmplw r3, r31
    bne @L_800242B8
    lwz r3, lbl_80478DF4@sda21(r0)
    addi r0, r3, 0x8
    lwzx r3, r30, r0
    bl fn_8005D934
    cmplw r3, r24
    bne @L_800242B8
    stb r28, 0x95(r25)
    b @L_800242F4
@L_800242B8:
    addi r28, r28, 0x1
@L_800242BC:
    lbz r0, 0x0(r24)
    extrwi r0, r0, 1, 25
    cmplwi r0, 0x0
    bne @L_800242DC
    lha r3, 0x18(r24)
    bl fn_8005D934
    mr r24, r3
    b @L_80024258
@L_800242DC:
    addi r30, r30, 0xc
    addi r27, r27, 0x1
@L_800242E4:
    lwz r3, lbl_80478DF0@sda21(r0)
    lwz r0, 0x0(r3)
    cmplw r27, r0
    blt @L_80024248
@L_800242F4:
    lmw r24, 0x10(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr

