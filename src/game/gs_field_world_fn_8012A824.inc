    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmpwi r4, 0x0
    bge @L_8012A838
    li r4, 0x0
@L_8012A838:
    lis r5, 0x99
    subi r0, r5, 0x6981
    cmpw r4, r0
    ble @L_8012A84C
    mr r4, r0
@L_8012A84C:
    stw r4, 0xa88(r3)
    blr
