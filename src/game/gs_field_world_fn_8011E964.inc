    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E974
    li r3, 0x0
    b @L_8011E978
@L_8011E974:
    addi r3, r3, 0xbd
@L_8011E978:
    cmplwi r3, 0x0
    bne @L_8011E988
    li r3, 0x0
    blr
@L_8011E988:
    lbz r3, 0xa(r3)
    blr
