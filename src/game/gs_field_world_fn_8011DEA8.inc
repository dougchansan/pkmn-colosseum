    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    beq @L_8011DED4
    cmplwi r4, 0x0
    bne @L_8011DEC8
    b @L_8011DED4
@L_8011DEC8:
    addi r3, r3, 0x44
    li r5, 0xb
    bl fn_800F9D24
@L_8011DED4:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
