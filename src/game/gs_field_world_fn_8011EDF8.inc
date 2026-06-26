    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE08
    li r3, 0x0
    blr
@L_8011EE08:
    lhz r3, 0xe4(r3)
    blr
