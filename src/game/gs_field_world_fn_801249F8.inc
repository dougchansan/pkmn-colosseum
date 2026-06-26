    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr. r29, r3
    beq @L_80124A44
    clrlwi r31, r4, 16
    li r30, 0x0
    b @L_80124A38
@L_80124A24:
    clrlwi r0, r30, 16
    mulli r0, r0, 0x138
    add r3, r29, r0
    bl fn_80124A60
    addi r30, r30, 0x1
@L_80124A38:
    clrlwi r0, r30, 16
    cmplw r0, r31
    blt @L_80124A24
@L_80124A44:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
