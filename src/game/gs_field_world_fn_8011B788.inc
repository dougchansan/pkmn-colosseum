    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    mr r30, r3
    stw r29, 0x14(r1)
    stw r28, 0x10(r1)
    beq @L_8011B930
    bne @L_8011B7C0
    li r0, 0x0
    b @L_8011B878
@L_8011B7C0:
    mr r3, r31
    bl fn_80119E90
    clrlwi r29, r3, 24
    mr r3, r31
    bl fn_80119F10
    mr r28, r3
    mr r3, r31
    bl fn_80119ED0
    mr r6, r3
    mr r3, r28
    mr r4, r30
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011B808
    li r28, 0x0
    b @L_8011B810
@L_8011B808:
    clrlslwi r0, r29, 16, 4
    add r28, r3, r0
@L_8011B810:
    cmplwi r28, 0x0
    bne @L_8011B820
    li r0, 0x0
    b @L_8011B878
@L_8011B820:
    bne @L_8011B82C
    li r0, 0x0
    b @L_8011B848
@L_8011B82C:
    mr r3, r28
    bl fn_8011A090
    clrlwi. r0, r3, 16
    bne @L_8011B844
    li r0, 0x0
    b @L_8011B848
@L_8011B844:
    li r0, 0x1
@L_8011B848:
    clrlwi r0, r0, 24
    cmplwi r0, 0x1
    bne @L_8011B874
    mr r3, r28
    bl fn_8011A090
    clrlwi r3, r3, 16
    clrlwi r0, r31, 16
    cmplw r0, r3
    bne @L_8011B874
    li r0, 0x1
    b @L_8011B878
@L_8011B874:
    li r0, 0x0
@L_8011B878:
    clrlwi. r0, r0, 24
    beq @L_8011B930
    mr r3, r31
    bl fn_80119E90
    clrlwi r28, r3, 24
    mr r3, r31
    bl fn_80119F10
    mr r29, r3
    mr r3, r31
    bl fn_80119ED0
    mr r6, r3
    mr r3, r29
    mr r4, r30
    li r5, 0x0
    li r7, 0x0
    bl statusGetStatus
    cmplwi r3, 0x0
    bne @L_8011B8C8
    li r30, 0x0
    b @L_8011B8D0
@L_8011B8C8:
    clrlslwi r0, r28, 16, 4
    add r30, r3, r0
@L_8011B8D0:
    cmplwi r30, 0x0
    beq @L_8011B930
    beq @L_8011B930
    mr r3, r30
    li r4, 0x0
    bl fn_80119FF0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FE0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FD0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FC0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FB0
    mr r3, r30
    li r4, 0x0
    bl fn_80119FA0
    mr r3, r30
    li r4, 0x0
    bl fn_80119F90
@L_8011B930:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
