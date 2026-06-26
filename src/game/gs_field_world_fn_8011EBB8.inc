    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EBC8
    li r3, 0x0
    b @L_8011EBCC
@L_8011EBC8:
    addi r3, r3, 0xb7
@L_8011EBCC:
    cmplwi r3, 0x0
    bne @L_8011EBDC
    li r3, 0x0
    blr
@L_8011EBDC:
    lbz r3, 0x2(r3)
    blr
