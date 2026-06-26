    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r3
    bl fn_800FF554
    clrlwi r0, r3, 24
    cmplwi r0, 0x0
    beq @L_80115150
    li r3, 0x0
    b @L_8011515C
@L_80115150:
    mr r3, r31
    bl fn_800F760C
    li r3, 0x1
@L_8011515C:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
