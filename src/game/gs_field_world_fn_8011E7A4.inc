    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E7B4
    li r3, 0x0
    blr
@L_8011E7B4:
    lwz r0, 0xf8(r3)
    rlwinm r3, r0, 0, 24, 31
    blr
