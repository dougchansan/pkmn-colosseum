    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E2BC
    li r3, 0x0
    blr
@L_8011E2BC:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x3a
    blt @L_8011E2D0
    li r3, 0x0
    blr
@L_8011E2D0:
    add r3, r3, r0
    lbz r3, 0x34(r3)
    blr
