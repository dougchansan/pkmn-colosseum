    nofralloc
    cmplwi r3, 0x0
    beq @L_8011704C
    lbz r0, 0x0(r3)
    rlwinm r3, r0, 26, 31, 31
    blr
@L_8011704C:
    li r3, 0x0
    blr
