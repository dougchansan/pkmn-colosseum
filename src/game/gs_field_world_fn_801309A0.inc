    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    li r4, 0x1
    stw r0, 0x154(r1)
    stw r31, 0x14c(r1)
    mr r31, r3
    li r3, 0x0
    bl fn_80135938
    mr r6, r3
    addi r3, r1, 0x8
    li r4, 0x161
    li r5, 0xd
    bl fn_801240C4
    addi r3, r1, 0x8
    li r4, 0x0
    li r5, 0x99
    li r6, 0x0
    li r7, 0x46
    bl fn_801254B4
    li r3, 0x12ac
    bl fn_800FA280
    lis r4, 0x1
    mr r9, r3
    subi r8, r4, 0x6ee3
    addi r3, r1, 0x8
    li r4, 0xfe
    li r5, 0xd
    li r6, 0x4
    li r7, 0x0
    bl fn_80123EF0
    lis r4, 0x1
    addi r3, r1, 0x8
    subi r7, r4, 0x6ee3
    li r4, -0x1
    li r5, -0x1
    li r6, 0x0
    bl fn_80124410
    mr r0, r3
    addi r3, r1, 0x8
    mr r7, r0
    li r4, 0x0
    li r5, 0x6f
    li r6, 0x0
    bl fn_801254B4
    addi r3, r1, 0x8
    bl fn_8012546C
    addi r4, r1, 0x8
    li r3, 0x0
    bl fn_8025FF9C
    mr r3, r31
    addi r4, r1, 0x8
    li r5, 0x1
    bl fn_80129E20
    lwz r0, 0x154(r1)
    lwz r31, 0x14c(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr
