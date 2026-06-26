    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E6EC
    li r3, 0x0
    b @L_8011E6F0
@L_8011E6EC:
    addi r3, r3, 0x84
@L_8011E6F0:
    cmplwi r3, 0x0
    bne @L_8011E700
    li r3, 0x0
    blr
@L_8011E700:
    lhz r3, 0x4(r3)
    blr
