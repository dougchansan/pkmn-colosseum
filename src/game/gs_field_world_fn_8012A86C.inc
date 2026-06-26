    nofralloc
    cmplwi r3, 0x0
    beqlr
    cmpwi r4, 0x0
    bge @L_8012A880
    li r4, 0x0
@L_8012A880:
    lis r5, 0x99
    subi r0, r5, 0x6981
    cmpw r4, r0
    ble @L_8012A894
    mr r4, r0
@L_8012A894:
    stw r4, 0xa84(r3)
    blr
