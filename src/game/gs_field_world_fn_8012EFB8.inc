    nofralloc
    stwu r1, -0x10(r1)
    cmpwi r5, 0x0
    lwz r6, -26224(r2)
    lwz r0, -26220(r2)
    stw r6, 0x8(r1)
    stw r0, 0xc(r1)
    blt @L_8012EFDC
    cmpwi r5, 0x2
    blt @L_8012EFE4
@L_8012EFDC:
    li r3, 0x0
    b @L_8012F000
@L_8012EFE4:
    li r6, 0x0
    slwi r0, r5, 2
    addi r5, r1, 0x8
    stw r6, 0x0(r3)
    lwzx r0, r5, r0
    li r3, 0x1
    stw r0, 0x0(r4)
@L_8012F000:
    addi r1, r1, 0x10
    blr
