    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    li r4, 0x1
    li r5, 0x0
    stw r0, 0x24(r1)
    li r0, 0x0
    stw r31, 0x1c(r1)
    mr r31, r3
    sth r0, 0x8(r1)
    bl fn_8012A5B0
    addi r4, r1, 0x8
    bl GScharCmp
    cmpwi r3, 0x0
    bne @L_8012A170
    li r3, 0x0
    b @L_8012A190
@L_8012A170:
    mr r3, r31
    li r4, 0xb
    li r5, 0x0
    bl fn_8012A5B0
    subfic r4, r3, 0x2
    subi r0, r3, 0x2
    or r0, r4, r0
    srwi r3, r0, 31
@L_8012A190:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
