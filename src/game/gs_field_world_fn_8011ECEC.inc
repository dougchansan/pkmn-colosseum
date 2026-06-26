    nofralloc
    cmplwi r3, 0x0
    bne @L_8011ECFC
    li r3, 0x0
    b @L_8011ED00
@L_8011ECFC:
    addi r3, r3, 0xb2
@L_8011ED00:
    cmplwi r3, 0x0
    bne @L_8011ED10
    li r3, 0x0
    blr
@L_8011ED10:
    lbz r3, 0x0(r3)
    blr
