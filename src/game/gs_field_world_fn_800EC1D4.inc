    nofralloc
/* 800EC1D4 000E91D4  80 03 00 00 */	lwz r0, 0x0(r3)
/* 800EC1D8 000E91D8  54 00 06 B0 */	rlwinm r0, r0, 0, 26, 24
/* 800EC1DC 000E91DC  90 03 00 00 */	stw r0, 0x0(r3)
/* 800EC1E0 000E91E0  4E 80 00 20 */	blr
