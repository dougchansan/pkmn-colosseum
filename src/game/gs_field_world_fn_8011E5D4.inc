    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E5E4
    li r3, 0x0
    b @L_8011E5E8
@L_8011E5E4:
    addi r3, r3, 0x90
@L_8011E5E8:
    cmplwi r3, 0x0
    bne @L_8011E5F8
    li r3, 0x0
    blr
@L_8011E5F8:
    lhz r3, 0x4(r3)
    blr
