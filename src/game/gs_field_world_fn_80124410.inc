    nofralloc
    stwu r1, -0x40(r1)
    mflr r0
    stw r0, 0x44(r1)
    stmw r21, 0x14(r1)
    mr. r24, r3
    mr r25, r4
    mr r26, r5
    mr r27, r6
    bne @L_80124898
    bl fn_800E0C54
    slwi r23, r3, 16
    bl fn_800E0C54
    clrlwi r0, r3, 16
    or r3, r0, r23
    b @L_801248B0
@L_8012444C:
    bl fn_800E0C54
    slwi r23, r3, 16
    bl fn_800E0C54
    extsb. r0, r25
    clrlwi r0, r3, 16
    or r31, r0, r23
    blt @L_801247FC
    cmplwi r24, 0x0
    bne @L_80124478
    li r0, 0x2
    b @L_801244FC
@L_80124478:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r23, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r23, r0
    bne @L_801244C0
    li r0, 0x0
    b @L_801244FC
@L_801244C0:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r23, r0
    bne @L_801244DC
    li r0, 0x1
    b @L_801244FC
@L_801244DC:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r23, r0
    bne @L_801244F8
    li r0, 0x2
    b @L_801244FC
@L_801244F8:
    li r0, -0x1
@L_801244FC:
    extsb. r0, r0
    bge @L_801247FC
    extsb r0, r25
    cmpwi r0, 0x2
    beq @L_80124600
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    cmplwi r24, 0x0
    clrlwi r23, r3, 16
    bne @L_8012454C
    li r3, 0x2
    b @L_801245D0
@L_8012454C:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r22, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r22, r0
    bne @L_80124594
    li r3, 0x0
    b @L_801245D0
@L_80124594:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r22, r0
    bne @L_801245B0
    li r3, 0x1
    b @L_801245D0
@L_801245B0:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r22, r0
    bne @L_801245CC
    li r3, 0x2
    b @L_801245D0
@L_801245CC:
    li r3, -0x1
@L_801245D0:
    extsb. r0, r3
    bge @L_801245F0
    clrlwi r0, r31, 24
    cmplw r23, r0
    ble @L_801245EC
    li r3, 0x1
    b @L_801245F0
@L_801245EC:
    li r3, 0x0
@L_801245F0:
    clrlwi r0, r3, 24
    cmpw r30, r0
    bne @L_8012444C
    b @L_801247FC
@L_80124600:
    cmplwi r24, 0x0
    bne @L_80124610
    li r23, 0x2
    b @L_8012470C
@L_80124610:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_8012640C
    mr r23, r3
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    cmplwi r24, 0x0
    clrlwi r22, r3, 16
    bne @L_80124664
    li r3, 0x2
    b @L_801246E8
@L_80124664:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r21, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_801246AC
    li r3, 0x0
    b @L_801246E8
@L_801246AC:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_801246C8
    li r3, 0x1
    b @L_801246E8
@L_801246C8:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_801246E4
    li r3, 0x2
    b @L_801246E8
@L_801246E4:
    li r3, -0x1
@L_801246E8:
    extsb. r0, r3
    bge @L_80124708
    clrlwi r0, r23, 24
    cmplw r22, r0
    ble @L_80124704
    li r3, 0x1
    b @L_80124708
@L_80124704:
    li r3, 0x0
@L_80124708:
    clrlwi r23, r3, 24
@L_8012470C:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    cmplwi r24, 0x0
    clrlwi r22, r3, 16
    bne @L_80124748
    li r4, 0x2
    b @L_801247CC
@L_80124748:
    mr r3, r24
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r4, r3, 16
    li r3, 0x0
    li r5, 0x13
    li r6, 0x0
    bl fn_8012640C
    clrlwi r21, r3, 16
    li r3, 0x0
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_80124790
    li r4, 0x0
    b @L_801247CC
@L_80124790:
    li r3, 0x1
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_801247AC
    li r4, 0x1
    b @L_801247CC
@L_801247AC:
    li r3, 0x2
    bl fn_80131574
    clrlwi r0, r3, 24
    cmpw r21, r0
    bne @L_801247C8
    li r4, 0x2
    b @L_801247CC
@L_801247C8:
    li r4, -0x1
@L_801247CC:
    extsb. r0, r4
    bge @L_801247EC
    clrlwi r0, r31, 24
    cmplw r22, r0
    ble @L_801247E8
    li r4, 0x1
    b @L_801247EC
@L_801247E8:
    li r4, 0x0
@L_801247EC:
    clrlwi r3, r23, 24
    clrlwi r0, r4, 24
    cmplw r3, r0
    bne @L_8012444C
@L_801247FC:
    extsb. r0, r26
    blt @L_80124828
    lis r3, 0x51ec
    subi r0, r3, 0x7ae1
    mulhwu r0, r0, r31
    srwi r0, r0, 3
    mulli r0, r0, 0x19
    subf r0, r0, r31
    clrlwi r0, r0, 24
    cmpw r29, r0
    bne @L_8012444C
@L_80124828:
    extsb. r0, r27
    blt @L_80124890
    extsb. r0, r27
    beq @L_80124864
    srwi r0, r31, 16
    clrlwi r4, r31, 16
    xor r0, r0, r28
    li r3, 0x8
    xor r0, r4, r0
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi. r0, r0, 31
    beq @L_8012444C
    b @L_80124890
@L_80124864:
    srwi r0, r31, 16
    clrlwi r4, r31, 16
    xor r0, r0, r28
    li r3, 0x8
    xor r0, r4, r0
    xor r0, r3, r0
    cntlzw r0, r0
    slw r0, r3, r0
    srwi r0, r0, 31
    cmplwi r0, 0x1
    beq @L_8012444C
@L_80124890:
    mr r3, r31
    b @L_801248B0
@L_80124898:
    srwi r3, r7, 16
    clrlwi r0, r7, 16
    extsb r30, r25
    extsb r29, r26
    xor r28, r3, r0
    b @L_8012444C
@L_801248B0:
    lmw r21, 0x14(r1)
    lwz r0, 0x44(r1)
    mtlr r0
    addi r1, r1, 0x40
    blr
