    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A028
    li r3, 0x0
    blr
@L_8011A028:
    lwz r3, 0x8(r3)
    blr
