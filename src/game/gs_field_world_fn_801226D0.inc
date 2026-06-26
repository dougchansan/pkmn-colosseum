    nofralloc
    stwu r1, -0x50(r1)
    mflr r0
    lis r7, lbl_80272948@ha
    stw r0, 0x54(r1)
    addi r12, r7, lbl_80272948@l
    stmw r24, 0x30(r1)
    mr r29, r4
    mr r28, r5
    mr r26, r6
    mr. r25, r3
    lwz r11, 0x0(r12)
    lwz r10, 0x4(r12)
    lwz r9, 0x8(r12)
    lwz r8, 0xc(r12)
    lwz r7, 0x10(r12)
    lwz r6, 0x14(r12)
    lwz r5, 0x18(r12)
    lwz r4, 0x1c(r12)
    lwz r0, 0x20(r12)
    stw r11, 0x8(r1)
    stw r10, 0xc(r1)
    stw r9, 0x10(r1)
    stw r8, 0x14(r1)
    stw r7, 0x18(r1)
    stw r6, 0x1c(r1)
    stw r5, 0x20(r1)
    stw r4, 0x24(r1)
    stw r0, 0x28(r1)
    beq @L_801229E0
    bne @L_80122750
    li r0, 0x0
    b @L_80122820
@L_80122750:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r27, r3, 16
    bne @L_80122770
    li r0, 0x0
    b @L_80122820
@L_80122770:
    bne @L_8012277C
    li r0, 0x0
    b @L_801227BC
@L_8012277C:
    mr r4, r27
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_801227A0
    li r0, 0x0
    b @L_801227BC
@L_801227A0:
    lwz r3, lbl_80478F90(r13)
    lwz r0, 0x0(r3)
    cmplw r27, r0
    blt @L_801227B8
    li r0, 0x0
    b @L_801227BC
@L_801227B8:
    li r0, 0x1
@L_801227BC:
    clrlwi. r0, r0, 24
    bne @L_801227CC
    li r0, 0x0
    b @L_80122820
@L_801227CC:
    mr r3, r25
    li r4, 0x0
    li r5, 0x70
    li r6, 0x0
    bl fn_8012640C
    bl gamedataAttestCheckValid
    clrlwi. r0, r3, 24
    bne @L_801227F4
    li r0, 0x0
    b @L_80122820
@L_801227F4:
    mr r3, r25
    li r4, 0x0
    li r5, 0xb8
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8012281C
    li r0, 0x0
    b @L_80122820
@L_8012281C:
    li r0, 0x1
@L_80122820:
    clrlwi. r0, r0, 24
    bne @L_80122830
    li r0, 0x0
    b @L_801228A8
@L_80122830:
    mr r3, r25
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    cmpwi r3, 0x19c
    bne @L_80122854
    li r0, 0x0
    b @L_801228A8
@L_80122854:
    mr r3, r25
    li r4, 0x0
    li r5, 0xb6
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8012287C
    li r0, 0x0
    b @L_801228A8
@L_8012287C:
    mr r3, r25
    li r4, 0x0
    li r5, 0x7b
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801228A4
    li r0, 0x0
    b @L_801228A8
@L_801228A4:
    li r0, 0x1
@L_801228A8:
    clrlwi. r0, r0, 24
    beq @L_801229E0
    li r24, 0x1
    li r27, 0x0
    li r31, 0x0
    b @L_801228F4
@L_801228C0:
    clrlwi r0, r31, 24
    addi r30, r1, 0x8
    mulli r0, r0, 0x6
    mr r3, r25
    li r4, 0x0
    li r6, 0x0
    add r30, r30, r0
    lhz r5, 0x0(r30)
    bl fn_8012640C
    sth r3, 0x4(r30)
    addi r31, r31, 0x1
    lhz r0, 0x4(r30)
    add r27, r27, r0
@L_801228F4:
    clrlwi r0, r31, 24
    cmplwi r0, 0x6
    blt @L_801228C0
    clrlwi r0, r28, 24
    cmplwi r0, 0x1
    bne @L_80122910
    li r24, 0x2
@L_80122910:
    clrlwi r30, r24, 24
    clrlwi r31, r29, 16
    li r28, 0x0
    b @L_801229D4
@L_80122920:
    clrlwi r0, r27, 16
    cmplwi r0, 0x1fe
    bge @L_801229E0
    clrlwi r0, r28, 24
    addi r29, r1, 0x8
    mulli r0, r0, 0x6
    mr r4, r26
    li r3, 0x0
    li r6, 0x0
    add r29, r29, r0
    lhz r5, 0x2(r29)
    bl fn_8012640C
    clrlwi r0, r3, 16
    cmplwi r31, 0x18
    mullw r0, r30, r0
    clrlwi r5, r0, 16
    bne @L_80122968
    clrlslwi r5, r5, 17, 1
@L_80122968:
    clrlwi r3, r27, 16
    clrlwi r0, r5, 16
    add r3, r3, r0
    cmpwi r3, 0x1fe
    ble @L_80122988
    subi r0, r3, 0x1fe
    subf r0, r0, r5
    clrlwi r5, r0, 16
@L_80122988:
    lhz r0, 0x4(r29)
    clrlwi r3, r5, 16
    add r3, r3, r0
    cmpwi r3, 0xff
    ble @L_801229A8
    subi r0, r3, 0xff
    subf r0, r0, r5
    clrlwi r5, r0, 16
@L_801229A8:
    lhz r0, 0x4(r29)
    mr r3, r25
    add r27, r27, r5
    li r4, 0x0
    add r0, r0, r5
    li r6, 0x0
    sth r0, 0x4(r29)
    lhz r5, 0x0(r29)
    lhz r7, 0x4(r29)
    bl fn_801254B4
    addi r28, r28, 0x1
@L_801229D4:
    clrlwi r0, r28, 24
    cmplwi r0, 0x6
    blt @L_80122920
@L_801229E0:
    lmw r24, 0x30(r1)
    lwz r0, 0x54(r1)
    mtlr r0
    addi r1, r1, 0x50
    blr
