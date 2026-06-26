    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DA6C
    li r3, 0x0
    b @L_8011DA70
@L_8011DA6C:
    addi r3, r3, 0x98
@L_8011DA70:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DA88
    li r4, 0xff
@L_8011DA88:
    sth r4, 0xa(r3)
    blr
