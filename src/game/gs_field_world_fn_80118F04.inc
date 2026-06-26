    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr r30, r3
    lwz r0, 0x44(r3)
    cmpwi r0, 0x0
    bne @L_80118F5C
    addi r3, r30, 0x14
    bl fn_800E01D0
    lfs f0, 0x0(r31)
    lwz r3, 0x10(r30)
    stfs f0, 0x20(r3)
    lfs f0, 0x4(r31)
    lwz r3, 0x10(r30)
    stfs f0, 0x24(r3)
    lfs f0, 0x8(r31)
    lwz r3, 0x10(r30)
    stfs f0, 0x28(r3)
    b @L_80118F64
@L_80118F5C:
    addi r3, r30, 0x50
    bl fn_800E01D0
@L_80118F64:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
