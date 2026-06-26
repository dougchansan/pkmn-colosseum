    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E560
    li r3, 0x0
    b @L_8011E564
@L_8011E560:
    addi r3, r3, 0x90
@L_8011E564:
    cmplwi r3, 0x0
    bne @L_8011E574
    li r3, 0x0
    blr
@L_8011E574:
    lhz r3, 0xa(r3)
    blr
