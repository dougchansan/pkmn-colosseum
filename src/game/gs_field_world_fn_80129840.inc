    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    li r30, 0x0
    stw r29, 0x14(r1)
    mr r29, r3
    b @L_80129890
@L_80129864:
    mr r3, r29
    mr r5, r30
    li r4, 0x3
    bl fn_8012A5B0
    mr r31, r3
    bl fn_80123FBC
    rlwinm. r0, r3, 0, 24, 31
    beq @L_8012988C
    mr r3, r31
    bl fn_80120674
@L_8012988C:
    addi r30, r30, 0x1
@L_80129890:
    rlwinm r0, r30, 0, 16, 31
    cmplwi r0, 0x6
    blt @L_80129864
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
