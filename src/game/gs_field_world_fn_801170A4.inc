    nofralloc
    stwu r1, -0x20(r1)
    mflr r0
    stw r0, 0x24(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    stw r29, 0x14(r1)
    mr r29, r3
    mr r30, r4
    cmplwi r29, 0x0
    bne @L_801170D4
    li r3, 0x0
    b @L_80117100
@L_801170D4:
    bl fn_800FF56C
    bl fn_80115C48
    mr r31, r3
    bl fn_80115A80
    cmplw r29, r3
    beq @L_801170F4
    li r3, 0x0
    b @L_80117100
@L_801170F4:
    mr r3, r31
    mr r4, r30
    bl fn_80115684
@L_80117100:
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x20
    blr
