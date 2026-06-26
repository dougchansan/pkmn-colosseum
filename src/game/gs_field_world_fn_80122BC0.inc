    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    clrlwi. r0, r4, 16
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r4
    stw r29, 0x14(r1)
    mr r29, r3
    bne @L_80122BF0
    li r3, 0x0
    b @L_80122C48
@L_80122BF0:
    cmplwi r29, 0x0
    bne @L_80122C00
    li r3, 0x0
    b @L_80122C48
@L_80122C00:
    li r4, 0x0
    li r5, 0x83
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    mr r3, r29
    li r4, 0x0
    li r5, 0x87
    li r6, 0x0
    bl fn_8012640C
    clrlwi r3, r3, 16
    clrlwi r0, r30, 16
    divw r0, r3, r0
    srwi r3, r31, 31
    srawi r4, r0, 31
    subfc r0, r31, r0
    adde r0, r4, r3
    clrlwi r3, r0, 24
@L_80122C48:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
