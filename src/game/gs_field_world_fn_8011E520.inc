    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E530
    li r3, 0x0
    blr
@L_8011E530:
    lbz r3, 0x0(r3)
    blr
