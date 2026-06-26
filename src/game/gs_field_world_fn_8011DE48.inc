    nofralloc
    cmplwi r3, 0x0
    beqlr
    rlwinm r0, r4, 0, 24, 31
    cmplwi r0, 0x64
    ble @L_8011DE60
    li r4, 0x64
@L_8011DE60:
    stb r4, 0x60(r3)
    blr
