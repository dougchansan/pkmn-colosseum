#include "dolphin/types.h"

/*
 * udp_cc.c - UDP (BBA/broadband adapter) communication backend stubs.
 *
 * These are weak stub functions for the BBA network debug pathway.
 * When BBA support is not linked in, these return error/no-op values.
 * All functions are weak so they can be overridden by a real BBA
 * implementation if one is linked.
 */

#pragma weak udp_cc_initialize
#pragma weak udp_cc_shutdown
#pragma weak udp_cc_open
#pragma weak udp_cc_close
#pragma weak udp_cc_read
#pragma weak udp_cc_write
#pragma weak udp_cc_peek
#pragma weak udp_cc_pre_continue
#pragma weak udp_cc_post_stop

s32 udp_cc_initialize(void) {
    return -1;
}

s32 udp_cc_shutdown(void) {
    return -1;
}

s32 udp_cc_open(void) {
    return -1;
}

s32 udp_cc_close(void) {
    return -1;
}

s32 udp_cc_read(void) {
    return 0;
}

s32 udp_cc_write(void) {
    return 0;
}

s32 udp_cc_peek(void) {
    return 0;
}

s32 udp_cc_pre_continue(void) {
    return -1;
}

s32 udp_cc_post_stop(void) {
    return -1;
}
