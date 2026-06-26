    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    li r5, 0x48
    stw r0, 0x34(r1)
    stmw r25, 0x14(r1)
    mr r31, r4
    mr r30, r3
    li r4, 0x0
    mr r3, r31
    bl memset
    mr r3, r30
    li r4, 0x0
    li r5, 0x77
    li r6, 0x0
    bl fn_8012640C
    stw r3, 0x0(r31)
    li r26, 0x0
    b @L_80120654
@L_801204F0:
    clrlwi r29, r26, 24
    cmplwi r30, 0x0
    mulli r3, r29, 0xc
    addi r25, r3, 0x4
    add r25, r31, r25
    bne @L_80120510
    li r0, 0x0
    b @L_8012055C
@L_80120510:
    mr r3, r30
    mr r6, r29
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x0
    bne @L_80120534
    li r0, 0x0
    b @L_8012055C
@L_80120534:
    mr r3, r30
    mr r6, r29
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    cmpwi r3, 0x163
    bne @L_80120558
    li r0, 0x0
    b @L_8012055C
@L_80120558:
    li r0, 0x1
@L_8012055C:
    clrlwi. r0, r0, 24
    bne @L_8012057C
    li r0, 0x0
    stw r0, 0x0(r25)
    stw r0, 0x4(r25)
    stb r0, 0xa(r25)
    stb r0, 0xb(r25)
    b @L_80120650
@L_8012057C:
    clrlwi r28, r26, 24
    mr r3, r30
    mr r6, r28
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    clrlwi r27, r3, 16
    li r3, 0x0
    mr r4, r27
    li r5, 0x3
    li r6, 0x0
    bl fn_8011BEB4
    clrlwi r29, r3, 16
    mr r4, r27
    li r3, 0x0
    li r5, 0x1
    li r6, 0x0
    bl fn_8011BEB4
    bl fn_800FA280
    stw r3, 0x0(r25)
    mr r3, r29
    bl fn_8010C4D4
    bl fn_800FA280
    stw r3, 0x4(r25)
    mr r3, r29
    bl fn_8010C46C
    cmplwi r30, 0x0
    sth r3, 0x8(r25)
    bne @L_801205F8
    li r3, 0x0
    b @L_80120634
@L_801205F8:
    addi r27, r28, 0x4
    mr r3, r30
    clrlwi r6, r27, 16
    li r4, 0x0
    li r5, 0x7f
    bl fn_8012640C
    clrlwi r29, r3, 16
    mr r3, r30
    clrlwi r6, r27, 16
    li r4, 0x0
    li r5, 0x81
    bl fn_8012640C
    clrlwi r4, r3, 24
    mr r3, r29
    bl fn_8011BB6C
@L_80120634:
    stb r3, 0xa(r25)
    mr r3, r30
    mr r6, r28
    li r4, 0x0
    li r5, 0x80
    bl fn_8012640C
    stb r3, 0xb(r25)
@L_80120650:
    addi r26, r26, 0x1
@L_80120654:
    clrlwi r0, r26, 24
    cmplwi r0, 0x4
    blt @L_801204F0
    lmw r25, 0x14(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
