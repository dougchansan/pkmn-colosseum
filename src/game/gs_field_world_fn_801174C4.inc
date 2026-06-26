    nofralloc
    lwz r0, -23224(r13)
    li r3, 0x0
    cmplwi r0, 0x0
    beq @L_801174E4
    lwz r0, -23220(r13)
    cmplwi r0, 0x0
    beq @L_801174E4
    li r3, 0x1
@L_801174E4:
    rlwinm r3, r3, 0, 24, 31
    blr
