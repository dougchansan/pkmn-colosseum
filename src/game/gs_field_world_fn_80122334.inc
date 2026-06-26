    nofralloc
    lis r4, 0x2492
    rlwinm r5, r3, 20, 26, 27
    rlwimi r5, r3, 14, 24, 25
    rlwimi r5, r3, 26, 28, 29
    addi r0, r4, 0x4925
    rlwimi r5, r3, 0, 30, 31
    mulhwu r3, r0, r5
    subf r0, r3, r5
    srwi r0, r0, 1
    add r0, r0, r3
    srwi r0, r0, 4
    mulli r0, r0, 0x1c
    subf r0, r0, r5
    clrlwi r3, r0, 24
    blr
