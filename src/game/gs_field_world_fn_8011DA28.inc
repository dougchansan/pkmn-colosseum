    nofralloc
    cmplwi r3, 0x0
    bne @L_8011DA38
    li r3, 0x0
    b @L_8011DA3C
@L_8011DA38:
    addi r3, r3, 0xa4
@L_8011DA3C:
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x1f
    ble @L_8011DA54
    li r4, 0x1f
@L_8011DA54:
    sth r4, 0x0(r3)
    blr
