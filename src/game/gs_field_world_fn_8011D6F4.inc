    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D704
    li r3, 0x0
    b @L_8011D708
@L_8011D704:
    addi r3, r3, 0xbd
@L_8011D708:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x2(r3)
    blr
