    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmpwi r4, 0x639c
    blt @L_8011D8EC
    li r4, 0x639c
@L_8011D8EC:
    stw r4, 0xdc(r3)
    blr
