    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E610
    li r3, 0x0
    b @L_8011E614
@L_8011E610:
    addi r3, r3, 0x90
@L_8011E614:
    cmplwi r3, 0x0
    bne @L_8011E624
    li r3, 0x0
    blr
@L_8011E624:
    lhz r3, 0x2(r3)
    blr
