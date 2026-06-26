    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    li r6, 0x0
    stw r0, 0x34(r1)
    stmw r24, 0x10(r1)
    mr r28, r4
    mr r29, r5
    mr r24, r3
    li r4, 0x0
    li r5, 0x93
    bl fn_8012640C
    clrlwi r27, r3, 16
    mr r3, r24
    li r4, 0x0
    li r5, 0x94
    li r6, 0x0
    bl fn_8012640C
    clrlwi r26, r3, 16
    mr r3, r24
    li r4, 0x0
    li r5, 0x95
    li r6, 0x0
    bl fn_8012640C
    clrlwi r25, r3, 16
    mr r3, r24
    li r4, 0x0
    li r5, 0x96
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    mr r3, r24
    li r4, 0x0
    li r5, 0x97
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 16
    mr r3, r24
    li r4, 0x0
    li r5, 0x98
    li r6, 0x0
    bl fn_8012640C
    rlwinm r4, r27, 0, 30, 30
    clrlwi r3, r3, 16
    srawi r5, r4, 1
    clrlslwi r0, r26, 31, 1
    rlwimi r5, r26, 0, 30, 30
    lis r4, 0x8208
    rlwimi r5, r25, 1, 29, 29
    rlwimi r0, r27, 0, 31, 31
    rlwimi r5, r3, 2, 28, 28
    addi r6, r4, 0x2083
    rlwimi r5, r31, 3, 27, 27
    rlwimi r0, r25, 2, 29, 29
    rlwimi r0, r3, 3, 28, 28
    rlwimi r5, r30, 4, 26, 26
    rlwimi r0, r31, 4, 27, 27
    clrlwi r3, r5, 24
    rlwimi r0, r30, 5, 26, 26
    mulli r4, r3, 0x28
    clrlwi r0, r0, 24
    mulli r0, r0, 0xf
    mulhw r5, r6, r4
    mulhw r3, r6, r0
    add r4, r5, r4
    srawi r4, r4, 5
    add r0, r3, r0
    srwi r5, r4, 31
    srawi r0, r0, 5
    srwi r3, r0, 31
    add r4, r4, r5
    add r3, r0, r3
    addi r0, r3, 0x1
    addi r4, r4, 0x1e
    clrlwi r3, r0, 24
    cmplwi r3, 0x9
    clrlwi r4, r4, 16
    mr r0, r3
    blt @L_80120C40
    addi r0, r3, 0x1
    clrlwi r0, r0, 16
@L_80120C40:
    cmplwi r28, 0x0
    beq @L_80120C4C
    sth r4, 0x0(r28)
@L_80120C4C:
    cmplwi r29, 0x0
    beq @L_80120C58
    sth r0, 0x0(r29)
@L_80120C58:
    lmw r24, 0x10(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
