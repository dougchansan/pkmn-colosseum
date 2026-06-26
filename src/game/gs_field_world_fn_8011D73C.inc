    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D74C
    li r3, 0x0
    b @L_8011D750
@L_8011D74C:
    addi r3, r3, 0xbd
@L_8011D750:
    cmplwi r3, 0x0
    beqlr
    stb r4, 0x0(r3)
    blr
