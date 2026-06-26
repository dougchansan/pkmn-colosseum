    nofralloc
    cmplwi r3, 0x0
    bne @L_8011A040
    li r3, 0x0
    blr
@L_8011A040:
    lbz r3, 0x6(r3)
    blr
