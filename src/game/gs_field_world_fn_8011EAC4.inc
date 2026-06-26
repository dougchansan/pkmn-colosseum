    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EAD4
    li r3, 0x0
    b @L_8011EAD8
@L_8011EAD4:
    addi r3, r3, 0xbd
@L_8011EAD8:
    cmplwi r3, 0x0
    bne @L_8011EAE8
    li r3, 0x0
    blr
@L_8011EAE8:
    lbz r3, 0x2(r3)
    blr
