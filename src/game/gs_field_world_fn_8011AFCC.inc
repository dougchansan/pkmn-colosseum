    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stmw r27, 0xc(r1)
    mr r30, r4
    mr r29, r3
    mr r31, r5
    beq @L_8011B11C
    bne @L_8011AFFC
    li r0, 0x0
    b @L_8011B0B4
@L_8011AFFC:
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
    bne @L_8011B044
    li r27, 0x0
    b @L_8011B04C
@L_8011B044:
    clrlslwi r0, r28, 16, 4
    add r27, r3, r0
@L_8011B04C:
    cmplwi r27, 0x0
    bne @L_8011B05C
    li r0, 0x0
    b @L_8011B0B4
@L_8011B05C:
    bne @L_8011B068
    li r0, 0x0
    b @L_8011B084
@L_8011B068:
    mr r3, r27
    bl fn_8011A090
    clrlwi. r0, r3, 16
    bne @L_8011B080
    li r0, 0x0
    b @L_8011B084
@L_8011B080:
    li r0, 0x1
@L_8011B084:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011B0B0
    mr r3, r27
    bl fn_8011A090
    clrlwi r3, r3, 16
    clrlwi r0, r30, 16
    cmplw r0, r3
    bne @L_8011B0B0
    li r0, 0x1
    b @L_8011B0B4
@L_8011B0B0:
    li r0, 0x0
@L_8011B0B4:
    clrlwi. r0, r0, 24
    beq @L_8011B11C
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
    bne @L_8011B104
    li r3, 0x0
    b @L_8011B10C
@L_8011B104:
    clrlslwi r0, r27, 16, 4
    add r3, r3, r0
@L_8011B10C:
    cmplwi r3, 0x0
    beq @L_8011B11C
    clrlwi r4, r31, 24
    bl fn_80119FB0
@L_8011B11C:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
