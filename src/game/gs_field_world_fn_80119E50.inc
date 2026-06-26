    nofralloc
    lwz r0, lbl_80478B48(r13)
    clrlwi r3, r3, 16
    cmplw r3, r0
    blt @L_80119E68
    li r3, 0x0
    b @L_80119E78
@L_80119E68:
    mulli r4, r3, 0x14
    lis r3, lbl_8035BBA8@ha
    addi r0, r3, lbl_8035BBA8@l
    add r3, r0, r4
@L_80119E78:
    cmplwi r3, 0x0
    bne @L_80119E88
    li r3, 0x0
    blr
@L_80119E88:
    lbz r3, 0x2(r3)
    blr
