#ifndef DEFS_H
#define DEFS_H

#define SCREEN_X      1000
#define SCREEN_Y       600
#define PIXEL_SIZE       2

#define MULTI_LAYERS      true
#define RENDER_CEILING       !MULTI_LAYERS    // render ceilings only for single layer world

// shading constants
#define RENDER_SHADED        true
#define OBJECT_INTENSITY       5.0f   // for testing, reset to 1.5f afterwards!
#define MULTIPLIER_INTENSITY   5.0f
#define INTENSITY_SPEED        1.0f

#define SHADE_FACTOR_MIN       0.1f   // the shade factor is clamped between these two values
#define SHADE_FACTOR_MAX       1.0f

// constants for speed movements - all movements are modulated with fElapsedTime
#define SPEED_ROTATE          60.0f   //                            60 degrees per second
#define SPEED_MOVE             5.0f   // forward and backward    -   5 units per second
#define SPEED_STRAFE           5.0f   // left and right strafing -   5 units per second
#define SPEED_LOOKUP         200.0f   // looking up or down      - 200 pixels per second
#define SPEED_STRAFE_UP        1.0f   // flying or chrouching    -   1.0 unit per second

// mini map constants
#define MINIMAP_TILE_SIZE     (32 / PIXEL_SIZE)   // each minimap tile is ... pixels
#define MINIMAP_SCALE_FACTOR   0.4    // should be 0.2

#define SENSE_RADIUS    2.0f    // player must be this close to map cell center to be able to open doors etc
#define SENSE_BLENDF    0.4f    // bland factor to render sense circle around player

#define FACE_UNKNOWN  -1
#define FACE_EAST      0
#define FACE_NORTH     1
#define FACE_WEST      2
#define FACE_SOUTH     3
#define FACE_TOP       4
#define FACE_BOTTOM    5
#define FACE_NR_OF     6

#define ANIM_STATE_CLOSED   0
#define ANIM_STATE_OPENED   1
#define ANIM_STATE_CLOSING  2
#define ANIM_STATE_OPENING  3


// colour constants
#define COL_HUD_TXT     olc::YELLOW
#define COL_HUD_BG      olc::VERY_DARK_GREEN
#endif // !DEFS_H

