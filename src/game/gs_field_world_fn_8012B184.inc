    nofralloc
    cmpwi r3, 0x0
    bltlr
    lis r4, lbl_80426BD0@ha
    addi r4, r4, lbl_80426BD0@l
    stw r3, 0x188(r4)
    blr
