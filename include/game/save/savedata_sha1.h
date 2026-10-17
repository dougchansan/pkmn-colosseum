#ifndef GAME_SAVE_SAVEDATA_SHA1_H
#define GAME_SAVE_SAVEDATA_SHA1_H

#include "dolphin/types.h"

/*
 * SHA-1 used to digest and scramble the memory-card save data
 * (0x801CBBAC - 0x801CDB04). The context and the Update/Final/Transform
 * code follow Steve Reid's public-domain SHA-1 in its SHA1HANDSOFF form
 * (the transform copies each block into a static 64-byte workspace, and
 * Final wipes the context and runs one last transform on it).
 *
 * The functions keep their address names:
 *   sha1Calc  one-shot digest of a buffer (Init + Update + Final)
 *   savedataDecryptCheck  unscramble save data and check it against a digest
 *   fn_801CBE44  digest save data, then scramble it
 *   fn_801CBF64  SHA1Final
 *   sha1Transform  SHA1Transform
 */
typedef struct FieldSha1Context {
    u32 state[5];
    u32 count[2];
    u8 buffer[64];
} FieldSha1Context;

void sha1Calc(u8 digest[20], const u8* input, u32 length);
void fn_801CBF64(u8 digest[20], FieldSha1Context* context);
void sha1Transform(u32 state[5], const u8 input[64]);

#endif /* GAME_SAVE_SAVEDATA_SHA1_H */
