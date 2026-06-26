    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    cmplwi r3, 0x0
    stw r0, 0x14(r1)
    bne @L_8011FC30
    lfs f1, -26288(r2)
    b @L_8011FC64
@L_8011FC30:
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    bl fn_8012640C
    xoris r3, r3, 0x8000
    lis r0, 0x4330
    stw r3, 0xc(r1)
    lfd f2, -26264(r2)
    stw r0, 0x8(r1)
    lfs f0, -26284(r2)
    lfd f1, 0x8(r1)
    fsubs f1, f1, f2
    fdivs f1, f1, f0
@L_8011FC64:
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
