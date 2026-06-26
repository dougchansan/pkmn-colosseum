    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E58C
    li r3, 0x0
    b @L_8011E590
@L_8011E58C:
    addi r3, r3, 0x90
@L_8011E590:
    cmplwi r3, 0x0
    bne @L_8011E5A0
    li r3, 0x0
    blr
@L_8011E5A0:
    lhz r3, 0x8(r3)
    blr
