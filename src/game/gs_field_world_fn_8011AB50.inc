    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stmw r27, 0xc(r1)
    mr r30, r4
    mr r29, r3
    mr r31, r5
    beq @L_8011ACA0
    bne @L_8011AB80
    li r0, 0x0
    b @L_8011AC38
@L_8011AB80:
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
    bne @L_8011ABC8
    li r27, 0x0
    b @L_8011ABD0
@L_8011ABC8:
    clrlslwi r0, r28, 16, 4
    add r27, r3, r0
@L_8011ABD0:
    cmplwi r27, 0x0
    bne @L_8011ABE0
    li r0, 0x0
    b @L_8011AC38
@L_8011ABE0:
    bne @L_8011ABEC
    li r0, 0x0
    b @L_8011AC08
@L_8011ABEC:
    mr r3, r27
    bl fn_8011A090
    clrlwi. r0, r3, 16
    bne @L_8011AC04
    li r0, 0x0
    b @L_8011AC08
@L_8011AC04:
    li r0, 0x1
@L_8011AC08:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011AC34
    mr r3, r27
    bl fn_8011A090
    clrlwi r3, r3, 16
    clrlwi r0, r30, 16
    cmplw r0, r3
    bne @L_8011AC34
    li r0, 0x1
    b @L_8011AC38
@L_8011AC34:
    li r0, 0x0
@L_8011AC38:
    clrlwi. r0, r0, 24
    beq @L_8011ACA0
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
    bne @L_8011AC88
    li r3, 0x0
    b @L_8011AC90
@L_8011AC88:
    clrlslwi r0, r27, 16, 4
    add r3, r3, r0
@L_8011AC90:
    cmplwi r3, 0x0
    beq @L_8011ACA0
    mr r4, r31
    bl fn_80119FC0
@L_8011ACA0:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
