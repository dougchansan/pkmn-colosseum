    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stmw r27, 0xc(r1)
    mr r27, r4
    mr r31, r3
    mr r28, r5
    beq @L_8011B430
    mr r3, r27
    bl fn_80119E90
    clrlwi r30, r3, 24
    mr r3, r27
    bl fn_80119F10
    mr r29, r3
    mr r3, r27
    bl fn_80119ED0
    mr r6, r3
    mr r3, r29
    mr r4, r31
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011B32C
    li r29, 0x0
    b @L_8011B334
@L_8011B32C:
    clrlslwi r0, r30, 16, 4
    add r29, r3, r0
@L_8011B334:
    cmplwi r29, 0x0
    beq @L_8011B430
    mr r3, r27
    bl fn_80119E50
    mr r30, r3
    clrlwi r0, r3, 24
    cmplwi r0, 0x4
    bne @L_8011B364
    mr r3, r29
    bl fn_8011A030
    mr r31, r3
    b @L_8011B368
@L_8011B364:
    li r31, 0x0
@L_8011B368:
    cmplwi r29, 0x0
    beq @L_8011B3C4
    mr r3, r29
    li r4, 0x0
    bl fn_80119FF0
    mr r3, r29
    li r4, 0x0
    bl fn_80119FE0
    mr r3, r29
    li r4, 0x0
    bl fn_80119FD0
    mr r3, r29
    li r4, 0x0
    bl fn_80119FC0
    mr r3, r29
    li r4, 0x0
    bl fn_80119FB0
    mr r3, r29
    li r4, 0x0
    bl fn_80119FA0
    mr r3, r29
    li r4, 0x0
    bl fn_80119F90
@L_8011B3C4:
    mr r3, r29
    mr r4, r27
    bl fn_80119FF0
    mr r3, r29
    mr r4, r28
    bl fn_80119FE0
    mr r3, r27
    bl fn_80119D90
    clrlwi r3, r3, 24
    bl fn_80101AC4
    extsb r4, r3
    mr r3, r29
    bl fn_80119FD0
    clrlwi r0, r30, 24
    cmplwi r0, 0x4
    bne @L_8011B430
    mr r3, r27
    addi r30, r31, 0x1
    bl fn_80119DD0
    clrlwi r0, r30, 24
    clrlwi r4, r3, 24
    cmplw r4, r0
    bge @L_8011B424
    mr r30, r3
@L_8011B424:
    mr r3, r29
    mr r4, r30
    bl fn_80119FB0
@L_8011B430:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
