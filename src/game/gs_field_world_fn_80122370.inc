    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr. r29, r3
    mr r30, r4
    mr r31, r5
    beq @L_801226BC
    bne @L_8012239C
    li r0, 0x0
    b @L_8012246C
@L_8012239C:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r28, r3, 16
    bne @L_801223BC
    li r0, 0x0
    b @L_8012246C
@L_801223BC:
    bne @L_801223C8
    li r0, 0x0
    b @L_80122408
@L_801223C8:
    mr r4, r28
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8012640C
    cmplwi r3, 0x0
    bne @L_801223EC
    li r0, 0x0
    b @L_80122408
@L_801223EC:
    lwz r3, lbl_80478F90(r13)
    lwz r0, 0x0(r3)
    cmplw r28, r0
    blt @L_80122404
    li r0, 0x0
    b @L_80122408
@L_80122404:
    li r0, 0x1
@L_80122408:
    clrlwi. r0, r0, 24
    bne @L_80122418
    li r0, 0x0
    b @L_8012246C
@L_80122418:
    mr r3, r29
    li r4, 0x0
    li r5, 0x70
    li r6, 0x0
    bl fn_8012640C
    bl gamedataAttestCheckValid
    clrlwi. r0, r3, 24
    bne @L_80122440
    li r0, 0x0
    b @L_8012246C
@L_80122440:
    mr r3, r29
    li r4, 0x0
    li r5, 0xb8
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122468
    li r0, 0x0
    b @L_8012246C
@L_80122468:
    li r0, 0x1
@L_8012246C:
    clrlwi. r0, r0, 24
    bne @L_8012247C
    li r0, 0x0
    b @L_80122510
@L_8012247C:
    mr r3, r29
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    cmpwi r3, 0x19c
    bne @L_801224A0
    li r0, 0x0
    b @L_80122510
@L_801224A0:
    mr r3, r29
    li r4, 0x0
    li r5, 0xb6
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_801224C8
    li r0, 0x0
    b @L_80122510
@L_801224C8:
    clrlwi r0, r31, 16
    cmplwi r0, 0x6
    beq @L_8012250C
    cmplwi r0, 0x7
    beq @L_8012250C
    cmplwi r0, 0x8
    beq @L_8012250C
    mr r3, r29
    li r4, 0x0
    li r5, 0x7b
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8012250C
    li r0, 0x0
    b @L_80122510
@L_8012250C:
    li r0, 0x1
@L_80122510:
    clrlwi. r0, r0, 24
    beq @L_801226BC
    mr r3, r29
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    bl fn_8012640C
    mr r28, r3
    mr r3, r29
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_80122568
    mr r3, r29
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    bl fn_8012640C
    add r28, r28, r3
@L_80122568:
    cmpwi r28, 0x64
    li r27, 0x0
    blt @L_80122578
    li r27, 0x1
@L_80122578:
    cmpwi r28, 0xc8
    blt @L_8012258C
    clrlwi r3, r27, 24
    addi r0, r3, 0x1
    clrlwi r27, r0, 24
@L_8012258C:
    clrlwi r0, r31, 16
    cmplwi r0, 0x5
    bne @L_801225A4
    bl fn_800E0C54
    clrlwi. r0, r3, 31
    bne @L_801226BC
@L_801225A4:
    mr r3, r31
    bl fn_8011CB10
    mr r4, r27
    bl fn_8011CAE0
    extsb. r31, r3
    ble @L_801225E4
    clrlwi r0, r30, 16
    cmplwi r0, 0x1b
    bne @L_801225E4
    mulli r0, r31, 0x96
    lis r3, 0x51ec
    subi r3, r3, 0x7ae1
    mulhw r0, r3, r0
    srawi r0, r0, 5
    srwi r3, r0, 31
    add r31, r0, r3
@L_801225E4:
    cmpwi r31, 0x0
    mr r30, r31
    ble @L_80122614
    mr r3, r29
    li r4, 0x0
    li r5, 0x73
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 16
    cmplwi r0, 0xb
    bne @L_80122614
    addi r30, r31, 0x1
@L_80122614:
    mr r3, r29
    li r4, 0x0
    li r5, 0xc2
    li r6, 0x0
    bl fn_8012640C
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8012268C
    mr r3, r29
    bl fn_8011F77C
    clrlwi r0, r3, 24
    cmplwi r0, 0x3
    blt @L_801226BC
    mr r3, r29
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    bl fn_8012640C
    add. r7, r3, r30
    bge @L_80122668
    li r7, 0x0
@L_80122668:
    cmpwi r7, 0xff
    ble @L_80122674
    li r7, 0xff
@L_80122674:
    mr r3, r29
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    bl fn_801254B4
    b @L_801226BC
@L_8012268C:
    add. r28, r28, r30
    bge @L_80122698
    li r28, 0x0
@L_80122698:
    cmpwi r28, 0xff
    ble @L_801226A4
    li r28, 0xff
@L_801226A4:
    mr r3, r29
    mr r7, r28
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    bl fn_801254B4
@L_801226BC:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
