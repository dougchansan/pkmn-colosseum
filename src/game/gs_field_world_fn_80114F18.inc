    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    stw r30, 0x8(r1)
    mr r30, r3
    mr r31, r4
    bl fn_800FF548
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80114F4C
    li r3, 0x0
    b @L_80114F6C
@L_80114F4C:
    mr r3, r30
    mr r4, r31
    bl fn_800F9318
    mr r31, r3
    cmplwi r31, 0x0
    beq @L_80114F68
    bl fn_800FC244
@L_80114F68:
    mr r3, r31
@L_80114F6C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
