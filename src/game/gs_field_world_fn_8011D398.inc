    nofralloc
    cmplwi r3, 0x0
    bne @L_8011D3A8
    li r3, 0x0
    b @L_8011D3AC
@L_8011D3A8:
    addi r3, r3, 0x84
@L_8011D3AC:
    cmplwi r3, 0x0
    beqlr
    sth r4, 0xa(r3)
    blr
