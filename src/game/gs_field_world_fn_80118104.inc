    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r4
    stw r29, 0x14(r1)
    mr r29, r3
    bl GScameraGetActiveCamera
    cmplwi r3, 0x0
    beq @L_80118194
    lwz r3, 0xc(r3)
    bl psSetBillboardCamera
    cmpwi r29, 0x1000
    beq @L_80118164
    bge @L_80118150
    cmpwi r29, 0x10
    beq @L_8011815C
    b @L_80118170
@L_80118150:
    cmpwi r29, 0x2000
    beq @L_8011816C
    b @L_80118170
@L_8011815C:
    li r31, 0x0
    b @L_80118170
@L_80118164:
    li r31, 0x1
    b @L_80118170
@L_8011816C:
    li r31, 0x2
@L_80118170:
    clrlwi. r0, r30, 24
    bne @L_80118188
    mr r4, r31
    li r3, 0x1
    bl fn_8016AB94
    b @L_80118194
@L_80118188:
    mr r4, r31
    li r3, 0x2
    bl fn_8016AB94
@L_80118194:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
