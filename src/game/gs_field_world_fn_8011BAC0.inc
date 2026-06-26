    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r3, 16
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r29, r3
    bne @L_8011BAFC
    cmplwi r0, 0x163
    bne @L_8011BAFC
    cmplwi r0, 0x165
    bne @L_8011BAFC
    li r3, 0x0
    b @L_8011BB50
@L_8011BAFC:
    clrlwi. r31, r4, 24
    bne @L_8011BB0C
    li r3, 0x0
    b @L_8011BB50
@L_8011BB0C:
    li r30, 0x0
    b @L_8011BB40
@L_8011BB14:
    mr r4, r29
    clrlwi r6, r30, 24
    li r3, 0x0
    li r5, 0x1a
    bl fn_8011BEB4
    clrlwi r0, r3, 24
    cmplw r31, r0
    bne @L_8011BB3C
    li r3, 0x1
    b @L_8011BB50
@L_8011BB3C:
    addi r30, r30, 0x1
@L_8011BB40:
    clrlwi r0, r30, 24
    cmplwi r0, 0x3
    blt @L_8011BB14
    li r3, 0x0
@L_8011BB50:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
