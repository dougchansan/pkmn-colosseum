    nofralloc
    clrlwi r0, r3, 24
    li r3, 0x0
    cmplwi r0, 0xb
    bgtlr
    lis r4, jumptable_8035C260@ha
    slwi r0, r0, 2
    addi r4, r4, jumptable_8035C260@l
    lwzx r0, r4, r0
    mtctr r0
    bctr
    li r3, 0x0
    blr
    lis r3, 0x1
    subi r3, r3, 0x13b0
    blr
    lis r3, 0x1
    subi r3, r3, 0x13af
    blr
    lis r3, 0x1
    subi r3, r3, 0x13ae
    blr
    lis r3, 0x1
    subi r3, r3, 0x12f7
    blr
    lis r3, 0x1
    subi r3, r3, 0x13ad
    blr
    lis r3, 0x1
    subi r3, r3, 0x13ac
    blr
    lis r3, 0x1
    subi r3, r3, 0x13ab
    blr
    lis r3, 0x1
    subi r3, r3, 0x13aa
    blr
    lis r3, 0x1
    subi r3, r3, 0x13a9
    blr
    lis r3, 0x1
    subi r3, r3, 0x13a8
    blr
    lis r3, 0x1
    subi r3, r3, 0x13a7
    blr
