    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EB2C
    li r3, 0x0
    b @L_8011EB30
@L_8011EB2C:
    addi r3, r3, 0xbd
@L_8011EB30:
    cmplwi r3, 0x0
    bne @L_8011EB40
    li r3, 0x0
    blr
@L_8011EB40:
    lbz r3, 0x0(r3)
    blr
