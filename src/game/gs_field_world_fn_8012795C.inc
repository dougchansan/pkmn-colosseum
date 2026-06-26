    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    li r5, 0x6e
    li r6, 0x0
    stw r0, 0x34(r1)
    stmw r24, 0x10(r1)
    mr r27, r4
    mr r30, r3
    li r4, 0x0
    bl fn_8012640C
    clrlwi r29, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    mr r3, r30
    clrlwi r7, r27, 24
    li r4, 0x0
    li r5, 0x7a
    li r6, 0x0
    bl fn_801254B4
    cmplwi r29, 0x12f
    bne @L_801279E0
    mr r3, r30
    li r28, 0x1
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    li r7, 0x1
    bl fn_801254B4
    b @L_80127AB4
@L_801279E0:
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 24
    mr r4, r25
    li r3, 0x0
    li r5, 0x3
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x93
    li r6, 0x0
    bl fn_8012640C
    clrlwi r28, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x8d
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r25, 16, 1
    add r0, r0, r3
    clrlwi r5, r27, 24
    add r0, r28, r0
    lis r3, 0x51ec
    mullw r0, r5, r0
    subi r4, r3, 0x7ae1
    mr r3, r26
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r0, r0, r4
    add r25, r5, r0
    addi r25, r25, 0xa
    bl fn_8011CE18
    cmplwi r3, 0x0
    mr r3, r30
    mr r7, r25
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_801254B4
    clrlwi r28, r25, 16
@L_80127AB4:
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 24
    mr r4, r24
    li r3, 0x0
    li r5, 0x4
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x94
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x8e
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r24, 16, 1
    add r0, r0, r3
    clrlwi r27, r27, 24
    add r0, r26, r0
    lis r3, 0x51ec
    mullw r0, r27, r0
    subi r4, r3, 0x7ae1
    mr r3, r25
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r4, r0, r4
    addi r24, r4, 0x5
    bl fn_8011CE18
    cmplwi r3, 0x0
    bne @L_80127B70
    b @L_80127BAC
@L_80127B70:
    bl fn_8011CDE8
    bl fn_8011CBC8
    mr. r25, r3
    bne @L_80127B88
    li r24, 0x0
    b @L_80127BAC
@L_80127B88:
    bl fn_8011CBB0
    mr r26, r3
    mr r3, r25
    bl fn_8011CB98
    clrlwi r4, r26, 24
    clrlwi. r0, r3, 24
    mullw r24, r24, r4
    beq @L_80127BAC
    divw r24, r24, r0
@L_80127BAC:
    mr r3, r30
    mr r7, r24
    li r4, 0x0
    li r5, 0x88
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 24
    mr r4, r25
    li r3, 0x0
    li r5, 0x5
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x95
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x8f
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r26, 16, 1
    add r0, r0, r3
    add r0, r25, r0
    lis r3, 0x51ec
    mullw r0, r27, r0
    subi r4, r3, 0x7ae1
    mr r3, r24
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r4, r0, r4
    addi r24, r4, 0x5
    bl fn_8011CE18
    cmplwi r3, 0x0
    bne @L_80127C7C
    b @L_80127CB8
@L_80127C7C:
    bl fn_8011CDD0
    bl fn_8011CBC8
    mr. r25, r3
    bne @L_80127C94
    li r24, 0x0
    b @L_80127CB8
@L_80127C94:
    bl fn_8011CBB0
    mr r26, r3
    mr r3, r25
    bl fn_8011CB98
    clrlwi r4, r26, 24
    clrlwi. r0, r3, 24
    mullw r24, r24, r4
    beq @L_80127CB8
    divw r24, r24, r0
@L_80127CB8:
    mr r3, r30
    mr r7, r24
    li r4, 0x0
    li r5, 0x89
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 24
    mr r4, r25
    li r3, 0x0
    li r5, 0x8
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x98
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x92
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r26, 16, 1
    add r0, r0, r3
    add r0, r25, r0
    lis r3, 0x51ec
    mullw r0, r27, r0
    subi r4, r3, 0x7ae1
    mr r3, r24
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r4, r0, r4
    addi r24, r4, 0x5
    bl fn_8011CE18
    cmplwi r3, 0x0
    bne @L_80127D88
    b @L_80127DC4
@L_80127D88:
    bl fn_8011CD88
    bl fn_8011CBC8
    mr. r25, r3
    bne @L_80127DA0
    li r24, 0x0
    b @L_80127DC4
@L_80127DA0:
    bl fn_8011CBB0
    mr r26, r3
    mr r3, r25
    bl fn_8011CB98
    clrlwi r4, r26, 24
    clrlwi. r0, r3, 24
    mullw r24, r24, r4
    beq @L_80127DC4
    divw r24, r24, r0
@L_80127DC4:
    mr r3, r30
    mr r7, r24
    li r4, 0x0
    li r5, 0x8c
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 24
    mr r4, r25
    li r3, 0x0
    li r5, 0x6
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x96
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x90
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r26, 16, 1
    add r0, r0, r3
    add r0, r25, r0
    lis r3, 0x51ec
    mullw r0, r27, r0
    subi r4, r3, 0x7ae1
    mr r3, r24
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r4, r0, r4
    addi r24, r4, 0x5
    bl fn_8011CE18
    cmplwi r3, 0x0
    bne @L_80127E94
    b @L_80127ED0
@L_80127E94:
    bl fn_8011CDB8
    bl fn_8011CBC8
    mr. r25, r3
    bne @L_80127EAC
    li r24, 0x0
    b @L_80127ED0
@L_80127EAC:
    bl fn_8011CBB0
    mr r26, r3
    mr r3, r25
    bl fn_8011CB98
    clrlwi r4, r26, 24
    clrlwi. r0, r3, 24
    mullw r24, r24, r4
    beq @L_80127ED0
    divw r24, r24, r0
@L_80127ED0:
    mr r3, r30
    mr r7, r24
    li r4, 0x0
    li r5, 0x8a
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0xbf
    li r6, 0x0
    bl fn_8012640C
    clrlwi r24, r3, 24
    mr r4, r25
    li r3, 0x0
    li r5, 0x7
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x97
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r30
    li r4, 0x0
    li r5, 0x91
    li r6, 0x0
    bl fn_8012640C
    extrwi r3, r3, 14, 16
    clrlslwi r0, r26, 16, 1
    add r0, r0, r3
    add r0, r25, r0
    lis r3, 0x51ec
    mullw r0, r27, r0
    subi r4, r3, 0x7ae1
    mr r3, r24
    mulhw r0, r4, r0
    srawi r0, r0, 5
    srwi r4, r0, 31
    add r4, r0, r4
    addi r24, r4, 0x5
    bl fn_8011CE18
    cmplwi r3, 0x0
    bne @L_80127FA0
    b @L_80127FDC
@L_80127FA0:
    bl fn_8011CDA0
    bl fn_8011CBC8
    mr. r25, r3
    bne @L_80127FB8
    li r24, 0x0
    b @L_80127FDC
@L_80127FB8:
    bl fn_8011CBB0
    mr r27, r3
    mr r3, r25
    bl fn_8011CB98
    clrlwi r4, r27, 24
    clrlwi. r0, r3, 24
    mullw r24, r24, r4
    beq @L_80127FDC
    divw r24, r24, r0
@L_80127FDC:
    mr r3, r30
    mr r7, r24
    li r4, 0x0
    li r5, 0x8b
    li r6, 0x0
    bl fn_801254B4
    mr r3, r30
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    clrlwi. r3, r3, 16
    bne @L_80128018
    cmplwi r31, 0x0
    bne @L_80128048
@L_80128018:
    cmplwi r29, 0x12f
    li r0, 0x1
    beq @L_80128030
    subf r0, r31, r28
    add r0, r3, r0
    clrlwi r0, r0, 16
@L_80128030:
    mr r3, r30
    clrlwi r7, r0, 16
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_801254B4
@L_80128048:
    lmw r24, 0x10(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
