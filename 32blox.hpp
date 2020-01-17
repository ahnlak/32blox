/*
 * 32blox.hpp - part of 32Blox, a breakout game for the 32blit built to 
 * explore the API.
 *
 * This is the main header, which defines structures, constants and enums as
 * well as our function prototypes. Note that program data (sprites, audio et
 * al) can be found in their own header files.
 *
 * Please note that this is a first attempt at understanding a somewhat fluid
 * API on a shiny new bit of kit, so it probably is not full of 'best practice'.
 * It will hopefully serve as some sort of starting point, however.
 *
 * Coyright (C) 2020 Pete Favelle <pete@fsquared.co.uk>
 *
 * This software is provided under the MIT License. See LICENSE.txt for details.
 */

#ifndef   _32BLOX_HPP_
#define   _32BLOX_HPP_

/* Constants. */

#define MAX_BALLS     5
#define MAX_SCORES    10
#define BOARD_WIDTH   10
#define BOARD_HEIGHT  10


/* Enums. */

typedef enum {
  STATE_SPLASH,
  STATE_GAME,
  STATE_DEATH,
  STATE_HISCORE
} gamestate_t;

typedef enum {
  ALIGN_TOPLEFT,
  ALIGN_TOPCENTRE,
  ALIGN_TOPRIGHT,
  ALIGN_MIDLEFT,
  ALIGN_MIDCENTRE,
  ALIGN_MIDRIGHT,
  ALIGN_BOTLEFT,
  ALIGN_BOTCENTRE,
  ALIGN_BOTRIGHT
} spritealign_t;

typedef enum {
  BAT_NORMAL,
  BAT_MAX
} battype_t;

/* Structures. */

typedef struct {
  battype_t   type;
  float       position;
  uint16_t    baseline;
  uint8_t     width;
} bat_t;


/* Function prototypes. */

void        init( void );
void        update( uint32_t );
void        render( uint32_t );

uint8_t     ball_create( bat_t );
uint8_t     ball_spawn( uint8_t );
int8_t      ball_update( uint8_t, bat_t );
void        ball_render( uint8_t );
void        ball_launch( uint8_t );
bool        ball_stuck( uint8_t );

bool        death_check_score( uint32_t );
gamestate_t death_update( void );
void        death_render( void );

void        game_init( void );
void        game_render( void );
gamestate_t game_update( void );

void        hiscore_init( void );
uint32_t    hiscore_get_score( uint8_t );
void        hiscore_save_score( uint32_t, const char * );
gamestate_t hiscore_update( void );
void        hiscore_render( void );

void        level_init( uint8_t );
uint8_t    *level_get_line( uint8_t );
void        level_hit_brick( uint8_t, uint8_t );
const char *level_get_bricktype( uint8_t );
uint16_t    level_get_bricks( void );

void        splash_render( void );
gamestate_t splash_update( void );

void        sprite_render( const char *, int16_t, int16_t, spritealign_t = ALIGN_TOPLEFT );
size        sprite_size( const char * );
bool        sprite_collide( const char *, int16_t, int16_t, spritealign_t, const char *, int16_t, int16_t, spritealign_t );


#endif /* _32BLOCK_HPP_ */

/* End of 32blox.hpp */
