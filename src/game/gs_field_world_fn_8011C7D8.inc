    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C7E8
    li r3, 0x0
    blr
@L_8011C7E8:
    lwz r3, 0x30(r3)
    blr
