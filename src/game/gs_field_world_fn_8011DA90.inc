    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DAA0
    li r3, 0x0
    b @L_8011DAA4
@L_8011DAA0:
    addi r3, r3, 0x98
@L_8011DAA4:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DABC
    li r4, 0xff
@L_8011DABC:
    sth r4, 0x8(r3)
    blr
