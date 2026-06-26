    nofralloc
    cmpwi r3, 0x0
    blt @L_8012F12C
    cmpwi r3, 0x2
    blt @L_8012F134
@L_8012F12C:
    li r3, 0x0
    blr
@L_8012F134:
    lis r4, lbl_80426BD0@ha
    rlwinm r0, r3, 5, 0, 26
    addi r3, r4, lbl_80426BD0@l
    add r3, r3, r0
    lhz r0, 0x4(r3)
    rlwinm r3, r0, 0, 31, 31
    blr
