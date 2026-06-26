    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    lis r4, lbl_802727C8@ha
    addi r3, r1, 0x8
    addi r7, r4, lbl_802727C8@l
    li r4, 0x1
    lwz r6, 0x0(r7)
    lwz r5, 0x4(r7)
    lwz r0, 0x8(r7)
    stw r6, 0x8(r1)
    stw r5, 0xc(r1)
    stw r0, 0x10(r1)
    bl fn_800FF4D4
    lwz r0, 0x24(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
