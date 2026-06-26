    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EAA8
    li r3, 0x0
    b @L_8011EAAC
@L_8011EAA8:
    addi r3, r3, 0xbd
@L_8011EAAC:
    cmplwi r3, 0x0
    bne @L_8011EABC
    li r3, 0x0
    blr
@L_8011EABC:
    lbz r3, 0x3(r3)
    blr
