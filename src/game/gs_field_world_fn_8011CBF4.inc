    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CC04
    li r3, 0x0
    blr
@L_8011CC04:
    rlwinm r0, r4, 0, 24, 31
    cmplwi r0, 0x7
    blt @L_8011CC18
    li r3, 0x0
    blr
@L_8011CC18:
    add r3, r3, r0
    lbz r3, 0x1f(r3)
    blr
