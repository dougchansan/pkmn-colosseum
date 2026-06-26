    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r4
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r29, r5
    beq @L_80123728
    cmplwi r29, 0x0
    bne @L_80123730
@L_80123728:
    li r3, 0x0
    b @L_8012379C
@L_80123730:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r30, r3, 16
    clrlwi r31, r31, 24
    b @L_8012378C
@L_8012374C:
    mr r4, r30
    li r3, 0x0
    li r5, 0x1d
    bl fn_8012640C
    cmpw r31, r3
    bne @L_80123780
    lbz r6, 0x0(r29)
    mr r4, r30
    li r3, 0x0
    li r5, 0x1e
    bl fn_8012640C
    clrlwi r3, r3, 16
    b @L_8012379C
@L_80123780:
    lbz r3, 0x0(r29)
    addi r0, r3, 0x1
    stb r0, 0x0(r29)
@L_8012378C:
    lbz r6, 0x0(r29)
    cmplwi r6, 0x14
    blt @L_8012374C
    li r3, 0x0
@L_8012379C:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
