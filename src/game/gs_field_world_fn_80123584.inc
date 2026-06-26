    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r29, r4
    bne @L_801235B0
    li r3, 0x0
    b @L_80123600
@L_801235B0:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 16
    clrlwi r31, r29, 24
    li r29, 0x0
    b @L_801235F0
@L_801235D0:
    mr r4, r30
    clrlwi r6, r29, 24
    li r3, 0x0
    li r5, 0x1d
    bl fn_8012640C
    cmpw r31, r3
    beq @L_801235FC
    addi r29, r29, 0x1
@L_801235F0:
    clrlwi r0, r29, 24
    cmplwi r0, 0x14
    blt @L_801235D0
@L_801235FC:
    mr r3, r29
@L_80123600:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
