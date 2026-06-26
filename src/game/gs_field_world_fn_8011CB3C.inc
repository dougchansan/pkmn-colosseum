    nofralloc
    cmplwi r3, 0x0
    bne @L_8011CB4C
    li r3, 0x0
    blr
@L_8011CB4C:
    lwz r3, 0x8(r3)
    blr
