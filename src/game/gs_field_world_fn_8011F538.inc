    nofralloc
    cmplwi r3, 0x0
    bne @L_8011F548
    li r3, 0x0
    blr
@L_8011F548:
    lbz r3, 0x10(r3)
    blr
