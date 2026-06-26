    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    beq @L_8012AA8C
    cmplwi r4, 0x0
    bne @L_8012AA84
    b @L_8012AA8C
@L_8012AA84:
    li r5, 0xb
    bl fn_800F9D24
@L_8012AA8C:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
