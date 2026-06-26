    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r4, 0x1
    stw r0, 0x24(r1)
    li r0, 0x0
    stw r31, 0x1c(r1)
    addi r31, r1, 0x8
    mr r5, r31
    stw r30, 0x18(r1)
    mr r30, r3
    sth r0, 0x8(r1)
    bl fn_8012A450
    mr r3, r30
    li r4, 0x2
    li r5, 0x0
    bl fn_8012A450
    mr r3, r30
    li r4, 0x3
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x6
    bl fn_801249F8
    mr r3, r30
    li r4, 0x4
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x14
    bl fn_80142A88
    mr r3, r30
    li r4, 0x5
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x2b
    bl fn_80142A88
    mr r3, r30
    li r4, 0x6
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x10
    bl fn_80142A88
    mr r3, r30
    li r4, 0x7
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x40
    bl fn_80142A88
    mr r3, r30
    li r4, 0x8
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x2e
    bl fn_80142A88
    mr r3, r30
    li r4, 0x9
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0x3
    bl fn_80142A88
    mr r3, r30
    li r4, 0xb
    li r5, 0x2
    bl fn_8012A450
    mr r3, r30
    li r4, 0xc
    li r5, 0x0
    bl fn_8012A450
    mr r3, r30
    li r4, 0xd
    li r5, 0x0
    bl fn_8012A450
    mr r3, r30
    li r4, 0xe
    li r5, 0x0
    bl fn_8012A450
    mr r3, r30
    li r4, 0xf
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x10
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x11
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x12
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x13
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x14
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x15
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    li r4, 0x16
    li r5, 0x1
    bl fn_8012A450
    mr r3, r30
    mr r5, r31
    li r4, 0x17
    bl fn_8012A450
    mr r3, r30
    li r4, 0x18
    li r5, 0x0
    bl fn_8012A450
    mr r3, r30
    li r4, 0xa
    li r5, 0x0
    bl fn_8012A5B0
    li r4, 0xa
    bl fn_80142A88
    mr r3, r30
    li r4, 0x19
    li r5, 0x0
    bl fn_8012A450
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
