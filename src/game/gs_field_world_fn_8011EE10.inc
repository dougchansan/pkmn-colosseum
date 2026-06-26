    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE20
    li r3, 0x0
    blr
@L_8011EE20:
    lwz r3, 0xe0(r3)
    blr
