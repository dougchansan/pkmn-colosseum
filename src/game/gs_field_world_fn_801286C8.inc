    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r22, 0x8(r1)
    mr r28, r3
    mr r29, r4
    mr r30, r5
    li r23, 0x0
    li r31, 0x0
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_80128700
    li r25, 0x0
    b @L_80128708
@L_80128700:
    bl fn_8011E778
    mr r25, r3
@L_80128708:
    cmplwi r25, 0x0
    bne @L_8012871C
    lis r3, 0x1
    subi r3, r3, 0x1
    b @L_80128A50
@L_8012871C:
    li r24, 0x0
@L_80128720:
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E3FC
    mr r26, r3
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E3B4
    clrlwi r0, r26, 24
    mr r27, r3
    cmplwi r0, 0xf
    bgt @L_80128A3C
    lis r3, jumptable_80363468@ha
    slwi r0, r0, 2
    addi r3, r3, jumptable_80363468@l
    lwzx r0, r3, r0
    mtctr r0
    bctr
    mr r3, r28
    bl fn_8011EE58
    clrlwi r0, r3, 16
    cmplwi r0, 0xdc
    blt @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011EE58
    b @L_80128A3C
    mr r3, r28
    bl fn_8011EE58
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r28
    bl fn_8011F130
    clrlwi r22, r3, 16
    mr r3, r28
    bl fn_8011F104
    clrlwi r0, r3, 16
    cmplw r0, r22
    bge @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r28
    bl fn_8011F130
    clrlwi r22, r3, 16
    mr r3, r28
    bl fn_8011F104
    clrlwi r0, r3, 16
    cmplw r0, r22
    bne @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r28
    bl fn_8011F130
    clrlwi r22, r3, 16
    mr r3, r28
    bl fn_8011F104
    clrlwi r0, r3, 16
    cmplw r0, r22
    ble @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r28
    bl fn_8011F5B0
    lis r4, 0xcccd
    srwi r3, r3, 16
    subi r0, r4, 0x3333
    mulhwu r0, r0, r3
    srwi r0, r0, 3
    mulli r0, r0, 0xa
    subf r0, r0, r3
    cmplwi r0, 0x5
    bge @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r28
    bl fn_8011F5B0
    lis r4, 0xcccd
    srwi r3, r3, 16
    subi r0, r4, 0x3333
    mulhwu r0, r0, r3
    srwi r0, r0, 3
    mulli r0, r0, 0xa
    subf r0, r0, r3
    cmplwi r0, 0x5
    blt @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
    b @L_80128A3C
    mr r3, r28
    bl fn_8011F4A8
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    li r22, 0x0
@L_801289B8:
    clrlwi r5, r22, 16
    li r3, 0x0
    li r4, 0x3
    bl fn_8012A5B0
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    beq @L_801289E0
    addi r22, r22, 0x1
    cmpwi r22, 0x6
    blt @L_801289B8
@L_801289E0:
    cmpwi r22, 0x6
    li r0, -0x1
    bge @L_801289F0
    extsb r0, r22
@L_801289F0:
    extsb. r0, r0
    blt @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    mr r31, r3
    b @L_80128A3C
    mr r3, r28
    bl fn_8011ECC0
    clrlwi r3, r3, 24
    clrlwi r0, r27, 16
    cmpw r3, r0
    blt @L_80128A3C
    mr r3, r25
    clrlwi r4, r24, 16
    bl fn_8011E36C
    stb r26, 0x0(r30)
    mr r23, r3
    sth r27, 0x2(r30)
@L_80128A3C:
    addi r24, r24, 0x1
    cmpwi r24, 0x5
    blt @L_80128720
    sth r31, 0x0(r29)
    mr r3, r23
@L_80128A50:
    lmw r22, 0x8(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
