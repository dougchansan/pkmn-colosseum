    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    bne @L_8011ED34
    li r3, 0x0
    b @L_8011ED58
@L_8011ED34:
    bne @L_8011ED40
    li r3, 0x0
    b @L_8011ED44
@L_8011ED40:
    lhz r3, 0xd8(r3)
@L_8011ED44:
    clrlwi. r0, r3, 16
    bne @L_8011ED54
    li r3, 0x0
    b @L_8011ED58
@L_8011ED54:
    bl fn_801EEEB8
@L_8011ED58:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
