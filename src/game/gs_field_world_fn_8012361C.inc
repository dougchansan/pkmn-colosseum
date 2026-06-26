    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stmw r27, 0xc(r1)
    mr. r27, r3
    mr r30, r4
    mr r28, r5
    mr r29, r6
    bne @L_80123648
    li r3, -0x2
    b @L_801236E4
@L_80123648:
    beq @L_80123654
    cmplwi r29, 0x0
    bne @L_8012365C
@L_80123654:
    li r4, 0x0
    b @L_801236C8
@L_8012365C:
    li r4, 0x0
    li r5, 0x6e
    li r6, 0x0
    bl fn_8012640C
    clrlwi r31, r3, 16
    clrlwi r30, r30, 24
    b @L_801236B8
@L_80123678:
    mr r4, r31
    li r3, 0x0
    li r5, 0x1d
    bl fn_8012640C
    cmpw r30, r3
    bne @L_801236AC
    lbz r6, 0x0(r29)
    mr r4, r31
    li r3, 0x0
    li r5, 0x1e
    bl fn_8012640C
    clrlwi r4, r3, 16
    b @L_801236C8
@L_801236AC:
    lbz r3, 0x0(r29)
    addi r0, r3, 0x1
    stb r0, 0x0(r29)
@L_801236B8:
    lbz r6, 0x0(r29)
    cmplwi r6, 0x14
    blt @L_80123678
    li r4, 0x0
@L_801236C8:
    clrlwi. r0, r4, 16
    bne @L_801236D8
    li r3, -0x3
    b @L_801236E4
@L_801236D8:
    mr r3, r27
    mr r5, r28
    bl fn_801237B8
@L_801236E4:
    lmw r27, 0xc(r1)
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
