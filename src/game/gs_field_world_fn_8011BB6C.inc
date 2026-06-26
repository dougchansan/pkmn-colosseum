    nofralloc
    stwu r1, -0x10(r1)
    mflr r0
    li r5, 0x2
    li r6, 0x0
    stw r0, 0x14(r1)
    stw r31, 0xc(r1)
    mr r31, r4
    mr r4, r3
    li r3, 0x0
    bl fn_8011BEB4
    rlwinm r5, r3, 0, 24, 31
    lis r3, 0x51ec
    mulli r0, r5, 0x14
    rlwinm r4, r31, 0, 24, 31
    subi r3, r3, 0x7ae1
    mullw r0, r4, r0
    mulhw r0, r3, r0
    srawi r0, r0, 5
    rlwinm r3, r0, 1, 31, 31
    add r0, r0, r3
    add r0, r5, r0
    rlwinm r3, r0, 0, 24, 31
    lwz r31, 0xc(r1)
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
