    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stmw r27, 0xc(r1)
    mr r30, r4
    mr r29, r3
    mr r31, r5
    beq @L_8011A3D0
    bne @L_8011A2B0
    li r0, 0x0
    b @L_8011A368
@L_8011A2B0:
    mr r3, r30
    bl fn_80119E90
    clrlwi r28, r3, 24
    mr r3, r30
    bl fn_80119F10
    mr r27, r3
    mr r3, r30
    bl fn_80119ED0
    mr r6, r3
    mr r3, r27
    mr r4, r29
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011A2F8
    li r27, 0x0
    b @L_8011A300
@L_8011A2F8:
    clrlslwi r0, r28, 16, 4
    add r27, r3, r0
@L_8011A300:
    cmplwi r27, 0x0
    bne @L_8011A310
    li r0, 0x0
    b @L_8011A368
@L_8011A310:
    bne @L_8011A31C
    li r0, 0x0
    b @L_8011A338
@L_8011A31C:
    mr r3, r27
    bl fn_8011A090
    clrlwi. r0, r3, 16
    bne @L_8011A334
    li r0, 0x0
    b @L_8011A338
@L_8011A334:
    li r0, 0x1
@L_8011A338:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011A364
    mr r3, r27
    bl fn_8011A090
    clrlwi r3, r3, 16
    clrlwi r0, r30, 16
    cmplw r0, r3
    bne @L_8011A364
    li r0, 0x1
    b @L_8011A368
@L_8011A364:
    li r0, 0x0
@L_8011A368:
    clrlwi. r0, r0, 24
    beq @L_8011A3D0
    mr r3, r30
    bl fn_80119E90
    clrlwi r27, r3, 24
    mr r3, r30
    bl fn_80119F10
    mr r28, r3
    mr r3, r30
    bl fn_80119ED0
    mr r6, r3
    mr r3, r28
    mr r4, r29
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011A3B8
    li r3, 0x0
    b @L_8011A3C0
@L_8011A3B8:
    clrlslwi r0, r27, 16, 4
    add r3, r3, r0
@L_8011A3C0:
    cmplwi r3, 0x0
    beq @L_8011A3D0
    mr r4, r31
    bl fn_80119FA0
@L_8011A3D0:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
