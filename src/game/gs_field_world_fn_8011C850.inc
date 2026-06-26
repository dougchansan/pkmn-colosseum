    nofralloc
    cmplwi r3, 0x0
    bne @L_8011C860
    li r3, 0x0
    blr
@L_8011C860:
    lbz r3, 0xd(r3)
    blr
