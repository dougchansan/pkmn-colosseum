    nofralloc
    lis r5, lbl_80426BD0@ha
    li r7, 0x0
    addi r5, r5, lbl_80426BD0@l
    lwz r0, 0x140(r5)
    cmplwi r0, 0x0
    beq @L_8012BE84
    addi r5, r5, 0x8
    li r7, 0x1
    lwz r0, 0x140(r5)
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x2
    addi r5, r5, 0x8
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x3
    addi r5, r5, 0x8
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x4
    addi r5, r5, 0x8
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x5
    addi r5, r5, 0x8
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x6
    addi r5, r5, 0x8
    cmplwi r0, 0x0
    beq @L_8012BE84
    lwz r0, 0x148(r5)
    li r7, 0x7
    cmplwi r0, 0x0
    beq @L_8012BE84
    li r7, 0x8
    @L_8012BE84:
    cmpwi r7, 0x8
    blt @L_8012BE94
    li r3, -0x1
    blr
    @L_8012BE94:
    lis r5, lbl_80426BD0@ha
    slwi r6, r7, 3
    addi r0, r5, lbl_80426BD0@l
    add r5, r0, r6
    stw r3, 0x140(r5)
    mr r3, r7
    stw r4, 0x144(r5)
    blr
