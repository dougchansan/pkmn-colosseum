    nofralloc
    stwu r1, -0x300(r1)
    mflr r0
    stw r0, 0x304(r1)
    stmw r24, 0x2e0(r1)
    mr r25, r3
    mr r31, r4
    mr r26, r5
    mr r24, r6
    mr r27, r7
    mr r28, r8
    mr r29, r9
    mr r30, r10
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    bne @L_801280A0
    li r3, 0x2
    b @L_801282EC
@L_801280A0:
    clrlwi. r0, r31, 16
    bne @L_801280B0
    li r3, 0x2
    b @L_801282EC
@L_801280B0:
    mr r4, r25
    mr r5, r31
    mr r6, r24
    addi r3, r1, 0x1a4
    addi r7, r1, 0x44
    bl fn_80128524
    mr. r31, r3
    bge @L_801280D8
    li r3, 0x2
    b @L_801282EC
@L_801280D8:
    cmpwi r30, 0x0
    beq @L_801280F4
    lfs f1, -26240(r2)
    li r3, 0x3
    bl fn_801C41C8
    li r3, 0x1
    bl fn_801C40F0
@L_801280F4:
    li r6, 0x0
    cmpw r6, r31
    bge @L_801281B0
    cmpwi r31, 0x8
    subi r3, r31, 0x8
    ble @L_80128178
    addi r0, r3, 0x7
    addi r4, r1, 0x44
    srwi r0, r0, 3
    addi r5, r1, 0x1c
    mtctr r0
    cmpwi r3, 0x0
    ble @L_80128178
@L_80128128:
    lhz r3, 0x0(r4)
    addi r6, r6, 0x8
    lhz r0, 0x2(r4)
    sth r3, 0x0(r5)
    lhz r3, 0x4(r4)
    sth r0, 0x2(r5)
    lhz r0, 0x6(r4)
    sth r3, 0x4(r5)
    lhz r3, 0x8(r4)
    sth r0, 0x6(r5)
    lhz r0, 0xa(r4)
    sth r3, 0x8(r5)
    lhz r3, 0xc(r4)
    sth r0, 0xa(r5)
    lhz r0, 0xe(r4)
    addi r4, r4, 0x10
    sth r3, 0xc(r5)
    sth r0, 0xe(r5)
    addi r5, r5, 0x10
    bdnz @L_80128128
@L_80128178:
    slwi r5, r6, 1
    addi r3, r1, 0x44
    addi r4, r1, 0x1c
    subf r0, r6, r31
    add r3, r3, r5
    add r4, r4, r5
    mtctr r0
    cmpw r6, r31
    bge @L_801281B0
@L_8012819C:
    lhz r0, 0x0(r3)
    addi r3, r3, 0x2
    sth r0, 0x0(r4)
    addi r4, r4, 0x2
    bdnz @L_8012819C
@L_801281B0:
    mr r3, r25
    mr r5, r29
    mr r7, r31
    addi r4, r1, 0x1a4
    addi r6, r1, 0x1c
    addi r8, r1, 0x8
    bl fn_8026132C
    cmpwi r30, 0x0
    mr r29, r3
    beq @L_801281EC
    lfs f1, -26240(r2)
    li r3, 0x2
    bl fn_801C41C8
    li r3, 0x1
    bl fn_801C40F0
@L_801281EC:
    cmpwi r29, 0x2
    bne @L_801281FC
    li r3, 0x2
    b @L_801282EC
@L_801281FC:
    cmpwi r29, 0x1
    bne @L_8012820C
    li r3, 0x1
    b @L_801282EC
@L_8012820C:
    cmpwi r28, 0x0
    beq @L_80128220
    addi r4, r1, 0x1a4
    li r3, 0x0
    bl memoDataSet
@L_80128220:
    mr r3, r25
    addi r4, r1, 0x1a4
    bl fn_8011F5FC
    addi r29, r1, 0x8
    addi r30, r1, 0x44
    li r24, 0x0
    b @L_80128260
@L_8012823C:
    lbz r4, 0x0(r29)
    cmplwi r4, 0xff
    beq @L_80128254
    lhz r5, 0x0(r30)
    mr r3, r25
    bl fn_80123D58
@L_80128254:
    addi r29, r29, 0x1
    addi r30, r30, 0x2
    addi r24, r24, 0x1
@L_80128260:
    cmpw r24, r31
    blt @L_8012823C
    clrlwi. r0, r26, 16
    beq @L_801282E8
    beq @L_801282E8
    li r29, 0x0
@L_80128278:
    mr r3, r27
    clrlwi r5, r29, 16
    li r4, 0x3
    bl fn_8012A5B0
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    beq @L_801282A0
    addi r29, r29, 0x1
    cmpwi r29, 0x6
    blt @L_80128278
@L_801282A0:
    cmpwi r29, 0x6
    li r0, -0x1
    bge @L_801282B0
    extsb r0, r29
@L_801282B0:
    extsb. r0, r0
    blt @L_801282E8
    mr r4, r25
    mr r5, r26
    addi r3, r1, 0x6c
    bl fn_80128300
    mr r3, r27
    addi r4, r1, 0x6c
    bl fn_8012A08C
    cmpwi r28, 0x0
    beq @L_801282E8
    addi r4, r1, 0x6c
    li r3, 0x0
    bl memoDataSet
@L_801282E8:
    li r3, 0x0
@L_801282EC:
    lmw r24, 0x2e0(r1)
    lwz r0, 0x304(r1)
    mtlr r0
    addi r1, r1, 0x300
    blr
