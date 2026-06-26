    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C950
    li r3, 0x0
    blr
@L_8011C950:
    lwz r3, 0x24(r3)
    blr
