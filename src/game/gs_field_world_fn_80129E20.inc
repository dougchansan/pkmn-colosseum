    nofralloc
    stwu r1, -0x150(r1)
    mflr r0
    cmplwi r4, 0x0
    stw r0, 0x154(r1)
    stw r31, 0x14c(r1)
    stw r30, 0x148(r1)
    stw r29, 0x144(r1)
    mr r29, r5
    stw r28, 0x140(r1)
    mr r28, r3
    bne @L_80129E54
    li r3, 0x6
    b @L_80129F00
@L_80129E54:
    addi r3, r1, 0x8
    bl fn_8011F5FC
    addic. r0, r1, 0x8
    bne @L_80129E6C
    li r31, 0x6
    b @L_80129EBC
@L_80129E6C:
    li r31, 0x0
    b @L_80129EAC
@L_80129E74:
    mr r3, r28
    clrlwi r5, r31, 24
    li r4, 0x3
    bl fn_8012A5B0
    mr r30, r3
    bl fn_80123FBC
    clrlwi r0, r3, 24
    cmplwi r0, 0x1
    beq @L_80129EA8
    mr r3, r30
    addi r4, r1, 0x8
    bl fn_8011F5FC
    b @L_80129EBC
@L_80129EA8:
    addi r31, r31, 0x1
@L_80129EAC:
    clrlwi r0, r31, 24
    cmplwi r0, 0x6
    blt @L_80129E74
    li r31, 0x6
@L_80129EBC:
    clrlwi r0, r31, 24
    cmplwi r0, 0x6
    blt @L_80129EFC
    clrlwi. r0, r29, 24
    bne @L_80129ED8
    li r3, -0x2
    b @L_80129F00
@L_80129ED8:
    addi r4, r1, 0x8
    li r3, 0x0
    li r5, -0x1
    bl fn_80134BC0
    subfic r0, r3, 0x1
    cntlzw r0, r0
    srwi r3, r0, 5
    subi r3, r3, 0x2
    b @L_80129F00
@L_80129EFC:
    extsh r3, r0
@L_80129F00:
    lwz r0, 0x154(r1)
    lwz r31, 0x14c(r1)
    lwz r30, 0x148(r1)
    lwz r29, 0x144(r1)
    lwz r28, 0x140(r1)
    mtlr r0
    addi r1, r1, 0x150
    blr
