    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C598
    li r3, 0x0
    blr
@L_8011C598:
    rlwinm r0, r4, 0, 16, 31
    cmplwi r0, 0x3
    blt @L_8011C5AC
    li r3, 0x0
    blr
@L_8011C5AC:
    add r3, r3, r0
    lbz r3, 0x34(r3)
    blr
