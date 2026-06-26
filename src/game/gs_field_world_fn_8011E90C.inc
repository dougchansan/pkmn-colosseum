    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E91C
    li r3, 0x0
    b @L_8011E920
@L_8011E91C:
    addi r3, r3, 0xbd
@L_8011E920:
    cmplwi r3, 0x0
    bne @L_8011E930
    li r3, 0x0
    blr
@L_8011E930:
    lbz r3, 0xc(r3)
    blr
