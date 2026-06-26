    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r4, 0x0
    li r5, 0xc3
    stw r0, 0x14(r1)
    li r6, 0x0
    li r7, 0x0
    stw r31, 0xc(r1)
    mr r31, r3
    bl fn_801254B4
    cmplwi r31, 0x0
    beq @L_80125280
    mr r3, r31
    li r4, 0x0
    li r5, 0xc5
    li r6, 0x0
    li r7, -0x64
    bl fn_801254B4
@L_80125280:
    mr r3, r31
    li r4, 0x0
    li r5, 0xc6
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r31
    li r4, 0x0
    li r5, 0xc7
    li r6, 0x0
    li r7, 0x0
    bl fn_801254B4
    mr r3, r31
    li r4, 0x0
    li r5, 0xc8
    li r6, 0x0
    bl fn_8012640C
    li r4, 0x1
    bl fn_8011B950
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
