    nofralloc
    stwu r1, -0x30(r1)
    mflr r0
    stw r0, 0x34(r1)
    stmw r23, 0xc(r1)
    mr r28, r3
    mr r29, r4
    mr r23, r5
    rlwinm r30, r29, 0, 1, 15
    li r31, 0x0
    subis r0, r30, 0x7fff
    cmplwi r0, 0x0
    mr r26, r31
    beq @L_8011619C
    b @L_80116200
@L_8011619C:
    mr r25, r31
    mr r24, r31
    clrlwi r27, r29, 23
    b @L_801161DC
@L_801161AC:
    lwz r0, lbl_80478EBC(r13)
    add r31, r0, r24
    bl fn_800FF56C
    lhz r0, 0x4(r31)
    cmplw r0, r3
    bne @L_801161D4
    mr r0, r26
    addi r26, r26, 0x1
    cmplw r27, r0
    beq @L_801161EC
@L_801161D4:
    addi r24, r24, 0x1c
    addi r25, r25, 0x1
@L_801161DC:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r25, r0
    blt @L_801161AC
@L_801161EC:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r25, r0
    bne @L_80116200
    li r31, 0x0
@L_80116200:
    cmplwi r31, 0x0
    bne @L_80116210
    li r3, -0x1
    b @L_8011645C
@L_80116210:
    clrlwi r0, r23, 24
    cmpwi r0, 0x1
    beq @L_80116298
    bge @L_8011622C
    cmpwi r0, 0x0
    bge @L_80116238
    b @L_80116458
@L_8011622C:
    cmpwi r0, 0x3
    bge @L_80116458
    b @L_801162EC
@L_80116238:
    lbz r0, 0x0(r31)
    extrwi r0, r0, 3, 24
    cmpwi r0, 0x1
    beq @L_80116258
    blt @L_80116458
    cmpwi r0, 0x4
    bge @L_80116458
    b @L_80116284
@L_80116258:
    mr r3, r28
    mr r4, r29
    li r5, 0x2
    li r6, 0x0
    li r7, 0x1
    bl fn_8018B76C
    mr r3, r28
    mr r4, r29
    li r5, 0x1
    bl fn_8018C7C8
    b @L_80116458
@L_80116284:
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    bl fn_8018C1E8
    b @L_80116458
@L_80116298:
    lbz r0, 0x0(r31)
    extrwi r0, r0, 3, 24
    cmpwi r0, 0x1
    beq @L_801162B8
    blt @L_80116458
    cmpwi r0, 0x4
    bge @L_80116458
    b @L_801162D0
@L_801162B8:
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    li r6, 0x0
    li r7, 0x1
    bl fn_8018B76C
@L_801162D0:
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    li r6, 0x0
    li r7, 0x1
    bl fn_8018B76C
    b @L_80116458
@L_801162EC:
    lhz r3, 0x6(r31)
    cmplwi r3, 0x0
    beq @L_80116310
    bl fn_801902E0
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80116310
    li r3, 0x0
    b @L_8011645C
@L_80116310:
    lbz r0, 0x0(r31)
    extrwi r0, r0, 3, 24
    cmpwi r0, 0x1
    beq @L_8011632C
    blt @L_8011635C
    cmpwi r0, 0x4
    b @L_8011635C
@L_8011632C:
    mr r3, r28
    mr r4, r29
    li r5, 0x1
    li r6, 0x0
    li r7, 0x0
    bl fn_8018B76C
    li r3, 0x3c2
    bl fn_80166A28
    mr r3, r28
    mr r4, r29
    li r5, 0x1
    bl fn_8018B07C
@L_8011635C:
    subis r0, r30, 0x7fff
    li r30, 0x0
    cmplwi r0, 0x0
    mr r26, r30
    beq @L_80116374
    b @L_801163D8
@L_80116374:
    mr r27, r30
    mr r24, r30
    clrlwi r25, r29, 23
    b @L_801163B4
@L_80116384:
    lwz r0, lbl_80478EBC(r13)
    add r30, r0, r24
    bl fn_800FF56C
    lhz r0, 0x4(r30)
    cmplw r0, r3
    bne @L_801163AC
    mr r0, r26
    addi r26, r26, 0x1
    cmplw r25, r0
    beq @L_801163C4
@L_801163AC:
    addi r24, r24, 0x1c
    addi r27, r27, 0x1
@L_801163B4:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r27, r0
    blt @L_80116384
@L_801163C4:
    lwz r3, lbl_80478EB8(r13)
    lwz r0, 0x0(r3)
    cmplw r27, r0
    bne @L_801163D8
    li r30, 0x0
@L_801163D8:
    cmplwi r30, 0x0
    beq @L_8011643C
    lbz r0, 0x0(r30)
    extrwi r0, r0, 3, 24
    cmpwi r0, 0x1
    beq @L_80116400
    blt @L_8011643C
    cmpwi r0, 0x4
    bge @L_8011643C
    b @L_8011642C
@L_80116400:
    mr r3, r28
    mr r4, r29
    li r5, 0x2
    li r6, 0x0
    li r7, 0x1
    bl fn_8018B76C
    mr r3, r28
    mr r4, r29
    li r5, 0x1
    bl fn_8018C7C8
    b @L_8011643C
@L_8011642C:
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    bl fn_8018C1E8
@L_8011643C:
    lhz r3, 0x6(r31)
    bl fn_80190528
    lbz r0, 0x0(r31)
    lwz r4, 0xc(r31)
    lbz r5, 0x1(r31)
    extrwi r3, r0, 3, 27
    bl fn_80115E6C
@L_80116458:
    li r3, 0x0
@L_8011645C:
    lmw r23, 0xc(r1)
    lwz r0, 0x34(r1)
    mtlr r0
    addi r1, r1, 0x30
    blr
