    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CC34
    li r3, 0x0
    blr
@L_8011CC34:
    rlwinm r0, r4, 0, 24, 31
    cmplwi r0, 0x7
    blt @L_8011CC48
    li r3, 0x0
    blr
@L_8011CC48:
    add r3, r3, r0
    lbz r3, 0x18(r3)
    blr
