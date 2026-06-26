    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EB58
    li r3, 0x0
    blr
@L_8011EB58:
    lbz r3, 0xbc(r3)
    blr
