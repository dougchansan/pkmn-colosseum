#include "real_content_host.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* framesEnv = getenv("PCPORT_BATTLE_PROBE_FRAMES");
    int frames = framesEnv != NULL ? atoi(framesEnv) : 24;

    (void)argc;
    (void)argv;

    if (frames <= 1) {
        frames = 24;
    }

    printf("[battle-probe-main] start frames=%d mode=headless\n", frames);
    fflush(stdout);
    PCPort_BattleProbe(frames);
    return 0;
}
