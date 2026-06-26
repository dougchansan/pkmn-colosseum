    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    stw r30, 0x8(r1)
    mr r30, r3
    mr r3, r31
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0x7c
    beq @L_801215C0
    mr r3, r31
    bl fn_80119ED0
    clrlwi r0, r3, 16
    cmplwi r0, 0xc8
    beq @L_801215C0
    li r3, 0x0
    b @L_801215CC
@L_801215C0:
    mr r3, r30
    mr r4, r31
    bl fn_8011A3E4
@L_801215CC:
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    lwz r30, 0x8(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
