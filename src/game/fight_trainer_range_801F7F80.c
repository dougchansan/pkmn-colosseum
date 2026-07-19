/**
 * @file fight_trainer_range_801F7F80.c
 * @brief game/pxdvs/app/fight/fightTrainer.cpp -- split from game/trainer.c
 *        (the XD fight-trainer bucket, 0x801F7F80-0x80201764),
 *        address range 0x801F7F80-0x801FBD10, 43 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainer.cpp
 * Physically split out of the trainer.c bucket by address (functions
 * bucketed via their /* 0xADDR *\/ markers), byte-identical
 * reconstruction verified against the original file.
 */
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * External declarations (duplicated verbatim from the original
 * game/trainer.c preamble into every split segment, so each new
 * TU keeps the same external visibility it had before the split)
 * ========================================================================= */

/* pokemonGetStatus - Master data table resolver
 * The most-called function in the entire game (1769 calls).
 * Takes a context pointer, slot index, table ID, and flags.
 * Returns a pointer to the resolved data, or NULL. */
extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);

/* pokemonSetStatus - Master data table writer (544 calls) */
extern u32 pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);

/* pokemonGrowBasisStatus - Data table auxiliary writer */
extern void pokemonGrowBasisStatus(void* context, u32 value);

/* itemGetStatus - Secondary data accessor (169 calls) */
extern u32 itemGetStatus(u32 context, u32 param, u16 field, u32 flags);

/* fightTargetGetPtr - PokemonSlotLookup (89 calls) */
extern u32 fightTargetGetPtr(u32 type, void* ptr, u32 param);

/* Category resolution sub-dispatchers (defined with real bodies at their
 * proper address-ordered locations, possibly in a sibling segment). */
void* fightTrainerDataBiosGetPtr(u16 slot); /* Battle trainer */
void* fightTrainerPokemonPartDataBiosGetPtr(u16 slot); /* Party config */
void* fightTrainerPokemonDataBiosGetPtr(u16 slot); /* Team roster */
void* fightTrainerAiDataBiosGetPtr(u16 slot); /* Story/event data */
void* fightTrainerAiValueAddsubDataBiosGetPtr(u16 slot); /* Misc attributes */

/* Event integration (defined with real bodies, possibly in a sibling segment) */
void fightOutPokemonSetHensinPokemonStatusId(void* trainer, u16 eventId, u32 param1, u32 param2);
u8   fightOutPokemonIsUseHensinBuff(void* trainer);
void fightOutPokemonSetHensinFightPokemonStatusId(void* trainer, u16 eventId, u32 param);

/* Item/Pokemon field access helpers used by battle item flow. */
u8 fn_80121574(void* obj, s32 arg);
u8 fn_8011A3E4(void* obj, s32 arg);

/* SDA table pointers for trainer data arrays */
extern u32* lbl_80478F08;  /* Party config header */
extern u8*  lbl_80478F0C;  /* Party config data */
extern u32* lbl_80478F10;  /* Team roster header */
extern u8*  lbl_80478F14;  /* Team roster data */
extern u32* lbl_80478F18;  /* Slot data header */
extern u8*  lbl_80478F1C;  /* Slot data */
extern u32* lbl_80478F20;  /* Battle trainer header */
extern u8*  lbl_80478F24;  /* Battle trainer data */
extern u32* lbl_80478F28;  /* Misc attributes header */
extern u8*  lbl_80478F2C;  /* Misc attributes data */
extern u32* lbl_80478F30;  /* Story/event header */
extern u8*  lbl_80478F34;  /* Story/event data */

/* Forward declarations for converted functions (defined with real bodies,
 * possibly in a sibling segment). */
u16 fn_801FCC94(u8* ptr);
u8 fn_801FCCAC(u8* ptr);
void fightTrainerSetStatus(void);
void fightTrainerGetStatus(void);

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F7F80_801F81F8)
/* 0x801F7F80 | size: 0x80 */
void* fightTrainerSearchInvalidFightPokemonPtr(void* context, u16 count) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern u8 fightPokemonCheckValid(void* ptr);
    int i;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    for (i = 0; (u16)i < count; i++) {
        result = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(result) == 0) {
            return result;
        }
    }
    return NULL;
}

/* 0x801F8000 | size: 0x100 */
u32 fn_801F8000(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern u8* fightTrainerKindDataBiosGetPtr(u16 val);
    extern u32 fightTrainerKindDataBiosGetPrefixName(u8* ptr);
    extern u32 GSmsgGetGSchar(u32 val);
    int valid;
    void* result;
    u16 slot;

    if (context == NULL) {
        return 0;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)fn_801EF634(context) == 1) {
            valid = 0;
        } else if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fightTrainerGetStatus(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)heroCheckValid(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return 0;
    }
    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    result = fightTrainerGetStatus(0, slot, 0x4, 0);
    result = fightTrainerKindDataBiosGetPtr((u16)(u32)result);
    return GSmsgGetGSchar(fightTrainerKindDataBiosGetPrefixName((u8*)result));
}

/* 0x801F8100 | size: 0xF8 */
void* fightTrainerGetNamePtr(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern void* heroGetStatus(void* ptr, u32 param1, u32 param2);
    int valid;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)fn_801EF634(context) == 1) {
            valid = 0;
        } else if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fightTrainerGetStatus(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)heroCheckValid(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return NULL;
    }
    result = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (result == NULL) {
        return NULL;
    }
    return heroGetStatus(result, 1, 0);
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801F81F8 | size: 0x22C | large */
void fightTrainerGetHikaeFightPokemonNum(void) {
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonCheckIrekaeReserveFightPokemon();
    extern void fightPokemonCheckMotoFightPokemon();
    extern void fightPokemonCheckFightOut();
    extern void fightOutPokemonCheckValid();
    extern void fightPokemonCheckValid();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r26, r3 */;
    r27 = r5;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r25 = r4 & 0xFFFF;
    r29 = 0x0;
    r28 = 0x0;
    r30 = 0x0;
    while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r26;
        r6 = r30;
        r4 = 0x0;
        r5 = 0x45;
        fightTrainerGetStatus();
        r31 = r3;
        fightPokemonCheckValid();
        r0 = r3 & 0xFF;
        if ((s32)r0 == (s32)0) {
            r31 = 0x0;
        }
        do {
        if (r31 == (u32)0x0) break;
        r3 = r31;
        fightPokemonCheckFightOut();
        r0 = r3 & 0xFF;
        if (r31 == (u32)0x0) {
            r0 = 0x1;

        } else {
            if (r26 == (u32)0x0) {
                r23 = 0x0;

            } else if (r31 == (u32)0x0) {
                r23 = 0x0;

            }
            r24 = 0x0;
            while (1) {
                r0 = r24 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r26;
                r6 = r24;
                r4 = 0x0;
                r5 = 0x46;
                fightTrainerGetStatus();
                r23 = r3;
                fightOutPokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r23 = 0x0;
                }
                if (r23 != (u32)0x0) {
                    r3 = r31;
                    r4 = r23;
                    fightPokemonCheckMotoFightPokemon();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        break;
                }
                }
                r24 = r24 + 0x1;

            }
            r23 = 0x0;

            if (r23 != (u32)0x0) {
                r0 = 0x2;
                goto L_801F83B4;
            }
            if (r26 == (u32)0x0) {
                r0 = 0x0;

            } else if (r31 == (u32)0x0) {
                r0 = 0x0;

            }
            r24 = 0x0;
            while (1) {
                r0 = r24 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r26;
                r6 = r24;
                r4 = 0x0;
                r5 = 0x46;
                fightTrainerGetStatus();
                r23 = r3;
                fightOutPokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r23 = 0x0;
                }
                if (r23 != (u32)0x0) {
                    r3 = r23;
                    r4 = r31;
                    fightOutPokemonCheckIrekaeReserveFightPokemon();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x1;
                        break;
                }
                }
                r24 = r24 + 0x1;

            }
            r0 = 0x0;

            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = 0x3;

            } else {
                r0 = 0x0;
            }
        }
        L_801F83B4: ;
        r0 = r0 & 0xFF;
        if (r0 == (u32)0x1) break;
        if (r0 == (u32)0x2) {
            r28 = r28 + 0x1;
            break;
        }
        r29 = r29 + 0x1;
        } while (0);
        r30 = r30 + 0x1;

    }
    r3 = r27 & 0xFFFF;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r3) { r3 = r29; return; }
        r0 = r29 & 0xFFFF;
        if (r0 < (u32)0x1) { r3 = r29; return; }
        /* subi r29, r29, 0x1 */;
        r28 = r28 + 0x1;
    }

    r3 = r29;

    return;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F8424_801F87CC)
/* 0x801F8424 | size: 0x78 */
BOOL fightTrainerIsAllyFightTargetPtr(void* arg0, void* arg1, u32 arg2) {
    u32 val0;

    if (arg0 == NULL) {
        return FALSE;
    }
    if (arg1 == NULL) {
        return FALSE;
    }
    val0 = fightTargetGetPtr(2, arg0, arg2);
    return (u8)(fightTargetGetPtr(2, arg1, arg2) - val0 == 0);
}

/* 0x801F849C | size: 0x7C */
void fightTrainerInitEnemyPokemonFightOutStatus(void* arg0, u32 arg1) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightTrainerEnemyPokemonCheckValid(void*);
    extern void fightTrainerEnemyPokemonInitFightOutStatus(void*);
    u32 mode;
    void* pokemon;
    u8 i;

    if (arg0 == NULL) {
        return;
    }
    mode = (u8)arg1;
    i = 0;
    while (i < 12) {
        pokemon = fightTrainerGetStatus(arg0, 0, 0x4e, i);
        if (mode != 1 || (u8)fightTrainerEnemyPokemonCheckValid(pokemon)) {
            fightTrainerEnemyPokemonInitFightOutStatus(pokemon);
        }
        i++;
    }
}

/* 0x801F8518 | size: 0x98 */
void* fightTrainerGetNoActionFightOutPokemonPtr(void* context, u32 index) {
    extern u8 fightOutPokemonCheckValid(void* ptr);
    extern u8 fightActionCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* pokemon;
    void* sub;

    pokemon = fightTrainerGetStatus(context, 0, 0x46, index);
    if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
        pokemon = NULL;
    }
    if (pokemon == NULL) {
        return NULL;
    }
    sub = pokemonGetStatus(pokemon, 0, 0xFE, 0);
    if (sub == NULL) {
        return NULL;
    }
    if ((u8)fightActionCheckValid(sub) == 1) {
        return NULL;
    }
    return pokemon;
}

/* 0x801F85B0 | size: 0x88 */
s32 fightTrainerGetFightOutPokemonToTemotiBanme(void* context, void* target) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 2; i++) {
        if (target == fightTrainerGetStatus(context, 0, 0x46, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F8638 | size: 0x88 */
s32 fightTrainerGetFightPokemonToTemotiBanme(void* context, void* target) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 6; i++) {
        if (target == fightTrainerGetStatus(context, 0, 0x45, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F86C0 | size: 0x10C */
void fightTrainerBackFightPokemonToTemotiPokemon(void* context, u8 flag) {
    extern void pokemonBiosCopy(void* ptr, void* pokemon);
    extern void fn_8012190C(void* ptr, u32 field, u32 val);
    extern u8 fn_80121ADC(void* ptr, u32 field);
    extern u16 pokemonGetSoubiItemDataId(void* pokemon);
    extern u8 fightPokemonCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    void* slot;
    u16 i;
    void* pokemon;
    void* moveData;
    u16 speciesId;

    if (context == NULL) {
        return;
    }
    for (i = 0; i < 6; i++) {
        slot = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(slot) == 0) {
            slot = NULL;
        }
        if (slot != NULL) {
            if ((pokemon = pokemonGetStatus(slot, 0, 0xCC, 0)) != NULL) {
                speciesId = pokemonGetSoubiItemDataId(pokemon);
                if ((moveData = pokemonGetStatus(slot, 0, 0xCB, 0)) != NULL) {
                    pokemonBiosCopy(moveData, pokemon);
                    if ((u8)fn_80121ADC(moveData, 4) == 1) {
                        fn_8012190C(moveData, 4, 1);
                    }
                    if (flag == 1) {
                        pokemonSetStatus(moveData, 0, 0x82, 0, speciesId);
                    }
                }
            }
        }
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801F87CC | size: 0x24C | large */
void fn_801F87CC(void) {
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonCheckIrekaeReserveFightPokemon();
    extern void fightPokemonCheckMotoFightPokemon();
    extern void fightPokemonCheckFightOut();
    extern void fightOutPokemonCheckValid();
    u8 sp[0x30];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = 0x0;
    r28 = r3;
    r29 = r4;
    *(u16*)(sp + 0x8) = r5;
    while (1) {
        r0 = *(u16*)(sp + 0x8);
        if (r0 >= (u32)0x6) break;
        r3 = *(u16*)(sp + 0x8);
        /* clrlslwi r0, r0, 16, 2 */;
        *(u32*)(r29 + r0) = r5;
        r0 = r3 + 0x1;
        *(u16*)(sp + 0x8) = r0;

    }
    r0 = 0x0;
    r30 = 0x0;
    *(u16*)(sp + 0x8) = r0;
    while (1) {
        if (r28 == (u32)0x0) {
            r4 = 0x0;

        } else {
            /* addic. r0, (u32)sp, 0x8 */;
            if (r28 == (u32)0x0) {
                r4 = 0x0;
                goto L_801F89DC;
            }
            goto L_801F89B8;
            do {
                r3 = r28;
                r4 = 0x0;
                r5 = 0x45;
                fightTrainerGetStatus();
                r31 = r3;
                fightPokemonCheckFightOut();
                r0 = r3 & 0xFF;
                if (r28 == (u32)0x0) {
                    r0 = 0x1;

                } else {
                    if (r28 == (u32)0x0) {
                        r26 = 0x0;

                    } else if (r31 == (u32)0x0) {
                        r26 = 0x0;

                    }
                    r27 = 0x0;
                    while (1) {
                        r0 = r27 & 0xFFFF;
                        if (r0 >= (u32)0x2) break;
                        r3 = r28;
                        r6 = r27;
                        r4 = 0x0;
                        r5 = 0x46;
                        fightTrainerGetStatus();
                        r26 = r3;
                        fightOutPokemonCheckValid();
                        r0 = r3 & 0xFF;
                        if (r31 == (u32)0x0) {
                            r26 = 0x0;
                        }
                        if (r26 != (u32)0x0) {
                            r3 = r31;
                            r4 = r26;
                            fightPokemonCheckMotoFightPokemon();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                break;
                        }
                        }
                        r27 = r27 + 0x1;

                    }
                    r26 = 0x0;

                    if (r26 != (u32)0x0) {
                        r0 = 0x2;
                        goto L_801F89A4;
                    }
                    if (r28 == (u32)0x0) {
                        r0 = 0x0;

                    } else if (r31 == (u32)0x0) {
                        r0 = 0x0;

                    }
                    r27 = 0x0;
                    while (1) {
                        r0 = r27 & 0xFFFF;
                        if (r0 >= (u32)0x2) break;
                        r3 = r28;
                        r6 = r27;
                        r4 = 0x0;
                        r5 = 0x46;
                        fightTrainerGetStatus();
                        r26 = r3;
                        fightOutPokemonCheckValid();
                        r0 = r3 & 0xFF;
                        if (r31 == (u32)0x0) {
                            r26 = 0x0;
                        }
                        if (r26 != (u32)0x0) {
                            r3 = r26;
                            r4 = r31;
                            fightOutPokemonCheckIrekaeReserveFightPokemon();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r0 = 0x1;
                                break;
                        }
                        }
                        r27 = r27 + 0x1;

                    }
                    r0 = 0x0;

                    r0 = r0 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x3;

                    } else {
                        r0 = 0x0;
                    }
                }
                L_801F89A4: ;
                r0 = r0 & 0xFF;
                if (r0 == (u32)0x1) break;
                r3 = *(u16*)(sp + 0x8);
                r0 = r3 + 0x1;
                *(u16*)(sp + 0x8) = r0;
                L_801F89B8: ;
                r6 = *(u16*)(sp + 0x8);
            } while (r6 < (u32)0x6);

            r0 = *(u16*)(sp + 0x8);
            if (r0 >= (u32)0x6) {
                r4 = 0x0;

            } else {
                r4 = r31;
            }
        }
        L_801F89DC: ;
        if (r4 == (u32)0x0) { r3 = r30; return; }
        r3 = *(u16*)(sp + 0x8);
        /* clrlslwi r0, r30, 16, 2 */;
        *(u32*)(r29 + r0) = r4;
        r30 = r30 + 0x1;
        r0 = r3 + 0x1;
        *(u16*)(sp + 0x8) = r0;
    }

    r3 = r30;
    return;
}

/* 0x801F8A18 | size: 0x1E8 | medium */
void fn_801F8A18(void) {
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonCheckIrekaeReserveFightPokemon();
    extern void fightPokemonCheckMotoFightPokemon();
    extern void fightPokemonCheckFightOut();
    extern void fightOutPokemonCheckValid();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r29, r3 */;
    r30 = r4;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    if (r30 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    goto L_801F8BC8;
    do {
        r3 = r29;
        r4 = 0x0;
        r5 = 0x45;
        fightTrainerGetStatus();
        r31 = r3;
        fightPokemonCheckFightOut();
        r0 = r3 & 0xFF;
        if (r30 == (u32)0x0) {
            r0 = 0x1;

        } else {
            if (r29 == (u32)0x0) {
                r27 = 0x0;

            } else if (r31 == (u32)0x0) {
                r27 = 0x0;

            }
            r28 = 0x0;
            while (1) {
                r0 = r28 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r29;
                r6 = r28;
                r4 = 0x0;
                r5 = 0x46;
                fightTrainerGetStatus();
                r27 = r3;
                fightOutPokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r27 = 0x0;
                }
                if (r27 != (u32)0x0) {
                    r3 = r31;
                    r4 = r27;
                    fightPokemonCheckMotoFightPokemon();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        break;
                }
                }
                r28 = r28 + 0x1;

            }
            r27 = 0x0;

            if (r27 != (u32)0x0) {
                r0 = 0x2;
                goto L_801F8BB4;
            }
            if (r29 == (u32)0x0) {
                r0 = 0x0;

            } else if (r31 == (u32)0x0) {
                r0 = 0x0;

            }
            r28 = 0x0;
            while (1) {
                r0 = r28 & 0xFFFF;
                if (r0 >= (u32)0x2) break;
                r3 = r29;
                r6 = r28;
                r4 = 0x0;
                r5 = 0x46;
                fightTrainerGetStatus();
                r27 = r3;
                fightOutPokemonCheckValid();
                r0 = r3 & 0xFF;
                if (r31 == (u32)0x0) {
                    r27 = 0x0;
                }
                if (r27 != (u32)0x0) {
                    r3 = r27;
                    r4 = r31;
                    fightOutPokemonCheckIrekaeReserveFightPokemon();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
                        r0 = 0x1;
                        break;
                }
                }
                r28 = r28 + 0x1;

            }
            r0 = 0x0;

            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) {
                r0 = 0x3;

            } else {
                r0 = 0x0;
            }
        }
        L_801F8BB4: ;
        r0 = r0 & 0xFF;
        if (r0 == (u32)0x1) break;
        r3 = *(u16*)((u8*)r30 + 0x0);
        r0 = r3 + 0x1;
        *(u16*)((u8*)r30 + 0x0) = r0;
        L_801F8BC8: ;
        r6 = *(u16*)((u8*)r30 + 0x0);
    } while (r6 < (u32)0x6);

    r0 = *(u16*)((u8*)r30 + 0x0);
    if (r0 >= (u32)0x6) {
        r3 = 0x0;
        return;
    }
    r3 = r31;

    return;
}

/* 0x801F8C00 | size: 0x180 */
u32 fightTrainerCheckCanIrekaeFightPokemon(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightOutPokemonCheckIrekaeReserveFightPokemon(void* pokemon, void* filter);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightPokemonCheckFightOut(void* ptr);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    void* pokemon;
    u8 found;
    u16 i;

    if ((u8)fightPokemonCheckFightOut(filter) == 0) {
        return 1;
    }
    pokemon = NULL;
    if (context == NULL) {
        pokemon = NULL;
    } else if (filter == NULL) {
        pokemon = NULL;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
            if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                    break;
                }
            }
        }
        pokemon = NULL;
    }
    if (pokemon != NULL) {
        return 2;
    }
    found = 0;
    if (context == NULL) {
        found = 0;
    } else if (filter == NULL) {
        found = 0;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
            if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fightOutPokemonCheckIrekaeReserveFightPokemon(pokemon, filter) == 1) {
                    found = 1;
                    break;
                }
            }
        }
        found = 0;
    }
    return found != 0 ? 3 : 0;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F8D80_801F9130)
/* 0x801F8D80 | size: 0xB4 */
void* fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
        if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8E34 | size: 0xF0 */
void* fightTrainerCheckFightPokemonFightOut(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightOutPokemonCheckFightOut(void* ptr);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    u16 i;
    void* pokemon;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    pokemon = NULL;
    if (context == NULL) {
        pokemon = NULL;
    } else if (filter == NULL) {
        pokemon = NULL;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
            if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                    if (((!filter) && (!filter)) && (!filter)) {
                        /* Preserve MWCC register allocation. */
                    }
                    goto found;
                }
            }
        }
        pokemon = NULL;
    }
found:
    if (pokemon != NULL) {
        if ((u8)fightOutPokemonCheckFightOut(pokemon) == 0) {
            pokemon = NULL;
        }
    }
    return pokemon;
}

/* 0x801F8F24 | size: 0xB4 */
void* fightTrainerGetEntryIdToFightPokemonPtr(void* context, s16 speciesId) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;
    s16 species;

    species = speciesId;
    if (species < 0) {
        return NULL;
    }
    for (i = 0; i < 6; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if (species == (s16)(s32)pokemonGetStatus(pokemon, 0, 0xCE, 0)) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8FD8 | size: 0x5C */
void* fightTrainerCreateSequence(void* arg0) {
    extern void* fn_801DA4E8(void*, u32);
    extern void* fn_801DE418(u32);
    extern void* fightTrainerGetStatus(void*, void*, u32, u32);
    void* result;

    result = fn_801DE418((u16)(u32)fightTrainerGetStatus(0, arg0, 9, 0));
    if (result == NULL) {
        return NULL;
    }
    fn_801DA4E8(result, 0);
    return result;
}

/* 0x801F9034 | size: 0x90 */
u32 fightTrainerCheckCanGetExp(void* context) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    u16 slot;
    u16 type;
    u8 val;

    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        val = 0;
    } else if (type == 2 || type == 3) {
        val = 1;
    } else {
        val = 2;
    }
    if ((u8)(val == 0) == 1) {
        if (((!type) && (!type)) && (!type)) {
            /* Preserve MWCC register allocation. */
        }
        return 1;
    }
    return 0;
}

/* 0x801F90C4 | size: 0x6C */
void* fightTrainerIsMineFightPokemon(void* arg0, void* arg1) {
    extern void* heroIsMinePokemon(void*, void*);
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    void* pokemon;
    void* trainerData;

    extern void* fightPokemonGetPokemonPtr(void*);
    pokemon = fightPokemonGetPokemonPtr(arg1);
    if (pokemon == NULL) {
        return NULL;
    }
    trainerData = fightTrainerGetStatus(arg0, 0, 0x44, 0);
    if (trainerData == NULL) {
        return NULL;
    }
    return heroIsMinePokemon(trainerData, pokemon);
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801F9130 | size: 0x10C */
#pragma push
#pragma optimization_level 2
void fightTrainerTimeOutSelectFightAction(void* unused, void* trainer, void* pokemon) {
    extern u8 lbl_80375CA8[];
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void* ptr, void* pokemon);
    extern u8 fightOutPokemonCheckFightActionWazaSelect(void* trainer, u32 mode);
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme(void* trainer, s32 slot, u32 field, u32 flags);
    extern void fightOutPokemonCreateFightActionAttackWaza(void* trainer, u32 p1, u32 p2, u32 p3, void* table, u16 moveId, u32 nameId, u32 slotIdx);
    extern void* fightOutPokemonGetPokemonPtr(void* trainer);
    extern void fn_8022B2CC(void* trainer, u16 moveId, void* pokemon, u32 p3, u32 p4, u32 p5, s32 p6);
    s32 slot;
    u16 moveId;
    s8 i;

    if ((u8)fightOutPokemonCheckFightActionWazaSelect(trainer, 1) != 0) {
        return;
    }
    for (i = 0; (s8)i < 4; i++) {
        moveId = (u16)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainer), 0, 0x7F, 0);
        slot = (s8)i;
        if ((u8)fightOutPokemonCheckCanOutOkWazaBanme(trainer, slot, 1, 0) == 0) {
            moveId = (u16)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainer), 0, 0x7F, slot);
            break;
        }
    }
    fn_8022B2CC(trainer, moveId, pokemon, 0, 1, 0, -1);
    fightOutPokemonCreateFightActionAttackWaza(trainer, 0, 0x13, 0, (void*)lbl_80375CA8, moveId, fightTargetGetTragetPtrToRelativeHostSideFightTargetId(trainer, pokemon), (u32)i);
}
#pragma pop

/* 0x801F923C | size: 0x1BC */
u32 fightTrainerSelectFightAction(void* context, void* param) {
    extern u16 fn_801EF634(void* ctx);
    extern void fightActionInit(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern void fightOutPokemonInitFightActionBuff(void* ptr);
    extern void fn_802342CC(void* ctx, void* param);
    extern u8 fightMenuFightTrainerAgbHeroOpenMenu(void* ctx, void* param);
    extern void fightMenuFightTrainerGcHeroOpenMenu(void* ctx, void* param, u32 flags);
    void* result;
    u16 slot;
    u16 type;
    u8 battleType;
    u16 i;

    result = fightTrainerGetStatus(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x46, i));
    }
    fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x47, 0));
    if ((u16)fn_801EF634(context) == 1) {
        return 0;
    }
    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    if ((u16)(u32)fightTrainerGetStatus(0, slot, 0x2, 0) != 0) {
        fn_802342CC(context, param);
        return 1;
    }
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType == 0) {
        fightMenuFightTrainerGcHeroOpenMenu(context, param, 0);
        return 1;
    }
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType != 1) {
        return 1;
    }
    return (u8)fightMenuFightTrainerAgbHeroOpenMenu(context, param) != 0;
}

/* 0x801F93F8 | size: 0x208 | large */
void fightTrainerTimeOutSelectIrekaeFightPokemon(void) {
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonCheckIrekaeReserveFightPokemon();
    extern void fightPokemonCheckMotoFightPokemon();
    extern void fightPokemonCheckFightOut();
    extern void fightOutPokemonCheckValid();
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r30, r3 */;
    r3 = 0x0;
    *(u16*)(sp + 0x8) = r3;
    if ((s32)r0 == (s32)0) {

    } else {
        /* addic. r0, (u32)sp, 0x8 */;
        if ((s32)r0 == (s32)0) {
            goto L_801F95C8;
        }
        goto L_801F95A4;
        do {
            r3 = r30;
            r4 = 0x0;
            r5 = 0x45;
            fightTrainerGetStatus();
            r31 = r3;
            fightPokemonCheckFightOut();
            r0 = r3 & 0xFF;
            if ((s32)r0 == (s32)0) {
                r0 = 0x1;

            } else {
                if (r30 == (u32)0x0) {
                    r28 = 0x0;

                } else if (r31 == (u32)0x0) {
                    r28 = 0x0;

                }
                r29 = 0x0;
                while (1) {
                    r0 = r29 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r3 = r30;
                    r6 = r29;
                    r4 = 0x0;
                    r5 = 0x46;
                    fightTrainerGetStatus();
                    r28 = r3;
                    fightOutPokemonCheckValid();
                    r0 = r3 & 0xFF;
                    if (r31 == (u32)0x0) {
                        r28 = 0x0;
                    }
                    if (r28 != (u32)0x0) {
                        r3 = r31;
                        r4 = r28;
                        fightPokemonCheckMotoFightPokemon();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            break;
                    }
                    }
                    r29 = r29 + 0x1;

                }
                r28 = 0x0;

                if (r28 != (u32)0x0) {
                    r0 = 0x2;
                    goto L_801F9590;
                }
                if (r30 == (u32)0x0) {
                    r0 = 0x0;

                } else if (r31 == (u32)0x0) {
                    r0 = 0x0;

                }
                r29 = 0x0;
                while (1) {
                    r0 = r29 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r3 = r30;
                    r6 = r29;
                    r4 = 0x0;
                    r5 = 0x46;
                    fightTrainerGetStatus();
                    r28 = r3;
                    fightOutPokemonCheckValid();
                    r0 = r3 & 0xFF;
                    if (r31 == (u32)0x0) {
                        r28 = 0x0;
                    }
                    if (r28 != (u32)0x0) {
                        r3 = r28;
                        r4 = r31;
                        fightOutPokemonCheckIrekaeReserveFightPokemon();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r0 = 0x1;
                            break;
                    }
                    }
                    r29 = r29 + 0x1;

                }
                r0 = 0x0;

                r0 = r0 & 0xFF;
                if (r0 == (u32)0x1) {
                    r0 = 0x3;

                } else {
                    r0 = 0x0;
                }
            }
            L_801F9590: ;
            r0 = r0 & 0xFF;
            if (r0 == (u32)0x1) break;
            r3 = *(u16*)(sp + 0x8);
            r0 = r3 + 0x1;
            *(u16*)(sp + 0x8) = r0;
            L_801F95A4: ;
            r6 = *(u16*)(sp + 0x8);
        } while (r6 < (u32)0x6);

        r0 = *(u16*)(sp + 0x8);
        if (r0 >= (u32)0x6) {
            r3 = 0x0;

        } else {
            r3 = r31;
        }
    }
    L_801F95C8: ;
    if (r3 != (u32)0x0) {
        r4 = 0x0;
        r5 = 0xce;
        r6 = 0x0;
        ((void(*)(void))pokemonGetStatus)();
        r3 = (s16)r3;
    } else {

        r3 = -0x1;
    }
    return;
}

/* 0x801F9600 | size: 0x190 */
s32 fightTrainerSelectIrekaeFightPokemon(void* context, void* p1, void* p2, void* p3) {
    extern u8 fn_80008174(void);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(void* ctx, u32 zero, void* p1, void* p2, u32 flags);
    u16 slot;
    u16 species;
    u16 type;
    u8 battleType;

    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    species = (u16)(u32)fightTrainerGetStatus(0, slot, 0x2, 0);
    if ((u8)fn_80008174() == 1) {
        return fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(context, 0, p1, p2, 0);
    }
    if (species != 0) {
        return fightTrainerAiSelectIrekaeDasuFightPokemon(context, p1, p2, p3);
    }
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType == 0) {
        return fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(context, 0, p1, p2, 0);
    }
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        battleType = 0;
    } else if (type == 2 || type == 3) {
        battleType = 1;
    } else {
        battleType = 2;
    }
    if (battleType != 1) {
        return -1;
    }
    return fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(context, p1, p2, p3);
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F9790_801F99C8)
/* 0x801F9790 | size: 0x8C */
void fightTrainerAllInitFightActionBuff(void* context) {
    extern void fightActionInit(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern void fightOutPokemonInitFightActionBuff(void* ptr);
    void* result;
    u16 i;

    result = fightTrainerGetStatus(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x46, i));
    }
    fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x47, 0));
}

/* 0x801F981C | size: 0x50 */
void* fightTrainerGetValidFightOutPokemonPtr(void* arg1, u32 arg2) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonCheckValid(void*);
    void* result = fightTrainerGetStatus(arg1, 0, 0x46, arg2);
    if ((u8)fightOutPokemonCheckValid(result) == 0) {
        return NULL;
    }
    return result;
    return;
}

/* 0x801F986C | size: 0x50 */
void* fightTrainerGetValidFightPokemonPtr(void* arg1, u32 arg2) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightPokemonCheckValid(void*);
    void* result = fightTrainerGetStatus(arg1, 0, 0x45, arg2);
    if ((u8)fightPokemonCheckValid(result) == 0) {
        return NULL;
    }
    return result;
}

/* 0x801F98BC | size: 0x74 */
u32 fightTrainerGetDoFightOutFightOutPokemonCount(void* arg0) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonCheckFightOut(void*);
    u8 i;
    u32 count;

    count = 0;
    for (i = 0; i < 2; i++) {
        if ((u8)fightOutPokemonCheckFightOut(fightTrainerGetStatus(arg0, 0, 0x46, i))) {
            count = ((count & 0xFF) + 1) & 0xFF;
        }
    }
    return count;
}

/* 0x801F9930 | size: 0x98 */
void* fightTrainerCheckTemotiPokemonFightEntry(void* context, void* moveData) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fightPokemonCheckValid(void* ptr);
    int i;
    void* pokemon;
    void* data;

    for (i = 0; (u16)i < 6; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(pokemon) != 0) {
            data = pokemonGetStatus(pokemon, 0, 0xCB, 0);
            if (data != NULL) {
                if (moveData == data) {
                    return pokemon;
                }
            }
        }
    }
    return NULL;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801F99C8 | size: 0x2F4 | large */
void fightTrainerSortFightTrainerDataIdToHeroTemotiPokemon(void) {
    extern void pokemonCheckValid();
    extern void pokemonInit();
    extern void heroCheckValid();
    extern void heroGetStatus();
    extern void fn_801EF634();
    extern void fightTrainerGetStatus();
    extern void fn_802331F4();
    u8 sp[0x790];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    /* mr. r28, r3 */;
    r29 = r4;
    r30 = r5;
    if ((s32)r0 == (s32)0) return;
    if ((s32)r0 != (s32)0) {

    fn_801EF634();
    r0 = r3 & 0xFFFF;
    if (r0 != (u32)0x1) {

        r3 = r28;
        r4 = 0x0;
        r5 = 0x43;
        r6 = 0x0;
        fightTrainerGetStatus();
    if ((s32)r3 != (s32)0x0) {

            r3 = r28;
            r4 = 0x0;
            r5 = 0x44;
            r6 = 0x0;
            fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

                heroCheckValid();
                r0 = r3 & 0xFF;
    if (r3 != (u32)0x0) {

                    r0 = 0x1;
    }
    }
    }
    }
    }
    r0 = r0 & 0xFF;
    if (r3 == (u32)0x0) return;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x43;
    r6 = 0x0;
    fightTrainerGetStatus();
    r26 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r26;
    r5 = 0x2;
    r6 = 0x0;
    fightTrainerGetStatus();
    r27 = r3 & 0xFFFF;
    r3 = r28;
    r4 = 0x0;
    r5 = 0x44;
    r6 = 0x0;
    fightTrainerGetStatus();
    r31 = r3;
    if (r27 == (u32)0x0) return;
    r4 = r26;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x3) return;
    r4 = r26;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x1) {
        return;
    }
    r27 = (u32)sp + 0x8;
    r25 = 0x0;
    r26 = 0x0;
    while (1) {
        r0 = r25 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r0 = r25 & 0xFF;
        /* clrlslwi r4, r25, 24, 2 */;
        r0 = r0 * 0x138;
        r3 = (u32)sp + 0x20;
        *(u32*)(r27 + r4) = r26;
        r3 = r3 + r0;
        pokemonInit();
        r25 = r25 + 0x1;

    }
    r0 = r29 & 0xFFFF;
    if (r0 > (u32)0x6) {
        r29 = 0x6;
    }
    r3 = r28;
    r4 = r31;
    r6 = r29;
    r7 = r30;
    r5 = (u32)sp + 0x8;
    fn_802331F4();
    r26 = r29 & 0xFFFF;
    r28 = (u32)sp + 0x8;
    r25 = 0x0;
    r27 = 0x0;
    while (1) {
        r0 = r27 & 0xFF;
        if ((s32)r0 >= (s32)r26) break;
        /* clrlslwi r0, r27, 24, 2 */;
        r29 = *(u32*)(r28 + r0);
        if (r29 != (u32)0x0) {
            r3 = r29;
            pokemonCheckValid();
            r0 = r3 & 0xFF;
            if (r29 != (u32)0x0) {
                r0 = r25 & 0xFF;
                r3 = (u32)sp + 0x20;
                r5 = r0 * 0x138;
                r0 = 0x27;
                /* subi r4, r29, 0x4 */;
                r5 = r3 + r5;
                ctr_fn = (void(*)(void))r0;
                /* subi r5, r5, 0x4 */;
                do {
                    r3 = *(u32*)((u8*)r4 + 0x4);
                    r0 = *(u32*)((u8*)r4 + 0x8);
                    *(u32*)((u8*)r5 + 0x4) = r3;
                    r5 += 8; *(u32*)r5 = r0;
                } while (--ctr != 0);
                r25 = r25 + 0x1;
        }
        }
        r27 = r27 + 0x1;

    }
    r29 = (u32)sp + 0x20;
    r26 = 0x0;
    r28 = 0x27;
    while (1) {
        r0 = r26 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r3 = r31;
        r5 = r26 & 0xFF;
        r4 = 0x3;
        heroGetStatus();
        if (r3 != (u32)0x0) {
            r0 = r26 & 0xFF;
            /* subi r5, r3, 0x4 */;
            r0 = r0 * 0x138;
            r4 = r29 + r0;
            ctr_fn = (void(*)(void))r28;
            /* subi r4, r4, 0x4 */;
            do {
                r3 = *(u32*)((u8*)r4 + 0x4);
                r0 = *(u32*)((u8*)r4 + 0x8);
                *(u32*)((u8*)r5 + 0x4) = r3;
                r5 += 8; *(u32*)r5 = r0;
            } while (--ctr != 0);
        }
        r26 = r26 + 0x1;

    }

    return;
}

/* 0x801F9CBC | size: 0x2BC | large */
void fightTrainerCreateFightTrainerDataIdToHero(void) {
    extern void fn_800896B8();
    extern void fn_800896C8();
    extern void GSmsgGetGSchar();
    extern void savedataGetStatus();
    extern void heroItemAddItemDataId();
    extern void heroCatchPokemon();
    extern void heroCreate();
    extern void heroSetStatus();
    extern void heroGetStatus();
    extern void heroBiosCopy();
    extern void fightTrainerCreateFightTrainerPokemonDataIdToPokemon();
    extern void fightTrainerGetStatus();
    u8 sp[0x150];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r31, r5 */;
    r30 = r3;
    r28 = r4;
    if ((s32)r0 == (s32)0) return;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r30 & 0xFFFF;
    r3 = r3 & 0xFFFF;
    if ((s32)r0 != (s32)0) {

    if (r3 != (u32)0x0 || (s32)r28 != (s32)0x0) {

        r0 = 0x1;
    }
    }
    r0 = r0 & 0xFF;
    if ((s32)r28 == (s32)0x0) return;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;

    } else {

        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

            r0 = 0x1;

        } else {
            r0 = 0x2;
        }
    }
    r0 = r0 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = 0x0;
        r4 = 0x2;
        savedataGetStatus();
        /* mr. r4, r3 */;
        if (r0 == (u32)0x3) return;
        r3 = r31;
        heroBiosCopy();
        return;
    }
    r4 = r30;
    r3 = 0x0;
    r5 = 0x4;
    r6 = 0x0;
    fightTrainerGetStatus();
    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x0;
    fightTrainerGetStatus();
    r29 = r3 & 0xFF;
    r4 = r30;
    r3 = 0x0;
    r5 = 0x3;
    r6 = 0x0;
    fightTrainerGetStatus();
    GSmsgGetGSchar();
    r28 = r3;
    fn_800896B8();
    r0 = r30 & 0xFFFF;
    if (r0 == (u32)r3) {
        fn_800896C8();
        r28 = r3;
    }
    r3 = r31;
    r4 = r28;
    r5 = r29;
    heroCreate();
    r4 = r30;
    r3 = 0x0;
    r5 = 0x5;
    r6 = 0x0;
    fightTrainerGetStatus();
    r29 = r3 & 0xFFFF;
    while (1) {
        r3 = r31;
        r4 = 0x2;
        r5 = 0x0;
        heroGetStatus();
        r5 = r3;
        r3 = r29;
        r4 = (u32)sp + 0x8;
        fightTrainerCreateFightTrainerPokemonDataIdToPokemon();
        r0 = r3 & 0xFF;
        if (r0 != (u32)r3) {
            r3 = (u32)sp + 0x8;
            r7 = r29 & 0xFFFF;
            r4 = 0x0;
            r5 = 0xc9;
            r6 = 0x0;
            ((void(*)(void))pokemonSetStatus)();
            r4 = r29;
            r3 = 0x0;
            r5 = 0x12;
            r6 = 0x0;
            fightTrainerGetStatus();
            r6 = r3 & 0xFFFF;
            r3 = r31;
            r4 = (u32)sp + 0x8;
            r5 = 0x0;
            r7 = 0x0;
            heroCatchPokemon();
            r0 = (s16)r3;
        }
        if (r0 >= (u32)r3) {
            r29 = r29 + 0x1;
    }
    }
    r29 = 0x0;
    while (1) {
        r0 = r29 & 0xFFFF;
        if (r0 >= (u32)0x8) break;
        r4 = r30;
        r6 = r29;
        r3 = 0x0;
        r5 = 0x6;
        fightTrainerGetStatus();
        r4 = r3 & 0xFFFF;
        if (r0 != (u32)r3) {
            r3 = r31;
            r5 = 0x1;
            r6 = -0x1;
            heroItemAddItemDataId();
            if ((s32)r3 < (s32)0x0) break;
        }
        r29 = r29 + 0x1;

    }

    r3 = r31;
    r4 = 0xf;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x10;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x11;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x12;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x13;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x14;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x15;
    r5 = 0x0;
    heroSetStatus();
    r3 = r31;
    r4 = 0x16;
    r5 = 0x0;
    heroSetStatus();

    return;
}

/* 0x801F9F78 | size: 0x53C | large */
void fightTrainerCreateFightTrainerPokemonDataIdToPokemon(void) {
    extern u8 lbl_80279C48[];
    extern u8 lbl_80279C54[];
    extern void GSmsgGetGSchar();
    extern void pokemonSetDarkPokemonStatus();
    extern void pokemonDoItemSoubi();
    extern void pokemonWazaCreate();
    extern void pokemonCreate();
    extern void pokemonCreateRndFit();
    extern void pokemonSetTokuseiFlag();
    extern void pokemonWazaInit();
    extern void pokemonResetBasisStatus();
    extern void fn_80135938();
    extern void fn_801EE750();
    extern void fn_801EE7BC();
    extern void fn_801EE824();
    extern void fn_801EE894();
    extern void fn_801EE8F4();
    extern void fightTrainerGetStatus();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r7 = (u32)lbl_80279C48;
    r6 = (u32)lbl_80279C54;
    r28 = r3;
    r31 = r4;
    r25 = r5;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x15;
    r11 = *(u32*)lbl_80279C48;
    r8 = *(u32*)lbl_80279C54;
    r10 = *(u32*)((u8*)r7 + 0x4);
    r9 = *(u32*)((u8*)r7 + 0x8);
    r7 = *(u32*)((u8*)r6 + 0x4);
    r0 = *(u32*)((u8*)r6 + 0x8);
    r6 = 0x0;
    *(u32*)(sp + 0x1C) = r0;
    fightTrainerGetStatus();
    r23 = r3 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x13;
    r6 = 0x0;
    fightTrainerGetStatus();
    r30 = r3 & 0xFF;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x11;
    r6 = 0x0;
    fightTrainerGetStatus();
    r21 = r3 & 0xFF;
    r4 = r28;
    r3 = 0x0;
    r5 = 0xe;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = (u32)sp + 0xc;
    r22 = r3;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r4 = r28;
        r6 = r19 & 0xFF;
        r3 = 0x0;
        r5 = 0xf;
        fightTrainerGetStatus();
        r0 = r19 & 0xFF;
        r19 = r19 + 0x1;
        *(u8*)(r24 + r0) = r3;

    }
    r24 = (u32)sp + 0x3c;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x6) break;
        r4 = r28;
        r6 = r19 & 0xFF;
        r3 = 0x0;
        r5 = 0x10;
        fightTrainerGetStatus();
        /* clrlslwi r0, r19, 24, 1 */;
        r19 = r19 + 0x1;
        *(u16*)(r24 + r0) = r3;

    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fightTrainerGetStatus();
    r27 = (s8)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x16;
    r6 = 0x0;
    fightTrainerGetStatus();
    r24 = r3;
    r29 = (u32)sp + 0x2c;
    r26 = (u32)sp + 0x8;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFF;
        if (r0 >= (u32)0x4) break;
        r20 = r19 & 0xFF;
        r4 = r28;
        r6 = r20;
        r3 = 0x0;
        r5 = 0x17;
        fightTrainerGetStatus();
        /* clrlslwi r0, r19, 24, 2 */;
        r4 = r28;
        *(u32*)(r29 + r0) = r3;
        r6 = r20;
        r3 = 0x0;
        r5 = 0x18;
        fightTrainerGetStatus();
        *(u8*)(r26 + r20) = r3;
        r19 = r19 + 0x1;

    }
    r4 = r28;
    r3 = 0x0;
    r5 = 0x19;
    r6 = 0x0;
    fightTrainerGetStatus();
    r26 = (s16)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x1a;
    r6 = 0x0;
    fightTrainerGetStatus();
    r29 = (s8)r3;
    r4 = r28;
    r3 = 0x0;
    r5 = 0x1b;
    r6 = 0x0;
    fightTrainerGetStatus();
    r28 = (s8)r3;
    r3 = 0x0;
    r4 = 0x1;
    fn_80135938();
    r6 = r3;
    r3 = r31;
    r4 = r23;
    r5 = r21;
    pokemonCreate();
    if (r22 != (u32)0x0) {
        r3 = r22;
        GSmsgGetGSchar();
        r7 = r3;
        r3 = r31;
        r4 = 0x0;
        r5 = 0x77;
        r6 = 0x0;
        ((void(*)(void))pokemonSetStatus)();
    }
    r23 = r30;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFF;
        if (r0 >= (u32)0x6) break;
        if (r30 != (u32)0x0) {
            r3 = r30;
            fn_801EE8F4();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r30;
                r4 = r22 & 0xFF;
                fn_801EE824();
                r5 = (s8)r3;
                /* clrlslwi r0, r22, 24, 1 */;
                r4 = (u32)sp + 0x20;
                r3 = r31;
                r7 = r5 & 0xFFFF;
                r5 = *(u16*)(r4 + r0);
                r4 = 0x0;
                r6 = 0x0;
                ((void(*)(void))pokemonSetStatus)();
                goto L_801FA250;
        }
        }
        r5 = r22 & 0xFF;
        r3 = (u32)sp + 0xc;
        r4 = *(u8*)(r3 + r5);
        r0 = (s8)r4;
        if (r0 >= (u32)0x1) {
            r0 = r5 << 1;
            r3 = (u32)sp + 0x20;
            r5 = *(u16*)(r3 + r0);
            r3 = r31;
            r7 = (s8)r4;
            r4 = 0x0;
            r6 = 0x0;
            ((void(*)(void))pokemonSetStatus)();
        }
        L_801FA250: ;
        r22 = r22 + 0x1;

    }
    r22 = (u32)sp + 0x3c;
    r21 = (u32)sp + 0x14;
    r20 = 0x0;
    while (1) {
        r0 = r20 & 0xFF;
        if (r0 >= (u32)0x6) break;
        /* clrlslwi r3, r20, 24, 1 */;
        r7 = *(s16*)(r22 + r3);
        r0 = (s16)r7;
        if (r0 >= (u32)0x6) {
            r5 = *(u16*)(r21 + r3);
            r3 = r31;
            r4 = 0x0;
            r6 = 0x0;
            ((void(*)(void))pokemonSetStatus)();
        }
        r20 = r20 + 0x1;

    }
    r0 = (s8)r27;
    if (r0 >= (u32)0x6) {
        r3 = r31;
        r4 = r27 & 0xFF;
        pokemonSetTokuseiFlag();
    }
    if ((s32)r24 >= (s32)0x0) {
        if ((s32)r24 == (s32)0x0) {
            r3 = r31;
            r4 = 0x0;
            r5 = 0x0;
            pokemonDoItemSoubi();
            goto L_801FA2E8;
        }
        r3 = r31;
        r4 = r24 & 0xFFFF;
        r5 = 0x1;
        pokemonDoItemSoubi();
    }
    L_801FA2E8: ;
    r20 = (u32)sp + 0x2c;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFF;
        if (r0 >= (u32)0x4) break;
        /* clrlslwi r0, r24, 24, 2 */;
        r21 = r24 & 0xFF;
        r3 = *(u32*)(r20 + r0);
        if ((s32)r3 >= (s32)0x0) {
            if ((s32)r3 == (s32)0x0 && (s32)r3 == (s32)0x164 && (s32)r3 == (s32)0x165 && (s32)r3 == (s32)0x163) {

                r3 = r31;
                r4 = r21;
                pokemonWazaInit();
                goto L_801FA368;
            }
            r0 = *(u32*)(r20 + r0);
            r22 = r24 & 0xFF;
            r3 = r31;
            r4 = r22;
            r5 = r0 & 0xFFFF;
            pokemonWazaCreate();
            r4 = (u32)sp + 0x8;
            r3 = r31;
            r7 = *(u8*)(r4 + r21);
            r6 = r22;
            r4 = 0x0;
            r5 = 0x81;
            ((void(*)(void))pokemonSetStatus)();
        }
        L_801FA368: ;
        r24 = r24 + 0x1;

    }
    r0 = (s16)r26;
    if (r0 >= (u32)0x4) {
        r3 = r31;
        r7 = r26;
        r4 = 0x0;
        r5 = 0x99;
        r6 = 0x0;
        ((void(*)(void))pokemonSetStatus)();
    }
    if (r30 != (u32)0x0) {
        r3 = r23;
        fn_801EE8F4();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r23;
            fn_801EE750();
            r7 = r3;
            r3 = r31;
            r4 = 0x0;
            r5 = 0x6f;
            r6 = 0x0;
            ((void(*)(void))pokemonSetStatus)();
            goto L_801FA408;
    }
    }
    r3 = r31;
    r4 = r29;
    r5 = r28;
    r7 = r25;
    r6 = 0x0;
    pokemonCreateRndFit();
    r7 = r3;
    r3 = r31;
    r4 = 0x0;
    r5 = 0x6f;
    r6 = 0x0;
    ((void(*)(void))pokemonSetStatus)();
    L_801FA408: ;
    if (r30 != (u32)0x0) {
        r3 = r31;
        r4 = r23;
        pokemonSetDarkPokemonStatus();
        r3 = r23;
        fn_801EE8F4();
        r0 = r3 & 0xFF;
        if (r30 == (u32)0x0) {
            r3 = r31;
            r4 = 0x0;
            r5 = 0x6f;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r4 = r3;
            r3 = r23;
            fn_801EE7BC();
            r21 = (u32)sp + 0x20;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFF;
                if (r0 >= (u32)0x6) break;
                /* clrlslwi r0, r20, 24, 1 */;
                r3 = r31;
                r5 = *(u16*)(r21 + r0);
                r4 = 0x0;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r0 = r3 & 0xFFFF;
                r3 = r23;
                r4 = r20 & 0xFF;
                r5 = (s8)r0;
                fn_801EE894();
                r20 = r20 + 0x1;

            }
    }
    }
    r3 = r31;
    pokemonResetBasisStatus();
    r3 = 0x1;

    return;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FA4B4_801FA524)
/* 0x801FA4B4 | size: 0x70 */
BOOL fightTrainerCheckTrainerDataIdValid(u32 arg0, s32 arg1) {
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    u32 result;

    result = (u16)fightTrainerGetStatus(0, arg0, 2, 0);
    if ((u16)arg0 == 0) {
        return FALSE;
    }
    if (result == 0 && arg1 == 0) {
        return FALSE;
    }
    return TRUE;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801FA524 | size: 0x110 */
u32 fightTrainerCheckDoFight(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fightPokemonCheckFightOut(void* ptr);
    u8 valid;
    u8 count;
    u8 i;

    valid = 0;
    if (context != NULL) {
        if ((u16)fn_801EF634(context) != 1) {
            if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) != 0) {
                if (fightTrainerGetStatus(context, 0, 0x44, 0) != NULL) {
                    if ((u8)heroCheckValid(fightTrainerGetStatus(context, 0, 0x44, 0)) != 0) {
                        valid = 1;
                    }
                }
            }
        }
    }
    if (valid == 0) {
        return 0;
    }
    count = 0;
    for (i = count; i < 6; i++) {
        if ((u8)fightPokemonCheckFightOut(fightTrainerGetStatus(context, 0, 0x45, i)) != 0) {
            count = (u8)(count + 1);
        }
    }
    return count != 0;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FA634_801FAA58)
/* 0x801FA634 | size: 0xA4 */
u32 fightTrainerCheckValid(void* context) {
    extern u16 fn_801EF634(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* result;

    if (context == NULL) {
        return 0;
    }
    if ((u16)fn_801EF634(context) == 1) {
        return 0;
    }
    if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
        return 0;
    }
    result = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (result == NULL) {
        return 0;
    }
    return (u8)heroCheckValid(result) != 0;
}

/* 0x801FA6D8 | size: 0x1F4 | medium */
void fightTrainerCreate(void* trainer, void* arg1, u16 arg2, u32 arg3, u32 arg4) {
    extern void heroInit(void* ptr);
    extern void heroBiosCopy(void* ptr, void* arg);
    extern void fightActionInit(void* ptr);
    extern void fightFloorSetShadow(void);
    extern void fightTrainerSetStatus(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fightOutPokemonInitAry(void* ptr, u32 count);
    extern void fightOutPokemonInit(void* ptr);
    extern void fightPokemonInitAry(void* ptr, u32 count);
    extern void fightTrainerEnemyPokemonInitAry(void* ptr, u32 count);
    void* result;

    if (trainer != NULL && arg1 != NULL) {
        if (trainer != NULL) {
            heroInit(fightTrainerGetStatus(trainer, 0, 0x44, 0));
            fightTrainerSetStatus(trainer, 0, 0x43, 0, 0);
            fightPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x45, 0), 6);
            fightOutPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x46, 0), 2);
            fightOutPokemonInit(fightTrainerGetStatus(trainer, 0, 0x47, 0));
            fightTrainerSetStatus(trainer, 0, 0x48, 0, 1);
            fightTrainerSetStatus(trainer, 0, 0x49, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4A, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4C, 0, 0);
            result = fightTrainerGetStatus(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fightTrainerEnemyPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x4E, 0), 0xC);
        }
        result = fightTrainerGetStatus(trainer, 0, 0x44, 0);
        if (result != NULL) {
            heroBiosCopy(result, arg1);
            fightTrainerSetStatus(trainer, 0, 0x43, 0, arg2);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, arg3);
            if (arg4 != 0) {
                fightTrainerSetStatus(trainer, 0, 0x4C, 0, arg4);
                fightFloorSetShadow();
            }
        }
    }
}

/* 0x801FA8CC | size: 0x18C */
void fightTrainerInit(void* base, u16 count) {
    extern void heroInit(void* ptr);
    extern void fightActionInit(void* ptr);
    extern void fightTrainerSetStatus(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fightOutPokemonInitAry(void* ptr, u32 count);
    extern void fightOutPokemonInit(void* ptr);
    extern void fightPokemonInitAry(void* ptr, u32 count);
    extern void fightTrainerEnemyPokemonInitAry(void* ptr, u32 count);
    u8* trainer;
    void* result;
    u16 i;

    if (base == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        trainer = (u8*)base + i * 0x28E4;
        if (trainer != NULL) {
            heroInit(fightTrainerGetStatus(trainer, 0, 0x44, 0));
            fightTrainerSetStatus(trainer, 0, 0x43, 0, 0);
            fightPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x45, 0), 6);
            fightOutPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x46, 0), 2);
            fightOutPokemonInit(fightTrainerGetStatus(trainer, 0, 0x47, 0));
            fightTrainerSetStatus(trainer, 0, 0x48, 0, 1);
            fightTrainerSetStatus(trainer, 0, 0x49, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4A, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4C, 0, 0);
            result = fightTrainerGetStatus(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fightTrainerEnemyPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x4E, 0), 0xC);
        }
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801FAA58 | size: 0x768 | large */
void fightTrainerSetStatus(void) {
    extern void fightTrainerGetStatus();
    extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetStoreTokuseiData();
    extern void fightTrainerEnemyPokemonBiosSetTokuseiFlag();
    extern void fn_801FBDF4();
    extern void fightTrainerAiValueAddsubDataBiosSetPrefixName();
    extern void fightTrainerAiValueAddsubDataBiosSetKoudouName();
    extern void fightTrainerAiValueAddsubDataBiosSetName();
    extern void fightTrainerAiValueAddsubDataBiosSetValue();
    extern void fightTrainerAiDataBiosSetLastValueRevise();
    extern void fightTrainerAiDataBiosSetWazaNokoriPpValue();
    extern void fightTrainerAiDataBiosSetWazaRiskFlag();
    extern void fightTrainerAiDataBiosSetWazaAvgValue();
    extern void fightTrainerAiDataBiosSetWazaHitFlag();
    extern void fightTrainerAiDataBiosSetWazaTypeReviseValue();
    extern void fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId();
    extern void fightTrainerAiDataBiosSetPartFlag();
    extern void fightTrainerAiDataBiosSetWazaInitValueFlag();
    extern void fightTrainerAiDataBiosSetWazaRndSelectFlag();
    extern void fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag();
    extern void fightTrainerAiDataBiosSetAbicntMinValue();
    extern void fightTrainerAiDataBiosSetAbicntMaxValue();
    extern void fightTrainerAiDataBiosSetWazaDamageFlag();
    extern void fightTrainerAiDataBiosSetNokoriHpValue();
    extern void fightTrainerAiDataBiosSetTokuseiCheckFlag();
    extern void fightTrainerAiDataBiosSetZokuseiReviseValue();
    extern void fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId();
    extern void fightTrainerAiDataBiosSetZokuseiCheckFlag();
    extern void fightTrainerAiDataBiosSetParamStoreFlag();
    extern void fightTrainerAiDataBiosSetParamExpectFlag();
    extern void fightTrainerAiDataBiosSetItemValue();
    extern void fightTrainerAiDataBiosSetIrekaeValue();
    extern void fightTrainerAiDataBiosSetComboValue();
    extern void fightTrainerAiDataBiosSetPokemonJoutaiFlag();
    extern void fightTrainerAiDataBiosSetKeyPlayerFlag();
    extern void fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag();
    extern void fightTrainerAiDataBiosSetPokemonDataOrderOutFlag();
    extern void fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag();
    extern void fightTrainerAiDataBiosSetPokemonSelectRandomFlag();
    extern void fightTrainerPokemonDataBiosSetPartDataId();
    extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag();
    extern void fightTrainerPokemonDataBiosSetSeikakuDataId();
    extern void fightTrainerPokemonDataBiosSetSexDataId();
    extern void fightTrainerPokemonDataBiosSetFriend();
    extern void fightTrainerPokemonDataBiosSetPpCnt();
    extern void fightTrainerPokemonDataBiosSetWazaDataId();
    extern void fightTrainerPokemonDataBiosSetItemDataId();
    extern void fightTrainerPokemonDataBiosSetPokemonDataId();
    extern void fightTrainerPokemonDataBiosSetTokuseiFlag();
    extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag();
    extern void fightTrainerPokemonDataBiosSetItemBallId();
    extern void fightTrainerPokemonDataBiosSetLevel();
    extern void fightTrainerPokemonDataBiosSetStatusEffort();
    extern void fightTrainerPokemonDataBiosSetStatusRnd();
    extern void fightTrainerPokemonDataBiosSetNickname();
    extern void fightTrainerPokemonPartDataBiosSetWazaTypeRevise();
    extern void fightTrainerPokemonPartDataBiosSetName();
    extern void fn_801FCAFC();
    extern void fn_801FCB0C();
    extern void fn_801FCB30();
    extern void fn_801FCB40();
    extern void fn_801FCB64();
    extern void fn_801FCB74();
    extern void fn_801FCB84();
    extern void fn_801FCB94();
    extern void fightTrainerDataBiosSetKindDataId();
    extern void fightTrainer_SetControllerId();
    extern void fightTrainer_SetNigeruCount();
    extern void fightTrainer_SetKoban();
    extern void fightTrainer_SetOkaneBai();
    extern void fightTrainer_SetFightTrainerDataId();
    extern void fightTrainer_SetSequencePtr();
    extern void fightTrainerEnemyPokemonEraseAry();
    extern void fightTrainerEnemyPokemonRegistAry();
    extern void fightTrainerEnemyPokemonSearchAry();
    extern void fightTrainerEnemyPokemonInitFightOutStatus();
    extern u8 jumptable_80375670[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r30 = r5;
    r28 = r4;
    r29 = r6;
    r31 = r7;
    if ((s32)r0 == (s32)0) return;
    if (r0 >= (u32)0x5b) {
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r28;
        ((void(*)(void))fightTrainerDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0xd) {
        r3 = r28;
        ((void(*)(void))fightTrainerPokemonPartDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x1e) {
        r3 = r28;
        ((void(*)(void))fightTrainerPokemonDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x3d) {
        r3 = r28;
        ((void(*)(void))fightTrainerAiDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x42) {
        r3 = r28;
        ((void(*)(void))fightTrainerAiValueAddsubDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;
    }
    if (r3 == (u32)0x0) return;
    r0 = r30 & 0xFFFF;
    if (r0 > (u32)0x59) return;
    r4 = (u32)jumptable_80375670;
    r0 = r0 << 2;
    r4 = (u32)jumptable_80375670;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r4 = r31 & 0xFF;
    fn_801FCB94();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCB84();
    return;
    r4 = r31;
    fn_801FCB74();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerDataBiosSetKindDataId();
    return;
    r4 = r31 & 0xFFFF;
    fn_801FCB64();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fn_801FCB40();
    return;
    r4 = r31;
    fn_801FCB30();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fn_801FCB0C();
    return;
    r4 = r31;
    fn_801FCAFC();
    return;
    r4 = r31;
    fightTrainerPokemonPartDataBiosSetName();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fightTrainerPokemonPartDataBiosSetWazaTypeRevise();
    return;
    r4 = r31;
    fightTrainerPokemonDataBiosSetNickname();
    return;
    r4 = r29 & 0xFF;
    r5 = (s8)r31;
    fightTrainerPokemonDataBiosSetStatusRnd();
    return;
    r4 = r29 & 0xFF;
    r5 = (s16)r31;
    fightTrainerPokemonDataBiosSetStatusEffort();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetLevel();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerPokemonDataBiosSetItemBallId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetDarkPokemonFlag();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetTokuseiFlag();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerPokemonDataBiosSetPokemonDataId();
    return;
    r4 = r31;
    fightTrainerPokemonDataBiosSetItemDataId();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fightTrainerPokemonDataBiosSetWazaDataId();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetPpCnt();
    return;
    r4 = (s16)r31;
    fightTrainerPokemonDataBiosSetFriend();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetSexDataId();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetSeikakuDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetKeyPlayerFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetPartDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonSelectRandomFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonDataOrderOutFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetKeyPlayerFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonJoutaiFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetComboValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetIrekaeValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetItemValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetParamExpectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetParamStoreFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiCheckFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetTokuseiCheckFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetNokoriHpValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaDamageFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetAbicntMaxValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetAbicntMinValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaRndSelectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaInitValueFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPartFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaHitFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaAvgValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaRiskFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaNokoriPpValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetLastValueRevise();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiReviseValue();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaTypeReviseValue();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetValue();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetName();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetKoudouName();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetPrefixName();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainer_SetFightTrainerDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetOkaneBai();
    return;
    r4 = r31;
    fightTrainer_SetKoban();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetNigeruCount();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetControllerId();
    return;
    r4 = r31;
    fightTrainer_SetSequencePtr();
    return;
    r30 = (s16)r28;
    if (r0 >= (u32)0x59) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fn_801FBDF4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fightTrainerEnemyPokemonBiosSetTokuseiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fightTrainerEnemyPokemonBiosSetStoreTokuseiData();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fightTrainerEnemyPokemonRegistAry();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fightTrainerEnemyPokemonEraseAry();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    fightTrainerEnemyPokemonInitFightOutStatus();

    return;
}

/* 0x801FB1C0 | size: 0x738 | large */
void fightTrainerGetStatus(void) {
    extern void fightTrainerGetStatus();
    extern void fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetStoreTokuseiData();
    extern void fightTrainerEnemyPokemonBiosGetTokuseiFlag();
    extern void fn_801FBED0();
    extern void fightTrainerAiValueAddsubDataBiosGetPrefixName();
    extern void fightTrainerAiValueAddsubDataBiosGetKoudouName();
    extern void fightTrainerAiValueAddsubDataBiosGetName();
    extern void fightTrainerAiValueAddsubDataBiosGetValue();
    extern void fightTrainerAiDataBiosGetLastValueRevise();
    extern void fightTrainerAiDataBiosGetWazaNokoriPpValue();
    extern void fightTrainerAiDataBiosGetWazaRiskFlag();
    extern void fightTrainerAiDataBiosGetWazaAvgValue();
    extern void fightTrainerAiDataBiosGetWazaHitFlag();
    extern void fightTrainerAiDataBiosGetWazaTypeReviseValue();
    extern void fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId();
    extern void fightTrainerAiDataBiosGetPartFlag();
    extern void fightTrainerAiDataBiosGetWazaInitValueFlag();
    extern void fightTrainerAiDataBiosGetWazaRndSelectFlag();
    extern void fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag();
    extern void fightTrainerAiDataBiosGetAbicntMinValue();
    extern void fightTrainerAiDataBiosGetAbicntMaxValue();
    extern void fightTrainerAiDataBiosGetWazaDamageFlag();
    extern void fightTrainerAiDataBiosGetNokoriHpValue();
    extern void fightTrainerAiDataBiosGetTokuseiCheckFlag();
    extern void fightTrainerAiDataBiosGetZokuseiReviseValue();
    extern void fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId();
    extern void fightTrainerAiDataBiosGetZokuseiCheckFlag();
    extern void fightTrainerAiDataBiosGetParamStoreFlag();
    extern void fightTrainerAiDataBiosGetParamExpectFlag();
    extern void fightTrainerAiDataBiosGetItemValue();
    extern void fightTrainerAiDataBiosGetIrekaeValue();
    extern void fightTrainerAiDataBiosGetComboValue();
    extern void fightTrainerAiDataBiosGetPokemonJoutaiFlag();
    extern void fightTrainerAiDataBiosGetKeyPlayerFlag();
    extern void fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag();
    extern void fightTrainerAiDataBiosGetPokemonDataOrderOutFlag();
    extern void fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag();
    extern void fightTrainerAiDataBiosGetPokemonSelectRandomFlag();
    extern void fightTrainerPokemonDataBiosGetPartDataId();
    extern void fightTrainerPokemonDataBiosGetKeyPlayerFlag();
    extern void fightTrainerPokemonDataBiosGetSeikakuDataId();
    extern void fightTrainerPokemonDataBiosGetSexDataId();
    extern void fightTrainerPokemonDataBiosGetFriend();
    extern void fightTrainerPokemonDataBiosGetPpCnt();
    extern void fightTrainerPokemonDataBiosGetWazaDataId();
    extern void fightTrainerPokemonDataBiosGetItemDataId();
    extern void fightTrainerPokemonDataBiosGetPokemonDataId();
    extern void fightTrainerPokemonDataBiosGetTokuseiFlag();
    extern void fightTrainerPokemonDataBiosGetDarkPokemonFlag();
    extern void fightTrainerPokemonDataBiosGetItemBallId();
    extern void fightTrainerPokemonDataBiosGetLevel();
    extern void fightTrainerPokemonDataBiosGetStatusEffort();
    extern void fightTrainerPokemonDataBiosGetStatusRnd();
    extern void fightTrainerPokemonDataBiosGetNickname();
    extern void fightTrainerPokemonPartDataBiosGetWazaTypeRevise();
    extern void fightTrainerPokemonPartDataBiosGetName();
    extern void fn_801FCBA4();
    extern void fn_801FCBBC();
    extern void fn_801FCBF0();
    extern void fn_801FCC08();
    extern void fn_801FCC3C();
    extern void fightTrainerDataBiosGetKindDataId();
    extern void fn_801FCC7C();
    extern void fn_801FCC94();
    extern void fn_801FCCAC();
    extern void fightTrainer_GetFightActionBuffPtr();
    extern void fightTrainer_GetFightTrainerEnemyPokemonAryPtr();
    extern void fightTrainer_GetSequencePtr();
    extern void fightTrainer_GetControllerId();
    extern void fightTrainer_GetNigeruCount();
    extern void fightTrainer_GetKoban();
    extern void fightTrainer_GetOkaneBai();
    extern void fightTrainer_GetFightoutPokemonBuffPtr();
    extern void fightTrainer_GetFightoutPokemonPtr();
    extern void fightTrainer_GetFightPokemonPtr();
    extern void fightTrainer_GetFightTrainerDataId();
    extern void fightTrainer_GetHeroPtr();
    extern void fightTrainerEnemyPokemonSearchAry();
    extern u8 jumptable_803757D8[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r31 = r5;
    r29 = r4;
    r30 = r6;
    if ((s32)r0 == (s32)0) { r3 = 0x0; return; }
    if (r0 >= (u32)0x5b) {

        r3 = 0x0;
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r29;
        ((void(*)(void))fightTrainerDataBiosGetPtr)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0xd) {
            r3 = r29;
            ((void(*)(void))fightTrainerPokemonPartDataBiosGetPtr)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
            }
            if (r0 < (u32)0x1e) {
                r3 = r29;
                ((void(*)(void))fightTrainerPokemonDataBiosGetPtr)();
                if (r3 == (u32)0x0) {
                    r3 = 0x0;
                    return;
                }
                if (r0 < (u32)0x3d) {
                    r3 = r29;
                    ((void(*)(void))fightTrainerAiDataBiosGetPtr)();
        }
        }
        }
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0x42) {
            r3 = r29;
            ((void(*)(void))fightTrainerAiValueAddsubDataBiosGetPtr)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
        }
        }
        }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r0 = r31 & 0xFFFF;
    if (r0 > (u32)0x56) { r3 = 0x0; return; }
    r4 = (u32)jumptable_803757D8;
    r0 = r0 << 2;
    r4 = (u32)jumptable_803757D8;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    fn_801FCCAC();
    r3 = r3 & 0xFF;
    return;
    fn_801FCC94();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC7C();
    return;
    fightTrainerDataBiosGetKindDataId();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC3C();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30 & 0xFF;
    fn_801FCC08();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCBF0();
    return;
    r4 = r30 & 0xFF;
    fn_801FCBBC();
    return;
    fn_801FCBA4();
    return;
    fightTrainerPokemonPartDataBiosGetName();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonPartDataBiosGetWazaTypeRevise();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetNickname();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetStatusRnd();
    r3 = (s8)r3;
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetStatusEffort();
    r3 = (s16)r3;
    return;
    fightTrainerPokemonDataBiosGetLevel();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetItemBallId();
    r3 = r3 & 0xFFFF;
    return;
    fightTrainerPokemonDataBiosGetDarkPokemonFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetTokuseiFlag();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetPokemonDataId();
    r3 = r3 & 0xFFFF;
    return;
    fightTrainerPokemonDataBiosGetItemDataId();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetWazaDataId();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetPpCnt();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetFriend();
    r3 = (s16)r3;
    return;
    fightTrainerPokemonDataBiosGetSexDataId();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetSeikakuDataId();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetKeyPlayerFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetPartDataId();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonSelectRandomFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonDataOrderOutFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetKeyPlayerFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonJoutaiFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetComboValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetIrekaeValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetItemValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetParamExpectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetParamStoreFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetZokuseiCheckFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetTokuseiCheckFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetNokoriHpValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaDamageFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetAbicntMaxValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetAbicntMinValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaRndSelectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaInitValueFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPartFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaHitFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaAvgValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaRiskFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaNokoriPpValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetLastValueRevise();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetZokuseiReviseValue();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetWazaTypeReviseValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiValueAddsubDataBiosGetValue();
    return;
    fightTrainerAiValueAddsubDataBiosGetName();
    return;
    fightTrainerAiValueAddsubDataBiosGetKoudouName();
    return;
    fightTrainerAiValueAddsubDataBiosGetPrefixName();
    return;
    fightTrainer_GetHeroPtr();
    return;
    fightTrainer_GetFightTrainerDataId();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30;
    fightTrainer_GetFightPokemonPtr();
    return;
    r4 = r30;
    fightTrainer_GetFightoutPokemonPtr();
    return;
    fightTrainer_GetFightoutPokemonBuffPtr();
    return;
    fightTrainer_GetOkaneBai();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetKoban();
    return;
    fightTrainer_GetNigeruCount();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetControllerId();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetFightActionBuffPtr();
    return;
    fightTrainer_GetSequencePtr();
    return;
    r4 = r30 & 0xFF;
    fightTrainer_GetFightTrainerEnemyPokemonAryPtr();
    return;
    r31 = (s16)r29;
    if (r0 >= (u32)0x56) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r4 = r30 & 0xFF;
    fn_801FBED0();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetTokuseiFlag();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetStoreTokuseiData();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag();
    r3 = r3 & 0xFF;
    return;

    r3 = 0x0;

    return;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FB8F8_801FBAD4)
/* 0x801FB8F8 | size: 0x7C */
u8 fightTrainerIsGcHero(void* arg0) {
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    u32 val;
    u32 category;

    val = (u16)fightTrainerGetStatus(arg0, 0, 0x43, 0);
    val = (u16)fightTrainerGetStatus(0, val, 4, 0);
    if (val == 1) {
        category = 0;
    } else if (val == 2 || val == 3) {
        category = 1;
    } else {
        category = 2;
    }
    return (u8)category == 0;
}

/* 0x801FB974 | size: 0xB0 */
void fn_801FB974(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, 0x57, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, 0x57, 4);
    }
}

/* 0x801FBA24 | size: 0xB0 */
void fn_801FBA24(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x74, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, 0x74, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, 0x74, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, 0x74, 4);
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801FBAD4 | size: 0x14C */
void fightTrainerHokakuThrowEffect(void* context, u32 param, u8 mode) {
    extern void _threadSwitch(void* ptr);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;
    u16 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    animId = (u16)itemGetStatus(0, param, 0x12, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch(data);
        } while (1);
    } else if (mode == 2) {
        fn_801DA8C4(data, animId, 4);
    } else if (mode == 3) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 4) {
        fn_801DA9E8(data, 0x57, 4);
    } else if (mode == 5) {
        do {
            if ((u8)fn_801DA94C(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch(data);
        } while (1);
    } else if (mode == 6) {
        fn_801DA8C4(data, 0x57, 4);
    }
}

/* 0x801FBC20 | size: 0xF0 */
void fightTrainerBallThrowEffect(void* context, void* trainerCtx, u8 mode) {
    extern void _threadSwitch(void);
    extern void fn_801DA8C4(void* ptr, u32 field, u32 size);
    extern u8 fn_801DA94C(void* ptr, u32 field, u32 size);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void* fightOutPokemonGetPokemonPtr(void* ctx);
    void* data;
    u8 typeId;
    u16 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    typeId = (u8)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainerCtx), 0, 0x73, 0);
    animId = (u16)itemGetStatus(0, typeId, 0x11, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        fn_801DA9E8(data, animId, 4);
    } else if (mode == 2) {
        do {
            if ((u8)fn_801DA94C(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        fn_801DA8C4(data, animId, 4);
    }
}
#endif

#undef FTR_BANK_EXACT_ACTIVE
