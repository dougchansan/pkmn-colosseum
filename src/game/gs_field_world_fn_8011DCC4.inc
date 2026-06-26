    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r5
    li r5, 0x0
    stw r29, 0x14(r1)
    mr r29, r4
    stw r28, 0x10(r1)
    mr r28, r3
    bl fn_8011F260
    mr. r31, r3
    beq @L_8011DD60
    clrlwi r0, r30, 24
    cmplwi r0, 0x3
    ble @L_8011DD0C
    li r30, 0x3
@L_8011DD0C:
    mr r3, r28
    mr r4, r29
    bl fn_80123CD4
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    bne @L_8011DD5C
    mr r3, r28
    mr r4, r29
    li r5, 0x1
    bl fn_8011F260
    cmplwi r3, 0x0
    bne @L_8011DD44
    li r3, 0x0
    b @L_8011DD48
@L_8011DD44:
    lhz r3, 0x0(r3)
@L_8011DD48:
    bl fn_8011CA34
    bl fn_8011CA04
    clrlwi r0, r3, 24
    cmplwi r0, 0x4
    ble @L_8011DD60
@L_8011DD5C:
    stb r30, 0x3(r31)
@L_8011DD60:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    lwz r28, 0x10(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
