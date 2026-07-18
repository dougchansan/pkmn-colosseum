/**
 * @file fight_out_pokemon.c
 * @brief fightOutPokemon + fightPokemon prefix, address range
 *        0x80202810-0x80206A04, 62 functions.
 *
 * OutPokemon/Pokemon field accessors, sequence/status writers, and
 * damage-calc support the seq/waza layers call into (statusGetStatus,
 * fadeEffectGetRandom callers, etc). Corresponds to XD's
 * fight.cpp fightOutPokemon+fightPokemon cluster (0x80200644-0x80208288).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct StatusIdTable7 {
    u16 id[7];
} StatusIdTable7;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

/* 0x80202810 | size: 0x188 | medium */
/* 0x80202810 | size: 0x188 */
void fightOutPokemonWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B788();
    extern void fn_80121B4C();
    extern void fn_801DA36C();
    void* eeData;

    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    if ((u16)(u32)typeObj == 0) {
        if (eeData != NULL) {
            fn_801DA36C(eeData, 1);
            fn_801DA36C(eeData, 2);
        }
    } else {
        if (eeData != NULL) {
            if ((u16)(u32)typeObj == 8) {
                fn_801DA36C(eeData, 1);
            }
            if ((u16)(u32)typeObj == 7) {
                fn_801DA36C(eeData, 2);
            }
        }
    }
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8 || fn_80119ED0(typeObj) == 0xCD) {
        eeData = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
            if (eeData == NULL) {
                eeData = NULL;
            } else {
                eeData = pokemonGetStatus(eeData, 0, 0xCC, 0);
            }
            fn_80121B4C(eeData, typeObj);
        } else if (fn_80119ED0(typeObj) == 0xCD) {
            fn_8011B788(eeData, typeObj);
        }
    } else if (fn_80119ED0(typeObj) == 0xD8) {
        fn_8011B788(ctx, typeObj);
    }
}

/* 0x80202998 | size: 0x94 */
void fightOutPokemonResetSeqStatus(void* ctx, u16 mode) {
    extern void fn_801DA36C();
    void* obj;
    u16 modeVal;
    obj = pokemonGetStatus(ctx, 0, 0xEE, 0);
    modeVal = mode;
    if (modeVal == 0) {
        if (obj != NULL) {
            fn_801DA36C(obj, 1);
            fn_801DA36C(obj, 2);
        }
    } else {
        if (obj != NULL) {
            if (modeVal == 8) {
                fn_801DA36C(obj, 1);
            }
            modeVal = mode;
            if (modeVal == 7) {
                fn_801DA36C(obj, 2);
            }
        }
    }
}

/* 0x80202A2C | size: 0xB0 */
void fightPokemonWriteJoutaiDataId(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011AFCC();
    extern void fn_8012190C();
    void* resolved;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        fn_8012190C(resolved, typeObj, param);
    } else if (fn_80119ED0(typeObj) == 0xCD) {
        fn_8011AFCC(ctx, typeObj, param);
    }
}

/* 0x80202ADC | size: 0xAC */
u32 fightPokemonCheckWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern u32 fn_8011B67C();
    extern u32 fn_80121ADC();
    void* resolved;
    u32 result;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        result = fn_80121ADC(resolved, typeObj);
    } else if (fn_80119ED0(typeObj) != 0xCD) {
        result = 0;
    } else {
        result = fn_8011B67C(ctx, typeObj);
    }
    return result;
}

/* 0x80202B88 | size: 0x94 */
u32 fightOutPokemonIsAlly(void* obj1, void* obj2) {
    extern u32 fightTargetGetPtr();
    extern u16 fightFloorGetStatus();
    u16 tableId;
    u32 val1;
    u32 val2;
    tableId = 0xFFFF & fightFloorGetStatus(NULL, 0, 0x14, 0);
    if (obj1 == NULL) {
        return 0;
    }
    if (obj2 == NULL) {
        return 0;
    }
    val1 = fightTargetGetPtr(2, obj1, tableId);
    val2 = fightTargetGetPtr(2, obj2, tableId);
    return (u8)(val1 == val2);
}

/* Address: 0x80202C1C | Size: 0x57c | Ghidra import */

void fn_80202C1C(u32 r3,u32 r4)

{
    extern s8 pokemonCheckFightOut();
    extern s8 pokemonCheckValid();
    extern u16 fn_801EF634();
    extern u32 fightFloorGetStatus();
    extern u32 fightSideGetStatus();
    extern s8 fightTrainerCheckValid();
    extern int fightTrainerGetStatus();
    extern int fightOutPokemonEnemySearchAry();
    extern s8 fightOutPokemonEnemyCheckValid();
    extern void fightOutPokemonEnemyCreate();
  u8 bVar1;
  u16 uVar2;
  u16 uVar3;
  u32 uVar4;
  u8 cVar9;
  u32 iVar5;
  u16 sVar8;
  u32 iVar6;
  u32 iVar7;
  u16 uVar10;
  u32 uVar11;
  u32 uVar12;
  
  fightFloorGetStatus(0,0,0x14,0);
  uVar2 = fightFloorGetStatus(0,0,0x16,0);
  uVar3 = fightFloorGetStatus(0,0,0x18,0);
  for (uVar12 = 0; (uVar12 & 0xffff) < (uVar2 & 0xffff); uVar12 = uVar12 + 1) {
    uVar4 = fightSideGetStatus(r4,0,7,uVar12);
    cVar9 = fightTrainerCheckValid();
    if (cVar9 != 0) {
      for (uVar11 = 0; (uVar11 & 0xffff) < (uVar3 & 0xffff); uVar11 = uVar11 + 1) {
        iVar5 = fightTrainerGetStatus(uVar4,0,0x46,uVar11);
        if (iVar5 == 0) {
          bVar1 = 0;
        }
        else {
          if (iVar5 == 0) {
            bVar1 = 0;
            goto initial_valid_done;
          }
          sVar8 = fn_801EF634();
          if (sVar8 == 1) {
            bVar1 = 0;
          }
          else {
            iVar6 = (int)pokemonGetStatus(iVar5,0,0xd6,0);
            if (iVar6 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar6 == 0) {
                bVar1 = 0;
                goto initial_valid_done;
              }
              sVar8 = fn_801EF634();
              if (sVar8 == 1) {
                bVar1 = 0;
              }
              else {
                iVar7 = (int)pokemonGetStatus(iVar6,0,0xcb,0);
                if (iVar7 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar9 = pokemonCheckValid();
                  if (cVar9 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar6 == 0) {
                      iVar7 = 0;
                    }
                    else {
                      iVar7 = (int)pokemonGetStatus(iVar6,0,0xcc,0);
                    }
                    if (iVar7 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar9 = pokemonCheckValid();
                      if (cVar9 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        iVar6 = (int)pokemonGetStatus(iVar6,0,0xce,0);
                        if ((s32)iVar6 < 0) {
                          bVar1 = 0;
                        }
                        else {
                          bVar1 = 1;
                        }
                      }
                    }
                  }
                }
              }
              if (!bVar1) {
                bVar1 = 0;
              }
              else {
                bVar1 = 1;
              }
            }
          }
        initial_valid_done:
          if (!bVar1) {
            bVar1 = 0;
            goto initial_stage_done;
          }
          {
            iVar6 = (int)pokemonGetStatus(iVar5,0,0x120,0);
            if ((s32)iVar6 == 1) {
              bVar1 = 0;
            }
            else {
              iVar6 = (int)pokemonGetStatus(iVar5,0,0xd6,0);
              if (iVar6 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar6 == 0) {
                  bVar1 = 0;
                  goto second_valid_done;
                }
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)pokemonGetStatus(iVar6,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar9 = pokemonCheckValid();
                    if (cVar9 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar6 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)pokemonGetStatus(iVar6,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar9 = pokemonCheckValid();
                        if (cVar9 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar7 = (int)pokemonGetStatus(iVar6,0,0xce,0);
                          if ((s32)iVar7 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
              second_valid_done:
                if (!bVar1) {
                  bVar1 = 0;
                  goto second_stage_done;
                }
                {
                  iVar7 = (int)pokemonGetStatus(iVar6,0,0xd2,0);
                  if ((s32)iVar7 == 1) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar6 == 0) {
                      iVar6 = 0;
                    }
                    else {
                      iVar6 = (int)pokemonGetStatus(iVar6,0,0xcc,0);
                    }
                    if (iVar6 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar9 = pokemonCheckFightOut();
                      if (cVar9 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
              second_stage_done:
                ;
              }
              if (!bVar1) {
                bVar1 = 0;
              }
              else {
                bVar1 = 1;
              }
            }
          }
        initial_stage_done:
          ;
        }
        if ((bVar1) && (r3 != 0)) {
          if (iVar5 == 0) {
            bVar1 = 0;
          }
          else {
            sVar8 = fn_801EF634();
            if (sVar8 == 1) {
              bVar1 = 0;
            }
            else {
              iVar6 = (int)pokemonGetStatus(iVar5,0,0xd6,0);
              if (iVar6 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar6 == 0) {
                  bVar1 = 0;
                  goto third_valid_done;
                }
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)pokemonGetStatus(iVar6,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar9 = pokemonCheckValid();
                    if (cVar9 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar6 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)pokemonGetStatus(iVar6,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar9 = pokemonCheckValid();
                        if (cVar9 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar6 = (int)pokemonGetStatus(iVar6,0,0xce,0);
                          if ((s32)iVar6 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
                if (!bVar1) {
                  bVar1 = 0;
                }
                else {
                  bVar1 = 1;
                }
              }
            }
          }
        third_valid_done:
          if (bVar1) {
            iVar6 = (int)pokemonGetStatus(r3,0,0x122,0);
            iVar7 = fightOutPokemonEnemySearchAry(iVar6,4,iVar5);
            if (iVar7 == 0) {
              for (uVar10 = 0; uVar10 < 4; uVar10++) {
                iVar7 = iVar6 + (u32)uVar10 * 0xc;
                cVar9 = fightOutPokemonEnemyCheckValid(iVar7);
                if (cVar9 == 0) {
                  fightOutPokemonEnemyCreate(iVar7,iVar5);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}

/* 0x80203198 | size: 0x14C | medium */
/* 0x80203198 | size: 0x14C */
void fn_80203198(void* ctx, u32 param) {
    extern u16 fightOutPokemonEnemyBiosGetOumuWazaDataId();
    extern void* fightOutPokemonEnemySearchAry();
    extern u8 fightOutPokemonEnemyCheckValid();
    extern void fightOutPokemonEnemyInit();
    void* tableData;
    void* entry;
    void* entryPtr;
    u16 species;
    u8 count;
    u8 i;

    if (ctx == NULL) { return; }
    tableData = pokemonGetStatus(ctx, 0, 0x122, 0);
    entry = fightOutPokemonEnemySearchAry(tableData, 4, param);
    if (entry == NULL) { return; }
    species = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
    fightOutPokemonEnemyInit(entry);
    if (species == 0 || species == 0x165 || species == 0xFFFF) { return; }
    if ((s32)(u32)pokemonGetStatus(ctx, 0, 0xF7, 0) != 0) { return; }
    if (ctx != NULL) {
        entryPtr = pokemonGetStatus(ctx, 0, 0x122, 0);
        for (i = 0; i < 4; i++) {}
        count = 0;
        for (i = 0; i < 4; i++) {
            entry = (void*)((u32)entryPtr + i * 0xC);
            if ((u8)fightOutPokemonEnemyCheckValid(entry) == 4) { continue; }
            species = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
            if (species == 4 || species == 0x165) { continue; }
            count++;
        }
    } else {
        count = 0;
    }
    if (count == 4) {
        pokemonSetStatus(ctx, 0, 0xF7, 0, (u32)species);
    }
}

/* 0x802032E4 | size: 0x138 */
#pragma push
#pragma scheduling on
static inline void* fn_802032E4_getCC(void* ctx) {
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetFriendFormPokemonFriendFilterId(void* ctx, u32 param) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void pokemonGetFriendFormPokemonFriendFilterId();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* resolvedPtr;
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_802032E4_getCC(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_802032E4_getCC(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { resolvedPtr = pokemonGetStatus(ctx, 0, 0xCC, 0); tmp = resolvedPtr; }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetFriendFormPokemonFriendFilterId(ccData, value, param);
}
#pragma pop

/* 0x8020341C | size: 0x140 */
#pragma push
#pragma scheduling off
static inline void* fn_8020341C_resolveCcData(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

static inline void* fn_8020341C_resolveCcCtx(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetEffortFromPokemon(void* ctx, u32 param1, u32 param2) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void pokemonGetEffortFromPokemon();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_8020341C_resolveCcData(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_8020341C_resolveCcCtx(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { tmp = pokemonGetStatus(ctx, 0, 0xCC, 0); }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetEffortFromPokemon(ccData, value, param1, param2);
}
#pragma pop

/* 0x8020355C | size: 0x60 */
u32 fightPokemonGetLevelToExp(u32 obj, u32 param) {
    extern u32 pokemonGetLevelToExp();
    extern u32 pokemonGetStatus();
    u32 result;
    if (obj == 0) {
        result = 0;
    } else {
        result = pokemonGetStatus(obj, 0, 0xCC, 0);
    }
    if (result == 0) {
        return 0;
    }
    return pokemonGetLevelToExp(result, param);
}

/* 0x802035BC | size: 0x64 */
void figthPokemonSetExp(void* obj, u32 value) {
    void* intermediate;
    if (obj == NULL) {
        intermediate = NULL;
    } else {
        intermediate = pokemonGetStatus(obj, 0, 0xCC, 0);
    }
    if (intermediate != NULL) {
        pokemonSetStatus(intermediate, 0, 0x79, 0, value);
    }
}

/* Forward declarations for converted functions */

/* =========================================================================
 * figthPokemonGetExp
 *
 * Navigate from a trainer context through two data table hops to reach
 * extended Pokemon/trainer data. Same pokemonGetStatus(..., 0xCC/0x79, ...)
 * hop pattern as fightPokemonGetLevelToExp/figthPokemonSetExp above.
 *
 * Hop 1: pokemonGetStatus(ctx, 0, 0xCC, 0) -> intermediate pointer
 * Hop 2: pokemonGetStatus(intermediate, 0, 0x79, 0) -> extended data
 *
 * If either hop returns NULL, the function returns NULL.
 *
 * @param context  Trainer/party context
 * @return         Extended data pointer, or NULL
 * ========================================================================= */
void* figthPokemonGetExp(void* context) {
    void* intermediate;
    if (context == NULL) {
        intermediate = NULL;
    } else {
        intermediate = pokemonGetStatus(context, 0, 0xCC, 0);
    }
    if (intermediate == NULL) {
        return NULL;
    }

    return pokemonGetStatus(intermediate, 0, 0x79, 0);
}

/* =========================================================================
 * fightPokemonGrowBasisStatus
 *
 * Similar two-hop navigation, but the second call writes data via
 * pokemonGrowBasisStatus instead of reading it.
 *
 * @param context  Trainer/party context
 * @param value    Value to write
 * ========================================================================= */
void fightPokemonGrowBasisStatus(void* context, u32 value) {
    void* intermediate;
    if (context == NULL) {
        intermediate = NULL;
    } else {
        intermediate = pokemonGetStatus(context, 0, 0xCC, 0);
    }
    if (intermediate != NULL) {
        pokemonGrowBasisStatus(intermediate, value);
    }
}

/* 0x802036D4 | size: 0x84 */
u32 fightOutPokemonGetVoiceSndId(void* ctx) {
    void* resolved;
    u16 species;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    species = (u16)(u32)pokemonGetStatus(resolved, 0, 0x6E, 0);
    return (u16)(u32)pokemonGetStatus(NULL, species, 0x61, 0);
}

/* 0x80203758 | size: 0x84 */
u32 fightOutPokemonGetNamePtr(void* ctx) {
    extern u32 GSmsgGetGSchar();
    void* resolved;
    u16 species;
    resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    species = (u16)(u32)pokemonGetStatus(resolved, 0, 0x6E, 0);
    resolved = pokemonGetStatus(NULL, species, 0x01, 0);
    return GSmsgGetGSchar(resolved);
}

/* 0x802037DC | size: 0x6C */
void* fightOutPokemonGetNicknamePtr(void* ctx) {
    void* resolved;
    resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return NULL;
    }
    return pokemonGetStatus(resolved, 0, 0x77, 0);
}

/* 0x80203848 | size: 0x5C | small */
u32 fightPokemonGetNicknamePtr(void* param_1) {
    void* iVar1;
    u32 uVar2;

    if (param_1 == NULL) {
        iVar1 = NULL;
    } else {
        iVar1 = pokemonGetStatus(param_1, 0, 0xCC, 0);
    }
    if (iVar1 == NULL) {
        uVar2 = 0;
    } else {
        uVar2 = (u32)pokemonGetStatus(iVar1, 0, 0x77, 0);
    }
    return uVar2;
}

/* Address: 0x802038A4 | Size: 0x1c8 | Ghidra import */

u8 fightOutPokemonIsHinsi(int r3)

{
    extern s8 pokemonCheckValid();
    extern short fn_801EF634();
  u8 bVar1;
  u16 sVar4;
  u32 iVar2;
  u8 cVar5;
  u32 iVar3;
  u8 uVar6;
  
  if (r3 == 0) {
    bVar1 = 0;
  }
  else {
    sVar4 = fn_801EF634();
    if (sVar4 == 1) {
      bVar1 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        bVar1 = 0;
      }
      else {
        if (iVar3 == 0) {
          bVar1 = 0;
        }
        else {
        sVar4 = fn_801EF634();
        if (sVar4 == 1) {
          bVar1 = 0;
        }
        else {
          iVar2 = (int)pokemonGetStatus(iVar3,0,0xcb,0);
          if (iVar2 == 0) {
            bVar1 = 0;
          }
          else {
            cVar5 = pokemonCheckValid();
            if (cVar5 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
              }
              if (iVar2 == 0) {
                bVar1 = 0;
              }
              else {
                cVar5 = pokemonCheckValid();
                if (cVar5 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar3 = (int)pokemonGetStatus(iVar3,0,0xce,0);
                  if ((int)iVar3 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        }
        if (bVar1 == 0) {
          bVar1 = 0;
        }
        else {
          bVar1 = 1;
        }
      }
    }
  }
  if (bVar1 == 0) {
    uVar6 = 1;
  }
  else {
    if ((u32)r3 == 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    if (iVar3 == 0) {
      uVar6 = 1;
    }
    else {
      uVar6 = (int)pokemonGetStatus(iVar3,0,0x7b,0);
    }
  }
  return uVar6;
}

/* 0x80203A6C | size: 0x70 */
u32 fightOutPokemonGetNowHpPercentage(void* ctx) {
    extern u32 pokemonGetNowHpPercentage();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonGetNowHpPercentage(resolved);
}

/* 0x80203ADC | size: 0x80 */
u32 fightOutPokemonNowHpWaruValue(void* ctx, u32 param) {
    extern u32 pokemonGetNowHpWaruValue();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonGetNowHpWaruValue(resolved, param);
}

/* 0x80203B5C | size: 0x80 */
u32 fightOutPokemonMaxHpWaruValue(void* ctx, u32 param) {
    extern u32 pokemonGetMaxHpWaruValue();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonGetMaxHpWaruValue(resolved, param);
}

/* 0x80203BDC | size: 0x80 */
u32 fightOutPokemonIsNokoriHpFollowing(void* ctx, u32 param) {
    extern u32 pokemonIsNokoriHpFollowing();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonIsNokoriHpFollowing(resolved, param);
}

/* 0x80203C5C | size: 0x70 */
u32 fightOutPokemonIsJoutaiKaragenki(void* ctx) {
    extern u32 pokemonIsJoutaiKaragenki();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonIsJoutaiKaragenki(resolved);
}

/* 0x80203CCC | size: 0x70 */
u32 fightOutPokemonIsJoutaiNormal(void* ctx) {
    extern u32 pokemonIsJoutaiNormal();
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }
    if (resolved == NULL) {
        return 0;
    }
    return pokemonIsJoutaiNormal(resolved);
}

/* 0x80203D3C | size: 0x70 */
u16 figthOutPokemonGetPokemonDataId(void* ctx) {
    void* resolved;
    resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u16)(u32)pokemonGetStatus(resolved, 0, 0x6E, 0);
}

/* 0x80203DAC | size: 0x60 */
u16 figthPokemonGetPokemonDataId(void* ctx) {
    void* resolved;
    if (ctx == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u16)(u32)pokemonGetStatus(resolved, 0, 0x6E, 0);
}

/* 0x80203E0C | size: 0x70 */
u8 figthOutPokemonGetLevel(void* ctx) {
    void* resolved;
    resolved = pokemonGetStatus(ctx, 0, 0xD6, 0);
    if (resolved == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
    }
    if (resolved == NULL) {
        return 0;
    }
    return (u8)(u32)pokemonGetStatus(resolved, 0, 0x7A, 0);
}

/* 0x80203E7C | size: 0x60 */
u32 figthPokemonGetLevel(u32 obj) {
    extern u32 pokemonGetStatus();
    u32 result;
    if (obj == 0) {
        result = 0;
    } else {
        result = pokemonGetStatus(obj, 0, 0xCC, 0);
    }
    if (result == 0) {
        return 0;
    }
    return pokemonGetStatus(result, 0, 0x7A, 0) & 0xFF;
}

/* 0x80203EDC | size: 0x108 */
u16 figthOutPokemonGetSoubiItemBuff(void* ctx) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern u16 pokemonGetSoubiItemBuff();
    void* d6Data;
    void* ccData;
    u16 typeId;
    u8 result;

    d6Data = pokemonGetStatus(ctx, 0, 0xD6, 0);
    ccData = !d6Data ? NULL : pokemonGetStatus(d6Data, 0, 0xCC, 0);
    if (ccData == NULL) { return 0; }
    typeId = fn_80119ED0(0x3D);
    if (typeId == 0x7C || typeId == 0xC8) {
        result = fn_80121ADC(!d6Data ? NULL : pokemonGetStatus(d6Data, 0, 0xCC, 0), 0x3D);
    } else if (fn_80119ED0(0x3D) == 0xCD) {
        result = fn_8011B67C(d6Data, 0x3D);
    } else {
        result = 0;
    }
    if (result == 1) { return 0; }
    return pokemonGetSoubiItemBuff(ccData);
}

/* 0x80203FE4 | size: 0x104 */
u32 fightOutPokemonGetSoubiItemSoubiDataId(void* ctx) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* d6Data;
    void* ccData;
    u16 typeId;
    u8 result;

    d6Data = pokemonGetStatus(ctx, 0, 0xD6, 0);
    ccData = !d6Data ? NULL : pokemonGetStatus(d6Data, 0, 0xCC, 0);
    if (ccData == NULL) { return 0; }
    typeId = fn_80119ED0(0x3D);
    if (typeId == 0x7C || typeId == 0xC8) {
        result = fn_80121ADC(!d6Data ? NULL : pokemonGetStatus(d6Data, 0, 0xCC, 0), 0x3D);
    } else if (fn_80119ED0(0x3D) == 0xCD) {
        result = fn_8011B67C(d6Data, 0x3D);
    } else {
        result = 0;
    }
    if (result == 1) { return 0; }
    return pokemonGetSoubiItemSoubiDataId(ccData);
}

#pragma peephole off
void* fightOutPokemonGetSoubiItemDataId(void) {
    extern void* pokemonGetStatus();
    extern u32 fn_80119ED0();
    extern u32 fn_80121ADC();
    extern void* fn_8011B67C();
    extern void* pokemonGetSoubiItemDataId();
    void* alloc2;
    void* alloc1;
    u32 r0;

    if ((alloc1 = pokemonGetStatus(0, 0, 0xD6, 0)) != 0) {
        alloc2 = pokemonGetStatus(0, 0, 0xCC, 0);
    } else {
        alloc2 = 0;
    }
    if (alloc2 == 0) {
        return 0;
    }
    if ((u16)(u32)fn_80119ED0(0x3D) == 0x7C ||
        (u16)(u32)fn_80119ED0(0x3D) == 0xC8) {
        r0 = fn_80121ADC(alloc1 ? pokemonGetStatus(alloc1, 0, 0xCC, 0) : 0, 0x3D);
    } else {
        r0 = fn_80119ED0(0x3D);
        if ((u16)r0 != 0xCD) {
            return 0;
        }
        r0 = (u32)fn_8011B67C(alloc1, 0x3D);
    }
    if ((u8)r0 == 1) {
        return 0;
    }
    return pokemonGetSoubiItemDataId(alloc2);
}

/* 0x802041EC | size: 0xF4 | medium */
#pragma push
#pragma peephole on
u32 fightPokemonGetSoubiItemSoubiDataId(void* param_1) {
    extern u16 fn_80119ED0(u32);
    extern u8 fn_8011B67C(void*, u32);
    extern u8 fn_80121ADC(void*, u32);
    extern u32 pokemonGetSoubiItemSoubiDataId(void*);
    u32 value;
    void* item;
    u8 result;

    item = param_1 == NULL ? NULL : pokemonGetStatus(param_1, 0, 0xCC, 0);
    if (item == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            result = fn_80121ADC(param_1 == NULL ? NULL : pokemonGetStatus(param_1, 0, 0xCC, 0), 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(param_1, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(item);
        }
    }
    return value;
}
#pragma pop

/* 0x802042E0 | size: 0xF4 | medium */
#pragma push
#pragma peephole on
u32 fightPokemonGetSoubiItemDataId(void* param_1) {
    extern u16 fn_80119ED0(u32);
    extern u8 fn_8011B67C(void*, u32);
    extern u8 fn_80121ADC(void*, u32);
    extern u32 pokemonGetSoubiItemDataId(void*);
    u32 value;
    void* item;
    u8 result;

    item = NULL;
    if (param_1 != NULL) {
        item = pokemonGetStatus(param_1, 0, 0xCC, 0);
    }
    if (item == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            result = fn_80121ADC(param_1 == NULL ? NULL : pokemonGetStatus(param_1, 0, 0xCC, 0), 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(param_1, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemDataId(item);
        }
    }
    return value;
}
#pragma pop

/* Address: 0x802043D4 | Size: 0x480 | Ghidra import */
u32 fightOutPokemonGetNowNimbleness(void)

{
    int r3;
    char r4;
    char r5;
    u32 r6;
    int r7;

    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern s8 fn_80121ADC();
    extern u32 pokemonGetSoubiItemBuff();
    extern short pokemonGetSoubiItemSoubiDataId();
    extern s8 heroGetStatus();
    extern int fightAbicntDoKakeWaru();
  u32 uVar1;
  short sVar4;
  int iVar2;
  short sVar5;
  short sVar6;
  u8 uVar7;
  s8 cVar8;
  u32 uVar3;
  int iVar9;
  u32 uVar10;
  int iVar11;
  
  if (r3 == 0) {
    iVar11 = 0;
  }
  else {
    iVar11 = (int)pokemonGetStatus(r3,0,0xd6,0);
    if (iVar11 == 0) {
      iVar11 = 0;
    }
    else {
      iVar11 = (int)pokemonGetStatus(iVar11,0,0xcc,0);
    }
  }
  if (iVar11 == 0) {
    uVar1 = 0;
  }
  else {
    sVar4 = (int)pokemonGetStatus(r3,0,0x100,0);
    iVar2 = (int)pokemonGetStatus(r3,0,0xd6,0);
    if (iVar2 == 0) {
      iVar9 = 0;
    }
    else {
      iVar9 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
    }
    if (iVar9 == 0) {
      sVar5 = 0;
    }
    else {
      sVar5 = fn_80119ED0(0x3d);
      if ((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x3d), sVar5 == 200)) {
        if (iVar2 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,0x3d);
      }
      else {
        sVar5 = fn_80119ED0(0x3d);
        if (sVar5 == 0xcd) {
          cVar8 = fn_8011B67C(iVar2,0x3d);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 == 1) {
        sVar5 = 0;
      }
      else {
        sVar5 = pokemonGetSoubiItemSoubiDataId(iVar9);
      }
    }
    iVar2 = (int)pokemonGetStatus(r3,0,0xd6,0);
    if (iVar2 == 0) {
      iVar9 = 0;
    }
    else {
      iVar9 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
    }
    if (iVar9 == 0) {
      uVar10 = 0;
    }
    else {
      sVar6 = fn_80119ED0(0x3d);
      if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x3d), sVar6 == 200)) {
        if (iVar2 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,0x3d);
      }
      else {
        sVar6 = fn_80119ED0(0x3d);
        if (sVar6 == 0xcd) {
          cVar8 = fn_8011B67C(iVar2,0x3d);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 == 1) {
        uVar10 = 0;
      }
      else {
        uVar10 = pokemonGetSoubiItemBuff(iVar9);
        uVar10 = uVar10 & 0xffff;
      }
    }
    uVar7 = (int)pokemonGetStatus(r3,0,0xea,0);
    if (r7 == 0) {
      cVar8 = 0;
    }
    else {
      cVar8 = heroGetStatus(r7,0x11,0);
    }
    uVar1 = (int)pokemonGetStatus(iVar11,0,0x8c,0);
    uVar1 = uVar1 & 0xffff;
    if ((sVar4 == 0x21) && (r5 == 2)) {
      uVar1 = uVar1 << 1;
    }
    else if ((sVar4 == 0x22) && (r5 == 1)) {
      uVar1 = uVar1 << 1;
    }
    uVar1 = fightAbicntDoKakeWaru(uVar7,uVar1);
    if ((r4 == 1) && (cVar8 == 1)) {
      uVar1 = (uVar1 * 0x6e) / 100;
    }
    if (sVar5 == 0x18) {
      uVar1 = uVar1 >> 1;
    }
    sVar4 = fn_80119ED0(5);
    if (((sVar4 == 0x7c) || (sVar4 = fn_80119ED0(5), sVar4 == 200)) ||
       (sVar4 = fn_80119ED0(5), sVar4 == 0xcd)) {
      iVar11 = (int)pokemonGetStatus(r3,0,0xd6,0);
      sVar4 = fn_80119ED0(5);
      if ((sVar4 == 0x7c) || (sVar4 = fn_80119ED0(5), sVar4 == 200)) {
        if (iVar11 == 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = (int)pokemonGetStatus(iVar11,0,0xcc,0);
        }
        cVar8 = fn_80121ADC(uVar3,5);
      }
      else {
        sVar4 = fn_80119ED0(5);
        if (sVar4 == 0xcd) {
          cVar8 = fn_8011B67C(iVar11,5);
        }
        else {
          cVar8 = 0;
        }
      }
    }
    else {
      sVar4 = fn_80119ED0(5);
      if (sVar4 == 0xd8) {
        cVar8 = fn_8011B67C(r3,5);
      }
      else {
        cVar8 = 0;
      }
    }
    if (cVar8 == 1) {
      uVar1 = uVar1 >> 2;
    }
    if ((sVar5 == 0x1a) && ((int)(r6 & 0xffff) < (int)(uVar10 * 0xffff) / 100)) {
      uVar1 = 0xffffffff;
    }
  }
  return uVar1;
}

/* 0x80204854 | size: 0xD4 | medium */
#pragma push
#pragma peephole on
#pragma scheduling on
u32 fightOutPokemonCheckIrekaeReserveFightPokemon(void* param_1, void* param_2) {
    extern u8 fightActionCheckValid(void*);
    extern s16 fightActionBiosGetBuffDataId(void*);
    extern u16 fightActionBiosGetKind(void*);
    void* iVar2;
    s16 sVar3;
    u32 uVar1;
    s16 sVar4;
    u8 cVar5;

    sVar3 = (s16)(u32)pokemonGetStatus(param_2, 0, 0xCE, 0);
    if (sVar3 < 0) {
        uVar1 = 0;
    } else {
        sVar4 = (s16)(u32)pokemonGetStatus(param_1, 0, 0x121, 0);
        if (sVar3 == sVar4) {
            uVar1 = 1;
        } else {
            iVar2 = pokemonGetStatus(param_1, 0, 0xFE, 0);
            if ((((iVar2 != NULL) && (cVar5 = fightActionCheckValid(iVar2), cVar5 == 1)) &&
                ((u16)fightActionBiosGetKind(iVar2) == 9)) &&
               (sVar4 = fightActionBiosGetBuffDataId(iVar2), sVar3 == sVar4)) {
                uVar1 = 1;
            } else {
                uVar1 = 0;
            }
        }
    }
    return uVar1;
}
#pragma pop

/* 0x80204928 | size: 0x48 | small */
#pragma push
#pragma peephole on
u8 fightPokemonCheckMotoFightPokemon(u32 expected, void* ctx) {
    u32 result = (u32)pokemonGetStatus(ctx, 0, 0xd5, 0);
    return (result == expected) ? 1 : 0;
}
#pragma pop

/* 0x80204970 | size: 0xA0 */
void fn_80204970(void* first, void* second)
{
    typedef struct FightPokemonCopy {
        u8 data[0x154];
    } FightPokemonCopy;
    FightPokemonCopy* firstPokemon = first;
    FightPokemonCopy* secondPokemon = second;

    if (firstPokemon != NULL) {
        FightPokemonCopy temporary;
        if (secondPokemon != NULL) {
            temporary = *firstPokemon;
            *firstPokemon = *secondPokemon;
            *secondPokemon = temporary;
        }
    }
}

/* fightOutPokemonIsGcHeroFightOutPokemon | Size: 0x4C | Check if trainer slot is active */
#pragma push
#pragma peephole on
u8 fightOutPokemonIsGcHeroFightOutPokemon(u32 slotId) {
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 context, u32 slot);
    extern u8 fightTrainerIsGcHero(void* trainer);
    void* trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, slotId);
    if (trainer == NULL) {
        return 0;
    }
    return fightTrainerIsGcHero(trainer) == 1;
}
#pragma pop

/* 0x80204A5C | size: 0x1AC | medium */
/* 0x80204A5C | size: 0x1AC */
#pragma push
#pragma peephole on
u32 fightOutPokemonIsFightActionUseItemKind(void* ctx, u8 targetSlot, u8 mode) {
    extern u32 lbl_80478BD8;
    extern u8 fn_80142984();
    extern u16 fightActionGetKindDataId();
    extern u8 fightActionCheckValid();
    extern void fightFloorGetStatus();
    u16 field1E;
    u16 field1F;
    int e5Data;
    int feData;
    u8 valid;
    u32 i;

    for (i = 0; (u16)i < lbl_80478BD8; i++) {
        if ((u8)fn_80142984(i) == 0) { continue; }
        if (mode == 1) {
            if (targetSlot != (u8)itemGetStatus(0, i, 0x2, 0)) { continue; }
        } else {
            if (targetSlot == (u8)itemGetStatus(0, i, 0x2, 0)) { continue; }
        }
        fightFloorGetStatus(0, 0, 0x14, 0);
        if (ctx == NULL) { valid = 0; }
        else {
            feData = (int)pokemonGetStatus(ctx, 0, 0xFE, 0);
            if (feData == 0) { valid = 0; }
            else if ((u8)fightActionCheckValid(feData) == 0) { valid = 0; }
            else if (fightActionGetKindDataId(feData) != 0x12) { valid = 0; }
            else {
                e5Data = (int)pokemonGetStatus(ctx, 0, 0xE5, 0);
                if (e5Data == 0) { valid = 0; }
                else {
                    field1E = (u16)itemGetStatus(e5Data, 0, 0x1E, 0);
                    field1F = (u16)itemGetStatus(e5Data, 0, 0x1F, 0);
                    if ((u16)i != 0 && field1E != (u16)i) {
                        valid = 0;
                    } else {
                        valid = 1;
                    }
                }
            }
        }
        if (valid == 1) { return 1; }
    }
    return 0;
}
#pragma pop

/* Address: 0x80204C08 | Size: 0xd8 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
u16 fightOutPokemonGetFightActionUseItemDataId(void* r3)

{
    extern u16 fightActionGetKindDataId();
    extern u8 fightActionCheckValid();
    extern void fightFloorGetStatus();
  u32 iVar1;
  u8 cVar4;
  u16 sVar2;
  u16 uVar3;

  fightFloorGetStatus(0,0,0x14,0);
  if (r3 == 0) {
    uVar3 = 0;
  }
  else {
    iVar1 = (u32)pokemonGetStatus(r3,0,0xfe,0);
    if (iVar1 == 0) {
      uVar3 = 0;
    }
    else {
      cVar4 = fightActionCheckValid();
      if (cVar4 == 0) {
        uVar3 = 0;
      }
      else {
        sVar2 = fightActionGetKindDataId(iVar1);
        if (sVar2 != 0x12) {
          uVar3 = 0;
        }
        else {
          iVar1 = (u32)pokemonGetStatus(r3,0,0xe5,0);
          if (iVar1 == 0) {
            uVar3 = 0;
          }
          else {
            uVar3 = itemGetStatus(iVar1,0,0x1e,0);
          }
        }
      }
    }
  }
  return uVar3;
}
#pragma pop

/* 0x80204CE0 | size: 0x104 */
void* fightOutPokemonCreateFightActionUseItem(void* ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, u8 p9) {
    extern void fn_80142B24();
    extern u8 fightActionCreate();
    extern void fightItemCreate();
    extern void fightActionBiosSetBuffDataId();
    void* e5Data;
    void* feData;

    e5Data = pokemonGetStatus(ctx, 0, 0xE5, 0);
    if (e5Data == NULL) { return NULL; }
    fightItemCreate(e5Data, (u16)p6, p7, p8);
    fn_80142B24(e5Data, 0, 0x21, 0, (u32)p9);
    feData = pokemonGetStatus(ctx, 0, 0xFE, 0);
    if (feData == NULL) { feData = NULL; }
    else {
        if ((u8)fightActionCreate(feData, p2, ctx, p3, p4, p5) == 1) {
            fightActionBiosSetBuffDataId(feData, p6);
        } else {
            feData = NULL;
        }
    }
    return feData;
}

/* 0x80204DE4 | size: 0x188 */
#pragma push
#pragma peephole on
u32 fightOutPokemonIsFightActionAttackWazaOut(void* ctx, u16 slotId, void* tablePtr) {
    extern u16 wazaGetStatus();
    extern void* fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern void* fightTargetGetPtrAsNowFightType();
    extern u16 fightActionGetKindDataId();
    extern u8 fightActionCheckValid();
    extern u16 fightFloorGetStatus();
    void* feData;
    void* d9Data;
    u16 partyCount;
    void* savedEntry;
    u16 field27;
    u16 field09;
    u32 field29;

    partyCount = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    if (ctx == NULL) { return 0; }
    savedEntry = !tablePtr ? NULL : fightTargetGetTragetPtrToRelativeHostSideFightTargetId(tablePtr, partyCount);
    feData = pokemonGetStatus(ctx, 0, 0xFE, 0);
    if (feData == NULL) { return 0; }
    if ((u8)fightActionCheckValid(feData) == 0) { return 0; }
    if (fightActionGetKindDataId(feData) != 0x13) { return 0; }
    d9Data = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (d9Data == NULL) { return 0; }
    field27 = (u16)wazaGetStatus(d9Data, 0, 0x27, 0);
    field09 = (u16)wazaGetStatus(0, field27, 0x9, 0);
    if (slotId != 0 && field27 != slotId) { return 0; }
    field29 = (u32)wazaGetStatus(d9Data, 0, 0x29, 0);
    if (field09 == 0xB0) {
        field29 = (u32)fightTargetGetTragetPtrToRelativeHostSideFightTargetId(fightTargetGetPtrAsNowFightType(0xE, ctx), partyCount);
    }
    if ((u16)(u32)savedEntry != 0 && (u16)field29 != (u16)(u32)savedEntry) { return 0; }
    return 1;
}
#pragma pop

/* 0x80204F6C | size: 0xF0 */
void* fightOutPokemonCreateFightActionAttackWaza(void* ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, u8 p9) {
    extern u8 fightActionCreate();
    extern void fightWazaCreate();
    extern void fightActionBiosSetBuffDataId();
    u16 zokusei = (u16)p6;
    void* result;
    u8 created;

    result = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (result == NULL) {
        result = NULL;
    } else {
        fightWazaCreate(result, p8, zokusei, p7, p9);
        result = pokemonGetStatus(ctx, 0, 0xFE, 0);
        if (result == NULL) {
            result = NULL;
        } else {
            created = fightActionCreate(result, p2, ctx, p3, p4, p5);
            if (created == 1) {
                fightActionBiosSetBuffDataId(result, p6);
            } else {
                result = NULL;
            }
        }
        if (result == NULL) {
            result = NULL;
        }
    }
    return result;
}

/* Address: 0x8020505C | Size: 0x98 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
int fightOutPokemonCreateFightAction(void* r3, u32 r4, u32 r5, u32 r6, u32 r7, u32 r8)

{
    extern u8 fightActionCreate();
    extern void fightActionBiosSetBuffDataId();
  int iVar1;
  u8 cVar2;

  iVar1 = (int)pokemonGetStatus(r3,0,0xfe,0);
  if (iVar1 == 0) {
    return 0;
  }
  cVar2 = fightActionCreate(iVar1,r4,r3,r5,r6,r7);
  if (cVar2 == 1) {
    fightActionBiosSetBuffDataId(iVar1,r8);
    return iVar1;
  }
  return 0;
}
#pragma pop

#pragma push
#pragma peephole on
#if 0
asm void fightOutPokemonGetFightActionPri(void) {
#include "src/game/colosseum_event_fn_802050F4.inc"
}
#else
void* fightOutPokemonGetFightActionPri(void* ctx) {
    void* p;
    p = pokemonGetStatus(ctx, 0, 0xFE, 0);
    if (p == NULL) {
        p = (void*)-0x80;
    } else {
        p = fightActionGetPri(p);
    }
    return p;
}
#endif
#pragma pop

/* fightOutPokemonGetWazaZokuseiDataId | Size: 0x50 | Get field 0x30 from resolved 0xD9, default 9 */
#pragma push
#pragma peephole on
u16 fightOutPokemonGetWazaZokuseiDataId(void* ctx) {
    extern u32 wazaGetStatus();
    void* resolved = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 9;
    }
    return (u16)wazaGetStatus(resolved, 0, 0x30, 0);
}

u16 fightOutPokemonGetUseWazaDataId(void* ctx) {
    extern void* pokemonGetStatus();
    extern u32 wazaGetStatus();
    void* resolved;
    resolved = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (resolved == 0) {
        return 0;
    }
    return (u16)wazaGetStatus(resolved, 0, 0x28, 0);
}

/* fightOutPokemonGetCmpNimblenessWazaDataId | Size: 0x50 | Get field 0x27 from resolved 0xD9, default 0 */
u16 fightOutPokemonGetCmpNimblenessWazaDataId(void* ctx) {
    extern u32 wazaGetStatus();
    void* resolved = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 0;
    }
    return (u16)wazaGetStatus(resolved, 0, 0x27, 0);
}

/* fightOutPokemonGetMotoWazaDataId | Size: 0x50 | Get field 0x27 from resolved 0xD9, default 0 */
u16 fightOutPokemonGetMotoWazaDataId(void* ctx) {
    extern u32 wazaGetStatus();
    void* resolved = pokemonGetStatus(ctx, 0, 0xD9, 0);
    if (resolved == NULL) {
        return 0;
    }
    return (u16)wazaGetStatus(resolved, 0, 0x27, 0);
}
#pragma pop

/* Address: 0x80205274 | Size: 0x690 | Ghidra import */

void fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll(int r3,u32 r4)

{
    extern s8 pokemonCheckFightOut();
    extern s8 pokemonCheckValid();
    extern short fn_801EF634();
    extern u32 fightFloorGetStatus();
    extern u32 fightSideGetStatus();
    extern s8 fightTrainerCheckValid();
    extern int fightTrainerGetStatus();
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar10;
  int iVar5;
  int iVar6;
  int iVar7;
  short sVar8;
  short sVar9;
  u8 bVar11;
  u32 uVar12;
  u32 uVar13;
  
  fightFloorGetStatus(0,0,0x14,0);
  uVar2 = fightFloorGetStatus(0,0,0x16,0);
  uVar3 = fightFloorGetStatus(0,0,0x18,0);
  uVar12 = 0;
  do {
    if ((uVar2 & 0xffff) <= (uVar12 & 0xffff)) {
      return;
    }
    uVar4 = fightSideGetStatus(r4,0,7,uVar12);
    cVar10 = fightTrainerCheckValid();
    if (cVar10 != 0) {
      for (uVar13 = 0; (uVar13 & 0xffff) < (uVar3 & 0xffff); uVar13 = uVar13 + 1) {
        iVar5 = fightTrainerGetStatus(uVar4,0,0x46,uVar13);
        if (iVar5 == 0) {
          bVar1 = 0;
        }
        else {
          sVar8 = fn_801EF634();
          if (sVar8 == 1) {
            bVar1 = 0;
          }
          else {
            iVar7 = (int)pokemonGetStatus(iVar5,0,0xd6,0);
            if (iVar7 == 0) {
              bVar1 = 0;
            }
            else {
              sVar8 = fn_801EF634();
              if (sVar8 == 1) {
                bVar1 = 0;
              }
              else {
                iVar6 = (int)pokemonGetStatus(iVar7,0,0xcb,0);
                if (iVar6 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar10 = pokemonCheckValid();
                  if (cVar10 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar7 == 0) {
                      iVar6 = 0;
                    }
                    else {
                      iVar6 = (int)pokemonGetStatus(iVar7,0,0xcc,0);
                    }
                    if (iVar6 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar10 = pokemonCheckValid();
                      if (cVar10 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        iVar7 = (int)pokemonGetStatus(iVar7,0,0xce,0);
                        if (iVar7 < 0) {
                          bVar1 = 0;
                        }
                        else {
                          bVar1 = 1;
                        }
                      }
                    }
                  }
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          if (bVar1) {
            iVar7 = (int)pokemonGetStatus(iVar5,0,0x120,0);
            if (iVar7 == 1) {
              bVar1 = 0;
            }
            else {
              iVar7 = (int)pokemonGetStatus(iVar5,0,0xd6,0);
              if (iVar7 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar6 = (int)pokemonGetStatus(iVar7,0,0xcb,0);
                  if (iVar6 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = pokemonCheckValid();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar7 == 0) {
                        iVar6 = 0;
                      }
                      else {
                        iVar6 = (int)pokemonGetStatus(iVar7,0,0xcc,0);
                      }
                      if (iVar6 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar10 = pokemonCheckValid();
                        if (cVar10 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar6 = (int)pokemonGetStatus(iVar7,0,0xce,0);
                          if (iVar6 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
                if (bVar1) {
                  iVar6 = (int)pokemonGetStatus(iVar7,0,0xd2,0);
                  if (iVar6 == 1) {
                    bVar1 = 0;
                  }
                  else {
                    if (iVar7 == 0) {
                      iVar7 = 0;
                    }
                    else {
                      iVar7 = (int)pokemonGetStatus(iVar7,0,0xcc,0);
                    }
                    if (iVar7 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      cVar10 = pokemonCheckFightOut();
                      if (cVar10 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
                else {
                  bVar1 = 0;
                }
              }
              if (bVar1) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
            }
          }
          else {
            bVar1 = 0;
          }
        }
        if ((bVar1) && (iVar5 = (int)pokemonGetStatus(iVar5,0,0xd5,0), r3 != 0)) {
          if (iVar5 == 0) {
            bVar1 = 0;
          }
          else {
            sVar8 = fn_801EF634();
            if (sVar8 == 1) {
              bVar1 = 0;
            }
            else {
              iVar7 = (int)pokemonGetStatus(iVar5,0,0xcb,0);
              if (iVar7 == 0) {
                bVar1 = 0;
              }
              else {
                cVar10 = pokemonCheckValid();
                if (cVar10 == 0) {
                  bVar1 = 0;
                }
                else {
                  if (iVar5 == 0) {
                    iVar7 = 0;
                  }
                  else {
                    iVar7 = (int)pokemonGetStatus(iVar5,0,0xcc,0);
                  }
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = pokemonCheckValid();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      iVar7 = (int)pokemonGetStatus(iVar5,0,0xce,0);
                      if (iVar7 < 0) {
                        bVar1 = 0;
                      }
                      else {
                        bVar1 = 1;
                      }
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            if (r3 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar5 == 0) {
                bVar1 = 0;
              }
              else {
                sVar8 = fn_801EF634();
                if (sVar8 == 1) {
                  bVar1 = 0;
                }
                else {
                  iVar7 = (int)pokemonGetStatus(iVar5,0,0xcb,0);
                  if (iVar7 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    cVar10 = pokemonCheckValid();
                    if (cVar10 == 0) {
                      bVar1 = 0;
                    }
                    else {
                      if (iVar5 == 0) {
                        iVar7 = 0;
                      }
                      else {
                        iVar7 = (int)pokemonGetStatus(iVar5,0,0xcc,0);
                      }
                      if (iVar7 == 0) {
                        bVar1 = 0;
                      }
                      else {
                        cVar10 = pokemonCheckValid();
                        if (cVar10 == 0) {
                          bVar1 = 0;
                        }
                        else {
                          iVar7 = (int)pokemonGetStatus(iVar5,0,0xce,0);
                          if (iVar7 < 0) {
                            bVar1 = 0;
                          }
                          else {
                            bVar1 = 1;
                          }
                        }
                      }
                    }
                  }
                }
              }
              if (bVar1) {
                sVar8 = (int)pokemonGetStatus(iVar5,0,0xce,0);
                for (bVar11 = 0; bVar11 < 0xc; bVar11 = bVar11 + 1) {
                  sVar9 = (int)pokemonGetStatus(r3,0,0xfd,bVar11);
                  if ((-1 < sVar9) && (sVar9 == sVar8)) {
                    bVar1 = 1;
                    goto LAB_00202858;
                  }
                }
                bVar1 = 0;
              }
              else {
                bVar1 = 0;
              }
            }
LAB_00202858:
            if (bVar1 == 0) {
              sVar8 = (int)pokemonGetStatus(iVar5,0,0xce,0);
              for (bVar11 = 0; bVar11 < 0xc; bVar11 = bVar11 + 1) {
                sVar9 = (int)pokemonGetStatus(r3,0,0xfd,bVar11);
                if (sVar9 < 0) {
                  pokemonSetStatus(r3,0,0xfd,bVar11,(int)sVar8);
                  break;
                }
              }
            }
          }
        }
      }
    }
    uVar12 = uVar12 + 1;
  } while (1);
}

/* Address: 0x80205904 | Size: 0x178 | Ghidra import */
u32 fightOutPokemonCheckMeetEnemyFightPokemon(void)

{
    int r3;
    int r4;

    extern s8 pokemonCheckValid();
    extern short fn_801EF634();
  u32 bVar1;
  int iVar2;
  s8 cVar5;
  short sVar3;
  short sVar4;
  u8 bVar6;
  
  if (r3 != 0) {
    if (r4 == 0) {
      bVar1 = 0;
    }
    else {
      sVar3 = fn_801EF634();
      if (sVar3 == 1) {
        bVar1 = 0;
      }
      else {
        iVar2 = (int)pokemonGetStatus(r4,0,0xcb,0);
        if (iVar2 == 0) {
          bVar1 = 0;
        }
        else {
          cVar5 = pokemonCheckValid();
          if (cVar5 == 0) {
            bVar1 = 0;
          }
          else {
            if (r4 == 0) {
              iVar2 = 0;
            }
            else {
              iVar2 = (int)pokemonGetStatus(r4,0,0xcc,0);
            }
            if (iVar2 == 0) {
              bVar1 = 0;
            }
            else {
              cVar5 = pokemonCheckValid();
              if (cVar5 == 0) {
                bVar1 = 0;
              }
              else {
                iVar2 = (int)pokemonGetStatus(r4,0,0xce,0);
                if (iVar2 < 0) {
                  bVar1 = 0;
                }
                else {
                  bVar1 = 1;
                }
              }
            }
          }
        }
      }
    }
    if (bVar1) {
      sVar3 = (int)pokemonGetStatus(r4,0,0xce,0);
      for (bVar6 = 0; bVar6 < 0xc; bVar6 = bVar6 + 1) {
        sVar4 = (int)pokemonGetStatus(r3,0,0xfd,bVar6);
        if ((-1 < sVar4) && (sVar4 == sVar3)) {
          return 1;
        }
      }
    }
  }
  return 0;
}

/* fightOutPokemonSetOnDarkPokemonFlag | Size: 0x58 | Two-hop resolve and call pokemonSetOnDarkPokemonFlag */
#pragma push
#pragma peephole on
void fightOutPokemonSetOnDarkPokemonFlag(void* ctx, u32 param) {
    extern void pokemonSetOnDarkPokemonFlag(void* obj, u32 param);
    if (ctx == NULL) {
        return;
    }
    ctx = pokemonGetStatus(ctx, 0, 0xD5, 0);
    ctx = pokemonGetStatus(ctx, 0, 0xCB, 0);
    pokemonSetOnDarkPokemonFlag(ctx, param);
}
#pragma pop

/* fightOutPokemonSetOnZukanFlag | Size: 0x58 | Two-hop resolve and call pokemonSetOnZukanFlag */
#pragma push
#pragma peephole on
void fightOutPokemonSetOnZukanFlag(void* ctx, u32 param) {
    extern void pokemonSetOnZukanFlag(void* obj, u32 param);
    if (ctx == NULL) {
        return;
    }
    ctx = pokemonGetStatus(ctx, 0, 0xD5, 0);
    ctx = pokemonGetStatus(ctx, 0, 0xCB, 0);
    pokemonSetOnZukanFlag(ctx, param);
}
#pragma pop

/* fightOutPokemonGetFightEntryId | Size: 0x60 | Two-hop resolve (0xD5 -> 0xCE), return s16 or -1 */
#pragma push
#pragma peephole on
s16 fightOutPokemonGetFightEntryId(void* ctx) {
    void* hop1;
    if (ctx == NULL) {
        return -1;
    }
    hop1 = pokemonGetStatus(ctx, 0, 0xD5, 0);
    if (hop1 == NULL) {
        return -1;
    }
    return (s16)(u32)pokemonGetStatus(hop1, 0, 0xCE, 0);
}
#pragma pop

#pragma push
#pragma peephole on
void* fightOutPokemonGetPokemonPtr(void* ctx) {
    extern void* pokemonGetStatus();
    if (ctx == 0) {
        return 0;
    }
    ctx = pokemonGetStatus(ctx, 0, 0xD6, 0);
    if (ctx == 0) {
        return 0;
    }
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}
#pragma pop

/* 0x80205BE8 | size: 0x3C | small */
#pragma push
#pragma peephole on
void* fightPokemonGetPokemonPtr(void* ctx) {
    if (ctx == 0) return 0;
    return pokemonGetStatus(ctx, 0, 0xcc, 0);
}
#pragma pop

/* Address: 0x80205C24 | Size: 0x684 | Ghidra import */

u32 fightOutPokemonCheckFightActionSelect(int r3,char r4)

{
    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern u32 wazaGetStatus();
    extern s8 fn_80121ADC();
    extern s8 pokemonCheckFightOut();
    extern s8 pokemonCheckValid();
    extern short fn_801EF634();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern s8 fightActionCreate();
    extern u32 fightFloorGetStatus();
    extern void fightWazaCreate();
    extern int fightWazaCheckValid();
    extern void fightActionBiosSetBuffDataId();
    extern u32 fn_8022B2CC();
  u32 bVar1;
  u16 uVar5;
  int iVar2;
  short sVar6;
  int iVar3;
  u32 uVar4;
  s8 cVar8;
  u16 uVar7;
  
  uVar5 = fightFloorGetStatus(0,0,0x14,0);
  if (r3 != 0) {
    sVar6 = fn_801EF634();
    if (sVar6 == 1) {
      bVar1 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        bVar1 = 0;
      }
      else {
        sVar6 = fn_801EF634();
        if (sVar6 == 1) {
          bVar1 = 0;
        }
        else {
          iVar2 = (int)pokemonGetStatus(iVar3,0,0xcb,0);
          if (iVar2 == 0) {
            bVar1 = 0;
          }
          else {
            cVar8 = pokemonCheckValid();
            if (cVar8 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
              }
              if (iVar2 == 0) {
                bVar1 = 0;
              }
              else {
                cVar8 = pokemonCheckValid();
                if (cVar8 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar3 = (int)pokemonGetStatus(iVar3,0,0xce,0);
                  if (iVar3 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    if (bVar1) {
      iVar3 = (int)pokemonGetStatus(r3,0,0x120,0);
      if (iVar3 == 1) {
        bVar1 = 0;
      }
      else {
        iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
        if (iVar3 == 0) {
          bVar1 = 0;
        }
        else {
          sVar6 = fn_801EF634();
          if (sVar6 == 1) {
            bVar1 = 0;
          }
          else {
            iVar2 = (int)pokemonGetStatus(iVar3,0,0xcb,0);
            if (iVar2 == 0) {
              bVar1 = 0;
            }
            else {
              cVar8 = pokemonCheckValid();
              if (cVar8 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar3 == 0) {
                  iVar2 = 0;
                }
                else {
                  iVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
                }
                if (iVar2 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar8 = pokemonCheckValid();
                  if (cVar8 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    iVar2 = (int)pokemonGetStatus(iVar3,0,0xce,0);
                    if (iVar2 < 0) {
                      bVar1 = 0;
                    }
                    else {
                      bVar1 = 1;
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            iVar2 = (int)pokemonGetStatus(iVar3,0,0xd2,0);
            if (iVar2 == 1) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar3 = 0;
              }
              else {
                iVar3 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
              }
              if (iVar3 == 0) {
                bVar1 = 0;
              }
              else {
                cVar8 = pokemonCheckFightOut();
                if (cVar8 == 0) {
                  bVar1 = 0;
                }
                else {
                  bVar1 = 1;
                }
              }
            }
          }
          else {
            bVar1 = 0;
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      sVar6 = fn_80119ED0(0x12);
      if (((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x12), sVar6 == 200)) ||
         (sVar6 = fn_80119ED0(0x12), sVar6 == 0xcd)) {
        iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
        sVar6 = fn_80119ED0(0x12);
        if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x12), sVar6 == 200)) {
          if (iVar3 == 0) {
            uVar4 = 0;
          }
          else {
            uVar4 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
          }
          cVar8 = fn_80121ADC(uVar4,0x12);
        }
        else {
          sVar6 = fn_80119ED0(0x12);
          if (sVar6 == 0xcd) {
            cVar8 = fn_8011B67C(iVar3,0x12);
          }
          else {
            cVar8 = 0;
          }
        }
      }
      else {
        sVar6 = fn_80119ED0(0x12);
        if (sVar6 == 0xd8) {
          cVar8 = fn_8011B67C(r3,0x12);
        }
        else {
          cVar8 = 0;
        }
      }
      if (cVar8 != 1) {
        sVar6 = fn_80119ED0(0x22);
        if (((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x22), sVar6 == 200)) ||
           (sVar6 = fn_80119ED0(0x22), sVar6 == 0xcd)) {
          iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
          sVar6 = fn_80119ED0(0x22);
          if ((sVar6 == 0x7c) || (sVar6 = fn_80119ED0(0x22), sVar6 == 200)) {
            if (iVar3 == 0) {
              uVar4 = 0;
            }
            else {
              uVar4 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
            }
            cVar8 = fn_80121ADC(uVar4,0x22);
          }
          else {
            sVar6 = fn_80119ED0(0x22);
            if (sVar6 == 0xcd) {
              cVar8 = fn_8011B67C(iVar3,0x22);
            }
            else {
              cVar8 = 0;
            }
          }
        }
        else {
          sVar6 = fn_80119ED0(0x22);
          if (sVar6 == 0xd8) {
            cVar8 = fn_8011B67C(r3,0x22);
          }
          else {
            cVar8 = 0;
          }
        }
        if (cVar8 != 1) {
          return 1;
        }
      }
      if (r4 != 0) {
        uVar4 = (int)pokemonGetStatus(r3,0,0xf8,0);
        cVar8 = fightWazaCheckValid();
        if (cVar8 != 0) {
          uVar7 = wazaGetStatus(uVar4,0,0x28,0);
          cVar8 = wazaGetStatus(uVar4,0,0x26,0);
          uVar4 = fn_8022B2CC(r3,uVar7,uVar5,0x802062a8,1,0, (void*)0xffffffff);
          uVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar4,uVar5);
          iVar3 = (int)pokemonGetStatus(r3,0,0xd9,0);
          if (iVar3 != 0) {
            fightWazaCreate(iVar3,(int)cVar8,uVar7,uVar4,1);
            iVar3 = (int)pokemonGetStatus(r3,0,0xfe,0);
            if ((iVar3 != 0) &&
               (cVar8 = fightActionCreate(iVar3,0,r3,0x13,0,0x80375ca8), cVar8 == 1)) {
              fightActionBiosSetBuffDataId(iVar3,uVar7);
            }
          }
        }
      }
    }
  }
  return 0;
}

/* 0x802062A8 | size: 0x54 | small */
#pragma push
#pragma peephole on
void _fightOutPokemonCheckFightActionSelectSub__FP15FightOutPokemonUsUs(void* param_1, u32 param_2, u32 param_3) {
    extern u32 wazaGetStatus();
    extern void fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    void* uVar1;
    u16 uVar2;

    uVar1 = pokemonGetStatus(param_1, 0, 0xF8, 0);
    uVar2 = (u16)wazaGetStatus(uVar1, 0, 0x29, 0);
    fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(uVar2, param_3);
}
#pragma pop

/* Address: 0x802062FC | Size: 0x30c | Ghidra import */

u8 fightOutPokemonCheckFightOut(int r3)

{
    extern s8 pokemonCheckFightOut();
    extern s8 pokemonCheckValid();
    extern short fn_801EF634();
  u32 bVar1;
  int iVar2;
  short sVar4;
  int iVar3;
  s8 cVar5;
  u8 uVar6;
  
  if (r3 == 0) {
    uVar6 = 0;
  }
  else {
    sVar4 = fn_801EF634();
    if (sVar4 == 1) {
      bVar1 = 0;
    }
    else {
      iVar2 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar2 == 0) {
        bVar1 = 0;
      }
      else {
        sVar4 = fn_801EF634();
        if (sVar4 == 1) {
          bVar1 = 0;
        }
        else {
          iVar3 = (int)pokemonGetStatus(iVar2,0,0xcb,0);
          if (iVar3 == 0) {
            bVar1 = 0;
          }
          else {
            cVar5 = pokemonCheckValid();
            if (cVar5 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar2 == 0) {
                iVar3 = 0;
              }
              else {
                iVar3 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
              }
              if (iVar3 == 0) {
                bVar1 = 0;
              }
              else {
                cVar5 = pokemonCheckValid();
                if (cVar5 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar2 = (int)pokemonGetStatus(iVar2,0,0xce,0);
                  if (iVar2 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        if (bVar1) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
    }
    if (bVar1) {
      iVar2 = (int)pokemonGetStatus(r3,0,0x120,0);
      if (iVar2 == 1) {
        uVar6 = 0;
      }
      else {
        iVar2 = (int)pokemonGetStatus(r3,0,0xd6,0);
        if (iVar2 == 0) {
          uVar6 = 0;
        }
        else {
          sVar4 = fn_801EF634();
          if (sVar4 == 1) {
            bVar1 = 0;
          }
          else {
            iVar3 = (int)pokemonGetStatus(iVar2,0,0xcb,0);
            if (iVar3 == 0) {
              bVar1 = 0;
            }
            else {
              cVar5 = pokemonCheckValid();
              if (cVar5 == 0) {
                bVar1 = 0;
              }
              else {
                if (iVar2 == 0) {
                  iVar3 = 0;
                }
                else {
                  iVar3 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
                }
                if (iVar3 == 0) {
                  bVar1 = 0;
                }
                else {
                  cVar5 = pokemonCheckValid();
                  if (cVar5 == 0) {
                    bVar1 = 0;
                  }
                  else {
                    iVar3 = (int)pokemonGetStatus(iVar2,0,0xce,0);
                    if (iVar3 < 0) {
                      bVar1 = 0;
                    }
                    else {
                      bVar1 = 1;
                    }
                  }
                }
              }
            }
          }
          if (bVar1) {
            iVar3 = (int)pokemonGetStatus(iVar2,0,0xd2,0);
            if (iVar3 == 1) {
              uVar6 = 0;
            }
            else {
              if (iVar2 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)pokemonGetStatus(iVar2,0,0xcc,0);
              }
              if (iVar2 == 0) {
                uVar6 = 0;
              }
              else {
                cVar5 = pokemonCheckFightOut();
                if (cVar5 == 0) {
                  uVar6 = 0;
                }
                else {
                  uVar6 = 1;
                }
              }
            }
          }
          else {
            uVar6 = 0;
          }
        }
      }
    }
    else {
      uVar6 = 0;
    }
  }
  return uVar6;
}

/* Address: 0x80206608 | Size: 0x178 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightPokemonCheckFightOut(void* r3)

{
    extern u8 pokemonCheckFightOut(void*);
    extern u8 pokemonCheckValid();
    extern u16 fn_801EF634();
  u16 sVar2;
  void* iVar1;
  u8 cVar3;
  u8 bVar4;
  
  if (r3 == 0) {
    return 0;
  }
  if (r3 == 0) {
    bVar4 = 0;
  }
  else {
    sVar2 = fn_801EF634();
    if (sVar2 == 1) {
      bVar4 = 0;
    }
    else {
      iVar1 = pokemonGetStatus(r3,0,0xcb,0);
      if (iVar1 == 0) {
        bVar4 = 0;
      }
      else {
        cVar3 = pokemonCheckValid();
        if (cVar3 == 0) {
          bVar4 = 0;
        }
        else {
          if (r3 == 0) {
            iVar1 = 0;
          }
          else {
            iVar1 = pokemonGetStatus(r3,0,0xcc,0);
          }
          if (iVar1 == 0) {
            bVar4 = 0;
          }
          else {
            cVar3 = pokemonCheckValid();
            if (cVar3 == 0) {
              bVar4 = 0;
            }
            else {
              iVar1 = pokemonGetStatus(r3,0,0xce,0);
              if ((s32)iVar1 < 0) {
                bVar4 = 0;
              }
              else {
                bVar4 = 1;
              }
            }
          }
        }
      }
    }
  }
  if (bVar4 == 0) {
    return 0;
  }
  iVar1 = pokemonGetStatus(r3,0,0xd2,0);
  if ((s32)iVar1 == 1) {
    return 0;
  }
  if (r3 == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = pokemonGetStatus(r3,0,0xcc,0);
  }
  if (iVar1 == 0) {
    return 0;
  }
  return (u8)pokemonCheckFightOut(iVar1) != 0;
}
#pragma pop

/* Address: 0x80206780 | Size: 0x148 | Ghidra import */
#pragma push
#pragma peephole on
u8 fightOutPokemonCheckValid(void* p1) {
    extern u8 pokemonCheckValid();
    extern u16 fn_801EF634();
    u16 sVar3;
    void* iVar1;
    void* iVar2;
    u8 cVar4;
    u8 uVar5;

    if (p1 == 0) {
        uVar5 = 0;
    } else {
        sVar3 = fn_801EF634();
        if (sVar3 == 1) {
            uVar5 = 0;
        } else {
            iVar1 = pokemonGetStatus(p1, 0, 0xd6, 0);
            if (iVar1 == 0) {
                uVar5 = 0;
            } else {
                sVar3 = fn_801EF634();
                if (sVar3 == 1) {
                    uVar5 = 0;
                } else {
                    iVar2 = pokemonGetStatus(iVar1, 0, 0xcb, 0);
                    if (iVar2 == 0) {
                        uVar5 = 0;
                    } else {
                        cVar4 = pokemonCheckValid();
                        if (cVar4 == 0) {
                            uVar5 = 0;
                        } else {
                            if (iVar1 == 0) {
                                iVar2 = 0;
                            } else {
                                iVar2 = pokemonGetStatus(iVar1, 0, 0xcc, 0);
                            }
                            if (iVar2 == 0) {
                                uVar5 = 0;
                            } else {
                                cVar4 = pokemonCheckValid();
                                if (cVar4 == 0) {
                                    uVar5 = 0;
                                } else {
                                    iVar1 = pokemonGetStatus(iVar1, 0, 0xce, 0);
                                    if ((s32)iVar1 < 0) {
                                        uVar5 = 0;
                                    } else {
                                        uVar5 = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return uVar5;
}
#pragma pop

/* Address: 0x802068C8 | Size: 0x13c | Ghidra import */

void fightOutPokemonCreate(int r3,int r4,int r5)

{
    extern u32 pokemonGetTokuseiDataId();
    extern void fightFloorSetShadow();
    extern void fightOutPokemonInit();
  u32 uVar1;
  u16 uVar2;
  u16 uVar3;
  u32 uVar4;
  
  if ((r3 != 0) && (r4 != 0)) {
    uVar1 = (int)pokemonGetStatus(r4,0,0xcc,0);
    fightOutPokemonInit(r3);
    pokemonSetStatus(r3,0,0xd5,0,r4);
    pokemonSetStatus(r3,0,0xd6,0,r4);
    if (r5 != 0) {
      pokemonSetStatus(r3,0,0xee,0,r5);
      fightFloorSetShadow();
    }
    uVar2 = (int)pokemonGetStatus(uVar1,0,0x6e,0);
    for (uVar4 = 0; (uVar4 & 0xffff) < 2; uVar4 = uVar4 + 1) {
      uVar3 = (int)pokemonGetStatus(0,uVar2,0x16,uVar4);
      pokemonSetStatus(r3,0,0xff,uVar4 & 0xff,uVar3);
    }
    uVar2 = pokemonGetTokuseiDataId(uVar1);
    pokemonSetStatus(r3,0,0x100,0,uVar2);
  }
  return;
}
