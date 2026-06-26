    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D780
    li r3, 0x0
    b @L_8011D784
@L_8011D780:
    addi r3, r3, 0xb7
@L_8011D784:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x4(r3)
    blr
