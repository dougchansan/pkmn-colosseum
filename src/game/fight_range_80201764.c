/**
 * @file fight_range_80201764.c
 * @brief fight code, 0x80201764 - 0x80202810 (14 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 *
 * Reconciliation (2026-07-02): the 14 definitions below were originally
 * written into src/game/trainer.c (fightOutPokemonSetPokemonJoutai08) and
 * src/game/colosseum_event.c (the other 13) -- the wrong translation
 * units for this address range per splits.txt. Moved here, in address
 * order, with no functional changes.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);

static inline void* getPartyStatus(void* context) {
    return pokemonGetStatus(context, 0, 0xD6, 0);
}

static inline void* getPokemonStatus(void* context) {
    return pokemonGetStatus(context, 0, 0xCC, 0);
}

/* 0x80201764 | size: 0x12C */
u16 fightOutPokemonSetPokemonJoutai08(void* context, void* item, void* target) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusSetJoutai08();
    extern u16 pokemonSetJoutai08();
    void* partyList;
    void* pokemon;
    if (joutaiDataBiosGetStatusKind(item) == 0x7C || joutaiDataBiosGetStatusKind(item) == 0xC8 || joutaiDataBiosGetStatusKind(item) == 0xCD) {
        partyList = getPartyStatus(context);
        if (joutaiDataBiosGetStatusKind(item) == 0x7C || joutaiDataBiosGetStatusKind(item) == 0xC8) {
            if (partyList == NULL) {
                pokemon = NULL;
            } else {
                pokemon = pokemonGetStatus(partyList, 0, 0xCC, 0);
            }
            return pokemonSetJoutai08(pokemon, item, target);
        } else if (joutaiDataBiosGetStatusKind(item) == 0xCD) {
            return statusSetJoutai08(partyList, item, target);
        }
    } else if (joutaiDataBiosGetStatusKind(item) == 0xD8) {
        return statusSetJoutai08(context, item, target);
    }
}

/* 0x80201890 | size: 0x12C */
u16 fightOutPokemonGetPokemonJoutai08(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusGetJoutai08();
    extern u16 pokemonGetJoutai08();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutai08(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return 0;
        }
        return statusGetJoutai08(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return 0;
    }
    return statusGetJoutai08(ctx, typeObj);
}

/* 0x802019BC | size: 0x170 */
static inline void fightRangeDispatch(void* ctx, void* src, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern void statusCopyJoutai();
    extern void pokemonCopyJoutai();
    void* srcResolved;
    void* ctxResolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        srcResolved = pokemonGetStatus(src, 0, 0xD6, 0);
        ctxResolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (ctxResolved == NULL) {
                ctxResolved = NULL;
            } else {
                ctxResolved = getPokemonStatus(ctxResolved);
            }
            if (srcResolved == NULL) {
                srcResolved = NULL;
            } else {
                srcResolved = pokemonGetStatus(srcResolved, 0, 0xCC, 0);
            }
            pokemonCopyJoutai(ctxResolved, srcResolved, typeObj);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            statusCopyJoutai(ctxResolved, srcResolved, typeObj);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        statusCopyJoutai(ctx, src, typeObj);
    }
}
void fightOutPokemonCopyPokemonJoutai(void* ctx, void* src, void* typeObj) {
    fightRangeDispatch(ctx, src, typeObj);
}

/* 0x80201B2C | size: 0x12C */
u16 fightOutPokemonSetPokemonJoutai0C(void* ctx, void* typeObj, u32 param) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusSetJoutai0C();
    extern u16 pokemonSetJoutai0C();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonSetJoutai0C(resolved, typeObj, param);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            return statusSetJoutai0C(resolved, typeObj, param);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        return statusSetJoutai0C(ctx, typeObj, param);
    }
}

/* 0x80201C58 | size: 0x12C */
u16 fightOutPokemonGetPokemonJoutai0C(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusGetJoutai0C();
    extern u16 pokemonGetJoutai0C();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutai0C(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return 0;
        }
        return statusGetJoutai0C(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return 0;
    }
    return statusGetJoutai0C(ctx, typeObj);
}

/* 0x80201D84 | size: 0x12C */
u16 fightOutPokemonGetJoutaiParam(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusGetJoutaiParam();
    extern u16 pokemonGetJoutaiParam();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutaiParam(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return 0;
        }
        return statusGetJoutaiParam(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return 0;
    }
    return statusGetJoutaiParam(ctx, typeObj);
}

/* 0x80201EB0 | size: 0x12C */
u16 fightOutPokemonSetPokemonJoutaiKaisuu(void* ctx, void* typeObj, u32 param) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusSetJoutaiKaisuu();
    extern u16 pokemonSetJoutaiKaisuu();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonSetJoutaiKaisuu(resolved, typeObj, param);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            return statusSetJoutaiKaisuu(resolved, typeObj, param);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        return statusSetJoutaiKaisuu(ctx, typeObj, param);
    }
}

/* 0x80201FDC | size: 0x12C */
u16 fightOutPokemonSetPokemonJoutai05(void* ctx, void* typeObj, u32 param) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u16 statusSetJoutai05();
    extern u16 pokemonSetJoutai05();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = getPartyStatus(ctx);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonSetJoutai05(resolved, typeObj, param);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            return statusSetJoutai05(resolved, typeObj, param);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        return statusSetJoutai05(ctx, typeObj, param);
    }
}

/* 0x80202108 | size: 0x12C */
s16 fightOutPokemonGetPokemonJoutai05(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusGetJoutai05();
    extern s16 pokemonGetJoutai05();
    void* originalCtx;
    void* resolved;
    resolved = (originalCtx = ctx);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutai05(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return -1;
        }
        return statusGetJoutai05(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return -1;
    }
    return statusGetJoutai05(originalCtx, typeObj);
}

/* 0x80202234 | size: 0x12C */
s16 fightOutPokemonGetPokemonJoutaiKaisuu(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusGetJoutaiKaisuu();
    extern s16 pokemonGetJoutaiKaisuu();
    void* originalCtx;
    void* resolved;
    resolved = (originalCtx = ctx);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutaiKaisuu(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return -1;
        }
        return statusGetJoutaiKaisuu(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return -1;
    }
    return statusGetJoutaiKaisuu(originalCtx, typeObj);
}

/* 0x80202360 | size: 0x12C */
s16 fightOutPokemonGetPokemonJoutaiStack(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusGetJoutaiStack();
    extern s16 pokemonGetJoutaiStack();
    void* originalCtx;
    void* resolved;
    resolved = (originalCtx = ctx);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonGetJoutaiStack(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return -1;
        }
        return statusGetJoutaiStack(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return -1;
    }
    return statusGetJoutaiStack(originalCtx, typeObj);
}

/* 0x8020248C | size: 0x12C */
s16 fightOutPokemonWritePokemonJoutaiDataId(void* ctx, void* typeObj, void* inputParam) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusWriteJoutaiDataId();
    extern s16 pokemonWriteJoutaiDataId();
    void* originalCtx;
    void* resolved;
    void* resolvedParam;
    void* directParam;
    resolved = (originalCtx = ctx);
    directParam = (resolvedParam = inputParam);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonWriteJoutaiDataId(resolved, typeObj, resolvedParam);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            return statusWriteJoutaiDataId(resolved, typeObj, resolvedParam);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        return statusWriteJoutaiDataId(originalCtx, typeObj, directParam);
    }
}

/* 0x802025B8 | size: 0x12C */
s16 fightOutPokemonCheckWritePokemonJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusCheckWriteJoutaiDataId();
    extern s16 pokemonCheckWriteJoutaiDataId();
    void* originalCtx;
    void* resolved;
    resolved = (originalCtx = ctx);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonCheckWriteJoutaiDataId(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return 0;
        }
        return statusCheckWriteJoutaiDataId(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return 0;
    }
    return statusCheckWriteJoutaiDataId(originalCtx, typeObj);
}

/* 0x802026E4 | size: 0x12C */
s16 fightOutPokemonIsPokemonJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern s16 statusIsJoutaiDataId();
    extern s16 pokemonIsJoutaiDataId();
    void* originalCtx;
    void* resolved;
    resolved = (originalCtx = ctx);
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        resolved = pokemonGetStatus(resolved, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (resolved == NULL) {
                resolved = NULL;
            } else {
                resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
            }
            return pokemonIsJoutaiDataId(resolved, typeObj);
        }
        if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
            return 0;
        }
        return statusIsJoutaiDataId(resolved, typeObj);
    }
    if (joutaiDataBiosGetStatusKind(typeObj) != 0xD8) {
        return 0;
    }
    return statusIsJoutaiDataId(originalCtx, typeObj);
}
