    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    beq @L_8011FC04
    lfs f0, -26284(r2)
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    fmuls f0, f0, f1
    fctiwz f0, f0
    stfd f0, 0x8(r1)
    lwz r7, 0xc(r1)
    bl fn_801254B4
@L_8011FC04:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
