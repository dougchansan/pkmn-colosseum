    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D858
    li r3, 0x0
    b @L_8011D85C
@L_8011D858:
    addi r3, r3, 0xb2
@L_8011D85C:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x3(r3)
    blr
