/**
 * @file effect_visual.h
 * @brief Visual effect sub-module declarations.
 *
 * Each effect sub-module registers itself with the GSeffect system
 * (gs_effect.c) and provides start/update/render/stop callbacks.
 *
 * Address range: 0x801380D4 - 0x801402AC
 *
 * All "Start" functions follow the same pattern:
 *   1. Call GSEffectAllocSlot with a struct size
 *   2. Register callbacks via GSEffectRegister
 *   3. Call GSEffectResetState to enter IDLE state
 *   4. Return the effect ID (or 0 on failure)
 */
#ifndef GAME_EFFECT_EFFECT_VISUAL_H
#define GAME_EFFECT_EFFECT_VISUAL_H

#include "dolphin/types.h"

/* -----------------------------------------------------------------------
 * Lightning effect (lightningEffectCreate)
 *
 * Creates electrical bolt effects between two points. Used for Thunder
 * and electric-type battle moves. Allocates 0x70-byte work struct.
 * ----------------------------------------------------------------------- */
u16 lightningStartEffect(u16 group, u16 modelA, u16 modelB);

/* -----------------------------------------------------------------------
 * Leaf particle effect (0x80138B00)
 *
 * Spawns leaf particles that drift and rotate. Used for Razor Leaf,
 * Leaf Blade, and similar grass-type moves. Also handles lightning
 * sub-particles. Allocates 0x98-byte work struct.
 * ----------------------------------------------------------------------- */
u16 leaffxEffectCreate(u16 group, u16 model, u16 param);

/* -----------------------------------------------------------------------
 * Electron arc effect (0x80139820)
 *
 * Creates arcing electrical discharges. Used for Thunderbolt, Spark,
 * and similar electric-type moves. Allocates 0x78-byte work struct.
 * ----------------------------------------------------------------------- */
u16 boltEffectCreate(u16 group, u16 model, u16 param);

/* -----------------------------------------------------------------------
 * Full-screen filter effect (electronEffectCreate)
 *
 * Applies colour tinting, fading, or overlay filters to the entire
 * screen. Used for screen transitions, weather overlays, and damage
 * flash effects. Allocates 0x14-byte work struct.
 * ----------------------------------------------------------------------- */
u16 filterStart(void);

/* -----------------------------------------------------------------------
 * Water surface wave effect (filterEffectCreate)
 *
 * Creates rippling water surface effects. Used in areas with water
 * features (Phenac City fountain, Pyrite caves). Allocates 0x28-byte
 * work struct.
 * ----------------------------------------------------------------------- */
u16 surfEffectStart(void);

/* -----------------------------------------------------------------------
 * Ocean/sea effect (surfEffectCreate)
 *
 * Renders large-scale ocean water with waves. Used for Gateon Port
 * and other coastal areas. Allocates 0x2C-byte work struct.
 * ----------------------------------------------------------------------- */
u16 seaEffectStart(void);

/* -----------------------------------------------------------------------
 * Environment map reflection effect (seaEffectCreate)
 *
 * Creates real-time environment-mapped reflections on shiny surfaces.
 * Used for metallic Pokemon, polished floors, etc. Allocates 0x48-byte
 * work struct.
 * ----------------------------------------------------------------------- */
u16 envMapEffectInit(void);

/* -----------------------------------------------------------------------
 * Motion blur post-process effect (envMapEffectCreate)
 *
 * Applies directional or radial blur to the screen. Used for speed
 * effects, impact effects, and dramatic moments. Allocates 0x24-byte
 * work struct.
 * ----------------------------------------------------------------------- */
u16 blurEffectStart(void);

/* -----------------------------------------------------------------------
 * Aura glow effect (blurEffectCreate)
 *
 * Renders the dark aura around Shadow Pokemon. Used in battle and
 * overworld when a Shadow Pokemon is present. Allocates 0x20-byte
 * work struct.
 * ----------------------------------------------------------------------- */
u16 auraEffectStart(void);

/* -----------------------------------------------------------------------
 * Screen distortion / heat-haze effect (auraEffectCreate)
 *
 * Creates localized screen distortion. Used for heat shimmer in desert
 * areas, psychic-type moves, and dimensional effects. Allocates 0x50-byte
 * work struct. References "translate" node for matrix setup.
 * ----------------------------------------------------------------------- */
u16 distortionEffectStart(void);

/* -----------------------------------------------------------------------
 * Billboard sprite particle effect (distortionEffectCreate)
 *
 * Renders camera-facing sprite particles. Used for sparkles, dust,
 * and generic particle effects. Allocates 0xB4-byte work struct.
 * ----------------------------------------------------------------------- */
u16 billboardEffectStart(void);

/* -----------------------------------------------------------------------
 * Patchiru (Spinda) texture effect (0x8013FBE0)
 *
 * Generates the unique spot pattern texture for Spinda (Patchiru in
 * Japanese). Each Spinda has a unique spot pattern based on its
 * personality value. Allocates 0x40-byte work struct.
 *
 * Debug string: "Failed to create Patchiru texture"
 * ----------------------------------------------------------------------- */
u16 billboardEffectCreate(void);

#endif /* GAME_EFFECT_EFFECT_VISUAL_H */
