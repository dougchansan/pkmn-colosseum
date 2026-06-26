    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE38
    li r3, 0x0
    blr
@L_8011EE38:
    lwz r3, 0xdc(r3)
    blr
