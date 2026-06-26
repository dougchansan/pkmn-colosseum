    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D650
    li r3, 0x0
    b @L_8011D654
@L_8011D650:
    addi r3, r3, 0xbd
@L_8011D654:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x7(r3)
    blr
