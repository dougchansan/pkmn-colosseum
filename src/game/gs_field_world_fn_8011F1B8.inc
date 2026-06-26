    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r5, 0x1
    stw r0, 0x14(r1)
    bl fn_8011F260
    cmplwi r3, 0x0
    bne @L_8011F1DC
    li r3, 0x0
    b @L_8011F1E0
@L_8011F1DC:
    lbz r3, 0x3(r3)
@L_8011F1E0:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
