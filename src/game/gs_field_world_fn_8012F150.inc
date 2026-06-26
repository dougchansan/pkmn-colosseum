    nofralloc
    cmpwi r3, 0x0
    blt @L_8012F160
    cmpwi r3, 0x2
    blt @L_8012F168
@L_8012F160:
    li r3, 0x0
    blr
@L_8012F168:
    lis r4, lbl_80426BD0@ha
    lwzu r0, lbl_80426BD0@l(r4)
    cmpw r3, r0
    bne @L_8012F180
    li r3, 0x0
    blr
@L_8012F180:
    slwi r0, r3, 5
    lfs f1, -26216(r2)
    add r3, r4, r0
    lfs f2, -26060(r2)
    lhz r0, 0x4(r3)
    li r5, 0x0
    rlwinm r0, r0, 0, 16, 30
    sth r0, 0x4(r3)
    lwz r0, 0x0(r4)
    slwi r0, r0, 5
    add r3, r4, r0
    stfs f1, 0x8(r3)
    lhz r0, 0x4(r4)
    clrlwi. r0, r0, 31
    beq @L_8012F1D0
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F1D0
    stfs f2, 0x8(r4)
    fadds f2, f2, f2
@L_8012F1D0:
    lhz r0, 0x24(r4)
    addi r3, r4, 0x20
    li r5, 0x1
    clrlwi. r0, r0, 31
    beq @L_8012F1F4
    lwz r0, 0x0(r4)
    cmpw r0, r5
    beq @L_8012F1F4
    stfs f2, 0x8(r3)
@L_8012F1F4:
    li r3, 0x1
    blr
