    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    bl fn_800F9318
    mr r31, r3
    bl fn_800FF548
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    bne @L_80114D54
    cmplwi r31, 0x0
    beq @L_80114D54
    mr r3, r31
    bl fn_800F76E4
    bl fn_80112700
@L_80114D54:
    mr r3, r31
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
