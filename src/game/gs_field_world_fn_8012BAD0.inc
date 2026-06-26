    nofralloc
    lis r8, lbl_80426BD0@ha
    addi r8, r8, lbl_80426BD0@l
    stw r3, 0x18c(r8)
    stw r4, 0x190(r8)
    stw r5, 0x194(r8)
    stw r6, 0x198(r8)
    stw r7, 0x19c(r8)
    blr
