    nofralloc
    cmplwi r3, 0x0
    bne @L_8011EE68
    li r3, 0x0
    blr
@L_8011EE68:
    lhz r3, 0xb0(r3)
    blr
