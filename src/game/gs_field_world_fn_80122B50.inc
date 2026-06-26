    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    bne @L_80122B74
    li r3, 0x0
    b @L_80122BAC
@L_80122B74:
    clrlwi. r0, r31, 16
    bne @L_80122B84
    li r3, 0x0
    b @L_80122BAC
@L_80122B84:
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 16
    clrlwi r0, r31, 16
    divw r0, r3, r0
    clrlwi. r3, r0, 16
    bne @L_80122BAC
    li r3, 0x1
@L_80122BAC:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
