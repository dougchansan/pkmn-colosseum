# Pokemon Colosseum Script Opcode Reference

## VM Architecture Overview

Pokemon Colosseum uses a custom bytecode interpreter called the **Particle Script (PS)** system. Despite the "particle" name, this system drives all scripted game events: NPC dialogue, cutscenes, camera control, story progression, and visual effects.

### Key Properties

| Property | Value |
|----------|-------|
| VM Type | Direct-state manipulation (not stack/register based) |
| Context Size | 0x94 bytes (148 bytes) per script instance |
| Max Concurrent Scripts | Limited only by heap memory |
| Priority Levels | 16 linked lists (link 0 = highest) |
| Bytecode Source | Loaded from FSYS archives, addressed by bank + script ID |
| Main Function | `psinterpret_Main` at `0x8016F500` (12,592 bytes) |
| Jump Table | 128 entries at `0x8036BFE0` |

### Execution Model

Each frame:
1. **Timer updates**: Interpolation timers for lerp, color, size, alpha, and heading are decremented
2. **Wait check**: If `waitTimer > 0`, decrement and skip execution
3. **Bytecode loop**: Fetch and execute opcodes until a yield/wait occurs
4. **Physics**: Apply velocity, gravity, friction, and orbital motion
5. **Camera**: Update camera tracking if attached

### Script Context (PSParticle) Layout

| Offset | Size | Field | Description |
|--------|------|-------|-------------|
| 0x00 | 4 | next | Linked list pointer |
| 0x04 | 4 | flags | Bitfield (see Flag Reference below) |
| 0x08 | 1 | bankIndex | Script data bank index |
| 0x09 | 1 | animIndex | Animation sub-index |
| 0x0A | 1 | objRefIndex | Object reference index |
| 0x0C | 2 | lerpTimer | General lerp countdown |
| 0x0E | 2 | color1Timer | Primary color interp timer |
| 0x10 | 2 | color2Timer | Secondary color interp timer |
| 0x12 | 1 | color1R | Primary red |
| 0x13 | 1 | color1G | Primary green |
| 0x14 | 1 | color1B | Primary blue |
| 0x15 | 1 | color1A | Primary alpha |
| 0x16 | 1 | color2R | Secondary red |
| 0x17 | 1 | color2G | Secondary green |
| 0x18 | 1 | color2B | Secondary blue |
| 0x19 | 1 | color2A | Secondary alpha |
| 0x1A | 2 | waitTimer | Frames to wait |
| 0x1C | 1 | loopCounter | Loop iteration counter |
| 0x1D | 1 | linkNo | Priority list number (0-15) |
| 0x1E | 2 | scriptId | Script identifier |
| 0x20 | 4 | scriptData | Pointer to bytecode start |
| 0x24 | 2 | pc | Program counter |
| 0x26 | 2 | savedPC | Saved PC (for goto) |
| 0x28 | 2 | loopPC | Loop return address |
| 0x2A | 2 | repeatCount | Repeat count (0 = infinite) |
| 0x2C | 4 | velocityX | X velocity (float) |
| 0x30 | 4 | velocityY | Y velocity (float) |
| 0x34 | 4 | velocityZ | Z velocity (float) |
| 0x38 | 4 | scaleFactor | Scale multiplier (float) |
| 0x3C | 4 | frictionFactor | Friction/damping (float) |
| 0x40 | 4 | positionX | X position (float) |
| 0x44 | 4 | positionY | Y position (float) |
| 0x48 | 4 | positionZ | Z position (float) |
| 0x4C | 4 | lerpValue | Current lerp value (float) |
| 0x50 | 4 | heading | Heading angle in radians (float) |
| 0x54 | 2 | alphaTimer | Alpha interp timer |
| 0x56 | 1 | alphaMode | Alpha blend mode |
| 0x57 | 1 | alphaStart | Alpha start value |
| 0x58 | 1 | alphaEnd | Alpha end value |
| 0x59 | 1 | cameraSlot | Camera slot index |
| 0x5A | 2 | sizeXTimer | Size X interp timer |
| 0x5C | 2 | sizeYTimer | Size Y interp timer |
| 0x5E | 2 | headingTimer | Heading interp timer |
| 0x60 | 4 | lerpTarget | Lerp target value (float) |
| 0x64 | 4 | headingSpeed | Rotation speed (float) |
| 0x68 | 4 | headingAccel | Rotation acceleration (float) |
| 0x6C | 2 | color1Countdown | Color 1 countdown |
| 0x6E | 2 | color2Countdown | Color 2 countdown |
| 0x70-0x73 | 4 | color1Target[4] | Color 1 target RGBA |
| 0x74-0x77 | 4 | color2Target[4] | Color 2 target RGBA |
| 0x78 | 2 | sizeXCountdown | Size X countdown |
| 0x7A | 2 | sizeYCountdown | Size Y countdown |
| 0x7C | 2 | alphaCountdown | Alpha countdown |
| 0x7E | 1 | alphaTargetStart | Alpha target start |
| 0x7F | 1 | alphaTargetEnd | Alpha target end |
| 0x80-0x87 | 8 | sizeData[8] | Size current/target/start values |
| 0x8C | 4 | peopleObj | People/NPC object pointer |
| 0x90 | 4 | parentObj | Parent/owner object pointer |

---

## Flag Reference (offset 0x04)

| Bit | Hex | Name | Description |
|-----|-----|------|-------------|
| 0 | 0x00000001 | SCALE_ACTIVE | Scale factor is applied each frame |
| 1 | 0x00000002 | FRICTION_ACTIVE | Friction is applied to velocity |
| 3 | 0x00000008 | ORBIT | Orbital motion mode (uses parent matrix) |
| 5 | 0x00000020 | GRAVITY_ON | Subtract gravity from Y velocity |
| 6 | 0x00000040 | GRAVITY_ALT | Alternative gravity mode |
| 7 | 0x00000080 | INVISIBLE | Entity is not rendered |
| 9 | 0x00000200 | BILLBOARD | Use billboard rendering |
| 10 | 0x00000400 | OBJ_REF | Has active object/animation reference |
| 11 | 0x00000800 | PAUSED | Execution is paused |
| 13 | 0x00002000 | NO_DETACH | Don't detach from parent on destroy |
| 16 | 0x00010000 | KILLED | Marked for destruction |
| 20 | 0x10000000 | SPECIAL | Special processing active |

---

## Opcode Encoding

### Low-range opcodes (0x00-0x7F): Wait / Object Reference

```
Byte 0: [M1 M0 E D4 D3 D2 D1 D0]
  D4..D0 (bits 0-4): Base delay value
  E (bit 5): Extended - if set, next byte provides additional delay bits
  M1 M0 (bits 5-6): Addressing mode
    0x00 = Simple delay/wait
    0x40 = Object reference setup
```

**Wait (mode 0x00):**
Pauses script execution for the given number of frames. If bit 5 is set, the delay is `(D4..D0 << 8) | next_byte`.

**Object Reference (mode 0x40):**
Reads one additional byte as the object reference index. Looks up the object from the data bank and sets the OBJ_REF flag.

### High-range opcodes (0x80-0xFF): Commands

The opcode byte is normalized based on its group:
- **0x80-0x97**: Masked with 0xF8 (groups of 8; low 3 bits = axis flags)
- **0x98-0xBF**: Used as-is
- **0xC0-0xCF**: Masked with 0xF0 (groups of 16)
- **0xD0-0xDF**: Masked with 0xF0 (groups of 16)
- **0xE0-0xFF**: Used as-is

After normalization, subtract 0x80 to get the jump table index.

---

## Complete Opcode Table

### Position / Transform (0x80-0x9F)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0x80-0x87 | SET_POSITION | 0-3 floats | Set absolute position. Low 3 bits select axes (bit0=X, bit1=Y, bit2=Z). Each selected axis reads a float from the stream. |
| 0x88-0x8F | ADD_POSITION | 0-3 floats | Add delta to current position. Same axis encoding. |
| 0x90-0x97 | SET_TARGET_POS | 0-3 floats | Set target/destination position (stored as velocity). Same axis encoding. |
| 0x98-0x9F | SET_VELOCITY | 0-3 floats | Set velocity vector. If ORBIT flag is set and parent exists, velocity is scaled by parent's bounding sphere ratio (extent_sum / 3.0). Same axis encoding. |

### Interpolation / Reference (0xA0-0xA3)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xA0 | SET_LERP | u16 timer, float target | Set lerp interpolation. If timer=0, immediately applies target value. |
| 0xA1 | CLEAR_OBJ_REF | none | Clear the OBJ_REF flag (0x400). |
| 0xA2 | SET_SCALE | float value | Set scale factor. 0.0 clears SCALE_ACTIVE flag. If parent has bounding data, scale is multiplied by parent's extent ratio. |
| 0xA3 | SET_FRICTION | float value | Set friction/damping. 1.0 clears FRICTION_ACTIVE flag. Parent-relative scaling if applicable. |

### Spawn / Child Scripts (0xA4-0xA9)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xA4 | SPAWN_SCRIPT | u16 scriptId | Spawn a child particle script. Inherits scriptId, peopleObj, position. Increments parent ref count. Recursively executes first frame. |
| 0xA5 | SPAWN_SCRIPT_TBL | u16 tableIdx | Spawn child via bank table lookup. Table index maps to script ID through `sScriptDataBanks[bankIndex]`. |
| 0xA6 | SPAWN_GENERATOR | u16 scriptId | Spawn a "generator" script (manages NPC/people objects). Sets up people attachment. |
| 0xA7 | SPAWN_GEN_TBL | u16 tableIdx | Spawn generator via bank table lookup. |
| 0xA8 | SPAWN_GEN_FLAGS | u16 scriptId, u8 flags | Spawn generator with explicit flags byte. Flags bits 0-2 map to bits 25-27 of the generator's flags field. |
| 0xA9 | SPAWN_GEN_TBL_FL | u16 tableIdx, u8 flags | Spawn generator via table + flags. |

### Rendering Flags (0xAC-0xB0)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xAC | SET_INVISIBLE | none | Set INVISIBLE flag (0x80). Entity won't be rendered. |
| 0xAD | CLEAR_INVISIBLE | none | Clear INVISIBLE flag. Entity becomes visible. |
| 0xAE | SET_GRAVITY | none | Clear gravity bits, set GRAVITY_ON (0x20). Y velocity decremented by gravity each frame. |
| 0xAF | SET_GRAVITY_ALT | none | Clear gravity bits, set GRAVITY_ALT (0x40). Alternative gravity mode. |
| 0xB0 | SET_GRAVITY_BOTH | none | Set both gravity flags (0x60). |

### NPC / People (0xB1)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB1 | DETACH_PEOPLE | none | Detach from parent NPC object. Checks collision byte, updates world matrices, transforms position to world space via 4x3 matrix multiply. |

### Alpha / Transparency (0xB2)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB2 | SET_ALPHA | u16 timer, u8 mode, u8 startA, u8 endA | Set alpha interpolation. Reads timer, blend mode, start/end alpha values. If timer=0, applies immediately. |

### Billboard (0xB3-0xB4)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB3 | SET_BILLBOARD | none | Enable billboard rendering (sprite always faces camera). |
| 0xB4 | CLEAR_BILLBOARD | none | Disable billboard rendering. |

### Rotation / Heading (0xB5-0xB6, 0xBF)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB5 | SET_ROT_LERP | u16 timer, float target | Set heading rotation with lerp timer + target speed. |
| 0xB6 | SET_ROT_RANDOM | u16 timer, float base, float randRange | Set rotation with random variation: `speed = base + randRange * random()`. |
| 0xBF | SET_HEADING_RND | u16 timer, float target, float random | Set heading with random lerp component. |

### Random Motion (0xB7-0xB8)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB7 | RANDOM_OFFSET | 3 floats (rx, ry, rz) | Apply random position jitter. Each axis: `pos += (2*range)*random() - range`. Uses `fmadds` for efficient computation. |
| 0xB8 | RANDOM_HEADING | float maxAngle | Set heading to `maxAngle * random()`. Calls rotation update. |

### Camera (0xB9-0xBA, 0xC7-0xC8)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xB9 | CAMERA_LOOKAT | 4 floats | Set camera look-at parameters (likely FOV, near clip, far clip, aspect ratio). Only executes if peopleObj is non-NULL. |
| 0xBA | LOCK_TO_PARENT | none | Toggle parent attachment. Checks parent's people data flags and updates accordingly. |
| 0xBB | SET_FLAG_SPECIAL | none | Set special processing flag (0x10000000). |
| 0xC7 | CAMERA_CALL | u8 slotOffset | Look up camera slot at `sCameraSlots[slotOffset + cameraSlot]` and call `psCameraAttach`. |
| 0xC8 | CAMERA_MOVE | u8 slot, float speedA, float speedB | Move camera with speed params. Scales by parent bounds. If collision check fails, yields. |

### Random Delay / Check (0xBC-0xBD)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xBC | RANDOM_DELAY | u16 min, u16 max | Set wait timer to random value in [min, min + (max-0x8000)*random()]. |
| 0xBD | RANDOM_CHECK | u8 threshold | Generate random value; if >= threshold, yield (used for probability-based branching). |

### Velocity Manipulation (0xBE)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xBE | SCALE_VELOCITY | float factor | Multiply all velocity components (X, Y, Z) by factor. |

### Random Spawn (0xD4)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xD4 | SPAWN_RANDOM | u16 base, u16 range | Spawn script with random ID: `id = base + (range-0x8000)*random()`. May additionally look up through bank table. |

### Color with Random (0xD6, 0xD8)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xD6 | COLOR1_RANDOM | 4 signed bytes | Set primary color targets with random variation per channel. If color1Timer is active, interpolates existing color first (16.16 fixed-point). Random applied as: `target += (variation * 2) * random()`. |
| 0xD8 | COLOR2_RANDOM | 4 signed bytes | Same as COLOR1_RANDOM but for secondary color channels. |

### Spawn with Velocity Inheritance (0xDB, 0xDC)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xDB | SPAWN_WITH_VEL | u16 scriptId | Spawn child inheriting velocity and scale data in addition to position. Copies velocityX/Y/Z and other fields. |
| 0xDC | SPAWN_TBL_VEL | u16 tableIdx | Same as SPAWN_WITH_VEL but via table lookup. |

### Rotation Reset (0xF5)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xF5 | RESET_ROTATION | none | Set headingSpeed = 0.0, headingAccel = 0.0. |

### Flow Control (0xF6-0xFB)

| Opcode | Name | Args | Description |
|--------|------|------|-------------|
| 0xF6 | SET_LOOP | u8 count | Set loop counter and save current PC as loop return address. |
| 0xF7 | DEC_LOOP | none | Decrement loop counter. If still > 0, jump back to loop PC. |
| 0xF8 | SAVE_PC | none | Save current PC as "return address" for GOTO_SAVED. |
| 0xF9 | GOTO_SAVED | none | Jump to the PC saved by SAVE_PC. |
| 0xFA | YIELD | none | End this frame's execution. Set repeatCount = 1. |
| 0xFB | TERMINATE | none | Same as YIELD (identical handler). |

---

## Jump Table Analysis

The jump table at `0x8036BFE0` has 128 entries. Each entry is a 4-byte pointer to a handler within `psinterpret_Main` (`fn_8016F500`).

### Active Entries (opcodes with handlers)

| Index | Opcode | Offset from fn_8016F500 | Handler Address | Category |
|-------|--------|-------------------------|-----------------|----------|
| 0x00 | 0x80 | +0x3D4 | 0x8016F8D4 | SET_POSITION |
| 0x08 | 0x88 | +0x45C | 0x8016F95C | ADD_POSITION |
| 0x10 | 0x90 | +0x4FC | 0x8016F9FC | SET_TARGET_POS |
| 0x18 | 0x98 | +0x584 | 0x8016FA84 | SET_VELOCITY |
| 0x20 | 0xA0 | +0x68C | 0x8016FB8C | SET_LERP |
| 0x21 | 0xA1 | +0x6BC | 0x8016FBBC | CLEAR_OBJ_REF |
| 0x22 | 0xA2 | +0x6CC | 0x8016FBCC | SET_SCALE |
| 0x23 | 0xA3 | +0x74C | 0x8016FC4C | SET_FRICTION |
| 0x24 | 0xA4 | +0x7CC | 0x8016FCCC | SPAWN_SCRIPT |
| 0x25 | 0xA5 | +0x99C | 0x8016FE9C (via 0x8A4) | SPAWN_SCRIPT_TBL... |
| 0x26 | 0xA6 | +0x10F8 | 0x801705F8 | SPAWN_GENERATOR |
| 0x27 | 0xA7 | +0x1158 | 0x80170658 | SPAWN_GEN_TBL... |
| 0x28 | 0xA8 | +0x118C | 0x8017068C | RANDOM_OFFSET |
| 0x29 | 0xA9 | +0x1244 | 0x80170744 | RANDOM_HEADING |
| 0x2A | 0xAA | +0x1344 | 0x80170844 | CAMERA_LOOKAT |
| 0x2B | 0xAB | +0x1488 | 0x80170988 | LOCK_TO_PARENT |
| 0x2C | 0xAC | +0x14CC | 0x801709CC | SET_LERP_TIMER_RND |
| 0x2D | 0xAD | +0x1518 | 0x80170A18 | SET_INVISIBLE |
| 0x2E | 0xAE | +0x1528 | 0x80170A28 | CLEAR_INVISIBLE |
| 0x2F | 0xAF | +0x1538 | 0x80170A38 | SET_GRAVITY |
| 0x30 | 0xB0 | +0x1554 | 0x80170A54 | SET_GRAVITY_ALT |
| 0x31 | 0xB1 | +0x1570 | 0x80170A70 | SET_GRAVITY_BOTH |
| 0x32 | 0xB2 | +0x1580 | 0x80170A80 | DETACH_PEOPLE |
| 0x33 | 0xB3 | +0x1644 | 0x80170B44 | SET_ALPHA |
| 0x34 | 0xB4 | +0x16F8 | 0x80170BF8 | SET_BILLBOARD |
| 0x35 | 0xB5 | +0x1708 | 0x80170C08 | CLEAR_BILLBOARD |
| 0x36 | 0xB6 | +0x1718 | 0x80170C18 | SET_ROT_LERP |
| 0x37 | 0xB7 | +0x1758 | 0x80170C58 | CAMERA_CALL |
| 0x38 | 0xB8 | +0x1784 | 0x80170C84 | CAMERA_MOVE |
| 0x39 | 0xB9 | +0x182C | 0x80170D2C | SPAWN_WITH_VEL |
| 0x3A | 0xBA | +0x1A34 | 0x80170E34 (via 0x1920) | SPAWN_TBL_VEL |
| 0x3B | 0xBB | +0x1BF0 | 0x80170FF0 (via 0xBF0) | COLOR1_RANDOM |
| 0x3C | 0xBC | +0x1DAC | 0x801710AC (via 0xE64) | COLOR2_RANDOM |
| 0x3D | 0xBD | +0x1E68 | 0x80171168 | unused/variant |
| 0x3E | 0xBE | +0x1F54 | 0x80171254 | SCALE_VELOCITY |
| 0x3F | 0xBF | +0x1FA8 | 0x801712A8 | SET_HEADING_RND |
| 0x40 | 0xC0 | +0x1FD0 | 0x801712D0 | variant |
| 0x4F | 0xCF | +0x20F8 | 0x801713F8 | (0xCF handler) |
| 0x5F | 0xDF | +0x2220 | 0x80171720 | (0xDF handler) |
| 0x62 | 0xE2 | +0x2904 | 0x80171E04 | unused? |
| 0x63 | 0xE3 | +0x2914 | 0x80171E14 | unused? |
| 0x64 | 0xE4 | +0x2924 | 0x80171E24 | unused? |
| 0x65 | 0xE5 | +0x29B8 | 0x80171EB8 | RANDOM_DELAY |
| 0x66 | 0xE6 | +0x2A4C | 0x80171F4C | RANDOM_CHECK |
| 0x67 | 0xE7 | +0x2A5C | 0x80171F5C | SCALE_VELOCITY_ALT |
| 0x68 | 0xE8 | +0x2A6C | 0x80171F6C | SET_HEADING_RND |
| 0x69 | 0xE9 | +0x24D0 | 0x801719D0 | (0xE9: orbit calc) |
| 0x6A | 0xEA | +0x2AB4 | 0x80171FB4 | SPAWN_RANDOM |
| 0x6B | 0xEB | +0x2B78 | 0x80172078 | (0xEB handler) |
| 0x6D | 0xED | +0x2C3C | 0x8017213C | (0xED handler) |
| 0x6F | 0xEF | +0xBF0 | 0x801700F0 | SPAWN_GEN_FLAGS |
| 0x70 | 0xF0 | +0xE64 | 0x80170364 | SPAWN_GEN_TBL_FL |
| 0x71 | 0xF1 | +0x8A4 | 0x8016FDA4 | SPAWN_SCRIPT_TBL_2 |
| 0x72 | 0xF2 | +0x1920 | 0x80170E20 | SPAWN_TBL_VEL_2 |
| 0x73 | 0xF3 | +0x2D0C | 0x8017220C | (0xF3 handler) |
| 0x74 | 0xF4 | +0x1264 | 0x80170764 | variant |
| 0x75 | 0xF5 | +0x12B4 | 0x801707B4 | variant |
| 0x76 | 0xF6 | +0x12FC | 0x801707FC | variant |
| 0x77 | 0xF7 | +0x1334 | 0x80170834 | SET_FLAG_SPECIAL |
| 0x7A | 0xFC | +0x2DA0 | 0x801722A0 | RESET_ROTATION |
| 0x7B | 0xFD | +0x2DBC | 0x801722BC (via 0xF6) | SET_LOOP |
| 0x7C | 0xFE | +0x2DE0 | 0x801722E0 (via 0xF7) | DEC_LOOP |
| 0x7D | 0xFF | +0x2DF0 | 0x801722F0 (via 0xF8) | SAVE_PC |
| 0x7E | (0x100) | +0x2E00 | 0x80172300 (via 0xFA) | YIELD |
| 0x7F | (0x101) | +0x2E00 | 0x80172300 (via 0xFB) | TERMINATE |

### Default/Skip Entry

Index offset `+0x2E0C` (address `0x8017230C`) is the "end of opcode" label used for:
- Unimplemented opcodes
- Default fall-through

This is the label `.L_8017230C` which checks the delay value and either loops back to fetch the next opcode or exits execution.

---

## Physics Integration

After opcode execution, the interpreter applies physics each frame:

### Standard Motion
```
if (flags & SCALE_ACTIVE):
    velocityY -= scaleFactor    // gravity
if (flags & FRICTION_ACTIVE):
    velocityX *= frictionFactor
    velocityY *= frictionFactor
    velocityZ *= frictionFactor
positionX += velocityX
positionY += velocityY
positionZ += velocityZ
```

### Orbital Motion (ORBIT flag)
When the ORBIT flag is set, position is calculated using:
- Parent's world transform matrix (4x3)
- sin/cos of scaleFactor and frictionFactor as rotation angles
- Parent's bounding sphere data
- Results in circular/elliptical orbits around the parent

---

## Related Source Files

| File | Address Range | Purpose |
|------|--------------|---------|
| psinterpret.c | 0x8016F430-0x80172630 | Main bytecode interpreter |
| pslist.c | 0x80168C64-0x80169104 | Script list management |
| psdisp.c | (referenced in rodata) | Script display/rendering |
| generator.c | 0x8017424C-0x8017572C | Generator system (spawns camera scripts) |
| particle.c | (referenced in rodata) | Particle rendering backend |

---

## String References

| Address | String | Context |
|---------|--------|---------|
| 0x802737B8 | `"pslist.c"` | Source file for list management |
| 0x802737C4 | `"linkNo >= 0 && linkNo < PS_NUM_LINK"` | Assert in pslist_GetHead |
| 0x802737E8 | `"activeParticle[pp->linkNo] == pp"` | Assert in pslist_Unlink |
| 0x80273808 | `"parent->next == pp"` | Assert in pslist_Unlink |
| 0x802739A0 | `"psinterpret.c"` | Source file for interpreter |
| 0x802738B8+0x80 | `"Particle:setBlendMode:Unknown mode"` | In particle display |
| 0x802738B8+0xA4 | `"psdisp.c"` | Display source file |
| 0x8027382C+0x60 | `"psInitDataBanks: unknown version"` | Version check in init |
| 0x8027382C+0x34 | `"illigal form data (strange number of group)"` | Data validation |
