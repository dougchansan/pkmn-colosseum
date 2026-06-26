    nofralloc
    cmplwi r3, 0x0
    bne @L_8011E860
    li r3, 0x0
    blr
@L_8011E860:
    lbz r3, 0xcd(r3)
    blr
