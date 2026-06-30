    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    lfs f3, lbl_8047B8A8@sda21(r0)
    lis r3, lbl_803A2058@ha
    li r0, 0x0
    lfs f2, lbl_8047B8AC@sda21(r0)
    addi r6, r3, lbl_803A2058@l
    lfs f1, lbl_8047B8E8@sda21(r0)
    lfs f0, lbl_8047B8EC@sda21(r0)
    lis r3, lbl_803A204C@ha
    addi r5, r3, lbl_803A204C@l
    lis r3, 0xc6a
    stw r0, lbl_8047A368@sda21(r0)
    addi r3, r3, 0x1000
    li r4, 0x0
    stw r0, lbl_8047A36C@sda21(r0)
    stw r0, lbl_8047A370@sda21(r0)
    stfs f3, lbl_80478898@sda21(r0)
    stfs f2, 0x0(r6)
    stfs f2, 0x4(r6)
    stfs f2, 0x8(r6)
    stfs f2, 0x0(r5)
    stfs f2, 0x4(r5)
    stfs f2, 0x8(r5)
    stfs f2, lbl_8047A374@sda21(r0)
    stfs f1, lbl_8047A378@sda21(r0)
    stfs f0, lbl_8047A37C@sda21(r0)
    stb r0, lbl_8047A380@sda21(r0)
    stw r0, lbl_8047A384@sda21(r0)
    stw r0, lbl_8047A388@sda21(r0)
    stw r0, lbl_8047A390@sda21(r0)
    bl fn_801CB954
    lis r3, 0xc6a
    li r4, 0x0
    addi r3, r3, 0x1001
    bl fn_801CB954
    lis r3, 0xc6a
    li r4, 0x0
    addi r3, r3, 0x1002
    bl fn_801CB954
    bl fn_8011394C
    cmplwi r3, 0x7b
    stw r3, lbl_8047A3AC@sda21(r0)
    bne @L_80025A6C
    li r0, 0x66
    stw r0, lbl_8047A3AC@sda21(r0)
@L_80025A6C:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr

