    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D3F0
    li r3, 0x0
    b @L_8011D3F4
@L_8011D3F0:
    addi r3, r3, 0x84
@L_8011D3F4:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0x6(r3)
    blr
