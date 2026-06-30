    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    mr r31, r3
    lfs f1, lbl_8047B8AC@sda21(r0)
    lfs f3, lbl_8047B8F8@sda21(r0)
    fmr f2, f1
    lfs f4, lbl_8047B8FC@sda21(r0)
    bl fn_800D9B58
    li r3, 0x1
    li r4, 0x6
    li r5, 0x7
    bl fn_800DA4C4
    li r3, 0x1
    li r4, 0x1
    li r5, 0x0
    bl fn_800DA2BC
    li r3, 0x0
    li r4, 0x1
    li r5, 0x1
    bl fn_800DA1E8
    li r3, 0x0
    li r4, 0x7
    li r5, 0x0
    li r6, 0x1
    li r7, 0x7
    li r8, 0x0
    bl fn_800DA100
    li r3, 0x0
    bl fn_800DA028
    li r3, 0x1
    bl fn_800D9ED8
    lfs f1, lbl_8047B8F0@sda21(r0)
    mr r3, r31
    lfs f2, lbl_8047B8F4@sda21(r0)
    addi r6, r1, 0x8
    lfs f0, lbl_8047B8AC@sda21(r0)
    li r4, 0x1
    stfs f1, 0x8(r1)
    li r5, 0x0
    lfs f1, lbl_8047A3A0@sda21(r0)
    stfs f2, 0xc(r1)
    lfs f2, lbl_8047B8B0@sda21(r0)
    stfs f0, 0x10(r1)
    lfs f3, lbl_8047B904@sda21(r0)
    bl fn_80025C1C
    lfs f2, lbl_8047A3A0@sda21(r0)
    lfs f1, lbl_8047B908@sda21(r0)
    lfs f0, lbl_8047B8AC@sda21(r0)
    fsubs f1, f2, f1
    fcmpo cr0, f1, f0
    stfs f1, lbl_8047A3A0@sda21(r0)
    bge @L_80025B5C
    stfs f0, lbl_8047A3A0@sda21(r0)
@L_80025B5C:
    lis r3, lbl_803A204C@ha
    lfs f0, lbl_8047B8AC@sda21(r0)
    addi r3, r3, lbl_803A204C@l
    lfs f4, lbl_8047B8E0@sda21(r0)
    lfs f2, 0x0(r3)
    addi r6, r1, 0x8
    lfs f1, 0x4(r3)
    li r4, 0x1
    fadds f3, f4, f2
    lfs f5, lbl_8047B8F8@sda21(r0)
    fadds f1, f4, f1
    lfs f2, lbl_8047B8FC@sda21(r0)
    stfs f0, 0x10(r1)
    li r5, 0x1
    fsubs f3, f5, f3
    lwz r3, lbl_8047A388@sda21(r0)
    fsubs f0, f2, f1
    lfs f1, lbl_8047A3A4@sda21(r0)
    lfs f2, lbl_8047B8B0@sda21(r0)
    stfs f3, 0x8(r1)
    lfs f3, lbl_8047B8E4@sda21(r0)
    stfs f0, 0xc(r1)
    bl fn_80025C1C
    lfs f2, lbl_8047A3A4@sda21(r0)
    lfs f1, lbl_8047B8C8@sda21(r0)
    lfs f0, lbl_8047B8AC@sda21(r0)
    fsubs f1, f2, f1
    fcmpo cr0, f1, f0
    stfs f1, lbl_8047A3A4@sda21(r0)
    bge @L_80025BD8
    stfs f0, lbl_8047A3A4@sda21(r0)
@L_80025BD8:
    lfs f0, lbl_8047A3A4@sda21(r0)
    lfs f1, lbl_8047B8AC@sda21(r0)
    fcmpo cr0, f0, f1
    cror eq, lt, eq
    bne @L_80025C04
    lfs f0, lbl_8047A3A0@sda21(r0)
    fcmpo cr0, f0, f1
    cror eq, lt, eq
    bne @L_80025C04
    li r3, 0x0
    b @L_80025C08
@L_80025C04:
    li r3, 0x1
@L_80025C08:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr

