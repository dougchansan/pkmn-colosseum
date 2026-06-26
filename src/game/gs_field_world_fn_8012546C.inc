    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr. r31, r3
    beq @L_801254A0
    li r4, 0x0
    li r5, 0xc0
    li r6, 0x0
    bl fn_8012640C
    rlwinm r4, r3, 0, 24, 31
    mr r3, r31
    bl fn_8012795C
@L_801254A0:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
