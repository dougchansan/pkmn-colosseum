    nofralloc
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0xff
    ble @L_8011DE80
    li r4, 0xff
@L_8011DE80:
    sth r4, 0xe4(r3)
    blr
