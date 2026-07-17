#ifndef GAME_GS_MODEL_ANIM_H
#define GAME_GS_MODEL_ANIM_H

#include "dolphin/types.h"

struct GSmodel;

u32 GSmodelCanAnimate(struct GSmodel* model);
u32 GSmodelCanTexAnimate(struct GSmodel* model);
void modelApplyAnimation__FP8_GSmodel(struct GSmodel* model);
void modelUpdateAttachments__FP8_GSmodel(struct GSmodel* model);

#endif /* GAME_GS_MODEL_ANIM_H */
