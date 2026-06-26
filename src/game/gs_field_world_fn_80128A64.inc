    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    li r0, 0x0
    stmw r24, 0x10(r1)
    mr r28, r3
    mr r24, r4
    mr r29, r5
    mr r30, r6
    mr r31, r7
    sth r0, 0x8(r1)
    bl fn_80123FBC
    clrlwi. r0, r3, 24
    bne @L_80128AA8
    lis r3, 0x1
    subi r3, r3, 0x1
    b @L_80128CAC
@L_80128AA8:
    mr r3, r28
    bl fn_801230E0
    clrlwi. r0, r3, 16
    bne @L_80128AC0
    li r3, 0x0
    b @L_80128AC8
@L_80128AC0:
    bl fn_801440A0
    bl fn_80143F6C
@L_80128AC8:
    clrlwi r0, r3, 16
    cmplwi r0, 0x26
    bne @L_80128AE4
    lhz r0, 0x8(r1)
    li r3, 0x0
    sth r0, 0x0(r30)
    b @L_80128CAC
@L_80128AE4:
    cmpwi r24, 0x1
    beq @L_80128B1C
    bge @L_80128AFC
    cmpwi r24, 0x0
    bge @L_80128B08
    b @L_80128C9C
@L_80128AFC:
    cmpwi r24, 0x3
    bge @L_80128C9C
    b @L_80128BC4
@L_80128B08:
    mr r3, r28
    mr r5, r31
    addi r4, r1, 0x8
    bl fn_801286C8
    b @L_80128CA4
@L_80128B1C:
    mr r3, r28
    li r26, 0x0
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_80128B38
    li r24, 0x0
    b @L_80128B40
@L_80128B38:
    bl fn_8011E778
    mr r24, r3
@L_80128B40:
    cmplwi r24, 0x0
    bne @L_80128B54
    lis r3, 0x1
    subi r26, r3, 0x1
    b @L_80128BBC
@L_80128B54:
    li r25, 0x0
@L_80128B58:
    mr r3, r24
    clrlwi r4, r25, 16
    bl fn_8011E3FC
    mr r28, r3
    mr r3, r24
    clrlwi r4, r25, 16
    bl fn_8011E3B4
    clrlwi r0, r28, 24
    mr r27, r3
    cmpwi r0, 0x7
    beq @L_80128B88
    b @L_80128BB0
@L_80128B88:
    clrlwi r3, r27, 16
    clrlwi r0, r29, 16
    cmplw r3, r0
    bne @L_80128BB0
    mr r3, r24
    clrlwi r4, r25, 16
    bl fn_8011E36C
    stb r28, 0x0(r31)
    mr r26, r3
    sth r27, 0x2(r31)
@L_80128BB0:
    addi r25, r25, 0x1
    cmpwi r25, 0x5
    blt @L_80128B58
@L_80128BBC:
    mr r3, r26
    b @L_80128CA4
@L_80128BC4:
    mr r3, r28
    li r25, 0x0
    bl fn_8011F5C8
    clrlwi. r0, r3, 16
    bne @L_80128BE0
    li r24, 0x0
    b @L_80128BE8
@L_80128BE0:
    bl fn_8011E778
    mr r24, r3
@L_80128BE8:
    cmplwi r24, 0x0
    bne @L_80128BFC
    lis r3, 0x1
    subi r25, r3, 0x1
    b @L_80128C94
@L_80128BFC:
    li r26, 0x0
@L_80128C00:
    mr r3, r24
    clrlwi r4, r26, 16
    bl fn_8011E3FC
    mr r27, r3
    mr r3, r24
    clrlwi r4, r26, 16
    bl fn_8011E3B4
    clrlwi r0, r27, 24
    mr r29, r3
    cmpwi r0, 0x6
    beq @L_80128C58
    bge @L_80128C88
    cmpwi r0, 0x5
    bge @L_80128C3C
    b @L_80128C88
@L_80128C3C:
    mr r3, r24
    clrlwi r4, r26, 16
    bl fn_8011E36C
    stb r27, 0x0(r31)
    mr r25, r3
    sth r29, 0x2(r31)
    b @L_80128C88
@L_80128C58:
    mr r3, r28
    bl fn_8011F1A0
    clrlwi r3, r3, 16
    clrlwi r0, r29, 16
    cmplw r0, r3
    bne @L_80128C88
    mr r3, r24
    clrlwi r4, r26, 16
    bl fn_8011E36C
    stb r27, 0x0(r31)
    mr r25, r3
    sth r29, 0x2(r31)
@L_80128C88:
    addi r26, r26, 0x1
    cmpwi r26, 0x5
    blt @L_80128C00
@L_80128C94:
    mr r3, r25
    b @L_80128CA4
@L_80128C9C:
    lis r3, 0x1
    subi r3, r3, 0x1
@L_80128CA4:
    lhz r0, 0x8(r1)
    sth r0, 0x0(r30)
@L_80128CAC:
    lmw r24, 0x10(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
