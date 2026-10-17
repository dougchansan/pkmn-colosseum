/**
 * @file gbaCommunication_candidate_80096C48.c
 * @brief gbaCommunication carve, 0x80096C48 - 0x800973EC (3 functions:
 *        fn_80096C48, fn_80096D54, fn_80096FA0). The rest of the status-menu
 *        callbacks, 0x800973EC - 0x80097FFC, are in gbaCommunication_exact_800973EC.c.
 *
 * Standalone carve: GBA_COMMUNICATION_DECLS_ONLY takes only the shared
 * declarations from gbaCommunication.c, the status-menu work types are
 * repeated here, and the functions follow in address order. The unit is
 * built with -opt nopeephole (configure.py) instead of the full file's
 * per-function peephole and scheduling pragmas; under it every function
 * is exact. The unit also owns the three jump tables, .data 0x802EF038 -
 * 0x802EF0A4. The bodies are gbaCommunication.c's; keep the
 * two in step.
 */
#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;


/* 0x80096C48 | size: 0x10C */
void fn_80096C48(u32 unused, u8* dst) {
    typedef struct {
        f32 x;
        f32 y;
        f32 z;
    } ColorTriple;

    ColorTriple state0;
    ColorTriple state1;
    ColorTriple state2;
    register u8* out;
    register ColorTriple* triple;
    u8* obj;
    s32 state;

    out = dst;
    state0 = *(ColorTriple*)(lbl_8026F5C0 + 0x00);
    state1 = *(ColorTriple*)(lbl_8026F5C0 + 0x0C);
    state2 = *(ColorTriple*)(lbl_8026F5C0 + 0x18);

    obj = windowSearchID(0x53);
    if (obj == NULL) {
        return;
    }

    state = (s8)obj[0x95];
    switch (state) {
    case 0:
        triple = &state0;
        break;
    case 1:
        triple = &state1;
        break;
    case 2:
        triple = &state2;
        break;
    }

    out[0x64] = triple->x;
    out[0x65] = triple->y;
    out[0x66] = triple->z;
}

/* Handle confirm/cancel input for the linked-Pokemon status menu. */
void fn_80096D54(u8* menu)
{
    extern void soundStartSE(s32 id);
    extern void pokemonWazaReplace();

    u16* keys;
    void* pokemon;
    PokemonStatusMenuWork* work;
    register s8* ribbonRow;
    register s32 ribbonColumn;
    register s32 ribbonIndex;
    s8 ribbon;

    keys = windowGetKeyInfo();
    if (keys[2] & 0x10) {
        work = (PokemonStatusMenuWork*)lbl_803FB380;
        switch (work->state) {
        case 0:
        case 1:
        case 6:
        case 8:
            break;
        case 2:
            work->selection = 0;
            work->state = 3;
            break;
        case 3:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
                    soundStartSE(0x26);
                } else if (lbl_803FB380[0] & 2) {
                    work->state = 4;
                    lbl_803FB380[3] = lbl_803FB380[2];
                }
            }
            break;
        case 4:
            pokemon = work->pokemon;
            if (pokemon != NULL) {
                pokemonWazaReplace(pokemon, work->previousSelection,
                                   work->selection);
            }
            *(s8*)(lbl_803FB380 + 3) = -1;
            work->state = 3;
            break;
        case 5:
            if ((s32)work->ribbons.count > 0) {
                for (ribbon = 0; ribbon < 36; ribbon++) {
                    ribbonIndex = ribbon % 9;
                    ribbonColumn = ribbon % 4;
                    ribbonRow = (s8*)work + ribbonIndex * 4;
                    ribbonRow += ribbonColumn;
                    if (ribbonRow[0x20] >= 0) {
                        break;
                    }
                }
                work->state = 6;
                *(s8*)(lbl_803FB380 + 0x1A) = ribbon;
            }
            break;
        case 7:
            if (work->flags & 0x10) {
                menu[0x98] = 1;
                work->result = work->selection;
            }
            break;
        }
    } else if (keys[2] & 0x20) {
        switch (lbl_803FB380[1]) {
        case 0:
        case 8:
            break;
        case 1:
        case 2:
        case 5:
        case 7:
            menu[0x98] = 1;
            menu[0x99] = 1;
            lbl_803FB380[1] = 8;
            break;
        case 3:
            lbl_803FB380[1] = 2;
            break;
        case 4:
            lbl_803FB380[1] = 3;
            break;
        case 6:
            *(s8*)(lbl_803FB380 + 0x1A) = -1;
            lbl_803FB380[1] = 5;
            break;
        }
    }
}

/* Handle input for the linked-Pokemon status submenus. */
extern void menuPlaySe(s32 menu, s32 se);
extern void menuPokemonModelRequest(void*, u32);
extern u8 pokemonWazaCheckValid(u32 pokemon, s32 waza);

/* Whether move slot `move` of the shown Pokemon can be selected. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusMoveValid(s32 move)
{
    u32 pokemon;
    u16 valid;

    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if ((u16)move == 4) {
        return *(u16*)(lbl_803FB380 + 0x18);
    }
    valid = pokemonGetStatus(pokemon, 0, 0x7F, move);
    if (pokemonWazaCheckValid(pokemon, move) == 0) {
        valid = 0;
    }
    return valid;
}

/* Move the cursor over the move list. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusMoveInput(u8* menu, u16 input, s8 limit)
{
    s8 moveSelection;

    moveSelection = *(s8*)(lbl_803FB380 + 2);
    if (input & 1) {
        moveSelection--;
    } else if (input & 2) {
        moveSelection++;
    }
    if (moveSelection >= limit) {
        moveSelection = (s8)(limit - 1);
    }
    if (moveSelection < 0) {
        moveSelection = 0;
    }
    if (menuStatusMoveValid(moveSelection) != 0 &&
        moveSelection != *(s8*)(lbl_803FB380 + 2)) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 2) = moveSelection;
    }
}

/* Move the cursor over the 9x4 ribbon grid. */
/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInputBody(u8* menu, u16 input)
{
    s32 previous;
    s32 row;
    s32 column;
    s32 scan;
    s8 selection;
    u8* cell;
    s8 current;
    s32 inner;

    current = *(s8*)(lbl_803FB380 + 0x1A);
    column = current % 9;
    row = current / 9;
    if (input & 1) {
        scan = row;
        while (scan-- > 0) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = -1;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 2) {
        scan = row;
        while (++scan < 4) {
            inner = column;
            cell = lbl_803FB380 + scan + column * 4;
            do {
                if ((s8)cell[0x20] >= 0) {
                    row = scan;
                    column = inner;
                    scan = 5;
                    break;
                }
                cell -= 4;
            } while (inner-- > 0);
        }
    } else if (input & 8) {
        previous = column;
        column++;
        if (column >= 9) {
            column = 8;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    } else if (input & 4) {
        previous = column;
        column--;
        if (column < 0) {
            column = 0;
        }
        if ((s8)(lbl_803FB380 + column * 4)[0x20 + row] < 0) {
            column = previous;
        }
    }
    selection = column + row * 9;
    if (selection != current) {
        menuPlaySe(*(s32*)(menu + 4), 1);
        *(s8*)(lbl_803FB380 + 0x1A) = selection;
    }
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline void menuStatusRibbonInput(u8* menu, u16 input)
{
    menuStatusRibbonInputBody(menu, input);
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_80096FA0) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuStatusGetTrigger(void)
{
    return windowGetKeyInfo()[3];
}

void fn_80096FA0(u8* menu)
{
    typedef u32 (*StatusChangeCallback)(u32, s32, s32);

    s32 input;
    u16 trigger;
    s32 action;
    s8 limit;
    u8 tabSelection;
    u32 result;

    action = 0;
    if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
        limit = 5;
    } else {
        limit = 4;
    }
    input = menuStatusGetTrigger();

    switch (lbl_803FB380[1]) {
        case 1:
        case 2:
        case 5: {
            u16 trigger = input;

            tabSelection = menu[0x95];
            if (trigger & 8) {
                tabSelection++;
            } else if (trigger & 4) {
                tabSelection--;
            }
            if ((s8)tabSelection > 2) {
                tabSelection = 2;
            }
            if ((s8)tabSelection < 0) {
                tabSelection = 0;
            }
            menu[0x95] = tabSelection;
            switch ((s8)tabSelection) {
            case 0:
                lbl_803FB380[1] = 1;
                break;
            case 1:
                lbl_803FB380[1] = 2;
                break;
            case 2:
                lbl_803FB380[1] = 5;
                break;
            }
            if (trigger & 1) {
                action = 1;
            } else if (trigger & 2) {
                action = 2;
            }
            if (action != 0 && *(u32*)(lbl_803FB380 + 0x10) != 0) {
                result = (*(StatusChangeCallback*)(lbl_803FB380 + 0x10))(
                    *(u32*)(lbl_803FB380 + 0x0C), action,
                    *(s32*)(lbl_803FB380 + 0x14));
                if (*(u32*)(lbl_803FB380 + 0x0C) != result) {
                    if (result != 0) {
                        menuPlaySe(*(s32*)(menu + 4), 1);
                        menuPokemonModelRequest(lbl_803FB338, result);
                    }
                    *(u32*)(lbl_803FB380 + 0x0C) = result;
                }
            }
            break;
        }

        case 3:
        case 4:
        case 7:
            trigger = input;
            /* RULE-EXCEPTION(user-approved): no-op self-conversion keeps the u16 input in a named variable, as in case 6, so it is allocated before the inlined move-cursor temporaries (retail r4) — see docs/RULE_EXCEPTIONS.md */
            trigger = (u16)trigger;
            menuStatusMoveInput(menu, trigger, limit);
            break;

        case 6:
            trigger = input;
            /* RULE-EXCEPTION(user-approved): no-op self-conversion keeps the u16 input in a named variable, so it is allocated after the ribbon locals (retail r8) — see docs/RULE_EXCEPTIONS.md */
            trigger = (u16)trigger;
            menuStatusRibbonInput(menu, trigger);
            break;

        case 0:
        case 8:
            break;
    }
}
